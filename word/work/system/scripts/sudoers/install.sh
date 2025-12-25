#!/bin/bash
# ============================================================================
# METADATA
# ============================================================================
# Sudoers Installation Script - CPI-SI Interactive Terminal System
# Purpose: Safely install sudoers configuration with validation
# Blocking: Requires manual execution with elevated privileges, validates before install
# Usage: ./install.sh
#        (Automatically elevates with pkexec if needed - GUI password prompt)
#
# HEALTH SCORING MAP (TRUE SCORE):
# ----------------------------------
# Action 1/6: Source logging library (+5 or -5)
# Action 2/6: Validate source file syntax (+50 or -50) - CRITICAL
# Action 3/6: Backup existing configuration (+12 or -12)
# Action 4/6: Copy file to destination (+30 or -30)
# Action 5/6: Set correct permissions (+25 or -25)
# Action 6/6: Verify installed configuration (+48 or -48) - CRITICAL
#
# Total Possible: 170 points
# Normalization: (cumulative_health / 170) × 100

# ============================================================================
# SETUP
# ============================================================================
set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SYSTEM_DIR="$(dirname "$(dirname "$SCRIPT_DIR")")"
SUDOERS_FILE="$SYSTEM_DIR/sudoers/90-cpi-si-safe-operations"
SUDOERS_DEST="/etc/sudoers.d/90-cpi-si-safe-operations"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Action 1/6: Source logging library (+5 or -5)
if [ -f "$SYSTEM_DIR/lib/logging/logger.sh" ]; then
    # shellcheck disable=SC1091
    source "$SYSTEM_DIR/lib/logging/logger.sh"
    LOG_BASE_DIR="$SYSTEM_DIR/logs"  # Override for sudo context (HOME becomes /root)
    declare_health_total 170  # Total Possible from HEALTH SCORING MAP
    log_check "sudoers-install" "source-logging" "true" 5 "library=logger.sh"
else
    echo -e "${YELLOW}Warning: Logging library not found, continuing without logging${NC}"
fi

# ============================================================================
# BODY
# ============================================================================

# Display header
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║     CPI-SI Interactive Terminal System - Sudoers Install      ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

# Check if running as root, if not, re-exec with pkexec
if [ "$EUID" -ne 0 ]; then
    echo "📋 Elevated privileges required for sudoers installation"
    echo "   Using pkexec (GUI password prompt)..."
    echo ""
    exec pkexec "$0" "$@"
    exit 1  # If pkexec fails or is cancelled
fi

# Check if sudoers file exists
if [ ! -f "$SUDOERS_FILE" ]; then
    echo -e "${RED}Error: Sudoers file not found: $SUDOERS_FILE${NC}"
    exit 1
fi

echo "📋 Validating sudoers configuration..."

# Action 2/6: Validate syntax using visudo (+50 or -50) - CRITICAL
if visudo -c -f "$SUDOERS_FILE" > /dev/null 2>&1; then
    echo -e "${GREEN}✓ Syntax validation passed${NC}"
    log_check "sudoers-install" "syntax-validation" "true" 50 "source_file=$SUDOERS_FILE"
else
    echo -e "${RED}✗ Syntax validation failed${NC}"
    echo ""
    echo "Running detailed validation:"
    visudo -c -f "$SUDOERS_FILE"
    log_check "sudoers-install" "syntax-validation" "false" -50 "source_file=$SUDOERS_FILE"
    log_failure "sudoers-install" "Syntax validation failed" "invalid sudoers syntax" 0
    exit 1
fi

# Action 3/6: Backup existing file if it exists (+12 or -12)
if [ -f "$SUDOERS_DEST" ]; then
    BACKUP_FILE="$SUDOERS_DEST.backup.$(date +%Y%m%d_%H%M%S)"
    echo "📦 Backing up existing configuration to: $BACKUP_FILE"
    if cp "$SUDOERS_DEST" "$BACKUP_FILE"; then
        log_check "sudoers-install" "backup-existing" "true" 12 "backup=$BACKUP_FILE"
    else
        log_check "sudoers-install" "backup-existing" "false" -12 "backup=$BACKUP_FILE error=cp failed"
    fi
else
    log_check "sudoers-install" "backup-existing" "true" 12 "no_existing_file=true"
fi

# Action 4/6: Install the sudoers file (+30 or -30)
echo "📥 Installing sudoers configuration..."
if cp "$SUDOERS_FILE" "$SUDOERS_DEST"; then
    log_check "sudoers-install" "install-file" "true" 30 "source=$SUDOERS_FILE dest=$SUDOERS_DEST"
else
    log_check "sudoers-install" "install-file" "false" -30 "source=$SUDOERS_FILE dest=$SUDOERS_DEST error=cp failed"
    log_failure "sudoers-install" "Installation failed" "could not copy file" 0
    exit 1
fi

# Action 5/6: Set correct permissions (+25 or -25)
if chmod 440 "$SUDOERS_DEST"; then
    log_check "sudoers-install" "set-permissions" "true" 25 "permissions=440 file=$SUDOERS_DEST"
else
    log_check "sudoers-install" "set-permissions" "false" -25 "permissions=440 file=$SUDOERS_DEST error=chmod failed"
fi

# Action 6/6: Verify the installed file (+48 or -48) - CRITICAL
echo "🔍 Verifying installed configuration..."
if visudo -c > /dev/null 2>&1; then
    echo -e "${GREEN}✓ Installation successful${NC}"
    log_check "sudoers-install" "verify-installation" "true" 48 "validation=passed"
    log_success "sudoers-install" "Sudoers configuration installed successfully" 0 "dest=$SUDOERS_DEST permissions=440"
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "  Interactive Terminal System Enabled"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    echo "Safe operations enabled (passwordless sudo):"
    echo "  • Package management (apt install, update, upgrade)"
    echo "  • Service management (systemctl start/stop/restart)"
    echo "  • File permissions (chmod, chown)"
    echo "  • Network configuration"
    echo ""
    echo "Safety boundaries active:"
    echo "  • Essential package removal requires password"
    echo "  • Filesystem operations require password"
    echo "  • Bootloader modifications require password"
    echo ""
    echo "Test the configuration:"
    echo "  sudo apt update    # Should work without password"
    echo ""
else
    echo -e "${RED}✗ Installation failed - sudoers syntax error${NC}"
    log_check "sudoers-install" "verify-installation" "false" -48 "validation=failed"
    echo "Restoring backup if available..."
    if [ -f "$BACKUP_FILE" ]; then
        cp "$BACKUP_FILE" "$SUDOERS_DEST"
        echo -e "${YELLOW}Backup restored${NC}"
        log_failure "sudoers-install" "Installation verification failed, backup restored" "visudo validation failed" 0
    else
        log_failure "sudoers-install" "Installation verification failed" "visudo validation failed, no backup available" 0
    fi
    exit 1
fi

# ============================================================================
# CLOSING
# ============================================================================

echo "Installation complete. Logout and login for changes to take full effect."
exit 0
