// ctplot — a thin command-line front-end for CTerminalPlotLib.
//
//   ctplot data.csv --line --y 1 --x 0
//   cat data.csv | ctplot --bar --y 1
//
// Reads a CSV (from a file or stdin), then renders one or more charts. Color is
// on for an interactive terminal and turns itself off when the output is piped
// or redirected (and honors --no-color / the NO_COLOR env var).
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifdef _WIN32
// MinGW hides _isatty/_fileno behind __STRICT_ANSI__ under -std=c11; declare the
// CRT prototypes ourselves (the linker resolves them from msvcrt).
int _isatty(int);
int _fileno(FILE *);
#define CTP_ISATTY _isatty
#define CTP_FILENO _fileno
#else
#include <unistd.h>
#define CTP_ISATTY isatty
#define CTP_FILENO fileno
#endif
#include "CTerminalPlotLib.h"

static void usage(const char *prog)
{
    fprintf(stderr,
            "Usage: %s [FILE|-] [charts] [options]\n"
            "  Reads CSV from FILE (or stdin when omitted / '-').\n\n"
            "Charts (combine freely; default: --table --scatter):\n"
            "  --table  --scatter  --line  --bar  --hist\n\n"
            "Options:\n"
            "  --y N         vertical / value column index\n"
            "  --x N,N,...   horizontal series column indices\n"
            "  --bins N      histogram bin count (default 10)\n"
            "  --braille     high-resolution braille strokes for --line (8x)\n"
            "  --no-color    force monochrome (auto-off when piped or NO_COLOR set)\n"
            "  -h, --help    this help\n",
            prog);
}

// Parse "0,2,3" into ints. Returns how many were read.
static int parse_ints(const char *s, int *out, int max)
{
    int n = 0;
    const char *p = s;
    while (*p && n < max)
    {
        out[n++] = atoi(p);
        const char *c = strchr(p, ',');
        if (!c)
            break;
        p = c + 1;
    }
    return n;
}

int main(int argc, char **argv)
{
    const char *file = NULL;
    bool t = false, sc = false, ln = false, bar = false, hist = false;
    bool no_color = false, braille = false;
    int y = -1, xarr[64], xn = 0, bins = 10;

    for (int i = 1; i < argc; i++)
    {
        const char *a = argv[i];
        if (!strcmp(a, "--table")) t = true;
        else if (!strcmp(a, "--scatter")) sc = true;
        else if (!strcmp(a, "--line")) ln = true;
        else if (!strcmp(a, "--bar")) bar = true;
        else if (!strcmp(a, "--hist")) hist = true;
        else if (!strcmp(a, "--no-color")) no_color = true;
        else if (!strcmp(a, "--braille")) braille = true;
        else if (!strcmp(a, "--y") && i + 1 < argc) y = atoi(argv[++i]);
        else if (!strcmp(a, "--x") && i + 1 < argc) xn = parse_ints(argv[++i], xarr, 64);
        else if (!strcmp(a, "--bins") && i + 1 < argc) bins = atoi(argv[++i]);
        else if (!strcmp(a, "-h") || !strcmp(a, "--help")) { usage(argv[0]); return 0; }
        else if (a[0] == '-' && a[1] != '\0' && strcmp(a, "-"))
        {
            fprintf(stderr, "ctplot: unknown option '%s'\n", a);
            usage(argv[0]);
            return 2;
        }
        else file = a; // positional FILE (or "-")
    }
    if (!(t || sc || ln || bar || hist)) { t = true; sc = true; }

    // Load the CSV. stdin is spooled to a temp file so ctp_read_csv can two-pass it.
    DataSet *ds = NULL;
    char tmppath[1024] = {0};
    if (!file || !strcmp(file, "-"))
    {
        const char *td = getenv("TEMP");
        if (!td) td = getenv("TMPDIR");
        if (!td) td = "/tmp";
        snprintf(tmppath, sizeof tmppath, "%s/ctplot_stdin.csv", td);
        FILE *tf = fopen(tmppath, "w");
        if (!tf) { perror("ctplot: temp file"); return 1; }
        char buf[8192];
        size_t r;
        while ((r = fread(buf, 1, sizeof buf, stdin)) > 0)
            fwrite(buf, 1, r, tf);
        fclose(tf);
        ds = ctp_read_csv(tmppath);
    }
    else
        ds = ctp_read_csv(file);

    if (!ds)
    {
        if (tmppath[0]) remove(tmppath);
        return 1;
    }

    // Monochrome when asked, or when stdout isn't an interactive terminal.
    if (no_color || !CTP_ISATTY(CTP_FILENO(stdout)))
        ctp_set_color(ds, false);
    if (braille)
        ctp_set_graph_braille(ds, true);

    // Axis selection (applies to scatter/line; sets the value column for bar/hist).
    if (y >= 0)
    {
        if (xn == 0)
            for (int c = 0; c < ds->db_cols_size; c++)
                if (c != y) xarr[xn++] = c;
        if (xn > 0)
            ctp_select_axes(ds, y, xarr, xn);
    }

    if (t) ctp_plot_table(ds);
    if (sc) ctp_plot_scatter(ds);
    if (ln) ctp_plot_line(ds);
    if (bar) ctp_plot_bar(ds);
    if (hist) ctp_plot_histogram(ds, bins);

    ctp_free_dataset(ds);
    if (tmppath[0]) remove(tmppath);
    return 0;
}
