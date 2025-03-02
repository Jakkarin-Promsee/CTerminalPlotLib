<!-- 2_basic-usage.md -->

# **Overview**

- ### 1. Initialize the Data Set

  ```c
  DataSet *ctp_initialize_dataset(max_cols_size, max_name_length, max_rows_size);
  ```

- ### 2. Add Data for each Column to the Data Set

  ```c
  void ctp_add_data(DataSet *dataset, CTP_PARAM *data, int max_row, int avaliable_col, int avaliable_row);
  ```

- ### 3. Add Labels of each Column to the Data Set

  ```c
  void ctp_add_label(DataSet *dataset, char *name, int max_name_length, int avaliable_name);
  ```

- ### 4. Print allocate memory from Data Set

  ```c
  void ctp_printf_memory_usage(const DataSet *dataSet);
  ```

- ### 5. Default Plot Data Set (Both Table and Scatter Plot)

  ```c
  void ctp_plot(DataSet *dataSet);
  ```

- ### 6. Free Allocate memory of Data Set

  ```c
  void ctp_free_dataset(DataSet *dataset);
  ```

---

<br>

# **Initialize the Data Set**

- ### Docs:

  ```c
  DataSet *ctp_initialize_dataset(int max_param, int max_name_size, int max_param_size);
  ```

- ### Parameters:

  - `max_param`: The maximum number of parameter columns.
  - `max_name_size`: The maximum number of characters for a column name.
  - `max_param_size`: The maximum number of data entries to store per column.

- ### Usage:

  ```c
  int max_cols_size = 3, max_name_length = 20, max_rows_size = 10;

  DataSet *dataSet = ctp_initialize_dataset(max_cols_size, max_name_length, max_rows_size);
  ```

---

<br>

# **Add Data for each Column to the Data Set**

- ### Docs:

  ```c
  void ctp_add_data(DataSet *dataset, CTP_PARAM *data, int max_row, int avaliable_col, int avaliable_row);
  ```

- ### Parameters:

  - `dataSet`: The data set pointer from `ctp_initialize_dataset` used to store all address of the Data Set.
  - `data`: The data pointer to add into the data set.
  - `avaliable_col`: The number of columns to add to the dataset.
  - `avaliable_row`: The number of rows to add to the dataset.
  - `max_row`: The maximum number of rows in the dataset (used to calculate the address of data in the array).

- ### Usage:

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

- ### Docs:

  ```c
  void ctp_add_label(DataSet *dataset, char *name, int max_name_length, int avaliable_name);
  ```

- ### Parameters:

  - `dataSet`: The data set pointer from `ctp_initialize_dataset` used to store all address of the Data Set.
  - `name`: The name pointer to add to the data set.
  - `avaliable_name`: The number of column labels to add to the dataset.
  - `max_name_length`: The maximum length of each label (used to determine the size of the name array).

- ### Usage:

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

# **Print allocated memory from Data Set**

- ### Docs:

  ```c
  void ctp_printf_memory_usage(const DataSet *dataSet);
  ```

- ### Parameters:

  - `dataSet`: The data set pointer from `ctp_initialize_dataset` used to store all address of the Data Set.

- ### Usage:

  ```c
  ctp_printf_memory_usage(dataSet);
  ```

---

<br>

# **Default Plot Data Set (Both Table and Scatter Plot)**

- ### Docs:

  ```c
  void ctp_plot(DataSet *dataSet);
  ```

- ### Parameters:

  - `dataSet`: The data set pointer from `ctp_initialize_dataset` used to store all address of the Data Set.

- ### Usage:

  ```c
  ctp_plot(dataSet);
  ```

---

<br>

# **Free Allocated memory of Data Set**

- ### Docs:

  ```c
  void ctp_free_dataset(DataSet *dataset);
  ```

- ### Parameters:

  - `dataSet`: The data set pointer from `ctp_initialize_dataset` used to store all address of the Data Set.

- ### Usage:

  ```c
  ctp_free_dataset(dataSet);
  ```

---

<br>

# **All Sample**

- ## **.C code** ([go to .c file](../examples/2_basic-usage.c))

  ```c
  #include <stdio.h>
  #include "../src/CTerminalPlotLib.c"

  int main()
  {
      // Initialize data set
      int max_cols_size = 5, max_name_length = 20, max_rows_size = 10;
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

      // Free Allocate memory of Data Set
      ctp_free_dataset(dataSet);

      return 0;
  }
  ```

- ## Terminal Output

    <img src="./images/2_0.png" alt="Table Plot" width="300" />

    <br>

    <img src="./images/2_1.png" alt="Scatter Plot" width="600" />
