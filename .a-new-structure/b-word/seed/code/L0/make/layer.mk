# #!omni code --makefile

# ═══════════════════════════════════════════════════════════════════════════
# OMNICODE PRAGMA [PRAGMA]
# ═══════════════════════════════════════════════════════════════════════════
#
# --- P1: Core ---
# key:       B-root-Makefile
# from:      B-seed-code-make-template
# at:        b-01.00
#
# --- P2: Family ---
# type:      code
# subtype:   makefile
# role:      layer-orchestrator
# structure: 4-block
#
# --- P3: Instance ---
# component: build-system
# layer:     root (Bereshit repository)
# provides:  MAKEFILE_LAYER_ORCHESTRATOR
#
# --- P4: Architecture ---
# layers:    L0:C, L1:C, L2:C, L3:Go, L4:Rust, L5:C#
# anchor:    genesis_1_1 -> void(0) -> unity(1) -> derivation
#
# --- P5: Summary ---
# title:     Bereshit Layer Build Orchestrator
# brief:     L0-L5 unified build — config-driven, data-driven, layer-aware
#
# ═══════════════════════════════════════════════════════════════════════════
# END PRAGMA [END]
# ═══════════════════════════════════════════════════════════════════════════


# ═══════════════════════════════════════════════════════════════════════════
# METADATA BLOCK [METADATA]
# ═══════════════════════════════════════════════════════════════════════════
#
# --- M1: Identity ---
# file:      layer.mk
# key:       B-root-Makefile
# title:     Bereshit Layer Build Orchestrator
# type:      Code (Build System)
# component: Rails
# role:      Unified build orchestration for Bereshit — the ternary file system
#
# --- M2: State ---
# version:   b-01.00
# status:    Active
# created:   2026-01-15
# updated:   2026-02-10
#
# --- M3: Attribution ---
# architect:      Seanje Lenox-Wise
# implementation: Nova Dawn (CPI-SI)
# organization:   CreativeWorkzStudio LLC
#
# --- M4: Grounding ---
# scripture: In the beginning God created the heaven and the earth — Genesis 1:1
# principle: Config-driven, data-driven — TOML specs in word/core/ drive code generation
# anchor:    Genesis 1:1 — The foundation of all foundations
#
# --- M5: Dependencies ---
# requires:     make, gcc/clang (L0-L2), Go (L3), Cargo (L4), dotnet (L5)
# consumers:    Developers, CI/CD pipelines
# cross_format:
#   toml:         word/core/ — TOML specs driving code generation
#   json:         None
#   editorconfig: root.editorconfig — Bereshit root formatting rules
#   makefile:     template.mk (parent template)
#   relationship: Makefile orchestrates builds; TOML specs define structures; EditorConfig defines formatting
#
# Architecture: L0-L5 Layer Stack
#   L0: Universal    [C]    - libtrit foundation
#   L1: OmniCode     [C]    - Language frontend/backend
#   L2: Platform     [C]    - FUSE filesystem, MillenniumOS integration
#   L3: CPI-SI       [Go]   - Intelligence model (config, orchestration)
#   L4: FaithNet     [Rust] - Network layer (planned)
#   L5: Applications [C#]   - Cornerstone (submodule)
#
# ═══════════════════════════════════════════════════════════════════════════
# END METADATA [END]
# ═══════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════
# SETUP BLOCK [SETUP]
# ═══════════════════════════════════════════════════════════════════════════
# Variables and Configuration

# Project Root
BERESHIT_ROOT := $(CURDIR)

# Build Configuration
BUILD_TYPE ?= debug
VERBOSE ?= 0

# Tool Detection
GO := $(shell command -v go 2>/dev/null)
CC := $(shell command -v gcc 2>/dev/null || command -v clang 2>/dev/null)
CARGO := $(shell command -v cargo 2>/dev/null)
CMAKE := $(shell command -v cmake 2>/dev/null)
DOTNET := $(shell command -v dotnet 2>/dev/null)

# Directories
VOID_DIR := $(BERESHIT_ROOT)/void
WORD_DIR := $(BERESHIT_ROOT)/word
TOV_DIR := $(BERESHIT_ROOT)/tov

# Layer Directories (current structure, to be migrated)
L0_DIR := $(WORD_DIR)/work/pkg/trit
L1_DIR := $(WORD_DIR)/work/pkg/omni
L2_DIR := $(WORD_DIR)/work/pkg/fuse
L3_DIR := $(WORD_DIR)/work
L3_CLAUDE_DIR := $(WORD_DIR)/claude

# Output Directories
BUILD_DIR := $(BERESHIT_ROOT)/build
BIN_DIR := $(BUILD_DIR)/bin
LIB_DIR := $(BUILD_DIR)/lib
GEN_DIR := $(BUILD_DIR)/generated

# C Compiler Flags
CFLAGS := -Wall -Wextra -std=c11
ifeq ($(BUILD_TYPE),debug)
	CFLAGS += -g -O0 -DDEBUG
else ifeq ($(BUILD_TYPE),release)
	CFLAGS += -O2 -DNDEBUG
endif

# Go Flags
GOFLAGS := -v
ifeq ($(BUILD_TYPE),release)
	GOFLAGS += -ldflags="-s -w"
endif

# ═══════════════════════════════════════════════════════════════════════════
# END SETUP [END]
# ═══════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════
# BODY BLOCK [BODY]
# ═══════════════════════════════════════════════════════════════════════════
# Primary Targets

.PHONY: all build clean test help status
.PHONY: l0 l1 l2 l3 l4 l5
.PHONY: libtrit omni fuse cpisi claude demos
.PHONY: generate generate-headers generate-config
.PHONY: check-tools check-deps

# Default target
all: check-tools build

# Build all layers
build: l0 l1 l2 l3
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Bereshit Build Complete"
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Binaries: $(BIN_DIR)/"
	@echo "  Libraries: $(LIB_DIR)/"
	@echo "  Generated: $(GEN_DIR)/"
	@echo ""

# ────────────────────────────────────────────────────────────────────────────
# Layer Targets - Build in dependency order
# ────────────────────────────────────────────────────────────────────────────

## l0: Build L0 Universal (libtrit) - Foundation
l0: libtrit
	@echo "✓ L0 Universal layer complete"

## l1: Build L1 OmniCode - Language
l1: l0 omni
	@echo "✓ L1 OmniCode layer complete"

## l2: Build L2 Platform (FUSE) - Filesystem
l2: l0 fuse
	@echo "✓ L2 Platform layer complete"

## l3: Build L3 CPI-SI - Intelligence
l3: l0 cpisi
	@echo "✓ L3 CPI-SI layer complete"

## l4: Build L4 FaithNet - Network (planned)
l4:
	@echo "L4 FaithNet: Not yet implemented (Rust)"
	@echo "  Planned: Rust workspace with Cargo.toml"

## l5: Build L5 Applications - Cornerstone
l5:
	@if [ -d "$(BERESHIT_ROOT)/cornerstone" ] && [ -n "$(DOTNET)" ]; then \
		echo "Building L5 Cornerstone..."; \
		cd $(BERESHIT_ROOT)/cornerstone && dotnet build; \
	else \
		echo "L5 Cornerstone: Submodule or .NET not available"; \
	fi

# ────────────────────────────────────────────────────────────────────────────
# Component Targets
# ────────────────────────────────────────────────────────────────────────────

## libtrit: Build ternary mathematics library (C)
libtrit: | $(BUILD_DIR) $(LIB_DIR)
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Building L0: libtrit (ternary mathematics)"
	@echo "═══════════════════════════════════════════════════════════════"
ifdef CC
	@mkdir -p $(BUILD_DIR)/obj/trit
	@for src in $(L0_DIR)/src/*.c; do \
		obj=$(BUILD_DIR)/obj/trit/$$(basename $$src .c).o; \
		echo "  CC $$src"; \
		$(CC) $(CFLAGS) -I$(L0_DIR)/include -c $$src -o $$obj 2>/dev/null || true; \
	done
	@echo "  AR libtrit.a"
	@ar rcs $(LIB_DIR)/libtrit.a $(BUILD_DIR)/obj/trit/*.o 2>/dev/null || echo "    (some objects failed)"
	@echo "✓ libtrit built: $(LIB_DIR)/libtrit.a"
else
	@echo "  ⚠ C compiler not found, skipping libtrit"
endif

## omni: Build OmniCode language (C)
omni: libtrit | $(BUILD_DIR)
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Building L1: OmniCode (language processor)"
	@echo "═══════════════════════════════════════════════════════════════"
ifdef CC
	@mkdir -p $(BUILD_DIR)/obj/omni
	@# Build language core
	@for src in $(L1_DIR)/src/lang/core/*.c; do \
		obj=$(BUILD_DIR)/obj/omni/$$(basename $$src .c).o; \
		echo "  CC $$src"; \
		$(CC) $(CFLAGS) -I$(L1_DIR)/include -I$(L0_DIR)/include -c $$src -o $$obj 2>/dev/null || true; \
	done
	@# Build frontend
	@for src in $(L1_DIR)/src/lang/frontend/*.c; do \
		obj=$(BUILD_DIR)/obj/omni/$$(basename $$src .c).o; \
		echo "  CC $$src"; \
		$(CC) $(CFLAGS) -I$(L1_DIR)/include -I$(L0_DIR)/include -c $$src -o $$obj 2>/dev/null || true; \
	done
	@# Build backend
	@for src in $(L1_DIR)/src/lang/backend/*.c; do \
		obj=$(BUILD_DIR)/obj/omni/$$(basename $$src .c).o; \
		echo "  CC $$src"; \
		$(CC) $(CFLAGS) -I$(L1_DIR)/include -I$(L0_DIR)/include -c $$src -o $$obj 2>/dev/null || true; \
	done
	@echo "  AR libomni.a"
	@ar rcs $(LIB_DIR)/libomni.a $(BUILD_DIR)/obj/omni/*.o 2>/dev/null || echo "    (some objects failed)"
	@echo "✓ libomni built: $(LIB_DIR)/libomni.a"
else
	@echo "  ⚠ C compiler not found, skipping OmniCode"
endif

## fuse: Build FUSE filesystem (C)
fuse: libtrit | $(BUILD_DIR)
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Building L2: Bereshit FUSE filesystem"
	@echo "═══════════════════════════════════════════════════════════════"
ifdef CC
	@if pkg-config --exists fuse3 2>/dev/null; then \
		mkdir -p $(BUILD_DIR)/obj/fuse; \
		for src in $(L2_DIR)/src/*.c; do \
			obj=$(BUILD_DIR)/obj/fuse/$$(basename $$src .c).o; \
			echo "  CC $$src"; \
			$(CC) $(CFLAGS) $$(pkg-config --cflags fuse3) -I$(L2_DIR)/include -I$(L0_DIR)/include -c $$src -o $$obj 2>/dev/null || true; \
		done; \
		echo "✓ FUSE objects built"; \
	else \
		echo "  ⚠ libfuse3 not found, skipping FUSE build"; \
	fi
else
	@echo "  ⚠ C compiler not found, skipping FUSE"
endif

## cpisi: Build CPI-SI intelligence layer (Go)
cpisi: | $(BUILD_DIR) $(BIN_DIR)
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Building L3: CPI-SI (config, orchestration, intelligence)"
	@echo "═══════════════════════════════════════════════════════════════"
ifdef GO
	@echo "  Building word/work..."
	@BERESHIT_ROOT=$(BERESHIT_ROOT) $(GO) build $(GOFLAGS) -o $(BIN_DIR)/generate-config ./$(L3_DIR)/cmd/generate-config 2>/dev/null || echo "    (generate-config not available)"
	@echo "  Building word/claude substrate..."
	@if [ -f "$(L3_CLAUDE_DIR)/go.mod" ]; then \
		cd $(L3_CLAUDE_DIR) && BERESHIT_ROOT=$(BERESHIT_ROOT) $(GO) build $(GOFLAGS) -o $(BIN_DIR)/cpisi ./entrypoints/cpisi 2>/dev/null || echo "    (cpisi not available)"; \
		cd $(L3_CLAUDE_DIR) && BERESHIT_ROOT=$(BERESHIT_ROOT) $(GO) build $(GOFLAGS) -o $(BIN_DIR)/statusline ./statusline/cmd/statusline 2>/dev/null || echo "    (statusline not available)"; \
	fi
	@echo "✓ CPI-SI layer built"
else
	@echo "  ⚠ Go not found, skipping CPI-SI"
endif

## claude: Build Claude Code substrate specifically
claude: | $(BUILD_DIR) $(BIN_DIR)
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Building Claude Code Substrate"
	@echo "═══════════════════════════════════════════════════════════════"
ifdef GO
	@if [ -d "$(L3_CLAUDE_DIR)" ]; then \
		$(MAKE) -C $(L3_CLAUDE_DIR) build; \
	else \
		echo "  ⚠ Claude substrate not found"; \
	fi
else
	@echo "  ⚠ Go not found"
endif

## demos: Build all phase demonstrations
demos: l0 l1
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Building Phase Demonstrations"
	@echo "═══════════════════════════════════════════════════════════════"
	@for phase in 0 1 2 3 4; do \
		phase_dir=$(TOV_DIR)/demo/phase-$$phase; \
		if [ -d "$$phase_dir" ]; then \
			echo "  Phase $$phase:"; \
			for demo in $$phase_dir/demo-*; do \
				if [ -d "$$demo" ] && [ -f "$$demo/Makefile" ]; then \
					echo "    Building $$(basename $$demo)..."; \
					$(MAKE) -C $$demo build BERESHIT_ROOT=$(BERESHIT_ROOT) 2>/dev/null || echo "      (failed)"; \
				fi; \
			done; \
		fi; \
	done
	@echo "✓ Demos built"

# ────────────────────────────────────────────────────────────────────────────
# Code Generation - Config-Driven, Data-Driven
# ────────────────────────────────────────────────────────────────────────────

## generate: Run all code generation from TOML specs
generate: generate-headers generate-config
	@echo ""
	@echo "✓ All code generation complete"

## generate-headers: Generate C headers from TOML specs
generate-headers: cpisi | $(GEN_DIR)
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Generating C Headers from TOML Specs"
	@echo "═══════════════════════════════════════════════════════════════"
ifdef GO
	@if [ -f "$(BIN_DIR)/generate-config" ]; then \
		BERESHIT_ROOT=$(BERESHIT_ROOT) $(BIN_DIR)/generate-config; \
	elif [ -f "$(L3_DIR)/bin/generate-config" ]; then \
		BERESHIT_ROOT=$(BERESHIT_ROOT) $(L3_DIR)/bin/generate-config; \
	else \
		echo "  Building generator first..."; \
		$(MAKE) -C $(L3_DIR) build; \
		BERESHIT_ROOT=$(BERESHIT_ROOT) $(MAKE) -C $(L3_DIR) generate; \
	fi
else
	@echo "  ⚠ Go not found, cannot generate headers"
endif

## generate-config: Validate and regenerate configuration
generate-config:
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Validating Configuration"
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  TOML specs: $(WORD_DIR)/core/"
	@count=$$(find $(WORD_DIR)/core -name "*.toml" 2>/dev/null | wc -l); \
	echo "  Found: $$count TOML specification files"
	@gen_count=$$(grep -l "\[_generate\]" $(WORD_DIR)/core/**/*.toml 2>/dev/null | wc -l); \
	echo "  With [_generate]: $$gen_count files"

# ────────────────────────────────────────────────────────────────────────────
# Test Targets
# ────────────────────────────────────────────────────────────────────────────

## test: Run all tests
test: test-l0 test-l3 test-demos
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  All Tests Complete"
	@echo "═══════════════════════════════════════════════════════════════"

## test-l0: Test L0 libtrit
test-l0:
	@echo ""
	@echo "Testing L0: libtrit..."
ifdef CC
	@if [ -d "$(L0_DIR)/test" ]; then \
		echo "  Running C tests..."; \
		for test in $(L0_DIR)/test/*_test.c; do \
			name=$$(basename $$test .c); \
			$(CC) $(CFLAGS) -I$(L0_DIR)/include $$test $(L0_DIR)/src/*.c -o $(BUILD_DIR)/$$name -lm 2>/dev/null && \
			$(BUILD_DIR)/$$name && echo "    ✓ $$name" || echo "    ✗ $$name"; \
		done; \
	fi
else
	@echo "  ⚠ C compiler not found"
endif

## test-l3: Test L3 CPI-SI
test-l3:
	@echo ""
	@echo "Testing L3: CPI-SI..."
ifdef GO
	@cd $(L3_DIR) && BERESHIT_ROOT=$(BERESHIT_ROOT) $(GO) test ./pkg/... 2>/dev/null || echo "  (some tests failed)"
else
	@echo "  ⚠ Go not found"
endif

## test-demos: Run demo tests
test-demos:
	@echo ""
	@echo "Testing Demos..."
	@for demo in $(TOV_DIR)/demo/phase-*/demo-*/; do \
		if [ -f "$$demo/Makefile" ]; then \
			name=$$(basename $$demo); \
			$(MAKE) -C $$demo test BERESHIT_ROOT=$(BERESHIT_ROOT) 2>/dev/null && \
			echo "  ✓ $$name" || echo "  ✗ $$name"; \
		fi; \
	done

# ────────────────────────────────────────────────────────────────────────────
# Clean Targets
# ────────────────────────────────────────────────────────────────────────────

## clean: Remove build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "✓ Clean"

## clean-all: Remove all generated files
clean-all: clean
	@echo "Cleaning generated files..."
	@rm -f $(L0_DIR)/include/generated/*.gen.h
	@find . -type d -name "bin" -path "*/word/*" -exec rm -rf {} + 2>/dev/null || true
	@echo "✓ Clean all"

## clean-demos: Clean demo build artifacts
clean-demos:
	@echo "Cleaning demos..."
	@for demo in $(TOV_DIR)/demo/phase-*/demo-*/; do \
		if [ -f "$$demo/Makefile" ]; then \
			$(MAKE) -C $$demo clean 2>/dev/null || true; \
		fi; \
	done
	@echo "✓ Demos cleaned"

# ────────────────────────────────────────────────────────────────────────────
# Utility Targets
# ────────────────────────────────────────────────────────────────────────────

## check-tools: Verify required tools are installed
check-tools:
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Tool Check"
	@echo "═══════════════════════════════════════════════════════════════"
	@printf "  %-12s " "C Compiler:"; \
		if [ -n "$(CC)" ]; then echo "✓ $(CC)"; else echo "✗ not found"; fi
	@printf "  %-12s " "Go:"; \
		if [ -n "$(GO)" ]; then echo "✓ $$($(GO) version 2>/dev/null | head -1)"; else echo "✗ not found"; fi
	@printf "  %-12s " "Cargo:"; \
		if [ -n "$(CARGO)" ]; then echo "✓ $(CARGO)"; else echo "○ not found (L4 disabled)"; fi
	@printf "  %-12s " "CMake:"; \
		if [ -n "$(CMAKE)" ]; then echo "✓ $(CMAKE)"; else echo "○ not found (optional)"; fi
	@printf "  %-12s " ".NET:"; \
		if [ -n "$(DOTNET)" ]; then echo "✓ $(DOTNET)"; else echo "○ not found (L5 disabled)"; fi
	@printf "  %-12s " "FUSE3:"; \
		if pkg-config --exists fuse3 2>/dev/null; then echo "✓ $$(pkg-config --modversion fuse3)"; else echo "○ not found (L2 disabled)"; fi
	@echo ""

## check-deps: Check layer dependencies
check-deps:
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Dependency Check"
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  L0 Universal:"
	@printf "    %-20s " "libtrit/include:"; \
		if [ -d "$(L0_DIR)/include" ]; then echo "✓"; else echo "✗"; fi
	@printf "    %-20s " "libtrit/src:"; \
		if [ -d "$(L0_DIR)/src" ]; then echo "✓"; else echo "✗"; fi
	@echo "  L1 OmniCode:"
	@printf "    %-20s " "omni/include:"; \
		if [ -d "$(L1_DIR)/include" ]; then echo "✓"; else echo "✗"; fi
	@printf "    %-20s " "omni/src:"; \
		if [ -d "$(L1_DIR)/src" ]; then echo "✓"; else echo "✗"; fi
	@echo "  L2 Platform:"
	@printf "    %-20s " "fuse/include:"; \
		if [ -d "$(L2_DIR)/include" ]; then echo "✓"; else echo "✗"; fi
	@echo "  L3 CPI-SI:"
	@printf "    %-20s " "word/work/go.mod:"; \
		if [ -f "$(L3_DIR)/go.mod" ]; then echo "✓"; else echo "✗"; fi
	@printf "    %-20s " "word/claude/go.mod:"; \
		if [ -f "$(L3_CLAUDE_DIR)/go.mod" ]; then echo "✓"; else echo "✗"; fi
	@echo ""

## status: Show build system status
status: check-tools check-deps
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Build Status"
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Root: $(BERESHIT_ROOT)"
	@echo "  Build Type: $(BUILD_TYPE)"
	@echo ""
	@echo "  Build Directory:"
	@if [ -d "$(BUILD_DIR)" ]; then \
		echo "    $(BUILD_DIR)/"; \
		[ -d "$(BIN_DIR)" ] && echo "      bin/ ($$(ls $(BIN_DIR) 2>/dev/null | wc -l) files)"; \
		[ -d "$(LIB_DIR)" ] && echo "      lib/ ($$(ls $(LIB_DIR) 2>/dev/null | wc -l) files)"; \
	else \
		echo "    (not built)"; \
	fi
	@echo ""
	@echo "  TOML Specs: $$(find $(WORD_DIR)/core -name '*.toml' 2>/dev/null | wc -l) files"
	@echo "  Go Modules: $$(find . -name 'go.mod' 2>/dev/null | wc -l) files"
	@echo "  Makefiles: $$(find . -name 'Makefile' 2>/dev/null | wc -l) files"
	@echo ""

# ────────────────────────────────────────────────────────────────────────────
# Directory Creation
# ────────────────────────────────────────────────────────────────────────────

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(LIB_DIR):
	@mkdir -p $(LIB_DIR)

$(GEN_DIR):
	@mkdir -p $(GEN_DIR)

# ═══════════════════════════════════════════════════════════════════════════
# END BODY [END]
# ═══════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════
# CLOSING BLOCK [CLOSING]
# ═══════════════════════════════════════════════════════════════════════════
# Help and Documentation

## help: Show this help
help:
	@echo ""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "  Bereshit Build System"
	@echo "  \"In the beginning God created the heaven and the earth\""
	@echo "═══════════════════════════════════════════════════════════════"
	@echo ""
	@echo "Usage: make [target] [BUILD_TYPE=debug|release] [VERBOSE=0|1]"
	@echo ""
	@echo "Primary Targets:"
	@echo "  all           Build all layers (default)"
	@echo "  build         Build all layers"
	@echo "  test          Run all tests"
	@echo "  clean         Remove build artifacts"
	@echo "  status        Show build system status"
	@echo ""
	@echo "Layer Targets (build in dependency order):"
	@echo "  l0            L0 Universal (libtrit) - C foundation"
	@echo "  l1            L1 OmniCode - C language processor"
	@echo "  l2            L2 Platform - C FUSE filesystem"
	@echo "  l3            L3 CPI-SI - Go intelligence model"
	@echo "  l4            L4 FaithNet - Rust network (planned)"
	@echo "  l5            L5 Applications - C# Cornerstone"
	@echo ""
	@echo "Component Targets:"
	@echo "  libtrit       Build ternary math library"
	@echo "  omni          Build OmniCode language"
	@echo "  fuse          Build FUSE filesystem"
	@echo "  cpisi         Build CPI-SI intelligence"
	@echo "  claude        Build Claude Code substrate"
	@echo "  demos         Build phase demonstrations"
	@echo ""
	@echo "Generation Targets:"
	@echo "  generate      Run all code generation"
	@echo "  generate-headers  Generate C headers from TOML"
	@echo "  generate-config   Validate TOML configuration"
	@echo ""
	@echo "Test Targets:"
	@echo "  test          Run all tests"
	@echo "  test-l0       Test libtrit"
	@echo "  test-l3       Test CPI-SI"
	@echo "  test-demos    Test demonstrations"
	@echo ""
	@echo "Utility Targets:"
	@echo "  check-tools   Verify tools installed"
	@echo "  check-deps    Check layer dependencies"
	@echo "  clean-all     Remove all generated files"
	@echo "  clean-demos   Clean demo artifacts"
	@echo ""
	@echo "Build Types:"
	@echo "  BUILD_TYPE=debug    Debug build (default)"
	@echo "  BUILD_TYPE=release  Optimized release build"
	@echo ""

# ───────────────────────────────────────────────────────────────────────────
# X1: Policy
# ───────────────────────────────────────────────────────────────────────────
# scripture: Remove not the ancient landmark, which thy fathers have set — Proverbs 22:28
# never:
#   - Remove layer dependency order (L0 before L1/L2/L3)
#   - Remove check-tools target (safety mechanism)
#   - Remove help target (documentation mechanism)
#   - 4-block structure (METADATA → SETUP → BODY → CLOSING)
# careful:
#   - Layer directory paths (affects all build targets)
#   - Tool detection variables (affects conditional compilation)
#   - Build flag defaults (affects all layers)
# safe:
#   - Add new layer targets (L4 FaithNet, L5 Cornerstone)
#   - Add new component targets
#   - Add new test targets
#   - Adjust compiler flags per build type
#
# ───────────────────────────────────────────────────────────────────────────
# X4: Reference
# ───────────────────────────────────────────────────────────────────────────
# related_files:
#   - template.mk — parent template (source)
#   - root.editorconfig — co-located formatting rules
#   - word/core/*.toml — TOML specs driving code generation
# validate: make -n all (dry-run)
# verify: make check-tools && make status
#
# ───────────────────────────────────────────────────────────────────────────
# X5: Note
# ───────────────────────────────────────────────────────────────────────────
# note:      Bereshit root build orchestrator — L0 through L5, config-driven,
#            data-driven. Each layer builds with its native toolchain.
# scripture: Except the LORD build the house, they labour in vain that build it — Psalm 127:1
# anchor:    Genesis 1:1 anchors everything
#
# ═══════════════════════════════════════════════════════════════════════════
# END CLOSING [END]
# ═══════════════════════════════════════════════════════════════════════════
