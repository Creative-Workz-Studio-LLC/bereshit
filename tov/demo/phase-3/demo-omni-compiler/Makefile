# ═══════════════════════════════════════════════════════════════════════════
# demo-omni-compiler - OmniCode Compiler Demo
# Key: B-tov-demo-phase-3-demo-omni-compiler-makefile
# ═══════════════════════════════════════════════════════════════════════════
#
# DEPENDENCY CLASSIFICATION: PURE (needs: gcc, make) + libomni + libtrit
#   Standard C toolchain plus internal libraries
#
# Biblical Foundation:
#   Scripture: "Let there be light: and there was light" — Genesis 1:3
#   Principle: Words that CREATE — OmniCode sentences become executables
#
# ═══════════════════════════════════════════════════════════════════════════

# ============================================================================
# SETUP
# ============================================================================

.PHONY: all clean test run help

# Project structure
DEMO_NAME = demo-omni-compiler
BUILD_DIR = bin

# Dependencies (relative to demo directory)
BERESHIT_ROOT ?= $(realpath ../../../..)
OMNI_DIR = $(BERESHIT_ROOT)/word/work/pkg/omni
TRIT_DIR = $(BERESHIT_ROOT)/word/work/pkg/trit

# Libraries
OMNI_LIB = $(OMNI_DIR)/build/libomni.a
TRIT_LIB = $(TRIT_DIR)/build/libtrit.a

# Compiler configuration
CC ?= gcc
CFLAGS ?= -std=c99 -Wall -Wextra -O2 -D_GNU_SOURCE
INCLUDES = -I$(OMNI_DIR)/include -I$(TRIT_DIR)/include
LIBS = $(OMNI_LIB) $(TRIT_LIB)

# Default target
all: $(BUILD_DIR)/$(DEMO_NAME)

# ============================================================================
# END SETUP
# ============================================================================

# ============================================================================
# BODY
# ============================================================================

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Ensure dependencies are built
$(OMNI_LIB):
	@echo "Building dependency: libomni..."
	@$(MAKE) -C $(OMNI_DIR) libomni.a

$(TRIT_LIB):
	@echo "Building dependency: libtrit..."
	@$(MAKE) -C $(TRIT_DIR) libtrit.a

# Build demo executable
$(BUILD_DIR)/$(DEMO_NAME): main.c $(OMNI_LIB) $(TRIT_LIB) | $(BUILD_DIR)
	@echo "Building $(DEMO_NAME)..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $@ main.c $(LIBS)
	@echo "✓ Built $(BUILD_DIR)/$(DEMO_NAME)"

## run: Build and run the demo
run: $(BUILD_DIR)/$(DEMO_NAME)
	@echo ""
	@./$(BUILD_DIR)/$(DEMO_NAME)

## test: Run demo and verify exit code
test: run
	@echo ""
	@if [ -f hello_omni ]; then \
		echo "═══════════════════════════════════════════════════════════════════"; \
		echo "  Compiled OmniCode standalone test:"; \
		echo "═══════════════════════════════════════════════════════════════════"; \
		./hello_omni; \
		echo ""; \
	fi

## clean: Remove build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)
	@rm -f hello_generated.c hello_omni
	@echo "✓ Clean complete"

## help: Show available targets
help:
	@echo "demo-omni-compiler - OmniCode Compiler Demo"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@sed -n 's/^## //p' Makefile | column -t -s ':' | sed -e 's/^/  /'
	@echo ""
	@echo "Pipeline:"
	@echo "  hello.omni → libomni → hello_generated.c → gcc + libtrit → hello_omni"

# ============================================================================
# END BODY
# ============================================================================

# ============================================================================
# CLOSING
# ============================================================================
#
# Usage:
#   make          # Build demo
#   make run      # Build and run demo
#   make test     # Full test with standalone verification
#   make clean    # Remove build artifacts
#
# Expected output:
#   Final Health: 100 (GRANTED)
#   "[INFO] Access granted"
#
# "Let there be light: and there was light." — Genesis 1:3
#
# ============================================================================
# END CLOSING
# ============================================================================
