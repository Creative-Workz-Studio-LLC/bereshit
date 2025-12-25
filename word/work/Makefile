# ============================================================================
# METADATA
# ============================================================================
# Key: B-word-work-Makefile
# Purpose: Build and run config loader/generator tools
# Biblical: "Except the LORD build the house, they labour in vain" — Psalm 127:1
#
# ============================================================================

# ============================================================================
# SETUP
# ============================================================================

# Paths
BERESHIT_ROOT := $(shell cd ../.. && pwd)
CMD_DIR := cmd
PKG_DIR := pkg
BIN_DIR := bin
GENERATED_DIR := pkg/trit/include/generated

# Go settings
GO := go
GOFLAGS := -v

# Tools
GENERATOR := generate-config
LOADER_TEST := loader-test

# ============================================================================
# BODY
# ============================================================================

.PHONY: all build clean generate test help

# Default target
all: build

# ─────────────────────────────────────────────────────────────────────────────
# Build Targets
# ─────────────────────────────────────────────────────────────────────────────

## build: Build all tools to bin/
build: $(BIN_DIR)/$(GENERATOR)

## $(GENERATOR): Build the config generator
$(BIN_DIR)/$(GENERATOR): $(CMD_DIR)/$(GENERATOR)/main.go $(PKG_DIR)/config/loader.go | $(BIN_DIR)
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Building $(GENERATOR)..."
	@echo "═══════════════════════════════════════════════════════════════"
	$(GO) build $(GOFLAGS) -o $@ ./$(CMD_DIR)/$(GENERATOR)
	@echo "✓ Built: $@"

## $(BIN_DIR): Create bin directory
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# ─────────────────────────────────────────────────────────────────────────────
# Generation Targets
# ─────────────────────────────────────────────────────────────────────────────

## generate: Run the config generator (TOML → C headers)
generate: $(BIN_DIR)/$(GENERATOR)
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Generating C headers from TOML specs..."
	@echo "═══════════════════════════════════════════════════════════════"
	BERESHIT_ROOT=$(BERESHIT_ROOT) ./$(BIN_DIR)/$(GENERATOR)
	@echo ""
	@echo "Generated files:"
	@ls -la $(GENERATED_DIR)/*.gen.h 2>/dev/null || echo "  (none found)"

## generate-clean: Remove generated headers and regenerate
generate-clean: clean-generated generate

# ─────────────────────────────────────────────────────────────────────────────
# Test Targets
# ─────────────────────────────────────────────────────────────────────────────

## test: Run all tests
test: test-loader test-generator

## test-loader: Run loader package tests
test-loader:
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Testing config loader..."
	@echo "═══════════════════════════════════════════════════════════════"
	BERESHIT_ROOT=$(BERESHIT_ROOT) $(GO) test $(GOFLAGS) ./$(PKG_DIR)/config/...

## test-generator: Run generator and verify output
test-generator: generate
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Verifying generated headers..."
	@echo "═══════════════════════════════════════════════════════════════"
	@for f in $(GENERATED_DIR)/*.gen.h; do \
		echo "  Checking: $$f"; \
		if grep -q "#ifndef" "$$f" && grep -q "#endif" "$$f"; then \
			echo "    ✓ Valid header guards"; \
		else \
			echo "    ✗ Missing header guards"; \
			exit 1; \
		fi \
	done
	@echo ""
	@echo "✓ All generated headers valid"

# ─────────────────────────────────────────────────────────────────────────────
# Clean Targets
# ─────────────────────────────────────────────────────────────────────────────

## clean: Remove built binaries
clean:
	@echo "Cleaning binaries..."
	rm -rf $(BIN_DIR)
	@echo "✓ Clean"

## clean-generated: Remove generated headers
clean-generated:
	@echo "Cleaning generated headers..."
	rm -f $(GENERATED_DIR)/*.gen.h
	@echo "✓ Generated headers removed"

## clean-all: Remove everything (binaries + generated)
clean-all: clean clean-generated

# ─────────────────────────────────────────────────────────────────────────────
# Info Targets
# ─────────────────────────────────────────────────────────────────────────────

## status: Show current state
status:
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Config System Status"
	@echo "═══════════════════════════════════════════════════════════════"
	@echo ""
	@echo "BERESHIT_ROOT: $(BERESHIT_ROOT)"
	@echo ""
	@echo "Binary ($(BIN_DIR)/):"
	@if [ -f $(BIN_DIR)/$(GENERATOR) ]; then \
		echo "  ✓ $(GENERATOR) (built)"; \
	else \
		echo "  ○ $(GENERATOR) (not built)"; \
	fi
	@echo ""
	@echo "Generated Headers:"
	@if ls $(GENERATED_DIR)/*.gen.h 1>/dev/null 2>&1; then \
		for f in $(GENERATED_DIR)/*.gen.h; do \
			echo "  ✓ $$(basename $$f)"; \
		done; \
	else \
		echo "  (none)"; \
	fi
	@echo ""
	@echo "TOML Specs with [_generate]:"
	@grep -l "\[_generate\]" $(BERESHIT_ROOT)/word/core/**/*.toml 2>/dev/null | \
		while read f; do echo "  ✓ $$(basename $$f)"; done || echo "  (none found)"

## help: Show this help
help:
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Bereshit Config Tools - Makefile Help"
	@echo "═══════════════════════════════════════════════════════════════"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@grep -E '^## ' $(MAKEFILE_LIST) | sed 's/## /  /'
	@echo ""
	@echo "Examples:"
	@echo "  make build      # Build the generator to bin/"
	@echo "  make generate   # Generate C headers from TOML"
	@echo "  make test       # Run all tests"
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
# "Except the LORD build the house, they labour in vain that build it"
# — Psalm 127:1 KJV
# ============================================================================
