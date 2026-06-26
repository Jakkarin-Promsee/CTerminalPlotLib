#include <stdio.h>
#include <math.h>
#include "../src/include/CTerminalPlotLib.h"

#define DEG2RAD(x) ((x) * M_PI / 180.0)

int main(void)
{
    const int N = 19; // -180 to 180, step 20

    DataSet *ds = ctp_initialize_dataset(
        /*max_cols*/ 4,
        /*max_name*/ 32,
        /*max_rows*/ N);

    CTP_PARAM angle[N];
    CTP_PARAM s1[N];
    CTP_PARAM s2[N];
    CTP_PARAM s3[N];

    int idx = 0;
    for (int deg = -180; deg <= 180; deg += 20)
    {
        angle[idx] = deg;

        double r0 = DEG2RAD(2 * deg);
        double r1 = DEG2RAD(1.3 * deg - 40);
        double r2 = DEG2RAD(2.5 * deg + 60);

        s1[idx] = 1.7 + sin(r0);
        s2[idx] = -0.2 + cos(r1);
        s3[idx] = -2.0 * ((float)deg / 180) + 0.4 * cos(r2);

        idx++;
    }

    ctp_add_column(ds, "Angle (deg)", angle, N);
    ctp_add_column(ds, "1.7 + sin(2x)", s1, N);
    ctp_add_column(ds, "-0.2 + cos(1.3x-40)", s2, N);
    ctp_add_column(ds, "-2x + 0.4cos(2.5x+60)", s3, N);

    ctp_plot(ds);

    ctp_free_dataset(ds);
    return 0;
}