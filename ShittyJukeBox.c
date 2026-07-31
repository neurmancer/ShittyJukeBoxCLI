//Fresh start time...

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>

//I am fucking paralyzed can't type anything...why?


//She Loves Purple, So Do I.

#define SECOND 1000000

//ANSI DEFINES 

#define FIX_FONT "\e[0m"
#define WIPE_TERMINAL "\033[H\033[J"
#define YET_ANOTHER_WIPE_TERMINAL "\033[3J"
#define WIPE_TOP "\033[2J"
#define GO_HOME "\033[H"
#define VANISH_CURSOR "\033[?25l"
#define BRING_BACK_THE_CURSOR_FROM_THE_DEAD "\033[?25h"



void typewriter(char *lyrics);


int main(void)
{
    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0);

    

    return(0);
}



void typewriter( char *lyrics)
{
    char *iter = lyrics;
    while (*iter != EOF) {
        printf("%c",*iter);
        iter++;
    }
}