#include <stdio.h>
#include "../include/CTerminalPlotLib.h"
#include "../src/CTerminalPlotLib.c"

int main()
{
    int max_param = 5, max_name_size = 20, max_param_size = 100;
    DataSet *dataSet = ctp_initializeDataSet(max_param, max_name_size, max_param_size);

    if (!dataSet)
    {
        printf("Error occur initialize DataSet\n");
        return 1; // Exit if initialization failed
    }

    printf("%d", dataSet->max_name_size);

    return 0;
}