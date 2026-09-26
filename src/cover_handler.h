#ifndef SHITTYJUKEBOX_COVER_HANDLER_H
#define SHITTYJUKEBOX_COVER_HANDLER_H

#include <stddef.h>

typedef struct CoverLoader CoverLoader;

CoverLoader *cover_create(void);
void cover_destroy(CoverLoader *loader);
/* Replaces pending work and cancels the previous request. Empty URI clears it. */
void cover_request(CoverLoader *loader, const char *uri);
/* Transfers ownership of ready RGBA pixels to the caller; NULL while unavailable. */
unsigned char *cover_take(CoverLoader *loader, size_t *width, size_t *height);

#endif
