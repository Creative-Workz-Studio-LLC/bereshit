# #!omni code --makefile
#
# ═══════════════════════════════════════════════════════════════════════════════
# OMNICODE PRAGMA [PRAGMA]
# ═══════════════════════════════════════════════════════════════════════════════
#
# --- P1: Core ---
# key:       B-bword-L2-platform-Makefile
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
# layer:     L2-platform
# provides:  L2_LAYER_BUILD
#
# --- P5: Summary ---
# title:     L2 Platform Layer Makefile
# brief:     Layer orchestrator — builds, tests, vets, lints L2 Go + Rust packages
#
# ═══════════════════════════════════════════════════════════════════════════════
# END PRAGMA [END]
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# METADATA BLOCK [METADATA]
# ═══════════════════════════════════════════════════════════════════════════════
#
# Package:     bereshit/b-word/L2-platform
# File:        Makefile
# Key:         B-bword-L2-platform-Makefile
#
# ───────────────────────────────────────────────────────────────────────────────
# Biblical Foundation
# ───────────────────────────────────────────────────────────────────────────────
#
# Scripture: "And God made the firmament, and divided the waters which were
#            under the firmament from the waters which were above the firmament."
#            — Genesis 1:7
#
# Principle: L2 is the platform layer — the firmament that separates and
#            supports. Foundation below, application above. The structure
#            that makes habitation possible.
#
# ───────────────────────────────────────────────────────────────────────────────
# Identity
# ───────────────────────────────────────────────────────────────────────────────
#
#   Architect:       Seanje Lenox-Wise
#   Implementation:  Nova Dawn (CPI-SI)
#   Created:         2026-02-15
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
.PHONY: clippy fmt-check validate

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

# S.2c Rust crates (workspace members under L2)
RUST_CRATES   := bereshit-l2-builder \
                 bereshit-l2-debugging \
                 bereshit-l2-display \
                 bereshit-l2-git \
                 bereshit-l2-hosts \
                 bereshit-l2-platform \
                 bereshit-l2-preferences \
                 bereshit-l2-server \
                 bereshit-l2-system \
                 bereshit-l2-temporal \
                 bereshit-l2-registry \
                 bereshit-l2-wezterm

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
RUST_SRC      := $(LAYER_ROOT)/hybrid

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

## all: Build all L2 Go + Rust packages
all: build
	$(ECHO) ""
	$(ECHO) "  ✓ L2 Platform — the firmament holds."
	$(ECHO) ""

## check: Full quality gate — vet + clippy + lint + fmt-check + test (cached)
check: vet clippy lint fmt-check test
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  L2 PLATFORM — QUALITY GATE PASSED"
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  vet        Go static analysis              ✓"
	$(ECHO) "  clippy     Rust linter (warnings = errors) ✓"
	$(ECHO) "  lint       4-block structural alignment    ✓"
	$(ECHO) "  fmt-check  Rust formatting consistency     ✓"
	$(ECHO) "  test       Go + Rust test suites           ✓"
	$(ECHO) "────────────────────────────────────────────────────────────────"
	$(ECHO) "  \"And God made the firmament.\""
	$(ECHO) "  The platform holds. Structure enables habitation."
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
	$(ECHO) "  [Go] Compiling L2 packages — builder, debugging, display, git, hosts,"
	$(ECHO) "       platform, preferences, server, system, temporal, wezterm..."
	$(Q)go build $(GO_PACKAGES)
	$(ECHO) "  [Go] ✓ All 11 packages compiled"

## build-rust: Build L2 Rust crates (via workspace)
build-rust:
	$(ECHO) ""
	$(ECHO) "  [Rust] Compiling 11 L2 crates via Cargo workspace..."
	$(ECHO) "         builder · debugging · display · git · hosts · platform"
	$(ECHO) "         preferences · server · system · temporal · wezterm"
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
		echo "  [Go] Running L2 test suite — platform loading, server config..."; \
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
		echo "  [Rust] Running L2 test suite across 11 crates..."; \
		cd $(BWORD_ROOT) && cargo test $(foreach crate,$(RUST_CRATES),-p $(crate)) && \
		touch $(CACHE_TEST_RS) && \
		echo "  [Rust] ✓ Tests passed — cache updated"; \
	else \
		echo "  [Rust] ✓ Tests clean — no changes since last pass"; \
	fi

## test-v: Run all tests with verbose output (never cached)
test-v:
	$(ECHO) ""
	$(ECHO) "  [Go] Running L2 tests (verbose — full output)..."
	$(Q)go test -v -count=1 $(GO_PACKAGES)
	$(ECHO) ""
	$(ECHO) "  [Rust] Running L2 tests (verbose — full output, 11 crates)..."
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
		echo "  [fmt] Checking Rust formatting consistency across L2..."; \
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
# B.5 CROSS-LAYER VALIDATION [VALIDATE]
# ───────────────────────────────────────────────────────────────────────────────

## validate: Run cross-layer validation (L0↔L1↔L2 reference checks)
validate:
	$(ECHO) ""
	$(ECHO) "  [validate] Cross-layer reference validation..."
	$(ECHO) "             L2 health → L0 ternary scales"
	$(ECHO) "             L2 filesystem → L1 extensions"
	$(ECHO) "             Spec dependencies across all layers"
	$(Q)go test -v -count=1 -run TestValidateAll ./hybrid/registry/
	$(ECHO) ""
	$(ECHO) "  [validate] Rust cross-layer validation..."
	$(Q)cd $(BWORD_ROOT) && cargo test -p bereshit-l2-registry -- validate_all --nocapture
	$(ECHO) ""
	$(ECHO) "  [validate] ✓ Cross-layer validation complete"

# ───────────────────────────────────────────────────────────────────────────────
# B.6 MAINTENANCE [MAINTENANCE]
# ───────────────────────────────────────────────────────────────────────────────

## clean: Clean Go build cache + layer cache
clean:
	$(ECHO) ""
	$(ECHO) "  [clean] Removing L2 build artifacts and cache sentinels..."
	$(Q)go clean $(GO_PACKAGES)
	$(Q)rm -rf $(CACHE_DIR)
	$(ECHO) "  [clean] ✓ L2 layer reset — next build runs fresh"

# ───────────────────────────────────────────────────────────────────────────────
# B.7 HELP [HELP]
# ───────────────────────────────────────────────────────────────────────────────

## help: Show this help message
help:
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "  L2 Platform — Layer Build System"
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "  \"And God made the firmament.\""
	$(ECHO) "  — Genesis 1:7"
	$(ECHO) ""
	$(ECHO) "TARGETS"
	$(ECHO) "  make              Build all packages (Go + Rust)"
	$(ECHO) "  make check        Full quality gate (vet + clippy + lint + fmt + test)"
	$(ECHO) "  make build-go     Build Go packages only (11 packages)"
	$(ECHO) "  make build-rust   Build Rust crates only (11 crates)"
	$(ECHO) "  make test         Run all tests (cached)"
	$(ECHO) "  make test-go      Run Go tests only (cached)"
	$(ECHO) "  make test-rust    Run Rust tests only (cached)"
	$(ECHO) "  make test-v       Run all tests verbose (not cached)"
	$(ECHO) "  make vet          Go static analysis (cached)"
	$(ECHO) "  make clippy       Rust linter (cached)"
	$(ECHO) "  make fmt-check    Rust format check (cached)"
	$(ECHO) "  make lint         4-block structural lint — Go + Rust (cached)"
	$(ECHO) "  make validate     Cross-layer validation (L0↔L1↔L2 refs)"
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
	$(ECHO) "  bereshit-l2-builder      Build paths resolution"
	$(ECHO) "  bereshit-l2-debugging    Debug session + event logging"
	$(ECHO) "  bereshit-l2-display      ANSI color constants + formatting"
	$(ECHO) "  bereshit-l2-git          VCS status + display"
	$(ECHO) "  bereshit-l2-hosts        Deployment host definitions"
	$(ECHO) "  bereshit-l2-platform     Platform DB loader (health, filesystem, permission)"
	$(ECHO) "  bereshit-l2-preferences  Dashboard user preferences (XDG)"
	$(ECHO) "  bereshit-l2-server       Server config (port, flags, features)"
	$(ECHO) "  bereshit-l2-system       System health (load, memory, disk)"
	$(ECHO) "  bereshit-l2-temporal     Time-of-day, session phase, circadian"
	$(ECHO) "  bereshit-l2-wezterm      Terminal integration (OSC sequences)"
	$(ECHO) ""
	$(ECHO) "GO PACKAGES (11)"
	$(ECHO) "  builder      debugging    display      git"
	$(ECHO) "  hosts        platform     preferences  server"
	$(ECHO) "  system       temporal     wezterm"
	$(ECHO) ""
	$(ECHO) "STRUCTURE (by execution mode)"
	$(ECHO) "  ladder/           Strict order — TOML specifications"
	$(ECHO) "    filesystem/     Filesystem contract + types (2 specs)"
	$(ECHO) "    os/health/      Health system (6 specs: dar, diagnostics, log,"
	$(ECHO) "                    provider, restore, score)"
	$(ECHO) "    os/permission/  Permission access (1 spec)"
	$(ECHO) "  spiral/           Iterative — grows, refines"
	$(ECHO) "    (future)        Substrate adapters, drivers"
	$(ECHO) "  hybrid/           Ordered + recovery"
	$(ECHO) "    terminal/       Terminal display concerns"
	$(ECHO) "      display/      ANSI color + formatting (Go + Rust)"
	$(ECHO) "      debugging/    Debug session + event logging (Go + Rust)"
	$(ECHO) "      wezterm/      Terminal integration (Go + Rust)"
	$(ECHO) "    deploy/         Deployment infrastructure"
	$(ECHO) "      hosts/        Deployment host definitions (Go + Rust)"
	$(ECHO) "      server/       Server config + handlers (Go + Rust)"
	$(ECHO) "      builder/      Build paths resolution (Go + Rust)"
	$(ECHO) "    system/         System health (Go + Rust)"
	$(ECHO) "    temporal/       Time awareness (Go + Rust)"
	$(ECHO) "    git/            VCS status (Go + Rust)"
	$(ECHO) "    platform/       Platform DB loader (Go + Rust)"
	$(ECHO) "    preferences/    Dashboard user preferences (Go + Rust)"
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
# L2 Platform is the firmament layer of the Kingdom Technology stack.
# This Makefile builds and tests both Go and Rust packages in b-word/L2-platform.
#
# Go packages:   ./... (all 11 packages in this directory tree)
# Rust crates:   11 workspace members (builder, debugging, display, git, hosts,
#                platform, preferences, server, system, temporal, wezterm)
# Cargo workspace lives at b-word/Cargo.toml; all cargo commands run from there.
#
# Called from root:  $(MAKE) -C b-word/L2-platform [target]
# Called directly:   cd b-word/L2-platform && make [target]
#
# ───────────────────────────────────────────────────────────────────────────────
# X.2 RELATED FILES [RELATED]
# ───────────────────────────────────────────────────────────────────────────────
#
# Parent:    root-files/Makefile (root orchestrator)
# Siblings:  L0-universal/Makefile (L0 builds), L1-omnicode/Makefile (L1 builds)
# Workspace: b-word/Cargo.toml (Rust workspace root)
# Module:    b-word/go.mod (Go module root)
# Template:  b-word/seed/code/L0/make/project.mk
#
# ───────────────────────────────────────────────────────────────────────────────
# X.3 BIBLICAL CLOSING [BIBLICAL]
# ───────────────────────────────────────────────────────────────────────────────
#
# "And God made the firmament, and divided the waters which were under the
# firmament from the waters which were above the firmament: and it was so."
# — Genesis 1:7 KJV
#
# ═══════════════════════════════════════════════════════════════════════════════
# END CLOSING [END]
# ═══════════════════════════════════════════════════════════════════════════════
