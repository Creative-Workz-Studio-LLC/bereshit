//omni:code --go -library
//omni:key B-L2-preferences
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-preferences
// Purpose: XDG-compliant user preferences read/write for dashboard
// Biblical: Psalm 119:30 — "I have chosen the way of truth"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package preferences

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"encoding/json"
	"errors"
	"fmt"
	"os"
	"path/filepath"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// XDG Directory — platform-compliant user data location
// ───────────────────────────────────────────────────────────────────────────

const prefsFilename = "preferences.json"

// Dir returns the XDG-compliant preferences directory for the dashboard.
// Follows the XDG Base Directory Specification:
//
//   - $XDG_CONFIG_HOME/cws-dashboard  (if XDG_CONFIG_HOME is set)
//   - ~/.config/cws-dashboard          (default fallback)
//
// This ensures dashboard preferences survive across server reinstalls
// and are discoverable by standard Linux tools.
func Dir() string {
	xdg := os.Getenv("XDG_CONFIG_HOME")
	if xdg == "" {
		home, _ := os.UserHomeDir()
		xdg = filepath.Join(home, ".config")
	}
	return filepath.Join(xdg, "cws-dashboard")
}

// ───────────────────────────────────────────────────────────────────────────
// Read/Write — JSON-based preferences persistence
// ───────────────────────────────────────────────────────────────────────────

// Read reads dashboard preferences from the XDG config directory.
// Returns an empty map if the file doesn't exist — this is intentional, not
// an error. A missing preferences file simply means the user hasn't customized
// anything yet, and the dashboard should use its built-in defaults.
func Read() (map[string]any, error) {
	path := filepath.Join(Dir(), prefsFilename)
	data, err := os.ReadFile(path)
	if err != nil {
		if errors.Is(err, os.ErrNotExist) {
			return map[string]any{}, nil
		}
		return nil, fmt.Errorf("read preferences: %w", err)
	}

	var prefs map[string]any
	if err := json.Unmarshal(data, &prefs); err != nil {
		return nil, fmt.Errorf("parse preferences: %w", err)
	}
	return prefs, nil
}

// Write writes dashboard preferences to the XDG config directory.
// Creates the directory if it doesn't exist.
func Write(prefs map[string]any) error {
	dir := Dir()
	if err := os.MkdirAll(dir, 0755); err != nil {
		return fmt.Errorf("create preferences dir: %w", err)
	}

	data, err := json.MarshalIndent(prefs, "", "  ")
	if err != nil {
		return fmt.Errorf("marshal preferences: %w", err)
	}

	path := filepath.Join(dir, prefsFilename)
	if err := os.WriteFile(path, data, 0644); err != nil {
		return fmt.Errorf("write preferences: %w", err)
	}
	return nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
