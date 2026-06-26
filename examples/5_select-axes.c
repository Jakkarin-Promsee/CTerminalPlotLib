#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

// Choose which column is the Y axis and which columns are the X series.
int main(void)
{
    DataSet *ds = ctp_initialize_dataset(4, 20, 16);

    CTP_PARAM y[]  = {-3, -2, -1, 0, 1, 2, 3};
    CTP_PARAM x1[] = {-3, -2, -1, 0, 1, 2, 3};
    CTP_PARAM x2[] = {3, 2, 1, 0, -1, -2, -3};
    CTP_PARAM x3[] = {9, 4, 1, 0, 1, 4, 9};

    ctp_add_column(ds, "y", y, 7);
    ctp_add_column(ds, "x1", x1, 7);
    ctp_add_column(ds, "x2", x2, 7);
    ctp_add_column(ds, "x3", x3, 7);

    // Plot column 0 (y) against columns 1 (x1) and 3 (x3); skip x2.
    int x_axes[] = {1, 3};
    ctp_select_axes(ds, 0, x_axes, 2);

    ctp_plot(ds);

    ctp_free_dataset(ds);
    return 0;
}
