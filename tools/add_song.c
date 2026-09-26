#define _POSIX_C_SOURCE 200809L
#include "../src/database.h"
#include "../src/lyrics_handler.h"
#include "genius.h"
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static volatile sig_atomic_t interrupted;

static void cancel_input(int signal_number)
{
    (void)signal_number;
    interrupted = 1;
}

static char *ask(const char *prompt, int required)
{
    for (;;) {
        if (interrupted) { return(NULL); }
        char *line = NULL;
        size_t capacity = 0;
        fputs(prompt, stdout);
        fflush(stdout);
        ssize_t length = getline(&line, &capacity, stdin);
        if (length < 0 || interrupted) { free(line); return(NULL); }
        if ((size_t)length != strlen(line)) { free(line); puts("Invalid input."); continue; }
        while (length && isspace((unsigned char)line[length - 1])) { line[--length] = '\0'; }
        size_t start = 0;
        while (isspace((unsigned char)line[start])) { ++start; }
        memmove(line, line + start, (size_t)length - start + 1);
        int valid = strlen(line) <= 8192;
        for (const unsigned char *p = (const unsigned char *)line; *p; ++p) {
            if (*p < 32 || *p == 127) { valid = 0; }
        }
        if (valid && (!required || *line)) { return(line); }
        free(line);
        puts("Enter printable text (required fields cannot be empty, maximum 8192 bytes).");
    }
}

static int confirm(void)
{
    char *answer = ask("Save to database? [y/N]: ", 0);
    int yes = answer && (!strcmp(answer, "y") || !strcmp(answer, "Y") || !strcmp(answer, "yes"));
    free(answer);
    return(yes);
}

static int fetch_lyrics(DbSong *song, int required)
{
    for (;;) {
        char *url = ask(required ? "Genius lyrics URL: " : "Genius lyrics URL (Enter to skip): ", required);
        if (!url) { return(-1); }
        if (!*url) { free(url); return(0); }
        char *lyrics = NULL, error[256];
        puts("Fetching lyrics...");
        if (genius_fetch_cancelable(url, &lyrics, error, sizeof error, &interrupted) == 0) {
            free(song->lyrics); free(song->lyrics_uri); free(song->lyrics_format);
            song->lyrics = lyrics;
            song->lyrics_uri = url;
            song->lyrics_format = strdup("plain");
            /* Web lyrics have no trustworthy timing metadata. */
            song->lyrics_start_ms = song->lyrics_end_ms = DB_TIME_UNKNOWN;
            if (!song->lyrics_format) { return(-1); }
            printf("\n--- Lyrics preview ---\n%s\n--- End lyrics ---\n\n", lyrics);
            return(0);
        }
        free(url);
        if (interrupted) { return(-1); }
        fprintf(stderr, "%s\n", error);
        puts(required ? "Try another URL, or Ctrl-D to cancel." : "Try another URL, Enter to skip lyrics, or Ctrl-D to cancel.");
    }
}

static int file_lyrics(DbSong *song, int timed)
{
    for (;;) {
        char *path = ask(timed ? "LRC file path: " : "Plain lyrics file path: ", 1);
        if (!path) { return(-1); }
        char *raw = NULL, error[256] = "Cannot read lyrics file (maximum 1 MiB, no embedded NULs).";
        Lyrics timeline = {0};
        if (timed) {
            if (lyrics_read(path, &raw, &timeline, error, sizeof error) == 0) {
                printf("%zu timed lines; %zu untimed/invalid lines ignored for playback.\n",
                       timeline.count, timeline.skipped_lines);
            }
            lyrics_free(&timeline);
        }

        else {
            FILE *file = fopen(path, "rb");
            if (file) {
                size_t limit = 1024 * 1024;
                raw = malloc(limit + 2);
                if (raw) {
                    size_t length = fread(raw, 1, limit + 1, file);
                    if (ferror(file) || length > limit || !length || memchr(raw, 0, length)) {
                        free(raw);
                        raw = NULL;
                    }

                    else { raw[length] = '\0'; }
                }
                fclose(file);
            }

            else { snprintf(error, sizeof error, "%s", strerror(errno)); }
        }
        if (!raw) {
            fprintf(stderr, "%s\n", error);
            free(path);
            if (interrupted) { return(-1); }
            puts("Try another file, or Ctrl-D to cancel.");
            continue;
        }
        free(song->lyrics); free(song->lyrics_uri); free(song->lyrics_format);
        song->lyrics = raw;
        song->lyrics_uri = path;
        song->lyrics_format = strdup(timed ? "lrc" : "plain");
        song->lyrics_start_ms = song->lyrics_end_ms = DB_TIME_UNKNOWN;
        if (!song->lyrics_format) { return(-1); }
        printf("Loaded %zu bytes; original file is unchanged.\n", strlen(raw));
        return(0);
    }
}

static int choose_lyrics(DbSong *song, int updating)
{
    for (;;) {
        char *choice = ask(updating ?
            "Lyrics: [l] LRC file, [p] plain file, [g] Genius, [n] clear, Enter cancel: " :
            "Lyrics: [l] LRC file, [p] plain file, [g] Genius, Enter none: ", 0);
        if (!choice) { return(-1); }
        char key = (char)tolower((unsigned char)choice[0]);
        int valid = !choice[0] || !choice[1];
        free(choice);
        if (!valid) { puts("Choose l, p, g, or n."); continue; }
        if (!key && updating) { return(-1); }
        if (!key || key == 'n') {
            free(song->lyrics); free(song->lyrics_uri); free(song->lyrics_format);
            song->lyrics = strdup("");
            song->lyrics_uri = strdup("");
            song->lyrics_format = strdup("plain");
            song->lyrics_start_ms = song->lyrics_end_ms = DB_TIME_UNKNOWN;
            return(song->lyrics && song->lyrics_uri && song->lyrics_format ? 0 : -1);
        }
        if (key == 'l' || key == 'p') { return(file_lyrics(song, key == 'l')); }
        if (key == 'g') { return(fetch_lyrics(song, 1)); }
        puts("Choose l, p, g, or n.");
    }
}

static int ask_time(const char *prompt, int required, int64_t minimum, int64_t maximum, int64_t *value)
{
    for (;;) {
        char *text = ask(prompt, required);
        if (!text) { return(-1); }
        if (!*text) { *value = DB_TIME_UNKNOWN; free(text); return(0); }
        int64_t time;
        int valid = database_duration_parse(text, &time) == 0 && time >= minimum &&
                    (maximum == DB_TIME_UNKNOWN || time <= maximum);
        free(text);
        if (valid) { *value = time; return(0); }
        puts("Enter nonnegative seconds (e.g. 247.5); end must follow start, and markers must fit the duration.");
    }
}

static int timing_markers(DbSong *song)
{
    char *answer = ask("Add optional lyrics/solo timing markers? [y/N]: ", 0);
    if (!answer) { return(-1); }
    int yes = !strcmp(answer, "y") || !strcmp(answer, "Y") || !strcmp(answer, "yes");
    free(answer);
    if (!yes) { return(0); }
    if (!strcmp(song->lyrics_format, "lrc")) { puts("LRC timestamps control lyric timing; additional lyric markers are unnecessary."); }

    else {
        if (ask_time("Lyrics start seconds (Enter to skip): ", 0, 0, song->duration_ms, &song->lyrics_start_ms) < 0) { return(-1); }
        if (song->lyrics_start_ms != DB_TIME_UNKNOWN &&
            ask_time("Lyrics end seconds (optional): ", 0, song->lyrics_start_ms, song->duration_ms, &song->lyrics_end_ms) < 0) { return(-1); }
    }
    if (ask_time("Solo start seconds (Enter to skip): ", 0, 0, song->duration_ms, &song->solo_start_ms) < 0) { return(-1); }
    if (song->solo_start_ms != DB_TIME_UNKNOWN &&
        ask_time("Solo end seconds: ", 1, song->solo_start_ms, song->duration_ms, &song->solo_end_ms) < 0) { return(-1); }
    return(0);
}

static void preview_time(const char *label, int64_t time)
{
    if (time == DB_TIME_UNKNOWN) { printf("%s: not set\n", label); }

    else { printf("%s: %" PRId64 ".%03" PRId64 " seconds\n", label, time / 1000, time % 1000); }
}

static void preview_song(const DbSong *song, const char *genre)
{
    printf("\nTitle: %s\nArtist: %s\nAlbum: %s\nAudio: %s\nCover: %s\n",
           song->title, song->artist, song->album, song->media_uri,
           song->cover_uri && *song->cover_uri ? song->cover_uri : "none");
    if (genre) { printf("Genre: %s\n", genre); }
    printf("Lyrics: %s (%zu bytes)\nLyrics source: %s\n", song->lyrics_format,
           strlen(song->lyrics), *song->lyrics_uri ? song->lyrics_uri : "none");
    preview_time("Duration", song->duration_ms);
    preview_time("Lyrics start", song->lyrics_start_ms);
    preview_time("Lyrics end", song->lyrics_end_ms);
    preview_time("Solo start", song->solo_start_ms);
    preview_time("Solo end", song->solo_end_ms);
}

static void usage(const char *program)
{
    printf("Usage: %s [--db PATH] [--list | --lyrics SONG_ID | --migrate]\n"
           "No action: interactively add a song. Ctrl-C or Ctrl-D cancels without saving.\n"
           "--list: show IDs for lyrics updates. --lyrics: preview and replace lyrics.\n"
           "Fields: title, artist, album, audio URL/path, cover URL/path, genre, duration,\n"
           "LRC/plain/Genius lyrics, and optional lyrics/solo timing markers.\n"
           "Duration is optional seconds; playback discovers it when omitted.\n"
           "A missing database is created automatically.\n", program);
}

int main(int argc, char **argv)
{
    struct sigaction action_sigint = {0};
    action_sigint.sa_handler = cancel_input;
    sigemptyset(&action_sigint.sa_mask);
    /* No SA_RESTART: interrupt getline as well as network requests. */
    if (sigaction(SIGINT, &action_sigint, NULL) < 0) { perror("sigaction"); return(1); }
    const char *path = DATABASE_PATH;
    enum { ADD, LIST, LYRICS, MIGRATE } action = ADD;
    int64_t selected = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--help")) { usage(argv[0]); return(0); }
        if (!strcmp(argv[i], "--db") && i + 1 < argc) { path = argv[++i]; }

        else if (!strcmp(argv[i], "--list") && action == ADD) { action = LIST; }

        else if (!strcmp(argv[i], "--migrate") && action == ADD) { action = MIGRATE; }

        else if (!strcmp(argv[i], "--lyrics") && i + 1 < argc && action == ADD) {
            char *end;
            errno = 0;
            selected = strtoll(argv[++i], &end, 10);
            if (errno || *end || selected <= 0) { usage(argv[0]); return(1); }
            action = LYRICS;
        }

        else { usage(argv[0]); return(1); }
    }
    Database db = {0};
    DbSong song = database_song_init();
    char *genre = NULL;
    int result = 1;
    if (database_open(&db, path) < 0) { goto failed; }
    if (action == MIGRATE) {
        puts("Database schema is current; catalog and existing lyrics retained.");
        result = 0;
        goto done;
    }
    if (action == LIST) {
        DbSong *songs = NULL;
        size_t count = 0;
        if (database_songs(&db, 0, &songs, &count) < 0) { goto failed; }
        for (size_t i = 0; i < count; ++i) {
            printf("%" PRId64 "  %s - %s%s\n", songs[i].id, songs[i].artist, songs[i].title,
                !strcmp(songs[i].lyrics_format, "lrc") ? " [LRC lyrics]" : *songs[i].lyrics ? " [plain lyrics]" : "");
        }
        database_songs_free(songs, count);
        result = 0;
        goto done;
    }
    if (action == LYRICS) {
        if (database_song_get(&db, selected, &song) < 0) { goto failed; }
        printf("Updating lyrics: %s - %s\n", song.artist, song.title);
        if (choose_lyrics(&song, 1) < 0) { goto cancelled; }
    }

    else {
        song.title = ask("Title: ", 1);
        if (!song.title) { goto cancelled; }
        song.artist = ask("Artist / author: ", 1);
        if (!song.artist) { goto cancelled; }
        song.album = ask("Album (optional): ", 0);
        if (!song.album) { goto cancelled; }
        song.cover_uri = ask("Cover image URL or local path (optional): ", 0);
        if (!song.cover_uri) { goto cancelled; }
        song.media_uri = ask("Audio URL (direct playable link) or local path: ", 1);
        if (!song.media_uri) { goto cancelled; }
        int64_t existing;
        int found = database_song_find(&db, song.media_uri, &existing);
        if (found < 0) { goto failed; }
        if (!found) {
            fprintf(stderr, "That audio link already belongs to song %" PRId64 ". Use --lyrics %" PRId64 " to update its lyrics.\n", existing, existing);
            goto done;
        }
        genre = ask("Genre (existing name or new genre): ", 1);
        if (!genre) { goto cancelled; }
        if (ask_time("Duration in seconds (Enter to detect during playback): ", 0, 0,
                     DB_TIME_UNKNOWN, &song.duration_ms) < 0) { goto cancelled; }
        if (song.duration_ms != DB_TIME_UNKNOWN) { song.duration_source = DB_DURATION_MANUAL; }
        if (choose_lyrics(&song, 0) < 0 || timing_markers(&song) < 0) { goto cancelled; }
    }
    preview_song(&song, genre);
    if (!confirm() || interrupted) { goto cancelled; }
    if (database_begin(&db) < 0) { goto failed; }
    int64_t id = song.id, genre_id;
    int saved = action == LYRICS ? database_lyrics_replace(&db, id, song.lyrics, song.lyrics_format, song.lyrics_uri) :
                                  database_song_save(&db, &song, &id);
    if (saved < 0 ||
        (genre && (database_genre_save(&db, genre, &genre_id) < 0 ||
                   database_song_genre_append(&db, id, genre_id) < 0)) || interrupted || database_commit(&db) < 0) {
        database_rollback(&db);
        goto failed;
    }
    printf("Saved song %" PRId64 " to %s. Restart the player to reload the catalog.\n", id, path);
    result = 0;
    goto done;
failed:
    if (interrupted) { goto cancelled; }
    fprintf(stderr, "Database error: %s\n", database_error(&db));
    goto done;
cancelled:
    puts("\nCancelled; no song changes saved.");
    result = interrupted ? 130 : 0;
done:
    free(genre);
    database_song_free(&song);
    database_close(&db);
    return(result);
}
