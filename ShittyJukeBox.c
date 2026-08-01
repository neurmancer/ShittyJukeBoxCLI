//Fresh start time...

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
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

//Coloring shit
#define RED_BG "\e[41m"
#define RED_AF_BG "\e[0;101m"
#define SHE_LOVES "\e[45m"  //So Do I...



//My shitty defines

#define PAGE 4096

//Fine let me handle termios today at least...



/* ================= OBJECTS =======================*/

typedef struct{
    int x, y;        //Note to self: terminal uses y,x format not x,y and starts at (1,1) not (0, 0) mwah OwO    -Past Me.
    int lastX, lastY;
}Cursor;


/* =============== FUNCTION PROTOTPES =============== */
//Yeah the guy with exuberance and shitty jokes have been partially grown-up...No more in code stand-ups unf...
void typewriter(char *lyrics);
void epilepsyTypewriter(char *lyrics);
void boldTypewriter(char *lyrics);


void confTermios(void);
void fixTerm(void);

/* I/O thingies */

int readInput(void);
int readKey(char *buf, int k);
void testCase(int key, Cursor *cursor);

typedef void (*WriterFunction)(char *lyrics);


/* ================= GLOBAL VARS ==================== (yeah I'll have globals...After all I am still me lol)*/

static struct termios ogTerm,newTerm;

/*
    Now the shit I gotta do... 

    1- Get a buffer reading func to yk...reading buffer
    2- Handle keystrokes (arrow keys to be specific for or you know what?we're going vim mod j and k for up and down )
    3- Do the thing xD
*/


//Yup handled the up down...NICE!

int main(void)
{
    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0);
    confTermios();
    printf(VANISH_CURSOR);
    printf(WIPE_TERMINAL);
    
    WriterFunction writerTypes[] = {
        typewriter,
        epilepsyTypewriter,
        boldTypewriter
    };    


    struct timespec req = { 0 };    //RTFM I won't explain shit this is not the basic repo 
    struct timespec rem = { 0 };    //This is where my toxic emo persona shines
    Cursor cursor = {1, 1, 1, 1 }; 


    int key = 0;
    size_t iter = 0;
    for (int i = 0;i < 5;i++) {
        printf(RED_BG"Test?          \n");
    }
    

    while (key != -1) {
        key = readInput();
        testCase(key, &cursor);
        req.tv_nsec = 0.1*SECOND;
        nanosleep(&req, &rem);
        iter++;
    }
    
    printf("Thingy ended\n");
    char *test = "This is the test\n";
    
    writerTypes[rand() % 3](test);

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

    for (int k = 0;k <= reader; k++) {
        int key = readKey(buf, k);
        if (!key) { continue; }
        lastPressedKey = key;   //That was the bug
    }
    //Feels like that's gonna have fuck tone of edge cases but here we are 
    
    return(lastPressedKey);
}

int readKey(char *buf, int k)
{
    if (buf[k] == 'q') { return(-1);}

    switch (buf[k]) {
        case 'j':
            return(1);  //I mean...I could've made them 1 and -1 want to make the navigation more natural with %d;%dH...but we'll see
        case 'k':
            return(2);
    }

    return(0);
}

void testCase(int key, Cursor *cursor)
{
    switch (key) {
        case -1:
            printf("Quit\n");
            break;

        case 1:
            cursor->lastX = cursor->x;
            cursor->lastY = cursor->y;
            cursor->y -= 1;
            printf(SUMMON_CURSOR RED_AF_BG "            " RESET_FONT,cursor->lastY, cursor->lastX);
            printf(SHE_LOVES SUMMON_CURSOR "Test        " RESET_FONT,cursor->y,cursor->x); 
            
            break;

        case 2:
            cursor->lastX = cursor->x;
            cursor->lastY = cursor->y;
            cursor->y += 1;
            printf(SUMMON_CURSOR RED_AF_BG "                " RESET_FONT ,cursor->lastY,cursor->lastX);
            printf(SHE_LOVES SUMMON_CURSOR "Test        " RESET_FONT,cursor->y,cursor->x);    
            break;
    }
}