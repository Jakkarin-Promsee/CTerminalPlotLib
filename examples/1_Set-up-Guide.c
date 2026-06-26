#include <stdio.h>
#include "../src/include/CTerminalPlotLib.h"

int main()
{
    if (ctp_isActive())
        printf("CTerminalPlotLib setup successful!\n");
    else
        printf("CTerminalPlotLib setup failed!\n");

    return 0;
}