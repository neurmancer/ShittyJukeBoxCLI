# SHITTY JUKEBOX IS NOW IN ITS OWN REPO  
- ShittyJUKEBOX REPO NOW OFFICIAL BABYYYY (And since I update readmes without an editor and at fucking 3AMish don't expect shakespeareian english or not even proper englsih)
- Current Version V4.5

## Table of Content (I start liking this shit.)

* [Features](#features)
* [In Development Features](#in-development-features)
* [Tweaks](#tweaks-where-i-brag-about-shit-for-the-nerds)
* [Q&A](#qa)
* [Compile&Run](#compiling--running)
* [Depenencies](#dependencies)
* [OUTDATED Windows port](#winportsux-for-v2-outdated-af)
* [Third-party requirements and legal shit(paperwork)](#third-party-app-needs-and-licenses-will-be-here-and-yeah-this-title-is-still-too-fucking-long-but-i-refuse-to-shorten-it-and-its-self-aware-of-the-situation)

## FEATURES

# NOW ALL THE PLAYLISTS ARE WITH AUDIO!! FUCK YEAH (V4.5) 

- 10emo/rock songs (with 2 included metallica songs)
- 11 Nightcore songs from early 2000s or smthg(I've been born in 2006 don't judge I wasn't even alive) 
- 10 NWOBHM songs for you to feel the speed metal/golden era in your bones (just with lyrics for now)
- A jukebox ASCII Art at the bottom of code for the vibes and lyrics on the top of the code (now in an array for the nerds)
- Interactive loop (unless you fuck up pressing digits then it roasts you)
- %25 chance of getting rickrolled as you try to exit and bricks Ctrl+C 'till it is done
- Unfortunetly not uses prime numbers to determine if you want RGB or not anymore since I've learned structs
- Now apperantly it got it's own windows branch which I'll provide with .exe file too since Win users don't know what a compiler is. but it is in another repo and outdated for now (link will be at the end for the old win repo)
- WindowsPORT has it's own branch called winPORTsux which I won't merge(and since I started to C 10 days ago. That port is possible thanks to Grok(taught me what IFNDEF is))
- Now with a ASCII JUKEBOX in Genre Menu (fullscreen advised by the developer, me, yk. I'd do as I say if I were you)
- Got all the songs hard-coded since I don't trust .so and ESPECIALLY .dlls (fuk u windows)
- New features will be added 'cuz why not?

- **3 Types of lyrics printing option**
- - RGB/Pale white choice for emo songs (and for you nerds check the periodic sin fade...I'll make you mathgasm for sure.)
- - Forced RGB for Nightcore for obvious reasons
- - Forced Bold white for NWOBHM songs   
- - Slightly better dynamic lyrics timing (but still in dev)



## In Development Features
- **Finished parts**
- - One more genre category : **DONE**  
- - Songs will be summed up to 10 + 10 songs in the new genre. **DONE** (added +1 nightcore song 18.02.2026)
- - CLI music playing **FUCKING DONE**
- - Better optimization **DONE**  

> **Still in Dev**
> - 'Headerlessness' (yeah I made that word up)
> - Windows port(still fuk u windows I am doing it to learn. Purely pragmatistic reasons) Postponed 'till I feel like it
> - Lyrics sync ups with songs(total length not word by word, I am not fucking spotify) *In Progress*
> - Shitty prime idea just got evolved and I got batshit crazy ideas for syncing

### TWEAKS (Where I brag about shit for the Nerds)

- Now it's not leaving ghost lyrics on top of the terminal (Used a cutting-edge tech called ANSI lol)
- The typing speed adjustments 
- Better exit state handling and more optimized loop
- 1 second between end of the song and genre menu to optimize reaction time on old PCs
- Reduced reaction time 
- Better verse seperation (only for NWOBHM for now) (and yeah I can misspell shit since it's 3.15AM)
- I made it literally plug and play if you know how dispatch tables are working you can plug your own urls,titles,lyrics and it will run just fine. I single handedly achieved object omnipotence(I'LL BRAG OFC THIS IS MY 13TH DAY OF PROGRAMMING. I FEEL LIKE A CYBER-GOD RN)

## QA

- Q: When will that be done?
- A: Never...when I add shit other ideas are coming to my mind and I grind to add them too

- Q:Is this a malware
- A:Depends...malware for your brain but no not as a malware for computers since I don't know how to script them yet.

- Q:Do I need to install anything else
- A:Yes ffmpeg for ffplay...-if needed- other dependencies will be added to here if needed

- Q: Why are you doing this to yourself?
- A:Bro you are talking to yourself in README pull yourself together.


## Compiling & Running

```bash
#To compile use:
cc/gcc ShittyJukeBox -o filename.out -lm #for math lib
#To run
./filename.o #Then have fun
```

## Dependencies

```bash

#For ubuntu/debian 
sudo apt-get update && apt-get upgrade && apt-get install ffmpeg 

#For Arch (I use NyArch btw OwO)
sudo pacman -Syu && pacman -S ffmpeg 

#For Fedora
sudo dnf upgrade && dnf install ffmpeg 

```



## winPORTsux for V2 (OUTDATED AF) 
- https://github.com/neurmancer/Basic-C-Examples/tree/winPORTsux/thingsThatSupposedToBasicButOverengineeredForNothing -> In the old repo will be added to here winPORTsux branch (Which I won't merge with main since it's sucks) when V4 be ready for first Linux then I'll port it to windows
- Yes it'll have .exe I know you can't compile for shit.


## THIRD-PARTY APP NEEDS AND LICENSES WILL BE HERE and yeah this title is still too fucking long but I refuse to shorten it and it's self-aware of the situation


 - **ffplay** (from FFmpeg): The poor bastard I yeet into a child process to actually play the songs because writing my own audio decoder sounded like too much work at 5 AM and my 12th day of programming.  
  I don't own FFmpeg(fuck I wish I had tho), they don't own me(not physically but my soul is already theirs), but I do owe them my firstborn for making -nodisp -autoexit exist.

  License: LGPL 2.1 or later (or GPL if you hate freedom). Go stare at https://ffmpeg.org/ if you're gonna get nerdy.

  Usage: forked + execlp abuse + zero mercy. If it cracks and dies, blame Google drive not ffplay (nor my code)

- **Google Drive links**: Yeah I'm streaming mp3s straight from my own cursed Drive folder because fuck local files and fuck .dlls even more.  

  Not affiliated, not endorsed, probably violates 47 different TOS clauses. Don't @ me, it's for educational purposes (my education in not getting DMCA'd... yet). And don't act like you never used Napster or shit I mean this is EMO Jukebox made by a sleep-deprived teenager what you expect (I can't even afford a new keyboad lol)

- If you are one of the artists that I yoinked your music to stream: 
- - First, man... I fucking love your music thx for existing.
- - Second, how tf you even find that repo?
- - Third sorry for yoinking OwO (I'd delete if you catch me lulz)

- If you are one of the record label that I yoinked the music you 'own' to stream: 
- - Simply go fuck yourselves.

- No other third-party bullshit required right now.  
  If I ever add real audio libs or some cursed ncurses dependency, I'll update this wall of text. Until then: pure C + vibes + war crimes against buffering.

**Legal disclaimer for the nerds who actually read this:**  
This project uses ffplay in the most cursed way possible. If FFmpeg foundation comes knocking, tell 'em Arda said "thanks for the -loglevel 8 flag, it saved my ass".  

If **Rick Astley** finds out about the rickroll brick, tell him I love him platonically and still have a cassette of 'Whenever You Need Somebody' album.

