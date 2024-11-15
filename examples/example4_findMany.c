#include <stdio.h>
#include "../include/CTerminalPlotLib.h"
#include "../src/CTerminalPlotLib.c"

int main()
{
    // Initialize
    int max_param = 10, max_name_size = 20, max_param_size = 100;
    DataSet *dataSet = ctp_initialize_dataset(max_param, max_name_size, max_param_size);

    // Add data
    int avaliable_col = 3, avaliable_row = 7, max_row = 100;
    CTP_PARAM data[][100] = {{-3, -2, -1, 0, 1, 2, 3},
                             {-3, -2, -1, 0, 1, 2, 10},
                             {3, 2, 1, 0, -1, -2, -3}};
    ctp_add_data(dataSet, data, max_row, avaliable_col, avaliable_row);

    // Add label
    int avaliable_name = 3, max_name_length = 20;
    char name[][20] = {
        "index",
        "group 1",
        "group 2"};
    ctp_add_label(dataSet, name, max_name_length, avaliable_name);

    // Show default data table
    ctp_plot_table(dataSet);

    // Show search data table
    ctp_findMany(dataSet, 2, "lte", 1);
    ctp_plot_table_search(dataSet);

    return 0;
}