#ifndef CTERMINAL_PLOT_LIB_H
#define CTERMINAL_PLOT_LIB_H

// The public header only needs <stdbool.h> (bool appears in the structs and
// prototypes). Implementation-only headers (stdio, stdlib, string, math,
// windows, locale) are included by CTerminalPlotLib.c, not here.
#include <stdbool.h>

// Default data type to double (8 Bytes) || float (4 Bytes) || char (1 Bytes)
typedef float CTP_PARAM;

// Default null value of data in DataSet (should set relate to CTP_PARAM, Caution this value will not show on terminal)
// Cast to CTP_PARAM so the sentinel round-trips exactly on comparison
// (a bare double literal never compares equal to its truncated float cell).
#define CTP_NULL_VALUE ((CTP_PARAM)4.04e-10)

// CTP DataFrame structure
typedef struct
{
    bool customize_display;
    bool table_plot;
    bool scatter_plot;
    bool line_plot;
} PlotProperties;

// Per-dataset render style — the mutable config that used to be global variables.
typedef struct
{
    int table_width;          // glyph cells per table column
    int back_space;           // right padding inside each table cell
    int screen_w;             // scatter plot width  (X resolution)
    int screen_h;             // scatter plot height (Y resolution)
    int border_edge;          // blank border around the scatter plot
    bool use_color;           // ANSI color on (default) vs monochrome marker mode
    bool braille;             // high-resolution Unicode braille line rendering (8x)
    char point_single[8];     // glyph for a single data point
    char point_overlapped[8]; // glyph for overlapping points
} CtpStyle;

typedef struct
{
    int max_param;                  // Maximum parameters in the dataset
    int max_name_size;              // Maximum size for each label
    int max_param_size;             // Maximum data points per parameter
    char **label;                   // Array of labels
    CTP_PARAM **db;                 // Data array
    CTP_PARAM **db_search;          // Use to keep find condition
    CTP_PARAM **db_cal;             // Use to keep sort data array
    int db_cols_size;               // Number of columns for data
    int db_cols_size_label;         // Number of columns for label
    int db_rows_size;               // Number of rows
    int db_search_size;             // Number of data keep in db_search_keep
    int chosen_X_param;             // X parameter index (single independent axis)
    int *chosen_Y_param;            // Array of Y parameter indices (the series)
    int chosen_Y_param_size;        // Number of chosen Y parameters
    int show_begin;                 // Start display index
    int show_end;                   // End display index
    PlotProperties *plotProperties; // Use to keep ploting poproties
    CtpStyle style;                 // Per-dataset render style (see ctp_default_style)
} DataSet;

// Render glyphs and colors are internal constants in CTerminalPlotLib.c.
// The mutable render config is per-dataset: see CtpStyle and the ctp_set_* setters.

// Test Access Library - use to comfirm that otherfile can access these function
bool ctp_isActive();

// Style: factory defaults + per-dataset setters
CtpStyle ctp_default_style(void);
void ctp_set_table_reset_default(DataSet *dataset);
void ctp_set_graph_reset_default(DataSet *dataset);
void ctp_set_reset_default(DataSet *dataset);
void ctp_set_table_backspace(DataSet *dataset, int new_backspace);
void ctp_set_table_width(DataSet *dataset, int new_width);
void ctp_set_graph_resolution(DataSet *dataset, int screen_w, int screen_h);
void ctp_set_graph_border(DataSet *dataset, int new_border);
void ctp_set_graph_point_x(DataSet *dataset, char new_point);
void ctp_set_graph_point_overlapped(DataSet *dataset, char new_point);
void ctp_set_color(DataSet *dataset, bool on);
void ctp_set_graph_braille(DataSet *dataset, bool on);

// Initial DataSet Function - use to initialize inside variable value
DataSet *ctp_initialize_dataset(int max_param, int max_name_size, int max_param_size);
PlotProperties *ctp_initialize_plotproperties();
void ctp_free_dataset(DataSet *dataset);

// Manage DataSet Function - use to manage value of inside variable
void ctp_add_row(DataSet *dataSet, CTP_PARAM data[]);
void ctp_add_data(DataSet *dataset, CTP_PARAM *data, int max_row, int available_col, int available_row);
void ctp_add_label(DataSet *dataset, char *name, int max_name_length, int available_name);
int ctp_get_dataset_memory_usage(const DataSet *dataSet);

// Ergonomic data entry: add one labelled column from a 1-D array (preferred API).
// No 2-D arrays, no max_row juggling — call once per data series.
void ctp_add_column(DataSet *dataset, const char *name, const CTP_PARAM *values, int count);

// Data I/O: read a CSV into a fresh DataSet (header row -> labels, numeric rows
// -> columns; blank/non-numeric cells -> CTP_NULL_VALUE). Simple comma split, no
// quoted fields. Returns NULL on error; caller frees with ctp_free_dataset.
DataSet *ctp_read_csv(const char *path);

// Axis selection: pick which column is the X axis (the shared, independent axis,
// drawn horizontally) and which columns are the Y series (drawn vertically).
// Enables the customized view and shows every row by default.
void ctp_select_axes(DataSet *dataset, int x_col, const int *y_cols, int y_count);
void ctp_reset_axes(DataSet *dataset);

// Print DataSet Function - use to show insid variable quickly
void ctp_printf_memory_usage(const DataSet *dataSet);
void ctp_printf_properties(const DataSet *dataSet);
void ctp_printf_dataset(const DataSet *dataSet, CTP_PARAM **db);

// Utils Function - use to calculate table and graph
void ctp_utils_update_db_cal(DataSet *data);
void ctp_utils_swap(CTP_PARAM **db, int col, int a, int b);
int ctp_utils_partition(CTP_PARAM **db, int key_col, int col, int low, int high);
void ctp_utils_quicksort(CTP_PARAM **db, int key_col, int col, int low, int high);
void ctp_utils_plot_with_space(const char s[], const char space[]);
void ctp_utils_print_color(const char s[]);

// Main Function - (user call) use to handle table and scatter plot
void ctp_plot(DataSet *dataSet);
void ctp_plot_search(DataSet *dataSet);
void ctp_plot_analyze(DataSet *dataSet, CTP_PARAM *st);
void ctp_plot_table(const DataSet *dataSet);
void ctp_plot_table_search(DataSet *dataSet);
void ctp_plot_table_customize(const DataSet *dataSet, CTP_PARAM **db);
void ctp_plot_scatter(DataSet *dataSet);
void ctp_plot_scatter_search(DataSet *dataSet);
void ctp_plot_line(DataSet *dataSet);
void ctp_plot_bar(DataSet *dataSet);
void ctp_plot_histogram(DataSet *dataSet, int bins);

// Sort Function - use to sort all data
void ctp_sort(DataSet *data);
void ctp_sort_search(DataSet *data);

// Search Function - use to filter all data
void ctp_reset_find();
void ctp_findOne(DataSet *dataSet, int select_col, char *operator, CTP_PARAM search_value);
void ctp_findMany(DataSet *dataSet, int select_col, char *operator, CTP_PARAM search_value);

// Analyze Function - use to analyze all data
CTP_PARAM *ctp_analyze_mean(DataSet *dataSet);
CTP_PARAM *ctp_analyze_sd(DataSet *dataSet);
CTP_PARAM *ctp_analyze_md(DataSet *dataSet);

// Analyze the current search result set (db_search) instead of the full data
CTP_PARAM *ctp_analyze_mean_search(DataSet *dataSet);
CTP_PARAM *ctp_analyze_sd_search(DataSet *dataSet);
CTP_PARAM *ctp_analyze_md_search(DataSet *dataSet);

#endif
