# CTerminalPlot — Documentation

These guides are the **manual**: they go deeper than the [README](../README.md) on how the
library is put together and on the tools the README only lists (search, sort, analyze,
styling). For a visual tour of every chart type, see the **[Showcases in the
README](../README.md#showcases)**; for complete runnable programs, see
**[`examples/`](../examples/)**.

## Contents

1. **[Setup & linking](1_Setup-Guide.md)** — download, build `libctp.a`, and link it into
   your own program (the right way — no `#include "*.c"`).
2. **[Basic usage](2_basic-usage.md)** — the `DataSet` model and the simplest path to a
   table + scatter with `ctp_add_column`.
3. **[Adding rows](3_Adding-Rows.md)** — append rows one at a time, and how ragged columns
   render.
4. **[Adding columns & choosing axes](4_Adding-Column.md)** — add more series and pick which
   column is the vertical axis with `ctp_select_axes`.
5. **[Search, sort & analyze](5_data-tools.md)** — filter with comparison operators, sort by
   a column, and compute per-column statistics.

## The 30-second version

```c
#include "CTerminalPlotLib.h"            // build with -Isrc/include

DataSet *ds = ctp_initialize_dataset(2, 20, 8);
ctp_add_column(ds, "x", (CTP_PARAM[]){1, 2, 3, 4}, 4);
ctp_add_column(ds, "y", (CTP_PARAM[]){1, 4, 9, 16}, 4);
ctp_plot(ds);                            // table + scatter
ctp_free_dataset(ds);
```

```sh
gcc -Isrc/include myplot.c build/libctp.a -o myplot -lm
```

## See also

- [README](../README.md) — overview, install, and the chart showcases.
- [CLAUDE.md](../CLAUDE.md) — the full source map and conventions (for contributors).
- [ROADMAP.md](../ROADMAP.md) — the refactor history and what's still planned.
