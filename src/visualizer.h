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
/* 20 Hz..20 kHz logarithmic bands, -72..0 dBFS, stereo power combined after FFT.
 * Interpolated band edges prevent artificial holes below the FFT bin spacing.
 * Returns true when the display changed. Paused/identical snapshots are held. */
bool spectrum_update(Spectrum *spectrum, const AudioSamples *samples);

#endif
