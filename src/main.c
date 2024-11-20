#include <stdio.h>
#include "../include/CTerminalPlotLib.h"
#include "../src/CTerminalPlotLib.c"

int main()
{
    // Initialize
    int max_param = 3, max_name_size = 10, max_param_size = 10;
    DataSet *dataSet = ctp_initialize_dataset(max_param, max_name_size, max_param_size);

    // Add data
    int avaliable_col = 3, avaliable_row = 7, max_row = 10;
    CTP_PARAM data[][10] = {{-3, -2, -1, 0, 1, 2, 3},
                            {-3, -2, -1, 0, 1, 2, 10},
                            {3, 2, 1, 0, -1, -2, -3}};
    ctp_add_data(dataSet, *data, max_row, avaliable_col, avaliable_row);

    CTP_PARAM new_row[3] = {4, 4, 4};
    ctp_add_row(dataSet, new_row);

    // Add label
    int avaliable_name = 3, max_name_length = 20;
    char name[][20] = {
        "index",
        "group 1",
        "group 2"};
    ctp_add_label(dataSet, *name, max_name_length, avaliable_name);

    // ctp_printf_properties(dataSet);
    // ctp_plot(dataSet);

    // ctp_plot(dataSet);

    // ctp_plot_table(dataSet);

    ctp_plot(dataSet);

    // ctp_findMany(dataSet, 0, "gte", -1);
    // ctp_findMany(dataSet, 0, "lte", 2);
    // ctp_plot_table_search(dataSet);
    // ctp_plot_search(dataSet);

    return 0;
}