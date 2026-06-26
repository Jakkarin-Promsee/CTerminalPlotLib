# CTerminalPlot

A small **C library** (and a `ctplot` CLI) that draws **tables, scatter plots, line
charts, bar charts, and histograms** straight into your terminal with UTF-8 box-drawing
and ANSI color — plus an **8× high-resolution Braille mode**. Matplotlib's spirit, in
~2,000 lines of dependency-free C.

```
Braille resolution (8×):
      ┌────────────────────────────────────────────────────────────┐
  1.00│           ⣀⠤⠒⠉⠉⠉⠒⢄                                         │
      │         ⡠⠊        ⠑⢄                                       │
      │       ⢀⠜            ⠱⡀                                     │
      │      ⢠⠊              ⠘⡄                                    │
      │     ⡰⠁                ⠘⢄                                   │
  0.47│    ⡜                   ⠈⢆                                  │
      │   ⡜                     ⠈⢆                                 │
      │  ⡜                       ⠈⢆                                │
      │ ⡜                         ⠈⢆                               │
      │⡜                           ⠈⢢                              │
 -0.05│                              ⢣                           ⢀⠎│
      │                               ⢣                         ⢀⠎ │
      │                                ⢣                       ⢀⠎  │
      │                                 ⢣                     ⢀⠎   │
      │                                  ⠣⡀                  ⢀⠎    │
 -0.58│                                   ⠱⡀                ⢀⠎     │
      │                                    ⠑⡄              ⡠⠃      │
      │                                     ⠈⢆            ⡔⠁       │
      │                                       ⠑⢄        ⡠⠊         │
 -1.00│                                         ⠑⠢⢄⣀⣀⡠⠔⠊           │
      └────────────────────────────────────────────────────────────┘
       0.00                        3.14                        6.28
```

> *Origin: this began as a first-year university C final project — hand-rolled "OOP in C",
> a dynamic array, and a terminal plotter, written before AI assistants were any good. It's
> since been refactored into a portfolio piece. The history is in [ROADMAP.md](ROADMAP.md).*

## Features

- **Five chart types** — table, scatter, line, bar, histogram — over one in-memory dataset.
- **8× Braille rendering** — a `W×H` canvas addresses a `2W×4H` dot grid for smooth curves
  (`ctp_set_graph_braille`).
- **Color *and* monochrome modes** — series are told apart by ANSI color, or by marker
  *shape* (`● ■ ▲ ◆`) and fill when color is off. Honors the `NO_COLOR` convention.
- **CSV in, plot out** — `ctp_read_csv()` and a `ctplot` command-line tool (`ctplot data.csv
  --line --y 1 --x 0`, or pipe via stdin).
- **Data tools** — sort, search/filter (operators `<`, `<=`, `==`, `!=`, `>`, `>=`), and
  per-column stats (mean, population std-dev, mean absolute deviation).
- **Dependency-free & portable** — pure C11 + libm. The only OS-specific call lives behind a
  platform shim; everything else is plain UTF-8 + ANSI.
- **Leak-clean & tested** — assertion tests + golden-output snapshots for every example.

## Gallery

**Table** — load a CSV (or build columns in code) and print it boxed:

```
┌──────────┬──────────┬──────────┐
│   month  │   sales  │ returns  │
├──────────┼──────────┼──────────┤
│    1.00  │  120.00  │    5.00  │
├──────────┼──────────┼──────────┤
│    2.00  │   95.00  │    3.00  │
├──────────┼──────────┼──────────┤
│    3.00  │  150.00  │    8.00  │
└──────────┴──────────┴──────────┘
```

**Scatter** (two series — shapes shown here in monochrome; in a color terminal they're hues):

```
      ┌────────────────────────────────────────────────────────────────┐
  3.6 ┼                 │                                              │
      │                 │              ■                             ◆ │
      │                 │                                              │
  2.1 ┼                 │                                              │
      │                 │         ■         ◆                          │
      │                 │                                              │
      │                 │    ⊕                                         │
  0.6 ┼                 │                                              │
    0 │─────────────────⊕──────────────────────────────────────────────│
```

**Bar chart** — one bar per row on a zero baseline (green up / red down in color; `█`/`▒` in
mono). A histogram (`ctp_plot_histogram`) bins a column and bars the counts the same way:

```
Bar: profit (8 bars)
      ┌────────────────────────────────────────────────────────────┐
  6.00│                                          ██████            │
      │       ██████                             ██████            │
      │       ██████                      ██████ ██████            │
  3.63│       ██████                      ██████ ██████            │
      │██████ ██████                      ██████ ██████            │
      │██████ ██████ ██████               ██████ ██████            │
  1.26│██████ ██████ ██████               ██████ ██████            │
      │██████ ██████ ██████               ██████ ██████ ██████     │
      │██████ ██████ ██████ ▒▒▒▒▒▒ ▒▒▒▒▒▒ ██████ ██████ ██████     │
      │                     ▒▒▒▒▒▒ ▒▒▒▒▒▒                          │
 -1.11│                     ▒▒▒▒▒▒ ▒▒▒▒▒▒                          │
      │                            ▒▒▒▒▒▒                          │
 -3.00│                            ▒▒▒▒▒▒                          │
      └────────────────────────────────────────────────────────────┘
          0      1      2      3      4      5      6      7
```

## Build & run

Pure C11 + libm. Needs a UTF-8 + ANSI terminal. Developed and tested with MinGW GCC on
Windows; the source is cross-platform by design (POSIX terminals are UTF-8/ANSI already).

```sh
make            # builds libctp.a, every examples/*.c, and the ctplot CLI
make test       # builds & runs the assertion tests
make clean      # removes build artifacts

./examples/output/0_quickstart.exe
```

Compile your own program by linking the library:

```sh
gcc -Isrc/include your_program.c build/libctp.a -o your_program -lm
```

## Quickstart (library)

One call per data series — no 2-D arrays, no manual sizing:

```c
#include "CTerminalPlotLib.h"   // build with -Isrc/include

int main(void)
{
    DataSet *ds = ctp_initialize_dataset(/*max_cols*/ 3, /*max_name*/ 20, /*max_rows*/ 16);

    CTP_PARAM x[]  = {-3, -2, -1, 0, 1, 2, 3};
    CTP_PARAM y1[] = {-3, -2, -1, 0, 1, 2, 3};
    CTP_PARAM y2[] = { 9,  4,  1, 0, 1, 4, 9};

    ctp_add_column(ds, "x",       x,  7);
    ctp_add_column(ds, "y = x",   y1, 7);
    ctp_add_column(ds, "y = x^2", y2, 7);

    ctp_plot(ds);             // table + scatter
    ctp_free_dataset(ds);
    return 0;
}
```

Typical flow: `initialize → add_column… → (select_axes / find / sort) → plot → free`.
Switch on Braille with `ctp_set_graph_braille(ds, true)` before `ctp_plot_line(ds)`; turn
color off with `ctp_set_color(ds, false)`.

## The `ctplot` CLI

A thin tool over the library — point it at a CSV (or pipe one in):

```sh
build/ctplot.exe examples/data/sample.csv --line --y 1 --x 0
build/ctplot.exe examples/data/sample.csv --bar --y 1
cat data.csv | build/ctplot.exe --hist --y 1 --bins 12
```

```
Charts (combine freely; default: --table --scatter):
  --table  --scatter  --line  --bar  --hist
Options:
  --y N        vertical / value column      --bins N    histogram bins (default 10)
  --x N,N,...  horizontal series columns     --braille   8× line rendering
  --no-color   force monochrome (also auto-off when piped, or when NO_COLOR is set)
```

## How it works

Everything hangs off one heap-allocated `DataSet` (column-major `float` cells, capacities
fixed at `ctp_initialize_dataset`). The vector renderers (line, bar, histogram, Braille)
rasterize into a small internal **`CtpCanvas`** — a glyph+color grid with a Bresenham
line-draw — then flush through one boxed-frame printer. Braille is the same idea at 8×: a
boolean dot-grid whose 2×4 cells map to code points `0x2800 + mask`.

See [CLAUDE.md](CLAUDE.md) for the full source map and conventions, and the
[docs/](docs/) folder for per-topic guides.

## API at a glance

| Area | Functions |
|------|-----------|
| Lifecycle | `ctp_initialize_dataset`, `ctp_free_dataset` |
| Load data | `ctp_add_column`, `ctp_add_row`, `ctp_add_data`, `ctp_read_csv` |
| Axes | `ctp_select_axes`, `ctp_reset_axes` |
| Plot | `ctp_plot`, `ctp_plot_table`, `ctp_plot_scatter`, `ctp_plot_line`, `ctp_plot_bar`, `ctp_plot_histogram` |
| Style | `ctp_set_color`, `ctp_set_graph_braille`, `ctp_set_graph_resolution`, `ctp_set_table_width`, … |
| Transform | `ctp_sort`, `ctp_findOne`, `ctp_findMany`, `ctp_reset_find` |
| Analyze | `ctp_analyze_mean`, `ctp_analyze_sd`, `ctp_analyze_md` |

## Project structure

```
CTerminalPlot/
 ├── src/
 │   ├── CTerminalPlotLib.c      # the whole library implementation
 │   ├── include/…               # the public header
 │   └── ctplot.c                # the CLI
 ├── examples/                   # runnable, documented programs (0 → 12)
 │   └── data/sample.csv         # sample input for the CSV demo
 ├── tests/                      # assertion tests + golden output snapshots
 └── docs/                       # per-topic guides
```

## Status

The library is correct (assertion-tested, leak-clean), structured as a real linkable
library, portable, and feature-complete for the chart types above. Remaining polish — a
plot legend, nicer axis ticks, a regression best-fit line, and CI — is tracked in
[ROADMAP.md](ROADMAP.md).

## License

[MIT](LICENSE).
