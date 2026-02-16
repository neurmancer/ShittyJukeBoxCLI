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
#define VANISH_CURSOR "\033[?25l"
#define BRING_BACK_THE_CURSOR_FROM_THE_DEAD "\033[?25h" //Yeah I know it doesn't need to be that dramatic nor that long but C'MON WE ARE BUILDING A EMO JUKEBOX, DUH.
#define FREQ 0.445


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

            //Thunder
    "Just a young gun with a quick fuse\n"
    "I was uptight, wanna let loose\n"
    "I was dreaming of bigger things\n"
    "And wanna leave my own life behind\n"
    "Not a yes-sir, not a follower\n"
    "Fit the box, fit the mold\n"
    "Have a seat in the foyer, take a number\n"
    "I was lightning before the thunder\n"
    "Thunder, thunder\n"
    "Thunder, thun-, thunder\n"
    "Thun-thun-thunder, thunder, thunder\n"
    "Thunder, thun-, thunder\n"
    "Thun-thun-thunder, thunder\n"
    "Thunder, feel the thunder\n"
    "Lightning then the thunder\n"
    "Thunder, feel the thunder\n"
    "Lightning then the thunder\n"
    "Thunder, thunder\n"
    "Thunder\n"
    "Kids were laughing in my classes\n"
    "While I was scheming for the masses\n"
    "Who do you think you are?\n"
    "Dreaming 'bout being a big star\n"
    "You say you're basic, you say you're easy\n"
    "You're always riding in the back seat\n"
    "Now I'm smiling from the stage while\n"
    "You were clapping in the nosebleeds\n"
    "Thunder, thunder\n"
    "Thunder, thun-, thunder\n"
    "Thun-thun-thunder, thunder, thunder\n"
    "Thunder, thun-, thunder\n"
    "Thun-thun-thunder, thunder\n"
    "Thunder, feel the thunder\n"
    "Lightning then the thunder\n"
    "Thunder, feel the thunder\n"
    "Lightning then the thunder\n"
    "Thunder, thunder\n"
    "Thunder\n"
    "Thunder, feel the thunder\n"
    "Lightning then the thunder, thunder\n"
    "Thunder, feel the thunder\n"
    "Lightning then the thunder, thunder\n"
    "Thunder, feel the thunder\n"
    "Lightning then the thunder, thunder\n"
    "Thunder, feel the thunder (never give up, never give up)\n"
    "Lightning then the thunder, thunder (never give up, never give up)\n"
    "Thunder, feel the thunder (never give up, never give up)\n"
    "Lightning then the thunder, thunder (never give up, never give up)\n"
    "Thunder, thunder\n"
    "Thunder, thun-, thunder\n"
    "Thun-thun-thunder, thunder, thunder\n"
    "Thunder, thun-, thunder\n"
    "Thun-thun-thunder, thunder\n",




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
    "Pretty rave girl\n"
};


typedef struct {
    int songIndex;
    int iWantEpilepsySeizures;
}songData;


void epilepsy_typewriter(const char* song);
void typewriter(const char* song);
void sigintHandler(int sig);
void asciiPrinter(void);
songData emoInput(void);
songData nightcoreInput(void);
int genreMenu(void);




enum emoSongList {
    BRING_ME_TO_LIFE,
    LITHIUM_EVA,
    MY_IMMORTAL,
    I_HATE_EVERYTHING_ABOUT_C,
    UNFORGIVEN,
    FADE_TO_BLACK,
    HOW_YOU_REMIND_ME,
    LITHIUM_NIRVANA
};

enum ADHDcore {

        
    ANGEL_WITH_A_SHOTGUN,
    ROCKEFELLER_STREET,
    TEETH,
    THUNDER,
    TAKE_A_HINT,
    HOW_TO_BE_A_HB,
    LIGHT_IT_UP,
    PRETTY_RAVE_GIRL
};


typedef void (*WriterFunction)(const char *lyrics);

typedef songData (*MenuFunction)();

WriterFunction writerType[] = {
    typewriter,
    epilepsy_typewriter
};

MenuFunction genre_menus[] = {
    NULL,
    emoInput,
    nightcoreInput
};

char *asciiArt[] = {"  ______________   ","  /  __________  \\"," |  | LITHIUM  |  | "," |  |----------|  | "," |  | SONG B2  |  | "," |  |__________|  |",
    "|  _   ____   _  |", " | (O) |____| (O) |","|  Coin [50c]    |", " |________________|"
};


char *genres[] = {"2000s Emo Music","2000s Nightcore ADHD"};

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
                WriterFunction writer = writerType[selectedSong.iWantEpilepsySeizures];
                switch (genreChoice)
                {
                    case 1:
                        writer(myTherapySession[selectedSong.songIndex-1]);
                        break;
                    case 2:
                        writer(ADHDSongs[selectedSong.songIndex-1]);
                        break;
                    }
            }
            
        }
        else {return(1);} //Menu Error Code (from that point I'll act like every error starting from 1-n to determine shit easier)
        printf(WIPE_TERMINAL);
    }
    
    
    return(0);
}


void sigintHandler(int sig) //Ctrl+C magic
{
    int gettingRickrolledOrNot = (rand() % 9)+1; //Never trust a computer's calculation use bracelets    -Sun Tzu (or Linus Torvalds IDK)
    if (gettingRickrolledOrNot == 3)
    {
        printf(WIPE_TERMINAL);
        usleep(5000);
        epilepsy_typewriter(rickroll); //If you get this ctrl+c ain't saving you. I FUCKING REALIZED IT AFTER 3RD ONE...REDUCED FREQ
    }
    printf(WIPE_TERMINAL);
    printf(BRING_BACK_THE_CURSOR_FROM_THE_DEAD); //Don't worry bro got your cursor back
    exit(0);
}


songData emoInput(void)
{
   
    songData songPrefs;
    songPrefs.songIndex = -1;
    songPrefs.iWantEpilepsySeizures = 0;
    const char *depressed_titles[] = {
        "Bring Me To Life - Evanescence",
        "Lithium - Evanescence",
        "My Immortal - Evanescence",
        "I Hate Everything About You - Three Days Grace",
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
        printf("%d)%s\n",i+1,depressed_titles[i]);
        usleep(55555);
        i++;
    }
    printf(FIX_FONT);
    printf("Please select a song(1-%d) or Ctrl+C to exit:",songCount-1); //-1 Because of NULL -                                                      
    //Program -for some reason- doesn't know how to handle just an 'enter' stroke please don't > /// < 
    if(scanf("%d", &songPrefs.songIndex) != 1 || songPrefs.songIndex < 1 || songPrefs.songIndex > songCount)  //I'll probably make the length dynamic one day but not today at 5.57AM
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { } //To clear stdin buffer ig my bro said trust me 
        printf("Bro...either try not to be a idiot or Delta the fuck out\n"); //Delta as in displacement a 'Naive' way to GTFO...(not physics 101 flex)
        songPrefs.songIndex = -1;
        return(songPrefs);
    } 
    else
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { } 
        char reply;
        printf("Want it to be RGB?(y/n):");
        scanf("%c",&reply);
        if (reply < 'n'){reply += 32;}
        
        if(reply == 'y' || reply == 'n')
        {
            switch (reply)
            {
                case 'y':
                    songPrefs.iWantEpilepsySeizures = 1;
                    return(songPrefs);
                    break;
                
                default:
                    return(songPrefs);
                    break;
            }
        }
        else
        {
            songPrefs.iWantEpilepsySeizures = -1;
            printf("Fuck you.Sincerely...\n");
            return(songPrefs);
        }
        
    }
}


songData nightcoreInput(void)
{
    songData songPrefs = {-1,1};
    char *nightcoreSongs[] = {"Angel with a Shotgun","Rockefeller Street",
            "Teeth","Thunder","Take a Hint","How to be a Heartbreaker","Light it Up","Pretty Rave Girl",NULL
    };
    int songCount = sizeof(nightcoreSongs)/sizeof(nightcoreSongs[0]);
    int i = 0;
    printf(WIPE_TERMINAL BOLD);
    while (nightcoreSongs[i] != NULL)
    {   
        printf("%d)%s\n",i+1,nightcoreSongs[i]);
        usleep(50000);
        i++;    
    }
    printf(FIX_FONT);

    printf("Please select a poison(1-%d) or Ctrl+C to exit:",songCount-1);                                                      
    //Program -for some reason- doesn't know how to handle just an 'enter' stroke please don't > /// < 
    if(scanf("%d", &songPrefs.songIndex) != 1 || songPrefs.songIndex < 1 || songPrefs.songIndex > songCount)  
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { } 
        printf("Bro...either try not to be a idiot or Delta the fuck out\n"); //Delta as in displacement a 'Naive' way to GTFO yeah yeah what ever.
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
    int lengthOfCatalouge = sizeof(genres)/sizeof(genres[0]);
    int genrePick = -1;
    printf(WIPE_TERMINAL);
    printf(BOLD "\t\t\t_-JUST A SHITTY JUKEBOX-_\n" FIX_FONT);
    asciiPrinter();
    printf(BOLD "Genres\n" FIX_FONT);
    printf("1)%s\n",genres[0]);
    printf("2)%s\n\n",genres[1]);

    printf("Select one genre to see its special menu(1-2):");
    if (scanf("%d",&genrePick) != 1 || genrePick < 1 || genrePick > lengthOfCatalouge)
    {   
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { } 
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
        else {usleep(135550);}
        song++;
        color_timer++;
    }
    printf("\033[0m"); // reset colors
    sleep(1);
    printf(WIPE_TERMINAL);
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
    sleep(1);
    printf(WIPE_TERMINAL);
}

void asciiPrinter()
{

    for (int i = 0; i < (sizeof(asciiArt)/sizeof(asciiArt[0])); i++)
    {
        printf("  \t\t\t%s\n",asciiArt[i]);
    }

    printf("\n\n");
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
    |  |  || |____||: Wurlitzer :||____| ||  |  |
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