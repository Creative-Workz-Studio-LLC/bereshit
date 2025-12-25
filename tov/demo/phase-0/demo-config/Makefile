# ============================================================================
# METADATA
# ============================================================================
# Key: B-tov-demo-phase0-demo-config-Makefile
# Purpose: Build and run Phase 0 config demonstration
# Biblical: "Prove all things; hold fast that which is good" — 1 Thessalonians 5:21
#
# ============================================================================

# ============================================================================
# SETUP
# ============================================================================

# Paths
BERESHIT_ROOT := $(shell cd ../../../.. && pwd)
BIN_DIR := bin
DOCS_DIR := docs

# Go settings
GO := go
GOFLAGS := -v

# Binary
BINARY := demo-config

# ============================================================================
# BODY
# ============================================================================

.PHONY: all build clean run test diagnostic status help

# Default target
all: build

# ─────────────────────────────────────────────────────────────────────────────
# Build Targets
# ─────────────────────────────────────────────────────────────────────────────

## build: Build demo-config binary to bin/
build: $(BIN_DIR)/$(BINARY)

## $(BINARY): Build the demo binary
$(BIN_DIR)/$(BINARY): main.go | $(BIN_DIR)
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Building $(BINARY)..."
	@echo "═══════════════════════════════════════════════════════════════"
	$(GO) build $(GOFLAGS) -o $@ .
	@echo "✓ Built: $@"

## $(BIN_DIR): Create bin directory
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# ─────────────────────────────────────────────────────────────────────────────
# Run Targets
# ─────────────────────────────────────────────────────────────────────────────

## run: Build and run the demo
run: build
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Running Phase 0 Demo..."
	@echo "═══════════════════════════════════════════════════════════════"
	BERESHIT_ROOT=$(BERESHIT_ROOT) ./$(BIN_DIR)/$(BINARY)

## run-verbose: Run with verbose output (shows BERESHIT_ROOT)
run-verbose: build
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Running Phase 0 Demo (verbose)..."
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "BERESHIT_ROOT: $(BERESHIT_ROOT)"
	@echo ""
	BERESHIT_ROOT=$(BERESHIT_ROOT) ./$(BIN_DIR)/$(BINARY)

# ─────────────────────────────────────────────────────────────────────────────
# Test Targets
# ─────────────────────────────────────────────────────────────────────────────

## test: Run demo and verify exit code
test: build
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Testing Phase 0 Demo..."
	@echo "═══════════════════════════════════════════════════════════════"
	@BERESHIT_ROOT=$(BERESHIT_ROOT) ./$(BIN_DIR)/$(BINARY) && \
		echo "" && echo "✓ Demo PASSED (exit 0)" || \
		(echo "" && echo "✗ Demo FAILED (exit $$?)" && exit 1)

## diagnostic: Run loader diagnostic (comprehensive debug)
diagnostic:
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Running Config Loader Diagnostic..."
	@echo "═══════════════════════════════════════════════════════════════"
	BERESHIT_ROOT=$(BERESHIT_ROOT) $(GO) test -v -run TestDiagnostic $(BERESHIT_ROOT)/word/work/pkg/config/...

# ─────────────────────────────────────────────────────────────────────────────
# Clean Targets
# ─────────────────────────────────────────────────────────────────────────────

## clean: Remove built binary
clean:
	@echo "Cleaning..."
	rm -rf $(BIN_DIR)
	@echo "✓ Clean"

# ─────────────────────────────────────────────────────────────────────────────
# Info Targets
# ─────────────────────────────────────────────────────────────────────────────

## status: Show current state
status:
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Phase 0 Demo Status"
	@echo "═══════════════════════════════════════════════════════════════"
	@echo ""
	@echo "BERESHIT_ROOT: $(BERESHIT_ROOT)"
	@echo ""
	@echo "Binary:"
	@if [ -f $(BIN_DIR)/$(BINARY) ]; then \
		echo "  ✓ $(BIN_DIR)/$(BINARY) (built)"; \
	else \
		echo "  ○ $(BIN_DIR)/$(BINARY) (not built)"; \
	fi
	@echo ""
	@echo "Source:"
	@ls -1 *.go 2>/dev/null | while read f; do echo "  ✓ $$f"; done
	@echo ""
	@echo "Documentation:"
	@ls -1 $(DOCS_DIR)/*.adoc 2>/dev/null | while read f; do echo "  ✓ $$f"; done || echo "  (none in docs/)"

## help: Show this help
help:
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Phase 0 Demo - Makefile Help"
	@echo "═══════════════════════════════════════════════════════════════"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@grep -E '^## ' $(MAKEFILE_LIST) | sed 's/## /  /'
	@echo ""
	@echo "Examples:"
	@echo "  make build      # Build to bin/demo-config"
	@echo "  make run        # Build and run demo"
	@echo "  make test       # Run and verify exit code"
	@echo "  make diagnostic # Run comprehensive loader diagnostic"
	@echo "  make status     # Show current state"
	@echo "  make clean      # Remove bin/ directory"
	@echo ""

# ============================================================================
# CLOSING
# ============================================================================
# Makefile structure follows CPI-SI patterns:
# - METADATA: Purpose and context
# - SETUP: Variables and configuration
# - BODY: Targets and rules
# - CLOSING: Documentation
#
# Chain: word/work/Makefile (template) → this Makefile (adapted)
#
# "Prove all things; hold fast that which is good"
# — 1 Thessalonians 5:21 KJV
# ============================================================================
