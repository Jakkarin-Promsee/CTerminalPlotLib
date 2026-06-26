# CTerminalPlot

**Plot tables, scatter, line, bar, and histogram charts straight into your terminal — in
pure, dependency-free C.** UTF-8 box-drawing, ANSI color, and an 8× Braille mode give you
matplotlib's spirit in ~2,000 lines of C11 and nothing else but `libm`.

```
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

> _Origin: this began as a first-year university C final project — hand-rolled "OOP in C",
> a dynamic array, and a terminal plotter, written before AI assistants were any good. It's
> since been refactored into a portfolio piece. The history is in [ROADMAP.md](ROADMAP.md)._

---

## Overview

CTerminalPlot is a small C library (plus a `ctplot` CLI) for visualizing numeric data
**without leaving the terminal** — no GUI, no Python, no plotting backend. You build a
`DataSet` in memory (columns × rows of `float`), optionally filter / sort / analyze it, then
call a `ctp_plot*` function to draw it as a boxed table or a chart.

It's built for the moments when you just want to *see* the numbers: debugging a C program,
eyeballing a CSV over SSH, or sketching a curve where a heavyweight plotting stack would be
overkill.

|                  |                                                              |
| ---------------- | ------------------------------------------------------------ |
| **Language**     | C11                                                          |
| **Dependencies** | the C standard library + `libm` — nothing else              |
| **Output**       | UTF-8 box-drawing + ANSI color (or pure monochrome)         |
| **Footprint**    | one `.c` (~2,000 lines), one header                         |
| **Tested**       | assertion suite + golden-output snapshots for every example |

## Features

- **Five chart types** — table, scatter, line, bar, histogram — over one in-memory dataset.
- **8× Braille rendering** — a `W×H` canvas addresses a `2W×4H` dot grid for smooth curves
  (`ctp_set_graph_braille`).
- **Color _and_ monochrome modes** — series are told apart by ANSI color, or by marker
  _shape_ (`● ■ ▲ ◆`) and fill when color is off. Honors the `NO_COLOR` convention.
- **CSV in, plot out** — `ctp_read_csv()` plus a `ctplot` command-line tool
  (`ctplot data.csv --line --y 1 --x 0`, or pipe via stdin).
- **Data tools** — sort, search / filter (operators `<`, `<=`, `==`, `!=`, `>`, `>=`), and
  per-column stats (mean, population std-dev, mean absolute deviation).
- **Dependency-free & portable** — pure C11 + libm. The only OS-specific call lives behind a
  platform shim; everything else is plain UTF-8 + ANSI.
- **Leak-clean & tested** — assertion tests plus golden snapshots that catch any rendering
  regression.

## Project structure

```
CTerminalPlot/
 ├── src/
 │   ├── CTerminalPlotLib.c      # the whole library implementation (~2,000 lines)
 │   ├── include/
 │   │   └── CTerminalPlotLib.h  # the public API: types, prototypes, config
 │   └── ctplot.c                # the ctplot CLI front-end
 ├── examples/                   # runnable, documented programs (0 → 12)
 │   ├── 0_quickstart.c          #   one call per series → table + scatter
 │   ├── …                       #   one example per feature
 │   └── data/sample.csv         #   sample input for the CSV demo
 ├── tests/
 │   ├── test_ctp.c              # assertion tests (make test)
 │   └── golden/*.txt            # captured output; diff to catch regressions
 ├── docs/                       # per-topic guides
 ├── Makefile                    # build the library, examples, CLI, and tests
 ├── ROADMAP.md                  # the refactor history & what's next
 └── CLAUDE.md                   # source map & conventions for contributors
```

## Install & build

Pure C11 + libm; needs a UTF-8 + ANSI terminal (every modern one qualifies). Developed and
tested with MinGW GCC on Windows; the source is cross-platform by design.

```sh
git clone <repo-url> CTerminalPlot
cd CTerminalPlot

make            # builds build/libctp.a, every examples/*.exe, and build/ctplot.exe
make test       # builds & runs the assertion tests
make clean      # removes build artifacts

./examples/output/0_quickstart.exe   # run any example
```

To use the library in your own program, include the header and link the static library —
**do not `#include` the `.c`**:

```sh
gcc -Isrc/include your_program.c build/libctp.a -o your_program -lm
```

```c
#include "CTerminalPlotLib.h"   // built with -Isrc/include
```

## Quickstart

One call per data series — no 2-D arrays, no manual sizing. This whole program prints a
boxed table **and** a scatter plot:

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

```
┌──────────┬──────────┬──────────┐
│       x  │   y = x  │ y = x^2  │
├──────────┼──────────┼──────────┤
│   -3.00  │   -3.00  │    9.00  │
├──────────┼──────────┼──────────┤
│   -2.00  │   -2.00  │    4.00  │
├──────────┼──────────┼──────────┤
│   -1.00  │   -1.00  │    1.00  │
├──────────┼──────────┼──────────┤
│    0.00  │    0.00  │    0.00  │
├──────────┼──────────┼──────────┤
│    1.00  │    1.00  │    1.00  │
├──────────┼──────────┼──────────┤
│    2.00  │    2.00  │    4.00  │
├──────────┼──────────┼──────────┤
│    3.00  │    3.00  │    9.00  │
└──────────┴──────────┴──────────┘
      ┌────────────────────────────────────────────────────────────────┐
  3.6 ┼                 │                                              │
      │                 │                                              │
      │                 │              ●                             ■ │
      │                 │                                              │
      │                 │                                              │
  2.1 ┼                 │                                              │
      │                 │         ●         ■                          │
      │                 │                                              │
      │                 │                                              │
      │                 │    ⊕                                         │
  0.6 ┼                 │                                              │
      │                 │                                              │
    0 │─────────────────⊕──────────────────────────────────────────────│
      │                 │                                              │
      │                 │                                              │
 -0.9 ┼                 │                                              │
      │            ●    │    ■                                         │
      │                 │                                              │
      │                 │                                              │
      │       ●         │                   ■                          │
 -2.4 ┼                 │                                              │
      │                 │                                              │
      │  ●              │                                            ■ │
      │                 │                                              │
      │                 │                                              │
 -3.9 ┼──────────────┼──────────────┼──────────────┼──────────────┼────┘
    -3.4           -0.4            2.6            5.6            8.6
```

Typical flow: `initialize → add_column… → (select_axes / find / sort) → plot → free`.
**Full example: [examples/0_quickstart.c](examples/0_quickstart.c).**

---

## Showcases

Each block shows the **rendered result first**, then the code that made it, then a link to
the full runnable example. _(Output below is monochrome so it renders cleanly here; in a real
terminal each series gets its own ANSI color.)_

### Line chart

Connect each X series into a continuous stroke. Multiple series are drawn at once — here a
straight line (`x = y`) and a sideways parabola (`x = y²`) share one vertical axis:

```
      ┌────────────────────────────────────────────────────────────┐
  3.00│                              ●                        ────■│
      │                            ╱╱                 ────────     │
      │                          ╱╱           ────────             │
      │                         ╱      ───────                     │
      │                       ╱╱   ────                            │
  1.42│                     ╱╱─────                                │
      │                    ───                                     │
      │                  ╱╱                                        │
      │                ╱╱                                          │
      │               ╱                                            │
 -0.16│              ╱ ╲                                           │
      │            ╱╱   ╲╲                                         │
      │           ╱       ╲                                        │
      │          ╱         ╲──                                     │
      │        ╱╱             ────                                 │
 -1.74│      ╱╱                   ─────                            │
      │     ╱                          ───────                     │
      │   ╱╱                                  ────────             │
      │ ╱╱                                            ────────     │
 -3.00│╱                                                      ─────│
      └────────────────────────────────────────────────────────────┘
       -3.00                       3.00                        9.00
```

```c
CTP_PARAM y[]    = {-3, -2, -1, 0, 1, 2, 3};
CTP_PARAM line[] = {-3, -2, -1, 0, 1, 2, 3}; // a straight line  (x = y)
CTP_PARAM para[] = { 9,  4,  1, 0, 1, 4, 9}; // a parabola       (x = y^2)

ctp_add_column(ds, "y", y, 7);
ctp_add_column(ds, "line", line, 7);
ctp_add_column(ds, "para", para, 7);

int x_axes[] = {1, 2};
ctp_select_axes(ds, 0, x_axes, 2);  // y is the vertical axis; line & para are series
ctp_plot_line(ds);
```

**Full example: [examples/7_line-plot.c](examples/7_line-plot.c).**

### Bar chart

One vertical bar per row on a zero baseline — positive bars rise (green in color, `█` in
mono), negatives drop below (red, `▒`). Here: a small shop's monthly net profit, underwater
in winter then climbing into the black.

```
Bar: net profit (8 bars)
      ┌────────────────────────────────────────────────────────────┐
 18.00│                                          ██████            │
      │                                          ██████            │
      │                                   ██████ ██████ ██████     │
      │                                   ██████ ██████ ██████     │
      │                                   ██████ ██████ ██████     │
 10.89│                                   ██████ ██████ ██████     │
      │                                   ██████ ██████ ██████     │
      │                                   ██████ ██████ ██████     │
      │                     ██████        ██████ ██████ ██████     │
      │                     ██████ ██████ ██████ ██████ ██████     │
  3.79│                     ██████ ██████ ██████ ██████ ██████     │
      │              ██████ ██████ ██████ ██████ ██████ ██████     │
      │              ██████ ██████ ██████ ██████ ██████ ██████     │
      │▒▒▒▒▒▒ ▒▒▒▒▒▒ ██████ ██████ ██████ ██████ ██████ ██████     │
      │▒▒▒▒▒▒ ▒▒▒▒▒▒                                               │
 -3.32│▒▒▒▒▒▒ ▒▒▒▒▒▒                                               │
      │▒▒▒▒▒▒ ▒▒▒▒▒▒                                               │
      │▒▒▒▒▒▒ ▒▒▒▒▒▒                                               │
      │       ▒▒▒▒▒▒                                               │
 -9.00│       ▒▒▒▒▒▒                                               │
      └────────────────────────────────────────────────────────────┘
          0      1      2      3      4      5      6      7
```

```c
CTP_PARAM net_profit[] = {-6, -9, 3, 7, 5, 14, 18, 15};
ctp_add_column(ds, "net profit", net_profit, 8);

ctp_plot_bar(ds);   // single column → uses column 0, zero baseline
```

**Full example: [examples/8_bar-chart.c](examples/8_bar-chart.c).**

### Histogram

Bin one column into equal-width buckets and bar the per-bin counts. Here: daily order counts
over four weeks, clustering into a rough bell once binned.

```
Histogram: daily orders (8 bins, range 42.00..59.00)
      ┌────────────────────────────────────────────────────────────┐
  6.00│                     ██████ ██████                          │
      │                     ██████ ██████                          │
      │                     ██████ ██████                          │
      │                     ██████ ██████ ██████                   │
      │                     ██████ ██████ ██████                   │
  4.42│                     ██████ ██████ ██████                   │
      │                     ██████ ██████ ██████                   │
      │                     ██████ ██████ ██████                   │
      │                     ██████ ██████ ██████                   │
      │              ██████ ██████ ██████ ██████                   │
  2.84│              ██████ ██████ ██████ ██████                   │
      │              ██████ ██████ ██████ ██████                   │
      │              ██████ ██████ ██████ ██████                   │
      │██████ ██████ ██████ ██████ ██████ ██████ ██████ ██████     │
      │██████ ██████ ██████ ██████ ██████ ██████ ██████ ██████     │
  1.26│██████ ██████ ██████ ██████ ██████ ██████ ██████ ██████     │
      │██████ ██████ ██████ ██████ ██████ ██████ ██████ ██████     │
      │██████ ██████ ██████ ██████ ██████ ██████ ██████ ██████     │
      │██████ ██████ ██████ ██████ ██████ ██████ ██████ ██████     │
  0.00│██████ ██████ ██████ ██████ ██████ ██████ ██████ ██████     │
      └────────────────────────────────────────────────────────────┘
       42.00                       50.50                      59.00
```

```c
CTP_PARAM daily_orders[] = {44, 47, 49, 50, 51, 52, 53, 54, 48, 46,
                            50, 52, 53, 55, 49, 51, 52, 54, 45, 50,
                            51, 56, 48, 53, 57, 50, 42, 59};
ctp_add_column(ds, "daily orders", daily_orders, 28);

ctp_plot_histogram(ds, 8);   // 8 bins
```

**Full example: [examples/9_histogram.c](examples/9_histogram.c).**

### Braille hi-res (8×)

A Braille cell packs a 2×4 dot grid, so the same canvas size resolves **8× more detail**. The
block-glyph renderer below draws a sine wave; the smooth Braille version of the *same* wave is
the image at the top of this README. `ctp_plot_line` switches between them on one flag:

```
Block resolution:
      ┌────────────────────────────────────────────────────────────┐
  1.00│            ─────╲                                          │
      │          ─╱      ──╲                                       │
      │        ─╱           ╲                                      │
      │       ╱              ─╲                                    │
      │      ╱                 ╲                                   │
  0.47│     ╱                   ╲                                  │
      │    ╱                     ╲                                 │
      │  ─╱                       ╲                                │
      │ ╱                          ╲                               │
      │╱                            ╲                              │
 -0.05│                              ╲                            ●│
      │                               ╲                          ╱ │
      │                                ╲                       ─╱  │
      │                                 ╲                     ╱    │
      │                                  ╲                   ╱     │
 -0.58│                                   ╲                 ╱      │
      │                                    ─╲              ╱       │
      │                                      ╲           ─╱        │
      │                                       ──╲      ─╱          │
 -1.00│                                          ─────╱            │
      └────────────────────────────────────────────────────────────┘
       0.00                        3.14                        6.28
```

```c
for (int i = 0; i < N; i++) {
    double x = (double)i / (N - 1) * 6.2831853; // 0 .. 2*pi
    sinv[i] = (CTP_PARAM)sin(x);
    t[i]    = (CTP_PARAM)x;
}
ctp_add_column(ds, "sin", sinv, N);
ctp_add_column(ds, "t",   t,    N);
ctp_select_axes(ds, 0, (int[]){1}, 1);

ctp_plot_line(ds);                  // block resolution (above)
ctp_set_graph_braille(ds, true);
ctp_plot_line(ds);                  // 8× braille (the hero image at the top)
```

**Full example: [examples/12_braille.c](examples/12_braille.c).**

### Monochrome / `NO_COLOR`

For terminals (or pipes) without color, series are told apart by marker **shape**
(`● ■ ▲ ◆`), overlaps by `⊕`, and bar sign by **fill** (`█` up / `▒` down). It turns on
automatically when `NO_COLOR` is set, or explicitly with `ctp_set_color(ds, false)`:

```
      ┌────────────────────────────────────────────────────────────────┐
  3.6 ┼                 │                                              │
      │                 │                                              │
      │                 │              ■                             ▲ │
      │                 │                                              │
      │                 │                                              │
  2.1 ┼                 │                                              │
      │                 │         ■         ▲                          │
      │                 │                                              │
      │                 │                                              │
      │                 │    ⊕                                         │
  0.6 ┼                 │                                              │
      │                 │                                              │
    0 │─────────────────⊕──────────────────────────────────────────────│
      │                 │                                              │
      │                 │                                              │
 -0.9 ┼                 │                                              │
      │            ■    │    ▲                                         │
      │                 │                                              │
      │                 │                                              │
      │       ■         │                   ▲                          │
 -2.4 ┼                 │                                              │
      │                 │                                              │
      │  ■              │                                            ▲ │
      │                 │                                              │
      │                 │                                              │
 -3.9 ┼──────────────┼──────────────┼──────────────┼──────────────┼────┘
    -3.4           -0.4            2.6            5.6            8.6
```

```c
ctp_select_axes(s, 0, (int[]){1, 2}, 2);
ctp_set_color(s, false);     // or run with NO_COLOR=1 in the environment
ctp_plot_scatter(s);
```

**Full example: [examples/10_mono-mode.c](examples/10_mono-mode.c).**

### Load a CSV

`ctp_read_csv()` turns a CSV into a `DataSet`: the header row becomes column labels, numeric
rows become data, blank cells become empty. Below, one file —
[examples/data/sample.csv](examples/data/sample.csv), an imaginary store's first year — is
read once and plotted as a table and a sales line (the example also bars the profit column):

```c
DataSet *ds = ctp_read_csv("examples/data/sample.csv");

ctp_plot_table(ds);                       // 1) the raw numbers, boxed

int month[] = {0};
ctp_select_axes(ds, 1, month, 1);         // 2) sales (col 1) over month (col 0)
ctp_plot_line(ds);

ctp_select_axes(ds, 2, month, 1);         // 3) profit (col 2) as bars
ctp_plot_bar(ds);
```

```
┌──────────┬──────────┬──────────┐
│   month  │   sales  │  profit  │
├──────────┼──────────┼──────────┤
│    1.00  │   42.00  │   -6.00  │
├──────────┼──────────┼──────────┤
│    2.00  │   38.00  │   -9.00  │
├──────────┼──────────┼──────────┤
│    3.00  │   55.00  │    3.00  │
├──────────┼──────────┼──────────┤
│    4.00  │   61.00  │    7.00  │
├──────────┼──────────┼──────────┤
│    5.00  │   58.00  │    5.00  │
├──────────┼──────────┼──────────┤
│    6.00  │   72.00  │   14.00  │
├──────────┼──────────┼──────────┤
│    7.00  │   79.00  │   18.00  │
├──────────┼──────────┼──────────┤
│    8.00  │   75.00  │   15.00  │
├──────────┼──────────┼──────────┤
│    9.00  │   88.00  │   23.00  │
├──────────┼──────────┼──────────┤
│   10.00  │   94.00  │   27.00  │
├──────────┼──────────┼──────────┤
│   11.00  │  118.00  │   41.00  │
├──────────┼──────────┼──────────┤
│   12.00  │  135.00  │   52.00  │
└──────────┴──────────┴──────────┘
      ┌────────────────────────────────────────────────────────────┐
135.00│                                                           ●│
      │                                                         ╱╱ │
      │                                                       ╱╱   │
      │                                                      ╱     │
      │                                                     ╱      │
109.47│                                                    ╱       │
      │                                                  ╱╱        │
      │                                                 ╱          │
      │                                              ──╱           │
      │                                           ───              │
 83.95│                                         ╱╱                 │
      │                              ──────   ╱╱                   │
      │                           ───      ──╱                     │
      │                         ╱╱                                 │
      │               ────    ╱╱                                   │
 58.42│             ──    ──╱╱                                     │
      │           ──                                               │
      │         ╱╱                                                 │
      │───    ╱╱                                                   │
 38.00│   ──╱╱                                                     │
      └────────────────────────────────────────────────────────────┘
       1.00                        6.50                       12.00
```

**Full example: [examples/11_csv.c](examples/11_csv.c).**

### The `ctplot` CLI

A thin command-line tool over the library — point it at a CSV (or pipe one in) and pick
charts with flags:

```sh
build/ctplot.exe examples/data/sample.csv --line --y 1 --x 0   # sales line
build/ctplot.exe examples/data/sample.csv --bar  --y 2         # profit bars
cat data.csv | build/ctplot.exe --hist --y 1 --bins 12         # or via stdin
```

```
Bar: profit (12 bars)
      ┌────────────────────────────────────────────────────────────┐
 52.00│                                                       ████ │
      │                                                       ████ │
      │                                                       ████ │
      │                                                  ████ ████ │
      │                                                  ████ ████ │
 35.95│                                                  ████ ████ │
      │                                                  ████ ████ │
      │                                                  ████ ████ │
      │                                             ████ ████ ████ │
      │                                        ████ ████ ████ ████ │
 19.89│                                        ████ ████ ████ ████ │
      │                              ████      ████ ████ ████ ████ │
      │                         ████ ████ ████ ████ ████ ████ ████ │
      │                         ████ ████ ████ ████ ████ ████ ████ │
      │               ████      ████ ████ ████ ████ ████ ████ ████ │
  3.84│          ████ ████ ████ ████ ████ ████ ████ ████ ████ ████ │
      │▒▒▒▒ ▒▒▒▒ ████ ████ ████ ████ ████ ████ ████ ████ ████ ████ │
      │▒▒▒▒ ▒▒▒▒                                                   │
      │▒▒▒▒ ▒▒▒▒                                                   │
 -9.00│     ▒▒▒▒                                                   │
      └────────────────────────────────────────────────────────────┘
         0    1    2    3    4    5    6    7    8    9   10   11
```

```
Charts (combine freely; default: --table --scatter):
  --table  --scatter  --line  --bar  --hist
Options:
  --y N        vertical / value column      --bins N    histogram bins (default 10)
  --x N,N,...  horizontal series columns     --braille   8× line rendering
  --no-color   force monochrome (also auto-off when piped, or when NO_COLOR is set)
```

`ctplot` auto-disables color when its output is piped or redirected — so the chart you pipe
into a file is clean text, exactly like the blocks above.

---

## API at a glance

| Area      | Functions                                                                                               |
| --------- | ------------------------------------------------------------------------------------------------------- |
| Lifecycle | `ctp_initialize_dataset`, `ctp_free_dataset`                                                            |
| Load data | `ctp_add_column`, `ctp_add_row`, `ctp_add_data`, `ctp_read_csv`                                         |
| Axes      | `ctp_select_axes`, `ctp_reset_axes`                                                                     |
| Plot      | `ctp_plot`, `ctp_plot_table`, `ctp_plot_scatter`, `ctp_plot_line`, `ctp_plot_bar`, `ctp_plot_histogram` |
| Style     | `ctp_set_color`, `ctp_set_graph_braille`, `ctp_set_graph_resolution`, `ctp_set_table_width`, …          |
| Transform | `ctp_sort`, `ctp_findOne`, `ctp_findMany`, `ctp_reset_find`                                             |
| Analyze   | `ctp_analyze_mean`, `ctp_analyze_sd`, `ctp_analyze_md`                                                  |

## How it works

Everything hangs off one heap-allocated `DataSet` — column-major `float` cells with
capacities fixed at `ctp_initialize_dataset`. The vector renderers (line, bar, histogram,
Braille) rasterize into a small internal **`CtpCanvas`** — a glyph + color grid with a
Bresenham line-draw — then flush through one boxed-frame printer. Braille is the same idea at
8×: a boolean dot grid whose 2×4 cells map to code points `0x2800 + mask`.

See [CLAUDE.md](CLAUDE.md) for the full source map and conventions, and [docs/](docs/) for
per-topic guides.

## Status & roadmap

The library is correct (assertion-tested, leak-clean), structured as a real linkable library,
portable, and feature-complete for the chart types above. Remaining polish — a plot legend,
nicer axis ticks, a regression best-fit line, and CI — is tracked in [ROADMAP.md](ROADMAP.md).

## License

[MIT](LICENSE).
