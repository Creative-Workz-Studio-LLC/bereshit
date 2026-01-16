# ============================================================================
# METADATA - Create Documentation Skill Build
# ============================================================================
# Purpose: Build the doc CLI tool
# Biblical: Proverbs 25:2 - "the honour of kings is to search out a matter"

.PHONY: build clean test

# ============================================================================
# SETUP - Variables
# ============================================================================

BINARY = doc
CMD_DIR = cmd/doc
BIN_DIR = bin

# ============================================================================
# BODY - Targets
# ============================================================================

build:
	@mkdir -p $(BIN_DIR)
	go build -o $(BIN_DIR)/$(BINARY) ./$(CMD_DIR)
	@echo "Built $(BIN_DIR)/$(BINARY)"

clean:
	rm -rf $(BIN_DIR)

test:
	go test ./...

# ============================================================================
# CLOSING - Help
# ============================================================================

help:
	@echo "Available targets:"
	@echo "  build  - Build the doc CLI tool"
	@echo "  clean  - Remove build artifacts"
	@echo "  test   - Run tests"
