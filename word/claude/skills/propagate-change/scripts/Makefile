# ═══════════════════════════════════════════════════════════════════════════
# METADATA
# ═══════════════════════════════════════════════════════════════════════════
#
# Key: propagate-change-makefile
# Purpose: Build orchestration for propagate-change skill
# Biblical: Galatians 5:9 - "A little leaven leaveneth the whole lump"
# Authors: Nova Dawn
# Version: 2.0.0
# Created: 2025-12-09
#
# ═══════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════
# SETUP
# ═══════════════════════════════════════════════════════════════════════════

BINARY := propagate
BUILD_DIR := bin
CMD_DIR := cmd/propagate
PKG_DIRS := pkg/chain pkg/config

GO := go
GOFLAGS := -v

# ═══════════════════════════════════════════════════════════════════════════
# BODY
# ═══════════════════════════════════════════════════════════════════════════

.PHONY: all build clean test fmt vet install help

# Default target
all: build

# Build the binary
build:
	@echo "Building $(BINARY)..."
	@mkdir -p $(BUILD_DIR)
	$(GO) build $(GOFLAGS) -o $(BUILD_DIR)/$(BINARY) ./$(CMD_DIR)
	@echo "Built: $(BUILD_DIR)/$(BINARY)"

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete"

# Run tests
test:
	@echo "Running tests..."
	$(GO) test $(GOFLAGS) ./...

# Format code
fmt:
	@echo "Formatting code..."
	$(GO) fmt ./...

# Vet code
vet:
	@echo "Vetting code..."
	$(GO) vet ./...

# Install to GOPATH/bin
install: build
	@echo "Installing $(BINARY)..."
	@cp $(BUILD_DIR)/$(BINARY) $(GOPATH)/bin/ 2>/dev/null || \
		cp $(BUILD_DIR)/$(BINARY) ~/go/bin/
	@echo "Installed"

# ═══════════════════════════════════════════════════════════════════════════
# CLOSING
# ═══════════════════════════════════════════════════════════════════════════

# Help
help:
	@echo "propagate-change skill build targets:"
	@echo ""
	@echo "  make build    - Build the propagate binary"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make test     - Run tests"
	@echo "  make fmt      - Format Go code"
	@echo "  make vet      - Vet Go code"
	@echo "  make install  - Install binary to GOPATH/bin"
	@echo ""
	@echo "Biblical: \"A little leaven leaveneth the whole lump.\" - Galatians 5:9"
