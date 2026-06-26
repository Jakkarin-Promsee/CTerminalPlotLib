# CLAUDE.md

Guidance for working in this repository.

## What this is

**CTerminalPlot** (a.k.a. `CTerminalPlotLib`) is a single-file C library that renders
**tables** and **scatter plots** directly in a Windows terminal using UTF-8 box-drawing
characters and ANSI color. You build a `DataSet` in memory (columns × rows of `float`),
optionally filter/sort/analyze it, then call a `ctp_plot*` function to draw it.

It was written as a first-year university C final project (no framework, hand-rolled
"OOP in C"). Treat it as a learning/showcase codebase, not production library code — see
[Known landmines](#known-landmines).

## ⚠️ Read this before building anything

1. **You consume the library by `#include`-ing the `.c` file**, not by linking:
   ```c
   #include "../src/CTerminalPlotLib.c"   // pulls in the whole implementation
   ```
   Every example and doc does this. The whole library is one translation unit.

2. **The `Makefile` is broken.** It compiles `src/CTerminalPlotLib.c` *and* `src/main.c`
   as separate objects and links them — but `main.c` already `#include`s the `.c`, so the
   link fails with *multiple definition* errors for every symbol. Don't use `make`.
   Build a single file instead (see below).

3. **Windows-only.** The code calls `SetConsoleOutputCP(CP_UTF8)` and includes
   `<windows.h>`. It will not compile on Linux/macOS as-is, despite README claims about
   Arduino/embedded use.

4. **`src/main.c` is a scratchpad**, not a real entry point — it's full of commented-out
   experiments. The real, runnable usage lives in `examples/`.

## Build & run

The reliable build is "compile one source file that includes the library":

```sh
# Build an example (each example #includes the .c)
gcc -Isrc/include -Wall examples/2_basic-usage.c -o examples/output/2_basic-usage.exe
./examples/output/2_basic-usage.exe
```

The VS Code task **"C/C++: gcc.exe build active file"** (`.vscode/tasks.json`) does the
same for whatever file is open and is the build path that actually works. The other two
tasks ("Build C Program") replicate the broken Makefile approach.

GCC available on this machine: MinGW GCC 6.3.0 (old; no `-fanalyzer`).

## Source map

| Path | Role |
|------|------|
| `src/CTerminalPlotLib.c` | **The entire library** (~1150 lines): globals, init/free, data mgmt, sort, search, analyze, table + scatter rendering. |
| `src/include/CTerminalPlotLib.h` | Public declarations + extern globals + `CTP_PARAM`/`CTP_NULL_VALUE` config. |
| `src/include/CTerminalPlotUtils.h` | Near-empty stub (only `#include <stdio.h>`). The "utils" split was never done — utils live inside the main `.c`. |
| `src/CTerminalPlotUtils.c` | **Empty (0 bytes).** Dead file. |
| `src/main.c` | Scratch experiments. Not a demo. |
| `examples/*.c` | Runnable, documented examples (setup → basic → add-row → add-col). The source of truth for "how do I use this". |
| `docs/*.md` | Per-topic guides (1 setup, 2 basic, 3 rows, 4 columns). Incomplete; search/customize/analyze undocumented in the new docs. |
| `old-ref/` | The original version + its README and find/search examples. The old README is the most complete API reference that exists. |
| `data/` | Empty. |
| `plan.md` | Three-line TODO. |

## Core data model

Everything hangs off one heap-allocated struct (`CTerminalPlotLib.h`):

```c
typedef float CTP_PARAM;            // the element type (change this typedef to retype the lib)
#define CTP_NULL_VALUE 4.04e-10     // sentinel for "empty cell"; compared with ==

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
- **Storage is column-major**: `db[column][row]`. Data is added column-blocks at a time.
- **Capacities are fixed at `ctp_initialize_dataset()`** — there is no realloc/growth.
  Adding past `max_param`/`max_param_size` is rejected with an stderr message.
- **`db_cal` is a scratch copy** rebuilt from `db` before sorting/normalizing so the
  original order is preserved.
- **Plot config is process-global**, not per-DataSet: `TABLE_WIDTH`, `SCREEN_W/H`,
  `BORDER_EDGE`, colors, point glyphs, and flags like `print_plot_total`, `isFirstSearch`
  are file-scope globals in the `.c`. Mutating them affects every subsequent plot.

## API surface (prefix `ctp_`)

- **Lifecycle:** `ctp_initialize_dataset`, `ctp_initialize_plotproperties`, `ctp_free_dataset`
- **Load data:** `ctp_add_data` (column block), `ctp_add_row` (one row), `ctp_add_label`
- **Plot:** `ctp_plot` (table+scatter), `ctp_plot_table`, `ctp_plot_scatter`,
  and `*_search` variants that render `db_search` instead of `db`
- **Sort:** `ctp_sort`, `ctp_sort_search` (in-place quicksort by `chosen_Y_param`)
- **Search/filter:** `ctp_findOne`, `ctp_findMany` (operator string: `e`/`=`, `lt`/`<`,
  `lte`/`<=`, `gt`/`>`, `gte`), `ctp_reset_find`. `findMany` chains — successive calls
  narrow the previous result set until `ctp_reset_find()`.
- **Analyze:** `ctp_analyze_mean`, `ctp_analyze_sd`, `ctp_analyze_md` (return malloc'd
  arrays, one value per column; caller frees)
- **Inspect:** `ctp_printf_memory_usage`, `ctp_printf_properties`, `ctp_printf_dataset`
- **Config:** `ctp_set_graph_resolution`, `ctp_set_table_width`, `ctp_set_table_backspace`,
  `ctp_set_graph_border`, `ctp_set_*_reset_default`

Typical flow: `initialize → add_data → add_label → (find/sort) → plot → free`.

## Rendering pipeline (scatter)

`ctp_plot_scatter` is the most complex function. Roughly:
1. Copy `db → db_cal`.
2. Quicksort `db_cal` by the Y column.
3. `ctp_utils_normalizes` maps X/Y into `SCREEN_W × SCREEN_H` integer grid coordinates
   and computes axis min/max for tick labels.
4. Walk Y from high→low; for each row of the grid, count how many points land in each X
   cell. Single point → colored `X` (color chosen by source column, cycled mod 4);
   multiple overlapping → green `O`. Draw axes/origin where x or y == 0.
5. Print Y scale labels every `Y_SCALE_MOD` rows; X scale labels every `X_SCALE_MOD` cols.

## Conventions

- All public symbols are prefixed `ctp_`; internal helpers `ctp_utils_`.
- Naming carries baked-in typos that are part of the **public API** — keep them unless
  doing a deliberate rename pass: `avaliable_*` (parameters), `ctp_set_grap_point_*`
  ("grap"). Comments/docs also have frequent typos ("propoties", "Buttom", "mememory").
- Commit messages follow a loose conventional style: `Feature:`, `Fix:`, `Perf:`, `docs:`,
  `chore:`, `refactor:`.
- `.exe` artifacts are committed under `*/output/` despite `.gitignore` listing `*.exe`
  (they were added before the ignore rule / forced in).

## Known landmines

This code has real bugs. If you touch a function, check it against this list first; if you
are doing the refactor, these are the priority fixes. Locations are in `src/CTerminalPlotLib.c`.

- **Memory leaks:** `ctp_free_dataset` never frees `db_search`/`db_search[i]` or
  `plotProperties`. `ctp_plot_analyze` leaks each inner `malloc`. `ctp_analyze_*` leak the
  internal `n` array. `ctp_analyze_*_search` `calloc` then overwrite the pointer (leak).
- **Out-of-bounds write:** `ctp_utils_normalizes` writes `initialDone[2]` to a 2-element
  array, and the intended `initialDone[0]` is never set (so its early-break never fires).
- **Wrong math:** `ctp_analyze_sd` returns *variance* (no `sqrt`); `ctp_analyze_md` uses
  integer `abs()` on floats (truncates) — should be `fabs()`; `ctp_analyze_md_search`
  calls `ctp_analyze_sd` by mistake.
- **Broken operators:** `findMany` has no `ne` (!=) branch though docs list it; the `gte`
  operator-string table has a copy-paste `"<"` that's dead code.
- **UB:** `ctp_set_grap_point_x/overlapped` write through a pointer to a string literal.
- **Div-by-zero:** normalization divides by `(max-min)` with no guard for constant columns.
- **Init error paths** in `ctp_initialize_dataset` leak / deref NULL out of order.
- **Flag mismatch:** `ctp_plot` gates the scatter on `line_plot`, not `scatter_plot`
  (so the `scatter_plot` toggle does nothing).
- **`main.c`** prints `sizeof` with `%d` (should be `%zu`).

When fixing, prefer a single-file build of an `examples/*.c` to verify output visually.
