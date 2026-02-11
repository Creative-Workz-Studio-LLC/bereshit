# ============================================================================
# METADATA - Hooks Build System
# ============================================================================
#
# Purpose: Build Claude Code hooks (CPI-SI lifecycle integration)
# Biblical: Nehemiah 3 — "So we built the wall... for the people had a mind to work"
#
# Config-Driven:
#   hooks.jsonc is the source of truth for hook definitions.
#   This Makefile reads from hooks.jsonc to know what to build.
#
# Architecture:
#   Source (exFAT):   hooks/cmd/                         Go source code
#   Build (exFAT):    hooks/bin/                         Compiled binaries (intermediate)
#   Install (ext4):   ~/.local/share/claude/hooks/bin/   Executable binaries
#
#   exFAT doesn't support execute permissions, so binaries must be installed
#   to ext4 via the root Makefile's install-bin target.
#
# Structure:
#   hooks.jsonc    — Configuration (source of truth)
#   cmd/           — Source packages for each hook
#   bin/           — Compiled binaries (local build output)
#   session/       — Session lifecycle hook library
#   tool/          — Tool lifecycle hook library
#   prompt/        — Prompt lifecycle hook library
#   permission/    — Permission hook library
#
# Deployment Chain:
#   hooks.jsonc (source of truth)
#        ↓
#   make build → bin/ (compile, local exFAT)
#        ↓
#   root: make install-bin → ~/.local/share/claude/hooks/bin/ (ext4, +x)
#        ↓
#   settings.json references installed hooks
#
# Quick start:
#   make build        Build all hooks to bin/
#   make <hook>       Build specific hook (e.g., make start)
#   make status       Show what's built
#   make test         Verify hooks execute
#   make help         Show all targets
#
# Integration with root Makefile:
#   Root calls: cd hooks && $(MAKE) BINFLAGS="$(BINFLAGS)" build
#   Variables use ?= so root can override, standalone builds use defaults.
#
# ============================================================================

# ============================================================================
# SETUP — Variables
# ============================================================================

# ?= allows root Makefile to override when calling $(MAKE) BINFLAGS="..." build.
# Standalone builds use these defaults (matching root Makefile conventions).
GO       ?= go
SHELL    := /bin/bash

# Binary build flags (strip debug, disable VCS stamping)
# -buildvcs=false: prevents git status on 22GB exFAT (causes timeouts)
# -ldflags="-s -w": strip debug symbols, reduce binary size
BINFLAGS ?= -buildvcs=false -ldflags="-s -w"

# Compile-check only flags (no linking, just verify sources compile)
NOVCS    ?= -buildvcs=false

BIN      := bin
CONFIG   := hooks.jsonc

# --- Config-Driven Hook Discovery ---
# Read hook names from hooks.jsonc (strips JSONC comments, extracts keys)
# Falls back to hardcoded list if jq unavailable
HOOKS_FROM_CONFIG := $(shell sed 's|//.*||' $(CONFIG) 2>/dev/null | jq -r '.build.hooks | keys[]' 2>/dev/null)

# Fallback if jq not available or config parsing fails
# Must match hooks.jsonc — keep in sync when adding/removing hooks
ifeq ($(HOOKS_FROM_CONFIG),)
  SESSION_HOOKS    := start end stop subagent-stop subagent-start setup notification pre-compact teammate-idle task-completed
  TOOL_HOOKS       := pre-use post-use
  PROMPT_HOOKS     := submit
  PERMISSION_HOOKS := permission-request
  ALL_HOOKS        := $(SESSION_HOOKS) $(TOOL_HOOKS) $(PROMPT_HOOKS) $(PERMISSION_HOOKS)
  CONFIG_STATUS    := (fallback — jq unavailable)
else
  ALL_HOOKS        := $(HOOKS_FROM_CONFIG)
  CONFIG_STATUS    := (from hooks.jsonc)
endif

.PHONY: all build hooks clean install update status test help list-hooks fmt vet $(ALL_HOOKS)

# Default
all: build

# ============================================================================
# BODY — Build Targets
# ============================================================================

# Build all hook binaries + orchestrator
build: $(ALL_HOOKS) hooks
	@echo ""
	@echo "  All hooks built to $(BIN)/ ($(words $(ALL_HOOKS)) hooks + orchestrator)"

# Orchestrator (built from root, not cmd/)
hooks: main.go session/*.go tool/*.go prompt/*.go permission/*.go internal/*.go | $(BIN)
	@$(GO) build $(BINFLAGS) -o $(BIN)/hooks .
	@echo "  Built: hooks (orchestrator)"

# Generic build rule — builds cmd/<name> to bin/<name>
$(ALL_HOOKS): | $(BIN)
	@$(GO) build $(BINFLAGS) -o $(BIN)/$@ ./cmd/$@
	@echo "  Built: $@"

# Ensure bin directory exists
$(BIN):
	@mkdir -p $(BIN)

# ============================================================================
# BODY — Incremental Update
# ============================================================================

# Rebuild only changed hooks (checks source timestamps against binaries)
update: | $(BIN)
	@echo ""
	@echo "  Hooks Incremental Update"
	@echo "  ────────────────────────────────────────"
	@echo ""
	@changed=0; unchanged=0; \
	for hook in $(ALL_HOOKS); do \
		if [ ! -f "$(BIN)/$$hook" ] || \
		   find cmd/$$hook session tool prompt permission internal -name '*.go' -newer "$(BIN)/$$hook" -print -quit 2>/dev/null | grep -q .; then \
			$(GO) build $(BINFLAGS) -o $(BIN)/$$hook ./cmd/$$hook; \
			echo "  * $$hook (rebuilt)"; \
			changed=$$((changed+1)); \
		else \
			unchanged=$$((unchanged+1)); \
		fi; \
	done; \
	if [ ! -f "$(BIN)/hooks" ] || \
	   find . -maxdepth 1 -name '*.go' -newer "$(BIN)/hooks" -print -quit 2>/dev/null | grep -q . || \
	   find session tool prompt permission internal -name '*.go' -newer "$(BIN)/hooks" -print -quit 2>/dev/null | grep -q .; then \
		$(GO) build $(BINFLAGS) -o $(BIN)/hooks .; \
		echo "  * hooks (orchestrator, rebuilt)"; \
		changed=$$((changed+1)); \
	else \
		unchanged=$$((unchanged+1)); \
	fi; \
	echo ""; \
	total=$$((changed + unchanged)); \
	echo "  ────────────────────────────────────────"; \
	echo "  $$changed rebuilt, $$unchanged cached ($$total total)"; \
	echo ""

# ============================================================================
# BODY — Install
# ============================================================================

# Build hooks (actual ext4 deployment is handled by root Makefile's install-bin)
install: build
	@echo ""
	@echo "  Hooks built to $(BIN)/"
	@echo "  Deploy to ext4 via root Makefile: make install-bin"
	@$(MAKE) status --no-print-directory

# ============================================================================
# BODY — Verification
# ============================================================================

# Show what's built in bin/
status:
	@echo ""
	@echo "  Hooks $(CONFIG_STATUS):"
	@if [ -d "$(BIN)" ]; then \
		for hook in $(ALL_HOOKS) hooks; do \
			if [ -f "$(BIN)/$$hook" ]; then \
				echo "    + $$hook"; \
			else \
				echo "    - $$hook (not built)"; \
			fi; \
		done; \
	else \
		echo "    (bin/ does not exist — run 'make build')"; \
	fi

# List hooks from config
list-hooks:
	@echo "  Hooks from $(CONFIG) $(CONFIG_STATUS):"
	@for hook in $(ALL_HOOKS); do echo "    $$hook"; done

# Verify hooks execute correctly
test: build
	@echo "  Testing hooks..."
	@echo ""
	@echo "  --- start hook ---"
	@echo '{"session_id":"test-session","source":"startup"}' | $(BIN)/start 2>&1 | head -5
	@echo ""
	@echo "  --- submit hook ---"
	@echo '{"session_id":"test-session","prompt":"test prompt"}' | $(BIN)/submit 2>&1 | head -5
	@echo ""
	@echo "  Hook tests passed"

# ============================================================================
# BODY — Utilities
# ============================================================================

# Remove compiled binaries
clean:
	@rm -rf $(BIN)
	@echo "  Cleaned $(BIN)/"

# Format Go code
fmt:
	@echo "  Formatting hooks..."
	@$(GO) fmt ./...

# Run go vet (uses NOVCS to avoid git scans)
vet:
	@echo "  Vetting hooks..."
	@$(GO) vet $(NOVCS) ./...

# ============================================================================
# CLOSING — Help
# ============================================================================

help:
	@echo ""
	@echo "  ================================================================"
	@echo "    Hooks Build System (Config-Driven)"
	@echo "    CPI-SI Lifecycle Integration"
	@echo "  ================================================================"
	@echo ""
	@echo "  Config: $(CONFIG) $(CONFIG_STATUS)"
	@echo "  Output: $(BIN)/"
	@echo ""
	@echo "Build:"
	@echo "  build         Build all hooks to $(BIN)/ (default)"
	@echo "  update        Rebuild only changed hooks (incremental)"
	@echo "  hooks         Build orchestrator only"
	@echo "  <hook>        Build specific hook (e.g., make start)"
	@echo ""
	@echo "Verification:"
	@echo "  status        Show what's built in $(BIN)/"
	@echo "  list-hooks    List hooks from config"
	@echo "  test          Verify hooks execute correctly"
	@echo ""
	@echo "Utilities:"
	@echo "  clean         Remove $(BIN)/ directory"
	@echo "  fmt           Format Go code"
	@echo "  vet           Run go vet"
	@echo "  help          Show this help"
	@echo ""
	@echo "Variables (overridable by root Makefile):"
	@echo "  GO=$(GO)  BINFLAGS=$(BINFLAGS)  NOVCS=$(NOVCS)"
	@echo ""
	@echo "Hooks: $(ALL_HOOKS)"
	@echo ""
	@echo "Deployment chain:"
	@echo "  hooks.jsonc → make build → $(BIN)/"
	@echo "  root: make install-bin → ~/.local/share/claude/hooks/bin/ (ext4, +x)"
	@echo ""
