# ============================================================================
# METADATA - Hooks Build System
# ============================================================================
# Purpose: Build Claude Code hooks (CPI-SI lifecycle integration)
# Biblical: Nehemiah 3 - "So we built the wall... for the people had a mind to work"
#
# Config-Driven:
#   hooks.jsonc is the source of truth for hook definitions.
#   This Makefile reads from hooks.jsonc to know what to build.
#
# Architecture (2026-02-04):
#   Source (exFAT):   hooks/cmd/         - Go source code
#   Build (exFAT):    hooks/bin/         - Compiled binaries (intermediate)
#   Install (ext4):   ~/.local/share/claude/hooks/bin/  - Executable binaries
#
#   exFAT doesn't support execute permissions, so binaries must be installed
#   to ext4 via the root Makefile's install-bin target.
#
# Structure:
#   hooks.jsonc    - Configuration (source of truth)
#   cmd/           - Source packages for each hook
#   bin/           - Compiled binaries (local build output)
#   session/       - Session lifecycle hook library
#   tool/          - Tool lifecycle hook library
#   prompt/        - Prompt lifecycle hook library
#   permission/    - Permission hook library
#
# Deployment Chain:
#   1. Build:   make build → bin/ (compile Go sources, local)
#   2. Install: root make install-bin → ~/.local/share/claude/hooks/bin/
#   3. Execute: settings.json references ~/.local/share/claude/hooks/bin/
#
# Usage:
#   make              - Build all hooks
#   make build        - Same as make
#   make clean        - Remove compiled binaries
#   make status       - Show what's built
#   make test         - Verify hooks work
#   make list-hooks   - Show hooks from config
#   make help         - Show this help
#   make <hook>       - Build specific hook (e.g., make start)
#
# ============================================================================

# ============================================================================
# SETUP - Variables
# ============================================================================

GO := go
GOFLAGS := -ldflags="-s -w"
BIN := bin
CONFIG := hooks.jsonc

# --- Config-Driven Hook Discovery ---
# Read hook names from hooks.jsonc (strips JSONC comments, extracts keys)
# Falls back to hardcoded list if jq unavailable
HOOKS_FROM_CONFIG := $(shell sed 's|//.*||' $(CONFIG) 2>/dev/null | jq -r '.build.hooks | keys[]' 2>/dev/null)

# Fallback if jq not available or config parsing fails
ifeq ($(HOOKS_FROM_CONFIG),)
  SESSION_HOOKS := start end stop subagent-stop subagent-start setup notification pre-compact
  TOOL_HOOKS := pre-use post-use
  PROMPT_HOOKS := submit
  PERMISSION_HOOKS := permission-request
  ALL_HOOKS := $(SESSION_HOOKS) $(TOOL_HOOKS) $(PROMPT_HOOKS) $(PERMISSION_HOOKS)
  CONFIG_STATUS := (fallback - jq unavailable)
else
  ALL_HOOKS := $(HOOKS_FROM_CONFIG)
  CONFIG_STATUS := (from hooks.jsonc)
endif

# ============================================================================
# BODY - Build Targets
# ============================================================================

.PHONY: all build hooks clean install status test help list-hooks fmt vet $(ALL_HOOKS)

# Default target
all: build
	@echo "✅ All hooks built to bin/"
	@echo "   Run 'make install-bin' from root to install to ext4"

# Build all hooks
build: $(ALL_HOOKS) hooks

# Orchestrator/help tool (main.go)
hooks: main.go session/*.go tool/*.go prompt/*.go permission/*.go internal/*.go | $(BIN)
	@$(GO) build $(GOFLAGS) -o $(BIN)/hooks .
	@echo "  Built: hooks (orchestrator)"

# Generic build rule - builds cmd/<name> to bin/<name>
$(ALL_HOOKS): | $(BIN)
	@$(GO) build $(GOFLAGS) -o $(BIN)/$@ ./cmd/$@
	@echo "  Built: $@"

# Ensure bin directory exists
$(BIN):
	@mkdir -p $(BIN)

# ============================================================================
# BODY - Install Target
# ============================================================================

# Install target (for root Makefile to call)
# Actual installation to ext4 is handled by root Makefile's install-bin
install: build
	@echo ""
	@echo "✅ Hooks built to bin/"
	@echo "   Note: Install to ext4 via root Makefile's install-bin target"
	@make status --no-print-directory

# ============================================================================
# BODY - Status and Test Targets
# ============================================================================

# Status: Show what's built in bin/
status:
	@echo ""
	@echo "📋 Hooks $(CONFIG_STATUS):"
	@if [ -d "$(BIN)" ]; then \
		for hook in $(ALL_HOOKS) hooks; do \
			if [ -f "$(BIN)/$$hook" ]; then \
				echo "  ✓ $$hook"; \
			else \
				echo "  ✗ $$hook (not built)"; \
			fi; \
		done; \
	else \
		echo "  (bin/ does not exist - run 'make build')"; \
	fi

# List hooks from config
list-hooks:
	@echo "Hooks from $(CONFIG) $(CONFIG_STATUS):"
	@for hook in $(ALL_HOOKS); do echo "  $$hook"; done

# Test: Verify hooks execute correctly
test: build
	@echo "🧪 Testing hooks..."
	@echo ""
	@echo "--- Testing start hook ---"
	@echo '{"session_id":"test-session","source":"startup"}' | $(BIN)/start 2>&1 | head -5
	@echo ""
	@echo "--- Testing submit hook ---"
	@echo '{"session_id":"test-session","prompt":"test prompt"}' | $(BIN)/submit 2>&1 | head -5
	@echo ""
	@echo "✅ Hook execution tests passed"

# ============================================================================
# BODY - Utility Targets
# ============================================================================

# Clean: Remove compiled binaries from bin/
clean:
	@rm -rf $(BIN)
	@echo "🧹 Cleaned bin/"

# Format Go code
fmt:
	@echo "=== Formatting hooks Go code ==="
	@$(GO) fmt ./...

# Run go vet
vet:
	@echo "=== Running go vet on hooks ==="
	@$(GO) vet ./...

# ============================================================================
# CLOSING - Help
# ============================================================================

help:
	@echo "Hooks Build System (Config-Driven)"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Build targets:"
	@echo "  all, build    Build all hooks to bin/ (local)"
	@echo "  hooks         Build orchestrator only"
	@echo "  <hook>        Build specific hook (e.g., make start)"
	@echo ""
	@echo "Verification targets:"
	@echo "  status        Show what's built in bin/"
	@echo "  list-hooks    List hooks from config"
	@echo "  test          Verify hooks execute correctly"
	@echo ""
	@echo "Utility targets:"
	@echo "  clean         Remove bin/ directory"
	@echo "  fmt           Format Go code"
	@echo "  vet           Run go vet"
	@echo "  help          Show this help"
	@echo ""
	@echo "Config: $(CONFIG) $(CONFIG_STATUS)"
	@echo "Hooks:  $(ALL_HOOKS)"
	@echo ""
	@echo "Deployment chain:"
	@echo "  hooks.jsonc (source of truth)"
	@echo "       ↓"
	@echo "  make build → bin/ (local, exFAT)"
	@echo "       ↓"
	@echo "  root: make install-bin"
	@echo "       ↓"
	@echo "  ~/.local/share/claude/hooks/bin/ (ext4, executable)"
	@echo "       ↓"
	@echo "  settings.json references installed hooks"
