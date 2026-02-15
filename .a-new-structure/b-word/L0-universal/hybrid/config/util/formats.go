//omni:code --go -library
//omni:key B-L0-config-util-formats
//omni:version b-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-config-util-formats
// Purpose: Standard format detection from file extensions
// Biblical: Genesis 1:4 - "And God divided the light from the darkness"
//           Format detection separates — each file gets its proper identity.
// Authors: Nova Dawn
// Version: b-01.00
// Created: 2026-02-13
//
// L0 knows standard formats (.go, .toml, .json, etc.).
// L1 adds OmniCode-specific extensions on top (.omni, .gen, .witness, etc.).
// This separation keeps L0 universal — no OmniCode dependency.
//
// ============================================================================
// END METADATA
// ============================================================================

package util

// ============================================================================
// SETUP
// ============================================================================

import (
	"path/filepath"
	"sort"
	"sync"
)

// ────────────────────────────────────────────────────────────────
// Format Registries
// ────────────────────────────────────────────────────────────────

// mu protects StandardFormats and ConfigFormats from concurrent registration.
// Reads don't need locking — maps are populated at init and extended via
// RegisterFormat/RegisterConfigFormat before concurrent access begins.
// The lock prevents races if multiple init() functions register simultaneously.
var mu sync.Mutex

// StandardFormats maps standard file extensions to format identifiers.
// These are universal — any project would recognize them.
// OmniCode-specific extensions (.omni, .gen, etc.) belong in L1
// and are registered via RegisterFormat.
var StandardFormats = map[string]string{
	// Compiled languages
	".go": "go",
	".c":  "c",
	".h":  "c",
	".rs": "rs",

	// Config/data formats
	".toml":  "toml",
	".json":  "json",
	".jsonc": "jsonc",
	".yaml":  "yaml",
	".yml":   "yaml",

	// Documentation
	".md":   "md",
	".adoc": "adoc",
	".txt":  "txt",

	// Scripting
	".py":   "py",
	".ts":   "ts",
	".js":   "js",
	".sh":   "sh",
	".bash": "sh",

	// Web
	".html": "html",
	".css":  "css",
}

// ConfigFormats lists formats that hold configuration data.
// Used by IsConfigFormat and IsConfigPath to identify loadable configs.
var ConfigFormats = map[string]bool{
	"toml":  true,
	"json":  true,
	"jsonc": true,
	"yaml":  true,
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Registration — L1 extends L0 without modifying it
// ────────────────────────────────────────────────────────────────

// RegisterFormat adds a file extension → format mapping.
// L1 calls this to register OmniCode-specific extensions (.omni, .gen, etc.)
// without modifying L0's source. Call during init().
//
// ext must include the dot: ".omni", not "omni".
func RegisterFormat(ext, format string) {
	mu.Lock()
	StandardFormats[ext] = format
	mu.Unlock()
}

// RegisterConfigFormat marks a format as holding configuration data.
// After registration, IsConfigFormat and IsConfigPath recognize the format.
func RegisterConfigFormat(format string) {
	mu.Lock()
	ConfigFormats[format] = true
	mu.Unlock()
}

// ────────────────────────────────────────────────────────────────
// Format Detection
// ────────────────────────────────────────────────────────────────

// FormatFromExt returns the format identifier for a file extension.
// Returns the format string if known, empty string if unrecognized.
//
// Checks both L0 standard formats and any L1-registered extensions.
func FormatFromExt(ext string) string {
	if f, ok := StandardFormats[ext]; ok {
		return f
	}
	return ""
}

// FormatFromPath extracts the file extension and returns its format identifier.
func FormatFromPath(path string) string {
	return FormatFromExt(filepath.Ext(path))
}

// IsConfigFormat returns true if the format holds configuration data.
func IsConfigFormat(format string) bool {
	return ConfigFormats[format]
}

// IsConfigPath returns true if the file path has a config format extension.
func IsConfigPath(path string) bool {
	return IsConfigFormat(FormatFromPath(path))
}

// IsKnownExt returns true if the file extension maps to a known format.
// Includes both L0 standard formats and L1-registered extensions.
func IsKnownExt(ext string) bool {
	_, ok := StandardFormats[ext]
	return ok
}

// ────────────────────────────────────────────────────────────────
// Introspection
// ────────────────────────────────────────────────────────────────

// AllKnownExts returns a sorted list of all registered file extensions.
// Includes both L0 standard and L1-registered extensions.
// Useful for help text, debugging, and validation error messages.
func AllKnownExts() []string {
	exts := make([]string, 0, len(StandardFormats))
	for ext := range StandardFormats {
		exts = append(exts, ext)
	}
	sort.Strings(exts)
	return exts
}

// AllConfigFormats returns a sorted list of all registered config format names.
func AllConfigFormats() []string {
	formats := make([]string, 0, len(ConfigFormats))
	for f := range ConfigFormats {
		formats = append(formats, f)
	}
	sort.Strings(formats)
	return formats
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// L0 format detection is deliberately limited to standard formats.
// The layering:
//
//   L0 (this file): .go → "go", .toml → "toml", .json → "json"
//   L1 (pragma):    .omni → "omni", .gen → "binary", .witness → "jsonc"
//
// L1 can call L0's FormatFromExt for standard files, and add its own
// extension registry on top. L0 never needs to know about OmniCode.
//
// "And God divided the light from the darkness." — Genesis 1:4
//
// ============================================================================
// END CLOSING
// ============================================================================
