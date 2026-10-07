#ifndef SJB_SESSION_H
#define SJB_SESSION_H

#include "database.h"
#include <stdbool.h>

typedef struct {
    int64_t id;
    char *uri;
} SessionTrack;

typedef struct {
    bool has_preferences;
    int typewriter_mode;
    int typewriter_color;
    int playback_view;
    bool karaoke;
    bool hide_lyrics;
    bool hide_cover;
    SessionTrack *tracks;
    size_t *order;
    size_t count;
    int64_t current;
    int64_t position_ms;
    int volume;
    int loop;
    bool shuffle;
} Session;

int session_load(Database *db, Session *session, char *error, size_t size);
int session_save(Database *db, const Session *session, char *error, size_t size);
void session_free(Session *session);

#endif
