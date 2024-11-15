#include <stdio.h>
#include "../include/CTerminalPlotLib.h"
#include "../src/CTerminalPlotLib.c"

int main()
{
    // Initialize the Data Set
    int max_cols_size = 3, max_name_legth = 20, max_rows_size = 10;
    DataSet *dataSet = ctp_initialize_dataset(max_cols_size, max_name_legth, max_rows_size);

    // Add Data
    int available_cols = 3, available_rows = 7, max_rows = 10;
    // data[][max_rows]
    CTP_PARAM data[][10] = {{-3, -2, -1, 0, 1, 2, 3},  // Column 0 (default y-axis)
                            {-3, -2, -1, 0, 1, 2, 10}, // Column 1 (default x-axis)
                            {3, 2, 1, 0, -1, -2, -3}}; // Column 2 (default x-axis)
    ctp_add_data(dataSet, data, max_rows, available_cols, available_rows);

    // Add label
    int available_name = 3, max_name_length = 20;
    // name[][max_name_length]
    char name[][20] = {"index",    // Column 0 (default y-axis)
                       "group 1",  // Column 1 (default x-axis)
                       "group 2"}; // Column 2 (default x-axis)
    ctp_add_label(dataSet, name, max_name_length, available_name);

    // Print DataSet properties
    ctp_plot(dataSet);

    return 0;
}