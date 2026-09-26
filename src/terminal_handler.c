#define _POSIX_C_SOURCE 200809L
#include "terminal_handler.h"
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

static struct termios original;
static const int signals[] = {SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGWINCH};
static struct sigaction previous[sizeof signals / sizeof signals[0]];
static size_t installed;
static int active, registered, escape_state, frame_open;
static char key_sequence[64];
static size_t key_length;
static int key_overflow;
static unsigned modifier_taps;
static int cover_loaded;
static char cover_error[160];
static char graphics_reply[512];
static size_t graphics_length;
static int graphics_overflow;

static volatile sig_atomic_t stopped, resized;  //You want explanations? go to fucking basic-c repo this is not kindergarten


/* Private to our alternate screen; never delete another application's images. */
#define COVER_NUMBER "1397375563"

static void on_signal(int number)
{
    if (number == SIGWINCH) { resized = 1; }

    else { stopped = number; }
}

void terminal_restore(void)
{
    if (active) {
        terminal_frame_end();
        terminal_cover_free();
        
        while (tcsetattr(STDIN_FILENO, TCSANOW, &original) < 0 && errno == EINTR) {}
        
        fputs("\033[<u" RESET "\033[?25h\033[?1049l", stdout);
        fflush(stdout);
        
        active = 0;
    }
    while (installed) {
        --installed;
        sigaction(signals[installed], &previous[installed], NULL);
    }
}

int terminal_init(void)
{
    if (active) { return(0); }
    if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO)) { errno = ENOTTY; return(-1); }
    if (tcgetattr(STDIN_FILENO, &original) < 0) { return(-1); }
    
    if (!registered) {
        if (atexit(terminal_restore) != 0) { errno = ENOMEM; return(-1); }
        registered = 1;
    }
    
    stopped = resized = escape_state = 0;
    
    struct sigaction handler = {0};
    
    handler.sa_handler = on_signal;
    
    sigemptyset(&handler.sa_mask);
    
    for (size_t i = 0; i < sizeof signals / sizeof signals[0]; ++i) {
        if (sigaction(signals[i], &handler, &previous[i]) < 0) { goto fail; }
        ++installed;
    }
    
    struct termios settings = original;
    
    settings.c_lflag &= ~(ICANON | ECHO | IEXTEN);
    settings.c_lflag |= ISIG;
    settings.c_iflag &= ~(IXON | ICRNL);
    /* Keep Ctrl+C; disable Ctrl+Z until suspend/resume is supported. */
    
    settings.c_cc[VSUSP] = _POSIX_VDISABLE;
    settings.c_cc[VMIN] = 1;
    settings.c_cc[VTIME] = 0;
    
    if (tcsetattr(STDIN_FILENO, TCSANOW, &settings) < 0) { goto fail; }
    
    active = 1;
    
    /* Kitty keyboard protocol: push disambiguation, events, and all keys.
     * Legacy terminals ignore this; restore pops our mode before leaving. */
    modifier_taps = 0;
    fputs("\033[?1049h\033[?25l\033[>11u", stdout);
    fflush(stdout);
    return(0);

fail: {
        int saved = errno;
        terminal_restore();
        errno = saved;
        return(-1);
    }
}

/* 1: byte, 0: timeout/interrupted, -1: error, -2: EOF (Future Neuro will fucking forget that). */
static int read_byte(unsigned char *byte, int timeout_ms)
{
    struct pollfd input = {STDIN_FILENO, POLLIN, 0};
    
    int ready = poll(&input, 1, timeout_ms);
    
    /*
    The poll() function provides applications with a mechanism for multiplexing input/output over a set of file descriptors.  For  each  member  of  the  array
     pointed  to  by fds, poll() shall examine the given file descriptor for the event(s) specified in events.  The number of pollfd structures in the fds array
     is specified by nfds.  The poll() function shall identify those file descriptors on which an application can read or write data, or on which certain events
     have occurred          - man 3 poll (I'll forget)
    */


    if (ready < 0) { return(errno == EINTR ? 0 : -1); }
    if (!ready) { return(0); }
    if (input.revents & POLLNVAL) { errno = EBADF; return(-1); }
    if (input.revents & (POLLIN | POLLHUP)) {
        ssize_t count = read(STDIN_FILENO, byte, 1);
    
        if (count > 0) { return(1); }
        if (!count) { return(-2); }
        if (errno == EINTR || errno == EAGAIN) { return(0); }
    
        return(-1);
    }
    
    errno = EIO;
    return(-1);
}

static TerminalAction plain_key(unsigned byte)
{
    switch (byte) {
        case 27: return(TERM_BACK);
        case 'k': return(TERM_UP);
        case 'j': return(TERM_DOWN);
        case 'h': return(TERM_LEFT);
        case 'l': return(TERM_RIGHT);
        case 'g': return(TERM_FIRST);
        case 'G': return(TERM_LAST);
        case '\r': case '\n': case ' ': return(TERM_ACTIVATE);
        case 127: case '\b': return(TERM_BACK);
        case 'q': return(TERM_QUIT);
        case '1': return(TERM_PLAYER);
        case '2': return(TERM_LYRICS);
        case '3': return(TERM_VISUALIZER);
        case 'Q': return(TERM_QUEUE);
        case 't': return(TERM_TYPEWRITER);
        case 's': return(TERM_SETTINGS);
        case '\t': return(TERM_NEXT_VIEW);
        case 4: return(TERM_END);
        default: return(TERM_NONE);
    }
}

/* Report modifier taps on release, so Shift+Q and Ctrl+C do not change volume. */
static TerminalAction escape_key(unsigned char final)
{
    unsigned key = 0, modifiers = 1, event = 1;
    const char *p = key_sequence;
    unsigned *fields[] = {&key, &modifiers, &event};
    for (size_t i = 0; i < 3; ++i) {
        if (*p >= '0' && *p <= '9') {
            *fields[i] = 0;
            while (*p >= '0' && *p <= '9') {
                if (*fields[i] > 1000000) { return(TERM_NONE); }
                *fields[i] = *fields[i] * 10 + (unsigned)(*p++ - '0');
            }
        }
        if (i == 0 && *p == ';') { ++p; continue; }
        if (i == 1 && *p == ':') { ++p; continue; }
        break;
    }
    if (*p || !modifiers || event < 1 || event > 3) { return(TERM_NONE); }
    unsigned tap = final == 'u' ? key == 57441 ? 1 : key == 57447 ? 2 :
                   key == 57442 ? 4 : key == 57448 ? 8 : 0 : 0;
    if (tap) {
        if (event == 1) {
            unsigned expected = tap <= 2 ? 1 : 4;
            modifier_taps = ((modifiers - 1) & 63) == expected ? tap : 0;
        }
        if (event != 3) { return(TERM_NONE); }
        int alone = (modifier_taps & tap) != 0;
        modifier_taps &= ~tap;
        return(alone ? (tap <= 2 ? TERM_VOLUME_UP : TERM_VOLUME_DOWN) : TERM_NONE);
    }
    modifier_taps = 0;
    if (event == 3) { return(TERM_NONE); }
    if (final == 'u') {
        unsigned mask = modifiers - 1;
        if ((mask & 4) && key == 'c') { raise(SIGINT); return(TERM_QUIT); }
        if ((mask & 4) && key == 'd') { return(TERM_END); }
        if (mask & ~193u) { return(TERM_NONE); } /* Shift and lock keys only. */
        if ((mask & 1) && key >= 'a' && key <= 'z') { key -= 'a' - 'A'; }
        return(plain_key(key));
    }
    if ((modifiers - 1) & ~192u) { return(TERM_NONE); }
    switch (final) {
        case 'A': return(TERM_ARROW_UP);
        case 'B': return(TERM_ARROW_DOWN);
        case 'C': return(TERM_RIGHT);
        case 'D': return(TERM_LEFT);
        case 'H': return(TERM_FIRST);
        case 'F': return(TERM_LAST);
        case '~':
            if (key == 1 || key == 7) { return(TERM_FIRST); }
            if (key == 4 || key == 8) { return(TERM_LAST); }
            return(TERM_NONE);
        default: return(TERM_NONE);
    }
}

TerminalAction terminal_read(int timeout_ms)
{
    if (!active) { errno = EINVAL; return(TERM_ERROR); }
    if (stopped) { return(TERM_QUIT); }
    if (resized) { resized = 0; return(TERM_RESIZE); }
    
    unsigned char byte;
    int result = read_byte(&byte, escape_state ? 40 : timeout_ms);
    
    if (result == -1) { return(TERM_ERROR); }
    if (result == -2) { escape_state = 0; return(TERM_END); }
    if (!result) {
    
        if (stopped) { return(TERM_QUIT); }
        if (resized) { resized = 0; return(TERM_RESIZE); }
    
        if (escape_state == 3 || escape_state == 4) { return(TERM_NONE); }
        TerminalAction action = escape_state == 1 ? TERM_BACK : TERM_NONE;
    
        escape_state = 0;
    
        return(action);
    }
    
    /* Kitty graphics replies are APC strings, never keyboard shortcuts. */
    if (escape_state == 3 || escape_state == 4) {
        if (escape_state == 4 && byte == '\\') {
            escape_state = 0;
            graphics_reply[graphics_length] = '\0';
            unsigned id = 0;
            const char *message = strchr(graphics_reply, ';');
            if (!graphics_overflow && cover_loaded && message &&
                sscanf(graphics_reply, "Gi=%u", &id) == 1 && id == 1397375563u &&
                strcmp(message + 1, "OK")) {
                snprintf(cover_error, sizeof cover_error, "Cover: %.150s", message + 1);
                for (char *p = cover_error; *p; ++p) {
                    if ((unsigned char)*p < 32 || (unsigned char)*p == 127) { *p = '?'; }
                }
                return(TERM_RESIZE);
            }
            return(TERM_NONE);
        }
        if (byte == 27) { escape_state = 4; return(TERM_NONE); }
        escape_state = 3;
        if (graphics_length + 1 < sizeof graphics_reply) { graphics_reply[graphics_length++] = (char)byte; }

        else { graphics_overflow = 1; }
        return(TERM_NONE);
    }
    if (escape_state == 1) {
        if (byte == 27) { return(TERM_BACK); }
        
        if (byte == '_') {
            escape_state = 3;
            graphics_length = 0;
            graphics_overflow = 0;
        }

        else if (byte == '[' || byte == 'O') {
            escape_state = 2;
            key_length = 0;
            key_overflow = 0;
        }

        else { escape_state = 0; }
        return(TERM_NONE);
    }
    if (escape_state == 2) {
        if (byte >= 0x40 && byte <= 0x7e) {
            escape_state = 0;
            key_sequence[key_length] = '\0';
            return(key_overflow ? TERM_NONE : escape_key(byte));
        }
        if (key_length + 1 < sizeof key_sequence) { key_sequence[key_length++] = (char)byte; }

        else { key_overflow = 1; }
        return(TERM_NONE);
    }
    if (byte == 27) { escape_state = 1; return(TERM_NONE); }
    modifier_taps = 0;
    return(plain_key(byte));
}

int terminal_signal(void) { return(stopped); }

void terminal_size(size_t *rows, size_t *columns)
{
    struct winsize size;
    *rows = 24;
    *columns = 80;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0) {
        if (size.ws_row) { *rows = size.ws_row; }
        if (size.ws_col) { *columns = size.ws_col; }
    }
}

void terminal_frame_begin(void)
{
    /* DEC mode 2026: supporting terminals retain the previous complete frame
     * while we clear, redraw lyrics, apply highlights, and paint overlays. */
    if (frame_open) { return; }
    frame_open = 1;
    fputs("\033[?2026h", stdout);
}

void terminal_frame_end(void)
{
    if (!frame_open) { return; }
    frame_open = 0;
    fputs("\033[?2026l", stdout);
    fflush(stdout);
}

void terminal_clear(void)
{
    terminal_cover_hide();
    /* ED 2 also destroys Kitty's stored images. ED 0 from home erases text
     * only; terminal_cover_hide() already removed our visible placement. */
    fputs("\033[H\033[J", stdout);
}

void terminal_cover_hide(void)
{
    if (cover_loaded) { fputs("\033_Ga=d,d=i,i=" COVER_NUMBER ",q=2;\033\\", stdout); }
}

void terminal_cover_free(void)
{
    if (cover_loaded) {
        fputs("\033_Ga=d,d=I,i=" COVER_NUMBER ",q=2;\033\\", stdout);
        cover_loaded = 0;
    }
}

const char *terminal_cover_error(void) { return(cover_error); }

int terminal_cover_draw(size_t row, size_t column, size_t width, size_t height)
{
    if (!cover_loaded || *cover_error || !width || !height) { return(0); }

    printf("\033[%zu;%zuH\033_Ga=p,i=" COVER_NUMBER ",p=1,c=%zu,r=%zu,C=1,q=1;\033\\", row, column, width, height);
    return(1);
}

int terminal_cover_supported(void)
{
    const char *term = getenv("TERM");
    return(active && term && !strcmp(term, "xterm-kitty") && !getenv("TMUX") && !getenv("STY"));
}

static int cover_upload(const unsigned char *data, size_t count, const char *format)
{
    terminal_cover_free();
    cover_error[0] = '\0';
    const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (size_t offset = 0; offset < count;) {
        size_t chunk = count - offset;
        if (chunk > 3072) { chunk = 3072; }
        char encoded[4097];
        size_t output = 0;
        for (size_t i = 0; i < chunk; i += 3) {
            unsigned value = (unsigned)data[offset + i] << 16;
            if (i + 1 < chunk) { value |= (unsigned)data[offset + i + 1] << 8; }
            if (i + 2 < chunk) { value |= data[offset + i + 2]; }
            encoded[output++] = alphabet[(value >> 18) & 63];
            encoded[output++] = alphabet[(value >> 12) & 63];
            encoded[output++] = i + 1 < chunk ? alphabet[(value >> 6) & 63] : '=';
            encoded[output++] = i + 2 < chunk ? alphabet[value & 63] : '=';
        }
        encoded[output] = '\0';
        printf("\033_G%sm=%d;%s\033\\", offset == 0 ? format : "", offset + chunk < count, encoded);
        offset += chunk;
    }
    cover_loaded = 1;
    if (fflush(stdout) == EOF) { terminal_cover_free(); errno = EIO; return(-1); }
    return(0);
}

int terminal_cover_rgba(const unsigned char *pixels, size_t width, size_t height)
{
    if (!terminal_cover_supported()) { errno = ENOTSUP; return(-1); }
    if (!pixels || !width || !height || width > 512 || height > 512) { errno = EINVAL; return(-1); }
    char format[128];
    snprintf(format, sizeof format, "a=t,f=32,s=%zu,v=%zu,t=d,i=" COVER_NUMBER ",q=1,", width, height);
    return(cover_upload(pixels, width * height * 4, format));
}

int terminal_cover_load(const char *path)
{
    terminal_cover_free();
    cover_error[0] = '\0';
    if (!active) { errno = EINVAL; return(-1); }
    /* Multiplexer passthrough is a separate backend
     don't emit the fucking raw APC there  
    */

    if (!terminal_cover_supported()) {
        errno = ENOTSUP;
        return(-1);
    }
    
    FILE *file = fopen(path, "rb");
    
    if (!file) { return(-1); }
    if (fseek(file, 0, SEEK_END) != 0) { fclose(file); errno = EINVAL; return(-1); }
    
    long length = ftell(file);
    
    if (length < 33 || length > 8 * 1024 * 1024) {
    
        fclose(file);
        errno = length > 8 * 1024 * 1024 ? EFBIG : EINVAL;
    
        return(-1);
    }
    
    rewind(file);
    
    unsigned char *data = malloc((size_t)length);
    
    if (!data) { fclose(file); errno = ENOMEM; return(-1); }
    
    size_t count = fread(data, 1, (size_t)length, file);
    int failed = ferror(file);
    
    fclose(file);
    
    if (failed || count != (size_t)length || memcmp(data, "\211PNG\r\n\032\n", 8) || memcmp(data + 12, "IHDR", 4)) {
        
        free(data);
        data = NULL;    //I trust future me none 

        errno = EINVAL;
        return(-1);
    }
    
    int result = cover_upload(data, count, "a=t,f=100,t=d,i=" COVER_NUMBER ",q=1,");
    free(data);
    return(result);
}
