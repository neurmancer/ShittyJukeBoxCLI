#!/usr/bin/env bash

set -euo pipefail

cd -- "$(dirname -- "$0")"

usage() {
    printf 'Usage: %s [--system | --help]\n' "$0"
    printf '  No option: build and run the jukebox.\n'
    printf '  --system: check/install dependencies, build, and install /usr/local/bin/ShittyJukeBox.\n'
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
modules=(sqlite3 libavformat libavcodec libavutil libswresample libswscale sdl2 lua5.4)
missing=()
check_dependencies() {
    missing=()
    local command_name module
    for command_name in cc make pkg-config install; do
        if ! command -v "$command_name" >/dev/null 2>&1; then
            missing+=("$command_name")
        fi
    done
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
            pacman:sqlite3) package=sqlite ;;
            apt-get:sqlite3) package=libsqlite3-dev ;;
            pacman:libav*|pacman:libsw*) package=ffmpeg ;;
            apt-get:libav*|apt-get:libsw*) package="$dependency-dev" ;;
            pacman:sdl2) package=sdl2-compat ;;
            apt-get:sdl2) package=libsdl2-dev ;;
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

printf 'Dependencies ready. Building ShittyJukeBox...\n'
make ShittyJukeBox
as_root install -Dm755 -- ShittyJukeBox /usr/local/bin/ShittyJukeBox
printf 'Installed /usr/local/bin/ShittyJukeBox\n'
printf 'Run from this project directory to use its library and theme, or pass --db and --config paths.\n'
