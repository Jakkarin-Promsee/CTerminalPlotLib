#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

// Choose which column is the X axis (the shared, independent axis) and which
// columns are the Y series. Column 0 is X by default; here we keep that and pick
// which Y columns to draw.
int main(void)
{
    DataSet *ds = ctp_initialize_dataset(4, 20, 16);

    CTP_PARAM x[]  = {-3, -2, -1, 0, 1, 2, 3};
    CTP_PARAM y1[] = {-3, -2, -1, 0, 1, 2, 3}; // y = x
    CTP_PARAM y2[] = {3, 2, 1, 0, -1, -2, -3};  // y = -x
    CTP_PARAM y3[] = {9, 4, 1, 0, 1, 4, 9};     // y = x^2

    ctp_add_column(ds, "x", x, 7);
    ctp_add_column(ds, "y = x", y1, 7);
    ctp_add_column(ds, "y = -x", y2, 7);
    ctp_add_column(ds, "y = x^2", y3, 7);

    // X is column 0; plot the Y series in columns 1 (y = x) and 3 (y = x^2),
    // skipping column 2.
    int y_series[] = {1, 3};
    ctp_select_axes(ds, 0, y_series, 2);

    ctp_plot(ds);

    ctp_free_dataset(ds);
    return 0;
}
