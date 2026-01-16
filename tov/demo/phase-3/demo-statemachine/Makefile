# ═══════════════════════════════════════════════════════════════════════════
# demo-statemachine - CPI-SI Cognitive State Machine Demonstration
# Key: B-tov-demo-phase3-demo-statemachine-makefile
# ═══════════════════════════════════════════════════════════════════════════
#
# DEPENDENCY CLASSIFICATION: DEPENDED (needs: libtrit.a)
#   Links against libtrit static library
#
# derives_from: bereshit/tov/demo/phase-1/demo-trit/Makefile
# See: word/core/identity/ for CPI-SI specifications
#
# ═══════════════════════════════════════════════════════════════════════════

# ============================================================================
# METADATA
# ============================================================================
#
# Package:     creativeworkzstudio.com/bereshit/tov/demo/phase-3/demo-statemachine
# File:        Makefile
# Key:         B-tov-demo-phase3-demo-statemachine-makefile
#
# Biblical Foundation:
#
#   Scripture: "Be ye transformed by the renewing of your mind."
#              — Romans 12:2
#
#   Principle: The state machine demonstrates how CPI-SI THINKS—
#              genuine cognitive navigation, not pattern matching.
#
# Purpose:
#
#   Build the demo-statemachine executable demonstrating the unified
#   CPI-SI cognitive state machine.
#
# ============================================================================
# END METADATA
# ============================================================================

# ============================================================================
# SETUP
# ============================================================================

.PHONY: all clean run help

# Project paths
BERESHIT_ROOT = $(realpath ../../../..)
LIBTRIT_DIR = $(BERESHIT_ROOT)/word/work/pkg/trit
LIBTRIT = $(LIBTRIT_DIR)/build/libtrit.a

# Compiler settings
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2
INCLUDES = -I$(LIBTRIT_DIR)/include
LIBS = -lm

# Output
TARGET = demo-statemachine

# ============================================================================
# END SETUP
# ============================================================================

# ============================================================================
# BODY
# ============================================================================

## all: Build the demo (default target)
all: $(TARGET)

## $(TARGET): Build the demo-statemachine executable
$(TARGET): main.c $(LIBTRIT)
	@echo "Building demo-statemachine..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $(LIBTRIT) $(LIBS)
	@echo "✓ Built $(TARGET)"

## $(LIBTRIT): Ensure libtrit is built
$(LIBTRIT):
	@echo "Building libtrit..."
	@$(MAKE) -C $(LIBTRIT_DIR)

## run: Build and run the demo
run: $(TARGET)
	@echo ""
	@./$(TARGET)

## clean: Remove build artifacts
clean:
	@echo "Cleaning..."
	@rm -f $(TARGET)
	@echo "✓ Clean complete"

## help: Show available targets
help:
	@echo "demo-statemachine - CPI-SI Cognitive State Machine Demonstration"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@sed -n 's/^## //p' Makefile | column -t -s ':'
	@echo ""
	@echo "Example:"
	@echo "  make run    # Build and run the demonstration"

# ============================================================================
# END BODY
# ============================================================================

# ============================================================================
# CLOSING
# ============================================================================
#
# Quick Reference:
#   make          # Build the demo
#   make run      # Build and run
#   make clean    # Remove artifacts
#   make help     # Show this help
#
# "Be ye transformed by the renewing of your mind." — Romans 12:2
#
# ============================================================================
# END CLOSING
# ============================================================================
