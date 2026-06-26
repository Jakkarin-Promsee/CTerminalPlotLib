#include <stdio.h>
#include "../src/CTerminalPlotLib.c"

int main()
{
    // 1. Initialize data set
    int max_cols_size = 3, max_name_length = 20, max_rows_size = 10;
    DataSet *dataSet = ctp_initialize_dataset(max_cols_size, max_name_length, max_rows_size);

    // 2. Prepare data
    int available_cols = 3, available_rows = 7, max_rows = 10;
    CTP_PARAM data[][10] = {
        // max_rows = 10
        {-3, -2, -1, 0, 1, 2, 3}, // Column 0 (default y-axis)
        {-3, -2, -1, 0, 1, 2, 3}, // Column 1 (default x-axis)
        {3, 2, 1, 0, -1, -2, -3}  // Column 2 (default x-axis)
    };

    // 3. Add data to data set
    ctp_add_data(dataSet, *data, max_rows, available_cols, available_rows);

    // 4. Prepare labels
    int available_name = 3;
    char name[][20] = {
        // max_name_length = 20
        "y",      // Column 0 (default y-axis)
        "y = x",  // Column 1 (default x-axis)
        "y = -x", // Column 2 (default x-axis)
    };

    // 5. Add labels to data set
    ctp_add_label(dataSet, *name, max_name_length, available_name);

    // 6. Display memory usage (optional)
    ctp_printf_memory_usage(dataSet);

    // 7. Create plots for both table and scatter
    ctp_plot(dataSet);

    // 8. Free allocated memory
    ctp_free_dataset(dataSet);

    return 0;
}