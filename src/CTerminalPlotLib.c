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
void ctp_printf_poproties(const DataSet *dataSet)
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

// Main function
void ctp_plot(const DataSet *dataSet)
{
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "");

    if (dataSet->plotProperties->table_plot)
        ctp_plot_table(dataSet, dataSet->db);
}

void ctp_plot_table(const DataSet *dataSet, CTP_PARAM **db)
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
