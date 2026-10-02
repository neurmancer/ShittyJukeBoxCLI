#!/usr/bin/env bash

set -euo pipefail

cd -- "$(dirname -- "$0")"

usage() {
    printf 'Usage: %s [--system | --help]\n' "$0"
    printf '  No option: build and run the jukebox.\n'
    printf '  --system: install /usr/local/bin/sjb and seed databases/config in $HOME/.sjb.\n'
    printf '            Install the manual in /usr/local/share/man/man1.\n'
}

if (( $# == 0 )); then
    exec make run
fi
if (( $# != 1 )); then
    usage >&2
    exit 2
fi
case "$1" in
    --help|-h) usage; exit 0 ;;
    --system) ;;
    *) usage >&2; exit 2 ;;
esac

as_root() {
    if (( EUID == 0 )); then
        "$@"
    elif command -v sudo >/dev/null 2>&1; then
        sudo -- "$@"
    else
        printf 'Root privileges are required for this step; install sudo or run --system as root.\n' >&2
        exit 1
    fi
}

# These are the dependencies of the player target, not the optional song importer.
modules=(sqlite3 libavformat libavcodec libavutil libswresample libswscale sdl2 lua5.4 gio-2.0)
missing=()
check_dependencies() {
    missing=()
    local command_name module
    for command_name in cc make pkg-config install; do
        if ! command -v "$command_name" >/dev/null 2>&1; then
            missing+=("$command_name")
        fi
    done
    if ! command -v sqlite3 >/dev/null 2>&1; then
        missing+=(sqlite3-cli)
    fi
    for module in "${modules[@]}"; do
        if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists "$module"; then
            missing+=("$module")
        fi
    done
}

check_dependencies
if (( ${#missing[@]} )); then
    printf 'Missing dependencies: %s\n' "${missing[*]}"
    ID=unknown
    ID_LIKE=
    if [[ -r /etc/os-release ]]; then
        . /etc/os-release
    fi
    case " $ID $ID_LIKE " in
        *' arch '*) manager=pacman ;;
        *' debian '*|*' ubuntu '*) manager=apt-get ;;
        *)
            printf 'Automatic dependency installation supports Arch and Debian/Ubuntu.\nInstall the missing dependencies above, then rerun --system.\n' >&2
            exit 1
            ;;
    esac
    packages=()
    for dependency in "${missing[@]}"; do
        case "$manager:$dependency" in
            pacman:cc) package=gcc ;;
            apt-get:cc) package=build-essential ;;
            *:make) package=make ;;
            pacman:pkg-config) package=pkgconf ;;
            apt-get:pkg-config) package=pkg-config ;;
            *:install) package=coreutils ;;
            pacman:sqlite3-cli) package=sqlite ;;
            apt-get:sqlite3-cli) package=sqlite3 ;;
            pacman:sqlite3) package=sqlite ;;
            apt-get:sqlite3) package=libsqlite3-dev ;;
            pacman:libav*|pacman:libsw*) package=ffmpeg ;;
            apt-get:libav*|apt-get:libsw*) package="$dependency-dev" ;;
            pacman:sdl2) package=sdl2-compat ;;
            apt-get:sdl2) package=libsdl2-dev ;;
            pacman:gio-2.0) package=glib2 ;;
            apt-get:gio-2.0) package=libglib2.0-dev ;;
            pacman:lua5.4) package=lua54 ;;
            apt-get:lua5.4) package=liblua5.4-dev ;;
        esac
        # Several FFmpeg modules share one Arch package.
        if [[ " ${packages[*]-} " != *" $package "* ]]; then
            packages+=("$package")
        fi
    done
    if [[ "$manager" == pacman ]]; then
        as_root pacman -S --needed "${packages[@]}"
    else
        as_root apt-get update
        as_root apt-get install "${packages[@]}"
    fi
    check_dependencies
    if (( ${#missing[@]} )); then
        printf 'Dependencies still unavailable after installation: %s\n' "${missing[*]}" >&2
        exit 1
    fi
fi

# When invoked through sudo, keep the data in the invoking user's home and owned
# by that user. Root gets the executable and the manual.
sjb_home=${HOME:?HOME must identify the user receiving the installation}
sjb_uid=$(id -u)
sjb_gid=$(id -g)
if (( EUID == 0 )) && [[ -n "${SUDO_USER:-}" && "$SUDO_USER" != root ]]; then
    sjb_account=$(getent passwd -- "$SUDO_USER")
    IFS=: read -r _ _ sjb_uid sjb_gid _ sjb_home _ <<< "$sjb_account"
fi
if [[ "$sjb_home" != /* ]]; then
    printf 'Cannot install user data: home directory must be absolute.\n' >&2
    exit 1
fi
sjb_data="$sjb_home/.sjb"
project_dir=$PWD
build_dir=$(mktemp -d)
trap 'rm -rf -- "$build_dir"' EXIT

printf 'Dependencies ready. Building sjb...\n'
# A separate output prevents the system defaults from leaking into local builds.
make TARGET="$build_dir/sjb" CPPFLAGS="${CPPFLAGS:-} -DSJB_SYSTEM_INSTALL" "$build_dir/sjb"
install -d -m700 -o "$sjb_uid" -g "$sjb_gid" -- "$sjb_data" "$sjb_data/config"
for database in jukebox.db playlists.db; do
    if [[ -e "$sjb_data/$database" || -L "$sjb_data/$database" ]]; then
        printf 'Keeping %s\n' "$sjb_data/$database"
        continue
    fi
    if [[ -f "$project_dir/$database" ]]; then
        # SQLite backup includes committed WAL data even while the player is open.
        (cd -- "$build_dir"; sqlite3 "$project_dir/$database" '.timeout 5000' ".backup $database")
    else
        sqlite3 "$build_dir/$database" 'VACUUM;'
    fi
    install -m600 -o "$sjb_uid" -g "$sjb_gid" -- "$build_dir/$database" "$sjb_data/$database"
done
# Copy every configuration file, preserving directory structure and user edits.
while IFS= read -r -d '' config_file; do
    relative=${config_file#config/}
    destination="$sjb_data/config/$relative"
    if [[ ! -e "$destination" && ! -L "$destination" ]]; then
        install -d -m700 -o "$sjb_uid" -g "$sjb_gid" -- "${destination%/*}"
        install -m600 -o "$sjb_uid" -g "$sjb_gid" -- "$config_file" "$destination"
    fi
done < <(find config -type f -print0)
as_root install -Dm755 -- "$build_dir/sjb" /usr/local/bin/sjb
as_root install -Dm644 -- "$project_dir/man/sjb.1" /usr/local/share/man/man1/sjb.1
printf 'Installed /usr/local/bin/sjb\n'
printf 'Installed /usr/local/share/man/man1/sjb.1; read with man 1 sjb\n'
printf 'User databases and config: %s\n' "$sjb_data"
printf 'Run sjb from any directory; --db and --config override these defaults.\n'
