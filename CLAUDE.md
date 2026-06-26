# CLAUDE.md

Guidance for working in this repository.

## What this is

**CTerminalPlot** (a.k.a. `CTerminalPlotLib`) is a small C library that renders **tables**
and **scatter plots** directly in a terminal using UTF-8 box-drawing characters and ANSI
color. You build a `DataSet` in memory (columns × rows of `float`), optionally
filter/sort/analyze it, then call a `ctp_plot*` function to draw it.

It began as a first-year university C final project (hand-rolled "OOP in C") and is being
refactored into a portfolio-quality library. Progress is tracked in [ROADMAP.md](ROADMAP.md):
**Levels 1–3 are done** (correctness/memory, real library structure, portability);
Levels 4–5 (a matplotlib-style API redesign + docs/CI) remain.

## How to build & use it

It is a normal header + static library now. **Include the header and link the library** —
do *not* `#include` the `.c` (that was the original mistake; it caused duplicate-symbol
link errors).

```sh
make            # builds libctp.a, every examples/*.c, and the ctplot CLI (build/ctplot.exe)
make test       # builds & runs the assertion tests (tests/test_ctp.c)
make clean      # removes build artifacts
```

There's also a small CLI built on the library:

```sh
build/ctplot.exe examples/data/sample.csv --line --x 0 --y 1   # plot a CSV column
cat data.csv | build/ctplot.exe --bar --y 1                    # or pipe via stdin
```
`ctplot` auto-disables color when its output is piped/redirected (or `--no-color` / `NO_COLOR`).

To compile one program by hand (also what the default VS Code task does):

```sh
gcc -Isrc/include your_program.c src/CTerminalPlotLib.c -o your_program -lm
# or link the prebuilt lib:  gcc -Isrc/include your_program.c build/libctp.a -o your_program -lm
```

In your code:

```c
#include "CTerminalPlotLib.h"   // with -Isrc/include
```

Toolchain on this machine: MinGW GCC **6.3.0** (old — no `-fanalyzer`, no AddressSanitizer
on this MinGW). `make` is provided as `mingw32-make`. Correctness is currently verified by
the test suite + golden-output diffs, not by a sanitizer; if you have MSYS2/Clang or
WSL+valgrind, prefer that for memory verification.

## Portability

Cross-platform by design: the only OS-specific code (`SetConsoleOutputCP(CP_UTF8)`) lives in
the `ctp_platform_init()` shim, guarded by `#ifdef _WIN32`. On Linux/macOS it falls back to
just `setlocale`. The source compiles with the non-Windows branch forced
(`gcc -U_WIN32 ...`), but has only been *run* on Windows so far — verify rendering on
Linux/macOS before claiming it there.

## Source map

| Path | Role |
|------|------|
| `src/CTerminalPlotLib.c` | **The entire implementation** (~1100 lines): globals, init/free, data mgmt, sort, search, analyze, table + scatter rendering, the platform shim. |
| `src/include/CTerminalPlotLib.h` | Public API: types (`DataSet`, `PlotProperties`, `CTP_PARAM`, `CTP_NULL_VALUE`), extern config globals, prototypes. Includes only `<stdbool.h>`. |
| `examples/*.c` | Runnable, documented programs (1 setup → 2 basic → 3 rows → 4 columns). Each includes the header and links the library. The source of truth for "how do I use this". |
| `tests/test_ctp.c` | Assertion tests (mean/sd/md + search counts). Run via `make test`. |
| `tests/golden/*.txt` | Captured plot output; diff against these to catch rendering regressions. |
| `src/ctplot.c` | The **`ctplot` CLI** — reads a CSV (file or stdin), renders table/scatter/line/bar/hist via flags; auto-mono when piped. `make` builds it to `build/ctplot.exe`. |
| `src/main.c` | Scratch experiments. Not a demo (links the lib like the examples). |
| `docs/*.md` | Per-topic manual (setup, basic usage, rows, columns/axes, search-sort-analyze) + index `0_all_docs.md`. Rewritten to the linkable-library API with real text output; complements the README showcases. |
| `old-ref/` | The original version + its README and search examples (archive; not built). |
| `build/` | Generated `libctp.a` + objects (git-ignored). |

(The empty `CTerminalPlotUtils.{c,h}` stubs were removed in Level 2.)

## Core data model

Everything hangs off one heap-allocated struct (`CTerminalPlotLib.h`):

```c
typedef float CTP_PARAM;            // element type (change this typedef to retype the lib)
#define CTP_NULL_VALUE 4.04e-10     // sentinel for "empty cell"; compared with == (fragile)

typedef struct {
    int max_param, max_name_size, max_param_size;  // capacities (fixed at init)
    char **label;                                  // [col] -> name
    CTP_PARAM **db;                                // [col][row]  <-- COLUMN-MAJOR
    CTP_PARAM **db_search;                         // filtered results
    CTP_PARAM **db_cal;                            // scratch for sort/normalize
    int db_cols_size, db_rows_size, db_search_size;
    int chosen_X_param, *chosen_Y_param, chosen_Y_param_size;  // axis selection (X single, Y series)
    int show_begin, show_end;                      // row window
    PlotProperties *plotProperties;                // table/scatter/line/customize toggles
} DataSet;
```

Key facts:
- **Storage is column-major**: `db[column][row]`; data is added column-blocks at a time.
- **Capacities are fixed at `ctp_initialize_dataset()`** — no realloc/growth; adding past
  the limits is rejected with an stderr message.
- **`db_cal` is a scratch copy** rebuilt from `db` before searching/sorting so the original
  order is preserved.
- Lifecycle is leak-clean: `ctp_initialize_dataset` cleans up on partial failure;
  `ctp_free_dataset` frees everything (incl. `db_search`, `plotProperties`) and is NULL-safe.

## API surface (prefix `ctp_`)

- **Lifecycle:** `ctp_initialize_dataset`, `ctp_initialize_plotproperties`, `ctp_free_dataset`
- **Load data:** `ctp_add_column` (preferred: one labelled column from a 1-D array),
  `ctp_add_data` (column block), `ctp_add_row` (one row), `ctp_add_label`
- **Data I/O:** `ctp_read_csv(path)` — header row → labels, numeric rows → columns,
  blank/non-numeric cells → `CTP_NULL_VALUE`. Simple comma split (no quoted fields).
  Returns a fresh `DataSet*` (caller frees). Sample at `examples/data/sample.csv`.
- **Axes:** `ctp_select_axes(ds, x_col, y_cols, n)` / `ctp_reset_axes` — pick the single X
  column (independent, horizontal) + the Y series columns (vertical). Convention: **column 0
  is X by default; columns 1.. are the Y series.** Enables the customized view, defaults the
  row window to all rows.
- **Plot:** `ctp_plot` (honors the `table_plot`/`scatter_plot`/`line_plot` flags),
  `ctp_plot_table`, `ctp_plot_scatter`, `ctp_plot_line`, `ctp_plot_bar`,
  `ctp_plot_histogram(ds, bins)`, and `*_search` variants that render `db_search`. The
  scatter/line/bar/histogram renderers all rasterize via the internal `CtpCanvas` (Bresenham
  strokes / block bars) and flush through `ctp_canvas_flush` (boxed frame + Y scale) +
  `ctp_canvas_print_xaxis`. Scatter and line share `ctp_xy_prepare` (X column + Y series +
  bounds): scatter drops each Y series' markers against X over a zero-axis crosshair
  (`ctp_canvas_draw_axes`, drawn behind the points; distinct-series cell collisions become the
  overlap glyph), line strokes each Y series against X in row order. Bar draws one
  vertical block bar per row of the value column (first selected Y series, else column 0) on a
  zero baseline (green up / red down); histogram bins one column and bars the counts (bar +
  histogram share `ctp_draw_bars`).
- **Sort:** `ctp_sort`, `ctp_sort_search` (quicksort by `chosen_X_param`, the X column)
- **Search/filter:** `ctp_findOne`, `ctp_findMany` (operators: `e`/`=`/`==`, `ne`/`!=`,
  `lt`/`<`, `lte`/`<=`, `gt`/`>`, `gte`/`>=`), `ctp_reset_find`. `findMany` chains — each
  call narrows the previous result until `ctp_reset_find()`.
- **Analyze:** `ctp_analyze_mean` / `_sd` (population std dev) / `_md` (mean abs deviation),
  plus `_search` variants. Return malloc'd arrays (one per column); **caller frees**.
- **Inspect:** `ctp_printf_memory_usage`, `ctp_printf_properties`, `ctp_printf_dataset`
- **Style (per-dataset):** `ctp_default_style`, and setters that all take the `DataSet*`:
  `ctp_set_table_width`, `ctp_set_table_backspace`, `ctp_set_graph_resolution`,
  `ctp_set_graph_border`, `ctp_set_graph_point_x/_overlapped`, `ctp_set_color`,
  `ctp_set_*_reset_default`

Typical flow: `initialize → add_column… → (select_axes / find / sort) → plot → free`.

## Conventions & remaining rough edges

- Public symbols are prefixed `ctp_`; internal helpers `ctp_utils_`; file-private state is
  `static`.
- **Mutable render config is per-dataset** (`DataSet.style`, a `CtpStyle`): table width,
  back-space, scatter resolution, border, point glyphs, and `use_color`. Set via the
  `ctp_set_*` functions. Decorative glyphs/colors (corners, axes, ANSI colors, mono markers)
  remain file-scope `static const` in the `.c` — read-only, so safely shared.
- **Color vs monochrome:** `style.use_color` (default on; auto-off when the `NO_COLOR` env
  var is set, toggle with `ctp_set_color`). Mono mode tells series apart by marker *shape*
  (`MONO_MARKERS` ● ■ ▲ ◆), overlaps with `⊕`, and bar sign by fill (`█` up / `▒` down) —
  no ANSI emitted. Default point glyphs are `●` / `◉`. Every renderer honors the flag.
- **Braille hi-res:** `style.braille` (default off; `ctp_set_graph_braille`) routes
  `ctp_plot_line` through a 2×4-dot rasterizer — a `W×H` canvas addresses a `2W×4H` pixel
  grid (8×), each cell emitting `0x2800 + mask`. Reuses the same bounds + `ctp_canvas_flush`
  frame as the block path; only the rasterization differs (`ctp_rasterize_braille` vs
  `ctp_rasterize_lines`). CLI: `--braille`.
- `CTP_NULL_VALUE` is still a float *sentinel* (now cast to `CTP_PARAM` so comparisons
  round-trip). Real data equal to `4.04e-10` would collide; a presence mask is the
  someday-fix. Empty cells render blank in tables.
- `PlotProperties.line_plot` defaults to **off** (so `ctp_plot` stays table+scatter unless
  asked); set the flag or call `ctp_plot_line` directly. The internal `CtpCanvas` (a
  glyph+color grid with a Bresenham line-draw) is the shared rasterization foundation for the
  scatter, line, bar, and braille renderers; scatter and line share the `ctp_xy_prepare`
  axis/bounds setup.
- Every public renderer calls `ctp_platform_init()` itself (including `ctp_plot_scatter`), so
  a direct call still emits correct UTF-8 without going through `ctp_plot`.
- Commit style: `feat:`/`fix:`/`refactor:`/`docs:`/`chore:`.

## When changing code

Keep examples building and rendering at every step: after a change run `make && make test`
and diff the examples against `tests/golden/`. If a change legitimately alters rendering,
regenerate the affected golden file and call it out in the commit.
