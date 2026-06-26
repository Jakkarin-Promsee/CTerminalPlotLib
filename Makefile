# CTerminalPlot — build the static library, the examples, and the tests.
#
# Targets:
#   make            build the library + all example programs
#   make test       build and run the assertion tests
#   make clean      remove build artifacts
#
# Run from a Unix-like shell (Git Bash / MSYS2 / Linux / macOS).

CC      = gcc
CFLAGS  = -Isrc/include -Wall -Wextra -std=c11 -g
LDLIBS  = -lm

BUILD   = build
LIB     = $(BUILD)/libctp.a
LIB_OBJ = $(BUILD)/CTerminalPlotLib.o
CLI_BIN = $(BUILD)/ctplot.exe

EXAMPLE_SRC  = $(wildcard examples/*.c)
EXAMPLE_BINS = $(patsubst examples/%.c,examples/output/%.exe,$(EXAMPLE_SRC))

.PHONY: all examples cli test clean

all: examples cli

# --- ctplot CLI ---
cli: $(CLI_BIN)

$(CLI_BIN): src/ctplot.c $(LIB) | $(BUILD)
	$(CC) $(CFLAGS) src/ctplot.c $(LIB) -o $@ $(LDLIBS)

# --- static library ---
$(LIB): $(LIB_OBJ)
	ar rcs $@ $^

$(LIB_OBJ): src/CTerminalPlotLib.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# --- examples (each one includes the header and links the library) ---
examples: $(EXAMPLE_BINS)

examples/output/%.exe: examples/%.c $(LIB) | examples/output
	$(CC) $(CFLAGS) $< $(LIB) -o $@ $(LDLIBS)

# --- tests ---
test: $(LIB) | tests/output
	$(CC) $(CFLAGS) tests/test_ctp.c $(LIB) -o tests/output/test_ctp.exe $(LDLIBS)
	./tests/output/test_ctp.exe

$(BUILD) examples/output tests/output:
	mkdir -p $@

clean:
	rm -rf $(BUILD)
	rm -f examples/output/*.exe tests/output/*.exe
