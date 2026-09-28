#ifndef SHITTYJUKEBOX_CONFIG_H
#define SHITTYJUKEBOX_CONFIG_H

#include <stddef.h>

/* Load a Lua table, validating all overrides before changing terminal colors. */
int config_load(const char *path, int allow_missing, char *error, size_t size);

#endif
