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
static int active, registered, escape_state;
static unsigned parameter;
static int cover_loaded;

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
        terminal_cover_free();
        
        while (tcsetattr(STDIN_FILENO, TCSANOW, &original) < 0 && errno == EINTR) {}
        
        fputs(RESET "\033[?25h\033[?1049l", stdout);
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
    
    fputs("\033[?1049h\033[?25l", stdout);
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
    
        TerminalAction action = escape_state == 1 ? TERM_BACK : TERM_NONE;
    
        escape_state = 0;
    
        return(action);
    }
    
    if (escape_state == 1) {
        
        if (byte == '[' || byte == 'O') { escape_state = 2; parameter = 0; }


        else { escape_state = 0; }
        
        return(TERM_NONE);
    }
    
    if (escape_state == 2) {
    
        if (byte >= '0' && byte <= '9') {
            parameter = parameter < 100 ? parameter * 10 + byte - '0' : 100;
    
            return(TERM_NONE);
        }
    
        if (byte < 0x40 || byte > 0x7e) { return(TERM_NONE); }
    
        escape_state = 0;
    
        switch (byte) { //This turned into fucking alphabet song A B C D E F G H I J K lmnop 
            case 'A': return(TERM_UP);
            case 'B': return(TERM_DOWN);
            case 'C': return(TERM_RIGHT);
            case 'D': return(TERM_LEFT);
            case 'H': return(TERM_FIRST);
            case 'F': return(TERM_LAST);
            case '~':
                if (parameter == 1 || parameter == 7) { return(TERM_FIRST); }
                if (parameter == 4 || parameter == 8) { return(TERM_LAST); }
                return(TERM_NONE);
            
            default: return(TERM_NONE);
        }
    }
    
    switch (byte) {
        case 27: escape_state = 1; return(TERM_NONE);
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
        case '\t': return(TERM_NEXT_VIEW);
        case 4: return(TERM_END);
        default: return(TERM_NONE);
    }
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

void terminal_clear(void)
{
    terminal_cover_hide();
    fputs("\033[H\033[2J", stdout);
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

int terminal_cover_draw(size_t row, size_t column, size_t width, size_t height)
{
    if (!cover_loaded || !width || !height) { return(0); }

    printf("\033[%zu;%zuH\033_Ga=p,i=" COVER_NUMBER ",p=1,c=%zu,r=%zu,C=1,q=2;\033\\", row, column, width, height);
    return(1);
}

int terminal_cover_load(const char *path)
{
    const char *term = getenv("TERM");
    if (!active) { errno = EINVAL; return(-1); }
    /* Multiplexer passthrough is a separate backend
     don't emit the fucking raw APC there  
    */

    if (!term || strcmp(term, "xterm-kitty") || getenv("TMUX") || getenv("STY")) {
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
    
    terminal_cover_free();
    
    const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; //Lowkey I gotta add a few more chars but nah
    
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
        
        printf("\033_G%sm=%d;%s\033\\", offset == 0 ?
               "a=t,f=100,t=d,i=" COVER_NUMBER ",q=2," : "",
               offset + chunk < count, encoded);
        offset += chunk;
    }
    
    free(data);
    data = NULL;
    
    cover_loaded = 1;
    
    if (fflush(stdout) == EOF) { errno = EIO; return(-1); }
    
    return(0);
}
