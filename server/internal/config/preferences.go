// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-config-preferences
// Purpose: XDG-compliant user preferences read/write
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package config

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

const prefsFilename = "preferences.json"

// ReadPreferences reads dashboard preferences from the XDG config directory.
// Returns empty map if file doesn't exist (not an error).
func ReadPreferences() (map[string]any, error) {
	path := filepath.Join(PreferencesDir(), prefsFilename)
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

// WritePreferences writes dashboard preferences to the XDG config directory.
// Creates the directory if it doesn't exist.
func WritePreferences(prefs map[string]any) error {
	dir := PreferencesDir()
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
