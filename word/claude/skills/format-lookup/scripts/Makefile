# ═══════════════════════════════════════════════════════════════════════════
# METADATA
# ═══════════════════════════════════════════════════════════════════════════
#
# Key: format-lookup-makefile
# Purpose: Build orchestration for format-lookup skill
# Biblical: Ecclesiastes 3:1 - "To every thing there is a season"
# Authors: Nova Dawn
# Version: 2.0.0
# Created: 2025-12-09
#
# ═══════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════
# SETUP
# ═══════════════════════════════════════════════════════════════════════════

MODULE := github.com/creativeworkzstudio/claude-global/skills/format-lookup/scripts
BIN_DIR := bin
CMD_DIR := cmd

# ═══════════════════════════════════════════════════════════════════════════
# BODY
# ═══════════════════════════════════════════════════════════════════════════

.PHONY: all build clean test help

# Default target
all: build

# Build the lookup binary
build:
	@echo "Building lookup..."
	@mkdir -p $(BIN_DIR)
	go build -o $(BIN_DIR)/lookup ./$(CMD_DIR)/lookup
	@echo "Built: $(BIN_DIR)/lookup"

# Clean build artifacts
clean:
	@echo "Cleaning..."
	rm -rf $(BIN_DIR)
	@echo "Clean complete"

# Run tests
test:
	@echo "Running tests..."
	go test ./...

# Format code
fmt:
	@echo "Formatting..."
	gofmt -w .

# Vet code
vet:
	@echo "Vetting..."
	go vet ./...

# Full check
check: fmt vet test

# ═══════════════════════════════════════════════════════════════════════════
# CLOSING
# ═══════════════════════════════════════════════════════════════════════════

help:
	@echo "format-lookup build targets:"
	@echo ""
	@echo "  make build   - Build lookup binary to bin/"
	@echo "  make clean   - Remove build artifacts"
	@echo "  make test    - Run tests"
	@echo "  make fmt     - Format Go code"
	@echo "  make vet     - Run go vet"
	@echo "  make check   - Run fmt, vet, and test"
	@echo ""
	@echo "Usage after build:"
	@echo "  ./bin/lookup --element <text> --format <fmt>  Find mappings"
	@echo "  ./bin/lookup --format <fmt>                   List format mappings"
	@echo "  ./bin/lookup --list                           List all formats"
	@echo ""
	@echo "Examples:"
	@echo "  ./bin/lookup --element 'grounded in' --format go"
	@echo "  ./bin/lookup --format adoc"
	@echo "  ./bin/lookup --list"
	@echo ""
	@echo "Configuration (TOML - edit to add/modify mappings):"
	@echo "  ../config/format-mappings.toml   All format mappings"
	@echo ""
	@echo "Reference documentation:"
	@echo "  ../references/format-mappings.md"
