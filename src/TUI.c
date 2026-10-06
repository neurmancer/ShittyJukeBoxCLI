#include "terminal_handler.h"
#define _XOPEN_SOURCE 700
#include "TUI.h"
#include "playlists.h"
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
    {"Blue", BLUE},
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

    cabinet_center(y + 2, x + 4, width - 8, GREEN_BOLD, "S H I T T Y   J U K E B O X");
    cabinet_bar(y + 4, x + 4, width - 8, '+', '-', '+');
    cabinet_center(y + 5, x + 4, width - 8, PURPLE_BOLD,
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
                !item->enabled ? DIM : i == menu->selected ? PURPLE_SELECTED : RESET, label);
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
    cabinet_center(rows, 1, columns - 1, DIM, "p:playlists  s:settings  t:typewriter  Q:queue  Esc:back  q:quit");
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
            case PLAYER_REPEAT:
                player->repeat = !(player->repeat || player->repeat_playlist);
                player->repeat_playlist = false;
                break;
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
    printf("%s\033[3;3H╭", SHE_LOVES_PURPLE);
    
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

    text_at(3, x, width, GREEN_BOLD, "RADIOPORT");

    char track[512];

    track_label(track, sizeof track, player);

    text_at(5, x, width, PURPLE_BOLD, track);
    text_at(6, x, width, DIM, player->album);

    unsigned long long elapsed = player->elapsed;

    if (player->duration_known && elapsed > player->duration) { elapsed = player->duration; }

    char stamp[32];

    snprintf(stamp, sizeof stamp, "%llu:%02llu", elapsed / 60, elapsed % 60);

    text_at(8, x, width, GREEN_BOLD, stamp);

    if (player->duration_known) {
        snprintf(stamp, sizeof stamp, "%llu:%02llu", player->duration / 60, player->duration % 60);
    }

    else { snprintf(stamp, sizeof stamp, "--:--"); }

    size_t stamp_width = strlen(stamp);

    text_at(8, x + width - stamp_width, stamp_width, GREEN_BOLD, stamp);

    size_t bar = width - 2;
    size_t filled = player->duration ? (size_t)((double)elapsed / player->duration * bar) : 0;

    printf("\033[9;%zuH%s[", x, GREEN);

    for (size_t i = 0; i < bar; ++i) { putchar(i < filled ? '=' : ' '); }

    fputs("]" RESET, stdout);

    const char *icons[] = {"⇄", "◀◀", player->loading ? "…" : player->paused ? "▶" : "Ⅱ", "▶▶", "↻"};

    size_t button_x = x + (width - 35) / 2;


    for (size_t i = 0; i < PLAYER_CONTROL_COUNT; ++i) {

        bool enabled = (i == PLAYER_SHUFFLE && player->shuffle) || (i == PLAYER_REPEAT && (player->repeat || player->repeat_playlist));

        const char *style = enabled ? (i == player->selected ? GREEN_ACTIVE : GREEN_BOLD) :
                            i == player->selected ? PURPLE_SELECTED : SHE_LOVES_PURPLE;
        char button[32];

        snprintf(button, sizeof button, "[ %s ]", icons[i]);

        text_at(11, button_x + i * 7, 6, style, button);
    }

    const char *names[] = {"Shuffle", "Prev", "Play / Pause", "Next", "Repeat"};
    size_t selected = player->selected < PLAYER_CONTROL_COUNT ? player->selected : PLAYER_PLAY;

    text_at(12, x, width, DIM, names[selected]);
    
    char modes[96];
    
    snprintf(modes, sizeof modes, "Volume: %d%%", player->volume_percent);
    
    if (rows >= 17) { text_at(13, x, width, GREEN, modes); }

    const char *message = status ? status : "";
    
    if (player->show_cover) {
        if (*terminal_cover_error()) { message = terminal_cover_error(); }

        else if (player->cover_status && !strncmp(player->cover_status, "Cover:", 6)) { message = player->cover_status; }

        else if (columns < 78 || rows < 18) { message = "Cover hidden: enlarge terminal to at least 78 columns / 18 rows."; }
    }
    
    line(rows - 2, columns - 1, FANCY, message);
    line(rows - 1, columns - 1, DIM, player->playback_status ? player->playback_status : "UI preview - audio is not connected");
    line(rows, columns - 1, DIM, "d:download D:all ,/.:seek s:search p:playlists Q:queue 2:lyrics 3:FFT t:typewriter Esc:back q:quit");

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
    state->karaoke = false;
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
    size_t options = state->typewriter_mode == TYPEWRITER_BOLD ? 3 : 2;
    if (state->typewriter_selected >= options) { state->typewriter_selected = options - 1; }
    if (action == TERM_UP || action == TERM_DOWN) {
        state->typewriter_selected = (state->typewriter_selected + (action == TERM_UP ? options - 1 : 1)) % options;
        return(TUI_CHANGED);
    }
    if (action == TERM_FIRST || action == TERM_LAST) {
        state->typewriter_selected = action == TERM_LAST ? options - 1 : 0;
        return(TUI_CHANGED);
    }
    if (action != TERM_LEFT && action != TERM_RIGHT && action != TERM_ACTIVATE) { return(TUI_UNCHANGED); }
    if (state->typewriter_selected == options - 1) { state->karaoke = !state->karaoke; }
    else {
        size_t count = state->typewriter_selected ? sizeof typewriter_colors / sizeof typewriter_colors[0] : TYPEWRITER_MODE_COUNT;
        size_t value = state->typewriter_selected ? state->typewriter_color : (size_t)state->typewriter_mode;
        value = (value + (action == TERM_LEFT ? count - 1 : 1)) % count;
        if (state->typewriter_selected) { state->typewriter_color = value; }
        else { state->typewriter_mode = (TypewriterMode)value; }
    }
    return(TUI_CHANGED);
}

void tui_quit_request(TuiState *state)
{
    if (!state->quit_requested) {
        state->quit_requested = true;
        state->quit_selected = 0;
    }
}

TuiResult tui_state_handle(TuiState *state, TerminalAction action)
{
    if (state->quit_requested) {
        if (action == TERM_BACK || action == TERM_QUIT) { state->quit_requested = false; return(TUI_CHANGED); }
        if (action == TERM_LEFT || action == TERM_UP || action == TERM_ARROW_UP || action == TERM_FIRST) {
            state->quit_selected = 0;
            return(TUI_CHANGED);
        }
        if (action == TERM_RIGHT || action == TERM_DOWN || action == TERM_ARROW_DOWN || action == TERM_LAST) {
            state->quit_selected = 1;
            return(TUI_CHANGED);
        }
        if (action == TERM_ACTIVATE || action == TERM_SPACE) {
            if (state->quit_selected) { return(TUI_QUIT); }
            state->quit_requested = false;
            return(TUI_CHANGED);
        }
        return(action == TERM_RESIZE ? TUI_CHANGED : TUI_UNCHANGED);
    }
    if (action == TERM_QUIT || action == TERM_END) { tui_quit_request(state); return(TUI_CHANGED); }
    if (action == TERM_RESIZE) { return(TUI_CHANGED); }
    if (action == TERM_QUEUE) {
        state->overlay = state->overlay == OVERLAY_QUEUE ? OVERLAY_NONE : OVERLAY_QUEUE;
        return(TUI_CHANGED);
    }
    if (action == TERM_TYPEWRITER) {
        if (state->screen == SCREEN_VISUALIZER) { return(TUI_UNCHANGED); }
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
    
    if (state->screen == SCREEN_VISUALIZER && action == TERM_ACTIVATE) { return(TUI_SELECTED); }
    if (state->screen == SCREEN_LYRICS) {
        size_t count = lyrics_count(state->player);
        size_t before = state->lyrics_top;

        if (!count) { return(TUI_UNCHANGED); }
        bool timed = state->player->timed_lyrics && state->player->timed_lyrics->count;
        if (action == TERM_FOLLOW_LYRICS && timed) {
            state->lyrics_browsing = false;
            size_t active = state->player->lyric_active;
            state->lyrics_top = active == SIZE_MAX || active < 2 ? 0 : active - 2;
            return(TUI_CHANGED);
        }
        bool moving = action == TERM_DOWN || action == TERM_RIGHT || action == TERM_UP ||
                      action == TERM_LEFT || action == TERM_FIRST || action == TERM_LAST;
        if (timed && moving && !state->lyrics_browsing) {
            state->lyrics_top = state->player->lyric_active < count ? state->player->lyric_active : 0;
        }
        if (timed && moving) { state->lyrics_browsing = true; }
        if (timed && action == TERM_ACTIVATE) { return(TUI_SELECTED); }
        if ((action == TERM_DOWN || action == TERM_RIGHT) && state->lyrics_top < count - 1) { ++state->lyrics_top; }
        if ((action == TERM_UP || action == TERM_LEFT) && state->lyrics_top) { --state->lyrics_top; }
        if (action == TERM_FIRST) { state->lyrics_top = 0; }
        if (action == TERM_LAST) { state->lyrics_top = count - 1; }
    
        return(before == state->lyrics_top && !(timed && moving) ? TUI_UNCHANGED : TUI_CHANGED);
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

static void timed_lyric_row(const TuiState *state, const Lyrics *lyrics, size_t i,
                            size_t row, size_t width, bool active, bool selected)
{
    const TuiPlayer *player = state->player;
    const LyricsCue *cue = &lyrics->cues[i];
    size_t column = lyrics_column(width, cue->text, strlen(cue->text));

    text_span(row, column, width - column + 1, selected ? PURPLE_INVERSE : DIM "\033[38;2;255;255;255m",
              *cue->text ? cue->text : selected ? "[instrumental]" : "", *cue->text ? strlen(cue->text) : selected ? 14 : 0);
    if (active && !selected) {
        LyricsSpan span;
        if (lyrics == player->backing_lyrics && player->backing_worker) {
            if (i >= player->backing_frame.count) { return; }
            const LyricsCueFrame *cue_frame = &player->backing_frame.cues[i];
            span = state->karaoke ? cue_frame->karaoke : (LyricsSpan){0, cue_frame->visible_bytes};
        }
        else {
            span = state->karaoke ? lyrics_karaoke_span(lyrics, i, player->position_ms, player->duration_ms) :
                   (LyricsSpan){0, lyrics_visible_bytes(lyrics, i, player->position_ms, player->duration_ms)};
        }
        size_t highlight_column = column + text_span_cells(cue->text, span.start);
        size_t color = state->typewriter_color % (sizeof typewriter_colors / sizeof typewriter_colors[0]);
        char style[80];
        snprintf(style, sizeof style, "%s%s%s", RESET,
                 state->typewriter_mode == TYPEWRITER_RGB ? "" : BOLDY,
                 state->typewriter_mode == TYPEWRITER_BOLD ? typewriter_colors[color].style :
                 "\033[38;2;255;255;255m");
        if (span.end > span.start && highlight_column <= width) {
            text_span_effect(row, highlight_column, width - highlight_column + 1, style,
                             cue->text + span.start, span.end - span.start, state->typewriter_mode == TYPEWRITER_RGB);
        }
    }
}

static bool backing_cue_active(const TuiPlayer *player, size_t cue)
{
    if (player->backing_worker) {
        return(cue < player->backing_frame.count && player->backing_frame.cues[cue].active);
    }
    return(lyrics_cue_active(player->backing_lyrics, cue, player->position_ms, player->duration_ms));
}

static size_t backing_lyric_rows(const TuiState *state, size_t active, size_t row,
                                 size_t limit, size_t width)
{
    const TuiPlayer *player = state->player;
    if (row >= limit) { return(row); }
    if (player->backing_lyrics_error) {
        line(row++, width, DIM, player->backing_lyrics_error);
        return(row);
    }
    const Lyrics *lyrics = player->backing_lyrics;
    if (!lyrics || active >= lyrics->count) { return(row); }
    for (size_t i = active; i < lyrics->count && row < limit &&
         lyrics->cues[i].time_ms == lyrics->cues[active].time_ms; ++i) {
        if (backing_cue_active(player, i)) {
            timed_lyric_row(state, lyrics, i, row++, width, true, false);
        }
    }
    return(row);
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
    const TuiPlayer *player = state->player;
    if (centered) { cabinet_center(2, 1, width, PURPLE_BOLD, "Lyrics"); }
    else { line(2, width, PURPLE_BOLD, "Audio visualizer"); }
    if (player) {
        char track[512];
        track_label(track, sizeof track, player);
        if (centered) { cabinet_center(4, 1, width, GREEN, track); }
        else { line(4, width, GREEN, track); }
    }

    size_t count = lyrics_count(player), capacity = rows - 7;
    const Lyrics *lead = player ? player->timed_lyrics : NULL;
    bool timed = centered && player && player->lyrics_visible && lead && lead->count;
    size_t anchor = SIZE_MAX, backing_active = SIZE_MAX, backing_count = 0;
    if (timed && player->lyric_active < lead->count) {
        anchor = player->lyric_active;
        while (anchor + 1 < lead->count && lead->cues[anchor + 1].time_ms == lead->cues[anchor].time_ms) { ++anchor; }
    }
    if (centered && player && player->lyrics_visible) {
        const Lyrics *backing = player->backing_lyrics;
        if (player->backing_lyrics_error) { backing_count = 1; }
        else if (backing) {
            backing_active = player->backing_worker ? player->backing_frame.active : lyrics_active(backing, player->position_ms);
            for (size_t i = backing_active; i < backing->count &&
                 backing->cues[i].time_ms == backing->cues[backing_active].time_ms; ++i) {
                if (backing_cue_active(player, i)) { ++backing_count; }
            }
        }
    }
    if (backing_count && count && capacity < 2) {
        line(6, width, DIM, "Overlapping vocals need 32x9");
        return;
    }
    size_t backing_limit = capacity - (count ? 1 : 0);
    if (backing_count > backing_limit) { backing_count = backing_limit; }
    size_t lead_rows = capacity - backing_count;
    size_t top = state->lyrics_top;
    if (timed && !state->lyrics_browsing && anchor != SIZE_MAX && anchor >= top + lead_rows) {
        top = anchor - lead_rows + 1;
    }
    if (timed && state->lyrics_browsing &&
        (anchor == SIZE_MAX || anchor < top || anchor >= top + lead_rows)) {
        backing_count = 0;
        lead_rows = capacity;
    }
    size_t visible = count > top ? count - top : 0;
    if (visible > lead_rows) { visible = lead_rows; }
    size_t total = visible + backing_count;
    size_t row = 6 + (capacity - (total ? total : 1)) / 2;

    if (timed) {
        if (backing_count && anchor == SIZE_MAX) {
            row = backing_lyric_rows(state, backing_active, row, row + backing_count, width);
        }
        for (size_t i = top; i < lead->count && row < rows - 1; ++i) {
            bool active = lyrics_cue_active(lead, i, player->position_ms, player->duration_ms);
            timed_lyric_row(state, lead, i, row++, width, active,
                            state->lyrics_browsing && i == state->lyrics_top);
            if (backing_count && i == anchor) {
                row = backing_lyric_rows(state, backing_active, row, row + backing_count, width);
            }
        }
    }
    else if (centered && count) {
        const char *cursor = player->lyrics;
        size_t index = 0;
        while (*cursor && index < top) {
            if (*cursor++ == '\n') { ++index; }
        }
        bool inserted = false;
        while (*cursor && row < rows - 1) {
            const char *end = strchr(cursor, '\n');
            size_t length = end ? (size_t)(end - cursor) : strlen(cursor);
            size_t column = lyrics_column(width, cursor, length);
            text_span(row++, column, width - column + 1, RESET, cursor, length);
            cursor += length;
            if (*cursor == '\n') { ++cursor; }
            if (backing_count && !inserted) {
                row = backing_lyric_rows(state, backing_active, row, row + backing_count, width);
                inserted = true;
            }
        }
    }
    else if (backing_count) {
        backing_lyric_rows(state, backing_active, row, row + backing_count, width);
    }
    else if (centered) {
        cabinet_center(row, 1, width, DIM,
                       player && !player->lyrics_visible ? "Lyrics are like Waldo." : "No lyrics for u.");
    }
    else { line(6, width, DIM, "Waiting for divine intervention."); }

    line(rows - 1, width, FANCY, state->status);
    line(rows, width, DIM, "Up/Down:line Enter:jump f:follow ,/.:seek 1:player 3:FFT q:quit");
}

static void visualizer_draw(const TuiState *state)
{
    size_t rows, columns;
    terminal_size(&rows, &columns);
    terminal_clear();
    size_t width = columns > 1 ? columns - 1 : 0;
    if (rows < 12 || columns < 32) {
        line(1, width, BOLDY, "Visualizer needs 32 columns / 12 rows");
        return;
    }
    cabinet_center(2, 1, width, PURPLE_BOLD, "AUDIO BUT VISIBLE");
    if (state->player) {
        char track[512];
        track_label(track, sizeof track, state->player);
        cabinet_center(4, 1, width, GREEN, track);
    }

    size_t chart_width = columns - 12;
    
    if (chart_width > 96) { chart_width = 96; }
    
    size_t bars = chart_width / 2;
    
    if (bars > SPECTRUM_BANDS) { bars = SPECTRUM_BANDS; }
    
    size_t bar_width = chart_width / bars - 1;
    
    chart_width = bars * (bar_width + 1) - 1;
    
    size_t x = (columns - chart_width) / 2 + 1;
    size_t height = rows - 11;
    
    if (height > 24) { height = 24; }
    
    size_t y = 6 + (rows - 11 - height) / 2;
    
    const Spectrum *spectrum = state->spectrum;
    
    bool paused = spectrum && spectrum->ready && state->player && state->player->song_id && state->player->paused && !state->player->loading;
    
    const char *blocks[] = {" ", "▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"}; //UTF chars
    
    for (size_t bar = 0; bar < bars; ++bar) {
        float level = 0, peak = 0;
        
        if (spectrum && spectrum->ready) {
        
            for (size_t band = bar * SPECTRUM_BANDS / bars; band < (bar + 1) * SPECTRUM_BANDS / bars; ++band) {
        
                if (spectrum->levels[band] > level) { level = spectrum->levels[band]; }
                if (spectrum->peaks[band] > peak) { peak = spectrum->peaks[band]; }
            }
        }
        
        if (level > 1) { level = 1; }
        if (peak > 1) { peak = 1; }
        
        size_t units = (size_t)(level * height * 8);
        size_t peak_row = peak > 0 ? (size_t)(peak * (height - 1)) : 0;
        
        if (paused) {
            units = height >= 3 ? (height / 2) * 8 : height * 4;
            peak_row = units / 8 + 1; /* One empty row above the green body. */
        }
        
        for (size_t row = 0; row < height; ++row) {
        
            size_t fill = units > row * 8 ? units - row * 8 : 0;
        
            if (fill > 8) { fill = 8; }
        
            bool cap = row == peak_row && (paused ? height >= 3 : peak > 0);
        
            if (!fill && !cap) { continue; }
        
            const char *glyph = fill ? blocks[fill] : "─";
            const char *style = paused ? (fill ? GREEN : PURPLE_BOLD) :
                                row * 3 >= height * 2 ? PURPLE_BOLD : GREEN;
        
                                printf("\033[%zu;%zuH%s", y + height - 1 - row, x + bar * (bar_width + 1), style);
            for (size_t column = 0; column < bar_width; ++column) { fputs(glyph, stdout); }
        
            fputs(RESET, stdout);
        }
    }
    
    text_at(y + height + 1, x, 4, DIM, "20Hz");
    
    if (chart_width >= 30) { text_at(y + height + 1, x + chart_width * 57 / 100, 3, DIM, "1k"); }
    
    text_at(y + height + 1, x + chart_width - 5, 5, DIM, "20kHz");
    
    if (!spectrum || !spectrum->ready) {
        cabinet_center(y + height / 2, x, chart_width, DIM,
                       state->player && state->player->song_id ? "Waiting for audio" : "Play a song to see its spectrum");
    }
    
    char status[512];
    const char *playback = state->player && state->player->playback_status ? state->player->playback_status : "";
    
    snprintf(status, sizeof status, "%s%s%s", playback,
             *playback && state->status && *state->status ? " | " : "",
             state->status ? state->status : "");
    
    line(rows - 2, width, FANCY, status);
    line(rows - 1, width, DIM, "Space:play/pause Up/Down:volume ,/.:seek d:download D:all Q:queue");
    line(rows, width, DIM, "1:player 2:lyrics 3:FFT Tab:next Esc:back q:quit");
}

/*
    I know nobody will see this in the line 904 of one of fuck knows how many files but BRO I JUST WANTED TO BLAST 
    EVANESCENCE WHAT AM I DOING


*/


static bool sidebar(const char *title, size_t *rows, size_t *width, size_t *x, bool left)
{
    size_t columns;
    
    terminal_size(rows, &columns);
    
    *width = columns > 42 ? 40 : columns > 2 ? columns - 2 : 0;
    
    if (*rows < 8 || *width < 20) {
        char message[80];
        snprintf(message, sizeof message, "%s: enlarge terminal", title);
        terminal_clear();
        line(1, columns > 1 ? columns - 1 : 0, BOLDY, message);
    
        return(false);
    }
    
    *x = left ? 1 : columns - *width;
    
    for (size_t row = 2; row < *rows; ++row) {
        printf("\033[%zu;%zuH" RESET, row, *x);
    
        for (size_t column = 0; column < *width; ++column) { putchar(' '); }
    
        printf("\033[%zu;%zuH%s│" RESET, row, left ? *x + *width - 1 : *x, SHE_LOVES_PURPLE);
    }
    
    text_at(2, *x + 2, *width - 3, PURPLE_BOLD, title);
    
    return(true);
}

static void typewriter_overlay(const TuiState *state)
{
    size_t rows, width, x;
    
    if (!sidebar("Typewriter", &rows, &width, &x, false)) { return; }
    
    size_t color = state->typewriter_color % (sizeof typewriter_colors / sizeof typewriter_colors[0]);
    size_t mode = (size_t)state->typewriter_mode % TYPEWRITER_MODE_COUNT;
    
    char labels[3][64];
    
    snprintf(labels[0], sizeof labels[0], "Mode: < %s >", typewriter_modes[mode]);
    snprintf(labels[1], sizeof labels[1], "Color: < %s >", typewriter_colors[color].name);
    
    size_t options = mode == TYPEWRITER_BOLD ? 3 : 2;
    snprintf(labels[options - 1], sizeof labels[0], "Karaoke: < %s >", state->karaoke ? "On" : "Off");
    
    for (size_t i = 0; i < options; ++i) {
        text_at(4 + i, x + 2, width - 3,
                state->typewriter_selected == i ? PURPLE_INVERSE : RESET, labels[i]);
    }
    
    if (rows >= 12) {
    
        text_at(7, x + 2, width - 3, DIM, "Normal: bold white over dim lyrics");
        text_at(8, x + 2, width - 3, DIM, "RGB: rainbow  Bold: selected color");
        text_at(9, x + 2, width - 3, DIM, "Karaoke: highlight current word");
    }
    
    text_at(rows - 2, x + 2, width - 3, DIM, "Up/Down: pick  Left/Right: change");
    text_at(rows - 1, x + 2, width - 3, DIM, "Enter: cycle  t / Esc: close");
}

static void queue_overlay(TuiState *state)
{
    size_t rows, width, x;
    
    if (!sidebar(state->queue && state->queue->title ? state->queue->title : "Queue", &rows, &width, &x, false)) { return; }
    
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
                    !queue->items[i].enabled ? DIM : i == queue->selected ? PURPLE_INVERSE : RESET,
                    label);
        }
    }

    text_at(rows - 2, x + 2, width - 3, DIM, "K/J:move x:remove n:next");
    text_at(rows - 1, x + 2, width - 3, DIM, "Enter:play d:download Q/Esc:close");
}

static void playlist_art(const Playlists *panel, size_t row, size_t column, size_t width, size_t height)
{
    if (!panel->cover_pixels || !panel->cover_width || !panel->cover_height) {
        for (size_t y = 0; y < height; ++y) {
            printf("\033[%zu;%zuH%s", row + y, column, SHE_LOVES_PURPLE);
            for (size_t x = 0; x < width; ++x) { putchar(y == 0 || y + 1 == height ? '-' : x == 0 || x + 1 == width ? '|' : ' '); }
        }
    
        text_at(row + height / 2, column + 1, width - 2, DIM, *panel->cover_uri ? "NO PREVIEW" : "NO COVER");
        fputs(RESET, stdout);
    
        return;
    }
    
    size_t square = panel->cover_width < panel->cover_height ? panel->cover_width : panel->cover_height;
    size_t left = (panel->cover_width - square) / 2, top = (panel->cover_height - square) / 2;
    
    for (size_t y = 0; y < height; ++y) {
    
        printf("\033[%zu;%zuH", row + y, column);
        for (size_t x = 0; x < width; ++x) {
    
            unsigned rgb[2][3];
    
            for (size_t half = 0; half < 2; ++half) {
                size_t sx = left + x * square / width, sy = top + (2 * y + half) * square / (2 * height);
                const unsigned char *pixel = panel->cover_pixels + (sy * panel->cover_width + sx) * 4;
                for (size_t channel = 0; channel < 3; ++channel) { rgb[half][channel] = pixel[channel] * pixel[3] / 255; }
            }
    
            printf("\033[38;2;%u;%u;%um\033[48;2;%u;%u;%um▀", rgb[0][0], rgb[0][1], rgb[0][2], rgb[1][0], rgb[1][1], rgb[1][2]);
        }
    
        fputs(RESET, stdout);
    }
}

static void playlist_title(const char *title, size_t row, size_t column, size_t width, size_t lines)
{
    for (size_t line_number = 0; *title && line_number < lines; ++line_number) {
    
        size_t bytes = 0, cells = 0, space = 0;
    
        while (title[bytes]) {
    
            wchar_t character;
            mbstate_t state = {0};
    
            size_t length = mbrtowc(&character, title + bytes, strlen(title + bytes), &state);
    
            if (length == (size_t)-1 || length == (size_t)-2) { length = 1; character = '?'; }
    
            int size = wcwidth(character);
    
            if (size < 0) { size = 1; }
            if (cells + (size_t)size > width) { break; }
            if (character == ' ') { space = bytes; }
    
            bytes += length;
    
            cells += (size_t)size;
        }
        if (title[bytes] && space) { bytes = space; }
        if (!bytes) { break; }
    
        text_span(row + line_number, column, width, PURPLE_BOLD, title, bytes);
    
        title += bytes;
    
        while (*title == ' ') { ++title; }
    }
}

static void playlist_detail(TuiState *state)
{
    Playlists *panel = state->playlists;
    
    size_t rows, columns;
    
    terminal_size(&rows, &columns);
    terminal_cover_hide();
    
    if (rows < 20 || columns < 44) {
        terminal_clear();
        line(1, columns > 1 ? columns - 1 : 0, BOLDY, "Playlist: enlarge terminal (44x20)");
    
        return;
    }
    
    size_t width = columns - (columns >= 64 ? 6 : 2);
    

    for (size_t row = 2; row <= rows; ++row) {
        printf("\033[%zu;1H" RESET, row);
    
        for (size_t x = 0; x < width; ++x) { putchar(' '); }
    
        printf("\033[%zu;%zuH%s│" RESET, row, width, SHE_LOVES_PURPLE);
    }
    
    size_t art_height = rows >= 28 ? 7 : 5, art_width = art_height * 2;
    
    playlist_art(panel, 3, 3, art_width, art_height);
    
    size_t title_x = art_width + 6, title_width = width - title_x - 2;
    
    text_at(3, title_x, title_width, DIM, "PERSONAL PLAYLIST");
    playlist_title(panel->title, 4, title_x, title_width, art_height - 2);
    
    uint64_t milliseconds = 0;
    bool unknown = false;
    
    for (size_t i = 0; i < panel->menu.count; ++i) {
    
        if (panel->durations[i] < 0) { unknown = true; }
    
        else if (UINT64_MAX - milliseconds >= (uint64_t)panel->durations[i]) { milliseconds += (uint64_t)panel->durations[i]; }
    }
    
    char info[96];
    unsigned long long seconds = milliseconds / 1000;
    
    snprintf(info, sizeof info, "%zu songs · %llu:%02llu%s", panel->menu.count, seconds / 60, seconds % 60, unknown ? " + ?" : "");
    text_at(2 + art_height, title_x, title_width, DIM, info);
    
    size_t controls = art_height + 4, heading = controls + 2, first = heading + 2;
    
    bool paused = !state->player || state->player->paused || state->playlist_playing_id != panel->playlist_id;
    
    text_at(controls, 3, 13, GREEN_BOLD, paused ? "[ Space ▶ ]" : "[ Space Ⅱ ]");
    text_at(controls, 17, 7, state->player && state->player->shuffle ? GREEN_BOLD : SHE_LOVES_PURPLE, "[ S ⇄ ]");
    text_at(controls, 25, 7, state->player && (state->player->repeat || state->player->repeat_playlist) ? GREEN_BOLD : SHE_LOVES_PURPLE, "[ R ↻ ]");
    text_at(controls, 34, width - 36, SHE_LOVES_PURPLE, "C: cover");
    text_at(heading, 3, width - 12, DIM, "#    TITLE / ARTIST");
    text_at(heading, width - 9, 7, DIM, "TIME");
    printf("\033[%zu;3H%s", heading + 1, SHE_LOVES_PURPLE);
    
    for (size_t i = 0; i < width - 5; ++i) { fputs("─", stdout); }
    
    fputs(RESET, stdout);
    
    TuiMenu *menu = &panel->menu;
    
    normalize(menu);
    
    size_t visible = rows > first + 5 ? (rows - first - 5) / 2 : 0;
    
    if (menu->selected != SIZE_MAX && visible) {
        if (menu->top > menu->selected) { menu->top = menu->selected; }
        if (menu->selected - menu->top >= visible) { menu->top = menu->selected - visible + 1; }
    }
    
    for (size_t i = menu->top; i < menu->count && i - menu->top < visible; ++i) {
        size_t row = first + 2 * (i - menu->top);
    
        char number[32], duration[32];
    
        bool current = state->player && state->player->song_id == panel->ids[i];
    
        snprintf(number, sizeof number, "%zu", i + 1);
    
        text_at(row, 3, 4, current ? GREEN_BOLD : DIM, current ? "▶" : number);
        text_at(row, 8, width - 20, i == menu->selected ? PURPLE_INVERSE_BOLD : BOLDY, menu->items[i].label);
        text_at(row + 1, 8, width - 20, DIM, *panel->artists[i] ? panel->artists[i] : "Artist unknown");
    
        if (panel->durations[i] < 0) { snprintf(duration, sizeof duration, "--:--"); }
    
        else { unsigned long long length = (uint64_t)panel->durations[i] / 1000; snprintf(duration, sizeof duration, "%llu:%02llu", length / 60, length % 60); }
    
        text_at(row, width - 9, 7, DIM, duration);
    }
    
    if (!menu->count) { text_at(first, 3, width - 5, DIM, "No songs. Use a to add from library."); }
    
    text_at(rows - 5, 3, width - 5, DIM, "Enter: play  x: remove  K/J: move");
    text_at(rows - 4, 3, width - 5, DIM, "C: cover  [ ]: skip  Esc: list  p: close");
    text_at(rows - 3, 3, width - 5, GREEN, panel->notice);
    
    char now[512];
    
    if (state->player && state->player->song_id) {
        size_t controls_x = width - 23;
    
        text_at(rows - 2, 3, controls_x - 4, BOLDY, state->player->title);
        text_at(rows - 1, 3, controls_x - 4, DIM, state->player->artist);
        text_at(rows - 2, controls_x, 3, state->player->shuffle ? GREEN_BOLD : SHE_LOVES_PURPLE, "⇄");
        text_at(rows - 2, controls_x + 4, 3, SHE_LOVES_PURPLE, "◀◀");
        text_at(rows - 2, controls_x + 8, 3, GREEN_BOLD, state->player->paused ? "▶" : "Ⅱ");
        text_at(rows - 2, controls_x + 12, 3, SHE_LOVES_PURPLE, "▶▶");
        text_at(rows - 2, controls_x + 16, 3, (state->player->repeat || state->player->repeat_playlist) ? GREEN_BOLD : SHE_LOVES_PURPLE, "↻");
    
        if (state->player->duration_known) {
            snprintf(now, sizeof now, "%llu:%02llu / %llu:%02llu", state->player->elapsed / 60, state->player->elapsed % 60,
                     state->player->duration / 60, state->player->duration % 60);
        }
    
        else { snprintf(now, sizeof now, "%llu:%02llu / --:--", state->player->elapsed / 60, state->player->elapsed % 60); }
    
        text_at(rows - 1, controls_x, 21, DIM, now);
    
        size_t bar = width - 5;
    
        double fraction = state->player->duration ? (double)state->player->elapsed / state->player->duration : 0;
    
        size_t filled = fraction >= 1 ? bar : (size_t)(fraction * bar);
    
        printf("\033[%zu;3H", rows);
    
        for (size_t i = 0; i < bar; ++i) { printf("%s%s", i < filled ? GREEN : SHE_LOVES_PURPLE, i < filled ? "━" : "─"); }
    
        fputs(RESET, stdout);
    }
    
    else { text_at(rows - 2, 3, width - 5, DIM, "No song selected"); }
}

static void playlists_overlay(TuiState *state)
{
    Playlists *panel = state->playlists;
    
    if (!panel) { return; }
    if (panel->playlist_id && panel->mode == PLAYLIST_BROWSE) { playlist_detail(state); return; }
    
    size_t rows, width, x;
    /* The left panel overlaps cover art; CHILL it is redrawn when the panel closes. */
    
    terminal_cover_hide();
    
    if (!sidebar(panel->title, &rows, &width, &x, true)) { return; }
    
    if (panel->mode == PLAYLIST_COVER) {
    
        text_at(4, x + 2, width - 4, BOLDY, "Playlist cover:");
        text_at(5, x + 2, width - 4, DIM, "Local image path or direct HTTP URL");
        text_at(7, x + 2, width - 4, SHE_LOVES_PURPLE, panel->cover_input);
        text_at(9, x + 2, width - 4, DIM, "Empty value removes the cover.");
        text_at(rows - 2, x + 2, width - 4, DIM, "Enter: save  Ctrl+U: clear  Esc: cancel");
    }
    
    else if (panel->mode == PLAYLIST_CREATE || panel->mode == PLAYLIST_RENAME) {
        text_at(4, x + 2, width - 4, BOLDY, panel->mode == PLAYLIST_CREATE ? "New playlist name:" : "Rename playlist:");
    
        char input[160];
    
        snprintf(input, sizeof input, "[ %s_ ]", panel->name);
    
        text_at(6, x + 2, width - 4, SHE_LOVES_PURPLE, input);
        text_at(rows - 2, x + 2, width - 4, DIM, "Enter: save  Esc: cancel");
    }
    
    else if (panel->mode == PLAYLIST_DELETE) {
    
        text_at(4, x + 2, width - 4, BOLDY, "Delete this playlist?");
        text_at(5, x + 2, width - 4, SHE_LOVES_PURPLE, panel->name);
        text_at(6, x + 2, width - 4, DIM, "Songs remain in your library.");
        text_at(rows - 2, x + 2, width - 4, DIM, "Enter: delete  Esc: cancel");
    }
    
    else {
        TuiMenu *menu = &panel->menu;
    
        normalize(menu);
    
        size_t visible = rows - 8;
    
        if (menu->selected != SIZE_MAX && visible) {
            if (menu->top > menu->selected) { menu->top = menu->selected; }
            if (menu->selected - menu->top >= visible) { menu->top = menu->selected - visible + 1; }
        }
    
        for (size_t i = menu->top; i < menu->count && i - menu->top < visible; ++i) {
            text_at(4 + i - menu->top, x + 2, width - 4,
                    i == menu->selected ? PURPLE_INVERSE : RESET, menu->items[i].label);
        }
    
        if (!menu->count) { text_at(4, x + 2, width - 4, DIM, panel->playlist_id ? "No songs. Use a to add from library." : "No playlists. Press c to create."); }
    
        text_at(rows - 3, x + 2, width - 4, DIM, panel->playlist_id ? "Enter: play  x: remove  K/J: move" :
                panel->pending_song ? "Enter: add song  c: new playlist" : "Enter: open  c: create  r: rename");
    
        text_at(rows - 2, x + 2, width - 4, DIM, panel->playlist_id ? "a: add to playlist  Esc: list  p: close" : "x: delete  p / Esc: close");
    }

    text_at(rows - 1, x + 2, width - 4, GREEN, panel->notice);
}

void tui_state_draw(TuiState *state)
{
    terminal_frame_begin();
    if (state->quit_requested) {
        TuiItem items[] = {{"Nah blast the music", TUI_BUTTON, true, false},
                           {"GET ME OUT OF HERE", TUI_BUTTON, true, false}};
        TuiMenu menu = {.title = "Leaving already UnU?", .items = items, .count = 2, .selected = state->quit_selected};
        tui_menu_draw(&menu, "Quit stops playback and cancels downloads. Esc / q: cancel");
        terminal_frame_end();
        return;
    }
    
    if (state->screen == SCREEN_SONGS && state->search_active && state->menus[SCREEN_SONGS]) {
        tui_menu_draw(state->menus[SCREEN_SONGS], state->status);
    
        size_t rows, columns;
    
        terminal_size(&rows, &columns);
    
        if (rows >= 7 && columns >= 24) {
            char bar[320];
    
            snprintf(bar, sizeof bar, "Search all: [ %s%s ]", state->search_query ? state->search_query : "", state->search_editing ? "_" : "");
    
            line(2, columns - 1, PURPLE_BOLD, bar);
    
            if (!state->menus[SCREEN_SONGS]->count) { line(3, columns - 1, DIM, "Just staric..."); }
    
            line(rows - 1, columns - 1, DIM, state->search_editing ?
                 "Type title/artist  Up/Down/Tab: select result  Enter: play" :
                 "Arrows:select Enter:play d:download D:all n:next a:save Q:queue");
    
            line(rows, columns - 1, DIM, state->search_editing ?
                 "Backspace: delete  Ctrl+U: clear  Esc: close search" :
                 "s: edit  Esc: close search");
        }
    }

    else if (state->screen == SCREEN_PLAYER && state->player) {
        tui_player_draw(state->player, state->status);
    }

    else if (state->menus[state->screen]) {
        if (state->screen == SCREEN_HOME || state->screen == SCREEN_GENRES || state->screen == SCREEN_SETTINGS) {
            jukebox_menu_draw(state->menus[state->screen], state->screen, state->status);
        }

        else {
            tui_menu_draw(state->menus[state->screen], state->status);
            if (state->screen == SCREEN_SONGS) {
                size_t rows, columns;
                terminal_size(&rows, &columns);
                if (rows >= 7 && columns >= 24) {
                    line(rows, columns - 1, DIM, "d:download D:all s:search n:next a:save p:playlists Q:queue Esc:back");
                }
            }
        }
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

    else if (state->screen == SCREEN_VISUALIZER) { visualizer_draw(state); }

    else { pending_screen(state); }
    
    if (state->overlay == OVERLAY_QUEUE) { queue_overlay(state); }
    if (state->overlay == OVERLAY_TYPEWRITER) { typewriter_overlay(state); }
    if (state->overlay == OVERLAY_PLAYLISTS) { playlists_overlay(state); }
    
    terminal_frame_end();
}
