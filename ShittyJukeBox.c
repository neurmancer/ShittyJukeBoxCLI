#include "src/TUI.h"
#include "src/database.h"
#include "src/audio_handler.h"
#include "src/cover_handler.h"
#include "src/shuffle.h"
#include "src/playlists.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
//She Loves Purple, So Do I...(yup everything may change this can not!)
//Dear beloved(Belgaphor's Prime) you won't be forgotten

//Here we go...once again...


//Man once I was using function prototypes before now I am just trying to make this shit work first...

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
    SongMenu custom;
    TuiMenu search;
    char search_query[256];
    size_t *search_indices;
    char **search_labels;
} Library;
static void library_free(Library *library);


static int library_load(Database *db, Library *library);


static void select_song(TuiState *ui, const DbSong *song);

typedef struct {
    DbSong *song;
    size_t genre;
    size_t index;
    uint64_t generation;
    CoverLoader *covers;
    const char *cover_override;
    size_t *order;
    TuiItem *queue_items;
    size_t order_count;
} PlaybackSelection;

static int start_song(AudioPlayer *audio, TuiState *ui, Library *library, PlaybackSelection *selection, size_t genre, size_t index);
static size_t next_song(const SongMenu *section, const PlaybackSelection *selection, bool previous);
static void shuffle_clear(PlaybackSelection *selection);
static int import_lrc(Database *db, int64_t id, const char *path);
static SongMenu *playback_section(Library *library, size_t genre);
static void queue_bind(TuiState *ui, Library *library, PlaybackSelection *selection);
static int queue_next(TuiState *ui, Library *library, PlaybackSelection *selection, const DbSong *song);
static void search_update(Library *library);
static void search_input(Library *library, TerminalAction action);
static int playlist_play(AudioPlayer *audio, TuiState *ui, Library *library, PlaybackSelection *selection, const Playlists *panel);



int main(int argc, char **argv)
{
    setlocale(LC_CTYPE, "");
    const char *cover_path = NULL;
    const char *database_path = DATABASE_PATH;
    bool preview = false;
    const char *lrc_path = NULL;
    int64_t lrc_song = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--preview")) { preview = true; }

        else if (!strcmp(argv[i], "--cover") && i + 1 < argc) {
            cover_path = argv[++i];
        }

        else if (!strcmp(argv[i], "--db") && i + 1 < argc) { database_path = argv[++i]; }

        else if (!strcmp(argv[i], "--import-lrc") && i + 2 < argc && !lrc_path) {
            char *end;
            errno = 0;
            lrc_song = strtoll(argv[++i], &end, 10);
            if (errno || *end || lrc_song <= 0) { fprintf(stderr, "Invalid song ID\n"); return(1); }
            lrc_path = argv[++i];
        }

        else {
            fprintf(stderr, "Usage: %s [--db jukebox.db] [--preview] [--cover album.png] [--import-lrc SONG_ID FILE]\n", argv[0]);
            return(strcmp(argv[i], "--help") ? 1 : 0);
        }
    }

    Database db = {0};
    Library library = {0};
    if (database_open(&db, database_path) < 0 || (!lrc_path && library_load(&db, &library) < 0)) {
        fprintf(stderr, "Library: %s\n", database_error(&db));
        library_free(&library);
        database_close(&db);
        return(1);
    }
    if (lrc_path) {
        int result = import_lrc(&db, lrc_song, lrc_path);
        database_close(&db);
        return(result);
    }
    char audio_error[256];
    AudioPlayer *audio = audio_create(audio_error, sizeof audio_error);
    if (!audio) {
        fprintf(stderr, "Audio: %s\n", audio_error);
        library_free(&library);
        database_close(&db);
        return(1);
    }
    
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
        {.title = "ShittyJukeBox", .items = home_items, .count = 4, .wrap = true},
        {.title = "Genres", .items = library.items, .count = library.count, .wrap = true},
        {.title = "Settings", .items = settings_items, .count = 2}
    };
    
    for (size_t i = 0; i < sizeof menus / sizeof menus[0]; ++i) tui_menu_init(&menus[i]);
    
    Playlists playlists = {0};
    if (playlists_init(&playlists, &db) < 0) {
        fprintf(stderr, "Playlists: %s\n", playlists.notice);
        playlists_free(&playlists);
        audio_destroy(audio);
        library_free(&library);
        database_close(&db);
        return(1);
    }
    if (terminal_init() < 0) {
        perror("Cannot initialize terminal");
        playlists_free(&playlists);
        audio_destroy(audio);
        library_free(&library);
        database_close(&db);
        return(1);
    }

    char cover_status[160] = "No album cover";
    if (preview && cover_path && terminal_cover_load(cover_path) < 0) {
        snprintf(cover_status, sizeof cover_status, "Cover: %s", strerror(errno));
    }
    Lyrics timed_lyrics = {0};
    TuiPlayer player = {
        .volume_percent = 100, .timed_lyrics = &timed_lyrics, .lyric_active = SIZE_MAX,
        .artist = preview ? "Lady Gaga" : "", .title = preview ? "Judas" : "No song selected",
        .album = preview ? "Born This Way" : "Choose a song from Genres",
        .cover_status = cover_status, .elapsed = preview ? 1 : 0, .duration = preview ? 247 : 0,
        .duration_known = preview, .show_cover = preview && cover_path, .lyrics_visible = true,
        .selected = PLAYER_PLAY, .paused = true
    };
    
    TuiMenu queue = {.title = "Queue", .wrap = false};
    tui_menu_init(&queue);
    Spectrum spectrum;
    spectrum_init(&spectrum);
    TuiState ui = {
        .spectrum = &spectrum,
        .menus = {[SCREEN_HOME] = &menus[0], [SCREEN_GENRES] = &menus[1],
                  [SCREEN_SETTINGS] = &menus[2]},
        .search_query = library.search_query,
        .queue = &queue,
        .playlists = &playlists,
        .player = &player
    };
    tui_state_init(&ui, preview ? SCREEN_PLAYER : SCREEN_HOME);
    int error = 0;
    size_t selected_genre = 0;
    bool search_from_player = false;
    PlaybackSelection selection = {
        .covers = terminal_cover_supported() ? cover_create() : NULL, .cover_override = cover_path
    };
    char volume_status[96] = "";
    char playback_status[256] = "Choose a song to start playback";
    AudioStatus last_audio = {.state = AUDIO_IDLE};
    uint64_t handled_end = 0;
    if (!preview) { player.playback_status = playback_status; }
    bool running = true;
    tui_state_draw(&ui);

    while (running) {
        bool timed_view = ui.screen == SCREEN_LYRICS && timed_lyrics.count && player.lyrics_visible && !player.paused;
        int refresh_ms = timed_view ? 10 : ui.screen == SCREEN_VISUALIZER ? 33 : 100;
        terminal_text_mode((ui.screen == SCREEN_SONGS && ui.search_editing && ui.overlay == OVERLAY_NONE) ||
                           (ui.overlay == OVERLAY_PLAYLISTS && (playlists.mode == PLAYLIST_CREATE || playlists.mode == PLAYLIST_RENAME || playlists.mode == PLAYLIST_COVER)));
        TerminalAction action = terminal_read(refresh_ms);
        if (action == TERM_SPACE && !(ui.overlay == OVERLAY_PLAYLISTS && playlists.playlist_id && playlists.mode == PLAYLIST_BROWSE)) {
            action = TERM_ACTIVATE;
        }
        if (action == TERM_ERROR) { error = errno; break; }
        if (action == TERM_ARROW_UP || action == TERM_ARROW_DOWN) {
            bool player_screen = (ui.screen == SCREEN_PLAYER || ui.screen == SCREEN_VISUALIZER) && ui.overlay == OVERLAY_NONE;
            action = action == TERM_ARROW_UP ? (player_screen ? TERM_VOLUME_UP : TERM_UP) :
                     (player_screen ? TERM_VOLUME_DOWN : TERM_DOWN);
        }
        bool volume_changed = action == TERM_VOLUME_UP || action == TERM_VOLUME_DOWN;
        if (volume_changed) {
            audio_set_volume(audio, player.volume_percent + (action == TERM_VOLUME_UP ? 5 : -5));
            player.volume_percent = audio_status(audio).volume_percent;
            snprintf(volume_status, sizeof volume_status, "Volume: %d%%", player.volume_percent);
            ui.status = volume_status;
            action = TERM_NONE;
        }
        TuiScreen previous_screen = ui.screen;
        bool was_shuffle = player.shuffle;
        bool transport_pressed = ui.overlay == OVERLAY_NONE && action == TERM_ACTIVATE &&
                                 ((previous_screen == SCREEN_PLAYER && player.selected == PLAYER_PLAY) || previous_screen == SCREEN_VISUALIZER);
        bool handled = false;
        bool search_focus_changed = false;
        if (action == TERM_PLAYLISTS) {
            if (ui.overlay == OVERLAY_PLAYLISTS) { ui.overlay = OVERLAY_NONE; }
            else if (playlists_picker(&playlists, 0) == 0) { ui.overlay = OVERLAY_PLAYLISTS; }
            else { ui.status = playlists.notice; }
            handled = true;
        }
        else if (action == TERM_ADD_PLAYLIST) {
            int64_t id = 0;
            if (ui.overlay == OVERLAY_QUEUE && queue.selected < queue.count) {
                size_t index = selection.order ? selection.order[queue.selected] : queue.selected;
                id = playback_section(&library, selection.genre)->songs[index].id;
            }
            else if (ui.overlay == OVERLAY_PLAYLISTS && playlists.mode == PLAYLIST_BROWSE && playlists.playlist_id &&
                     playlists.menu.selected < playlists.menu.count) { id = playlists.ids[playlists.menu.selected]; }
            else if (ui.overlay == OVERLAY_NONE && ui.screen == SCREEN_SONGS) {
                TuiMenu *menu = ui.menus[SCREEN_SONGS];
                if (menu->selected < menu->count) {
                    id = ui.search_active ? library.sections[0].songs[library.search_indices[menu->selected]].id :
                                           library.sections[selected_genre].songs[menu->selected].id;
                }
            }
            else if (ui.overlay == OVERLAY_NONE && ui.screen == SCREEN_PLAYER && selection.song) { id = selection.song->id; }
            if (id) {
                if (playlists_picker(&playlists, id) == 0) { ui.overlay = OVERLAY_PLAYLISTS; }
                else { ui.status = playlists.notice; }
            }
            handled = true;
        }
        else if (ui.overlay == OVERLAY_PLAYLISTS && action != TERM_QUIT && action != TERM_END) {
            if (action == TERM_QUEUE) { ui.overlay = OVERLAY_QUEUE; }
            else if (action == TERM_SHUFFLE && playlists.playlist_id && playlists.mode == PLAYLIST_BROWSE) { player.shuffle = !player.shuffle; }
            else if (action == TERM_REPEAT && playlists.playlist_id && playlists.mode == PLAYLIST_BROWSE) { player.repeat = !player.repeat; }
            else if (action == TERM_SPACE && ui.playlist_playing_id == playlists.playlist_id && selection.song && selection.genre == library.count) {
                AudioStatus current = audio_status(audio);
                if (current.state == AUDIO_FAILED || current.state == AUDIO_FINISHED || current.state == AUDIO_IDLE) {
                    start_song(audio, &ui, &library, &selection, selection.genre, selection.index);
                }
                else { audio_pause(audio, !current.pause_requested); }
            }
            else if (action != TERM_NONE && action != TERM_RESIZE) {
                if (action == TERM_SPACE) { action = TERM_ACTIVATE; }
                PlaylistResult change = playlists_handle(&playlists, action);
                if (change == PLAYLIST_CLOSE) { ui.overlay = OVERLAY_NONE; ui.status = playlists.notice; }
                else if (change == PLAYLIST_PLAY && playlist_play(audio, &ui, &library, &selection, &playlists) == 0) {
                    ui.playlist_playing_id = playlists.playlist_id;
                    player.selected = PLAYER_PLAY;
                    tui_state_switch(&ui, SCREEN_PLAYER);
                    ui.overlay = OVERLAY_PLAYLISTS;
                }
            }
            handled = action != TERM_NONE;
            /* Never let sidebar keys reach the underlying player or song list. */
            action = TERM_NONE;
        }
        if (ui.screen == SCREEN_PLAYER && ui.overlay == OVERLAY_NONE && action == TERM_SETTINGS) {
            search_from_player = true;
            tui_state_switch(&ui, SCREEN_SONGS);
        }
        if (ui.screen == SCREEN_SONGS && ui.overlay == OVERLAY_NONE) {
            if (action == TERM_SETTINGS) {
                ui.search_active = ui.search_editing = true;
                library.search.title = library.sections[selected_genre].menu.title;
                ui.menus[SCREEN_SONGS] = &library.search;
                ui.status = "";
                handled = true;
            }

            else if (action == TERM_BACK && ui.search_active) {
                ui.search_active = ui.search_editing = false;
                library.search_query[0] = '\0';
                search_update(&library);
                ui.menus[SCREEN_SONGS] = &library.sections[selected_genre].menu;
                ui.status = "";
                if (search_from_player) {
                    tui_state_handle(&ui, TERM_BACK);
                    search_from_player = false;
                }
                handled = true;
            }

            else if (ui.search_editing && (action == TERM_TEXT || action == TERM_ERASE || action == TERM_CLEAR_TEXT)) {
                search_input(&library, action);
                ui.status = "";
                handled = true;
            }

            else if (action == TERM_UP || action == TERM_DOWN || action == TERM_FIRST || action == TERM_LAST) {
                search_focus_changed = ui.search_editing;
                ui.search_editing = false;
            }
        }
        if (action == TERM_QUEUE_NEXT) {
            const DbSong *song = NULL;
            if (ui.overlay == OVERLAY_QUEUE && queue.selected < queue.count) {
                size_t index = selection.order ? selection.order[queue.selected] : queue.selected;
                song = &playback_section(&library, selection.genre)->songs[index];
            }

            else if (ui.screen == SCREEN_SONGS && ui.overlay == OVERLAY_NONE) {
                TuiMenu *menu = ui.menus[SCREEN_SONGS];
                if (menu->selected < menu->count) {
                    song = ui.search_active ? &library.sections[0].songs[library.search_indices[menu->selected]] :
                                             &library.sections[selected_genre].songs[menu->selected];
                }
            }
            if (song) {
                if (queue_next(&ui, &library, &selection, song) < 0) { ui.status = "Cannot allocate queue."; }
                else { ui.status = "Queued next. Q: view queue."; }
                handled = true;
            }
        }
        TuiResult result = volume_changed || handled ? TUI_CHANGED : tui_state_handle(&ui, action);
        if (search_focus_changed && result == TUI_UNCHANGED) { result = TUI_CHANGED; }
        if (result == TUI_QUIT) { break; }

        if (previous_screen == SCREEN_SETTINGS && !handled) { player.repeat = settings_items[1].value; }

        else { settings_items[1].value = player.repeat; }
        player.lyrics_visible = settings_items[0].value;

        if (result == TUI_SELECTED) {
            if (ui.overlay == OVERLAY_QUEUE) {
                if (queue.selected < queue.count) {
                    size_t index = selection.order ? selection.order[queue.selected] : queue.selected;
                    start_song(audio, &ui, &library, &selection, selection.genre, index);
                }
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
                ui.search_active = ui.search_editing = false;
                ui.menus[SCREEN_SONGS] = &library.sections[selected_genre].menu;
                tui_state_switch(&ui, SCREEN_SONGS);
                if (!library.sections[selected_genre].count) { ui.status = "This genre has no songs yet."; }
            }

            else if (ui.screen == SCREEN_SONGS) {
                TuiMenu *menu = ui.menus[SCREEN_SONGS];
                if (menu->selected < menu->count) {
                    size_t genre = ui.search_active ? 0 : selected_genre;
                    size_t index = ui.search_active ? library.search_indices[menu->selected] : menu->selected;
                    if (start_song(audio, &ui, &library, &selection, genre, index) == 0) {
                        ui.search_editing = false;
                        player.selected = PLAYER_PLAY;
                        if (search_from_player) {
                            tui_state_handle(&ui, TERM_BACK);
                            search_from_player = false;
                            ui.search_active = false;
                            ui.menus[SCREEN_SONGS] = &library.sections[selected_genre].menu;
                        }

                        else { tui_state_switch(&ui, SCREEN_PLAYER); }
                    }
                }
            }

            else if (ui.screen == SCREEN_PLAYER && selection.song && player.selected != PLAYER_PLAY) {
                SongMenu *section = playback_section(&library, selection.genre);
                size_t index = next_song(section, &selection, player.selected == PLAYER_PREVIOUS);
                start_song(audio, &ui, &library, &selection, selection.genre, index);
            }

            else if (ui.screen == SCREEN_PLAYER && !selection.song) { ui.status = "Select a song from Genres first."; }
        }

        else if (result == TUI_CHANGED && ui.screen == SCREEN_PLAYER && !handled) { ui.status = ""; }

        if (was_shuffle != player.shuffle && selection.song) {
            shuffle_clear(&selection);
            queue_bind(&ui, &library, &selection);
        }

        if (transport_pressed && selection.song) {
            AudioStatus status = audio_status(audio);
            if (status.state == AUDIO_FAILED || status.state == AUDIO_FINISHED || status.state == AUDIO_IDLE) {
                start_song(audio, &ui, &library, &selection, selection.genre, selection.index);
            }

            else { audio_pause(audio, !status.pause_requested); }
        }

        else if (transport_pressed && !preview) {
            player.paused = true;
            ui.status = "Select a song from Genres first.";
        }

        AudioStatus status = audio_status(audio);
        if (selection.song && status.song_id == selection.song->id && status.generation == selection.generation) {
            if (status.duration_ms >= 0 && (selection.song->duration_ms != status.duration_ms ||
                selection.song->duration_source != DB_DURATION_FFMPEG)) {
                selection.song->duration_ms = status.duration_ms;
                selection.song->duration_source = DB_DURATION_FFMPEG;
                if (database_duration_set(&db, selection.song->id, status.duration_ms, DB_DURATION_FFMPEG) < 0) {
                    ui.status = database_error(&db);
                }
            }
            player.duration_known = selection.song->duration_ms >= 0;
            player.duration = player.duration_known ? (unsigned long long)selection.song->duration_ms / 1000 : 0;
            size_t active = lyrics_active(&timed_lyrics, status.position_ms);
            if (active != player.lyric_active) {
                ui.lyrics_top = active == SIZE_MAX ? 0 : active > 2 ? active - 2 : 0;
                player.lyric_active = active;
            }
            if (timed_lyrics.count && player.lyrics_visible && ui.screen == SCREEN_LYRICS &&
                status.position_ms != player.position_ms) { result = TUI_CHANGED; }
            player.position_ms = status.position_ms;
            player.duration_ms = selection.song->duration_ms;
            player.elapsed = (unsigned long long)status.position_ms / 1000;
            player.loading = status.state == AUDIO_LOADING;
            player.paused = status.pause_requested || status.state == AUDIO_FINISHED || status.state == AUDIO_FAILED;
            const char *labels[] = {"Stopped", "Loading stream...", "Playing", "Paused", "Finished", "Playback failed"};
            snprintf(playback_status, sizeof playback_status, "%s", status.state == AUDIO_FAILED ? status.error : status.state == AUDIO_LOADING && status.pause_requested ? "Loading (paused)..." : labels[status.state]);
            player.playback_status = playback_status;
            if (status.generation != last_audio.generation || status.state != last_audio.state || status.pause_requested != last_audio.pause_requested ||
                status.position_ms / 1000 != last_audio.position_ms / 1000 || status.duration_ms != last_audio.duration_ms) {
                result = TUI_CHANGED;
            }
            if (status.state == AUDIO_FINISHED && handled_end != status.generation) {
                handled_end = status.generation;
                SongMenu *section = playback_section(&library, selection.genre);
                bool advance = player.repeat || player.shuffle || selection.index + 1 < section->count;
                if (advance) {
                    size_t index = player.repeat ? selection.index : next_song(section, &selection, false);
                    start_song(audio, &ui, &library, &selection, selection.genre, index);
                }
            }
        }
        last_audio = status;
        if (playlists_poll(&playlists)) { result = TUI_CHANGED; }
        if (ui.screen == SCREEN_VISUALIZER) {
            AudioSamples samples;
            audio_samples(audio, &samples);
            if (spectrum_update(&spectrum, &samples)) { result = TUI_CHANGED; }
        }

        size_t cover_width, cover_height;
        unsigned char *pixels = cover_take(selection.covers, &cover_width, &cover_height);
        if (pixels) {
            player.show_cover = terminal_cover_rgba(pixels, cover_width, cover_height) == 0;
            free(pixels);
            result = TUI_CHANGED;
        }

        if (running && result != TUI_UNCHANGED) { tui_state_draw(&ui); }
    }

    int signal_number = terminal_signal();
    
    cover_destroy(selection.covers);
    playlists_free(&playlists);
    shuffle_clear(&selection);
    terminal_restore();
    lyrics_free(&timed_lyrics);
    audio_destroy(audio);
    library_free(&library);
    database_close(&db);
    
    if (error) { fprintf(stderr, "Terminal input: %s\n", strerror(error)); return(1); }
    
    return(signal_number ? 128 + signal_number : 0);
}

static int import_lrc(Database *db, int64_t id, const char *path)
{
    Lyrics timeline = {0};
    char *source = NULL, error[256];
    DbSong song = database_song_init();
    int result = 1;
    if (lyrics_read(path, &source, &timeline, error, sizeof error) < 0) {
        fprintf(stderr, "LRC: %s\n", error);
        goto done;
    }
    if (database_begin(db) < 0) { goto database_error; }
    if (database_song_get(db, id, &song) < 0) { goto rollback; }
    free(song.lyrics);
    song.lyrics = source;
    source = NULL;
    free(song.lyrics_format);
    free(song.lyrics_uri);
    song.lyrics_format = malloc(4);
    song.lyrics_uri = malloc(strlen(path) + 1);
    if (!song.lyrics_format || !song.lyrics_uri) {
        snprintf(db->error, sizeof db->error, "Cannot allocate LRC metadata");
        goto rollback;
    }
    strcpy(song.lyrics_format, "lrc");
    strcpy(song.lyrics_uri, path);
    song.lyrics_start_ms = song.lyrics_end_ms = DB_TIME_UNKNOWN;
    if (database_song_save(db, &song, &id) < 0 || database_commit(db) < 0) { goto rollback; }
    printf("Imported %zu timed lines (%zu untimed/invalid lines ignored for playback).\n"
           "Original LRC bytes retained in the database; source file unchanged.\n",
           timeline.count, timeline.skipped_lines);
    result = 0;
    goto done;
rollback:
    database_rollback(db);
database_error:
    fprintf(stderr, "Library: %s\n", database_error(db));
done:
    free(source);
    lyrics_free(&timeline);
    database_song_free(&song);
    return(result);
}

static SongMenu *playback_section(Library *library, size_t genre)
{
    return(genre == library->count ? &library->custom : &library->sections[genre]);
}

static void shuffle_clear(PlaybackSelection *selection)
{
    free(selection->order);
    free(selection->queue_items);
    selection->order = NULL;
    selection->queue_items = NULL;
    selection->order_count = 0;
}

static void queue_bind(TuiState *ui, Library *library, PlaybackSelection *selection)
{
    SongMenu *section = playback_section(library, selection->genre);
    bool custom = selection->genre == library->count;
    if (ui->player->shuffle && !selection->order && section->count) {
        size_t *order = section->count <= SIZE_MAX / sizeof *order ? malloc(section->count * sizeof *order) : NULL;
        TuiItem *items = section->count <= SIZE_MAX / sizeof *items ? malloc(section->count * sizeof *items) : NULL;
        if (!order || !items || shuffle_order(order, section->count, selection->index) < 0) {
            free(order);
            free(items);
            ui->player->shuffle = false;
            ui->status = "Cannot shuffle: allocation or system random source failed.";
        }

        else {
            selection->order = order;
            selection->queue_items = items;
            selection->order_count = section->count;
            for (size_t i = 0; i < section->count; ++i) { items[i] = section->menu.items[order[i]]; }
        }
    }
    size_t position = selection->index;
    if (selection->order) {
        for (size_t i = 0; i < selection->order_count; ++i) {
            if (selection->order[i] == selection->index) { position = i; break; }
        }
    }
    for (size_t i = 0; i < library->count; ++i) {
        library->sections[i].menu.has_active = false;
    }
    library->custom.menu.has_active = false;
    section->menu.has_active = selection->song != NULL;
    section->menu.active = selection->index;
    ui->menus[SCREEN_GENRES]->has_active = selection->song && !custom;
    ui->menus[SCREEN_GENRES]->active = selection->genre;
    *ui->queue = (TuiMenu){.title = selection->order ? "Queue: shuffled" : custom ? "Queue: custom" : "Queue: current genre",
                           .items = selection->order ? selection->queue_items : section->menu.items, .count = section->count,
                           .selected = position, .has_active = selection->song != NULL,
                           .active = position};
}

static int queue_next(TuiState *ui, Library *library, PlaybackSelection *selection, const DbSong *song)
{
    SongMenu *source = selection->song ? playback_section(library, selection->genre) : &library->custom;
    size_t count = source->count;
    if (count == SIZE_MAX || count + 1 > SIZE_MAX / sizeof(DbSong) ||
        count + 1 > SIZE_MAX / sizeof(TuiItem) || count + 1 > SIZE_MAX / sizeof(size_t)) { return(-1); }
    DbSong *songs = malloc((count + 1) * sizeof *songs);
    TuiItem *items = malloc((count + 1) * sizeof *items);
    size_t *order = selection->order ? malloc((count + 1) * sizeof *order) : NULL;
    TuiItem *shuffled = selection->order ? malloc((count + 1) * sizeof *shuffled) : NULL;
    if (!songs || !items || (selection->order && (!order || !shuffled))) {
        free(songs); free(items); free(order); free(shuffled);
        return(-1);
    }
    size_t insert = selection->song ? selection->index + 1 : 0;
    for (size_t i = 0; i < count + 1; ++i) {
        songs[i] = i == insert ? *song : source->songs[i < insert ? i : i - 1];
        items[i] = (TuiItem){songs[i].title, TUI_BUTTON, true, false};
    }
    if (order) {
        size_t at = 0;
        for (size_t i = 0; i < count; ++i) {
            size_t original = selection->order[i];
            order[at++] = original < insert ? original : original + 1;
            if (selection->song && original == selection->index) { order[at++] = insert; }
        }
        if (!selection->song) {
            memmove(order + 1, order, count * sizeof *order);
            order[0] = insert;
        }
        for (size_t i = 0; i < count + 1; ++i) { shuffled[i] = items[order[i]]; }
    }
    /* Copy before replacing: the hovered song can belong to the old queue. */
    free(library->custom.songs);
    free(library->custom.menu.items);
    library->custom = (SongMenu){.songs = songs, .count = count + 1,
                                .menu = {.items = items, .count = count + 1}};
    shuffle_clear(selection);
    selection->order = order;
    selection->queue_items = shuffled;
    selection->order_count = order ? count + 1 : 0;
    selection->genre = library->count;
    if (!selection->song) { selection->index = 0; }
    queue_bind(ui, library, selection);
    return(0);
}

static size_t next_song(const SongMenu *section, const PlaybackSelection *selection, bool previous)
{
    size_t index = selection->index;
    if (selection->order) {
        for (size_t i = 0; i < selection->order_count; ++i) {
            if (selection->order[i] == index) {
                size_t next = previous ? (i ? i - 1 : section->count - 1) : (i + 1) % section->count;
                return(selection->order[next]);
            }
        }
    }
    return(previous ? (index ? index - 1 : section->count - 1) : (index + 1) % section->count);
}

static int start_song(AudioPlayer *audio, TuiState *ui, Library *library,
                      PlaybackSelection *selection, size_t genre, size_t index)
{
    SongMenu *section = playback_section(library, genre);
    if (index >= section->count) { return(-1); }
    DbSong *song = &section->songs[index];
    if (genre == library->count) {
        for (size_t i = 0; i < library->sections[0].count; ++i) {
            if (library->sections[0].songs[i].id == song->id) { song = &library->sections[0].songs[i]; break; }
        }
    }
    if (audio_play(audio, song->id, song->media_uri) < 0) {
        ui->status = "Cannot allocate playback request.";
        return(-1);
    }
    if (genre < library->count) { ui->playlist_playing_id = 0; }
    selection->song = song;
    if (selection->genre != genre) { shuffle_clear(selection); }
    selection->genre = genre;
    selection->index = index;
    selection->generation = audio_status(audio).generation;
    select_song(ui, song);
    cover_request(selection->covers, selection->cover_override ? selection->cover_override : song->cover_uri);
    ui->player->paused = false;
    ui->status = "";
    queue_bind(ui, library, selection);
    return(0);
}

static int playlist_play(AudioPlayer *audio, TuiState *ui, Library *library,
                         PlaybackSelection *selection, const Playlists *panel)
{
    size_t count = panel->menu.count;
    SongMenu snapshot = {.count = count};
    snapshot.songs = calloc(count, sizeof *snapshot.songs);
    snapshot.menu.items = calloc(count, sizeof *snapshot.menu.items);
    if (!snapshot.songs || !snapshot.menu.items) {
        free(snapshot.songs); free(snapshot.menu.items);
        ui->status = "Cannot allocate playlist queue.";
        return(-1);
    }
    snapshot.menu.count = count;
    for (size_t i = 0; i < count; ++i) {
        bool found = false;
        for (size_t j = 0; j < library->sections[0].count; ++j) {
            if (library->sections[0].songs[j].id == panel->ids[i]) {
                snapshot.songs[i] = library->sections[0].songs[j];
                snapshot.menu.items[i] = (TuiItem){snapshot.songs[i].title, TUI_BUTTON, true, false};
                found = true;
                break;
            }
        }
        if (!found) {
            free(snapshot.songs); free(snapshot.menu.items);
            ui->status = "Library changed; restart to load the new songs.";
            return(-1);
        }
    }
    SongMenu previous = library->custom;
    library->custom = snapshot;
    PlaybackSelection next = *selection;
    next.order = NULL;
    next.queue_items = NULL;
    next.order_count = 0;
    if (start_song(audio, ui, library, &next, library->count, panel->menu.selected) < 0) {
        library->custom = previous;
        free(snapshot.songs); free(snapshot.menu.items);
        return(-1);
    }
    shuffle_clear(selection);
    *selection = next;
    free(previous.songs); free(previous.menu.items);
    return(0);
}

static void select_song(TuiState *ui, const DbSong *song)
{
    TuiPlayer *player = ui->player;
    player->song_id = song->id;
    player->title = song->title;
    player->artist = song->artist;
    player->album = *song->album ? song->album : "Album unknown";
    player->lyrics = song->lyrics;
    lyrics_free(player->timed_lyrics);
    player->lyric_active = SIZE_MAX;
    player->position_ms = 0;
    player->duration_ms = song->duration_ms;
    if (!strcmp(song->lyrics_format, "lrc")) {
        char error[160];
        if (lyrics_parse(song->lyrics, player->timed_lyrics, error, sizeof error) < 0) {
            player->lyrics = "Cannot parse timed lyrics; re-import a valid LRC file.";
        }
    }
    player->duration_known = song->duration_ms != DB_TIME_UNKNOWN;
    player->duration = player->duration_known ? (unsigned long long)(song->duration_ms / 1000) : 0;
    player->elapsed = 0;
    player->paused = true;
    ui->lyrics_top = 0;
    player->show_cover = false;
    player->loading = true;
    player->playback_status = "Loading stream...";
    terminal_cover_free();
}

static wint_t search_character(const char **text)
{
    wchar_t character;
    mbstate_t state = {0};
    size_t length = mbrtowc(&character, *text, MB_CUR_MAX, &state);
    if (length == (size_t)-1 || length == (size_t)-2) {
        character = (unsigned char)**text;
        length = 1;
    }
    *text += length;
    /* Keep ASCII matching predictable even in a Turkish locale. */
    return(character >= 'A' && character <= 'Z' ? (wint_t)(character + ('a' - 'A')) : towlower(character));
}

static bool search_prefix(const char *text, const char *query)
{
    while (*query) {
        if (!*text || search_character(&text) != search_character(&query)) { return(false); }
    }
    return(true);
}

static bool search_contains(const char *text, const char *query)
{
    do {
        if (search_prefix(text, query)) { return(true); }
        if (!*text) { break; }
        search_character(&text);
    } while (*text);
    return(false);
}

static void search_update(Library *library)
{
    library->search.count = 0;
    SongMenu *all = &library->sections[0];
    for (int rank = 0; rank < 3; ++rank) {
        for (size_t i = 0; i < all->count; ++i) {
            DbSong *song = &all->songs[i];
            int score = search_prefix(song->title, library->search_query) ? 0 :
                        search_contains(song->title, library->search_query) ? 1 :
                        search_contains(song->artist, library->search_query) ? 2 : -1;
            if (score == rank) {
                size_t index = library->search.count++;
                library->search_indices[index] = i;
                library->search.items[index] = (TuiItem){library->search_labels[i], TUI_BUTTON, true, false};
            }
        }
    }
    tui_menu_init(&library->search);
}

static void search_input(Library *library, TerminalAction action)
{
    char *query = library->search_query;
    size_t length = strlen(query);
    if (action == TERM_CLEAR_TEXT) { query[0] = '\0'; }

    else if (action == TERM_ERASE && length) {
        do { --length; } while (length && ((unsigned char)query[length] & 0xc0) == 0x80);
        query[length] = '\0';
    }

    else if (action == TERM_TEXT) {
        const char *input = terminal_text();
        size_t added = strlen(input);
        if (added < sizeof library->search_query - length) { memcpy(query + length, input, added + 1); }
    }
    search_update(library);
}

static int library_load(Database *db, Library *library)
{
    if (database_genres(db, &library->genres, &library->count) < 0) { return(-1); }
    DbGenre *genres = realloc(library->genres, (library->count + 1) * sizeof *genres);
    if (!genres) { goto no_memory; }
    library->genres = genres;
    char *all_name = malloc(sizeof "All");
    if (!all_name) { goto no_memory; }
    memcpy(all_name, "All", sizeof "All");
    memmove(genres + 1, genres, library->count * sizeof *genres);
    /* Genre ID zero queries every song, including songs with no genre. */
    genres[0] = (DbGenre){.id = 0, .name = all_name};
    ++library->count;
    library->items = calloc(library->count, sizeof *library->items);
    library->sections = calloc(library->count, sizeof *library->sections);
    if (!library->items || !library->sections) { goto no_memory; }
    for (size_t i = 0; i < library->count; ++i) {
        library->items[i] = (TuiItem){library->genres[i].name, TUI_BUTTON, true, false};
        SongMenu *section = &library->sections[i];
        if (database_songs(db, library->genres[i].id, &section->songs, &section->count) < 0) { return(-1); }
        library->genres[i].song_count = section->count;
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
    size_t count = library->sections[0].count;
    library->search = (TuiMenu){.title = "Search songs", .wrap = true};
    if (count) {
        library->search.items = calloc(count, sizeof *library->search.items);
        library->search_indices = calloc(count, sizeof *library->search_indices);
        library->search_labels = calloc(count, sizeof *library->search_labels);
        if (!library->search.items || !library->search_indices || !library->search_labels) { goto no_memory; }
        for (size_t i = 0; i < count; ++i) {
            DbSong *song = &library->sections[0].songs[i];
            size_t length = strlen(song->title) + strlen(song->artist) + 4;
            library->search_labels[i] = malloc(length);
            if (!library->search_labels[i]) { goto no_memory; }
            snprintf(library->search_labels[i], length, "%s%s%s", song->title, *song->artist ? " - " : "", song->artist);
        }
    }
    search_update(library);
    return(0);
no_memory:
    snprintf(db->error, sizeof db->error, "Cannot allocate library menus");
    return(-1);
}

static void library_free(Library *library)
{
    if (library->search_labels) {
        for (size_t i = 0; i < library->sections[0].count; ++i) { free(library->search_labels[i]); }
    }
    free(library->search_labels);
    free(library->search_indices);
    free(library->search.items);
    if (library->sections) {
        for (size_t i = 0; i < library->count; ++i) {
            database_songs_free(library->sections[i].songs, library->sections[i].count);
            free(library->sections[i].menu.items);
        }
    }
    free(library->custom.songs);
    free(library->custom.menu.items);
    free(library->sections);
    free(library->items);
    database_genres_free(library->genres, library->count);
    *library = (Library){0};
}
