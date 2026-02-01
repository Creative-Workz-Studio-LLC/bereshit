#!/bin/bash
# =============================================================================
# METADATA
# =============================================================================
#
# Key:         millenniumos-run-uefi-debug-script
# Title:       MillenniumOS UEFI Debug Runner
# Type:        Shell Script
# Component:   Build Infrastructure
# Role:        Launch MillenniumOS in QEMU with UEFI boot and GDB debugging
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
#              the earth." Debug mode reveals the inner workings.
#
# Usage:       ./run-uefi-debug.sh [--no-wait]
#              --no-wait  Start immediately without waiting for GDB
#
# Debug Steps:
#   1. Run this script (QEMU pauses waiting for GDB)
#   2. In another terminal: gdb
#   3. In GDB: target remote :1234
#   4. In GDB: symbol-file ../../build/kernel.elf (if available)
#   5. In GDB: continue
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

# Navigate: scripts/qemu/uefi/ → scripts/qemu/ → scripts/ → root/
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

readonly UEFI_CONFIG="${CONFIG_DIR}/uefi.cfg"

# Load UEFI configuration
if [[ -f "${UEFI_CONFIG}" ]]; then
    # shellcheck source=/dev/null
    source "${UEFI_CONFIG}"
fi

# -----------------------------------------------------------------------------
# S.4 Output Files
# -----------------------------------------------------------------------------

readonly ESP_DIR="${BUILD_DIR}/esp"
readonly ESP_IMAGE="${BUILD_DIR}/esp.img"
readonly DEBUG_LOG="${BUILD_DIR}/debug.log"
readonly OVMF_LOG="${BUILD_DIR}/ovmf.log"
readonly OVMF_VARS_LOCAL="${BUILD_DIR}/OVMF_VARS.fd"

# -----------------------------------------------------------------------------
# S.5 UEFI Debug Defaults (from config or fallback)
# -----------------------------------------------------------------------------

# Machine configuration
DEBUG_MACHINE="${QEMU_MACHINE:-q35}"
DEBUG_MACHINE_OPTS="${QEMU_MACHINE_OPTS:-smm=off}"
DEBUG_CPU="${QEMU_CPU:-qemu64}"
DEBUG_MEMORY="${QEMU_MEMORY:-512M}"
DEBUG_VGA="${QEMU_VGA:-std}"

# OVMF firmware paths
DEBUG_OVMF_CODE="${SYS_OVMF_CODE:-/usr/share/OVMF/OVMF_CODE_4M.fd}"
DEBUG_OVMF_VARS="${SYS_OVMF_VARS:-/usr/share/OVMF/OVMF_VARS_4M.fd}"

# GDB configuration
DEBUG_GDB_PORT="${GDB_PORT:-1234}"

# -----------------------------------------------------------------------------
# S.6 Color Palette
# -----------------------------------------------------------------------------

readonly GOLD='\033[1;33m'
readonly GREEN='\033[0;32m'
readonly CYAN='\033[0;36m'
readonly RED='\033[0;31m'
readonly YELLOW='\033[0;33m'
readonly MAGENTA='\033[0;35m'
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

log_debug() {
    echo -e "${MAGENTA}[DEBUG]${NC} $*"
}

# -----------------------------------------------------------------------------
# B.2 Pre-flight Checks
# -----------------------------------------------------------------------------

check_ovmf() {
    if [[ ! -f "${DEBUG_OVMF_CODE}" ]]; then
        log_error "OVMF firmware not found at ${DEBUG_OVMF_CODE}"
        echo ""
        echo "    Install with: sudo apt install ovmf"
        echo ""
        exit 1
    fi
    log_success "OVMF firmware found"
}

check_esp_image() {
    if [[ ! -f "${ESP_IMAGE}" ]]; then
        log_error "ESP image not found at ${ESP_IMAGE}"
        echo ""
        echo "    Build with: make"
        echo "    Or run:     ./scripts/run.sh build"
        echo ""
        exit 1
    fi
    log_success "ESP image found: ${ESP_IMAGE}"
}

check_bootloader() {
    if [[ ! -f "${ESP_DIR}/EFI/BOOT/BOOTX64.EFI" ]]; then
        log_error "UEFI bootloader not found at ${ESP_DIR}/EFI/BOOT/BOOTX64.EFI"
        echo ""
        echo "    Build with: make"
        echo ""
        exit 1
    fi
    log_success "UEFI bootloader found"
}

# -----------------------------------------------------------------------------
# B.3 OVMF Setup
# -----------------------------------------------------------------------------

setup_ovmf_vars() {
    if [[ ! -f "${OVMF_VARS_LOCAL}" ]]; then
        log_info "Copying OVMF_VARS for local modification..."
        cp "${DEBUG_OVMF_VARS}" "${OVMF_VARS_LOCAL}"
        log_success "OVMF_VARS copied to ${OVMF_VARS_LOCAL}"
    fi
}

# -----------------------------------------------------------------------------
# B.4 Debug Log Setup
# -----------------------------------------------------------------------------

setup_debug_logs() {
    # Clear previous logs
    rm -f "${DEBUG_LOG}" "${OVMF_LOG}"

    # Initialize debug log
    {
        echo "=== MillenniumOS Debug Log (UEFI Debug Mode) ==="
        echo "Started: $(date)"
        echo "GDB Port: ${DEBUG_GDB_PORT}"
        echo "Machine: ${DEBUG_MACHINE}, CPU: ${DEBUG_CPU}, Memory: ${DEBUG_MEMORY}"
        echo "================================================="
        echo ""
    } > "${DEBUG_LOG}"

    # Initialize OVMF log
    {
        echo "=== OVMF Firmware Log (UEFI Debug Mode) ==="
        echo "Started: $(date)"
        echo "============================================"
        echo ""
    } > "${OVMF_LOG}"

    log_success "Debug logs initialized"
}

# -----------------------------------------------------------------------------
# B.5 Status Display
# -----------------------------------------------------------------------------

print_status() {
    echo ""
    echo -e "${GOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo -e "${BOLD}  ${OS_NAME:-MillenniumOS}${NC} ${DIM}— UEFI Debug Mode (GDB Enabled)${NC}"
    echo -e "${GOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo ""
    echo -e "${CYAN}Machine:${NC}    ${DEBUG_MACHINE}"
    echo -e "${CYAN}CPU:${NC}        ${DEBUG_CPU}"
    echo -e "${CYAN}Memory:${NC}     ${DEBUG_MEMORY}"
    echo -e "${CYAN}Graphics:${NC}   GOP framebuffer (${DEBUG_VGA})"
    echo -e "${CYAN}Boot Mode:${NC}  UEFI (OVMF/EDK2)"
    echo ""
    echo -e "${MAGENTA}GDB Server:${NC}   localhost:${DEBUG_GDB_PORT}"
    echo -e "${MAGENTA}GDB Status:${NC}   ${YELLOW}PAUSED — Waiting for debugger${NC}"
    echo ""
    echo -e "${YELLOW}Debug log:${NC}  ${DEBUG_LOG}"
    echo -e "${YELLOW}OVMF log:${NC}   ${OVMF_LOG}"
    echo ""
}

print_gdb_instructions() {
    echo -e "${GOLD}───────────────────────────────────────────────────────────────────${NC}"
    echo -e "${BOLD}  GDB Connection Instructions${NC}"
    echo -e "${GOLD}───────────────────────────────────────────────────────────────────${NC}"
    echo ""
    echo -e "  ${CYAN}1.${NC} Open another terminal"
    echo -e "  ${CYAN}2.${NC} Run: ${GREEN}gdb${NC}"
    echo -e "  ${CYAN}3.${NC} In GDB: ${GREEN}target remote :${DEBUG_GDB_PORT}${NC}"
    echo -e "  ${CYAN}4.${NC} (Optional) Load symbols: ${GREEN}symbol-file build/kernel.elf${NC}"
    echo -e "  ${CYAN}5.${NC} In GDB: ${GREEN}continue${NC}"
    echo ""
    echo -e "${GOLD}───────────────────────────────────────────────────────────────────${NC}"
    echo ""
}

# -----------------------------------------------------------------------------
# B.6 QEMU Launch
# -----------------------------------------------------------------------------

launch_qemu() {
    local wait_for_gdb="${1:-1}"

    # Build QEMU arguments
    local -a qemu_args=(
        -machine "${DEBUG_MACHINE},${DEBUG_MACHINE_OPTS}"
        -cpu "${DEBUG_CPU}"
        -m "${DEBUG_MEMORY}"

        # OVMF firmware
        -drive "if=pflash,format=raw,unit=0,readonly=on,file=${DEBUG_OVMF_CODE}"
        -drive "if=pflash,format=raw,unit=1,file=${OVMF_VARS_LOCAL}"

        # Storage - AHCI controller with GPT disk
        -drive "file=${ESP_IMAGE},format=raw,if=none,id=disk0"
        -device "ahci,id=ahci"
        -device "ide-hd,drive=disk0,bus=ahci.0"

        # Display
        -vga "${DEBUG_VGA}"
        -display "gtk,show-cursor=on"

        # Serial output - dual port configuration
        # COM1 (0x3F8) → Kernel debug, COM2 (0x2F8) → OVMF
        -serial "file:${DEBUG_LOG}"
        -serial "file:${OVMF_LOG}"

        # Monitor on stdio
        -monitor stdio

        # GDB server
        -s

        # Debug tracing
        -d "int,cpu_reset,guest_errors"

        # Don't auto-reboot on crash
        -no-reboot
    )

    # Add CPU pause for GDB connection
    if [[ ${wait_for_gdb} -eq 1 ]]; then
        qemu_args+=(-S)
        log_warn "QEMU will PAUSE waiting for GDB connection"
    else
        log_info "QEMU will start immediately (--no-wait specified)"
    fi

    # Launch
    log_success "Launching QEMU (UEFI Debug mode)..."
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
    WAIT_FOR_GDB=1

    while [[ $# -gt 0 ]]; do
        case "$1" in
            --no-wait)
                WAIT_FOR_GDB=0
                shift
                ;;
            --help|-h)
                echo "Usage: $0 [--no-wait]"
                echo ""
                echo "Options:"
                echo "  --no-wait  Start QEMU immediately without waiting for GDB"
                echo "  --help     Show this help message"
                exit 0
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

    # Pre-flight checks
    check_ovmf
    check_esp_image
    check_bootloader

    # Setup
    setup_ovmf_vars
    setup_debug_logs

    # Display status
    print_status

    # Show GDB instructions if waiting
    if [[ ${WAIT_FOR_GDB} -eq 1 ]]; then
        print_gdb_instructions
    fi

    # Launch
    launch_qemu "${WAIT_FOR_GDB}"
}

# -----------------------------------------------------------------------------
# X.3 Script Execution
# -----------------------------------------------------------------------------

main "$@"
