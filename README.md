# **CTerminalPlot**

`CTerminalPlot` is a **lightweight C library** for **real-time table plotting and scatter plotting** directly in the **terminal**, utilizing `CP_UTF8` encoding. Inspired by **Matplotlib**, this library provides a simple and efficient way to visualize data with a **default resolution of 30×90 pixels**, which can be adjusted based on terminal capabilities.

## **Features**

✅ **Lightweight & Optimized** – Small, dependency-free core that runs in any UTF-8 + ANSI terminal on **Windows, Linux, and macOS**, with minimal memory and CPU usage.  
✅ **Versatile Data Types** – The element type is a single `typedef` (`CTP_PARAM`, default `float`) you can switch to **`char` (1 byte), `int` (4 bytes), `float` (4 bytes), or `double` (8 bytes)**.  
✅ **Dynamic Memory Management** – Automatically adjusts **rows, columns, and parameters** to optimize memory usage efficiently.  
✅ **Matplotlib-Like Syntax** – Provides an intuitive API similar to **Matplotlib**, making it easy to use for those familiar with Python-based plotting.  
✅ **Multi-Parameter Plotting** – Supports **multiple variables** with **distinct colors** for better graph visualization.  
✅ **Search & Filtering Functions** – Includes built-in **search capabilities** for analyzing data across multiple parameters.  
✅ **UTF-8 Terminal Graphics** – Uses **UTF-8 encoded symbols and colors** to enhance graph readability and aesthetics.

## **Project Structure**

```plaintext
CTerminalPlot/
 ├── 📂 src/        # Source code (.c files) (for users to include)
 ├── 📂 docs/       # Documentation folder
 |   └──
 └── 📂 examples/   # Standalone example programs referenced in docs/

```

## **Showcase**
