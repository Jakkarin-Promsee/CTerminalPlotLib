<!-- 3_Adding-Rows.md -->

# CTerminalPlot — Adding Rows

Sometimes data arrives a record at a time rather than a column at a time — readings from a
loop, a sensor, a stream. `ctp_add_row` appends one row across **all** current columns.

```c
void ctp_add_row(DataSet *ds, CTP_PARAM data[]);
```

- `data` must hold one value **per existing column**, in column order.
- The dataset must have room (`db_rows_size < max_param_size`); a full dataset rejects the
  row with a message on `stderr`.
- Add your columns (or at least know how many there are) before adding rows.

## Example

Start from the three-column dataset in [Basic usage](2_basic-usage.md), then append one more
data point `(4, 4, -4)`:

```c
DataSet *ds = ctp_initialize_dataset(/*cols*/ 3, /*name*/ 20, /*rows*/ 10);

ctp_add_column(ds, "y",    (CTP_PARAM[]){-3,-2,-1,0,1,2,3}, 7);
ctp_add_column(ds, "y = x", (CTP_PARAM[]){-3,-2,-1,0,1,2,3}, 7);
ctp_add_column(ds, "y = -x",(CTP_PARAM[]){ 3, 2, 1,0,-1,-2,-3}, 7);

CTP_PARAM new_row[3] = {4, 4, -4};   // one value per column
ctp_add_row(ds, new_row);

ctp_plot_table(ds);
```

### Output

The new `4.00 / 4.00 / -4.00` row appears at the bottom:

```
┌──────────┬──────────┬──────────┐
│       x  │   y = x  │  y = -x  │
├──────────┼──────────┼──────────┤
│   -3.00  │   -3.00  │    3.00  │
├──────────┼──────────┼──────────┤
   … rows -2 … 2 …
├──────────┼──────────┼──────────┤
│    3.00  │    3.00  │   -3.00  │
├──────────┼──────────┼──────────┤
│    4.00  │    4.00  │   -4.00  │
└──────────┴──────────┴──────────┘
```

## Ragged columns render as blank

Columns don't have to be the same length. Cells you never fill keep the internal
`CTP_NULL_VALUE` sentinel and **render as blank** in the table (and are skipped by the charts
and the statistics). So a short column simply trails off:

```c
ctp_add_column(ds, "a", (CTP_PARAM[]){1, 2, 3},     3);   // 3 values
ctp_add_column(ds, "b", (CTP_PARAM[]){4, 5, 6, 7},  4);   // 4 values
// row 4 of column "a" stays empty
```

> **Caution:** `CTP_NULL_VALUE` is a float *sentinel* (`4.04e-10`), compared with `==`. Real
> data exactly equal to it would be treated as empty. For ordinary measurements this never
> happens, but it's why the value is a deliberately odd magnitude.

## When to use which

`ctp_add_row` is convenient for incremental/streaming input. If you already have whole
columns up front, [`ctp_add_column`](2_basic-usage.md) is simpler. Loading a file? See the
[CSV showcase](../README.md#load-a-csv).

## Next

→ [Adding columns & choosing axes](4_Adding-Column.md).
