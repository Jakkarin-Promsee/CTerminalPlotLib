<!-- 1_Setup-Guide.md -->

# CTerminalPlot — Setup & Linking

CTerminalPlot is a normal C library: **one header you include, one static library you link
against.** This page gets it building in your project.

> **Do not `#include` the `.c` file.** Early versions did, and it caused duplicate-symbol
> link errors the moment a project had more than one translation unit. Include the header;
> link the library.

## 1. Get the source

```sh
git clone <repository_url> CTerminalPlot
cd CTerminalPlot
```

Everything you need is under `src/`:

| Path                              | Role                                   |
| --------------------------------- | -------------------------------------- |
| `src/include/CTerminalPlotLib.h`  | the public header — include this       |
| `src/CTerminalPlotLib.c`          | the implementation — compile/link this |

## 2. Build the static library

```sh
make            # produces build/libctp.a (plus the examples and the ctplot CLI)
```

`make` needs a Unix-style shell (Git Bash / MSYS2 / Linux / macOS) and a C11 compiler.

## 3. Link it into your program

Include the header (with `-Isrc/include` so the compiler can find it) and link the library:

```c
#include "CTerminalPlotLib.h"
```

```sh
gcc -Isrc/include myplot.c build/libctp.a -o myplot -lm
```

Prefer not to run `make` first? Compile the library source directly alongside your program —
a single translation unit, so no duplicate-symbol problem:

```sh
gcc -Isrc/include myplot.c src/CTerminalPlotLib.c -o myplot -lm
```

`-lm` links the math library (`libm`), which the renderers use. That's the only dependency.

## 4. Verify the setup

```c
#include <stdio.h>
#include "CTerminalPlotLib.h"

int main(void)
{
    if (ctp_isActive())
        printf("CTerminalPlotLib setup successful!\n");
    else
        printf("CTerminalPlotLib setup failed!\n");
    return 0;
}
```

If it prints `CTerminalPlotLib setup successful!`, you're linked and ready.

## Troubleshooting

- **Garbled glyphs (e.g. `Ã¢ÂÂ`) instead of boxes/curves** — your terminal isn't in UTF-8.
  The library calls `ctp_platform_init()` (which sets the Windows console to UTF-8) from
  every top-level `ctp_plot*`; on Linux/macOS modern terminals are UTF-8 already.
- **No color** — color auto-disables when `NO_COLOR` is set or when output is piped. Force it
  on/off in code with `ctp_set_color(ds, true/false)`.
- **`undefined reference to sin`/`floor`** — add `-lm` to the link line.

## Next

→ [Basic usage](2_basic-usage.md): build a `DataSet` and draw your first table + scatter.
