<!-- 4_add-col.md -->

# **Overview**

**It can add more column to Data Set with same function before**

- ### 0. Used Sample Data Set from part before ([2_basic_usage.md](2_basic-usage.md))

- ### 1. Add more Column Data

  ```c
  void ctp_add_data(dataSet, *data, max_rows, available_cols, available_rows);
  ```

- ### 2. Add more Column Label

  ```c
  void ctp_add_label(dataSet, *name, max_name_length, available_name);
  ```

---

<br>

# Add more Column Data

- ### Docs:

  ```c
  void ctp_add_data(dataSet, *data, max_rows, available_cols, available_rows);
  ```

- ### Parameters:

  - `dataSet`: The data set pointer from `ctp_initialize_dataset` used to store all address of Data Set
  - `data`: The data pointer to add into the data set.
  - `max_cols_size`: The maximum number of parameter columns.
  - `max_name_length`: The maximum number of characters for a column name.
  - `max_rows_size`: The maximum number of data entries to store per column.

- ### Usage:

  ```c
  // Add new data column to data set
  int new_available_cols = 2, new_available_rows = 7, new_max_rows = 10;
  CTP_PARAM new_data[][10] = {
      {-6, -4, -2, 0, 2, 4, 6}, // Column 4 (default x-axis)
      {6, 4, 2, 0, -2, -4, -6}  // Column 5 (default x-axis)
  };
  ctp_add_data(dataSet, *new_data, new_max_rows, new_available_cols, new_available_rows);
  ```

---

<br>

# Add more Column Label

- ### Docs:

  ```c
  void ctp_add_label(dataSet, *name, max_name_length, available_name);
  ```

- ### Parameters:

  - `dataSet`: The data set pointer from `ctp_initialize_dataset` used to store all address of Data Set
  - `name`: The name pointer to add to the data set.
  - `available_cols`: The number of columns to add to the dataset.
  - `available_rows`: The number of rows to add to the dataset.
  - `max_rows`: The maximum number of rows in the dataset (used to calculate the address of data in the array).

- ### Usage:

  ```c
  // Add new label column to data set
  int new_available_name = 2;
  char new_name[][20] = {
      "y = 0.5x", // Column 4 (default x-axis)
      "y =-0.5x"  // Column 5 (default x-axis)
  };
  ctp_add_label(dataSet, *new_name, max_name_length, new_available_name);
  ```

---

<br>

# All Sample

- ## **.C code** ([go to .c file](../examples/4_add-col.c))

  ```c
  #include <stdio.h>
  #include "../src/CTerminalPlotLib.c"

  int main()
  {
      // Example basic usage from 2_basic-usages
      // Initialize data set
      int max_cols_size = 5, max_name_length = 20, max_rows_size = 10;
      DataSet *dataSet = ctp_initialize_dataset(max_cols_size, max_name_length, max_rows_size);

      // Add data to data set
      int available_cols = 3, available_rows = 7, max_rows = 10;
      CTP_PARAM data[][10] = {
          {-3, -2, -1, 0, 1, 2, 3}, // Column 0 (default y-axis)
          {-3, -2, -1, 0, 1, 2, 3}, // Column 1 (default x-axis)
          {3, 2, 1, 0, -1, -2, -3}  // Column 2 (default x-axis)
      };
      ctp_add_data(dataSet, *data, max_rows, available_cols, available_rows);

      // Add label to data set
      int available_name = 3;
      char name[][20] = {
          "y",      // Column 0 (default y-axis)
          "y = x",  // Column 1 (default x-axis)
          "y = -x", // Column 2 (default x-axis)
      };
      ctp_add_label(dataSet, *name, max_name_length, available_name);

      //--------------------------------------------------------------------
      // Add new data column to data set
      int new_available_cols = 2, new_available_rows = 7, new_max_rows = 10;
      CTP_PARAM new_data[][10] = {
          {-6, -4, -2, 0, 2, 4, 6}, // Column 4 (default x-axis)
          {6, 4, 2, 0, -2, -4, -6}  // Column 5 (default x-axis)
      };
      ctp_add_data(dataSet, *new_data, new_max_rows, new_available_cols, new_available_rows);

      // Add new label column to data set
      int new_available_name = 2;
      char new_name[][20] = {
          "y = 0.5x", // Column 4 (default x-axis)
          "y =-0.5x"  // Column 5 (default x-axis)
      };
      ctp_add_label(dataSet, *new_name, max_name_length, new_available_name);

      // Default Plot (both table and scatter)
      ctp_plot(dataSet);

      // Free Allocate memory of Data Set
      ctp_free_dataset(dataSet);

      return 0;
  }
  ```

- ## Terminal Output

    <img src="./images/4_0.png" alt="Table Plot" width="500" />

    <br>

    <img src="./images/4_1.png" alt="Scatter Plot" width="600" />
