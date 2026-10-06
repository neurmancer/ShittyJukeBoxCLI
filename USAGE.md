# HOW TO USE THE FUCKING JUKEBOX

> _'Sup? Apparently "just run it bro" doesn't count as documentation...so HERE WE FUCKING GO._
>
> Every CLI argument for `sjb` and `jukebox-add`, with actual examples

## First things first

From the project directory:

```sh
make                 # builds BOTH of the little shits
./sjb                # opens the player
./jukebox-add        # starts the add-song questions
```

Examples below use `42` as a song ID. Replace it with an actual ID from `./jukebox-add --list`. Quote paths with spaces. `SONG_ID`, `FILE`, and `DIR` are placeholders, not some fucking ancient spell you gotta type literally.

Both programs expect separate arguments like `--db "my music.db"`. No short flags, no `--db=file` syntax, no positional "here's an MP3, figure it out" shit.

## sjb — THE ACTUAL FUCKING PLAYER

```text
./sjb [--db FILE] [--config FILE] [--cover FILE] [--preview]
./sjb --offline [--config FILE] [--cover FILE]
./sjb --path DIR [--config FILE] [--cover FILE]
./sjb [--db FILE | --offline | --path DIR] --import-lrc SONG_ID FILE
./sjb --help
```

With no arguments, it opens the default catalog. A saved session comes back paused; hit Play to continue the questionable music choices from where you left off.

### --db FILE

Pick the catalog you wanna use.

```sh
./sjb --db "my music.db"
```

A local build defaults to `jukebox.db` in the current directory. The player installed through `./build.sh --system` defaults to `~/.sjb/jukebox.db`. A missing database gets created; an existing supported database gets migrated automatically.

**Don't combine `--db` with `--offline` or `--path`.** Those modes select their own local-library database. The player will reject that argument soup.

### --config FILE

Load your Lua color theme.

```sh
./sjb --config "config/theme.lua"
```

The local default is `config/theme.lua`; the system-installed default is `~/.sjb/config/theme.lua`. A missing default theme is fine. Explicitly asking for a missing or invalid file is an error. You pointed at the file bro, it has to exist.

### --offline

Scan and play downloaded MP3s from `~/.sjb/songs`.

```sh
./sjb --offline
```

That directory must already exist. This selects the downloaded library; it doesn't download songs for you. Use the player's `d` / `D` controls for that shit.

### --path DIR

Point the player at your own MP3 directory. This also enables offline mode, so you don't need to write `--offline` beside it like a nervous parent.

```sh
./sjb --path "$HOME/Music"
./sjb --path "$HOME/Music" --config "config/theme.lua"
```

The directory must exist. Subfolders become genres, and files at the root appear in All. If you supply both `--offline` and `--path`, the explicit directory wins.

### --cover FILE

Override the artwork used during playback.

```sh
./sjb --cover "covers/my extremely serious album.png"
```

With preview mode, use a local PNG:

```sh
./sjb --preview --cover "covers/album.png"
```

The terminal still needs image support to show the cover. Yup...this is where I mention kitty again.

### --preview

Open the player layout with sample display data. Useful for checking the UI and cover art without picking a song first.

```sh
./sjb --preview
```

Preview doesn't restore or save your playback session. It still initializes the catalog and audio backend; this ain't a standalone screenshot generator.

### --import-lrc SONG_ID FILE

Import a UTF-8 LRC file as the song's **lead lyrics**, then exit without opening the player.

```sh
./sjb --import-lrc 42 "song.lrc"
./sjb --db "my music.db" --import-lrc 42 "song.lrc"
./sjb --path "$HOME/Music" --import-lrc 42 "song.lrc"
```

The ID must be positive and exist in the selected library. Offline libraries have their own IDs; don't assume online song 42 is also offline song 42.

This replaces the existing lead lyrics immediately, with **no confirmation prompt**, and resets the old lead start/end markers. Backing vocals stay as they were. The file contents are stored in the database, and the original file stays untouched. Imports leave the saved playback session alone.

LRC input is limited to 1 MiB. The importer reports how many timed lines it found and how many untimed/invalid lines playback ignored. Files without valid cues, invalid UTF-8, and invalid inline timing are rejected.

### --help

Print usage and exit.

```sh
./sjb --help
```

The CLI equivalent of "bro what buttons do I press".

## jukebox-add — FEED THE FUCKING DATABASE

```text
./jukebox-add [--db FILE]
./jukebox-add [--db FILE] --list
./jukebox-add [--db FILE] --edit SONG_ID
./jukebox-add [--db FILE] --lyrics SONG_ID
./jukebox-add [--db FILE] --backing-lyrics SONG_ID
./jukebox-add [--db FILE] --migrate
./jukebox-add --help
```

Pick **one action per run**: `--list`, `--edit`, `--lyrics`, `--backing-lyrics`, or `--migrate`. With no action, it adds a song interactively. `--db` works with any action.

### No action — add a song

```sh
./jukebox-add
```

Answer the questions for title, artist, album, cover, playable audio URL/local path, genre, duration, lead lyrics, optional backing LRC, and optional lyrics/solo timing markers. Duration and manual markers use seconds, such as `247.5`. Leave duration empty to let playback detect it.

Lead lyrics can come from an LRC file, a plain text file, or a Genius URL. Genius supplies plain lyrics; it doesn't magically stamp them. Backing vocals use a separate LRC file.

The helper shows a preview and asks `Save to database? [y/N]:`. Enter means no. Ctrl-C or Ctrl-D cancels without saving song changes. Opening the database may still create it or migrate its schema, because the plumbing has to exist before we put shit in it.

An audio URL/path already in the catalog is rejected as a duplicate. Use `--edit` for that song instead. Restart the player after saving so it reloads the catalog.

### --db FILE

Choose which database to add to, inspect, or edit.

```sh
./jukebox-add --db "my music.db"
./jukebox-add --db "$HOME/.sjb/jukebox.db" --list
```

**This helper defaults to `jukebox.db` in the current directory**, even when you're using the system-installed player. To edit that player's catalog:

```sh
./jukebox-add --db "$HOME/.sjb/jukebox.db" --edit 42
```

`./build.sh --system` installs the player and its manual, not this helper. Run the helper from your project directory after building it with `make jukebox-add` (or `make`).

A missing database is created automatically. Supported older schemas migrate when opened, including when you only ask for a list.

### --list

Print song IDs, artists, titles, and indicators for plain/LRC lyrics and backing vocals.

```sh
./jukebox-add --list
```

**GET YOUR IDs HEREEEEE.** Editing commands take database IDs, not titles or positions in the queueueueue.

### --edit SONG_ID

Edit an existing song while keeping its ID.

```sh
./jukebox-add --edit 42
```

Enter keeps a field. `-` clears optional text fields and timing values; required fields cannot be cleared. Lyrics have their own letter menus. For genre, Enter keeps all memberships, `-` clears them all, and a name replaces them all with that genre.

Changing the audio URL/path resets the detected duration unless you enter a new one. Review the preview, then confirm the save.

### --lyrics SONG_ID

Replace or clear just the lead lyrics.

```sh
./jukebox-add --lyrics 42
```

Choose `l` for LRC, `p` for a plain file, `g` for Genius, or `n` to clear. At this command's lyrics menu, Enter **cancels**. In the full `--edit` flow, Enter **keeps** the lyrics instead.

Replacing or clearing lead lyrics resets the manual lead start/end markers. Backing vocals stay intact. The helper asks for confirmation before saving.

### --backing-lyrics SONG_ID

Import, replace, or clear the separate backing-vocals LRC.

```sh
./jukebox-add --backing-lyrics 42
```

Choose `l` and enter the file path, `n` to clear, or Enter to keep what's there. Confirm the save after the preview. Lead lyrics and their timing markers stay intact.

**YES THE TWO PEOPLE CAN FUCKING SING AT ONCE NOW.** Stamp each pass against the same audio, with timestamps measured from the song's start:

```sh
./jukebox-add --lyrics 42
# choose l, give it song.lrc, confirm

./jukebox-add --backing-lyrics 42
# choose l, give it song.backing.lrc, confirm
```

The filenames are your choice; the helper asks for each path. Both files are validated and stored in the database, so playback doesn't depend on keeping them beside the audio.

Use a blank timed line when a backing cue should end before the next vocal enters:

```lrc
[00:12.000]Ooooooh
[00:14.000]
[00:18.000]Aaaaaah
[00:20.000]
```

Without the blank cue, that line stays active until the next timestamp in its own track. Lead and backing vocals animate independently and follow the same pause/seek clock. Backing vocals appear below the lead lyrics; displaying both needs at least a 32-column, 10-row terminal. Lyric browsing and line jumps operate on the lead track.

### --migrate

Create or upgrade the selected database schema, then exit.

```sh
./jukebox-add --db "my music.db" --migrate
```

Existing songs and lyrics are retained. Normal opens already migrate automatically; this is the explicit "do the database plumbing and leave" command.

### --help

Print the helper's usage and exit.

```sh
./jukebox-add --help
```

There. Actual documentation. **I CAN RETURN TO OVERENGINEERING THE MUSIC PLAYER NOW.**
