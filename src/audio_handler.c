#define _POSIX_C_SOURCE 200809L
#include <sched.h>
#include <errno.h>
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

#define SEEK_REQUEST (-0x534a42)
#define OUTPUT_RATE AUDIO_SAMPLE_RATE
#define SAMPLE_HISTORY 65536
#define FRAME_BYTES 4
#define QUEUE_LIMIT (OUTPUT_RATE * FRAME_BYTES / 4)

struct AudioPlayer {
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t wake;
    atomic_bool quitting;
    atomic_bool paused;
    atomic_bool seek_pending;
    int64_t seek_ms;
    bool seek_in_progress;
    atomic_uint_fast64_t generation;
    char *pending_uri;
    
    int16_t sample_history[SAMPLE_HISTORY][2];
    uint64_t sample_total;
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
    int64_t base_ms;
    int64_t discard_until;
    AVRational time_base;
    int64_t stream_start;
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
            player->status.position_ms = play->base_ms + (int64_t)((play->samples > pending ? play->samples - pending : 0) * 1000 / OUTPUT_RATE);
        }
    }
    
    pthread_mutex_unlock(&player->mutex);
}

/* Bound the PCM queue and apply pause even while decoding is ahead of playback. */

static int wait_output(Playback *play, bool drain)
{
    while (!cancelled(play)) {

        if (atomic_load(&play->player->seek_pending)) { return(SEEK_REQUEST); }
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
    int ready = wait_output(play, false);
    if (ready < 0) { return(ready); }

    int capacity = swr_get_out_samples(resampler, frame ? frame->nb_samples : 0);

    if (capacity < 0) { return(capacity); }
    if (!capacity) { return(0); }

    uint8_t *pcm = NULL;
    int result = av_samples_alloc(&pcm, NULL, 2, capacity, AV_SAMPLE_FMT_S16, 0);

    if (result < 0) { return(result); }

    int samples = swr_convert(resampler, &pcm, capacity,
                             frame ? (const uint8_t **)frame->extended_data : NULL,
                             frame ? frame->nb_samples : 0);
    if (samples > 0 && frame && play->discard_until >= 0) {
        int64_t timestamp = frame->best_effort_timestamp;
        if (timestamp != AV_NOPTS_VALUE) {
            int64_t start = av_rescale_q(timestamp - play->stream_start, play->time_base, (AVRational){1, OUTPUT_RATE});
            int64_t target = av_rescale(play->discard_until, OUTPUT_RATE, 1000);
            int64_t skip = target > start ? target - start : 0;
            if (skip >= samples) { av_freep(&pcm); return(0); }
            if (skip) { memmove(pcm, pcm + skip * FRAME_BYTES, (size_t)(samples - skip) * FRAME_BYTES); samples -= (int)skip; }
        }
        play->discard_until = -1;
    }
    if (samples > 0) {
        pthread_mutex_lock(&play->player->mutex);

        scale_volume((int16_t *)pcm, (size_t)samples * 2, play->player->volume_percent);
        int queued = cancelled(play) ? -2 : SDL_QueueAudio(play->device, pcm, (Uint32)samples * FRAME_BYTES);

        if (queued == 0) {
            const int16_t *stereo = (const int16_t *)pcm;
            size_t first = (size_t)samples > SAMPLE_HISTORY ? (size_t)samples - SAMPLE_HISTORY : 0;
            for (size_t i = first; i < (size_t)samples; ++i) {
                size_t slot = (size_t)((play->samples + i) % SAMPLE_HISTORY);
                play->player->sample_history[slot][0] = stereo[i * 2];
                play->player->sample_history[slot][1] = stereo[i * 2 + 1];
            }
            play->player->sample_total = play->samples + (unsigned)samples;
        }

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
    const char *prefix = "https://drive.google.com/uc?";
    
    if (!strncmp(uri, prefix, strlen(prefix))) {
    
        const char *open = strstr(uri + strlen(prefix), "export=open");
    
        if (open && (open == uri + strlen(prefix) || open[-1] == '&') && (open[11] == '\0' || open[11] == '&')) {
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

static int cached_read(void *opaque, uint8_t *buffer, int size)
{
    FILE *cache = opaque;
    
    size_t bytes = fread(buffer, 1, (size_t)size, cache);
    
    if (bytes) { return((int)bytes); }
    
    return(ferror(cache) ? AVERROR(EIO) : AVERROR_EOF);
}

static int64_t cached_seek(void *opaque, int64_t offset, int whence)
{
    FILE *cache = opaque;
    
    whence &= ~AVSEEK_FORCE;
    if (whence == AVSEEK_SIZE) {
    
        off_t position = ftello(cache);
    
        if (position < 0 || fseeko(cache, 0, SEEK_END) < 0) { return(AVERROR(errno)); }
    
        off_t size = ftello(cache);
    
        if (fseeko(cache, position, SEEK_SET) < 0) { return(AVERROR(errno)); }
    
        return(size < 0 ? AVERROR(EIO) : size);
    }
    
    if (whence != SEEK_SET && whence != SEEK_CUR && whence != SEEK_END) { return(AVERROR(EINVAL)); }
    if (fseeko(cache, (off_t)offset, whence) < 0) { return(AVERROR(errno)); }
    
    off_t position = ftello(cache);
    
    return(position < 0 ? AVERROR(errno) : position);
}

static int cache_stream(Playback *play, const char *url, FILE **cache, AVIOContext **cached)
{
    const int64_t limit = INT64_C(512) * 1024 * 1024;
    
    AVIOContext *input = NULL;
    AVDictionary *options = NULL;
    
    *cache = tmpfile();
    
    if (!*cache) { return(AVERROR(errno)); }
    
    av_dict_set(&options, "rw_timeout", "15000000", 0);
    av_dict_set(&options, "tls_verify", "1", 0);
    av_dict_set(&options, "seekable", "0", 0);
    av_dict_set(&options, "multiple_requests", "1", 0);
    av_dict_set(&options, "protocol_whitelist", "http,https,tcp,tls", 0);
    
    AVIOInterruptCB interrupt = {interrupt_io, play};
    
    play->deadline = av_gettime_relative() + INT64_C(300000000);
    
    publish(play, AUDIO_LOADING, "Buffering stream...");
    
    int result = avio_open2(&input, url, AVIO_FLAG_READ, &interrupt, &options);


    av_dict_free(&options);

    if (result < 0) { avio_closep(&input); return(result); }

    int64_t expected = avio_seek(input, 0, AVSEEK_SIZE), total = 0;

    if (expected > limit) { result = AVERROR(EFBIG); goto done; }

    unsigned char buffer[65536];

    while (!interrupt_io(play)) {
        result = avio_read(input, buffer, sizeof buffer);

        if (result == AVERROR_EOF || !result) { break; }
        if (result < 0) { goto done; }
        if (total > limit - result) { result = AVERROR(EFBIG); goto done; }
        if (fwrite(buffer, 1, (size_t)result, *cache) != (size_t)result) { result = AVERROR(EIO); goto done; }

        total += result;

        char progress[128];


        if (expected > 0) {
            snprintf(progress, sizeof progress, "Buffering stream: %lld%%", (long long)(total * 100 / expected));
        }
    
        else { snprintf(progress, sizeof progress, "Buffering stream: %lld KiB", (long long)(total / 1024)); }
    
        publish(play, AUDIO_LOADING, progress);
    }
    
    if (interrupt_io(play)) { result = AVERROR_EXIT; goto done; }
    if (!total || (expected > 0 && total != expected)) { result = AVERROR(EIO); goto done; }
    if (fflush(*cache) != 0 || fseeko(*cache, 0, SEEK_SET) < 0) { result = AVERROR(errno); goto done; }
    
    uint8_t *io_buffer = av_malloc(32768);
    
    if (!io_buffer) { result = AVERROR(ENOMEM); goto done; }
    /* Cache the whole fucking track so seeking never pokes the HTTP connection. */
    *cached = avio_alloc_context(io_buffer, 32768, 0, *cache, cached_read, NULL, cached_seek);
    
    if (!*cached) { av_free(io_buffer); result = AVERROR(ENOMEM); goto done; }
    
    (*cached)->seekable = AVIO_SEEKABLE_NORMAL;
    
    publish(play, AUDIO_LOADING, "");
    
    result = 0;
done:
    avio_closep(&input);
    return(result);
}

static void play_track(AudioPlayer *player, uint64_t generation, const char *uri)
{
    
    Playback play = {.player = player, .generation = generation, .discard_until = -1};
    AVFormatContext *format = avformat_alloc_context();
    
    AVCodecContext *decoder = NULL;
    FILE *cache = NULL;
    AVIOContext *cached = NULL;
    SwrContext *resampler = NULL;
    AVPacket *packet = NULL;
    AVFrame *frame = NULL;
    AVDictionary *options = NULL;
    
    char *url = media_url(uri);
    
    int result = AVERROR(ENOMEM);
    
    const char *stage = "Opening stream";
    
    if (!format || !url) { goto done; }
    
    format->interrupt_callback = (AVIOInterruptCB){interrupt_io, &play};
    if (!strncmp(url, "http://", 7) || !strncmp(url, "https://", 8)) {
        stage = "Buffering stream";
        result = cache_stream(&play, url, &cache, &cached);
        if (result < 0 || cancelled(&play)) { goto done; }
        format->pb = cached;
        format->flags |= AVFMT_FLAG_CUSTOM_IO;
        av_dict_set(&options, "protocol_whitelist", "file", 0);
        stage = "Opening cached stream";
    }

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
    play.time_base = format->streams[stream]->time_base;
    play.stream_start = format->streams[stream]->start_time == AV_NOPTS_VALUE ? 0 : format->streams[stream]->start_time;
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
    
    pthread_mutex_lock(&player->mutex);
    if (!cancelled(&play)) {
        player->status.seekable = duration >= 0 && (!format->pb || (format->pb->seekable & AVIO_SEEKABLE_NORMAL));
    }
    pthread_mutex_unlock(&player->mutex);
    stage = "Decoding stream";
decode:
    if (atomic_load(&player->seek_pending)) {
        pthread_mutex_lock(&player->mutex);
        int64_t target = player->seek_ms;
        atomic_store(&player->seek_pending, false);
        player->seek_in_progress = true;
        pthread_mutex_unlock(&player->mutex);
        play.deadline = av_gettime_relative() + 15000000;
        int64_t timestamp = av_rescale_q(target, (AVRational){1, 1000}, play.time_base) + play.stream_start;
        result = avformat_seek_file(format, stream, INT64_MIN, timestamp, timestamp, 0);
        if (result >= 0) {
            avcodec_flush_buffers(decoder);
            swr_close(resampler);
            if ((result = swr_init(resampler)) < 0) { goto done; }
            av_packet_unref(packet);
            av_frame_unref(frame);
            pthread_mutex_lock(&player->mutex);
            if (!cancelled(&play)) {
                SDL_ClearQueuedAudio(play.device);
                player->sample_total = 0;
                memset(player->sample_history, 0, sizeof player->sample_history);
                player->status.position_ms = target;
                player->status.seek_position_ms = target;
                player->seek_in_progress = false;
                ++player->status.seek_serial;
            }
            pthread_mutex_unlock(&player->mutex);
            play.samples = 0;
            play.base_ms = target;
            play.discard_until = target;
        }
        else {
            pthread_mutex_lock(&player->mutex);
            if (!cancelled(&play)) { player->status.seekable = false; player->seek_in_progress = false; }
            pthread_mutex_unlock(&player->mutex);
        }
    }
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
    if (result == SEEK_REQUEST && !cancelled(&play)) { goto decode; }
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
    if (cached) { av_freep(&cached->buffer); avio_context_free(&cached); }
    if (cache) { fclose(cache); }
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
    atomic_init(&player->seek_pending, false);
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
    atomic_store(&player->seek_pending, false);
    player->seek_in_progress = false;
    
    if (player->device) { SDL_PauseAudioDevice(player->device, 1); SDL_ClearQueuedAudio(player->device); }
    
    player->sample_total = 0;
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
    atomic_store(&player->seek_pending, false);
    player->seek_in_progress = false;
    atomic_store(&player->paused, false);
    
    free(player->pending_uri);
    
    player->pending_uri = NULL;
    
    if (player->device) { SDL_PauseAudioDevice(player->device, 1); SDL_ClearQueuedAudio(player->device); }
    
    player->sample_total = 0;
    player->status = (AudioStatus){.generation = atomic_load(&player->generation), .duration_ms = -1};
    
    pthread_mutex_unlock(&player->mutex);
}

bool audio_seek(AudioPlayer *player, int64_t position_ms)
{
    pthread_mutex_lock(&player->mutex);
    bool allowed = player->status.seekable && position_ms >= 0 &&
                   (player->status.state == AUDIO_PLAYING || player->status.state == AUDIO_PAUSED) &&
                   (player->status.duration_ms < 0 || position_ms <= player->status.duration_ms);
    if (allowed) {
        player->seek_ms = position_ms;
        atomic_store(&player->seek_pending, true);
    }
    pthread_mutex_unlock(&player->mutex);
    return(allowed);
}

bool audio_seek_relative(AudioPlayer *player, int64_t offset_ms)
{
    pthread_mutex_lock(&player->mutex);
    bool allowed = player->status.seekable &&
                   (player->status.state == AUDIO_PLAYING || player->status.state == AUDIO_PAUSED);
    if (allowed) {
        int64_t base = atomic_load(&player->seek_pending) || player->seek_in_progress ? player->seek_ms : player->status.position_ms;
        int64_t limit = player->status.duration_ms >= 0 ? player->status.duration_ms : INT64_MAX;
        if (base > limit) { base = limit; }
        int64_t target = offset_ms > 0 && offset_ms > limit - base ? limit :
                         offset_ms < 0 && offset_ms < -base ? 0 : base + offset_ms;
        player->seek_ms = target;
        atomic_store(&player->seek_pending, true);
    }
    pthread_mutex_unlock(&player->mutex);
    return(allowed);
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

void audio_samples(AudioPlayer *player, AudioSamples *samples)
{
    *samples = (AudioSamples){0};
    pthread_mutex_lock(&player->mutex);
    samples->generation = player->status.generation;
    if (player->device && (player->status.state == AUDIO_PLAYING || player->status.state == AUDIO_PAUSED)) {
        uint64_t pending = SDL_GetQueuedAudioSize(player->device) / FRAME_BYTES;
        uint64_t played = player->sample_total > pending ? player->sample_total - pending : 0;
        uint64_t first = played > AUDIO_ANALYSIS_FRAMES ? played - AUDIO_ANALYSIS_FRAMES : 0;
        uint64_t oldest = player->sample_total > SAMPLE_HISTORY ? player->sample_total - SAMPLE_HISTORY : 0;
        if (played && first >= oldest) {
            size_t padding = AUDIO_ANALYSIS_FRAMES - (size_t)(played - first);
            for (uint64_t i = first; i < played; ++i) {
                size_t out = padding + (size_t)(i - first);
                samples->pcm[out][0] = player->sample_history[i % SAMPLE_HISTORY][0] / 32768.0f;
                samples->pcm[out][1] = player->sample_history[i % SAMPLE_HISTORY][1] / 32768.0f;
            }
            samples->ready = true;
            samples->played_frames = played;
        }
    }
    pthread_mutex_unlock(&player->mutex);
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
