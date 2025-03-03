<!-- 1_Setup-Guide.md -->

# **CTerminalPlotLib - Setup Guide**

## **Overview**

CTerminalPlotLib is a lightweight C library for creating text-based plots directly in your terminal. This guide will help you set up the library in your project.

## **1. Download the Source Code**

Clone the repository or download the `src/` folder:

```sh
git clone <repository_url>
```

Alternatively, manually download the `src/` folder and place it inside your project directory.

## **2. Include the Library in Your Project**

To use CTerminalPlotLib, include the main source file in your project:

```c
#include "../src/CTerminalPlotLib.c"

// Your project code goes here
```

## **3. Verify Library**

Use this simple program to verify that the library is correctly set up:

```c
#include <stdio.h>
#include "../src/CTerminalPlotLib.c"

int main() {
    if (ctp_isActive())
        printf("CTerminalPlotLib setup successful!\n");
    else
        printf("CTerminalPlotLib setup failed!\n");

    return 0;
}
```

## **5. Troubleshooting**

- **Terminal Display Issues**: Ensure your terminal supports ANSI escape codes
- **Runtime Errors**: Check that all library dependencies are properly included

## **6. Next Steps**

Once you've successfully set up CTerminalPlotLib, proceed to the [Basic Usage Guide](2_basic-usage.md) to learn how to create your first plots.

For more detailed information, refer to the documentation in the `docs/` folder.
