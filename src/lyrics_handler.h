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

/* Zero-initialize Lyrics before use. Parsing never edits source; on failure the
 * previous timeline is retained. Line and inline <mm:ss.mmm> timestamps; UTF-8,
 * at most 1 MiB/16384 cues. Inline times are absolute, and repeated line tags
 * shift their word times relative to the first line tag.
 * Positive offset advances lyrics. Duplicate timestamps retain source order.
 */
int lyrics_parse(const char *source, Lyrics *lyrics, char *error, size_t size);
int lyrics_read(const char *path, char **source, Lyrics *lyrics, char *error, size_t size);
void lyrics_free(Lyrics *lyrics);
/* SIZE_MAX before the first cue. Stateless lookup also supports backward seeks. */
size_t lyrics_active(const Lyrics *lyrics, int64_t position_ms);
/* Explicit inline timestamps take priority: interpolate within each stamped
 * segment without speeding up or crossing its boundary early.
 * Otherwise reveal UTF-8 codepoints across the interval to the next distinct cue or known
 * track end; use roughly 100 ms per codepoint when neither is known. The first
 * character appears at the cue. Characters within each word run 20% faster,
 * then hold at spaces/tabs until the next word's estimated start.
 * Line timestamps provide approximate pacing, not word alignment. */
size_t lyrics_visible_bytes(const Lyrics *lyrics, size_t cue, int64_t position_ms,
                            int64_t duration_ms);

int parsed_lyrics(char *lyrics);

#endif
