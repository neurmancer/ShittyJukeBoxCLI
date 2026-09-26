#ifndef SHITTYJUKEBOX_LYRICS_HANDLER_H
#define SHITTYJUKEBOX_LYRICS_HANDLER_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int64_t time_ms;
    size_t byte_offset;
} LyricsWord;

typedef struct {
    int64_t time_ms;
    char *text;
    size_t order;
    LyricsWord *words;
    size_t word_count;
} LyricsCue;

typedef struct {
    LyricsCue *cues;
    size_t count;
    size_t skipped_lines;
} Lyrics;


int lyrics_parse(const char *source, Lyrics *lyrics, char *error, size_t size);
int lyrics_read(const char *path, char **source, Lyrics *lyrics, char *error, size_t size);
void lyrics_free(Lyrics *lyrics);
/* SIZE_MAX before the first cue. Stateless lookup also supports backward seeks. */
size_t lyrics_active(const Lyrics *lyrics, int64_t position_ms);
size_t lyrics_visible_bytes(const Lyrics *lyrics, size_t cue, int64_t position_ms,
                            int64_t duration_ms);

int parsed_lyrics(char *lyrics);

#endif
