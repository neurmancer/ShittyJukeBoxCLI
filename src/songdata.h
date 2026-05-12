#ifndef SONGDATA_H         
#define SONGDATA_H

#include <time.h>
#define FLAG 1

typedef struct {
    const char *title;
    const char *lyrics;
    const char *url;
    double duration;
    int writerType;
} songMetaData;


typedef struct {

    const char *genre;
    songMetaData *songs;
    int songCount;


}genreMetaData;




/* ==================== THERAPY SONGS (Emo) ==================== */
songMetaData myTherapySession[] = {
    {
        .title = "Bring Me To Life - Evanescence",
        .lyrics = "How can you see into my eyes like open doors?\nLeading you down into my core\nWhere I've become so numb\nWithout a soul\nMy spirit's sleeping somewhere cold\nUntil you find it there and lead it back home\nWake me up inside (save me)\nCall my name and save me from the dark (wake me up)\nBid my blood to run (I can't wake up)\nBefore I come undone (save me)\nSave me from the nothing I've become\nNow that I know what I'm without\nYou can't just leave me\nBreathe into me and make me real\nBring (bring) me (me) to life\nWake me up inside (save me)\nCall my name and save me from the dark (wake me up)\nBid my blood to run (I can't wake up)\nBefore I come undone (save me)\nSave me from the nothing I've become\nBring me to life\nI've been living a lie\nThere's nothing inside\nBring me to life\nFrozen (frozen) inside without your touch\nWithout your love darling\nOnly (only) you are the life among the dead\nAll of this time I can't believe I couldn't see\nKept in the dark but you were there in front of me\nI've been sleeping a thousand years it seems\nGot to open my eyes to everything\nWithout a thought without a voice without a soul\nDon't let me die here\n(There must be something more) bring me to life\nWake me up inside (save me)\nCall my name and save me from the dark (wake me up)\nBid my blood to run (I can't wake up)\nBefore I come undone (save me)\nSave me from the nothing I've become\nBring me to life\nI've been living a lie\nThere's nothing inside\nBring me to life\n",
        .url = "https://drive.google.com/uc?export=open&id=1vzC0YESu0JxHRP6hxKO1TS54FOXHw3c5",
        .duration = 3.54018,
        .writerType = 0
    },
    {
        .title = "Lithium - Evanescence",
        .lyrics = "Lithium, don't want to lock me up inside\nLithium, don't want to forget how it feels without\nLithium, I want to stay in love with my sorrow\nDon't want to let it lay me down this time\nDrown my will to fly\nHere in the darkness I know myself\nCan't break free until I let it go\nLet me go\nDarling, I forgive you after all\nAnything is better than to be alone\nAnd in the end I guess I had to fall\nAlways find my place among the ashes\nI can't hold on to me\nWonder what's wrong with me\nLithium, don't want to lock me up inside\nLithium, don't want to forget how it feels without\nLithium, I want to stay in love with my sorrow\nDon't want to let it lay me down this time\nDrown my will to fly\nHere in the darkness I know myself\nCan't break free until I let it go\nLet me go\nOoh, ooh, ooh\nOoh, ooh, ooh\nOoh, ooh, ooh\nOoh, ooh, ooh\nLithium, don't want to lock me up inside\nLithium, don't want to forget how it feels without\nLithium, I want to stay in love with my sorrow\nStay in love with you\nOh\n",
        .url = "https://drive.google.com/uc?export=open&id=1Ea99sesm8Ho-gp08jaySqgwXB_-MR-oj",
        .duration = 3.40000,
        .writerType = 0
    },
    {
        .title = "My Immortal - Evanescence",
        .lyrics = "I'm so tired of being here\nSuppressed by all my childish fears\nAnd if you have to leave\nI wish that you would just leave\n'Cause your presence still lingers here\nAnd it won't leave me alone\nThese wounds won't seem to heal\nThis pain is just too real\nThere's just too much that time cannot erase\nWhen you cried I'd wipe away all of your tears\nWhen you'd scream I'd fight away all of your fears\nAnd I held your hand through all of these years\nBut you still have\nAll of me\nYou used to captivate me\nBy your resonating light\nNow I'm bound by the life you left behind\nYour face it haunts\nMy once pleasant dreams\nYour voice it chased away\nAll the sanity in me\nThese wounds won't seem to heal\nThis pain is just too real\nThere's just too much that time cannot erase\nWhen you cried I'd wipe away all of your tears\nWhen you'd scream I'd fight away all of your fears\nAnd I held your hand through all of these years\nBut you still have\nAll of me\nI've tried so hard to tell myself that you're gone\nBut though you're still with me\nI've been alone all along\nWhen you cried I'd wipe away all of your tears\nWhen you'd scream I'd fight away all of your fears\nAnd I held your hand through all of these years\nBut you still have\nAll of me\nAll of me\nAll of me\n",
        .url = "https://drive.google.com/uc?export=open&id=1_bPQGDr58Sk_gZLP_FgB-0O7RMi5f6sE",
        .duration = 4.03015,
        .writerType = 0
    },
    {
        .title = "I Hate Everything About You - Three Days Grace",
        .lyrics = "Every time we lie awake\nAfter every hit we take\nEvery feeling that I get\nBut I haven't missed you yet\nEvery roommate kept awake\nBy every sigh and scream we make\nAll the feelings that I get\nBut I still don't miss you yet\nOnly when I stop to think about it\nI hate everything about you\nWhy do I love you?\nI hate everything about you\nWhy do I love you?\nEvery time we lie awake\nAfter every hit we take\nEvery feeling that I get\nBut I haven't missed you yet\nOnly when I stop to think about it\nI hate everything about you\nWhy do I love you?\nI hate everything about you\nWhy do I love you?\nOnly when I stop to think about you\nI know\nOnly when you stop to think about me\nDo you know?\nI hate everything about you\nWhy do I love you?\nYou hate everything about me\nWhy do you love me?\nI hate\nYou hate\nI hate\nYou love me\nI hate everything about you\nWhy do I love you?\n",
        .url = "https://drive.google.com/uc?export=open&id=19XGnFxFeJG68XmTbckq5_BpfkIK5udey",
        .duration = 3.40012,
        .writerType = 0
    },
    {
        .title = "Pain - Three Days Grace",
        .lyrics = "Pain without love\nPain, I can't get enough\nPain, I like it rough\n'Cause I'd rather feel pain than nothing at all\nYou're sick of feeling numb\nYou're not the only one\nI'll take you by the hand\nAnd I'll show you a world that you can understand\nThis life is filled with hurt\nWhen happiness doesn't work\nTrust me and take my hand\nWhen the lights go out, you'll understand\nPain without love\nPain, I can't get enough\nPain, I like it rough\n'Cause I'd rather feel pain than nothing at all\nPain without love\nPain, I can't get enough\nPain, I like it rough\n'Cause I'd rather feel pain than nothing at all\nAnger and agony are better than misery\nTrust me, I've got a plan\nWhen the lights go up, you'll understand\nPain without love\nPain, I can't get enough\nPain, I like it rough\n'Cause I'd rather feel pain than nothing at all\nPain, without love\nPain, I can't get enough\nPain, I like it rough\n'Cause I'd like to feel pain than nothing\nRather feel pain\nI know, I know, I know, I know, I know\nThat you're wounded\nYou know, you know, you know, you know\nThat I'm here to save you\nYou know, you know, you know, you know\nI'm always here for you\nI know, I know, I know, I know\nThat you'll thank me later\nPain without love\nPain, can't get enough\nPain, I like it rough\n'Cause I'd rather feel pain than nothing at all\nPain without love\nPain, I can't get enough\nPain, I like it rough\n'Cause I'd rather feel pain than nothing at all\nPain without love\nPain, I can't get enough\nPain, I like it rough\n'Cause I'd rather feel pain than nothing at all\nRather feel pain than nothing at all\nRather feel pain\n",
        .url = "https://drive.google.com/uc?export=open&id=1YdCiss7e9pFfuuhDxpAB91kjxflPOdw5",
        .duration = 3.24009,
        .writerType = 0
    },
    {
        .title = "Boulevard of Broken Dreams - Green Day",
        .lyrics = "I walk a lonely road\nThe only one that I have ever known\nDon't know where it goes\nBut it's home to me, and I walk alone\nI walk this empty street\nOn the Boulevard of Broken Dreams\nWhere the city sleeps\nAnd I'm the only one, and I walk alone\nI walk alone, I walk alone\nI walk alone, I walk a-\nMy shadow's the only one that walks beside me\nMy shallow heart's the only thing that's beating\nSometimes, I wish someone out there will find me\n'Til then, I walk alone\nAh-ah, ah-ah, ah-ah, ah-ah\nAh-ah, ah-ah, ah-ah\nI'm walking down the line\nThat divides me somewhere in my mind\nOn the borderline\nOf the edge, and where I walk alone\nRead between the lines\nWhat's fucked up, and everything's alright\nCheck my vital signs\nTo know I'm still alive, and I walk alone\nI walk alone, I walk alone\nI walk alone, I walk a-\nMy shadow's the only one that walks beside me\nMy shallow heart's the only thing that's beating\nSometimes, I wish someone out there will find me\n'Til then, I walk alone\nAh-ah, ah-ah, ah-ah, ah-ah\nAh-ah, ah-ah, I walk alone, I walk a-\nI walk this empty street\nOn the Boulevard of Broken Dreams\nWhere the city sleeps\nAnd I'm the only one, and I walk a-\nMy shadow's the only one that walks beside me\nMy shallow heart's the only thing that's beating\nSometimes, I wish someone out there will find me\n'Til then, I walk alone\n",
        .url = "https://drive.google.com/uc?export=open&id=1XHR_qNuP2VCISxJrhnzFqrNKhS8XpfeN",
        .duration = 3.43012,
        .writerType = 0
    },
    {
        .title = "Unforgiven - Metallica",
        .lyrics = "New blood joins this earth\nAnd quickly he's subdued\nThrough constant pained disgrace\nThe young boy learns their rules\nWith time the child draws in\nThis whipping boy done wrong\nDeprived of all his thoughts\nThe young man struggles on and on he's known\nA vow unto his own\nThat never from this day\nHis will they'll take away\nWhat I've felt\nWhat I've known\nNever shined through in what I've shown\nNever be\nNever see\nWon't see what might have been\nWhat I've felt\nWhat I've known\nNever shined through in what I've shown\nNever free\nNever me\nSo I dub thee unforgiven\nThey dedicate their lives\nTo running all of his\nHe tries to please them all\nThis bitter man he is\nThroughout his life the same\nHe's battled constantly\nThis fight he cannot win\nA tired man they see no longer cares\nThe old man then prepares\nTo die regretfully\nThat old man here is me\nWhat I've felt\nWhat I've known\nNever shined through in what I've shown\nNever be\nNever see\nWon't see what might have been\nWhat I've felt\nWhat I've known\nNever shined through in what I've shown\nNever free\nNever me\nSo I dub thee unforgiven\nWhat I've felt\nWhat I've known\nNever shined through in what I've shown\nNever be\nNever see\nWon't see what might have been\nWhat I've felt\nWhat I've known\nNever shined through in what I've shown\nNever free\nNever me\nSo I dub thee unforgiven\nNever free\nNever me\nSo I dub thee unforgiven\nYou labeled me\nI'll label you\nSo I dub thee unforgiven\nNever free\nNever me\nSo I dub thee unforgiven\nNever free\nNever me\nSo I dub thee unforgiven\n",
        .url = "https://drive.google.com/uc?export=open&id=18G2SYGLKntyhzgLXZJ0LJdmkNkxpJf5E",
        .duration = 6.10055,
        .writerType = 0
    },
    {
        .title = "Fade To Black - Metallica",
        .lyrics = "Life it seems will fade away\nDrifting further every day\nGetting lost within myself\nNothing matters, no one else\nI have lost the will to live\nSimply nothing more to give\nThere is nothing more for me\nNeed the end to set me free\nThings not what they used to be\nMissing one inside of me\nDeathly lost, this can't be real\nCannot stand this hell I feel\nEmptiness is filling me\nTo the point of agony\nGrowing darkness taking dawn\nI was me, but now he's gone\nNo one but me can save myself, but it's too late\nNow I can't think, think why I should even try\nYesterday seems as though it never existed\nDeath greets me warm, now I will just say goodbye\nGoodbye\n",
        .url = "https://drive.google.com/uc?export=open&id=1CpFLSZwQliL9uDfYplewixTY4iuvlid4",
        .duration = 4.57089,
        .writerType = 0
    },
    {
        .title = "How You Remind Me - Nickelback",
        .lyrics = "Never made it as a wise man\nI couldn't cut it as a poor man stealing\nTired of living like a blind man\nI'm sick of sight without a sense of feeling\nAnd this is how you remind me\nThis is how you remind me\nOf what I really am\nThis is how you remind me\nOf what I really am\nIt's not like you to say sorry\nI was waiting on a different story\nThis time I'm mistaken\nFor handing you a heart worth breaking\nAnd I've been wrong\nI've been down\nTo the bottom of every bottle\nThese five words in my head\nScream \"Are we having fun yet?\"\nYeah, yeah, yeah, no no\nYeah, yeah, yeah, no no\nIt's not like you didn't know that\nI said I love you and I swear I still do\nAnd it must have been so bad\nCause living with him must have damn near killed you\nAnd this is how you remind me\nOf what I really am\nThis is how you remind me\nOf what I really am\nIt's not like you to say sorry\nI was waiting on a different story\nThis time I'm mistaken\nFor handing you a heart worth breaking\nAnd I've been wrong\nI've been down\nTo the bottom of every bottle\nThese five words in my head\nScream \"Are we having fun yet?\"\nYeah, yeah, yeah, no no\nYeah, yeah, yeah, no no\nYeah, yeah, yeah, no no\nYeah, yeah, yeah, no no\nNever made it as a wise man\nI couldn't cut it as a poor man stealing\nAnd this is how you remind me\nThis is how you remind me\nThis is how you remind me\nOf what I really am\nThis is how you remind me\nOf what I really am\nIt's not like you to say sorry\nI was waiting on a different story\nThis time I'm mistaken\nFor handing you a heart worth breaking\nAnd I've been wrong\nI've been down\nTo the bottom of every bottle\nThese five words in my head\nScream \"Are we having fun yet?\"\nYeah, yeah\nAre we having fun yet?\nYeah, yeah\nAre we having fun yet?\nYeah, yeah\nAre we having fun yet?\nYeah, yeah\nNo no\n",
        .url = "https://drive.google.com/uc?export=open&id=1fPDH35PpZqSxNUxW0Pxrakv5iSvKi8u1",
        .duration = 3.33004,
        .writerType = 0
    },
    {
        .title = "Lithium - Nirvana",
        .lyrics = "I'm so happy 'cause today I found my friends\nThey're in my head\nI'm so ugly but that's okay 'cause so are you\nWe broke our mirrors\nSunday morning is everyday for all I care\nAnd I'm not scared\nLight my candles in a daze 'cause I found God\nYeah\nYeah\nYeah\nYeah\nYeah\nYeah\nI'm so lonely, that's okay, I shaved my head\nAnd I'm not sad\nAnd just maybe I'm to blame for all I've heard\nBut I'm not sure\nI'm so excited, I can't wait to meet you there\nAnd I don't care\nI'm so horny, that's okay, my will is good\nYeah\nYeah\nYeah\nYeah\nYeah\nYeah\nYeah\nI like it, I'm not gonna crack\nI miss you, I'm not gonna crack\nI love you, I'm not gonna crack\nI killed you, I'm not gonna crack\nI like it, I'm not gonna crack\nI miss you, I'm not gonna crack\nI love you, I'm not gonna crack\nI killed you, I'm not gonna crack\nI'm so happy 'cause today I found my friends\nThey're in my head\nI'm so ugly, that's okay 'cause so are you\nWe broke our mirrors\nSunday morning is everyday for all I care\nAnd I'm not scared\nLight my candles in a daze 'cause I found God\nYeah\nYeah\nYeah\nYeah\nYeah\nYeah\nYeah\nI like it, I'm not gonna crack\nI miss you, I'm not gonna crack\nI love you, I'm not gonna crack\nI killed you, I'm not gonna crack\nI like it, I'm not gonna crack\nI miss you, I'm not gonna crack\nI love you, I'm not gonna crack\nI killed you, I'm not gonna crack\n",
        .url = "https://drive.google.com/uc?export=open&id=1C-IcedC0m2aRJnGNpLztS-960AE4QIP4",
        .duration = 4.10009,
        .writerType = 0
    },
    {
        .title = "Still Loving You - Scorpions",
        .lyrics = "Time, it needs time\nTo win back your love again\nI will be there\nI will be there\nLove, only love\nCan bring back your love someday\nI will be there\nI will be there\nFight, babe, I'll fight\nTo win back your love again\nI will be there\nI will be there\nLove, only love\nCan bring down the wall someday\nI will be there\nI will be there\nIf we'd go again\nAll the way from the start\nI would try to change\nThings that killed our love\nYour pride has built a wall\nSo strong that I can't get through\nIs there really no chance\nTo start once again?\nI'm loving you\nAnd try, baby, try\nTo trust in my love again\nI will be there\nI will be there\nLove, our love\nShouldn't be thrown away\nI will be there\nI will be there\nIf we'd go again\nAll the way from the start\nI would try to change\nThings that killed our love\nYour pride has built a wall\nSo strong that I can't get through\nIs there really no chance\nTo start once again?\nIf we'd go again\nAll the way from the start\nI would try to change\nThings that killed our love\nYes, I've hurt your pride\nAnd I know what you've been through\nYou should give me a chance\nThis can't be the end\nI'm still loving you\nI'm still loving you\nI'm still loving you, I need your love\nI'm still loving you\nStill loving you, baby\nI'm still loving you\nI need your love\nI'm still loving you\nI need your love\nI need your love\nI need your love\nStill loving you\n",
        .url = "https://drive.google.com/uc?export=open&id=10XGgVvBJJkP6XLux0OhKcqzENPZ07nF0",
        .duration = 6.43022,
        .writerType = 0
    },
    {
        .title = "Don't Fear The Reaper - HIM (Blue Öyster Cult cover)",
        .lyrics = "All our times have come\nHere, but now they're gone\nSeasons don't fear the Reaper\nNor do the wind, the sun, or the rain\nCome on, baby\n(Don't fear the Reaper) Baby, take my hand\n(Don't fear the Reaper) We'll be able to fly\n(Don't fear the Reaper) Baby, I'm your man\nLa, la, la, la, la\nLa, la, la, la, la\nValentine is done\nHere, but now they're gone\nRomeo and Juliet\nAre together in eternity (Romeo and Juliet)\n40, 000 men and women everyday (like Romeo and Juliet)\n40, 000 men and women everyday (redefine happiness)\nAnother 40, 000 coming everyday\nCome on, baby\n(Don't fear the Reaper) Baby, take my hand\n(Don't fear the Reaper) We'll be able to fly\n(Don't fear the Reaper) Baby, I'm your man\nLa, la, la, la, la\nLa, la, la, la, la\nLove of two is one\nHere, but now they're gone\nCame the last night of sadness\nAnd it was clear she couldn't go on\nAnd the door was open and the wind appeared\nThe candles blew and then disappeared\nThe curtains flew and then he appeared\nCome on, baby\n(And she had no fear) And she ran to him\n(Then they started to fly) They looked backward and said goodbye\n(She had become like they are) She had taken his hand\n(She had become like they are) Come on, baby\n(Don't fear the Reaper)\n",
        .url = "https://drive.google.com/uc?export=open&id=1fJNOgM3PMav_vf0MwhOR7ZVnYDfH-dLh",
        .duration = 6.25020,
        .writerType = 0
    },
    {
        .title = "I Don't Love You - My Chemical Romance",
        .lyrics = "Well, when you go\nDon't ever think I'll make you try to stay\nAnd maybe when you get back\nI'll be off to find another way\nAnd after all this time that you still owe\nYou're still a good-for-nothing, I don't know\nSo take your gloves and get out\nBetter get out while you can\nWhen you go, and would you even turn to say\n\"I don't love you like I did yesterday\"?\nSometimes I cry so hard from pleading\nSo sick and tired of all the needless beating\nBut baby, when they knock you down and out\nIs where you oughta stay\nAnd after all the blood that you still owe\nAnother dollar's just another blow\nSo fix your eyes and get up\nBetter get up while you can\nWhoa, whoa, whoa-whoa, whoa-whoa\nWhen you go, and would you even turn to say\n\"I don't love you like I did yesterday\"?\nWell, come on, come on\nWhen you go, would you have the guts to say\n\"I don't love you like I loved you yesterday\"?\nI don't love you like I loved you yesterday\nI don't love you like I loved you yesterday\n",
        .url = "https://drive.google.com/uc?export=open&id=1lxlXqaJ1QoFO4ERzeTKVGtiBfijFkuos",
        .duration = 3.58027,
        .writerType = 0
    },
    {
        .title = "I Never Told You What I Do For A Living - My Chemical Romance",
        .lyrics = "Stay out of the light\nOr the photograph that I gave you\nYou can say a prayer if you need to\nOr just get in line and I'll grieve you\nCan I meet you, alone\nAnother night and I'll see you\nAnother night and I'll be you\nSome other way to continue\nTo hide my face\nAnother knife in my hands\nA stain that never comes off the sheets\nClean me off\nI'm so dirty babe\nThe kind of dirty where the water never cleans off the clothes\nI keep a book of the names and those\nOnly go so far 'til you bury them\nSo deep and down we go\nTouched by angels, though I fall out of grace\nI did it all so maybe I'd live this every day\nAnother knife in my hands\nA stain that never comes off the sheets\nClean me off\nI'm so dirty babe\nIt ain't the money and it sure as hell ain't just for the fame\nIt's for the bodies I claim and lose\nOnly go so far 'til you bury them\nSo deep and down we go\nDown\nAnd down we go\nAnd down we go\nAnd down we go\nAnd we all fall down\nI tried\nI tried\nAnd we'll all dance alone to the tune of your death\nWe'll love again, we'll laugh again\nAnd it's better off this way\nAnd never again, and never again\nThey gave us two shots to the back of the head\nAnd we're all dead now\nWell never again, and never again\nThey gave us two shots to the back of the head\nAnd we're all dead now\nWell I tried\nOne more night\nOne more night\nWell I'm laughin' out, cryin' out, laughin' out loud\nI tried, well I tried, well I tried\n'Cause I tried, but I lied\nI lied\nI tried\nI tried\nI tried, well\nAnd we'll love again, we'll laugh again\nWe'll cry again and we'll dance again\nAnd it's better off this way\nSo much better off this way\nI can't clean the blood off the sheets in my bed\nAnd never again, and never again\nThey gave us two shots to the back of the head\nAnd we're all dead now\n",
        .url = "https://drive.google.com/uc?export=open&id=1J1-bcyZy12nECoRbonQIaEqe6RsSxkqL",
        .duration = 3.53007,
        .writerType = 0
    },
    { .title = NULL, .lyrics = NULL, .url = NULL, .duration = 0, .writerType = 0 }
};


/* ==================== ADHD / NIGHTCORE SONGS ==================== */
songMetaData adhdSongs[] = {
    {
        .title = "Angel With A Shotgun",
        .lyrics = "I'm an angel with a shotgun\nFighting 'til the war's won\nI don't care if heaven won't take me back\nI'll throw away my faith, babe, just to keep you safe\nDon't you know you're everything I have?\nAnd I wanna live, not just survive\nTonight\nWe'll be the hunters\nWe'll be the hunted\nWe'll be the ones who never back down\nWe'll be the ones who never give up\nWe'll be the ones who never stop\nWe'll be the ones who never drop\nWe'll be the ones who never fall\nWe'll be the ones who stand tall\nI'm an angel with a shotgun\nFighting 'til the war's won\nI don't care if heaven won't take me back\nI'll throw away my faith, babe, just to keep you safe\nDon't you know you're everything I have?\nAnd I wanna live, not just survive\nTonight\nWe'll be the hunters\nWe'll be the hunted\nWe'll be the ones who never back down\nWe'll be the ones who never give up\nWe'll be the ones who never stop\nWe'll be the ones who never drop\nWe'll be the ones who never fall\nWe'll be the ones who stand tall\nI'm an angel with a shotgun\nFighting 'til the war's won\nI don't care if heaven won't take me back\nI'll throw away my faith, babe, just to keep you safe\nDon't you know you're everything I have?\nAnd I wanna live, not just survive\nTonight\n",
        .url = "https://drive.google.com/uc?export=open&id=1QmFON5P2dDa0_0-Qwn_l6iPlNsdhyYQc",
        .duration = 3.12008,
        .writerType = 1
    },
    {
        .title = "Rockefeller Street",
        .lyrics = "Daylight is fading away, night silhouettes in the sky\nLED lights are flashing on towers\nIt's Manhattan's magical time\nBallerinas dancing the Swan Lake\nOn a river made of diamonds and pearls\nEverything's a little bit weird now\nBecause tonight it is showtime\nIn the middle of the street life\nAll we celebrate are good times\nBecause tonight it is showtime\nCome and walk with me\n1273 down the Rockefeller Street\nLife is marching on, do you feel that?\n1273 down the Rockefeller Street\nEverything is more than surreal\nAlright, let's go, na zdorovje girls\nOld school Hollywood stars\nParty cinderellas are here\nThey come for fun and for fear\nBecause tonight it is showtime\nIn the middle of the street life\nAll we celebrate are good times\nBecause tonight it is showtime\nCome and walk with me\n1273 down the Rockefeller Street\nLife is marching on, do you feel that?\n1273 down the Rockefeller Street\nEverything is more than surreal\nSo let's go, na zdorovje girls\nCome and walk with me\n1273 down the Rockefeller Street\nLife is marching on, do you feel that?\n1273 down the Rockefeller Street\nEverything is more than surreal\n",
        .url = "https://drive.google.com/uc?export=open&id=1mFC4ZYCDnyHU7kAkYi_ujsKwZGU_imBN",
        .duration = 2.41013,
        .writerType = 1
    },
    {
        .title = "Teeth",
        .lyrics = "Some days, you're the only thing I know\nOnly thing that's burning when the nights grow cold\nCan't look away, can't look away\nBeg you to stay, beg you to stay, yeah\nSometimes, you're a stranger in my bed\nDon't know if you love me or you want me dead\nPush me away, push me away\nThen beg me to stay, beg me to stay, yeah\nCall me in the morning to apologize\nEvery little lie gives me butterflies\nSomething in the way you're looking through my eyes\nDon't know if I'm gonna make it out alive\nFight so dirty, but your love so sweet\nTalk so pretty, but your heart got teeth\nLate night devil, put your hands on me\nAnd never, never, never ever let go\nFight so dirty, but your love so sweet\nTalk so pretty, but your heart got teeth\nLate night devil, put your hands on me\nAnd never, never, never ever let go\nSome days, you're the best thing in my life\nSometimes, you're the worst, you stab me in the back\nCan't look away, can't look away\nBeg you to stay, beg you to stay, yeah\nCall me in the morning to apologize\nEvery little lie gives me butterflies\nSomething in the way you're looking through my eyes\nDon't know if I'm gonna make it out alive\nFight so dirty, but your love so sweet\nTalk so pretty, but your heart got teeth\nLate night devil, put your hands on me\nAnd never, never, never ever let go\nFight so dirty, but your love so sweet\nTalk so pretty, but your heart got teeth\nLate night devil, put your hands on me\nAnd never, never, never ever let go\nBlood on my shirt, rose in my hand\nYou're looking at me like you don't know who I am\nBlood on my shirt, heart in my hand\nStill beating\nFight so dirty, but your love so sweet\nTalk so pretty, but your heart got teeth\nLate night devil, put your hands on me\nAnd never, never, never ever let go\nFight so dirty, but your love so sweet\nTalk so pretty, but your heart got teeth\nLate night devil, put your hands on me\nAnd never, never, never ever let go\nTeeth\nTeeth\nTeeth\nNever, never, never ever let go\n",
        .url = "https://drive.google.com/uc?export=open&id=1OW6MmzzRFk-fN7ztpcm-Zwm3LnB8KYJO",
        .duration = 2.53007,
        .writerType = 1
    },
    {
        .title = "Thunder",
        .lyrics = "Down the river, we're drunk and all of our thumbs went up in the air\nSung in the wind and sung to the thunder rolling over and over, we\nDown the river, we're drunk and all of our thumbs went up in the air\nSung in the wind and sung to the thunder rolling over and over\nWe down\n\nWe're getting on fire\nWe're rising up higher\nThe bottle is up\nWe're over the top\nIt's time to get wild\nThe time of our life\nIt's happening now\nWe're out of control\nTogether we're rolling\nOver and over, we\n\nDown the river, we're drunk and all of our thumbs went up in the air\nSung in the wind and sung to the thunder rolling over and over\nDown the river, we're drunk and all of our thumbs went up in the air\nSung in the wind and sung to the thunder rolling over and over\nDown the river, we're drunk and all of our thumbs went up in the air\nSung in the wind and sung to the thunder rolling over and over\nDown the river, we're drunk and all of our thumbs went up in the air\nSung in the wind and sung to the thunder rolling over and over\n\nNa-na-na-na\nNa-na-na-na\n\nWe gotta rock, rock\nParty till the last shot\nFill that jug and raise your cup up\nWe got chug chug\nBetter take it non stop\nHey! Come drink it till the last drop\n\nSo please just let us burning down this place\nAnd we will end up in the hall of fame\nLet's get ready for a drinking game\nSo come and sip it to the last drop\nOver and over we're\n\nDown the river, we're drunk and all of our thumbs went up in the air\nSung in the wind and sung to the thunder rolling over and over\nDown the river, we're drunk and all of our thumbs went up in the air\nSung in the wind and sung to the thunder rolling over and over\nDown the river, we're drunk and all of our thumbs went up in the air\nSung in the wind and sung to the thunder rolling over and over\nDown the river, we're drunk and all of our thumbs went up in the air\nSung in the wind and sung to the thunder rolling over and over\n",
        .url = "https://drive.google.com/uc?export=open&id=1myR3WUpP7mtgBK62_6vFB7zmk-wbt5bP",
        .duration = 2.07002,
        .writerType = 1
    },
    {
        .title = "Take a Hint",
        .lyrics = "Why am I always hit on by the boys I never like?\nI can always see 'em coming, from the left or from the right\nI don't want to be a priss, I'm just try'na be polite\nBut it always seems to bite me in the...\nAsk me for my number, yeah, you put me on the spot\nYou think that we should hook-up, but I think that we should not\nYou had me at hello, then you opened up your mouth\nAnd that is when it started going south\nOh!\nGet your hands off my hips, 'fore I'll punch you in the lips\nStop your staring at my... Hey!\nTake a hint, take a hint\nNo you can't buy me a drink, let me tell you what I think\nI think you could use a mint\nTake a hint, take a hint\nT-take a hint, take a hint\nI hate you like I love you in the same sentence\nI can't be the one to fill the space in your bed, yeah\nI guess you think you still got me\nI could have you if I wanted\nBut sorry, it ain't happening, oh\nOh!\nGet your hands off my hips, 'fore I'll punch you in the lips\nStop your staring at my... Hey!\nTake a hint, take a hint\nNo you can't buy me a drink, let me tell you what I think\nI think you could use a mint\nTake a hint, take a hint\nWhat about \"no\" don't you get?\nSo go and tell your friends\nI'm not really interested\nIt's about time that you're leavin'\nI'm gonna count to three and\nOpen my eyes and\nYou'll be gone\nOne\nGet your hands off my...\nTwo\nOr I'll punch you in the...\nThree\nStop your staring at my... Hey!\nTake a hint, take a hint\nI am not your missing link\nLet me tell you what I think\nI think you could use a mint\nTake a hint, take a hint\nTake a hint, take a hint\nWoah!\nGet your hands off my hips, 'fore I'll punch you in the lips\nStop your staring at my... Hey!\nTake a hint, take a hint\nT-take a hint, take a hint\n",
        .url = "https://drive.google.com/uc?export=open&id=1uU2mjKzS-wUUxcwGJ4Dsv2tDOx0zfWeE",
        .duration = 2.04011,
        .writerType = 1
    },
    {
        .title = "How to Be a Heartbreaker",
        .lyrics = "Rule number one, is that you gotta have fun\nBut baby when you're done, you gotta be the first to run\nRule number two, just don't get attached to\nSomebody you could lose\nSo le-let me tell you\nThis is how to be a heartbreaker\nBoys they like a little danger\nWe'll get him falling for a stranger\nA player, singing I lo-lo-lo love you\nHow to be a heartbreaker\nBoys they like the look of danger\nWe'll get him falling for a stranger\nA player, singing I lo-lo-lo love you\nAt least I think I do\nCause I lo-lo-lo love you\nRule number three, wear your heart on your cheek\nBut never on your sleeve, unless you wanna taste defeat\nRule number four, gotta be looking pure\nKiss him goodbye at the door, and leave him wanting more, more\nThis is how to be a heartbreaker\nBoys they like a little danger\nWe'll get him falling for a stranger\nA player, singing I lo-lo-lo love you\nHow to be a heartbreaker\nBoys they like the look of danger\nWe'll get him falling for a stranger\nA player, singing I lo-lo-lo love you\nAt least I think I do\nCause I lo-lo-lo love you\nGirls, we do, whatever it will take\nCause girls don't want, we don't want our hearts to break in two\nSo it's better to be fake, can't risk losing\nIn love again, babe\nThis is how to be a heartbreaker\nBoys they like a little danger\nWe'll get him falling for a stranger\nA player, singing I lo-lo-lo love you\nHow to be a heartbreaker\nBoys they like the look of danger\nWe'll get him falling for a stranger\nA player, singing I lo-lo-lo love you\nCause I lo-lo-lo love you\nAt least I think I do\n",
        .url = "https://drive.google.com/uc?export=open&id=1AcytXQetZIAHnfpg7parij8JTmKTSKiT",
        .duration = 2.57005,
        .writerType = 1
    },
    {
        .title = "Light it Up",
        .lyrics = "Just like the street lights lit this town\nLike a fire in a blaze, gotta burn it down\nCan't be afraid to live this out\nWe got this far don't know how\nI see danger in your eyes\nThey know we'll burn down the night\nComing just like the sunrise\nYou know that we gonna light it up\nBabe, tonight we gonna light it up\nBabe, tonight we gonna light it up\nJust like the sunrise, we gonna light it up\nNow\nLight it up, light it up\nWe light it, light it up\nJust like a spark that's breaking out\nCan't make it stop, can't turn back now\nWe know that's there no room for doubt\nNo we can't turn back now\nI see danger in your eyes\nThey know we'll burn down the night\nComing just like the sunrise\nYou know that we gonna light it up\nBabe, tonight we gonna light it up\nBabe, tonight we gonna light it up\nJust like the sunrise, we gonna light it up\nNow\nLight it up, light it up\nWe light it, light it up\n",
        .url = "https://drive.google.com/uc?export=open&id=165F_05Gmr-iz5gjTTAjcwaEB9pZpHWLz",
        .duration = 2.01006,
        .writerType = 1
    },
    {
        .title = "Pretty Rave Girl",
        .lyrics = "I know this pretty rave girl\nAlways think about her\nAnd when she says hi to me\nButterflies go right through me\nAnd when I see her dancing\nWanna take a chance in\nGetting a little closer\nAnd maybe get to know her\nPretty rave girl\nI know this pretty rave girl\nAlways think about her\nAnd when she says hi to me\nButterflies go right through me\nAnd when I see her dancing\nWanna take a chance in\nGetting a little closer\nAnd maybe get to know her\nPretty rave girl\nPretty rave girl\nPretty rave girl\nPretty rave girl\nPretty rave girl\nPretty rave girl\nI know this pretty rave girl\nAlways think about her\nAnd when she says hi to me\nButterflies go right through me\nAnd when I see her dancing\nWanna take a chance in\nGetting a little closer\nAnd maybe get to know her\nPretty rave girl\nPretty rave girl\nPretty rave girl\nPretty rave girl\nPretty rave girl\nPretty rave girl\n",
        .url = "https://drive.google.com/uc?export=open&id=1mHtT9xwhyjNZKxpPsza0UgUsQTol0kZf",
        .duration = 2.39010,
        .writerType = 1
    },
    {
        .title = "All I Ever Wanted",
        .lyrics = "All I ever wanted was to see you smiling\nI know that I love you, oh baby why don't you see\nAll I ever wanted was to see you smiling\nAll I ever wanted was to make you mine\nI know that I love you, oh baby why don't you see\nThat all I ever wanted was you and me\nAll I ever wanted was to see you smiling\nAll I ever wanted was to make you mine\nI know that I love you, oh baby why don't you see\nThat all I ever wanted was you and me\nDrop the bass\nI'm so alone\nHere on my own\nAnd I'm waiting for you to come\nI want to be a part of you\nThink of all the things we could do\nAnd everyday\nYou're in my head\nI want to have you in my bed\nYou are the one\nYou're in my eyes\nAll I ever wanted in my life\nAll I ever wanted was to see you smiling\nAll I ever wanted was to make you mine\nI know that I love you, oh baby why don't you see\nThat all I ever wanted was you and me\nAll I ever wanted was to see you smiling\nAll I ever wanted was to make you mine\nI know that I love you oh baby why don't you see\nThat all I ever wanted was you and me\nAre you ready?\nAll I ever wanted was to see you smiling\nAll I ever wanted was to make you mine\nI know that I love you, oh baby why don't you see\nThat all I ever wanted was you and me\nAll I ever wanted\n",
        .url = "https://drive.google.com/uc?export=open&id=1tYChoSq68A3IiyQ9A8oUe03PVOLfTAwd",
        .duration = 2.25002,
        .writerType = 1
    },
    {
        .title = "Angel of Darkness",
        .lyrics = "Angel of Darkness\nAngel of Darkness\nThe world is in your hand\nBut I will fight until the end\nAngel of Darkness\nAngel of Darkness\nDon't follow your command\nBut I will fight and I will stand\nWhen darkness falls pain is all\nThe Angel of Darkness\nWill leave behind and I will fight\nThe love is lost\nBeauty and light\nHave vanished from garden of delight\nThe dreams are gone\nMidnight has come\nThe darkness is our new kingdom\nAngel of Darkness\nAngel of Darkness\nThe world is in your hand\nBut I will fight until the end\nAngel of Darkness\nAngel of Darkness\nDon't follow your command\nBut I will fight and I will stand\nHunt goes on deep in the night\nTime to pray down on your knees\nYou can't hide from me\nEtern the light\nUntil my last breath I will fight\n(I will fight, I will fight, I will fight, I will fight)\nNow realize\nThe stars they die\nDarkness has fallen in paradise\nBut we'll be strong\nAnd we will fight\nAgainst the creatures of the night\nAngel of Darkness\nAngel of Darkness\nThe world is in your hand\nBut I will fight until the end\nAngel of Darkness\nAngel of Darkness\nDon't follow your command\nBut I will fight and I will stand\n",
        .url = "https://drive.google.com/uc?export=open&id=1sU39rFpMAb4ajAFvGAsGpu3oJCv540FH",
        .duration = 2.55003,
        .writerType = 1
    },
    {
        .title = "Pretty Little Psycho",
        .lyrics = "Oh, oh, oh\nOh, here we go\nWalkin', talkin' like you know\nI want your pretty little psycho\n(Pretty little psycho)\n\nI'm ready to go, I'm taking chances\nSippin' lust from champagne glasses\nGotta have you, I'm movin' closer\nI won't take no for an answer\nYou're lookin' crazy, you're lookin' wrong\nIt looks like we're gonna get along\nAnd once I've got you, it's a fact\nBaby, there's no turning back\n\nMake me, make me impressed\nMake me, make me obsessed\n\nOh, oh, oh\nOh, here we go\nWalkin', talkin' like you know\nI want your pretty little psycho\nOh, oh, oh\nOh, here we go\nBaby strike a pose\nI want your pretty little psycho\n\nPretty little, pretty little, pretty little psycho\nPretty little, pretty little\nPretty little psycho\nPretty little, pretty little, pretty little psycho\nPretty little, pretty little\nPretty little psycho\n\nBlack lipstick just like a cat\nHave you purrin' on your back\nTip the glass, now light it up\nRebel, rebel can't get enough\nYou're lookin' crazy, you're lookin' wrong\nWe look like we belong\nAnd once I've got you, it's a fact\nBaby, there's no turning back\n\nMake me, make me impressed\nMake me, make me obsessed\n\nOh, oh, oh\nOh, here we go\nWalkin', talkin' like you know\nI want your pretty little psycho\nOh, oh, oh\nOh, here we go\nBaby strike a pose\nI want your pretty little psycho\n\nPretty little, pretty little, pretty little psycho\nPretty little, pretty little\nPretty little psycho\nPretty little, pretty little, pretty little psycho\nPretty little, pretty little\nPretty little psycho\n\nThere's somethin' about you driving me mad\nThere's somethin' about you I've got to have\nThere's somethin' about you\nThere's somethin' about you\nThere's somethin' about you driving me mad\nThere's somethin' about you I've got to have\nThere's somethin' about you\nThere's somethin' about you\n\nOh, oh, oh  \nOh, here we go                 \nWalkin', talkin' like you know\nI want your pretty little psycho\nOh, oh, oh\nOh, here we go\nBaby strike a pose\nI want your pretty little psycho\n\nOh, oh, oh\nOh, here we go\nWalkin', talkin' like you know\nI want your pretty little psycho\nOh, oh, oh\nOh, here we go\nBaby strike a pose\nI want your pretty little psycho\n\nPretty little, pretty little, pretty little psycho\nPretty little, pretty little\nPretty little psycho\nPretty little, pretty little, pretty little psycho\nPretty little, pretty little\nPretty little psycho\n",
        .url = "https://drive.google.com/uc?export=open&id=1AcSaE1naXCEZgP4RJVIx__Kv-Pg5D3Bf",
        .duration = 2.48000,
        .writerType = 1
    },
    { .title = NULL, .lyrics = NULL, .url = NULL, .duration = 0, .writerType = 0}
};

/* ==================== NWOBHM SONGS ==================== */
songMetaData nwobhmSongs[] = {
    {
        .title = "Ace of Spades - Motörhead",
        .lyrics = "If you like to gamble\nI tell you, I'm your man\nYou win some, lose some\nIt's all the same to me\nThe pleasure is to play\nMakes no difference what you say\nI don't share your greed\nThe only card I need\nIs the Ace of Spades\nThe Ace of Spades\n\nPlaying for the high one\nDancing with the devil\nGoing with the flow\nIt's all a game to me\nSeven or eleven\nSnake eyes watching you\nDouble up or quit\nDouble stake or split\nThe Ace of Spades\nThe Ace of Spades\n\nYou know I'm born to lose\nAnd gambling's for fools\nBut that's the way I like it, baby\nI don't want to live forever\nAnd don't forget the joker\n\nPushing up the ante\nI know you got to see me\nRead 'em and weep\nThe dead man's hand again\nI see it in your eyes\nTake one look and die\nThe only thing you see\nYou know it's gonna be\nThe Ace of Spades\nThe Ace of Spades, uh\n",
        .url = "https://drive.google.com/uc?export=open&id=1j6quK4yfQ-V4rpmDECUnWg5xglOeF22d",
        .duration = 2.29014,
        .writerType = 2
    },
    {
        .title = "Overkill - Motörhead",
        .lyrics = "Only way to feel the noise is when it's good and loud\nSo good I can't believe it, screaming with the crowd\nDon't sweat it, we'll get it back to you\nDon't sweat it, we'll get it back to you\nOverkill, overkill, overkill\n\nOn your feet you feel the beat, it goes straight to your spine\nShake your head you must be dead if it don't make you fly\nDon't sweat it, we'll get it back to you\nDon't sweat it, we'll get it back to you\nOverkill, overkill, overkill\n\nKnow your body's made to move, ya feel it in your guts\nRock 'n' roll ain't worth the name if it don't make ya strut\nDon't sweat it, we'll get it back to you\nDon't sweat it, we'll get it back to you\nOverkill, overkill, overkill\n",
        .url = "https://drive.google.com/uc?export=open&id=1zjgn3CeieJk7Dwn6vUkn1aWrziKY0C3N",
        .duration = 2.45025,
        .writerType = 2
    },
    {
        .title = "Too Late Too Late - Motörhead",
        .lyrics = "I see that nothing's changed\nInsist on playing games\nSome waste of time you are\nAnd you're so popular\nWell, this is it, you bitch\nI got to make a switch\nYou find some other John\nI know what's going on\n\nGood move\nWhat do I have to lose?\nStalemate\nToo late, too late\n\nI thought you were for real\nJust one more rip-off deal\nDon't give me all that crap\nI just escaped your trap\nYou think you see the joke\nBut you're just chasing smoke\n'Cause now the thrill is gone\nI know what's going on\n\nGood move\nWhat do I have to lose?\nStalemate\nToo late, too late\n\nMisunderstanding me\nThe way you felt so free\nI'm gonna jump the gun\nI'm gonna cut and run\nYour credibility\nDon't cut no ice with me\nYou're just a feeble con\nI know what's going on\n\nGood move\nWhat do I have to lose?\nStalemate\nToo late, too late\nToo late, too late\n",
        .url = "https://drive.google.com/uc?export=open&id=1WBJrOQTFzlmkQ5z0Q_EBfvBTMcuXSK3J",
        .duration = 2.58024,
        .writerType = 2
    },
    {
        .title = "Am I Evil? - Diamond Head",
        .lyrics = "My mother was a witch\nShe was burned alive\nThankless little bitch\nFor the tears I cried\nTake her down now\nDon't want to see her face\nBlistered and burned\nCan't hide my disgrace\n\nTwenty-seven, everyone was nice\nGot to see them\nMake them pay the price\nSee their bodies out on the ice\nTake my time\n\nAm I evil?\nYes, I am\nAm I evil?\nI am, man\nYes, I am\n\nAs I watched my mother die\nI lost my head\nRevenge now I sought to break with my bread\nTaking no chances\nYou come with me\nI'll split you to the bone\nHelp set you free\n\nTwenty-seven\nEveryone was nice\nGot to see them\nMake them pay the price\nSee their bodies out on the ice\nTake my time\n\nAm I evil?\nYes, I am\nAm I evil?\nI am, man, yes, I am\n\nOhoh!\n\nOn with the action now\nI'll strip your pride\nI'll spread your blood around\nI'll see you ride\nYour face is scarred with steel\nWounds deep and neat\nLike a double dozen before you\nSmells so sweet\n\nAm I evil?\nYes, I am\nAm I evil?\nI am, man\n\nMy soul is longing before\nAwait my hell\nSent to avenge my mother\nSleep myself\nMy face is long forgotten\nMy face not my own\nSweet and timely whore\nTake me home\n\nAm I evil?\nYes I am\nAm I evil?\nI am, man, yes, I am\nOh\n\nOn with the action now, I'll strip your pride\nI'll spread your blood around, I'll see you ride\nYour face is scarred with steel, your spirits dance\nLike a double dozen before you, no romance\n\nAm I evil? Yes, I am\nAm I evil? I am, man\nAm I evil? Yes, I am\nAm I evil? I am, man, yes, I am\n",
        .url = "https://drive.google.com/uc?export=open&id=1hEIX3vW2nalBgn2V0gyzn4iHrDjmDglz",
        .duration = 6.562112,
        .writerType = 2
    },
    {
        .title = "In the Heat of The Night - Diamond Head",
        .lyrics = "Steely eyes of a silvery people\nWalk behind me, with evil intent\nUp to the tower, I stand alone\nStripped of rank before them I'm sent\n\nWhat befalls us in the heat of the night?\nWhat befalls us in the heat of the night?\n\nBefore long, behind adamantine doors\nI taste of the wine of their joy\nThrough delirium of a tortured mind\nHer face gave me strength to this day\n\nWhat befalls us in the heat of the night?\nWhat befalls us in the heat of the night?\nIn the heat of the night\n\nBut love deceived me, and in my despair\nI recalled the ring once again\nBorn of Sorrow my cunning revives me\nAnd I took them with me enchained\n\nWhat befalls us in the heat of the night?\nWhat befalls us in the heat of the night?\nIn the heat of the night\n",
        .url = "https://drive.google.com/uc?export=open&id=1TA4Kpj3Oc_Cz70RUaMzlviL1uRwTf_IV",
        .duration = 3.55024,
        .writerType = 2
    },
    {
        .title = "Denim & Leather - Saxon",
        .lyrics = "Where were you in '79 when the dam began to burst\nDid you check us out down at the local show\nWere you wearing denim, wearing leather\nDid you run down to the front\nDid you queue for your ticket through the ice and snow\n\nDenim and Leather\nBrought us all together\nIt was you that set the spirit free\nDenim and Leather\nBrought us all together\nIt was you that set the spirit free\n\nDid you read the music paper from the back and to the front\nDid you find out where to see your favorite band\nDid you listen to the radio every Friday night\nDid hang around your local record store\n\nDenim and Leather\nBrought us all together\nIt was you that set the spirit free\nDenim and Leather\nBrought us all together\nIt was you that set the spirit free\n\nDo you dream of playing guitar or smashing up the drums\nMaybe you can learn to play the bass\nYou can always be a singer like me and front the band\nWhen on the stage we wait at your command\n\nDenim and Leather\nBrought us all together\nIt was you that set the spirit free\nDenim and Leather\nBrought us all together\nIt was you that set the spirit free\n\nIt was you - that set the spirit free\nIt was you - that put us here today\nIt was you - that filled the concert halls\nIt was you - that set the spirit free\n\nDenim and Leather\nBrought us all together\nIt was you that set the spirit free\nDenim and Leather\nBrought us all together\nIt was you that set the spirit free\n",
        .url = "https://drive.google.com/uc?export=open&id=1hrL2x7x7SRdEeJdehbuwmSik_dJBcuGR",
        .duration = 5.11017,
        .writerType = 2
    },
    {
        .title = "Crusader - Saxon",
        .lyrics = "Crusader, crusader, please take me with you\nThe battle lies far to the east\nCrusader, crusader, don't leave me alone\nI want to ride out on your quest\nI'm waiting, I'm waiting, to stand by your side\nTo fight with you over the sea\nThey're calling, they're calling, I have to be there\nThe holy land has to be free\n\nFight the good fight, believe what is right\nCrusader, the Lord of the Realm\nFight the good fight with all your might\nCrusader, the Lord of the Realm\n\nWe're marching, we're marching, to a land far from home\nNo one can say who'll return\nFor Christendom's sake, we'll take our revenge\nOn the pagans from out of the east\nWe Christians are coming, with swords held on high\nUnited by faith and the cause\nThe Saracen heathen will soon taste our steel\nOur standards will rise 'cross the land\n\nFight the good fight, believe what is right\nCrusader, the Lord of the Realm\nFight the good fight with all your might\nCrusader, the Lord of the Realm\n\nTo battle, to battle, the Saracen hordes\nWe follow the warrior king\nOnward, ride onward, into the fight\nWe carry the sign of the cross\nWarlords of England, Knights of the Realm\nSpilling their blood in the sand\nCrusader, crusader, the legend is born\nThe future will honour your deeds\n\nFight the good fight, believe what is right\nCrusader, the Lord of the Realm\nFight the good fight with all your might\nCrusader, the Lord of the Realm\n",
        .url = "https://drive.google.com/uc?export=open&id=10sDVNalQM0w6VEO21KmMYhmM73bnZzfP",
        .duration = 7.25131,
        .writerType = 2
    },
    {
        .title = "Fire in The Sky - Saxon",
        .lyrics = "There's a fire in the sky\nThere's a feeling in the air\nWhen the ground starts to tremble\nThe first attack is here\n\nYou won't stand a chance\nWhen the missiles start to fly\nThey never tell the people\n'Til the holocaust is here\n\nWhen the fire's in the sky\nYou'll be running for your lives\nWhen the fire's in the sky\nPrepare yourselves to die\n\nWhere, where are the shelters?\nThey said they would be there\nYou'll need, need some protection\nFrom the radiation flare\n\nDon't, don't start a panic\nIt won't do you no good\nYou'll just have to wait\nFor the end of all mankind\n\nIf you're near the point of impact\nThere's nowhere you can run\nIf the heatwave don't get you\nThe shockwave surely will\n\nThere won't be no more heroes\nThere's no one left to fight\nIt's the end of our planet\nWe'd commit eternal sight\n\nWhen the fire's in the sky\nYou'll be running for your lives\nWhen the fire's in the sky\nPrepare yourselves to die\n\nWhen the fire's in the sky\nYou'll be running for your lives\nWhen the fire's in the sky\nPrepare yourselves to die\nTo die, to die, to die, to die\n",
        .url = "https://drive.google.com/uc?export=open&id=1MesOIRuHZqty4OqvFhEvqjJjMuKmu6T6",
        .duration = 3.37015,
        .writerType = 2
    },
    {
        .title = "The Trooper - Iron Maiden",
        .lyrics = "You'll take my life, but I'll take yours too\nYou'll fire your musket, but I'll run you through\nSo when you're waiting for the next attack\nYou'd better stand, there's no turning back\n\nThe bugle sounds, the charge begins\nBut on this battlefield, no one wins\nThe smell of acrid smoke and horses' breath\nAs I plunge on into certain death, oh, oh\n\nThe horse, he sweats with fear, we break to run\nThe mighty roar of the Russian guns\nAnd as we race towards the human wall\nThe screams of pain as my comrades fall\n\nWe hurdle bodies that lay on the ground\nAnd the Russians fire another round\nWe get so near, yet so far away\nWe were meant to fight another day, oh, oh\n\nWe get so close, near enough to fight\nWhen a Russian gets me in his sights\nHe pulls the trigger and I feel the blow\nA burst of rounds take my horse below\n\nAnd as I lay there gazing at the sky\nMy body's numb and my throat is dry\nAnd as I lay forgotten and alone\nWithout a tear, I draw my parting groan, oh, oh\n",
        .url = "https://drive.google.com/uc?export=open&id=1gKy2Juy8sBNvNUoZhSK--go2LSb1niv-",
        .duration = 3.57037,
        .writerType = 2
    },
    {
        .title = "Prisoner of Your Eyes - Judas Priest",
        .lyrics = "When I saw your face\nI became a prisoner of your eyes\nAnd I would do just anything\nTo stay and be with you\n\nYou know there are times\nWhen I let myself wonder\nAs I was going under\nYou pulled me back to earth\n\nDon't you hear me crying?\nTake me in your arms again\nTell me that you're trying\nOr is our love a lie?\n\nLove is blind\nAnd love deceives you\nYou came along and captured me\nNow I'm a prisoner of your eyes\nTrapped in time\nI cannot leave you\nI'm just a prisoner of your eyes\n\nAs each day goes by\nI've given up completely\nI've locked myself inside your heart\nAnd thrown away the key\n\nOnly time will tell\nIf I can live without you\nCan you see into the future?\nWill you ever set me free?\n\nDon't you hear me crying?\nTake me in your arms again\nTell me that you're trying\nOr is our love a lie?\n\nLove is blind\nAnd love deceives you\nYou came along and captured me\nNow I'm a prisoner of your eyes\nTrapped in time\nI cannot leave you\nI'm just a prisoner of your eyes\n\nIn this heartache\nWe can try and start again\nStop the heartbreak\nA little time will help to kill the pain\n\nDon't you hear me crying?\nTake me in your arms again\nTell me that you're trying\nOr is our love a lie?\n\nLove is blind\nAnd love deceives you\nYou came along and captured me\nNow I'm a prisoner of your eyes\nTrapped in time\nI cannot leave you\nI'm just a prisoner of your eyes\n\nLove is blind\nAnd love deceives you\nYou came along and captured me\nNow I'm a prisoner of your eyes\nTrapped in time\nI cannot leave you\nI'm just a prisoner of your eyes\n\nLove is blind\nAnd love deceives you\nYou came along and captured me\nNow I'm a prisoner of your eyes\nTrapped in time\nI cannot leave you\nI'm just a prisoner of your eyes\n",
        .url = "https://drive.google.com/uc?export=open&id=1S6Ikwiyt6q5fQBJiyi8cBPpg5btZXKb2",
        .duration = 6.55024,
        .writerType = 2
    },
    { .title = NULL, .lyrics = NULL, .url = NULL, .duration = 0 }
};

/* ==================== WHITE GIRL POP ==================== */
songMetaData whiteGirlPopSongs[] = {
    {
        .title = "Toxic - Britney Spears",
        .lyrics = "Baby, can't you see I'm callin'?\nA guy like you should wear a warnin'\nIt's dangerous, I'm fallin'\nThere's no escape, I can't wait\nI need a hit, baby, give me it\nYou're dangerous, I'm lovin' it\nToo high, can't come down\nLosing my head, spinnin' 'round and 'round\nDo you feel me now?\nWith a taste of your lips, I'm on a ride\nYou're toxic, I'm slippin' under\nWith a taste of a poison paradise\nI'm addicted to you\nDon't you know that you're toxic?\nAnd I love what you do\nDon't you know that you're toxic?\nIt's gettin' late to give you up\nI took a sip from my devil's cup\nSlowly, it's takin' over me\nToo high, can't come down\nIt's in the air and it's all around\nCan you feel me now?\nWith a taste of your lips, I'm on a ride\nYou're toxic, I'm slippin' under\nWith a taste of a poison paradise\nI'm addicted to you\nDon't you know that you're toxic?\nAnd I love what you do\nDon't you know that you're toxic?\nDon't you know that you're toxic?\nTaste of your lips, I'm on a ride\nYou're toxic, I'm slippin' under\nWith a taste of a poison paradise\nI'm addicted to you\nDon't you know that you're toxic?\nWith a taste of your lips, I'm on a ride\nYou're toxic, I'm slippin' under (Toxic)\nWith a taste of a poison paradise\nI'm addicted to you\nDon't you know that you're toxic?\nIntoxicate me now with your lovin' now\nI think I'm ready now (I think I'm ready now)\nIntoxicate me now with your lovin' now\nI think I'm ready now\n",
        .url = "https://drive.google.com/uc?export=open&id=1p1r9InaIGQYSbouOabUicTRJw2phNTfq",
        .duration = 3.32013,
        .writerType = 0
    },
    {
        .title = "Gimme More - Britney Spears",
        .lyrics = "It's Britney, bitch\nI see you\nAnd I just wanna dance with you\nEvery time they turn the lights down\nJust wanna go that extra mile for you\nPublic display of affection (Oh-oh-oh)\nFeels like no one else in the room (But you)\nWe can get down\nLike there's no one around\nWe keep on rocking (We keep on rocking)\nWe keep on rocking (We keep on rocking)\nCameras are flashing\nWhile we're dirty dancing\nThey keep watching (They keep watching)\nKeep watching, feels like the crowd is saying\nGimme, gimme (More), gimme (More)\nGimme, gimme (More)\nGimme, gimme (More), gimme (Mo')\nGimme, gimme (More)\nGimme, gimme (More), gimme (More)\nGimme, gimme (More)\nGimme, gimme (More), gimme (Mo')\nGimme, gimme (More)\nThe center of attention (Do you feel that?)\nEven when we're up against the wall\nYou got me in a crazy position (Yeah)\nIf you're on a mission (Uh-huh), you got my permission (Oh)\nWe can get down\nLike there's no one around\nWe keep on rocking (Keep on rocking)\nWe keep on rocking (Oh), rocking (Uh-huh)\nCameras are flashing\nWhile we're dirty dancing\nThey keep watching (They keep watching)\nKeep watching, feels like the crowd is saying\nGimme, gimme (More), gimme (More)\nGimme, gimme (More)\nGimme, gimme (More), gimme (Mo')\nGimme, gimme (More)\nGimme, gimme (More), gimme (More)\nGimme, gimme (More)\nGimme, gimme (More), gimme (Mo')\nGimme, gimme (More)\nOoh-ooh-ooh\nOoh-ooh-ooh (I just can't)\nOoh-ooh-ooh (Control myself)\nOoh-ooh-ooh (More)\nOoh-ooh-ooh (They want more?)\nOoh-ooh-ooh\nOoh-ooh-ooh (Well, I'll give them more)\nOoh-ooh-ooh (Ow, more)\nGimme, gimme (More), gimme (More)\nGimme, gimme (More), give me more\nGimme, gimme (More), gimme (Mo')\nGimme, gimme (More), ooh-ooh\nGimme, gimme (More), gimme (More)\nGimme, gimme (More)\nGimme, gimme (More), give me more, gimme (Mo'), yeah\nGimme, gimme (More)\nOh-oh-oh\nGimme more, gimme more\nMo— Mo— Mo— Mo— Mo— Mo— Mo— (More)\nGimme more, gimme more, babe\nDanja, Danja, Danja, Danja\nDanja, Danja, Danja, Danja (I just want more)\nOoh-oh, ah-ah-ah, ah\nOoh-oh, ah-ah-ah, ah\nOoh-oh, ah-ah-ah, ah\nOoh-oh, ah-ah-ah, ah\nOoh-oh, ah-ah-ah, ah\nOoh-oh, ah-ah-ah, ah\nOoh-oh, ah-ah-ah, ah\nOoh-oh, ah-ah-ah, ah\nGimme, gimme, gimme\nGimme, gimme\nGimme, gimme, gimme\n(Gimme, gimme)\nGimme, gimme (Danja, Danja, Danja, Danja)\nGimme, gimme, gimme\nGimme, gimme\nGimme, gimme, gimme\nGimme, gimme\nBet you didn't see this one coming\nThe Incredible Lago (Mo— Mo— Mo— Mo— Mo— Mo— Mo—)\nThe legendary Miss Britney Spears, haha\nAnd the unstoppable Danja\nAh, you gonna have to remove me\n'Cause I ain't going nowhere\nMo— Mo— Mo— Mo— Mo— Mo— Mo— (More)\n",
        .url = "https://drive.google.com/uc?export=open&id=1vICbwuedVzN-k5nt3wc5hIZECTCZyMRE",
        .duration = 4.01002,
        .writerType = 0
    },
    {
        .title = "Oops!...I Did It Again - Britney Spears",
        .lyrics = "Mmm, yeah\nYeah, yeah, yeah, yeah, yeah, yeah\nYeah, yeah, yeah, yeah, yeah, yeah\nI think I did it again\nI made you believe we're more than just friends\nOh, baby, it might seem like a crush\nBut it doesn't mean that I'm serious\n'Cause to lose all my senses\nThat is just so typically me\nOoh, baby, baby\nOops, I did it again,\nI played with your heart\nGot lost in the game\n, oh, baby, baby\nOops, you think I'm in love, that I'm sent from above\nI'm not that innocent\nYou see, my problem is this\nI'm dreamin' away\nWishin' that heroes, they truly exist\nI cry watching the days\nCan't you see I'm a fool in so many ways?\nBut to lose all my senses\nThat is just so typically me\nOh, baby, oh\nOops, I did it again, I played with your heart\nGot lost in the game, oh, baby, baby\nOops, you think I'm in love, that I'm sent from above\nI'm not that innocent\nYeah, yeah, yeah, yeah, yeah, yeah\nYeah, yeah, yeah, yeah, yeah, yeah\n\"All aboard!\"\n\"Britney! Before you go, there's something I want you to have.\"\n\"Oh, it's beautiful! But wait a minute, isn't this...?\"\n\"Yeah, yes, it is.\"\n\"But I thought the old lady dropped it into the ocean in the end.\"\n\"Well, baby, I went down and got it for ya.\"\n\"Aw, you shouldn't have\"\nOops, I did it again to your heart\nGot lost in this game, oh, baby (Baby)\nOops, you (Oops, you), think that I'm sent from above\nI'm not that innocent\nOops, I did it again, I played with your heart\nGot lost in the game (Yeah, yeah), oh, baby, baby\nOops, you think I'm in love, that I'm sent from above (Yeah, yeah)\nI'm not that innocent (Not that innocent, babe)\nOops, I (Oops, I) did it again (Did it again to your heart), I played with your heart (Got lost)\nGot lost in the game (In this game, oh, baby), oh, baby, baby\nOops, you (Oops, you) think I'm in love (Think that I'm sent), that I'm sent from above\nI'm not that innocent\n",
        .url = "https://drive.google.com/uc?export=open&id=1AI1qJxkp73IxiYuUrg11U07MkRYoPTQb",
        .duration = 3.31013,
        .writerType = 0
    },
    {
        .title = "S&M - Rihanna",
        .lyrics = "Na-na-na, come on\nNa-na-na, come on\nNa-na-na-na-na, come on\nNa-na-na, come on, come on, come on\nNa-na-na-na, come on\nNa-na-na, come on\nNa-na-na-na-na, come on\nNa-na-na, come on, come on, come on\nNa-na-na-na\nFeels so good being bad (Oh-oh-oh-oh-oh)\nThere's no way I'm turning back (Oh-oh-oh-oh-oh)\nNow the pain is my pleasure 'cause nothing could measure\n(Oh, oh, oh-oh-oh-oh-oh)\nLove is great, love is fine (Oh-oh-oh-oh-oh)\nOut the box, outta line (Oh-oh-oh-oh-oh)\nThe affliction of the feeling leaves me wanting more\n(Oh, oh, oh-oh-oh-oh-oh)\n'Cause I may be bad, but I'm perfectly good at it\nSex in the air, I don't care, I love the smell of it\nSticks and stones may break my bones\nBut chains and whips excite me\n'Cause I may be bad, but I'm perfectly good at it\nSex in the air, I don't care, I love the smell of it\nSticks and stones may break my bones\nBut chains and whips excite me\nNa-na-na, come on, come on, come on\nI like it, like it\nCome on, come on, come on\nI like it, like it (Na-na-na)\nCome on, come on, come on\nI like it, like it\nCome on, come on, come on\nI like it, like it\nLove is great, love is fine (Oh-oh-oh-oh-oh)\nOut the box, outta line (Oh-oh-oh-oh-oh)\nThe affliction of the feeling leaves me wanting more\n(Oh, oh, oh-oh-oh-oh-oh)\n'Cause I may be bad, but I'm perfectly good at it\nSex in the air, I don't care, I love the smell of it\nSticks and stones may break my bones\nBut chains and whips excite me\nNa-na-na, come on, come on, come on\nI like it, like it\nCome on, come on, come on\nI like it, like it (Na-na-na)\nCome on, come on, come on\nI like it, like it\nCome on, come on, come on\nI like it, like it\nS-S-S and M-M-M\nS-S-S and M-M-M\nOh, I love the feeling you bring to me\nOh, you turn me on\nIt's exactly what I've been yearning for\nGive it to me strong\nAnd meet me in my boudoir\nMake my body say, \"Ah-ah-ah\"\nI like it, like it\n'Cause I may be bad, but I'm perfectly good at it\nSex in the air, I don't care, I love the smell of it\nSticks and stones may break my bones\nBut chains and whips excite me\n'Cause I may be bad, but I'm perfectly good at it\nSex in the air, I don't care, I love the smell of it\nSticks and stones may break my bones\nBut chains and whips excite me\nNa-na-na, come on, come on, come on\nI like it, like it\nCome on, come on, come on\nI like it, like it (Na-na-na)\nCome on, come on, come on\nI like it, like it\nCome on, come on, come on\nI like it, like it\nS-S-S and M-M-M\nS-S-S and M-M-M\nS-S-S and M-M-M\nS-S-S and M-M-M\n",
        .url = "https://drive.google.com/uc?export=open&id=1L3JPUMiqoykptsdTkF8xzRoOHouyGrbn",
        .duration = 4.05001,
        .writerType = 0
    },
    {
        .title = "Call Me Maybe - Carly Rae Jepsen",
        .lyrics = "I threw a wish in the well\nDon't ask me, I'll never tell\nI looked to you as it fell\nAnd now you're in my way\nI trade my soul for a wish\nPennies and dimes for a kiss\nI wasn't lookin' for this\nBut now you're in my way\nYour stare was holdin'\nRipped jeans, skin was showin'\nHot night, wind was blowin'\nWhere you think you're goin', baby?\nHey, I just met you, and this is crazy\nBut here's my number, so call me maybe\nIt's hard to look right at you, baby\nBut here's my number, so call me maybe\nHey, I just met you, and this is crazy\nBut here's my number, so call me maybe\nAnd all the other boys try to chase me\nBut here's my number, so call me maybe\nYou took your time with the call\nI took no time with the fall\nYou gave me nothin' at all\nBut still, you're in my way\nI beg and borrow and steal\nAt first sight, and it's real\nI didn't know I would feel it\nBut it's in my way\nYour stare was holdin'\nRipped jeans, skin was showin'\nHot night, wind was blowin'\nWhere you think you're goin', baby?\nHey, I just met you, and this is crazy\nBut here's my number, so call me maybe\nIt's hard to look right at you, baby\nBut here's my number, so call me maybe\nHey, I just met you, and this is crazy\nBut here's my number, so call me maybe\nAnd all the other boys try to chase me\nBut here's my number, so call me maybe\nBefore you came into my life, I missed you so bad\nI missed you so bad, I missed you so, so bad\nBefore you came into my life, I missed you so bad\nAnd you should know that, I missed you so, so bad\nIt's hard to look right at you, baby\nBut here's my number, so call me maybe\nHey, I just met you, and this is crazy\nBut here's my number, so call me maybe\nIt's hard to look right at you, baby\nBut here's my number, so call me maybe\nHey, I just met you, and this is crazy\nBut here's my number, so call me maybe\nAnd all the other boys try to chase me\nBut here's my number, so call me maybe\nHey, I just met you, and this is crazy\nBut here's my number, so call me maybe\nAnd all the other boys try to chase me\nBut here's my number, so call me maybe\nBefore you came into my life, I missed you so bad\nI missed you so bad, I missed you so, so bad\nBefore you came into my life, I missed you so bad\nAnd you should know that\n",
        .url = "https://drive.google.com/uc?export=open&id=1Yx_n1dMvqmZBtweq4yRkReiAy9pl7b5q",
        .duration = 3.19005,
        .writerType = 0
    },
    {
        .title = "Bad Romance - Lady Gaga",
        .lyrics = "Oh, caught in a bad romance\nOh, caught in a bad romance\nRa, ra, ah-ah-ah\nRoma, roma-ma\nGaga, ooh, la, la\nWant your bad romance\nRa, ra, ah-ah-ah\nRoma, roma-ma\nGaga, ooh, la, la\nWant your bad romance\nI want your ugly, I want your disease\nI want your everything as long as it's free\nI want your love\nLove, love, love, I want your love, ayy\nI want your drama, the touch of your hand ( Hey )\nI want your leather-studded kiss in the sand\nI want your love\nLove, love, love, I want your love\n(Love, love, love, I want your love)\nYou know that I want you\nAnd you know that I need you\nI want it bad, your bad romance\nI want your love and I want your revenge\nYou and me could write a bad romance (Oh)\nI want your love and all your lover's revenge\nYou and me could write a bad romance\nOh, caught in a bad romance\nOh, caught in a bad romance\nRa, ra, ah-ah-ah\nRoma, roma-ma\nGaga, ooh, la, la\nWant your bad romance\nI want your horror, I want your design\n'Cause you're a criminal as long as you're mine\nI want your love\nLove, love, love, I want your love\nI want your psycho, your vertigo schtick (Hey)\nWant you in my rear window, baby, you're sick\nI want your love\nLove, love, love, I want your love\n(Love, love, love, I want your love)\nYou know that I want you\nAnd you know that I need you ('Cause I'm a free bitch, baby)\nI want it bad, your bad romance\nI want your love and I want your revenge\nYou and me could write a bad romance (Oh)\nI want your love and all your lover's revenge\nYou and me could write a bad romance\nOh, caught in a bad romance\nOh, caught in a bad romance\nRa, ra, ah-ah-ah\nRoma, roma-ma\nGaga, ooh, la, la\nWant your bad romance\nRa, ra, ah-ah-ah\nRoma, roma-ma\nGaga, ooh, la, la\nWant your bad romance\nWalk, walk, fashion, baby\nWork it, move that bitch crazy\nWalk, walk, fashion, baby\nWork it, move that bitch crazy\nWalk, walk, fashion, baby\nWork it, move that bitch crazy\nWalk, walk, passion, baby\nWork it, I'm a free bitch, baby\nI want your love and I want your revenge\nI want your love, I don't wanna be friends\nJe veux ton amour et je veux ta revanche\nJe veux ton amour, I don't wanna be friends (Oh)\n(I want your bad romance, I want your bad romance)\nNo, I don't wanna be friends (Caught in a bad romance)\n(I want your bad romance, I want your bad romance)\nI don't wanna be friends (Oh)\n(I want your bad romance, I want your bad romance)\nWant your bad romance (I want your bad romance)\nCaught in a bad romance\nWant your bad romance\nI want your love and I want your revenge\nYou and me could write a bad romance (Oh)\nI want your love and all your lover's revenge\nYou and me could write a bad romance\nOh (Want your bad romance)\nCaught in a bad romance (Want your bad romance)\nOh (Want your bad romance)\nCaught in a bad romance\nRa, ra, ah-ah-ah\nRoma, roma-ma\nGaga, ooh, la, la\nWant your bad romance\n",
        .url = "https://drive.google.com/uc?export=open&id=1XOlk2SbEp8YEQSIm0kLlmyQ59m9WJgfw",
        .duration = 5.08014,
        .writerType = 0
    },
    {
        .title = "Judas - Lady Gaga",
        .lyrics = "Woah-woah-woah-woah-woah\nI'm in love with Juda-as, Juda-as\nWoah-woah-woah-woah-woah\nI'm in love with Juda-as, Juda-as\nJudas, Juda-ah-ah\nJudas, Juda-ah-ah\nJudas, Juda-ah-ah\nJudas, Gaga\nJudas, Juda-ah-ah\nJudas, Juda-ah-ah\nJudas, Juda-ah-ah\nJudas, Gaga\nWhen he calls to me, I am ready\nI'll wash his feet with my hair if he needs\nForgive him when his tongue lies through his brain\nEven after three times, he betrays me\n(Ah-ah, ah-ah, ah-ah, ah-ah)\nI'll bring him down, bring him down, down\n(Ah-ah, ah-ah, ah-ah, ah-ah)\nA king with no crown, king with no crown\nI'm just a Holy Fool, oh, baby, it's so cruel\nBut I'm still in love with Judas, baby\nI'm just a Holy Fool, oh, baby, it's so cruel\nBut I'm still in love with Judas, baby\nWoah-woah-woah-woah-woah\nI'm in love with Juda-as, Juda-as\nWoah-woah-woah-woah-woah\nI'm in love with Juda-as, Juda-as\nJudas, Juda-ah-ah\nJudas, Juda-ah-ah\nJudas, Juda-ah-ah\nJudas, Gaga\nI couldn't love a man so purely\nEven prophets forgave his goofy way\nI've learned love is like a brick, you can\nBuild a house or sink a dead body\n(Ah-ah, ah-ah, ah-ah, ah-ah)\nI'll bring him down, bring him down, down\n(Ah-ah, ah-ah, ah-ah, ah-ah)\nA king with no crown, king with no crown\nI'm just a Holy Fool, oh, baby, it's so cruel\nBut I'm still in love with Judas, baby\nI'm just a Holy Fool, oh, baby, it's so cruel\nBut I'm still in love with Judas, baby\nWoah-woah-woah-woah-woah\nI'm in love with Juda-as, Juda-as\nWoah-woah-woah-woah-woah\nI'm in love with Juda-as, Juda-as\n*Ew*\nIn the most Biblical sense (Ew)\nI am beyond repentance\nFame, hooker, prostitute, wench\nVomits her mind\nBut in the cultural sense\nI just speak in future tense\nJudas, kiss me, if offenced\nOr wear ear condom next time\nI wanna love you\nBut something's pulling me away from you\nJesus is my virtue\nAnd Judas is the demon I cling to\nI cling to\nJust a Holy Fool, oh, baby, it's so cruel\nBut I'm still in love with Judas, baby\nI'm just a Holy Fool, oh, baby, it's so cruel (Just a Holy Fool, woah-oh-oh)\nBut I'm still in love with Judas, baby\nWoah-woah-woah-woah-woah (Woah)\nI'm in love with Juda-as, Juda-as (Juda-a-as)\nWoah-woah-woah-woah-woah (Woah)\nI'm in love with Juda-as, Juda-as (Juda-a-as)\nJudas, Juda-ah-ah\nJudas, Juda-ah-ah\nJudas, Juda-ah-ah\nJudas, Gaga\n",
        .url = "https://drive.google.com/uc?export=open&id=1Jo37Lza4Z8caEuWU0mmm1GTdLY1reMye",
        .duration = 4.07001,
        .writerType = 0
    },
    {
        .title = "If U Seek Amy - Britney Spears",
        .lyrics = "La, la, la, la, la-la, la, la\nLa, la, la, la, la-la, la, la\nLa, la, la, la, la-la, la, la\nLa, la, la, la, la-la, la, la\nOh, baby, baby, have you seen Amy tonight?\nIs she in the bathroom? Is she smoking up outside? ( Oh )\nOh, baby, baby, does she take a piece of lime\nFor the drink that I'ma buy her? Do you know just what she likes? (Oh)\nOh, oh, tell me, have you seen her?\n'Cause I'm so, oh, I can't get her off of my brain\nI just wanna go to the party she gon' go\nCan somebody take me home? Ha-ha, hee-hee, ha-ha, ho\nLove me, hate me, say what you want about me\nBut all of the boys and all of the girls are begging to, if you seek Amy\nLove me, hate me, but can't you see what I see?\nAll of the boys and all of the girls are begging to, if you seek Amy\n(Love me, hate me)\nLa, la, la, la, la-la, la, la\nLa, la, la, la, la-la, la, la\nAmy told me that she's gonna meet me up\nI don't know where or when, and now they're closing up the club ( Oh )\nI've seen her once or twice before, she knows my face\nBut it's hard to see with all the people standing in the way ( Oh )\nOh, oh, tell me, have you seen her?\n'Cause I'm so, oh, I can't get her off of my brain\nI just wanna go to the party she gon' go\nCan somebody take me home? Ha-ha, hee-hee, ha-ha, ho\nLove me, hate me, say what you want about me\nBut all of the boys and all of the girls are begging to, if you seek Amy\nLove me, hate me, but can't you see what I see?\nAll of the boys and all of the girls are begging to, if you seek Amy\n(Love me, hate me) Oh, say what you want about me\nOh, but can't you see what I see?\n(Love me, hate me) Yeah, say what you want about me\nSo tell me if you see her\n( Let me know what she was wearing and what she was like )\n'Cause I've been waiting here forever\n( Let me know if she was going out her mind )\nOh, baby, baby, if you seek Amy tonight ( Oh )\nOh, baby, baby, we'll do whatever you like\nOh, baby, baby, baby\nOh, baby, baby, baby\nLa, la, la, la, la-la, la, la\nLa, la, la, la, la-la, la, la\nLa, la, la, la, la-la, la, la\nLa, la, la, la, la-la, la, la\nLove me, hate me, say what you want about me\nBut all of the boys and all of the girls are begging to, if you seek Amy\nLove me, hate me, but can't you see what I see?\nAll of the boys and all of the girls are begging to, if you seek Amy\nLove me, hate me, say what you want about me (Yeah, yeah)\nLove me, hate me, but can't you see what I see?\nAll of the boys and all of the girls are begging to, if you seek Amy\n(Love me, hate me) Oh, say what you want about me\nOh, but can't you see what I see?\n(Love me, hate me) Yeah, say what you want about me\nAll of the boys and all of the girls are begging to, if you seek Amy\n",
        .url = "https://drive.google.com/uc?export=open&id=1rbxz8r20K8FFt5_aDJDp6cFwu9TwuZwP",
        .duration = 3.46006,
        .writerType = 0
    },
    {
        .title = "I Kissed a Girl - Katy Perry",
        .lyrics = "This was never the way I planned\nNot my intention\nI got so brave, drink in hand\nLost my discretion\nIt's not what I'm used to\nJust wanna try you on\nI'm curious for you\nCaught my attention\nI kissed a girl and I liked it\nThe taste of her cherry ChapStick\nI kissed a girl just to try it\nI hope my boyfriend don't mind it\nIt felt so wrong, it felt so right\nDon't mean I'm in love tonight\nI kissed a girl and I liked it\nI liked it\nNo, I don't even know your name\nIt doesn't matter\nYou're my experimental game\nJust human nature\nIt's not what good girls do\nNot how they should behave\nMy head gets so confused\nHard to obey\nI kissed a girl and I liked it\nThe taste of her cherry ChapStick\nI kissed a girl just to try it\nI hope my boyfriend don't mind it\nIt felt so wrong, it felt so right\nDon't mean I'm in love tonight\nI kissed a girl and I liked it\nI liked it\nUs girls, we are so magical\nSoft skin, red lips, so kissable\nHard to resist, so touchable\nToo good to deny it\nAin't no big deal, it's innocent\nI kissed a girl and I liked it\nThe taste of her cherry ChapStick\nI kissed a girl just to try it\nI hope my boyfriend don't mind it\nIt felt so wrong, it felt so right\nDon't mean I'm in love tonight\nI kissed a girl and I liked it\nI liked it\n",
        .url = "https://drive.google.com/uc?export=open&id=1o0-CpP0r-ATeY_EDvYAaehfi4--4_yii",
        .duration = 3.04004,
        .writerType = 0
    },
    {
        .title = "...Baby One More Time - Britney Spears",
        .lyrics = "Oh, baby, baby\nOh, baby, baby\n\nOh, baby, baby\nHow was I supposed to know\nThat somethin' wasn't right here?\nOh, baby, baby\nI shouldn't have let you go\nAnd now you're out of sight, yeah\n\nShow me how you want it to be\nTell me, baby, 'cause I need to know now\nOh, because\n\nMy loneliness is killin' me (And I)\nI must confess, I still believe (Still believe)\nWhen I'm not with you, I lose my mind\nGive me a sign\nHit me, baby, one more time\n\nOh, baby, baby\nThe reason I breathe is you (Oh yeah)\nBoy, you got me blinded\nOh, pretty baby\nThere's nothing that I wouldn't do\nIt's not the way I planned it\n\nShow me how you want it to be\nTell me, baby, 'cause I need to know now\nOh, because\n\nMy loneliness is killin' me (And I)\nI must confess, I still believe (Still believe)\nWhen I'm not with you, I lose my mind\nGive me a sign\nHit me, baby, one more time\n\nOh, baby, baby (Oh)\nOh, baby, baby (Yeah, yeah)\n\nOh, baby, baby\nHow was I supposed to know?\nOh, pretty baby\nI shouldn't have let you go\nI must confess that my loneliness is killin' me now\nDon't you know I still believe\nThat you will be here and give me a sign?\nHit me, baby, one more time\n\nMy loneliness is killin' me (And I)\nI must confess, I still believe (Still believe)\nWhen I'm not with you, I lose my mind\nGive me a sign\nHit me, baby, one more time\n\nI must confess that my loneliness (My loneliness is killin' me)\nIs killin' me now (I must confess, I still believe)\nDon't you know I still believe (When I'm not with you, I lose my mind)\nThat you will be here and give me a sign?\nHit me, baby, one more time\n",
        .url = "https://drive.google.com/uc?export=open&id=1VbU4WSPnkc-0szpSDAmLeKa96VebmK2U",
        .duration = 3.30002,
        .writerType = 0
    },
    { .title = NULL, .lyrics = NULL, .url = NULL, .duration = 0, .writerType = 0}
};

/* ==================== DISPATCH TABLE ==================== */

songMetaData *genreSongArrays[] = {
    NULL,
    myTherapySession,
    adhdSongs,
    nwobhmSongs,
    whiteGirlPopSongs
};

/* ==================== SPECIAL SONGS (rickroll, roast, MeaLux) ==================== */
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

char *rickrollAudio = "https://drive.google.com/uc?export=open&id=10aiePDkX2t2XruCaFROIum9nGkQGUzth";
char *roast = "https://drive.google.com/uc?export=open&id=1LXCZzIJ_LR9Thw8-M2oVnKfYCHUn5obY";
char *meaLuxAudio = "https://drive.google.com/uc?export=open&id=1oG209grBSgu_q6MxCbw-L9wMNBw5HKOe";

/* Genres list (keep for menu) */
genreMetaData allGenres[] = {
    {
        .genre = "2000s Emo Music",
        .songs = myTherapySession,
        .songCount = sizeof(myTherapySession) / sizeof(myTherapySession[0]) - 1
    },
    {
        .genre = "2000s Nightcore ADHD",
        .songs = adhdSongs,
        .songCount = sizeof(adhdSongs) / sizeof(adhdSongs[0]) - 1
    },
    {
        .genre = "New Wave of British Heavy Metal",
        .songs = nwobhmSongs,
        .songCount = sizeof(nwobhmSongs) / sizeof(nwobhmSongs[0]) - 1
    },
    {
        .genre = "White Girl Pop",
        .songs = whiteGirlPopSongs,
        .songCount = sizeof(whiteGirlPopSongs) / sizeof(whiteGirlPopSongs[0]) - 1
    }
};


#endif // SONGDATA_H
