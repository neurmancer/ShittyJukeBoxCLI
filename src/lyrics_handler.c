#include "lyrics_handler.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define LRC_MAX_BYTES (1024 * 1024)
#define LRC_MAX_CUES 16384
#define LRC_MAX_TIME (INT64_MAX / 4)

static int failure(char *error, size_t size, const char *message)
{
    if (error && size) { snprintf(error, size, "%s", message); }
    return(-1);
}

static int valid_utf8(const unsigned char *p)
{
    while (*p) {
        uint32_t value = *p++;
        if (value < 128) { continue; }
        unsigned count;
        uint32_t minimum;
        if (value >= 0xc2 && value <= 0xdf) { count = 1; minimum = 0x80; value &= 31; }

        else if (value >= 0xe0 && value <= 0xef) { count = 2; minimum = 0x800; value &= 15; }

        else if (value >= 0xf0 && value <= 0xf4) { count = 3; minimum = 0x10000; value &= 7; }

        else { return(0); }

        while (count--) {
            if ((*p & 0xc0) != 0x80) { return(0); }
            value = (value << 6) | (*p++ & 63);
        }

        if (value < minimum || value > 0x10ffff || (value >= 0xd800 && value <= 0xdfff)) { return(0); }
    }
    return(1);
}

static int digit(char c) { return(c >= '0' && c <= '9'); }

static const char *timestamp(const char *p, const char *end, int64_t *time)
{
    if (p == end || *p++ != '[') { return(NULL); }

    const char *start = p;
    int64_t minutes = 0;

    while (p < end && digit(*p)) {
        if (minutes > (LRC_MAX_TIME / 60000 - 9) / 10) { return(NULL); }
        minutes = minutes * 10 + (*p++ - '0');
    }
    if (p - start < 2 || end - p < 4 || *p++ != ':' || !digit(p[0]) || !digit(p[1])) { return(NULL); }

    int seconds = (p[0] - '0') * 10 + p[1] - '0';

    if (seconds >= 60) { return(NULL); }

    p += 2;

    int fraction = 0, places = 0;

    if (p < end && *p == '.') {
        ++p;
        while (p < end && digit(*p) && places < 3) { fraction = fraction * 10 + (*p++ - '0'); ++places; }
        if (!places) { return(NULL); }
    }

    while (places++ < 3) { fraction *= 10; }

    if (p == end || *p++ != ']') { return(NULL); }

    *time = minutes * 60000 + seconds * 1000 + fraction;

    return(p);
}

void lyrics_free(Lyrics *lyrics)
{
    for (size_t i = 0; i < lyrics->count; ++i) { free(lyrics->cues[i].text); }
    free(lyrics->cues);
    *lyrics = (Lyrics){0};
}

static int compare_cues(const void *left, const void *right)
{
    const LyricsCue *a = left, *b = right;
    if (a->time_ms != b->time_ms) { return(a->time_ms < b->time_ms ? -1 : 1); }
    return((a->order > b->order) - (a->order < b->order));
}

int lyrics_parse(const char *source, Lyrics *lyrics, char *error, size_t size)
{
    if (!source || strlen(source) > LRC_MAX_BYTES || !valid_utf8((const unsigned char *)source)) {
        return(failure(error, size, "LRC must be UTF-8 and at most 1 MiB"));
    }
    Lyrics parsed = {0};
    
    size_t capacity = 0, text_bytes = 0;
    
    int64_t offset = 0;
    
    const char *p = source;
    
    if (!strncmp(p, "\xef\xbb\xbf", 3)) { p += 3; }
    
    while (*p) {
        const char *end = p + strcspn(p, "\r\n");
        const char *next = end;
    
        if (*next == '\r') { ++next; }
        if (*next == '\n') { ++next; }
    
        if (end - p >= 9 && !strncmp(p, "[offset:", 8)) {
            char *tail;
    
            errno = 0;
    
            long long value = strtoll(p + 8, &tail, 10);
    
            if (!errno && tail > p + 8 && tail == end - 1 && *tail == ']' &&
                value >= -LRC_MAX_TIME && value <= LRC_MAX_TIME) { offset = value; }

            else { ++parsed.skipped_lines; }
        }

        else {
            const char *text = p, *after;
            int64_t time;
            size_t first = parsed.count;
    
            while ((after = timestamp(text, end, &time))) {
    
                if (parsed.count == LRC_MAX_CUES) {
    
                    lyrics_free(&parsed);
    
                    return(failure(error, size, "Too many LRC timestamps (maximum 16384)"));
                }
    
                if (parsed.count == capacity) {
    
                    capacity = capacity ? capacity * 2 : 64;
                    LyricsCue *grown = realloc(parsed.cues, capacity * sizeof *grown);
    
                    if (!grown) { goto memory; }
    
                    parsed.cues = grown;
                }
    
                parsed.cues[parsed.count] = (LyricsCue){.time_ms = time, .order = parsed.count};
    
                ++parsed.count; //Yeah I lowkey start using ++var instead of var++; for no reason...
    
                text = after;
            }
    
            for (size_t i = first; i < parsed.count; ++i) {
    
                size_t length = (size_t)(end - text);
    
                if (length + 1 > 8 * LRC_MAX_BYTES - text_bytes) {
                    lyrics_free(&parsed);
    
                    return(failure(error, size, "Expanded LRC text exceeds 8 MiB"));
                }
    
                text_bytes += length + 1;
    
                parsed.cues[i].text = malloc(length + 1);
    
                if (!parsed.cues[i].text) { goto memory; }
    
                memcpy(parsed.cues[i].text, text, length);
    
                parsed.cues[i].text[length] = '\0';
            }
            if (first == parsed.count && end != p) { ++parsed.skipped_lines; }
        }
        p = next;
    }
    if (!parsed.count) {
        
        lyrics_free(&parsed);
        
        return(failure(error, size, "No valid LRC timestamps"));
    }
    
    for (size_t i = 0; i < parsed.count; ++i) { parsed.cues[i].time_ms -= offset; }
    
    qsort(parsed.cues, parsed.count, sizeof *parsed.cues, compare_cues);
    
    lyrics_free(lyrics);
    
    *lyrics = parsed;
    
    if (error && size) { *error = '\0'; }
    
    return(0);
memory:
    lyrics_free(&parsed);
    
    return(failure(error, size, "Cannot allocate lyrics"));
}

int lyrics_read(const char *path, char **source, Lyrics *lyrics, char *error, size_t size)
{
    FILE *file = fopen(path, "rb");
    
    if (!file) { return(failure(error, size, strerror(errno))); }
    
    char *raw = malloc(LRC_MAX_BYTES + 2);
    
    if (!raw) { fclose(file); return(failure(error, size, "Cannot allocate lyrics")); }
    
    size_t length = fread(raw, 1, LRC_MAX_BYTES + 1, file);
    
    int bad = ferror(file);
    
    fclose(file);
    
    if (bad || length > LRC_MAX_BYTES || memchr(raw, 0, length)) {
        free(raw);
        raw = NULL;
        
        return(failure(error, size, "Cannot read LRC: I/O error, embedded NUL, or file exceeds 1 MiB"));
    }

    raw[length] = '\0';
    if (lyrics_parse(raw, lyrics, error, size) < 0) { free(raw); return(-1); }
    
    *source = raw;
    
    return(0);
}

size_t lyrics_active(const Lyrics *lyrics, int64_t position_ms)
{
    size_t low = 0, high = lyrics->count;
    
    while (low < high) {
    
        size_t mid = low + (high - low) / 2;
    
        if (lyrics->cues[mid].time_ms <= position_ms) { low = mid + 1; }

        else { high = mid; }
    }
    
    if (!low) { return(SIZE_MAX); }
    
    --low;
    
    while (low && lyrics->cues[low - 1].time_ms == lyrics->cues[low].time_ms) { --low; }
    
    return(low);
}

size_t lyrics_visible_bytes(const Lyrics *lyrics, size_t cue, int64_t position_ms, int64_t duration_ms)
{
    if (cue >= lyrics->count || position_ms < lyrics->cues[cue].time_ms) { return(0); }
    
    const char *text = lyrics->cues[cue].text;
    
    size_t length = strlen(text), characters = 0;
    
    for (size_t i = 0; i < length; ++i) {
    
        if (((unsigned char)text[i] & 0xc0) != 0x80) { ++characters; }
    }
    
    if (!characters) { return(0); }
    
    int64_t window = (int64_t)characters * 40;
    int64_t start = lyrics->cues[cue].time_ms;
    size_t next = cue + 1;
    
    while (next < lyrics->count && lyrics->cues[next].time_ms == start) { ++next; }
    
    int64_t end = next < lyrics->count ? lyrics->cues[next].time_ms : duration_ms;
    if (end > start && end < start + window) { window = end - start; }
    
    if (position_ms >= start + window) { return(length); }
    
    size_t visible = 1 + (size_t)((position_ms - start) * (int64_t)characters / window);
    size_t bytes = 0;
    
    while (bytes < length && visible--) {
        ++bytes;
    
        while (bytes < length && ((unsigned char)text[bytes] & 0xc0) == 0x80) { ++bytes; }
    }
    
    return(bytes);
}
