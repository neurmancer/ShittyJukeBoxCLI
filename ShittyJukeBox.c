//Fresh start time...

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <math.h>

//I am fucking paralyzed can't type anything...why?


//She Loves Purple, So Do I.

#define SECOND 1000000
#define FREQ 0.5f

//ANSI DEFINES 

#define FIX_FONT "\e[0m"
#define WIPE_TERMINAL "\033[H\033[J"
#define YET_ANOTHER_WIPE_TERMINAL "\033[3J"
#define WIPE_TOP "\033[2J"
#define GO_HOME "\033[H"
#define VANISH_CURSOR "\033[?25l"
#define BRING_BACK_THE_CURSOR_FROM_THE_DEAD "\033[?25h"
#define BOLD_PURPLE "\e[1;95m"


//Fine let me handle termios today at least...


/* =============== FUNCTION PROTOTPES =============== */
//Yeah the guy with exuberance and shitty jokes have been partially grown-up...No more in code stand-ups unf...
void typewriter(char *lyrics);
void epilepsyTypewriter(char *lyrics);
void boldTypewriter(char *lyrics);

void confTermios(void);


typedef void (*WriterFunction)(char *lyrics);


/* ================= GLOBAL VARS ==================== (yeah I'll have globals...After all I am still me lol)*/

static struct termios ogTerm,newTerm;






int main(void)
{
    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0);
    printf(VANISH_CURSOR);
    
    
    WriterFunction writerTypes[] = {
        typewriter,
        epilepsyTypewriter,
        boldTypewriter
    };    


    char *test = "This is the test\n";
    
    writerTypes[rand() % 3](test);


    printf(BRING_BACK_THE_CURSOR_FROM_THE_DEAD);
    return(0);
}


void typewriter( char *lyrics)
{
    char *iter = lyrics;
    while (*iter != '\0') {
        printf("%c",*iter);
        iter++;
        usleep(100220);
    }
}


void epilepsyTypewriter(char *lyrics)
{
    char *c = lyrics;
    long colorTimer = 0;
    int i = 0;
    while (*c != '\0') {
        
        int r = (sin(FREQ * i + colorTimer * 0.2 + 0) * 127) + 128;
        int g = (sin(FREQ * i + colorTimer * 0.2 + 2) * 127) + 128;
        int b = (sin(FREQ * i + colorTimer * 0.2 + 4) * 127) + 128;

        printf("\033[38;2;%d;%d;%dm%c", r, g, b, *c);
        i++;
        c++;
        usleep(50000);
    }
    printf(FIX_FONT);
}


void boldTypewriter(char* song)
{
    printf(BOLD_PURPLE);
    while (*song != '\0')
    {
        if (*song == '\n') 
        {
            usleep(SECOND);
            printf("%c",*song);
        }
        
        else
        {
            printf("%c",*song);
            usleep((2/3)); 
        }
        song++;
    }
    printf(FIX_FONT);

}


void confTermios(void)
{
    tcgetattr(STDOUT_FILENO,  &ogTerm);

    newTerm = ogTerm;
    newTerm.c_lflag &= (ICANON | ECHO);
    newTerm.c_cc[VMIN] = 0;
    newTerm.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &newTerm);
}


void fixTerm(void)
{
    printf(FIX_FONT BRING_BACK_THE_CURSOR_FROM_THE_DEAD);

    tcsetattr(STDIN_FILENO, TCSANOW, &ogTerm);
}