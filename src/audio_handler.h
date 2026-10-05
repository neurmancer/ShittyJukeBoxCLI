#ifndef SHITTYJUKEBOX_AUDIO_HANDLER_H
#define SHITTYJUKEBOX_AUDIO_HANDLER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_ANALYSIS_FRAMES 2048L

typedef struct AudioPlayer AudioPlayer;

typedef struct {
    float pcm[AUDIO_ANALYSIS_FRAMES][2];
    uint64_t generation;
    uint64_t played_frames;
    bool ready;
} AudioSamples;

typedef enum { AUDIO_IDLE, AUDIO_LOADING, AUDIO_PLAYING, AUDIO_PAUSED, AUDIO_FINISHED, AUDIO_FAILED } AudioState;

typedef struct {
    uint64_t generation;
    int64_t song_id;
    int64_t position_ms;
    int64_t duration_ms; /* -1 if the stream does not advertise a duration. */
    
    AudioState state;
    
    bool seekable;
    
    uint64_t seek_serial; /* Incremented only after a successful worker seek. */
    int64_t seek_position_ms;
    
    bool pause_requested;
    
    int volume_percent;
    
    char error[256];
} AudioStatus;


AudioPlayer *audio_create(char *error, size_t size);

void audio_destroy(AudioPlayer *player);

int audio_play(AudioPlayer *player, int64_t song_id, const char *uri);
int audio_play_at(AudioPlayer *player, int64_t song_id, const char *uri, int64_t position_ms, bool paused);

void audio_pause(AudioPlayer *player, bool paused);
void audio_stop(AudioPlayer *player);

bool audio_seek(AudioPlayer *player, int64_t position_ms);
bool audio_seek_relative(AudioPlayer *player, int64_t offset_ms);

void audio_set_volume(AudioPlayer *player, int percent);

AudioStatus audio_status(AudioPlayer *player);

void audio_samples(AudioPlayer *player, AudioSamples *samples);

#endif
