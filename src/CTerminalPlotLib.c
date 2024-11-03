#include "../include/CTerminalPlotLib.h"

// Table border assets initialize
int TABLE_WIDTH = 10;
int BACK_SPACE = 2;

char *CORNER_TL = "┌";
char *CORNER_TR = "┐";
char *CORNER_BL = "└";
char *CORNER_BR = "┘";
char *CORNER_THZ = "┴";
char *CORNER_BHZ = "┬";
char *CORNER_LVC = "├";
char *CORNER_RVC = "┤";
char *CORNER_ALL = "┼";
char *CORNER_VC = "│";
char *CORNER_HZ = "─";

// 2D Graph Plot assets initialize
int SCREEN_H = 20;
int SCREEN_W = 60;
int SPACE_FRONT = 0;
int SPACE_BACK = 0;

char *XY = "┼";
char *Y = "│";
char *X = "─";
char *Y_ORIGIN = "┼";
char *X_ORIGIN = "┼";

char *POINT_SINGLE = "X";
char *POINT_OVERLAPPED = "O";
char *P2 = "O";

char *COLOR_RESET = "\033[0m";
char *COLOR_RED = "\033[1;31m";
char *COLOR_GREEN = "\033[1;32m";
char *COLOR_BLUE = "\033[1;34m";
char *COLOR_YELLOW = "\033[1;33m";
char *COLOR_MAGENTA = "\033[1;35m";

// Initial DataSet Function - use to initialize inside variable value
DataSet *ctp_initialize_dataset(int max_param, int max_name_size, int max_param_size)
{
    // Allocate memory for the dataset structure
    DataSet *dataset = (DataSet *)malloc(sizeof(DataSet));
    if (!dataset)
        return NULL;

    // Setting paramiter control value
    dataset->max_name_size = max_name_size;
    dataset->max_param = max_param;
    dataset->max_param_size = max_param_size;

    // Allocate memory for labels array
    dataset->label = (char **)malloc(max_param * sizeof(char *));
    if (!dataset->label)
        return NULL;
    for (int i = 0; i < max_param; i++)
    {
        dataset->label[i] = (char *)malloc(max_name_size * sizeof(char));
        dataset->label[i][0] = '\0';
        if (!dataset->label[i])
            return NULL;
    }

    // Allocate memory for data arrays
    dataset->db = (CTP_PARAM **)malloc(max_param * sizeof(CTP_PARAM *));
    dataset->db_cal = (CTP_PARAM **)malloc(max_param * sizeof(CTP_PARAM *));
    if (!dataset->db || !dataset->db_cal)
        return NULL;
    for (int i = 0; i < max_param; i++)
    {
        dataset->db[i] = (CTP_PARAM *)malloc(max_param_size * sizeof(CTP_PARAM));
        dataset->db_cal[i] = (CTP_PARAM *)malloc(max_param_size * sizeof(CTP_PARAM));
        if (!dataset->db[i] || !dataset->db_cal[i])
            return NULL;

        for (int j = 0; j < max_param_size; j++)
            dataset->db[i][j] = CTP_NULL_VALUE;
    }

    // Allocate memory for chosen_X_param array
    dataset->chosen_X_param = (int *)malloc(max_param * sizeof(int));
    if (!dataset->chosen_X_param)
        return NULL;

    // Initialize other properties
    dataset->db_cols_size = 0;
    dataset->db_rows_size = 0;
    dataset->chosen_Y_param = 0;
    dataset->chosen_X_param_size = 0;
    dataset->show_begin = 0;
    dataset->show_end = 0;
    dataset->plotProperties = ctp_initialize_plotproperties();

    return dataset;
}
PlotProperties *ctp_initialize_plotproperties()
{
    // Allocate memory for the dataset structure
    PlotProperties *prop = (PlotProperties *)malloc(sizeof(PlotProperties));

    if (!prop)
        return NULL;

    prop->customize_display = false;
    prop->table_plot = true;
    prop->scatter_plot = true;
    prop->line_plot = true;

    return prop;
}
void ctp_free_dataset(DataSet *dataset)
{
    if (dataset == NULL)
        return;

    // Free labels array
    for (int i = 0; i < dataset->max_param; i++)
    {
        free(dataset->label[i]);
    }
    free(dataset->label);

    // Free data arrays
    for (int i = 0; i < dataset->max_param; i++)
    {
        free(dataset->db[i]);
        free(dataset->db_cal[i]);
    }
    free(dataset->db);
    free(dataset->db_cal);

    // Free chosen_X_param array
    free(dataset->chosen_X_param);

    // Finally, free the DataSet structure itself
    free(dataset);
}

// Manage DataSet Function - use to manage value of inside variable
void ctp_add_data(DataSet *dataset, CTP_PARAM *data, int max_row, int avaliable_col, int avaliable_row)
{
    // Check if the input parameters are valid
    if (dataset == NULL || data == NULL || avaliable_col <= 0 || max_row <= 0)
    {
        fprintf(stderr, "Invalid parameters provided to ctp_addData\n");
        return;
    }

    // Ensure we do not exceed the current dataset sizes
    if (dataset->max_param_size < avaliable_row || dataset->max_param < avaliable_col)
    {
        fprintf(stderr, "Not enough space in the dataset to add more data\n");
        return;
    }

    // Copy the data from db to the dataset
    for (int i = 0; i < avaliable_col; i++)
    {
        for (int j = 0; j < avaliable_row; j++)
        {
            dataset->db[dataset->db_cols_size + i][j] = (CTP_PARAM)(*((data + j) + i * max_row));
        }
    }

    // Update the row and column size of the dataset
    dataset->db_cols_size += avaliable_col;
    if (dataset->db_rows_size < avaliable_row)
        dataset->db_rows_size = avaliable_row;
}
void ctp_add_label(DataSet *dataset, char *label, int max_name_length, int avaliable_label)
{
    // Check if the input parameters are valid
    if (dataset == NULL || label == NULL || max_name_length <= 0 || avaliable_label <= 0)
    {
        fprintf(stderr, "Invalid parameters provided to ctp_addData\n");
        return;
    }
    for (int i = 0; i < avaliable_label; i++)
    {
        strcpy(dataset->label[i], (label + i * max_name_length));
    }
}
int ctp_get_dataset_memory_usage(const DataSet *dataSet)
{
    int mem = 0;

    // Calculate memory for label (2D character array)
    mem += dataSet->max_param * dataSet->max_name_size * sizeof(char);

    // Calculate memory for db and db_sort (2D CTP_PARAM arrays)
    mem += 2 * (dataSet->max_param * dataSet->max_param_size * sizeof(CTP_PARAM));

    // Calculate memory for chosen_X_param (integer array)
    mem += dataSet->max_param * sizeof(int);

    // Calculate memory for individual integers
    mem += 10 * sizeof(int);

    return mem;
}

// Print DataSet Function - use to show insid variable quickly
void ctp_printf_properties(const DataSet *dataSet)
{
    printf("DataSet uses ");
    if (sizeof(CTP_PARAM) == sizeof(float))
        printf("type float ");
    else if (sizeof(CTP_PARAM) == sizeof(double))
        printf("type double ");
    else if (sizeof(CTP_PARAM) == sizeof(long double))
        printf("type long double ");
    else
        printf("an unknown type ");
    printf("to keep data\n");
    printf("This DataSet memory usage is %d Bytes\n\n", ctp_get_dataset_memory_usage(dataSet));

    printf("max_param: %d\nmax_name_size: %d\nmax_param_size: %d\n", dataSet->max_param, dataSet->max_name_size, dataSet->max_param_size);

    printf("db_rows_size: %d\ndb_cols_size: %d\n", dataSet->db_rows_size, dataSet->db_cols_size);

    printf("chosen_Y_param: %d\nchosen_X_param_size: %d\nchosen_X_param: ", dataSet->chosen_Y_param, dataSet->chosen_X_param_size);
    for (int i = 0; i < dataSet->chosen_X_param_size; i++)
    {
        printf("%d, ", dataSet->chosen_X_param[i]);
    }
    printf("\n");

    printf("show_begin: %d\nshow_end: %d\n", dataSet->show_begin, dataSet->show_end);
    printf("plotProperties:\n");
    printf(" - table_plot: %s\n", (dataSet->plotProperties->table_plot) ? "true" : "false");
    printf(" - scatter_plot: %s\n", (dataSet->plotProperties->scatter_plot) ? "true" : "false");
    printf(" - line_plot: %s\n", (dataSet->plotProperties->line_plot) ? "true" : "false");
}
void ctp_printf_dataset(const DataSet *dataSet, CTP_PARAM **db)
{
    printf("\nDb value:\n");
    for (int i = 0; i < dataSet->db_cols_size; i++)
        printf("%s\t", dataSet->label[i]);
    printf("\n");

    for (int i = 0; i < dataSet->db_rows_size; i++)
    {
        for (int j = 0; j < dataSet->db_cols_size; j++)
        {
            if (db[j][i] != CTP_NULL_VALUE)
                printf("%.2lf\t", (double)db[j][i]);
            else
                printf("\t");
        }
        printf("\n");
    }
}

// Utils function
void ctp_utils_update_db_cal(DataSet *data)
{
    for (int i = 0; i < data->db_rows_size; i++)
    {
        for (int j = 0; j < data->db_cols_size; j++)
        {
            data->db_cal[j][i] = data->db[j][i];
        }
    }
}
void ctp_utils_swap(CTP_PARAM **db, int col, int a, int b)
{
    for (int i = 0; i < col; i++)
    {
        double temp = db[i][a];
        db[i][a] = db[i][b];
        db[i][b] = temp;
    }
}
int ctp_utils_partition(CTP_PARAM **db, int chosen_Y_param, int col, int low, int high)
{
    double pivot = db[chosen_Y_param][high]; // Choose the last element as pivot
    int i = (low - 1);                       // Index of smaller element

    for (int j = low; j < high; j++)
    {
        // If current element is smaller than or equal to pivot
        if (db[chosen_Y_param][j] < pivot)
        {
            i++; // Increment index of smaller element
            ctp_utils_swap(db, col, i, j);
        }
    }
    ctp_utils_swap(db, col, i + 1, high);
    return (i + 1); // Return the partitioning index
}
void ctp_utils_quicksort(CTP_PARAM **db, int chosen_Y_param, int col, int low, int high)
{
    if (low < high)
    {
        // Partition the array
        int pi = ctp_utils_partition(db, chosen_Y_param, col, low, high);

        // Recursively sort elements before and after partition
        ctp_utils_quicksort(db, chosen_Y_param, col, low, pi - 1);
        ctp_utils_quicksort(db, chosen_Y_param, col, pi + 1, high);
    }
}
void ctp_utils_sort_db(DataSet *data)
{
    ctp_utils_update_db_cal(data);
    ctp_utils_quicksort(data->db_cal, data->chosen_Y_param, data->db_cols_size, 0, data->db_rows_size - 1);
}
void ctp_utils_sort_db_by_y_param(DataSet *data)
{
    ctp_utils_update_db_cal(data);
    ctp_utils_quicksort(data->db_cal, data->chosen_Y_param, data->db_cols_size, data->show_begin, data->show_end - 1);
}
void ctp_utils_normalizes(const DataSet *dataSet, double normalize_min[], double normalize_max[])
{
    // Find min and max of X and Y axis (0 : x, 1 : y)
    double min[2], max[2];

    // Initialize value in min and max
    bool initialDone[2] = {};
    for (int i = 0; i < dataSet->db_cols_size; i++)
    {
        if (initialDone[0] && initialDone[1])
            break;
        if (i == dataSet->chosen_Y_param)
        {
            min[1] = dataSet->db_cal[i][dataSet->show_begin];
            max[1] = dataSet->db_cal[i][dataSet->show_begin];
            initialDone[1] = true;
        }
        else
        {
            min[0] = dataSet->db_cal[i][dataSet->show_begin];
            max[0] = dataSet->db_cal[i][dataSet->show_begin];
            initialDone[2] = true;
        }
    }

    // Find Min and Max by find 1 rows each
    for (int i = 0; i < dataSet->db_cols_size; i++)
    {
        bool isY = (i == ((dataSet->plotProperties->customize_display) ? dataSet->chosen_Y_param : 0)) ? true : false;
        for (int j = ((dataSet->plotProperties->customize_display) ? dataSet->show_begin : 0); j < ((dataSet->plotProperties->customize_display) ? dataSet->show_end : dataSet->db_rows_size); j++)
        {
            if (isY)
            {
                if (min[1] > dataSet->db_cal[i][j])
                    min[1] = dataSet->db_cal[i][j];
                if (max[1] < dataSet->db_cal[i][j])
                    max[1] = dataSet->db_cal[i][j];
            }
            else
            {
                if (min[0] > dataSet->db_cal[i][j])
                    min[0] = dataSet->db_cal[i][j];
                if (max[0] < dataSet->db_cal[i][j])
                    max[0] = dataSet->db_cal[i][j];
            }
        }
    }

    // Calculate normalize
    for (int i = 0; i < dataSet->db_cols_size; i++)
    {
        bool isY = (i == ((dataSet->plotProperties->customize_display) ? dataSet->chosen_Y_param : 0)) ? true : false;
        printf("\ni:%d %s\n", i, (isY) ? "Y" : "X");
        for (int j = ((dataSet->plotProperties->customize_display) ? dataSet->show_begin : 0); j < ((dataSet->plotProperties->customize_display) ? dataSet->show_end : dataSet->db_rows_size); j++)
        {
            if (isY)
                dataSet->db_cal[i][j] = (int)floor(((dataSet->db_cal[i][j]) / (max[1] - min[1])) * SCREEN_H);
            else
                dataSet->db_cal[i][j] = (int)floor(((dataSet->db_cal[i][j]) / (max[0] - min[0])) * SCREEN_W);
        }
    }
    printf("%lf %lf %lf %lf\n", min[0], max[0], min[1], max[1]);
    for (int i = 0; i < 2; i++)
    {
        int multiplier = (i == 0) ? SCREEN_W : SCREEN_H;

        normalize_min[i] = ((min[i]) / (max[i] - min[i])) * multiplier;
        normalize_max[i] = ((max[i]) / (max[i] - min[i])) * multiplier;
    }
}
void ctp_utils_plot_with_space(const char s[], const char space[])
{
    for (int i = 0; i < SPACE_FRONT; i++)
        printf("%*s", 1, space);
    printf("%*s", 1, s);
    for (int i = 0; i < SPACE_BACK; i++)
        printf("%*s", 1, space);
}
void ctp_utils_print_color(const char s[])
{
    printf("%s", s);
}

// Main function
void ctp_plot(const DataSet *dataSet)
{
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "");

    if (dataSet->plotProperties->table_plot)
        ctp_plot_table(dataSet);
    if (dataSet->plotProperties->line_plot)
        ctp_plot_scatter(dataSet);
}
void ctp_plot_table(const DataSet *dataSet)
{
    ctp_plot_table_customize(dataSet, dataSet->db);
}
void ctp_plot_table_customize(const DataSet *dataSet, CTP_PARAM **db)
{
    // Print header
    printf("%d Plots Total\n", (dataSet->plotProperties->customize_display) ? (dataSet->show_end - dataSet->show_begin) : dataSet->db_rows_size);

    // Top Line
    printf("%s", CORNER_TL);
    for (int i = 0; i < ((dataSet->plotProperties->customize_display) ? (dataSet->chosen_X_param_size + 1) : dataSet->db_cols_size); i++)
    {
        for (int j = 0; j < TABLE_WIDTH; j++)
        {
            printf("%s", CORNER_HZ);
        }
        if (i != ((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param_size : (dataSet->db_cols_size - 1)))
            printf("%s", CORNER_BHZ);
    }
    printf("%s", CORNER_TR);
    printf("\n");

    // Print Lable name
    printf("%s", CORNER_VC);
    printf("%*s", TABLE_WIDTH - BACK_SPACE, ((dataSet->plotProperties->customize_display) ? dataSet->label[dataSet->chosen_Y_param] : dataSet->label[0]));
    for (int k = 0; k < BACK_SPACE; k++)
        printf(" ");
    for (int i = ((dataSet->plotProperties->customize_display) ? 0 : 1); i < ((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param_size : dataSet->db_cols_size); i++)
    {
        printf("%s", CORNER_VC);
        printf("%*s", TABLE_WIDTH - BACK_SPACE, ((dataSet->plotProperties->customize_display) ? dataSet->label[dataSet->chosen_X_param[i]] : dataSet->label[i]));
        for (int k = 0; k < BACK_SPACE; k++)
            printf(" ");
    }
    printf("%s", CORNER_VC);
    printf("\n");

    // Print the data
    for (int i = ((dataSet->plotProperties->customize_display) ? dataSet->show_begin : 0); i < ((dataSet->plotProperties->customize_display) ? dataSet->show_end : dataSet->db_rows_size); i++)
    {
        // Pararel Line
        printf("%s", CORNER_LVC);
        for (int i = 0; i < ((dataSet->plotProperties->customize_display) ? (dataSet->chosen_X_param_size + 1) : dataSet->db_cols_size); i++)
        {
            for (int j = 0; j < TABLE_WIDTH; j++)
            {
                printf("%s", CORNER_HZ);
            }
            if (i != ((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param_size : (dataSet->db_cols_size - 1)))
                printf("%s", CORNER_ALL);
        }
        printf("%s", CORNER_RVC);
        printf("\n");

        // Data
        printf("%s", CORNER_VC);
        printf("%*.2f", TABLE_WIDTH - BACK_SPACE, (double)db[((dataSet->plotProperties->customize_display) ? dataSet->chosen_Y_param : 0)][i]); // Print Y values
        for (int k = 0; k < BACK_SPACE; k++)
            printf(" ");
        for (int j = ((dataSet->plotProperties->customize_display) ? 0 : 1); j < ((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param_size : dataSet->db_cols_size); j++)
        {
            printf("%s", CORNER_VC);
            if (db[((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param[j] : j)][i] != CTP_NULL_VALUE)
                printf("%*.2f", TABLE_WIDTH - BACK_SPACE, (double)db[((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param[j] : j)][i]); // Print X values
            else
                printf("%*s", TABLE_WIDTH - BACK_SPACE, "");

            for (int k = 0; k < BACK_SPACE; k++)
                printf(" ");
        }
        printf("%s", CORNER_VC);
        printf("\n");
    }

    // Buttom Line
    printf("%s", CORNER_BL);
    for (int i = 0; i < ((dataSet->plotProperties->customize_display) ? (dataSet->chosen_X_param_size + 1) : dataSet->db_cols_size); i++)
    {
        for (int j = 0; j < TABLE_WIDTH; j++)
        {
            printf("%s", CORNER_HZ);
        }
        if (i != ((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param_size : (dataSet->db_cols_size - 1)))
            printf("%s", CORNER_THZ);
    }
    printf("%s", CORNER_BR);
    printf("\n");
}
void ctp_plot_scatter(DataSet *dataSet)
{
    // Copy db to db_cal
    ctp_utils_update_db_cal(dataSet);

    // Sort db_cal
    if (dataSet->plotProperties->customize_display)
        ctp_utils_sort_db_by_y_param(dataSet);
    else
        ctp_utils_sort_db(dataSet);

    // Print head of graph
    printf("\nInitialize Scatter Plot\n");
    printf("%d Plots Total\n", ((dataSet->plotProperties->customize_display) ? dataSet->show_end - dataSet->show_begin : dataSet->db_rows_size));
    printf("Chart Size : %d x %d\n\n", SCREEN_H, SCREEN_W);

    // Find min and max of X and Y axis (0 : x, 1 : y)
    CTP_PARAM min_normalize[2], max_normalize[2];

    // Normalize scale of X and Y axis by saving on db_cal and recieve min andmax normalize
    ctp_utils_normalizes(dataSet, min_normalize, max_normalize);

    ctp_printf_dataset(dataSet, dataSet->db_cal);
    // Declare iterator to find each point on graph
    int ite = (dataSet->plotProperties->customize_display) ? dataSet->show_end
                                                           : dataSet->db_rows_size;

    // Count y from max to min (high to low)
    for (int y = (int)ceil(max_normalize[1]) + 1; y >= (int)floor(min_normalize[1]) - 1; y--)
    {
        // Check that this y point have x
        int y_stack = 0;
        if ((int)(dataSet->db_cal[((dataSet->plotProperties->customize_display) ? dataSet->chosen_Y_param : 0)][ite - 1]) == y)
        {
            y_stack++;
            ite--;

            while ((int)(dataSet->db_cal[((dataSet->plotProperties->customize_display) ? dataSet->chosen_Y_param : 0)][ite - y_stack]) == y)
            {
                y_stack++;
            }
        }

        char str[5];
        sprintf(str, "%d", y);
        printf("%*s\t", 5, str);

        // Check amout of point in each x value
        for (int x = (int)floor(min_normalize[0]); x < (int)ceil(max_normalize[0]) + 1; x++)
        {
            int overlapped = 0;
            int col_overlapped = 0;
            int col_stack = 1;

            for (int i = ((dataSet->plotProperties->customize_display) ? 0 : 1); i < ((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param_size : dataSet->db_cols_size); i++)
            {
                bool isFistForThisCol = false;
                for (int j = 0; j < y_stack; j++)
                {
                    if ((int)floor(dataSet->db_cal[((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param[i] : i)][ite - j]) == x)
                    {
                        if (!isFistForThisCol)
                            isFistForThisCol = true;
                        else
                            col_stack++;
                        overlapped++;
                        col_overlapped = ((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param[i] : (i - 1));
                    }
                }
            }

            if (overlapped - col_stack >= 0 && y_stack >= 1)
            {
                if (overlapped - col_stack == 0)
                {
                    if (col_overlapped == ((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param[0] : 0))
                        ctp_utils_print_color(COLOR_RED);
                    else if ((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param[1] : 1)
                        ctp_utils_print_color(COLOR_BLUE);
                    else if ((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param[2] : 2)
                        ctp_utils_print_color(COLOR_YELLOW);
                    else if ((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param[3] : 3)
                        ctp_utils_print_color(COLOR_MAGENTA);
                    else
                        ctp_utils_print_color(COLOR_RESET);
                    ctp_utils_plot_with_space(POINT_SINGLE, " ");
                }
                else
                {
                    ctp_utils_print_color(COLOR_GREEN);
                    ctp_utils_plot_with_space(POINT_OVERLAPPED, " ");
                }
                ctp_utils_print_color(COLOR_RESET);
            }
            else
            {
                if (x == 0 && y == 0)
                    ctp_utils_plot_with_space(XY, X);
                else if (x == 0)
                    ctp_utils_plot_with_space(Y, " ");
                else if (y == 0)
                    ctp_utils_plot_with_space(X, X);
                else
                    ctp_utils_plot_with_space(" ", " ");
            }
        }
        printf("\n");
    }

    for (int x = (int)floor(min_normalize[0]); x < (int)ceil(max_normalize[0]); x++)
        printf("%d", x);
    printf("\n");
}