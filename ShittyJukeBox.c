#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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


#define FIX_FONT "\e[0m"
#define WIPE_TERMINAL "\033[H\033[J"
#define WIPE_TOP "\033[2J"
#define GO_HOME "\033[H"
#define VANISH_CURSOR "\033[?25l"
#define BRING_BACK_THE_CURSOR_FROM_THE_DEAD "\033[?25h" //Yeah I know it doesn't need to be that dramatic nor that long but C'MON WE ARE BUILDING A EMO JUKEBOX, DUH.
#define FREQ 0.4
#define SONG_OFFSET 1


struct winsize window;



typedef struct {

    int printableCharCount;
    int newLineCount;
}LyricsParser;

/*  What we do when C starts bitching? WE ADD MORE GLOBAL VARS !!!!LESSS GO!*/
int BoldWriterColor = -1;


void colorPicker(void);
void clearIBuffer(void);
void bold_typewriter(const char* song,double duration);
void epilepsy_typewriter(const char* song,double duration);
void typewriter(const char* song,double duration);
void sigintHandler(int sig);
void sigWINCHHandler(int sig);
void asciiPrinter(void);

songMetaData genreInput(int getGenre);

int playShit(char *url);
int genreMenu(void);

LyricsParser countPrintables(const char *lyrics);

time_t currentTime = 0; 
struct tm *parsedTime;

extern char *myTherapySession[];        // myTherapySession
extern char *ADHDSongs[];  // ADHDSongs
extern char *NWOBHSongs[];     // NWOBHSongs
extern char *WhiteGirlPop[];   //WhiteGirlPop cuz why not? We're happy Emos...

char **LyricsDispatch[] = {
    NULL,
    myTherapySession,
    ADHDSongs,
    NWOBHSongs,
    WhiteGirlPop,
};


extern char *myTherapySessionAudio[];
extern char *ADHDSongsAudio[];
extern char *NWOBHMSongsAudio[];
extern char *WhiteGirlPopAudio[];


char **AudioDispatch[] = {
    NULL,
    myTherapySessionAudio,
    ADHDSongsAudio,
    NWOBHMSongsAudio,
    WhiteGirlPopAudio
};


extern char *depressed_titles[];
extern char *nightcoreTitles[];
extern char *nwobhmTitles[];
extern char *WhiteGirlPopTitles[];


char **TitleDispatch[] = {
    NULL,
    depressed_titles,
    nightcoreTitles,
    nwobhmTitles,
    WhiteGirlPopTitles
};


extern double myTherapySessionLengths[];
extern double ADHDSongsLengths[];
extern double NWOBHSongsLengths[];
extern double WhiteGirlPopLengths[];


double *DurationDispatch[] = {
    NULL,
    myTherapySessionLengths,
    ADHDSongsLengths,
    NWOBHSongsLengths,
    WhiteGirlPopLengths
};

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
    
    srand(time(NULL));
    setvbuf(stdout,NULL,_IONBF,0);

 

    struct sigaction sa = { 0 };
    sa.sa_handler = &sigintHandler;
    sigaction(SIGINT,&sa,NULL);

    struct sigaction sa2 = { 0 };
    sa2.sa_handler = &sigWINCHHandler;
    sigaction(SIGWINCH,&sa2,NULL);

    int pid = 0;
    //Yeah I do fucking need IPC FOR SOME REASON 
    //Note for future-self 0->Read      1->Write

    while (1) //Yeah I ain't giving up my infinite loop unless you use Ctrl+C to roll rickroll dice
    {
        int genreChoice = genreMenu();
        if (genreChoice) 
        {
            char **selectedLyricsArray = LyricsDispatch[genreChoice]; //ADHD
            char **selectedAudioArray = AudioDispatch[genreChoice];
            char **selectedTitleArray = TitleDispatch[genreChoice];
            double *selectedDurationArray = DurationDispatch[genreChoice];
            
            songMetaData selectedSong = genreInput(genreChoice); 

            
            if (selectedSong.songIndex == -1)
            {
                return(2);
            } //As I said that's gonna help me
            
            
            else 
            {
                //I'll optimize this shit too
                WriterFunction writer = writerType[selectedSong.writerType]; //Writer : Assigned 
                selectedSong.songIndex -= SONG_OFFSET; //OFF-Set set here so no off by one from now on in arrays
                selectedSong.lyrics = selectedLyricsArray[selectedSong.songIndex]; //Lyrics gotten
                selectedSong.url = selectedAudioArray[selectedSong.songIndex]; // Audio checked too
                selectedSong.title = selectedTitleArray[selectedSong.songIndex]; //Title drop
                selectedSong.duration = selectedDurationArray[selectedSong.songIndex]; //Duration handled


                pid = fork();
                if (pid == -1) { return(-13); } //Errno of Love    
                
                if (pid == 0)
                {
                    playShit(selectedSong.url);
                }
                else
                {
                    printf(WIPE_TERMINAL "Current Song: %s\n",selectedSong.title);
                    usleep(SECOND * 2.5);
                    writer(selectedSong.lyrics,selectedSong.duration);
                    wait(NULL);
                }
            }
            printf(WIPE_TOP GO_HOME); //Clear the top and go (1,1)
        }
        else {return(1);} //Menu Error Code (from that point I'll act like every error starting from 1-n to determine shit easier)
        BoldWriterColor = -1;
        printf(WIPE_TERMINAL);
        
    }
    

    return(0);
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


songMetaData genreInput(int getGenre)
{
    songMetaData songPrefs = {0};

    songPrefs.songIndex = -1;
    songPrefs.writerType = 0;
    int iter = 0; //I know I fucking used 'i' in somewhere in globabl scope so I don't wanna overwrite it (or I am just trippin')

    char **titleArray = TitleDispatch[getGenre];
    
    printf(WIPE_TERMINAL BOLD_PURPLE);
    while ( titleArray[iter] != NULL) {
        
        printf("%d)%s\n",iter+SONG_OFFSET,titleArray[iter]);
        usleep(50000);
        iter++;  
    }
    int songCount = iter;
    
    const char *intro = "Pick your poison(1-%d) or Ctrl+C to exit:";

    printf(intro,iter);   // +1 comes from the NULL                                                    
    //Still...program doesn't know how to handle just entering please don't.     > /// < 
    if(scanf("%d", &songPrefs.songIndex) != 1 || songPrefs.songIndex < 1 || songPrefs.songIndex > songCount)  
    {
        // New shiny fucntion to clear buffer OwO
        clearIBuffer();

        printf("Bro...either try not to be a idiot or Delta the fuck out\n"); 
        return(songPrefs);
    } 
    else
    {
        switch (getGenre) {
            case 1:
                char rgbSelection = ' ';
                printf("Wanna RGB? (y/n): ");
                if (scanf(" %c",&rgbSelection) != 1) {
                    clearIBuffer();
                    printf("Just y or n...I mean C'mon that wasn't this hard...");
                    songPrefs.songIndex = -1;
                    break;
                }            
                else {
                    rgbSelection = tolower(rgbSelection);
                    if (rgbSelection == 'y') {
                        songPrefs.writerType = 1;
                    }
                    else if (rgbSelection == 'n') {
                        songPrefs.writerType = 0;
                    }

                    else 
                    {
                        printf("Bro... just Y OR N duh...");
                        clearIBuffer();
                    }
                    break;
                }
            
                break;

            case 4:
                int writerArraySize = sizeof(writerTypes) / sizeof(writerTypes[0]);
                printf(WIPE_TERMINAL BOLD_RED);
                for (short i = 0;i < writerArraySize; i++) {
                    printf("%d)%s\n",i+1,writerTypes[i]);
                }
                printf(FIX_FONT);
                printf("Select a writer bruh: ");
                if (scanf(" %d",&songPrefs.writerType) != 1 || songPrefs.writerType < 1 || songPrefs.writerType > writerArraySize ) {
                    printf("Bro just learn to count up to %d or delta the fuck out\n",writerArraySize);
                    songPrefs.writerType = 0;
                    clearIBuffer();
                }
                songPrefs.writerType-=SONG_OFFSET;
                if (songPrefs.writerType == 2) {
                    colorPicker();
                }
                break;

            default:
                songPrefs.writerType = getGenre-SONG_OFFSET;
                break;
        }
    }
    printf(FIX_FONT);
    return(songPrefs);
}



int genreMenu(void)
{
    int lengthOfCatalouge = (sizeof(genres)/sizeof(genres[0]))- SONG_OFFSET;
    int genrePick = -1;
    printf(WIPE_TERMINAL);
    printf(BOLD_RED "\t\t\t_-JUST A SHITTY JUKEBOX-_\n" FIX_FONT);
    asciiPrinter();
    printf(BOLD_RED "Genres\n" FIX_FONT);
    for (int i = 0; genres[i] != NULL; i++)
    {
        printf("%d)%s\n",i+SONG_OFFSET,genres[i]);
    }
    
    printf("Select one genre to see its special menu(1-%d):",lengthOfCatalouge);
    int pidGenre = fork();
    if (pidGenre == -1) { printf("Fork just exploded :/ \n");return (genrePick); }
    if (pidGenre == 0) 
    {
        usleep(SECOND*30);
        kill(getppid(),SIGTERM);
        printf(BRING_BACK_THE_CURSOR_FROM_THE_DEAD FIX_FONT WIPE_TERMINAL);
        playShit(roast);
    }
    else {        
        if (scanf("%d",&genrePick) != 1 || genrePick < 1 || genrePick > lengthOfCatalouge)
        {   
            clearIBuffer();
            printf("Bro how hard can it be to pick 1 or 2? Delta...\n");
            kill(pidGenre, SIGTERM);
            return(0); 
        }
        else
        {
            kill(pidGenre, SIGTERM);
            return(genrePick);
        }
    }
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
        usleep((1*charDelay)/3); 
        song++;
        color_timer++;
    }
    printf(FIX_FONT); // reset colors
    sleep(1);
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
    sleep(1);

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

    sleep(1);
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
    while ((c = getchar()) != '\n' && c != EOF) { } 
}


int playShit(char *url)
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

void sigWINCHHandler(int sig)
{
    ioctl(STDOUT_FILENO,TIOCGWINSZ,&window);
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
        if(scanf("%d",&BoldWriterColor) != 1 || BoldWriterColor < 1 || BoldWriterColor > (sizeof(Colors)/sizeof(Colors[1])-SONG_OFFSET))
        {
            printf("You'll get red bitch");
            BoldWriterColor = 1;
        }
    }
    
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
