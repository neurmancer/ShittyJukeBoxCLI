#include "src/TUI.h"
#include "src/database.h"
#include "src/audio_handler.h"
#include "src/cover_handler.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
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
} PlaybackSelection;

static int start_song(AudioPlayer *audio, TuiState *ui, Library *library, PlaybackSelection *selection, size_t genre, size_t index);
static size_t next_song(const SongMenu *section, size_t index, bool shuffle, bool previous);
static int import_lrc(Database *db, int64_t id, const char *path);
static SongMenu *playback_section(Library *library, size_t genre);
static void queue_bind(TuiState *ui, Library *library, PlaybackSelection *selection);
static int queue_add(Library *library, size_t index);
static void queue_edit(TuiState *ui, Library *library, PlaybackSelection *selection, TerminalAction action);



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
    srand((unsigned)time(NULL));
    
    TuiItem home_items[] = {
        {"Genres", TUI_BUTTON, true, false},
        {"Settings", TUI_BUTTON, true, false},
        {"Player", TUI_BUTTON, true, false},
        {"Queue your own songs", TUI_BUTTON, true, false},
        {"Quit", TUI_BUTTON, true, false}
    };
    
    TuiItem settings_items[] = {
        {"Show lyrics", TUI_TOGGLE, true, true},
        {"Loop playback", TUI_TOGGLE, true, false}
    };
    
    TuiMenu menus[] = {
        {.title = "ShittyJukeBox", .items = home_items, .count = 5, .wrap = true},
        {.title = "Genres", .items = library.items, .count = library.count, .wrap = true},
        {.title = "Settings", .items = settings_items, .count = 2}
    };
    
    for (size_t i = 0; i < sizeof menus / sizeof menus[0]; ++i) tui_menu_init(&menus[i]);
    
    if (terminal_init() < 0) {
        perror("Cannot initialize terminal");
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
    
    TuiItem queue_options[] = {
        {"Add songs", TUI_BUTTON, true, false},
        {"Edit custom queue", TUI_BUTTON, true, false},
        {"Play custom queue", TUI_BUTTON, true, false},
        {"Use default genre queue", TUI_BUTTON, true, false}
    };
    TuiMenu queue_menu = {.title = "Queue your own songs (session only)", .items = queue_options, .count = 4, .wrap = true};
    TuiMenu queue_add_menu = {.title = "Add songs: Enter adds, Esc returns", .items = library.sections[0].menu.items,
                             .count = library.sections[0].count, .wrap = true};
    library.custom.menu = (TuiMenu){.title = "Custom queue: Enter plays, x removes, K/J moves"};
    tui_menu_init(&queue_menu);
    tui_menu_init(&queue_add_menu);
    tui_menu_init(&library.custom.menu);
    TuiMenu queue = {.title = "Queue", .wrap = false};
    tui_menu_init(&queue);
    Spectrum spectrum;
    spectrum_init(&spectrum);
    TuiState ui = {
        .spectrum = &spectrum,
        .menus = {[SCREEN_HOME] = &menus[0], [SCREEN_GENRES] = &menus[1],
                  [SCREEN_SETTINGS] = &menus[2], [SCREEN_QUEUE_MENU] = &queue_menu,
                  [SCREEN_QUEUE_ADD] = &queue_add_menu, [SCREEN_QUEUE_EDIT] = &library.custom.menu},
        .queue = &queue,
        .player = &player
    };
    tui_state_init(&ui, preview ? SCREEN_PLAYER : SCREEN_HOME);
    int error = 0;
    size_t selected_genre = 0;
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
        TerminalAction action = terminal_read(refresh_ms);
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
        bool transport_pressed = ui.overlay == OVERLAY_NONE && action == TERM_ACTIVATE &&
                                 ((previous_screen == SCREEN_PLAYER && player.selected == PLAYER_PLAY) || previous_screen == SCREEN_VISUALIZER);
        bool editing_queue = ui.overlay == OVERLAY_NONE && ui.screen == SCREEN_QUEUE_EDIT &&
                             (action == TERM_REMOVE || action == TERM_MOVE_UP || action == TERM_MOVE_DOWN);
        if (editing_queue) { queue_edit(&ui, &library, &selection, action); }
        TuiResult result = volume_changed || editing_queue ? TUI_CHANGED : tui_state_handle(&ui, action);
        if (result == TUI_QUIT) { break; }

        if (previous_screen == SCREEN_SETTINGS) { player.repeat = settings_items[1].value; }

        else { settings_items[1].value = player.repeat; }
        player.lyrics_visible = settings_items[0].value;

        if (result == TUI_SELECTED) {
            if (ui.overlay == OVERLAY_QUEUE) {
                if (selection.song && queue.selected < queue.count) {
                    start_song(audio, &ui, &library, &selection, selection.genre, queue.selected);
                }
            }

            else if (ui.screen == SCREEN_HOME) {
                static const TuiScreen destinations[] = {SCREEN_GENRES, SCREEN_SETTINGS, SCREEN_PLAYER, SCREEN_QUEUE_MENU};
                size_t selected = menus[0].selected;
                if (selected < sizeof destinations / sizeof destinations[0]) {
                    tui_state_switch(&ui, destinations[selected]);
                    if (ui.screen == SCREEN_SETTINGS) { ui.status = "Settings last for this session."; }
                }

                else { running = false; }
            }

            else if (ui.screen == SCREEN_QUEUE_MENU) {
                if (queue_menu.selected == 0) { tui_state_switch(&ui, SCREEN_QUEUE_ADD); }
                else if (queue_menu.selected == 1) { tui_state_switch(&ui, SCREEN_QUEUE_EDIT); }
                else if (queue_menu.selected == 2) {
                    if (!library.custom.count) { ui.status = "Add songs to your custom queue first."; }
                    else if (start_song(audio, &ui, &library, &selection, library.count, 0) == 0) {
                        tui_state_switch(&ui, SCREEN_PLAYER);
                    }
                }
                else {
                    if (selection.song && selection.genre == library.count) {
                        selection.genre = 0;
                        for (size_t i = 0; i < library.sections[0].count; ++i) {
                            if (library.sections[0].songs[i].id == selection.song->id) { selection.index = i; break; }
                        }
                        queue_bind(&ui, &library, &selection);
                    }
                    ui.status = "Default queue: follows the genre you play. Custom queue kept for this session.";
                }
            }

            else if (ui.screen == SCREEN_QUEUE_ADD) {
                if (queue_add_menu.selected < queue_add_menu.count) {
                    if (queue_add(&library, queue_add_menu.selected) < 0) { ui.status = "Cannot allocate custom queue."; }
                    else {
                        if (selection.song && selection.genre == library.count) { queue_bind(&ui, &library, &selection); }
                        ui.status = "Added to custom queue. Esc returns; choose Play custom queue to use it.";
                    }
                }
            }

            else if (ui.screen == SCREEN_QUEUE_EDIT) {
                if (library.custom.menu.selected < library.custom.count &&
                    start_song(audio, &ui, &library, &selection, library.count, library.custom.menu.selected) == 0) {
                    tui_state_switch(&ui, SCREEN_PLAYER);
                }
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
                    if (start_song(audio, &ui, &library, &selection, selected_genre, section->menu.selected) == 0) {
                        player.selected = PLAYER_PLAY;
                        tui_state_switch(&ui, SCREEN_PLAYER);
                    }
                }
            }

            else if (ui.screen == SCREEN_PLAYER && selection.song && player.selected != PLAYER_PLAY) {
                SongMenu *section = playback_section(&library, selection.genre);
                size_t index = next_song(section, selection.index, player.shuffle, player.selected == PLAYER_PREVIOUS);
                start_song(audio, &ui, &library, &selection, selection.genre, index);
            }

            else if (ui.screen == SCREEN_PLAYER && !selection.song) { ui.status = "Select a song from Genres first."; }
        }

        else if (result == TUI_CHANGED && ui.screen == SCREEN_PLAYER) { ui.status = ""; }

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
                    size_t index = player.repeat ? selection.index : next_song(section, selection.index, player.shuffle, false);
                    start_song(audio, &ui, &library, &selection, selection.genre, index);
                }
            }
        }
        last_audio = status;
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

static size_t next_song(const SongMenu *section, size_t index, bool shuffle, bool previous)
{
    if (shuffle && section->count > 1) {
        return((index + 1 + (size_t)rand() % (section->count - 1)) % section->count);
    }
    return((index + (previous ? section->count - 1 : 1)) % section->count);
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
    selection->song = song;
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
    return(0);
no_memory:
    snprintf(db->error, sizeof db->error, "Cannot allocate library menus");
    return(-1);
}

static void library_free(Library *library)
{
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
