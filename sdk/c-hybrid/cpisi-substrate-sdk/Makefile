# ============================================================================
# METADATA - CPI-SI Substrate SDK Makefile
# ============================================================================
#
# Purpose: Build the Rust-based Substrate SDK shared library
# Key:     B-sdk-substrate-makefile
#
# ============================================================================

.PHONY: all build test clean help cpisi universal ts

# Variables
CARGO := cargo
DENO := deno
# Use ext4 target dir to avoid exFAT cluster overhead and execution issues
TARGET_DIR ?= $(HOME)/.cargo-targets/sdk
LIB_NAME := libcpisi_substrate_sdk.so
RELEASE_DIR := $(TARGET_DIR)/release
DEBUG_DIR := $(TARGET_DIR)/debug

# Default target
all: build

build: cpisi universal ts
	@echo "  Building Rust SDK (Release)..."
	@CARGO_TARGET_DIR=$(TARGET_DIR) $(CARGO) build --release --quiet
	@mkdir -p lib
	@# Strip debug symbols for production grade performance
	@strip $(RELEASE_DIR)/$(LIB_NAME)
	@cp $(RELEASE_DIR)/$(LIB_NAME) lib/
	@echo "    Built and Stripped: lib/$(LIB_NAME)"

cpisi:
	@cd cpisi && $(MAKE) build --no-print-directory

universal:
	@cd universal && $(MAKE) build --no-print-directory

ts:
	@echo "  Compiling SDK Mind Layer (Deno)..."
	@mkdir -p bin
	@cd ts && $(DENO) compile --quiet --allow-all --output ../bin/sabbath mod.ts
	@echo "    Built: bin/sabbath"

install: build
	@echo "  Installing SDK components..."
	@cd cpisi && $(MAKE) install --no-print-directory
	@cd universal && $(MAKE) install --no-print-directory
	@mkdir -p $(HOME)/.local/share/cpisi/bin
	@cp bin/sabbath $(HOME)/.local/share/cpisi/bin/
	@echo "  SDK installation complete."

debug:
	@echo "  Building Rust SDK (Debug)..."
	@CARGO_TARGET_DIR=$(TARGET_DIR) $(CARGO) build --quiet
	@mkdir -p lib
	@cp $(DEBUG_DIR)/$(LIB_NAME) lib/
	@echo "    Built: lib/$(LIB_NAME)"

test:
	@echo "  Testing Rust SDK..."
	@CARGO_TARGET_DIR=$(TARGET_DIR) $(CARGO) test --quiet

clean:
	@echo "  Cleaning Rust SDK..."
	@CARGO_TARGET_DIR=$(TARGET_DIR) $(CARGO) clean
	@rm -rf lib/
	@echo "    Clean complete"

help:
	@echo ""
	@echo "CPI-SI Substrate SDK Build System"
	@echo "--------------------------------"
	@echo "Targets:"
	@echo "  build   - Build release shared library (so)"
	@echo "  debug   - Build debug shared library (so)"
	@echo "  test    - Run unit and integration tests"
	@echo "  clean   - Remove build artifacts"
	@echo ""
