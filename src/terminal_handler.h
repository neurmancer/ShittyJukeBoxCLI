#ifndef SHITTYJUKEBOX_TERMINAL_HANDLER_H
#define SHITTYJUKEBOX_TERMINAL_HANDLER_H
#define RESET         "\033[0m"
typedef enum { THEME_PURPLE, THEME_GREEN, THEME_BLUE, THEME_COLOR_COUNT } ThemeColor;
typedef enum { THEME_NORMAL, THEME_BOLD, THEME_SELECTED, THEME_ACTIVE,
               THEME_INVERSE, THEME_INVERSE_BOLD, THEME_STYLE_COUNT } ThemeStyle;
extern char terminal_colors[THEME_COLOR_COUNT][THEME_STYLE_COUNT][64];
void terminal_set_color(ThemeColor color, unsigned rgb);
#define SHE_LOVES_PURPLE terminal_colors[THEME_PURPLE][THEME_NORMAL]
#define GREEN terminal_colors[THEME_GREEN][THEME_NORMAL]
#define BLUE terminal_colors[THEME_BLUE][THEME_NORMAL]
#define PURPLE_BOLD terminal_colors[THEME_PURPLE][THEME_BOLD]
#define PURPLE_SELECTED terminal_colors[THEME_PURPLE][THEME_SELECTED]
#define PURPLE_INVERSE terminal_colors[THEME_PURPLE][THEME_INVERSE]
#define PURPLE_INVERSE_BOLD terminal_colors[THEME_PURPLE][THEME_INVERSE_BOLD]
#define GREEN_BOLD terminal_colors[THEME_GREEN][THEME_BOLD]
#define GREEN_ACTIVE terminal_colors[THEME_GREEN][THEME_ACTIVE]

#define BOLDY          "\033[1m"
#define DIM           "\033[2m"
#define FANCY        "\033[3m"
#define UNDERLINE     "\033[4m"
#define WINK         "\033[5m"
#define INVERSE       "\033[7m"
#define STEALTH        "\033[8m"
#define STRIKE        "\033[9m"

#define NOPE_BOLD_DIM   "\033[22m"
#define NOPE_FANCY     "\033[23m"
#define NOPE_UNDERLINE  "\033[24m"
#define NOPE_WINK      "\033[25m"
#define NOPE_INVERSE    "\033[27m"
#define NOPE_STEALTH     "\033[28m"
#define NOPE_STRIKE     "\033[29m"




/*

    I just realized (or realised for the UK guys) something...when there are more than one giant monolith I can't comment as feral as I was able to...
    It fucks with my ADHD my all energy goes to finding the right fucking file
*/


#include <stddef.h>

typedef enum {
    TERM_NONE, TERM_UP, TERM_DOWN, TERM_LEFT, TERM_RIGHT,
    TERM_FIRST, TERM_LAST, TERM_ACTIVATE, TERM_BACK, TERM_QUIT,
    TERM_RESIZE, TERM_END, TERM_EOF, TERM_ERROR,
    TERM_ARROW_UP, TERM_ARROW_DOWN, TERM_VOLUME_UP, TERM_VOLUME_DOWN,
    TERM_PLAYER, TERM_LYRICS, TERM_VISUALIZER, TERM_QUEUE, TERM_NEXT_VIEW, TERM_SETTINGS, TERM_TYPEWRITER,
    TERM_REWIND, TERM_FAST_FORWARD, TERM_FOLLOW_LYRICS, TERM_DOWNLOAD, TERM_DOWNLOAD_ALL, TERM_REMOVE, TERM_MOVE_UP, TERM_MOVE_DOWN,
    TERM_TEXT, TERM_ERASE, TERM_CLEAR_TEXT, TERM_QUEUE_NEXT,
    TERM_PLAYLISTS, TERM_ADD_PLAYLIST, TERM_CREATE, TERM_RENAME,
    TERM_COVER, TERM_SPACE, TERM_SHUFFLE, TERM_REPEAT, TERM_PREVIOUS_TRACK, TERM_NEXT_TRACK
} TerminalAction;

int terminal_init(void);
void terminal_restore(void);

TerminalAction terminal_read(int timeout_ms);
void terminal_text_mode(int enabled);
const char *terminal_text(void);

int terminal_signal(void);

void terminal_size(size_t *rows, size_t *columns);
void terminal_clear(void);
/* Present a complete redraw together on terminals supporting synchronized output. */
void terminal_frame_begin(void);
void terminal_frame_end(void);

/* Kitty PNG transport(stole the idea from fastfetch ngl). Upload once, then draw again and again
 * Returns -1 with errno for unavailable graphics or unreadable/invalid PNG.
 */
int terminal_cover_load(const char *path);
int terminal_cover_supported(void);
int terminal_cover_rgba(const unsigned char *pixels, size_t width, size_t height);
/* Asynchronous errors returned by the terminal; empty when none reported. */
const char *terminal_cover_error(void);
int terminal_cover_draw(size_t row, size_t column, size_t width, size_t height);
void terminal_cover_hide(void);
void terminal_cover_free(void);

#endif
