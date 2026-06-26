<!-- 5_data-tools.md -->

# CTerminalPlot — Search, Sort & Analyze

Beyond drawing, a `DataSet` can be **filtered**, **sorted**, and **summarized**. These tools
all operate on the data you've already loaded.

## Search / filter

Filtering keeps the rows that match a comparison and stores them in a separate result set
(`db_search`), leaving your original data untouched.

```c
void ctp_reset_find(void);                                              // start fresh
void ctp_findMany(DataSet *ds, int col, char *op, CTP_PARAM value);     // keep all matches
void ctp_findOne (DataSet *ds, int col, char *op, CTP_PARAM value);     // keep first match
```

**Operators** (symbol or short name, both accepted):

| Meaning           | Forms              |
| ----------------- | ------------------ |
| equal             | `e`, `=`, `==`     |
| not equal         | `ne`, `!=`         |
| less than         | `lt`, `<`          |
| less or equal     | `lte`, `<=`        |
| greater than      | `gt`, `>`          |
| greater or equal  | `gte`, `>=`        |

`ctp_findMany` **chains**: each call narrows the previous result, so you can build up an
`AND` of conditions. Call `ctp_reset_find()` to begin a new, independent search.

```c
ctp_reset_find();
ctp_findMany(ds, 0, "gte", 5);   // keep rows where column 0 >= 5
ctp_plot_table_search(ds);       // render just the matches
```

Given `value = {5, 2, 8, 1, 9, 4, 7, 3}` (and an `index` column for reference):

```
Search Found: 4 data
┌──────────┬──────────┐
│   value  │   index  │
├──────────┼──────────┤
│    5.00  │    0.00  │
├──────────┼──────────┤
│    8.00  │    2.00  │
├──────────┼──────────┤
│    9.00  │    4.00  │
├──────────┼──────────┤
│    7.00  │    6.00  │
└──────────┴──────────┘
```

The matches keep their **original order** (the `index` column shows which rows they came
from). Read results from `ds->db_search` / `ds->db_search_size`, or render them with the
`_search` renderers: `ctp_plot_table_search`, `ctp_plot_scatter_search`, `ctp_plot_search`.

## Sort

`ctp_sort` reorders the rows **in place** by the chosen X column (column 0 by default, or
whatever you passed to [`ctp_select_axes`](4_Adding-Column.md)). All columns move together, so
rows stay intact.

```c
ctp_sort(ds);          // sort the whole dataset by the X column, ascending
ctp_sort_search(ds);   // sort the current search result set instead
```

Sorting the same data by `value`:

```
┌──────────┬──────────┐
│   value  │   index  │
├──────────┼──────────┤
│    1.00  │    3.00  │
├──────────┼──────────┤
│    2.00  │    1.00  │
├──────────┼──────────┤
   … 3 … 7 …
├──────────┼──────────┤
│    8.00  │    2.00  │
├──────────┼──────────┤
│    9.00  │    4.00  │
└──────────┴──────────┘
```

(`index` is now shuffled — it records where each row started.)

## Analyze

Per-column statistics. Each returns a freshly `malloc`'d array with **one value per column** —
**the caller frees it**. Empty cells (`CTP_NULL_VALUE`) are ignored.

```c
CTP_PARAM *ctp_analyze_mean(DataSet *ds);   // arithmetic mean
CTP_PARAM *ctp_analyze_sd  (DataSet *ds);   // population standard deviation
CTP_PARAM *ctp_analyze_md  (DataSet *ds);   // mean absolute deviation
```

```c
CTP_PARAM *mean = ctp_analyze_mean(ds);
CTP_PARAM *sd   = ctp_analyze_sd(ds);
CTP_PARAM *md   = ctp_analyze_md(ds);

printf("value: mean=%.2f  sd=%.2f  md=%.2f\n",
       (double)mean[0], (double)sd[0], (double)md[0]);

free(mean); free(sd); free(md);     // <-- don't forget
```

For `value = {5, 2, 8, 1, 9, 4, 7, 3}`:

```
value: mean=4.88  sd=2.71  md=2.38
```

There are `_search` variants (`ctp_analyze_mean_search`, …) that summarize the current search
result set instead of the full data.

## Putting it together

A common flow: **filter → sort → plot**.

```c
ctp_reset_find();
ctp_findMany(ds, 0, "gte", 5);   // only the rows we care about
ctp_sort_search(ds);             // tidy them
ctp_plot_search(ds);             // table + scatter of the result
```

## See also

- [Adding columns & choosing axes](4_Adding-Column.md) — sort respects the X column you
  select; analyze runs per column.
- [README showcases](../README.md#showcases) — the chart renderers.
