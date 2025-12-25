# ═══════════════════════════════════════════════════════════════════════════
# METADATA
# ═══════════════════════════════════════════════════════════════════════════
#
# Key: create-from-template-makefile
# Purpose: Build orchestration for create-from-template skill
# Biblical: Exodus 25:40 - "Look that thou make them after their pattern"
# Authors: Nova Dawn
# Version: 1.0.0
# Created: 2025-12-09
#
# ═══════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════
# SETUP
# ═══════════════════════════════════════════════════════════════════════════

BINARY := template
BUILD_DIR := bin
CMD_DIR := cmd/template
PKG_DIRS := pkg/config pkg/discovery

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
	@echo "create-from-template skill build targets:"
	@echo ""
	@echo "  make build    - Build the template binary"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make test     - Run tests"
	@echo "  make fmt      - Format Go code"
	@echo "  make vet      - Vet Go code"
	@echo "  make install  - Install binary to GOPATH/bin"
	@echo ""
	@echo "Biblical: \"Look that thou make them after their pattern.\" — Exodus 25:40"
