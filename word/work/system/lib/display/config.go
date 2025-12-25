// ============================================================================
// METADATA
// ============================================================================
//
// Display Configuration Primitive - JSONC Configuration Loading
//
// Biblical Foundation: See format.go (rails pattern applies to all primitives)
// CPI-SI Identity: RAIL PRIMITIVE (orthogonal infrastructure component)
// Component Type: Configuration management for display rail
//
// Purpose: Loads display formatting configuration from JSONC files, providing
//          multi-layer tripwire pattern (graceful degradation on failure)
//
// Authorship: Nova Dawn (extracted 2025-11-21 from format.go v2.0.0)
// Version: 1.0.0
//
// HEALTH SCORING MAP (Total = 100):
//   Config Loading (50): Read file → strip comments → unmarshal JSON
//   Init Execution (50): Load config → graceful fallback on error
//
package display

// ============================================================================
// SETUP
// ============================================================================

import (
	"encoding/json" // JSON unmarshaling for configuration file parsing
	"fmt"           // Error wrapping for config loading failures
	"os"            // File operations for configuration loading
	"regexp"        // JSONC comment stripping (remove // and /* */ comments)
)

// ────────────────────────────────────────────────────────────────
// Configuration Structures
// ────────────────────────────────────────────────────────────────

// DisplayConfig holds all configuration values loaded from formatting.jsonc
type DisplayConfig struct {
	Colors ColorConfig  `json:"colors"`
	Icons  IconConfig   `json:"icons"`
	Layout LayoutConfig `json:"layout"`
}

// ColorConfig holds ANSI color escape codes
type ColorConfig struct {
	Basic          BasicColors          `json:"basic"`
	Foreground     ForegroundColors     `json:"foreground"`
	BoldForeground BoldForegroundColors `json:"bold_foreground"`
}

// BasicColors holds text modifiers
type BasicColors struct {
	Reset string `json:"reset"`
	Bold  string `json:"bold"`
	Dim   string `json:"dim"`
}

// ForegroundColors holds standard 8-color foreground codes
type ForegroundColors struct {
	Red     string `json:"red"`
	Green   string `json:"green"`
	Yellow  string `json:"yellow"`
	Blue    string `json:"blue"`
	Magenta string `json:"magenta"`
	Cyan    string `json:"cyan"`
	Gray    string `json:"gray"`
}

// BoldForegroundColors holds bold foreground codes
type BoldForegroundColors struct {
	BoldRed     string `json:"bold_red"`
	BoldGreen   string `json:"bold_green"`
	BoldYellow  string `json:"bold_yellow"`
	BoldBlue    string `json:"bold_blue"`
	BoldMagenta string `json:"bold_magenta"`
	BoldCyan    string `json:"bold_cyan"`
}

// IconConfig holds Unicode icons and current mode
type IconConfig struct {
	Status StatusIcons `json:"status"`
}

// StatusIcons holds primary status indicator icons
type StatusIcons struct {
	Success string `json:"success"`
	Failure string `json:"failure"`
	Warning string `json:"warning"`
	Info    string `json:"info"`
	Check   string `json:"check"`
	Cross   string `json:"cross"`
}

// LayoutConfig holds spacing and padding values
type LayoutConfig struct {
	Header      HeaderLayout      `json:"header"`
	KeyValue    KeyValueLayout    `json:"key_value"`
	Table       TableLayout       `json:"table"`
	Box         BoxLayout         `json:"box"`
	Indentation IndentationLayout `json:"indentation"`
}

// HeaderLayout holds header padding configuration
type HeaderLayout struct {
	Padding int `json:"padding"`
}

// KeyValueLayout holds key-value column width configuration
type KeyValueLayout struct {
	ColumnWidth int `json:"column_width"`
}

// TableLayout holds table column padding configuration
type TableLayout struct {
	ColumnPadding int `json:"column_padding"`
}

// BoxLayout holds box border padding configuration
type BoxLayout struct {
	WidthPadding int `json:"width_padding"`
}

// IndentationLayout holds indentation strings
type IndentationLayout struct {
	StatusLine string `json:"status_line"`
	KeyValue   string `json:"key_value"`
}

// ────────────────────────────────────────────────────────────────
// Package-Level State (Configuration Exception)
// ────────────────────────────────────────────────────────────────

// config holds the loaded configuration from formatting.jsonc
// Rails normally have no package state, but configuration is read-only
// after init(), making it effectively constant.
var config DisplayConfig

// init loads configuration from system/data/config/display/formatting.jsonc
// Phase 7c: Graceful fallback - if config fails to load, set empty config
// and let tripwires in each function fall back to constants.
func init() {
	// NOTE: Using relative path from system/runtime/lib/display to system/data/config/display
	// Path calculation: ../../../ goes up to system/, then data/config/display/formatting.jsonc
	configPath := "../../../data/config/display/formatting.jsonc"

	var err error
	config, err = loadConfig(configPath)
	if err != nil {
		// Phase 7c: GRACEFUL FALLBACK - config loading failed, use empty config
		// Tripwires in each function will catch empty values and use constants
		// This allows the system to work even if config is missing/broken
		config = DisplayConfig{} // Empty config triggers all tripwires

		// Silent fallback - rails self-evidence pattern
		// Broken config = functions use constants = slightly different output
		// This is self-evident if someone checks the values carefully
		// Future: Could add optional logging here when logging rail is available
	}

	// Config loaded successfully - tripwires won't fire, config values used
}

// ============================================================================
// BODY
// ============================================================================

// loadConfig loads display configuration from the specified JSONC file.
//
// What It Does:
//   - Reads JSONC file from disk
//   - Strips JSON comments (// and /* */) to convert JSONC → JSON
//   - Unmarshals into DisplayConfig struct
//   - Returns populated config or error if loading fails
//
// Parameters:
//   - path: Relative or absolute path to formatting.jsonc file
//
// Returns:
//   - DisplayConfig: Populated configuration struct
//   - error: File read error, comment stripping error, or JSON unmarshal error
//
// Phase 7a: Used by init() to load config at package initialization
// NO FALLBACKS - intentionally returns error to verify loading works
func loadConfig(path string) (DisplayConfig, error) {
	// Read JSONC file
	data, err := os.ReadFile(path)
	if err != nil {
		return DisplayConfig{}, fmt.Errorf("failed to read config file: %w", err)
	}

	// Strip JSONC comments to convert to valid JSON
	// Remove single-line comments (// ...)
	singleLineComment := regexp.MustCompile(`//.*`)
	cleaned := singleLineComment.ReplaceAll(data, []byte(""))

	// Remove multi-line comments (/* ... */)
	multiLineComment := regexp.MustCompile(`/\*[\s\S]*?\*/`)
	cleaned = multiLineComment.ReplaceAll(cleaned, []byte(""))

	// Unmarshal JSON into DisplayConfig struct
	var cfg DisplayConfig
	if err := json.Unmarshal(cleaned, &cfg); err != nil {
		return DisplayConfig{}, fmt.Errorf("failed to unmarshal config JSON: %w", err)
	}

	return cfg, nil
}

// GetConfig returns the loaded configuration for use by other display primitives.
//
// What It Does:
//   - Returns package-level config loaded during init()
//   - Config may be empty (DisplayConfig{}) if loading failed
//   - Callers should use tripwire pattern to fall back to constants
//
// Returns:
//   - DisplayConfig: Loaded config (or empty if init() failed to load)
//
// Usage Pattern (in other display primitives):
//   cfg := GetConfig()
//   colorGreen := cfg.Colors.Foreground.Green
//   if colorGreen == "" {
//       colorGreen = Green  // Tripwire: fall back to constant
//   }
func GetConfig() DisplayConfig {
	return config
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Code Validation: Compile with format.go (go build ./display)
// Code Execution: init() runs at package initialization
// Code Cleanup: None needed (read-only config, GC handles memory)
//
// Modification Policy:
//   ✅ Safe: Adding new config struct fields (extend DisplayConfig)
//   ⚠️ Care: Changing config file path (breaks loading for existing systems)
//   ❌ Never: Adding dependencies to system libraries (must be stdlib-only)
//
// Multi-Layer Tripwire Architecture:
//   Layer 1 (init): Config file loading failures → set empty config
//   Layer 2 (functions): Empty config values → fall back to constants
//
// Quick Reference:
//   cfg := GetConfig()  // Get loaded config
//   // Use with tripwires in calling functions
