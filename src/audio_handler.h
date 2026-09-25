#ifndef SHITTYJUKEBOX_AUDIO_HANDLER_H
#define SHITTYJUKEBOX_AUDIO_HANDLER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct AudioPlayer AudioPlayer;

typedef enum { AUDIO_IDLE, AUDIO_LOADING, AUDIO_PLAYING, AUDIO_PAUSED, AUDIO_FINISHED, AUDIO_FAILED } AudioState;

typedef struct {
    uint64_t generation;
    int64_t song_id;
    int64_t position_ms;
    int64_t duration_ms; /* -1 if the stream does not advertise a duration. */
    AudioState state;
    bool pause_requested;
    char error[256];
} AudioStatus;

/* One player, owned by the UI thread. Commands never wait for network I/O.
 * Status is copied under a mutex; FFmpeg and the audio device live on a worker.
 * Destroy cancels outstanding I/O and joins that worker before returning.
 */

 AudioPlayer *audio_create(char *error, size_t size);

void audio_destroy(AudioPlayer *player);

int audio_play(AudioPlayer *player, int64_t song_id, const char *uri);

void audio_pause(AudioPlayer *player, bool paused);
void audio_stop(AudioPlayer *player);

AudioStatus audio_status(AudioPlayer *player);

#endif
