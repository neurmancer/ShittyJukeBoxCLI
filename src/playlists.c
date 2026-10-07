#define _XOPEN_SOURCE 700
#include "playlists.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PLAYLIST_APPLICATION_ID 1397375568

static int storage_open(Database *db)
{
    const char *library = sqlite3_db_filename(db->handle, "main");
    const char *slash = library ? strrchr(library, '/') : NULL;
    size_t prefix = slash ? (size_t)(slash - library + 1) : 0;
    char *path = malloc(prefix + sizeof "playlists.db");
    if (!path) { snprintf(db->error, sizeof db->error, "Cannot allocate playlist path"); return(-1); }
    if (prefix) { memcpy(path, library, prefix); }
    memcpy(path + prefix, "playlists.db", sizeof "playlists.db");
    sqlite3_stmt *stmt = NULL;
    int result = sqlite3_prepare_v2(db->handle, "ATTACH DATABASE ?1 AS saved", -1, &stmt, NULL);
    if (result == SQLITE_OK) { result = sqlite3_bind_text(stmt, 1, library && *library ? path : ":memory:", -1, SQLITE_TRANSIENT); }
    if (result == SQLITE_OK) { result = sqlite3_step(stmt); }
    sqlite3_finalize(stmt);
    free(path);
    if (result != SQLITE_DONE) { goto fail; }
    if (database_begin(db) < 0) { return(-1); }
    int application = 0, version = 0, tables = 0;
    const char *checks[] = {"PRAGMA saved.application_id", "PRAGMA saved.user_version",
        "SELECT count(*) FROM saved.sqlite_master WHERE name NOT LIKE 'sqlite_%'"};
    int *values[] = {&application, &version, &tables};
    for (size_t i = 0; i < 3; ++i) {
        stmt = NULL;
        result = sqlite3_prepare_v2(db->handle, checks[i], -1, &stmt, NULL);
        if (result == SQLITE_OK) { result = sqlite3_step(stmt); }
        if (result == SQLITE_ROW) { *values[i] = sqlite3_column_int(stmt, 0); }
        sqlite3_finalize(stmt);
        if (result != SQLITE_ROW) { goto rollback; }
    }
    if (!((application == 0 && version == 0 && tables == 0) ||
          (application == PLAYLIST_APPLICATION_ID && version == 1))) {
        snprintf(db->error, sizeof db->error, "Unrecognized or newer playlists.db; left unchanged");
        database_rollback(db);
        return(-1);
    }
    if (!version && sqlite3_exec(db->handle,
        "CREATE TABLE saved.playlists(id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL UNIQUE CHECK(length(trim(name))>0),cover_uri TEXT NOT NULL DEFAULT '');"
        "CREATE TABLE saved.playlist_songs(playlist_id INTEGER NOT NULL REFERENCES playlists(id) ON DELETE CASCADE,"
        "song_id INTEGER NOT NULL,position INTEGER NOT NULL CHECK(position>=0),PRIMARY KEY(playlist_id,song_id));"
        "CREATE INDEX saved.playlist_order ON playlist_songs(playlist_id,position,song_id);"
        "PRAGMA saved.application_id=1397375568;PRAGMA saved.user_version=1;",
        NULL, NULL, NULL) != SQLITE_OK) { goto rollback; }
    stmt = NULL;
    result = sqlite3_prepare_v2(db->handle, "SELECT count(*) FROM main.sqlite_master WHERE type='table' AND name='playlists'", -1, &stmt, NULL);
    if (result == SQLITE_OK) { result = sqlite3_step(stmt); }
    int legacy = result == SQLITE_ROW ? sqlite3_column_int(stmt, 0) : 0;
    sqlite3_finalize(stmt);
    if (result != SQLITE_ROW) { goto rollback; }
    if (legacy && sqlite3_exec(db->handle,
        "INSERT INTO saved.playlists(name) SELECT name FROM main.playlists WHERE 1 ON CONFLICT(name) DO NOTHING;"
        "INSERT OR IGNORE INTO saved.playlist_songs(playlist_id,song_id,position) "
        "SELECT target.id,entry.song_id,entry.position+coalesce(tail.last+1,0) "
        "FROM main.playlist_songs entry JOIN main.playlists old ON old.id=entry.playlist_id "
        "JOIN saved.playlists target ON target.name=old.name "
        "LEFT JOIN (SELECT playlist_id,max(position) AS last FROM saved.playlist_songs GROUP BY playlist_id) tail ON tail.playlist_id=target.id;"
        "DROP TABLE main.playlist_songs;DROP TABLE main.playlists;",
        NULL, NULL, NULL) != SQLITE_OK) { goto rollback; }
    if (database_commit(db) < 0) { goto rollback; }
    return(0);
rollback:
    snprintf(db->error, sizeof db->error, "%s", sqlite3_errmsg(db->handle));
    database_rollback(db);
    return(-1);
fail:
    snprintf(db->error, sizeof db->error, "%s", sqlite3_errmsg(db->handle));
    return(-1);
}

static int error(Playlists *panel)
{
    snprintf(panel->notice, sizeof panel->notice, "%s", sqlite3_errmsg(panel->db->handle));
    return(-1);
}

static void free_items(TuiItem *items, size_t count, int64_t *ids, char **artists, int64_t *durations)
{
    for (size_t i = 0; i < count; ++i) { free((char *)items[i].label); free((char *)items[i].media_uri); free(artists[i]); }
    free(items);
    free(ids);
    free(artists);
    free(durations);
}

void playlists_free(Playlists *panel)
{
    free_items(panel->menu.items, panel->menu.count, panel->ids, panel->artists, panel->durations);
    cover_destroy(panel->cover_loader);
    free(panel->cover_pixels);
    *panel = (Playlists){0};
}

static int refresh(Playlists *panel)
{
    sqlite3_stmt *stmt = NULL;
    const char *sql = panel->playlist_id ?
        "SELECT p.song_id,coalesce(s.title,'[Unavailable song]'),coalesce(s.artist,''),s.duration_ms,coalesce(s.media_uri,'') "
        "FROM saved.playlist_songs p LEFT JOIN main.songs s ON s.id=p.song_id WHERE p.playlist_id=?1 ORDER BY p.position,p.song_id" :
        "SELECT id,name,'',NULL,'' FROM saved.playlists ORDER BY id";
    if (sqlite3_prepare_v2(panel->db->handle, sql, -1, &stmt, NULL) != SQLITE_OK) { return(error(panel)); }
    if (panel->playlist_id) { sqlite3_bind_int64(stmt, 1, panel->playlist_id); }
    TuiItem *items = NULL;
    int64_t *ids = NULL, *durations = NULL;
    char **artists = NULL;
    size_t count = 0;
    int result;
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
        TuiItem *grown = realloc(items, (count + 1) * sizeof *items);
        if (!grown) { goto no_memory; }
        items = grown;
        int64_t *more = realloc(ids, (count + 1) * sizeof *ids);
        if (!more) { goto no_memory; }
        ids = more;
        char **more_artists = realloc(artists, (count + 1) * sizeof *artists);
        if (!more_artists) { goto no_memory; }
        artists = more_artists;
        int64_t *more_durations = realloc(durations, (count + 1) * sizeof *durations);
        if (!more_durations) { goto no_memory; }
        durations = more_durations;
        char *label = strdup((const char *)sqlite3_column_text(stmt, 1));
        char *artist = strdup((const char *)sqlite3_column_text(stmt, 2));
        char *uri = strdup((const char *)sqlite3_column_text(stmt, 4));
        if (!label || !artist || !uri) { free(label); free(artist); free(uri); goto no_memory; }
        ids[count] = sqlite3_column_int64(stmt, 0);
        artists[count] = artist;
        durations[count] = sqlite3_column_type(stmt, 3) == SQLITE_NULL ? -1 : sqlite3_column_int64(stmt, 3);
        items[count++] = (TuiItem){label, TUI_BUTTON, true, false, uri};
    }
    if (result != SQLITE_DONE) {
        error(panel);
        sqlite3_finalize(stmt);
        free_items(items, count, ids, artists, durations);
        return(-1);
    }
    sqlite3_finalize(stmt);
    char title[sizeof panel->title] = "", cover[sizeof panel->cover_uri] = "";
    if (panel->playlist_id) {
        stmt = NULL;
        result = sqlite3_prepare_v2(panel->db->handle, "SELECT name,cover_uri FROM saved.playlists WHERE id=?1", -1, &stmt, NULL);
        if (result == SQLITE_OK) { result = sqlite3_bind_int64(stmt, 1, panel->playlist_id); }
        if (result == SQLITE_OK) { result = sqlite3_step(stmt); }
        if (result == SQLITE_ROW) {
            snprintf(title, sizeof title, "%s", sqlite3_column_text(stmt, 0));
            snprintf(cover, sizeof cover, "%s", sqlite3_column_text(stmt, 1));
        }
        sqlite3_finalize(stmt);
        if (result != SQLITE_ROW) { free_items(items, count, ids, artists, durations); return(error(panel)); }
    }
    size_t selected = panel->menu.selected;
    free_items(panel->menu.items, panel->menu.count, panel->ids, panel->artists, panel->durations);
    if (!panel->playlist_id) {
        snprintf(panel->title, sizeof panel->title, "%s", panel->pending_song ? "Add to playlist" : "Playlists");
    }
    else { snprintf(panel->title, sizeof panel->title, "%s", title); }
    if (strcmp(cover, panel->cover_uri)) {
        snprintf(panel->cover_uri, sizeof panel->cover_uri, "%s", cover);
        free(panel->cover_pixels);
        panel->cover_pixels = NULL;
        cover_request(panel->cover_loader, panel->cover_uri);
    }
    panel->ids = ids;
    panel->artists = artists;
    panel->durations = durations;
    panel->menu = (TuiMenu){.title = panel->title, .items = items, .count = count, .wrap = true};
    tui_menu_init(&panel->menu);
    if (count && selected != SIZE_MAX) { panel->menu.selected = selected < count ? selected : count - 1; }
    return(0);
no_memory:
    snprintf(panel->notice, sizeof panel->notice, "Cannot allocate playlists.");
    sqlite3_finalize(stmt);
    free_items(items, count, ids, artists, durations);
    return(-1);
}

int playlists_init(Playlists *panel, Database *db)
{
    *panel = (Playlists){.db = db};
    if (storage_open(db) < 0) {
        snprintf(panel->notice, sizeof panel->notice, "%s", database_error(db));
        return(-1);
    }
    panel->cover_loader = cover_create();
    return(refresh(panel));
}

bool playlists_poll(Playlists *panel)
{
    size_t width, height;
    unsigned char *pixels = cover_take(panel->cover_loader, &width, &height);
    if (!pixels) { return(false); }
    free(panel->cover_pixels);
    panel->cover_pixels = pixels;
    panel->cover_width = width;
    panel->cover_height = height;
    return(true);
}

int playlists_picker(Playlists *panel, int64_t song_id)
{
    int64_t previous_id = panel->playlist_id;
    int64_t previous_song = panel->pending_song;
    PlaylistMode previous_mode = panel->mode;
    panel->playlist_id = 0;
    panel->pending_song = song_id;
    panel->mode = PLAYLIST_BROWSE;
    panel->notice[0] = '\0';
    if (refresh(panel) < 0) {
        panel->playlist_id = previous_id;
        panel->pending_song = previous_song;
        panel->mode = previous_mode;
        return(-1);
    }
    return(0);
}

static int write_change(Playlists *panel, const char *sql, int64_t id, int64_t song, const char *name)
{
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(panel->db->handle, sql, -1, &stmt, NULL) != SQLITE_OK) { return(error(panel)); }
    int index = sqlite3_bind_parameter_index(stmt, ":id");
    int result = index ? sqlite3_bind_int64(stmt, index, id) : SQLITE_OK;
    index = sqlite3_bind_parameter_index(stmt, ":song");
    if (result == SQLITE_OK && index) { result = sqlite3_bind_int64(stmt, index, song); }
    index = sqlite3_bind_parameter_index(stmt, ":name");
    if (result == SQLITE_OK && index) { result = sqlite3_bind_text(stmt, index, name, -1, SQLITE_TRANSIENT); }
    if (result == SQLITE_OK) { result = sqlite3_step(stmt); }
    if (result != SQLITE_DONE) {
        if (sqlite3_extended_errcode(panel->db->handle) == SQLITE_CONSTRAINT_UNIQUE) {
            snprintf(panel->notice, sizeof panel->notice, "That playlist name already exists.");
        }
        else { error(panel); }
    }
    sqlite3_finalize(stmt);
    return(result == SQLITE_DONE ? 0 : -1);
}

static int add_song(Playlists *panel, int64_t id)
{
    return(write_change(panel,
        "INSERT INTO saved.playlist_songs(playlist_id,song_id,position) "
        "SELECT :id,:song,coalesce(max(position)+1,0) FROM saved.playlist_songs WHERE playlist_id=:id "
        "ON CONFLICT(playlist_id,song_id) DO NOTHING", id, panel->pending_song, NULL));
}

static int move_song(Playlists *panel, size_t from, size_t to)
{
    if (database_begin(panel->db) < 0) { return(error(panel)); }
    if (write_change(panel, "DELETE FROM saved.playlist_songs WHERE playlist_id=:id", panel->playlist_id, 0, NULL) < 0) { goto fail; }
    for (size_t i = 0; i < panel->menu.count; ++i) {
        size_t index = i == from ? to : i == to ? from : i;
        panel->pending_song = panel->ids[index];
        if (add_song(panel, panel->playlist_id) < 0) { goto fail; }
    }
    panel->pending_song = 0;
    if (database_commit(panel->db) < 0) { error(panel); goto fail; }
    panel->menu.selected = to;
    return(0);
fail:
    panel->pending_song = 0;
    database_rollback(panel->db);
    return(-1);
}

PlaylistResult playlists_handle(Playlists *panel, TerminalAction action)
{
    if (panel->mode == PLAYLIST_COVER) {
        size_t length = strlen(panel->cover_input);
        if (action == TERM_BACK) { panel->mode = PLAYLIST_BROWSE; panel->notice[0] = '\0'; }
        else if (action == TERM_CLEAR_TEXT) { panel->cover_input[0] = '\0'; }
        else if (action == TERM_ERASE && length) {
            do { --length; } while (length && ((unsigned char)panel->cover_input[length] & 0xc0) == 0x80);
            panel->cover_input[length] = '\0';
        }
        else if (action == TERM_TEXT && strlen(terminal_text()) < sizeof panel->cover_input - length) {
            strcat(panel->cover_input, terminal_text());
        }
        else if (action == TERM_ACTIVATE) {
            const char *uri = panel->cover_input;
            char *absolute = NULL;
            if (*uri && strncmp(uri, "https://", 8) && strncmp(uri, "http://", 7)) {
                absolute = realpath(uri, NULL);
                if (!absolute || access(absolute, R_OK) != 0) {
                    free(absolute);
                    snprintf(panel->notice, sizeof panel->notice, "Cover file is not readable.");
                    return(PLAYLIST_CHANGED);
                }
                uri = absolute;
            }
            int result = write_change(panel, "UPDATE saved.playlists SET cover_uri=:name WHERE id=:id", panel->playlist_id, 0, uri);
            free(absolute);
            if (result == 0) {
                panel->mode = PLAYLIST_BROWSE;
                if (refresh(panel) == 0) {
                    free(panel->cover_pixels);
                    panel->cover_pixels = NULL;
                    cover_request(panel->cover_loader, panel->cover_uri);
                    snprintf(panel->notice, sizeof panel->notice, "%s", *panel->cover_uri ? "Playlist cover saved." : "Playlist cover removed.");
                }
            }
        }
        return(PLAYLIST_CHANGED);
    }
    if (panel->mode == PLAYLIST_CREATE || panel->mode == PLAYLIST_RENAME) {
        size_t length = strlen(panel->name);
        if (action == TERM_BACK) { panel->mode = PLAYLIST_BROWSE; panel->notice[0] = '\0'; }
        else if (action == TERM_CLEAR_TEXT) { panel->name[0] = '\0'; }
        else if (action == TERM_ERASE && length) {
            do { --length; } while (length && ((unsigned char)panel->name[length] & 0xc0) == 0x80);
            panel->name[length] = '\0';
        }
        else if (action == TERM_TEXT) {
            const char *input = terminal_text();
            if (strlen(input) < sizeof panel->name - length) { strcat(panel->name, input); }
        }
        else if (action == TERM_ACTIVATE) {
            const char *name = panel->name;
            while (*name == ' ') { ++name; }
            if (!*name) {
                snprintf(panel->notice, sizeof panel->notice, "Enter a playlist name.");
                return(PLAYLIST_CHANGED);
            }
            if (database_begin(panel->db) < 0) { error(panel); return(PLAYLIST_CHANGED); }
            bool creating = panel->mode == PLAYLIST_CREATE;
            int result = write_change(panel, creating ? "INSERT INTO saved.playlists(name) VALUES(trim(:name))" :
                "UPDATE saved.playlists SET name=trim(:name) WHERE id=:id", panel->edit_id, 0, panel->name);
            int64_t id = creating ? sqlite3_last_insert_rowid(panel->db->handle) : panel->edit_id;
            if (result == 0 && creating && panel->pending_song) { result = add_song(panel, id); }
            if (result < 0 || database_commit(panel->db) < 0) {
                if (result == 0) { error(panel); }
                database_rollback(panel->db);
                return(PLAYLIST_CHANGED);
            }
            bool added = creating && panel->pending_song;
            panel->pending_song = 0;
            panel->mode = PLAYLIST_BROWSE;
            if (refresh(panel) < 0) { return(PLAYLIST_CHANGED); }
            for (size_t i = 0; i < panel->menu.count; ++i) { if (panel->ids[i] == id) { panel->menu.selected = i; } }
            snprintf(panel->notice, sizeof panel->notice, "%s", added ? "Song saved to playlist." : "Playlist saved.");
            if (added) { return(PLAYLIST_CLOSE); }
        }
        return(PLAYLIST_CHANGED);
    }
    if (panel->mode == PLAYLIST_DELETE) {
        if (action == TERM_BACK) { panel->mode = PLAYLIST_BROWSE; }
        else if (action == TERM_ACTIVATE) {
            if (write_change(panel, "DELETE FROM saved.playlists WHERE id=:id", panel->edit_id, 0, NULL) == 0) {
                panel->mode = PLAYLIST_BROWSE;
                if (refresh(panel) == 0) { snprintf(panel->notice, sizeof panel->notice, "Playlist deleted; songs kept."); }
            }
        }
        return(PLAYLIST_CHANGED);
    }
    panel->notice[0] = '\0';
    if (action == TERM_BACK) {
        if (panel->playlist_id) { panel->playlist_id = 0; refresh(panel); }
        else { panel->pending_song = 0; return(PLAYLIST_CLOSE); }
    }
    else if (action == TERM_COVER && panel->playlist_id) {
        panel->mode = PLAYLIST_COVER;
        snprintf(panel->cover_input, sizeof panel->cover_input, "%s", panel->cover_uri);
    }
    else if (action == TERM_CREATE && !panel->playlist_id) {
        panel->mode = PLAYLIST_CREATE;
        panel->name[0] = '\0';
    }
    else if (panel->menu.selected < panel->menu.count) {
        size_t selected = panel->menu.selected;
        int64_t id = panel->ids[selected];
        if (action == TERM_ACTIVATE) {
            if (panel->playlist_id) { return(PLAYLIST_PLAY); }
            if (panel->pending_song) {
                if (add_song(panel, id) == 0) {
                    panel->pending_song = 0;
                    snprintf(panel->notice, sizeof panel->notice, "Song saved to playlist.");
                    return(PLAYLIST_CLOSE);
                }
            }
            else {
                panel->playlist_id = id;
                snprintf(panel->title, sizeof panel->title, "%s", panel->menu.items[selected].label);
                panel->menu.selected = 0;
                if (refresh(panel) < 0) { panel->playlist_id = 0; }
            }
        }
        else if (action == TERM_RENAME && !panel->playlist_id) {
            panel->mode = PLAYLIST_RENAME;
            panel->edit_id = id;
            snprintf(panel->name, sizeof panel->name, "%s", panel->menu.items[selected].label);
        }
        else if (action == TERM_REMOVE) {
            if (!panel->playlist_id) {
                panel->mode = PLAYLIST_DELETE;
                panel->edit_id = id;
                snprintf(panel->name, sizeof panel->name, "%s", panel->menu.items[selected].label);
            }
            else if (write_change(panel, "DELETE FROM saved.playlist_songs WHERE playlist_id=:id AND song_id=:song", panel->playlist_id, id, NULL) == 0) {
                if (refresh(panel) == 0) { snprintf(panel->notice, sizeof panel->notice, "Song removed from playlist."); }
            }
        }
        else if (panel->playlist_id && (action == TERM_MOVE_UP || action == TERM_MOVE_DOWN)) {
            if ((action == TERM_MOVE_UP && selected) || (action == TERM_MOVE_DOWN && selected + 1 < panel->menu.count)) {
                size_t to = action == TERM_MOVE_UP ? selected - 1 : selected + 1;
                if (move_song(panel, selected, to) == 0 && refresh(panel) == 0) {
                    snprintf(panel->notice, sizeof panel->notice, "Playlist order saved.");
                }
            }
        }
        else { tui_menu_handle(&panel->menu, action); }
    }
    return(PLAYLIST_CHANGED);
}
