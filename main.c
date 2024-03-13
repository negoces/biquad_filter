#include "biquad.h"
#include <stdio.h>

#define FILTER_NUM 2

/// https://en.wikipedia.org/wiki/Octave_band
/// https://sengpielaudio.com/calculator-bandwidth.htm

int main()
{
    FILE* fp_in  = fopen("./bb_ch1_16000_s16le.h.pcm", "rb");
    FILE* fp_out = fopen("./bb_ch1_16000_s16le.eq.pcm", "wb");
    short in     = 0;
    short out    = 0;
    if (fp_in) {
        printf("File: %p %p\n", fp_in, fp_out);
        biquad_filter* filter[FILTER_NUM] = { NULL };

        filter[0] = biquad_init_filter(1.32, 16000, +3, 505);
        filter[1] = biquad_init_filter(1.02, 16000, +6, 1450);

        while (fread(&in, 2, 1, fp_in)) {
            out = in;
            for (int i = 0; i < FILTER_NUM; ++i) {
                out = biquad_apply(out, filter[i]);
            }
            fwrite(&in, 2, 1, fp_out);
            fwrite(&out, 2, 1, fp_out);
        }
        fclose(fp_in);
        fclose(fp_out);
        printf("Done.\n");
    }
    return 0;
}
