#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <locale.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "./include/CTerminalPlotLib.h"

// ---------------------------------------------------------------------------
// Decorative render constants (read-only, shared by every dataset). Glyphs and
// colors are never mutated, so file-scope const is fine here. The *mutable*
// render config (table width, resolution, point glyphs, ...) is per-dataset:
// see CtpStyle and ctp_default_style().
// ---------------------------------------------------------------------------

// Table border glyphs
static const char *CORNER_TL = "┌";
static const char *CORNER_TR = "┐";
static const char *CORNER_BL = "└";
static const char *CORNER_BR = "┘";
static const char *CORNER_THZ = "┴";
static const char *CORNER_BHZ = "┬";
static const char *CORNER_LVC = "├";
static const char *CORNER_RVC = "┤";
static const char *CORNER_ALL = "┼";
static const char *CORNER_VC = "│";
static const char *CORNER_HZ = "─";

// Spacing knobs for ctp_utils_plot_with_space (front/back padding per cell).
static const int SPACE_FRONT = 0;
static const int SPACE_BACK = 0;

// ANSI colors
static const char *COLOR_RESET = "\033[0m";
static const char *COLOR_RED = "\033[1;31m";
static const char *COLOR_GREEN = "\033[1;32m";
static const char *COLOR_BLUE = "\033[1;34m";
static const char *COLOR_YELLOW = "\033[1;33m";
static const char *COLOR_MAGENTA = "\033[1;35m";

// Monochrome-mode glyphs: when color is off, series are told apart by shape and
// bar sign by fill (instead of red/green). Markers cycle per series.
static const char *MONO_MARKERS[4] = {"●", "■", "▲", "◆"};
static const char *MONO_OVERLAP = "⊕"; // 2+ series share a cell
static const char *MONO_BAR_NEG = "▒"; // negative bar fill

// Factory defaults for the per-dataset mutable style
CtpStyle ctp_default_style(void)
{
    CtpStyle s;
    s.table_width = 10;
    s.back_space = 2;
    s.screen_w = 60;
    s.screen_h = 20;
    s.border_edge = 2;
    s.use_color = (getenv("NO_COLOR") == NULL); // honor the NO_COLOR convention
    s.braille = false;
    strcpy(s.point_single, "●");
    strcpy(s.point_overlapped, "◉");
    return s;
}

// Test Access Library - use to comfirm that otherfile can access these function
bool ctp_isActive()
{
    return true;
}

// Setting Plot Function - per-dataset style setters
void ctp_set_table_reset_default(DataSet *dataset)
{
    CtpStyle d = ctp_default_style();
    dataset->style.table_width = d.table_width;
    dataset->style.back_space = d.back_space;
}
void ctp_set_graph_reset_default(DataSet *dataset)
{
    CtpStyle d = ctp_default_style();
    dataset->style.screen_w = d.screen_w;
    dataset->style.screen_h = d.screen_h;
    dataset->style.border_edge = d.border_edge;
    strcpy(dataset->style.point_single, d.point_single);
    strcpy(dataset->style.point_overlapped, d.point_overlapped);
    dataset->style.braille = d.braille;
}
void ctp_set_reset_default(DataSet *dataset)
{
    ctp_set_graph_reset_default(dataset);
    ctp_set_table_reset_default(dataset);
}
void ctp_set_table_backspace(DataSet *dataset, int new_backspace)
{
    dataset->style.back_space = new_backspace;
}
void ctp_set_table_width(DataSet *dataset, int new_width)
{
    dataset->style.table_width = new_width;
}
void ctp_set_graph_resolution(DataSet *dataset, int screen_w, int screen_h)
{
    dataset->style.screen_w = screen_w;
    dataset->style.screen_h = screen_h;
}
void ctp_set_graph_border(DataSet *dataset, int new_border)
{
    dataset->style.border_edge = new_border;
}
void ctp_set_graph_point_x(DataSet *dataset, char new_point)
{
    dataset->style.point_single[0] = new_point;
    dataset->style.point_single[1] = '\0';
}
void ctp_set_graph_point_overlapped(DataSet *dataset, char new_point)
{
    dataset->style.point_overlapped[0] = new_point;
    dataset->style.point_overlapped[1] = '\0';
}
void ctp_set_color(DataSet *dataset, bool on)
{
    dataset->style.use_color = on;
}
void ctp_set_graph_braille(DataSet *dataset, bool on)
{
    dataset->style.braille = on;
}

// Initial DataSet Function - use to initialize inside variable value
DataSet *ctp_initialize_dataset(int max_param, int max_name_size, int max_param_size)
{
    // Allocate the dataset structure. calloc() zero-fills every pointer so that a
    // failure partway through can be cleaned up safely (free(NULL) is a no-op).
    DataSet *dataset = (DataSet *)calloc(1, sizeof(DataSet));
    if (!dataset)
        return NULL;

    // Setting parameter control values
    dataset->max_name_size = max_name_size;
    dataset->max_param = max_param;
    dataset->max_param_size = max_param_size;

    // Allocate the pointer arrays (zero-filled, so partial cleanup is NULL-safe)
    dataset->label = (char **)calloc(max_param, sizeof(char *));
    dataset->db = (CTP_PARAM **)calloc(max_param, sizeof(CTP_PARAM *));
    dataset->db_search = (CTP_PARAM **)calloc(max_param, sizeof(CTP_PARAM *));
    dataset->db_cal = (CTP_PARAM **)calloc(max_param, sizeof(CTP_PARAM *));
    dataset->chosen_Y_param = (int *)malloc(max_param * sizeof(int));
    if (!dataset->label || !dataset->db || !dataset->db_search ||
        !dataset->db_cal || !dataset->chosen_Y_param)
        goto fail;

    // Allocate each column buffer and initialise its cells
    for (int i = 0; i < max_param; i++)
    {
        dataset->label[i] = (char *)malloc(max_name_size * sizeof(char));
        dataset->db[i] = (CTP_PARAM *)malloc(max_param_size * sizeof(CTP_PARAM));
        dataset->db_search[i] = (CTP_PARAM *)malloc(max_param_size * sizeof(CTP_PARAM));
        dataset->db_cal[i] = (CTP_PARAM *)malloc(max_param_size * sizeof(CTP_PARAM));
        if (!dataset->label[i] || !dataset->db[i] ||
            !dataset->db_search[i] || !dataset->db_cal[i])
            goto fail;

        dataset->label[i][0] = '\0';
        for (int j = 0; j < max_param_size; j++)
        {
            dataset->db[i][j] = CTP_NULL_VALUE;
            dataset->db_search[i][j] = 0;
        }
    }

    dataset->plotProperties = ctp_initialize_plotproperties();
    if (!dataset->plotProperties)
        goto fail;

    // Per-dataset render style starts at the factory defaults
    dataset->style = ctp_default_style();

    // Remaining scalar properties (calloc already zeroed them; set for clarity)
    dataset->db_cols_size = 0;
    dataset->db_cols_size_label = 0;
    dataset->db_rows_size = 0;
    dataset->db_search_size = 0;
    dataset->chosen_X_param = 0;
    dataset->chosen_Y_param_size = 0;
    dataset->show_begin = 0;
    dataset->show_end = 0;

    return dataset;

fail:
    ctp_free_dataset(dataset);
    return NULL;
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
    prop->line_plot = false; // off by default: ctp_plot draws table + scatter unless asked

    return prop;
}
void ctp_free_dataset(DataSet *dataset)
{
    if (dataset == NULL)
        return;

    // Each pointer is checked because the dataset may be only partially
    // allocated (ctp_initialize_dataset calls this to clean up a failed init).
    if (dataset->label)
    {
        for (int i = 0; i < dataset->max_param; i++)
            free(dataset->label[i]);
        free(dataset->label);
    }
    if (dataset->db)
    {
        for (int i = 0; i < dataset->max_param; i++)
            free(dataset->db[i]);
        free(dataset->db);
    }
    if (dataset->db_search)
    {
        for (int i = 0; i < dataset->max_param; i++)
            free(dataset->db_search[i]);
        free(dataset->db_search);
    }
    if (dataset->db_cal)
    {
        for (int i = 0; i < dataset->max_param; i++)
            free(dataset->db_cal[i]);
        free(dataset->db_cal);
    }

    free(dataset->chosen_Y_param);
    free(dataset->plotProperties);
    free(dataset);
}

// Manage DataSet Function - use to manage value of inside variable
void ctp_add_row(DataSet *dataSet, CTP_PARAM data[])
{
    if (dataSet->db_rows_size + 1 > dataSet->max_param_size)
    {
        fprintf(stderr, "ctp_add_row: dataset is full (max_param_size reached)\n");
        return;
    }

    for (int i = 0; i < dataSet->db_cols_size; i++)
    {
        dataSet->db[i][dataSet->db_rows_size] = (CTP_PARAM)(data[i]);
    }
    dataSet->db_rows_size++;
}
void ctp_add_data(DataSet *dataset, CTP_PARAM *data, int max_row, int available_col, int available_row)
{
    // Check if the input parameters are valid
    if (dataset == NULL || data == NULL || available_col <= 0 || max_row <= 0)
    {
        fprintf(stderr, "ctp_add_data: invalid parameters\n");
        return;
    }

    // Ensure we do not exceed the current dataset sizes
    if (dataset->max_param_size < available_row || dataset->max_param < available_col)
    {
        fprintf(stderr, "Not enough space in the dataset to add more data\n");
        return;
    }

    // Copy the data from db to the dataset
    for (int i = 0; i < available_col; i++)
    {
        for (int j = 0; j < available_row; j++)
        {
            dataset->db[dataset->db_cols_size + i][j] = (CTP_PARAM)(*((data + j) + i * max_row));
        }
    }

    // Update the row and column size of the dataset
    dataset->db_cols_size += available_col;
    if (dataset->db_rows_size < available_row)
        dataset->db_rows_size = available_row;
}
void ctp_add_label(DataSet *dataset, char *label, int max_name_length, int available_label)
{
    // Check if the input parameters are valid
    if (dataset == NULL || label == NULL || max_name_length <= 0 || available_label <= 0)
    {
        fprintf(stderr, "ctp_add_label: invalid parameters\n");
        return;
    }
    for (int i = 0; i < available_label; i++)
    {
        strcpy(dataset->label[dataset->db_cols_size_label + i], (label + i * max_name_length));
    }

    dataset->db_cols_size_label += available_label;
}
void ctp_add_column(DataSet *dataset, const char *name, const CTP_PARAM *values, int count)
{
    if (dataset == NULL || values == NULL || count <= 0)
    {
        fprintf(stderr, "ctp_add_column: invalid parameters\n");
        return;
    }
    if (dataset->db_cols_size >= dataset->max_param)
    {
        fprintf(stderr, "ctp_add_column: no column capacity left (max_param = %d)\n", dataset->max_param);
        return;
    }
    if (count > dataset->max_param_size)
    {
        fprintf(stderr, "ctp_add_column: count %d exceeds max_param_size %d\n", count, dataset->max_param_size);
        return;
    }

    int col = dataset->db_cols_size;

    // Copy the values; any trailing cells keep the CTP_NULL_VALUE set at init,
    // so columns of different lengths render as blank in the tail.
    for (int j = 0; j < count; j++)
        dataset->db[col][j] = values[j];

    if (name != NULL)
    {
        strncpy(dataset->label[col], name, dataset->max_name_size - 1);
        dataset->label[col][dataset->max_name_size - 1] = '\0';
    }

    dataset->db_cols_size++;
    dataset->db_cols_size_label++;
    if (dataset->db_rows_size < count)
        dataset->db_rows_size = count;
}
void ctp_select_axes(DataSet *dataset, int x_col, const int *y_cols, int y_count)
{
    if (dataset == NULL || y_cols == NULL || y_count <= 0)
    {
        fprintf(stderr, "ctp_select_axes: invalid parameters\n");
        return;
    }
    if (y_count > dataset->max_param)
    {
        fprintf(stderr, "ctp_select_axes: y_count %d exceeds max_param %d\n", y_count, dataset->max_param);
        return;
    }

    dataset->chosen_X_param = x_col;
    for (int i = 0; i < y_count; i++)
        dataset->chosen_Y_param[i] = y_cols[i];
    dataset->chosen_Y_param_size = y_count;

    // Turn on the customized view and default the row window to "all rows"
    // (otherwise show_end stays 0 and nothing is drawn).
    dataset->plotProperties->customize_display = true;
    dataset->show_begin = 0;
    dataset->show_end = dataset->db_rows_size;
}
void ctp_reset_axes(DataSet *dataset)
{
    if (dataset == NULL)
        return;
    dataset->plotProperties->customize_display = false;
}
int ctp_get_dataset_memory_usage(const DataSet *dataSet)
{
    int mem = 0;

    // Calculate memory for label (2D character array)
    mem += dataSet->max_param * dataSet->max_name_size * sizeof(char);

    // Calculate memory for db and db_sort (2D CTP_PARAM arrays)
    mem += 3 * (dataSet->max_param * dataSet->max_param_size * sizeof(CTP_PARAM));

    // Calculate customize PlotProperties
    mem += sizeof(PlotProperties);

    // Calculate memory for chosen_Y_param (integer array of series indices)
    mem += dataSet->max_param * sizeof(int);

    // Calculate memory for individual integers
    mem += 13 * sizeof(int);

    return mem;
}

// Print DataSet Function - use to show insid variable quickly
void ctp_printf_memory_usage(const DataSet *dataSet)
{
    printf("max_param: %d, max_name_size: %d, max_param_size: %d\n", dataSet->max_param, dataSet->max_name_size, dataSet->max_param_size);

    printf("current db_cols_size: %d, current db_rows_size: %d\n\n", dataSet->db_cols_size, dataSet->db_rows_size);

    printf("DataSet uses ");
    if (sizeof(CTP_PARAM) == sizeof(char))
        printf("type char ");
    else if (sizeof(CTP_PARAM) == sizeof(int))
        printf("type int/float ");
    else if (sizeof(CTP_PARAM) == sizeof(double))
        printf("type double ");
    else if (sizeof(CTP_PARAM) == sizeof(long double))
        printf("type long double ");
    else
        printf("an unknown type ");
    printf("to keep data.\n");
    printf("This DataSet memory usage is %d Bytes.\n\n", ctp_get_dataset_memory_usage(dataSet));
}
void ctp_printf_properties(const DataSet *dataSet)
{
    ctp_printf_memory_usage(dataSet);

    printf("max_param: %d\nmax_name_size: %d\nmax_param_size: %d\n", dataSet->max_param, dataSet->max_name_size, dataSet->max_param_size);

    printf("db_cols_size: %d\ndb_rows_size: %d\n", dataSet->db_cols_size, dataSet->db_rows_size);

    printf("chosen_X_param: %d\nchosen_Y_param_size: %d\nchosen_Y_param: ", dataSet->chosen_X_param, dataSet->chosen_Y_param_size);
    for (int i = 0; i < dataSet->chosen_Y_param_size; i++)
    {
        printf("%d, ", dataSet->chosen_Y_param[i]);
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

// Utils Function - use to calculate table and graph
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
        CTP_PARAM temp = db[i][a];
        db[i][a] = db[i][b];
        db[i][b] = temp;
    }
}
int ctp_utils_partition(CTP_PARAM **db, int key_col, int col, int low, int high)
{
    CTP_PARAM pivot = db[key_col][high]; // Choose the last element as pivot
    int i = (low - 1);                   // Index of smaller element

    for (int j = low; j < high; j++)
    {
        // If current element is smaller than or equal to pivot
        if (db[key_col][j] < pivot)
        {
            i++; // Increment index of smaller element
            ctp_utils_swap(db, col, i, j);
        }
    }
    ctp_utils_swap(db, col, i + 1, high);
    return (i + 1); // Return the partitioning index
}
void ctp_utils_quicksort(CTP_PARAM **db, int key_col, int col, int low, int high)
{
    if (low < high)
    {
        // Partition the array
        int pi = ctp_utils_partition(db, key_col, col, low, high);

        // Recursively sort elements before and after partition
        ctp_utils_quicksort(db, key_col, col, low, pi - 1);
        ctp_utils_quicksort(db, key_col, col, pi + 1, high);
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

// Main Function - (user call) use to handle table and scatter plot
static bool print_plot_total = false;

// Prepare the terminal for UTF-8 + ANSI output. On Windows this switches the
// console output code page to UTF-8; modern Linux/macOS terminals already use
// UTF-8, so there only the locale is set.
static void ctp_platform_init(void)
{
    setlocale(LC_ALL, "");
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

void ctp_plot(DataSet *dataSet)
{
    ctp_platform_init();

    if (dataSet->plotProperties->table_plot)
        ctp_plot_table(dataSet);
    if (dataSet->plotProperties->scatter_plot)
        ctp_plot_scatter(dataSet);
    if (dataSet->plotProperties->line_plot)
        ctp_plot_line(dataSet);
}
void ctp_plot_search(DataSet *dataSet)
{
    CTP_PARAM **temp_db = dataSet->db;
    int temp_rows_size = dataSet->db_rows_size;

    dataSet->db = dataSet->db_search;
    dataSet->db_rows_size = dataSet->db_search_size;

    // ctp_printf_properties(dataSet);
    ctp_plot(dataSet);

    dataSet->db = temp_db;
    dataSet->db_rows_size = temp_rows_size;
}
void ctp_plot_analyze(DataSet *dataSet, CTP_PARAM *st)
{
    CTP_PARAM **_ = (CTP_PARAM **)malloc(dataSet->db_cols_size * sizeof(CTP_PARAM *));

    for (int i = 0; i < dataSet->db_cols_size; i++)
    {
        _[i] = malloc(sizeof(CTP_PARAM));
        _[i][0] = st[i];
    }

    int row_temp = dataSet->db_rows_size;
    dataSet->db_rows_size = 1;

    ctp_plot_table_customize(dataSet, _);

    dataSet->db_rows_size = row_temp;

    for (int i = 0; i < dataSet->db_cols_size; i++)
        free(_[i]);
    free(_);
}

void ctp_plot_table(const DataSet *dataSet)
{
    ctp_plot_table_customize(dataSet, dataSet->db);
}
void ctp_plot_table_search(DataSet *dataSet)
{
    ctp_platform_init();
    bool temp_customize = dataSet->plotProperties->customize_display;
    int temp_rows_size = dataSet->db_rows_size;

    dataSet->plotProperties->customize_display = false;
    dataSet->db_rows_size = dataSet->db_search_size;

    printf("Search Found: %d data\n", dataSet->db_search_size);
    ctp_plot_table_customize(dataSet, dataSet->db_search);

    dataSet->plotProperties->customize_display = temp_customize;
    dataSet->db_rows_size = temp_rows_size;
}
void ctp_plot_table_customize(const DataSet *dataSet, CTP_PARAM **db)
{
    ctp_platform_init();

    const int TABLE_WIDTH = dataSet->style.table_width;
    const int BACK_SPACE = dataSet->style.back_space;

    // Print header
    if (print_plot_total)
        printf("%d Plots Total\n", (dataSet->plotProperties->customize_display) ? (dataSet->show_end - dataSet->show_begin) : dataSet->db_rows_size);

    // In the customized view the table shows the X column first, then each Y
    // series — so the column count is (chosen_Y_param_size + 1).
    // Top Line
    printf("%s", CORNER_TL);
    for (int i = 0; i < ((dataSet->plotProperties->customize_display) ? (dataSet->chosen_Y_param_size + 1) : dataSet->db_cols_size); i++)
    {
        for (int j = 0; j < TABLE_WIDTH; j++)
        {
            printf("%s", CORNER_HZ);
        }
        if (i != ((dataSet->plotProperties->customize_display) ? dataSet->chosen_Y_param_size : (dataSet->db_cols_size - 1)))
            printf("%s", CORNER_BHZ);
    }
    printf("%s", CORNER_TR);
    printf("\n");

    // Print Lable name (X column first, then the Y series)
    printf("%s", CORNER_VC);
    printf("%*s", TABLE_WIDTH - BACK_SPACE, ((dataSet->plotProperties->customize_display) ? dataSet->label[dataSet->chosen_X_param] : dataSet->label[0]));
    for (int k = 0; k < BACK_SPACE; k++)
        printf(" ");
    for (int i = ((dataSet->plotProperties->customize_display) ? 0 : 1); i < ((dataSet->plotProperties->customize_display) ? dataSet->chosen_Y_param_size : dataSet->db_cols_size_label); i++)
    {
        printf("%s", CORNER_VC);
        printf("%*s", TABLE_WIDTH - BACK_SPACE, ((dataSet->plotProperties->customize_display) ? dataSet->label[dataSet->chosen_Y_param[i]] : dataSet->label[i]));
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
        for (int i = 0; i < ((dataSet->plotProperties->customize_display) ? (dataSet->chosen_Y_param_size + 1) : dataSet->db_cols_size); i++)
        {
            for (int j = 0; j < TABLE_WIDTH; j++)
            {
                printf("%s", CORNER_HZ);
            }
            if (i != ((dataSet->plotProperties->customize_display) ? dataSet->chosen_Y_param_size : (dataSet->db_cols_size - 1)))
                printf("%s", CORNER_ALL);
        }
        printf("%s", CORNER_RVC);
        printf("\n");

        // Data
        printf("%s", CORNER_VC);
        printf("%*.2lf", TABLE_WIDTH - BACK_SPACE, (double)db[((dataSet->plotProperties->customize_display) ? dataSet->chosen_X_param : 0)][i]); // Print X value
        for (int k = 0; k < BACK_SPACE; k++)
            printf(" ");
        for (int j = ((dataSet->plotProperties->customize_display) ? 0 : 1); j < ((dataSet->plotProperties->customize_display) ? dataSet->chosen_Y_param_size : dataSet->db_cols_size); j++)
        {
            printf("%s", CORNER_VC);
            if (db[((dataSet->plotProperties->customize_display) ? dataSet->chosen_Y_param[j] : j)][i] != CTP_NULL_VALUE)
                printf("%*.2lf", TABLE_WIDTH - BACK_SPACE, (double)db[((dataSet->plotProperties->customize_display) ? dataSet->chosen_Y_param[j] : j)][i]); // Print Y values
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
    for (int i = 0; i < ((dataSet->plotProperties->customize_display) ? (dataSet->chosen_Y_param_size + 1) : dataSet->db_cols_size); i++)
    {
        for (int j = 0; j < TABLE_WIDTH; j++)
        {
            printf("%s", CORNER_HZ);
        }
        if (i != ((dataSet->plotProperties->customize_display) ? dataSet->chosen_Y_param_size : (dataSet->db_cols_size - 1)))
            printf("%s", CORNER_THZ);
    }
    printf("%s", CORNER_BR);
    printf("\n");
}

// ctp_plot_scatter is defined further down, alongside the other CtpCanvas-based
// renderers (line/bar/braille) it now shares its rasterization with.
void ctp_plot_scatter_search(DataSet *dataSet)
{
    CTP_PARAM **temp_db = dataSet->db;
    int temp_rows_size = dataSet->db_rows_size;

    dataSet->db = dataSet->db_search;
    dataSet->db_rows_size = dataSet->db_search_size;

    // ctp_printf_properties(dataSet);
    ctp_plot_scatter(dataSet);

    dataSet->db = temp_db;
    dataSet->db_rows_size = temp_rows_size;
}

// ---------------------------------------------------------------------------
// Internal char-canvas — a width x height grid of UTF-8 glyph cells, each with
// an optional ANSI color. Renderers rasterize into it, then flush it to the
// terminal in one pass. Born for the line plot; reused later by the bar and
// braille renderers. File-static: not part of the public API.
// ---------------------------------------------------------------------------
#define CTP_CANVAS_GLYPH_BYTES 4 // up to a 3-byte UTF-8 glyph + NUL

typedef struct
{
    int w, h;
    char *glyph;        // w*h cells, each CTP_CANVAS_GLYPH_BYTES wide
    const char **color; // w*h ANSI color strings (NULL = no color)
} CtpCanvas;

static CtpCanvas *ctp_canvas_new(int w, int h)
{
    CtpCanvas *cv = (CtpCanvas *)malloc(sizeof(CtpCanvas));
    if (!cv)
        return NULL;
    cv->w = w;
    cv->h = h;
    cv->glyph = (char *)malloc((size_t)w * h * CTP_CANVAS_GLYPH_BYTES);
    cv->color = (const char **)calloc((size_t)w * h, sizeof(char *));
    if (!cv->glyph || !cv->color)
    {
        free(cv->glyph);
        free(cv->color);
        free(cv);
        return NULL;
    }
    for (int i = 0; i < w * h; i++)
        strcpy(&cv->glyph[i * CTP_CANVAS_GLYPH_BYTES], " ");
    return cv;
}

static void ctp_canvas_free(CtpCanvas *cv)
{
    if (!cv)
        return;
    free(cv->glyph);
    free(cv->color);
    free(cv);
}

static void ctp_canvas_set(CtpCanvas *cv, int x, int y, const char *g, const char *color)
{
    if (x < 0 || x >= cv->w || y < 0 || y >= cv->h)
        return;
    int idx = y * cv->w + x;
    strncpy(&cv->glyph[idx * CTP_CANVAS_GLYPH_BYTES], g, CTP_CANVAS_GLYPH_BYTES - 1);
    cv->glyph[idx * CTP_CANVAS_GLYPH_BYTES + CTP_CANVAS_GLYPH_BYTES - 1] = '\0';
    cv->color[idx] = color;
}

// Bresenham segment. The glyph is chosen once from the segment's slope so a run
// of cells reads as one stroke (─ │ ╱ ╲); vertices are marked by the caller.
static void ctp_canvas_line(CtpCanvas *cv, int x0, int y0, int x1, int y1, const char *color)
{
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    const char *g;
    if (dx > 2 * dy)
        g = "─"; // mostly horizontal
    else if (dy > 2 * dx)
        g = "│"; // mostly vertical
    else
        g = ((x1 > x0) == (y1 < y0)) ? "╱" : "╲"; // ~diagonal

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    int x = x0, y = y0;
    while (1)
    {
        ctp_canvas_set(cv, x, y, g, color);
        if (x == x1 && y == y1)
            break;
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
    }
}

// Map a data value onto [0, n-1] cell indices, clamped.
static int ctp_canvas_map(double v, double mn, double range, int n)
{
    int c = (int)lround(((v - mn) / range) * (n - 1));
    if (c < 0)
        c = 0;
    if (c >= n)
        c = n - 1;
    return c;
}

// Drop an ASCII string into a row buffer at pos (used for the x-axis ticks).
static void ctp_canvas_place(char *buf, int w, int pos, const char *s)
{
    for (int i = 0; s[i] != '\0'; i++)
        if (pos + i >= 0 && pos + i < w)
            buf[pos + i] = s[i];
}

// Flush a finished canvas to the terminal: a boxed frame with a Y-axis scale
// down the left edge (labels every 5th row + the bottom). The caller prints its
// own X-axis labels afterward (numeric ticks for line, categories for bars).
static void ctp_canvas_flush(const CtpCanvas *cv, int lab_w, double ymin, double ymax)
{
    const int W = cv->w, H = cv->h;

    printf("%*s%s", lab_w, "", CORNER_TL);
    for (int x = 0; x < W; x++)
        printf("%s", CORNER_HZ);
    printf("%s\n", CORNER_TR);

    for (int cy = 0; cy < H; cy++)
    {
        char lab[24];
        if (cy % 5 == 0 || cy == H - 1)
        {
            double yval = ymax - ((double)cy / (H - 1)) * (ymax - ymin);
            snprintf(lab, sizeof lab, "%.2f", yval);
        }
        else
            lab[0] = '\0';
        printf("%*s%s", lab_w, lab, CORNER_VC);

        for (int cx = 0; cx < W; cx++)
        {
            int idx = cy * W + cx;
            const char *g = &cv->glyph[idx * CTP_CANVAS_GLYPH_BYTES];
            const char *col = cv->color[idx];
            if (col)
                printf("%s%s%s", col, g, COLOR_RESET);
            else
                printf("%s", g);
        }
        printf("%s\n", CORNER_VC);
    }

    printf("%*s%s", lab_w, "", CORNER_BL);
    for (int x = 0; x < W; x++)
        printf("%s", CORNER_HZ);
    printf("%s\n", CORNER_BR);
}

// Print the numeric X-axis tick row (left / middle / right) under a flushed
// canvas. Shared by the line, scatter, and histogram renderers.
static void ctp_canvas_print_xaxis(int W, int lab_w, double xmin, double xmax)
{
    char *xrow = (char *)malloc(W + 1);
    if (!xrow)
        return;
    for (int i = 0; i < W; i++)
        xrow[i] = ' ';
    xrow[W] = '\0';
    char tick[24];
    snprintf(tick, sizeof tick, "%.2f", xmin);
    ctp_canvas_place(xrow, W, 0, tick);
    snprintf(tick, sizeof tick, "%.2f", (xmin + xmax) / 2);
    ctp_canvas_place(xrow, W, W / 2 - (int)strlen(tick) / 2, tick);
    snprintf(tick, sizeof tick, "%.2f", xmax);
    ctp_canvas_place(xrow, W, W - (int)strlen(tick), tick);
    printf("%*s%s\n", lab_w + 1, "", xrow);
    free(xrow);
}

// Draw the zero reference axes into the canvas as a background layer: a vertical
// line at x = 0 and a horizontal line at y = 0, but only where 0 falls within the
// data range, crossing at ┼. Call before rasterizing data so markers/strokes draw
// on top of the axes.
static void ctp_canvas_draw_axes(CtpCanvas *cv, double xmin, double xmax, double ymin, double ymax)
{
    const int W = cv->w, H = cv->h;
    double xr = xmax - xmin;
    if (xr == 0)
        xr = 1;
    double yr = ymax - ymin;
    if (yr == 0)
        yr = 1;

    int x0 = (xmin <= 0 && xmax >= 0) ? ctp_canvas_map(0.0, xmin, xr, W) : -1;
    int y0 = (ymin <= 0 && ymax >= 0) ? (H - 1) - ctp_canvas_map(0.0, ymin, yr, H) : -1;

    if (y0 >= 0)
        for (int cx = 0; cx < W; cx++)
            ctp_canvas_set(cv, cx, y0, "─", NULL);
    if (x0 >= 0)
        for (int cy = 0; cy < H; cy++)
            ctp_canvas_set(cv, x0, cy, "│", NULL);
    if (x0 >= 0 && y0 >= 0)
        ctp_canvas_set(cv, x0, y0, "┼", NULL);
}

// --- High-resolution Braille rendering --------------------------------------
// A Braille cell packs a 2x4 dot grid, so a W x H character canvas addresses a
// 2W x 4H pixel grid — 8x the resolution. Each lit dot maps to a bit of the
// code point (0x2800 + mask).
static const unsigned char BRAILLE_BIT[4][2] = {
    {0x01, 0x08},
    {0x02, 0x10},
    {0x04, 0x20},
    {0x40, 0x80},
};

// Encode a Basic-Multilingual-Plane code point (3-byte range) as UTF-8.
static void ctp_utf8_encode(unsigned int cp, char *out)
{
    out[0] = (char)(0xE0 | (cp >> 12));
    out[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
    out[2] = (char)(0x80 | (cp & 0x3F));
    out[3] = '\0';
}

// Light a pixel in the 2W x 4H dot grid and remember its series color per cell.
static void ctp_braille_set(unsigned char *mask, const char **cellcolor, int w, int h,
                            int px, int py, const char *color)
{
    if (px < 0 || py < 0 || px >= 2 * w || py >= 4 * h)
        return;
    int idx = (py / 4) * w + (px / 2);
    mask[idx] |= BRAILLE_BIT[py % 4][px % 2];
    if (color)
        cellcolor[idx] = color;
}

// Bresenham in pixel space.
static void ctp_braille_line(unsigned char *mask, const char **cellcolor, int w, int h,
                             int x0, int y0, int x1, int y1, const char *color)
{
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1, sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy, x = x0, y = y0;
    while (1)
    {
        ctp_braille_set(mask, cellcolor, w, h, x, y, color);
        if (x == x1 && y == y1)
            break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 < dx)  { err += dx; y += sy; }
    }
}

// Rasterize each Y series against the shared X column as high-res braille (8x).
static void ctp_rasterize_braille(CtpCanvas *cv, const DataSet *ds, int x_col, const int *ycols, int ny,
                                  int row0, int nrows,
                                  double xmin, double xr, double ymin, double yr, bool use_color)
{
    const int W = cv->w, H = cv->h;
    unsigned char *mask = (unsigned char *)calloc((size_t)W * H, 1);
    const char **cellcolor = (const char **)calloc((size_t)W * H, sizeof(char *));
    if (!mask || !cellcolor)
    {
        free(mask);
        free(cellcolor);
        return;
    }

    const char *palette[4] = {COLOR_RED, COLOR_BLUE, COLOR_YELLOW, COLOR_MAGENTA};
    for (int k = 0; k < ny; k++)
    {
        const char *color = use_color ? palette[k % 4] : NULL;
        bool have_prev = false;
        int ppx = 0, ppy = 0;
        for (int i = 0; i < nrows; i++)
        {
            CTP_PARAM xv = ds->db[x_col][row0 + i];
            CTP_PARAM yv = ds->db[ycols[k]][row0 + i];
            if (xv == CTP_NULL_VALUE || yv == CTP_NULL_VALUE) { have_prev = false; continue; }
            int px = ctp_canvas_map(xv, xmin, xr, 2 * W);
            int py = (4 * H - 1) - ctp_canvas_map(yv, ymin, yr, 4 * H);
            if (have_prev)
                ctp_braille_line(mask, cellcolor, W, H, ppx, ppy, px, py, color);
            else
                ctp_braille_set(mask, cellcolor, W, H, px, py, color);
            have_prev = true;
            ppx = px;
            ppy = py;
        }
    }

    char glyph[CTP_CANVAS_GLYPH_BYTES];
    for (int c = 0; c < W * H; c++)
    {
        if (mask[c] == 0)
            continue;
        ctp_utf8_encode(0x2800u + mask[c], glyph);
        ctp_canvas_set(cv, c % W, c / W, glyph, cellcolor[c]);
    }
    free(mask);
    free(cellcolor);
}

// Rasterize each Y series against the shared X column as ordinary char-cell
// strokes + markers.
static void ctp_rasterize_lines(CtpCanvas *cv, const DataSet *ds, int x_col, const int *ycols, int ny,
                                int row0, int nrows,
                                double xmin, double xr, double ymin, double yr,
                                bool use_color, const char *point)
{
    const int W = cv->w, H = cv->h;
    const char *palette[4] = {COLOR_RED, COLOR_BLUE, COLOR_YELLOW, COLOR_MAGENTA};
    for (int k = 0; k < ny; k++)
    {
        // Color mode: one hue per series, shared point glyph. Mono: no color,
        // per-series marker shape so the lines are still distinguishable.
        const char *color = use_color ? palette[k % 4] : NULL;
        const char *marker = use_color ? point : MONO_MARKERS[k % 4];
        bool have_prev = false;
        int pcx = 0, pcy = 0;
        for (int i = 0; i < nrows; i++)
        {
            CTP_PARAM xv = ds->db[x_col][row0 + i];
            CTP_PARAM yv = ds->db[ycols[k]][row0 + i];
            if (xv == CTP_NULL_VALUE || yv == CTP_NULL_VALUE) { have_prev = false; continue; }
            int cx = ctp_canvas_map(xv, xmin, xr, W);
            int cy = (H - 1) - ctp_canvas_map(yv, ymin, yr, H);
            if (have_prev)
                ctp_canvas_line(cv, pcx, pcy, cx, cy, color);
            ctp_canvas_set(cv, cx, cy, marker, color);
            have_prev = true;
            pcx = cx;
            pcy = cy;
        }
    }
}

// Rasterize each Y series against the shared X column as standalone markers (no
// connecting strokes) — the scatter plot. When markers from two different series
// land in the same cell the cell shows the overlap glyph instead.
static void ctp_rasterize_points(CtpCanvas *cv, const DataSet *ds, int x_col, const int *ycols, int ny,
                                 int row0, int nrows,
                                 double xmin, double xr, double ymin, double yr,
                                 bool use_color, const char *point, const char *point_overlap)
{
    const int W = cv->w, H = cv->h;
    const char *palette[4] = {COLOR_RED, COLOR_BLUE, COLOR_YELLOW, COLOR_MAGENTA};

    // Track which series owns each cell so distinct-series collisions become an
    // overlap marker: -1 = empty, -2 = already an overlap, >= 0 = series index.
    int *owner = (int *)malloc((size_t)W * H * sizeof(int));
    if (!owner)
        return;
    for (int i = 0; i < W * H; i++)
        owner[i] = -1;

    for (int k = 0; k < ny; k++)
    {
        const char *color = use_color ? palette[k % 4] : NULL;
        const char *marker = use_color ? point : MONO_MARKERS[k % 4];
        for (int i = 0; i < nrows; i++)
        {
            CTP_PARAM xv = ds->db[x_col][row0 + i];
            CTP_PARAM yv = ds->db[ycols[k]][row0 + i];
            if (xv == CTP_NULL_VALUE || yv == CTP_NULL_VALUE)
                continue;
            int cx = ctp_canvas_map(xv, xmin, xr, W);
            int cy = (H - 1) - ctp_canvas_map(yv, ymin, yr, H);
            int idx = cy * W + cx;
            if (owner[idx] == -1)
            {
                ctp_canvas_set(cv, cx, cy, marker, color);
                owner[idx] = k;
            }
            else if (owner[idx] != k && owner[idx] != -2)
            {
                // A different series already drew here -> mark the overlap.
                if (use_color)
                    ctp_canvas_set(cv, cx, cy, point_overlap, COLOR_GREEN);
                else
                    ctp_canvas_set(cv, cx, cy, MONO_OVERLAP, NULL);
                owner[idx] = -2;
            }
            // owner[idx] == k (same series) or -2 (overlap): leave it as is.
        }
    }
    free(owner);
}

// X column + Y series + row window + data-space bounds shared by the line and
// scatter renderers (both plot the same X-vs-series geometry on a CtpCanvas).
typedef struct
{
    int x_col;                     // the shared, independent X column
    int *ycols;                    // the Y series columns (malloc'd; caller frees)
    int ny;                        // number of Y series
    int row0;                      // first row of the active window
    int nrows;                     // rows in the active window
    double xmin, xmax, ymin, ymax; // data-space bounds
} CtpXY;

// Fill `out` from the dataset's axis selection, or the default convention
// (column 0 = X, columns 1.. = Y series). Allocates out->ycols. Returns false
// (after cleanup) and prints a message tagged `who` when there is nothing to plot.
static bool ctp_xy_prepare(const DataSet *ds, const char *who, CtpXY *out)
{
    const bool custom = ds->plotProperties->customize_display;
    out->x_col = custom ? ds->chosen_X_param : 0;
    out->row0 = custom ? ds->show_begin : 0;
    int row1 = custom ? ds->show_end : ds->db_rows_size;
    out->nrows = row1 - out->row0;

    if (custom)
    {
        out->ny = ds->chosen_Y_param_size;
        out->ycols = (int *)malloc((out->ny > 0 ? out->ny : 1) * sizeof(int));
        if (!out->ycols)
            return false;
        for (int k = 0; k < out->ny; k++)
            out->ycols[k] = ds->chosen_Y_param[k];
    }
    else
    {
        out->ny = ds->db_cols_size - 1; // columns 1.. are the Y series
        out->ycols = (int *)malloc((out->ny > 0 ? out->ny : 1) * sizeof(int));
        if (!out->ycols)
            return false;
        for (int k = 0; k < out->ny; k++)
            out->ycols[k] = k + 1;
    }

    if (out->nrows <= 0 || out->ny <= 0)
    {
        fprintf(stderr, "%s: need at least one row and one Y series\n", who);
        free(out->ycols);
        return false;
    }

    // Data-space bounds: X over the shared column, Y over every series (skip empties).
    bool xinit = false, yinit = false;
    out->xmin = out->xmax = out->ymin = out->ymax = 0;
    for (int i = 0; i < out->nrows; i++)
    {
        CTP_PARAM xv = ds->db[out->x_col][out->row0 + i];
        if (xv == CTP_NULL_VALUE)
            continue;
        if (!xinit) { out->xmin = out->xmax = xv; xinit = true; }
        else if (xv < out->xmin) out->xmin = xv;
        else if (xv > out->xmax) out->xmax = xv;
    }
    for (int k = 0; k < out->ny; k++)
        for (int i = 0; i < out->nrows; i++)
        {
            CTP_PARAM yv = ds->db[out->ycols[k]][out->row0 + i];
            if (yv == CTP_NULL_VALUE)
                continue;
            if (!yinit) { out->ymin = out->ymax = yv; yinit = true; }
            else if (yv < out->ymin) out->ymin = yv;
            else if (yv > out->ymax) out->ymax = yv;
        }
    if (!xinit || !yinit)
    {
        fprintf(stderr, "%s: no plottable data\n", who);
        free(out->ycols);
        return false;
    }
    return true;
}

// Line plot: connect each Y series' points (in row order) against the shared X
// column with straight strokes, or high-res braille when style.braille is on.
// Column 0 is X by default; ctp_select_axes overrides which column is X.
void ctp_plot_line(DataSet *dataSet)
{
    ctp_platform_init();

    const int W = dataSet->style.screen_w;
    const int H = dataSet->style.screen_h;
    const char *POINT = dataSet->style.point_single;
    const bool use_color = dataSet->style.use_color;
    if (W < 2 || H < 2)
    {
        fprintf(stderr, "ctp_plot_line: need a >=2x2 canvas\n");
        return;
    }

    CtpXY xy;
    if (!ctp_xy_prepare(dataSet, "ctp_plot_line", &xy))
        return;

    // Connect points in the data's row order (matplotlib-style): the line follows
    // the sequence the rows were given in.
    double xr = xy.xmax - xy.xmin;
    if (xr == 0)
        xr = 1;
    double yr = xy.ymax - xy.ymin;
    if (yr == 0)
        yr = 1;

    CtpCanvas *cv = ctp_canvas_new(W, H);
    if (!cv)
    {
        fprintf(stderr, "ctp_plot_line: out of memory\n");
        free(xy.ycols);
        return;
    }
    if (dataSet->style.braille)
        ctp_rasterize_braille(cv, dataSet, xy.x_col, xy.ycols, xy.ny, xy.row0, xy.nrows, xy.xmin, xr, xy.ymin, yr, use_color);
    else
        ctp_rasterize_lines(cv, dataSet, xy.x_col, xy.ycols, xy.ny, xy.row0, xy.nrows, xy.xmin, xr, xy.ymin, yr, use_color, POINT);

    const int LAB = 6; // width reserved for the Y-axis scale labels
    if (print_plot_total)
    {
        printf("%d Plots Total\n", xy.nrows);
        printf("Chart Size : %d x %d\n\n", H, W);
    }

    ctp_canvas_flush(cv, LAB, xy.ymin, xy.ymax);
    ctp_canvas_print_xaxis(W, LAB, xy.xmin, xy.xmax);

    ctp_canvas_free(cv);
    free(xy.ycols);
}

// Scatter plot: draw each Y series against the shared X column as standalone
// markers on a CtpCanvas. Column 0 is X by default; ctp_select_axes overrides
// which column is X and which are the Y series.
void ctp_plot_scatter(DataSet *dataSet)
{
    ctp_platform_init();

    const int W = dataSet->style.screen_w;
    const int H = dataSet->style.screen_h;
    const char *POINT = dataSet->style.point_single;
    const char *POINT_OVERLAP = dataSet->style.point_overlapped;
    const bool use_color = dataSet->style.use_color;
    if (W < 2 || H < 2)
    {
        fprintf(stderr, "ctp_plot_scatter: need a >=2x2 canvas\n");
        return;
    }

    CtpXY xy;
    if (!ctp_xy_prepare(dataSet, "ctp_plot_scatter", &xy))
        return;

    double xr = xy.xmax - xy.xmin;
    if (xr == 0)
        xr = 1;
    double yr = xy.ymax - xy.ymin;
    if (yr == 0)
        yr = 1;

    CtpCanvas *cv = ctp_canvas_new(W, H);
    if (!cv)
    {
        fprintf(stderr, "ctp_plot_scatter: out of memory\n");
        free(xy.ycols);
        return;
    }
    ctp_canvas_draw_axes(cv, xy.xmin, xy.xmax, xy.ymin, xy.ymax); // zero crosshair behind the points
    ctp_rasterize_points(cv, dataSet, xy.x_col, xy.ycols, xy.ny, xy.row0, xy.nrows, xy.xmin, xr, xy.ymin, yr, use_color, POINT, POINT_OVERLAP);

    const int LAB = 6;
    if (print_plot_total)
    {
        printf("%d Plots Total\n", xy.nrows);
        printf("Chart Size : %d x %d\n\n", H, W);
    }

    ctp_canvas_flush(cv, LAB, xy.ymin, xy.ymax);
    ctp_canvas_print_xaxis(W, LAB, xy.xmin, xy.xmax);

    ctp_canvas_free(cv);
    free(xy.ycols);
}

// Fill vertical bars into a canvas: one bar per value, scaled to [lo, hi] with a
// zero baseline (positive bars rise green, negative bars drop red). Shared by
// the bar chart and the histogram.
static void ctp_draw_bars(CtpCanvas *cv, const double *vals, int n, double lo, double hi, bool use_color)
{
    const int W = cv->w, H = cv->h;
    double range = hi - lo;
    if (range == 0)
        range = 1;
    int base_cy = (H - 1) - ctp_canvas_map(0.0, lo, range, H); // canvas row of value 0

    int slot = W / n; // columns allotted per bar
    if (slot < 1)
        slot = 1;
    int barw = slot > 2 ? slot - 1 : slot; // leave a 1-col gap when there's room

    for (int b = 0; b < n; b++)
    {
        int top_cy = (H - 1) - ctp_canvas_map(vals[b], lo, range, H);
        bool positive = (vals[b] >= 0);
        // Color mode: sign by hue (green up / red down), solid fill. Mono: sign
        // by fill (█ up / ▒ down), no color.
        const char *color = use_color ? (positive ? COLOR_GREEN : COLOR_RED) : NULL;
        const char *glyph = (use_color || positive) ? "█" : MONO_BAR_NEG;
        int from = top_cy < base_cy ? top_cy : base_cy;
        int to = top_cy < base_cy ? base_cy : top_cy;
        int x0 = b * slot;
        for (int cx = x0; cx < x0 + barw && cx < W; cx++)
            for (int cy = from; cy <= to; cy++)
                ctp_canvas_set(cv, cx, cy, glyph, color);
    }
}

// Bar chart: one vertical bar per row of the value column (the first selected Y
// series if axes are selected, else column 0). Bars share a zero baseline so
// negatives drop below.
void ctp_plot_bar(DataSet *dataSet)
{
    ctp_platform_init();

    const bool custom = dataSet->plotProperties->customize_display;
    const int W = dataSet->style.screen_w;
    const int H = dataSet->style.screen_h;
    const int val_col = custom ? dataSet->chosen_Y_param[0] : 0;
    const int row0 = custom ? dataSet->show_begin : 0;
    const int row1 = custom ? dataSet->show_end : dataSet->db_rows_size;
    const int n = row1 - row0;
    if (n <= 0 || W < 2 || H < 2)
    {
        fprintf(stderr, "ctp_plot_bar: nothing to draw (need rows and a >=2x2 canvas)\n");
        return;
    }

    // Gather bar values (empty cells count as 0) and their range.
    double *vals = (double *)malloc(n * sizeof(double));
    double vmin = 0, vmax = 0;
    bool init = false;
    for (int i = 0; i < n; i++)
    {
        CTP_PARAM v = dataSet->db[val_col][row0 + i];
        vals[i] = (v == CTP_NULL_VALUE) ? 0.0 : (double)v;
        if (!init) { vmin = vmax = vals[i]; init = true; }
        else if (vals[i] < vmin) vmin = vals[i];
        else if (vals[i] > vmax) vmax = vals[i];
    }
    double lo = vmin < 0 ? vmin : 0; // always include the zero baseline
    double hi = vmax > 0 ? vmax : 0;
    if (hi == lo)
        hi = lo + 1;

    CtpCanvas *cv = ctp_canvas_new(W, H);
    if (!cv)
    {
        fprintf(stderr, "ctp_plot_bar: out of memory\n");
        free(vals);
        return;
    }

    ctp_draw_bars(cv, vals, n, lo, hi, dataSet->style.use_color);

    const int LAB = 6;
    printf("Bar: %s (%d bars)\n", dataSet->label[val_col], n);
    ctp_canvas_flush(cv, LAB, lo, hi);

    // X-axis: a row index centered under each bar slot.
    int slot = W / n;
    if (slot < 1)
        slot = 1;
    char *xrow = (char *)malloc(W + 1);
    for (int i = 0; i < W; i++)
        xrow[i] = ' ';
    xrow[W] = '\0';
    for (int b = 0; b < n; b++)
    {
        char idx[16];
        snprintf(idx, sizeof idx, "%d", b);
        int len = (int)strlen(idx);
        ctp_canvas_place(xrow, W, b * slot + (slot - len) / 2, idx);
    }
    printf("%*s%s\n", LAB + 1, "", xrow);

    free(xrow);
    ctp_canvas_free(cv);
    free(vals);
}

// Histogram: bin one column's values into `bins` equal-width buckets and draw
// the per-bin counts as vertical bars. Shares ctp_draw_bars / ctp_canvas_flush
// with the bar chart; only the binning and the X-axis labels differ.
void ctp_plot_histogram(DataSet *dataSet, int bins)
{
    ctp_platform_init();

    const bool custom = dataSet->plotProperties->customize_display;
    const int W = dataSet->style.screen_w;
    const int H = dataSet->style.screen_h;
    const int col = custom ? dataSet->chosen_Y_param[0] : 0;
    const int row0 = custom ? dataSet->show_begin : 0;
    const int row1 = custom ? dataSet->show_end : dataSet->db_rows_size;
    const int n = row1 - row0;
    if (n <= 0 || W < 2 || H < 2 || bins < 1)
    {
        fprintf(stderr, "ctp_plot_histogram: bad args (need rows, >=2x2 canvas, bins>=1)\n");
        return;
    }
    if (bins > W)
        bins = W; // can't show more bars than there are columns

    // Data range over the column.
    double dmin = 0, dmax = 0;
    bool init = false;
    for (int i = 0; i < n; i++)
    {
        CTP_PARAM v = dataSet->db[col][row0 + i];
        if (v == CTP_NULL_VALUE)
            continue;
        if (!init) { dmin = dmax = v; init = true; }
        else if (v < dmin) dmin = v;
        else if (v > dmax) dmax = v;
    }
    if (!init)
    {
        fprintf(stderr, "ctp_plot_histogram: no data\n");
        return;
    }
    double span = dmax - dmin;
    if (span == 0)
        span = 1;
    double binw = span / bins;

    // Tally counts per bin.
    double *counts = (double *)calloc(bins, sizeof(double));
    double maxc = 0;
    for (int i = 0; i < n; i++)
    {
        CTP_PARAM v = dataSet->db[col][row0 + i];
        if (v == CTP_NULL_VALUE)
            continue;
        int b = (int)floor(((double)v - dmin) / binw);
        if (b < 0) b = 0;
        if (b >= bins) b = bins - 1; // dmax falls in the last bin
        counts[b] += 1.0;
        if (counts[b] > maxc)
            maxc = counts[b];
    }
    if (maxc == 0)
        maxc = 1;

    CtpCanvas *cv = ctp_canvas_new(W, H);
    if (!cv)
    {
        fprintf(stderr, "ctp_plot_histogram: out of memory\n");
        free(counts);
        return;
    }

    ctp_draw_bars(cv, counts, bins, 0, maxc, dataSet->style.use_color); // counts >= 0

    const int LAB = 6;
    printf("Histogram: %s (%d bins, range %.2f..%.2f)\n", dataSet->label[col], bins, dmin, dmax);
    ctp_canvas_flush(cv, LAB, 0, maxc);

    // X-axis: the binned data range (left / middle / right edges).
    ctp_canvas_print_xaxis(W, LAB, dmin, dmax);

    ctp_canvas_free(cv);
    free(counts);
}

// Sort Function - use to sort all data (by the X column — the shared axis)
void ctp_sort(DataSet *data)
{
    if (data->plotProperties->customize_display)
        ctp_utils_quicksort(data->db, data->chosen_X_param, data->db_cols_size, data->show_begin, data->show_end - 1);
    else
        ctp_utils_quicksort(data->db, data->chosen_X_param, data->db_cols_size, 0, data->db_rows_size - 1);
}
void ctp_sort_search(DataSet *data)
{
    if (data->plotProperties->customize_display)
        ctp_utils_quicksort(data->db_search, data->chosen_X_param, data->db_cols_size, data->show_begin, data->show_end - 1);
    else
        ctp_utils_quicksort(data->db_search, data->chosen_X_param, data->db_cols_size, 0, data->db_search_size - 1);
}

// Search Function - use to filter all data
static bool isFindOne = false;
static bool isFirstSearch = false;
void ctp_reset_find()
{
    isFirstSearch = false;
}
void ctp_findOne(DataSet *dataSet, int select_col, char *operator, CTP_PARAM search_value)
{
    isFindOne = true;
    ctp_findMany(dataSet, select_col, operator, search_value);
    isFindOne = false;
}
void ctp_findMany(DataSet *dataSet, int select_col, char *operator, CTP_PARAM search_value)
{
    bool const_isFirstSearch = isFirstSearch;
    // Copy db to db_cal
    if (!const_isFirstSearch)
    {
        isFirstSearch = true;
        ctp_utils_update_db_cal(dataSet);
    }
    else
    {
        CTP_PARAM **temp_db = dataSet->db;
        int temp_rows_size = dataSet->db_rows_size;

        dataSet->db = dataSet->db_search;
        dataSet->db_rows_size = dataSet->db_search_size;

        ctp_utils_update_db_cal(dataSet);

        dataSet->db = temp_db;
        dataSet->db_rows_size = temp_rows_size;
    }

    // Resolve the comparison operator once (accepts either symbol or short name)
    enum
    {
        OP_EQ,
        OP_NE,
        OP_LT,
        OP_LTE,
        OP_GT,
        OP_GTE
    } op;
    if (strcmp(operator, "e") == 0 || strcmp(operator, "=") == 0 || strcmp(operator, "==") == 0)
        op = OP_EQ;
    else if (strcmp(operator, "ne") == 0 || strcmp(operator, "!=") == 0)
        op = OP_NE;
    else if (strcmp(operator, "lt") == 0 || strcmp(operator, "<") == 0)
        op = OP_LT;
    else if (strcmp(operator, "lte") == 0 || strcmp(operator, "<=") == 0)
        op = OP_LTE;
    else if (strcmp(operator, "gt") == 0 || strcmp(operator, ">") == 0)
        op = OP_GT;
    else if (strcmp(operator, "gte") == 0 || strcmp(operator, ">=") == 0)
        op = OP_GTE;
    else
    {
        fprintf(stderr, "ctp_findMany: unknown operator \"%s\"\n", operator);
        return;
    }

    int const_temp_search_size = dataSet->db_search_size;
    dataSet->db_search_size = 0;

    for (int i = 0; i < ((!const_isFirstSearch) ? dataSet->db_rows_size : const_temp_search_size); i++)
    {
        CTP_PARAM value = dataSet->db_cal[select_col][i];
        bool match = false;
        switch (op)
        {
        case OP_EQ:  match = (value == search_value); break;
        case OP_NE:  match = (value != search_value); break;
        case OP_LT:  match = (value < search_value);  break;
        case OP_LTE: match = (value <= search_value); break;
        case OP_GT:  match = (value > search_value);  break;
        case OP_GTE: match = (value >= search_value); break;
        }

        if (!match)
            continue;

        for (int j = 0; j < dataSet->db_cols_size; j++)
            dataSet->db_search[j][dataSet->db_search_size] = dataSet->db_cal[j][i];
        dataSet->db_search_size++;

        if (isFindOne)
            return;
    }
}

// Analyze Function - use to analyze all data
CTP_PARAM *ctp_analyze_mean(DataSet *dataSet)
{
    CTP_PARAM *mean = calloc(dataSet->db_cols_size, sizeof(CTP_PARAM));
    int *n = calloc(dataSet->db_cols_size, sizeof(int));

    for (int i = 0; i < dataSet->db_cols_size; i++)
        for (int j = 0; j < dataSet->db_rows_size; j++)
            if (dataSet->db[i][j] != CTP_NULL_VALUE)
            {
                mean[i] += dataSet->db[i][j];
                n[i]++;
            }

    for (int i = 0; i < dataSet->db_cols_size; i++)
        if (n[i] != 0)
            mean[i] /= n[i];

    free(n);
    return mean;
}
CTP_PARAM *ctp_analyze_sd(DataSet *dataSet)
{
    CTP_PARAM *mean = ctp_analyze_mean(dataSet);
    CTP_PARAM *sd = calloc(dataSet->db_cols_size, sizeof(CTP_PARAM));
    int *n = calloc(dataSet->db_cols_size, sizeof(int));

    for (int i = 0; i < dataSet->db_cols_size; i++)
        for (int j = 0; j < dataSet->db_rows_size; j++)
            if (dataSet->db[i][j] != CTP_NULL_VALUE)
            {
                sd[i] += pow(dataSet->db[i][j] - mean[i], 2);
                n[i]++;
            }

    // Population standard deviation: sqrt of the mean squared deviation
    for (int i = 0; i < dataSet->db_cols_size; i++)
        if (n[i] != 0)
            sd[i] = (CTP_PARAM)sqrt(sd[i] / n[i]);

    free(mean);
    free(n);
    return sd;
}
CTP_PARAM *ctp_analyze_md(DataSet *dataSet)
{
    CTP_PARAM *mean = ctp_analyze_mean(dataSet);
    CTP_PARAM *md = calloc(dataSet->db_cols_size, sizeof(CTP_PARAM));
    int *n = calloc(dataSet->db_cols_size, sizeof(int));

    // Mean absolute deviation: average of |x - mean| (fabs, not integer abs)
    for (int i = 0; i < dataSet->db_cols_size; i++)
        for (int j = 0; j < dataSet->db_rows_size; j++)
            if (dataSet->db[i][j] != CTP_NULL_VALUE)
            {
                md[i] += (CTP_PARAM)fabs(dataSet->db[i][j] - mean[i]);
                n[i]++;
            }

    for (int i = 0; i < dataSet->db_cols_size; i++)
        if (n[i] != 0)
            md[i] /= n[i];

    free(mean);
    free(n);
    return md;
}

CTP_PARAM *ctp_analyze_mean_search(DataSet *dataSet)
{
    CTP_PARAM **temp_db = dataSet->db;
    int temp_row = dataSet->db_rows_size;
    dataSet->db = dataSet->db_search;
    dataSet->db_rows_size = dataSet->db_search_size;

    CTP_PARAM *mean = ctp_analyze_mean(dataSet);

    dataSet->db = temp_db;
    dataSet->db_rows_size = temp_row;

    return mean;
}
CTP_PARAM *ctp_analyze_sd_search(DataSet *dataSet)
{
    CTP_PARAM **temp_db = dataSet->db;
    int temp_row = dataSet->db_rows_size;
    dataSet->db = dataSet->db_search;
    dataSet->db_rows_size = dataSet->db_search_size;

    CTP_PARAM *sd = ctp_analyze_sd(dataSet);

    dataSet->db = temp_db;
    dataSet->db_rows_size = temp_row;

    return sd;
}
CTP_PARAM *ctp_analyze_md_search(DataSet *dataSet)
{
    CTP_PARAM **temp_db = dataSet->db;
    int temp_row = dataSet->db_rows_size;
    dataSet->db = dataSet->db_search;
    dataSet->db_rows_size = dataSet->db_search_size;

    CTP_PARAM *md = ctp_analyze_md(dataSet);

    dataSet->db = temp_db;
    dataSet->db_rows_size = temp_row;

    return md;
}

// ---------------------------------------------------------------------------
// Data I/O — read a CSV into a DataSet. First row is the header (column names);
// remaining rows are numeric. Blank / non-numeric cells become CTP_NULL_VALUE
// (rendered as empty). Simple comma split: no quoted-field / embedded-comma
// support. Caller frees the returned DataSet with ctp_free_dataset.
// ---------------------------------------------------------------------------

// Portable strdup (strdup is POSIX, not C11).
static char *ctp_strdup(const char *s)
{
    size_t n = strlen(s) + 1;
    char *p = (char *)malloc(n);
    if (p)
        memcpy(p, s, n);
    return p;
}

// Split a line in place on commas, recording up to `max` field pointers. Empty
// fields are kept (out[i] -> ""). Returns the true field count.
static int ctp_csv_split(char *line, char **out, int max)
{
    int n = 0;
    char *start = line;
    for (char *p = line;; p++)
    {
        if (*p == ',' || *p == '\0')
        {
            bool end = (*p == '\0');
            if (n < max)
                out[n] = start;
            n++;
            *p = '\0';
            start = p + 1;
            if (end)
                break;
        }
    }
    return n;
}

DataSet *ctp_read_csv(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f)
    {
        fprintf(stderr, "ctp_read_csv: cannot open '%s'\n", path);
        return NULL;
    }

    char line[4096];

    // --- header: column count, names, longest name length ---
    if (!fgets(line, sizeof line, f))
    {
        fprintf(stderr, "ctp_read_csv: '%s' is empty\n", path);
        fclose(f);
        return NULL;
    }
    line[strcspn(line, "\r\n")] = '\0';

    int ncols = 1;
    for (char *p = line; *p; p++)
        if (*p == ',')
            ncols++;

    char **fields = (char **)malloc(ncols * sizeof(char *));
    char **names = (char **)malloc(ncols * sizeof(char *));
    ctp_csv_split(line, fields, ncols);
    int max_name = 1;
    for (int c = 0; c < ncols; c++)
    {
        names[c] = ctp_strdup(fields[c]); // copy: the line buffer gets reused
        int len = (int)strlen(names[c]);
        if (len + 1 > max_name)
            max_name = len + 1;
    }

    // --- count data rows (non-blank lines) ---
    int nrows = 0;
    while (fgets(line, sizeof line, f))
    {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] != '\0')
            nrows++;
    }

    DataSet *ds = ctp_initialize_dataset(ncols, max_name, nrows > 0 ? nrows : 1);
    if (!ds)
    {
        for (int c = 0; c < ncols; c++)
            free(names[c]);
        free(names);
        free(fields);
        fclose(f);
        return NULL;
    }

    // Labels (the parse below fills the columns row by row).
    ds->db_cols_size = ncols;
    ds->db_cols_size_label = ncols;
    for (int c = 0; c < ncols; c++)
    {
        strncpy(ds->label[c], names[c], max_name - 1);
        ds->label[c][max_name - 1] = '\0';
    }

    // --- second pass: parse each data row ---
    rewind(f);
    if (!fgets(line, sizeof line, f)) // skip header
    {
        // File shrank between passes? Nothing more to read.
    }
    CTP_PARAM *vals = (CTP_PARAM *)malloc(ncols * sizeof(CTP_PARAM));
    while (fgets(line, sizeof line, f))
    {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
            continue;
        int nf = ctp_csv_split(line, fields, ncols);
        for (int c = 0; c < ncols; c++)
        {
            if (c < nf && fields[c][0] != '\0')
            {
                char *endp;
                double v = strtod(fields[c], &endp);
                vals[c] = (endp == fields[c]) ? CTP_NULL_VALUE : (CTP_PARAM)v;
            }
            else
                vals[c] = CTP_NULL_VALUE; // blank / missing cell
        }
        ctp_add_row(ds, vals);
    }

    free(vals);
    for (int c = 0; c < ncols; c++)
        free(names[c]);
    free(names);
    free(fields);
    fclose(f);
    return ds;
}