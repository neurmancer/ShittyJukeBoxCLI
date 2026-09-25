#define _POSIX_C_SOURCE 200809L
#include "../src/database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Just a basic test suite no recompiling this time(at least not yet lol)
#include "../songdata.h"

static size_t inserted, existing;

static int import_song(Database *db, const songMetaData *old, int64_t *id)
{
    if (!old->title || !old->url) { return(-1); }
    int found = database_song_find(db, old->url, id);
    if (found < 0) { return(-1); }
    if (!found) { ++existing; return(0); }
    char *title = strdup(old->title);
    if (!title) { fprintf(stderr, "Cannot allocate title\n"); return(-1); }
    char *separator = NULL;
    for (char *cursor = title; (cursor = strstr(cursor, " - ")) != NULL; cursor += 3) {
        separator = cursor;
    }
    char *artist = NULL;
    if (separator && separator != title && separator[3]) {
        *separator = '\0';
        artist = separator + 3;
    }
    DbSong song = database_song_init();
    song.title = title;
    song.artist = artist;
    song.media_uri = (char *)old->url;
    song.lyrics = (char *)old->lyrics;
    song.writer_type = old->writerType;
    song.legacy_title = (char *)old->title;
    song.legacy_duration = old->duration;
    /* Old duration mixes lyric pacing and delay. Keep it verbatim, not as ms. */
    int result = database_song_save(db, &song, id);
    free(title);
    if (result == 0) { ++inserted; }
    return(result);
}

int main(int argc, char **argv)
{
    if (argc > 2) { fprintf(stderr, "Usage: %s [jukebox.db]\n", argv[0]); return(1); }
    Database db = {0};
    if (database_open(&db, argc == 2 ? argv[1] : DATABASE_PATH) < 0) {
        fprintf(stderr, "%s\n", database_error(&db));
        return(1);
    }
    if (database_begin(&db) < 0) { goto failed; }
    for (size_t i = 0; i < sizeof allGenres / sizeof allGenres[0]; ++i) {
        genreMetaData *genre = &allGenres[i];
        int64_t genre_id;
        if (!genre->genre || genre->songCount < 0 || (!genre->songs && genre->songCount)) {
            fprintf(stderr, "Invalid legacy genre\n");
            goto rollback;
        }
        if (database_genre_save(&db, genre->genre, &genre_id) < 0) { goto rollback; }
        for (int j = 0; j < genre->songCount; ++j) {
            int64_t song_id;
            if (import_song(&db, &genre->songs[j], &song_id) < 0 ||
                database_song_genre(&db, song_id, genre_id, j) < 0) { goto rollback; }
        }
    }
    /* Special audio stays addressable by role, outside the regular genres. */
    songMetaData specials[] = {
        {"Never Gonna Give You Up - Rick Astley", rickroll, rickrollAudio, 3.32019, 1},
        {"Idle roast", "", roast, 0, 0},
        {"Mea Lux - Ad Lucem Meum", "", meaLuxAudio, 0, 0}
    };
    const char *roles[] = {"rickroll", "idle_roast", "mea_lux"};
    for (size_t i = 0; i < sizeof specials / sizeof specials[0]; ++i) {
        int64_t song_id, role_id;
        if (import_song(&db, &specials[i], &song_id) < 0) { goto rollback; }
        int found = database_special_get(&db, roles[i], &role_id);
        if (found < 0 || (found == 1 && database_special_set(&db, roles[i], song_id) < 0)) { goto rollback; }
    }
    if (database_commit(&db) < 0) { goto rollback; }
    printf("Imported %zu tracks; kept %zu existing tracks unchanged.\n", inserted, existing);
    database_close(&db);
    return(0);
rollback:
    database_rollback(&db);
failed:
    fprintf(stderr, "Legacy import failed: %s\n", database_error(&db));
    database_close(&db);
    return(1);
}
