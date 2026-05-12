#ifndef SONGDATA_H         
#define SONGDATA_H

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




/* ==================== MY THERAPY SONGS  ==================== */
songMetaData myTherapySession[] = {0};


/* ==================== ADHD EXPLOSUION  SONGS ==================== */
songMetaData adhdSongs[] = {0};

/* ==================== NWOBHM SONGS ==================== */
songMetaData nwobhmSongs[] = {0};

/* ==================== WHITE GIRL POP ==================== */
songMetaData whiteGirlPopSongs[] = {0};

/* ==================== DISPATCH TABLE ==================== */

songMetaData *genreSongArrays[] = {
    NULL,
    myTherapySession,
    adhdSongs,
    nwobhmSongs,
    whiteGirlPopSongs
};

/* ==================== SPECIAL ONES (Yup me...no I didn't meanit...FUCK)  ==================== */
const char *rickroll = "";
char *rickrollAudio = "https://drive.google.com/uc?export=open&id=10aiePDkX2t2XruCaFROIum9nGkQGUzth";
char *roast = "https://drive.google.com/uc?export=open&id=1LXCZzIJ_LR9Thw8-M2oVnKfYCHUn5obY";
char *meaLuxAudio = "https://drive.google.com/uc?export=open&id=1oG209grBSgu_q6MxCbw-L9wMNBw5HKOe";

/* Genres list (keep for menu) */
genreMetaData allGenres[] = {0};
#endif // SONGDATA_H