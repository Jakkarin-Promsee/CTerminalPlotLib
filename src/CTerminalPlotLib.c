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

// Initial DataSet Function
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
        if (!dataset->label[i])
            return NULL;
    }

    // Allocate memory for data arrays
    dataset->db = (CTP_PARAM **)malloc(max_param * sizeof(CTP_PARAM *));
    dataset->db_sort = (CTP_PARAM **)malloc(max_param * sizeof(CTP_PARAM *));
    if (!dataset->db || !dataset->db_sort)
        return NULL;
    for (int i = 0; i < max_param; i++)
    {
        dataset->db[i] = (CTP_PARAM *)malloc(max_param_size * sizeof(CTP_PARAM));
        dataset->db_sort[i] = (CTP_PARAM *)malloc(max_param_size * sizeof(CTP_PARAM));
        if (!dataset->db[i] || !dataset->db_sort[i])
            return NULL;
    }

    // Allocate memory for chosen_X_param array
    dataset->chosen_X_param = (int *)malloc(max_param * sizeof(int));
    if (!dataset->chosen_X_param)
        return NULL;

    // Initialize other properties
    dataset->db_cols_size = 0;
    dataset->db_rows_size = 0;
    dataset->chosen_X_param_size = 0;
    dataset->show_begin = 0;
    dataset->show_end = 0;

    return dataset;
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
        free(dataset->db_sort[i]);
    }
    free(dataset->db);
    free(dataset->db_sort);

    // Free chosen_X_param array
    free(dataset->chosen_X_param);

    // Finally, free the DataSet structure itself
    free(dataset);
}
void ctp_add_data(DataSet *dataset, CTP_PARAM *data, int max_row, int avaliable_col, int avaliable_row)
{
    // Check if the input parameters are valid
    if (dataset == NULL || data == NULL || avaliable_col <= 0 || max_row <= 0)
    {
        fprintf(stderr, "Invalid parameters provided to ctp_addData\n");
        return;
    }

    // Ensure we do not exceed the current dataset sizes
    int new_rows = dataset->db_rows_size + avaliable_row;
    if (new_rows > dataset->max_param_size)
    {
        fprintf(stderr, "Not enough space in the dataset to add more data\n");
        return;
    }

    // Copy the data from db to the dataset
    for (int i = 0; i < avaliable_col; i++)
    {
        for (int j = 0; j < avaliable_row; j++)
        {
            dataset->db[i][j] = (CTP_PARAM)(*((data + j) + i * max_row));
        }
    }

    // Update the row size of the dataset
    dataset->db_cols_size += avaliable_col;
    dataset->db_rows_size += avaliable_row;
}
void ctp_add_label(DataSet *dataset, char *name, int max_name_length, int avaliable_name)
{
    // Check if the input parameters are valid
    if (dataset == NULL || name == NULL || max_name_length <= 0 || avaliable_name <= 0)
    {
        fprintf(stderr, "Invalid parameters provided to ctp_addData\n");
        return;
    }
    for (int i = 0; i < avaliable_name; i++)
    {
        strcpy(dataset->label[i], (name + i * max_name_length));
    }
}
void ctp_printf_dataset(const DataSet *dataSet, CTP_PARAM **db)
{
    printf("rows: %d, cols: %d\n", dataSet->db_rows_size, dataSet->db_cols_size);
    for (int i = 0; i < dataSet->db_cols_size; i++)
        printf("%s\t", dataSet->label[i]);
    printf("\n");

    for (int i = 0; i < dataSet->db_rows_size; i++)
    {
        for (int j = 0; j < dataSet->db_cols_size; j++)
        {
            printf("%.2lf\t", (double)db[j][i]);
        }
        printf("\n");
    }
}
