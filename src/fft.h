#ifndef COMPLEX_FFT_H
#define COMPLEX_FFT_H

/* YEAH I STOLE AND USED MY OWN FFT lol*/

typedef struct {
    double re;
    double im;
} complexNum;

int fft(complexNum *x, int n, int inverse);
int fft_arbitrary(complexNum *x, int n, int inverse);

#endif
