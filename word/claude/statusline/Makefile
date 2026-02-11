# ============================================================================
# METADATA - Statusline Build
# ============================================================================
#
# Purpose: Build the statusline executable (CPI-SI state machine visualization)
# Biblical: 1 Corinthians 14:40 — "Let all things be done decently and in order"
#
# Architecture:
#   Source (exFAT):   statusline/cmd/statusline/   Entry point
#                     statusline/lib/              Sections, builders, session
#   Build (exFAT):    statusline/bin/              Compiled binary (intermediate)
#   Install (ext4):   ~/.local/share/claude/bin/   Executable binary
#
# WezTerm Integration:
#   OSC 1337 SetUserVar — state machine variables
#   OSC 1/2             — window/tab titles
#   OSC 10/11/12        — dynamic colors based on state
#
# Quick start:
#   make build         Build statusline executable
#   make test          Test with full Claude Code JSON
#   make help          Show all targets
#
# Integration with root Makefile:
#   Root calls: cd statusline && $(MAKE) BINFLAGS="$(BINFLAGS)" build
#   Variables use ?= so root can override, standalone builds use defaults.
#
# ============================================================================

# ============================================================================
# SETUP — Variables
# ============================================================================

# Match root Makefile conventions. ?= allows override if ever called from root.
GO       ?= go
SHELL    := /bin/bash

# Binary build flags (strip debug, disable VCS stamping)
# -buildvcs=false: prevents git status on 22GB exFAT (causes timeouts)
# -ldflags="-s -w": strip debug symbols, reduce binary size
BINFLAGS ?= -buildvcs=false -ldflags="-s -w"

# Compile-check only flags
NOVCS    ?= -buildvcs=false

BINARY   := statusline
CMD_DIR  := cmd/statusline
BIN_DIR  := bin

# Full Claude Code JSON schema for testing
define FULL_TEST_JSON
{
  "hook_event_name": "Status",
  "session_id": "test-session-123",
  "transcript_path": "/tmp/transcript.json",
  "cwd": "/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/claude-global",
  "version": "1.0.0",
  "model": {
    "id": "claude-opus-4-5-20251101",
    "display_name": "Opus 4.5"
  },
  "workspace": {
    "current_dir": "/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/claude-global",
    "project_dir": "/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC"
  },
  "output_style": {
    "name": "Nova Dawn"
  },
  "cost": {
    "total_cost_usd": 0.15,
    "total_duration_ms": 120000,
    "total_api_duration_ms": 45000,
    "total_lines_added": 150,
    "total_lines_removed": 20
  },
  "context_window": {
    "total_input_tokens": 50000,
    "total_output_tokens": 10000,
    "context_window_size": 200000,
    "current_usage": {
      "input_tokens": 35000,
      "output_tokens": 8000,
      "cache_creation_input_tokens": 5000,
      "cache_read_input_tokens": 25000
    }
  }
}
endef
export FULL_TEST_JSON

# Minimal JSON for quick testing
define MINIMAL_TEST_JSON
{"session_id":"test","model":{"display_name":"Sonnet"},"cwd":"/tmp"}
endef
export MINIMAL_TEST_JSON

.PHONY: all build clean test test-minimal test-full test-states install update fmt vet help

# Default
all: build

# ============================================================================
# BODY — Build Targets
# ============================================================================

build:
	@mkdir -p $(BIN_DIR)
	@$(GO) build $(BINFLAGS) -o $(BIN_DIR)/$(BINARY) ./$(CMD_DIR)
	@echo "  Built: $(BIN_DIR)/$(BINARY)"

# ============================================================================
# BODY — Incremental Update
# ============================================================================

# Rebuild only if sources changed (checks timestamps)
update:
	@echo ""
	@echo "  Statusline Incremental Update"
	@echo "  ────────────────────────────────────────"
	@echo ""
	@if [ ! -f "$(BIN_DIR)/$(BINARY)" ] || \
	   find $(CMD_DIR) lib -name '*.go' -newer "$(BIN_DIR)/$(BINARY)" -print -quit 2>/dev/null | grep -q .; then \
		mkdir -p $(BIN_DIR); \
		$(GO) build $(BINFLAGS) -o $(BIN_DIR)/$(BINARY) ./$(CMD_DIR); \
		echo "  * $(BINARY) (rebuilt)"; \
	else \
		echo "  $(BINARY) up to date (cached)"; \
	fi
	@echo ""
	@echo "  ────────────────────────────────────────"
	@echo "  Update complete."
	@echo ""

# ============================================================================
# BODY — Install
# ============================================================================

# Install to parent bin/ (root Makefile handles ext4 deployment)
install: build
	@mkdir -p ../bin
	@cp $(BIN_DIR)/$(BINARY) ../bin/$(BINARY)
	@echo "  Installed to ../bin/$(BINARY)"

# ============================================================================
# BODY — Verification
# ============================================================================

# Default test uses full JSON schema
test: test-full

# Quick test with minimal JSON
test-minimal: build
	@echo "  Testing with minimal JSON..."
	@echo "$$MINIMAL_TEST_JSON" | ./$(BIN_DIR)/$(BINARY)

# Full test with complete Claude Code JSON schema
test-full: build
	@echo "  Testing with full Claude Code JSON schema..."
	@echo "$$FULL_TEST_JSON" | ./$(BIN_DIR)/$(BINARY)

# Test state machine positions
test-states: build
	@echo "  Testing state machine visualization..."
	@echo "$$FULL_TEST_JSON" | ./$(BIN_DIR)/$(BINARY) 2>&1 | head -1
	@echo ""
	@echo "  WezTerm variables set:"
	@echo "$$FULL_TEST_JSON" | ./$(BIN_DIR)/$(BINARY) 2>&1 | grep -o 'SM_[A-Z_]*' | sort -u || true

# ============================================================================
# BODY — Utilities
# ============================================================================

clean:
	@rm -rf $(BIN_DIR)
	@echo "  Cleaned $(BIN_DIR)/"

fmt:
	@echo "  Formatting statusline..."
	@$(GO) fmt ./...

vet:
	@echo "  Vetting statusline..."
	@$(GO) vet $(NOVCS) ./...

# ============================================================================
# CLOSING — Help
# ============================================================================

help:
	@echo ""
	@echo "  ================================================================"
	@echo "    Statusline Build System"
	@echo "    CPI-SI State Machine Visualization"
	@echo "  ================================================================"
	@echo ""
	@echo "  Output: $(BIN_DIR)/$(BINARY)"
	@echo ""
	@echo "Build:"
	@echo "  build          Build statusline executable (default)"
	@echo "  update         Rebuild only if sources changed (incremental)"
	@echo ""
	@echo "Verification:"
	@echo "  test           Test with full JSON schema"
	@echo "  test-minimal   Test with minimal JSON"
	@echo "  test-full      Test with complete Claude Code JSON"
	@echo "  test-states    Test state machine variable output"
	@echo ""
	@echo "Utilities:"
	@echo "  clean          Remove build artifacts"
	@echo "  fmt            Format Go code"
	@echo "  vet            Run go vet"
	@echo "  install        Install to ../bin/"
	@echo "  help           Show this help"
	@echo ""
	@echo "Variables (overridable by root Makefile):"
	@echo "  GO=$(GO)  BINFLAGS=$(BINFLAGS)  NOVCS=$(NOVCS)"
	@echo ""
	@echo "WezTerm Integration:"
	@echo "  SM_POSITION    Current state machine position"
	@echo "  SM_COMMAND     Current command (HALT/AWAIT/PROCEED)"
	@echo "  SM_ANCHOR      Current anchor point"
	@echo "  SM_X/Y/Z       Cube coordinates"
	@echo ""
