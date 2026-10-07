#define _POSIX_C_SOURCE 200809L
#include "lyrics_worker.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct LyricsWorker {
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t wake;
    AudioPlayer *audio;
    Lyrics lyrics;
    LyricsFrame frame;
    uint64_t generation;
    int64_t duration_ms;
    AudioStatus last;
    int sampled;
    int stop;
};

void lyrics_frame_free(LyricsFrame *frame)
{
    free(frame->cues);
    *frame = (LyricsFrame){.active = SIZE_MAX};
}

static void update_frame(LyricsWorker *worker)
{
    AudioStatus status = audio_status(worker->audio);
    if (worker->sampled && status.generation == worker->last.generation &&
        status.position_ms == worker->last.position_ms && status.duration_ms == worker->last.duration_ms &&
        status.state == worker->last.state) { return; }
    worker->last = status;
    worker->sampled = 1;
    LyricsFrame *frame = &worker->frame;
    for (size_t i = frame->active; i < frame->count &&
         worker->lyrics.cues[i].time_ms == worker->lyrics.cues[frame->active].time_ms; ++i) {
        frame->cues[i] = (LyricsCueFrame){0};
    }
    frame->position_ms = status.position_ms;
    frame->active = status.generation == worker->generation && status.state != AUDIO_IDLE &&
                    status.state != AUDIO_FAILED ? lyrics_active(&worker->lyrics, status.position_ms) : SIZE_MAX;
    int64_t duration = status.duration_ms >= 0 ? status.duration_ms : worker->duration_ms;
    for (size_t i = frame->active; i < frame->count &&
         worker->lyrics.cues[i].time_ms == worker->lyrics.cues[frame->active].time_ms; ++i) {
        if (lyrics_cue_active(&worker->lyrics, i, status.position_ms, duration)) {
            frame->cues[i] = (LyricsCueFrame){
                .active = 1,
                .visible_bytes = lyrics_visible_bytes(&worker->lyrics, i, status.position_ms, duration),
                .karaoke = lyrics_karaoke_span(&worker->lyrics, i, status.position_ms, duration)
            };
        }
    }
    ++frame->revision;
}

static void *animate(void *opaque)
{
    LyricsWorker *worker = opaque;
    pthread_mutex_lock(&worker->mutex);
    while (!worker->stop) {
        if (!worker->lyrics.count) {
            pthread_cond_wait(&worker->wake, &worker->mutex);
            continue;
        }
        update_frame(worker);
        struct timespec deadline;
        clock_gettime(CLOCK_MONOTONIC, &deadline);
        deadline.tv_nsec += 10000000;
        if (deadline.tv_nsec >= 1000000000) { ++deadline.tv_sec; deadline.tv_nsec -= 1000000000; }
        pthread_cond_timedwait(&worker->wake, &worker->mutex, &deadline);
    }
    pthread_mutex_unlock(&worker->mutex);
    return(NULL);
}

LyricsWorker *lyrics_worker_create(AudioPlayer *audio)
{
    LyricsWorker *worker = calloc(1, sizeof *worker);
    if (!worker) { return(NULL); }
    worker->audio = audio;
    worker->frame.active = SIZE_MAX;
    if (pthread_mutex_init(&worker->mutex, NULL)) { free(worker); return(NULL); }
    pthread_condattr_t attributes;
    if (pthread_condattr_init(&attributes)) { goto mutex_failed; }
    int result = pthread_condattr_setclock(&attributes, CLOCK_MONOTONIC);
    if (!result) { result = pthread_cond_init(&worker->wake, &attributes); }
    pthread_condattr_destroy(&attributes);
    if (result) { goto mutex_failed; }
    if (pthread_create(&worker->thread, NULL, animate, worker)) {
        pthread_cond_destroy(&worker->wake);
        goto mutex_failed;
    }
    return(worker);
mutex_failed:
    pthread_mutex_destroy(&worker->mutex);
    free(worker);
    return(NULL);
}

int lyrics_worker_set(LyricsWorker *worker, const char *source, uint64_t generation, int64_t duration_ms)
{
    Lyrics parsed = {0};
    LyricsCueFrame *cues = NULL;
    int result = 0;
    if (source && *source) {
        result = lyrics_parse(source, &parsed, NULL, 0);
        if (!result && parsed.count) {
            cues = calloc(parsed.count, sizeof *cues);
            if (!cues) { lyrics_free(&parsed); result = -1; }
        }
    }
    pthread_mutex_lock(&worker->mutex);
    lyrics_free(&worker->lyrics);
    uint64_t revision = worker->frame.revision + 1;
    lyrics_frame_free(&worker->frame);
    worker->lyrics = parsed;
    worker->frame = (LyricsFrame){.cues = cues, .count = parsed.count, .active = SIZE_MAX, .revision = revision};
    worker->generation = generation;
    worker->duration_ms = duration_ms;
    worker->sampled = 0;
    pthread_cond_signal(&worker->wake);
    pthread_mutex_unlock(&worker->mutex);
    return(result);
}

int lyrics_worker_snapshot(LyricsWorker *worker, LyricsFrame *frame)
{
    pthread_mutex_lock(&worker->mutex);
    if (frame->revision == worker->frame.revision) { pthread_mutex_unlock(&worker->mutex); return(0); }
    if (frame->count != worker->frame.count) {
        LyricsCueFrame *cues = worker->frame.count ? malloc(worker->frame.count * sizeof *cues) : NULL;
        if (worker->frame.count && !cues) { pthread_mutex_unlock(&worker->mutex); return(-1); }
        free(frame->cues);
        frame->cues = cues;
    }
    LyricsCueFrame *cues = frame->cues;
    *frame = worker->frame;
    frame->cues = cues;
    if (frame->count) { memcpy(frame->cues, worker->frame.cues, frame->count * sizeof *cues); }
    pthread_mutex_unlock(&worker->mutex);
    return(1);
}

void lyrics_worker_destroy(LyricsWorker *worker)
{
    if (!worker) { return; }
    pthread_mutex_lock(&worker->mutex);
    worker->stop = 1;
    pthread_cond_signal(&worker->wake);
    pthread_mutex_unlock(&worker->mutex);
    pthread_join(worker->thread, NULL);
    lyrics_free(&worker->lyrics);
    lyrics_frame_free(&worker->frame);
    pthread_cond_destroy(&worker->wake);
    pthread_mutex_destroy(&worker->mutex);
    free(worker);
}
