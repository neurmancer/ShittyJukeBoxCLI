#ifndef SHITTYJUKEBOX_TERMINAL_HANDLER_H
#define SHITTYJUKEBOX_TERMINAL_HANDLER_H
#define RESET         "\033[0m"
#define PURPLE        "\033[38;2;193;112;255m"
#define GREEN         "\033[38;2;0;235;116m"

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
    TERM_RESIZE, TERM_END, TERM_ERROR,
    TERM_PLAYER, TERM_LYRICS, TERM_VISUALIZER, TERM_QUEUE, TERM_NEXT_VIEW
} TerminalAction;

int terminal_init(void);
void terminal_restore(void);

TerminalAction terminal_read(int timeout_ms);

int terminal_signal(void);

void terminal_size(size_t *rows, size_t *columns);
void terminal_clear(void);

/* Kitty PNG transport(stole the idea from fastfetch ngl). Upload once, then draw again and again
 * Returns -1 with errno for unavailable graphics or unreadable/invalid PNG.
 */
int terminal_cover_load(const char *path);
int terminal_cover_draw(size_t row, size_t column, size_t width, size_t height);
void terminal_cover_hide(void);
void terminal_cover_free(void);

#endif
