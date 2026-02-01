#!/bin/bash
# =============================================================================
# METADATA
# =============================================================================
#
# Key:         millenniumos-run-uefi-script
# Title:       MillenniumOS UEFI Production Runner
# Type:        Shell Script
# Component:   Build Infrastructure
# Role:        Launch MillenniumOS in QEMU with full UEFI boot (GPT + ESP image)
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
#              the earth." Production boot — proper foundation for the Kingdom.
#
# Boot Mode Ladder:
#   ┌────────────────────────────────────────────────────────────┐
#   │  run-uefi.sh        │ PRODUCTION — Full GPT + ESP (THIS)  │
#   ├────────────────────────────────────────────────────────────┤
#   │  run-uefi-simple.sh │ DEVELOPMENT — QEMU vvfat            │
#   ├────────────────────────────────────────────────────────────┤
#   │  run-uefi-debug.sh  │ DEBUG — GDB server enabled          │
#   ├────────────────────────────────────────────────────────────┤
#   │  run-bios.sh        │ LEGACY — SeaBIOS fallback           │
#   └────────────────────────────────────────────────────────────┘
#
# Features:
#   - Creates proper GPT disk with EFI System Partition
#   - Auto-detects EDK2-built OVMF or falls back to system OVMF
#   - Dual serial output (OVMF → ovmf.log, Bootloader → debug.log)
#   - Q35 machine with AHCI controller (modern SATA)
#   - Kernel integrity verification after ESP creation
#   - Full debugging support with optional GDB
#
# Usage:
#   ./run-uefi.sh              # Normal production run
#   ./run-uefi.sh --debug      # Enable QEMU debug tracing
#   ./run-uefi.sh --gdb        # Wait for GDB connection on :1234
#   ./run-uefi.sh --rebuild    # Force ESP image rebuild
#   ./run-uefi.sh --help       # Show help
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
# S.4 Output Files and Directories
# -----------------------------------------------------------------------------

readonly ESP_DIR="${BUILD_DIR}/esp"
readonly ESP_IMAGE="${BUILD_DIR}/esp.img"
readonly DEBUG_LOG="${BUILD_DIR}/debug.log"
readonly OVMF_LOG="${BUILD_DIR}/ovmf.log"
readonly OVMF_VARS_LOCAL="${BUILD_DIR}/OVMF_VARS.fd"
readonly MTOOLSRC_FILE="${BUILD_DIR}/.mtoolsrc"

# -----------------------------------------------------------------------------
# S.5 UEFI Production Defaults (from config or fallback)
# -----------------------------------------------------------------------------

# Machine configuration
UEFI_MACHINE="${QEMU_MACHINE:-q35}"
UEFI_MACHINE_OPTS="${QEMU_MACHINE_OPTS:-smm=off}"
UEFI_CPU="${QEMU_CPU:-qemu64}"
UEFI_SMP="${QEMU_SMP:-2}"
UEFI_MEMORY="${QEMU_MEMORY:-512M}"
UEFI_VGA="${QEMU_VGA:-std}"

# OVMF firmware paths (system and EDK2-built)
UEFI_SYS_OVMF_CODE="${SYS_OVMF_CODE:-/usr/share/OVMF/OVMF_CODE_4M.fd}"
UEFI_SYS_OVMF_VARS="${SYS_OVMF_VARS:-/usr/share/OVMF/OVMF_VARS_4M.fd}"
UEFI_EDK2_OVMF_CODE="${EDK2_OVMF_CODE:-${TOOLS_DIR}/edk2/Build/OvmfX64/DEBUG_GCC5/FV/OVMF_CODE.fd}"
UEFI_EDK2_OVMF_VARS="${EDK2_OVMF_VARS:-${TOOLS_DIR}/edk2/Build/OvmfX64/DEBUG_GCC5/FV/OVMF_VARS.fd}"

# ESP configuration
UEFI_ESP_SIZE_MB="${ESP_SIZE_MB:-64}"
UEFI_ESP_PART_START="${ESP_PARTITION_START:-2048}"

# GDB configuration
UEFI_GDB_PORT="${GDB_PORT:-1234}"

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

log_step() {
    echo -e "${MAGENTA}[STEP]${NC} $*"
}

# -----------------------------------------------------------------------------
# B.2 OVMF Detection
# -----------------------------------------------------------------------------

# Will be set by detect_ovmf()
OVMF_CODE=""
OVMF_VARS=""
OVMF_SOURCE=""

detect_ovmf() {
    log_step "Detecting OVMF firmware..."

    # Prefer EDK2-built OVMF if available (more debug features)
    if [[ -f "${UEFI_EDK2_OVMF_CODE}" ]]; then
        OVMF_CODE="${UEFI_EDK2_OVMF_CODE}"
        OVMF_VARS="${UEFI_EDK2_OVMF_VARS}"
        OVMF_SOURCE="EDK2 Build"
        log_success "Using EDK2-built OVMF (debug enabled)"
        return 0
    fi

    # Fall back to system OVMF
    if [[ -f "${UEFI_SYS_OVMF_CODE}" ]]; then
        OVMF_CODE="${UEFI_SYS_OVMF_CODE}"
        OVMF_VARS="${UEFI_SYS_OVMF_VARS}"
        OVMF_SOURCE="System Package"
        log_success "Using system OVMF"
        return 0
    fi

    # No OVMF found
    log_error "OVMF firmware not found"
    echo ""
    echo "    Checked locations:"
    echo "      - EDK2: ${UEFI_EDK2_OVMF_CODE}"
    echo "      - System: ${UEFI_SYS_OVMF_CODE}"
    echo ""
    echo "    Install system OVMF:"
    echo "      sudo apt install ovmf"
    echo ""
    echo "    Or build EDK2 OVMF:"
    echo "      cd tools/edk2"
    echo "      source edksetup.sh"
    echo "      build -a X64 -t GCC5 -p OvmfPkg/OvmfPkgX64.dsc"
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
    local bootloader_size
    bootloader_size=$(stat -c%s "${ESP_DIR}/EFI/BOOT/BOOTX64.EFI" 2>/dev/null || echo "0")
    log_success "UEFI bootloader found (${bootloader_size} bytes)"
}

check_kernel() {
    if [[ ! -f "${ESP_DIR}/kernel.bin" ]]; then
        log_warn "kernel.bin not found in ESP (bootloader may fail to load kernel)"
        return 1
    fi
    local kernel_size
    kernel_size=$(stat -c%s "${ESP_DIR}/kernel.bin" 2>/dev/null || echo "0")
    log_success "kernel.bin found (${kernel_size} bytes)"
    return 0
}

check_tools() {
    local missing=0

    if ! command -v sgdisk &>/dev/null; then
        log_warn "sgdisk not found (install: sudo apt install gdisk)"
        missing=1
    fi

    if ! command -v mcopy &>/dev/null; then
        log_warn "mtools not found (install: sudo apt install mtools)"
        missing=1
    fi

    if [[ ${missing} -eq 1 ]]; then
        log_error "Required tools missing for ESP image creation"
        echo ""
        echo "    Install with: sudo apt install gdisk mtools dosfstools"
        echo ""
        exit 1
    fi

    log_success "Required tools available (sgdisk, mtools)"
}

# -----------------------------------------------------------------------------
# B.4 OVMF Setup
# -----------------------------------------------------------------------------

setup_ovmf_vars() {
    if [[ ! -f "${OVMF_VARS_LOCAL}" ]]; then
        log_info "Copying OVMF_VARS for writable UEFI variables..."
        cp "${OVMF_VARS}" "${OVMF_VARS_LOCAL}"
        log_success "OVMF_VARS ready"
    else
        log_info "Using existing OVMF_VARS"
    fi
}

# -----------------------------------------------------------------------------
# B.5 ESP Image Creation — GPT with EFI System Partition
# -----------------------------------------------------------------------------

create_esp_image() {
    local img="${ESP_IMAGE}"
    local size_mb="${UEFI_ESP_SIZE_MB}"
    local part_start="${UEFI_ESP_PART_START}"

    echo ""
    echo -e "${GOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo -e "${BOLD}  Creating GPT Disk with EFI System Partition${NC}"
    echo -e "${GOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo ""

    # -------------------------------------------------------------------------
    # Step 1: Create empty disk image
    # -------------------------------------------------------------------------
    log_step "Creating ${size_mb}MB disk image..."
    dd if=/dev/zero of="${img}" bs=1M count="${size_mb}" status=none
    log_success "Disk image created"

    # -------------------------------------------------------------------------
    # Step 2: Create GPT partition table with EFI System Partition
    # -------------------------------------------------------------------------
    log_step "Creating GPT partition table..."
    sgdisk --clear \
           --new=1:"${part_start}":+60M \
           --typecode=1:EF00 \
           --change-name=1:"EFI System" \
           "${img}" >/dev/null 2>&1
    log_success "GPT with ESP partition created"

    # -------------------------------------------------------------------------
    # Step 3: Format the partition as FAT32
    # -------------------------------------------------------------------------
    log_step "Formatting partition as FAT32..."
    local part_offset=$((part_start * 512))
    local part_size=$((60 * 1024 * 1024))
    local part_sectors=$((part_size / 512))

    # Extract partition to temp file for formatting
    local part_img="${BUILD_DIR}/esp_partition.img"
    dd if="${img}" of="${part_img}" bs=512 skip="${part_start}" count="${part_sectors}" status=none

    if command -v mkfs.fat &>/dev/null; then
        mkfs.fat -F 32 -n "ESP" "${part_img}" >/dev/null 2>&1
        log_success "Formatted with mkfs.fat (FAT32)"
    else
        # Fallback to mformat
        local temp_mtoolsrc="${BUILD_DIR}/.mtoolsrc.tmp"
        cat > "${temp_mtoolsrc}" << EOF
drive e:
    file="${part_img}"
EOF
        MTOOLSRC="${temp_mtoolsrc}" mformat -F -v ESP e:
        rm -f "${temp_mtoolsrc}"
        log_success "Formatted with mformat (FAT32)"
    fi

    # Write formatted partition back to disk image
    dd if="${part_img}" of="${img}" bs=512 seek="${part_start}" conv=notrunc status=none

    # -------------------------------------------------------------------------
    # Step 4: Configure mtools for partition access
    # -------------------------------------------------------------------------
    log_step "Configuring mtools for ESP access..."
    cat > "${MTOOLSRC_FILE}" << EOF
drive e:
    file="${img}"
    offset=${part_offset}
    mtools_skip_check=1
EOF
    log_success "mtools configured"

    # -------------------------------------------------------------------------
    # Step 5: Create directory structure
    # -------------------------------------------------------------------------
    log_step "Creating EFI directory structure..."
    MTOOLSRC="${MTOOLSRC_FILE}" mmd e:/EFI 2>/dev/null || true
    MTOOLSRC="${MTOOLSRC_FILE}" mmd e:/EFI/BOOT 2>/dev/null || true
    log_success "Directory structure created"

    # -------------------------------------------------------------------------
    # Step 6: Copy bootloader
    # -------------------------------------------------------------------------
    log_step "Copying BOOTX64.EFI..."
    MTOOLSRC="${MTOOLSRC_FILE}" mcopy "${ESP_DIR}/EFI/BOOT/BOOTX64.EFI" e:/EFI/BOOT/
    log_success "Bootloader copied"

    # -------------------------------------------------------------------------
    # Step 7: Copy kernel
    # -------------------------------------------------------------------------
    if [[ -f "${ESP_DIR}/kernel.bin" ]]; then
        log_step "Copying kernel.bin..."
        MTOOLSRC="${MTOOLSRC_FILE}" mcopy "${ESP_DIR}/kernel.bin" e:/
        log_success "Kernel copied to ESP root"
    else
        log_warn "No kernel.bin found - bootloader may fail"
    fi

    # -------------------------------------------------------------------------
    # Step 8: Create startup.nsh for shell auto-boot
    # -------------------------------------------------------------------------
    log_step "Creating startup.nsh..."
    local startup_nsh="${BUILD_DIR}/startup.nsh"
    cat > "${startup_nsh}" << 'STARTUP_EOF'
@echo -off
echo MillenniumOS UEFI Boot
echo Kingdom Technology Operating System
echo.
fs0:
\EFI\BOOT\BOOTX64.EFI
STARTUP_EOF
    MTOOLSRC="${MTOOLSRC_FILE}" mcopy "${startup_nsh}" e:/startup.nsh
    rm -f "${startup_nsh}"
    log_success "startup.nsh created"

    # -------------------------------------------------------------------------
    # Step 9: Verify ESP contents
    # -------------------------------------------------------------------------
    echo ""
    echo -e "${GOLD}───────────────────────────────────────────────────────────────────${NC}"
    echo -e "${BOLD}  ESP Contents${NC}"
    echo -e "${GOLD}───────────────────────────────────────────────────────────────────${NC}"
    echo ""
    echo -e "  ${CYAN}Root (e:/)${NC}"
    MTOOLSRC="${MTOOLSRC_FILE}" mdir e:/ 2>/dev/null | sed 's/^/    /' || echo "    (empty)"
    echo ""
    echo -e "  ${CYAN}EFI/BOOT/ (e:/EFI/BOOT/)${NC}"
    MTOOLSRC="${MTOOLSRC_FILE}" mdir e:/EFI/BOOT/ 2>/dev/null | sed 's/^/    /' || echo "    (empty)"
    echo ""
    echo -e "${GOLD}───────────────────────────────────────────────────────────────────${NC}"

    # -------------------------------------------------------------------------
    # Step 10: Verify kernel integrity
    # -------------------------------------------------------------------------
    if [[ -f "${ESP_DIR}/kernel.bin" ]]; then
        log_step "Verifying kernel.bin integrity..."
        local verify_kernel="${BUILD_DIR}/verify_kernel.bin"
        MTOOLSRC="${MTOOLSRC_FILE}" mcopy e:/kernel.bin "${verify_kernel}" 2>/dev/null
        if cmp -s "${ESP_DIR}/kernel.bin" "${verify_kernel}"; then
            log_success "kernel.bin integrity verified"
        else
            log_error "kernel.bin integrity check FAILED"
            rm -f "${verify_kernel}"
            exit 1
        fi
        rm -f "${verify_kernel}"
    fi

    # Cleanup temp partition
    rm -f "${part_img}"

    echo ""
    log_success "GPT disk created: ${img}"
    echo ""
}

# -----------------------------------------------------------------------------
# B.6 ESP Image Management
# -----------------------------------------------------------------------------

should_rebuild_esp() {
    # Force rebuild if requested
    if [[ ${FORCE_REBUILD} -eq 1 ]]; then
        return 0
    fi

    # Rebuild if image doesn't exist
    if [[ ! -f "${ESP_IMAGE}" ]]; then
        return 0
    fi

    # Rebuild if bootloader is newer
    if [[ "${ESP_DIR}/EFI/BOOT/BOOTX64.EFI" -nt "${ESP_IMAGE}" ]]; then
        return 0
    fi

    # Rebuild if kernel is newer
    if [[ -f "${ESP_DIR}/kernel.bin" && "${ESP_DIR}/kernel.bin" -nt "${ESP_IMAGE}" ]]; then
        return 0
    fi

    # No rebuild needed
    return 1
}

manage_esp_image() {
    if should_rebuild_esp; then
        create_esp_image
    else
        log_info "ESP image up to date (use --rebuild to force)"
    fi
}

# -----------------------------------------------------------------------------
# B.7 Debug Log Setup
# -----------------------------------------------------------------------------

setup_debug_logs() {
    # Clear previous logs
    rm -f "${DEBUG_LOG}" "${OVMF_LOG}"

    # Initialize debug log
    {
        echo "=== MillenniumOS Debug Log (UEFI Production Mode) ==="
        echo "Started: $(date)"
        echo "OVMF Source: ${OVMF_SOURCE}"
        echo "Machine: ${UEFI_MACHINE}, CPU: ${UEFI_CPU}, Memory: ${UEFI_MEMORY}"
        echo "======================================================"
        echo ""
    } > "${DEBUG_LOG}"

    # Initialize OVMF log
    {
        echo "=== OVMF Firmware Log ==="
        echo "Started: $(date)"
        echo "========================="
        echo ""
    } > "${OVMF_LOG}"

    log_success "Debug logs initialized"
}

# -----------------------------------------------------------------------------
# B.8 Status Display
# -----------------------------------------------------------------------------

print_status() {
    echo ""
    echo -e "${GOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo -e "${BOLD}  ${OS_NAME:-MillenniumOS}${NC} ${DIM}— UEFI Production Mode${NC}"
    echo -e "${GOLD}═══════════════════════════════════════════════════════════════════${NC}"
    echo ""
    echo -e "${CYAN}Machine:${NC}     ${UEFI_MACHINE} (${UEFI_MACHINE_OPTS})"
    echo -e "${CYAN}CPU:${NC}         ${UEFI_CPU} (SMP: ${UEFI_SMP})"
    echo -e "${CYAN}Memory:${NC}      ${UEFI_MEMORY}"
    echo -e "${CYAN}Graphics:${NC}    GOP framebuffer (${UEFI_VGA})"
    echo -e "${CYAN}Boot Mode:${NC}   UEFI 64-bit"
    echo ""
    echo -e "${MAGENTA}OVMF Source:${NC} ${OVMF_SOURCE}"
    echo -e "${MAGENTA}Storage:${NC}     GPT disk with EFI System Partition (AHCI)"
    echo ""
    echo -e "${YELLOW}Debug log:${NC}   ${DEBUG_LOG}"
    echo -e "${YELLOW}OVMF log:${NC}    ${OVMF_LOG}"
    echo -e "${YELLOW}Live tail:${NC}   tail -f ${DEBUG_LOG}"
    echo ""
}

# -----------------------------------------------------------------------------
# B.9 QEMU Launch
# -----------------------------------------------------------------------------

launch_qemu() {
    local debug_mode="${1:-0}"
    local gdb_mode="${2:-0}"

    # Build QEMU arguments
    local -a qemu_args=(
        -machine "${UEFI_MACHINE},${UEFI_MACHINE_OPTS}"
        -cpu "${UEFI_CPU}"
        -smp "${UEFI_SMP}"
        -m "${UEFI_MEMORY}"

        # OVMF firmware (pflash for UEFI variables)
        -drive "if=pflash,format=raw,unit=0,readonly=on,file=${OVMF_CODE}"
        -drive "if=pflash,format=raw,unit=1,file=${OVMF_VARS_LOCAL}"

        # ESP disk on AHCI (modern SATA controller)
        -drive "file=${ESP_IMAGE},format=raw,if=none,id=disk0"
        -device "ahci,id=ahci"
        -device "ide-hd,drive=disk0,bus=ahci.0"

        # Display
        -vga "${UEFI_VGA}"
        -display "gtk,show-cursor=on"

        # Serial output - dual port configuration
        # COM1 (0x3F8) → Kernel/bootloader debug output (primary)
        # COM2 (0x2F8) → OVMF firmware console (secondary)
        -serial "file:${DEBUG_LOG}"
        -serial "file:${OVMF_LOG}"

        # Monitor on stdio
        -monitor stdio

        # Don't auto-reboot on crash
        -no-reboot
    )

    # Add debug tracing if requested
    if [[ ${debug_mode} -eq 1 ]]; then
        qemu_args+=(-d "int,cpu_reset,guest_errors")
        log_info "Debug tracing enabled (interrupts, CPU resets, guest errors)"
    fi

    # Add GDB server if requested
    if [[ ${gdb_mode} -eq 1 ]]; then
        qemu_args+=(-s -S)
        log_warn "GDB mode: QEMU will PAUSE waiting for connection on :${UEFI_GDB_PORT}"
        echo ""
        echo -e "    ${CYAN}Connect with:${NC} gdb -ex 'target remote :${UEFI_GDB_PORT}'"
        echo -e "    ${CYAN}Load symbols:${NC} symbol-file build/kernel.elf"
        echo ""
    fi

    # Launch
    log_success "Launching QEMU (UEFI Production mode)..."
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
    FORCE_REBUILD=0

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
            --rebuild)
                FORCE_REBUILD=1
                shift
                ;;
            --help|-h)
                print_help
                exit 0
                ;;
            *)
                shift
                ;;
        esac
    done
}

# -----------------------------------------------------------------------------
# X.2 Script Help
# -----------------------------------------------------------------------------

print_help() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "UEFI Production Mode — Full GPT disk with EFI System Partition"
    echo ""
    echo "Options:"
    echo "  --debug    Enable QEMU debug tracing (interrupts, CPU resets)"
    echo "  --gdb      Start GDB server on :${UEFI_GDB_PORT} and pause for connection"
    echo "  --rebuild  Force ESP image rebuild even if up to date"
    echo "  --help     Show this help message"
    echo ""
    echo "Boot Mode Ladder:"
    echo "  run-uefi.sh        PRODUCTION — Full GPT + ESP image (this script)"
    echo "  run-uefi-simple.sh DEVELOPMENT — QEMU vvfat (fast iteration)"
    echo "  run-uefi-debug.sh  DEBUG — GDB server always enabled"
    echo "  run-bios.sh        LEGACY — SeaBIOS fallback"
    echo ""
    echo "This is the PRODUCTION boot path. For quick development iteration,"
    echo "consider using: ./scripts/qemu/uefi/run-uefi-simple.sh"
    echo ""
    echo "Serial Ports:"
    echo "  COM1 (0x3F8) → build/ovmf.log   (OVMF firmware messages)"
    echo "  COM2 (0x2F8) → build/debug.log  (MillenniumOS bootloader/kernel)"
    echo ""
    echo "Live Monitoring:"
    echo "  tail -f build/debug.log"
}

# -----------------------------------------------------------------------------
# X.3 Script Main Entry Point
# -----------------------------------------------------------------------------

main() {
    parse_args "$@"

    # Detect OVMF
    detect_ovmf

    # Pre-flight checks
    check_tools
    check_bootloader
    check_kernel

    # Setup
    setup_ovmf_vars
    manage_esp_image
    setup_debug_logs

    # Display status
    print_status

    # Launch
    launch_qemu "${DEBUG_MODE}" "${GDB_MODE}"
}

# -----------------------------------------------------------------------------
# X.4 Script Execution
# -----------------------------------------------------------------------------

main "$@"
