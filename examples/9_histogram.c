#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

// Histogram — bin a single column of samples into equal-width buckets and draw
// the per-bin counts as bars. Pass the bin count to ctp_plot_histogram.
int main(void)
{
    DataSet *ds = ctp_initialize_dataset(1, 20, 32);

    // 20 samples clustered around the middle of the range.
    CTP_PARAM samples[] = {2, 3, 3, 4, 4, 4, 5, 5, 5, 5,
                           6, 6, 6, 7, 7, 8, 5, 4, 6, 5};
    ctp_add_column(ds, "samples", samples, 20);

    ctp_plot_histogram(ds, 8);

    ctp_free_dataset(ds);
    return 0;
}
