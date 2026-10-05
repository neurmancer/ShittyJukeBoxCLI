import configparser
import os
import pathlib
import pty
import select
import shlex
import signal
import subprocess
import tempfile
import time
import wave

ROOT = pathlib.Path(__file__).resolve().parents[1]
HARNESS = r'''
#define main sjb_main
#include "ShittyJukeBox.c"
#undef main
#include <assert.h>
#include <SDL.h>
#include <sqlite3.h>

typedef struct {
    Library library;
    Lyrics lyrics;
    TuiPlayer player;
    TuiItem settings[2];
    TuiMenu settings_menu;
    TuiMenu genres;
    TuiMenu queue;
    TuiState ui;
    PlaybackSelection selection;
    AudioPlayer *audio;
} Fixture;

static void setup(Fixture *f, Database *db)
{
    *f = (Fixture){0};
    char error[256];
    assert(library_load(db, &f->library) == 0);
    f->audio = audio_create(error, sizeof error);
    assert(f->audio);
    f->player.timed_lyrics = &f->lyrics;
    f->player.volume_percent = 100;
    f->settings_menu.items = f->settings;
    f->ui.player = &f->player;
    f->ui.queue = &f->queue;
    f->ui.menus[SCREEN_SETTINGS] = &f->settings_menu;
    f->ui.menus[SCREEN_GENRES] = &f->genres;
}

static void cleanup(Fixture *f)
{
    audio_destroy(f->audio);
    shuffle_clear(&f->selection);
    lyrics_free(&f->lyrics);
    library_free(&f->library);
}

static AudioStatus paused_at(AudioPlayer *audio, int64_t position)
{
    for (int i = 0; i < 400; ++i) {
        AudioStatus status = audio_status(audio);
        assert(status.state != AUDIO_FAILED);
        if (status.state == AUDIO_PAUSED && status.position_ms == position) {
            assert(status.pause_requested);
            SDL_Delay(100);
            assert(audio_status(audio).position_ms == position);
            AudioSamples samples;
            audio_samples(audio, &samples);
            assert(samples.played_frames == 0);
            return(status);
        }
        SDL_Delay(10);
    }
    assert(!"restored playback did not settle paused at its saved position");
    return((AudioStatus){0});
}

int main(int argc, char **argv)
{
    if (argc > 1 && !strcmp(argv[1], "app")) { return(sjb_main(argc - 1, argv + 1)); }
    assert(argc == 4);
    Database db = {0};
    char error[512] = "";
    assert(database_open(&db, argv[1]) == 0);
    Session loaded = {0};
    assert(session_load(&db, &loaded, error, sizeof error) == 0);
    DbSong song = database_song_init();
    song.title = "First";
    song.media_uri = argv[2];
    int64_t first, second;
    assert(database_song_save(&db, &song, &first) == 0);
    song.title = "Second";
    song.media_uri = argv[3];
    assert(database_song_save(&db, &song, &second) == 0);
    SessionTrack tracks[] = {{first, argv[2]}, {second, argv[3]}, {first, argv[2]}, {-1, argv[3]}};
    size_t order[] = {3, 0, 2, 1};
    Session saved = {.tracks = tracks, .order = order, .count = 4, .current = 2,
                     .position_ms = 7000, .volume = 35, .loop = 2, .shuffle = true};
    assert(session_save(&db, &saved, error, sizeof error) == 0);
    assert(session_load(&db, &loaded, error, sizeof error) == 1);
    assert(loaded.count == 4 && loaded.current == 2 && loaded.position_ms == 7000);
    assert(loaded.volume == 35 && loaded.loop == 2 && loaded.shuffle);
    for (size_t i = 0; i < 4; ++i) {
        assert(loaded.order[i] == order[i] && loaded.tracks[i].id == tracks[i].id);
        assert(!strcmp(loaded.tracks[i].uri, tracks[i].uri));
    }
    session_free(&loaded);
    Fixture f;
    setup(&f, &db);
    restore_session(&db, f.audio, &f.ui, &f.library, &f.selection, false, error, sizeof error);
    assert(f.ui.screen == SCREEN_PLAYER && f.selection.index == 2);
    assert(f.selection.song->id == first && f.library.custom.count == 4);
    assert(f.library.external && f.library.custom.songs[3].id < 0);
    assert(f.player.shuffle && f.player.repeat_playlist && !f.player.repeat);
    assert(f.settings[1].value && f.player.volume_percent == 35);
    assert(f.queue.active == 2);
    for (size_t i = 0; i < 4; ++i) { assert(f.selection.order[i] == order[i]); }
    assert(save_session(&db, f.audio, &f.ui, &f.library, &f.selection, error, sizeof error) == 0);
    assert(session_load(&db, &loaded, error, sizeof error) == 1);
    assert(loaded.position_ms == 7000);
    session_free(&loaded);
    paused_at(f.audio, 7000);
    assert(save_session(&db, f.audio, &f.ui, &f.library, &f.selection, error, sizeof error) == 0);
    assert(session_load(&db, &loaded, error, sizeof error) == 1);
    assert(loaded.position_ms == 7000 && loaded.current == 2);
    session_free(&loaded);
    audio_pause(f.audio, false);
    SDL_Delay(200);
    assert(audio_status(f.audio).position_ms > 7000);
    cleanup(&f);

    saved.position_ms = INT64_MAX;
    assert(session_save(&db, &saved, error, sizeof error) == 0);
    setup(&f, &db);
    restore_session(&db, f.audio, &f.ui, &f.library, &f.selection, false, error, sizeof error);
    paused_at(f.audio, 35000);
    cleanup(&f);

    saved.position_ms = 7000;
    saved.current = -1;
    assert(session_save(&db, &saved, error, sizeof error) == 0);
    setup(&f, &db);
    restore_session(&db, f.audio, &f.ui, &f.library, &f.selection, false, error, sizeof error);
    assert(f.library.custom.count == 4 && !f.selection.song);
    assert(audio_status(f.audio).state == AUDIO_IDLE);
    cleanup(&f);

    saved.current = 2;
    assert(session_save(&db, &saved, error, sizeof error) == 0);
    assert(sqlite3_exec(db.handle, "DELETE FROM songs WHERE title='First'", NULL, NULL, NULL) == SQLITE_OK);
    setup(&f, &db);
    restore_session(&db, f.audio, &f.ui, &f.library, &f.selection, false, error, sizeof error);
    assert(f.library.custom.count == 2 && f.selection.index == 1);
    assert(f.selection.order[0] == 1 && f.selection.order[1] == 0);
    paused_at(f.audio, 0);
    assert(strstr(error, "skipped"));
    cleanup(&f);

    saved.count = 3;
    size_t plain_order[] = {0, 1, 2};
    saved.order = plain_order;
    saved.shuffle = false;
    assert(session_save(&db, &saved, error, sizeof error) == 0);
    assert(sqlite3_exec(db.handle, "DELETE FROM songs", NULL, NULL, NULL) == SQLITE_OK);
    setup(&f, &db);
    restore_session(&db, f.audio, &f.ui, &f.library, &f.selection, false, error, sizeof error);
    assert(!f.selection.song && !f.library.custom.count);
    assert(audio_status(f.audio).state == AUDIO_IDLE);
    cleanup(&f);

    char *path = g_strconcat(argv[1], ".session", NULL);
    const char *invalid[] = {
        "not a session",
        "[session]\nversion=9\ncount=0\ncurrent=-1\nposition_ms=0\nvolume=35\nloop=0\nshuffle=0\n",
        "[session]\nversion=1\ncount=1\ncurrent=0\nposition_ms=-1\nvolume=35\nloop=0\nshuffle=0\n",
        "[session]\nversion=1\ncount=2\ncurrent=0\nposition_ms=0\nvolume=35\nloop=0\nshuffle=1\n"
        "[track0]\nid=1\nuri=a\norder=0\n[track1]\nid=2\nuri=b\norder=0\n"
    };
    for (size_t i = 0; i < sizeof invalid / sizeof *invalid; ++i) {
        assert(g_file_set_contents(path, invalid[i], -1, NULL));
        assert(session_load(&db, &loaded, error, sizeof error) == -1);
        assert(!loaded.tracks);
    }
    g_free(path);

    song.title = "First";
    song.media_uri = argv[2];
    assert(database_song_save(&db, &song, &first) == 0);
    tracks[0].id = first;
    saved.count = 1;
    saved.current = 0;
    saved.loop = 1;
    assert(session_save(&db, &saved, error, sizeof error) == 0);
    database_close(&db);
    puts("PASS: session round trip, duplicate queue entries, exact shuffle order, external tracks, paused seek/resume, bounds, missing songs, invalid files");
    return(0);
}
'''


def terminal_run(binary, arguments, close_signal=None, play=False):
    master, slave = pty.openpty()
    process = subprocess.Popen(
        [str(binary), "app", *arguments], stdin=slave, stdout=slave, stderr=slave,
        env=dict(os.environ, SDL_AUDIODRIVER="dummy", TERM="xterm"),
    )
    os.close(slave)
    output = bytearray()

    def drain(seconds):
        deadline = time.monotonic() + seconds
        while time.monotonic() < deadline:
            if select.select([master], [], [], 0.05)[0]:
                try:
                    chunk = os.read(master, 65536)
                except OSError:
                    break
                if not chunk:
                    break
                output.extend(chunk)

    try:
        drain(0.8)
        assert process.poll() is None, output.decode(errors="replace")
        if play:
            os.write(master, b" ")
            drain(0.5)
            os.write(master, b" ")
            drain(0.2)
        if close_signal:
            process.send_signal(close_signal)
        else:
            os.write(master, b"q")
            drain(0.2)
            os.write(master, b"l\r")
        drain(0.6)
        code = process.wait(timeout=5)
        assert code == (128 + close_signal if close_signal else 0), (code, output.decode(errors="replace"))
    finally:
        if process.poll() is None:
            process.kill()
            process.wait()
        os.close(master)


with tempfile.TemporaryDirectory(prefix="sjb-session-") as directory:
    work = pathlib.Path(directory)
    tracks = [work / "first track.wav", work / "second track.wav"]
    for track in tracks:
        with wave.open(str(track), "wb") as audio:
            audio.setparams((2, 2, 48000, 0, "NONE", "not compressed"))
            audio.writeframes(b"\0" * (35 * 48000 * 4))
    source = work / "check.c"
    source.write_text(HARNESS)
    flags = shlex.split(subprocess.check_output([
        "pkg-config", "--cflags", "--libs", "libavformat", "libavcodec", "libavutil",
        "libswresample", "libswscale", "sdl2", "lua5.4", "gio-2.0", "sqlite3",
    ], text=True))
    binary = work / "check"
    extra = shlex.split(os.environ.get("SJB_TEST_CFLAGS", ""))
    subprocess.run([
        "cc", "-Wall", "-Wextra", "-Werror", "-std=c11", *extra, "-I", str(ROOT), str(source),
        *map(str, sorted((ROOT / "src").glob("*.c"))), "-pthread", "-lm", *flags, "-o", str(binary),
    ], check=True)
    database = work / "library.db"
    subprocess.run([str(binary), str(database), *map(str, tracks)],
                   env=dict(os.environ, SDL_AUDIODRIVER="dummy"), check=True, timeout=30)
    session = pathlib.Path(str(database) + ".session")
    assert session.stat().st_mode & 0o777 == 0o600
    original = session.read_bytes()
    terminal_run(binary, ["--db", str(database)])
    assert session.read_bytes() == original
    terminal_run(binary, ["--db", str(database)], signal.SIGTERM)
    assert session.read_bytes() == original
    terminal_run(binary, ["--db", str(database), "--preview"])
    assert session.read_bytes() == original
    other = work / "other.db"
    terminal_run(binary, ["--db", str(other)])
    assert "count=0" in pathlib.Path(str(other) + ".session").read_text()
    assert session.read_bytes() == original
    print("PASS: private session file, normal quit, handled signal, preview isolation, separate catalogs")
    terminal_run(binary, ["--db", str(database)], play=True)
    state = configparser.ConfigParser(interpolation=None)
    state.read(session)
    assert 7000 < state.getint("session", "position_ms") < 9000
    print("PASS: Play continues the restored track and saves the new position")
