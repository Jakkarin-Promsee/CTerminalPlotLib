#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

// Line plot — connect each X series into a continuous stroke, ordered by the
// shared Y axis. Same data shape as the scatter examples: column 0 is the
// (vertical) Y axis, each selected X column is a series of horizontal positions.
//
// `line` traces the diagonal x = y; `para` traces the sideways parabola x = y^2.
int main(void)
{
    DataSet *ds = ctp_initialize_dataset(3, 20, 16);

    CTP_PARAM y[]    = {-3, -2, -1, 0, 1, 2, 3}; // shared vertical axis
    CTP_PARAM line[] = {-3, -2, -1, 0, 1, 2, 3}; // a straight line  (x = y)
    CTP_PARAM para[] = { 9,  4,  1, 0, 1, 4, 9}; // a parabola       (x = y^2)

    ctp_add_column(ds, "y", y, 7);
    ctp_add_column(ds, "line", line, 7);
    ctp_add_column(ds, "para", para, 7);

    // Plot column 0 (y) against columns 1 (line) and 2 (para).
    int x_axes[] = {1, 2};
    ctp_select_axes(ds, 0, x_axes, 2);

    ctp_plot_table(ds);
    ctp_plot_line(ds);

    ctp_free_dataset(ds);
    return 0;
}
