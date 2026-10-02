# SHITTY JUKEBOX Ω² NOW OFFICAL BABYYYYYYYYYYY


> _I'll add shit here once I am happy with everything else_
> You remember the original Shitty Jukebox right? **THIS IS EVEN CRAZIEEEEEEEEEEEEEER** AND yup my english is still as bad as before 

> **WHY Ω²?** 'Cuz squared the resistance against DMCA, squared the entropy, squared the song amount, squared the overengineering and squared the sleep-deprivation 


> _so without furher shit let's start bragging_

> _'Sup?_ it's exactly 1.53AM and you know what that means...DOCUMENTATION TIMEEEE 


--- 

## Table of Contents 

- [You are here](#table-of-contents)
- [Bragging](#bragging)
- [LOOK AT THE FUCKING SONGS](SONGS.md)
- [Niche Shit](oled_support/README.md)
- [In Dev](#in-dev)
- [Q&A](#qa)
- [Compile&Shit](#how-to-run)
- [Legal Stuff](#legal-stuff)

---


## Bragging

**BEHOLD** This is the bad-assest Shitty Jukebox ever existed (well it lowkey just works rn but there is a fucking long way to go)

- using termios and native hjkl terminal movement for selections and enter but also supports arrow keys for non-terminal-goblins

- A dope ass screen for playing song with title, audio and display shit with states that you have control over

- I own the fucking AUIDO PLUMBING BACKEND NOW rather than saying 'GO GET THE FUCKING SONG FFPLAY' 

- using databases like a civilized app instead of my old brute-the-fucking-force-everything-with-pointers approach

- Well...still uses the cutting edge tech called ANSI(I'll **NEVER** get sick of this joke) 

- THE OG TYPEWRITERS ARE **HEREEEEEEEEEEE**
> 1. `Plain white (BOLD)`
> 2. `Epilespy Seizure RGB mathgasm`
> 3. `BOLD PICK YOUR COLOR TYPESHIT`
> And they're fucking states that you can change when the fuckever you want

- there is a hotkey menu down below on terminal so you never have to guess which key does what(spoiler alert scroll works too) 

- THE FUCKING ASCII JUKEBOX HAVE RETURNED 

- Audio visualizer using FFT 

- Song search feature added

- The qeuueueue feature is also here

- WELL...I _lowkey_ overengineered the shuffle with fisher-yates and CSPRNG but it worths lol...

- **BETA FEATURE**: PLAYLIST CREATION ADDED NOW USERS CAN GROUP SONGS HOW THE FUCK THEY WANT and add custom playlist covers (local or online)

- Well...we don't have SkyNet anymore...

- Mr. Rick Astley kindly got evacuated to the ring 0 and does not rickroll the user on exit (He did, in fact, give up)

- Now there is a tiny `theme.lua file` to customize TUI's primary colors

- I mean...**YOU FUCKING GOTTA TRY IT TO UNDERSTAND MY HYPE BRO** go fucking [compile](#how-to-run)

- playerCTL full-ish support

---


## In Dev

- [X] Song database recreation

- [ ] Lyrics sync (AND TRUST ME THIS TIME IT'LL BE FUCKING PERFECT) (I AM BETTER THAN FUCKING SPOTIFY (at least morally 'cuz I don'p put the lyrics behind a fucking paywall)(yeah if old me gets the dibs on dispatch tables as Cyber-god status I AM FUCKING REINCARNATION OF KEN THOMPSON))

- [X] AUDIO VISUALIZER

- [X] DATABASE MIGRATION AND CLEAN UP

- [X] Cover art display 

- [X] MPRIS + playerctl support 

- [X] Offline mode support (via local files or downloading the genres)

- [X] There is a known bug(haunting) going on with `In The Heat of The Night` where it plays perfectly when selected but aborts and terminates
the process when you hit on it on queueueue untill I find why, I declare it is haunted (same goes for a few more songs)

- [ ] rewind, fastforward and jump to wanted line feature (on stand-by till I get my left arm's functionality I guess...)


---

## Q&A

> _Where is my mind noises intesifies_

> Q: What do I need to run this?
> A: FFMPeg and SDL on the system and a c compiler for now... and I advise using kitty emulator for best experience(were I you, I'd do as the dev says...yk me...I am the dev)

> Q: You promoted kitty all the time in readme is that an ad? Are they paying you? 
> A: Nope the image rendering capability is enough payment to me

> Q: Why termios instead of ncurses you already started to use 3rd party libs?
> A: Yup...but I still wanna feel my fair share of questionable life choices and misery

> Q: Can I add my own songs?
> A: Yup! database comes with **my** inital songs and links but local lyrics, audio and shit works too

> Q: for whom are you writing this q&a sections?
> A: For you? 

> Q: Who?
> A: You?

> Q: Say it!
> A: Say what?

> Q: Just say it...
> A: We're the same person (__Where is my mind__ dın dın dın dıdı)

---

## How to run

> _Before running_ I advise using the kitty terminal emulator for the cover art feature support but other terminals are working just well for the rest

```sh

./build.sh # to build duh...

./sjb 

#or for system-wide installation

./build.sh --system    # requries sudo privs to accsess /usr/local/bin/ and dependency installations

man 1 sjb # for usage (if installed with --system)
```

## Legal stuff

here is the boring shit [LICENSE](LICENSE.md)

same thing but for 3rd party shit [3rd party LICENSES](THIRD_PARTY_LICENSE.md)