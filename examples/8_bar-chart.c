#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

// Bar chart — one vertical bar per row of a single value column. Bars share a
// zero baseline: positive values rise (green), negative values drop (red).
//
// Here: a small shop's monthly net profit ($k) over its first eight months —
// underwater in the winter, then climbing into the black.
//
// No axis selection needed: with a single column, ctp_plot_bar uses column 0.
int main(void)
{
    DataSet *ds = ctp_initialize_dataset(1, 20, 16);

    CTP_PARAM net_profit[] = {-6, -9, 3, 7, 5, 14, 18, 15};
    ctp_add_column(ds, "net profit", net_profit, 8);

    ctp_plot_bar(ds);

    ctp_free_dataset(ds);
    return 0;
}
