// Minimal assertion tests for CTerminalPlot — proves the Level 1 correctness fixes.
// Build (current single-TU model): gcc -Isrc/include tests/test_ctp.c -o test.exe -lm
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "../src/CTerminalPlotLib.c"

#define APPROX(a, b) (fabs((double)(a) - (double)(b)) < 1e-4)

static void test_statistics(void)
{
    // Column {2,4,4,4,5,5,7,9}: mean = 5, population sd = 2, mean-abs-dev = 1.5
    DataSet *ds = ctp_initialize_dataset(1, 10, 8);
    CTP_PARAM col[8] = {2, 4, 4, 4, 5, 5, 7, 9};
    ctp_add_data(ds, col, 8, 1, 8);

    CTP_PARAM *mean = ctp_analyze_mean(ds);
    CTP_PARAM *sd = ctp_analyze_sd(ds);
    CTP_PARAM *md = ctp_analyze_md(ds);

    assert(APPROX(mean[0], 5.0)); // mean
    assert(APPROX(sd[0], 2.0));   // sqrt(32/8) = 2   (was returning variance = 4)
    assert(APPROX(md[0], 1.5));   // 12/8 = 1.5       (was truncated to 0 by int abs)

    free(mean);
    free(sd);
    free(md);
    ctp_free_dataset(ds);
    printf("  [ok] statistics: mean=5, sd=2, md=1.5\n");
}

static void test_search(void)
{
    // Column {1,2,3,4,5}
    DataSet *ds = ctp_initialize_dataset(1, 10, 5);
    CTP_PARAM col[5] = {1, 2, 3, 4, 5};
    ctp_add_data(ds, col, 5, 1, 5);

    ctp_reset_find();
    ctp_findMany(ds, 0, "gte", 3);
    assert(ds->db_search_size == 3); // 3,4,5  (gte was broken: mapped to "<")

    ctp_reset_find();
    ctp_findMany(ds, 0, "lt", 3);
    assert(ds->db_search_size == 2); // 1,2

    ctp_reset_find();
    ctp_findMany(ds, 0, "ne", 3);
    assert(ds->db_search_size == 4); // 1,2,4,5  (ne was unimplemented)

    ctp_reset_find();
    ctp_findMany(ds, 0, "e", 3);
    assert(ds->db_search_size == 1); // 3

    ctp_free_dataset(ds);
    printf("  [ok] search: gte/lt/ne/e counts correct\n");
}

int main(void)
{
    printf("Running CTerminalPlot tests...\n");
    test_statistics();
    test_search();
    printf("All tests passed.\n");
    return 0;
}
