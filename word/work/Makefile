# ═══════════════════════════════════════════════════════════════════════════════
# OMNICODE PRAGMA [PRAGMA]
# ═══════════════════════════════════════════════════════════════════════════════
#
#!omni makefile
#
# @omni:req  make >= 4.0
# @omni:req  go >= 1.21
# @omni:req  gcc|clang (C11)
#
# @omni:inh  bereshit/word/seed/code/make/makefile.mk
#
# @omni:ins  This is the root coordination Makefile for word/work/
#            It orchestrates building sub-packages: libtrit, libomni, Go tools
#
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# METADATA BLOCK [METADATA]
# ═══════════════════════════════════════════════════════════════════════════════
#
# ───────────────────────────────────────────────────────────────────────────────
# M.1 IDENTITY — What This Is [IDENTITY]
# ───────────────────────────────────────────────────────────────────────────────
#
# Key:         B-word-work-Makefile
# Package:     bereshit/word/work
# Purpose:     Root coordination Makefile — orchestrates sub-package builds
# Type:        Orchestrator (coordinates, doesn't build directly)
#
# ───────────────────────────────────────────────────────────────────────────────
# M.2 LINEAGE — Where This Comes From [LINEAGE]
# ───────────────────────────────────────────────────────────────────────────────
#
# derives_from: bereshit/word/seed/code/make/makefile.mk
# modeled_on:   cornerstone/Makefile (orchestrator pattern)
#
# ───────────────────────────────────────────────────────────────────────────────
# M.3 BIBLICAL FOUNDATION [BIBLICAL]
# ───────────────────────────────────────────────────────────────────────────────
#
# Scripture: "Except the LORD build the house, they labour in vain that build it"
#            — Psalm 127:1
#
# Principle: Coordination enables the parts to become the whole.
#            Each sub-package has its own Makefile; this orchestrates them.
#
# ───────────────────────────────────────────────────────────────────────────────
# M.4 BUILD CHAIN — Dependency Order [CHAIN]
# ───────────────────────────────────────────────────────────────────────────────
#
#   ┌─────────────────────────────────────────────────────────────────────────┐
#   │  BUILD CHAIN (dependency order)                                         │
#   ├─────────────────────────────────────────────────────────────────────────┤
#   │                                                                         │
#   │  Level 0 (Foundation):                                                  │
#   │    pkg/trit/          libtrit.a — Balanced ternary types                │
#   │                                                                         │
#   │  Level 1 (Depends on L0):                                               │
#   │    pkg/omni/          libomni.a — OmniCode language library             │
#   │                                                                         │
#   │  Level 2 (Go tools):                                                    │
#   │    cmd/generate-config/   TOML → C header generator                     │
#   │    cmd/cpisi/             CPI-SI CLI tool                               │
#   │                                                                         │
#   │  Dependency Flow:                                                       │
#   │    libtrit.a ← libomni.a ← IDE binaries                                 │
#   │                                                                         │
#   └─────────────────────────────────────────────────────────────────────────┘
#
# ───────────────────────────────────────────────────────────────────────────────
# M.5 TARGETS OVERVIEW [TARGETS]
# ───────────────────────────────────────────────────────────────────────────────
#
# Build:
#   make                Build all libraries (libtrit + libomni)
#   make libs           Build all libraries
#   make libtrit        Build libtrit.a only
#   make libomni        Build libomni.a (depends on libtrit)
#   make tools          Build Go tools
#
# Test:
#   make test           Run all tests
#   make test-trit      Test libtrit only
#   make test-omni      Test libomni only
#   make test-go        Test Go packages
#
# Generate:
#   make generate       Generate C headers from TOML specs
#
# Maintain:
#   make clean          Clean all build artifacts
#   make info           Show configuration
#   make help           This message
#
# ═══════════════════════════════════════════════════════════════════════════════
# END METADATA
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# SETUP BLOCK [SETUP]
# ═══════════════════════════════════════════════════════════════════════════════

# ───────────────────────────────────────────────────────────────────────────────
# S.1 PHONY — Non-File Targets [PHONY]
# ───────────────────────────────────────────────────────────────────────────────

.PHONY: all libs libtrit libomni tools \
        test test-trit test-omni test-go \
        generate clean clean-all status info help

# Set default goal explicitly
.DEFAULT_GOAL := all

# ───────────────────────────────────────────────────────────────────────────────
# S.2 VARIABLES — Configuration [VARIABLES]
# ───────────────────────────────────────────────────────────────────────────────

# S.2a Paths [PATHS]

BERESHIT_ROOT := $(shell cd ../.. && pwd)
PKG_DIR       := pkg
CMD_DIR       := cmd
BIN_DIR       := bin

# S.2b Sub-package Locations [PACKAGES]

TRIT_DIR      := $(PKG_DIR)/trit
OMNI_DIR      := $(PKG_DIR)/omni
CONFIG_DIR    := $(PKG_DIR)/config

# S.2c Generated Output [OUTPUT]

GENERATED_DIR := $(TRIT_DIR)/include/generated

# S.2d Go Settings [GO]

GO            := go
GOFLAGS       := -v

# S.2e Tools [TOOLS]

GENERATOR     := generate-config
CPISI_CLI     := cpisi

# S.2f Build Mode (passed to sub-Makefiles) [MODE]

BUILD_MODE    ?= dev
export BUILD_MODE

# S.2g Verbose Mode [VERBOSE]

VERBOSE       ?=
export VERBOSE

# S.2h Output Control [OUTPUT_CONTROL]

Q             := $(if $(VERBOSE),,@)
ECHO          := @echo

# ═══════════════════════════════════════════════════════════════════════════════
# END SETUP
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# BODY BLOCK [BODY]
# ═══════════════════════════════════════════════════════════════════════════════

# ───────────────────────────────────────────────────────────────────────────────
# B.1 ENTRY POINTS (L0) — User-Facing Targets [ENTRY]
# ───────────────────────────────────────────────────────────────────────────────

## all: Build all libraries (default)
all: libs
	$(ECHO) ""
	$(ECHO) "✓ All libraries built"
	$(ECHO) "  Test:  make test"
	$(ECHO) "  Clean: make clean"

## libs: Build all C libraries (libtrit + libomni)
libs: libtrit libomni

## tools: Build Go tools
tools: $(BIN_DIR)/$(GENERATOR)

# ───────────────────────────────────────────────────────────────────────────────
# B.2 LIBRARY TARGETS (L1) — Sub-Package Builds [LIBRARIES]
# ───────────────────────────────────────────────────────────────────────────────

## libtrit: Build libtrit.a (balanced ternary types)
libtrit:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  Building libtrit..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(TRIT_DIR)

## libomni: Build libomni.a (depends on libtrit)
libomni: libtrit
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  Building libomni..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(OMNI_DIR)

# ───────────────────────────────────────────────────────────────────────────────
# B.3 GO TOOL TARGETS (L2) — Go Build [GO_TOOLS]
# ───────────────────────────────────────────────────────────────────────────────

## $(BIN_DIR)/$(GENERATOR): Build config generator
$(BIN_DIR)/$(GENERATOR): $(CMD_DIR)/$(GENERATOR)/main.go $(CONFIG_DIR)/loader.go | $(BIN_DIR)
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  Building $(GENERATOR)..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(GO) build $(GOFLAGS) -o $@ ./$(CMD_DIR)/$(GENERATOR)
	$(ECHO) "✓ Built: $@"

$(BIN_DIR):
	$(Q)mkdir -p $(BIN_DIR)

# ───────────────────────────────────────────────────────────────────────────────
# B.4 GENERATION TARGETS — TOML → C Headers [GENERATE]
# ───────────────────────────────────────────────────────────────────────────────

## generate: Generate C headers from TOML specs
generate: $(BIN_DIR)/$(GENERATOR)
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  Generating C headers from TOML specs..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)BERESHIT_ROOT=$(BERESHIT_ROOT) ./$(BIN_DIR)/$(GENERATOR)
	$(ECHO) ""
	$(ECHO) "Generated files:"
	$(Q)ls -la $(GENERATED_DIR)/*.gen.h 2>/dev/null || echo "  (none found)"

# ───────────────────────────────────────────────────────────────────────────────
# B.5 TEST TARGETS — Testing [TEST]
# ───────────────────────────────────────────────────────────────────────────────

## test: Run all tests
test: test-trit test-omni test-go
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  All tests complete!"
	$(ECHO) "════════════════════════════════════════════════════════════════"

## test-trit: Test libtrit
test-trit:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  Testing libtrit..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(TRIT_DIR) test

## test-omni: Test libomni
test-omni:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  Testing libomni..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(OMNI_DIR) test

## test-go: Test Go packages
test-go:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  Testing Go packages..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)BERESHIT_ROOT=$(BERESHIT_ROOT) $(GO) test $(GOFLAGS) ./$(CONFIG_DIR)/...

# ───────────────────────────────────────────────────────────────────────────────
# B.6 CLEAN TARGETS — Maintenance [CLEAN]
# ───────────────────────────────────────────────────────────────────────────────

## clean: Clean all build artifacts
clean:
	$(ECHO) "Cleaning all build artifacts..."
	$(Q)$(MAKE) -C $(TRIT_DIR) clean
	$(Q)$(MAKE) -C $(OMNI_DIR) clean
	$(Q)rm -rf $(BIN_DIR)
	$(ECHO) "✓ Clean complete"

## clean-all: Clean everything including generated headers
clean-all: clean
	$(ECHO) "Cleaning generated headers..."
	$(Q)rm -f $(GENERATED_DIR)/*.gen.h
	$(ECHO) "✓ Full clean complete"

# ───────────────────────────────────────────────────────────────────────────────
# B.7 INFO TARGETS — Diagnostics [INFO]
# ───────────────────────────────────────────────────────────────────────────────

## status: Show current build state
status:
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  word/work - Build Status"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "BERESHIT_ROOT: $(BERESHIT_ROOT)"
	$(ECHO) "BUILD_MODE:    $(BUILD_MODE)"
	$(ECHO) ""
	$(ECHO) "Libraries:"
	@if [ -f $(TRIT_DIR)/build/libtrit.a ]; then \
		echo "  ✓ libtrit.a (built)"; \
	else \
		echo "  ○ libtrit.a (not built)"; \
	fi
	@if [ -f $(OMNI_DIR)/build/libomni.a ]; then \
		echo "  ✓ libomni.a (built)"; \
	else \
		echo "  ○ libomni.a (not built)"; \
	fi
	$(ECHO) ""
	$(ECHO) "Go Tools:"
	@if [ -f $(BIN_DIR)/$(GENERATOR) ]; then \
		echo "  ✓ $(GENERATOR) (built)"; \
	else \
		echo "  ○ $(GENERATOR) (not built)"; \
	fi
	$(ECHO) ""
	$(ECHO) "Generated Headers:"
	@if ls $(GENERATED_DIR)/*.gen.h 1>/dev/null 2>&1; then \
		for f in $(GENERATED_DIR)/*.gen.h; do \
			echo "  ✓ $$(basename $$f)"; \
		done; \
	else \
		echo "  (none)"; \
	fi

## info: Show configuration
info:
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  word/work - Configuration"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "Paths:"
	$(ECHO) "  BERESHIT_ROOT:  $(BERESHIT_ROOT)"
	$(ECHO) "  PKG_DIR:        $(PKG_DIR)"
	$(ECHO) "  CMD_DIR:        $(CMD_DIR)"
	$(ECHO) "  BIN_DIR:        $(BIN_DIR)"
	$(ECHO) ""
	$(ECHO) "Sub-packages:"
	$(ECHO) "  TRIT_DIR:       $(TRIT_DIR)"
	$(ECHO) "  OMNI_DIR:       $(OMNI_DIR)"
	$(ECHO) "  CONFIG_DIR:     $(CONFIG_DIR)"
	$(ECHO) ""
	$(ECHO) "Settings:"
	$(ECHO) "  BUILD_MODE:     $(BUILD_MODE)"
	$(ECHO) "  GO:             $(GO)"
	$(ECHO) ""
	$(ECHO) "Sub-Makefile Status:"
	@$(MAKE) -C $(TRIT_DIR) info 2>/dev/null | head -5 || echo "  (libtrit info unavailable)"

## help: Show this help message
help:
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  word/work - Build System Help"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "QUICK START"
	$(ECHO) "  make                 Build all libraries"
	$(ECHO) "  make test            Run all tests"
	$(ECHO) "  make status          Show build state"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "BUILD TARGETS"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "  Libraries:"
	$(ECHO) "    make libs          Build all C libraries"
	$(ECHO) "    make libtrit       Build libtrit.a (Level 0)"
	$(ECHO) "    make libomni       Build libomni.a (Level 1, depends on libtrit)"
	$(ECHO) ""
	$(ECHO) "  Tools:"
	$(ECHO) "    make tools         Build Go tools"
	$(ECHO) "    make generate      Generate C headers from TOML"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "TEST TARGETS"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "    make test          Run all tests"
	$(ECHO) "    make test-trit     Test libtrit only"
	$(ECHO) "    make test-omni     Test libomni only"
	$(ECHO) "    make test-go       Test Go packages"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "MAINTENANCE"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "    make clean         Clean all build artifacts"
	$(ECHO) "    make clean-all     Clean including generated headers"
	$(ECHO) "    make status        Show current build state"
	$(ECHO) "    make info          Show configuration"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "BUILD OPTIONS"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "  BUILD_MODE=   dev      Fast iteration (default)"
	$(ECHO) "                debug    Full debugging"
	$(ECHO) "                release  Production"
	$(ECHO) ""
	$(ECHO) "  VERBOSE=1     Show full compilation commands"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "BUILD CHAIN"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "  Level 0:  libtrit.a   ← Foundation (balanced ternary)"
	$(ECHO) "  Level 1:  libomni.a   ← Depends on libtrit"
	$(ECHO) "  Level 2:  Go tools    ← Independent"
	$(ECHO) ""

# ═══════════════════════════════════════════════════════════════════════════════
# END BODY
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# CLOSING BLOCK [CLOSING]
# ═══════════════════════════════════════════════════════════════════════════════
#
# ───────────────────────────────────────────────────────────────────────────────
# X.1 STRUCTURE NOTES [STRUCTURE]
# ───────────────────────────────────────────────────────────────────────────────
#
# This Makefile follows the 4-block structure:
#   METADATA → SETUP → BODY → CLOSING
#
# As an orchestrator Makefile, it:
#   - Does NOT compile code directly
#   - Delegates to sub-package Makefiles
#   - Passes BUILD_MODE and VERBOSE to children
#   - Provides unified test/clean/info interface
#
# ───────────────────────────────────────────────────────────────────────────────
# X.2 RELATED FILES [RELATED]
# ───────────────────────────────────────────────────────────────────────────────
#
# Children (called via $(MAKE) -C):
#   pkg/trit/Makefile   — libtrit.a build
#   pkg/omni/Makefile   — libomni.a build
#
# Parent (can call this):
#   (none - this is the root work/ Makefile)
#
# ───────────────────────────────────────────────────────────────────────────────
# X.3 BIBLICAL CLOSING [BIBLICAL]
# ───────────────────────────────────────────────────────────────────────────────
#
# "Except the LORD build the house, they labour in vain that build it"
# — Psalm 127:1 KJV
#
# ═══════════════════════════════════════════════════════════════════════════════
# END CLOSING
# ═══════════════════════════════════════════════════════════════════════════════
