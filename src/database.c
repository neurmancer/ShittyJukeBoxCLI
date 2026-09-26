#define _POSIX_C_SOURCE 200809L
#include "database.h"
#include <sqlite3.h>    //From forcing database join out of spite to use sqlite3...The exubrant child have died R.I.P Cybergod Neuro
#include <ctype.h>
#include <limits.h>
#include <math.h>   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_APPLICATION_ID 1397375576
#define DB_VERSION 2

static int fail(Database *db, const char *message)
{
    snprintf(db->error, sizeof db->error, "%s", message);
    return(-1);
}

static int execute(Database *db, const char *sql)
{
    if (sqlite3_exec(db->handle, sql, NULL, NULL, NULL) != SQLITE_OK) {
        return(fail(db, sqlite3_errmsg(db->handle)));
    }
    
    return(0);
}

static int prepare(Database *db, sqlite3_stmt **statement, const char *sql)
{
    if (!db->handle) { return(fail(db, "Database is not open")); }
    
    if (sqlite3_prepare_v2(db->handle, sql, -1, statement, NULL) != SQLITE_OK) {
        return(fail(db, sqlite3_errmsg(db->handle)));
    }
    
    return(0);
}

static int finish(Database *db, sqlite3_stmt *statement, int result)
{
    if (result != SQLITE_DONE && result != SQLITE_ROW) {
        fail(db, sqlite3_errmsg(db->handle));
        sqlite3_finalize(statement);
        return(-1);
    }
    
    if (sqlite3_finalize(statement) != SQLITE_OK) { return(fail(db, sqlite3_errmsg(db->handle))); }
    
    return(0);
}

const char *database_error(const Database *db) { return(db->error); }

int database_begin(Database *db) { return(execute(db, "BEGIN IMMEDIATE")); }

int database_commit(Database *db) { return(execute(db, "COMMIT")); }

void database_rollback(Database *db)
{
    /* I DO WANT TO KNOW WHAT MADE ME RETURN TO STONE-AGE. */
    sqlite3_exec(db->handle, "ROLLBACK", NULL, NULL, NULL);
}

void database_close(Database *db)
{
    if (db->handle) { sqlite3_close_v2(db->handle); db->handle = NULL; }
}

static int scalar(Database *db, const char *sql, int *value)
{
    sqlite3_stmt *statement = NULL;
    
    if (prepare(db, &statement, sql) < 0) { return(-1); }
    
    int result = sqlite3_step(statement);
    
    if (result == SQLITE_ROW) { *value = sqlite3_column_int(statement, 0); }
    
    return(finish(db, statement, result));
}

int database_open(Database *db, const char *path)
{
    if (db->handle) { return(fail(db, "Database is already open")); }
    
    db->error[0] = '\0';
    
    if (!path || !*path) { return(fail(db, "Database path is empty")); }
    if (sqlite3_open_v2(path, &db->handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) != SQLITE_OK) {
        fail(db, db->handle ? sqlite3_errmsg(db->handle) : "Cannot allocate database");
        database_close(db);
        return(-1);
    }
    
    sqlite3_busy_timeout(db->handle, 3000);
    
    if (execute(db, "PRAGMA foreign_keys=ON") < 0 || database_begin(db) < 0) { goto fucked; }
    
    int version = 0, application = 0, tables = 0;
    
    if (scalar(db, "PRAGMA user_version", &version) < 0 || scalar(db, "PRAGMA application_id", &application) < 0) { goto rollback; }
    
    if (version > DB_VERSION || version < 0) {
        fail(db, "Unsupported database version; refusing to change it");
        goto rollback;
    }
    
    if (application != DB_APPLICATION_ID && (application != 0 || version != 0)) {
        fail(db, "This database belongs to another application");
        goto rollback;
    }
    
    if (version == 0) {
        if (scalar(db, "SELECT count(*) FROM sqlite_schema WHERE name NOT LIKE 'sqlite_%'", &tables) < 0) { goto rollback; }
        if (tables) { fail(db, "Refusing to initialize a nonempty unrecognized database"); goto rollback; }
        if (execute(db,
            "CREATE TABLE songs("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "title TEXT NOT NULL CHECK(length(trim(title))>0),"
            "artist TEXT NOT NULL DEFAULT '', album TEXT NOT NULL DEFAULT '',"
            "media_uri TEXT NOT NULL UNIQUE CHECK(length(trim(media_uri))>0),"
            "cover_uri TEXT NOT NULL DEFAULT '', lyrics TEXT NOT NULL DEFAULT '',"
            "lyrics_format TEXT NOT NULL DEFAULT 'plain' CHECK(lyrics_format IN('plain','lrc')),"
            "lyrics_uri TEXT NOT NULL DEFAULT '',"
            "duration_ms INTEGER CHECK(duration_ms IS NULL OR duration_ms>=0),"
            "duration_source INTEGER NOT NULL DEFAULT 0 CHECK(duration_source BETWEEN 0 AND 2),"
            "lyrics_start_ms INTEGER CHECK(lyrics_start_ms IS NULL OR lyrics_start_ms>=0),"
            "lyrics_end_ms INTEGER CHECK(lyrics_end_ms IS NULL OR lyrics_end_ms>=0),"
            "solo_start_ms INTEGER CHECK(solo_start_ms IS NULL OR solo_start_ms>=0),"
            "solo_end_ms INTEGER CHECK(solo_end_ms IS NULL OR solo_end_ms>=0),"
            "CHECK((duration_ms IS NULL AND duration_source=0) OR (duration_ms IS NOT NULL AND duration_source IN(1,2))),"
            "CHECK(lyrics_end_ms IS NULL OR (lyrics_start_ms IS NOT NULL AND lyrics_end_ms>=lyrics_start_ms)),"
            "CHECK((solo_start_ms IS NULL AND solo_end_ms IS NULL) OR "
            "(solo_start_ms IS NOT NULL AND solo_end_ms IS NOT NULL AND solo_end_ms>=solo_start_ms)));"
            "CREATE TABLE genres(id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT NOT NULL UNIQUE CHECK(length(trim(name))>0));"
            "CREATE TABLE song_genres(song_id INTEGER NOT NULL REFERENCES songs(id) ON DELETE CASCADE,"
            "genre_id INTEGER NOT NULL REFERENCES genres(id) ON DELETE CASCADE,"
            "position INTEGER NOT NULL CHECK(position>=0), PRIMARY KEY(song_id,genre_id));"
            "CREATE INDEX genre_order ON song_genres(genre_id,position,song_id);"
            "CREATE TABLE special_tracks(role TEXT PRIMARY KEY CHECK(length(trim(role))>0),"
            "song_id INTEGER NOT NULL REFERENCES songs(id) ON DELETE CASCADE);"
            "PRAGMA application_id=1397375576; PRAGMA user_version=2;") < 0) { goto rollback; }
        version = 2;
    }
    if (version < 2) {
        if (execute(db,
            "ALTER TABLE songs DROP COLUMN legacy_title;"
            "ALTER TABLE songs DROP COLUMN legacy_duration;"
            "ALTER TABLE songs DROP COLUMN writer_type;"
            "ALTER TABLE songs ADD COLUMN lyrics_uri TEXT NOT NULL DEFAULT '';"
            "PRAGMA user_version=2;") < 0) { goto rollback; }
    }
    if (database_commit(db) < 0) { goto rollback; }
    return(0);

rollback:
    database_rollback(db);
fucked:
    database_close(db);
    return(-1);
}

DbSong database_song_init(void)
{
    DbSong song = {0};

    song.duration_ms = song.lyrics_start_ms = song.lyrics_end_ms = DB_TIME_UNKNOWN;
    song.solo_start_ms = song.solo_end_ms = DB_TIME_UNKNOWN;

    return(song);
}

static int bind_text(sqlite3_stmt *statement, int index, const char *text)
{
    return(sqlite3_bind_text(statement, index, text ? text : "", -1, SQLITE_TRANSIENT));
}

static int bind_time(sqlite3_stmt *statement, int index, int64_t value)
{
    if (value == DB_TIME_UNKNOWN) { return(sqlite3_bind_null(statement, index)); }

    return(sqlite3_bind_int64(statement, index, value));
}

int database_song_save(Database *db, const DbSong *song, int64_t *id)
{
    if (!song || !id || song->id < 0 || !song->title || !*song->title ||
        !song->media_uri || !*song->media_uri) { return(fail(db, "Invalid song")); }

    const char *insert =
        "INSERT INTO songs(title,artist,album,media_uri,cover_uri,lyrics,lyrics_format,lyrics_uri,"
        "duration_ms,duration_source,lyrics_start_ms,lyrics_end_ms,solo_start_ms,solo_end_ms)"
        "VALUES(?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11,?12,?13,?14)";

    const char *update =
        "UPDATE songs SET title=?1,artist=?2,album=?3,media_uri=?4,cover_uri=?5,lyrics=?6,"
        "lyrics_format=?7,lyrics_uri=?8,duration_ms=?9,duration_source=?10,lyrics_start_ms=?11,"
        "lyrics_end_ms=?12,solo_start_ms=?13,solo_end_ms=?14 WHERE id=?15";

    sqlite3_stmt *statement = NULL;
    
    if (prepare(db, &statement, song->id ? update : insert) < 0) { return(-1); }
    
    int result = SQLITE_OK;
    
    const char *strings[] = {song->title, song->artist, song->album, song->media_uri, song->cover_uri,
                            song->lyrics, song->lyrics_format ? song->lyrics_format : "plain", song->lyrics_uri};
    
    for (int i = 0; i < 8 && result == SQLITE_OK; ++i) { result = bind_text(statement, i + 1, strings[i]); }
    
    if (result == SQLITE_OK) { result = bind_time(statement, 9, song->duration_ms); }
    if (result == SQLITE_OK) { result = sqlite3_bind_int(statement, 10, song->duration_source); }
    
    const int64_t times[] = {song->lyrics_start_ms, song->lyrics_end_ms, song->solo_start_ms, song->solo_end_ms};
    
    for (int i = 0; i < 4 && result == SQLITE_OK; ++i) { result = bind_time(statement, i + 11, times[i]); }
    
    if (result == SQLITE_OK && song->id) { result = sqlite3_bind_int64(statement, 15, song->id); }
    if (result == SQLITE_OK) { result = sqlite3_step(statement); }
    
    if (finish(db, statement, result) < 0) { return(-1); }
    
    if (!sqlite3_changes(db->handle)) { return(fail(db, "Song does not exist")); }
    
    *id = song->id ? song->id : sqlite3_last_insert_rowid(db->handle);
    
    return(0);
}

int database_lyrics_set(Database *db, int64_t id, const char *lyrics, const char *source_uri)
{
    return(database_lyrics_replace(db, id, lyrics, "plain", source_uri));
}

int database_lyrics_replace(Database *db, int64_t id, const char *lyrics, const char *format, const char *source_uri)
{
    if (!format || (strcmp(format, "plain") && strcmp(format, "lrc"))) { return(fail(db, "Invalid lyrics format")); }
    sqlite3_stmt *statement = NULL;
    if (prepare(db, &statement, "UPDATE songs SET lyrics=?1,lyrics_uri=?2,lyrics_format=?4,"
        "lyrics_start_ms=NULL,lyrics_end_ms=NULL WHERE id=?3") < 0) { return(-1); }
    int result = bind_text(statement, 1, lyrics);
    if (result == SQLITE_OK) { result = bind_text(statement, 2, source_uri); }
    if (result == SQLITE_OK) { result = sqlite3_bind_int64(statement, 3, id); }
    if (result == SQLITE_OK) { result = bind_text(statement, 4, format); }
    if (result == SQLITE_OK) { result = sqlite3_step(statement); }
    if (finish(db, statement, result) < 0) { return(-1); }
    if (!sqlite3_changes(db->handle)) { return(fail(db, "Song does not exist")); }
    return(0);
}

static const char song_columns[] =
    "s.id,s.title,s.artist,s.album,s.media_uri,s.cover_uri,s.lyrics,s.lyrics_format,s.lyrics_uri,"
    "s.duration_ms,s.duration_source,s.lyrics_start_ms,s.lyrics_end_ms,s.solo_start_ms,s.solo_end_ms";

void database_song_free(DbSong *song)
{
    free(song->title); free(song->artist); free(song->album); free(song->media_uri);
    free(song->cover_uri); free(song->lyrics); free(song->lyrics_format); free(song->lyrics_uri);

    *song = database_song_init();
}

void database_songs_free(DbSong *songs, size_t count)
{
    for (size_t i = 0; i < count; ++i) { database_song_free(&songs[i]); }

    free(songs);
}

static int64_t column_time(sqlite3_stmt *statement, int index)
{
    return(sqlite3_column_type(statement, index) == SQLITE_NULL ? DB_TIME_UNKNOWN : sqlite3_column_int64(statement, index));
}

static int read_song(Database *db, sqlite3_stmt *statement, DbSong *song)
{
    *song = database_song_init();
    
    song->id = sqlite3_column_int64(statement, 0);
    
    char **strings[] = {&song->title, &song->artist, &song->album, &song->media_uri, &song->cover_uri,
                        &song->lyrics, &song->lyrics_format, &song->lyrics_uri};
    
    for (int i = 0; i < 8; ++i) {
        const unsigned char *value = sqlite3_column_text(statement, i + 1);
    
        *strings[i] = value ? strdup((const char *)value) : NULL;
    
        if (!*strings[i]) { database_song_free(song); return(fail(db, "Cannot allocate song strings")); }
    }
    
    song->duration_ms = column_time(statement, 9);
    song->duration_source = sqlite3_column_int(statement, 10);
    song->lyrics_start_ms = column_time(statement, 11);
    song->lyrics_end_ms = column_time(statement, 12);
    song->solo_start_ms = column_time(statement, 13);
    song->solo_end_ms = column_time(statement, 14);
    
    
    return(0);
}

int database_song_get(Database *db, int64_t id, DbSong *song)
{
    char sql[640];
    
    snprintf(sql, sizeof sql, "SELECT %s FROM songs s WHERE s.id=?1", song_columns);
    
    sqlite3_stmt *statement = NULL;
    *song = database_song_init();
    
    if (prepare(db, &statement, sql) < 0) { return(-1); }
    
    int result = sqlite3_bind_int64(statement, 1, id);
    
    if (result == SQLITE_OK) { result = sqlite3_step(statement); }
    if (result == SQLITE_DONE) { sqlite3_finalize(statement); return(fail(db, "Song does not exist")); }
    if (result == SQLITE_ROW && read_song(db, statement, song) < 0) { sqlite3_finalize(statement); return(-1); }
    
    return(finish(db, statement, result));
}

int database_songs(Database *db, int64_t genre_id, DbSong **songs, size_t *count)
{
    *songs = NULL;
    *count = 0;
    
    char sql[768];
    
    snprintf(sql, sizeof sql, "SELECT %s FROM songs s %s", song_columns, genre_id > 0 ? "JOIN song_genres g ON s.id=g.song_id WHERE g.genre_id=?1 ORDER BY g.position,s.id" : "ORDER BY s.id");
    
    sqlite3_stmt *statement = NULL;
    
    if (prepare(db, &statement, sql) < 0) { return(-1); }
    
    int result = genre_id > 0 ? sqlite3_bind_int64(statement, 1, genre_id) : SQLITE_OK;
    
    if (result != SQLITE_OK) { return(finish(db, statement, result)); }
    
    while ((result = sqlite3_step(statement)) == SQLITE_ROW) {
        DbSong *grown = realloc(*songs, (*count + 1) * sizeof **songs);
    
        if (!grown) { fail(db, "Cannot allocate song list"); goto fucked; }
    
        *songs = grown;
    
        if (read_song(db, statement, &grown[*count]) < 0) { goto fucked; }
    
        ++*count;   //I'll forget which operation is the first then increment the wrong thing in the future mark my words...
    }
    if (finish(db, statement, result) < 0) {
        
        database_songs_free(*songs, *count);
        
        *songs = NULL; 
        *count = 0;
        
        return(-1);
    }
    
    return(0);

fucked:
    sqlite3_finalize(statement);
    database_songs_free(*songs, *count);
    
    *songs = NULL; 
    *count = 0;
    
    return(-1);
}

static int find_id(Database *db, const char *sql, const char *text, int64_t *id)
{
    *id = 0;
    sqlite3_stmt *statement = NULL;
    
    if (prepare(db, &statement, sql) < 0) { return(-1); }
    
    int result = bind_text(statement, 1, text);
    
    if (result == SQLITE_OK) { result = sqlite3_step(statement); }
    if (result == SQLITE_ROW) { *id = sqlite3_column_int64(statement, 0); }
    
    int absent = result == SQLITE_DONE;
    
    if (finish(db, statement, result) < 0) { return(-1); }
    
    return(absent);
}

int database_song_find(Database *db, const char *uri, int64_t *id)
{
    return(find_id(db, "SELECT id FROM songs WHERE media_uri=?1", uri, id));
}

int database_genre_save(Database *db, const char *name, int64_t *id)
{
    sqlite3_stmt *statement = NULL;
    
    if (prepare(db, &statement, "INSERT INTO genres(name) VALUES(?1) ON CONFLICT(name) DO NOTHING") < 0) { return(-1); }
    
    int result = bind_text(statement, 1, name);
    
    if (result == SQLITE_OK) { result = sqlite3_step(statement); }
    if (finish(db, statement, result) < 0) { return(-1); }
    
    return(find_id(db, "SELECT id FROM genres WHERE name=?1", name, id));
}

void database_genres_free(DbGenre *genres, size_t count)
{
    for (size_t i = 0; i < count; ++i) { free(genres[i].name); }
    
    free(genres);
}

int database_genres(Database *db, DbGenre **genres, size_t *count)
{
    *genres = NULL; 
    *count = 0;
    sqlite3_stmt *statement = NULL;

    if (prepare(db, &statement, "SELECT g.id,g.name,count(s.song_id) FROM genres g " "LEFT JOIN song_genres s ON g.id=s.genre_id GROUP BY g.id ORDER BY g.id") < 0) { return(-1); }
    
    int result;
    
    while ((result = sqlite3_step(statement)) == SQLITE_ROW) {
        
        DbGenre *grown = realloc(*genres, (*count + 1) * sizeof **genres);
        
        if (!grown) { fail(db, "Cannot allocate genres"); goto fucked; }
        
        *genres = grown;
        
        const unsigned char *name = sqlite3_column_text(statement, 1);
        
        grown[*count].name = name ? strdup((const char *)name) : NULL;
        
        if (!grown[*count].name) { fail(db, "Cannot allocate genre name"); goto fucked; }
        
        grown[*count].id = sqlite3_column_int64(statement, 0);
        grown[*count].song_count = (size_t)sqlite3_column_int64(statement, 2);
        
        ++*count;
    }
    if (finish(db, statement, result) < 0) {
        
        database_genres_free(*genres, *count);
        *genres = NULL; 
        *count = 0;
        
        return(-1);
    }
    
    return(0);
fucked:
    
    sqlite3_finalize(statement);
    database_genres_free(*genres, *count);
    
    *genres = NULL; 
    *count = 0;
    
    return(-1);
}

int database_song_genre(Database *db, int64_t song_id, int64_t genre_id, int position)
{
    sqlite3_stmt *statement = NULL;

    if (prepare(db, &statement, "INSERT INTO song_genres(song_id,genre_id,position) VALUES(?1,?2,?3)" " ON CONFLICT(song_id,genre_id) DO NOTHING") < 0) { return(-1); }

        int result = sqlite3_bind_int64(statement, 1, song_id);

    if (result == SQLITE_OK) { result = sqlite3_bind_int64(statement, 2, genre_id); }
    if (result == SQLITE_OK) { result = sqlite3_bind_int(statement, 3, position); }
    if (result == SQLITE_OK) { result = sqlite3_step(statement); }

    return(finish(db, statement, result));
}

int database_song_genre_append(Database *db, int64_t song_id, int64_t genre_id)
{
    sqlite3_stmt *statement = NULL;
    if (prepare(db, &statement, "INSERT INTO song_genres(song_id,genre_id,position) "
        "SELECT ?1,?2,coalesce(max(position)+1,0) FROM song_genres WHERE genre_id=?2 "
        "ON CONFLICT(song_id,genre_id) DO NOTHING") < 0) { return(-1); }
    int result = sqlite3_bind_int64(statement, 1, song_id);
    if (result == SQLITE_OK) { result = sqlite3_bind_int64(statement, 2, genre_id); }
    if (result == SQLITE_OK) { result = sqlite3_step(statement); }
    return(finish(db, statement, result));
}

int database_special_set(Database *db, const char *role, int64_t song_id)
{
    sqlite3_stmt *statement = NULL;

    if (prepare(db, &statement, "INSERT INTO special_tracks(role,song_id) VALUES(?1,?2)"
        " ON CONFLICT(role) DO UPDATE SET song_id=excluded.song_id") < 0) { return(-1); }

        int result = bind_text(statement, 1, role);

    if (result == SQLITE_OK) { result = sqlite3_bind_int64(statement, 2, song_id); }
    if (result == SQLITE_OK) { result = sqlite3_step(statement); }

    return(finish(db, statement, result));
}

int database_special_get(Database *db, const char *role, int64_t *song_id)
{
    return(find_id(db, "SELECT song_id FROM special_tracks WHERE role=?1", role, song_id));
}

int database_duration_parse(const char *seconds, int64_t *milliseconds)
{

    if (!seconds || !milliseconds) { return(-1); }

    const unsigned char *p = (const unsigned char *)seconds;

    while (isspace(*p)) { ++p; }

    if (!isdigit(*p)) { return(-1); }

    int64_t whole = 0;

    while (isdigit(*p)) {
        int digit = *p++ - '0';
        if (whole > (INT64_MAX / 1000 - digit) / 10) { return(-1); }
        whole = whole * 10 + digit;
    }

    int fraction = 0, places = 0, round_up = 0;

    if (*p == '.') {
        ++p;
        if (!isdigit(*p)) { return(-1); }
        while (isdigit(*p)) {
            if (places < 3) { fraction = fraction * 10 + (*p - '0'); }
            if (places == 3) { round_up = *p >= '5'; }
            if (places < 4) { ++places; }
            ++p;
        }
    }

    while (places < 3) { fraction *= 10; ++places; }

    while (isspace(*p)) { ++p; }

    if (*p || whole > (INT64_MAX - fraction - round_up) / 1000) { return(-1); }

    *milliseconds = whole * 1000 + fraction + round_up;

    return(0);
}

int database_duration_set(Database *db, int64_t id, int64_t milliseconds, DbDurationSource source)
{
    sqlite3_stmt *statement = NULL;

    if (prepare(db, &statement, "UPDATE songs SET duration_ms=?1,duration_source=?2 WHERE id=?3") < 0) { return(-1); }

    int result = bind_time(statement, 1, milliseconds);

    if (result == SQLITE_OK) { result = sqlite3_bind_int(statement, 2, source); }
    if (result == SQLITE_OK) { result = sqlite3_bind_int64(statement, 3, id); }
    if (result == SQLITE_OK) { result = sqlite3_step(statement); }
    if (finish(db, statement, result) < 0) { return(-1); }
    if (!sqlite3_changes(db->handle)) { return(fail(db, "Song does not exist")); }

    return(0);
}
