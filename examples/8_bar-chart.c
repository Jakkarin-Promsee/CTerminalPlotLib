#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

// Bar chart — one vertical bar per row of a single value column. Bars share a
// zero baseline: positive values rise (green), negative values drop (red).
//
// No axis selection needed: with a single column, ctp_plot_bar uses column 0.
int main(void)
{
    DataSet *ds = ctp_initialize_dataset(1, 20, 16);

    CTP_PARAM profit[] = {3, 5, 2, -1, -3, 4, 6, 1}; // e.g. weekly profit
    ctp_add_column(ds, "profit", profit, 8);

    ctp_plot_bar(ds);

    ctp_free_dataset(ds);
    return 0;
}
