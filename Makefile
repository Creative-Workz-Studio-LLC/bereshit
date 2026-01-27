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
# @omni:ins  ROOT Makefile for Bereshit repository
#            Orchestrates ALL builds: libtrit, libomni, cornerstone
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
# Key:         B-root-Makefile
# Package:     bereshit (root)
# Purpose:     ROOT orchestrator — builds entire Bereshit system
# Type:        Master Orchestrator
#
# ───────────────────────────────────────────────────────────────────────────────
# M.2 BIBLICAL FOUNDATION [BIBLICAL]
# ───────────────────────────────────────────────────────────────────────────────
#
# Scripture: "In the beginning God created the heaven and the earth."
#            — Genesis 1:1
#
# Principle: From one source, all things are built in order.
#            Foundation first, then what depends on it.
#
# ───────────────────────────────────────────────────────────────────────────────
# M.3 BUILD CHAIN — Complete Dependency Order [CHAIN]
# ───────────────────────────────────────────────────────────────────────────────
#
#   ┌─────────────────────────────────────────────────────────────────────────┐
#   │  BERESHIT BUILD CHAIN                                                   │
#   ├─────────────────────────────────────────────────────────────────────────┤
#   │                                                                         │
#   │  Level 0 (Foundation):                                                  │
#   │    word/work/pkg/trit/     libtrit.a — Balanced ternary types           │
#   │                                                                         │
#   │  Level 1 (Depends on L0):                                               │
#   │    word/work/pkg/omni/     libomni.a — OmniCode language library        │
#   │                                                                         │
#   │  Level 2 (Depends on L0+L1):                                            │
#   │    cornerstone/            Cornerstone engine (game engine)             │
#   │                                                                         │
#   │  Level 3 (Go tools - independent):                                      │
#   │    word/work/cmd/          Go CLI tools                                 │
#   │                                                                         │
#   │  Dependency Flow:                                                       │
#   │    libtrit.a → libomni.a → cornerstone                                  │
#   │                                                                         │
#   └─────────────────────────────────────────────────────────────────────────┘
#
# ───────────────────────────────────────────────────────────────────────────────
# M.4 TARGETS OVERVIEW [TARGETS]
# ───────────────────────────────────────────────────────────────────────────────
#
# Build:
#   make                Build all (libs + cornerstone)
#   make libs           Build libraries only (libtrit + libomni)
#   make libtrit        Build libtrit.a only
#   make libomni        Build libomni.a only
#   make cornerstone    Build cornerstone engine
#   make tools          Build Go tools
#
# Test:
#   make test           Run all tests
#   make test-libs      Test libraries only
#   make test-engine    Test cornerstone
#
# Run:
#   make run            Run cornerstone (GUI mode)
#   make run-tui        Run cornerstone (TUI mode)
#   make run-cli        Run cornerstone (CLI mode)
#
# Maintain:
#   make clean          Clean all build artifacts
#   make status         Show build state
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

.PHONY: all libs libtrit libomni cornerstone tools \
        test test-libs test-engine \
        run run-tui run-cli \
        clean clean-libs clean-engine status info help

# Set default goal explicitly
.DEFAULT_GOAL := all

# ───────────────────────────────────────────────────────────────────────────────
# S.2 VARIABLES — Configuration [VARIABLES]
# ───────────────────────────────────────────────────────────────────────────────

# S.2a Root Paths [PATHS]

BERESHIT_ROOT := $(CURDIR)
WORD_WORK     := $(BERESHIT_ROOT)/word/work

# S.2b Package Locations [PACKAGES]

TRIT_DIR      := $(WORD_WORK)/pkg/trit
OMNI_DIR      := $(WORD_WORK)/pkg/omni
CORNERSTONE   := $(BERESHIT_ROOT)/cornerstone

# S.2c Build Artifacts [ARTIFACTS]

LIBTRIT       := $(TRIT_DIR)/build/libtrit.a
LIBOMNI       := $(OMNI_DIR)/build/libomni.a
ENGINE_BIN    := $(CORNERSTONE)/build/bin/cornerstone

# S.2d Build Mode (passed to all sub-Makefiles) [MODE]

BUILD_MODE    ?= dev
export BUILD_MODE

# S.2e Verbose Mode [VERBOSE]

VERBOSE       ?=
export VERBOSE

# S.2f Output Control [OUTPUT_CONTROL]

Q             := $(if $(VERBOSE),,@)
ECHO          := @echo

# ═══════════════════════════════════════════════════════════════════════════════
# END SETUP
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# BODY BLOCK [BODY]
# ═══════════════════════════════════════════════════════════════════════════════

# ───────────────────────────────────────────────────────────────────────────────
# B.1 ENTRY POINTS — User-Facing Targets [ENTRY]
# ───────────────────────────────────────────────────────────────────────────────

## all: Build everything (libraries + cornerstone)
all: libs cornerstone
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  ✓ Bereshit build complete"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "  Run:     make run           (GUI mode)"
	$(ECHO) "  Test:    make test"
	$(ECHO) "  Clean:   make clean"
	$(ECHO) ""

## libs: Build all libraries (libtrit + libomni)
libs: libtrit libomni
	$(ECHO) ""
	$(ECHO) "✓ All libraries built"

# ───────────────────────────────────────────────────────────────────────────────
# B.2 LIBRARY TARGETS — Foundation Builds [LIBRARIES]
# ───────────────────────────────────────────────────────────────────────────────

## libtrit: Build libtrit.a (Level 0 - foundation)
libtrit:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  [L0] Building libtrit..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(TRIT_DIR)

## libomni: Build libomni.a (Level 1 - depends on libtrit)
libomni: libtrit
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  [L1] Building libomni..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(OMNI_DIR)

# ───────────────────────────────────────────────────────────────────────────────
# B.3 ENGINE TARGET — Cornerstone Build [ENGINE]
# ───────────────────────────────────────────────────────────────────────────────

## cornerstone: Build cornerstone engine (Level 2 - depends on libs)
cornerstone: libs
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  [L2] Building cornerstone..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(CORNERSTONE) build

# ───────────────────────────────────────────────────────────────────────────────
# B.4 GO TOOLS — Independent Builds [TOOLS]
# ───────────────────────────────────────────────────────────────────────────────

## tools: Build Go tools
tools:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  [L3] Building Go tools..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(WORD_WORK) tools

# ───────────────────────────────────────────────────────────────────────────────
# B.5 TEST TARGETS — Testing [TEST]
# ───────────────────────────────────────────────────────────────────────────────

## test: Run all tests
test: test-libs test-engine
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  ✓ All tests complete!"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"

## test-libs: Test libraries (libtrit + libomni)
test-libs:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  Testing libraries..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(TRIT_DIR) test
	$(Q)$(MAKE) -C $(OMNI_DIR) test

## test-engine: Test cornerstone
test-engine:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  Testing cornerstone..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(CORNERSTONE) test

# ───────────────────────────────────────────────────────────────────────────────
# B.6 RUN TARGETS — Execution [RUN]
# ───────────────────────────────────────────────────────────────────────────────

## run: Run cornerstone (GUI mode)
run: cornerstone
	$(Q)$(MAKE) -C $(CORNERSTONE) run

## run-tui: Run cornerstone (TUI mode)
run-tui: cornerstone
	$(Q)$(MAKE) -C $(CORNERSTONE) run-tui

## run-cli: Run cornerstone (CLI mode)
run-cli: cornerstone
	$(Q)$(MAKE) -C $(CORNERSTONE) run-cli

# ───────────────────────────────────────────────────────────────────────────────
# B.7 CLEAN TARGETS — Maintenance [CLEAN]
# ───────────────────────────────────────────────────────────────────────────────

## clean: Clean all build artifacts
clean: clean-libs clean-engine
	$(ECHO) ""
	$(ECHO) "✓ All clean"

## clean-libs: Clean library artifacts
clean-libs:
	$(ECHO) "Cleaning libraries..."
	$(Q)$(MAKE) -C $(TRIT_DIR) clean
	$(Q)$(MAKE) -C $(OMNI_DIR) clean

## clean-engine: Clean cornerstone artifacts
clean-engine:
	$(ECHO) "Cleaning cornerstone..."
	$(Q)$(MAKE) -C $(CORNERSTONE) clean

# ───────────────────────────────────────────────────────────────────────────────
# B.8 INFO TARGETS — Diagnostics [INFO]
# ───────────────────────────────────────────────────────────────────────────────

## status: Show build state across all components
status:
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  Bereshit - Build Status"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "BUILD_MODE: $(BUILD_MODE)"
	$(ECHO) ""
	$(ECHO) "Libraries:"
	@if [ -f $(LIBTRIT) ]; then \
		echo "  ✓ libtrit.a  ($(TRIT_DIR)/build/)"; \
	else \
		echo "  ○ libtrit.a  (not built)"; \
	fi
	@if [ -f $(LIBOMNI) ]; then \
		echo "  ✓ libomni.a  ($(OMNI_DIR)/build/)"; \
	else \
		echo "  ○ libomni.a  (not built)"; \
	fi
	$(ECHO) ""
	$(ECHO) "Engine:"
	@if [ -f $(ENGINE_BIN) ]; then \
		echo "  ✓ cornerstone  ($(CORNERSTONE)/)"; \
	else \
		echo "  ○ cornerstone  (not built)"; \
	fi
	$(ECHO) ""

## info: Show configuration
info:
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  Bereshit - Configuration"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "Paths:"
	$(ECHO) "  BERESHIT_ROOT:  $(BERESHIT_ROOT)"
	$(ECHO) "  WORD_WORK:      $(WORD_WORK)"
	$(ECHO) "  CORNERSTONE:    $(CORNERSTONE)"
	$(ECHO) ""
	$(ECHO) "Packages:"
	$(ECHO) "  TRIT_DIR:       $(TRIT_DIR)"
	$(ECHO) "  OMNI_DIR:       $(OMNI_DIR)"
	$(ECHO) ""
	$(ECHO) "Settings:"
	$(ECHO) "  BUILD_MODE:     $(BUILD_MODE)"
	$(ECHO) "  VERBOSE:        $(if $(VERBOSE),yes,no)"
	$(ECHO) ""

## help: Show this help message
help:
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  Bereshit - Root Build System"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "  \"In the beginning God created the heaven and the earth.\" — Genesis 1:1"
	$(ECHO) ""
	$(ECHO) "QUICK START"
	$(ECHO) "  make                 Build everything"
	$(ECHO) "  make run             Run cornerstone (GUI)"
	$(ECHO) "  make test            Run all tests"
	$(ECHO) "  make status          Show build state"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "BUILD TARGETS"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "  Full Build:"
	$(ECHO) "    make               Build all (libs + cornerstone)"
	$(ECHO) "    make libs          Build libraries only"
	$(ECHO) "    make cornerstone   Build engine only (builds libs first)"
	$(ECHO) ""
	$(ECHO) "  Individual:"
	$(ECHO) "    make libtrit       Build libtrit.a (L0 - foundation)"
	$(ECHO) "    make libomni       Build libomni.a (L1 - depends on libtrit)"
	$(ECHO) "    make tools         Build Go tools (L3 - independent)"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "RUN TARGETS"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "    make run           Run cornerstone (GUI mode)"
	$(ECHO) "    make run-tui       Run cornerstone (TUI mode)"
	$(ECHO) "    make run-cli       Run cornerstone (CLI mode)"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "TEST TARGETS"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "    make test          Run all tests"
	$(ECHO) "    make test-libs     Test libraries only"
	$(ECHO) "    make test-engine   Test cornerstone only"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "MAINTENANCE"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "    make clean         Clean all build artifacts"
	$(ECHO) "    make clean-libs    Clean libraries only"
	$(ECHO) "    make clean-engine  Clean cornerstone only"
	$(ECHO) "    make status        Show build state"
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
	$(ECHO) "  L0:  libtrit.a     ← Foundation (balanced ternary)"
	$(ECHO) "  L1:  libomni.a     ← Depends on libtrit"
	$(ECHO) "  L2:  cornerstone   ← Depends on libs"
	$(ECHO) "  L3:  Go tools      ← Independent"
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
# This is the ROOT Makefile for the entire Bereshit repository.
# It orchestrates all sub-builds in dependency order:
#
#   1. libtrit.a   (word/work/pkg/trit/)
#   2. libomni.a   (word/work/pkg/omni/)
#   3. cornerstone (cornerstone/)
#
# Each sub-package has its own Makefile that handles its specific build.
# This Makefile only coordinates and delegates.
#
# ───────────────────────────────────────────────────────────────────────────────
# X.2 RELATED FILES [RELATED]
# ───────────────────────────────────────────────────────────────────────────────
#
# Children (called via $(MAKE) -C):
#   word/work/pkg/trit/Makefile   — libtrit.a build
#   word/work/pkg/omni/Makefile   — libomni.a build
#   cornerstone/Makefile          — engine build
#   word/work/Makefile            — Go tools
#
# ───────────────────────────────────────────────────────────────────────────────
# X.3 BIBLICAL CLOSING [BIBLICAL]
# ───────────────────────────────────────────────────────────────────────────────
#
# "In the beginning God created the heaven and the earth."
# — Genesis 1:1 KJV
#
# From one source, all things are built in order.
#
# ═══════════════════════════════════════════════════════════════════════════════
# END CLOSING
# ═══════════════════════════════════════════════════════════════════════════════
