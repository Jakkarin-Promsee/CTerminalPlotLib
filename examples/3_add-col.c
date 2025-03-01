#include <stdio.h>
#include "../src/CTerminalPlotLib.c"

int main()
{
    // Example basic usage from 1_basic-usages
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

    return 0;
}