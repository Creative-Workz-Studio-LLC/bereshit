# ═══════════════════════════════════════════════════════════════════════════
# libtrit - Balanced Ternary Type Library
# Key: B-word-work-pkg-trit-makefile
# ═══════════════════════════════════════════════════════════════════════════
#
# DEPENDENCY CLASSIFICATION: PURE (needs: gcc, ar, make)
#   Standard C toolchain only - no external scripts or tools
#
# derives_from: bereshit/word/seed/code/make/makefile.mk
# See: word/constants/ternary-math.toml for mathematical foundations
#
# ═══════════════════════════════════════════════════════════════════════════

# ============================================================================
# METADATA
# ============================================================================
#
# Package:     creativeworkzstudio.com/bereshit/word/work/pkg/trit
# File:        Makefile
# Key:         B-word-work-pkg-trit-makefile
#
# ────────────────────────────────────────────────────────────────
# CORE IDENTITY
# ────────────────────────────────────────────────────────────────
#
# Biblical Foundation:
#
#   Scripture: "In the beginning God created the heaven and the earth."
#              — Genesis 1:1
#
#   Principle: Foundation before construction. The trit type is the
#              foundational unit of ternary computing, just as Genesis 1:1
#              is the foundation of all Scripture.
#
#   Anchor: "Except the LORD build the house, they labour in vain that
#            build it." — Psalm 127:1
#
# CPI-SI Identity:
#
#   Component Type: Ladder (foundational building block)
#   Role: Provides core trit type for all ternary operations
#   Paradigm: CPI-SI framework component
#
# Authorship & Lineage:
#
#   Architect: Seanje Lenox-Wise
#   Implementation: Nova Dawn
#   Created: 2025-12-12
#   Version: 0.1.0
#
# Purpose & Function:
#
#   Purpose: Build libtrit.a static library for balanced ternary operations
#
#   Key Features:
#     - Balanced ternary trit type (-1, 0, +1)
#     - Dimensional mapping (TIME, SPACE, MATTER)
#     - Pack/unpack algorithms (Horner's method, repeated division)
#     - Static library for linking into larger programs
#
#   Philosophy: Measure twice, cut once. Build foundations that last.
#
# ────────────────────────────────────────────────────────────────
# INTERFACE
# ────────────────────────────────────────────────────────────────
#
# Dependencies:
#
#   System Tools: make, ar
#   Language Toolchain: gcc (C99)
#   External Tools: None
#
# Usage:
#
#   make              # Build library (default)
#   make libtrit.a    # Build static library
#   make test         # Run tests
#   make clean        # Remove build artifacts
#   make help         # Show targets
#
# ============================================================================
# END METADATA
# ============================================================================

# ============================================================================
# SETUP
# ============================================================================

# ────────────────────────────────────────────────────────────────
# Declarations
# ────────────────────────────────────────────────────────────────

.PHONY: all generate libtrit.a check-headers test clean help info \
        test-trit test-pack test-dimension test-temporal test-integration \
        test-math test-bible-rail test-verse-addr test-scripture-path test-scripture-text \
        test-statemachine

# ────────────────────────────────────────────────────────────────
# Constants
# ────────────────────────────────────────────────────────────────

# Project identity
LIB_NAME = libtrit.a
BUILD_DIR = build

# Directory structure
SRC_DIR = src
INC_DIR = include
TEST_DIR = test
GEN_DIR = $(INC_DIR)/generated

# Code generation (TOML → C headers)
# BERESHIT_ROOT must be set, default to 4 levels up from pkg/trit/
BERESHIT_ROOT ?= $(realpath ../../../..)
GENERATOR_PKG = $(BERESHIT_ROOT)/word/work/cmd/generate-config

# ────────────────────────────────────────────────────────────────
# Variables
# ────────────────────────────────────────────────────────────────

# Tool configuration (overridable)
CC ?= gcc
AR ?= ar
CFLAGS ?= -std=c99 -Wall -Wextra -Werror -pedantic -O2
ARFLAGS ?= rcs

# Include paths
INCLUDES = -I$(INC_DIR)

# Source and object files (will be populated in Phase 2)
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# ────────────────────────────────────────────────────────────────
# Pattern Rules
# ────────────────────────────────────────────────────────────────

# Compile C source to object file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "  CC    $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# ────────────────────────────────────────────────────────────────
# Default Target
# ────────────────────────────────────────────────────────────────

all: libtrit.a

# ============================================================================
# END SETUP
# ============================================================================

# ============================================================================
# BODY
# ============================================================================

# ────────────────────────────────────────────────────────────────
# Target Dependency Graph
# ────────────────────────────────────────────────────────────────
#
# Ladder Structure:
#
#   User-Facing (Top):
#   ├── all → libtrit.a
#   ├── generate → $(GEN_DIR)
#   ├── test → libtrit.a
#   ├── clean → (standalone)
#   └── help → (standalone)
#
#   Build Operations (Middle):
#   ├── libtrit.a → generate → $(OBJS) → $(BUILD_DIR)
#   └── $(BUILD_DIR)/%.o → $(SRC_DIR)/%.c
#
#   Code Generation:
#   └── generate → go run generate-config → .gen.h files
#
#   Internal Helpers (Bottom):
#   ├── $(BUILD_DIR) → (creates directory)
#   └── $(GEN_DIR) → (creates directory)
#
# Baton Flow:
#   make → all → libtrit.a → generate → $(OBJS) → $(BUILD_DIR) → exit
#

# ────────────────────────────────────────────────────────────────
# Internal Helpers
# ────────────────────────────────────────────────────────────────

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# ────────────────────────────────────────────────────────────────
# Build Operations
# ────────────────────────────────────────────────────────────────

## generate: Generate C headers from TOML specs
generate: | $(GEN_DIR)
	@echo "Generating headers from TOML specs..."
	@cd $(BERESHIT_ROOT) && go run $(GENERATOR_PKG)/...
	@echo "✓ Headers generated"

$(GEN_DIR):
	@mkdir -p $(GEN_DIR)

## libtrit.a: Build the static library
libtrit.a: generate $(BUILD_DIR)/$(LIB_NAME)

$(BUILD_DIR)/$(LIB_NAME): $(OBJS) | $(BUILD_DIR)
	@if [ -z "$(OBJS)" ]; then \
		echo "⚠ No source files yet (Phase 1: structure only)"; \
		echo "  Add .c files to src/ in Phase 2"; \
		touch $(BUILD_DIR)/$(LIB_NAME); \
	else \
		echo "  AR    $(LIB_NAME)"; \
		$(AR) $(ARFLAGS) $@ $(OBJS); \
		echo "✓ Built $(BUILD_DIR)/$(LIB_NAME)"; \
	fi

## check-headers: Validate headers compile (no source needed)
check-headers: | $(BUILD_DIR)
	@echo "Checking headers..."
	@echo '#include "trit.h"' > $(BUILD_DIR)/_check.c
	@echo 'int main(void) { return 0; }' >> $(BUILD_DIR)/_check.c
	@$(CC) $(CFLAGS) $(INCLUDES) -fsyntax-only $(BUILD_DIR)/_check.c && \
		echo "✓ Headers valid" || \
		(echo "✗ Header errors"; exit 1)
	@rm -f $(BUILD_DIR)/_check.c

## test: Run all tests (MATTER, SPACE, TIME, Integration, Math, Bible Rail, Verse Address, Scripture Path, Scripture Text, State Machine)
test: test-trit test-pack test-dimension test-temporal test-integration test-math test-bible-rail test-verse-addr test-scripture-path test-scripture-text test-statemachine
	@echo ""
	@echo "════════════════════════════════════════════════════════════════"
	@echo "All tests complete. Run individual tests with: make test-<layer>"
	@echo "════════════════════════════════════════════════════════════════"

## test-trit: Run MATTER layer tests (trit.c)
test-trit: libtrit.a
	@echo "Testing MATTER layer (trit.c)..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/test_trit $(TEST_DIR)/trit_test.c $(BUILD_DIR)/$(LIB_NAME)
	@./$(BUILD_DIR)/test_trit

## test-pack: Run MATTER packing tests (pack.c)
test-pack: libtrit.a
	@echo "Testing MATTER packing (pack.c)..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/test_pack $(TEST_DIR)/pack_test.c $(BUILD_DIR)/$(LIB_NAME)
	@./$(BUILD_DIR)/test_pack

## test-dimension: Run SPACE layer tests (dimension.c)
test-dimension: libtrit.a
	@echo "Testing SPACE layer (dimension.c)..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/test_dimension $(TEST_DIR)/dimension_test.c $(BUILD_DIR)/$(LIB_NAME)
	@./$(BUILD_DIR)/test_dimension

## test-temporal: Run TIME layer tests (temporal.c)
test-temporal: libtrit.a
	@echo "Testing TIME layer (temporal.c)..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/test_temporal $(TEST_DIR)/temporal_test.c $(BUILD_DIR)/$(LIB_NAME)
	@./$(BUILD_DIR)/test_temporal

## test-integration: Run integration tests (all layers)
test-integration: libtrit.a
	@echo "Testing Integration (all layers)..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/test_integration $(TEST_DIR)/integration_test.c $(BUILD_DIR)/$(LIB_NAME)
	@./$(BUILD_DIR)/test_integration

## test-math: Run math operations tests (Phase 2)
test-math: libtrit.a
	@echo "Testing Math Operations (5 families)..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/math_test $(TEST_DIR)/math_test.c $(BUILD_DIR)/$(LIB_NAME)
	@./$(BUILD_DIR)/math_test

## test-bible-rail: Run Bible Rail encoding tests (Phase 2)
test-bible-rail: libtrit.a
	@echo "Testing Bible Rail Encoding (13 variants)..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/bible_rail_test $(TEST_DIR)/bible_rail_test.c $(BUILD_DIR)/$(LIB_NAME)
	@./$(BUILD_DIR)/bible_rail_test

## test-verse-addr: Run Verse Address encoding tests (Phase 2)
test-verse-addr: libtrit.a
	@echo "Testing Verse Address Encoding (31,102 verses)..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/verse_addr_test $(TEST_DIR)/verse_addr_test.c $(BUILD_DIR)/$(LIB_NAME)
	@./$(BUILD_DIR)/verse_addr_test

## test-scripture-path: Run Scripture Path resolution tests (Phase 2)
test-scripture-path: libtrit.a
	@echo "Testing Scripture Path Resolution (31,115 entries)..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/scripture_path_test $(TEST_DIR)/scripture_path_test.c $(BUILD_DIR)/$(LIB_NAME)
	@./$(BUILD_DIR)/scripture_path_test

## test-scripture-text: Run Scripture Text encoding tests (Phase 2)
test-scripture-text: libtrit.a
	@echo "Testing Scripture Text Encoding (243 values)..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/scripture_text_test $(TEST_DIR)/scripture_text_test.c $(BUILD_DIR)/$(LIB_NAME)
	@./$(BUILD_DIR)/scripture_text_test

## test-statemachine: Run CPI-SI State Machine tests (Hebrew, Cube, K:ALIGN, Trajectory, Unified)
test-statemachine: libtrit.a
	@echo "Testing CPI-SI State Machine (25 tests)..."
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/statemachine_test $(TEST_DIR)/statemachine_test.c $(BUILD_DIR)/$(LIB_NAME) -lm
	@./$(BUILD_DIR)/statemachine_test

## clean: Remove build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)
	@echo "✓ Clean complete"

## help: Show available targets
help:
	@echo "libtrit - Balanced Ternary Type Library"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@sed -n 's/^## //p' Makefile | column -t -s ':' | sed -e 's/^/  /'
	@echo ""
	@echo "Configuration:"
	@echo "  CC=$(CC)  AR=$(AR)"
	@echo "  CFLAGS=$(CFLAGS)"

## info: Show build configuration
info:
	@echo "Build Configuration:"
	@echo "  LIB_NAME:   $(LIB_NAME)"
	@echo "  BUILD_DIR:  $(BUILD_DIR)"
	@echo "  SRC_DIR:    $(SRC_DIR)"
	@echo "  INC_DIR:    $(INC_DIR)"
	@echo "  CC:         $(CC)"
	@echo "  CFLAGS:     $(CFLAGS)"
	@echo ""
	@echo "Source files: $(SRCS)"
	@echo "Object files: $(OBJS)"

# ============================================================================
# END BODY
# ============================================================================

# ============================================================================
# CLOSING
# ============================================================================
#
# ────────────────────────────────────────────────────────────────
# Validation
# ────────────────────────────────────────────────────────────────
#
# Test sequence:
#   make clean && make        # Fresh build
#   make test                 # Run tests
#   make help                 # Verify documentation
#
# ────────────────────────────────────────────────────────────────
# Modification Policy
# ────────────────────────────────────────────────────────────────
#
# Safe to Modify:
#   ✅ CFLAGS optimization flags
#   ✅ Add new source files to src/
#   ✅ Add new test files to test/
#
# Modify with Care:
#   ⚠️ Pattern rules (affect all compilations)
#   ⚠️ Library name (breaks linking)
#
# Never Modify:
#   ❌ 4-block structure
#   ❌ Remove clean target
#   ❌ Add rm -rf without constraints
#
# ────────────────────────────────────────────────────────────────
# Related Components
# ────────────────────────────────────────────────────────────────
#
# Specifications:
#   - word/constants/ternary-math.toml (powers, algorithms, dimensions)
#   - word/core/primitives.toml (trit type definition)
#
# Documentation:
#   - word/research/trit-foundations.adoc
#   - word/research/ternary-theological-foundations.adoc
#
# Demo:
#   - tov/demo/phase-1/demo-trit/
#
# ────────────────────────────────────────────────────────────────
# Quick Reference
# ────────────────────────────────────────────────────────────────
#
# Basic:
#   make              # Build library (regenerates headers first)
#   make generate     # Generate .gen.h from TOML specs
#   make test         # Run tests
#   make clean        # Clean artifacts
#
# Debug:
#   make -n all       # Dry-run
#   make info         # Show config
#   make CC=clang     # Use clang
#
# "Except the LORD build the house, they labour in vain." — Psalm 127:1
#
# ============================================================================
# END CLOSING
# ============================================================================
