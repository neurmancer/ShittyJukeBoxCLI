# ShittyJukeBox Version: Ω²

## ToC

- [I ran out mythical creature names for recursion](#toc)
- [File System Plan](#fsp)
- [Original dev notes](#notes)


### FSP 

- This is where I'll put the filetree draft on my real notebook so I can continue to plan accordinghly

```text
    ShittyJukeBox/
        ShittyJukeBox.c
        Makefile
        build.sh
        src/
            audio_handler.c
            audio_handler.h
            lyircs_handler.c
            lyrics_handler.h
            terminal_handler.c
            terminal_handler.h
            TUI.c
            TUI.h
            database.c
            database.h
        config/
            config.lua    #Possible but no promises
     
```

### Notes

> the original notes on the dev branch of shitty jukebox

## Future of The ShittyJukebox


> This file is a kind of roadmap for future updates of the Shittyjukebox I'll refactor almost everything from the old code and old version will live in a 'Legacy' branch

### Planned Updates

> Basically everything so I'll just give you the new shit I am planning

> Getting rid of scanf with termios and non-canon mode or raw mode 
> Random song selection 
> Queue feature 
> Lyrics toggle on/off with a different approach to lyrics placement
> Pause/Play, Song length, current second, the usual UI of all music players yk
> Loop feature 
> More songs 
> and more shit as ideas drop 
> Lame ass elevator music as a joke for genre menu selection time
> My own audio encoder so if ffplay fucke me up I can still work gracefully 
(and I want playerctl to see my program as it is not as 'ffplay')(but still...I am not fucking spotify)



### Scope Creep 

> This part is where I go feral and say 'No promises' the 'Planned Updates' section is my vision for next
'stable' version before leaving project for a long time (like current Legacy version) 

> These are the possible future paths 
> Audio Visualizer using fourier transforms ('cuz yk...me)
> A modular design with lots of files instead of just 'main.c'
> A makefile to make user experience easier 
> A GUI version using raylib(I probably won't do it this started as a CLI passion and will end like that)
> This is a stretch I am aware but customization and ricing with Lua support
> > This requries it's own terminal emulator and Imma also build it don't worry bruh
> Spoiler: IoT shit support will eventually end up being here

#### REASON OF 'SCOPE CREEP' SECTION'S EXISTENCE

> I'm currently ricing a DWM setup under another repo which gonna power my CyberDeck and ShittyJukebox
is part of the Legacy and there will be shit that added for feral CyberGremlins like me 
(For example: CLI flags that allows physical media such as cassette tapes) 

