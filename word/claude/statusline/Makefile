# ============================================================================
# METADATA - Statusline Build
# ============================================================================
# Purpose: Build the statusline executable (CPI-SI state machine visualization)
# Biblical: 1 Corinthians 14:40 - "Let all things be done decently and in order"
#
# Components:
#   cmd/statusline/main.go  — Entry point, JSON parsing, state machine display
#   pkg/statemachine/       — 27-position cube, trajectory navigation
#   pkg/types/session.go    — Claude Code JSON schema types
#
# WezTerm Integration:
#   - OSC 1337 SetUserVar for state machine variables
#   - OSC 1/2 for window/tab titles
#   - OSC 10/11/12 for dynamic colors based on state
#
# ============================================================================

.PHONY: build clean test test-minimal test-full install help

# ============================================================================
# SETUP
# ============================================================================

BINARY = statusline
CMD_DIR = cmd/statusline
BIN_DIR = bin
TEST_DIR = testdata

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

# ============================================================================
# BODY
# ============================================================================

build:
	@mkdir -p $(BIN_DIR)
	go build -o $(BIN_DIR)/$(BINARY) ./$(CMD_DIR)
	@echo "Built $(BIN_DIR)/$(BINARY)"

clean:
	rm -rf $(BIN_DIR)

# Default test uses full JSON schema
test: test-full

# Quick test with minimal JSON
test-minimal: build
	@echo "Testing with minimal JSON..."
	@echo "$$MINIMAL_TEST_JSON" | ./$(BIN_DIR)/$(BINARY)

# Full test with complete Claude Code JSON schema
test-full: build
	@echo "Testing with full Claude Code JSON schema..."
	@echo "$$FULL_TEST_JSON" | ./$(BIN_DIR)/$(BINARY)

# Test state machine positions
test-states: build
	@echo "Testing state machine visualization..."
	@echo "$$FULL_TEST_JSON" | ./$(BIN_DIR)/$(BINARY) 2>&1 | head -1
	@echo ""
	@echo "WezTerm variables set:"
	@echo "$$FULL_TEST_JSON" | ./$(BIN_DIR)/$(BINARY) 2>&1 | grep -o 'SM_[A-Z_]*' | sort -u || true

install: build
	@mkdir -p ~/.claude/cpi-si/statusline/
	@cp $(BIN_DIR)/$(BINARY) ~/.claude/cpi-si/statusline/
	@echo "Installed to ~/.claude/cpi-si/statusline/"

# ============================================================================
# CLOSING
# ============================================================================

help:
	@echo "Statusline Build System (CPI-SI State Machine Visualization)"
	@echo ""
	@echo "Targets:"
	@echo "  build        - Build statusline executable"
	@echo "  clean        - Remove build artifacts"
	@echo "  test         - Test with full JSON schema (default)"
	@echo "  test-minimal - Test with minimal JSON"
	@echo "  test-full    - Test with complete Claude Code JSON"
	@echo "  test-states  - Test state machine variable output"
	@echo "  install      - Install to ~/.claude/cpi-si/statusline/"
	@echo ""
	@echo "WezTerm Integration:"
	@echo "  SM_POSITION  - Current state machine position"
	@echo "  SM_COMMAND   - Current command (HALT/AWAIT/PROCEED)"
	@echo "  SM_ANCHOR    - Current anchor point"
	@echo "  SM_X/Y/Z     - Cube coordinates"
