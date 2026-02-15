# #!omni code --makefile
#
# ═══════════════════════════════════════════════════════════════════════════════
# OMNICODE PRAGMA [PRAGMA]
# ═══════════════════════════════════════════════════════════════════════════════
#
# --- P1: Core ---
# key:       B-bword-L0-universal-Makefile
# from:      B-seed-code-make-project
# at:        a-02.00
#
# --- P2: Family ---
# type:      code
# subtype:   makefile
# role:      layer-build
# structure: 4-block
#
# --- P3: Instance ---
# component: build-system
# layer:     L0-universal
# provides:  L0_LAYER_BUILD
#
# --- P5: Summary ---
# title:     L0 Universal Layer Makefile
# brief:     Layer orchestrator — builds, tests, vets, lints L0 Go + Rust packages
#
# ═══════════════════════════════════════════════════════════════════════════════
# END PRAGMA [END]
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# METADATA BLOCK [METADATA]
# ═══════════════════════════════════════════════════════════════════════════════
#
# Package:     bereshit/b-word/L0-universal
# File:        Makefile
# Key:         B-bword-L0-universal-Makefile
#
# ───────────────────────────────────────────────────────────────────────────────
# Biblical Foundation
# ───────────────────────────────────────────────────────────────────────────────
#
# Scripture: "In the beginning God created the heaven and the earth."
#            — Genesis 1:1
#
# Principle: L0 is the universal foundation. Everything builds from here.
#
# ───────────────────────────────────────────────────────────────────────────────
# Identity
# ───────────────────────────────────────────────────────────────────────────────
#
#   Architect:       Seanje Lenox-Wise
#   Implementation:  Nova Dawn (CPI-SI)
#   Created:         2026-02-13
#   Updated:         2026-02-15
#   Version:         a-02.00
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

.PHONY: all build test vet lint check clean help
.PHONY: build-go build-rust test-go test-rust test-v
.PHONY: clippy fmt-check

.DEFAULT_GOAL := all

# ───────────────────────────────────────────────────────────────────────────────
# S.2 VARIABLES — Configuration [VARIABLES]
# ───────────────────────────────────────────────────────────────────────────────

# S.2a Paths
LAYER_ROOT    := $(CURDIR)
BWORD_ROOT    := $(realpath $(LAYER_ROOT)/..)
NEW_STRUCT    := $(realpath $(LAYER_ROOT)/../..)
BERESHIT_ROOT := $(realpath $(NEW_STRUCT)/..)
CWS_STRUCT    := deno run --allow-read $(NEW_STRUCT)/c-tov/cws-struct/mod.ts

# S.2b Go packages in this layer
GO_PACKAGES   := ./...

# S.2c Rust crates (workspace members under L0)
RUST_CRATES   := bereshit-l0-cli \
                 bereshit-l0-config \
                 bereshit-l0-format \
                 bereshit-l0-output \
                 bereshit-l0-paths \
                 bereshit-l0-identity

# S.2d Output control
VERBOSE       ?=
Q             := $(if $(VERBOSE),,@)
ECHO          := @echo

# S.2e Build cache
CACHE_DIR     := $(LAYER_ROOT)/.cache
CACHE_TEST_GO := $(CACHE_DIR)/.test-go
CACHE_TEST_RS := $(CACHE_DIR)/.test-rust
CACHE_VET     := $(CACHE_DIR)/.vet
CACHE_LINT    := $(CACHE_DIR)/.lint
CACHE_CLIPPY  := $(CACHE_DIR)/.clippy
CACHE_FMT     := $(CACHE_DIR)/.fmt

# S.2f Source directories for change detection
GO_SRC        := $(LAYER_ROOT)
RUST_SRC      := $(LAYER_ROOT)/hybrid $(LAYER_ROOT)/spiral

# ═══════════════════════════════════════════════════════════════════════════════
# END SETUP [END]
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# BODY BLOCK [BODY]
# ═══════════════════════════════════════════════════════════════════════════════

# ───────────────────────────────────────────────────────────────────────────────
# B.1 ENTRY POINTS [ENTRY]
# ───────────────────────────────────────────────────────────────────────────────

$(CACHE_DIR):
	$(Q)mkdir -p $(CACHE_DIR)

## all: Build all L0 Go + Rust packages
all: build
	$(ECHO) ""
	$(ECHO) "  ✓ L0 Universal — the foundation stands."
	$(ECHO) ""

## check: Full quality gate — vet + clippy + lint + fmt-check + test (cached)
check: vet clippy lint fmt-check test
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  L0 UNIVERSAL — QUALITY GATE PASSED"
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  vet        Go static analysis              ✓"
	$(ECHO) "  clippy     Rust linter (warnings = errors) ✓"
	$(ECHO) "  lint       4-block structural alignment    ✓"
	$(ECHO) "  fmt-check  Rust formatting consistency     ✓"
	$(ECHO) "  test       Go + Rust test suites           ✓"
	$(ECHO) "────────────────────────────────────────────────────────────────"
	$(ECHO) "  \"In the beginning God created the heaven and the earth.\""
	$(ECHO) "  The foundation holds. Everything builds from here."
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) ""

# ───────────────────────────────────────────────────────────────────────────────
# B.2 BUILD TARGETS [BUILD]
# ───────────────────────────────────────────────────────────────────────────────

## build: Build Go + Rust packages
build: build-go build-rust

## build-go: Build all Go packages in this layer
build-go:
	$(ECHO) ""
	$(ECHO) "  [Go] Compiling L0 packages — config loader, path resolver, formatters..."
	$(Q)go build $(GO_PACKAGES)
	$(ECHO) "  [Go] ✓ All packages compiled"

## build-rust: Build L0 Rust crates (via workspace)
build-rust:
	$(ECHO) ""
	$(ECHO) "  [Rust] Compiling 6 L0 crates via Cargo workspace..."
	$(ECHO) "         cli · config · format · output · paths · identity"
	$(Q)cd $(BWORD_ROOT) && cargo build $(foreach crate,$(RUST_CRATES),-p $(crate))
	$(ECHO) "  [Rust] ✓ All 6 crates compiled"

# ───────────────────────────────────────────────────────────────────────────────
# B.3 TEST TARGETS (cached) [TEST]
# ───────────────────────────────────────────────────────────────────────────────

## test: Run Go + Rust tests (cached)
test: test-go test-rust

## test-go: Run Go tests (cached — skips if no changes)
test-go: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_TEST_GO)" ] || \
	    [ -n "$$(find $(GO_SRC) -name '*.go' -newer $(CACHE_TEST_GO) 2>/dev/null)" ]; then \
		echo ""; \
		echo "  [Go] Running L0 test suite — config loading, path resolution..."; \
		go test -count=1 $(GO_PACKAGES) && \
		touch $(CACHE_TEST_GO) && \
		echo "  [Go] ✓ Tests passed — cache updated"; \
	else \
		echo "  [Go] ✓ Tests clean — no changes since last pass"; \
	fi

## test-rust: Run Rust tests (cached — skips if no changes)
test-rust: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_TEST_RS)" ] || \
	    [ -n "$$(find $(RUST_SRC) -name '*.rs' -newer $(CACHE_TEST_RS) 2>/dev/null)" ]; then \
		echo ""; \
		echo "  [Rust] Running L0 test suite across 6 crates..."; \
		cd $(BWORD_ROOT) && cargo test $(foreach crate,$(RUST_CRATES),-p $(crate)) && \
		touch $(CACHE_TEST_RS) && \
		echo "  [Rust] ✓ Tests passed — cache updated"; \
	else \
		echo "  [Rust] ✓ Tests clean — no changes since last pass"; \
	fi

## test-v: Run all tests with verbose output (never cached)
test-v:
	$(ECHO) ""
	$(ECHO) "  [Go] Running L0 tests (verbose — full output)..."
	$(Q)go test -v -count=1 $(GO_PACKAGES)
	$(ECHO) ""
	$(ECHO) "  [Rust] Running L0 tests (verbose — full output, 6 crates)..."
	$(Q)cd $(BWORD_ROOT) && cargo test $(foreach crate,$(RUST_CRATES),-p $(crate)) -- --nocapture

# ───────────────────────────────────────────────────────────────────────────────
# B.4 STATIC ANALYSIS (cached) [ANALYSIS]
# ───────────────────────────────────────────────────────────────────────────────

## vet: Go static analysis (cached — skips if no changes)
vet: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_VET)" ] || \
	    [ -n "$$(find $(GO_SRC) -name '*.go' -newer $(CACHE_VET) 2>/dev/null)" ]; then \
		echo ""; \
		echo "  [vet] Analyzing Go packages for correctness issues..."; \
		go vet $(GO_PACKAGES) && \
		touch $(CACHE_VET) && \
		echo "  [vet] ✓ Static analysis passed — no issues found"; \
	else \
		echo "  [vet] ✓ Clean — no Go changes since last pass"; \
	fi

## clippy: Rust linter (cached — skips if no changes)
clippy: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_CLIPPY)" ] || \
	    [ -n "$$(find $(RUST_SRC) -name '*.rs' -newer $(CACHE_CLIPPY) 2>/dev/null)" ]; then \
		echo ""; \
		echo "  [clippy] Linting 6 Rust crates — warnings treated as errors..."; \
		cd $(BWORD_ROOT) && cargo clippy $(foreach crate,$(RUST_CRATES),-p $(crate)) -- -D warnings && \
		touch $(CACHE_CLIPPY) && \
		echo "  [clippy] ✓ All 6 crates lint-clean"; \
	else \
		echo "  [clippy] ✓ Clean — no Rust changes since last pass"; \
	fi

## fmt-check: Rust format check (cached — skips if no changes)
fmt-check: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_FMT)" ] || \
	    [ -n "$$(find $(RUST_SRC) -name '*.rs' -newer $(CACHE_FMT) 2>/dev/null)" ]; then \
		echo ""; \
		echo "  [fmt] Checking Rust formatting consistency across L0..."; \
		cd $(BWORD_ROOT) && cargo fmt --check $(foreach crate,$(RUST_CRATES),-p $(crate)) && \
		touch $(CACHE_FMT) && \
		echo "  [fmt] ✓ All files formatted correctly"; \
	else \
		echo "  [fmt] ✓ Clean — no Rust changes since last pass"; \
	fi

## lint: Structural alignment lint (cached — skips if no changes)
lint: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_LINT)" ] || \
	    [ -n "$$(find $(GO_SRC) \( -name '*.go' -o -name '*.rs' \) -newer $(CACHE_LINT) 2>/dev/null)" ]; then \
		echo ""; \
		echo "  [lint] Checking 4-block structural alignment..."; \
		echo "         Go files  — METADATA > SETUP > BODY > CLOSING"; \
		$(CWS_STRUCT) lint go $(LAYER_ROOT) --errors-only && \
		echo "         Rust files — METADATA > SETUP > BODY > CLOSING"; \
		$(CWS_STRUCT) lint rust $(LAYER_ROOT) --errors-only && \
		touch $(CACHE_LINT) && \
		echo "  [lint] ✓ All Go + Rust files structurally aligned"; \
	else \
		echo "  [lint] ✓ Clean — no source changes since last pass"; \
	fi

# ───────────────────────────────────────────────────────────────────────────────
# B.5 MAINTENANCE [MAINTENANCE]
# ───────────────────────────────────────────────────────────────────────────────

## clean: Clean Go build cache + layer cache
clean:
	$(ECHO) ""
	$(ECHO) "  [clean] Removing L0 build artifacts and cache sentinels..."
	$(Q)go clean $(GO_PACKAGES)
	$(Q)rm -rf $(CACHE_DIR)
	$(ECHO) "  [clean] ✓ L0 layer reset — next build runs fresh"

# ───────────────────────────────────────────────────────────────────────────────
# B.6 HELP [HELP]
# ───────────────────────────────────────────────────────────────────────────────

## help: Show this help message
help:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  L0 Universal — Layer Build System"
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "  \"In the beginning God created the heaven and the earth.\""
	$(ECHO) "  — Genesis 1:1"
	$(ECHO) ""
	$(ECHO) "TARGETS"
	$(ECHO) "  make              Build all packages (Go + Rust)"
	$(ECHO) "  make check        Full quality gate (vet + clippy + lint + fmt + test)"
	$(ECHO) "  make build-go     Build Go packages only"
	$(ECHO) "  make build-rust   Build Rust crates only (6 crates)"
	$(ECHO) "  make test         Run all tests (cached)"
	$(ECHO) "  make test-go      Run Go tests only (cached)"
	$(ECHO) "  make test-rust    Run Rust tests only (cached)"
	$(ECHO) "  make test-v       Run all tests verbose (not cached)"
	$(ECHO) "  make vet          Go static analysis (cached)"
	$(ECHO) "  make clippy       Rust linter (cached)"
	$(ECHO) "  make fmt-check    Rust format check (cached)"
	$(ECHO) "  make lint         4-block structural lint — Go + Rust (cached)"
	$(ECHO) "  make clean        Clean build + cache"
	$(ECHO) "  make help         This message"
	$(ECHO) ""
	$(ECHO) "OPTIONS"
	$(ECHO) "  VERBOSE=1         Show full commands"
	$(ECHO) ""
	$(ECHO) "CACHE"
	$(ECHO) "  Sentinels: .cache/.test-go .cache/.test-rust .cache/.vet"
	$(ECHO) "             .cache/.lint .cache/.clippy .cache/.fmt"
	$(ECHO) "  Skips re-run if no source files changed since last pass."
	$(ECHO) "  make clean resets cache."
	$(ECHO) ""
	$(ECHO) "RUST CRATES (6)"
	$(ECHO) "  bereshit-l0-cli        CLI scaffolding"
	$(ECHO) "  bereshit-l0-config     Configuration loader"
	$(ECHO) "  bereshit-l0-format     Pure value formatting"
	$(ECHO) "  bereshit-l0-output     Terminal output rendering"
	$(ECHO) "  bereshit-l0-paths      System-wide path resolution"
	$(ECHO) "  bereshit-l0-identity   OmniCode identity registry"
	$(ECHO) ""
	$(ECHO) "STRUCTURE (by execution mode)"
	$(ECHO) "  ladder/           Strict order — specs, compiled libs"
	$(ECHO) "    foundation/     TOML type specifications (60+ files)"
	$(ECHO) "    libtrit/        C balanced ternary library (scaffold)"
	$(ECHO) "    libmath/        C math library (scaffold)"
	$(ECHO) "  spiral/           Iterative — grows, refines"
	$(ECHO) "    identity/       OmniCode package identity registry (Rust)"
	$(ECHO) "  hybrid/           Ordered + recovery"
	$(ECHO) "    cli/            CLI scaffolding (Rust)"
	$(ECHO) "    config/         Configuration loader (Rust + Go)"
	$(ECHO) "    format/         Pure value formatting (Rust)"
	$(ECHO) "    output/         Terminal output rendering (Rust)"
	$(ECHO) "    paths/          System-wide path resolution (Rust)"
	$(ECHO) ""

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
# L0 Universal is the foundation layer of the Kingdom Technology stack.
# This Makefile builds and tests both Go and Rust packages in b-word/L0-universal.
#
# Go packages:   ./... (all packages in this directory tree)
# Rust crates:   6 workspace members (cli, config, format, output, paths, identity)
# Cargo workspace lives at b-word/Cargo.toml; all cargo commands run from there.
#
# Called from root:  $(MAKE) -C b-word/L0-universal [target]
# Called directly:   cd b-word/L0-universal && make [target]
#
# ───────────────────────────────────────────────────────────────────────────────
# X.2 RELATED FILES [RELATED]
# ───────────────────────────────────────────────────────────────────────────────
#
# Parent:    root-files/Makefile (root orchestrator)
# Sibling:   L1-omnicode/Makefile (L1 layer build)
# Workspace: b-word/Cargo.toml (Rust workspace root)
# Module:    b-word/go.mod (Go module root)
# Template:  b-word/seed/code/L0/make/project.mk
#
# ───────────────────────────────────────────────────────────────────────────────
# X.3 BIBLICAL CLOSING [BIBLICAL]
# ───────────────────────────────────────────────────────────────────────────────
#
# "In the beginning God created the heaven and the earth."
# — Genesis 1:1 KJV
#
# ═══════════════════════════════════════════════════════════════════════════════
# END CLOSING [END]
# ═══════════════════════════════════════════════════════════════════════════════
