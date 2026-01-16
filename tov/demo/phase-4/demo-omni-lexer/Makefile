# =============================================================================
# demo-omni-lexer Makefile
# Key: B-tov-demo-phase-4-demo-omni-lexer-makefile
# =============================================================================
#
# Purpose: Build and run the OmniCode lexer demo
#
# =============================================================================

# ============================================================================
# SETUP
# ============================================================================

.PHONY: all run clean help

# Directories
BERESHIT_ROOT = ../../../..
OMNI_DIR = $(BERESHIT_ROOT)/word/work/pkg/omni
BIN_DIR = bin

# Compiler settings
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2
INCLUDES = -I$(OMNI_DIR)/include

# Source files
MAIN_SRC = main.c
OMNI_SRCS = $(OMNI_DIR)/src/omni_lexer.c

# Output
TARGET = $(BIN_DIR)/demo-omni-lexer

# Default file to tokenize
FILE ?= void/root.omni

# ============================================================================
# BODY
# ============================================================================

## all: Build the demo (default)
all: $(TARGET)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(TARGET): $(MAIN_SRC) $(OMNI_SRCS) | $(BIN_DIR)
	@echo "Building demo-omni-lexer..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(MAIN_SRC) $(OMNI_SRCS)
	@echo "Built: $@"

## run: Build and run the demo
run: $(TARGET)
	@echo ""
	@cd $(BERESHIT_ROOT) && ./tov/demo/phase-4/demo-omni-lexer/$(TARGET) $(FILE)

## clean: Remove build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(BIN_DIR)
	@echo "Clean complete"

## help: Show available targets
help:
	@echo "demo-omni-lexer - OmniCode Lexer Demo"
	@echo ""
	@echo "Usage: make [target] [FILE=path]"
	@echo ""
	@echo "Targets:"
	@sed -n 's/^## //p' Makefile | column -t -s ':'
	@echo ""
	@echo "Examples:"
	@echo "  make run                      # Tokenize void/root.omni"
	@echo "  make run FILE=tov/root.omni   # Tokenize specified file"

# ============================================================================
# CLOSING
# ============================================================================
#
# Build: make
# Run:   make run
# Clean: make clean
#
# =============================================================================
