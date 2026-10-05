#include "session.h"
#include <glib.h>
#include <glib/gstdio.h>
#include <sqlite3.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define SESSION_MAX_TRACKS 100000
#define SESSION_MAX_BYTES (16 * 1024 * 1024)

static char *session_path(Database *db)
{
    const char *path = sqlite3_db_filename(db->handle, "main");
    return(path && *path ? g_strconcat(path, ".session", NULL) : NULL);
}

void session_free(Session *session)
{
    for (size_t i = 0; i < session->count; ++i) { g_free(session->tracks[i].uri); }
    free(session->tracks);
    free(session->order);
    *session = (Session){.current = -1, .volume = 100};
}

static int64_t number(GKeyFile *file, const char *group, const char *key, bool *valid)
{
    GError *error = NULL;
    int64_t value = g_key_file_get_int64(file, group, key, &error);
    if (error) { *valid = false; g_error_free(error); }
    return(value);
}

int session_load(Database *db, Session *session, char *error, size_t size)
{
    char *path = session_path(db);
    if (!path) { return(0); }
    GStatBuf info;
    int exists = g_stat(path, &info);
    if (exists < 0 && errno == ENOENT) { g_free(path); return(0); }
    GKeyFile *file = g_key_file_new();
    GError *cause = NULL;
    Session loaded = {.current = -1, .volume = 100};
    bool valid = true;
    int result = -1;
    if (exists < 0 || !S_ISREG(info.st_mode) || info.st_size > SESSION_MAX_BYTES ||
        !g_key_file_load_from_file(file, path, G_KEY_FILE_NONE, &cause)) { goto done; }
    int64_t version = number(file, "session", "version", &valid);
    int64_t count = number(file, "session", "count", &valid);
    loaded.current = number(file, "session", "current", &valid);
    loaded.position_ms = number(file, "session", "position_ms", &valid);
    int64_t volume = number(file, "session", "volume", &valid);
    int64_t loop = number(file, "session", "loop", &valid);
    int64_t shuffle = number(file, "session", "shuffle", &valid);
    if (!valid || version != 1 || count < 0 || count > SESSION_MAX_TRACKS ||
        loaded.current < -1 || loaded.current >= count || loaded.position_ms < 0 ||
        volume < 0 || volume > 100 || loop < 0 || loop > 2 || shuffle < 0 || shuffle > 1) { goto done; }
    loaded.volume = (int)volume;
    loaded.loop = (int)loop;
    loaded.shuffle = shuffle;
    if (count) {
        loaded.tracks = calloc((size_t)count, sizeof *loaded.tracks);
        loaded.order = calloc((size_t)count, sizeof *loaded.order);
        if (!loaded.tracks || !loaded.order) { goto done; }
    }
    loaded.count = (size_t)count;
    bool *seen = calloc(loaded.count + 1, sizeof *seen);
    if (!seen) { goto done; }
    for (size_t i = 0; i < loaded.count; ++i) {
        char group[40];
        snprintf(group, sizeof group, "track%zu", i);
        loaded.tracks[i].id = number(file, group, "id", &valid);
        int64_t order = number(file, group, "order", &valid);
        loaded.tracks[i].uri = g_key_file_get_string(file, group, "uri", NULL);
        if (!valid || !loaded.tracks[i].id || !loaded.tracks[i].uri || !*loaded.tracks[i].uri ||
            order < 0 || order >= count || seen[order]) { valid = false; break; }
        seen[order] = true;
        loaded.order[i] = (size_t)order;
    }
    free(seen);
    if (!valid) { goto done; }
    *session = loaded;
    loaded = (Session){0};
    result = 1;
done:
    if (result < 0) { snprintf(error, size, "Session ignored: %s", cause ? cause->message : "invalid or unreadable session file"); }
    g_clear_error(&cause);
    session_free(&loaded);
    g_key_file_unref(file);
    g_free(path);
    return(result);
}

int session_save(Database *db, const Session *session, char *error, size_t size)
{
    char *path = session_path(db);
    if (!path) { return(0); }
    if (session->count > SESSION_MAX_TRACKS) {
        snprintf(error, size, "Cannot save session: queue is too large");
        g_free(path);
        return(-1);
    }
    GKeyFile *file = g_key_file_new();
    g_key_file_set_int64(file, "session", "version", 1);
    g_key_file_set_int64(file, "session", "count", (int64_t)session->count);
    g_key_file_set_int64(file, "session", "current", session->current);
    g_key_file_set_int64(file, "session", "position_ms", session->position_ms);
    g_key_file_set_int64(file, "session", "volume", session->volume);
    g_key_file_set_int64(file, "session", "loop", session->loop);
    g_key_file_set_int64(file, "session", "shuffle", session->shuffle);
    for (size_t i = 0; i < session->count; ++i) {
        char group[40];
        snprintf(group, sizeof group, "track%zu", i);
        g_key_file_set_int64(file, group, "id", session->tracks[i].id);
        g_key_file_set_string(file, group, "uri", session->tracks[i].uri);
        g_key_file_set_int64(file, group, "order", (int64_t)session->order[i]);
    }
    gsize length;
    char *data = g_key_file_to_data(file, &length, NULL);
    GError *cause = NULL;
    bool saved = length <= SESSION_MAX_BYTES &&
        g_file_set_contents_full(path, data, (gssize)length,
                                 G_FILE_SET_CONTENTS_CONSISTENT | G_FILE_SET_CONTENTS_DURABLE, 0600, &cause);
    if (!saved) { snprintf(error, size, "Cannot save session: %s", cause ? cause->message : "session is too large"); }
    g_clear_error(&cause);
    g_free(data);
    g_key_file_unref(file);
    g_free(path);
    return(saved ? 0 : -1);
}
