// Minimal assertion tests for CTerminalPlot — proves the Level 1 correctness fixes.
// Build & run:  make test
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "../src/include/CTerminalPlotLib.h"

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

static void test_add_column(void)
{
    DataSet *ds = ctp_initialize_dataset(3, 20, 8);
    CTP_PARAM a[] = {1, 2, 3};
    CTP_PARAM b[] = {4, 5, 6, 7};
    ctp_add_column(ds, "a", a, 3);
    ctp_add_column(ds, "b", b, 4);

    assert(ds->db_cols_size == 2);
    assert(ds->db_rows_size == 4); // max of the column lengths
    assert(strcmp(ds->label[0], "a") == 0);
    assert(strcmp(ds->label[1], "b") == 0);
    assert(APPROX(ds->db[0][0], 1.0));
    assert(APPROX(ds->db[1][3], 7.0));
    assert(ds->db[0][3] == CTP_NULL_VALUE); // ragged tail stays "empty"

    ctp_free_dataset(ds);
    printf("  [ok] add_column: cols/rows/labels/ragged tail\n");
}

static void test_select_axes(void)
{
    DataSet *ds = ctp_initialize_dataset(3, 20, 8);
    CTP_PARAM y[] = {0, 1, 2, 3};
    CTP_PARAM x1[] = {3, 2, 1, 0};
    CTP_PARAM x2[] = {0, 2, 4, 6};
    ctp_add_column(ds, "y", y, 4);
    ctp_add_column(ds, "x1", x1, 4);
    ctp_add_column(ds, "x2", x2, 4);

    int xs[] = {1, 2};
    ctp_select_axes(ds, 0, xs, 2);
    assert(ds->chosen_Y_param == 0);
    assert(ds->chosen_X_param_size == 2);
    assert(ds->chosen_X_param[0] == 1 && ds->chosen_X_param[1] == 2);
    assert(ds->plotProperties->customize_display == true);
    assert(ds->show_end == 4); // defaulted to all rows

    ctp_reset_axes(ds);
    assert(ds->plotProperties->customize_display == false);

    ctp_free_dataset(ds);
    printf("  [ok] select_axes: Y/X selection + show_end default\n");
}

static void test_csv(void)
{
    const char *path = "tests/output/_ctp_test.csv";
    FILE *f = fopen(path, "w");
    assert(f);
    fprintf(f, "a,b,c\n1,2,3\n4,,6\n7,8,9\n"); // note the empty middle cell
    fclose(f);

    DataSet *ds = ctp_read_csv(path);
    assert(ds);
    assert(ds->db_cols_size == 3);
    assert(ds->db_rows_size == 3);
    assert(strcmp(ds->label[0], "a") == 0);
    assert(strcmp(ds->label[2], "c") == 0);
    assert(APPROX(ds->db[0][0], 1.0));
    assert(APPROX(ds->db[2][2], 9.0));
    assert(ds->db[1][1] == CTP_NULL_VALUE); // empty cell -> null

    ctp_free_dataset(ds);
    remove(path);
    printf("  [ok] csv: cols/rows/labels/values/empty-cell\n");
}

int main(void)
{
    printf("Running CTerminalPlot tests...\n");
    test_statistics();
    test_search();
    test_add_column();
    test_select_axes();
    test_csv();
    printf("All tests passed.\n");
    return 0;
}
