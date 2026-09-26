#ifndef SHITTYJUKEBOX_TUI_H
#define SHITTYJUKEBOX_TUI_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "terminal_handler.h"
#include "lyrics_handler.h"

typedef enum { TUI_BUTTON, TUI_TOGGLE } TuiItemKind;

typedef struct {
    const char *label;
    TuiItemKind kind;
    bool enabled;
    bool value;
} TuiItem;

typedef struct {
    const char *title;
    TuiItem *items;
    size_t count;
    size_t selected; /* SIZE_MAX when no enabled item exists or shit */
    size_t top;
    bool wrap;
    bool has_active;
    size_t active;
} TuiMenu;

typedef enum { TUI_UNCHANGED, TUI_CHANGED, TUI_SELECTED, TUI_BACK, TUI_QUIT } TuiResult;

/* 
 Escape always goes back (escapes yk...).
*/
void tui_menu_init(TuiMenu *menu);

TuiResult tui_menu_handle(TuiMenu *menu, TerminalAction action);

void tui_menu_draw(TuiMenu *menu, const char *status);

typedef enum {
    PLAYER_SHUFFLE, PLAYER_PREVIOUS, PLAYER_PLAY, PLAYER_NEXT, PLAYER_REPEAT,
    PLAYER_CONTROL_COUNT    //I AM THE PLAYER
} PlayerControl;

typedef struct {
    
    const char *artist;
    const char *title;
    const char *album;
    const char *cover_status;
    const char *lyrics;
    const char *playback_status;
    
    Lyrics *timed_lyrics;
    size_t lyric_active;
    int64_t position_ms;
    int64_t duration_ms;
    int volume_percent;
    int64_t song_id;
    unsigned long long elapsed;
    unsigned long long duration;
    
    bool duration_known;
    bool show_cover;
    bool loading;
    bool lyrics_visible;
    
    size_t selected;
    
    bool paused;
    bool shuffle;
    bool repeat;
} TuiPlayer;

/* TUI_SELECTED demands songs other controls fucks with the local UI state.
 * The caller connects those changes to an audio backend when available
 */

TuiResult tui_player_handle(TuiPlayer *player, TerminalAction action);

void tui_player_draw(const TuiPlayer *player, const char *status);

typedef enum {
    SCREEN_HOME, SCREEN_GENRES, SCREEN_SETTINGS,
    SCREEN_PLAYER, SCREEN_LYRICS, SCREEN_VISUALIZER, SCREEN_SONGS, SCREEN_COUNT
} TuiScreen;

typedef enum { OVERLAY_NONE, OVERLAY_QUEUE } TuiOverlay;

typedef struct {
    TuiScreen screen;
    TuiOverlay overlay;
    TuiScreen history[16];
    size_t history_count;
    /* Caller-owned models outlive this state. screen changes doesn't affect that (effect?affect? which is the correct one ) */
    TuiMenu *menus[SCREEN_COUNT];
    TuiMenu *queue;
    TuiPlayer *player;
    size_t lyrics_top;
    const char *status;
} TuiState;

void tui_state_init(TuiState *state, TuiScreen initial);
void tui_state_switch(TuiState *state, TuiScreen screen);
void tui_state_draw(TuiState *state);

TuiResult tui_state_handle(TuiState *state, TerminalAction action);

#endif
