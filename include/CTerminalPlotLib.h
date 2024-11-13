#ifndef CTERMINAL_PLOT_LIB_H
#define CTERMINAL_PLOT_LIB_H

#include "CTerminalPlotUtils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <locale.h>

// Default data type to double (8 Bytes) || float (4 Bytes) || char (1 Bytes)
typedef double CTP_PARAM;

// Default null value of data in DataSet (should set relate to CTP_PARAM, Caution this value will not show on terminal)
#define CTP_NULL_VALUE 4.04e-10

// CTP DataFrame structure
typedef struct
{
    bool customize_display;
    bool table_plot;
    bool scatter_plot;
    bool line_plot;
} PlotProperties;

typedef struct
{
    int max_param;                  // Maximum parameters in the dataset
    int max_name_size;              // Maximum size for each label
    int max_param_size;             // Maximum data points per parameter
    char **label;                   // Array of labels
    CTP_PARAM **db;                 // Data array
    CTP_PARAM **db_search;          // Use to keep find condition
    CTP_PARAM **db_cal;             // Use to keep sort data array
    int db_cols_size;               // Number of columns
    int db_rows_size;               // Number of rows
    int db_search_size;             // Number of data keep in db_search_keep
    int chosen_Y_param;             // Y parameter index
    int *chosen_X_param;            // Array of X parameter indices
    int chosen_X_param_size;        // Number of chosen X parameters
    int show_begin;                 // Start display index
    int show_end;                   // End display index
    PlotProperties *plotProperties; // Use to keep ploting poproties
} DataSet;

// Table border assets
extern int TABLE_WIDTH;
extern int BACK_SPACE;

extern char *CORNER_TL;
extern char *CORNER_TR;
extern char *CORNER_BL;
extern char *CORNER_BR;
extern char *CORNER_THZ;
extern char *CORNER_BHZ;
extern char *CORNER_LVC;
extern char *CORNER_RVC;
extern char *CORNER_ALL;
extern char *CORNER_VC;
extern char *CORNER_HZ;

// 2D Graph Plot assets
extern int SCREEN_H;
extern int SCREEN_W;
extern int BORDER_EDGE;
extern int Y_SCALE_LENGTH;
extern int X_SCALE_LENGTH;
extern int Y_SCALE_MOD;
extern int X_SCALE_MOD;
extern int SPACE_FRONT;
extern int SPACE_BACK;

extern char *XY;
extern char *Y;
extern char *X;
extern char *Y_ORIGIN;
extern char *X_ORIGIN;

extern char *POINT_SINGLE;
extern char *POINT_OVERLAPPED;
extern char *P2;

extern char *COLOR_RESET;
extern char *COLOR_RED;
extern char *COLOR_GREEN;
extern char *COLOR_BLUE;
extern char *COLOR_YELLOW;
extern char *COLOR_MAGENTA;

// Initial DataSet Function - use to initialize inside variable value
DataSet *ctp_initialize_dataset(int max_param, int max_name_size, int max_param_size);
PlotProperties *ctp_initialize_plotproperties();
void ctp_free_dataset(DataSet *dataset);

// Manage DataSet Function - use to manage value of inside variable
void ctp_add_data(DataSet *dataset, CTP_PARAM *data, int max_row, int avaliable_col, int avaliable_row);
void ctp_add_label(DataSet *dataset, char *name, int max_name_length, int avaliable_name);
int ctp_get_dataset_memory_usage(const DataSet *dataSet);

// Print DataSet Function - use to show insid variable quickly
void ctp_printf_dataset(const DataSet *dataSet, CTP_PARAM **db);
void ctp_printf_properties(const DataSet *dataSet);

// Utils Function
void ctp_utils_update_db_cal(DataSet *data);
void ctp_utils_swap(CTP_PARAM **db, int col, int a, int b);
int ctp_utils_partition(CTP_PARAM **db, int chosen_Y_param, int col, int low, int high);
void ctp_utils_quickSort(CTP_PARAM **db, int chosen_Y_param, int col, int low, int high);
void ctp_utils_sort_db(DataSet *data);
void ctp_utils_sort_db_by_y_param(DataSet *data);
void ctp_utils_normalizes(const DataSet *dataSet, double normalize_min[], double normalize_max[], double min[], double max[]);
void ctp_utils_plot_with_space(const char s[], const char space[]);
void ctp_utils_print_color(const char s[]);

// Main Function
void ctp_plot(const DataSet *dataSet);
void ctp_plot_table(const DataSet *dataSet);
void ctp_plot_table_customize(const DataSet *dataSet, CTP_PARAM **db);
void ctp_plot_scatter(DataSet *dataSet);

#endif
