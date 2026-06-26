#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

// Monochrome mode — for terminals without ANSI color. Series are told apart by
// marker shape (● ■ ▲ ◆) and bar sign by fill (█ up / ▒ down) instead of hue.
//
// Toggle explicitly with ctp_set_color(ds, false). It also turns on by itself
// when the NO_COLOR environment variable is set (the cross-tool convention).
int main(void)
{
    // --- mono scatter: two series distinguished by marker shape ---
    DataSet *s = ctp_initialize_dataset(3, 20, 16);
    CTP_PARAM x[] = {-3, -2, -1, 0, 1, 2, 3}; // shared X axis
    CTP_PARAM a[] = {-3, -2, -1, 0, 1, 2, 3}; // series 0 -> ●
    CTP_PARAM b[] = {9, 4, 1, 0, 1, 4, 9};    // series 1 -> ■
    ctp_add_column(s, "x", x, 7);
    ctp_add_column(s, "a", a, 7);
    ctp_add_column(s, "b", b, 7);
    int ys[] = {1, 2};
    ctp_select_axes(s, 0, ys, 2);
    ctp_set_color(s, false);
    ctp_plot_scatter(s);
    ctp_free_dataset(s);

    // --- mono bar: sign shown by fill (█ positive / ▒ negative) ---
    DataSet *bar = ctp_initialize_dataset(1, 20, 16);
    CTP_PARAM net_profit[] = {-6, -9, 3, 7, 5, 14, 18, 15};
    ctp_add_column(bar, "net profit", net_profit, 8);
    ctp_set_color(bar, false);
    ctp_plot_bar(bar);
    ctp_free_dataset(bar);

    return 0;
}
