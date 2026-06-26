#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

// Load a CSV and explore it three ways — no hand-coded arrays. The bundled
// sample is one (made-up) online store's first year: monthly sales and profit.
// Pass your own CSV path as argv[1], or run from the repo root so the default
// relative path resolves.
//
//   month,sales,profit   <- header row becomes the column labels
//   1,42,-6              <- numeric rows become the data
//   ...
int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "examples/data/sample.csv";

    DataSet *ds = ctp_read_csv(path);
    if (!ds)
        return 1;

    // 1) The raw numbers, boxed.
    ctp_plot_table(ds);

    // 2) Sales (column 1) over the months (column 0) as a line — the growth trend.
    //    Month is the X axis; sales is the Y series.
    int sales[] = {1};
    ctp_select_axes(ds, 0, sales, 1);
    ctp_plot_line(ds);

    // 3) Profit (column 2) as bars — red where the store lost money, green once
    //    it turned a profit. Point the Y series at the profit column.
    int profit[] = {2};
    ctp_select_axes(ds, 0, profit, 1);
    ctp_plot_bar(ds);

    ctp_free_dataset(ds);
    return 0;
}
