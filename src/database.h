#ifndef SHITTYJUKEBOX_DATABASE_H    //I wanna use #pragma once but this is for the ~backwards compability~  
#define SHITTYJUKEBOX_DATABASE_H

#include <stddef.h>
#include <stdint.h>

#define DATABASE_PATH "jukebox.db"
#define DB_TIME_UNKNOWN INT64_C(-1)

typedef struct sqlite3 sqlite3;
typedef struct {
    sqlite3 *handle;
    char error[256];
} Database;

typedef enum { DB_DURATION_UNKNOWN, DB_DURATION_MANUAL, DB_DURATION_FFPROBE } DbDurationSource;

typedef struct {
    int64_t id;
    char *title;
    char *artist;
    char *album;
    char *media_uri;
    char *cover_uri;
    char *lyrics;
    char *lyrics_format; /* plain or lrc */
    char *legacy_title;
    int64_t duration_ms;
    DbDurationSource duration_source;
    int64_t lyrics_start_ms;
    int64_t lyrics_end_ms;
    int64_t solo_start_ms;
    int64_t solo_end_ms;
    int writer_type;
    double legacy_duration; //My old fucking sorcery
} DbSong;


typedef struct {
    int64_t id;
    char *name;
    size_t song_count;
} DbGenre;

/* 
    Hmmm...

    Initialize Database with {0}; open creates/migrates a database atomically.
  Returns 0 on success, -1 on error; error text remains valid until next call.
  One connection belongs to one thread. No network work occurs on open/read.
*/

int database_open(Database *db, const char *path);
void database_close(Database *db);
const char *database_error(const Database *db);

/* Start input records here to get unknown timestamps, not accidental zeroes.
  Save borrows strings. Get/list allocate strings: release with the free APIs.
  id=0 inserts; id>0 updates that record. media_uri is unique; IDs remain stable.
 */

DbSong database_song_init(void);
int database_song_save(Database *db, const DbSong *song, int64_t *id);
int database_song_get(Database *db, int64_t id, DbSong *song);

/* find returns 0 when found, 1 when absent, -1 on error. */

int database_song_find(Database *db, const char *media_uri, int64_t *id);
int database_songs(Database *db, int64_t genre_id, DbSong **songs, size_t *count);
void database_song_free(DbSong *song);
void database_songs_free(DbSong *songs, size_t count);

int database_genre_save(Database *db, const char *name, int64_t *id);
int database_genres(Database *db, DbGenre **genres, size_t *count);
void database_genres_free(DbGenre *genres, size_t count);
int database_song_genre(Database *db, int64_t song_id, int64_t genre_id, int position);
int database_special_set(Database *db, const char *role, int64_t song_id);
int database_special_get(Database *db, const char *role, int64_t *song_id);

int database_begin(Database *db);
int database_commit(Database *db);
void database_rollback(Database *db);

/* Parses ffprobe's format=duration numeric output (seconds), or rejects N/A.
 * Probing belongs outside the UI thread. Store successful results explicitly.
 */
int database_duration_parse(const char *seconds, int64_t *milliseconds);
int database_duration_set(Database *db, int64_t song_id, int64_t milliseconds, DbDurationSource source);

#endif
