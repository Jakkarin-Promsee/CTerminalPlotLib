<!-- 2_basic-usage.md -->

# CTerminalPlot — Basic Usage

Every plot starts from a **`DataSet`**: a fixed-capacity, column-major store of `float` cells
(`CTP_PARAM`). The flow is always the same:

> **initialize → add columns → plot → free**

## The DataSet model

```c
DataSet *ctp_initialize_dataset(int max_param, int max_name_size, int max_param_size);
```

| Parameter        | Meaning                                          |
| ---------------- | ------------------------------------------------ |
| `max_param`      | maximum number of **columns**                    |
| `max_name_size`  | maximum length of a column **label** (incl. NUL) |
| `max_param_size` | maximum number of **rows** per column            |

Capacities are **fixed at initialization** — the dataset does not grow. Pick sizes with a
little headroom; adding past them is rejected with a message on `stderr`. Storage is
**column-major** (`db[column][row]`), because data is added a column at a time.

## Adding data — the easy way

`ctp_add_column` adds one labelled column from a plain 1-D array. This is the preferred entry
point: no 2-D arrays, no stride math.

```c
void ctp_add_column(DataSet *ds, const char *name, const CTP_PARAM *values, int count);
```

```c
#include "CTerminalPlotLib.h"

int main(void)
{
    DataSet *ds = ctp_initialize_dataset(/*cols*/ 3, /*name*/ 20, /*rows*/ 10);

    CTP_PARAM y[]    = {-3, -2, -1, 0, 1, 2, 3};
    CTP_PARAM yx[]   = {-3, -2, -1, 0, 1, 2, 3};
    CTP_PARAM ynx[]  = { 3,  2,  1, 0, -1, -2, -3};

    ctp_add_column(ds, "y",    y,   7);
    ctp_add_column(ds, "y = x", yx,  7);
    ctp_add_column(ds, "y = -x", ynx, 7);

    ctp_plot(ds);              // table + scatter
    ctp_free_dataset(ds);
    return 0;
}
```

### Output

`ctp_plot` draws the table, then the scatter. Column 0 is the vertical (Y) axis; the
remaining columns are plotted as series against it.

```
┌──────────┬──────────┬──────────┐
│       y  │   y = x  │  y = -x  │
├──────────┼──────────┼──────────┤
│   -3.00  │   -3.00  │    3.00  │
├──────────┼──────────┼──────────┤
│   -2.00  │   -2.00  │    2.00  │
├──────────┼──────────┼──────────┤
│   -1.00  │   -1.00  │    1.00  │
├──────────┼──────────┼──────────┤
│    0.00  │    0.00  │    0.00  │
├──────────┼──────────┼──────────┤
│    1.00  │    1.00  │   -1.00  │
├──────────┼──────────┼──────────┤
│    2.00  │    2.00  │   -2.00  │
├──────────┼──────────┼──────────┤
│    3.00  │    3.00  │   -3.00  │
└──────────┴──────────┴──────────┘
      ┌────────────────────────────────────────────────────────────────┐
  3.6 ┼                                │                               │
      │  ■                             │                             ● │
  2.1 ┼            ■                   │                   ●           │
      │                      ■         │         ●                     │
    0 │────────────────────────────────⊕───────────────────────────────│
 -0.9 ┼                      ●         │         ■                     │
      │            ●                   │                   ■           │
 -3.9 ┼──────────────┼──────────────┼──────────────┼──────────────┼────┘
    -3.2           -1.7           -0.2            1.3            2.8
```

_(Scatter blank rows trimmed for length; the program prints the full grid. In a color
terminal the two series are red and blue rather than `●` and `■`.)_

## Choosing which plots to draw

`ctp_plot` honors the dataset's plot flags (table on, scatter on, line off by default). To
draw exactly one kind, call its renderer directly:

```c
ctp_plot_table(ds);      // just the table
ctp_plot_scatter(ds);    // just the scatter
ctp_plot_line(ds);       // a line chart (see Adding columns & the README showcases)
```

## Other ways to load data

| Function          | Use it when…                                                            |
| ----------------- | ----------------------------------------------------------------------- |
| `ctp_add_column`  | you have one labelled series in a 1-D array (**preferred**)             |
| `ctp_add_row`     | you build the table row by row — see [Adding rows](3_Adding-Rows.md)    |
| `ctp_add_data`    | you already have a contiguous 2-D block to copy in bulk                 |
| `ctp_read_csv`    | your data is a CSV file — see the [README CSV showcase](../README.md#load-a-csv) |

## Inspecting a dataset

```c
ctp_printf_memory_usage(ds);   // capacities, current size, byte footprint
ctp_printf_properties(ds);     // the above plus axis selection & plot flags
```

## Free when done

```c
void ctp_free_dataset(DataSet *ds);   // NULL-safe; frees everything the dataset owns
```

## Next

- [Adding rows](3_Adding-Rows.md) — append data row by row.
- [Adding columns & choosing axes](4_Adding-Column.md) — more series, and picking the axis.
- [Search, sort & analyze](5_data-tools.md) — filter and summarize the data.
