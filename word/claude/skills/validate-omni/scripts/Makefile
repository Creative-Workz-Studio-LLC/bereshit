# ═══════════════════════════════════════════════════════════════════════════
# METADATA
# ═══════════════════════════════════════════════════════════════════════════
#
# Key: validate-omni-makefile
# Purpose: Build orchestration for validate-omni skill
# Biblical: Ecclesiastes 3:3 - "A time to break down, and a time to build up"
# Authors: Nova Dawn
# Version: 1.0.0
# Created: 2025-12-08
#
# ═══════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════
# SETUP
# ═══════════════════════════════════════════════════════════════════════════

MODULE := github.com/creativeworkzstudio/claude-global/skills/validate-omni/scripts
BIN_DIR := bin
CMD_DIR := cmd

# ═══════════════════════════════════════════════════════════════════════════
# BODY
# ═══════════════════════════════════════════════════════════════════════════

.PHONY: all build clean test help

# Default target
all: build

# Build the validate binary
build:
	@echo "Building validate..."
	@mkdir -p $(BIN_DIR)
	go build -o $(BIN_DIR)/validate ./$(CMD_DIR)/validate
	@echo "Built: $(BIN_DIR)/validate"

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
	@echo "validate-omni build targets:"
	@echo ""
	@echo "  make build   - Build validate binary to bin/"
	@echo "  make clean   - Remove build artifacts"
	@echo "  make test    - Run tests"
	@echo "  make fmt     - Format Go code"
	@echo "  make vet     - Run go vet"
	@echo "  make check   - Run fmt, vet, and test"
	@echo ""
	@echo "Usage after build:"
	@echo "  ./bin/validate <file.omni>           Full validation"
	@echo "  ./bin/validate pragma <file.omni>    Pragma check only"
	@echo "  ./bin/validate blocks <file.omni>    Block structure only"
	@echo "  ./bin/validate sections <file.omni>  Required sections only"
	@echo ""
	@echo "Configuration (TOML - edit to change validation rules):"
	@echo "  ../config/pragma-types.toml       Valid pragma types and formats"
	@echo "  ../config/block-structures.toml   Block patterns by type"
	@echo "  ../config/required-sections.toml  Required METADATA sections"
	@echo ""
	@echo "Reference documentation:"
	@echo "  ../references/pragma-types.md"
	@echo "  ../references/block-structures.md"
	@echo "  ../references/required-sections.md"
