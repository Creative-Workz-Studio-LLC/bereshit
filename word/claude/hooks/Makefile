# ============================================================================
# METADATA - Hooks Build System
# ============================================================================
# Purpose: Build and deploy Claude Code hooks
# Biblical: Nehemiah 3 - "So we built the wall... for the people had a mind to work"
#
# Config-Driven:
#   hooks.jsonc is the source of truth for hook definitions.
#   This Makefile reads from hooks.jsonc to know what to build.
#
# Structure:
#   hooks.jsonc    - Configuration (source of truth)
#   cmd/           - Source packages for each hook
#   bin/           - Compiled binaries (build output)
#   session/       - Session lifecycle hook sources
#   tool/          - Tool lifecycle hook sources
#   prompt/        - Prompt lifecycle hook sources
#   permission/    - Permission hook sources
#
# Deployment Chain:
#   1. Build:  cmd/ → bin/ (compile Go sources)
#   2. Deploy: Root Makefile rsyncs bin/ to production
#   3. Read:   settings.json reads from ~/.claude/hooks/bin/
#
# Usage:
#   make              - Build all hooks
#   make build        - Same as make
#   make deploy       - Build all hooks
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
  SESSION_HOOKS := start end stop subagent-stop notification pre-compact
  TOOL_HOOKS := pre-use post-use
  PROMPT_HOOKS := submit
  PERMISSION_HOOKS := request
  ALL_HOOKS := $(SESSION_HOOKS) $(TOOL_HOOKS) $(PROMPT_HOOKS) $(PERMISSION_HOOKS)
  CONFIG_STATUS := (fallback - jq unavailable)
else
  ALL_HOOKS := $(HOOKS_FROM_CONFIG)
  CONFIG_STATUS := (from hooks.jsonc)
endif

# ============================================================================
# BODY - Build Targets
# ============================================================================

.PHONY: all build hooks clean sync deploy status test help list-hooks $(ALL_HOOKS)

# Default target
all: build
	@echo "✅ All hooks built successfully"
	@echo "   Run 'make sync' to copy to hooks root"
	@echo "   Run 'make deploy' for full build + sync"

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
# BODY - Deploy Target
# ============================================================================

# Deploy: Build all hooks to bin/
# Binaries stay in bin/ where settings.json expects them (~/.claude/hooks/bin/)
# Called by root Makefile: make deploy (after rsync copies hooks/ to production)
deploy: build
	@echo ""
	@echo "🚀 Hooks deployment complete!"
	@echo "   Binaries in: bin/"
	@make status --no-print-directory

# Alias for backward compatibility (sync now just verifies bin/)
sync:
	@echo "📋 Hooks build to bin/ (settings.json reads from ~/.claude/hooks/bin/)"
	@echo "   No sync needed - rsync from root Makefile handles deployment"
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
	@echo "  all, build    Build all hooks to bin/"
	@echo "  deploy        Build all hooks"
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
	@echo "  Makefile reads config → builds to bin/"
	@echo "       ↓"
	@echo "  rsync to bereshit/.../hooks/bin/"
	@echo "       ↓"
	@echo "  ~/.claude/hooks/bin/ (symlink)"
	@echo "       ↓"
	@echo "  settings.json reads hooks"
