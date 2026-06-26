#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

// Two datasets, each with its OWN render style, alive at the same time.
// (Impossible back when the style was a single set of global variables.)
int main(void)
{
    CTP_PARAM x[] = {-3, -2, -1, 0, 1, 2, 3};
    CTP_PARAM y[] = {9, 4, 1, 0, 1, 4, 9};

    DataSet *a = ctp_initialize_dataset(2, 20, 8);
    ctp_add_column(a, "x", x, 7);
    ctp_add_column(a, "y=x^2", y, 7);

    DataSet *b = ctp_initialize_dataset(2, 20, 8);
    ctp_add_column(b, "x", x, 7);
    ctp_add_column(b, "y=x^2", y, 7);
    ctp_set_table_width(b, 14);          // wider table columns
    ctp_set_graph_resolution(b, 80, 24); // bigger scatter plot
    ctp_set_graph_point_x(b, '*');       // different point glyph

    printf("== Dataset A: default style ==\n");
    ctp_plot_table(a);

    printf("\n== Dataset B: custom style (wider table) — same data ==\n");
    ctp_plot_table(b);

    ctp_free_dataset(a);
    ctp_free_dataset(b);
    return 0;
}
