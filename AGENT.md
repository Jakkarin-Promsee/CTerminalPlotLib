# AGENT.md

Guidance for AI coding agents working in this repository.

> **Source of truth: [CLAUDE.md](CLAUDE.md).** This file is a short pointer; everything
> below is summarized from it. When the two ever disagree, follow `CLAUDE.md` and fix this
> file. Read `CLAUDE.md` in full before making non-trivial changes — it has the complete
> source map, data model, API surface, and conventions.

## What this is

**CTerminalPlot** is a small, dependency-free **C11** library (plus a `ctplot` CLI) that
renders tables and scatter / line / bar / histogram charts directly in the terminal using
UTF-8 box-drawing and ANSI color. You build a `DataSet` in memory (column-major `float`
cells), optionally filter / sort / analyze it, then call a `ctp_plot*` function. The whole
implementation is one `.c` file behind one header.

## Build & test

It is a header + static library. **Include the header and link the library — never
`#include` the `.c`** (that causes duplicate-symbol link errors).

```sh
make            # build build/libctp.a, every examples/*.exe, and build/ctplot.exe
make test       # build & run the assertion tests (tests/test_ctp.c)
make clean      # remove build artifacts
```

On Windows the make tool is **`mingw32-make`**, run from a Unix-style shell (Git Bash /
MSYS2) — the Makefile uses `mkdir -p`, `rm`, `ar`. Toolchain here is MinGW GCC 6.3.0 (old:
no `-fanalyzer`, no ASan), so correctness is verified by the **assertion tests + golden-output
diffs**, not a sanitizer.

Compile one program by hand:

```sh
gcc -Isrc/include your_program.c build/libctp.a -o your_program -lm   # or src/CTerminalPlotLib.c
```

## Where things live

| Path | Role |
|------|------|
| `src/CTerminalPlotLib.c` | the entire implementation (~2,000 lines) |
| `src/include/CTerminalPlotLib.h` | the public API (types, prototypes, config) |
| `src/ctplot.c` | the `ctplot` CLI front-end |
| `examples/*.c` | runnable, documented programs — the "how do I use this" |
| `tests/test_ctp.c`, `tests/golden/*.txt` | assertion tests + captured-output snapshots |
| `docs/*.md` | per-topic guides (index: `docs/0_all_docs.md`) |

## Working rules

- **Keep it green at every step.** After any change run `make && make test` and diff the
  examples against `tests/golden/`. If a change legitimately alters rendering, regenerate the
  affected golden file and call it out in the commit.
- **Public symbols are prefixed `ctp_`**, internal helpers `ctp_utils_`, file-private state is
  `static`. Mutable render config is **per-`DataSet`** (`CtpStyle`), not global.
- **Commit style:** `feat:` / `fix:` / `refactor:` / `docs:` / `chore:`.
- See [CLAUDE.md](CLAUDE.md) for the full data model, API surface, portability shim, and the
  remaining rough edges (e.g. the `CTP_NULL_VALUE` float sentinel).
