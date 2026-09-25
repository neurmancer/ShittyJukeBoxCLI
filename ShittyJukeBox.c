#include "src/TUI.h"
#include "src/database.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>
//She Loves Purple, So Do I...(yup everything may change this can not!)
//Dear beloved(Belgaphor's Prime) you won't be forgotten

//Here we go...once again...


typedef struct {
    DbSong *songs;
    size_t count;
    TuiMenu menu;
} SongMenu;

typedef struct {
    DbGenre *genres;
    size_t count;
    TuiItem *items;
    SongMenu *sections;
} Library;

static void library_free(Library *library)
{
    if (library->sections) {
        for (size_t i = 0; i < library->count; ++i) {
            database_songs_free(library->sections[i].songs, library->sections[i].count);
            free(library->sections[i].menu.items);
        }
    }
    free(library->sections);
    free(library->items);
    database_genres_free(library->genres, library->count);
    *library = (Library){0};
}

static int library_load(Database *db, Library *library)
{
    if (database_genres(db, &library->genres, &library->count) < 0) { return(-1); }
    if (!library->count) { return(0); }
    library->items = calloc(library->count, sizeof *library->items);
    library->sections = calloc(library->count, sizeof *library->sections);
    if (!library->items || !library->sections) { goto no_memory; }
    for (size_t i = 0; i < library->count; ++i) {
        library->items[i] = (TuiItem){library->genres[i].name, TUI_BUTTON, true, false};
        SongMenu *section = &library->sections[i];
        if (database_songs(db, library->genres[i].id, &section->songs, &section->count) < 0) { return(-1); }
        section->menu = (TuiMenu){.title = library->genres[i].name, .count = section->count, .wrap = true};
        if (section->count) {
            section->menu.items = calloc(section->count, sizeof *section->menu.items);
            if (!section->menu.items) { goto no_memory; }
        }
        for (size_t j = 0; j < section->count; ++j) {
            section->menu.items[j] = (TuiItem){section->songs[j].title, TUI_BUTTON, true, false};
        }
        tui_menu_init(&section->menu);
    }
    return(0);
no_memory:
    snprintf(db->error, sizeof db->error, "Cannot allocate library menus");
    return(-1);
}

static void select_song(TuiState *ui, const DbSong *song, char *cover_status, size_t size)
{
    TuiPlayer *player = ui->player;
    player->song_id = song->id;
    player->title = song->title;
    player->artist = song->artist;
    player->album = *song->album ? song->album : "Album unknown";
    player->lyrics = song->lyrics;
    player->duration_known = song->duration_ms != DB_TIME_UNKNOWN;
    player->duration = player->duration_known ? (unsigned long long)(song->duration_ms / 1000) : 0;
    player->elapsed = 0;
    player->paused = true;
    player->selected = PLAYER_PLAY;
    ui->lyrics_top = 0;
    terminal_cover_free();
    snprintf(cover_status, size, "No album cover");
    if (*song->cover_uri && terminal_cover_load(song->cover_uri) < 0) {
        snprintf(cover_status, size, "Cover: %s", strerror(errno));
    }
    tui_state_switch(ui, SCREEN_PLAYER);
}

int main(int argc, char **argv)
{
    setlocale(LC_CTYPE, "");
    const char *cover_path = NULL;
    const char *database_path = DATABASE_PATH;
    bool preview = false;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--preview")) { preview = true; }

        else if (!strcmp(argv[i], "--cover") && i + 1 < argc) {
            cover_path = argv[++i];
            preview = true;
        }

        else if (!strcmp(argv[i], "--db") && i + 1 < argc) { database_path = argv[++i]; }

        else {
            fprintf(stderr, "Usage: %s [--db jukebox.db] [--preview] [--cover album.png]\n", argv[0]);
            return(strcmp(argv[i], "--help") ? 1 : 0);
        }
    }

    Database db = {0};
    Library library = {0};
    if (database_open(&db, database_path) < 0 || library_load(&db, &library) < 0) {
        fprintf(stderr, "Library: %s\n", database_error(&db));
        library_free(&library);
        database_close(&db);
        return(1);
    }
    database_close(&db);
    
    TuiItem home_items[] = {
        {"Genres", TUI_BUTTON, true, false},
        {"Settings", TUI_BUTTON, true, false},
        {"Player", TUI_BUTTON, true, false},
        {"Quit", TUI_BUTTON, true, false}
    };
    
    TuiItem settings_items[] = {
        {"Show lyrics", TUI_TOGGLE, true, true},
        {"Loop playback", TUI_TOGGLE, true, false}
    };
    
    TuiMenu menus[] = {
        {"ShittyJukeBox", home_items, 4, 0, 0, true},
        {"Genres", library.items, library.count, 0, 0, true},
        {"Settings", settings_items, 2, 0, 0, false}
    };
    
    for (size_t i = 0; i < sizeof menus / sizeof menus[0]; ++i) tui_menu_init(&menus[i]);
    
    if (terminal_init() < 0) { perror("Cannot initialize terminal"); library_free(&library); return(1); }

    char cover_status[160] = "No album cover";
    if (cover_path && terminal_cover_load(cover_path) < 0) {
        snprintf(cover_status, sizeof cover_status, "Cover: %s", strerror(errno));
    }
    TuiPlayer player = {
        .artist = preview ? "Lady Gaga" : "", .title = preview ? "Judas" : "No song selected",
        .album = preview ? "Born This Way" : "Choose a song from Genres",
        .cover_status = cover_status, .elapsed = preview ? 1 : 0, .duration = preview ? 247 : 0,
        .duration_known = preview, .lyrics_visible = true,
        .selected = PLAYER_PLAY, .paused = true
    };
    
    TuiMenu queue = {.title = "Queue", .wrap = false};
    tui_menu_init(&queue);
    TuiState ui = {
        .menus = {[SCREEN_HOME] = &menus[0], [SCREEN_GENRES] = &menus[1],
                  [SCREEN_SETTINGS] = &menus[2]},
        .queue = &queue,
        .player = &player
    };
    tui_state_init(&ui, preview ? SCREEN_PLAYER : SCREEN_HOME);
    int error = 0;
    size_t selected_genre = 0;
    bool running = true;
    tui_state_draw(&ui);

    while (running) {
        TerminalAction action = terminal_read(100);
        if (action == TERM_ERROR) { error = errno; break; }
        TuiScreen previous_screen = ui.screen;
        TuiResult result = tui_state_handle(&ui, action);
        if (result == TUI_QUIT) { break; }

        if (previous_screen == SCREEN_SETTINGS) { player.repeat = settings_items[1].value; }

        else { settings_items[1].value = player.repeat; }
        player.lyrics_visible = settings_items[0].value;

        if (result == TUI_SELECTED) {
            if (ui.overlay == OVERLAY_QUEUE) {
                ui.status = "Queue playback is not connected yet.";
            }

            else if (ui.screen == SCREEN_HOME) {
                static const TuiScreen destinations[] = {SCREEN_GENRES, SCREEN_SETTINGS, SCREEN_PLAYER};
                size_t selected = menus[0].selected;
                if (selected < sizeof destinations / sizeof destinations[0]) {
                    tui_state_switch(&ui, destinations[selected]);
                    if (ui.screen == SCREEN_SETTINGS) { ui.status = "Settings last for this session."; }
                }

                else { running = false; }
            }

            else if (ui.screen == SCREEN_GENRES && menus[1].selected < library.count) {
                selected_genre = menus[1].selected;
                ui.menus[SCREEN_SONGS] = &library.sections[selected_genre].menu;
                tui_state_switch(&ui, SCREEN_SONGS);
                if (!library.sections[selected_genre].count) { ui.status = "This genre has no songs yet."; }
            }

            else if (ui.screen == SCREEN_SONGS) {
                SongMenu *section = &library.sections[selected_genre];
                if (section->menu.selected < section->count) {
                    select_song(&ui, &section->songs[section->menu.selected], cover_status, sizeof cover_status);
                }
            }

            else if (ui.screen == SCREEN_PLAYER) { ui.status = "No song queue connected yet."; }
        }

        else if (result == TUI_CHANGED && ui.screen == SCREEN_PLAYER) { ui.status = ""; }

        if (running && result != TUI_UNCHANGED) { tui_state_draw(&ui); }
    }

    int signal_number = terminal_signal();
    
    terminal_restore();
    library_free(&library);
    
    if (error) { fprintf(stderr, "Terminal input: %s\n", strerror(error)); return(1); }
    
    return(signal_number ? 128 + signal_number : 0);
}
