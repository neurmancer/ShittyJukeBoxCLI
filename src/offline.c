#define _XOPEN_SOURCE 700
#include "offline.h"
#include <dirent.h>
#include <errno.h>
#include <glib.h>
#include <libavformat/avformat.h>
#include <libavutil/time.h>
#include <sqlite3.h>
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

static int fail(Database *db, const char *message)
{
    snprintf(db->error, sizeof db->error, "%s", message);
    return(-1);
}

char *offline_default_root(void)
{
    return(g_build_filename(g_get_home_dir(), ".sjb", "songs", NULL));
}

static int sql(Database *db, const char *statement)
{
    if (sqlite3_exec(db->handle, statement, NULL, NULL, NULL) != SQLITE_OK) {
        return(fail(db, sqlite3_errmsg(db->handle)));
    }
    return(0);
}

static int mark_seen(Database *db, int64_t id)
{
    sqlite3_stmt *statement = NULL;
    int result = sqlite3_prepare_v2(db->handle, "INSERT INTO offline_seen VALUES(?1)", -1, &statement, NULL);
    if (result == SQLITE_OK) { result = sqlite3_bind_int64(statement, 1, id); }
    if (result == SQLITE_OK) { result = sqlite3_step(statement); }
    sqlite3_finalize(statement);
    return(result == SQLITE_DONE ? 0 : fail(db, sqlite3_errmsg(db->handle)));
}

static const char *tag(AVFormatContext *format, const char *name, const char *fallback)
{
    AVDictionaryEntry *entry = av_dict_get(format->metadata, name, NULL, 0);
    return(entry && entry->value[0] ? entry->value : fallback);
}

static int local_song(Database *db, const char *path, const char *name, int64_t genre)
{
    AVFormatContext *format = NULL;
    AVDictionary *options = NULL;
    av_dict_set(&options, "protocol_whitelist", "file", 0);
    int result = avformat_open_input(&format, path, NULL, &options);
    av_dict_free(&options);
    if (result >= 0) { result = avformat_find_stream_info(format, NULL); }
    int stream = result < 0 ? -1 : av_find_best_stream(format, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (stream < 0 || format->streams[stream]->codecpar->codec_id != AV_CODEC_ID_MP3) {
        fprintf(stderr, "Skipping invalid MP3: %s\n", path);
        avformat_close_input(&format);
        return(0);
    }
    char *fallback = g_strndup(name, strlen(name) - 4);
    DbSong song = database_song_init();
    int64_t id = 0;
    result = database_song_find(db, path, &id);
    if (result < 0) { goto done; }
    if (!result && database_song_get(db, id, &song) < 0) { result = -1; goto done; }
    DbSong updated = song;
    updated.id = id;
    updated.title = (char *)tag(format, "title", *fallback ? fallback : name);
    updated.artist = (char *)tag(format, "artist", "");
    updated.album = (char *)tag(format, "album", "");
    char *metadata = g_strconcat(path, ".sjb", NULL);
    GKeyFile *saved = g_key_file_new();
    char *saved_title = NULL, *saved_artist = NULL, *saved_album = NULL;
    if (g_key_file_load_from_file(saved, metadata, G_KEY_FILE_NONE, NULL)) {
        saved_title = g_key_file_get_string(saved, "Song", "title", NULL);
        saved_artist = g_key_file_get_string(saved, "Song", "artist", NULL);
        saved_album = g_key_file_get_string(saved, "Song", "album", NULL);
        if (saved_title && *saved_title) { updated.title = saved_title; }
        if (saved_artist) { updated.artist = saved_artist; }
        if (saved_album) { updated.album = saved_album; }
    }
    updated.media_uri = (char *)path;
    updated.cover_uri = "";
    for (unsigned i = 0; i < format->nb_streams; ++i) {
        if (format->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC) { updated.cover_uri = (char *)path; break; }
    }
    if (format->duration != AV_NOPTS_VALUE && format->duration >= 0) {
        updated.duration_ms = av_rescale(format->duration, 1000, AV_TIME_BASE);
        updated.duration_source = DB_DURATION_FFMPEG;
    }
    result = database_song_save(db, &updated, &id);
    g_free(saved_title);
    g_free(saved_artist);
    g_free(saved_album);
    g_free(metadata);
    g_key_file_unref(saved);
    if (!result) { result = database_song_genres_replace(db, id, genre); }
    if (!result) { result = mark_seen(db, id); }
done:
    database_song_free(&song);
    g_free(fallback);
    avformat_close_input(&format);
    return(result);
}

static int scan(Database *db, const char *directory, int64_t genre, unsigned depth)
{
    if (depth > 64) { return(fail(db, "Local music directory nesting exceeds 64 levels")); }
    struct dirent **entries = NULL;
    int count = scandir(directory, &entries, NULL, alphasort);
    if (count < 0) { return(fail(db, strerror(errno))); }
    int result = 0;
    for (int i = 0; i < count; ++i) {
        const char *name = entries[i]->d_name;
        if (!result && name[0] != '.') {
            char *path = g_build_filename(directory, name, NULL);
            struct stat info;
            if (lstat(path, &info) < 0) { result = fail(db, strerror(errno)); }
            else if (S_ISDIR(info.st_mode)) {
                int64_t child_genre = genre;
                if (depth == 0) { result = database_genre_save(db, name, &child_genre); }
                if (!result) { result = scan(db, path, child_genre, depth + 1); }
            }
            else if (S_ISREG(info.st_mode) && strlen(name) > 4 && !strcasecmp(name + strlen(name) - 4, ".mp3")) {
                result = local_song(db, path, name, genre);
            }
            g_free(path);
        }
        free(entries[i]);
    }
    free(entries);
    return(result);
}

int offline_open(Database *db, const char *root)
{
    char *canonical = realpath(root, NULL);
    if (!canonical) { return(fail(db, strerror(errno))); }
    struct stat info;
    if (stat(canonical, &info) < 0 || !S_ISDIR(info.st_mode)) {
        free(canonical);
        return(fail(db, "--path must name a directory"));
    }
    char *hash = g_compute_checksum_for_string(G_CHECKSUM_SHA256, canonical, -1);
    char *state = g_build_filename(g_get_home_dir(), ".sjb", "libraries", hash, NULL);
    char *path = g_build_filename(state, "jukebox.db", NULL);
    int result = g_mkdir_with_parents(state, 0700) < 0 ? fail(db, strerror(errno)) : database_open(db, path);
    if (!result) { result = database_begin(db); }
    if (!result) {
        result = sql(db, "CREATE TEMP TABLE offline_seen(id INTEGER PRIMARY KEY)");
        if (!result) { result = scan(db, canonical, 0, 0); }
        if (!result) {
            result = sql(db, "DELETE FROM songs WHERE id NOT IN (SELECT id FROM offline_seen);"
                             "DELETE FROM genres WHERE id NOT IN (SELECT genre_id FROM song_genres);"
                             "DROP TABLE offline_seen;");
        }
        if (!result) { result = database_commit(db); }
        if (result < 0) { database_rollback(db); }
    }
    g_free(path);
    g_free(state);
    g_free(hash);
    free(canonical);
    return(result);
}

static char *component(const char *name)
{
    char *safe = g_strdup(name);
    for (char *p = safe; *p; ++p) {
        if (*p == '/' || *p == '\\' || (unsigned char)*p < 32 || *p == 127) { *p = '_'; }
    }
    if (!*safe || !strcmp(safe, ".") || !strcmp(safe, "..")) { g_free(safe); return(g_strdup("Unknown")); }
    if (*safe == '.') { *safe = '_'; }
    if (strlen(safe) > 160) {
        size_t end = 160;
        while (end && ((unsigned char)safe[end] & 0xc0) == 0x80) { --end; }
        safe[end] = '\0';
    }
    return(safe);
}

typedef struct {
    int64_t deadline;
    atomic_bool *cancel;
} DownloadIO;

static int timeout(void *opaque)
{
    DownloadIO *io = opaque;
    return((io->cancel && atomic_load(io->cancel)) || av_gettime_relative() >= io->deadline);
}

static int is_mp3(const char *path)
{
    AVFormatContext *format = NULL;
    AVDictionary *options = NULL;
    av_dict_set(&options, "protocol_whitelist", "file", 0);
    int result = avformat_open_input(&format, path, NULL, &options);
    av_dict_free(&options);
    if (result >= 0) { result = avformat_find_stream_info(format, NULL); }
    int stream = result < 0 ? -1 : av_find_best_stream(format, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    int valid = stream >= 0 && format->streams[stream]->codecpar->codec_id == AV_CODEC_ID_MP3;
    avformat_close_input(&format);
    return(valid);
}

static int download_song(Database *db, const char *root, const DbSong *song, atomic_bool *cancel)
{
    DbGenre *genres = NULL;
    size_t count = 0;
    if (database_song_genres(db, song->id, &genres, &count) < 0) { return(-1); }
    char *genre = count ? component(genres[0].name) : NULL;
    char *directory = genre ? g_build_filename(root, genre, NULL) : g_strdup(root);
    char *title = component(song->title);
    char *name = g_strdup_printf("%s.mp3", title);
    char *path = g_build_filename(directory, name, NULL);
    char *metadata = g_strconcat(path, ".sjb", NULL);
    GKeyFile *saved = g_key_file_new();
    if (access(path, F_OK) == 0) {
        g_key_file_load_from_file(saved, metadata, G_KEY_FILE_NONE, NULL);
        char *source = g_key_file_get_string(saved, "Song", "uri", NULL);
        if (!source || strcmp(source, song->media_uri)) {
            g_free(path);
            g_free(metadata);
            g_free(name);
            name = g_strdup_printf("%s (%lld).mp3", title, (long long)song->id);
            path = g_build_filename(directory, name, NULL);
            metadata = g_strconcat(path, ".sjb", NULL);
            g_key_file_unref(saved);
            saved = g_key_file_new();
        }
        g_free(source);
    }
    char *temporary = g_strconcat(path, ".part-XXXXXX", NULL);
    char *url = g_strdup(song->media_uri);
    AVIOContext *input = NULL;
    AVDictionary *options = NULL;
    FILE *output = NULL;
    int temporary_created = 0;
    int result = -1;
    if (g_mkdir_with_parents(directory, 0700) < 0) { fail(db, strerror(errno)); goto done; }
    if (access(path, F_OK) == 0) {
        g_key_file_load_from_file(saved, metadata, G_KEY_FILE_NONE, NULL);
        char *source = g_key_file_get_string(saved, "Song", "uri", NULL);
        int ours = source && !strcmp(source, song->media_uri);
        g_free(source);
        if (ours && is_mp3(path)) { result = 0; goto done; }
        fail(db, "Destination exists but is not a valid MP3; move it aside before retrying");
        goto done;
    }
    if (g_str_has_prefix(url, "https://drive.google.com/uc?")) {
        char *legacy = strstr(url, "export=open");
        if (legacy && (legacy[-1] == '?' || legacy[-1] == '&') && (!legacy[11] || legacy[11] == '&')) {
            char *fixed = g_strdup_printf("%.*sexport=download%s", (int)(legacy - url), url, legacy + 11);
            g_free(url);
            url = fixed;
        }
    }
    DownloadIO io = {av_gettime_relative() + INT64_C(300000000), cancel};
    AVIOInterruptCB interrupt = {timeout, &io};
    av_dict_set(&options, "rw_timeout", "15000000", 0);
    av_dict_set(&options, "tls_verify", "1", 0);
    av_dict_set(&options, "protocol_whitelist", "file,http,https,tcp,tls,crypto", 0);
    result = avio_open2(&input, url, AVIO_FLAG_READ, &interrupt, &options);
    if (result < 0) {
        char reason[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(result, reason, sizeof reason);
        fail(db, reason);
        goto done;
    }
    int descriptor = mkstemp(temporary);
    if (descriptor < 0) { result = fail(db, strerror(errno)); goto done; }
    temporary_created = 1;
    output = fdopen(descriptor, "wb");
    if (!output) { close(descriptor); result = fail(db, strerror(errno)); goto done; }
    unsigned char buffer[65536];
    int bytes;
    int64_t total = 0;
    int64_t expected = avio_size(input);
    while ((bytes = avio_read(input, buffer, sizeof buffer)) > 0) {
        if (timeout(&io)) { result = fail(db, "Download cancelled or timed out"); goto done; }
        if (fwrite(buffer, 1, (size_t)bytes, output) != (size_t)bytes) { result = fail(db, "Cannot write downloaded song"); goto done; }
        total += bytes;
    }
    if (bytes != AVERROR_EOF || !total || (expected >= 0 && total != expected)) {
        result = fail(db, "Download interrupted or incomplete");
        goto done;
    }
    if (fclose(output) != 0) { output = NULL; result = fail(db, "Cannot finish downloaded song"); goto done; }
    output = NULL;
    if (!is_mp3(temporary)) { result = fail(db, "Downloaded response is not MP3 audio"); goto done; }
    /* No half-downloaded shit in the library, even if the connection dies. */
    if (link(temporary, path) < 0) { result = fail(db, strerror(errno)); goto done; }
    g_key_file_set_string(saved, "Song", "uri", song->media_uri);
    g_key_file_set_string(saved, "Song", "title", song->title);
    g_key_file_set_string(saved, "Song", "artist", song->artist);
    g_key_file_set_string(saved, "Song", "album", song->album);
    if (!g_key_file_save_to_file(saved, metadata, NULL)) {
        unlink(path);
        result = fail(db, "Cannot save downloaded song metadata");
        goto done;
    }
    result = 0;
done:
    if (output) { fclose(output); }
    if (temporary_created) { unlink(temporary); }
    avio_closep(&input);
    av_dict_free(&options);
    database_genres_free(genres, count);
    g_free(genre);
    g_free(directory);
    g_free(title);
    g_free(metadata);
    g_key_file_unref(saved);
    g_free(name);
    g_free(path);
    g_free(temporary);
    g_free(url);
    return(result < 0 ? -1 : 0);
}

static int download_catalog(Database *db, const char *root, int64_t song_id, atomic_bool *cancel)
{
    DbSong *songs = NULL;
    size_t count = 0;
    if (database_songs(db, 0, &songs, &count) < 0) { return(-1); }
    int result = 0, found = !song_id;
    for (size_t i = 0; i < count; ++i) {
        if (song_id && songs[i].id != song_id) { continue; }
        found = 1;
        if (cancel && atomic_load(cancel)) { result = fail(db, "Download cancelled"); break; }
        if (download_song(db, root, &songs[i], cancel) < 0) {
            result = -1;
        }
    }
    database_songs_free(songs, count);
    if (!found) { return(fail(db, "Song ID not found")); }
    return(result);
}

int offline_download(Database *db, const char *root, int64_t song_id)
{
    return(download_catalog(db, root, song_id, NULL));
}

struct OfflineDownload {
    Database db;
    char *root;
    pthread_t thread;
    atomic_bool cancel, finished;
    int result;
};

static void *download_worker(void *opaque)
{
    OfflineDownload *download = opaque;
    download->result = download_catalog(&download->db, download->root, 0, &download->cancel);
    atomic_store(&download->finished, 1);
    return(NULL);
}

OfflineDownload *offline_download_start(Database *db, const char *root, int64_t song_id, char *message, size_t size)
{
    OfflineDownload *download = calloc(1, sizeof *download);
    if (!download) { snprintf(message, size, "Cannot allocate download"); return(NULL); }
    atomic_init(&download->cancel, 0);
    atomic_init(&download->finished, 0);
    download->root = g_strdup(root);
    DbSong *songs = NULL;
    size_t count = 0;
    int copied = 0;
    if (database_open(&download->db, ":memory:") < 0) { goto failed; }
    if (database_songs(db, 0, &songs, &count) < 0) { fail(&download->db, database_error(db)); goto failed; }
    for (size_t i = 0; i < count; ++i) {
        if (song_id && songs[i].id != song_id) { continue; }
        sqlite3_stmt *insert = NULL;
        int result = sqlite3_prepare_v2(download->db.handle, "INSERT INTO songs(id,title,media_uri) VALUES(?1,?2,?3)", -1, &insert, NULL);
        if (result == SQLITE_OK) { result = sqlite3_bind_int64(insert, 1, songs[i].id); }
        if (result == SQLITE_OK) { result = sqlite3_bind_text(insert, 2, songs[i].title, -1, SQLITE_TRANSIENT); }
        if (result == SQLITE_OK) { result = sqlite3_bind_text(insert, 3, songs[i].media_uri, -1, SQLITE_TRANSIENT); }
        if (result == SQLITE_OK) { result = sqlite3_step(insert); }
        sqlite3_finalize(insert);
        int64_t id;
        if (result != SQLITE_DONE || database_song_save(&download->db, &songs[i], &id) < 0) { goto failed; }
        DbGenre *genres = NULL;
        size_t genres_count = 0;
        if (database_song_genres(db, songs[i].id, &genres, &genres_count) < 0) { fail(&download->db, database_error(db)); goto failed; }
        int bad = 0;
        for (size_t j = 0; j < genres_count && !bad; ++j) {
            int64_t genre;
            bad = database_genre_save(&download->db, genres[j].name, &genre) < 0 ||
                  database_song_genre_append(&download->db, id, genre) < 0;
        }
        database_genres_free(genres, genres_count);
        if (bad) { goto failed; }
        ++copied;
    }
    if (!copied) { fail(&download->db, "No catalog songs selected"); goto failed; }
    int error = pthread_create(&download->thread, NULL, download_worker, download);
    if (error) { fail(&download->db, strerror(error)); goto failed; }
    database_songs_free(songs, count);
    snprintf(message, size, "Downloading %d song%s to ~/.sjb/songs...", copied, copied == 1 ? "" : "s");
    return(download);
failed:
    snprintf(message, size, "Download: %s", database_error(&download->db));
    database_songs_free(songs, count);
    database_close(&download->db);
    g_free(download->root);
    free(download);
    return(NULL);
}

int offline_download_poll(OfflineDownload *download, char *message, size_t size)
{
    if (!atomic_load(&download->finished)) { return(0); }
    if (download->result < 0) { snprintf(message, size, "Download failed: %s", database_error(&download->db)); }
    else { snprintf(message, size, "Download complete: songs saved in ~/.sjb/songs."); }
    return(1);
}

void offline_download_destroy(OfflineDownload *download)
{
    if (!download) { return; }
    atomic_store(&download->cancel, 1);
    pthread_join(download->thread, NULL);
    database_close(&download->db);
    g_free(download->root);
    free(download);
}
