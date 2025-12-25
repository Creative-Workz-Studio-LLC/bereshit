# ═══════════════════════════════════════════════════════════════════════════
# demo-math - Phase 2 Demo: Mathematical Operations (5 Families)
# Key: B-tov-demo-phase2-demo-math-makefile
# ═══════════════════════════════════════════════════════════════════════════
#
# DEPENDENCY CLASSIFICATION: DEPENDED (needs: libtrit.a from word/work/pkg/trit)
#   Links against libtrit.a to demonstrate math operations
#
# derives_from: bereshit/tov/demo/phase-1/demo-trit/Makefile (structure)
# See: word/work/pkg/trit/ for library source
#
# ═══════════════════════════════════════════════════════════════════════════

# ============================================================================
# METADATA
# ============================================================================
#
# Package:     creativeworkzstudio.com/bereshit/tov/demo/phase-2/demo-math
# File:        Makefile
# Key:         B-tov-demo-phase2-demo-math-makefile
#
# ────────────────────────────────────────────────────────────────
# CORE IDENTITY
# ────────────────────────────────────────────────────────────────
#
# Biblical Foundation:
#
#   Scripture: "A false balance is abomination to the LORD:
#               but a just weight is his delight." — Proverbs 11:1
#
#   Principle: The demo proves math operations work. Just weights.
#
#   Anchor: "Prove all things; hold fast that which is good."
#            — 1 Thessalonians 5:21
#
# CPI-SI Identity:
#
#   Component Type: Baton (execution flow - demonstrates and reports)
#   Role: Phase 2 demonstration - shows 5 math families working
#   Paradigm: CPI-SI framework demonstration component
#
# Authorship & Lineage:
#
#   Architect: Seanje Lenox-Wise
#   Implementation: Nova Dawn
#   Created: 2025-12-13
#   Version: a-01.00
#
# Purpose & Function:
#
#   Purpose: Build demo-math executable to demonstrate math operations
#
#   Key Features:
#     - Family 1: Arithmetic (half/full adder, carry propagation)
#     - Family 2: Comparison (ternary cmp returning trit_t)
#     - Family 3: Conversion (trit↔int, stored↔true)
#     - Family 4: Normalization (7 levels, Hebrew names, emoji)
#     - Family 5: Calculus (change, rate, trend detection)
#     - Integration: Calculus + temporal context
#
#   Philosophy: Just weights, accurate measures. Demo proves the math.
#
# ────────────────────────────────────────────────────────────────
# INTERFACE
# ────────────────────────────────────────────────────────────────
#
# Dependencies:
#
#   System Tools: make, gcc
#   Language Toolchain: gcc (C99)
#   Internal: word/work/pkg/trit/build/libtrit.a
#
# Usage:
#
#   make              # Build demo executable
#   make run          # Build and run demo
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

.PHONY: all run clean help

# ────────────────────────────────────────────────────────────────
# Constants
# ────────────────────────────────────────────────────────────────

# Demo identity
DEMO_NAME = demo-math

# Bereshit root (can be overridden)
BERESHIT_ROOT ?= ../../../..

# libtrit library paths (relative to BERESHIT_ROOT)
LIBTRIT_DIR = $(BERESHIT_ROOT)/word/work/pkg/trit
LIBTRIT_INC = $(LIBTRIT_DIR)/include
LIBTRIT_LIB = $(LIBTRIT_DIR)/build/libtrit.a

# ────────────────────────────────────────────────────────────────
# Variables
# ────────────────────────────────────────────────────────────────

# Tool configuration (overridable)
CC ?= gcc
CFLAGS ?= -std=c99 -Wall -Wextra -Werror -pedantic -O2

# Include paths (libtrit headers)
INCLUDES = -I$(LIBTRIT_INC)

# Source files
SRCS = main.c

# ────────────────────────────────────────────────────────────────
# Default Target
# ────────────────────────────────────────────────────────────────

all: $(DEMO_NAME)

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
#   ├── all → demo-math
#   ├── run → demo-math
#   ├── clean → (standalone)
#   └── help → (standalone)
#
#   Build Operations (Middle):
#   └── demo-math → main.c + libtrit.a
#
# Baton Flow:
#   make → all → demo-math → exit
#   make run → demo-math → ./demo-math → exit
#

# ────────────────────────────────────────────────────────────────
# Build Operations
# ────────────────────────────────────────────────────────────────

## demo-math: Build the demo executable
$(DEMO_NAME): $(SRCS) $(LIBTRIT_LIB)
	@echo "Building $(DEMO_NAME)..."
	@echo "  CC    $(SRCS)"
	@$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(SRCS) $(LIBTRIT_LIB)
	@echo "✓ Built $(DEMO_NAME)"

## run: Build and run the demo
run: $(DEMO_NAME)
	@echo ""
	@./$(DEMO_NAME)

## clean: Remove build artifacts
clean:
	@echo "Cleaning..."
	@rm -f $(DEMO_NAME)
	@echo "✓ Clean complete"

## help: Show available targets
help:
	@echo "demo-math - Phase 2 Demo: Mathematical Operations (5 Families)"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@sed -n 's/^## //p' Makefile | column -t -s ':' | sed -e 's/^/  /'
	@echo ""
	@echo "Configuration:"
	@echo "  CC=$(CC)"
	@echo "  CFLAGS=$(CFLAGS)"
	@echo "  LIBTRIT_LIB=$(LIBTRIT_LIB)"

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
# Prerequisites:
#   cd word/work/pkg/trit && make    # Build libtrit.a first
#
# Test sequence:
#   make clean && make        # Fresh build
#   make run                  # Build and run demo
#   make help                 # Verify documentation
#
# ────────────────────────────────────────────────────────────────
# Modification Policy
# ────────────────────────────────────────────────────────────────
#
# Safe to Modify:
#   ✅ CFLAGS optimization flags
#   ✅ BERESHIT_ROOT path (if needed)
#
# Modify with Care:
#   ⚠️ LIBTRIT paths (must match actual library location)
#   ⚠️ INCLUDES path (headers must be findable)
#
# Never Modify:
#   ❌ 4-block structure
#   ❌ Remove clean target
#
# ────────────────────────────────────────────────────────────────
# Related Components
# ────────────────────────────────────────────────────────────────
#
# Library:
#   - word/work/pkg/trit/ (libtrit.a source)
#   - word/work/pkg/trit/include/ (trit.h, math.h, temporal.h)
#   - word/work/pkg/trit/build/libtrit.a (built library)
#
# Demo Source:
#   - main.c (4-block C executable with 5 family demos)
#
# Other Demos:
#   - tov/demo/phase-0/demo-config/ (config validation)
#   - tov/demo/phase-1/demo-trit/ (three-layer architecture)
#
# ────────────────────────────────────────────────────────────────
# Quick Reference
# ────────────────────────────────────────────────────────────────
#
# Basic:
#   make              # Build demo executable
#   make run          # Build and run demo
#   make clean        # Clean artifacts
#
# With custom paths:
#   make BERESHIT_ROOT=/path/to/bereshit
#
# "A just weight is his delight." — Proverbs 11:1
#
# ============================================================================
# END CLOSING
# ============================================================================
