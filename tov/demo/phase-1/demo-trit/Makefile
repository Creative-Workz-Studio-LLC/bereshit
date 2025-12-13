# ═══════════════════════════════════════════════════════════════════════════
# demo-trit - Phase 1 Demo: libtrit Three-Layer Architecture Validation
# Key: B-tov-demo-phase1-demo-trit-makefile
# ═══════════════════════════════════════════════════════════════════════════
#
# DEPENDENCY CLASSIFICATION: DEPENDED (needs: libtrit.a from word/work/pkg/trit)
#   Links against libtrit.a to demonstrate three-layer architecture
#
# derives_from: bereshit/word/work/pkg/trit/Makefile
# See: word/work/pkg/trit/ for library source
#
# ═══════════════════════════════════════════════════════════════════════════

# ============================================================================
# METADATA
# ============================================================================
#
# Package:     creativeworkzstudio.com/bereshit/tov/demo/phase-1/demo-trit
# File:        Makefile
# Key:         B-tov-demo-phase1-demo-trit-makefile
#
# ────────────────────────────────────────────────────────────────
# CORE IDENTITY
# ────────────────────────────────────────────────────────────────
#
# Biblical Foundation:
#
#   Scripture: "By their fruits ye shall know them." — Matthew 7:20
#
#   Principle: The demo proves the library works. Fruit validates the tree.
#
#   Anchor: "Prove all things; hold fast that which is good."
#            — 1 Thessalonians 5:21
#
# CPI-SI Identity:
#
#   Component Type: Baton (execution flow - validates and reports)
#   Role: Phase 1 validation demo - proves three-layer architecture
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
#   Purpose: Build demo-trit executable to validate libtrit three-layer arch
#
#   Key Features:
#     - Tests MATTER layer (trit operations)
#     - Tests SPACE layer (pack/unpack, dimensions)
#     - Tests TIME layer (temporal cognition)
#     - Tests Bible Rail (spare states 243-255)
#     - Tests 3D integration (all layers together)
#     - Public API for portability (games, tutorials, SDK examples)
#
#   Philosophy: By their fruits ye shall know them. Demo proves the library.
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
DEMO_NAME = demo-trit

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
#   ├── all → demo-trit
#   ├── run → demo-trit
#   ├── clean → (standalone)
#   └── help → (standalone)
#
#   Build Operations (Middle):
#   └── demo-trit → main.c + libtrit.a
#
# Baton Flow:
#   make → all → demo-trit → exit
#   make run → demo-trit → ./demo-trit → exit
#

# ────────────────────────────────────────────────────────────────
# Build Operations
# ────────────────────────────────────────────────────────────────

## demo-trit: Build the demo executable
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
	@echo "demo-trit - Phase 1 Demo: libtrit Three-Layer Architecture"
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
#   - word/work/pkg/trit/include/ (headers)
#   - word/work/pkg/trit/build/libtrit.a (built library)
#
# Demo Source:
#   - main.c (4-block C executable with public API)
#
# Documentation:
#   - README.adoc (5-block, three-layer architecture)
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
# "By their fruits ye shall know them." — Matthew 7:20
#
# ============================================================================
# END CLOSING
# ============================================================================
