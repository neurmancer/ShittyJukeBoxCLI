#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include "src/songdata.h"


/*Hiii...me from future I feel so fucked up (mentally) but here we are for therapy that costs nothing but a little price of my sanity */

//She Loves Purple,So Do I
//define NORMAL_EMO_OFFSET   13  IT was a pleasure to work with you guys...but I learned structs :/
//define SEIZURE_EMO_OFFSET  53  Goodbye infinite mode ideas via using infinite primes 1000000000000066600000000000001 you'll always live in my heart
#define SECOND 1000000 //Microseconds
#define BOLD_RED "\e[1;91m"
#define BOLD_BLACK "\e[1;90m"	//and I won't even fucking use this  
#define BOLD_PURPLE "\e[1;95m"

#ifndef FLAG 
#define FLAG 0
#endif

#define FIX_FONT "\e[0m"
#define WIPE_TERMINAL "\033[H\033[J"
#define WIPE_TOP "\033[2J"
#define GO_HOME "\033[H"
#define VANISH_CURSOR "\033[?25l"
#define BRING_BACK_THE_CURSOR_FROM_THE_DEAD "\033[?25h" //Yeah I know it doesn't need to be that dramatic nor that long but C'MON WE ARE BUILDING A EMO JUKEBOX, DUH.
#define FREQ 0.6
#define OFFSET 1
#define GENRE_SIZE sizeof(allGenres)/sizeof(allGenres[0])



typedef struct {

    int printableCharCount;
    int newLineCount;
}LyricsParser;

/*  What we do when C starts bitching? WE ADD MORE GLOBAL VARS !!!!LESSS GO!...or refactor almost every logic shit...yeah that's a pain in the ass - me from 13 hours in the future*/
int BoldWriterColor = -1;


void colorPicker(void);
void clearIBuffer(void);
void bold_typewriter(const char* song,double duration);
void epilepsy_typewriter(const char* song,double duration);
void typewriter(const char* song,double duration);
void sigintHandler(int sig);
void asciiPrinter(void);
void errandBoy(void);
void SkyNet(void);

genreMetaData genreSelection(void);//I named those more obvious since I fucking deleted the wrong fucking function for fuck sake and I gotta rebuild 2 fucking different fucntions again
songMetaData songSelection(genreMetaData selectedGenre); //AAAAAAAAAAAA I am at the edge of fucking either committing arson or suidice AAAAH OwO


int playShit(const char *url);
int safe_scanf(int min,int max);

LyricsParser countPrintables(const char *lyrics);


extern genreMetaData allGenres[];
extern char *roast;






char *Colors[] = {
            NULL,
            "Red",
        	"Green",
        	"Yellow",
        	"Blue",
        	"Purple",
        	"Cyan",
            "White",
        };

typedef void (*WriterFunction)(const char *lyrics,double duration);

typedef songMetaData (*MenuFunction)();

//Poor Man's OOP in C 

WriterFunction writerType[] = {
    typewriter,
    epilepsy_typewriter,
    bold_typewriter
};

extern char *genres[];


char *asciiArt[] = {
    "   ______________   ","  /  __________  \\"," |  | LITHIUM  |  | "," |  |----------|  | "," |  | TEETH    |  | "," |  |__________|  |",
    "|  _   ____   _  |", " | (O) |____| (O) |","|  Coin [50c]    |", " |________________|"
};


char *writerTypes[] = {"Pale White","RGB (seziure guranteed)","Bold X (pick your own color)"};




int main(void)
{

    
    if (!FLAG) {
        printf("Getting the cutting edge version of lyrics and shit don't worry bruh\n\n\n");
        pid_t pid = fork();
        if (pid == -1) {return (-689);}
        if (pid == 0) {
            errandBoy();
        }
        wait(NULL);
        usleep(SECOND*0.75);
        
        SkyNet();
    }

    printf(WIPE_TERMINAL);
    usleep(SECOND*0.25);


    srand(time(NULL));
    setvbuf(stdout,NULL,_IONBF,0);

 

    struct sigaction sa = { 0 };
    sa.sa_handler = &sigintHandler;
    sigaction(SIGINT,&sa,NULL);

    int pid = 0;
    //Yeah I do fucking need IPC FOR SOME REASON 
    //Note for future-self 0->Read      1->Write

    while (1) //Yeah I ain't giving up my infinite loop unless you use Ctrl+C to roll rickroll dice
    {
        printf(WIPE_TERMINAL);

        genreMetaData genre = genreSelection();
        songMetaData song = songSelection(genre);
        pid = fork();

        
        if(pid == -1){return(-1);}
        if (pid == 0) {
            playShit(song.url);
        }
        
        usleep(SECOND*3);
        writerType[song.writerType](song.lyrics,song.duration);
        wait(NULL);
        usleep(SECOND*3);

        printf(WIPE_TERMINAL FIX_FONT);
    }
    

    return(0); //Yeah you can see but can't touch...deal with it
}



void sigintHandler(int sig) //Ctrl+C magic
{
    int gettingRickrolledOrNot = (rand() % 4)+1; //Never trust a computer's calculation use bracelets    -Sun Tzu (or Linus Torvalds IDK)
    if (gettingRickrolledOrNot == 3)
    {

        printf(WIPE_TERMINAL); //Rickroll 3.32
        usleep(5000);
        int pid = fork();
        if (pid == -1) { printf("Rickroll child fucked up and I can't return value in a void func\n");exit(-1987);} //yeah album date of Whenever you need somebody
        if (pid == 0)
        {

            playShit(rickrollAudio);
        }

        else
        {
            epilepsy_typewriter(rickroll,3.32019);
            wait(NULL);
        } //If you get this ctrl+c ain't saving you. 
    }
    printf(FIX_FONT WIPE_TERMINAL);
    printf(WIPE_TOP GO_HOME);
    printf(BRING_BACK_THE_CURSOR_FROM_THE_DEAD); //Don't worry bro got your cursor back
    exit(0);
}



void epilepsy_typewriter(const char* song,double duration) {
    printf(VANISH_CURSOR);
    printf(WIPE_TERMINAL);
    usleep(250000);
    int charCount = 0; 
    LyricsParser parsedLyrics = countPrintables(song);

    int fraction = (int) (duration*100000) % 100000;
    int last_three = fraction % 1000;
    //in seconds
    
    duration *= (SECOND*60);
    if (parsedLyrics.newLineCount == 0) {parsedLyrics.newLineCount = 1;}
    double lineDelay = duration / parsedLyrics.newLineCount;
    double charDelay = duration / parsedLyrics.printableCharCount;



    usleep(SECOND*last_three);
    // hide cursor clear screen and shit.
    long color_timer = 0;
    while (*song != '\0') {
        int r = (sin(FREQ * color_timer * 0.2) * 127) + 128;
        int g = (sin(FREQ * color_timer * 0.2 + 2) * 127) + 128;
        int b = (sin(FREQ * color_timer * 0.2 + 4) * 127) + 128;

        printf("\033[38;2;%d;%d;%dm%c", r, g, b, *song);
        fflush(stdout);


    //Fucking with usleep data old rates : 75000 and 23987
        charCount++;
        if (*song == '\n') 
        {
            if (lineDelay-(charDelay*charCount) > 0)
            {
                usleep(lineDelay-(charDelay*charCount));
            }
            else
            {
                usleep(SECOND*0.95);
            }
            charCount = 0;
        }
        usleep((2*charDelay)/3); 
        song++;
        color_timer++;
    }
    printf(FIX_FONT); // reset colors
    usleep(SECOND*7.5);
}

void bold_typewriter(const char* song,double duration)
{
    LyricsParser parsedLyrics = countPrintables(song);
    int charCount;
    int fraction = (int) (duration*100000) % 100000;
    int last_three = fraction % 1000;
   
    duration *= (SECOND*60);
    double lineDelay = duration / parsedLyrics.newLineCount;
    double charDelay = duration / parsedLyrics.printableCharCount;
    printf(BOLD_RED);

    printf(VANISH_CURSOR);
    printf(WIPE_TERMINAL);
    printf("\e[1;9%dm",BoldWriterColor);
    usleep(last_three * SECOND);
    while (*song != '\0')
    {
        charCount++;
        if (*song == '\n') 
        {
            if (lineDelay-(charDelay*charCount) > 0)
            {
                usleep(lineDelay-(charDelay*charCount));
            }
            else
            {
                usleep(SECOND);
            }
            printf("%c",*song);
            charCount = 0;
        }
        
        else
        {
            printf("%c",*song);
            usleep((2*charDelay)/3); 
        }
        song++;
    }
    printf(FIX_FONT);
    usleep(SECOND*7.5);

}

void typewriter(const char* song,double duration)
{
    LyricsParser parsedLyrics = countPrintables(song);
    int charCount = 0;
    int fraction = (int) (duration*100000) % 100000;
    int last_three = fraction % 1000;
   
    duration *= (SECOND*60);
    double lineDelay = duration / parsedLyrics.newLineCount;
    double charDelay = duration / parsedLyrics.printableCharCount;
    


    printf(VANISH_CURSOR);
    printf(WIPE_TERMINAL);

    usleep(last_three * SECOND);
    
    while (*song != '\0')
    {
        charCount++;
        if (*song == '\n') 
        {
            if (lineDelay-(charDelay*charCount) > 0)
            {
                usleep(lineDelay-(charDelay*charCount));
            }
            else
            {
                usleep(SECOND);
            }
            printf("%c",*song);
            charCount = 0;
        }

        else if (*song == '\n' && *(song+1) == '\n')
        {
            if (lineDelay-(charDelay*charCount) > 0)
            {
                usleep(lineDelay-(charDelay*charCount));
            }
            else
            {
                usleep(SECOND);
            }
            charCount = 0; 
            printf("%c",*song);
            song++;
            printf("%c",*song);
        }
        
        else
        {
            printf("%c",*song);
            usleep((2*charDelay)/3); 
        }
        song++;
    }

    usleep(SECOND*7.5);
    printf(FIX_FONT);
}

void asciiPrinter()
{

    for (int i = 0; i < (sizeof(asciiArt)/sizeof(asciiArt[0])); i++)
    {
        printf("  \t\t\t%s\n",asciiArt[i]);
    }

    printf("\n\n");
}

void clearIBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {/*Nothing to see here just clearing some leftovers*/}
        
    if (c == EOF) {
        clearerr(stdin);   // Yeah that's like wearing two condoms but better be safe than sorry
    }
    
}


int playShit(const char *url)
{
    int errno = execlp("ffplay", "ffplay", "-nodisp", "-autoexit", "-fflags", "nobuffer", "-flags", "low_delay", "-probesize", "32", "-analyzeduration", "0", "-strict", "experimental", url, "-loglevel", "8", NULL);
    if (errno == -1) { return(-7); }
    /*If you were to ask me how tf I came up with those flags...I'd say 'Random bullshit go! besides I read all the fucking man ffplay for 4 hours...'*/
    return(0);
}



LyricsParser countPrintables(const char *lyrics)
{
    LyricsParser parsedLyrics = {0,0};

    while (*lyrics != '\0')
    {
        if (*lyrics != '\n')
        {
            parsedLyrics.printableCharCount++;
        }
        else
        {
            parsedLyrics.newLineCount++;
        }
        lyrics++;
    }
    return(parsedLyrics);
}

genreMetaData genreSelection(void)
{
    
	printf(WIPE_TERMINAL BOLD_RED);
      	
	for(int i = 0;i < GENRE_SIZE;i++)
	{
	    printf("%2d)%s\n",i+1,allGenres[i].genre);
        usleep(SECOND*0.1);
	}
    printf(FIX_FONT "\nPick your poison bruh (1-%d): ",GENRE_SIZE);

    int genreChoice = safe_scanf(1,GENRE_SIZE);
    if (genreChoice == -1) {
        genreMetaData chosenGenre = {.genre=NULL,.songs=NULL,.songCount=-1};
        printf("Either pick a genre or Delta the fuck out bro\n");
        return(chosenGenre);
    }
    return(allGenres[genreChoice-OFFSET]);
}

songMetaData songSelection(genreMetaData genre)
{

    printf(WIPE_TERMINAL BOLD_PURPLE);
    for (int i = 0;i < genre.songCount;i++) {
        printf("%2d)%s\n",i+1,genre.songs[i].title);
        usleep(SECOND*0.13);
    }
    printf(FIX_FONT "What you wanna listen bruh (1-%d):",genre.songCount);
    int songPick = safe_scanf(1,genre.songCount);
    if (songPick == -1) {printf("How hard can it be to pick a fucking number on screen...Just...Delta\n");return(genre.songs[genre.songCount]);}
    
    return(genre.songs[songPick-OFFSET]);
}

void errandBoy(void)
{
    const char *LyricsFromOverSeas = "./src/songdata.h";
    const char *url    = "https://drive.google.com/uc?export=download&id=1rrCXmmoeMtXCK-0lUjjKZTSBRmMuTuOM";

execlp("curl", "curl",
               "-L", "-J", "-S",
               "-o", LyricsFromOverSeas,      // ← THIS IS THE FUCKING IMPORTANT PART
               url,
               NULL);


        perror("curl exec fucked up hard");
        exit(1);

}

void SkyNet(void)
{

    pid_t pid = fork();
    if (pid == -1)
    {
        printf("Something fucked hard\n");
        exit(1);
    }
    if (pid == 0)
    {   
        pid = fork();
        { 
            execlp("cc","cc","ShittyJukeBox.c","-o","ShittyJukeBox","-lm");
            
            perror("Damn bro you wander in C repos and don't have cc shame on you...Delta\n");
            exit(1);
        }
        
    } 
    else {
        wait(NULL);
        usleep(SECOND*1.5);
        execl("./ShittyJukeBox", "ShittyJukeBox", NULL);
    }
}


void colorPicker(void)
{

    if (BoldWriterColor == -1) {

            
            printf(BOLD_RED);
        for (int i = 1; i < sizeof(Colors)/sizeof(Colors[1]);i++) {
            printf("%d)%s\n",i,Colors[i]);
        }
        printf(FIX_FONT);
        printf("Pick your color: ");
        if(scanf("%d",&BoldWriterColor) != 1 || BoldWriterColor < 1 || BoldWriterColor > (sizeof(Colors)/sizeof(Colors[1])-OFFSET))
        {
            printf("You'll get red bitch");
            BoldWriterColor = 1;
        }
    }
    
}

int safe_scanf(int min,int max)
{
    int choice = 0;
    if(scanf(" %d",&choice) != 1 || choice < min || choice > max)
    {
        clearIBuffer();
        return(-1);
    }

    return(choice);
   
}


/*


                       _______
                  _.-'\       /'-._
              _.-'    _\ .-. /_    '-._
           .-'    _.-' |/.-.\| '-._    '-.
         .'    .-'    _||   ||_    '-.    '.
        /    .'    .-' ||___|| '-.    '.    \
       /   .'   .-' _.-'-----'-._ '-.   '.   \
      /   /   .' .-' ~       ~   '-. '.   \   \
     /   /   / .' ~   *   ~     ~   '. \   \   \
    /   /   /.'........    *  ~   *  ~'.\   \   \
    |  /   //:::::::::: ~   _____._____ \\   \  |
    |  |  |/:::::::::::  * '-----------' \|  |  |
   .--.|__||_____________________________||__|.--.
  .'   '----. .-----------------------. .----'   '.
  '.________' |o|==|o|====:====|o|==|o| '________.'
  .'--------. |o|==|o|====:====|o|==|o| .--------'.
  '.________' |o|==|o|====:====|o|==|o| '________.'
  .'--------. |o|==|o|====:====|o|==|o| .--------'.
  '.________' |o|==|o|====:====|o|==|o| '________.'
    |  |  ||  ____ |:| | | | | |:| ____  ||  |  |
    |  |  || |    ||:| | | | | |:||    | ||  |  |
    |  |  || |____||: UwU   OwO :||____| ||  |  |
    |  |  ||  |   /|:| | | | | |:|\   |  ||  |  |
    |  |  ||  |_.` |:| | | | | |:| `._|  ||  |  |
    |  |  || .---.-'-'-'-'-'-'-'-'-.---. ||  |  |
    |  |  || |   |\  /\  / \  /\  /|   | ||  |  |
    |  |  || |   |~\/  \/ ~ \/  \/ |   | ||  |  |
    |  |  || |   | /\ ~/\ ~ /\ ~/\ |   | ||  |  |
    |  |  || |   |/  \/  \ /  \/ ~\|   | ||  |  |
    |  |  || |   |\~ /\~ / \~ /\  /|   | ||  |  |
    |  |  || |   | \/  \/ ~ \/  \/ |   | ||  |  |
    |  |  || |   | /\~ /\ ~ /\ ~/\ |   | ||  |  |
    |  |  || |===|/  \/  .-.  \/  \|===| ||  |  |
    |  |  || |   | ~ /\ ( * ) /\ ~ |   | ||  |  |
    |  |  || |    \ /  \/'-'\/  \ /    | ||  |  |
   /-._|__||  \    \ ~ /\ ~ /\~  /    /  ||__|_.-\
   |-._/__/|   \    './  .-.  \.'    /   |\__\_.-|
   | | |  ||    '._    '-| |-'    _.'    ||  | | |
   | | |  ||      '._    | |    _.'      ||  | | |
   | | |  ||         '-._| |_.-'         ||  | | |
   | | |  ||  __         | |             ||  | | |
   | | |  || O__O        |_|             ||  | | |
   '.|_|__||_____________________________||__|_|.'
    |  |   |-----------------------------|   |  |
    |  |   [_____________________________]   |  |
    |  |   |/                           \|   |  |
    '._|__.'                             '.__|_.'



For the vibe it'll stay here 


*/
