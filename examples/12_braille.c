#include <stdio.h>
#include <math.h>
#include "../src/include/CTerminalPlotLib.h"

// High-resolution Braille rendering. A Braille cell holds a 2x4 dot grid, so the
// same canvas size resolves 8x more detail. Toggle with ctp_set_graph_braille.
//
// We plot one period of a sine wave twice — once with block-glyph strokes, once
// in Braille — at the same chart size, to show the resolution jump.
int main(void)
{
    const int N = 60;
    DataSet *ds = ctp_initialize_dataset(2, 20, 64);

    CTP_PARAM sinv[64], t[64];
    for (int i = 0; i < N; i++)
    {
        double x = (double)i / (N - 1) * 6.2831853; // 0 .. 2*pi
        sinv[i] = (CTP_PARAM)sin(x);
        t[i] = (CTP_PARAM)x;
    }
    ctp_add_column(ds, "t", t, N);      // column 0 -> horizontal (X)
    ctp_add_column(ds, "sin", sinv, N); // column 1 -> vertical (Y)

    int y[] = {1};
    ctp_select_axes(ds, 0, y, 1); // plot sin (Y) against t (X)

    printf("Block resolution:\n");
    ctp_plot_line(ds);

    printf("\nBraille resolution (8x):\n");
    ctp_set_graph_braille(ds, true);
    ctp_plot_line(ds);

    ctp_free_dataset(ds);
    return 0;
}
