#include "src/TUI.h"
#include "src/database.h"
#include "src/audio_handler.h"
#include "src/cover_handler.h"
#include "src/shuffle.h"
#include "src/playlists.h"
#include "src/config.h"
#include "src/mpris.h"
#include "src/offline.h"
#include "src/session.h"
#include <glib.h>
#include <errno.h>
#ifdef SJB_SYSTEM_INSTALL
#include <pwd.h>
#include <unistd.h>
#endif
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

typedef struct ExternalSong {
    DbSong song;
    struct ExternalSong *next;
} ExternalSong;

typedef struct {
    ExternalSong *external;
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
static int start_song_at(AudioPlayer *audio, TuiState *ui, Library *library, PlaybackSelection *selection,
                         size_t genre, size_t index, int64_t position_ms, bool paused);
static void restore_session(Database *db, AudioPlayer *audio, TuiState *ui, Library *library,
                            PlaybackSelection *selection, bool offline, char *message, size_t size);
static int save_session(Database *db, AudioPlayer *audio, TuiState *ui, Library *library,
                         PlaybackSelection *selection, char *message, size_t size);
static size_t next_song(const SongMenu *section, const PlaybackSelection *selection, bool previous);
static void shuffle_clear(PlaybackSelection *selection);
static int import_lrc(Database *db, int64_t id, const char *path);
static SongMenu *playback_section(Library *library, size_t genre);
static void queue_bind(TuiState *ui, Library *library, PlaybackSelection *selection);
static int queue_next(TuiState *ui, Library *library, PlaybackSelection *selection, const DbSong *song);
static void queue_edit(TuiState *ui, Library *library, PlaybackSelection *selection, TerminalAction action);
static void search_update(Library *library);
static void search_input(Library *library, TerminalAction action);
static int playlist_play(AudioPlayer *audio, TuiState *ui, Library *library, PlaybackSelection *selection, const Playlists *panel);


typedef struct {
    AudioPlayer *audio;
    TuiState *ui;
    Library *library;
    PlaybackSelection *selection;
    bool *running;
    bool offline;
} RemotePlayer;

static size_t queue_position(const PlaybackSelection *selection)
{
    if (selection->order) {
        for (size_t i = 0; i < selection->order_count; ++i) {
            if (selection->order[i] == selection->index) { return(i); }
        }
    }
    return(selection->index);
}

static MprisState remote_read(void *opaque)
{
    RemotePlayer *remote = opaque;
    PlaybackSelection *selection = remote->selection;
    SongMenu *section = playback_section(remote->library, selection->genre);
    size_t position = queue_position(selection);
    bool wrap = remote->ui->player->repeat_playlist || remote->ui->player->repeat;
    return((MprisState){
        .audio = audio_status(remote->audio), .song = selection->song,
        .lyrics = remote->ui->player->timed_lyrics,
        .backing_lyrics = remote->ui->player->backing_lyrics,
        .art_uri = selection->cover_override ? selection->cover_override : selection->song ? selection->song->cover_uri : "",
        .track = selection->generation, .shuffle = remote->ui->player->shuffle,
        .loop = remote->ui->player->repeat ? 1 : remote->ui->player->repeat_playlist ? 2 : 0,
        .can_play = selection->song || remote->library->sections[0].count,
        .can_next = selection->song && (wrap || position + 1 < section->count),
        .can_previous = selection->song && (wrap || position > 0)
    });
}

static bool remote_control(void *opaque, MprisCommand command, int64_t value, const char *text)
{
    RemotePlayer *remote = opaque;
    PlaybackSelection *selection = remote->selection;
    TuiPlayer *player = remote->ui->player;
    AudioStatus status = audio_status(remote->audio);
    bool stopped = status.state == AUDIO_IDLE || status.state == AUDIO_FINISHED || status.state == AUDIO_FAILED;
    if (command == MPRIS_QUIT) { tui_quit_request(remote->ui); }
    else if (command == MPRIS_VOLUME) {
        audio_set_volume(remote->audio, (int)value);
        player->volume_percent = audio_status(remote->audio).volume_percent;
    }
    else if (command == MPRIS_LOOP) {
        player->repeat = value == 1;
        remote->ui->player->repeat_playlist = value == 2;
        remote->ui->menus[SCREEN_SETTINGS]->items[1].value = player->repeat || player->repeat_playlist;
    }
    else if (command == MPRIS_SHUFFLE && player->shuffle != (bool)value) {
        player->shuffle = value;
        shuffle_clear(selection);
        if (selection->song) { queue_bind(remote->ui, remote->library, selection); }
    }
    else if (command == MPRIS_STOP) { audio_stop(remote->audio); }
    else if (command == MPRIS_SEEK) { audio_seek(remote->audio, value); }
    else if (command == MPRIS_PAUSE) {
        if (!stopped) { audio_pause(remote->audio, true); }
    }
    else if (command == MPRIS_PLAY || command == MPRIS_TOGGLE) {
        if (stopped) {
            if (!selection->song && !remote->library->sections[0].count) { return(true); }
            return(start_song(remote->audio, remote->ui, remote->library, selection,
                              selection->song ? selection->genre : 0, selection->song ? selection->index : 0) == 0);
        }
        audio_pause(remote->audio, command == MPRIS_TOGGLE ? !status.pause_requested : false);
    }
    else if (command == MPRIS_NEXT || command == MPRIS_PREVIOUS) {
        MprisState state = remote_read(remote);
        if (!(command == MPRIS_NEXT ? state.can_next : state.can_previous)) { return(true); }
        SongMenu *section = playback_section(remote->library, selection->genre);
        size_t index = next_song(section, selection, command == MPRIS_PREVIOUS);
        if (start_song(remote->audio, remote->ui, remote->library, selection, selection->genre, index) < 0) { return(false); }
        if (stopped) { audio_stop(remote->audio); }
        else if (status.pause_requested) { audio_pause(remote->audio, true); }
    }
    else if (command == MPRIS_OPEN) {
        if (remote->offline && text[0] != '/' && !g_str_has_prefix(text, "file:///")) { return(false); }
        SongMenu *all = &remote->library->sections[0];
        for (size_t i = 0; i < all->count; ++i) {
            if (!strcmp(all->songs[i].media_uri, text)) {
                return(start_song(remote->audio, remote->ui, remote->library, selection, 0, i) == 0);
            }
        }
        ExternalSong *external = calloc(1, sizeof *external);
        if (!external) { return(false); }
        external->song = database_song_init();
        external->song.artist = calloc(1, 1);
        external->song.album = calloc(1, 1);
        external->song.cover_uri = calloc(1, 1);
        external->song.lyrics = calloc(1, 1);
        external->song.lyrics_format = calloc(1, 1);
        external->song.lyrics_uri = calloc(1, 1);
        external->song.id = remote->library->external ? remote->library->external->song.id - 1 : -1;
        free(external->song.media_uri);
        free(external->song.title);
        external->song.media_uri = malloc(strlen(text) + 1);
        const char *title = strrchr(text, '/');
        title = title && title[1] ? title + 1 : text;
        external->song.title = malloc(strlen(title) + 1);
        if (!external->song.media_uri || !external->song.title || !external->song.artist || !external->song.album ||
            !external->song.cover_uri || !external->song.lyrics || !external->song.lyrics_format || !external->song.lyrics_uri) {
            database_song_free(&external->song); free(external); return(false);
        }
        strcpy(external->song.media_uri, text);
        strcpy(external->song.title, title);
        if (queue_next(remote->ui, remote->library, selection, &external->song) < 0) {
            database_song_free(&external->song); free(external); return(false);
        }
        external->next = remote->library->external;
        remote->library->external = external;
        size_t index = selection->song ? selection->index + 1 : 0;
        return(start_song(remote->audio, remote->ui, remote->library, selection, remote->library->count, index) == 0);
    }
    return(true);
}



int main(int argc, char **argv)
{
    setlocale(LC_CTYPE, "");
    const char *cover_path = NULL;
    const char *database_path = DATABASE_PATH;
    const char *config_path = "config/theme.lua";
    bool config_explicit = false;
    bool database_explicit = false;
    bool preview = false;
    bool offline = false;
    const char *music_path = NULL;
    const char *lrc_path = NULL;
    int64_t lrc_song = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--preview")) { preview = true; }

        else if (!strcmp(argv[i], "--offline")) { offline = true; }

        else if (!strcmp(argv[i], "--path") && i + 1 < argc) { music_path = argv[++i]; offline = true; }

        else if (!strcmp(argv[i], "--cover") && i + 1 < argc) {
            cover_path = argv[++i];
        }

        else if (!strcmp(argv[i], "--config") && i + 1 < argc) {
            config_path = argv[++i];
            config_explicit = true;
        }

        else if (!strcmp(argv[i], "--db") && i + 1 < argc) { database_path = argv[++i]; database_explicit = true; }

        else if (!strcmp(argv[i], "--import-lrc") && i + 2 < argc && !lrc_path) {
            char *end;
            errno = 0;
            lrc_song = strtoll(argv[++i], &end, 10);
            if (errno || *end || lrc_song <= 0) { fprintf(stderr, "Invalid song ID\n"); return(1); }
            lrc_path = argv[++i];
        }

        else {
            fprintf(stderr, "Usage: %s [--db jukebox.db] [--config config/theme.lua] [--preview] [--cover album.png] [--import-lrc SONG_ID FILE]\n"
                            "  --offline              Play downloaded MP3s from ~/.sjb/songs\n"
                            "  --path DIR             Play local MP3s; subfolders are genres, root files appear in All\n", argv[0]);
            return(strcmp(argv[i], "--help") ? 1 : 0);
        }
    }

    if (offline && database_explicit) {
        fprintf(stderr, "--db selects the online catalog; use --path or --offline separately.\n");
        return(1);
    }

#ifdef SJB_SYSTEM_INSTALL
    char installed_database[4096], installed_config[4096];
    if (!database_explicit || (!config_explicit && !lrc_path)) {
        const char *user_home = getenv("HOME");
        if (!user_home || !*user_home) {
            struct passwd *account = getpwuid(getuid());
            user_home = account ? account->pw_dir : NULL;
        }
        if (!user_home || !*user_home ||
            snprintf(installed_database, sizeof installed_database, "%s/.sjb/jukebox.db", user_home) >= (int)sizeof installed_database ||
            snprintf(installed_config, sizeof installed_config, "%s/.sjb/config/theme.lua", user_home) >= (int)sizeof installed_config) {
            fprintf(stderr, "Cannot resolve ~/.sjb; pass --db and --config explicitly.\n");
            return(1);
        }
        if (!database_explicit) { database_path = installed_database; }
        if (!config_explicit) { config_path = installed_config; }
    }
#else
    (void)database_explicit;
#endif

    char config_error[512];
    if (!lrc_path && config_load(config_path, !config_explicit, config_error, sizeof config_error) < 0) {
        fprintf(stderr, "Config: %s\n", config_error);
        return(1);
    }

    Database db = {0};
    Library library = {0};
    char *default_root = offline && !music_path ? offline_default_root() : NULL;
    const char *root = music_path ? music_path : default_root;
    int opened = offline ? offline_open(&db, root) : database_open(&db, database_path);
    g_free(default_root);
    if (opened < 0 || (!lrc_path && library_load(&db, &library) < 0)) {
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
        {"Genres", TUI_BUTTON, true, false, NULL},
        {"Settings", TUI_BUTTON, true, false, NULL},
        {"Player", TUI_BUTTON, true, false, NULL},
        {"Quit", TUI_BUTTON, true, false, NULL}
    };
    
    TuiItem settings_items[] = {
        {"Show lyrics", TUI_TOGGLE, true, true, NULL},
        {"Loop playback", TUI_TOGGLE, true, false, NULL},
        {"Show cover", TUI_TOGGLE, true, true, NULL}
    };
    
    TuiMenu menus[] = {
        {.title = "ShittyJukeBox", .items = home_items, .count = 4, .wrap = true},
        {.title = "Genres", .items = library.items, .count = library.count, .wrap = true},
        {.title = "Settings", .items = settings_items, .count = 3}
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
    Lyrics timed_lyrics = {0}, backing_lyrics = {0};
    LyricsWorker *backing_worker = lyrics_worker_create(audio);
    if (!backing_worker) {
        terminal_restore();
        playlists_free(&playlists);
        audio_destroy(audio);
        library_free(&library);
        database_close(&db);
        fprintf(stderr, "Cannot start backing vocals worker.\n");
        return(1);
    }
    TuiPlayer player = {
        .volume_percent = 100, .timed_lyrics = &timed_lyrics, .backing_lyrics = &backing_lyrics, .lyric_active = SIZE_MAX,
        .backing_worker = backing_worker, .backing_frame = {.active = SIZE_MAX},
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
    OfflineDownload *download = NULL;
    char download_status[512] = "";
    char seek_status[160] = "";
    char volume_status[96] = "";
    char playback_status[256] = "Choose a song to start playback";
    AudioStatus last_audio = {.state = AUDIO_IDLE};
    uint64_t handled_end = 0;
    if (!preview) { player.playback_status = playback_status; }
    bool running = true;
    char session_status[512] = "";
    if (!preview) { restore_session(&db, audio, &ui, &library, &selection, offline, session_status, sizeof session_status); }
    RemotePlayer remote = {.audio = audio, .ui = &ui, .library = &library, .selection = &selection, .running = &running, .offline = offline};
    char mpris_error[256] = "";
    Mpris *mpris = mpris_create(remote_read, remote_control, &remote, mpris_error, sizeof mpris_error);
    if (!mpris) { ui.status = mpris_error; }
    offline_downloads_refresh();
    tui_state_draw(&ui);

    while (running) {
        bool timed_view = ui.screen == SCREEN_LYRICS && (timed_lyrics.count || backing_lyrics.count) && player.lyrics_visible && !player.paused;
        int refresh_ms = timed_view ? 10 : ui.screen == SCREEN_VISUALIZER ? 33 : 100;
        terminal_text_mode(!ui.quit_requested && ((ui.screen == SCREEN_SONGS && ui.search_editing && ui.overlay == OVERLAY_NONE) ||
                           (ui.overlay == OVERLAY_PLAYLISTS && (playlists.mode == PLAYLIST_CREATE || playlists.mode == PLAYLIST_RENAME || playlists.mode == PLAYLIST_COVER))));
        TerminalAction action = terminal_read(refresh_ms);
        if (terminal_signal() || action == TERM_EOF) { break; }
        if (action == TERM_ERROR) { error = errno; break; }
        bool quit_handled = false;
        if (ui.quit_requested || action == TERM_QUIT || action == TERM_END) {
            TuiResult change = tui_state_handle(&ui, action);
            if (change == TUI_QUIT) { break; }
            quit_handled = change == TUI_CHANGED;
            action = TERM_NONE;
        }
        if (action == TERM_SPACE && !(ui.overlay == OVERLAY_PLAYLISTS && playlists.playlist_id && playlists.mode == PLAYLIST_BROWSE)) {
            action = TERM_ACTIVATE;
        }
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
        bool handled = quit_handled;
        bool search_focus_changed = false;
        if (ui.overlay == OVERLAY_NONE &&
            (ui.screen == SCREEN_PLAYER || ui.screen == SCREEN_LYRICS || ui.screen == SCREEN_VISUALIZER) &&
            (action == TERM_REWIND || action == TERM_FAST_FORWARD ||
             (ui.screen == SCREEN_LYRICS && action == TERM_ACTIVATE))) {
            bool requested = false;
            if (action == TERM_ACTIVATE) {
                if (!timed_lyrics.count || !player.lyrics_visible) {
                    snprintf(seek_status, sizeof seek_status, "Jumping to a line requires timed LRC lyrics.");
                }
                else {
                    size_t index = ui.lyrics_browsing ? ui.lyrics_top :
                                   player.lyric_active < timed_lyrics.count ? player.lyric_active : 0;
                    int64_t target = timed_lyrics.cues[index].time_ms;
                    requested = audio_seek(audio, target < 0 ? 0 : target);
                    snprintf(seek_status, sizeof seek_status, "%s", requested ? "Seeking to lyric..." : "This track cannot seek to that lyric.");
                }
            }
            else {
                requested = audio_seek_relative(audio, action == TERM_REWIND ? -10000 : 10000);
                snprintf(seek_status, sizeof seek_status, "%s", requested ? "Seeking..." : "Seeking is unavailable for this track right now.");
            }
            if (requested) {
                ui.lyrics_browsing = false;
                ui.lyrics_top = player.lyric_active == SIZE_MAX || player.lyric_active < 2 ? 0 : player.lyric_active - 2;
            }
            ui.status = seek_status;
            handled = true;
        }
        else if (action == TERM_DOWNLOAD || action == TERM_DOWNLOAD_ALL) {
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
            else if (ui.overlay == OVERLAY_NONE && selection.song) { id = selection.song->id; }
            if (offline || preview) { snprintf(download_status, sizeof download_status, "Open the online catalog to download songs."); }
            else if (download) { snprintf(download_status, sizeof download_status, "A download is already running."); }
            else if (action == TERM_DOWNLOAD && id <= 0) { snprintf(download_status, sizeof download_status, "Select a catalog song to download."); }
            else {
                char *root = offline_default_root();
                download = offline_download_start(&db, root, action == TERM_DOWNLOAD_ALL ? 0 : id,
                                                   download_status, sizeof download_status);
                g_free(root);
            }
            ui.status = download_status;
            handled = true;
        }
        else if (action == TERM_PLAYLISTS) {
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
            else if (action == TERM_REPEAT && playlists.playlist_id && playlists.mode == PLAYLIST_BROWSE) { player.repeat = !(player.repeat || player.repeat_playlist); player.repeat_playlist = false; }
            else if ((action == TERM_PREVIOUS_TRACK || action == TERM_NEXT_TRACK) &&
                     playlists.playlist_id && playlists.mode == PLAYLIST_BROWSE && selection.song) {
                SongMenu *section = playback_section(&library, selection.genre);
                size_t index = next_song(section, &selection, action == TERM_PREVIOUS_TRACK);
                start_song(audio, &ui, &library, &selection, selection.genre, index);
            }
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
        if (ui.overlay == OVERLAY_QUEUE &&
            (action == TERM_MOVE_UP || action == TERM_MOVE_DOWN || action == TERM_REMOVE || action == TERM_QUEUE_NEXT)) {
            queue_edit(&ui, &library, &selection, action);
            handled = true;
        }
        else if (action == TERM_QUEUE_NEXT) {
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

        if (previous_screen == SCREEN_SETTINGS && !handled && settings_items[1].value != (player.repeat || player.repeat_playlist)) {
            player.repeat = settings_items[1].value;
            player.repeat_playlist = false;
        }

        else { settings_items[1].value = player.repeat || player.repeat_playlist; }
        player.lyrics_visible = settings_items[0].value;
        player.hide_cover = !settings_items[2].value;

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
                    if (ui.screen == SCREEN_SETTINGS) { ui.status = "Loop playback is saved on exit."; }
                }

                else { tui_quit_request(&ui); }
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
        if (status.generation == last_audio.generation && status.seek_serial != last_audio.seek_serial) {
            snprintf(seek_status, sizeof seek_status, "Seeked to %lld:%02lld.",
                     (long long)(status.seek_position_ms / 60000), (long long)(status.seek_position_ms / 1000 % 60));
            ui.status = seek_status;
            result = TUI_CHANGED;
        }
        else if (status.generation == last_audio.generation && last_audio.seekable && !status.seekable) {
            ui.status = "Seeking failed; this stream no longer supports seeking.";
            result = TUI_CHANGED;
        }
        if (selection.song && status.song_id == selection.song->id && status.generation == selection.generation) {
            if (status.duration_ms >= 0 && (selection.song->duration_ms != status.duration_ms ||
                selection.song->duration_source != DB_DURATION_FFMPEG)) {
                selection.song->duration_ms = status.duration_ms;
                selection.song->duration_source = DB_DURATION_FFMPEG;
                if (selection.song->id > 0 && database_duration_set(&db, selection.song->id, status.duration_ms, DB_DURATION_FFMPEG) < 0) {
                    ui.status = database_error(&db);
                }
            }
            player.duration_known = selection.song->duration_ms >= 0;
            player.duration = player.duration_known ? (unsigned long long)selection.song->duration_ms / 1000 : 0;
            size_t active = lyrics_active(&timed_lyrics, status.position_ms);
            if (active != player.lyric_active) {
                if (!ui.lyrics_browsing) { ui.lyrics_top = active == SIZE_MAX ? 0 : active > 2 ? active - 2 : 0; }
                player.lyric_active = active;
            }
            if ((timed_lyrics.count || backing_lyrics.count) && player.lyrics_visible && ui.screen == SCREEN_LYRICS &&
                status.position_ms != player.position_ms) { result = TUI_CHANGED; }
            player.position_ms = status.position_ms;
            player.duration_ms = selection.song->duration_ms;
            player.elapsed = (unsigned long long)status.position_ms / 1000;
            player.loading = status.state == AUDIO_LOADING;
            player.paused = status.pause_requested || status.state == AUDIO_FINISHED || status.state == AUDIO_FAILED;
            const char *labels[] = {"Stopped", "Loading stream...", "Playing", "Paused", "Finished", "Playback failed"};
            snprintf(playback_status, sizeof playback_status, "%s", (status.state == AUDIO_FAILED || (status.state == AUDIO_LOADING && *status.error && !status.pause_requested)) ? status.error : status.state == AUDIO_LOADING && status.pause_requested ? "Loading (paused)..." : labels[status.state]);
            player.playback_status = playback_status;
            if (status.generation != last_audio.generation || status.state != last_audio.state || status.pause_requested != last_audio.pause_requested ||
                status.position_ms / 1000 != last_audio.position_ms / 1000 || status.duration_ms != last_audio.duration_ms ||
                strcmp(status.error, last_audio.error)) {
                result = TUI_CHANGED;
            }
            if (status.state == AUDIO_FINISHED && handled_end != status.generation) {
                handled_end = status.generation;
                SongMenu *section = playback_section(&library, selection.genre);
                bool advance = player.repeat || player.repeat_playlist || queue_position(&selection) + 1 < section->count;
                if (advance) {
                    size_t index = player.repeat ? selection.index : next_song(section, &selection, false);
                    start_song(audio, &ui, &library, &selection, selection.genre, index);
                }
            }
        }
        if (status.state == AUDIO_IDLE && selection.song) {
            player.paused = true;
            player.loading = false;
            player.position_ms = 0;
            player.elapsed = 0;
            player.playback_status = "Stopped";
            if (last_audio.state != AUDIO_IDLE) { result = TUI_CHANGED; }
        }
        if (download && offline_download_poll(download, download_status, sizeof download_status)) {
            offline_download_destroy(download);
            download = NULL;
            offline_downloads_refresh();
            ui.status = download_status;
            result = TUI_CHANGED;
        }
        if (mpris_poll(mpris)) { result = TUI_CHANGED; }
        AudioStatus preload_status = audio_status(audio);
        if (selection.song && (preload_status.state == AUDIO_PLAYING || preload_status.state == AUDIO_PAUSED)) {
            SongMenu *section = playback_section(&library, selection.genre);
            bool advance = player.repeat || player.repeat_playlist || queue_position(&selection) + 1 < section->count;
            size_t index = player.repeat ? selection.index : next_song(section, &selection, false);
            audio_preload(audio, advance ? section->songs[index].media_uri : NULL);
        }
        else if (preload_status.state != AUDIO_LOADING) { audio_preload(audio, NULL); }
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

        int backing_changed = lyrics_worker_snapshot(backing_worker, &player.backing_frame);
        if (backing_changed < 0) {
            lyrics_frame_free(&player.backing_frame);
            player.backing_lyrics_error = "Cannot allocate backing vocals frame.";
        }
        if (backing_changed && ui.screen == SCREEN_LYRICS && player.lyrics_visible) { result = TUI_CHANGED; }
        if (running && result != TUI_UNCHANGED) { tui_state_draw(&ui); }
    }

    int signal_number = terminal_signal();
    char session_error[512] = "";
    if (!preview) {
        audio_pause(audio, true);
        save_session(&db, audio, &ui, &library, &selection, session_error, sizeof session_error);
    }
    
    offline_download_destroy(download);
    offline_downloads_clear();
    mpris_destroy(mpris);
    cover_destroy(selection.covers);
    playlists_free(&playlists);
    shuffle_clear(&selection);
    terminal_restore();
    lyrics_free(&timed_lyrics);
    lyrics_free(&backing_lyrics);
    lyrics_worker_destroy(backing_worker);
    lyrics_frame_free(&player.backing_frame);
    audio_destroy(audio);
    library_free(&library);
    database_close(&db);
    if (*session_error) { fprintf(stderr, "%s\n", session_error); }
    
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
        items[i] = (TuiItem){songs[i].title, TUI_BUTTON, true, false, songs[i].media_uri};
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
    else { selection->song = &songs[selection->index]; }
    queue_bind(ui, library, selection);
    return(0);
}

static void queue_edit(TuiState *ui, Library *library, PlaybackSelection *selection, TerminalAction action)
{
    size_t from = ui->queue->selected;
    SongMenu *source = playback_section(library, selection->genre);
    size_t count = source->count;
    if (from >= count) { return; }
    size_t current = queue_position(selection);
    size_t first = selection->song ? current + 1 : 0;
    if (from < first) { ui->status = "Select an upcoming song to edit."; return; }
    size_t to = from;
    if (action == TERM_MOVE_UP && from > first) { to = from - 1; }
    if (action == TERM_MOVE_DOWN && from + 1 < count) { to = from + 1; }
    if (action == TERM_QUEUE_NEXT) { to = first; }
    if (action != TERM_REMOVE && to == from) { return; }
    DbSong *songs = malloc(count * sizeof *songs);
    TuiItem *items = malloc(count * sizeof *items);
    size_t *order = ui->player->shuffle ? malloc(count * sizeof *order) : NULL;
    TuiItem *shuffled = ui->player->shuffle ? malloc(count * sizeof *shuffled) : NULL;
    if (!songs || !items || (ui->player->shuffle && (!order || !shuffled))) {
        free(songs); free(items); free(order); free(shuffled);
        ui->status = "Cannot allocate queue edit.";
        return;
    }
    for (size_t i = 0; i < count; ++i) { songs[i] = source->songs[selection->order ? selection->order[i] : i]; }
    DbSong moved = songs[from];
    if (action == TERM_REMOVE) {
        memmove(songs + from, songs + from + 1, (count - from - 1) * sizeof *songs);
        --count;
        to = from < count ? from : count ? count - 1 : SIZE_MAX;
    }
    else {
        if (to < from) { memmove(songs + to + 1, songs + to, (from - to) * sizeof *songs); }
        else { memmove(songs + from, songs + from + 1, (to - from) * sizeof *songs); }
        songs[to] = moved;
    }
    for (size_t i = 0; i < count; ++i) {
        items[i] = (TuiItem){songs[i].title, TUI_BUTTON, true, false, songs[i].media_uri};
        if (order) { order[i] = i; shuffled[i] = items[i]; }
    }
    free(library->custom.songs);
    free(library->custom.menu.items);
    library->custom = (SongMenu){.songs = songs, .count = count, .menu = {.items = items, .count = count}};
    shuffle_clear(selection);
    selection->order = order;
    selection->queue_items = shuffled;
    selection->order_count = order ? count : 0;
    selection->genre = library->count;
    selection->index = current;
    if (selection->song) { selection->song = &songs[current]; }
    queue_bind(ui, library, selection);
    ui->queue->selected = to;
    ui->status = action == TERM_REMOVE ? "Removed from queue." : "Queue reordered.";
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
    return(start_song_at(audio, ui, library, selection, genre, index, 0, false));
}

static int start_song_at(AudioPlayer *audio, TuiState *ui, Library *library,
                         PlaybackSelection *selection, size_t genre, size_t index, int64_t position_ms, bool paused)
{
    SongMenu *section = playback_section(library, genre);
    if (index >= section->count) { return(-1); }
    DbSong *song = &section->songs[index];
    if (genre == library->count) {
        for (size_t i = 0; i < library->sections[0].count; ++i) {
            if (library->sections[0].songs[i].id == song->id) { song = &library->sections[0].songs[i]; break; }
        }
    }
    if (song->id < 0) {
        for (ExternalSong *external = library->external; external; external = external->next) {
            if (external->song.id == song->id) { song = &external->song; break; }
        }
    }
    if (audio_play_at(audio, song->id, song->media_uri, position_ms, paused) < 0) {
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
    if (ui->player->backing_worker &&
        lyrics_worker_set(ui->player->backing_worker, song->backing_lyrics,
                          selection->generation, song->duration_ms) < 0) {
        ui->player->backing_lyrics_error = "Cannot load backing vocals into the animation worker.";
    }
    cover_request(selection->covers, selection->cover_override ? selection->cover_override : song->cover_uri);
    ui->player->paused = paused;
    ui->player->position_ms = position_ms;
    ui->player->elapsed = (unsigned long long)position_ms / 1000;
    ui->status = "";
    queue_bind(ui, library, selection);
    return(0);
}

static DbSong *session_song(Library *library, const SessionTrack *track, bool offline)
{
    SongMenu *all = &library->sections[0];
    for (size_t i = 0; i < all->count; ++i) {
        if (all->songs[i].id == track->id && !strcmp(all->songs[i].media_uri, track->uri)) { return(&all->songs[i]); }
    }
    if (track->id >= 0 || (offline && track->uri[0] != '/' && !g_str_has_prefix(track->uri, "file:///"))) { return(NULL); }
    for (ExternalSong *entry = library->external; entry; entry = entry->next) {
        if (!strcmp(entry->song.media_uri, track->uri)) { return(&entry->song); }
    }
    ExternalSong *entry = calloc(1, sizeof *entry);
    if (!entry) { return(NULL); }
    entry->song = database_song_init();
    entry->song.id = library->external ? library->external->song.id - 1 : -1;
    const char *title = strrchr(track->uri, '/');
    entry->song.title = g_strdup(title && title[1] ? title + 1 : track->uri);
    entry->song.media_uri = g_strdup(track->uri);
    entry->song.artist = g_strdup("");
    entry->song.album = g_strdup("");
    entry->song.cover_uri = g_strdup("");
    entry->song.lyrics = g_strdup("");
    entry->song.lyrics_format = g_strdup("plain");
    entry->song.lyrics_uri = g_strdup("");
    entry->next = library->external;
    library->external = entry;
    return(&entry->song);
}

static void restore_session(Database *db, AudioPlayer *audio, TuiState *ui, Library *library,
                            PlaybackSelection *selection, bool offline, char *message, size_t size)
{
    Session session = {0};
    int loaded = session_load(db, &session, message, size);
    if (loaded <= 0) { if (loaded < 0) { ui->status = message; } return; }
    if (session.has_preferences) {
        ui->typewriter_mode = session.typewriter_mode;
        ui->typewriter_color = (size_t)session.typewriter_color;
        ui->karaoke = session.karaoke;
        ui->playback_view = SCREEN_PLAYER + session.playback_view;
        ui->player->lyrics_visible = !session.hide_lyrics;
        ui->player->hide_cover = session.hide_cover;
        ui->menus[SCREEN_SETTINGS]->items[0].value = !session.hide_lyrics;
        ui->menus[SCREEN_SETTINGS]->items[2].value = !session.hide_cover;
    }
    ui->player->volume_percent = session.volume;
    audio_set_volume(audio, session.volume);
    ui->player->shuffle = session.shuffle;
    ui->player->repeat = session.loop == 1;
    ui->player->repeat_playlist = session.loop == 2;
    ui->menus[SCREEN_SETTINGS]->items[1].value = session.loop != 0;
    if (!session.count) { session_free(&session); return; }
    SongMenu snapshot = {0};
    snapshot.songs = calloc(session.count, sizeof *snapshot.songs);
    snapshot.menu.items = calloc(session.count, sizeof *snapshot.menu.items);
    size_t *mapping = malloc(session.count * sizeof *mapping);
    size_t *order = malloc(session.count * sizeof *order);
    TuiItem *items = calloc(session.count, sizeof *items);
    if (!snapshot.songs || !snapshot.menu.items || !mapping || !order || !items) {
        free(snapshot.songs); free(snapshot.menu.items); free(mapping); free(order); free(items);
        session_free(&session);
        ui->status = "Cannot allocate restored queue.";
        return;
    }
    for (size_t i = 0; i < session.count; ++i) {
        DbSong *song = session_song(library, &session.tracks[i], offline);
        mapping[i] = SIZE_MAX;
        if (!song) { continue; }
        mapping[i] = snapshot.count;
        snapshot.songs[snapshot.count] = *song;
        snapshot.menu.items[snapshot.count++] = (TuiItem){song->title, TUI_BUTTON, true, false, song->media_uri};
    }
    size_t at = 0;
    for (size_t i = 0; i < session.count; ++i) {
        size_t index = mapping[session.order[i]];
        if (index != SIZE_MAX) { order[at] = index; items[at++] = snapshot.menu.items[index]; }
    }
    bool missing = snapshot.count != session.count;
    size_t current = session.current >= 0 ? mapping[session.current] : SIZE_MAX;
    int64_t position = session.position_ms;
    if (current == SIZE_MAX && snapshot.count && session.current >= 0) {
        current = session.shuffle ? order[0] : 0;
        position = 0;
    }
    free(mapping);
    snapshot.menu.count = snapshot.count;
    library->custom = snapshot;
    selection->genre = library->count;
    if (session.shuffle) {
        selection->order = order;
        selection->queue_items = items;
        selection->order_count = snapshot.count;
    }
    else { free(order); free(items); }
    queue_bind(ui, library, selection);
    if (current != SIZE_MAX) {
        if (start_song_at(audio, ui, library, selection, library->count, current, position, true) < 0) {
            session_free(&session);
            return;
        }
        tui_state_switch(ui, ui->playback_view);
    }
    snprintf(message, size, "%s", missing ? "Session restored; unavailable songs were skipped. Playback is paused." :
             "Session restored. Playback is paused.");
    ui->status = message;
    session_free(&session);
}

static int save_session(Database *db, AudioPlayer *audio, TuiState *ui, Library *library,
                         PlaybackSelection *selection, char *message, size_t size)
{
    SongMenu *section = selection->song ? playback_section(library, selection->genre) : &library->custom;
    AudioStatus status = audio_status(audio);
    Session session = {.has_preferences = true,
        .typewriter_mode = ui->typewriter_mode, .typewriter_color = (int)ui->typewriter_color,
        .karaoke = ui->karaoke, .hide_lyrics = !ui->player->lyrics_visible,
        .hide_cover = ui->player->hide_cover, .playback_view = ui->playback_view - SCREEN_PLAYER,
        .count = section->count, .current = selection->song ? (int64_t)selection->index : -1,
        .position_ms = selection->song && status.generation == selection->generation && status.state != AUDIO_FINISHED ? status.position_ms : 0,
        .volume = status.volume_percent, .shuffle = ui->player->shuffle,
        .loop = ui->player->repeat ? 1 : ui->player->repeat_playlist ? 2 : 0};
    if (session.count) {
        session.tracks = calloc(session.count, sizeof *session.tracks);
        session.order = malloc(session.count * sizeof *session.order);
        if (!session.tracks || !session.order) {
            free(session.tracks); free(session.order);
            snprintf(message, size, "Cannot allocate session snapshot.");
            return(-1);
        }
        for (size_t i = 0; i < session.count; ++i) {
            session.tracks[i] = (SessionTrack){section->songs[i].id, section->songs[i].media_uri};
            session.order[i] = selection->order ? selection->order[i] : i;
        }
    }
    int result = session_save(db, &session, message, size);
    free(session.tracks);
    free(session.order);
    return(result);
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
                snapshot.menu.items[i] = (TuiItem){snapshot.songs[i].title, TUI_BUTTON, true, false, snapshot.songs[i].media_uri};
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
    lyrics_free(player->backing_lyrics);
    lyrics_frame_free(&player->backing_frame);
    player->backing_lyrics_error = NULL;
    player->lyric_active = SIZE_MAX;
    player->position_ms = 0;
    player->duration_ms = song->duration_ms;
    if (!strcmp(song->lyrics_format, "lrc")) {
        char error[160];
        if (lyrics_parse(song->lyrics, player->timed_lyrics, error, sizeof error) < 0) {
            player->lyrics = "Cannot parse timed lyrics; re-import a valid LRC file.";
        }
    }
    if (song->backing_lyrics && *song->backing_lyrics) {
        char error[160];
        if (lyrics_parse(song->backing_lyrics, player->backing_lyrics, error, sizeof error) < 0) {
            player->backing_lyrics_error = "Cannot parse backing vocals; re-import a valid LRC file.";
        }
    }
    player->duration_known = song->duration_ms != DB_TIME_UNKNOWN;
    player->duration = player->duration_known ? (unsigned long long)(song->duration_ms / 1000) : 0;
    player->elapsed = 0;
    player->paused = true;
    ui->lyrics_top = 0;
    ui->lyrics_browsing = false;
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
                library->search.items[index] = (TuiItem){library->search_labels[i], TUI_BUTTON, true, false, song->media_uri};
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
        library->items[i] = (TuiItem){library->genres[i].name, TUI_BUTTON, true, false, NULL};
        SongMenu *section = &library->sections[i];
        if (database_songs(db, library->genres[i].id, &section->songs, &section->count) < 0) { return(-1); }
        library->genres[i].song_count = section->count;
        section->menu = (TuiMenu){.title = library->genres[i].name, .count = section->count, .wrap = true};
        if (section->count) {
            section->menu.items = calloc(section->count, sizeof *section->menu.items);
            if (!section->menu.items) { goto no_memory; }
        }
        for (size_t j = 0; j < section->count; ++j) {
            section->menu.items[j] = (TuiItem){section->songs[j].title, TUI_BUTTON, true, false, section->songs[j].media_uri};
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
    while (library->external) {
        ExternalSong *next = library->external->next;
        database_song_free(&library->external->song);
        free(library->external);
        library->external = next;
    }
    free(library->sections);
    free(library->items);
    database_genres_free(library->genres, library->count);
    *library = (Library){0};
}
