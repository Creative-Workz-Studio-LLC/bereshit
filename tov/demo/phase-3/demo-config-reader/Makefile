# =============================================================================
# demo-config-reader Makefile
# Key: B-tov-demo-phase-3-demo-config-reader-makefile
# =============================================================================
#
# Purpose: Build and run the config reader demo
#
# =============================================================================

# ============================================================================
# SETUP
# ============================================================================

.PHONY: all run clean help

# Directories
BERESHIT_ROOT = ../../../..
LIBTRIT_DIR = $(BERESHIT_ROOT)/word/work/pkg/trit
CORE_DIR = $(BERESHIT_ROOT)/word/core
BIN_DIR = bin

# Compiler settings
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2
INCLUDES = -I$(LIBTRIT_DIR)/include

# Source files
MAIN_SRC = main.c
LIBTRIT_SRCS = $(LIBTRIT_DIR)/src/toml.c $(LIBTRIT_DIR)/src/config.c

# Output
TARGET = $(BIN_DIR)/demo-config-reader

# ============================================================================
# BODY
# ============================================================================

## all: Build the demo (default)
all: $(TARGET)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(TARGET): $(MAIN_SRC) $(LIBTRIT_SRCS) | $(BIN_DIR)
	@echo "Building demo-config-reader..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(MAIN_SRC) $(LIBTRIT_SRCS)
	@echo "Built: $@"

## run: Build and run the demo
run: $(TARGET)
	@echo ""
	@cd $(BERESHIT_ROOT) && ./tov/demo/phase-3/demo-config-reader/$(TARGET) word/core

## clean: Remove build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(BIN_DIR)
	@echo "Clean complete"

## help: Show available targets
help:
	@echo "demo-config-reader - Config Reader Demo"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@sed -n 's/^## //p' Makefile | column -t -s ':'

# ============================================================================
# CLOSING
# ============================================================================
#
# Build: make
# Run:   make run
# Clean: make clean
#
# =============================================================================
