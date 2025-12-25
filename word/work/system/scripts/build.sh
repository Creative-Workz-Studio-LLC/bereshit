#!/bin/bash
# ============================================================================
# METADATA - CPI-SI Runtime Build Script
# ============================================================================
# Purpose: Build CPI-SI runtime binaries and coordinate with root Makefile
# Biblical: Nehemiah 4:6 - "So built we the wall... for the people had a mind to work"
#
# Architecture (2025-12-25):
#   claude-global/
#   ├── Makefile           - Root build system (delegates to modules)
#   ├── hooks/             - Lifecycle hooks (has own Makefile)
#   ├── pkg/               - Shared Go packages
#   ├── statusline/        - Status line implementation
#   ├── skills/            - Skill implementations
#   ├── entrypoints/       - Main entry points (cpisi, statusline)
#   └── system/runtime/    - Runtime scripts and future CPI-SI binaries
#       ├── bin/           - Runtime binaries (built by this script)
#       ├── cmd/           - Runtime command sources (TO BE ADDED)
#       ├── lib/           - Shared libraries
#       └── scripts/       - This script and others
#
# Current State:
#   - Core build handled by root Makefile (make build)
#   - Hooks built by hooks/Makefile (make -C hooks deploy)
#   - This script builds CPI-SI specific runtime binaries
#   - Runtime binaries TO BE IMPLEMENTED in Phase 3+
#
# Usage:
#   ./build.sh              - Build all runtime binaries
#   ./build.sh --status     - Show what would be built
#   ./build.sh --delegate   - Delegate to root Makefile
#
# ============================================================================

# ============================================================================
# SETUP
# ============================================================================
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SYSTEM_DIR="$(dirname "$SCRIPT_DIR")"
RUNTIME_DIR="$SYSTEM_DIR"
CLAUDE_GLOBAL="$(cd "$SYSTEM_DIR/../.." && pwd)"
BIN_DIR="$RUNTIME_DIR/bin"

# Colors
YELLOW='\033[1;33m'
GREEN='\033[0;32m'
RED='\033[0;31m'
CYAN='\033[0;36m'
NC='\033[0m'

# ============================================================================
# BODY - Main Logic
# ============================================================================

show_header() {
    echo -e "${YELLOW}╔════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${YELLOW}║         CPI-SI Runtime Build Script (2025-12-25)              ║${NC}"
    echo -e "${YELLOW}╚════════════════════════════════════════════════════════════════╝${NC}"
    echo ""
}

show_status() {
    echo -e "${CYAN}=== Current Architecture ===${NC}"
    echo ""
    echo "Root Makefile targets (use 'make' from claude-global/):"
    echo "  make build      - Build all modules"
    echo "  make hooks      - Build hooks module"
    echo "  make deploy     - Full deployment (sync to bereshit + rebuild hooks)"
    echo ""
    echo "Hooks Makefile targets (use 'make' from hooks/):"
    echo "  make            - Build all hooks"
    echo "  make deploy     - Build + sync to root"
    echo "  make status     - Show deployment state"
    echo ""
    echo -e "${CYAN}=== Runtime Binaries ===${NC}"
    echo ""
    echo "Location: $BIN_DIR"
    echo ""

    # Check what exists
    if [ -d "$BIN_DIR" ]; then
        local count=$(ls -1 "$BIN_DIR" 2>/dev/null | wc -l)
        if [ "$count" -gt 0 ]; then
            echo "Built binaries:"
            ls -la "$BIN_DIR"
        else
            echo "  (no binaries built yet)"
        fi
    else
        echo "  (bin/ directory does not exist)"
    fi

    echo ""
    echo -e "${CYAN}=== Planned Runtime Commands (Phase 3+) ===${NC}"
    echo ""
    echo "These will be implemented after state machine integration:"
    echo "  cpisi-status    - CPI-SI health and state check"
    echo "  cpisi-validate  - Validate installation and config"
    echo "  cpisi-diagnose  - Detailed diagnostics with rail correlation"
    echo "  cpisi-debug     - Interactive debugging with log analysis"
    echo ""
}

delegate_to_makefile() {
    echo -e "${CYAN}Delegating to root Makefile...${NC}"
    echo ""
    cd "$CLAUDE_GLOBAL"
    make build
}

build_runtime() {
    show_header

    # Ensure bin directory exists
    mkdir -p "$BIN_DIR"

    # Check for cmd directory with sources
    CMD_DIR="$RUNTIME_DIR/cmd"

    if [ ! -d "$CMD_DIR" ]; then
        echo -e "${YELLOW}Note: $CMD_DIR does not exist yet${NC}"
        echo ""
        echo "Runtime command sources will be added in Phase 3+"
        echo "For now, use the root Makefile for core builds:"
        echo ""
        echo "  cd $CLAUDE_GLOBAL"
        echo "  make build      # Build all modules"
        echo "  make deploy     # Full deployment"
        echo ""
        echo -e "${CYAN}Or build hooks directly:${NC}"
        echo ""
        echo "  cd $CLAUDE_GLOBAL/hooks"
        echo "  make deploy     # Build and sync hooks"
        echo ""
        exit 0
    fi

    # If cmd/ exists, build whatever is there
    echo -e "${CYAN}Building runtime commands...${NC}"
    echo ""

    SUCCESS=0
    FAILED=0

    for cmd_dir in "$CMD_DIR"/*/; do
        if [ -d "$cmd_dir" ]; then
            cmd_name=$(basename "$cmd_dir")
            echo -n "Building $cmd_name... "

            if go build -o "$BIN_DIR/$cmd_name" "$cmd_dir" 2>/dev/null; then
                echo -e "${GREEN}✓${NC}"
                SUCCESS=$((SUCCESS + 1))
            else
                echo -e "${RED}✗${NC}"
                FAILED=$((FAILED + 1))
            fi
        fi
    done

    echo ""
    echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "  Build Results: ${GREEN}$SUCCESS passed${NC}, ${RED}$FAILED failed${NC}"
    echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

    if [ $FAILED -eq 0 ]; then
        echo ""
        echo -e "${GREEN}✓ All runtime binaries built successfully${NC}"
        exit 0
    else
        echo ""
        echo -e "${RED}✗ Some builds failed${NC}"
        exit 1
    fi
}

# ============================================================================
# CLOSING - Entry Point
# ============================================================================

case "${1:-}" in
    --status|-s)
        show_header
        show_status
        ;;
    --delegate|-d)
        show_header
        delegate_to_makefile
        ;;
    --help|-h)
        show_header
        echo "Usage: $0 [option]"
        echo ""
        echo "Options:"
        echo "  (none)        Build runtime binaries"
        echo "  --status, -s  Show current build state"
        echo "  --delegate, -d  Delegate to root Makefile"
        echo "  --help, -h    Show this help"
        ;;
    *)
        build_runtime
        ;;
esac
