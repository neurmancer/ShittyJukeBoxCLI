#ifndef SHITTYJUKEBOX_SHUFFLE_H
#define SHITTYJUKEBOX_SHUFFLE_H

#include <stddef.h>

/* Keep current first, then Fisher-Yates shuffle the remaining queue entries. */
int shuffle_order(size_t *order, size_t count, size_t current);

#endif
