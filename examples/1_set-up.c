#include <stdio.h>
#include "../src/CTerminalPlotLib.c"

int main()
{
    if (ctp_isActive())
        printf("CTerminalPlotLib setup successful!\n");

    return 0;
}