# #!omni code --makefile
#
# ═══════════════════════════════════════════════════════════════════════════════
# OMNICODE PRAGMA [PRAGMA]
# ═══════════════════════════════════════════════════════════════════════════════
#
# --- P1: Core ---
# key:       B-bword-L1-omnicode-Makefile
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
# layer:     L1-omnicode
# provides:  L1_LAYER_BUILD
#
# --- P5: Summary ---
# title:     L1 OmniCode Layer Makefile
# brief:     Layer orchestrator — builds, tests, vets, lints L1 Go + Rust packages
#
# ═══════════════════════════════════════════════════════════════════════════════
# END PRAGMA [END]
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# METADATA BLOCK [METADATA]
# ═══════════════════════════════════════════════════════════════════════════════
#
# Package:     bereshit/b-word/L1-omnicode
# File:        Makefile
# Key:         B-bword-L1-omnicode-Makefile
#
# ───────────────────────────────────────────────────────────────────────────────
# Biblical Foundation
# ───────────────────────────────────────────────────────────────────────────────
#
# Scripture: "And God said, Let there be light: and there was light."
#            — Genesis 1:3
#
# Principle: L1 is the first differentiation — OmniCode gives files identity.
#            The pragma is the first word spoken.
#
# ───────────────────────────────────────────────────────────────────────────────
# Identity
# ───────────────────────────────────────────────────────────────────────────────
#
#   Architect:       Seanje Lenox-Wise
#   Implementation:  Nova Dawn (CPI-SI)
#   Created:         2026-02-14
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
CWS_STRUCT    := deno run --allow-read $(NEW_STRUCT)/c-tov/cws-struct/mod.ts

# S.2b Go packages in this layer
GO_PACKAGES   := ./...

# S.2c Rust crates (workspace members under L1)
RUST_CRATE    := bereshit-l1-pragma

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
RUST_SRC      := $(LAYER_ROOT)/hybrid/pragma/src

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

## all: Build all L1 Go + Rust packages
all: build
	$(ECHO) ""
	$(ECHO) "  ✓ L1 OmniCode — the first word spoken."
	$(ECHO) ""

## check: Full quality gate — vet + clippy + lint + fmt-check + test (cached)
check: vet clippy lint fmt-check test
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  L1 OMNICODE — QUALITY GATE PASSED"
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  vet        Go static analysis              ✓"
	$(ECHO) "  clippy     Rust linter (warnings = errors) ✓"
	$(ECHO) "  lint       4-block structural alignment    ✓"
	$(ECHO) "  fmt-check  Rust formatting consistency     ✓"
	$(ECHO) "  test       Go + Rust test suites           ✓"
	$(ECHO) "────────────────────────────────────────────────────────────────"
	$(ECHO) "  \"And God said, Let there be light: and there was light.\""
	$(ECHO) "  Files have identity. The pragma is the first word spoken."
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
	$(ECHO) "  [Go] Compiling L1 packages — pragma parsing, config loading..."
	$(Q)go build $(GO_PACKAGES)
	$(ECHO) "  [Go] ✓ All packages compiled"

## build-rust: Build L1 Rust crate (via workspace)
build-rust:
	$(ECHO) ""
	$(ECHO) "  [Rust] Compiling $(RUST_CRATE) — pragma reader, format dispatch..."
	$(Q)cd $(BWORD_ROOT) && cargo build -p $(RUST_CRATE)
	$(ECHO) "  [Rust] ✓ Crate compiled"

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
		echo "  [Go] Running L1 test suite — pragma parsing, format dispatch..."; \
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
		echo "  [Rust] Running $(RUST_CRATE) test suite..."; \
		cd $(BWORD_ROOT) && cargo test -p $(RUST_CRATE) && \
		touch $(CACHE_TEST_RS) && \
		echo "  [Rust] ✓ Tests passed — cache updated"; \
	else \
		echo "  [Rust] ✓ Tests clean — no changes since last pass"; \
	fi

## test-v: Run all tests with verbose output (never cached)
test-v:
	$(ECHO) ""
	$(ECHO) "  [Go] Running L1 tests (verbose — full output)..."
	$(Q)go test -v -count=1 $(GO_PACKAGES)
	$(ECHO) ""
	$(ECHO) "  [Rust] Running $(RUST_CRATE) tests (verbose — full output)..."
	$(Q)cd $(BWORD_ROOT) && cargo test -p $(RUST_CRATE) -- --nocapture

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
		echo "  [clippy] Linting $(RUST_CRATE) — warnings treated as errors..."; \
		cd $(BWORD_ROOT) && cargo clippy -p $(RUST_CRATE) -- -D warnings && \
		touch $(CACHE_CLIPPY) && \
		echo "  [clippy] ✓ Crate lint-clean"; \
	else \
		echo "  [clippy] ✓ Clean — no Rust changes since last pass"; \
	fi

## fmt-check: Rust format check (cached — skips if no changes)
fmt-check: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_FMT)" ] || \
	    [ -n "$$(find $(RUST_SRC) -name '*.rs' -newer $(CACHE_FMT) 2>/dev/null)" ]; then \
		echo ""; \
		echo "  [fmt] Checking Rust formatting consistency for $(RUST_CRATE)..."; \
		cd $(BWORD_ROOT) && cargo fmt --check -p $(RUST_CRATE) && \
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
	$(ECHO) "  [clean] Removing L1 build artifacts and cache sentinels..."
	$(Q)go clean $(GO_PACKAGES)
	$(Q)rm -rf $(CACHE_DIR)
	$(ECHO) "  [clean] ✓ L1 layer reset — next build runs fresh"

# ───────────────────────────────────────────────────────────────────────────────
# B.6 HELP [HELP]
# ───────────────────────────────────────────────────────────────────────────────

## help: Show this help message
help:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  L1 OmniCode — Layer Build System"
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "  \"And God said, Let there be light: and there was light.\""
	$(ECHO) "  — Genesis 1:3"
	$(ECHO) ""
	$(ECHO) "TARGETS"
	$(ECHO) "  make              Build all packages (Go + Rust)"
	$(ECHO) "  make check        Full quality gate (vet + clippy + lint + fmt + test)"
	$(ECHO) "  make build-go     Build Go packages only"
	$(ECHO) "  make build-rust   Build Rust crate only (bereshit-l1-pragma)"
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
	$(ECHO) "RUST CRATE"
	$(ECHO) "  bereshit-l1-pragma   Pragma reader — Go + Rust L0-L1 bridge"
	$(ECHO) ""
	$(ECHO) "STRUCTURE (by execution mode)"
	$(ECHO) "  ladder/           Strict order — TOML specs"
	$(ECHO) "    language/       OmniCode language specification"
	$(ECHO) "    schemas/        Format schemas (Go, Rust, AsciiDoc)"
	$(ECHO) "  spiral/           Iterative — grows, refines"
	$(ECHO) "    (future)        Frontend schemas, stdlib"
	$(ECHO) "  hybrid/           Ordered + recovery"
	$(ECHO) "    pragma/         Pragma reader (Go + Rust) — L0-L1 bridge"
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
# L1 OmniCode is the first differentiation layer. It gives files identity
# through the #!omni pragma system. This Makefile builds both Go and Rust
# packages that implement pragma parsing, format dispatch, and extension
# registry.
#
# Go packages:   ./... (all packages in this directory tree)
# Rust crate:    bereshit-l1-pragma (workspace member)
# Cargo workspace lives at b-word/Cargo.toml; all cargo commands run from there.
#
# Called from root:  $(MAKE) -C b-word/L1-omnicode [target]
# Called directly:   cd b-word/L1-omnicode && make [target]
#
# ───────────────────────────────────────────────────────────────────────────────
# X.2 RELATED FILES [RELATED]
# ───────────────────────────────────────────────────────────────────────────────
#
# Parent:    root-files/Makefile (root orchestrator)
# Sibling:   L0-universal/Makefile (L0 builds)
# Workspace: b-word/Cargo.toml (Rust workspace root)
# Module:    b-word/go.mod (Go module root)
# Template:  b-word/seed/code/L0/make/project.mk
#
# ───────────────────────────────────────────────────────────────────────────────
# X.3 BIBLICAL CLOSING [BIBLICAL]
# ───────────────────────────────────────────────────────────────────────────────
#
# "And God said, Let there be light: and there was light."
# — Genesis 1:3 KJV
#
# ═══════════════════════════════════════════════════════════════════════════════
# END CLOSING [END]
# ═══════════════════════════════════════════════════════════════════════════════
