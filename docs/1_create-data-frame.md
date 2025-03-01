# **Initialize the Data Frame**

```c
int max_cols_size = 3, max_name_length = 20, max_rows_size = 10;

DataSet *dataSet = ctp_initialize_dataset(max_cols_size, max_name_length, max_rows_size);
```

### Parameters:

- `max_cols_size`: The maximum number of parameter columns.
- `max_name_length`: The maximum number of characters for a column name.
- `max_rows_size`: The maximum number of data entries to store per column.

---

<br>

# **Add Data to the Data Frame**

```c
int available_cols = 3, available_rows = 7, max_rows = 10;

CTP_PARAM data[][max_rows] = {
    {-3, -2, -1, 0, 1, 2, 3},  // Column 0 (default y-axis)
    {-3, -2, -1, 0, 1, 2, 10}, // Column 1 (default x-axis)
    {3, 2, 1, 0, -1, -2, -3}   // Column 2 (default x-axis)
};

ctp_add_data(dataSet, *data, max_rows, available_cols, available_rows);
```

### Parameters:

- `available_cols`: The number of columns to add to the dataset.
- `available_rows`: The number of rows to add to the dataset.
- `max_rows`: The maximum number of rows in the dataset (used to calculate the address of data in the array).

---

<br>

# **Add Labels to the Data Frame**

```c
int available_name = 3, max_name_length = 20;

char name[][max_name_length] = {
    "index",   // Column 0 (default y-axis)
    "group 1", // Column 1 (default x-axis)
    "group 2"  // Column 2 (default x-axis)
};

ctp_add_label(dataSet, *name, max_name_length, available_name);
```

### Parameters:

- `available_name`: The number of column labels to add to the dataset.
- `max_name_length`: The maximum length of each label (used to determine the size of the name array).
