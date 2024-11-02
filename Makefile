# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Iinclude -Wall -g

# Source files
SRC = src/CTerminalPlotLib.c src/main.c

# Object files
OBJ = $(SRC:.c=.o)

# Target executable
TARGET = my_program

# Default target
all: $(TARGET)

# Rule to link object files to create the executable
$(TARGET): $(OBJ)
	$(CC) -o $@ $^

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove object files and the target executable
clean:
	rm -f $(OBJ) $(TARGET)
