#include "visualizer.h"
#include "fft.h"
#include <math.h>
#include <string.h>

void spectrum_init(Spectrum *spectrum)
{
    *spectrum = (Spectrum){0};
    /* Periodic Hann for an N-point FFT: one period, without repeating an endpoint. */
    for (size_t i = 0; i < AUDIO_ANALYSIS_FRAMES; ++i) {
        spectrum->window[i] = 0.5 - 0.5 * cos(6.28318530717958647692 * i / AUDIO_ANALYSIS_FRAMES);
        spectrum->window_sum += spectrum->window[i];
    }
}

static double power_at(const double *power, double frequency)
{
    double bin = frequency * AUDIO_ANALYSIS_FRAMES / AUDIO_SAMPLE_RATE;
    if (bin <= 1) { return(power[1]); }
    if (bin >= AUDIO_ANALYSIS_FRAMES / 2) { return(power[AUDIO_ANALYSIS_FRAMES / 2]); }
    size_t lower = (size_t)bin;
    double fraction = bin - lower;
    return(power[lower] + (power[lower + 1] - power[lower]) * fraction);
}

bool spectrum_update(Spectrum *spectrum, const AudioSamples *samples)
{
    bool reset = spectrum->generation != samples->generation || samples->played_frames < spectrum->played_frames;
    if (reset || !samples->ready) {
        bool changed = spectrum->ready || reset;
        memset(spectrum->levels, 0, sizeof spectrum->levels);
        memset(spectrum->peaks, 0, sizeof spectrum->peaks);
        spectrum->generation = samples->generation;
        spectrum->played_frames = 0;
        spectrum->ready = false;
        if (!samples->ready) { return(changed); }
    }
    if (spectrum->ready && spectrum->played_frames == samples->played_frames) { return(false); }

    double power[AUDIO_ANALYSIS_FRAMES / 2 + 1] = {0};
    complexNum bins[AUDIO_ANALYSIS_FRAMES];
    for (size_t channel = 0; channel < 2; ++channel) {
        double mean = 0;
        for (size_t i = 0; i < AUDIO_ANALYSIS_FRAMES; ++i) { mean += samples->pcm[i][channel]; }
        mean /= AUDIO_ANALYSIS_FRAMES;
        for (size_t i = 0; i < AUDIO_ANALYSIS_FRAMES; ++i) {
            bins[i] = (complexNum){(samples->pcm[i][channel] - mean) * spectrum->window[i], 0};
        }
        if (fft(bins, AUDIO_ANALYSIS_FRAMES, 0) < 0) { return(false); }
        for (size_t i = 1; i <= AUDIO_ANALYSIS_FRAMES / 2; ++i) {
            power[i] += (bins[i].re * bins[i].re + bins[i].im * bins[i].im) * 0.5;
        }
    }

    double dt = spectrum->ready ? (double)(samples->played_frames - spectrum->played_frames) / AUDIO_SAMPLE_RATE : 1;
    double attack = 1 - exp(-dt / 0.015), release = 1 - exp(-dt / 0.18);
    for (size_t band = 0; band < SPECTRUM_BANDS; ++band) {
        double low = 20 * pow(1000, (double)band / SPECTRUM_BANDS);
        double high = 20 * pow(1000, (double)(band + 1) / SPECTRUM_BANDS);
        size_t first = (size_t)ceil(low * AUDIO_ANALYSIS_FRAMES / AUDIO_SAMPLE_RATE);
        size_t last = (size_t)ceil(high * AUDIO_ANALYSIS_FRAMES / AUDIO_SAMPLE_RATE);
        if (first < 1) { first = 1; }
        if (last > AUDIO_ANALYSIS_FRAMES / 2 + 1) { last = AUDIO_ANALYSIS_FRAMES / 2 + 1; }
        /* Treat the FFT as a piecewise-linear power spectrum. Narrow bass bands
         * may contain no bin centers, but their interpolated edges still carry
         * energy. Wider bands retain their interior peaks and calibration. */
        double maximum = power_at(power, low);
        double upper = power_at(power, high);
        if (upper > maximum) { maximum = upper; }
        for (size_t i = first; i < last; ++i) {
            if (power[i] > maximum) { maximum = power[i]; }
        }
        double amplitude = 2 * sqrt(maximum) / spectrum->window_sum;
        double level = amplitude > 0 ? (20 * log10(amplitude) + 72) / 72 : 0;
        if (level < 0) { level = 0; }
        if (level > 1) { level = 1; }
        double previous = spectrum->levels[band];
        spectrum->levels[band] = (float)(previous + (level - previous) * (level > previous ? attack : release));
        if (!level && spectrum->levels[band] < 0.001f) { spectrum->levels[band] = 0; }
        float peak = spectrum->peaks[band] - (float)(dt * 0.45);
        spectrum->peaks[band] = peak > spectrum->levels[band] ? peak : spectrum->levels[band];
    }
    spectrum->generation = samples->generation;
    spectrum->played_frames = samples->played_frames;
    spectrum->ready = true;
    return(true);
}
