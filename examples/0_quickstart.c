#include <stdio.h>
#include <math.h>
#include "../src/include/CTerminalPlotLib.h"

#define DEG2RAD(x) ((x) * M_PI / 180.0)

int main(void)
{
    const int N = 25; // -180 to 180, step 10

    DataSet *ds = ctp_initialize_dataset(
        /*max_cols*/ 3,
        /*max_name*/ 32,
        /*max_rows*/ N);

    CTP_PARAM s0[N];
    CTP_PARAM s1[N];
    CTP_PARAM s2[N];
    CTP_PARAM s3[N];

    // Generate data
    int idx = 0;
    for (int deg = -180; deg <= 180; deg += 15)
    {
        double rx = DEG2RAD(deg);
        double r0 = DEG2RAD(2 * deg - 10);
        double r1 = DEG2RAD(2 * deg);

        s0[idx] = deg;
        s1[idx] = cos(r0);
        s2[idx] = 0.4 * rx * cos(r1);

        idx++;
    }

    // Add data
    ctp_add_column(ds, "Angle (deg)", s0, N);
    ctp_add_column(ds, "y = cos(x)", s1, N);
    ctp_add_column(ds, "y = 0.4x * cos(x)", s2, N);

    // Default plot
    ctp_plot(ds);

    // Free data
    ctp_free_dataset(ds);
    return 0;
}