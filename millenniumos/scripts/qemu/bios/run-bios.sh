#!/bin/bash
# =============================================================================
# METADATA
# =============================================================================
#
# Key:         millenniumos-run-bios-script
# Title:       MillenniumOS BIOS Runner
# Type:        Shell Script
# Component:   Build Infrastructure
# Role:        Launch MillenniumOS in QEMU with BIOS/legacy boot
#
# Status:      Active
# Version:     2.0.0
# Created:     2025-01-30
# Updated:     2025-01-31
#
# Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
# Organization: CreativeWorkzStudio LLC
#
# Grounding:   Genesis 1:1 — "In the beginning God created the heaven and
#              the earth." Legacy boot path for compatibility.
#
# =============================================================================
# END METADATA
# =============================================================================

# =============================================================================
# SETUP
# =============================================================================

# -----------------------------------------------------------------------------
# S.1 Shell Configuration
# -----------------------------------------------------------------------------

set -e
set -u
set -o pipefail

# -----------------------------------------------------------------------------
# S.2 Path Configuration
# -----------------------------------------------------------------------------

# Get real script location (handles symlinks)
SCRIPT_PATH=""
SCRIPT_PATH="$(readlink -f "$0" 2>/dev/null || realpath "$0" 2>/dev/null || echo "$0")"
SCRIPT_DIR=""
SCRIPT_DIR="$(cd "$(dirname "${SCRIPT_PATH}")" && pwd)"
readonly SCRIPT_DIR

# Navigate: scripts/qemu/bios/ → scripts/qemu/ → scripts/ → root/
ROOT_DIR=""
ROOT_DIR="$(cd "${SCRIPT_DIR}/../../.." && pwd)"
readonly ROOT_DIR

# Verify we found the project root
if [[ ! -f "${ROOT_DIR}/Makefile" ]]; then
    echo "ERROR: Could not locate project root (no Makefile found at ${ROOT_DIR})" >&2
    exit 1
fi

readonly BUILD_DIR="${ROOT_DIR}/build"
readonly CONFIG_DIR="${SCRIPT_DIR}/../config"

# -----------------------------------------------------------------------------
# S.3 Load Configuration
# -----------------------------------------------------------------------------

readonly BIOS_CONFIG="${CONFIG_DIR}/bios.cfg"
readonly UEFI_CONFIG="${CONFIG_DIR}/uefi.cfg"

# Load config (use UEFI config for shared values, BIOS config if exists)
if [[ -f "${UEFI_CONFIG}" ]]; then
    # shellcheck source=/dev/null
    source "${UEFI_CONFIG}"
fi
if [[ -f "${BIOS_CONFIG}" ]]; then
    # shellcheck source=/dev/null
    source "${BIOS_CONFIG}"
fi

# -----------------------------------------------------------------------------
# S.4 Output Files
# -----------------------------------------------------------------------------

readonly OS_IMAGE="${BUILD_DIR}/millenniumos.img"
readonly DEBUG_LOG="${BUILD_DIR}/debug.log"

# -----------------------------------------------------------------------------
# S.5 BIOS-Specific Defaults (override config if needed)
# -----------------------------------------------------------------------------

# Machine - use older i440fx for BIOS compatibility
BIOS_MACHINE="${BIOS_MACHINE:-pc}"
BIOS_CPU="${QEMU_CPU:-qemu64}"
BIOS_MEMORY="${QEMU_MEMORY:-512M}"
BIOS_VGA="${QEMU_VGA:-std}"

# -----------------------------------------------------------------------------
# S.6 Color Palette
# -----------------------------------------------------------------------------

readonly GOLD='\033[1;33m'
readonly GREEN='\033[0;32m'
readonly CYAN='\033[0;36m'
readonly RED='\033[0;31m'
readonly YELLOW='\033[0;33m'
readonly DIM='\033[2m'
readonly BOLD='\033[1m'
readonly NC='\033[0m'

# =============================================================================
# BODY
# =============================================================================

# -----------------------------------------------------------------------------
# B.1 Output Functions
# -----------------------------------------------------------------------------

log_info() {
    echo -e "${CYAN}[INFO]${NC} $*"
}

log_success() {
    echo -e "${GREEN}[OK]${NC} $*"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*" >&2
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $*"
}

# -----------------------------------------------------------------------------
# B.2 Pre-flight Checks
# -----------------------------------------------------------------------------

check_image() {
    if [[ ! -f "${OS_IMAGE}" ]]; then
        log_error "OS image not found at ${OS_IMAGE}"
        echo ""
        echo "    Build with: make image"
        echo "    Or run:     ./scripts/run.sh build"
        echo ""
        exit 1
    fi
    log_success "OS image found: ${OS_IMAGE}"
}

# -----------------------------------------------------------------------------
# B.3 Debug Log Setup
# -----------------------------------------------------------------------------

setup_debug_log() {
    # Clear previous log
    rm -f "${DEBUG_LOG}"

    # Initialize log
    {
        echo "=== MillenniumOS Debug Log (BIOS Mode) ==="
        echo "Started: $(date)"
        echo "Image: ${OS_IMAGE}"
        echo "Machine: ${BIOS_MACHINE}, CPU: ${BIOS_CPU}, Memory: ${BIOS_MEMORY}"
        echo "==========================================="
        echo ""
    } > "${DEBUG_LOG}"

    log_success "Debug log initialized: ${DEBUG_LOG}"
}

# -----------------------------------------------------------------------------
# B.4 Status Display
# -----------------------------------------------------------------------------

print_status() {
    echo ""
    echo -e "${GOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo -e "${BOLD}  ${OS_NAME:-MillenniumOS}${NC} ${DIM}— BIOS Boot (Legacy Mode)${NC}"
    echo -e "${GOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo ""
    echo -e "${CYAN}Machine:${NC}    ${BIOS_MACHINE}"
    echo -e "${CYAN}CPU:${NC}        ${BIOS_CPU}"
    echo -e "${CYAN}Memory:${NC}     ${BIOS_MEMORY}"
    echo -e "${CYAN}Graphics:${NC}   VESA fallback chain (1024x768 → 800x600 → 640x480 → VGA Text)"
    echo -e "${CYAN}Boot Mode:${NC}  BIOS (SeaBIOS)"
    echo ""
    echo -e "${YELLOW}Debug log:${NC}  ${DEBUG_LOG}"
    echo -e "${YELLOW}Live tail:${NC}  tail -f ${DEBUG_LOG}"
    echo ""
}

# -----------------------------------------------------------------------------
# B.5 QEMU Launch
# -----------------------------------------------------------------------------

launch_qemu() {
    local debug_mode="${1:-0}"
    local gdb_mode="${2:-0}"

    # Build QEMU arguments
    local -a qemu_args=(
        -machine "${BIOS_MACHINE}"
        -cpu "${BIOS_CPU}"
        -m "${BIOS_MEMORY}"

        # Storage - modern blockdev syntax (QEMU 10.x compatible)
        -blockdev "driver=file,filename=${OS_IMAGE},node-name=disk-file"
        -blockdev "driver=raw,file=disk-file,node-name=disk"
        -device "ide-hd,drive=disk"

        # Display
        -vga "${BIOS_VGA}"
        -display "gtk,show-cursor=on"

        # Serial output to log file
        -serial "file:${DEBUG_LOG}"

        # Monitor on stdio
        -monitor stdio

        # Don't auto-reboot on crash
        -no-reboot
    )

    # Add debug options
    if [[ ${debug_mode} -eq 1 ]]; then
        qemu_args+=(-d "int,cpu_reset")
        log_info "Debug mode enabled (interrupts, CPU resets)"
    fi

    # Add GDB server
    if [[ ${gdb_mode} -eq 1 ]]; then
        qemu_args+=(-s -S)
        log_warn "GDB mode: Waiting for connection on :1234"
        echo "    Connect with: gdb -ex 'target remote :1234'"
        echo ""
    fi

    # Launch
    log_success "Launching QEMU (BIOS mode)..."
    echo ""
    exec qemu-system-x86_64 "${qemu_args[@]}"
}

# =============================================================================
# CLOSING
# =============================================================================

# -----------------------------------------------------------------------------
# X.1 Script Argument Parsing
# -----------------------------------------------------------------------------

parse_args() {
    DEBUG_MODE=0
    GDB_MODE=0

    while [[ $# -gt 0 ]]; do
        case "$1" in
            --debug)
                DEBUG_MODE=1
                shift
                ;;
            --gdb)
                GDB_MODE=1
                shift
                ;;
            *)
                shift
                ;;
        esac
    done
}

# -----------------------------------------------------------------------------
# X.2 Script Main Entry Point
# -----------------------------------------------------------------------------

main() {
    parse_args "$@"

    # Pre-flight
    check_image

    # Setup
    setup_debug_log

    # Display status
    print_status

    # Launch
    launch_qemu "${DEBUG_MODE}" "${GDB_MODE}"
}

# -----------------------------------------------------------------------------
# X.3 Script Execution
# -----------------------------------------------------------------------------

main "$@"
