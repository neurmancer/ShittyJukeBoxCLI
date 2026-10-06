#ifndef SHITTYJUKEBOX_MPRIS_H
#define SHITTYJUKEBOX_MPRIS_H

#include "audio_handler.h"
#include "database.h"
#include "lyrics_handler.h"

typedef struct Mpris Mpris;
typedef struct {
    AudioStatus audio;
    const DbSong *song;
    const Lyrics *lyrics;
    const Lyrics *backing_lyrics;
    const char *art_uri;
    uint64_t track;
    bool shuffle, can_play, can_next, can_previous;
    int loop; /* 0: None, 1: Track, 2: Playlist */
} MprisState;

typedef enum {
    MPRIS_PLAY, MPRIS_PAUSE, MPRIS_TOGGLE, MPRIS_STOP, MPRIS_NEXT, MPRIS_PREVIOUS,
    MPRIS_SEEK, MPRIS_VOLUME, MPRIS_SHUFFLE, MPRIS_LOOP, MPRIS_OPEN, MPRIS_QUIT
} MprisCommand;

/* Callbacks and dispatch belong to the UI thread. No UI pointers cross threads. */
typedef MprisState (*MprisRead)(void *context);
typedef bool (*MprisControl)(void *context, MprisCommand command, int64_t value, const char *text);
Mpris *mpris_create(MprisRead read, MprisControl control, void *context, char *error, size_t size);
bool mpris_poll(Mpris *mpris);
void mpris_destroy(Mpris *mpris);

#endif
