#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

// Load a CSV and plot it — no hand-coded arrays. Pass a path as argv[1], or the
// bundled sample is used (run from the repo root so the relative path resolves).
int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "examples/data/sample.csv";

    DataSet *ds = ctp_read_csv(path);
    if (!ds)
        return 1;

    ctp_plot_table(ds);

    // Plot sales (column 1) on the vertical axis against month (column 0).
    int x[] = {0};
    ctp_select_axes(ds, 1, x, 1);
    ctp_plot_line(ds);

    ctp_free_dataset(ds);
    return 0;
}
