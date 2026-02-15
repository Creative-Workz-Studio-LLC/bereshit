# #!omni code --makefile
#
# ═══════════════════════════════════════════════════════════════════════════════
# OMNICODE PRAGMA [PRAGMA]
# ═══════════════════════════════════════════════════════════════════════════════
#
# --- P1: Core ---
# key:       B-root-Makefile
# from:      B-seed-code-makefile-template
# at:        a-02.00
#
# --- P2: Family ---
# type:      code
# subtype:   makefile
# role:      root-orchestrator
# structure: 4-block
#
# --- P3: Instance ---
# component: build-system
# layer:     root
# provides:  ROOT_BUILD_ORCHESTRATOR
#
# --- P4: Architecture ---
# layers:    L0:C, L1:C, L2:C, L3:Go, L4:Go/Rust, L5:C#
# anchor:    genesis_1_1 -> void(0) -> unity(1) -> derivation
#
# --- P5: Summary ---
# title:     Bereshit Root Makefile
# brief:     ROOT orchestrator — builds entire L0-L5 Kingdom Technology stack
#
# ═══════════════════════════════════════════════════════════════════════════════
# END PRAGMA [END]
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
# Purpose:     ROOT orchestrator — builds entire Kingdom Technology stack
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
# M.3 BUILD CHAIN — Kingdom Technology Stack [CHAIN]
# ───────────────────────────────────────────────────────────────────────────────
#
#   ┌─────────────────────────────────────────────────────────────────────────┐
#   │  KINGDOM TECHNOLOGY STACK — L0-L5 Build Chain                          │
#   ├─────────────────────────────────────────────────────────────────────────┤
#   │                                                                         │
#   │  L0 Universal (C — no dependencies):                                    │
#   │    word/work/pkg/trit/     libtrit.a — Balanced ternary types           │
#   │    word/work/pkg/fuse/     bereshit_fs — Zone-aware filesystem          │
#   │                                                                         │
#   │  L1 OmniCode (C — depends on L0):                                      │
#   │    word/work/pkg/omni/     libomni.a — OmniCode language library        │
#   │                                                                         │
#   │  L2 Platform (C — depends on L0):                                       │
#   │    millenniumos/           MillenniumOS (kernel + bootloader)           │
#   │                                                                         │
#   │  L3 CPI-SI (Go — independent):                                         │
#   │    word/claude/            CPI-SI intelligence (hooks, pkg, skills)     │
#   │    word/work/cmd/          Go CLI tools                                 │
#   │                                                                         │
#   │  L4 FaithNet (Go/Rust — depends on L3):                                │
#   │    server/                 FaithNet server (HTTP API + website)         │
#   │                                                                         │
#   │  L5 Applications (C# — depends on L0+L1):                              │
#   │    cornerstone/            Cornerstone engine (game engine)             │
#   │                                                                         │
#   │  Build Streams:                                                         │
#   │    Stream 1 (C):    L0 ──► L1 ──► L2                                   │
#   │    Stream 2 (Go):   L3 ──► L4                                          │
#   │    Stream 3 (C#):   L5 (submodule, independent)                        │
#   │                                                                         │
#   │  Rule: NO UPWARD IMPORTS — higher layers depend on lower, never reverse│
#   │                                                                         │
#   └─────────────────────────────────────────────────────────────────────────┘
#
# ───────────────────────────────────────────────────────────────────────────────
# M.4 TARGETS OVERVIEW [TARGETS]
# ───────────────────────────────────────────────────────────────────────────────
#
# Build (by layer):
#   make l0             Build L0 Universal (libtrit + bereshit-fs)
#   make l1             Build L1 OmniCode (libomni, depends on L0)
#   make l2             Build L2 Platform (MillenniumOS, depends on L0)
#   make l3             Build L3 CPI-SI (hooks, pkg, skills)
#   make l4             Build L4 FaithNet (server)
#   make l5             Build L5 Applications (Cornerstone, depends on L0+L1)
#
# Build (by component — backward compatible):
#   make                Build all (libs + cornerstone)
#   make libs           Build libraries only (libtrit + libomni + bereshit-fs)
#   make libtrit        Build libtrit.a only
#   make bereshit-fs    Build bereshit_fs only (zone-aware filesystem)
#   make libomni        Build libomni.a only
#   make cornerstone    Build cornerstone engine
#   make millenniumos   Build MillenniumOS (kernel + bootloader)
#   make server         Build FaithNet server
#   make tools          Build Go tools
#
# Update (incremental — only rebuild changed layers):
#   make update         Incremental rebuild of all layers
#   make update-l0      Incremental L0 only
#   make update-l3      Incremental L3 only
#   make update-l4      Incremental L4 only
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
# b-word (Go+Rust hybrid — orchestrates L0+L1+L2 layer Makefiles):
#   make bword-build    Build L0+L1+L2 Go+Rust (delegates to layer Makefiles)
#   make bword-test     Run L0+L1+L2 Go+Rust tests
#   make bword-check    Full quality gate: L0+L1+L2 Go+Rust (vet, lint, clippy, test)
#   make bword-clean    Clean L0+L1+L2 structural caches
#
# Structural (cached sentinels):
#   make lint           Run all structural linters (Go + Rust)
#   make lint-go        Lint Go 4-block alignment
#   make lint-rust      Lint Rust 4-block alignment
#   make go-check       Full Go quality gate: vet + lint + test
#   make go-test        Run Go tests
#   make go-build       Build Go packages
#   make go-vet         Vet Go packages
#   make clean-struct   Clear structural caches (root + L0 + L1 + L2)
#
# Maintain:
#   make clean          Clean all build artifacts + cache
#   make clean-libs     Clean library artifacts
#   make clean-fuse     Clean bereshit-fs artifacts
#   make clean-engine   Clean cornerstone artifacts
#   make clean-os       Clean MillenniumOS artifacts
#   make clean-cache    Clean build cache only
#   make status         Show build state
#   make layer-status   Show layer build cache state
#   make info           Show configuration
#   make help           This message
#
# ═══════════════════════════════════════════════════════════════════════════════
# END METADATA [END]
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# SETUP BLOCK [SETUP]
# ═══════════════════════════════════════════════════════════════════════════════

# ───────────────────────────────────────────────────────────────────────────────
# S.1 PHONY — Non-File Targets [PHONY]
# ───────────────────────────────────────────────────────────────────────────────

.PHONY: all libs libtrit libomni bereshit-fs cornerstone millenniumos tools server \
        l0 l1 l2 l3 l4 l5 layer-status \
        update update-l0 update-l1 update-l2 update-l3 update-l4 update-l5 \
        test test-libs test-fuse test-engine test-os \
        diag diag-trit \
        run run-tui run-cli run-os debug-os launcher \
        install-user uninstall-user update-user \
        clean clean-libs clean-fuse clean-engine clean-os clean-cache \
        lint lint-go lint-rust go-test go-build go-vet go-check \
        bword-build bword-test bword-check bword-clean clean-struct \
        status info help

# Set default goal explicitly
.DEFAULT_GOAL := all

# ───────────────────────────────────────────────────────────────────────────────
# S.2 VARIABLES — Configuration [VARIABLES]
# ───────────────────────────────────────────────────────────────────────────────

# S.2a Root Paths [PATHS]

BERESHIT_ROOT := $(CURDIR)
WORD_WORK     := $(BERESHIT_ROOT)/word/work

# S.2b Package Locations (current paths — code migrates to b-word/ naturally) [PACKAGES]

TRIT_DIR      := $(WORD_WORK)/pkg/trit
FUSE_DIR      := $(WORD_WORK)/pkg/fuse
OMNI_DIR      := $(WORD_WORK)/pkg/omni
CORNERSTONE   := $(BERESHIT_ROOT)/cornerstone
MILLENNIUMOS  := $(BERESHIT_ROOT)/millenniumos
SERVER_DIR    := $(BERESHIT_ROOT)/server
CLAUDE_DIR    := $(BERESHIT_ROOT)/word/claude

# S.2c Layer Paths (target structure — skeleton planted, code migrates here) [LAYERS]

L0_LAYER      := $(BERESHIT_ROOT)/b-word/L0-universal
L1_LAYER      := $(BERESHIT_ROOT)/b-word/L1-omnicode
L2_LAYER      := $(BERESHIT_ROOT)/b-word/L2-platform
L3_LAYER      := $(BERESHIT_ROOT)/b-word/L3-cpisi
L4_LAYER      := $(BERESHIT_ROOT)/b-word/L4-faithnet
L5_LAYER      := $(BERESHIT_ROOT)/b-word/L5-applications

# S.2d Build Artifacts [ARTIFACTS]

LIBTRIT       := $(TRIT_DIR)/build/libtrit.a
BERESHIT_FS   := $(FUSE_DIR)/bin/bereshit_fs
LIBOMNI       := $(OMNI_DIR)/build/libomni.a
ENGINE_BIN    := $(CORNERSTONE)/build/bin/cornerstone
OS_IMAGE      := $(MILLENNIUMOS)/build/millenniumos.img

# S.2e Build Cache — Sentinel files track layer build state [CACHE]

CACHE_DIR     := $(BERESHIT_ROOT)/.build-cache
CACHE_L0      := $(CACHE_DIR)/.l0-built
CACHE_L1      := $(CACHE_DIR)/.l1-built
CACHE_L2      := $(CACHE_DIR)/.l2-built
CACHE_L3      := $(CACHE_DIR)/.l3-built
CACHE_L4      := $(CACHE_DIR)/.l4-built
CACHE_L5      := $(CACHE_DIR)/.l5-built

# S.2f Build Mode (passed to all sub-Makefiles) [MODE]

BUILD_MODE    ?= dev
export BUILD_MODE

# S.2g Verbose Mode [VERBOSE]

VERBOSE       ?=
export VERBOSE

# S.2h Output Control [OUTPUT_CONTROL]

Q             := $(if $(VERBOSE),,@)
ECHO          := @echo

# S.2i Structural Tools [STRUCT]

NEW_STRUCT    := $(BERESHIT_ROOT)/.a-new-structure
CWS_STRUCT    := deno run --allow-read $(NEW_STRUCT)/c-tov/cws-struct/mod.ts
B_WORD        := $(NEW_STRUCT)/b-word

# Structural cache (lint is root-level; build/test/vet delegated to layer Makefiles)
CACHE_LINT_GO   := $(CACHE_DIR)/.lint-go
CACHE_LINT_RUST := $(CACHE_DIR)/.lint-rust
GO_SRC_DIRS     := $(B_WORD)/L0-universal/hybrid $(B_WORD)/L1-omnicode/hybrid \
                    $(B_WORD)/L2-platform/hybrid
GO_TPL_DIR      := $(B_WORD)/seed/code/L0/go
RUST_SRC_DIRS   := $(B_WORD)/L0-universal/hybrid $(B_WORD)/L0-universal/spiral \
                    $(B_WORD)/L1-omnicode/hybrid $(B_WORD)/L2-platform/hybrid

# ═══════════════════════════════════════════════════════════════════════════════
# END SETUP [END]
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
	$(ECHO) "  Layers:  make l0 l1 l2 l3   (build by layer)"
	$(ECHO) "  Update:  make update        (incremental rebuild)"
	$(ECHO) "  Clean:   make clean"
	$(ECHO) ""

## libs: Build all libraries (libtrit + libomni + bereshit-fs)
libs: libtrit libomni bereshit-fs
	$(ECHO) ""
	$(ECHO) "✓ All libraries and tools built"

# ───────────────────────────────────────────────────────────────────────────────
# B.1b LAYER TARGETS — L0-L5 Stack Build [LAYERS]
# ───────────────────────────────────────────────────────────────────────────────
#
# The Kingdom Technology stack: L0 (foundation) → L5 (applications)
# Each target delegates to existing build locations and touches a sentinel.
# As code migrates to b-word/ paths, these targets follow.
#

$(CACHE_DIR):
	$(Q)mkdir -p $(CACHE_DIR)

## l0: Build L0 Universal — libtrit + bereshit-fs (C foundation)
l0: libtrit bereshit-fs | $(CACHE_DIR)
	$(Q)touch $(CACHE_L0)
	$(ECHO) "✓ L0 Universal layer complete"

## l1: Build L1 OmniCode — language library (depends on L0)
l1: l0 libomni | $(CACHE_DIR)
	$(Q)touch $(CACHE_L1)
	$(ECHO) "✓ L1 OmniCode layer complete"

## l2: Build L2 Platform — MillenniumOS (depends on L0)
l2: millenniumos | $(CACHE_DIR)
	$(Q)touch $(CACHE_L2)
	$(ECHO) "✓ L2 Platform layer complete"

## l3: Build L3 CPI-SI — intelligence model (Go)
l3: | $(CACHE_DIR)
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  [L3] Building CPI-SI..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)if [ -f "$(CLAUDE_DIR)/Makefile" ]; then \
		$(MAKE) -C $(CLAUDE_DIR) build; \
	else \
		echo "  word/claude/Makefile not found — skipping"; \
	fi
	$(Q)touch $(CACHE_L3)
	$(ECHO) "✓ L3 CPI-SI layer complete"

## l4: Build L4 FaithNet — server (Go)
l4: server | $(CACHE_DIR)
	$(Q)touch $(CACHE_L4)
	$(ECHO) "✓ L4 FaithNet layer complete"

## l5: Build L5 Applications — Cornerstone (depends on L0+L1)
l5: l0 l1 cornerstone | $(CACHE_DIR)
	$(Q)touch $(CACHE_L5)
	$(ECHO) "✓ L5 Applications layer complete"

## server: Build FaithNet server
server:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  [L4] Building FaithNet server..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)if [ -f "$(SERVER_DIR)/Makefile" ]; then \
		$(MAKE) -C $(SERVER_DIR) build; \
	else \
		echo "  server/Makefile not found — skipping"; \
	fi

# ───────────────────────────────────────────────────────────────────────────────
# B.1c UPDATE TARGETS — Incremental Rebuild [UPDATE]
# ───────────────────────────────────────────────────────────────────────────────
#
# Only rebuild layers whose source has changed since last build.
# Checks sentinel timestamp against source directories.
# Use: make update (all layers) or make update-l3 (single layer)
#

## update: Incremental rebuild — only layers with changes
update: update-l0 update-l1 update-l3 update-l4
	$(ECHO) ""
	$(ECHO) "✓ Incremental update complete"

## update-l0: Rebuild L0 if sources changed since last build
update-l0: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_L0)" ] || \
	    [ -n "$$(find $(TRIT_DIR)/src $(TRIT_DIR)/include -newer $(CACHE_L0) 2>/dev/null)" ] || \
	    [ -n "$$(find $(FUSE_DIR)/src $(FUSE_DIR)/include -newer $(CACHE_L0) 2>/dev/null)" ]; then \
		echo "  L0 sources changed — rebuilding..."; \
		$(MAKE) l0; \
	else \
		echo "  L0 Universal: up to date"; \
	fi

## update-l1: Rebuild L1 if sources changed since last build
update-l1: update-l0 | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_L1)" ] || \
	    [ -n "$$(find $(OMNI_DIR)/src $(OMNI_DIR)/include -newer $(CACHE_L1) 2>/dev/null)" ]; then \
		echo "  L1 sources changed — rebuilding..."; \
		$(MAKE) l1; \
	else \
		echo "  L1 OmniCode: up to date"; \
	fi

## update-l2: Rebuild L2 if sources changed since last build
update-l2: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_L2)" ] || \
	    [ -n "$$(find $(MILLENNIUMOS)/kernel $(MILLENNIUMOS)/boot -newer $(CACHE_L2) 2>/dev/null)" ]; then \
		echo "  L2 sources changed — rebuilding..."; \
		$(MAKE) l2; \
	else \
		echo "  L2 Platform: up to date"; \
	fi

## update-l3: Rebuild L3 if sources changed since last build
update-l3: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_L3)" ] || \
	    [ -n "$$(find $(CLAUDE_DIR)/hooks $(CLAUDE_DIR)/pkg $(CLAUDE_DIR)/statusline \
	        $(CLAUDE_DIR)/dashboard $(CLAUDE_DIR)/skills \
	        -maxdepth 3 -name '*.go' -newer $(CACHE_L3) 2>/dev/null)" ]; then \
		echo "  L3 sources changed — rebuilding..."; \
		$(MAKE) l3; \
	else \
		echo "  L3 CPI-SI: up to date"; \
	fi

## update-l4: Rebuild L4 if sources changed since last build
update-l4: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_L4)" ] || \
	    [ -n "$$(find $(SERVER_DIR) -maxdepth 3 -name '*.go' -newer $(CACHE_L4) 2>/dev/null)" ]; then \
		echo "  L4 sources changed — rebuilding..."; \
		$(MAKE) l4; \
	else \
		echo "  L4 FaithNet: up to date"; \
	fi

## update-l5: Rebuild L5 if sources changed since last build
update-l5: update-l0 update-l1 | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_L5)" ] || \
	    { [ -d "$(CORNERSTONE)/src" ] && \
	      [ -n "$$(find $(CORNERSTONE)/src -newer $(CACHE_L5) 2>/dev/null)" ]; }; then \
		echo "  L5 sources changed — rebuilding..."; \
		$(MAKE) l5; \
	else \
		echo "  L5 Applications: up to date"; \
	fi

# ───────────────────────────────────────────────────────────────────────────────
# B.2 LIBRARY TARGETS — Foundation Builds [LIBRARIES]
# ───────────────────────────────────────────────────────────────────────────────

## libtrit: Build libtrit.a (L0 - foundation)
libtrit:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  [L0] Building libtrit..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(TRIT_DIR)

## libomni: Build libomni.a (L1 - depends on libtrit)
libomni: libtrit
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  [L1] Building libomni..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(OMNI_DIR)

## bereshit-fs: Build zone-aware filesystem (L0 - foundation)
bereshit-fs:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  [L0] Building bereshit-fs..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(FUSE_DIR)

# ───────────────────────────────────────────────────────────────────────────────
# B.3 ENGINE TARGET — Cornerstone Build [ENGINE]
# ───────────────────────────────────────────────────────────────────────────────

## cornerstone: Build cornerstone engine (L5 - depends on libs)
cornerstone: libs
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  [L5] Building cornerstone..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(CORNERSTONE) build

# ───────────────────────────────────────────────────────────────────────────────
# B.3b OS TARGET — MillenniumOS Build [OS]
# ───────────────────────────────────────────────────────────────────────────────

## millenniumos: Build MillenniumOS (L2 - depends on libtrit)
millenniumos: libtrit
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  [L2] Building MillenniumOS..."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(Q)$(MAKE) -C $(MILLENNIUMOS)

# ───────────────────────────────────────────────────────────────────────────────
# B.4 GO TOOLS — Independent Builds [TOOLS]
# ───────────────────────────────────────────────────────────────────────────────

## tools: Build Go tools (L3)
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

## clean: Clean all build artifacts + cache
clean: clean-libs clean-fuse clean-engine clean-os clean-cache
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

## clean-cache: Clean build cache (sentinel files)
clean-cache:
	$(ECHO) "Cleaning build cache..."
	$(Q)rm -rf $(CACHE_DIR)
	$(ECHO) "✓ Build cache cleared"

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

## layer-status: Show layer build cache state
layer-status:
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  Kingdom Technology Stack — Layer Cache"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	@if [ -f "$(CACHE_L0)" ]; then \
		echo "  ✓ L0 Universal     built $$(date -r $(CACHE_L0) '+%Y-%m-%d %H:%M')"; \
	else \
		echo "  ○ L0 Universal     not cached"; \
	fi
	@if [ -f "$(CACHE_L1)" ]; then \
		echo "  ✓ L1 OmniCode      built $$(date -r $(CACHE_L1) '+%Y-%m-%d %H:%M')"; \
	else \
		echo "  ○ L1 OmniCode      not cached"; \
	fi
	@if [ -f "$(CACHE_L2)" ]; then \
		echo "  ✓ L2 Platform      built $$(date -r $(CACHE_L2) '+%Y-%m-%d %H:%M')"; \
	else \
		echo "  ○ L2 Platform      not cached"; \
	fi
	@if [ -f "$(CACHE_L3)" ]; then \
		echo "  ✓ L3 CPI-SI        built $$(date -r $(CACHE_L3) '+%Y-%m-%d %H:%M')"; \
	else \
		echo "  ○ L3 CPI-SI        not cached"; \
	fi
	@if [ -f "$(CACHE_L4)" ]; then \
		echo "  ✓ L4 FaithNet      built $$(date -r $(CACHE_L4) '+%Y-%m-%d %H:%M')"; \
	else \
		echo "  ○ L4 FaithNet      not cached"; \
	fi
	@if [ -f "$(CACHE_L5)" ]; then \
		echo "  ✓ L5 Applications  built $$(date -r $(CACHE_L5) '+%Y-%m-%d %H:%M')"; \
	else \
		echo "  ○ L5 Applications  not cached"; \
	fi
	$(ECHO) ""
	@if [ -d "$(CACHE_DIR)" ]; then \
		cached=$$(ls $(CACHE_DIR)/.l*-built 2>/dev/null | wc -l); \
		echo "  $$cached/6 layers cached"; \
	else \
		echo "  No build cache (run make l0 l3 to populate)"; \
	fi
	$(ECHO) ""

## info: Show configuration
info:
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  Bereshit - Configuration"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "Root Paths:"
	$(ECHO) "  BERESHIT_ROOT:  $(BERESHIT_ROOT)"
	$(ECHO) "  WORD_WORK:      $(WORD_WORK)"
	$(ECHO) "  CLAUDE_DIR:     $(CLAUDE_DIR)"
	$(ECHO) "  SERVER_DIR:     $(SERVER_DIR)"
	$(ECHO) "  CORNERSTONE:    $(CORNERSTONE)"
	$(ECHO) ""
	$(ECHO) "Package Locations:"
	$(ECHO) "  TRIT_DIR:       $(TRIT_DIR)"
	$(ECHO) "  FUSE_DIR:       $(FUSE_DIR)"
	$(ECHO) "  OMNI_DIR:       $(OMNI_DIR)"
	$(ECHO) ""
	$(ECHO) "Layer Paths (target — skeleton planted):"
	$(ECHO) "  L0:  $(L0_LAYER)"
	$(ECHO) "  L1:  $(L1_LAYER)"
	$(ECHO) "  L2:  $(L2_LAYER)"
	$(ECHO) "  L3:  $(L3_LAYER)"
	$(ECHO) "  L4:  $(L4_LAYER)"
	$(ECHO) "  L5:  $(L5_LAYER)"
	$(ECHO) ""
	$(ECHO) "Settings:"
	$(ECHO) "  BUILD_MODE:     $(BUILD_MODE)"
	$(ECHO) "  VERBOSE:        $(if $(VERBOSE),yes,no)"
	$(ECHO) "  CACHE_DIR:      $(CACHE_DIR)"
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
	$(ECHO) "  make update          Incremental rebuild (only changed layers)"
	$(ECHO) "  make run             Run cornerstone (GUI)"
	$(ECHO) "  make test            Run all tests"
	$(ECHO) "  make status          Show build state"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "LAYER TARGETS (Kingdom Technology Stack)"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "    make l0            L0 Universal  — libtrit + bereshit-fs (C)"
	$(ECHO) "    make l1            L1 OmniCode   — libomni (C, depends on L0)"
	$(ECHO) "    make l2            L2 Platform   — MillenniumOS (C, depends on L0)"
	$(ECHO) "    make l3            L3 CPI-SI     — hooks, pkg, skills (Go)"
	$(ECHO) "    make l4            L4 FaithNet   — server (Go)"
	$(ECHO) "    make l5            L5 Applications — cornerstone (C#, depends on L0+L1)"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "UPDATE TARGETS (Incremental — cached)"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "    make update        Rebuild all layers with changes"
	$(ECHO) "    make update-l0     Rebuild L0 if sources changed"
	$(ECHO) "    make update-l1     Rebuild L1 if sources changed"
	$(ECHO) "    make update-l2     Rebuild L2 if sources changed"
	$(ECHO) "    make update-l3     Rebuild L3 if sources changed"
	$(ECHO) "    make update-l4     Rebuild L4 if sources changed"
	$(ECHO) "    make update-l5     Rebuild L5 if sources changed"
	$(ECHO) ""
	$(ECHO) "    Cache: .build-cache/.l*-built sentinels"
	$(ECHO) "    make layer-status  Show cache timestamps"
	$(ECHO) "    make clean-cache   Clear all sentinels"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "COMPONENT TARGETS (backward compatible)"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "    make libs          Build libraries only"
	$(ECHO) "    make libtrit       Build libtrit.a (L0)"
	$(ECHO) "    make bereshit-fs   Build bereshit_fs (L0)"
	$(ECHO) "    make libomni       Build libomni.a (L1)"
	$(ECHO) "    make cornerstone   Build engine (L5)"
	$(ECHO) "    make millenniumos  Build MillenniumOS (L2)"
	$(ECHO) "    make server        Build FaithNet server (L4)"
	$(ECHO) "    make tools         Build Go tools (L3)"
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
	$(ECHO) "B-WORD TARGETS (Go+Rust hybrid — delegates to L0+L1+L2 Makefiles)"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "    make bword-build   Build L0+L1+L2 Go+Rust packages"
	$(ECHO) "    make bword-test    Run L0+L1+L2 Go+Rust tests"
	$(ECHO) "    make bword-check   Full quality gate: Go+Rust across L0+L1+L2"
	$(ECHO) "    make bword-clean   Clean L0+L1+L2 structural caches"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "STRUCTURAL TARGETS (cached sentinels)"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "    make lint          Run all structural linters (Go + Rust)"
	$(ECHO) "    make lint-go       Lint Go 4-block alignment (cached)"
	$(ECHO) "    make lint-rust     Lint Rust 4-block alignment (cached)"
	$(ECHO) "    make go-check      Full Go quality gate: vet + lint + test"
	$(ECHO) "    make go-test       Run Go tests (cached)"
	$(ECHO) "    make go-build      Build Go packages"
	$(ECHO) "    make go-vet        Vet Go packages (cached)"
	$(ECHO) "    make clean-struct  Clear structural caches (root + L0 + L1 + L2)"
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
	$(ECHO) "    make clean         Clean all build artifacts + cache"
	$(ECHO) "    make clean-libs    Clean libraries only"
	$(ECHO) "    make clean-engine  Clean cornerstone only"
	$(ECHO) "    make clean-cache   Clean build cache only"
	$(ECHO) "    make status        Show build state"
	$(ECHO) "    make layer-status  Show layer cache timestamps"
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
	$(ECHO) "KINGDOM TECHNOLOGY STACK"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) ""
	$(ECHO) "  Stream 1 (C):    L0 ──► L1 ──► L2"
	$(ECHO) "  Stream 2 (Go):   L3 ──► L4"
	$(ECHO) "  Stream 3 (C#):   L5"
	$(ECHO) ""
	$(ECHO) "  L0:  libtrit.a     ← Universal foundation (balanced ternary)"
	$(ECHO) "  L0:  bereshit_fs   ← Zone-aware filesystem (FUSE)"
	$(ECHO) "  L1:  libomni.a     ← OmniCode language (depends on L0)"
	$(ECHO) "  L2:  millenniumos  ← Platform (depends on L0)"
	$(ECHO) "  L3:  CPI-SI        ← Intelligence model (Go, independent)"
	$(ECHO) "  L4:  FaithNet      ← Server (depends on L3)"
	$(ECHO) "  L5:  cornerstone   ← Applications (depends on L0+L1)"
	$(ECHO) ""

# ───────────────────────────────────────────────────────────────────────────────
# B.9 STRUCTURAL TARGETS — b-word Build, Test, Lint [STRUCT]
# ───────────────────────────────────────────────────────────────────────────────
#
# Orchestrates the b-word/ Go + Rust workspace across L0 and L1.
# Layer Makefiles handle per-layer caching. Root handles cross-layer ops.
# Force re-run: make clean-struct then make bword-check (or delete sentinels).
#

# ── b-word Orchestration ─────────────────────────────────────────────────────

## bword-build: Build all Go + Rust across L0, L1, and L2
bword-build:
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  [b-word] Building L0 Universal + L1 OmniCode + L2 Platform — Go and Rust..."
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "  [L0] Foundation layer — config loader, format, output, paths, CLI, identity"
	$(Q)$(MAKE) -C $(B_WORD)/L0-universal build
	$(ECHO) ""
	$(ECHO) "  [L1] OmniCode layer — pragma reader, extension registry, format dispatch"
	$(Q)$(MAKE) -C $(B_WORD)/L1-omnicode build
	$(ECHO) ""
	$(ECHO) "  [L2] Platform layer — builder, debugging, display, git, hosts, platform,"
	$(ECHO) "       preferences, server, system, temporal, wezterm"
	$(Q)$(MAKE) -C $(B_WORD)/L2-platform build
	$(ECHO) ""
	$(ECHO) "  ✓ b-word build complete — all three layers compiled"

## bword-test: Run all Go + Rust tests across L0, L1, and L2
bword-test:
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  [b-word] Testing L0 + L1 + L2 — Go and Rust suites..."
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "  [L0] Go: config, util | Rust: cli, config, format, identity, output, paths"
	$(Q)$(MAKE) -C $(B_WORD)/L0-universal test
	$(ECHO) ""
	$(ECHO) "  [L1] Go: pragma | Rust: bereshit-l1-pragma"
	$(Q)$(MAKE) -C $(B_WORD)/L1-omnicode test
	$(ECHO) ""
	$(ECHO) "  [L2] Go + Rust: 11 packages (builder, debugging, display, git, hosts,"
	$(ECHO) "       platform, preferences, server, system, temporal, wezterm)"
	$(Q)$(MAKE) -C $(B_WORD)/L2-platform test
	$(ECHO) ""
	$(ECHO) "  ✓ b-word tests complete — all suites passed"

## bword-check: Full quality gate across L0, L1, and L2 (vet + clippy + lint + fmt + test)
bword-check:
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  [b-word] QUALITY GATE — L0 + L1 + L2 full verification..."
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "  Stage 1/3: L0 Universal — 6 crates, 16 Go files, 19 Rust files"
	$(Q)$(MAKE) -C $(B_WORD)/L0-universal check
	$(ECHO) ""
	$(ECHO) "  Stage 2/3: L1 OmniCode — 1 crate, 4 Go files, 6 Rust files"
	$(Q)$(MAKE) -C $(B_WORD)/L1-omnicode check
	$(ECHO) ""
	$(ECHO) "  Stage 3/3: L2 Platform — 11 crates, 11 Go packages"
	$(Q)$(MAKE) -C $(B_WORD)/L2-platform check
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  b-word QUALITY GATE — ALL PASSED"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  L0  vet · clippy · lint · fmt · test                  ✓"
	$(ECHO) "  L1  vet · clippy · lint · fmt · test                  ✓"
	$(ECHO) "  L2  vet · clippy · lint · fmt · test                  ✓"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "  \"In the beginning God created the heaven and the earth.\""
	$(ECHO) "  Foundation verified. All three layers hold."
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""

## bword-clean: Clean all b-word layer caches and artifacts
bword-clean:
	$(ECHO) ""
	$(ECHO) "  [b-word] Clearing L0 + L1 + L2 build caches..."
	$(Q)$(MAKE) -C $(B_WORD)/L0-universal clean
	$(Q)$(MAKE) -C $(B_WORD)/L1-omnicode clean
	$(Q)$(MAKE) -C $(B_WORD)/L2-platform clean
	$(ECHO) "  ✓ b-word caches cleared"
	$(ECHO) ""

# ── Individual Structural Targets ────────────────────────────────────────────

## lint: Run all structural linters — Go + Rust (cached)
lint: lint-go lint-rust
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  ✓ All structural lints passed — Go and Rust aligned"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""

## lint-go: Lint Go files across L0 + L1 + L2 for 4-block alignment (cached)
lint-go: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_LINT_GO)" ] || \
	    [ -n "$$(find $(GO_SRC_DIRS) -name '*.go' -newer $(CACHE_LINT_GO) 2>/dev/null)" ] || \
	    [ -n "$$(find $(GO_TPL_DIR) -name '*.go' -newer $(CACHE_LINT_GO) 2>/dev/null)" ]; then \
		echo ""; \
		echo "  [lint-go] Checking 4-block alignment across L0 + L1 + L2..."; \
		echo ""; \
		$(CWS_STRUCT) lint go $(B_WORD)/L0-universal/hybrid/ && \
		$(CWS_STRUCT) lint go $(B_WORD)/L1-omnicode/hybrid/ && \
		$(CWS_STRUCT) lint go $(B_WORD)/L2-platform/hybrid/ && \
		$(CWS_STRUCT) lint go $(B_WORD)/seed/code/L0/go/ && \
		touch $(CACHE_LINT_GO) && \
		echo "" && \
		echo "  [lint-go] ✓ All Go files structurally aligned"; \
	else \
		echo "  [lint-go] ✓ Clean — no Go changes since last pass"; \
	fi

## lint-rust: Lint Rust files across L0 + L1 + L2 for 4-block alignment (cached)
lint-rust: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_LINT_RUST)" ] || \
	    [ -n "$$(find $(RUST_SRC_DIRS) -name '*.rs' -newer $(CACHE_LINT_RUST) 2>/dev/null)" ]; then \
		echo ""; \
		echo "  [lint-rust] Checking 4-block alignment across L0 + L1 + L2..."; \
		echo ""; \
		$(CWS_STRUCT) lint rust $(B_WORD)/L0-universal/hybrid/ && \
		$(CWS_STRUCT) lint rust $(B_WORD)/L0-universal/spiral/ && \
		$(CWS_STRUCT) lint rust $(B_WORD)/L1-omnicode/hybrid/ && \
		$(CWS_STRUCT) lint rust $(B_WORD)/L2-platform/hybrid/ && \
		touch $(CACHE_LINT_RUST) && \
		echo "" && \
		echo "  [lint-rust] ✓ All Rust files structurally aligned"; \
	else \
		echo "  [lint-rust] ✓ Clean — no Rust changes since last pass"; \
	fi

## go-test: Run Go tests across L0 + L1 + L2
go-test:
	$(Q)$(MAKE) -C $(B_WORD)/L0-universal test
	$(Q)$(MAKE) -C $(B_WORD)/L1-omnicode test
	$(Q)$(MAKE) -C $(B_WORD)/L2-platform test

## go-build: Build Go packages across L0 + L1 + L2
go-build:
	$(Q)$(MAKE) -C $(B_WORD)/L0-universal build
	$(Q)$(MAKE) -C $(B_WORD)/L1-omnicode build
	$(Q)$(MAKE) -C $(B_WORD)/L2-platform build

## go-vet: Vet Go packages across L0 + L1 + L2
go-vet:
	$(Q)$(MAKE) -C $(B_WORD)/L0-universal vet
	$(Q)$(MAKE) -C $(B_WORD)/L1-omnicode vet
	$(Q)$(MAKE) -C $(B_WORD)/L2-platform vet

## go-check: Full Go quality gate — vet + lint + test across all layers
go-check: go-vet lint-go go-test
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  ✓ Go quality gate passed — vet, lint, test across L0 + L1 + L2"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""

## clean-struct: Clear all structural caches (root + L0 + L1 + L2)
clean-struct:
	$(ECHO) "  [clean] Clearing structural caches — root, L0, L1, L2..."
	$(Q)rm -f $(CACHE_LINT_GO) $(CACHE_LINT_RUST)
	$(Q)$(MAKE) -C $(B_WORD)/L0-universal clean
	$(Q)$(MAKE) -C $(B_WORD)/L1-omnicode clean
	$(Q)$(MAKE) -C $(B_WORD)/L2-platform clean
	$(ECHO) "  ✓ All structural caches cleared"

# ═══════════════════════════════════════════════════════════════════════════════
# END BODY [END]
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
# It orchestrates all sub-builds across the L0-L5 Kingdom Technology stack:
#
#   L0: libtrit.a    (word/work/pkg/trit/)   — Balanced ternary types
#   L0: bereshit_fs  (word/work/pkg/fuse/)   — Zone-aware filesystem
#   L1: libomni.a    (word/work/pkg/omni/)   — OmniCode language library
#   L2: millenniumos (millenniumos/)          — Platform (kernel + bootloader)
#   L3: CPI-SI       (word/claude/)           — Intelligence model
#   L4: FaithNet     (server/)                — Server (HTTP API + website)
#   L5: cornerstone  (cornerstone/)           — Game engine
#
# Each sub-package has its own Makefile that handles its specific build.
# This Makefile only coordinates and delegates.
#
# Build cache: .build-cache/.l*-built sentinel files track layer state.
# Use "make update" for incremental rebuilds, "make clean-cache" to reset.
#
# ───────────────────────────────────────────────────────────────────────────────
# X.2 RELATED FILES [RELATED]
# ───────────────────────────────────────────────────────────────────────────────
#
# Children (called via $(MAKE) -C):
#   word/work/pkg/trit/Makefile   — libtrit.a build (L0)
#   word/work/pkg/fuse/Makefile   — bereshit_fs build (L0)
#   word/work/pkg/omni/Makefile   — libomni.a build (L1)
#   millenniumos/Makefile         — MillenniumOS build (L2)
#   word/claude/Makefile          — CPI-SI build (L3)
#   server/Makefile               — FaithNet build (L4)
#   cornerstone/Makefile          — Cornerstone build (L5)
#   word/work/Makefile            — Go tools
#
# Layer skeleton (planted, code migrates naturally):
#   b-word/L0-universal/          — Target for L0 source
#   b-word/L1-omnicode/           — Target for L1 source
#   b-word/L2-platform/           — Target for L2 source
#   b-word/L3-cpisi/              — Target for L3 source
#   b-word/L4-faithnet/           — Target for L4 source
#   b-word/L5-applications/       — Target for L5 source
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
# END CLOSING [END]
# ═══════════════════════════════════════════════════════════════════════════════
