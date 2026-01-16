# demo-omni-parser Makefile
# Phase 2: OmniCode Parser Validation

BERESHIT := $(shell git rev-parse --show-toplevel 2>/dev/null || echo "/media/seanje-lenox-wise/Project/Bereshit")

# Library locations
OMNI_DIR := $(BERESHIT)/word/work/pkg/omni
TRIT_DIR := $(BERESHIT)/word/work/pkg/trit

# Compiler settings
CC := gcc
CFLAGS := -Wall -Wextra -O2 -I$(OMNI_DIR)/include -I$(TRIT_DIR)/include
LDFLAGS := -L$(OMNI_DIR)/build -L$(TRIT_DIR)/build -lomni -ltrit

# Output
BIN_DIR := bin
TARGET := $(BIN_DIR)/demo-omni-parser

# Test files
TEST_3BLOCK := $(OMNI_DIR)/test/test_3block.omni
TEST_4BLOCK := $(OMNI_DIR)/test/test_4block.omni
TEST_5BLOCK := $(OMNI_DIR)/test/test_5block.omni

.PHONY: all clean test test-all

all: $(TARGET)

$(BIN_DIR):
	mkdir -p $@

$(TARGET): main.c | $(BIN_DIR)
	@echo "  CC    $@"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "✓ Built $@"

test: $(TARGET)
	@echo "Testing with 3-block file..."
	@./$(TARGET) $(TEST_3BLOCK)

test-all: $(TARGET)
	@echo "=== Testing all block patterns ===\n"
	@echo "--- 3-Block (Data) ---"
	@./$(TARGET) $(TEST_3BLOCK) || true
	@echo "\n--- 4-Block (Code) ---"
	@./$(TARGET) $(TEST_4BLOCK) || true
	@echo "\n--- 5-Block (Documentation) ---"
	@./$(TARGET) $(TEST_5BLOCK) || true

clean:
	rm -rf $(BIN_DIR)
	@echo "✓ Clean complete"

help:
	@echo "demo-omni-parser - OmniCode Parser Validation Demo"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build the demo (default)"
	@echo "  test     - Run with 3-block test file"
	@echo "  test-all - Run with all test files"
	@echo "  clean    - Remove build artifacts"
	@echo "  help     - Show this help"
