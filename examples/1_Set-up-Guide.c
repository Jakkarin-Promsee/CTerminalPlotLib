#include <stdio.h>
#include "../src/CTerminalPlotLib.c"

int main()
{
    if (ctp_isActive())
        printf("CTerminalPlotLib setup successful!\n");
    else
        printf("CTerminalPlotLib setup failed!\n");

    return 0;
}