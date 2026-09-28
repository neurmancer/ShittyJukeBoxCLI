#include "config.h"
#include "terminal_handler.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int read_rgb(lua_State *lua, unsigned *rgb)
{
    size_t length;
    if (lua_type(lua, -1) != LUA_TSTRING) { return(-1); }
    const char *value = lua_tolstring(lua, -1, &length);
    if (length != 7 || value[0] != '#') { return(-1); }
    unsigned result = 0;
    for (size_t i = 1; i < length; ++i) {
        unsigned nibble;
        if (value[i] >= '0' && value[i] <= '9') { nibble = value[i] - '0'; }
        else if (value[i] >= 'a' && value[i] <= 'f') { nibble = value[i] - 'a' + 10; }
        else if (value[i] >= 'A' && value[i] <= 'F') { nibble = value[i] - 'A' + 10; }
        else { return(-1); }
        result = (result << 4) | nibble;
    }
    *rgb = result;
    return(0);
}

int config_load(const char *path, int allow_missing, char *error, size_t size)
{
    FILE *file = fopen(path, "r");
    if (!file) {
        if (allow_missing && errno == ENOENT) { return(0); }
        snprintf(error, size, "%s: %s", path, strerror(errno));
        return(-1);
    }
    fclose(file);
    lua_State *lua = luaL_newstate();
    if (!lua) {
        snprintf(error, size, "%s: cannot allocate Lua state", path);
        return(-1);
    }
    luaL_openlibs(lua);
    int result = -1;
    unsigned colors[] = {0xc170ff, 0x00eb74, 0x5a96ff};
    const char *names[] = {"purple", "green", "blue"};
    if (luaL_loadfile(lua, path) != LUA_OK || lua_pcall(lua, 0, 1, 0) != LUA_OK) {
        const char *message = lua_tostring(lua, -1);
        snprintf(error, size, "%s: %s", path, message ? message : "Lua execution failed");
        goto done;
    }
    if (!lua_istable(lua, -1)) {
        snprintf(error, size, "%s: return a table containing colors = { ... }", path);
        goto done;
    }
    lua_pushliteral(lua, "colors");
    lua_rawget(lua, -2);
    if (!lua_isnil(lua, -1)) {
        if (!lua_istable(lua, -1)) {
            snprintf(error, size, "%s: colors must be a table", path);
            goto done;
        }
        for (size_t i = 0; i < THEME_COLOR_COUNT; ++i) {
            lua_pushstring(lua, names[i]);
            lua_rawget(lua, -2);
            if (!lua_isnil(lua, -1) && read_rgb(lua, &colors[i]) < 0) {
                snprintf(error, size, "%s: colors.%s must be a #RRGGBB string", path, names[i]);
                goto done;
            }
            lua_pop(lua, 1);
        }
    }
    for (size_t i = 0; i < THEME_COLOR_COUNT; ++i) { terminal_set_color((ThemeColor)i, colors[i]); }
    result = 0;
done:
    lua_close(lua);
    return(result);
}
