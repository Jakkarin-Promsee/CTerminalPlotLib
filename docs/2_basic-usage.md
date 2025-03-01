# Overview

### 1. Initialize the Data Set

```c
DataSet *ctp_initialize_dataset(max_cols_size, max_name_length, max_rows_size);
```

### 2. Add Data for each Column to the Data Set

```c
void ctp_add_data(dataSet, *data, max_rows, available_cols, available_rows);
```

### 3. Add Labels of each Column to the Data Set

```c
void ctp_add_label(dataSet, *name, max_name_length, available_name);
```

### 4. Default Plot Data Set (Both Table and Scatter Plot)

```c
void ctp_plot(dataSet);
```

---

<br>

# **Initialize the Data Set**

```c
DataSet *ctp_initialize_dataset(max_cols_size, max_name_length, max_rows_size);
```

### Parameters:

- `max_cols_size`: The maximum number of parameter columns.
- `max_name_length`: The maximum number of characters for a column name.
- `max_rows_size`: The maximum number of data entries to store per column.

### Usage:

```c
int max_cols_size = 3, max_name_length = 20, max_rows_size = 10;

DataSet *dataSet = ctp_initialize_dataset(max_cols_size, max_name_length, max_rows_size);
```

---

<br>

# **Add Data for each Column to the Data Set**

```c
void ctp_add_data(dataSet, *data, max_rows, available_cols, available_rows);
```

### Parameters:

- `available_cols`: The number of columns to add to the dataset.
- `available_rows`: The number of rows to add to the dataset.
- `max_rows`: The maximum number of rows in the dataset (used to calculate the address of data in the array).

### Usage:

```c
int available_cols = 3, available_rows = 7, max_rows = 10;

// data [][max_rows], Can not declare array size with variable
CTP_PARAM data[][10] = {
    {-3, -2, -1, 0, 1, 2, 3}, // Column 0 (default y-axis)
    {-3, -2, -1, 0, 1, 2, 3}, // Column 1 (default x-axis)
    {3, 2, 1, 0, -1, -2, -3}  // Column 2 (default x-axis)
};

ctp_add_data(dataSet, *data, max_rows, available_cols, available_rows);
```

---

<br>

# **Add Labels of each Column to the Data Set**

```c
void ctp_add_label(dataSet, *name, max_name_length, available_name);
```

### Parameters:

- `available_name`: The number of column labels to add to the dataset.
- `max_name_length`: The maximum length of each label (used to determine the size of the name array).

### Usage:

```c
int available_name = 3, max_name_length = 20;

// name [][max_name_length], Can not declare array size with variable
char name[][20] = {
    "y",      // Column 0 (default y-axis)
    "y = x",  // Column 1 (default x-axis)
    "y = -x", // Column 2 (default x-axis)
};

ctp_add_label(dataSet, *name, max_name_length, available_name);
```

---

<br>

# **Default Plot Data Set (Both Table and Scatter Plot)**

```c
void ctp_plot(dataSet);
```

### Parameters:

- `dataSet`: The data set pointer from `ctp_initialize_dataset` used to store and display data in both table and scatter plot formats.

### Usage:

```c
ctp_plot(dataSet);
```

---

<br>

# **All Sample**

## **.C code** ([go to .c file](../examples/2_basic-usage.c))

```c
#include <stdio.h>
#include "../src/CTerminalPlotLib.c"

int main()
{
    // Initialize data set
    int max_cols_size = 3, max_name_length = 20, max_rows_size = 10;
    DataSet *dataSet = ctp_initialize_dataset(max_cols_size, max_name_length, max_rows_size);

    //---

    // Add data to data set
    int available_cols = 3, available_rows = 7, max_rows = 10;

    // data [][max_rows], Can not declare array size with variable
    CTP_PARAM data[][10] = {
        {-3, -2, -1, 0, 1, 2, 3}, // Column 0 (default y-axis)
        {-3, -2, -1, 0, 1, 2, 3}, // Column 1 (default x-axis)
        {3, 2, 1, 0, -1, -2, -3}  // Column 2 (default x-axis)
    };

    ctp_add_data(dataSet, *data, max_rows, available_cols, available_rows);

    //---

    // Add label to data set
    int available_name = 3;

    // data [][max_name_length], Can not declare array size with variable
    char name[][20] = {
        "y",      // Column 0 (default y-axis)
        "y = x",  // Column 1 (default x-axis)
        "y = -x", // Column 2 (default x-axis)
    };

    ctp_add_label(dataSet, *name, max_name_length, available_name);

    //---

    // Default Plot (both table and scatter)
    ctp_plot(dataSet);

    return 0;
}
```

## Terminal Output

<img src="./images/2_0.png" alt="Table Plot" width="300" />

<br>

<img src="./images/2_1.png" alt="Scatter Plot" width="600" />
