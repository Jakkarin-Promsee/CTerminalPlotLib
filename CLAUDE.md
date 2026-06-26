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
make            # builds build/libctp.a + every examples/*.c into examples/output/
make test       # builds & runs the assertion tests (tests/test_ctp.c)
make clean      # removes build artifacts
```

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
| `src/main.c` | Scratch experiments. Not a demo (links the lib like the examples). |
| `docs/*.md` | Per-topic guides. Incomplete and **still show the old `#include "*.c"` usage** — slated for the Level 5 docs pass. |
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
    int chosen_Y_param, *chosen_X_param, chosen_X_param_size;  // axis selection
    int show_begin, show_end;                      // row window
    PlotProperties *plotProperties;                // table/scatter/line/customize toggles
} DataSet;
```

Key facts:
- **Storage is column-major**: `db[column][row]`; data is added column-blocks at a time.
- **Capacities are fixed at `ctp_initialize_dataset()`** — no realloc/growth; adding past
  the limits is rejected with an stderr message.
- **`db_cal` is a scratch copy** rebuilt from `db` before sorting/normalizing so the original
  order is preserved.
- Lifecycle is leak-clean: `ctp_initialize_dataset` cleans up on partial failure;
  `ctp_free_dataset` frees everything (incl. `db_search`, `plotProperties`) and is NULL-safe.

## API surface (prefix `ctp_`)

- **Lifecycle:** `ctp_initialize_dataset`, `ctp_initialize_plotproperties`, `ctp_free_dataset`
- **Load data:** `ctp_add_column` (preferred: one labelled column from a 1-D array),
  `ctp_add_data` (column block), `ctp_add_row` (one row), `ctp_add_label`
- **Axes:** `ctp_select_axes(ds, y_col, x_cols, n)` / `ctp_reset_axes` — pick Y + X columns
  (enables the customized view, defaults the row window to all rows)
- **Plot:** `ctp_plot` (honors the `table_plot`/`scatter_plot`/`line_plot` flags),
  `ctp_plot_table`, `ctp_plot_scatter`, `ctp_plot_line`, and `*_search` variants that
  render `db_search`. The line plot rasterizes via the internal `CtpCanvas` (Bresenham
  strokes), connecting each X series ordered by the shared Y value.
- **Sort:** `ctp_sort`, `ctp_sort_search` (quicksort by `chosen_Y_param`)
- **Search/filter:** `ctp_findOne`, `ctp_findMany` (operators: `e`/`=`/`==`, `ne`/`!=`,
  `lt`/`<`, `lte`/`<=`, `gt`/`>`, `gte`/`>=`), `ctp_reset_find`. `findMany` chains — each
  call narrows the previous result until `ctp_reset_find()`.
- **Analyze:** `ctp_analyze_mean` / `_sd` (population std dev) / `_md` (mean abs deviation),
  plus `_search` variants. Return malloc'd arrays (one per column); **caller frees**.
- **Inspect:** `ctp_printf_memory_usage`, `ctp_printf_properties`, `ctp_printf_dataset`
- **Style (per-dataset):** `ctp_default_style`, and setters that all take the `DataSet*`:
  `ctp_set_table_width`, `ctp_set_table_backspace`, `ctp_set_graph_resolution`,
  `ctp_set_graph_border`, `ctp_set_graph_point_x/_overlapped`, `ctp_set_*_reset_default`

Typical flow: `initialize → add_column… → (select_axes / find / sort) → plot → free`.

## Conventions & remaining rough edges

- Public symbols are prefixed `ctp_`; internal helpers `ctp_utils_`; file-private state is
  `static`.
- **Mutable render config is per-dataset** (`DataSet.style`, a `CtpStyle`): table width,
  back-space, scatter resolution, border, point glyphs. Set via the `ctp_set_*` functions.
  Decorative glyphs/colors (corners, axes, ANSI colors) remain file-scope `static const`
  in the `.c` — read-only, so safely shared.
- `CTP_NULL_VALUE` is still a float *sentinel* (now cast to `CTP_PARAM` so comparisons
  round-trip). Real data equal to `4.04e-10` would collide; a presence mask is the
  someday-fix. Empty cells render blank in tables.
- `PlotProperties.line_plot` defaults to **off** (so `ctp_plot` stays table+scatter unless
  asked); set the flag or call `ctp_plot_line` directly. The internal `CtpCanvas` (a
  glyph+color grid with a Bresenham line-draw) is the rasterization foundation the bar and
  braille renderers will also build on.
- `ctp_plot_scatter`, if called directly (not via `ctp_plot`), doesn't run
  `ctp_platform_init()` first. (`ctp_plot_line` does init itself.)
- Commit style: `feat:`/`fix:`/`refactor:`/`docs:`/`chore:`.

## When changing code

Keep examples building and rendering at every step: after a change run `make && make test`
and diff the examples against `tests/golden/`. If a change legitimately alters rendering,
regenerate the affected golden file and call it out in the commit.
