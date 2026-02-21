#!/usr/bin/env bash
# ============================================================================
# CWS Studio — Desktop Application Installer
# ============================================================================
#
# Installs the CWS Studio desktop application:
#   - Launcher script → ~/.local/bin/cws-studio
#   - Desktop entry   → ~/.local/share/applications/
#   - Icon            → ~/.local/share/icons/
#   - State directory  → ~/.local/state/cws-studio/
#
# Usage:
#   ./install-desktop.sh           # Install
#   ./install-desktop.sh --remove  # Uninstall
#
# ============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$(readlink -f "$0")")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

BIN_DIR="$HOME/.local/bin"
APPS_DIR="$HOME/.local/share/applications"
ICONS_DIR="$HOME/.local/share/icons"
STATE_DIR="${XDG_STATE_HOME:-$HOME/.local/state}/cws-studio"

LAUNCHER="cws-studio"
DESKTOP="com.creativeworkzstudio.cws-studio.desktop"
ICON="cws-studio.svg"

log() {
  echo "  $*"
}

install() {
  echo "Installing CWS Studio..."
  echo ""

  # Create directories
  mkdir -p "$BIN_DIR" "$APPS_DIR" "$ICONS_DIR" "$STATE_DIR"

  # Install launcher wrapper (exfat can't set execute permissions on project drive)
  cat > "$BIN_DIR/$LAUNCHER" <<WRAPPER
#!/usr/bin/env bash
# CWS Studio launcher — wrapper calling project script via bash
# (exfat can't set execute permissions — wrapper on native FS calls bash explicitly)
exec bash "$SCRIPT_DIR/$LAUNCHER" "\$@"
WRAPPER
  chmod 755 "$BIN_DIR/$LAUNCHER"
  log "Launcher  → $BIN_DIR/$LAUNCHER (wraps $SCRIPT_DIR/$LAUNCHER)"

  # Install icon
  cp "$PROJECT_ROOT/public/icon.svg" "$ICONS_DIR/$ICON"
  log "Icon      → $ICONS_DIR/$ICON"

  # Install desktop entry
  cp "$SCRIPT_DIR/$DESKTOP" "$APPS_DIR/$DESKTOP"
  log "Desktop   → $APPS_DIR/$DESKTOP"

  # Update desktop database
  if command -v update-desktop-database &>/dev/null; then
    update-desktop-database "$APPS_DIR" 2>/dev/null || true
    log "Desktop database updated"
  fi

  echo ""
  echo "CWS Studio installed."
  echo ""
  echo "  Launch:  cws-studio"
  echo "  Menu:    Search 'CWS Studio' in application launcher"
  echo "  Stop:    cws-studio --stop"
  echo ""
}

remove() {
  echo "Removing CWS Studio..."
  echo ""

  # Stop running instance first
  if [ -f "${XDG_RUNTIME_DIR:-/tmp}/cws-studio.pid" ]; then
    "$BIN_DIR/$LAUNCHER" --stop 2>/dev/null || true
  fi

  # Remove files (launcher may be symlink or copy)
  [ -e "$BIN_DIR/$LAUNCHER" ]   && rm -f "$BIN_DIR/$LAUNCHER"   && log "Removed $BIN_DIR/$LAUNCHER"
  [ -f "$APPS_DIR/$DESKTOP" ]   && rm -f "$APPS_DIR/$DESKTOP"   && log "Removed $APPS_DIR/$DESKTOP"
  [ -f "$ICONS_DIR/$ICON" ]     && rm -f "$ICONS_DIR/$ICON"     && log "Removed $ICONS_DIR/$ICON"

  # Update desktop database
  if command -v update-desktop-database &>/dev/null; then
    update-desktop-database "$APPS_DIR" 2>/dev/null || true
  fi

  echo ""
  echo "CWS Studio removed."
  echo ""
}

# Main
case "${1:-}" in
  --remove|--uninstall)
    remove
    ;;
  --help|-h)
    echo "Usage: $0 [--remove]"
    echo "  (no args)  Install CWS Studio desktop application"
    echo "  --remove   Uninstall CWS Studio"
    ;;
  *)
    install
    ;;
esac
