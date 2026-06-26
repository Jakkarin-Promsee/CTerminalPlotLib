#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

// The shortest path to a plot: one call per data series.
// No 2-D arrays, no max_row juggling.
int main(void)
{
    DataSet *ds = ctp_initialize_dataset(/*max_cols*/ 3, /*max_name*/ 20, /*max_rows*/ 16);

    CTP_PARAM x[]  = {-3, -2, -1, 0, 1, 2, 3};
    CTP_PARAM y1[] = {-3, -2, -1, 0, 1, 2, 3};
    CTP_PARAM y2[] = { 9,  4,  1, 0, 1, 4, 9};

    ctp_add_column(ds, "x", x, 7);
    ctp_add_column(ds, "y = x", y1, 7);
    ctp_add_column(ds, "y = x^2", y2, 7);

    ctp_plot(ds); // table + scatter

    ctp_free_dataset(ds);
    return 0;
}
