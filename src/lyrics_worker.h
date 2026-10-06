#ifndef SJB_LYRICS_WORKER_H
#define SJB_LYRICS_WORKER_H

#include "lyrics_handler.h"
#include "audio_handler.h"

typedef struct LyricsWorker LyricsWorker;

typedef struct {
    int active;
    size_t visible_bytes;
    LyricsSpan karaoke;
} LyricsCueFrame;

typedef struct {
    LyricsCueFrame *cues;
    size_t count;
    size_t active;
    int64_t position_ms;
    uint64_t revision;
} LyricsFrame;

LyricsWorker *lyrics_worker_create(AudioPlayer *audio);
int lyrics_worker_set(LyricsWorker *worker, const char *source, uint64_t generation, int64_t duration_ms);
int lyrics_worker_snapshot(LyricsWorker *worker, LyricsFrame *frame);
void lyrics_frame_free(LyricsFrame *frame);
void lyrics_worker_destroy(LyricsWorker *worker);

#endif
