#!/bin/bash
# =============================================================================
# METADATA
# =============================================================================
#
# Key:         millenniumos-run-uefi-simple-script
# Title:       MillenniumOS UEFI Simple Runner
# Type:        Shell Script
# Component:   Build Infrastructure
# Role:        Launch MillenniumOS in QEMU with UEFI boot using vvfat (no disk image)
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
#              the earth." Simple mode for rapid development iteration.
#
# Boot Mode Ladder:
#   ┌────────────────────────────────────────────────────────────┐
#   │  run-uefi.sh        │ PRODUCTION — Full GPT + ESP image   │
#   ├────────────────────────────────────────────────────────────┤
#   │  run-uefi-simple.sh │ DEVELOPMENT — QEMU vvfat (THIS)     │
#   ├────────────────────────────────────────────────────────────┤
#   │  run-uefi-debug.sh  │ DEBUG — GDB server enabled          │
#   ├────────────────────────────────────────────────────────────┤
#   │  run-bios.sh        │ LEGACY — SeaBIOS fallback           │
#   └────────────────────────────────────────────────────────────┘
#
# When to Use:
#   - Quick smoke tests during development
#   - When mtools/mkfs.fat not available
#   - Rapid iteration (no disk image rebuild)
#   - NOT for production testing (use run-uefi.sh)
#
# Trade-offs:
#   + Faster startup (no disk image creation)
#   + No external tools needed (gdisk, mtools)
#   - Less compatible with some OVMF features
#   - Cannot test disk/partition operations
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
readonly TOOLS_DIR="${ROOT_DIR}/tools"

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
readonly DEBUG_LOG="${BUILD_DIR}/debug.log"
readonly OVMF_LOG="${BUILD_DIR}/ovmf.log"
readonly OVMF_VARS_LOCAL="${BUILD_DIR}/OVMF_VARS.fd"

# -----------------------------------------------------------------------------
# S.5 UEFI Simple Defaults (from config or fallback)
# -----------------------------------------------------------------------------

# Machine configuration
SIMPLE_MACHINE="${QEMU_MACHINE:-q35}"
SIMPLE_MACHINE_OPTS="${QEMU_MACHINE_OPTS:-smm=off}"
SIMPLE_CPU="${QEMU_CPU:-qemu64}"
SIMPLE_MEMORY="${QEMU_MEMORY:-512M}"
SIMPLE_VGA="${QEMU_VGA:-std}"

# OVMF firmware paths (system and EDK2-built)
SIMPLE_SYS_OVMF_CODE="${SYS_OVMF_CODE:-/usr/share/OVMF/OVMF_CODE_4M.fd}"
SIMPLE_SYS_OVMF_VARS="${SYS_OVMF_VARS:-/usr/share/OVMF/OVMF_VARS_4M.fd}"
SIMPLE_EDK2_OVMF_CODE="${EDK2_OVMF_CODE:-${TOOLS_DIR}/edk2/Build/OvmfX64/DEBUG_GCC5/FV/OVMF_CODE.fd}"
SIMPLE_EDK2_OVMF_VARS="${EDK2_OVMF_VARS:-${TOOLS_DIR}/edk2/Build/OvmfX64/DEBUG_GCC5/FV/OVMF_VARS.fd}"

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

# -----------------------------------------------------------------------------
# B.2 OVMF Detection
# -----------------------------------------------------------------------------

# Will be set by detect_ovmf()
OVMF_CODE=""
OVMF_VARS=""
OVMF_SOURCE=""

detect_ovmf() {
    # Prefer EDK2-built OVMF if available
    if [[ -f "${SIMPLE_EDK2_OVMF_CODE}" ]]; then
        OVMF_CODE="${SIMPLE_EDK2_OVMF_CODE}"
        OVMF_VARS="${SIMPLE_EDK2_OVMF_VARS}"
        OVMF_SOURCE="EDK2 Build"
        log_success "Using EDK2-built OVMF"
        return 0
    fi

    # Fall back to system OVMF
    if [[ -f "${SIMPLE_SYS_OVMF_CODE}" ]]; then
        OVMF_CODE="${SIMPLE_SYS_OVMF_CODE}"
        OVMF_VARS="${SIMPLE_SYS_OVMF_VARS}"
        OVMF_SOURCE="System Package"
        log_success "Using system OVMF"
        return 0
    fi

    # No OVMF found
    log_error "OVMF firmware not found"
    echo ""
    echo "    Checked:"
    echo "      - EDK2: ${SIMPLE_EDK2_OVMF_CODE}"
    echo "      - System: ${SIMPLE_SYS_OVMF_CODE}"
    echo ""
    echo "    Install with: sudo apt install ovmf"
    echo ""
    exit 1
}

# -----------------------------------------------------------------------------
# B.3 Pre-flight Checks
# -----------------------------------------------------------------------------

check_bootloader() {
    if [[ ! -f "${ESP_DIR}/EFI/BOOT/BOOTX64.EFI" ]]; then
        log_error "UEFI bootloader not found at ${ESP_DIR}/EFI/BOOT/BOOTX64.EFI"
        echo ""
        echo "    Build with: make"
        echo "    Or run:     ./scripts/run.sh build"
        echo ""
        exit 1
    fi
    log_success "UEFI bootloader found"
}

check_kernel() {
    if [[ ! -f "${ESP_DIR}/kernel.bin" ]]; then
        log_warn "kernel.bin not found in ESP (bootloader may fail to load kernel)"
    else
        local kernel_size
        kernel_size=$(stat -c%s "${ESP_DIR}/kernel.bin" 2>/dev/null || echo "0")
        log_success "kernel.bin found (${kernel_size} bytes)"
    fi
}

# -----------------------------------------------------------------------------
# B.4 OVMF Setup
# -----------------------------------------------------------------------------

setup_ovmf_vars() {
    # Fresh OVMF_VARS each run to avoid stale boot entries
    log_info "Copying fresh OVMF_VARS..."
    cp "${OVMF_VARS}" "${OVMF_VARS_LOCAL}"
    log_success "OVMF_VARS ready"
}

# -----------------------------------------------------------------------------
# B.5 Debug Log Setup
# -----------------------------------------------------------------------------

setup_debug_logs() {
    # Clear previous logs
    rm -f "${DEBUG_LOG}" "${OVMF_LOG}"

    # Initialize debug log
    {
        echo "=== MillenniumOS Debug Log (Simple vvfat Mode) ==="
        echo "Started: $(date)"
        echo "OVMF Source: ${OVMF_SOURCE}"
        echo "Machine: ${SIMPLE_MACHINE}, CPU: ${SIMPLE_CPU}, Memory: ${SIMPLE_MEMORY}"
        echo "Mode: QEMU vvfat (development fallback)"
        echo "===================================================="
        echo ""
    } > "${DEBUG_LOG}"

    # Initialize OVMF log
    {
        echo "=== OVMF Firmware Log (Simple vvfat Mode) ==="
        echo "Started: $(date)"
        echo "=============================================="
        echo ""
    } > "${OVMF_LOG}"

    log_success "Debug logs initialized"
}

# -----------------------------------------------------------------------------
# B.6 Status Display
# -----------------------------------------------------------------------------

print_status() {
    echo ""
    echo -e "${GOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo -e "${BOLD}  ${OS_NAME:-MillenniumOS}${NC} ${DIM}— UEFI Simple Mode (vvfat)${NC}"
    echo -e "${GOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo ""
    echo -e "${CYAN}Machine:${NC}     ${SIMPLE_MACHINE}"
    echo -e "${CYAN}CPU:${NC}         ${SIMPLE_CPU}"
    echo -e "${CYAN}Memory:${NC}      ${SIMPLE_MEMORY}"
    echo -e "${CYAN}Graphics:${NC}    GOP framebuffer (${SIMPLE_VGA})"
    echo -e "${CYAN}Boot Mode:${NC}   UEFI (OVMF/EDK2)"
    echo ""
    echo -e "${MAGENTA}OVMF Source:${NC} ${OVMF_SOURCE}"
    echo -e "${MAGENTA}Storage:${NC}     QEMU vvfat (directory mounted as FAT)"
    echo ""
    echo -e "${YELLOW}Debug log:${NC}   ${DEBUG_LOG}"
    echo -e "${YELLOW}OVMF log:${NC}    ${OVMF_LOG}"
    echo -e "${YELLOW}Live tail:${NC}   tail -f ${DEBUG_LOG}"
    echo ""
}

print_esp_contents() {
    echo -e "${GOLD}───────────────────────────────────────────────────────────────────${NC}"
    echo -e "${BOLD}  ESP Directory Contents${NC}"
    echo -e "${GOLD}───────────────────────────────────────────────────────────────────${NC}"
    echo ""
    echo -e "  ${CYAN}Root:${NC}"
    find "${ESP_DIR}" -maxdepth 1 -printf "    %M %u %g %s %f\n" 2>/dev/null || echo "    (empty)"
    echo ""
    echo -e "  ${CYAN}EFI/BOOT/:${NC}"
    find "${ESP_DIR}/EFI/BOOT" -maxdepth 1 -printf "    %M %u %g %s %f\n" 2>/dev/null || echo "    (empty)"
    echo ""
    echo -e "${GOLD}───────────────────────────────────────────────────────────────────${NC}"
    echo ""
}

print_mode_warning() {
    echo -e "${YELLOW}╭─────────────────────────────────────────────────────────────────╮${NC}"
    echo -e "${YELLOW}│${NC}  ${BOLD}⚠ Development Mode${NC}                                            ${YELLOW}│${NC}"
    echo -e "${YELLOW}│${NC}                                                                 ${YELLOW}│${NC}"
    echo -e "${YELLOW}│${NC}  Using QEMU vvfat — fast but less compatible.                   ${YELLOW}│${NC}"
    echo -e "${YELLOW}│${NC}  For production testing, use: ${GREEN}./scripts/run.sh${NC}                 ${YELLOW}│${NC}"
    echo -e "${YELLOW}╰─────────────────────────────────────────────────────────────────╯${NC}"
    echo ""
}

# -----------------------------------------------------------------------------
# B.7 QEMU Launch
# -----------------------------------------------------------------------------

launch_qemu() {
    local gdb_mode="${1:-0}"

    # Build QEMU arguments
    local -a qemu_args=(
        -machine "${SIMPLE_MACHINE},${SIMPLE_MACHINE_OPTS}"
        -cpu "${SIMPLE_CPU}"
        -m "${SIMPLE_MEMORY}"

        # OVMF firmware
        -drive "if=pflash,format=raw,unit=0,readonly=on,file=${OVMF_CODE}"
        -drive "if=pflash,format=raw,unit=1,file=${OVMF_VARS_LOCAL}"

        # ESP via QEMU's vvfat driver (directory mounted as virtual FAT filesystem)
        # This is the key difference from run-uefi.sh which uses a proper disk image
        -drive "format=raw,file=fat:rw:${ESP_DIR},media=disk"

        # Boot menu enabled for development flexibility
        -boot "menu=on"

        # Display
        -vga "${SIMPLE_VGA}"
        -display "gtk,show-cursor=on"

        # Serial output - dual port configuration
        # COM1 (0x3F8) → Kernel debug, COM2 (0x2F8) → OVMF
        -serial "file:${DEBUG_LOG}"
        -serial "file:${OVMF_LOG}"

        # Monitor on stdio
        -monitor stdio

        # Don't auto-reboot on crash
        -no-reboot
    )

    # Add GDB server if requested
    if [[ ${gdb_mode} -eq 1 ]]; then
        qemu_args+=(-s -S)
        log_warn "GDB mode: Waiting for connection on :1234"
        echo "    Connect with: gdb -ex 'target remote :1234'"
        echo ""
    fi

    # Launch
    log_success "Launching QEMU (UEFI Simple mode)..."
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
    GDB_MODE=0

    while [[ $# -gt 0 ]]; do
        case "$1" in
            --gdb)
                GDB_MODE=1
                shift
                ;;
            --help|-h)
                echo "Usage: $0 [--gdb]"
                echo ""
                echo "UEFI Simple Mode — Development fallback using QEMU vvfat"
                echo ""
                echo "Options:"
                echo "  --gdb   Start GDB server on :1234 and pause for connection"
                echo "  --help  Show this help message"
                echo ""
                echo "Boot Mode Ladder:"
                echo "  run-uefi.sh        PRODUCTION — Full GPT + ESP image"
                echo "  run-uefi-simple.sh DEVELOPMENT — QEMU vvfat (this script)"
                echo "  run-uefi-debug.sh  DEBUG — GDB server enabled"
                echo "  run-bios.sh        LEGACY — SeaBIOS fallback"
                echo ""
                echo "For production testing, use: ./scripts/run.sh"
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

    # Detect OVMF
    detect_ovmf

    # Pre-flight checks
    check_bootloader
    check_kernel

    # Setup
    setup_ovmf_vars
    setup_debug_logs

    # Display status
    print_status
    print_esp_contents
    print_mode_warning

    # Launch
    launch_qemu "${GDB_MODE}"
}

# -----------------------------------------------------------------------------
# X.3 Script Execution
# -----------------------------------------------------------------------------

main "$@"
