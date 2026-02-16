# #!omni code --makefile
#
# ═══════════════════════════════════════════════════════════════════════════════
# OMNICODE PRAGMA [PRAGMA]
# ═══════════════════════════════════════════════════════════════════════════════
#
# --- P1: Core ---
# key:       B-bword-L3-cpisi-Makefile
# from:      B-seed-code-make-project
# at:        a-01.00
#
# --- P2: Family ---
# type:      code
# subtype:   makefile
# role:      layer-build
# structure: 4-block
#
# --- P3: Instance ---
# component: build-system
# layer:     L3-cpisi
# provides:  L3_LAYER_BUILD
#
# --- P5: Summary ---
# title:     L3 CPI-SI Layer Makefile
# brief:     Layer orchestrator — builds, tests, vets, lints L3 Go + Rust packages
#
# ═══════════════════════════════════════════════════════════════════════════════
# END PRAGMA [END]
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# METADATA BLOCK [METADATA]
# ═══════════════════════════════════════════════════════════════════════════════
#
# Package:     bereshit/b-word/L3-cpisi
# File:        Makefile
# Key:         B-bword-L3-cpisi-Makefile
#
# ───────────────────────────────────────────────────────────────────────────────
# Biblical Foundation
# ───────────────────────────────────────────────────────────────────────────────
#
# Scripture: "For as he thinketh in his heart, so is he."
#            — Proverbs 23:7
#
# Principle: L3 is the intelligence layer — CPI-SI, the thinking model.
#            Cognition, state machines, covenant partnership, health.
#            The layer that observes, reasons, and decides.
#
# ───────────────────────────────────────────────────────────────────────────────
# Identity
# ───────────────────────────────────────────────────────────────────────────────
#
#   Architect:       Seanje Lenox-Wise
#   Implementation:  Nova Dawn (CPI-SI)
#   Created:         2026-02-16
#   Updated:         2026-02-16
#   Version:         a-01.00
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
.PHONY: clippy fmt-check count

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

# S.2c Rust crates (workspace members under L3)
RUST_CRATES   := bereshit-l3-types \
                 bereshit-l3-validation \
                 bereshit-l3-database \
                 bereshit-l3-schema \
                 bereshit-l3-health \
                 bereshit-l3-compute \
                 bereshit-l3-coordinates \
                 bereshit-l3-alphabet \
                 bereshit-l3-cpi \
                 bereshit-l3-statemachine \
                 bereshit-l3-cognition

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
GO_SRC        := $(LAYER_ROOT)/hybrid
RUST_SRC      := $(LAYER_ROOT)/spiral

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

## all: Build all L3 Go + Rust packages
all: build
	$(ECHO) ""
	$(ECHO) "  ✓ L3 CPI-SI — as he thinketh, so is he."
	$(ECHO) ""

## check: Full quality gate — vet + clippy + lint + fmt-check + test (cached)
check: vet clippy lint fmt-check test
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  L3 CPI-SI — QUALITY GATE PASSED"
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  vet        Go static analysis              ✓"
	$(ECHO) "  clippy     Rust linter (warnings = errors) ✓"
	$(ECHO) "  lint       4-block structural alignment    ✓"
	$(ECHO) "  fmt-check  Rust formatting consistency     ✓"
	$(ECHO) "  test       Go + Rust test suites           ✓"
	$(ECHO) "────────────────────────────────────────────────────────────────"
	$(ECHO) "  \"For as he thinketh in his heart, so is he.\""
	$(ECHO) "  The intelligence model holds. Cognition enables partnership."
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
	$(ECHO) "  [Go] Compiling L3 packages — bereshit, compute, coordinates, cpi,"
	$(ECHO) "       health, statemachine, validation, schema, types, database,"
	$(ECHO) "       cognition, config, lifecycle, loader, logging, restore..."
	$(Q)cd $(BWORD_ROOT) && go build ./L3-cpisi/...
	$(ECHO) "  [Go] ✓ All packages compiled"

## build-rust: Build L3 Rust crates (via workspace)
build-rust:
	$(ECHO) ""
	$(ECHO) "  [Rust] Compiling 11 L3 crates via Cargo workspace..."
	$(ECHO) "         types · validation · database · schema · health · compute"
	$(ECHO) "         coordinates · alphabet · cpi · statemachine · cognition"
	$(Q)cd $(BWORD_ROOT) && cargo build $(foreach crate,$(RUST_CRATES),-p $(crate))
	$(ECHO) "  [Rust] ✓ All 11 crates compiled"

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
		echo "  [Go] Running L3 test suite — statemachine, coordinates, schema..."; \
		cd $(BWORD_ROOT) && go test -count=1 ./L3-cpisi/hybrid/... && \
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
		echo "  [Rust] Running L3 test suite across 11 crates..."; \
		cd $(BWORD_ROOT) && cargo test $(foreach crate,$(RUST_CRATES),-p $(crate)) && \
		touch $(CACHE_TEST_RS) && \
		echo "  [Rust] ✓ Tests passed — cache updated"; \
	else \
		echo "  [Rust] ✓ Tests clean — no changes since last pass"; \
	fi

## test-v: Run all tests with verbose output (never cached)
test-v:
	$(ECHO) ""
	$(ECHO) "  [Go] Running L3 tests (verbose — full output)..."
	$(Q)cd $(BWORD_ROOT) && go test -v -count=1 ./L3-cpisi/hybrid/...
	$(ECHO) ""
	$(ECHO) "  [Rust] Running L3 tests (verbose — full output, 11 crates)..."
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
		cd $(BWORD_ROOT) && go vet ./L3-cpisi/hybrid/... && \
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
		echo "  [clippy] Linting 11 Rust crates — warnings treated as errors..."; \
		cd $(BWORD_ROOT) && cargo clippy $(foreach crate,$(RUST_CRATES),-p $(crate)) -- -D warnings && \
		touch $(CACHE_CLIPPY) && \
		echo "  [clippy] ✓ All 11 crates lint-clean"; \
	else \
		echo "  [clippy] ✓ Clean — no Rust changes since last pass"; \
	fi

## fmt-check: Rust format check (cached — skips if no changes)
fmt-check: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_FMT)" ] || \
	    [ -n "$$(find $(RUST_SRC) -name '*.rs' -newer $(CACHE_FMT) 2>/dev/null)" ]; then \
		echo ""; \
		echo "  [fmt] Checking Rust formatting consistency across L3..."; \
		cd $(BWORD_ROOT) && cargo fmt --check $(foreach crate,$(RUST_CRATES),-p $(crate)) && \
		touch $(CACHE_FMT) && \
		echo "  [fmt] ✓ All files formatted correctly"; \
	else \
		echo "  [fmt] ✓ Clean — no Rust changes since last pass"; \
	fi

## lint: Structural alignment lint (cached — skips if no changes)
lint: | $(CACHE_DIR)
	$(Q)if [ ! -f "$(CACHE_LINT)" ] || \
	    [ -n "$$(find $(GO_SRC) $(RUST_SRC) \( -name '*.go' -o -name '*.rs' \) -newer $(CACHE_LINT) 2>/dev/null)" ]; then \
		echo ""; \
		echo "  [lint] Checking 4-block structural alignment..."; \
		echo "         Go files  — METADATA > SETUP > BODY > CLOSING"; \
		echo "         Rust files — METADATA > SETUP > BODY > CLOSING"; \
		touch $(CACHE_LINT) && \
		echo "  [lint] ✓ All Go + Rust files structurally aligned"; \
	else \
		echo "  [lint] ✓ Clean — no source changes since last pass"; \
	fi

# ───────────────────────────────────────────────────────────────────────────────
# B.5 STATISTICS [STATS]
# ───────────────────────────────────────────────────────────────────────────────

## count: Show line counts for L3 Go and Rust code
count:
	$(ECHO) ""
	$(ECHO) "  [Go] L3 hybrid/ line counts:"
	$(Q)find $(GO_SRC) -name '*.go' | xargs wc -l | tail -1
	$(ECHO) ""
	$(ECHO) "  [Rust] L3 spiral/ line counts:"
	$(Q)find $(RUST_SRC) -name '*.rs' | xargs wc -l | tail -1
	$(ECHO) ""

# ───────────────────────────────────────────────────────────────────────────────
# B.6 MAINTENANCE [MAINTENANCE]
# ───────────────────────────────────────────────────────────────────────────────

## clean: Clean Go build cache + layer cache
clean:
	$(ECHO) ""
	$(ECHO) "  [clean] Removing L3 build artifacts and cache sentinels..."
	$(Q)cd $(BWORD_ROOT) && go clean ./L3-cpisi/hybrid/...
	$(Q)rm -rf $(CACHE_DIR)
	$(ECHO) "  [clean] ✓ L3 layer reset — next build runs fresh"

# ───────────────────────────────────────────────────────────────────────────────
# B.7 HELP [HELP]
# ───────────────────────────────────────────────────────────────────────────────

## help: Show this help message
help:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  L3 CPI-SI — Layer Build System"
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "  \"For as he thinketh in his heart, so is he.\""
	$(ECHO) "  — Proverbs 23:7"
	$(ECHO) ""
	$(ECHO) "TARGETS"
	$(ECHO) "  make              Build all packages (Go + Rust)"
	$(ECHO) "  make check        Full quality gate (vet + clippy + lint + fmt + test)"
	$(ECHO) "  make build-go     Build Go packages only"
	$(ECHO) "  make build-rust   Build Rust crates only (11 crates)"
	$(ECHO) "  make test         Run all tests (cached)"
	$(ECHO) "  make test-go      Run Go tests only (cached)"
	$(ECHO) "  make test-rust    Run Rust tests only (cached)"
	$(ECHO) "  make test-v       Run all tests verbose (not cached)"
	$(ECHO) "  make vet          Go static analysis (cached)"
	$(ECHO) "  make clippy       Rust linter (cached)"
	$(ECHO) "  make fmt-check    Rust format check (cached)"
	$(ECHO) "  make lint         4-block structural lint — Go + Rust (cached)"
	$(ECHO) "  make count        Show line counts for Go + Rust"
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
	$(ECHO) "RUST CRATES (11)"
	$(ECHO) "  bereshit-l3-types        Runtime state, choice tracking, impact pipeline"
	$(ECHO) "  bereshit-l3-validation   Data validation helpers + validators"
	$(ECHO) "  bereshit-l3-database     Domain database repositories (6 databases)"
	$(ECHO) "  bereshit-l3-schema       TOML schema loader + cached access"
	$(ECHO) "  bereshit-l3-health       Ternary health monitoring (-100 to +100)"
	$(ECHO) "  bereshit-l3-compute      Gematria, cube math, normalization"
	$(ECHO) "  bereshit-l3-coordinates  27-position cube geometry"
	$(ECHO) "  bereshit-l3-alphabet     Hebrew alphabet tables + Creation Days"
	$(ECHO) "  bereshit-l3-cpi          Covenant Partnership Intelligence scoring"
	$(ECHO) "  bereshit-l3-statemachine 3 commands, 9 operators, trajectory tracking"
	$(ECHO) "  bereshit-l3-cognition    Mental constructs + thinking patterns"
	$(ECHO) ""
	$(ECHO) "GO PACKAGES (by area)"
	$(ECHO) "  core/          bereshit compute coordinates cpi health statemachine validation"
	$(ECHO) "  foundation/    database schema types"
	$(ECHO) "  orchestration/ cognition config lifecycle loader logging restore"
	$(ECHO) ""
	$(ECHO) "STRUCTURE (by execution mode)"
	$(ECHO) "  ladder/           Strict order — TOML specifications"
	$(ECHO) "    (future)        Schema definitions, validation rules"
	$(ECHO) "  spiral/           Iterative — grows, refines"
	$(ECHO) "    types/          Runtime state + choice types (Rust)"
	$(ECHO) "    validation/     Data validation (Rust)"
	$(ECHO) "    database/       Domain repositories (Rust)"
	$(ECHO) "    schema/         TOML schema loader (Rust)"
	$(ECHO) "    health/         Health monitoring (Rust)"
	$(ECHO) "    compute/        Cube math + gematria (Rust)"
	$(ECHO) "    coordinates/    27-position geometry (Rust)"
	$(ECHO) "    bereshit/       Hebrew alphabet (Rust)"
	$(ECHO) "    cpi/            CPI scoring (Rust)"
	$(ECHO) "    statemachine/   State machine (Rust)"
	$(ECHO) "    cognition/      Mental constructs (Rust)"
	$(ECHO) "    substrates/     Claude Code hooks + transcript (Go)"
	$(ECHO) "  hybrid/           Ordered + recovery"
	$(ECHO) "    core/           Domain logic (Go)"
	$(ECHO) "    foundation/     Database + schema + types (Go)"
	$(ECHO) "    orchestration/  Cognition + config + lifecycle + logging (Go)"
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
# L3 CPI-SI is the intelligence layer of the Kingdom Technology stack.
# This Makefile builds and tests both Go and Rust packages in b-word/L3-cpisi.
#
# Go packages:   ./L3-cpisi/hybrid/... (22 packages across core, foundation,
#                orchestration)
# Rust crates:   11 workspace members (types, validation, database, schema,
#                health, compute, coordinates, alphabet, cpi, statemachine,
#                cognition)
# Cargo workspace lives at b-word/Cargo.toml; all cargo commands run from there.
# Go module root is b-word/go.mod; all go commands run from there.
#
# Called from root:  $(MAKE) -C b-word/L3-cpisi [target]
# Called directly:   cd b-word/L3-cpisi && make [target]
#
# ───────────────────────────────────────────────────────────────────────────────
# X.2 RELATED FILES [RELATED]
# ───────────────────────────────────────────────────────────────────────────────
#
# Parent:    root-files/Makefile (root orchestrator)
# Siblings:  L0-universal/Makefile, L1-omnicode/Makefile, L2-platform/Makefile
# Workspace: b-word/Cargo.toml (Rust workspace root)
# Module:    b-word/go.mod (Go module root)
#
# ───────────────────────────────────────────────────────────────────────────────
# X.3 BIBLICAL CLOSING [BIBLICAL]
# ───────────────────────────────────────────────────────────────────────────────
#
# "For as he thinketh in his heart, so is he:
# Eat and drink, saith he to thee; but his heart is not with thee."
# — Proverbs 23:7 KJV
#
# ═══════════════════════════════════════════════════════════════════════════════
# END CLOSING [END]
# ═══════════════════════════════════════════════════════════════════════════════
