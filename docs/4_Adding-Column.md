<!-- 4_Adding-Column.md -->

# CTerminalPlot — Adding Columns & Choosing Axes

Adding another data series is just another `ctp_add_column` call (up to the `max_param` you
set at init). The more interesting question is **which** column is the vertical axis and
which are plotted against it — that's what `ctp_select_axes` controls.

## Adding more columns

```c
DataSet *ds = ctp_initialize_dataset(/*cols*/ 4, /*name*/ 20, /*rows*/ 16);

ctp_add_column(ds, "y",  (CTP_PARAM[]){-3,-2,-1,0,1,2,3}, 7);
ctp_add_column(ds, "x1", (CTP_PARAM[]){-3,-2,-1,0,1,2,3}, 7);
ctp_add_column(ds, "x2", (CTP_PARAM[]){ 3, 2, 1,0,-1,-2,-3}, 7);
ctp_add_column(ds, "x3", (CTP_PARAM[]){ 9, 4, 1,0,1,4,9}, 7);
```

By default the first column (index 0) is the vertical axis and **every** other column is
drawn as a series against it.

## Choosing the axes

```c
void ctp_select_axes(DataSet *ds, int y_col, const int *x_cols, int x_count);
void ctp_reset_axes(DataSet *ds);
```

Pick the Y column and an explicit list of X columns. This both **restricts** the plot to
those columns and enables the "customized" view (table and charts then show only the selected
columns). Here we plot `y` against `x1` and `x3`, **skipping `x2`**:

```c
int x_axes[] = {1, 3};            // columns x1 and x3 (skip x2 at index 2)
ctp_select_axes(ds, 0, x_axes, 2); // y_col = 0
ctp_plot(ds);
```

### Output

The table shows only `y`, `x1`, `x3`; the scatter plots those two series (skipping `x2`):

```
┌──────────┬──────────┬──────────┐
│       y  │      x1  │      x3  │
├──────────┼──────────┼──────────┤
│   -3.00  │   -3.00  │    9.00  │
├──────────┼──────────┼──────────┤
   … rows -2 … 2 …
├──────────┼──────────┼──────────┤
│    3.00  │    3.00  │    9.00  │
└──────────┴──────────┴──────────┘
      ┌────────────────────────────────────────────────────────────────┐
  3.6 ┼                 │              ■                             ◆ │
      │                 │                                              │
  2.1 ┼                 │         ■         ◆                          │
      │                 │    ⊕                                         │
    0 │─────────────────⊕──────────────────────────────────────────────│
 -0.9 ┼            ■    │    ◆                                         │
      │       ■         │                   ◆                          │
      │  ■              │                                            ◆ │
 -3.9 ┼──────────────┼──────────────┼──────────────┼──────────────┼────┘
    -3.4           -0.4            2.6            5.6            8.6
```

`ctp_reset_axes(ds)` returns to the default (all columns, column 0 as Y).

## The same selection feeds the charts

Axis selection drives every renderer, not just the scatter:

- **Line** — `ctp_plot_line` connects each selected X series in row order.
- **Bar** — `ctp_plot_bar` bars the selected **Y** column (so re-select Y to bar a different
  column).
- **Histogram** — `ctp_plot_histogram` bins the selected Y column.

See the [README showcases](../README.md#showcases) for those rendered, and
[`examples/11_csv.c`](../examples/11_csv.c) for one dataset re-pointed at different columns
for a line and then a bar.

## Next

→ [Search, sort & analyze](5_data-tools.md): filter rows, sort by a column, and compute stats.
