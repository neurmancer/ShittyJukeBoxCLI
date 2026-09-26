CC = cc
CPPFLAGS ?=
CFLAGS ?= -Wall -Wextra -std=c11
LDFLAGS ?=
LDLIBS = -lm -lsqlite3
AUDIO_PACKAGES = libavformat libavcodec libavutil libswresample libswscale sdl2
AUDIO_CFLAGS = $(shell pkg-config --cflags $(AUDIO_PACKAGES))
AUDIO_LIBS = $(shell pkg-config --libs $(AUDIO_PACKAGES))

TARGET = ShittyJukeBox
DB ?= jukebox.db
SOURCES = ShittyJukeBox.c src/audio_handler.c src/lyrics_handler.c \
          src/terminal_handler.c src/TUI.c src/database.c src/cover_handler.c
HEADERS = $(wildcard src/*.h)

.PHONY: all run clean test test-audio add-song add-songs

all: $(TARGET) jukebox-add

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(AUDIO_CFLAGS) -pthread $(SOURCES) $(LDFLAGS) $(LDLIBS) $(AUDIO_LIBS) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(TARGET) jukebox-add

# Well...I can't test the shit I built with the shit I built so .gitignored tests uses python (not user sided event)

test:
	python3 tests/check.py
	python3 tests/import_check.py

test-audio:
	python3 tests/audio_check.py

IMPORT_PACKAGES = libcurl libxml-2.0

jukebox-add: tools/add_song.c tools/genius.c tools/genius.h src/database.c src/database.h src/lyrics_handler.c src/lyrics_handler.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(shell pkg-config --cflags $(IMPORT_PACKAGES)) tools/add_song.c tools/genius.c src/database.c src/lyrics_handler.c $(LDFLAGS) $(LDLIBS) $(shell pkg-config --libs $(IMPORT_PACKAGES)) -o $@

add-song add-songs: jukebox-add
	./jukebox-add --db "$(DB)"
