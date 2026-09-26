#include "terminal_handler.h"
#define _XOPEN_SOURCE 700
#include "TUI.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

static size_t edge(const TuiMenu *menu, bool last)
{
    for (size_t i = 0; i < menu->count; ++i) {
        size_t index = last ? menu->count - 1 - i : i;
        if (menu->items[index].enabled) { return(index); }
    }
    return(SIZE_MAX);
}

static void normalize(TuiMenu *menu)
{
    if (!menu->items) { menu->count = 0; }
    if (menu->selected >= menu->count || !menu->items[menu->selected].enabled) {
        menu->selected = edge(menu, false);
    }
}

void tui_menu_init(TuiMenu *menu)
{
    menu->selected = SIZE_MAX;
    menu->top = 0;
    normalize(menu);
}

TuiResult tui_menu_handle(TuiMenu *menu, TerminalAction action)
{
    normalize(menu);
    if (action == TERM_QUIT || action == TERM_END) { return(TUI_QUIT); }
    if (action == TERM_BACK) { return(TUI_BACK); }
    if (action == TERM_RESIZE) { return(TUI_CHANGED); }
    
    size_t old = menu->selected;
    
    if (old == SIZE_MAX) { return(action == TERM_LEFT ? TUI_BACK : TUI_UNCHANGED); }
    if (action == TERM_FIRST || action == TERM_LAST) {
        menu->selected = edge(menu, action == TERM_LAST);
    }

    else if (action == TERM_UP || action == TERM_DOWN) {
        size_t candidate = old;
        for (size_t i = 0; i < menu->count; ++i) {
            if (action == TERM_DOWN) {
                if (candidate == menu->count - 1) {
                    if (!menu->wrap) { break; }
                    candidate = 0;
                }

                else { ++candidate; }
            }

            else {
                if (!candidate) {
                    if (!menu->wrap) { break; }
                    candidate = menu->count - 1;
                }

                else { --candidate; }
            }
            if (menu->items[candidate].enabled) { menu->selected = candidate; break; }
        }
    }

    else {
    
        TuiItem *item = &menu->items[old];
    
        if (item->kind == TUI_TOGGLE) {
            bool before = item->value;
            if (action == TERM_LEFT) { item->value = false; }
            if (action == TERM_RIGHT) { item->value = true; }
            if (action == TERM_ACTIVATE) { item->value = !item->value; }
    
            return(before != item->value ? TUI_CHANGED : TUI_UNCHANGED);
        }
    
        if (action == TERM_LEFT) { return(TUI_BACK); }
        if (action == TERM_RIGHT || action == TERM_ACTIVATE) { return(TUI_SELECTED); }
    }
    
    return(old != menu->selected ? TUI_CHANGED : TUI_UNCHANGED);
}

static const struct {
    const char *name;
    const char *style;
} typewriter_colors[] = {
    {"Purple", SHE_LOVES_PURPLE},
    {"White", "\033[38;2;255;255;255m"},
    {"Red", "\033[38;2;255;80;80m"},
    {"Green", GREEN},
    {"Blue", "\033[38;2;90;150;255m"},
    {"Yellow", "\033[38;2;255;220;80m"},
    {"Cyan", "\033[38;2;80;230;255m"},
    {"Pink", "\033[38;2;255;110;190m"}
};

static const char *typewriter_modes[] = {"Normal", "RGB", "Bold"};

/* Clip by terminal cells, never halfway through a UTF-8 sequence. */
static void text_span_effect(size_t row, size_t column, size_t width, const char *style,
                             const char *text, size_t left, bool rainbow)
{
    printf("\033[%zu;%zuH%s", row, column, style);
    mbstate_t state = {0};
    size_t used = 0, character_index = 0;
    while (left) {
        wchar_t character;
        size_t bytes = mbrtowc(&character, text, left, &state);
        int cells;
        if (bytes == (size_t)-1 || bytes == (size_t)-2) {
            state = (mbstate_t){0};
            bytes = 1;
            cells = -1;
        }

        else { cells = wcwidth(character); }
        if (cells < 0) {
            if (used == width) { break; }
            putchar('?');
            ++used;
        }

        else {
            if (used + (size_t)cells > width) { break; }
            if (rainbow) {
                /* One color per codepoint, stable across redraws and paused playback. */
                unsigned phase = (unsigned)(character_index % 96) * 16;
                unsigned sector = phase / 256, rising = phase % 256, falling = 255 - rising;
                unsigned colors[6][3] = {
                    {255, rising, 0}, {falling, 255, 0}, {0, 255, rising},
                    {0, falling, 255}, {rising, 0, 255}, {255, 0, falling}
                };
                printf("\033[38;2;%u;%u;%um", colors[sector][0], colors[sector][1], colors[sector][2]);
            }
            fwrite(text, 1, bytes, stdout);
            used += (size_t)cells;
        }
        ++character_index;
        text += bytes;
        left -= bytes;
    }
    fputs(RESET, stdout);
}

static void text_span(size_t row, size_t column, size_t width, const char *style, const char *text, size_t left)
{
    text_span_effect(row, column, width, style, text, left, false);
}

static void text_at(size_t row, size_t column, size_t width, const char *style, const char *text)
{
    text_span(row, column, width, style, text, strlen(text));
}

static void track_label(char *buffer, size_t size, const TuiPlayer *player)
{
    snprintf(buffer, size, "%s%s%s", player->artist && *player->artist ? player->artist : "",
             player->artist && *player->artist ? " - " : "", player->title);
}

static void line(size_t row, size_t width, const char *style, const char *text)
{
    text_at(row, 1, width, style, text);
}

void tui_menu_draw(TuiMenu *menu, const char *status)
{
    normalize(menu);
    size_t rows, columns;
    terminal_size(&rows, &columns);
    size_t width = columns > 1 ? columns - 1 : 0;
    terminal_clear();
    if (rows < 7 || columns < 24) {
        line(1, width, BOLDY, "Enlarge terminal (24x7)");
        fflush(stdout);
        return;
    }
    line(1, width, BOLDY UNDERLINE, menu->title);
    size_t visible = rows - 6;
    if (menu->selected != SIZE_MAX) {
        if (menu->top > menu->selected) { menu->top = menu->selected; }
        if (menu->selected - menu->top >= visible) { menu->top = menu->selected - visible + 1; }
    }
    if (menu->top >= menu->count) { menu->top = 0; }
    for (size_t i = menu->top; i < menu->count && i - menu->top < visible; ++i) {
        TuiItem *item = &menu->items[i];
        char label[512];
        snprintf(label, sizeof label, "%s %s%s%s%s", i == menu->selected ? ">" : " ",
                 item->kind == TUI_TOGGLE ? (item->value ? "[on]  " : "[off] ") : "",
                 item->label, menu->has_active && menu->active == i ? " [playing]" : "",
                 item->enabled ? "" : " (wtf?)");
        line(3 + i - menu->top, width,
             !item->enabled ? DIM : i == menu->selected ? BOLDY INVERSE : RESET, label);
    }
    if (!menu->count) { line(3, width, DIM, "Just static..."); }
    char position[80];
    if (menu->selected == SIZE_MAX) { snprintf(position, sizeof position, "Nothing to fuck with"); }

    else { snprintf(position, sizeof position, "%zu / %zu", menu->selected + 1, menu->count); }
    line(rows - 3, width, DIM, position);
    line(rows - 2, width, FANCY, status ? status : "");
    line(rows - 1, width, DIM, "Arrows/hjkl: move/set  Enter/Space: select");
    line(rows, width, DIM, "Esc: back  q: quit  Home/End: first/last");
    fflush(stdout);
}

/* Cell widths keep the cabinet centered with accented and wide genre names.(Fuck me...where are the old jokes?) */

static size_t text_span_cells(const char *text, size_t left)
{
    mbstate_t state = {0};
    size_t width = 0;
    while (left) {
        wchar_t character;
        size_t bytes = mbrtowc(&character, text, left, &state);
        int cells;
        if (bytes == (size_t)-1 || bytes == (size_t)-2) {
            state = (mbstate_t){0};
            bytes = 1;
            cells = 1;
        }

        else { cells = wcwidth(character); }
        width += cells < 0 ? 1 : (size_t)cells;
        text += bytes;
        left -= bytes;
    }
    return(width);
}

static size_t text_cells(const char *text)
{
    return(text_span_cells(text, strlen(text)));
}

static size_t lyrics_column(size_t width, const char *text, size_t length)
{
    size_t cells = text_span_cells(text, length);
    if (cells > width) { cells = width; }
    return(1 + (width - cells) / 2);
}

static void cabinet_center(size_t row, size_t x, size_t width, const char *style, const char *text)
{
    size_t cells = text_cells(text);
    if (cells > width) { cells = width; }
    text_at(row, x + (width - cells) / 2, cells, style, text);
}

static void cabinet_bar(size_t row, size_t x, size_t width, char left, char fill, char right)
{
    char bar[65];
    memset(bar, fill, width);
    bar[0] = left;
    bar[width - 1] = right;
    bar[width] = '\0';
    text_at(row, x, width, SHE_LOVES_PURPLE, bar);
}

static void jukebox_menu_draw(TuiMenu *menu, TuiScreen screen, const char *status)
{
    size_t rows, columns;

    terminal_size(&rows, &columns);
    if (rows < 18 || columns < 44) {

        tui_menu_draw(menu, status);

        if (rows >= 7 && columns >= 24) {
            line(rows, columns - 1, DIM, screen == SCREEN_SETTINGS ?
                 "s: genres  Esc: back  q: quit" : "s: settings  Esc: back  q: quit");
            fflush(stdout);
        }

        return;
    }

    normalize(menu);

    size_t width = columns - 4;

    if (width > 64) { width = 64; }

    size_t visible = rows - 16;

    if (visible > 8) { visible = 8; }
    if (visible > menu->count) { visible = menu->count ? menu->count : 1; }

    size_t height = visible + 13;
    size_t x = (columns - width) / 2 + 1;
    size_t y = (rows - 3 - height) / 2 + 1;

    terminal_clear();
    cabinet_bar(y, x + 6, width - 12, '.', '-', '.');
    text_at(y + 1, x + 3, 3, SHE_LOVES_PURPLE, ".-'");
    text_at(y + 1, x + width - 6, 3, SHE_LOVES_PURPLE, "'-.");
    text_at(y + 2, x + 1, 1, SHE_LOVES_PURPLE, "/");
    text_at(y + 2, x + width - 2, 1, SHE_LOVES_PURPLE, "\\");

    for (size_t row = y + 3; row < y + height - 2; ++row) {
        text_at(row, x, 3, SHE_LOVES_PURPLE, "| |");
        text_at(row, x + width - 3, 3, SHE_LOVES_PURPLE, "| |");
    }

    cabinet_center(y + 2, x + 4, width - 8, GREEN BOLDY, "S H I T T Y   J U K E B O X");
    cabinet_bar(y + 4, x + 4, width - 8, '+', '-', '+');
    cabinet_center(y + 5, x + 4, width - 8, SHE_LOVES_PURPLE BOLDY,
                   screen == SCREEN_GENRES ? "Genres     [s: Settings]" :
                   screen == SCREEN_SETTINGS ? "Settings     [s: Genres]" : "Choose your next track");
    if (menu->selected != SIZE_MAX) {
        if (menu->top > menu->selected) { menu->top = menu->selected; }
        if (menu->selected - menu->top >= visible) { menu->top = menu->selected - visible + 1; }
    }

    if (menu->top >= menu->count) { menu->top = 0; }

    size_t label_width = 0;

    for (size_t i = 0; i < menu->count; ++i) {
        size_t cells = 2 + text_cells(menu->items[i].label) +
                       (menu->items[i].kind == TUI_TOGGLE ? 6 : 0) +
                       (menu->has_active && menu->active == i ? 10 : 0) +
                       (menu->items[i].enabled ? 0 : 7);
        if (cells > label_width) { label_width = cells; }
    }

    if (label_width > width - 12) { label_width = width - 12; }

    for (size_t i = menu->top; i < menu->count && i - menu->top < visible; ++i) {

        TuiItem *item = &menu->items[i];
        char label[512];

        snprintf(label, sizeof label, "%s %s%s%s%s", i == menu->selected ? ">" : " ",
                 item->kind == TUI_TOGGLE ? (item->value ? "[on]  " : "[off] ") : "",
                 item->label, menu->has_active && menu->active == i ? " [playing]" : "",
                 item->enabled ? "" : " (wtf?)");

                 text_at(y + 7 + i - menu->top, x + (width - label_width) / 2, label_width,
                !item->enabled ? DIM : i == menu->selected ? SHE_LOVES_PURPLE BOLDY INVERSE : RESET, label);
    }

    if (!menu->count) { cabinet_center(y + 7, x + 4, width - 8, DIM, "Just static..."); }

    char position[96];

    if (menu->selected == SIZE_MAX) { snprintf(position, sizeof position, "No selections yet"); }

    else { snprintf(position, sizeof position, "%zu / %zu%s%s", menu->selected + 1, menu->count,
                    menu->top ? "   ^ more" : "", menu->top + visible < menu->count ? "   v more" : ""); }
    cabinet_center(y + 7 + visible, x + 4, width - 8, DIM, position);
    cabinet_bar(y + 8 + visible, x + 4, width - 8, '+', '-', '+');
    cabinet_center(y + 9 + visible, x + 4, width - 8, GREEN, "(((OwO)))     [ 13 / 53 ]     (((UwU)))");
    cabinet_center(y + 10 + visible, x + 4, width - 8, DIM, ":::::::::    INSERT COIN    :::::::::");
    cabinet_bar(y + 11 + visible, x, width, '\'', '=', '\'');

    text_at(y + 12 + visible, x + 4, 5, SHE_LOVES_PURPLE, "[___]");
    text_at(y + 12 + visible, x + width - 9, 5, SHE_LOVES_PURPLE, "[___]");

    line(rows - 2, columns - 1, FANCY, status ? status : "");
    cabinet_center(rows - 1, 1, columns - 1, DIM, "Arrows/hjkl: move/set  Enter/Space: select");
    cabinet_center(rows, 1, columns - 1, DIM, "s: genres/settings  t:typewriter  Q:queue  Esc:back  q:quit");
    fflush(stdout);
}

TuiResult tui_player_handle(TuiPlayer *player, TerminalAction action)
{
    if (player->selected >= PLAYER_CONTROL_COUNT) { player->selected = PLAYER_PLAY; }
    if (action == TERM_QUIT || action == TERM_END) { return(TUI_QUIT); }
    if (action == TERM_BACK) { return(TUI_BACK); }
    if (action == TERM_RESIZE) { return(TUI_CHANGED); }
    if (action == TERM_LEFT || action == TERM_UP) {
        player->selected = (player->selected + PLAYER_CONTROL_COUNT - 1) % PLAYER_CONTROL_COUNT;
        return(TUI_CHANGED);
    }
    
    if (action == TERM_RIGHT || action == TERM_DOWN) {
        player->selected = (player->selected + 1) % PLAYER_CONTROL_COUNT;
        return(TUI_CHANGED);
    }
    
    if (action == TERM_FIRST || action == TERM_LAST) {
        player->selected = action == TERM_FIRST ? 0 : PLAYER_CONTROL_COUNT - 1;
        return(TUI_CHANGED);
    }
    
    if (action == TERM_ACTIVATE) {
        switch (player->selected) {
            case PLAYER_SHUFFLE: player->shuffle = !player->shuffle; break;
            case PLAYER_REPEAT: player->repeat = !player->repeat; break;
            case PLAYER_PLAY:
                if (player->song_id) { return(TUI_SELECTED); }
                player->paused = !player->paused;
                break;
            default: return(TUI_SELECTED);
        }
    
        return(TUI_CHANGED);
    }
    
    return(TUI_UNCHANGED);
}

static int cover_frame(size_t width, size_t height)
{
    if (!terminal_cover_draw(4, 4, width - 2, height - 2)) { return(0); }
    printf(SHE_LOVES_PURPLE "\033[3;3H╭");
    
    for (size_t i = 2; i < width; ++i) { fputs("─", stdout); }
    
    fputs("╮", stdout);
    
    for (size_t i = 1; i + 1 < height; ++i) {
        printf("\033[%zu;3H│\033[%zu;%zuH│", 3 + i, 3 + i, 2 + width);
    }
    
    printf("\033[%zu;3H╰", height + 2);
    
    for (size_t i = 2; i < width; ++i) { fputs("─", stdout); }
    
    fputs("╯" RESET, stdout);
    
    return(1);
}


void tui_player_draw(const TuiPlayer *player, const char *status)
{
    size_t rows, columns;

    terminal_size(&rows, &columns);
    terminal_clear();

    if (rows < 15 || columns < 40) {
        line(1, columns > 1 ? columns - 1 : 0, BOLDY, "Player needs 40 columns / 15 rows");
        fflush(stdout);
        return;
    }

    size_t x = 3;

    if (player->show_cover && columns >= 78 && rows >= 18) {
        size_t cover_width = columns / 3;

        if (cover_width > 34) { cover_width = 34; }

        size_t cover_height = cover_width / 2 + 2;

        if (cover_height > rows - 7) { cover_height = rows - 7; }

        if (cover_frame(cover_width, cover_height)) { x = cover_width + 6; }
    }

    size_t width = columns - x - 2;

    text_at(3, x, width, GREEN BOLDY, "RADIOPORT");

    char track[512];

    track_label(track, sizeof track, player);

    text_at(5, x, width, SHE_LOVES_PURPLE BOLDY, track);
    text_at(6, x, width, DIM, player->album);

    unsigned long long elapsed = player->elapsed;

    if (player->duration_known && elapsed > player->duration) { elapsed = player->duration; }

    char stamp[32];

    snprintf(stamp, sizeof stamp, "%llu:%02llu", elapsed / 60, elapsed % 60);

    text_at(8, x, width, GREEN BOLDY, stamp);

    if (player->duration_known) {
        snprintf(stamp, sizeof stamp, "%llu:%02llu", player->duration / 60, player->duration % 60);
    }

    else { snprintf(stamp, sizeof stamp, "--:--"); }

    size_t stamp_width = strlen(stamp);

    text_at(8, x + width - stamp_width, stamp_width, GREEN BOLDY, stamp);

    size_t bar = width - 2;
    size_t filled = player->duration ? (size_t)((double)elapsed / player->duration * bar) : 0;

    printf("\033[9;%zuH" GREEN "[", x);

    for (size_t i = 0; i < bar; ++i) { putchar(i < filled ? '=' : ' '); }

    fputs("]" RESET, stdout);

    const char *icons[] = {"⇄", "◀◀", player->loading ? "…" : player->paused ? "▶" : "Ⅱ", "▶▶", "↻"};

    size_t button_x = x + (width - 35) / 2;


    for (size_t i = 0; i < PLAYER_CONTROL_COUNT; ++i) {

        bool enabled = (i == PLAYER_SHUFFLE && player->shuffle) || (i == PLAYER_REPEAT && player->repeat);

        const char *style = i == player->selected ? SHE_LOVES_PURPLE BOLDY INVERSE : enabled ? GREEN BOLDY : SHE_LOVES_PURPLE;
        char button[32];

        snprintf(button, sizeof button, "[ %s ]", icons[i]);

        text_at(11, button_x + i * 7, 6, style, button);
    }

    const char *names[] = {"Shuffle", "Previous", "Play / Pause", "Next", "Repeat"};
    size_t selected = player->selected < PLAYER_CONTROL_COUNT ? player->selected : PLAYER_PLAY;

    text_at(12, x, width, DIM, names[selected]);
    char modes[96];
    snprintf(modes, sizeof modes, "Volume: %d%%   Shuffle: %s   Repeat: %s", player->volume_percent, player->shuffle ? "on" : "off", player->repeat ? "on" : "off");
    if (rows >= 17) { text_at(13, x, width, GREEN, modes); }

    const char *message = status ? status : "";
    if (player->show_cover) {
        if (*terminal_cover_error()) { message = terminal_cover_error(); }

        else if (player->cover_status && !strncmp(player->cover_status, "Cover:", 6)) { message = player->cover_status; }

        else if (columns < 78 || rows < 18) { message = "Cover hidden: enlarge terminal to at least 78 columns / 18 rows."; }
    }
    line(rows - 2, columns - 1, FANCY, message);
    line(rows - 1, columns - 1, DIM, player->playback_status ? player->playback_status : "UI preview - audio is not connected");
    line(rows, columns - 1, DIM, "1:player 2:lyrics 3:FFT Q:queue t:typewriter Up/Down:vol Esc:back q:quit");

    fflush(stdout);
}

void tui_state_init(TuiState *state, TuiScreen initial)
{
    state->screen = initial >= SCREEN_HOME && initial < SCREEN_COUNT ? initial : SCREEN_HOME;
    state->overlay = OVERLAY_NONE;
    state->history_count = 0;
    state->status = "";
    state->lyrics_top = 0;
    state->typewriter_mode = TYPEWRITER_NORMAL;
    state->typewriter_color = 0;
    state->typewriter_selected = 0;
}

void tui_state_switch(TuiState *state, TuiScreen screen)
{
    if (screen < SCREEN_HOME || screen >= SCREEN_COUNT || screen == state->screen) { return; }

    size_t capacity = sizeof state->history / sizeof state->history[0];

    if (state->history_count == capacity) {
        memmove(state->history, state->history + 1, (capacity - 1) * sizeof state->history[0]);
        --state->history_count;
    }

    state->history[state->history_count++] = state->screen;
    state->screen = screen;
    state->overlay = OVERLAY_NONE;
    state->status = "";
}


static TuiResult state_back(TuiState *state)
{
    if (state->history_count) { state->screen = state->history[--state->history_count]; }

    else { state->screen = SCREEN_HOME; }

    state->status = "";

    return(TUI_CHANGED);
}

static size_t lyrics_count(const TuiPlayer *player)
{
    if (!player || !player->lyrics_visible) { return(0); }
    if (player->timed_lyrics && player->timed_lyrics->count) { return(player->timed_lyrics->count); }
    if (!player->lyrics || !*player->lyrics) { return(0); }

    size_t count = 1;

    for (const char *p = player->lyrics; *p; ++p) {
        if (*p == '\n' && p[1]) { ++count; }
    }
    return(count);
}

static TuiResult typewriter_handle(TuiState *state, TerminalAction action)
{
    if (action == TERM_BACK) { state->overlay = OVERLAY_NONE; return(TUI_CHANGED); }
    if (state->typewriter_mode != TYPEWRITER_BOLD) { state->typewriter_selected = 0; }
    if (action == TERM_UP || action == TERM_DOWN) {
        if (state->typewriter_mode != TYPEWRITER_BOLD) { return(TUI_UNCHANGED); }
        state->typewriter_selected = !state->typewriter_selected;
        return(TUI_CHANGED);
    }
    if (action == TERM_FIRST || action == TERM_LAST) {
        state->typewriter_selected = action == TERM_LAST && state->typewriter_mode == TYPEWRITER_BOLD;
        return(TUI_CHANGED);
    }
    if (action != TERM_LEFT && action != TERM_RIGHT && action != TERM_ACTIVATE) { return(TUI_UNCHANGED); }
    size_t count = state->typewriter_selected ? sizeof typewriter_colors / sizeof typewriter_colors[0] : TYPEWRITER_MODE_COUNT;
    size_t value = state->typewriter_selected ? state->typewriter_color : (size_t)state->typewriter_mode;
    value = (value + (action == TERM_LEFT ? count - 1 : 1)) % count;
    if (state->typewriter_selected) { state->typewriter_color = value; }

    else { state->typewriter_mode = (TypewriterMode)value; }
    return(TUI_CHANGED);
}

TuiResult tui_state_handle(TuiState *state, TerminalAction action)
{
    if (action == TERM_QUIT || action == TERM_END) { return(TUI_QUIT); }
    if (action == TERM_RESIZE) { return(TUI_CHANGED); }
    if (action == TERM_QUEUE) {
        state->overlay = state->overlay == OVERLAY_QUEUE ? OVERLAY_NONE : OVERLAY_QUEUE;
        return(TUI_CHANGED);
    }
    if (action == TERM_TYPEWRITER) {
        state->overlay = state->overlay == OVERLAY_TYPEWRITER ? OVERLAY_NONE : OVERLAY_TYPEWRITER;
        return(TUI_CHANGED);
    }
    if (state->overlay == OVERLAY_TYPEWRITER) { return(typewriter_handle(state, action)); }
    /* To prevent my ADHD to leak keys to the other screens */
    if (state->overlay == OVERLAY_QUEUE) {
        if (action == TERM_BACK) { state->overlay = OVERLAY_NONE; return(TUI_CHANGED); }
        if (!state->queue) { return(TUI_UNCHANGED); }
        TuiResult result = tui_menu_handle(state->queue, action);
        if (result == TUI_BACK) { state->overlay = OVERLAY_NONE; return(TUI_CHANGED); }
        return(result);
    }

    if (action == TERM_SETTINGS && (state->screen == SCREEN_HOME || state->screen == SCREEN_GENRES || state->screen == SCREEN_SETTINGS)) {
        if (state->screen == SCREEN_SETTINGS && state->history_count &&
            state->history[state->history_count - 1] == SCREEN_GENRES) { return(state_back(state)); }
        tui_state_switch(state, state->screen == SCREEN_SETTINGS ? SCREEN_GENRES : SCREEN_SETTINGS);
        return(TUI_CHANGED);
    }

    if (action == TERM_PLAYER || action == TERM_LYRICS || action == TERM_VISUALIZER) {
        TuiScreen target = action == TERM_PLAYER ? SCREEN_PLAYER :
                           action == TERM_LYRICS ? SCREEN_LYRICS : SCREEN_VISUALIZER;
        tui_state_switch(state, target);
        return(TUI_CHANGED);
    }
    
    if (action == TERM_NEXT_VIEW) {
        TuiScreen target = state->screen == SCREEN_PLAYER ? SCREEN_LYRICS :
                           state->screen == SCREEN_LYRICS ? SCREEN_VISUALIZER : SCREEN_PLAYER;
        tui_state_switch(state, target);
        return(TUI_CHANGED);
    }
    
    if (action == TERM_BACK) { return(state_back(state)); }
    
    if (state->screen == SCREEN_LYRICS) {
        size_t count = lyrics_count(state->player);
        size_t before = state->lyrics_top;
    
        if (!count) { return(TUI_UNCHANGED); }
        if ((action == TERM_DOWN || action == TERM_RIGHT) && state->lyrics_top < count - 1) { ++state->lyrics_top; }
        if ((action == TERM_UP || action == TERM_LEFT) && state->lyrics_top) { --state->lyrics_top; }
        if (action == TERM_FIRST) { state->lyrics_top = 0; }
        if (action == TERM_LAST) { state->lyrics_top = count - 1; }
    
        return(before == state->lyrics_top ? TUI_UNCHANGED : TUI_CHANGED);
    }
    
    TuiResult result = TUI_UNCHANGED;
    
    if (state->screen == SCREEN_PLAYER && state->player) {
        result = tui_player_handle(state->player, action);
    }

    else if (state->menus[state->screen]) {
        result = tui_menu_handle(state->menus[state->screen], action);
    }
    
    if (result == TUI_BACK) { return(state_back(state)); }
    
    return(result);
}

static void pending_screen(const TuiState *state)
{
    
    size_t rows, columns;
    
    terminal_size(&rows, &columns);
    terminal_clear();
    
    size_t width = columns > 1 ? columns - 1 : 0;
    
    if (rows < 8 || columns < 32) {
        line(1, width, BOLDY, "Enlarge terminal (32x8)");
        return;
    }
    
    bool centered = state->screen == SCREEN_LYRICS;
    if (centered) { cabinet_center(2, 1, width, SHE_LOVES_PURPLE BOLDY, "Lyrics"); }

    else { line(2, width, SHE_LOVES_PURPLE BOLDY, "Audio visualizer"); }
    size_t visible = lyrics_count(state->player);
    visible = visible > state->lyrics_top ? visible - state->lyrics_top : 0;
    if (visible > rows - 7) { visible = rows - 7; }
    size_t first_row = 6 + (rows - 7 - (visible ? visible : 1)) / 2;
    
    if (state->player) {
        char track[512];
        track_label(track, sizeof track, state->player);
        if (centered) { cabinet_center(4, 1, width, GREEN, track); }

        else { line(4, width, GREEN, track); }
    }

    if (state->screen == SCREEN_LYRICS && state->player && state->player->lyrics_visible &&
        state->player->timed_lyrics && state->player->timed_lyrics->count) {
        const TuiPlayer *player = state->player;
        const Lyrics *lyrics = player->timed_lyrics;
        for (size_t row = first_row, i = state->lyrics_top; i < lyrics->count && row < rows - 1; ++row, ++i) {
            const LyricsCue *cue = &lyrics->cues[i];
            bool active = player->lyric_active < lyrics->count &&
                          cue->time_ms == lyrics->cues[player->lyric_active].time_ms;
            size_t column = lyrics_column(width, cue->text, strlen(cue->text));
            /* Anchor the highlight to the full line so typing never shifts it. */
            text_span(row, column, width - column + 1, DIM "\033[38;2;255;255;255m", cue->text, strlen(cue->text));
            if (active) {
                size_t length = lyrics_visible_bytes(lyrics, i, player->position_ms, player->duration_ms);
                size_t color = state->typewriter_color % (sizeof typewriter_colors / sizeof typewriter_colors[0]);
                char style[80];
                snprintf(style, sizeof style, "%s%s%s", RESET,
                         state->typewriter_mode == TYPEWRITER_RGB ? "" : BOLDY,
                         state->typewriter_mode == TYPEWRITER_BOLD ? typewriter_colors[color].style :
                         "\033[38;2;255;255;255m");
                text_span_effect(row, column, width - column + 1, style, cue->text, length, state->typewriter_mode == TYPEWRITER_RGB);
            }
        }
    }

    else if (state->screen == SCREEN_LYRICS && lyrics_count(state->player)) {
    
        const char *cursor = state->player->lyrics;
        size_t index = 0;
    
        while (*cursor && index < state->lyrics_top) {
            if (*cursor++ == '\n') { ++index; }
        }
    
        for (size_t row = first_row; *cursor && row < rows - 1; ++row) {
            const char *end = strchr(cursor, '\n');
            size_t length = end ? (size_t)(end - cursor) : strlen(cursor);
            size_t column = lyrics_column(width, cursor, length);
            text_span(row, column, width - column + 1, RESET, cursor, length);
            cursor += length;
    
            if (*cursor == '\n') { ++cursor; }
        }
    }

    else {
        if (centered) {
            cabinet_center(first_row, 1, width, DIM,
                           state->player && !state->player->lyrics_visible ? "Lyrics are like Waldo." : "No lyrics for u.");
        }

        else { line(6, width, DIM, "Waiting for divine intervention."); }
    }
    
    line(rows - 1, width, FANCY, state->status);
    line(rows, width, DIM, "1:player 2:lyrics 3:FFT Q:queue t:typewriter Tab:next Esc:back q:quit");
}

static bool sidebar(const char *title, size_t *rows, size_t *width, size_t *x)
{
    size_t columns;
    terminal_size(rows, &columns);
    terminal_cover_hide();
    *width = columns > 42 ? 40 : columns > 2 ? columns - 2 : 0;
    if (*rows < 8 || *width < 20) {
        char message[80];
        snprintf(message, sizeof message, "%s: enlarge terminal", title);
        terminal_clear();
        line(1, columns > 1 ? columns - 1 : 0, BOLDY, message);
        return(false);
    }
    *x = columns - *width;
    for (size_t row = 2; row < *rows; ++row) {
        printf("\033[%zu;%zuH" RESET, row, *x);
        for (size_t column = 0; column < *width; ++column) { putchar(' '); }
        printf("\033[%zu;%zuH" SHE_LOVES_PURPLE "│" RESET, row, *x);
    }
    text_at(2, *x + 2, *width - 3, SHE_LOVES_PURPLE BOLDY, title);
    return(true);
}

static void typewriter_overlay(const TuiState *state)
{
    size_t rows, width, x;
    if (!sidebar("Typewriter", &rows, &width, &x)) { return; }
    size_t color = state->typewriter_color % (sizeof typewriter_colors / sizeof typewriter_colors[0]);
    size_t mode = (size_t)state->typewriter_mode % TYPEWRITER_MODE_COUNT;
    char labels[2][64];
    snprintf(labels[0], sizeof labels[0], "Mode: < %s >", typewriter_modes[mode]);
    snprintf(labels[1], sizeof labels[1], "Color: < %s >", typewriter_colors[color].name);
    size_t options = mode == TYPEWRITER_BOLD ? 2 : 1;
    for (size_t i = 0; i < options; ++i) {
        text_at(4 + i, x + 2, width - 3,
                state->typewriter_selected == i ? INVERSE SHE_LOVES_PURPLE : RESET, labels[i]);
    }
    if (rows >= 12) {
        text_at(7, x + 2, width - 3, DIM, "Normal: bold white over dim lyrics");
        text_at(8, x + 2, width - 3, DIM, "RGB: rainbow  Bold: selected color");
        text_at(9, x + 2, width - 3, DIM, "Timed LRC lyrics; session settings");
    }
    text_at(rows - 2, x + 2, width - 3, DIM, "Up/Down: pick  Left/Right: change");
    text_at(rows - 1, x + 2, width - 3, DIM, "Enter: cycle  t / Esc: close");
}

static void queue_overlay(TuiState *state)
{
    size_t rows, width, x;
    if (!sidebar("Queue", &rows, &width, &x)) { return; }
    TuiMenu *queue = state->queue;
    if (!queue || !queue->count) {
        text_at(4, x + 2, width - 3, DIM, "Your queue is empty.");
    }

    else {
        normalize(queue);
        size_t visible = rows - 6;
        if (queue->selected != SIZE_MAX) {
            if (queue->top > queue->selected) { queue->top = queue->selected; }
            if (queue->selected - queue->top >= visible) { queue->top = queue->selected - visible + 1; }
        }
        if (queue->top >= queue->count) { queue->top = 0; }
        for (size_t i = queue->top; i < queue->count && i - queue->top < visible; ++i) {
            char label[512];
            snprintf(label, sizeof label, "%s%s", queue->items[i].label,
                     queue->has_active && queue->active == i ? " [current]" : "");
            text_at(4 + i - queue->top, x + 2, width - 3,
                    !queue->items[i].enabled ? DIM : i == queue->selected ? INVERSE SHE_LOVES_PURPLE : RESET,
                    label);
        }
    }
    text_at(rows - 2, x + 2, width - 3, DIM, "Q / Esc: close queue");
}

void tui_state_draw(TuiState *state)
{
    terminal_frame_begin();
    if (state->screen == SCREEN_PLAYER && state->player) {
        tui_player_draw(state->player, state->status);
    }

    else if (state->menus[state->screen]) {
        if (state->screen == SCREEN_HOME || state->screen == SCREEN_GENRES || state->screen == SCREEN_SETTINGS) {
            jukebox_menu_draw(state->menus[state->screen], state->screen, state->status);
        }

        else { tui_menu_draw(state->menus[state->screen], state->status); }
        if (state->player && state->player->song_id && (!state->status || !*state->status)) {
            size_t rows, columns;
            terminal_size(&rows, &columns);
            if (rows >= 7 && columns >= 24) {
                char now[768];
                snprintf(now, sizeof now, "%s: %s | %llu:%02llu | 1: player",
                         state->player->playback_status ? state->player->playback_status : "Selected",
                         state->player->title, state->player->elapsed / 60, state->player->elapsed % 60);
                line(rows - 2, columns - 1, GREEN, now);
            }
        }
    }

    else { pending_screen(state); }
    if (state->overlay == OVERLAY_QUEUE) { queue_overlay(state); }
    if (state->overlay == OVERLAY_TYPEWRITER) { typewriter_overlay(state); }
    terminal_frame_end();
}
