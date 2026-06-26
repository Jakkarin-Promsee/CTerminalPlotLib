#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

// Histogram — bin a single column of samples into equal-width buckets and draw
// the per-bin counts as bars. Pass the bin count to ctp_plot_histogram.
//
// Here: daily order counts over four weeks. Most days land in the low-50s, with
// a few quiet and a few busy days — a rough bell shape once binned.
int main(void)
{
    DataSet *ds = ctp_initialize_dataset(1, 20, 32);

    CTP_PARAM daily_orders[] = {44, 47, 49, 50, 51, 52, 53, 54, 48, 46,
                                50, 52, 53, 55, 49, 51, 52, 54, 45, 50,
                                51, 56, 48, 53, 57, 50, 42, 59};
    ctp_add_column(ds, "daily orders", daily_orders, 28);

    ctp_plot_histogram(ds, 8);

    ctp_free_dataset(ds);
    return 0;
}
