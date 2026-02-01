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
#   │  Level 0 (Foundation - no dependencies):                                │
#   │    word/work/pkg/trit/     libtrit.a — Balanced ternary types           │
#   │    word/work/pkg/fuse/     bereshit_fs — Zone-aware filesystem          │
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
#   │  Level 4 (MillenniumOS - depends on libtrit):                           │
#   │    millenniumos/           MillenniumOS (kernel + bootloader)           │
#   │                                                                         │
#   │  Dependency Flow:                                                       │
#   │    libtrit.a ─┬─► libomni.a ─► cornerstone ─► millenniumos              │
#   │    bereshit_fs─┘                                                        │
#   │                                                                         │
#   │  Crossing Pattern:                                                      │
#   │    pkg/trit declares → cornerstone implements → millenniumos extends    │
#   │    pkg/fuse userspace → millenniumos kernel VFS                         │
#   │                                                                         │
#   └─────────────────────────────────────────────────────────────────────────┘
#
# ───────────────────────────────────────────────────────────────────────────────
# M.4 TARGETS OVERVIEW [TARGETS]
# ───────────────────────────────────────────────────────────────────────────────
#
# Build:
#   make                Build all (libs + cornerstone)
#   make libs           Build libraries only (libtrit + libomni + bereshit-fs)
#   make libtrit        Build libtrit.a only
#   make bereshit-fs    Build bereshit_fs only (zone-aware filesystem)
#   make libomni        Build libomni.a only
#   make cornerstone    Build cornerstone engine
#   make millenniumos   Build MillenniumOS (kernel + bootloader)
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
#   make run-os         Run MillenniumOS in QEMU
#   make debug-os       Run MillenniumOS with debug output
#   make launcher       Interactive launcher (all modes)
#
# Install (User-Local):
#   make install-user   Install all to ~/.local (Cornerstone + OmniCode + MillenniumOS)
#   make uninstall-user Remove all from ~/.local
#   make update-user    Rebuild and reinstall all
#
# Maintain:
#   make clean          Clean all build artifacts
#   make clean-libs     Clean library artifacts
#   make clean-fuse     Clean bereshit-fs artifacts
#   make clean-engine   Clean cornerstone artifacts
#   make clean-os       Clean MillenniumOS artifacts
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

.PHONY: all libs libtrit libomni bereshit-fs cornerstone millenniumos tools \
        test test-libs test-fuse test-engine test-os \
        diag diag-trit \
        run run-tui run-cli run-os debug-os launcher \
        install-user uninstall-user update-user \
        clean clean-libs clean-fuse clean-engine clean-os status info help

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
FUSE_DIR      := $(WORD_WORK)/pkg/fuse
OMNI_DIR      := $(WORD_WORK)/pkg/omni
CORNERSTONE   := $(BERESHIT_ROOT)/cornerstone
MILLENNIUMOS  := $(BERESHIT_ROOT)/millenniumos

# S.2c Build Artifacts [ARTIFACTS]

LIBTRIT       := $(TRIT_DIR)/build/libtrit.a
BERESHIT_FS   := $(FUSE_DIR)/bin/bereshit_fs
LIBOMNI       := $(OMNI_DIR)/build/libomni.a
ENGINE_BIN    := $(CORNERSTONE)/build/bin/cornerstone
OS_IMAGE      := $(MILLENNIUMOS)/build/millenniumos.img

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

## libs: Build all libraries (libtrit + libomni + bereshit-fs)
libs: libtrit libomni bereshit-fs
	$(ECHO) ""
	$(ECHO) "✓ All libraries and tools built"

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

## bereshit-fs: Build zone-aware filesystem (Level 0 - foundation)
bereshit-fs:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  [L0] Building bereshit-fs..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(FUSE_DIR)

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
# B.3b OS TARGET — MillenniumOS Build [OS]
# ───────────────────────────────────────────────────────────────────────────────

## millenniumos: Build MillenniumOS (Level 3 - depends on libtrit)
millenniumos: libtrit
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  [L3] Building MillenniumOS..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(MILLENNIUMOS)

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
# B.5b DIAGNOSTIC TARGETS — System Verification [DIAGNOSTICS]
# ───────────────────────────────────────────────────────────────────────────────

## diag: Build all diagnostic tools (libtrit)
diag:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  Building diagnostic tools..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(TRIT_DIR) diag

## diag-trit: Build and list libtrit diagnostic tools
diag-trit:
	$(Q)$(MAKE) -C $(TRIT_DIR) diag

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

## run-os: Run MillenniumOS in QEMU
run-os: millenniumos
	$(Q)$(MAKE) -C $(MILLENNIUMOS) run

## debug-os: Run MillenniumOS in QEMU with debug output
debug-os: millenniumos
	$(Q)$(MAKE) -C $(MILLENNIUMOS) debug

## launcher: Run the Cornerstone interactive launcher
launcher: cornerstone
	$(Q)$(CORNERSTONE)/cws-launcher --select

# ───────────────────────────────────────────────────────────────────────────────
# B.6b INSTALL TARGETS — User-Local Installation [INSTALL]
# ───────────────────────────────────────────────────────────────────────────────
#
# Orchestrated installation of all three projects to ~/.local
# Pattern: install-user / uninstall-user / update-user (aligned across all)
#

## install-user: Install Cornerstone + OmniCode IDE + MillenniumOS to ~/.local
install-user: cornerstone millenniumos
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  Installing Kingdom Technology Suite to ~/.local"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "  [1/3] Installing Cornerstone Engine..."
	$(Q)$(MAKE) -C $(CORNERSTONE) install-user
	$(ECHO) ""
	$(ECHO) "  [2/3] Installing OmniCode IDE..."
	$(Q)$(MAKE) -C $(OMNI_DIR) install-user
	$(ECHO) ""
	$(ECHO) "  [3/3] Installing MillenniumOS..."
	$(Q)$(MAKE) -C $(MILLENNIUMOS) install-user
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  ✓ Kingdom Technology Suite installed to ~/.local"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "  Search your applications menu for:"
	$(ECHO) "    • Cornerstone Engine"
	$(ECHO) "    • OmniCode IDE"
	$(ECHO) "    • MillenniumOS"
	$(ECHO) ""

## uninstall-user: Remove Cornerstone + OmniCode IDE + MillenniumOS from ~/.local
uninstall-user:
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  Uninstalling Kingdom Technology Suite from ~/.local"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "  [1/3] Uninstalling Cornerstone Engine..."
	$(Q)$(MAKE) -C $(CORNERSTONE) uninstall-user 2>/dev/null || true
	$(ECHO) ""
	$(ECHO) "  [2/3] Uninstalling OmniCode IDE..."
	$(Q)$(MAKE) -C $(OMNI_DIR) uninstall-user 2>/dev/null || true
	$(ECHO) ""
	$(ECHO) "  [3/3] Uninstalling MillenniumOS..."
	$(Q)$(MAKE) -C $(MILLENNIUMOS) uninstall-user 2>/dev/null || true
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  ✓ Kingdom Technology Suite uninstalled from ~/.local"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"

## update-user: Rebuild and reinstall all to ~/.local
update-user:
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  Updating Kingdom Technology Suite in ~/.local"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "  [1/3] Updating Cornerstone Engine..."
	$(Q)$(MAKE) -C $(CORNERSTONE) update-user
	$(ECHO) ""
	$(ECHO) "  [2/3] Updating OmniCode IDE..."
	$(Q)$(MAKE) -C $(OMNI_DIR) update-user
	$(ECHO) ""
	$(ECHO) "  [3/3] Updating MillenniumOS..."
	$(Q)$(MAKE) -C $(MILLENNIUMOS) update-user
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  ✓ Kingdom Technology Suite updated in ~/.local"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"

# ───────────────────────────────────────────────────────────────────────────────
# B.7 CLEAN TARGETS — Maintenance [CLEAN]
# ───────────────────────────────────────────────────────────────────────────────

## clean: Clean all build artifacts
clean: clean-libs clean-fuse clean-engine clean-os
	$(ECHO) ""
	$(ECHO) "✓ All clean"

## clean-libs: Clean library artifacts
clean-libs:
	$(ECHO) "Cleaning libraries..."
	$(Q)$(MAKE) -C $(TRIT_DIR) clean
	$(Q)$(MAKE) -C $(OMNI_DIR) clean

## clean-fuse: Clean bereshit-fs artifacts
clean-fuse:
	$(ECHO) "Cleaning bereshit-fs..."
	$(Q)$(MAKE) -C $(FUSE_DIR) clean

## clean-engine: Clean cornerstone artifacts
clean-engine:
	$(ECHO) "Cleaning cornerstone..."
	$(Q)$(MAKE) -C $(CORNERSTONE) clean

## clean-os: Clean MillenniumOS artifacts
clean-os:
	$(ECHO) "Cleaning MillenniumOS..."
	$(Q)$(MAKE) -C $(MILLENNIUMOS) clean 2>/dev/null || true

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
	$(ECHO) "Filesystem:"
	@if [ -f $(BERESHIT_FS) ]; then \
		echo "  ✓ bereshit_fs  ($(FUSE_DIR)/bin/)"; \
	else \
		echo "  ○ bereshit_fs  (not built)"; \
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
	$(ECHO) "  FUSE_DIR:       $(FUSE_DIR)"
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
	$(ECHO) "    make bereshit-fs   Build bereshit_fs (L0 - zone filesystem)"
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
	$(ECHO) "    make run-os        Run MillenniumOS in QEMU"
	$(ECHO) "    make debug-os      Run MillenniumOS with debug output"
	$(ECHO) "    make launcher      Interactive launcher (all modes)"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "INSTALL TARGETS (User-Local)"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "    make install-user   Install all to ~/.local (no sudo)"
	$(ECHO) "    make uninstall-user Remove all from ~/.local"
	$(ECHO) "    make update-user    Rebuild and reinstall all"
	$(ECHO) ""
	$(ECHO) "    Installs: Cornerstone + OmniCode IDE + MillenniumOS"
	$(ECHO) "    Search your applications menu after install"
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
	$(ECHO) "DIAGNOSTIC TOOLS"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "    make diag          Build all diagnostic tools"
	$(ECHO) "    make diag-trit     Build libtrit diagnostic tools"
	$(ECHO) ""
	$(ECHO) "    Diagnostic tools are built to:"
	$(ECHO) "    word/work/pkg/trit/build/diag/"
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
	$(ECHO) "  L0:  bereshit_fs   ← Zone-aware filesystem (FUSE)"
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
#   L0: libtrit.a   (word/work/pkg/trit/)   — Balanced ternary types
#   L0: bereshit_fs (word/work/pkg/fuse/)   — Zone-aware filesystem
#   L1: libomni.a   (word/work/pkg/omni/)   — OmniCode language library
#   L2: cornerstone (cornerstone/)          — Game engine
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
#   word/work/pkg/fuse/Makefile   — bereshit_fs build
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
