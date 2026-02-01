# =============================================================================
# OmniCode Tools Makefile
# =============================================================================
#
# Builds OmniCode learning system, transpiler, and seed compiler.
#
# Architecture:
#   - omnic (compiler): Uses libomni.a parser - standalone, no graphics
#   - verify (test): Uses libomni.a parser - standalone, no graphics
#   - viewer: requires full MillenniumOS kernel (built separately)
#
# Dependencies:
#   - libomni.a (word/work/pkg/omni) - OmniCode parser, lexer, codegen
#   - libtrit.a (word/work/pkg/trit) - Balanced ternary types
#
# "In the beginning was the Word" — John 1:1
#
# =============================================================================

CC := gcc

# Bereshit root (relative to this Makefile)
BERESHIT_ROOT := ../../..

# Library paths
OMNI_DIR := $(BERESHIT_ROOT)/word/work/pkg/omni
TRIT_DIR := $(BERESHIT_ROOT)/word/work/pkg/trit
OMNI_LIB := $(OMNI_DIR)/build/libomni.a
TRIT_LIB := $(TRIT_DIR)/build/libtrit.a

# Include paths:
#   pkg/omni/include      - OmniCode parser headers (frontend, backend, core)
#   pkg/trit/include      - libtrit headers
#   ../../kernel          - MillenniumOS kernel headers (for viewer only)
#   .                     - Local headers
CFLAGS := -Wall -Wextra -O2 \
	-I$(OMNI_DIR)/include \
	-I$(OMNI_DIR)/include/lang/frontend \
	-I$(OMNI_DIR)/include/lang/backend \
	-I$(OMNI_DIR)/include/lang/core \
	-I$(TRIT_DIR)/include \
	-I../../kernel \
	-I.

# Directories
TRANSPILER_DIR := transpiler
COMPILER_DIR := compiler
BUILD_DIR := build

# =============================================================================
# Sources - Core tools (use libomni for parsing)
# =============================================================================

TRANSPILER_SRC := $(TRANSPILER_DIR)/omni_transpiler.c
COMPILER_SRC := $(COMPILER_DIR)/omnic.c

# =============================================================================
# Objects
# =============================================================================

TRANSPILER_OBJ := $(BUILD_DIR)/omni_transpiler.o
COMPILER_OBJ := $(BUILD_DIR)/omnic.o

# =============================================================================
# Targets
# =============================================================================

OMNIC := $(BUILD_DIR)/omnic
VERIFY := $(BUILD_DIR)/verify

.PHONY: all clean test help deps

all: deps $(OMNIC) $(VERIFY)

# Build dependencies (libomni.a and libtrit.a)
deps: $(OMNI_LIB) $(TRIT_LIB)

$(TRIT_LIB):
	@echo "Building libtrit..."
	@$(MAKE) -C $(TRIT_DIR) libtrit.a

$(OMNI_LIB): $(TRIT_LIB)
	@echo "Building libomni..."
	@$(MAKE) -C $(OMNI_DIR) libomni.a

# Seed compiler - links against libomni.a and libtrit.a
$(OMNIC): $(BUILD_DIR) $(COMPILER_OBJ) $(TRANSPILER_OBJ) $(OMNI_LIB)
	$(CC) $(CFLAGS) -o $@ \
		$(BUILD_DIR)/omnic.o \
		$(BUILD_DIR)/omni_transpiler.o \
		$(OMNI_LIB) $(TRIT_LIB)

# Transpiler verification - links against libomni.a and libtrit.a
$(VERIFY): $(BUILD_DIR) $(BUILD_DIR)/verify.o $(TRANSPILER_OBJ) $(OMNI_LIB)
	$(CC) $(CFLAGS) -o $@ \
		$(BUILD_DIR)/verify.o \
		$(BUILD_DIR)/omni_transpiler.o \
		$(OMNI_LIB) $(TRIT_LIB)

# Build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# =============================================================================
# Compilation Rules
# =============================================================================

$(BUILD_DIR)/omnic.o: $(COMPILER_DIR)/omnic.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/verify.o: $(TRANSPILER_DIR)/verify.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/omni_transpiler.o: $(TRANSPILER_DIR)/omni_transpiler.c
	$(CC) $(CFLAGS) -c -o $@ $<

# =============================================================================
# Test and Verify
# =============================================================================

test: $(VERIFY)
	@echo "=== Running transpiler verification ==="
	./$(VERIFY)

bootstrap-test: $(OMNIC)
	@echo "=== Running bootstrap test ==="
	./$(OMNIC) -v $(COMPILER_DIR)/test/hello.omni -o $(BUILD_DIR)/hello
	@echo "=== Compiling generated C ==="
	$(CC) -o $(BUILD_DIR)/hello $(BUILD_DIR)/hello.c
	@echo "=== Running hello ==="
	./$(BUILD_DIR)/hello

# =============================================================================
# Clean
# =============================================================================

clean:
	rm -rf $(BUILD_DIR)

# =============================================================================
# Help
# =============================================================================

help:
	@echo "OmniCode Tools Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all            Build seed compiler and verification"
	@echo "  test           Run transpiler verification tests"
	@echo "  bootstrap-test Test the bootstrap compilation"
	@echo "  clean          Remove build artifacts"
	@echo ""
	@echo "Components:"
	@echo "  build/omnic    Seed compiler (Stage 0) - standalone"
	@echo "  build/verify   Transpiler verification - standalone"
	@echo ""
	@echo "Note: The viewer requires full MillenniumOS and is built separately."
	@echo ""
	@echo "Bootstrap Process:"
	@echo "  1. make all            # Build seed compiler"
	@echo "  2. make bootstrap-test # Test compilation"
	@echo "  3. (Future) Compile OmniCode compiler in OmniCode"
