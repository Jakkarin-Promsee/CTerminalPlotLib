#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

// Line plot — connect each Y series into a continuous stroke against the shared
// X axis. Column 0 is the (horizontal) X axis; each selected Y column is a series
// of vertical positions.
//
// `line` traces y = x; `para` traces the parabola y = x^2.
int main(void)
{
    DataSet *ds = ctp_initialize_dataset(3, 20, 16);

    CTP_PARAM x[]    = {-3, -2, -1, 0, 1, 2, 3}; // shared horizontal axis
    CTP_PARAM line[] = {-3, -2, -1, 0, 1, 2, 3}; // a straight line  (y = x)
    CTP_PARAM para[] = { 9,  4,  1, 0, 1, 4, 9}; // a parabola       (y = x^2)

    ctp_add_column(ds, "x", x, 7);
    ctp_add_column(ds, "line", line, 7);
    ctp_add_column(ds, "para", para, 7);

    // X is column 0; plot the Y series in columns 1 (line) and 2 (para).
    int y_series[] = {1, 2};
    ctp_select_axes(ds, 0, y_series, 2);

    ctp_plot_table(ds);
    ctp_plot_line(ds);

    ctp_free_dataset(ds);
    return 0;
}
