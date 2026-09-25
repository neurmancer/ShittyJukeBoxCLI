CC = cc
CPPFLAGS ?=
CFLAGS ?= -Wall -Wextra -std=c11
LDFLAGS ?=
LDLIBS = -lm -lsqlite3

TARGET = ShittyJukeBox
DB ?= jukebox.db
SOURCES = ShittyJukeBox.c src/audio_handler.c src/lyrics_handler.c \
          src/terminal_handler.c src/TUI.c src/database.c
HEADERS = $(wildcard src/*.h)

.PHONY: all run clean test import-legacy

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(TARGET)

test:
	python3 tests/check.py

# Compile the private legacy header only for the explicit import operation.
import-legacy:
	@set -eu; binary=$$(mktemp /tmp/jukebox-import.XXXXXX); \
	trap 'rm -f "$$binary"' EXIT; \
	$(CC) $(CPPFLAGS) $(CFLAGS) tools/import_legacy.c src/database.c $(LDFLAGS) $(LDLIBS) -o "$$binary"; \
	"$$binary" "$(DB)"
