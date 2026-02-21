#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <sys/wait.h>
#include "src/lyrics.h"


//She Loves Purple,So Do I
//define NORMAL_EMO_OFFSET   13  IT was a pleasure to work with you guys...but I learned structs :/
//define SEIZURE_EMO_OFFSET  53  Goodbye infinite mode ideas via using infinite primes 1000000000000066600000000000001 you'll always live in my heart
#define SECOND 1000000 //Microseconds
#define BOLD "\e[1m"
#define FIX_FONT "\e[0m"
#define WIPE_TERMINAL "\033[H\033[J"
#define WIPE_TOP "\033[2J"
#define GO_HOME "\033[H"
#define VANISH_CURSOR "\033[?25l"
#define BRING_BACK_THE_CURSOR_FROM_THE_DEAD "\033[?25h" //Yeah I know it doesn't need to be that dramatic nor that long but C'MON WE ARE BUILDING A EMO JUKEBOX, DUH.
#define FREQ 0.4
#define SONG_OFFSET 1


typedef struct {
    char *title;
    char *lyrics;
    char *url;
    int songIndex;
    int writerType; 
    float duration;
 
    
    //Will use in future just a prep for now
}songMetaData;




void playShit(char *url);
void clearIBuffer(void);
void bold_typewriter(const char* song);
void epilepsy_typewriter(const char* song);
void typewriter(const char* song);
void sigintHandler(int sig);
void asciiPrinter(void);
songMetaData emoInput(void);
songMetaData nightcoreInput(void);
songMetaData nwobhmInput(void);
int genreMenu(void);




extern char *myTherapySession[];        // myTherapySession
extern char *ADHDSongs[];  // ADHDSongs
extern char *NWOBHSongs[];     // NWOBHSongs


char **LyricsDispatch[] = {
    NULL,
    myTherapySession,
    ADHDSongs,
    NWOBHSongs
};


extern char *myTherapySessionAudio[];
extern char *ADHDSongsAudio[];
extern char *NWOBHMSongsAudio[];


char **AudioDispatch[] = {
    NULL,
    myTherapySessionAudio,
    ADHDSongsAudio,
    NWOBHMSongsAudio
};


extern char *depressed_titles[];
extern char *nightcoreTitles[];
extern char *nwobhmTitles[];

char **TitleDispatch[] = {
    NULL,
    depressed_titles,
    nightcoreTitles,
    nwobhmTitles
};


typedef void (*WriterFunction)(const char *lyrics);

typedef songMetaData (*MenuFunction)();

WriterFunction writerType[] = {
    typewriter,
    epilepsy_typewriter,
    bold_typewriter
};

MenuFunction genre_menus[] = {
    NULL, //Since we start getting input from 1-n basically index shifting...and yeah I started to comment actual important shit to keep track funny ones will be when I enter flow state 
    emoInput,
    nightcoreInput,
    nwobhmInput
};

char *asciiArt[] = {"   ______________   ","  /  __________  \\"," |  | LITHIUM  |  | "," |  |----------|  | "," |  | TEETH    |  | "," |  |__________|  |",
    "|  _   ____   _  |", " | (O) |____| (O) |","|  Coin [50c]    |", " |________________|"
};

char *genres[] = {"2000s Emo Music","2000s Nightcore ADHD","New Wave of British Heavy Metal",NULL};


songMetaData songPrefs = { };

int main(void)
{
    srand(time(NULL));
    setvbuf(stdout,NULL,_IONBF,0);
    signal(SIGINT,sigintHandler);

    int pid = 0;

    while (1) //Yeah I ain't giving up my infinite loop unless you use Ctrl+C to roll rickroll dice
    {
        int genreChoice = genreMenu();
        if (genreChoice) 
        {
            char **selectedLyricsArray = LyricsDispatch[genreChoice]; //ADHD
            char **selectedAudioArray = AudioDispatch[genreChoice];
            char **selectedTitleArray = TitleDispatch[genreChoice];

            MenuFunction genre = genre_menus[genreChoice];
            songMetaData selectedSong = genre(); 
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
                selectedSong.title = selectedTitleArray[selectedSong.songIndex];
                pid = fork();
                if (pid == -1) { return(-13); } //Errno of Love    
                
                if (pid == 0)
                {
                    playShit(selectedSong.url);
                }
                else
                {
                    printf(WIPE_TERMINAL "Current Song:%s\n",selectedSong.title);
                    usleep(SECOND * 1.75);
                    writer(selectedSong.lyrics);
                    wait(NULL);
                }
            }
            printf(WIPE_TOP GO_HOME); //Clear the top and go (1,1)
        }
        else {return(1);} //Menu Error Code (from that point I'll act like every error starting from 1-n to determine shit easier)
        printf(WIPE_TERMINAL);
    }
    
    
    return(0);
}



void sigintHandler(int sig) //Ctrl+C magic
{
    int gettingRickrolledOrNot = (rand() % 4)+1; //Never trust a computer's calculation use bracelets    -Sun Tzu (or Linus Torvalds IDK)
    if (gettingRickrolledOrNot == 3)
    {

        printf(WIPE_TERMINAL);
        usleep(5000);
        int pid = fork();
        if (pid == -1) { printf("Rickroll child fucked up and I can't return value in a void func\n"); } 
        if (pid == 0)
        {
            playShit(rickrollAudio);
        }
        else
        {
            epilepsy_typewriter(rickroll);
            wait(NULL);
        } //If you get this ctrl+c ain't saving you. 
    }
    printf(FIX_FONT WIPE_TERMINAL);
    printf(WIPE_TOP GO_HOME);
    printf(BRING_BACK_THE_CURSOR_FROM_THE_DEAD); //Don't worry bro got your cursor back
    exit(0);
}


songMetaData emoInput(void)
{
   

    songPrefs.songIndex = -1;
    songPrefs.writerType = 0;

    int songCount = sizeof(depressed_titles) / sizeof(depressed_titles[0]);
    int i = 0;
    printf(WIPE_TERMINAL BOLD);
    while (depressed_titles[i] != NULL)
    {
        printf("%d)%s\n",i+SONG_OFFSET,depressed_titles[i]);
        usleep(55555);
        i++;
    }
    printf(FIX_FONT);
    printf("Please select a song(1-%d) or Ctrl+C to exit:",songCount-SONG_OFFSET); //-1 Because of NULL -                                                      
    //Program -for some reason- doesn't know how to handle just an 'enter' stroke please don't > /// < 
    if(scanf("%d", &songPrefs.songIndex) != 1 || songPrefs.songIndex < 1 || songPrefs.songIndex > songCount) 
    {
        clearIBuffer();

        printf("Bro...either try not to be a idiot or Delta the fuck out\n"); //Delta as in displacement a 'Naive' way to GTFO...(not physics 101 flex)
        songPrefs.songIndex = -1;
        return(songPrefs);
    } 

    else
    {
        clearIBuffer();
        songPrefs.title = depressed_titles[songPrefs.songIndex-SONG_OFFSET];
        char reply;
        printf("Want it to be RGB?(y/n):");
        scanf("%c",&reply);
        if (reply < 'n'){reply += 32;}
        
        if(reply == 'y' || reply == 'n')
        {
            switch (reply)
            {
                case 'y':
                    songPrefs.writerType = 1;
                    return(songPrefs);
                    break;
                
                default:
                    return(songPrefs);
                    break;
            }
        }
        else
        {
            songPrefs.writerType = -1;
            printf("Fuck you.Sincerely...\n");
            return(songPrefs);
        }
        
    }
}


songMetaData nightcoreInput(void)
{
    songPrefs.songIndex = -1;
    songPrefs.writerType = 0;

    int songCount = sizeof(nightcoreTitles)/sizeof(nightcoreTitles[0]);
    int i = 0;
    printf(WIPE_TERMINAL BOLD);
    while (nightcoreTitles[i] != NULL)
    {   
        printf("%d)%s\n",i+SONG_OFFSET,nightcoreTitles[i]);
        usleep(50000);
        i++;    
    }

    printf(FIX_FONT);

    printf("Please select a poison(1-%d) or Ctrl+C to exit:",songCount-SONG_OFFSET);   // +1 comes from the NULL                                                    
    //Program -for some reason- doesn't know how to handle just an 'enter' stroke please don't > /// < 
    if(scanf("%d", &songPrefs.songIndex) != 1 || songPrefs.songIndex < 1 || songPrefs.songIndex > songCount)  
    {

        clearIBuffer();

        printf("Bro...either try not to be a idiot or Delta the fuck out\n"); //Delta as in displacement a 'Naive' way to GTFO yeah yeah what ever.
        songPrefs.songIndex = -1;
        return(songPrefs);
    } 
    else
    {
        return(songPrefs);
    }
}

songMetaData nwobhmInput(void)
{
    songPrefs.songIndex = -1;
    songPrefs.writerType = 0;

    int songCount = sizeof(nwobhmTitles)/sizeof(nwobhmTitles[0]);
    int i = 0;
    printf(WIPE_TERMINAL BOLD);
    while (nwobhmTitles[i] != NULL)
    {   
        printf("%d)%s\n",i+SONG_OFFSET,nwobhmTitles[i]);
        usleep(50000);
        i++;    
    }
    printf(FIX_FONT);

    printf("Please select a poison(1-%d) or Ctrl+C to exit:",songCount-SONG_OFFSET);   // +1 comes from the NULL                                                    
    //Still...program doesn't know how to handle just entering please don't.     > /// < 
    if(scanf("%d", &songPrefs.songIndex) != 1 || songPrefs.songIndex < 1 || songPrefs.songIndex > songCount)  
    {
        // NEw shiny fucntion to clear buffer OwO
        clearIBuffer();

        printf("Bro...either try not to be a idiot or Delta the fuck out\n"); 
        songPrefs.songIndex = -1;
        return(songPrefs);
    } 
    else
    {
        return(songPrefs);
    }
}

int genreMenu(void)
{
    int lengthOfCatalouge = (sizeof(genres)/sizeof(genres[0]))- SONG_OFFSET;
    int genrePick = -1;
    printf(WIPE_TERMINAL);
    printf(BOLD "\t\t\t_-JUST A SHITTY JUKEBOX-_\n" FIX_FONT);
    asciiPrinter();
    printf(BOLD "Genres\n" FIX_FONT);
    for (int i = 0; genres[i] != NULL; i++)
    {
        printf("%d)%s\n",i+SONG_OFFSET,genres[i]);
    }
    
    printf("Select one genre to see its special menu(1-%d):",lengthOfCatalouge);
    if (scanf("%d",&genrePick) != 1 || genrePick < 1 || genrePick > lengthOfCatalouge)
    {   
        clearIBuffer();
        printf("Bro how hard can it be to pick 1 or 2? Delta...\n");
        return(0); 
    }
    else
    {
        return(genrePick);
    }
}


void epilepsy_typewriter(const char* song) {
    printf(VANISH_CURSOR);
    printf(WIPE_TERMINAL);
    usleep(250000);
    // hide cursor clear screen and shit.
    long color_timer = 0;
    while (*song != '\0') {
        int r = (sin(FREQ * color_timer * 0.2) * 127) + 128;
        int g = (sin(FREQ * color_timer * 0.2 + 2) * 127) + 128;
        int b = (sin(FREQ * color_timer * 0.2 + 4) * 127) + 128;

        printf("\033[38;2;%d;%d;%dm%c", r, g, b, *song);
        fflush(stdout);
    //Fucking with usleep data old rates : 75000 and 23987
        if (*song == '\n') 
        {
            usleep(99000);
        }
        else {usleep(50210);}
        song++;
        color_timer++;
    }
    printf("\033[0m"); // reset colors
    printf(WIPE_TERMINAL);
    sleep(1);
}

void bold_typewriter(const char* song)
{
    printf(VANISH_CURSOR);
    printf(WIPE_TERMINAL);
    printf(BOLD);
    usleep(250000);
    while (*song != '\0')
    {
        if (*song == '\n')
        {
            usleep(325000); 
            printf("%c",*song);
        }
        else
        {
            printf("%c",*song);
            usleep(12*1200); 
        }
        song++;
    }
    printf(FIX_FONT);
    printf(WIPE_TERMINAL);
    sleep(1);

}

void typewriter(const char* song)
{
    printf(VANISH_CURSOR);
    printf(WIPE_TERMINAL);
    usleep(250000);
    while (*song != '\0')
    {
        if (*song == '\n')
        {
            usleep(320000); //I'm selecting those totally vibe based
            printf("%c",*song);
        }
        else
        {
            printf("%c",*song);
            usleep(8*1440); //1440p montior vibes...
        }
        song++;
    }
    printf(WIPE_TERMINAL);
    sleep(1);

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


void playShit(char *url)
{
    execlp("ffplay", "ffplay", "-nodisp", "-autoexit", "-fflags", "nobuffer", "-flags", "low_delay", "-probesize", "32", "-analyzeduration", "0", "-strict", "experimental", url, "-loglevel", "8", NULL);
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