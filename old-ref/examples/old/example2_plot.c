#include <stdio.h>
#include "../include/CTerminalPlotLib.h"
#include "../src/CTerminalPlotLib.c"

int main()
{
    // Initialize DataSet variable
    int max_param = 10, max_name_size = 20, max_param_size = 100;
    DataSet *dataSet = ctp_initialize_dataset(max_param, max_name_size, max_param_size);

    // Add data into DataSet variable
    // data index 0 will be Y axis and over 1 will be X axis
    int avaliable_col = 3, avaliable_row = 6, max_row = 100;
    CTP_PARAM data[][100] = {{1, 2, 3, 4, 5, 6}, {11, 22, 33, 44, 55, 66}, {15, 2, 42, 53, 62}};
    ctp_add_data(dataSet, data, max_row, avaliable_col, avaliable_row);

    // Add label into DataSet variable
    int avaliable_name = 3, max_name_length = 20;
    char name[][20] = {
        "index",
        "group 1",
        "group 2"};
    ctp_add_label(dataSet, name, max_name_length, avaliable_name);

    // Defaul plot
    ctp_plot(dataSet);

    return 0;
}