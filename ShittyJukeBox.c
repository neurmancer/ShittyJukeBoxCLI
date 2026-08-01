//Fresh start time...

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>  //Yk...the usual terminal shenanigans
#include <termios.h>

//The mistfit requiring -lm
#include <math.h>


//I am fucking paralyzed can't type anything...why?


//She Loves Purple, So Do I.

#define SECOND 1000000000
#define FREQ 0.5f

//ANSI DEFINES 

#define RESET_FONT "\e[0m"
#define WIPE_TERMINAL "\033[H\033[J"
#define YET_ANOTHER_WIPE_TERMINAL "\033[3J"
#define WIPE_TOP "\033[3J"
#define GO_HOME "\033[H"
#define VANISH_CURSOR "\033[?25l"
#define BRING_BACK_THE_CURSOR_FROM_THE_DEAD "\033[?25h"
#define BOLD_PURPLE "\e[1;95m"
#define SUMMON_CURSOR "\033[%d;%dH"
#define CLEAR_TRAIL "\033[%d;%dH\033[J"

//Coloring shit
#define RED_BG "\e[41m"
#define RED_AF_BG "\e[0;101m"
#define SHE_LOVES "\e[45m"  //So Do I...



//My shitty defines

#define PAGE 4096
#define SPACE_BUF 32


//Fine let me handle termios today at least...



/* ================= OBJECTS =======================*/

typedef struct{
    int x, y;        //Note to self: terminal uses y,x format not x,y and starts at (1,1) not (0, 0) mwah OwO    -Past Me.
    int lastX, lastY;
}Cursor;


/* =============== FUNCTION PROTOTPES =============== */
//Yeah the guy with exuberance and shitty jokes have been partially grown-up...No more in code stand-ups unf...
//Lyrical shit
void typewriter(char *lyrics);
void epilepsyTypewriter(char *lyrics);
void boldTypewriter(char *lyrics);

//UI things
void printMainMenu(char *genres[], size_t genreAmount);

void confTermios(void);
void fixTerm(void);

/* I/O thingies */

int readInput(void);
int keyHandler(char *buf, int k);
void testCase(int key, Cursor *cursor);

typedef void (*WriterFunction)(char *lyrics);


/* ================= GLOBAL VARS ==================== (yeah I'll have globals...After all I am still me lol)*/

static struct termios ogTerm,newTerm;

/*
    Now the shit I gotta do... 

    1- Get a buffer reading func to yk...reading buffer
    2- Handle keystrokes (arrow keys to be specific for or you know what?we're going vim mod j and k for up and down )
    3- Do the thing xD


    2.1 - Figure out how to fuck 'hover' over genres with j-k
*/


//Yup handled the up down...NICE!

int main(void)
{
    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0);
    confTermios();
    printf(VANISH_CURSOR);
    printf(WIPE_TERMINAL);

    char *genres[]= {
        "2000s Emo Music",
        "2000s Nightcore ADHD",
        "New Wave of British Heavy Metal",
        "White Girl Pop",
    };

    size_t genreAmount = sizeof(genres)/sizeof(genres[0]);

    WriterFunction writerTypes[] = {
        typewriter,
        epilepsyTypewriter,
        boldTypewriter
    };    


    struct timespec req = { 0 };    //RTFM I won't explain shit this is not the basic repo 
    struct timespec rem = { 0 };    //This is where my toxic emo persona shines
    Cursor cursor = {1, 4, 1, 1 }; 

    int key = 0;
    

/*======== EXPERIMENTAL ZONE ================ */

    printMainMenu(genres, genreAmount);
goto debug;

/*======== EXPERIMENTAL ZONE ================ */


    while (key != -1) {
        key = readInput();

        testCase(key, &cursor);

  

        printf(SUMMON_CURSOR RESET_FONT " " , cursor.lastY, cursor.lastX);
        printf(SUMMON_CURSOR RED_AF_BG " " RESET_FONT,cursor.y,cursor.x);
        req.tv_nsec = 0.05*SECOND;
        nanosleep(&req, &rem);
    }

debug:  //Poor man's gdb

    fixTerm();
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
    printf(RESET_FONT);
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
    printf(RESET_FONT);

}


void confTermios(void)
{
    tcgetattr(STDIN_FILENO,  &ogTerm);

    newTerm = ogTerm;
    newTerm.c_lflag &= ~(ICANON | ECHO);    //Ops forgot to switch the bits 
    newTerm.c_cc[VMIN] = 0;
    newTerm.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &newTerm);
}


void fixTerm(void)
{
    printf(RESET_FONT BRING_BACK_THE_CURSOR_FROM_THE_DEAD);

    tcsetattr(STDIN_FILENO, TCSANOW, &ogTerm);
}


/*          **I/O THINGIES**    */

int readInput(void)
{

    char buf[PAGE] = { 0 };
    int reader = read(STDIN_FILENO, buf, sizeof(buf));
    int lastPressedKey = 0;

    for (int k = 0;k < reader; k++) {
        int key = keyHandler(buf, k);
        if (!key) { continue; }
        lastPressedKey = key;   //That was the bug
    }
    //Feels like that's gonna have fuck tone of edge cases but here we are 
    
    return(lastPressedKey);
}

int keyHandler(char *buf, int k)
{
    if (buf[k] == 'q') { return(-1);}

    if (buf[k] == '\n') {
        //Line feed (lol you didn't know that name do you?)
        //and yeah since we're on Linux why struggling with carriage return too amirite?
        return(-2);
    }

    switch (buf[k]) {
        case 'j':
            return(1);  //I mean...I could've made them 1 and -1 want to make the navigation more natural with %d;%dH...but we'll see
        case 'k':
            return(2);
   
        case 'h':
            return(3);  //I mean...I could've made them 1 and -1 want to make the navigation more natural with %d;%dH...but we'll see
        case 'l':
            return(4);
    }

    return(0);
}

void testCase(int key, Cursor *cursor)
{

    cursor->lastX = cursor->x;
    cursor->lastY = cursor->y;

    switch (key) {
        case 1:
            cursor->y++;
            break;
    
    
        case 2:
            cursor->y--;
            break;
    

        case 3:
            cursor->x--;
            break;

        case 4:
            cursor->x++;
            break;
    }
}


void printMainMenu(char *genres[], size_t genreAmount)
{

    char emptyString[SPACE_BUF];
    for (size_t i = 0;i < SPACE_BUF-1;i++) {
        emptyString[i] = ' ';        // ' ' character without \0
    }

    for (size_t i = 0; i < 4; i++) {
        int genreLength = strlen(genres[i]);
        if (genreLength > SPACE_BUF) { return; }
        int spacing = SPACE_BUF - genreLength;
        printf(RED_AF_BG BOLD_PURPLE"%s%.*s\n",genres[i],spacing,emptyString);
    }

}