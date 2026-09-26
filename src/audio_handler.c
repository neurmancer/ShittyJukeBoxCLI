#define _POSIX_C_SOURCE 200809L
#include <sched.h>
#include <time.h>
#include "audio_handler.h"
#include <SDL.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/time.h>
#include <libswresample/swresample.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUTPUT_RATE 48000
#define FRAME_BYTES 4
#define QUEUE_LIMIT (OUTPUT_RATE * FRAME_BYTES / 4)

struct AudioPlayer {
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t wake;
    atomic_bool quitting;
    atomic_bool paused;
    atomic_uint_fast64_t generation;
    char *pending_uri;
    
    AudioStatus status;
    int volume_percent;
    SDL_AudioDeviceID device; /* Protected by mutex for immediate pause/stop. */
};

typedef struct {
    AudioPlayer *player;
    
    uint64_t generation;
    
    int64_t deadline;
    
    SDL_AudioDeviceID device;
    
    uint64_t samples;
} Playback;

static int cancelled(Playback *play)
{
    return(atomic_load(&play->player->quitting) || atomic_load(&play->player->generation) != play->generation);
}

static int interrupt_io(void *opaque)
{
    Playback *play = opaque;
    
    return(cancelled(play) || (play->deadline && av_gettime_relative() >= play->deadline));
}

static void publish(Playback *play, AudioState state, const char *error)
{
    AudioPlayer *player = play->player;
    
    pthread_mutex_lock(&player->mutex);
    
    if (!cancelled(play)) {
        player->status.state = state;
        if (error) { snprintf(player->status.error, sizeof player->status.error, "%s", error); }
        if (play->device) {
            uint64_t pending = SDL_GetQueuedAudioSize(play->device) / FRAME_BYTES;
            player->status.position_ms = (int64_t)((play->samples > pending ? play->samples - pending : 0) * 1000 / OUTPUT_RATE);
        }
    }
    
    pthread_mutex_unlock(&player->mutex);
}

/* Bound the PCM queue and apply pause even while decoding is ahead of playback. */

static int wait_output(Playback *play, bool drain)
{
    while (!cancelled(play)) {

        bool paused = atomic_load(&play->player->paused);

        publish(play, paused ? AUDIO_PAUSED : AUDIO_PLAYING, NULL);

        unsigned bytes = SDL_GetQueuedAudioSize(play->device);

        if (!paused && (drain ? bytes == 0 : bytes < QUEUE_LIMIT)) { return(0); }
        SDL_Delay(10);
    }

    return(AVERROR_EXIT);
}

static void scale_volume(int16_t *pcm, size_t samples, int percent)
{
    if (percent == 100) { return; }
    for (size_t i = 0; i < samples; ++i) { pcm[i] = (int16_t)((int32_t)pcm[i] * percent / 100); }
}

static int output_frame(Playback *play, SwrContext *resampler, AVFrame *frame)
{
    if (wait_output(play, false) < 0) { return(AVERROR_EXIT); }

    int capacity = swr_get_out_samples(resampler, frame ? frame->nb_samples : 0);

    if (capacity < 0) { return(capacity); }
    if (!capacity) { return(0); }

    uint8_t *pcm = NULL;
    int result = av_samples_alloc(&pcm, NULL, 2, capacity, AV_SAMPLE_FMT_S16, 0);

    if (result < 0) { return(result); }

    int samples = swr_convert(resampler, &pcm, capacity,
                             frame ? (const uint8_t **)frame->extended_data : NULL,
                             frame ? frame->nb_samples : 0);
    if (samples > 0) {
        pthread_mutex_lock(&play->player->mutex);

        scale_volume((int16_t *)pcm, (size_t)samples * 2, play->player->volume_percent);
        int queued = cancelled(play) ? -2 : SDL_QueueAudio(play->device, pcm, (Uint32)samples * FRAME_BYTES);

        pthread_mutex_unlock(&play->player->mutex); //Better than fork bombs eh?

        if (queued == -2) { samples = AVERROR_EXIT; }

        else if (queued < 0) {
            publish(play, AUDIO_FAILED, SDL_GetError());
            samples = AVERROR_EXTERNAL;
        }

        else { play->samples += (unsigned)samples; }
    }
    
    av_freep(&pcm);
    return(samples);
}

static int receive_frames(Playback *play, AVCodecContext *decoder, SwrContext *resampler, AVFrame *frame)
{
    int result;
    
    while ((result = avcodec_receive_frame(decoder, frame)) >= 0) {
        result = output_frame(play, resampler, frame);
        av_frame_unref(frame);
        if (result < 0) { return(result); }
    }
    
    if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) { return(0); }
    
    return(result);
}

static char *media_url(const char *uri)
{
    /* Legacy Drive links request a preview page. Request the file instead. */
    
    const char *prefix = "https://drive.google.com/uc?";
    
    if (!strncmp(uri, prefix, strlen(prefix))) {
    
        const char *open = strstr(uri + strlen(prefix), "export=open");
    
        if (open && (open == uri + strlen(prefix) || open[-1] == '&') &&
            (open[11] == '\0' || open[11] == '&')) {
            size_t before = (size_t)(open - uri);
            char *url = malloc(strlen(uri) + 5);
    
            if (!url) { return(NULL); }
    
            memcpy(url, uri, before);
    
            strcpy(url + before, "export=download");
            strcpy(url + before + 15, open + 11);
    
            return(url);
        }
    }
    
    return(strdup(uri));
}

static void play_track(AudioPlayer *player, uint64_t generation, const char *uri)
{
    
    Playback play = {.player = player, .generation = generation};
    AVFormatContext *format = avformat_alloc_context();
    AVCodecContext *decoder = NULL;
    SwrContext *resampler = NULL;
    AVPacket *packet = NULL;
    AVFrame *frame = NULL;
    AVDictionary *options = NULL;
    
    char *url = media_url(uri);
    
    int result = AVERROR(ENOMEM);
    
    const char *stage = "Opening stream";
    
    if (!format || !url) { goto done; }
    
    format->interrupt_callback = (AVIOInterruptCB){interrupt_io, &play};
    
    av_dict_set(&options, "rw_timeout", "5000000", 0);
    av_dict_set(&options, "tls_verify", "1", 0);
    
    play.deadline = av_gettime_relative() + 20000000;
    
    result = avformat_open_input(&format, url, NULL, &options);
    
    av_dict_free(&options);
    
    if (result < 0 || cancelled(&play)) { goto done; }
    
    stage = "Reading stream metadata";
    result = avformat_find_stream_info(format, NULL);
    
    if (result < 0 || cancelled(&play)) { goto done; }
    
    const AVCodec *codec = NULL;
    result = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
    
    if (result < 0) { goto done; }
    
    int stream = result;
    int64_t duration = -1;
    
    if (format->duration != AV_NOPTS_VALUE && format->duration >= 0) {
        duration = av_rescale(format->duration, 1000, AV_TIME_BASE);
    }

    else if (format->streams[stream]->duration != AV_NOPTS_VALUE && format->streams[stream]->duration >= 0) {
        duration = av_rescale_q(format->streams[stream]->duration, format->streams[stream]->time_base, (AVRational){1, 1000});
    }
    
    pthread_mutex_lock(&player->mutex);
    
    if (!cancelled(&play)) { player->status.duration_ms = duration; }
    
    pthread_mutex_unlock(&player->mutex);
    stage = "Opening decoder";
    decoder = avcodec_alloc_context3(codec);
    
    if (!decoder) { result = AVERROR(ENOMEM); goto done; }
    
    result = avcodec_parameters_to_context(decoder, format->streams[stream]->codecpar);
    if (result < 0) { goto done; }
    
    result = avcodec_open2(decoder, codec, NULL);
    if (result < 0) { goto done; }
    
    if (!decoder->sample_rate || !decoder->ch_layout.nb_channels) { result = AVERROR_INVALIDDATA; goto done; }
    
    AVChannelLayout stereo = AV_CHANNEL_LAYOUT_STEREO;
    
    result = swr_alloc_set_opts2(&resampler, &stereo, AV_SAMPLE_FMT_S16, OUTPUT_RATE,
                                &decoder->ch_layout, decoder->sample_fmt, decoder->sample_rate, 0, NULL);
    if (result < 0 || (result = swr_init(resampler)) < 0) { goto done; }
    
    SDL_AudioSpec wanted = {0};
    
    wanted.freq = OUTPUT_RATE;
    wanted.format = AUDIO_S16SYS;
    wanted.channels = 2;
    wanted.samples = 1024;
    
    play.device = SDL_OpenAudioDevice(NULL, 0, &wanted, NULL, 0);
    
    if (!play.device) {
        publish(&play, AUDIO_FAILED, SDL_GetError());
        result = AVERROR_EXTERNAL;
        goto done;
    }
    
    pthread_mutex_lock(&player->mutex);
    
    if (!cancelled(&play)) {
        player->device = play.device;
        SDL_PauseAudioDevice(play.device, atomic_load(&player->paused));
    }
    
    pthread_mutex_unlock(&player->mutex);
    
    packet = av_packet_alloc();
    frame = av_frame_alloc();
    
    if (!packet || !frame) { result = AVERROR(ENOMEM); goto done; }
    
    stage = "Decoding stream";
    while (!cancelled(&play)) {
        result = wait_output(&play, false);
        if (result < 0) { goto done; }
        play.deadline = av_gettime_relative() + 15000000;
        result = av_read_frame(format, packet);
        if (result == AVERROR_EOF) { break; }
        if (result < 0) { goto done; }
        if (packet->stream_index == stream) {
            result = avcodec_send_packet(decoder, packet);
            if (result == AVERROR(EAGAIN)) {
                result = receive_frames(&play, decoder, resampler, frame);
                if (result >= 0) { result = avcodec_send_packet(decoder, packet); }
            }
            if (result >= 0) { result = receive_frames(&play, decoder, resampler, frame); }
        }
        av_packet_unref(packet);
        if (result < 0) { goto done; }
    }
    
    if (cancelled(&play)) { result = AVERROR_EXIT; goto done; }
    
    result = avcodec_send_packet(decoder, NULL);
    
    if (result >= 0) { result = receive_frames(&play, decoder, resampler, frame); }
    if (result < 0) { goto done; }
    
    do { result = output_frame(&play, resampler, NULL); } while (result > 0);
    
    if (result < 0) { goto done; }
    
    result = wait_output(&play, true);
    
    if (result >= 0) { publish(&play, AUDIO_FINISHED, NULL); }
done:
    if (result < 0 && !cancelled(&play)) {
    
        AudioStatus status = audio_status(player);
    
        if (status.state != AUDIO_FAILED) {
            char reason[AV_ERROR_MAX_STRING_SIZE], message[256];
            av_strerror(result, reason, sizeof reason);
            snprintf(message, sizeof message, "%s: %s", stage, reason);
            publish(&play, AUDIO_FAILED, message);
        }
    }
    
    pthread_mutex_lock(&player->mutex);
    
    if (player->device == play.device) { player->device = 0; }
    
    pthread_mutex_unlock(&player->mutex);
    
    if (play.device) { SDL_ClearQueuedAudio(play.device); SDL_CloseAudioDevice(play.device); }
    
    av_packet_free(&packet);
    av_frame_free(&frame);
    
    swr_free(&resampler);
    avcodec_free_context(&decoder);
    avformat_close_input(&format);
    av_dict_free(&options);
    
    free(url);
    url = NULL;
}

static void *worker(void *opaque)
{
    AudioPlayer *player = opaque;
    
    for (;;) {
    
        pthread_mutex_lock(&player->mutex);
        
        while (!player->pending_uri && !atomic_load(&player->quitting)) {
            pthread_cond_wait(&player->wake, &player->mutex);
        }
        
        if (atomic_load(&player->quitting)) { pthread_mutex_unlock(&player->mutex); break; }
        
        char *uri = player->pending_uri;
        player->pending_uri = NULL;
        
        uint64_t generation = atomic_load(&player->generation);
        
        pthread_mutex_unlock(&player->mutex);
        play_track(player, generation, uri);
        free(uri);
        uri = NULL;
    }
    return(NULL);
}

AudioPlayer *audio_create(char *error, size_t size)
{
    AudioPlayer *player = calloc(1, sizeof *player);
    
    if (!player) { snprintf(error, size, "Cannot allocate audio player"); return(NULL); }
    
    int result = pthread_mutex_init(&player->mutex, NULL);
    
    if (result) { snprintf(error, size, "%s", strerror(result)); free(player); return(NULL); }
    
    result = pthread_cond_init(&player->wake, NULL);
    
    if (result) {
        snprintf(error, size, "%s", strerror(result));
        pthread_mutex_destroy(&player->mutex); free(player);
        return(NULL);
    }
    
    atomic_init(&player->quitting, false);
    atomic_init(&player->paused, false);
    atomic_init(&player->generation, 0);
    
    player->status.duration_ms = -1;
    player->volume_percent = 100;
    
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        snprintf(error, size, "%s", SDL_GetError());
        goto failed;
    }
    
    avformat_network_init();
    
    av_log_set_level(AV_LOG_QUIET); /* Report worker failures in the UI, not over it. */
    
    result = pthread_create(&player->thread, NULL, worker, player);
    
    if (result) {
        snprintf(error, size, "%s", strerror(result));
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        avformat_network_deinit();
        goto failed;
    }
    
    return(player);
failed:
    pthread_cond_destroy(&player->wake);
    pthread_mutex_destroy(&player->mutex);
    free(player);
    return(NULL);
}

int audio_play(AudioPlayer *player, int64_t song_id, const char *uri)
{
    if (!uri || !*uri) { return(-1); }
    
    char *copy = strdup(uri);
    
    if (!copy) { return(-1); }
    
    pthread_mutex_lock(&player->mutex);
    
    free(player->pending_uri);
    
    player->pending_uri = copy;
    
    uint64_t generation = atomic_fetch_add(&player->generation, 1) + 1;
    
    atomic_store(&player->paused, false);
    
    if (player->device) { SDL_PauseAudioDevice(player->device, 1); SDL_ClearQueuedAudio(player->device); }
    
    player->status = (AudioStatus){.generation = generation, .song_id = song_id,
                                   .duration_ms = -1, .state = AUDIO_LOADING};
    pthread_cond_signal(&player->wake);
    pthread_mutex_unlock(&player->mutex);
    


    return(0);
}

void audio_pause(AudioPlayer *player, bool paused)
{
    pthread_mutex_lock(&player->mutex);
    
    atomic_store(&player->paused, paused);
    
    if (player->device) { SDL_PauseAudioDevice(player->device, paused); }
    if (player->status.state == AUDIO_PLAYING || player->status.state == AUDIO_PAUSED) {
        player->status.state = paused ? AUDIO_PAUSED : AUDIO_PLAYING;
    }
    
    pthread_mutex_unlock(&player->mutex);
}

void audio_stop(AudioPlayer *player)
{
    
    pthread_mutex_lock(&player->mutex);
    atomic_fetch_add(&player->generation, 1);
    
    free(player->pending_uri);
    
    player->pending_uri = NULL;
    
    if (player->device) { SDL_PauseAudioDevice(player->device, 1); SDL_ClearQueuedAudio(player->device); }
    
    player->status = (AudioStatus){.generation = atomic_load(&player->generation), .duration_ms = -1};
    
    pthread_mutex_unlock(&player->mutex);
}

void audio_set_volume(AudioPlayer *player, int percent)
{
    if (percent < 0) { percent = 0; }
    if (percent > 100) { percent = 100; }
    pthread_mutex_lock(&player->mutex);
    player->volume_percent = percent;
    pthread_mutex_unlock(&player->mutex);
}

AudioStatus audio_status(AudioPlayer *player)
{
    pthread_mutex_lock(&player->mutex);
    
    AudioStatus status = player->status;
    
    status.volume_percent = player->volume_percent;
    status.pause_requested = atomic_load(&player->paused);
    
    pthread_mutex_unlock(&player->mutex);
    return(status);
}

void audio_destroy(AudioPlayer *player)
{
    if (!player) { return; }
    
    atomic_store(&player->quitting, true);
    audio_stop(player);
    
    pthread_mutex_lock(&player->mutex);
    pthread_cond_signal(&player->wake);
    pthread_mutex_unlock(&player->mutex);
    pthread_join(player->thread, NULL);
    pthread_cond_destroy(&player->wake);
    pthread_mutex_destroy(&player->mutex);
    
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    
    avformat_network_deinit();
    
    free(player);
}
