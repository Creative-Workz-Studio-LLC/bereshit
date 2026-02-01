#!/bin/bash
# =============================================================================
# METADATA
# =============================================================================
#
# Key:         millenniumos-run-script
# Title:       MillenniumOS Launcher
# Type:        Shell Script
# Component:   Build Infrastructure
# Role:        Main entry point for building and running MillenniumOS
#
# Status:      Active
# Version:     2.1.0
# Created:     2025-01-30
# Updated:     2025-01-31
#
# Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
# Organization: CreativeWorkzStudio LLC
#
# Grounding:   Genesis 1:1 — "In the beginning God created the heaven and
#              the earth." This script creates order from source.
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

set -e  # Exit on error
set -u  # Exit on undefined variable
set -o pipefail  # Exit on pipe failure

# -----------------------------------------------------------------------------
# S.2 Path Configuration
# -----------------------------------------------------------------------------

# Declare first, then assign (prevents masking return codes)
SCRIPT_DIR=""
SCRIPT_DIR="$(cd "$(dirname "$(readlink -f "$0")")" && pwd)"
readonly SCRIPT_DIR

ROOT_DIR=""
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
readonly ROOT_DIR

readonly BUILD_DIR="${ROOT_DIR}/build"
readonly QEMU_DIR="${SCRIPT_DIR}/qemu"
readonly CONFIG_DIR="${QEMU_DIR}/config"
readonly UEFI_CONFIG="${CONFIG_DIR}/uefi.cfg"

# -----------------------------------------------------------------------------
# S.2b Load Configuration
# -----------------------------------------------------------------------------

# Load UEFI config if it exists (provides all QEMU settings)
if [[ -f "${UEFI_CONFIG}" ]]; then
    # shellcheck source=/dev/null
    source "${UEFI_CONFIG}"
fi

# -----------------------------------------------------------------------------
# S.3 Output Files
# -----------------------------------------------------------------------------

# BIOS boot chain
readonly STAGE0A_BIN="${BUILD_DIR}/stage0a.bin"
readonly STAGE0B_BIN="${BUILD_DIR}/stage0b.bin"
readonly KERNEL_BIN="${BUILD_DIR}/kernel.bin"
readonly KERNEL_ELF="${BUILD_DIR}/kernel.elf"
readonly OS_IMAGE="${BUILD_DIR}/millenniumos.img"

# UEFI boot chain
readonly ESP_DIR="${BUILD_DIR}/esp"
readonly ESP_IMAGE="${BUILD_DIR}/esp.img"
readonly UEFI_BOOTLOADER="${ESP_DIR}/EFI/BOOT/BOOTX64.EFI"
readonly ESP_KERNEL="${ESP_DIR}/kernel.bin"

# Debug output
readonly DEBUG_LOG="${BUILD_DIR}/debug.log"
readonly OVMF_LOG="${BUILD_DIR}/ovmf.log"

# -----------------------------------------------------------------------------
# S.4 Color Palette — Kingdom Theme
# -----------------------------------------------------------------------------

readonly GOLD='\033[1;33m'
readonly ROYAL='\033[0;34m'
readonly WHITE='\033[1;37m'
readonly GREEN='\033[0;32m'
readonly RED='\033[0;31m'
readonly CYAN='\033[0;36m'
readonly YELLOW='\033[0;33m'
readonly MAGENTA='\033[0;35m'
readonly DIM='\033[2m'
readonly BOLD='\033[1m'
readonly NC='\033[0m'

# -----------------------------------------------------------------------------
# S.5 Required Tools
# -----------------------------------------------------------------------------

readonly REQUIRED_TOOLS=(
    "nasm"
    "gcc"
    "ld"
    "objcopy"
    "make"
    "qemu-system-x86_64"
)

readonly UEFI_TOOLS=(
    "sgdisk"
    "mkfs.fat"
    "mcopy"
)

# -----------------------------------------------------------------------------
# S.6 Scripture Verses
# -----------------------------------------------------------------------------

readonly VERSES=(
    "The LORD is my light and my salvation — Psalm 27:1"
    "Trust in the LORD with all thine heart — Proverbs 3:5"
    "I can do all things through Christ — Philippians 4:13"
    "Be strong and of a good courage — Joshua 1:9"
    "The LORD is my shepherd — Psalm 23:1"
    "Commit thy works unto the LORD — Proverbs 16:3"
    "For God hath not given us the spirit of fear — 2 Timothy 1:7"
    "Thy word is a lamp unto my feet — Psalm 119:105"
    "The fear of the LORD is the beginning of wisdom — Proverbs 9:10"
    "In all thy ways acknowledge him — Proverbs 3:6"
    "Be still, and know that I am God — Psalm 46:10"
    "The LORD is good, a strong hold in trouble — Nahum 1:7"
)

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

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $*"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*" >&2
}

log_step() {
    echo -e "${MAGENTA}[STEP]${NC} $*"
}

print_divider() {
    echo -e "${DIM}────────────────────────────────────────────────────────────${NC}"
}

# -----------------------------------------------------------------------------
# B.2 Banner & Branding
# -----------------------------------------------------------------------------

print_banner() {
    echo ""
    echo -e "${GOLD}    ╔═══════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${GOLD}    ║${NC}                                                                   ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${WHITE}       ███╗   ███╗ ██╗ ██╗      ██╗      ███████╗ ███╗   ██╗       ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${WHITE}       ████╗ ████║ ██║ ██║      ██║      ██╔════╝ ████╗  ██║       ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${WHITE}       ██╔████╔██║ ██║ ██║      ██║      █████╗   ██╔██╗ ██║       ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${WHITE}       ██║╚██╔╝██║ ██║ ██║      ██║      ██╔══╝   ██║╚██╗██║       ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${WHITE}       ██║ ╚═╝ ██║ ██║ ███████╗ ███████╗ ███████╗ ██║ ╚████║       ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${WHITE}       ╚═╝     ╚═╝ ╚═╝ ╚══════╝ ╚══════╝ ╚══════╝ ╚═╝  ╚═══╝       ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${NC}                                                                   ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${ROYAL}                    N   I   U   M   O   S                          ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${NC}                                                                   ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${DIM}              Kingdom Technology Operating System                  ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${NC}                                                                   ${GOLD}║${NC}"
    echo -e "${GOLD}    ╠═══════════════════════════════════════════════════════════════════╣${NC}"
    echo -e "${GOLD}    ║${NC}                                                                   ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${WHITE}     \"In the beginning God created the heaven and the earth.\"     ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${DIM}                         — Genesis 1:1                              ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${NC}                                                                   ${GOLD}║${NC}"
    echo -e "${GOLD}    ╠═══════════════════════════════════════════════════════════════════╣${NC}"
    echo -e "${GOLD}    ║${NC}                                                                   ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${CYAN}                   CreativeWorkzStudio LLC                         ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${DIM}                  Redeeming Technology for the Kingdom              ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${NC}                                                                   ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${DIM}                         © 2025-2026                                ${GOLD}║${NC}"
    echo -e "${GOLD}    ║${NC}                                                                   ${GOLD}║${NC}"
    echo -e "${GOLD}    ╚═══════════════════════════════════════════════════════════════════╝${NC}"
    echo ""
}

print_verse() {
    local idx=$((RANDOM % ${#VERSES[@]}))
    echo -e "${DIM}    ${VERSES[$idx]}${NC}"
    echo ""
}

print_mini_banner() {
    echo ""
    echo -e "${GOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo -e "${WHITE}  ${OS_NAME:-MillenniumOS}${NC} ${DIM}— ${OS_TAGLINE:-Kingdom Technology Operating System}${NC}"
    echo -e "${GOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo ""
}

# -----------------------------------------------------------------------------
# B.3 Component Checking
# -----------------------------------------------------------------------------

check_component() {
    local path="$1"
    local name="$2"
    local size=""

    if [[ -f "${path}" ]]; then
        size=$(du -h "${path}" 2>/dev/null | cut -f1)
        echo -e "    ${GREEN}✓${NC} ${name} ${DIM}(${size})${NC}"
    else
        echo -e "    ${RED}✗${NC} ${name} ${DIM}(not built)${NC}"
    fi
    # Always return 0 - this is for display, not flow control
    return 0
}

check_tool() {
    local tool="$1"
    if command -v "${tool}" &>/dev/null; then
        return 0
    else
        return 1
    fi
}

# -----------------------------------------------------------------------------
# B.4 Requirements Checking
# -----------------------------------------------------------------------------

check_requirements() {
    local missing=0

    log_step "Checking build requirements..."
    echo ""

    echo -e "    ${BOLD}Core Tools${NC}"
    for tool in "${REQUIRED_TOOLS[@]}"; do
        if check_tool "${tool}"; then
            echo -e "    ${GREEN}✓${NC} ${tool}"
        else
            echo -e "    ${RED}✗${NC} ${tool} ${DIM}(missing)${NC}"
            missing=1
        fi
    done
    echo ""

    echo -e "    ${BOLD}UEFI Tools${NC}"
    for tool in "${UEFI_TOOLS[@]}"; do
        if check_tool "${tool}"; then
            echo -e "    ${GREEN}✓${NC} ${tool}"
        else
            echo -e "    ${YELLOW}○${NC} ${tool} ${DIM}(optional)${NC}"
        fi
    done
    echo ""

    if [[ ${missing} -eq 1 ]]; then
        log_error "Missing required tools."
        echo ""
        echo "    Install with:"
        echo "      sudo apt install nasm build-essential qemu-system-x86"
        echo ""
        exit 1
    fi

    log_success "All required tools present"
    echo ""
}

check_uefi_requirements() {
    local missing=0

    for tool in "${UEFI_TOOLS[@]}"; do
        if ! check_tool "${tool}"; then
            missing=1
        fi
    done

    if [[ ${missing} -eq 1 ]]; then
        log_warn "Some UEFI tools missing. Install with:"
        echo "      sudo apt install gdisk dosfstools mtools"
        return 1
    fi
    return 0
}

# -----------------------------------------------------------------------------
# B.5 Help System
# -----------------------------------------------------------------------------

print_help() {
    print_banner
    print_verse

    echo -e "${BOLD}USAGE${NC}"
    echo "    ./scripts/run.sh [command] [options]"
    echo ""

    echo -e "${BOLD}COMMANDS${NC}"
    echo -e "    ${GREEN}(default)${NC}     Build if needed, run in UEFI mode"
    echo -e "    ${GREEN}build${NC}         Build MillenniumOS"
    echo -e "    ${GREEN}run${NC}           Run in QEMU (requires built image)"
    echo -e "    ${GREEN}clean${NC}         Clean build artifacts"
    echo -e "    ${GREEN}rebuild${NC}       Clean and rebuild"
    echo -e "    ${GREEN}status${NC}        Show build status"
    echo -e "    ${GREEN}check${NC}         Check requirements"
    echo -e "    ${GREEN}logs${NC}          Show debug logs"
    echo -e "    ${GREEN}help${NC}          Show this help"
    echo ""

    echo -e "${BOLD}BOOT MODES${NC}"
    echo -e "    ${CYAN}--uefi${NC}        UEFI mode with GOP graphics ${DIM}(default, modern)${NC}"
    echo -e "    ${CYAN}--bios${NC}        BIOS mode with VESA graphics ${DIM}(legacy)${NC}"
    echo ""

    echo -e "${BOLD}DEBUG OPTIONS${NC}"
    echo -e "    ${CYAN}--debug${NC}       Enable debug output"
    echo -e "    ${CYAN}--gdb${NC}         Start GDB server on :1234"
    echo -e "    ${CYAN}--verbose${NC}     Verbose build output"
    echo ""

    echo -e "${BOLD}EXAMPLES${NC}"
    echo "    ./scripts/run.sh                 # Build + run UEFI"
    echo "    ./scripts/run.sh --bios          # Build + run BIOS"
    echo "    ./scripts/run.sh build           # Just build"
    echo "    ./scripts/run.sh status          # Check components"
    echo "    ./scripts/run.sh logs            # View debug logs"
    echo ""

    print_architecture
}

print_architecture() {
    echo -e "${BOLD}ARCHITECTURE${NC}"
    echo ""
    echo "    ┌─────────────────────────────────────────────────────────────┐"
    echo "    │                      MillenniumOS                           │"
    echo "    │           Kingdom Technology Operating System               │"
    echo "    ├─────────────────────────────────────────────────────────────┤"
    echo "    │                                                             │"
    echo "    │   UEFI Boot (Primary)        │   BIOS Boot (Fallback)      │"
    echo "    │   ├─ BOOTX64.EFI             │   ├─ Stage 0a (MBR)         │"
    echo "    │   ├─ GOP Framebuffer         │   ├─ Stage 0b (Loader)      │"
    echo "    │   └─ kernel.bin              │   └─ kernel.bin             │"
    echo "    │                                                             │"
    echo "    ├─────────────────────────────────────────────────────────────┤"
    echo "    │                        Kernel                               │"
    echo "    │   ├─ entry.asm (64-bit entry point)                        │"
    echo "    │   ├─ kernel.c (kernel main)                                │"
    echo "    │   ├─ DAR Witness System (logging)                          │"
    echo "    │   └─ Subsystems (memory, interrupts, timer, scheduler)     │"
    echo "    │                                                             │"
    echo "    └─────────────────────────────────────────────────────────────┘"
    echo ""
}

# -----------------------------------------------------------------------------
# B.6 Status Display
# -----------------------------------------------------------------------------

print_status() {
    print_mini_banner
    print_verse

    echo -e "${BOLD}BUILD STATUS${NC}"
    print_divider
    echo ""

    # UEFI Components
    echo -e "    ${BOLD}UEFI Boot Chain${NC}"
    check_component "${UEFI_BOOTLOADER}" "BOOTX64.EFI (bootloader)"
    check_component "${ESP_KERNEL}" "kernel.bin (ESP copy)"
    check_component "${ESP_IMAGE}" "esp.img (disk image)"
    echo ""

    # BIOS Components
    echo -e "    ${BOLD}BIOS Boot Chain${NC}"
    check_component "${STAGE0A_BIN}" "stage0a.bin (MBR)"
    check_component "${STAGE0B_BIN}" "stage0b.bin (loader)"
    check_component "${KERNEL_BIN}" "kernel.bin"
    check_component "${OS_IMAGE}" "millenniumos.img (disk)"
    echo ""

    # Kernel artifacts
    echo -e "    ${BOLD}Kernel Artifacts${NC}"
    check_component "${KERNEL_ELF}" "kernel.elf (debug symbols)"
    echo ""

    # Debug logs
    echo -e "    ${BOLD}Debug Logs${NC}"
    if [[ -f "${DEBUG_LOG}" ]]; then
        local log_lines
        log_lines=$(wc -l < "${DEBUG_LOG}" 2>/dev/null || echo "0")
        echo -e "    ${GREEN}✓${NC} debug.log ${DIM}(${log_lines} lines)${NC}"
    else
        echo -e "    ${DIM}○${NC} debug.log ${DIM}(no log yet)${NC}"
    fi
    if [[ -f "${OVMF_LOG}" ]]; then
        local ovmf_lines
        ovmf_lines=$(wc -l < "${OVMF_LOG}" 2>/dev/null || echo "0")
        echo -e "    ${GREEN}✓${NC} ovmf.log ${DIM}(${ovmf_lines} lines)${NC}"
    else
        echo -e "    ${DIM}○${NC} ovmf.log ${DIM}(no log yet)${NC}"
    fi
    echo ""

    # Configuration
    echo -e "    ${BOLD}Configuration${NC}"
    if [[ -f "${UEFI_CONFIG}" ]]; then
        echo -e "    ${GREEN}✓${NC} uefi.cfg ${DIM}(loaded)${NC}"
        echo -e "    ${DIM}  Machine: ${QEMU_MACHINE:-q35}, CPU: ${QEMU_CPU:-qemu64}, Memory: ${QEMU_MEMORY:-512M}${NC}"
        echo -e "    ${DIM}  Version: ${OS_VERSION:-unknown}${NC}"
    else
        echo -e "    ${RED}✗${NC} uefi.cfg ${DIM}(not found)${NC}"
    fi
    echo ""

    print_divider
}

# -----------------------------------------------------------------------------
# B.7 Log Viewing
# -----------------------------------------------------------------------------

show_logs() {
    local log_type="${1:-all}"

    print_mini_banner

    case "${log_type}" in
        debug)
            if [[ -f "${DEBUG_LOG}" ]]; then
                echo -e "${BOLD}Debug Log (${DEBUG_LOG})${NC}"
                print_divider
                tail -50 "${DEBUG_LOG}"
            else
                log_warn "No debug log found. Run the OS first."
            fi
            ;;
        ovmf)
            if [[ -f "${OVMF_LOG}" ]]; then
                echo -e "${BOLD}OVMF Log (${OVMF_LOG})${NC}"
                print_divider
                tail -50 "${OVMF_LOG}"
            else
                log_warn "No OVMF log found. Run in UEFI mode first."
            fi
            ;;
        all|*)
            if [[ -f "${DEBUG_LOG}" ]]; then
                echo -e "${BOLD}Debug Log (last 30 lines)${NC}"
                print_divider
                tail -30 "${DEBUG_LOG}"
                echo ""
            fi
            if [[ -f "${OVMF_LOG}" ]]; then
                echo -e "${BOLD}OVMF Log (last 20 lines)${NC}"
                print_divider
                tail -20 "${OVMF_LOG}"
            fi
            if [[ ! -f "${DEBUG_LOG}" ]] && [[ ! -f "${OVMF_LOG}" ]]; then
                log_warn "No logs found. Run the OS first."
            fi
            ;;
    esac
    echo ""
}

# -----------------------------------------------------------------------------
# B.8 Build Operations
# -----------------------------------------------------------------------------

do_build() {
    print_mini_banner
    check_requirements

    log_step "Building MillenniumOS..."
    print_divider
    echo ""

    local -a make_args=("-C" "${ROOT_DIR}")
    if [[ ${VERBOSE} -eq 1 ]]; then
        make_args+=("V=1")
        log_info "Verbose build enabled"
    fi

    if ! make "${make_args[@]}"; then
        log_error "Build failed!"
        exit 1
    fi

    echo ""
    print_divider
    echo ""
    echo -e "${GREEN}╔═══════════════════════════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║                    Build Complete!                        ║${NC}"
    echo -e "${GREEN}╚═══════════════════════════════════════════════════════════╝${NC}"
    echo ""
    echo "    Run with:  ./scripts/run.sh"
    echo "    Or:        make run"
    echo ""
    print_verse
}

do_clean() {
    print_mini_banner
    log_step "Cleaning build artifacts..."

    if make -C "${ROOT_DIR}" clean; then
        log_success "Clean complete"
    else
        log_error "Clean failed"
        exit 1
    fi
    echo ""
}

# -----------------------------------------------------------------------------
# B.9 Run Operations (Delegates to QEMU scripts)
# -----------------------------------------------------------------------------

ensure_built_uefi() {
    if [[ ! -f "${UEFI_BOOTLOADER}" ]]; then
        log_info "UEFI bootloader not found. Building first..."
        do_build
    fi
}

ensure_built_bios() {
    if [[ ! -f "${OS_IMAGE}" ]]; then
        log_info "BIOS image not found. Building first..."
        do_build
    fi
}

# Run UEFI mode via dedicated script
use_run_uefi_script() {
    ensure_built_uefi
    exec "${QEMU_DIR}/uefi/run-uefi.sh"
}

# Run BIOS mode via dedicated script
use_run_bios_script() {
    local extra_args="${1:-}"
    ensure_built_bios
    if [[ -n "${extra_args}" ]]; then
        exec "${QEMU_DIR}/bios/run-bios.sh" "${extra_args}"
    else
        exec "${QEMU_DIR}/bios/run-bios.sh"
    fi
}

# Run UEFI debug mode via dedicated script
use_run_uefi_debug_script() {
    ensure_built_uefi
    exec "${QEMU_DIR}/uefi/run-uefi-debug.sh"
}

# Run UEFI simple mode via dedicated script
use_run_uefi_simple_script() {
    ensure_built_uefi
    exec "${QEMU_DIR}/uefi/run-uefi-simple.sh"
}

# =============================================================================
# CLOSING
# =============================================================================

# -----------------------------------------------------------------------------
# X.1 Script Argument Parsing
# -----------------------------------------------------------------------------

parse_args() {
    # Defaults
    CMD=""
    BOOT_MODE="uefi"
    DEBUG_MODE=0
    GDB_MODE=0
    VERBOSE=0

    while [[ $# -gt 0 ]]; do
        case "$1" in
            # Boot modes
            --uefi)
                BOOT_MODE="uefi"
                shift
                ;;
            --bios)
                BOOT_MODE="bios"
                shift
                ;;

            # Debug options
            --debug)
                DEBUG_MODE=1
                shift
                ;;
            --gdb)
                GDB_MODE=1
                shift
                ;;
            --verbose|-v)
                VERBOSE=1
                shift
                ;;

            # Commands
            help|--help|-h)
                CMD="help"
                shift
                ;;
            build|clean|rebuild|run|status|check|logs)
                CMD="$1"
                shift
                ;;

            # Unknown
            *)
                if [[ -z "${CMD}" ]]; then
                    CMD="$1"
                fi
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

    case "${CMD}" in
        help)
            print_help
            ;;
        build)
            do_build
            ;;
        clean)
            do_clean
            ;;
        rebuild)
            do_clean
            do_build
            ;;
        status)
            print_status
            ;;
        check)
            print_mini_banner
            check_requirements
            check_uefi_requirements || true
            ;;
        logs)
            show_logs
            ;;
        run|"")
            if [[ ${DEBUG_MODE} -eq 1 ]] || [[ ${GDB_MODE} -eq 1 ]]; then
                if [[ "${BOOT_MODE}" == "bios" ]]; then
                    use_run_bios_script --debug
                else
                    use_run_uefi_debug_script
                fi
            elif [[ "${BOOT_MODE}" == "bios" ]]; then
                use_run_bios_script
            else
                use_run_uefi_script
            fi
            ;;
        *)
            log_error "Unknown command: ${CMD}"
            echo "Run: ./scripts/run.sh help"
            exit 1
            ;;
    esac
}

# -----------------------------------------------------------------------------
# X.3 Script Execution
# -----------------------------------------------------------------------------

main "$@"
