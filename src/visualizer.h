#ifndef SHITTYJUKEBOX_VISUALIZER_H
#define SHITTYJUKEBOX_VISUALIZER_H

#include "audio_handler.h"

#define SPECTRUM_BANDS 32

typedef struct {
    float levels[SPECTRUM_BANDS];
    float peaks[SPECTRUM_BANDS];
    double window[AUDIO_ANALYSIS_FRAMES];
    double window_sum;
    uint64_t generation;
    uint64_t played_frames;
    bool ready;
} Spectrum;

void spectrum_init(Spectrum *spectrum);

bool spectrum_update(Spectrum *spectrum, const AudioSamples *samples);

#endif
