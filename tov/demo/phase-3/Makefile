# #!omni data --makefile -build
# Phase 3 Demo: OmniCode Full Pipeline Tests
#
# Targets:
#   make all         - Build all tests
#   make test        - Run all tests
#   make clean       - Remove build artifacts

# =============================================================================
# Configuration
# =============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -g -I../../../word/work/pkg/omni/include \
         -I../../../word/work/pkg/trit/include

# Source directories
OMNI_SRC = ../../../word/work/pkg/omni/src
TRIT_SRC = ../../../word/work/pkg/trit/src

# OmniCode library sources
OMNI_SRCS = $(OMNI_SRC)/omni_lexer.c \
            $(OMNI_SRC)/omni_chunks.c \
            $(OMNI_SRC)/omni_parser.c \
            $(OMNI_SRC)/omni_ir.c \
            $(OMNI_SRC)/omni_ir_emit.c \
            $(OMNI_SRC)/omni_vm.c \
            $(OMNI_SRC)/omni_lib.c \
            $(OMNI_SRC)/omni_dar.c \
            $(OMNI_SRC)/omni_debug.c \
            $(OMNI_SRC)/omni_runtime.c

# Trit config sources (for bible lookups)
TRIT_SRCS = $(TRIT_SRC)/config.c \
            $(TRIT_SRC)/toml.c

# Build directory
BUILD = build

# =============================================================================
# Targets
# =============================================================================

.PHONY: all test clean help test-omni

all: $(BUILD)/test_omni_to_vm $(BUILD)/test_stdlib_parse $(BUILD)/test_cpisi_pipeline $(BUILD)/test_lib_loader $(BUILD)/test_dar $(BUILD)/test_debug $(BUILD)/test_intrinsics $(BUILD)/omni_runner

# Build directory
$(BUILD):
	mkdir -p $(BUILD)

# Full pipeline test (parse → IR → VM)
$(BUILD)/test_omni_to_vm: test_omni_to_vm.c $(OMNI_SRCS) $(TRIT_SRCS) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^

# Stdlib parsing test
$(BUILD)/test_stdlib_parse: test_stdlib_parse.c $(OMNI_SRCS) $(TRIT_SRCS) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^

# CPI-SI integration test
$(BUILD)/test_cpisi_pipeline: test_cpisi_pipeline.c $(OMNI_SRCS) $(TRIT_SRCS) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^

# Library loader test
$(BUILD)/test_lib_loader: test_lib_loader.c $(OMNI_SRCS) $(TRIT_SRCS) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^

# DAR system test
$(BUILD)/test_dar: test_dar.c $(OMNI_SRCS) $(TRIT_SRCS) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^

# Debug system test
$(BUILD)/test_debug: test_debug.c $(OMNI_SRCS) $(TRIT_SRCS) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^

# VM intrinsics test
$(BUILD)/test_intrinsics: test_intrinsics.c $(OMNI_SRCS) $(TRIT_SRCS) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^

# OmniCode runner (executes .omni files)
$(BUILD)/omni_runner: omni_runner.c $(OMNI_SRCS) $(TRIT_SRCS) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^

# Run all tests
test: all
	@echo "=== Running Phase 3 Tests ==="
	@echo ""
	@echo "--- Test 1: Full Pipeline (OmniCode → IR → VM) ---"
	./$(BUILD)/test_omni_to_vm || exit 1
	@echo ""
	@echo "--- Test 2: Stdlib Parsing ---"
	./$(BUILD)/test_stdlib_parse || exit 1
	@echo ""
	@echo "--- Test 3: CPI-SI Pipeline Integration ---"
	./$(BUILD)/test_cpisi_pipeline || exit 1
	@echo ""
	@echo "--- Test 4: Library Loader ---"
	./$(BUILD)/test_lib_loader || exit 1
	@echo ""
	@echo "--- Test 5: DAR System ---"
	./$(BUILD)/test_dar || exit 1
	@echo ""
	@echo "--- Test 6: Debug System ---"
	./$(BUILD)/test_debug || exit 1
	@echo ""
	@echo "--- Test 7: VM Intrinsics ---"
	./$(BUILD)/test_intrinsics || exit 1
	@echo ""
	@echo "=== All Phase 3 Tests Passed ==="

# Run .omni test files
test-omni: $(BUILD)/omni_runner
	@echo "=== Running OmniCode Test Suite ==="
	@echo ""
	@for f in tests/*.omni; do \
		echo "--- Running $$f ---"; \
		./$(BUILD)/omni_runner "$$f" || exit 1; \
		echo ""; \
	done
	@echo "=== All OmniCode Tests Passed ==="

clean:
	rm -rf $(BUILD)

help:
	@echo "Phase 3 Demo Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all     - Build all test executables"
	@echo "  test    - Build and run all tests"
	@echo "  clean   - Remove build artifacts"
	@echo "  help    - Show this help"
