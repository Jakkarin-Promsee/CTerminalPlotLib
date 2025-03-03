#** CTerminalPlotLib - Adding Rows **

##**Contents * *
        -[Overview](#overview) -
    [Adding Additional Rows](#adding - additional - rows) - [Complete Example](#complete - example)

                                                                ##**Overview **

                                                            This guide demonstrates how to add new rows to an existing dataset in CTerminalPlotLib.This is useful when you want to add data points incrementally without recreating the entire dataset.

                                                            ##**Adding Additional Rows **

```c void ctp_add_row(DataSet *dataSet, CTP_PARAM data[]);
```

**Parameters:**
- `dataSet`: Pointer to your initialized data set
- `data`: Array of values to add as a new row (must match the number of columns in the dataset)

**Important Notes:**
- The size of the `data` array must exactly match the number of columns in your dataset
- The dataset must be initialized with sufficient capacity for additional rows
- You must add rows before calling `ctp_plot()`

**Example:**
```c
// Add a new row with values for all 3 columns
CTP_PARAM new_row[3] = {4, 4, -4};
ctp_add_row(dataSet, new_row);
```

    ##**Complete Example **

        The following example demonstrates how to add a new row to an existing dataset :

```c
#include <stdio.h>
#include "../src/CTerminalPlotLib.c"

    int
    main()
{
    // 1. Initialize data set with room for growth
    int max_cols_size = 5, max_name_length = 20, max_rows_size = 10;
    DataSet *dataSet = ctp_initialize_dataset(max_cols_size, max_name_length, max_rows_size);

    // 2. Add initial data
    int available_cols = 3, available_rows = 7, max_rows = 10;
    CTP_PARAM data[][10] = {
        {-3, -2, -1, 0, 1, 2, 3}, // Column 0 (y-axis)
        {-3, -2, -1, 0, 1, 2, 3}, // Column 1 (x-axis, series 1)
        {3, 2, 1, 0, -1, -2, -3}  // Column 2 (x-axis, series 2)
    };
    ctp_add_data(dataSet, *data, max_rows, available_cols, available_rows);

    // 3. Add column labels
    int available_name = 3;
    char name[][20] = {
        "y",      // Column 0 (y-axis)
        "y = x",  // Column 1 (x-axis, series 1)
        "y = -x", // Column 2 (x-axis, series 2)
    };
    ctp_add_label(dataSet, *name, max_name_length, available_name);

    // 4. Add a new row of data
    // The number of elements must match the number of columns (3)
    CTP_PARAM new_row[3] = {4, 4, -4};
    ctp_add_row(dataSet, new_row);

    // 5. Create plots with the updated dataset
    ctp_plot(dataSet);

    // 6. Free allocated memory
    ctp_free_dataset(dataSet);

    return 0;
}
```

        **Output : **

                   Table with added row : ![Table Plot](./ images / 3_0.png)

                                              Scatter plot with added point : ![Scatter Plot](./ images / 3_1.png)

                                                                                  ##**Use Cases **

                                                                              Adding rows is particularly useful when : -You are collecting data points over time
                                                                                                                        -
                                                                                                                        You want to incrementally build your dataset
                                                                                                                        -
                                                                                                                        You're simulating real-time data updates

                                                                                                                        ##**Next Steps **

                                                                                                                        After learning how to add rows,
    see[Adding Columns](4_add - col.md) to understand how to expand your dataset horizontally.
