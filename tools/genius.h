#ifndef JUKEBOX_GENIUS_H
#define JUKEBOX_GENIUS_H
#include <stddef.h>
#include <signal.h>

/* Allocates plain UTF-8 lyrics; caller frees them. Errors never return partial lyrics. */
int genius_fetch(const char *url, char **lyrics, char *error, size_t error_size);
int genius_fetch_cancelable(const char *url, char **lyrics, char *error, size_t error_size,
                            const volatile sig_atomic_t *cancelled);
int genius_extract(const char *html, size_t length, char **lyrics, char *error, size_t error_size);
#endif
