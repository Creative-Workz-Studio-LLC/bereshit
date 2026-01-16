# ============================================================================
# METADATA - Create Data Skill Build
# ============================================================================
# Purpose: Build the data CLI tool
# Biblical: Proverbs 25:2 - "the honour of kings is to search out a matter"

.PHONY: build clean test

# ============================================================================
# SETUP - Variables
# ============================================================================

BINARY = data
CMD_DIR = cmd/data
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
	@echo "  build  - Build the data CLI tool"
	@echo "  clean  - Remove build artifacts"
	@echo "  test   - Run tests"
