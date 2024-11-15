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
    ctp_add_data(dataSet, data, max_row, avaliable_col, avaliable_row);
    // ctp_add_data(dataSet, data, max_row, avaliable_col, avaliable_row);

    // Add label
    int avaliable_name = 3, max_name_length = 20;
    char name[][20] = {
        "index",
        "group 1",
        "group 2"};
    ctp_add_label(dataSet, name, max_name_length, avaliable_name);

    // ctp_printf_properties(dataSet);
    // ctp_plot(dataSet);

    dataSet->plotProperties->customize_display = true;
    dataSet->chosen_Y_param = 1;
    dataSet->chosen_X_param_size = 2;
    dataSet->chosen_X_param[0] = 0;
    dataSet->chosen_X_param[1] = 2;
    dataSet->show_begin = 0;
    dataSet->show_end = 7;

    // ctp_plot(dataSet);

    // ctp_plot_table(dataSet);

    ctp_plot(dataSet);

    // ctp_findMany(dataSet, 0, "gte", -1);
    // ctp_findMany(dataSet, 0, "lte", 2);
    // ctp_plot_table_search(dataSet);
    // ctp_plot_search(dataSet);

    return 0;
}