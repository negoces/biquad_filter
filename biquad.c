#include "biquad.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/// https://en.wikipedia.org/wiki/Digital_biquad_filter#Direct_form_1
short biquad_apply(short in, biquad_filter* filter)
{
    short* buffer               = (short*)filter->buf;
    buffer[filter->buf_pos & 7] = in;
    int yn                      = 0;

/// yn = b0 * xn + b1 * xn1 + b2 * xn2 - a1 * yn1 - a2 * yn2
#if FIX_POINT
    yn = filter->b0 * buffer[filter->buf_pos & 7] + filter->b1 * buffer[(filter->buf_pos - 2) & 7]
        + filter->b2 * buffer[(filter->buf_pos - 4) & 7] - filter->a1 * buffer[(filter->buf_pos - 1) & 7]
        - filter->a2 * buffer[(filter->buf_pos - 3) & 7];
    yn = yn >> SHIFT_BIT;
#else
    yn = (int)(filter->b0 * (float)(buffer[filter->buf_pos & 7])
               + filter->b1 * (float)(buffer[(filter->buf_pos - 2) & 7])
               + filter->b2 * (float)(buffer[(filter->buf_pos - 4) & 7])
               - filter->a1 * (float)(buffer[(filter->buf_pos - 1) & 7])
               - filter->a2 * (float)(buffer[(filter->buf_pos - 3) & 7]));
#endif

    yn = (yn > (short)0x7FFF) ? (short)0x7FFF : yn;
    yn = (yn < (short)0x8000) ? (short)0x8000 : yn;

    buffer[(filter->buf_pos + 1) & 7] = (short)yn;
    filter->buf_pos += 2;

    return (short)yn;
}

/// https://arachnoid.com/BiQuadDesigner/index.html
biquad_filter* biquad_init_filter(double Q, int sample_rate, int gainDB, int center_freq)
{
    biquad_filter* filter = (biquad_filter*)malloc(sizeof(biquad_filter));
    memset(filter, 0, sizeof(biquad_filter));

    double gain_abs = pow(10.0, (double)gainDB / 40.0);                               /// alias A
    double omega    = (double)2.0 * M_PI * (double)center_freq / (double)sample_rate; /// alias w0
    double sn       = sin(omega);                                                     /// alias sin(w0)
    double cs       = cos(omega);                                                     /// alias cos(w0)

    Q            = Q == 0 ? 1e-9f : Q;
    double alpha = sn / (2.0 * Q);
    double a0    = 1.0 + alpha / gain_abs;

#if FIX_POINT
    filter->a1 = (int)((-2.0 * cs) / a0 * (double)(1 << SHIFT_BIT));
    filter->a2 = (int)((1.0 - alpha / gain_abs) / a0 * (double)(1 << SHIFT_BIT));
    filter->b0 = (int)((1.0 + alpha * gain_abs) / a0 * (double)(1 << SHIFT_BIT));
    filter->b1 = (int)((-2.0 * cs) / a0 * (double)(1 << SHIFT_BIT));
    filter->b2 = (int)((1.0 - alpha * gain_abs) / a0 * (double)(1 << SHIFT_BIT));
#else
    filter->a1 = (float)((-2.0 * cs) / a0);
    filter->a2 = (float)((1.0 - alpha / gain_abs) / a0);
    filter->b0 = (float)((1.0 + alpha * gain_abs) / a0);
    filter->b1 = (float)((-2.0 * cs) / a0);
    filter->b2 = (float)((1.0 - alpha * gain_abs) / a0);
#endif
    return filter;
}
