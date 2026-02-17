#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
//She Loves Purple,So Do I
//define NORMAL_EMO_OFFSET   13  IT was a pleasure to work with you guys...but I learned structs :/
//define SEIZURE_EMO_OFFSET  53  Goodbye infinite mode ideas via using infinite primes 1000000000000066600000000000001 you'll always live in my heart
#define BOLD "\e[1m"
#define FIX_FONT "\e[0m"
#define WIPE_TERMINAL "\033[H\033[J"
#define WIPE_TOP "\033[2J"
#define GO_HOME "\033[H"
#define VANISH_CURSOR "\033[?25l"
#define BRING_BACK_THE_CURSOR_FROM_THE_DEAD "\033[?25h" //Yeah I know it doesn't need to be that dramatic nor that long but C'MON WE ARE BUILDING A EMO JUKEBOX, DUH.
#define FREQ 0.4
#define SONG_OFFSET 1


char *myTherapySession[] = { "How can you see into my eyes like open doors?\nLeading you down into my core\nWhere I've become so numb\nWithout a soul\nMy spirit's sleeping somewhere cold\nUntil you find it there and lead it back home\nWake me up inside (save me)\nCall my name and save me from the dark (wake me up)\nBid my blood to run (I can't wake up)\nBefore I come undone (save me)\nSave me from the nothing I've become\nNow that I know what I'm without\nYou can't just leave me\nBreathe into me and make me real\nBring (bring) me (me) to life\nWake me up inside (save me)\nCall my name and save me from the dark (wake me up)\nBid my blood to run (I can't wake up)\nBefore I come undone (save me)\nSave me from the nothing I've become\nBring me to life\nI've been living a lie\nThere's nothing inside\nBring me to life\nFrozen (frozen) inside without your touch\nWithout your love darling\nOnly (only) you are the life among the dead\nAll of this time I can't believe I couldn't see\nKept in the dark but you were there in front of me\nI've been sleeping a thousand years it seems\nGot to open my eyes to everything\nWithout a thought without a voice without a soul\nDon't let me die here\n(There must be something more) bring me to life\nWake me up inside (save me)\nCall my name and save me from the dark (wake me up)\nBid my blood to run (I can't wake up)\nBefore I come undone (save me)\nSave me from the nothing I've become\nBring me to life\nI've been living a lie\nThere's nothing inside\nBring me to life\n",
    
        //Lithium Eva
    "Lithium, don't want to lock me up inside\n"
    "Lithium, don't want to forget how it feels without\n"
    "Lithium, I want to stay in love with my sorrow\n"
    "Don't want to let it lay me down this time\n"
    "Drown my will to fly\n"
    "Here in the darkness I know myself\n"
    "Can't break free until I let it go\n"
    "Let me go\n"
    "Darling, I forgive you after all\n"
    "Anything is better than to be alone\n"
    "And in the end I guess I had to fall\n"
    "Always find my place among the ashes\n"
    "I can't hold on to me\n"
    "Wonder what's wrong with me\n"
    "Lithium, don't want to lock me up inside\n"
    "Lithium, don't want to forget how it feels without\n"
    "Lithium, I want to stay in love with my sorrow\n"
    "Don't want to let it lay me down this time\n"
    "Drown my will to fly\n"
    "Here in the darkness I know myself\n"
    "Can't break free until I let it go\n"
    "Let me go\n"
    "Ooh, ooh, ooh\n"
    "Ooh, ooh, ooh\n"
    "Ooh, ooh, ooh\n"
    "Ooh, ooh, ooh\n"
    "Lithium, don't want to lock me up inside\n"
    "Lithium, don't want to forget how it feels without\n"
    "Lithium, I want to stay in love with my sorrow\n"
    "Stay in love with you\n"
    "Oh\n",

        //My Immortal Eva
    "I'm so tired of being here\n"
    "Suppressed by all my childish fears\n"
    "And if you have to leave\n"
    "I wish that you would just leave\n"
    "'Cause your presence still lingers here\n"
    "And it won't leave me alone\n"
    "These wounds won't seem to heal\n"
    "This pain is just too real\n"
    "There's just too much that time cannot erase\n"
    "When you cried I'd wipe away all of your tears\n"
    "When you'd scream I'd fight away all of your fears\n"
    "And I held your hand through all of these years\n"
    "But you still have\n"
    "All of me\n"
    "You used to captivate me\n"
    "By your resonating light\n"
    "Now I'm bound by the life you left behind\n"
    "Your face it haunts\n"
    "My once pleasant dreams\n"
    "Your voice it chased away\n"
    "All the sanity in me\n"
    "These wounds won't seem to heal\n"
    "This pain is just too real\n"
    "There's just too much that time cannot erase\n"
    "When you cried I'd wipe away all of your tears\n"
    "When you'd scream I'd fight away all of your fears\n"
    "And I held your hand through all of these years\n"
    "But you still have\n"
    "All of me\n"
    "I've tried so hard to tell myself that you're gone\n"
    "But though you're still with me\n"
    "I've been alone all along\n"
    "When you cried I'd wipe away all of your tears\n"
    "When you'd scream I'd fight away all of your fears\n"
    "And I held your hand through all of these years\n"
    "But you still have\n"
    "All of me\n"
    "All of me\n"
    "All of me\n",

    //I hate everything aboout you
    "Every time we lie awake\n"
    "After every hit we take\n"
    "Every feeling that I get\n"
    "But I haven't missed you yet\n"
    "Every roommate kept awake\n"
    "By every sigh and scream we make\n"
    "All the feelings that I get\n"
    "But I still don't miss you yet\n"
    "Only when I stop to think about it\n"
    "I hate everything about you\n"
    "Why do I love you?\n"
    "I hate everything about you\n"
    "Why do I love you?\n"
    "Every time we lie awake\n"
    "After every hit we take\n"
    "Every feeling that I get\n"
    "But I haven't missed you yet\n"
    "Only when I stop to think about it\n"
    "I hate everything about you\n"
    "Why do I love you?\n"
    "I hate everything about you\n"
    "Why do I love you?\n"
    "Only when I stop to think about you\n"
    "I know\n"
    "Only when you stop to think about me\n"
    "Do you know?\n"
    "I hate everything about you\n"
    "Why do I love you?\n"
    "You hate everything about me\n"
    "Why do you love me?\n"
    "I hate\n"
    "You hate\n"
    "I hate\n"
    "You love me\n"
    "I hate everything about you\n"
    "Why do I love you?\n",
            //Pain
    "Pain without love\n"
    "Pain, I can't get enough\n"
    "Pain, I like it rough\n"
    "'Cause I'd rather feel pain than nothing at all\n"
    "You're sick of feeling numb\n"
    "You're not the only one\n"
    "I'll take you by the hand\n"
    "And I'll show you a world that you can understand\n"
    "This life is filled with hurt\n"
    "When happiness doesn't work\n"
    "Trust me and take my hand\n"
    "When the lights go out, you'll understand\n"
    "Pain without love\n"
    "Pain, I can't get enough\n"
    "Pain, I like it rough\n"
    "'Cause I'd rather feel pain than nothing at all\n"
    "Pain without love\n"
    "Pain, I can't get enough\n"
    "Pain, I like it rough\n"
    "'Cause I'd rather feel pain than nothing at all\n"
    "Anger and agony are better than misery\n"
    "Trust me, I've got a plan\n"
    "When the lights go up, you'll understand\n"
    "Pain without love\n"
    "Pain, I can't get enough\n"
    "Pain, I like it rough\n"
    "'Cause I'd rather feel pain than nothing at all\n"
    "Pain, without love\n"
    "Pain, I can't get enough\n"
    "Pain, I like it rough\n"
    "'Cause I'd like to feel pain than nothing\n"
    "Rather feel pain\n"
    "I know, I know, I know, I know, I know\n"
    "That you're wounded\n"
    "You know, you know, you know, you know\n"
    "That I'm here to save you\n"
    "You know, you know, you know, you know\n"
    "I'm always here for you\n"
    "I know, I know, I know, I know\n"
    "That you'll thank me later\n"
    "Pain without love\n"
    "Pain, can't get enough\n"
    "Pain, I like it rough\n"
    "'Cause I'd rather feel pain than nothing at all\n"
    "Pain without love\n"
    "Pain, I can't get enough\n"
    "Pain, I like it rough\n"
    "'Cause I'd rather feel pain than nothing at all\n"
    "Pain without love\n"
    "Pain, I can't get enough\n"
    "Pain, I like it rough\n"
    "'Cause I'd rather feel pain than nothing at all\n"
    "Rather feel pain than nothing at all\n"
    "Rather feel pain\n",

        //Boulevard of Broken Dreams

    "I walk a lonely road\n"
    "The only one that I have ever known\n"
    "Don't know where it goes\n"
    "But it's home to me, and I walk alone\n"
    "I walk this empty street\n"
    "On the Boulevard of Broken Dreams\n"
    "Where the city sleeps\n"
    "And I'm the only one, and I walk alone\n"
    "I walk alone, I walk alone\n"
    "I walk alone, I walk a-\n"
    "My shadow's the only one that walks beside me\n"
    "My shallow heart's the only thing that's beating\n"
    "Sometimes, I wish someone out there will find me\n"
    "'Til then, I walk alone\n"
    "Ah-ah, ah-ah, ah-ah, ah-ah\n"
    "Ah-ah, ah-ah, ah-ah\n"
    "I'm walking down the line\n"
    "That divides me somewhere in my mind\n"
    "On the borderline\n"
    "Of the edge, and where I walk alone\n"
    "Read between the lines\n"
    "What's fucked up, and everything's alright\n"
    "Check my vital signs\n"
    "To know I'm still alive, and I walk alone\n"
    "I walk alone, I walk alone\n"
    "I walk alone, I walk a-\n"
    "My shadow's the only one that walks beside me\n"
    "My shallow heart's the only thing that's beating\n"
    "Sometimes, I wish someone out there will find me\n"
    "'Til then, I walk alone\n"
    "Ah-ah, ah-ah, ah-ah, ah-ah\n"
    "Ah-ah, ah-ah, I walk alone, I walk a-\n"
    "I walk this empty street\n"
    "On the Boulevard of Broken Dreams\n"
    "Where the city sleeps\n"
    "And I'm the only one, and I walk a-\n"
    "My shadow's the only one that walks beside me\n"
    "My shallow heart's the only thing that's beating\n"
    "Sometimes, I wish someone out there will find me\n"
    "'Til then, I walk alone\n",


        //Unforgiven (my hardcoding habbits)
    "New blood joins this earth\n"
    "And quickly he's subdued\n"
    "Through constant pained disgrace\n"
    "The young boy learns their rules\n"
    "With time the child draws in\n"
    "This whipping boy done wrong\n"
    "Deprived of all his thoughts\n"
    "The young man struggles on and on he's known\n"
    "A vow unto his own\n"
    "That never from this day\n"
    "His will they'll take away\n"
    "What I've felt\n"
    "What I've known\n"
    "Never shined through in what I've shown\n"
    "Never be\n"
    "Never see\n"
    "Won't see what might have been\n"
    "What I've felt\n"
    "What I've known\n"
    "Never shined through in what I've shown\n"
    "Never free\n"
    "Never me\n"
    "So I dub thee unforgiven\n"
    "They dedicate their lives\n"
    "To running all of his\n"
    "He tries to please them all\n"
    "This bitter man he is\n"
    "Throughout his life the same\n"
    "He's battled constantly\n"
    "This fight he cannot win\n"
    "A tired man they see no longer cares\n"
    "The old man then prepares\n"
    "To die regretfully\n"
    "That old man here is me\n"
    "What I've felt\n"
    "What I've known\n"
    "Never shined through in what I've shown\n"
    "Never be\n"
    "Never see\n"
    "Won't see what might have been\n"
    "What I've felt\n"
    "What I've known\n"
    "Never shined through in what I've shown\n"
    "Never free\n"
    "Never me\n"
    "So I dub thee unforgiven\n"
    "What I've felt\n"
    "What I've known\n"
    "Never shined through in what I've shown\n"
    "Never be\n"
    "Never see\n"
    "Won't see what might have been\n"
    "What I've felt\n"
    "What I've known\n"
    "Never shined through in what I've shown\n"
    "Never free\n"
    "Never me\n"
    "So I dub thee unforgiven\n"
    "Never free\n"
    "Never me\n"
    "So I dub thee unforgiven\n"
    "You labeled me\n"
    "I'll label you\n"
    "So I dub thee unforgiven\n"
    "Never free\n"
    "Never me\n"
    "So I dub thee unforgiven\n"
    "Never free\n"
    "Never me\n"
    "So I dub thee unforgiven\n",
    
            //Fade to black(my sleep schedule)
    "Life it seems will fade away\n"
    "Drifting further every day\n"
    "Getting lost within myself\n"
    "Nothing matters, no one else\n"
    "I have lost the will to live\n"
    "Simply nothing more to give\n"
    "There is nothing more for me\n"
    "Need the end to set me free\n"
    "Things not what they used to be\n"
    "Missing one inside of me\n"
    "Deathly lost, this can't be real\n"
    "Cannot stand this hell I feel\n"
    "Emptiness is filling me\n"
    "To the point of agony\n"
    "Growing darkness taking dawn\n"
    "I was me, but now he's gone\n"
    "No one but me can save myself, but it's too late\n"
    "Now I can't think, think why I should even try\n"
    "Yesterday seems as though it never existed\n"
    "Death greets me warm, now I will just say goodbye\n"
    "Goodbye\n",

    //How you remind me
    "Never made it as a wise man\n"
    "I couldn't cut it as a poor man stealing\n"
    "Tired of living like a blind man\n"
    "I'm sick of sight without a sense of feeling\n"
    "And this is how you remind me\n"
    "This is how you remind me\n"
    "Of what I really am\n"
    "This is how you remind me\n"
    "Of what I really am\n"
    "It's not like you to say sorry\n"
    "I was waiting on a different story\n"
    "This time I'm mistaken\n"
    "For handing you a heart worth breaking\n"
    "And I've been wrong\n"
    "I've been down\n"
    "To the bottom of every bottle\n"
    "These five words in my head\n"
    "Scream \"Are we having fun yet?\"\n"
    "Yeah, yeah, yeah, no no\n"
    "Yeah, yeah, yeah, no no\n"
    "It's not like you didn't know that\n"
    "I said I love you and I swear I still do\n"
    "And it must have been so bad\n"
    "Cause living with him must have damn near killed you\n"
    "And this is how you remind me\n"
    "Of what I really am\n"
    "This is how you remind me\n"
    "Of what I really am\n"
    "It's not like you to say sorry\n"
    "I was waiting on a different story\n"
    "This time I'm mistaken\n"
    "For handing you a heart worth breaking\n"
    "And I've been wrong\n"
    "I've been down\n"
    "To the bottom of every bottle\n"
    "These five words in my head\n"
    "Scream \"Are we having fun yet?\"\n"
    "Yeah, yeah, yeah, no no\n"
    "Yeah, yeah, yeah, no no\n"
    "Yeah, yeah, yeah, no no\n"
    "Yeah, yeah, yeah, no no\n"
    "Never made it as a wise man\n"
    "I couldn't cut it as a poor man stealing\n"
    "And this is how you remind me\n"
    "This is how you remind me\n"
    "This is how you remind me\n"
    "Of what I really am\n"
    "This is how you remind me\n"
    "Of what I really am\n"
    "It's not like you to say sorry\n"
    "I was waiting on a different story\n"
    "This time I'm mistaken\n"
    "For handing you a heart worth breaking\n"
    "And I've been wrong\n"
    "I've been down\n"
    "To the bottom of every bottle\n"
    "These five words in my head\n"
    "Scream \"Are we having fun yet?\"\n"
    "Yeah, yeah\n"
    "Are we having fun yet?\n"
    "Yeah, yeah\n"
    "Are we having fun yet?\n"
    "Yeah, yeah\n"
    "Are we having fun yet?\n"
    "Yeah, yeah\n"
    "No no\n",

    //Lithium - Nirvana

    
    "I'm so happy 'cause today I found my friends\n"
    "They're in my head\n"
    "I'm so ugly but that's okay 'cause so are you\n"
    "We broke our mirrors\n"
    "Sunday morning is everyday for all I care\n"
    "And I'm not scared\n"
    "Light my candles in a daze 'cause I found God\n"
    "Yeah\n"
    "Yeah\n"
    "Yeah\n"
    "Yeah\n"
    "Yeah\n"
    "Yeah\n"
    "I'm so lonely, that's okay, I shaved my head\n"
    "And I'm not sad\n"
    "And just maybe I'm to blame for all I've heard\n"
    "But I'm not sure\n"
    "I'm so excited, I can't wait to meet you there\n"
    "And I don't care\n"
    "I'm so horny, that's okay, my will is good\n"
    "Yeah\n"
    "Yeah\n"
    "Yeah\n"
    "Yeah\n"
    "Yeah\n"
    "Yeah\n"
    "Yeah\n"
    "I like it, I'm not gonna crack\n"
    "I miss you, I'm not gonna crack\n"
    "I love you, I'm not gonna crack\n"
    "I killed you, I'm not gonna crack\n"
    "I like it, I'm not gonna crack\n"
    "I miss you, I'm not gonna crack\n"
    "I love you, I'm not gonna crack\n"
    "I killed you, I'm not gonna crack\n"
    "I'm so happy 'cause today I found my friends\n"
    "They're in my head\n"
    "I'm so ugly, that's okay 'cause so are you\n"
    "We broke our mirrors\n"
    "Sunday morning is everyday for all I care\n"
    "And I'm not scared\n"
    "Light my candles in a daze 'cause I found God\n"
    "Yeah\n"
    "Yeah\n"
    "Yeah\n"
    "Yeah\n"
    "Yeah\n"
    "Yeah\n"
    "Yeah\n"
    "I like it, I'm not gonna crack\n"
    "I miss you, I'm not gonna crack\n"
    "I love you, I'm not gonna crack\n"
    "I killed you, I'm not gonna crack\n"
    "I like it, I'm not gonna crack\n"
    "I miss you, I'm not gonna crack\n"
    "I love you, I'm not gonna crack\n"
    "I killed you, I'm not gonna crack\n"
};


//You'll stay as constant Mr.Astley
const char *rickroll = 
    "We're no strangers to love\n"
    "You know the rules and so do I\n"
    "A full commitment's what I'm thinking of\n"
    "You wouldn't get this from any other guy\n"
    "I just wanna tell you how I'm feeling\n"
    "Gotta make you understand\n"
    "Never gonna give you up\n"
    "Never gonna let you down\n"
    "Never gonna run around and desert you\n"
    "Never gonna make you cry\n"
    "Never gonna say goodbye\n"
    "Never gonna tell a lie and hurt you\n"
    "Never gonna give you up\n"
    "Never gonna let you down\n"
    "Never gonna run around and desert you\n"
    "Never gonna make you cry\n"
    "Never gonna say goodbye\n"
    "Never gonna tell a lie and hurt you\n"
    "We've known each other for so long\n"
    "Your heart's been aching but you're too shy to say it\n"
    "Inside we both know what's been going on\n"
    "We know the game and we're gonna play it\n"
    "And if you ask me how I'm feeling\n"
    "Don't tell me you're too blind to see\n"
    "Never gonna give you up\n"
    "Never gonna let you down\n"
    "Never gonna run around and desert you\n"
    "Never gonna make you cry\n"
    "Never gonna say goodbye\n"
    "Never gonna tell a lie and hurt you\n"
    "Never gonna give you up\n"
    "Never gonna let you down\n"
    "Never gonna run around and desert you\n"
    "Never gonna make you cry\n"
    "Never gonna say goodbye\n"
    "Never gonna tell a lie and hurt you\n";



char *ADHDSongs[] = {
        //Angel With A Shotgun
    "I'm an angel with a shotgun\n"
    "Fighting 'til the war's won\n"
    "I don't care if heaven won't take me back\n"
    "I'll throw away my faith, babe, just to keep you safe\n"
    "Don't you know you're everything I have?\n"
    "And I wanna live, not just survive\n"
    "Tonight\n"
    "We'll be the hunters\n"
    "We'll be the hunted\n"
    "We'll be the ones who never back down\n"
    "We'll be the ones who never give up\n"
    "We'll be the ones who never stop\n"
    "We'll be the ones who never drop\n"
    "We'll be the ones who never fall\n"
    "We'll be the ones who stand tall\n"
    "I'm an angel with a shotgun\n"
    "Fighting 'til the war's won\n"
    "I don't care if heaven won't take me back\n"
    "I'll throw away my faith, babe, just to keep you safe\n"
    "Don't you know you're everything I have?\n"
    "And I wanna live, not just survive\n"
    "Tonight\n"
    "We'll be the hunters\n"
    "We'll be the hunted\n"
    "We'll be the ones who never back down\n"
    "We'll be the ones who never give up\n"
    "We'll be the ones who never stop\n"
    "We'll be the ones who never drop\n"
    "We'll be the ones who never fall\n"
    "We'll be the ones who stand tall\n"
    "I'm an angel with a shotgun\n"
    "Fighting 'til the war's won\n"
    "I don't care if heaven won't take me back\n"
    "I'll throw away my faith, babe, just to keep you safe\n"
    "Don't you know you're everything I have?\n"
    "And I wanna live, not just survive\n"
    "Tonight\n",


            //Rockefeller Street
    "Daylight is fading away, night silhouettes in the sky\n"
    "LED lights are flashing on towers\n"
    "It's Manhattan's magical time\n"
    "Ballerinas dancing the Swan Lake\n"
    "On a river made of diamonds and pearls\n"
    "Everything's a little bit weird now\n"
    "Because tonight it is showtime\n"
    "In the middle of the street life\n"
    "All we celebrate are good times\n"
    "Because tonight it is showtime\n"
    "Come and walk with me\n"
    "1273 down the Rockefeller Street\n"
    "Life is marching on, do you feel that?\n"
    "1273 down the Rockefeller Street\n"
    "Everything is more than surreal\n"
    "Alright, let's go, na zdorovje girls\n"
    "Old school Hollywood stars\n"
    "Party cinderellas are here\n"
    "They come for fun and for fear\n"
    "Because tonight it is showtime\n"
    "In the middle of the street life\n"
    "All we celebrate are good times\n"
    "Because tonight it is showtime\n"
    "Come and walk with me\n"
    "1273 down the Rockefeller Street\n"
    "Life is marching on, do you feel that?\n"
    "1273 down the Rockefeller Street\n"
    "Everything is more than surreal\n"
    "So let's go, na zdorovje girls\n"
    "Come and walk with me\n"
    "1273 down the Rockefeller Street\n"
    "Life is marching on, do you feel that?\n"
    "1273 down the Rockefeller Street\n"
    "Everything is more than surreal\n",


    
                //Teeth
    "Some days, you're the only thing I know\n"
    "Only thing that's burning when the nights grow cold\n"
    "Can't look away, can't look away\n"
    "Beg you to stay, beg you to stay, yeah\n"
    "Sometimes, you're a stranger in my bed\n"
    "Don't know if you love me or you want me dead\n"
    "Push me away, push me away\n"
    "Then beg me to stay, beg me to stay, yeah\n"
    "Call me in the morning to apologize\n"
    "Every little lie gives me butterflies\n"
    "Something in the way you're looking through my eyes\n"
    "Don't know if I'm gonna make it out alive\n"
    "Fight so dirty, but your love so sweet\n"
    "Talk so pretty, but your heart got teeth\n"
    "Late night devil, put your hands on me\n"
    "And never, never, never ever let go\n"
    "Fight so dirty, but your love so sweet\n"
    "Talk so pretty, but your heart got teeth\n"
    "Late night devil, put your hands on me\n"
    "And never, never, never ever let go\n"
    "Some days, you're the best thing in my life\n"
    "Sometimes, you're the worst, you stab me in the back\n"
    "Can't look away, can't look away\n"
    "Beg you to stay, beg you to stay, yeah\n"
    "Call me in the morning to apologize\n"
    "Every little lie gives me butterflies\n"
    "Something in the way you're looking through my eyes\n"
    "Don't know if I'm gonna make it out alive\n"
    "Fight so dirty, but your love so sweet\n"
    "Talk so pretty, but your heart got teeth\n"
    "Late night devil, put your hands on me\n"
    "And never, never, never ever let go\n"
    "Fight so dirty, but your love so sweet\n"
    "Talk so pretty, but your heart got teeth\n"
    "Late night devil, put your hands on me\n"
    "And never, never, never ever let go\n"
    "Blood on my shirt, rose in my hand\n"
    "You're looking at me like you don't know who I am\n"
    "Blood on my shirt, heart in my hand\n"
    "Still beating\n"
    "Fight so dirty, but your love so sweet\n"
    "Talk so pretty, but your heart got teeth\n"
    "Late night devil, put your hands on me\n"
    "And never, never, never ever let go\n"
    "Fight so dirty, but your love so sweet\n"
    "Talk so pretty, but your heart got teeth\n"
    "Late night devil, put your hands on me\n"
    "And never, never, never ever let go\n"
    "Teeth\n"
    "Teeth\n"
    "Teeth\n"
    "Never, never, never ever let go\n",

            //Thunder - Updated
    "Down the river, we're drunk and all of our thumbs went up in the air\n"
    "Sung in the wind and sung to the thunder rolling over and over, we\n"
    "Down the river, we're drunk and all of our thumbs went up in the air\n"
    "Sung in the wind and sung to the thunder rolling over and over\n"
    "We down\n\n"
    "We're getting on fire\n"
    "We're rising up higher\n"
    "The bottle is up\n"
    "We're over the top\n"
    "It's time to get wild\n"
    "The time of our life\n"
    "It's happening now\n"
    "We're out of control\n"
    "Together we're rolling\n"
    "Over and over, we\n\n"
    "Down the river, we're drunk and all of our thumbs went up in the air\n"
    "Sung in the wind and sung to the thunder rolling over and over\n"
    "Down the river, we're drunk and all of our thumbs went up in the air\n"
    "Sung in the wind and sung to the thunder rolling over and over\n"
    "Down the river, we're drunk and all of our thumbs went up in the air\n"
    "Sung in the wind and sung to the thunder rolling over and over\n"
    "Down the river, we're drunk and all of our thumbs went up in the air\n"
    "Sung in the wind and sung to the thunder rolling over and over\n\n"
    "Na-na-na-na\n"
    "Na-na-na-na\n\n"
    "We gotta rock, rock\n"
    "Party till the last shot\n"
    "Fill that jug and raise your cup up\n"
    "We got chug chug\n"
    "Better take it non stop\n"
    "Hey! Come drink it till the last drop\n\n"
    "So please just let us burning down this place\n"
    "And we will end up in the hall of fame\n"
    "Let's get ready for a drinking game\n"
    "So come and sip it to the last drop\n"
    "Over and over we're\n\n"
    "Down the river, we're drunk and all of our thumbs went up in the air\n"
    "Sung in the wind and sung to the thunder rolling over and over\n"
    "Down the river, we're drunk and all of our thumbs went up in the air\n"
    "Sung in the wind and sung to the thunder rolling over and over\n"
    "Down the river, we're drunk and all of our thumbs went up in the air\n"
    "Sung in the wind and sung to the thunder rolling over and over\n"
    "Down the river, we're drunk and all of our thumbs went up in the air\n"
    "Sung in the wind and sung to the thunder rolling over and over\n",




            //Take a hint

 
    "Why am I always hit on by the boys I never like?\n"
    "I can always see 'em coming, from the left or from the right\n"
    "I don't want to be a priss, I'm just try'na be polite\n"
    "But it always seems to bite me in the...\n"
    "Ask me for my number, yeah, you put me on the spot\n"
    "You think that we should hook-up, but I think that we should not\n"
    "You had me at hello, then you opened up your mouth\n"
    "And that is when it started going south\n"
    "Oh!\n"
    "Get your hands off my hips, 'fore I'll punch you in the lips\n"
    "Stop your staring at my... Hey!\n"
    "Take a hint, take a hint\n"
    "No you can't buy me a drink, let me tell you what I think\n"
    "I think you could use a mint\n"
    "Take a hint, take a hint\n"
    "T-take a hint, take a hint\n"
    "I hate you like I love you in the same sentence\n"
    "I can't be the one to fill the space in your bed, yeah\n"
    "I guess you think you still got me\n"
    "I could have you if I wanted\n"
    "But sorry, it ain't happening, oh\n"
    "Oh!\n"
    "Get your hands off my hips, 'fore I'll punch you in the lips\n"
    "Stop your staring at my... Hey!\n"
    "Take a hint, take a hint\n"
    "No you can't buy me a drink, let me tell you what I think\n"
    "I think you could use a mint\n"
    "Take a hint, take a hint\n"
    "What about \"no\" don't you get?\n"
    "So go and tell your friends\n"
    "I'm not really interested\n"
    "It's about time that you're leavin'\n"
    "I'm gonna count to three and\n"
    "Open my eyes and\n"
    "You'll be gone\n"
    "One\n"
    "Get your hands off my...\n"
    "Two\n"
    "Or I'll punch you in the...\n"
    "Three\n"
    "Stop your staring at my... Hey!\n"
    "Take a hint, take a hint\n"
    "I am not your missing link\n"
    "Let me tell you what I think\n"
    "I think you could use a mint\n"
    "Take a hint, take a hint\n"
    "Take a hint, take a hint\n"
    "Woah!\n"
    "Get your hands off my hips, 'fore I'll punch you in the lips\n"
    "Stop your staring at my... Hey!\n"
    "Take a hint, take a hint\n"
    "T-take a hint, take a hint\n",

    
        //How to Be a Heartbreaker

    "Rule number one, is that you gotta have fun\n"
    "But baby when you're done, you gotta be the first to run\n"
    "Rule number two, just don't get attached to\n"
    "Somebody you could lose\n"
    "So le-let me tell you\n"
    "This is how to be a heartbreaker\n"
    "Boys they like a little danger\n"
    "We'll get him falling for a stranger\n"
    "A player, singing I lo-lo-lo love you\n"
    "How to be a heartbreaker\n"
    "Boys they like the look of danger\n"
    "We'll get him falling for a stranger\n"
    "A player, singing I lo-lo-lo love you\n"
    "At least I think I do\n"
    "Cause I lo-lo-lo love you\n"
    "Rule number three, wear your heart on your cheek\n"
    "But never on your sleeve, unless you wanna taste defeat\n"
    "Rule number four, gotta be looking pure\n"
    "Kiss him goodbye at the door, and leave him wanting more, more\n"
    "This is how to be a heartbreaker\n"
    "Boys they like a little danger\n"
    "We'll get him falling for a stranger\n"
    "A player, singing I lo-lo-lo love you\n"
    "How to be a heartbreaker\n"
    "Boys they like the look of danger\n"
    "We'll get him falling for a stranger\n"
    "A player, singing I lo-lo-lo love you\n"
    "At least I think I do\n"
    "Cause I lo-lo-lo love you\n"
    "Girls, we do, whatever it will take\n"
    "Cause girls don't want, we don't want our hearts to break in two\n"
    "So it's better to be fake, can't risk losing\n"
    "In love again, babe\n"
    "This is how to be a heartbreaker\n"
    "Boys they like a little danger\n"
    "We'll get him falling for a stranger\n"
    "A player, singing I lo-lo-lo love you\n"
    "How to be a heartbreaker\n"
    "Boys they like the look of danger\n"
    "We'll get him falling for a stranger\n"
    "A player, singing I lo-lo-lo love you\n"
    "Cause I lo-lo-lo love you\n"
    "At least I think I do\n",
    



            //Light it Up
    "Just like the street lights lit this town\n"
    "Like a fire in a blaze, gotta burn it down\n"
    "Can't be afraid to live this out\n"
    "We got this far don't know how\n"
    "I see danger in your eyes\n"
    "They know we'll burn down the night\n"
    "Coming just like the sunrise\n"
    "You know that we gonna light it up\n"
    "Babe, tonight we gonna light it up\n"
    "Babe, tonight we gonna light it up\n"
    "Just like the sunrise, we gonna light it up\n"
    "Now\n"
    "Light it up, light it up\n"
    "We light it, light it up\n"
    "Just like a spark that's breaking out\n"
    "Can't make it stop, can't turn back now\n"
    "We know that's there no room for doubt\n"
    "No we can't turn back now\n"
    "I see danger in your eyes\n"
    "They know we'll burn down the night\n"
    "Coming just like the sunrise\n"
    "You know that we gonna light it up\n"
    "Babe, tonight we gonna light it up\n"
    "Babe, tonight we gonna light it up\n"
    "Just like the sunrise, we gonna light it up\n"
    "Now\n"
    "Light it up, light it up\n"
    "We light it, light it up\n",

            //Pretty Rave Girl 
    "I know this pretty rave girl\n"
    "Always think about her\n"
    "And when she says hi to me\n"
    "Butterflies go right through me\n"
    "And when I see her dancing\n"
    "Wanna take a chance in\n"
    "Getting a little closer\n"
    "And maybe get to know her\n"
    "Pretty rave girl\n"
    "I know this pretty rave girl\n"
    "Always think about her\n"
    "And when she says hi to me\n"
    "Butterflies go right through me\n"
    "And when I see her dancing\n"
    "Wanna take a chance in\n"
    "Getting a little closer\n"
    "And maybe get to know her\n"
    "Pretty rave girl\n"
    "Pretty rave girl\n"
    "Pretty rave girl\n"
    "Pretty rave girl\n"
    "Pretty rave girl\n"
    "Pretty rave girl\n"
    "I know this pretty rave girl\n"
    "Always think about her\n"
    "And when she says hi to me\n"
    "Butterflies go right through me\n"
    "And when I see her dancing\n"
    "Wanna take a chance in\n"
    "Getting a little closer\n"
    "And maybe get to know her\n"
    "Pretty rave girl\n"
    "Pretty rave girl\n"
    "Pretty rave girl\n"
    "Pretty rave girl\n"
    "Pretty rave girl\n"
    "Pretty rave girl\n",

        //All I Ever Wanted

    "All I ever wanted was to see you smiling\n"
    "I know that I love you, oh baby why don't you see\n"
    "All I ever wanted was to see you smiling\n"
    "All I ever wanted was to make you mine\n"
    "I know that I love you, oh baby why don't you see\n"
    "That all I ever wanted was you and me\n"
    "All I ever wanted was to see you smiling\n"
    "All I ever wanted was to make you mine\n"
    "I know that I love you, oh baby why don't you see\n"
    "That all I ever wanted was you and me\n"
    "Drop the bass\n"
    "I'm so alone\n"
    "Here on my own\n"
    "And I'm waiting for you to come\n"
    "I want to be a part of you\n"
    "Think of all the things we could do\n"
    "And everyday\n"
    "You're in my head\n"
    "I want to have you in my bed\n"
    "You are the one\n"
    "You're in my eyes\n"
    "All I ever wanted in my life\n"
    "All I ever wanted was to see you smiling\n"
    "All I ever wanted was to make you mine\n"
    "I know that I love you, oh baby why don't you see\n"
    "That all I ever wanted was you and me\n"
    "All I ever wanted was to see you smiling\n"
    "All I ever wanted was to make you mine\n"
    "I know that I love you oh baby why don't you see\n"
    "That all I ever wanted was you and me\n"
    "Are you ready?\n"
    "All I ever wanted was to see you smiling\n"
    "All I ever wanted was to make you mine\n"
    "I know that I love you, oh baby why don't you see\n"
    "That all I ever wanted was you and me\n"
    "All I ever wanted\n",

        //Angel of Darkness

    "Angel of Darkness\n"
    "Angel of Darkness\n"
    "The world is in your hand\n"
    "But I will fight until the end\n"
    "Angel of Darkness\n"
    "Angel of Darkness\n"
    "Don't follow your command\n"
    "But I will fight and I will stand\n"
    "When darkness falls pain is all\n"
    "The Angel of Darkness\n"
    "Will leave behind and I will fight\n"
    "The love is lost\n"
    "Beauty and light\n"
    "Have vanished from garden of delight\n"
    "The dreams are gone\n"
    "Midnight has come\n"
    "The darkness is our new kingdom\n"
    "Angel of Darkness\n"
    "Angel of Darkness\n"
    "The world is in your hand\n"
    "But I will fight until the end\n"
    "Angel of Darkness\n"
    "Angel of Darkness\n"
    "Don't follow your command\n"
    "But I will fight and I will stand\n"
    "Hunt goes on deep in the night\n"
    "Time to pray down on your knees\n"
    "You can't hide from me\n"
    "Etern the light\n"
    "Until my last breath I will fight\n"
    "(I will fight, I will fight, I will fight, I will fight)\n"
    "Now realize\n"
    "The stars they die\n"
    "Darkness has fallen in paradise\n"
    "But we'll be strong\n"
    "And we will fight\n"
    "Against the creatures of the night\n"
    "Angel of Darkness\n"
    "Angel of Darkness\n"
    "The world is in your hand\n"
    "But I will fight until the end\n"
    "Angel of Darkness\n"
    "Angel of Darkness\n"
    "Don't follow your command\n"
    "But I will fight and I will stand\n"
    
};


char *NWOBHSongs[] = {

        //Ace of Spades 

    "If you like to gamble\n"
    "I tell you, I'm your man\n"
    "You win some, lose some\n"
    "It's all the same to me\n"
    "The pleasure is to play\n"
    "Makes no difference what you say\n"
    "I don't share your greed\n"
    "The only card I need\n"
    "Is the Ace of Spades\n"
    "The Ace of Spades\n\n"
    "Playing for the high one\n"
    "Dancing with the devil\n"
    "Going with the flow\n"
    "It's all a game to me\n"
    "Seven or eleven\n"
    "Snake eyes watching you\n"
    "Double up or quit\n"
    "Double stake or split\n"
    "The Ace of Spades\n"
    "The Ace of Spades\n\n"
    "You know I'm born to lose\n"
    "And gambling's for fools\n"
    "But that's the way I like it, baby\n"
    "I don't want to live forever\n"
    "And don't forget the joker\n\n"
    "Pushing up the ante\n"
    "I know you got to see me\n"
    "Read 'em and weep\n"
    "The dead man's hand again\n"
    "I see it in your eyes\n"
    "Take one look and die\n"
    "The only thing you see\n"
    "You know it's gonna be\n"
    "The Ace of Spades\n"
    "The Ace of Spades, uh\n",


        //Overkill

    "Only way to feel the noise is when it's good and loud\n"
    "So good I can't believe it, screaming with the crowd\n"
    "Don't sweat it, we'll get it back to you\n"
    "Don't sweat it, we'll get it back to you\n"
    "Overkill, overkill, overkill\n\n"
    "On your feet you feel the beat, it goes straight to your spine\n"
    "Shake your head you must be dead if it don't make you fly\n"
    "Don't sweat it, we'll get it back to you\n"
    "Don't sweat it, we'll get it back to you\n"
    "Overkill, overkill, overkill\n\n"
    "Know your body's made to move, ya feel it in your guts\n"
    "Rock 'n' roll ain't worth the name if it don't make ya strut\n"
    "Don't sweat it, we'll get it back to you\n"
    "Don't sweat it, we'll get it back to you\n"
    "Overkill, overkill, overkill\n", 


        //Too late Too Late

    "I see that nothing's changed\n"
    "Insist on playing games\n"
    "Some waste of time you are\n"
    "And you're so popular\n"
    "Well, this is it, you bitch\n"
    "I got to make a switch\n"
    "You find some other John\n"
    "I know what's going on\n\n"
    "Good move\n"
    "What do I have to lose?\n"
    "Stalemate\n"
    "Too late, too late\n\n"
    "I thought you were for real\n"
    "Just one more rip-off deal\n"
    "Don't give me all that crap\n"
    "I just escaped your trap\n"
    "You think you see the joke\n"
    "But you're just chasing smoke\n"
    "'Cause now the thrill is gone\n"
    "I know what's going on\n\n"
    "Good move\n"
    "What do I have to lose?\n"
    "Stalemate\n"
    "Too late, too late\n\n"
    "Misunderstanding me\n"
    "The way you felt so free\n"
    "I'm gonna jump the gun\n"
    "I'm gonna cut and run\n"
    "Your credibility\n"
    "Don't cut no ice with me\n"
    "You're just a feeble con\n"
    "I know what's going on\n\n"
    "Good move\n"
    "What do I have to lose?\n"
    "Stalemate\n"
    "Too late, too late\n"
    "Too late, too late\n",

        //Am I Evil? (spoiler alert nope I am autistic af)
    "My mother was a witch\nShe was burned alive\n"
    "Thankless little bitch\nFor the tears I cried\n"
    "Take her down now\nDon't want to see her face\n"
    "Blistered and burned\nCan't hide my disgrace\n\n"
    "Twenty-seven, everyone was nice\n"
    "Got to see them\nMake them pay the price\n"
    "See their bodies out on the ice\n"
    "Take my time\n\n"
    "Am I evil?\nYes, I am\n"
    "Am I evil?\nI am, man\nYes, I am\n\n"
    "As I watched my mother die\nI lost my head\n"
    "Revenge now I sought to break with my bread\n"
    "Taking no chances\nYou come with me\n"
    "I'll split you to the bone\nHelp set you free\n\n"
    "Twenty-seven\nEveryone was nice\n"
    "Got to see them\nMake them pay the price\n"
    "See their bodies out on the ice\n"
    "Take my time\n\n"
    "Am I evil?\nYes, I am\n"
    "Am I evil?\nI am, man, yes, I am\n\n"
    "Ohoh!\n\n"
    "On with the action now\nI'll strip your pride\n"
    "I'll spread your blood around\nI'll see you ride\n"
    "Your face is scarred with steel\nWounds deep and neat\n"
    "Like a double dozen before you\nSmells so sweet\n\n"
    "Am I evil?\nYes, I am\n"
    "Am I evil?\nI am, man\n\n"
    "My soul is longing before\nAwait my hell\n"
    "Sent to avenge my mother\nSleep myself\n"
    "My face is long forgotten\nMy face not my own\n"
    "Sweet and timely whore\nTake me home\n\n"
    "Am I evil?\nYes I am\n"
    "Am I evil?\nI am, man, yes, I am\n"
    "Oh\n\n"
    "On with the action now, I'll strip your pride\n"
    "I'll spread your blood around, I'll see you ride\n"
    "Your face is scarred with steel, your spirits dance\n"
    "Like a double dozen before you, no romance\n\n"
    "Am I evil? Yes, I am\n"
    "Am I evil? I am, man\n"
    "Am I evil? Yes, I am\n"
    "Am I evil? I am, man, yes, I am\n",

                //In the Heat of The Night 
    "Steely eyes of a silvery people\n"
    "Walk behind me, with evil intent\n"
    "Up to the tower, I stand alone\n"
    "Stripped of rank before them I'm sent\n\n"
    "What befalls us in the heat of the night?\n"
    "What befalls us in the heat of the night?\n\n"
    "Before long, behind adamantine doors\n"
    "I taste of the wine of their joy\n"
    "Through delirium of a tortured mind\n"
    "Her face gave me strength to this day\n\n"
    "What befalls us in the heat of the night?\n"
    "What befalls us in the heat of the night?\n"
    "In the heat of the night\n\n"
    "But love deceived me, and in my despair\n"
    "I recalled the ring once again\n"
    "Born of Sorrow my cunning revives me\n"
    "And I took them with me enchained\n\n"
    "What befalls us in the heat of the night?\n"
    "What befalls us in the heat of the night?\n"
    "In the heat of the night\n",

        //Denim&Leather

    "Where were you in '79 when the dam began to burst\n"
    "Did you check us out down at the local show\n"
    "Were you wearing denim, wearing leather\n"
    "Did you run down to the front\n"
    "Did you queue for your ticket through the ice and snow\n\n"
    "Denim and Leather\n"
    "Brought us all together\n"
    "It was you that set the spirit free\n"
    "Denim and Leather\n"
    "Brought us all together\n"
    "It was you that set the spirit free\n\n"
    "Did you read the music paper from the back and to the front\n"
    "Did you find out where to see your favorite band\n"
    "Did you listen to the radio every Friday night\n"
    "Did hang around your local record store\n\n"
    "Denim and Leather\n"
    "Brought us all together\n"
    "It was you that set the spirit free\n"
    "Denim and Leather\n"
    "Brought us all together\n"
    "It was you that set the spirit free\n\n"
    "Do you dream of playing guitar or smashing up the drums\n"
    "Maybe you can learn to play the bass\n"
    "You can always be a singer like me and front the band\n"
    "When on the stage we wait at your command\n\n"
    "Denim and Leather\n"
    "Brought us all together\n"
    "It was you that set the spirit free\n"
    "Denim and Leather\n"
    "Brought us all together\n"
    "It was you that set the spirit free\n\n"
    "It was you - that set the spirit free\n"
    "It was you - that put us here today\n"
    "It was you - that filled the concert halls\n"
    "It was you - that set the spirit free\n\n"
    "Denim and Leather\n"
    "Brought us all together\n"
    "It was you that set the spirit free\n"
    "Denim and Leather\n"
    "Brought us all together\n"
    "It was you that set the spirit free\n",
            
    
                //Crusader
    "Crusader, crusader, please take me with you\n"
    "The battle lies far to the east\n"
    "Crusader, crusader, don't leave me alone\n"
    "I want to ride out on your quest\n"
    "I'm waiting, I'm waiting, to stand by your side\n"
    "To fight with you over the sea\n"
    "They're calling, they're calling, I have to be there\n"
    "The holy land has to be free\n\n"
    "Fight the good fight, believe what is right\n"
    "Crusader, the Lord of the Realm\n"
    "Fight the good fight with all your might\n"
    "Crusader, the Lord of the Realm\n\n"
    "We're marching, we're marching, to a land far from home\n"
    "No one can say who'll return\n"
    "For Christendom's sake, we'll take our revenge\n"
    "On the pagans from out of the east\n"
    "We Christians are coming, with swords held on high\n"
    "United by faith and the cause\n"
    "The Saracen heathen will soon taste our steel\n"
    "Our standards will rise 'cross the land\n\n"
    "Fight the good fight, believe what is right\n"
    "Crusader, the Lord of the Realm\n"
    "Fight the good fight with all your might\n"
    "Crusader, the Lord of the Realm\n\n"
    "To battle, to battle, the Saracen hordes\n"
    "We follow the warrior king\n"
    "Onward, ride onward, into the fight\n"
    "We carry the sign of the cross\n"
    "Warlords of England, Knights of the Realm\n"
    "Spilling their blood in the sand\n"
    "Crusader, crusader, the legend is born\n"
    "The future will honour your deeds\n\n"
    "Fight the good fight, believe what is right\n"
    "Crusader, the Lord of the Realm\n"
    "Fight the good fight with all your might\n"
    "Crusader, the Lord of the Realm\n",
            
    
    
            //Fire in The SKYYYY
    "There's a fire in the sky\n"
    "There's a feeling in the air\n"
    "When the ground starts to tremble\n"
    "The first attack is here\n\n"
    "You won't stand a chance\n"
    "When the missiles start to fly\n"
    "They never tell the people\n"
    "'Til the holocaust is here\n\n"
    "When the fire's in the sky\n"
    "You'll be running for your lives\n"
    "When the fire's in the sky\n"
    "Prepare yourselves to die\n\n"
    "Where, where are the shelters?\n"
    "They said they would be there\n"
    "You'll need, need some protection\n"
    "From the radiation flare\n\n"
    "Don't, don't start a panic\n"
    "It won't do you no good\n"
    "You'll just have to wait\n"
    "For the end of all mankind\n\n"
    "If you're near the point of impact\n"
    "There's nowhere you can run\n"
    "If the heatwave don't get you\n"
    "The shockwave surely will\n\n"
    "There won't be no more heroes\n"
    "There's no one left to fight\n"
    "It's the end of our planet\n"
    "We'd commit eternal sight\n\n"
    "When the fire's in the sky\n"
    "You'll be running for your lives\n"
    "When the fire's in the sky\n"
    "Prepare yourselves to die\n\n"
    "When the fire's in the sky\n"
    "You'll be running for your lives\n"
    "When the fire's in the sky\n"
    "Prepare yourselves to die\n"
    "To die, to die, to die, to die\n",

        //The Trooper

    "You'll take my life, but I'll take yours too\n"
    "You'll fire your musket, but I'll run you through\n"
    "So when you're waiting for the next attack\n"
    "You'd better stand, there's no turning back\n\n"
    "The bugle sounds, the charge begins\n"
    "But on this battlefield, no one wins\n"
    "The smell of acrid smoke and horses' breath\n"
    "As I plunge on into certain death, oh, oh\n\n"
    "The horse, he sweats with fear, we break to run\n"
    "The mighty roar of the Russian guns\n"
    "And as we race towards the human wall\n"
    "The screams of pain as my comrades fall\n\n"
    "We hurdle bodies that lay on the ground\n"
    "And the Russians fire another round\n"
    "We get so near, yet so far away\n"
    "We were meant to fight another day, oh, oh\n\n"
    "We get so close, near enough to fight\n"
    "When a Russian gets me in his sights\n"
    "He pulls the trigger and I feel the blow\n"
    "A burst of rounds take my horse below\n\n"
    "And as I lay there gazing at the sky\n"
    "My body's numb and my throat is dry\n"
    "And as I lay forgotten and alone\n"
    "Without a tear, I draw my parting groan, oh, oh\n",


                //Prisoner of Your (Hazel) Eyes

    "When I saw your face\n"
    "I became a prisoner of your eyes\n"
    "And I would do just anything\n"
    "To stay and be with you\n\n"
    "You know there are times\n"
    "When I let myself wonder\n"
    "As I was going under\n"
    "You pulled me back to earth\n\n"
    "Don't you hear me crying?\n"
    "Take me in your arms again\n"
    "Tell me that you're trying\n"
    "Or is our love a lie?\n\n"
    "Love is blind\n"
    "And love deceives you\n"
    "You came along and captured me\n"
    "Now I'm a prisoner of your eyes\n"
    "Trapped in time\n"
    "I cannot leave you\n"
    "I'm just a prisoner of your eyes\n\n"
    "As each day goes by\n"
    "I've given up completely\n"
    "I've locked myself inside your heart\n"
    "And thrown away the key\n\n"
    "Only time will tell\n"
    "If I can live without you\n"
    "Can you see into the future?\n"
    "Will you ever set me free?\n\n"
    "Don't you hear me crying?\n"
    "Take me in your arms again\n"
    "Tell me that you're trying\n"
    "Or is our love a lie?\n\n"
    "Love is blind\n"
    "And love deceives you\n"
    "You came along and captured me\n"
    "Now I'm a prisoner of your eyes\n"
    "Trapped in time\n"
    "I cannot leave you\n"
    "I'm just a prisoner of your eyes\n\n"
    "In this heartache\n"
    "We can try and start again\n"
    "Stop the heartbreak\n"
    "A little time will help to kill the pain\n\n"
    "Don't you hear me crying?\n"
    "Take me in your arms again\n"
    "Tell me that you're trying\n"
    "Or is our love a lie?\n\n"
    "Love is blind\n"
    "And love deceives you\n"
    "You came along and captured me\n"
    "Now I'm a prisoner of your eyes\n"
    "Trapped in time\n"
    "I cannot leave you\n"
    "I'm just a prisoner of your eyes\n\n"
    "Love is blind\n"
    "And love deceives you\n"
    "You came along and captured me\n"
    "Now I'm a prisoner of your eyes\n"
    "Trapped in time\n"
    "I cannot leave you\n"
    "I'm just a prisoner of your eyes\n\n"
    "Love is blind\n"
    "And love deceives you\n"
    "You came along and captured me\n"
    "Now I'm a prisoner of your eyes\n"
    "Trapped in time\n"
    "I cannot leave you\n"
    "I'm just a prisoner of your eyes\n"
    
};


typedef struct {
    int songIndex;
    int writerType; //Yeah no more isSeizuring or shit since I got more typers 
}songData;

void clearIBuffer(void);
void bold_typewriter(const char* song);
void epilepsy_typewriter(const char* song);
void typewriter(const char* song);
void sigintHandler(int sig);
void asciiPrinter(void);
songData emoInput(void);
songData nightcoreInput(void);
songData nwobhmInput(void);
int genreMenu(void);



/*those enums will be mine to navigate through URLs when I got the ffplay working like parallel arrays cuz I can't be bothered by hashmaps and 
    We are not python dict pussies here
*/

enum EmoSongList {
    LIVE_SHELL,             //Yeah over dramatized but c'mon IT'S EMO
    BRING_ME_TO_LIFE,
    LITHIUM_EVA,
    MY_IMMORTAL,
    I_HATE_EVERYTHING_ABOUT_C,
    PAIN,
    BOULEVARD_OF_BROKEN_DREAMS,
    UNFORGIVEN,
    FADE_TO_BLACK,
    HOW_YOU_REMIND_ME,
    LITHIUM_NIRVANA
};

enum ADHDcore {
    BLANK_SHELL,
    ANGEL_WITH_A_SHOTGUN,
    ROCKEFELLER_STREET,
    TEETH,
    THUNDER,
    TAKE_A_HINT,
    HOW_TO_BE_A_HB,
    LIGHT_IT_UP,
    PRETTY_RAVE_GIRL,
    ALL_I_EVER_WANTED,
    ANGEL_OF_DARKNESS
};

enum Bangers {
    BULLET_FOR_MY_VALENTINE,            //Yeah band name as place holder
    ACE_OF_SPADES,
    OVERKILL,
    TOO_LATE,
    AM_I_EVIL,
    IN_THE_HEAT,
    DENIM_AND_LEATHER,
    CRUSADER,
    FIRE_IN_THE_SKY,
    TROOPER,
    PRISONER_OF_YOUR_EYES
};

typedef void (*WriterFunction)(const char *lyrics);

typedef songData (*MenuFunction)();

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





int main(void)
{
    srand(time(NULL));
    setvbuf(stdout,NULL,_IONBF,0);
    signal(SIGINT,sigintHandler);


    while (1) //Yeah I ain't giving up my infinite loop unless you use Ctrl+C to roll rickroll dice
    {
        int genreChoice = genreMenu();
        if (genreChoice) 
        {            
            MenuFunction genre = genre_menus[genreChoice];
            songData selectedSong = genre(); 
            if (selectedSong.songIndex == -1)
            {
                return(2);
            } //As I said that's gonna help me
            else 
            {
                //I'll optimize this shit too
                WriterFunction writer = writerType[selectedSong.writerType];
                selectedSong.songIndex -= SONG_OFFSET;
                switch (genreChoice)
                {
                    case 1:
                        writer(myTherapySession[selectedSong.songIndex]);
                        break;
                    
                    case 2:
                        writer(ADHDSongs[selectedSong.songIndex]);
                        break;
                    
                    case 3:
                        writer(NWOBHSongs[selectedSong.songIndex]);
                        break;
            
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
        epilepsy_typewriter(rickroll); //If you get this ctrl+c ain't saving you. 
    }
    printf(FIX_FONT WIPE_TERMINAL);
    printf(WIPE_TOP GO_HOME);
    printf(BRING_BACK_THE_CURSOR_FROM_THE_DEAD); //Don't worry bro got your cursor back
    exit(0);
}


songData emoInput(void)
{
   
    songData songPrefs;
    songPrefs.songIndex = -1;
    songPrefs.writerType = 0;
    const char *depressed_titles[] = {
        "Bring Me To Life - Evanescence",
        "Lithium - Evanescence",
        "My Immortal - Evanescence",
        "I Hate Everything About You - Three Days Grace",
        "Pain - Three Days Grace",
        "Boulevard of Broken Dreams - Green Day",
        "Unforgiven - Metallica",
        "Fade To Black - Metallica",
        "How You Remind Me - Nickelback",
        "Lithium - Nirvana",
        NULL 
    };
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


    {
        clearIBuffer();
        
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


songData nightcoreInput(void)
{
    songData songPrefs = {-1,1};
    char *nightcoreSongs[] = {"Angel with a Shotgun","Rockefeller Street",
            "Teeth","Thunder","Take a Hint","How to be a Heartbreaker",
            "Light it Up","Pretty Rave Girl","All I Ever Wanted","Angel Of Darkness",
            NULL
    };
    int songCount = sizeof(nightcoreSongs)/sizeof(nightcoreSongs[0]);
    int i = 0;
    printf(WIPE_TERMINAL BOLD);
    while (nightcoreSongs[i] != NULL)
    {   
        printf("%d)%s\n",i+SONG_OFFSET,nightcoreSongs[i]);
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

songData nwobhmInput(void)
{
    songData songPrefs = {-1,2};
    char *nwobhmTitles[] = {
        "Ace of Spades - Motörhead", "Overkill - Motörhead", "Too Late Too Late - Motörhead",
        "Am I Evil - Diamondhead","In The Heat of The Night - Diamondhead","Denim&Leather - Saxon","Crusader - Saxon","Fire In The Sky - Saxon","The Trooper - Iron Maiden","Prisoner Of Your Eyes - Judas Priest", 
        NULL
    }; //Not to future self...Don't forget NULL or you get core dump (like how she dumped you...but bright side:You found the bug under a min since you are used to get dumped)

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

        if (*song == '\n') 
        {
            usleep(75000);
        }
        else {usleep(23987);}
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
            printf("%c\a",*song);
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
            printf("%c\a",*song);
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