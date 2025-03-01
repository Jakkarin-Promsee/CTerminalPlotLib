# **CTerminalPlotLib - Setup Guide**

## **1. Download the Source Code**

Clone or download the `src/` folder from the repository.

```sh
git clone <repository_url>
```

Or manually download the `src/` folder and place it inside your project directory.

## **2. Linking the Library to Your Project**

To use **CTerminalPlotLib**, include the main source file in your project:

```c
#include "../src/CTerminalPlotLib.c"

// Your project code
```

<!-- Alternatively, compile the library separately and link it during compilation:

```sh
gcc -o your_program your_source.c ../src/CTerminalPlotLib.c -lm
```

- Ensure that the `-lm` flag is included for mathematical operations.
- Adjust the relative path (`../src/`) based on your project structure. -->

<!-- ## **3. Compilation Example**

If your project consists of multiple source files, compile them together:

```sh
gcc -o your_program main.c ../src/CTerminalPlotLib.c -lm
``` -->

## **3. Usage Example**

Here’s a basic example to ensure the library is working:

```c
#include <stdio.h>
#include "../src/CTerminalPlotLib.c"

int main()
{
    if (ctp_isActive())
        printf("CTerminalPlotLib setup successful!\n");

    return 0;
}
```

## **5. Additional Notes**

- Ensure your terminal supports ANSI escape codes for proper rendering.
- If you encounter any issues, verify that your compiler supports C99 or later.
- For more details, refer to the documentation inside the `docs/` folder.
