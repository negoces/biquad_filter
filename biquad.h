#ifndef EQ_BIQUAD_H
#define EQ_BIQUAD_H

#define FIX_POINT (1)
#define SHIFT_BIT (12)

typedef struct biquad_filter {
#if FIX_POINT
    int b0;
    int b1;
    int b2;
    int a1;
    int a2;
#else
    float b0;
    float b1;
    float b2;
    float a1;
    float a2;
#endif
    /// x1, y1, x2, y2, ... x4, y4
    short        buf[8];
    unsigned int buf_pos;
} biquad_filter;

short biquad_apply(short in, biquad_filter* filter);

biquad_filter* biquad_init_filter(double Q, int sample_rate, int gainDB, int center_freq);

#endif // EQ_BIQUAD_H
