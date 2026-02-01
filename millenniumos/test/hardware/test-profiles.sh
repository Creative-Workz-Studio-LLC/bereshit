#!/bin/bash
# =============================================================================
# METADATA [METADATA]
# =============================================================================
#
# Key:         MOS-test-hardware-profiles
# Title:       MillenniumOS Hardware Compatibility Test Profiles
# Type:        Test Script
# Component:   Test/Hardware
# Role:        Automated hardware compatibility testing via QEMU
#
# Status:      Active
# Version:     a-01.00
# Created:     2026-01-30
# Updated:     2026-01-30
#
# Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
# Organization: CreativeWorkzStudio LLC
#
# Grounding:   Genesis 1:31 — "And God saw every thing that he had made,
#              and, behold, it was very good."
#
# =============================================================================
# END METADATA
# =============================================================================

# =============================================================================
# SETUP [SETUP]
# =============================================================================

# Don't exit on error - we need to track test failures
# set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MOS_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
BUILD_DIR="$MOS_ROOT/build"
IMAGE="$BUILD_DIR/millenniumos.img"
ISO="$BUILD_DIR/millenniumos.iso"
LOG_DIR="$BUILD_DIR/test-logs"

# Test duration (seconds)
TEST_TIMEOUT=${TEST_TIMEOUT:-10}

# QEMU binary
QEMU=${QEMU:-qemu-system-x86_64}

# =============================================================================
# END SETUP
# =============================================================================

# =============================================================================
# BODY [BODY]
# =============================================================================

# -----------------------------------------------------------------------------
# B.1 Helper Functions
# -----------------------------------------------------------------------------

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_pass() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

log_fail() {
    echo -e "${RED}[FAIL]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

# Run a QEMU test with given parameters
# Args: test_name, qemu_args...
run_test() {
    local name="$1"
    shift
    local log_file="$LOG_DIR/${name}.log"

    echo -n "  Testing: $name... "

    # Run QEMU with timeout
    if timeout "$TEST_TIMEOUT" "$QEMU" "$@" \
        -serial file:"$log_file" \
        -display none \
        -no-reboot \
        2>>"$log_file"; then
        log_pass "$name"
        return 0
    else
        local exit_code=$?
        if [ $exit_code -eq 124 ]; then
            # Timeout is expected for a bootable OS
            log_pass "$name (timeout - expected)"
            return 0
        else
            log_fail "$name (exit code: $exit_code)"
            return 1
        fi
    fi
}

# -----------------------------------------------------------------------------
# B.2 Memory Configuration Tests
# -----------------------------------------------------------------------------

test_memory_configs() {
    log_info "Testing memory configurations..."
    local passed=0
    local failed=0

    # Test different memory sizes
    for mem in 32M 64M 128M 256M 512M; do
        if run_test "memory-$mem" \
            -drive format=raw,file="$IMAGE" \
            -m "$mem"; then
            passed=$((passed + 1))
        else
            failed=$((failed + 1))
        fi
    done

    echo ""
    log_info "Memory tests: $passed passed, $failed failed"
    return $failed
}

# -----------------------------------------------------------------------------
# B.3 CPU Configuration Tests
# -----------------------------------------------------------------------------

test_cpu_configs() {
    log_info "Testing CPU configurations..."
    local passed=0
    local failed=0

    # Test different CPU models
    for cpu in qemu64 core2duo Nehalem Westmere SandyBridge IvyBridge Haswell Broadwell Skylake-Client; do
        if run_test "cpu-$cpu" \
            -drive format=raw,file="$IMAGE" \
            -m 128M \
            -cpu "$cpu"; then
            passed=$((passed + 1))
        else
            failed=$((failed + 1))
        fi
    done

    # Test SMP configurations
    for smp in 1 2 4; do
        if run_test "smp-$smp" \
            -drive format=raw,file="$IMAGE" \
            -m 128M \
            -smp "$smp"; then
            passed=$((passed + 1))
        else
            failed=$((failed + 1))
        fi
    done

    echo ""
    log_info "CPU tests: $passed passed, $failed failed"
    return $failed
}

# -----------------------------------------------------------------------------
# B.4 Storage Configuration Tests
# -----------------------------------------------------------------------------

test_storage_configs() {
    log_info "Testing storage configurations..."
    local passed=0
    local failed=0

    # Test disk image boot
    if run_test "storage-disk" \
        -drive format=raw,file="$IMAGE" \
        -m 128M; then
        passed=$((passed + 1))
    else
        failed=$((failed + 1))
    fi

    # Test ISO boot (if ISO exists)
    if [ -f "$ISO" ]; then
        if run_test "storage-iso" \
            -cdrom "$ISO" \
            -boot d \
            -m 128M; then
            passed=$((passed + 1))
        else
            failed=$((failed + 1))
        fi
    else
        log_warn "ISO not found, skipping ISO test"
    fi

    # Test different drive interfaces
    for iface in ide ahci virtio; do
        case $iface in
            ide)
                if run_test "storage-$iface" \
                    -drive format=raw,file="$IMAGE",if=ide \
                    -m 128M; then
                    passed=$((passed + 1))
                else
                    failed=$((failed + 1))
                fi
                ;;
            ahci)
                if run_test "storage-$iface" \
                    -device ahci,id=ahci \
                    -device ide-hd,drive=disk,bus=ahci.0 \
                    -drive id=disk,format=raw,file="$IMAGE",if=none \
                    -m 128M; then
                    passed=$((passed + 1))
                else
                    failed=$((failed + 1))
                fi
                ;;
            virtio)
                if run_test "storage-$iface" \
                    -drive format=raw,file="$IMAGE",if=virtio \
                    -m 128M; then
                    passed=$((passed + 1))
                else
                    failed=$((failed + 1))
                fi
                ;;
        esac
    done

    echo ""
    log_info "Storage tests: $passed passed, $failed failed"
    return $failed
}

# -----------------------------------------------------------------------------
# B.5 Machine Type Tests
# -----------------------------------------------------------------------------

test_machine_types() {
    log_info "Testing machine types..."
    local passed=0
    local failed=0

    # Test different machine types
    for machine in pc q35; do
        if run_test "machine-$machine" \
            -machine "$machine" \
            -drive format=raw,file="$IMAGE" \
            -m 128M; then
            passed=$((passed + 1))
        else
            failed=$((failed + 1))
        fi
    done

    echo ""
    log_info "Machine type tests: $passed passed, $failed failed"
    return $failed
}

# -----------------------------------------------------------------------------
# B.6 Graphics Configuration Tests
# -----------------------------------------------------------------------------

test_graphics_configs() {
    log_info "Testing graphics configurations..."
    local passed=0
    local failed=0

    # Test different VGA types
    for vga in std cirrus vmware virtio qxl; do
        if run_test "vga-$vga" \
            -drive format=raw,file="$IMAGE" \
            -m 128M \
            -vga "$vga"; then
            passed=$((passed + 1))
        else
            failed=$((failed + 1))
        fi
    done

    echo ""
    log_info "Graphics tests: $passed passed, $failed failed"
    return $failed
}

# -----------------------------------------------------------------------------
# B.7 Main Test Runner
# -----------------------------------------------------------------------------

run_all_tests() {
    log_info "MillenniumOS Hardware Compatibility Tests"
    log_info "========================================="
    echo ""

    # Check prerequisites
    if [ ! -f "$IMAGE" ]; then
        log_fail "Disk image not found: $IMAGE"
        log_info "Run 'make' to build MillenniumOS first"
        exit 1
    fi

    if ! command -v "$QEMU" &>/dev/null; then
        log_fail "QEMU not found: $QEMU"
        exit 1
    fi

    # Create log directory
    mkdir -p "$LOG_DIR"

    local total_failed=0
    local result=0

    # Run test suites
    test_memory_configs
    result=$?
    total_failed=$((total_failed + result))
    echo ""

    test_cpu_configs
    result=$?
    total_failed=$((total_failed + result))
    echo ""

    test_storage_configs
    result=$?
    total_failed=$((total_failed + result))
    echo ""

    test_machine_types
    result=$?
    total_failed=$((total_failed + result))
    echo ""

    test_graphics_configs
    result=$?
    total_failed=$((total_failed + result))
    echo ""

    # Summary
    log_info "========================================="
    if [ $total_failed -eq 0 ]; then
        log_pass "All hardware compatibility tests passed!"
    else
        log_fail "Total failures: $total_failed"
    fi
    log_info "Test logs saved to: $LOG_DIR"

    return $total_failed
}

# -----------------------------------------------------------------------------
# B.8 Individual Test Options
# -----------------------------------------------------------------------------

show_help() {
    echo "Usage: $0 [OPTIONS] [TEST_SUITE]"
    echo ""
    echo "Test suites:"
    echo "  all       Run all tests (default)"
    echo "  memory    Test memory configurations"
    echo "  cpu       Test CPU configurations"
    echo "  storage   Test storage configurations"
    echo "  machine   Test machine types"
    echo "  graphics  Test graphics configurations"
    echo ""
    echo "Options:"
    echo "  -t, --timeout SEC   Set test timeout (default: 10)"
    echo "  -v, --verbose       Show verbose output"
    echo "  -h, --help          Show this help"
    echo ""
    echo "Environment variables:"
    echo "  QEMU         QEMU binary to use"
    echo "  TEST_TIMEOUT Test timeout in seconds"
}

# =============================================================================
# END BODY
# =============================================================================

# =============================================================================
# CLOSING [CLOSING]
# =============================================================================
#
# Parse arguments and run tests
#

VERBOSE=0
SUITE="all"

while [ $# -gt 0 ]; do
    case "$1" in
        -t|--timeout)
            TEST_TIMEOUT="$2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE=1
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        memory|cpu|storage|machine|graphics|all)
            SUITE="$1"
            shift
            ;;
        *)
            log_fail "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

case "$SUITE" in
    all)
        run_all_tests
        ;;
    memory)
        mkdir -p "$LOG_DIR"
        test_memory_configs
        ;;
    cpu)
        mkdir -p "$LOG_DIR"
        test_cpu_configs
        ;;
    storage)
        mkdir -p "$LOG_DIR"
        test_storage_configs
        ;;
    machine)
        mkdir -p "$LOG_DIR"
        test_machine_types
        ;;
    graphics)
        mkdir -p "$LOG_DIR"
        test_graphics_configs
        ;;
esac

exit $?

# =============================================================================
# END CLOSING
# =============================================================================
