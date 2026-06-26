#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

int main()
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

    // 3. Add initial column labels
    int available_name = 3;
    char name[][20] = {
        "y",      // Column 0 (y-axis)
        "y = x",  // Column 1 (x-axis, series 1)
        "y = -x", // Column 2 (x-axis, series 2)
    };
    ctp_add_label(dataSet, *name, max_name_length, available_name);

    // 4. Add new data columns
    int new_available_cols = 2, new_available_rows = 7, new_max_rows = 10;
    CTP_PARAM new_data[][10] = {
        {-6, -4, -2, 0, 2, 4, 6}, // New column 3
        {6, 4, 2, 0, -2, -4, -6}  // New column 4
    };
    ctp_add_data(dataSet, *new_data, new_max_rows, new_available_cols, new_available_rows);

    // 5. Add labels for the new columns
    int new_available_name = 2;
    char new_name[][20] = {
        "y = 2x",  // Label for new column 3
        "y = -2x"  // Label for new column 4
    };
    ctp_add_label(dataSet, *new_name, max_name_length, new_available_name);

    // 6. Create plots with the expanded dataset
    ctp_plot(dataSet);

    // 7. Free allocated memory
    ctp_free_dataset(dataSet);

    return 0;
}
