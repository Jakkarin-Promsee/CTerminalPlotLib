# The project story

The story is so simple. My c class want final project, and I don't know what should I do. I want a challenge that not boring. So I pick pointer topic. (Hardest in the class Then). Then I got insane topic... The OOP in c. 🤣🤣💀. First I pick STL to copy, that those are too easy. So I pick py matplotlib that we can adjust everthing for our graph. Plus that I have to build the dynamic array. Plus for more interesting, I use terminal and microcontroller target. That is the story behind this level of hell. 🤣🤣🗿🗿💀💀

# What it do now (At that time)

Tbh, it's too many year ago. I don't sure that my memory will correct or not. Think like I'm yapping. 🤣🤣🤣💀

1. We an intial my library object. Having to set max_param (maximum col), max_name_size (max character for each col name), max_param_size (maximum row). This is the 3 require thing to form first stucture.
2. ctp_add_data: If I don't misunderstand it, this function will extend the table by row. (multi row)
3. ctp_add_row: This is adding by single row
4. ctp_plot: just plot. 🤣🤣

```c
// Initialize the Data Set
int max_cols_size = 4, max_name_legth = 20, max_rows_size = 10;
DataSet *dataSet = ctp_initialize_dataset(max_cols_size, max_name_legth, max_rows_size);

// Add Data
int available_cols = 3, available_rows = 7, max_rows = 10;
// data[][max_rows]
CTP_PARAM data[][10] = {{0, 1, 2, 3, 4, 5, 6},     // Column 0 (default y-axis)
                        {0, 1, 2, 3, 4, 5, 6},     // Column 1 (default x-axis)
                        {0, 1, 4, 9, 16, 25, 36}}; // Column 2 (default x-axis)
ctp_add_data(dataSet, *data, max_rows, available_cols, available_rows);

// Add label
int available_name = 3, max_name_length = 20;
// name[][max_name_length]
char name[][20] = {"y",      // Column 0 (default y-axis)
                    "x=y",    // Column 1 (default x-axis)
                    "x=y^2"}; // Column 2 (default x-axis)
ctp_add_label(dataSet, *name, max_name_length, available_name);

// Add row
CTP_PARAM new_row[3] = {7, 7, 49};
ctp_add_row(dataSet, new_row);

// Print DataSet properties
ctp_plot(dataSet);
```

---

5. I use static variable to customize graph. This it the extend feature that I didn't plan before. At first it just simple static valriable. But now it can do all. 🤣🤣🤣. (Don't know should it be static or not, but if possible change it to some poiter will be great. It's easy to fix by outsider. I already have only some function to do it too. But it's a big migration that wasn't done and the class was ended, then I'm move on to ml 🤣🤣🗿💀)

```c
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
int BORDER_EDGE = 2;
int Y_SCALE_LENGTH = 5;
int X_SCALE_LENGTH = 5;
int Y_SCALE_MOD = 5;
int X_SCALE_MOD = 15;
int SPACE_FRONT = 0;
int SPACE_BACK = 0;

char *XY = "┼";
char *Y = "│";
char *X = "─";
char *Y_ORIGIN = "┼";
char *X_ORIGIN = "┼";

// Writable buffers (not string literals): the point glyph can be changed at runtime
char POINT_SINGLE[] = "X";
char POINT_OVERLAPPED[] = "O";
char *P2 = "O";

char *COLOR_RESET = "\033[0m";
char *COLOR_RED = "\033[1;31m";
char *COLOR_GREEN = "\033[1;32m";
char *COLOR_BLUE = "\033[1;34m";
char *COLOR_YELLOW = "\033[1;33m";
char *COLOR_MAGENTA = "\033[1;35m";

// BACK_SPACE | TABLE_WIDTH | SCREEN_H | SCREEN_W | BORDER_EDGE
// File-private: these back the "reset to default" feature and are not public API.
static bool DEFAULT_SAVE_BOOL[5] = {false, false, false, false, false};
static int *DEFAULT_SAVE_POINTER[5];
static int DEFAULT_SAVE_VALUE[5];
```

---

6. The hidden properties that I hide it cause it's so hell to handle. My body almost break that time. 🤣🤣🤣. This feature is about select X and Y (single Y) to show. We can choose whic axis to display. 🔥🔥. And that how I die too. 🤣🤣🤣💀💀

```c
int chosen_Y_param;             // Y parameter index
int *chosen_X_param;            // Array of X parameter indices
int chosen_X_param_size;        // Number of chosen X parameters
```

# What I want to fix it

1. I want to fix 5. and 6. that can let's other people can use it easily.
2. If possible, I want make my library is more easy to use. Now I stuck on 2D+ array declaration, I have to know exactly how width it is. Making init a bit messy. And about the function name or it ux too. It's great now. But I think it can be better. 🤣🤣🤣
3. I want to extend more feature to make it useful but not heavy much. Don't do it yet, but after all refactor and fix done, I want you to critic it again and give me advice what is the feature should I add or remove
