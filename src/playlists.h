#ifndef SHITTYJUKEBOX_PLAYLISTS_H
#define SHITTYJUKEBOX_PLAYLISTS_H

#include "database.h"
#include "TUI.h"
#include "cover_handler.h"

typedef enum { PLAYLIST_BROWSE, PLAYLIST_CREATE, PLAYLIST_RENAME, PLAYLIST_DELETE, PLAYLIST_COVER } PlaylistMode;
typedef enum { PLAYLIST_CHANGED, PLAYLIST_PLAY, PLAYLIST_CLOSE } PlaylistResult;

typedef struct Playlists {
    Database *db;
    TuiMenu menu;
    int64_t *ids;
    char **artists;
    int64_t *durations;
    int64_t playlist_id;
    int64_t pending_song;
    int64_t edit_id;
    PlaylistMode mode;
    char title[160];
    char name[128];
    char cover_uri[2048];
    char cover_input[2048];
    CoverLoader *cover_loader;
    unsigned char *cover_pixels;
    size_t cover_width, cover_height;
    char notice[256];
} Playlists;

int playlists_init(Playlists *panel, Database *db);
void playlists_free(Playlists *panel);
int playlists_picker(Playlists *panel, int64_t song_id);
PlaylistResult playlists_handle(Playlists *panel, TerminalAction action);
bool playlists_poll(Playlists *panel);

#endif
