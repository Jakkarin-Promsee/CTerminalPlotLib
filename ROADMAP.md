# CTerminalPlot — Refactor Roadmap

A leveled plan to take this from "impressive year-1 project" to "portfolio piece that
proves C mastery." We execute **one level at a time, top to bottom.** Each level leaves the
repo in a working, committable state.

## Status (branch `refactor/levels-1-3`)
- **Level 0** — folded into the work below: working build came with Level 2, the test
  harness + golden snapshots with Level 1. (Toolchain not modernized; still GCC 6.3.0.)
- **Level 1 — ✅ DONE** (commit `fix: Level 1`)
- **Level 2 — ✅ DONE** (`refactor: Level 2`). The deferred 2.3 (per-dataset config) is
  now done as part of Level 4 below.
- **Level 3 — ✅ DONE** (`refactor: Level 3` / portability).
- **Level 4 — in progress**: ergonomic data API (`ctp_add_column`), usable axis selection
  (`ctp_select_axes`), per-dataset `CtpStyle` config (2.3), public typo renames (4.4),
  sentinel cast fix (4.3, partial). Remaining: ternary-soup readability (4.1), const pass
  (4.2), quicksort pivot (4.6), and the new-feature decisions (from the post-refactor critique).
- **Level 5** — pending (docs / CI / packaging / README rewrite).

## How to use this doc
- Levels are ordered by dependency: later levels assume earlier ones are done.
- Each task has **What / Why / How / Done when**. Check the box when it's merged.
- Rule of thumb while working: *one logical change per commit; examples must still build
  and render correctly after every commit.*

## Guiding principles
1. **Don't refactor without a safety net.** Working build + tests + golden output first.
2. **Correctness before structure.** Fix bugs in place, then move correct code around.
3. **Keep it green.** Every example compiles and renders at every step.
4. **Document the final API, not a moving target.** Docs come last.

---

## Level 0 — Safety net & ground truth
*Goal: be able to build, run, and detect regressions before changing any logic.*
*Why first: the Makefile is broken and there are no tests — right now we'd be refactoring blind.*

- [ ] **0.1 Modernize the toolchain (recommended).**
  - *Why:* current compiler is MinGW.org **GCC 6.3.0 (2016)** — no `-fanalyzer`, and
    AddressSanitizer isn't available on this MinGW. We can't catch the memory bugs with it.
  - *How:* install **MSYS2** (gives modern GCC 13+/Clang with ASan+UBSan on Windows), *or*
    use **WSL + valgrind**. If you'd rather not change environment, we proceed on 6.3.0 with
    UBSan-only / manual review (slower, less safe).
  - *Done when:* `gcc --version` ≥ 12, and `-fsanitize=address,undefined` links a hello-world.

- [ ] **0.2 Get a build that actually works + fix the Makefile.**
  - *Why:* `make` currently fails with multiple-definition errors (it compiles the `.c` twice).
  - *How:* rewrite `Makefile` to build each `examples/*.c` as its own single-TU binary into
    `examples/output/`. Add `examples`, `clean`, and (later) `test` targets. Don't restructure
    the library yet — just stop the double-compile.
  - *Done when:* `make examples` builds every example; `make clean` removes artifacts.

- [ ] **0.3 Capture golden output.**
  - *Why:* the refactor must not silently change what the plots look like.
  - *How:* run each example, save stdout to `tests/golden/<name>.txt`. (Set fixed
    `SCREEN_W/H` so output is deterministic.) Later we diff against these.
  - *Done when:* a script regenerates output and `diff`s it against the goldens.

- [ ] **0.4 Minimal test harness.**
  - *Why:* we need assertions, not eyeballing, for the logic bugs in Level 1.
  - *How:* one `tests/test_ctp.c` that `#include`s the library, with tiny `assert`-based
    checks (mean of a known column, search counts, etc.). A `make test` target runs it under
    `-fsanitize=address,undefined`.
  - *Done when:* `make test` runs and (initially) **fails** on the known bugs — that's good.

**Exit criteria:** `make examples`, `make test`, golden diff all runnable from a clean checkout.

---

## Level 1 — Stop the bleeding (correctness + memory)
*Goal: every documented feature produces correct results and leaks nothing.*
*Why now: these are localized, objectively-wrong bugs; the Level 0 net catches mistakes.*
*All locations in `src/CTerminalPlotLib.c` unless noted.*

### Memory
- [ ] **1.1 `ctp_free_dataset` leaks.** Free `db_search[i]` + `db_search` and `plotProperties`
  (currently never freed). *Done:* ASan reports zero leaks on init→free.
- [ ] **1.2 `ctp_analyze_mean/sd/md` leak `n`.** `free(n)` before return. (~line 1040)
- [ ] **1.3 `ctp_plot_analyze` leaks inner allocs.** Free each `_[i]`, then `_`. (~line 532)
- [ ] **1.4 `ctp_analyze_*_search` leak.** They `calloc` then overwrite the pointer — remove
  the dead allocation. (~line 1100)
- [ ] **1.5 `ctp_initialize_dataset` error paths.** Null-check `label[i]` *before* writing
  `label[i][0]`; on any failure free what's allocated and return NULL cleanly. (~line 136)

### Correctness
- [ ] **1.6 Out-of-bounds write.** `initialDone[2]=true` → `initialDone[0]=true`; verify the
  early-break now works. (~line 445)
- [ ] **1.7 `ctp_analyze_sd`.** Take `sqrt()` of the variance (decide population vs sample,
  document it). (~line 1055)
- [ ] **1.8 `ctp_analyze_md`.** Use `fabs()` not integer `abs()`. (~line 1089)
- [ ] **1.9 `ctp_analyze_md_search`.** Call `ctp_analyze_md`, not `ctp_analyze_sd`. (~line 1141)
- [ ] **1.10 Search operators.** Implement `ne` (!=); fix the `gte` string table (stray `"<"`);
  make an unknown operator a clear no-op + stderr. (~line 907)
- [ ] **1.11 Division-by-zero in normalize.** Guard `max==min` (constant/single-point column):
  center the points instead of dividing by zero. (~line 479)
- [ ] **1.12 UB in `ctp_set_grap_point_*`.** Don't write through a string-literal pointer —
  make the glyphs writable `char[]` buffers. (~line 107)
- [ ] **1.13 `scatter_plot` flag is dead.** `ctp_plot` should honor `scatter_plot`, not
  `line_plot`. Decide what `line_plot` means (line-drawing is unimplemented). (~line 515)
- [ ] **1.14 `main.c` format bug.** `%zu` for `sizeof`. (`src/main.c:44`)
- [ ] **1.15 Wrong-pasted error strings.** `ctp_add_row/add_data/add_label` all say
  "ctp_addData" — name each correctly. (~line 274)

**Exit criteria:** `make test` passes; ASan/UBSan clean on every example; golden output
updated only where a *correctness* fix legitimately changes it (note which in the commit).

---

## Level 2 — Make it a real library (architecture)
*Goal: a header + linkable implementation, no global-state surprises.*
*Why now: biggest credibility jump, but invasive — do it once bugs are fixed and tests exist.*

- [ ] **2.1 Header/source split.** `.h` = declarations + types only; `.c` = definitions.
  Stop `#include`-ing the `.c`. Examples/tests `#include "CTerminalPlotLib.h"` and **link**.
  - *Why:* the `#include "*.c"` pattern blocks multi-file use and is the #1 "student" tell.
  - *How:* move the `extern` global *definitions* into the `.c` (they're already `extern` in
    the `.h`); switch the build to compile `libctp.a` and link it.
- [ ] **2.2 Build a static library.** `make` → `libctp.a`; examples link `-lctp`.
- [ ] **2.3 Tame global state.** Move per-run state off file-scope globals:
  - search flags (`isFirstSearch`, `isFindOne`, `print_plot_total`) → into `DataSet`.
  - render config (`TABLE_WIDTH`, `SCREEN_W/H`, `BORDER_EDGE`, colors, glyphs) → a
    `RenderConfig` struct (per-DataSet or passed in), so plots are independent & reentrant.
- [ ] **2.4 Header hygiene.** Keep `windows.h` out of the public header (only the `.c` needs
  it); include only what each file uses.
- [ ] **2.5 Remove dead scaffolding.** Either populate `CTerminalPlotUtils.{c,h}` by moving the
  `ctp_utils_*` functions there, or delete them. Fix the `quickSort`/`quicksort` name
  mismatch. Declare the `_search` analyze functions in the header (or remove them).

**Exit criteria:** `nm libctp.a` shows clean symbols; an external 2-file program links and
runs; tests now link the lib instead of including the `.c`; golden output unchanged.

---

## Level 3 — Portability decision
*Goal: the code matches its own claims about where it runs.*
*Why now: the lib is structured, so a platform shim has a clean home.*

- [ ] **3.1 Abstract the Windows console.** Put `SetConsoleOutputCP(CP_UTF8)` etc. behind a
  `ctp_platform_init()` with a POSIX no-op fallback (modern Linux/mac terminals are UTF-8 +
  ANSI by default). Guard with `#ifdef _WIN32`.
- [ ] **3.2 Build cross-desktop.** Confirm it compiles & renders on Linux/macOS (WSL is fine).
- [ ] **3.3 Fix the README's reach.** Either keep the embedded claims and actually support a
  freestanding build, or (recommended) **drop Arduino/NodeMCU** (no terminal there) and
  market it honestly as a cross-platform desktop terminal-plot lib.

**Exit criteria:** same binary source builds on Windows + Linux; README claims are true.

---

## Level 4 — Readability & API polish
*Goal: code that reads like a pro wrote it; a clean, consistent public API.*
*Why now: correctness + structure are locked, so cosmetic/API changes are low-risk.*

- [ ] **4.1 Kill the ternary soup.** In `ctp_plot_table_customize` and `ctp_plot_scatter`,
  hoist the repeated `customize_display ? A : B` into named locals at function top; extract
  border-drawing into helpers. (~lines 571, 661)
- [ ] **4.2 `const`-correctness pass** across the API.
- [ ] **4.3 Replace the float sentinel.** `CTP_NULL_VALUE` compared with `==` is fragile —
  use a parallel presence bitmask (or at minimum document the limitation).
- [ ] **4.4 API rename (one deliberate breaking commit).** `avaliable_*`→`available_*`,
  `ctp_set_grap_*`→`ctp_set_graph_*`. Note it loudly in the commit + CHANGELOG.
- [ ] **4.5 Consistent error strategy.** Pick return-codes *or* stderr and apply uniformly.
- [ ] **4.6 (Nice-to-have)** quicksort pivot → median-of-three to avoid O(n²) on sorted input.

**Exit criteria:** no function body repeats the same ternary >2×; API names are consistent;
tests + golden output still green.

---

## Level 5 — Docs, tests & packaging (the showcase finish)
*Goal: what a recruiter/reviewer sees first is polished and accurate.*
*Why last: docs should describe the finished API.*

- [ ] **5.1 Rewrite the top-level `README.md`.** Hero screenshots (the scatter/table look
  great), accurate feature list, real build/install + quickstart, link to `docs/`. (Current
  README cuts off at "## Showcase".)
- [ ] **5.2 Finish `docs/`.** Add search, customize-axis, and analyze guides; fix the wrong
  type sizes (float=4, double=8, bool=1 byte); fix section numbering (1→3→5→6) and the dead
  `[Filter or Search]()` link.
- [ ] **5.3 Spelling sweep** of docs/comments ("propoties", "Buttom", "mememory", "legth"…).
- [ ] **5.4 Expand the test suite** + run it under sanitizers in **GitHub Actions CI**
  (badge in README — strong signal).
- [ ] **5.5 Repo cleanup.** `git rm --cached` the committed `*.exe`; keep `output/` dirs
  ignored. Verify `LICENSE`. Add a `CHANGELOG.md`. Retire/replace this `ROADMAP.md`'s stub
  ancestor `plan.md`.

**Exit criteria:** a stranger can clone, read the README, build, and run an example in under
five minutes — and the code backs up every claim the README makes.

---

### Suggested first session
Level 0 only: **0.2 (working Makefile)** → **0.4 (a failing test that proves a bug)**.
That gives us the net; then Level 1 is fast, satisfying bug-squashing. 🔥
