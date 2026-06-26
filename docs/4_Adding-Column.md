<!-- 4_Adding-Column.md -->

# CTerminalPlot — Adding Columns & Choosing Axes

Adding another data series is just another `ctp_add_column` call (up to the `max_param` you
set at init). The more interesting question is **which** column is the horizontal X axis and
which columns are the Y series plotted against it — that's what `ctp_select_axes` controls.

## Adding more columns

```c
DataSet *ds = ctp_initialize_dataset(/*cols*/ 4, /*name*/ 20, /*rows*/ 16);

ctp_add_column(ds, "x",       (CTP_PARAM[]){-3,-2,-1,0,1,2,3}, 7);
ctp_add_column(ds, "y = x",   (CTP_PARAM[]){-3,-2,-1,0,1,2,3}, 7);
ctp_add_column(ds, "y = -x",  (CTP_PARAM[]){ 3, 2, 1,0,-1,-2,-3}, 7);
ctp_add_column(ds, "y = x^2", (CTP_PARAM[]){ 9, 4, 1,0,1,4,9}, 7);
```

By default the first column (index 0) is the horizontal **X** axis and **every** other column
is drawn as a **Y** series against it.

## Choosing the axes

```c
void ctp_select_axes(DataSet *ds, int x_col, const int *y_cols, int y_count);
void ctp_reset_axes(DataSet *ds);
```

Pick the X column and an explicit list of Y columns. This both **restricts** the plot to
those columns and enables the "customized" view (table and charts then show only the selected
columns). Here X is column 0, and we plot `y = x` and `y = x^2`, **skipping `y = -x`**:

```c
int y_series[] = {1, 3};             // columns y=x and y=x^2 (skip y=-x at index 2)
ctp_select_axes(ds, 0, y_series, 2); // x_col = 0
ctp_plot(ds);
```

### Output

The table shows only `x`, `y = x`, `y = x^2`; the scatter plots those two series against X
(skipping `y = -x`). `y = x` rises on the diagonal (`●`) and `y = x^2` traces an upward
parabola (`■`):

```
┌──────────┬──────────┬──────────┐
│       x  │   y = x  │ y = x^2  │
├──────────┼──────────┼──────────┤
│   -3.00  │   -3.00  │    9.00  │
├──────────┼──────────┼──────────┤
   … rows -2 … 2 …
├──────────┼──────────┼──────────┤
│    3.00  │    3.00  │    9.00  │
└──────────┴──────────┴──────────┘
      ┌────────────────────────────────────────────────────────────┐
  9.00│■                             │                            ■│
      │                              │                             │
      │                              │                             │
      │                              │                             │
      │                              │                             │
  5.84│                              │                             │
      │                              │                             │
      │                              │                             │
      │          ■                   │                  ■          │
      │                              │                            ●│
  2.68│                              │                             │
      │                              │                  ●          │
      │                              │                             │
      │                    ■         │        ⊕                    │
      │──────────────────────────────⊕─────────────────────────────│
 -0.47│                              │                             │
      │                    ●         │                             │
      │          ●                   │                             │
      │                              │                             │
 -3.00│●                             │                             │
      └────────────────────────────────────────────────────────────┘
       -3.00                       0.00                        3.00
```

`ctp_reset_axes(ds)` returns to the default (all columns, column 0 as X).

## The same selection feeds the charts

Axis selection drives every renderer, not just the scatter:

- **Line** — `ctp_plot_line` connects each selected Y series against X in row order.
- **Bar** — `ctp_plot_bar` bars the first selected **Y** column (so re-select Y to bar a
  different column).
- **Histogram** — `ctp_plot_histogram` bins the first selected Y column.

See the [README showcases](../README.md#showcases) for those rendered, and
[`examples/11_csv.c`](../examples/11_csv.c) for one dataset re-pointed at different columns
for a line and then a bar.

## Next

→ [Search, sort & analyze](5_data-tools.md): filter rows, sort by a column, and compute stats.
