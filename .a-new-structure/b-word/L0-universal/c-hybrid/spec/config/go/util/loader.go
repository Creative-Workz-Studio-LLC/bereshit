//omni:code --go -library
//omni:key B-L0-config-util-loader
//omni:version b-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-config-util-loader
// Purpose: Format-aware config loading — TOML, JSONC, with extension dispatch
// Biblical: Proverbs 3:5-6 - "In all thy ways acknowledge him, and he shall direct thy paths"
// Authors: Nova Dawn
// Version: b-01.00
// Created: 2025-12-10
//
// This is a CONFIG loader, not a TOML loader. Dispatches by file extension:
//   .toml         → TOML parser (BurntSushi/toml)
//   .json, .jsonc → JSONC parser (jsonc.go, strips comments)
//
// Used by: Parent config/loader.go, skills, system configs
//
// Ladder: LoadConfigFile → format detection (formats.go) → parser (toml or jsonc.go)
// Baton:  FindAndLoad → FindConfig → LoadFile/LoadConfigFile → parsed result
//
// ============================================================================
// END METADATA
// ============================================================================

package util

// ============================================================================
// SETUP
// ============================================================================

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"

	"github.com/BurntSushi/toml"
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Format-Aware Loading
// ────────────────────────────────────────────────────────────────

// LoadConfigFile loads a config file into a map, dispatching by extension.
// Supports .toml, .json, and .jsonc. Returns structured LoadError on failure.
//
// This is the primary entry point for format-agnostic config loading.
// Use LoadTOMLFile or LoadJSONCFile when you know the format at compile time.
func LoadConfigFile(path string) (map[string]any, error) {
	if !FileExists(path) {
		return nil, NewLoadError(path, "stat", fmt.Errorf("file not found"))
	}

	ext := filepath.Ext(path)
	switch ext {
	case ".toml":
		var result map[string]any
		if _, err := toml.DecodeFile(path, &result); err != nil {
			return nil, NewLoadError(path, "parse", fmt.Errorf("toml: %w", err))
		}
		return result, nil

	case ".json", ".jsonc":
		data, err := os.ReadFile(path)
		if err != nil {
			return nil, NewLoadError(path, "read", err)
		}
		cleaned, err := ParseJSONC(data)
		if err != nil {
			return nil, NewLoadError(path, "parse", err)
		}
		var result map[string]any
		if err := json.Unmarshal(cleaned, &result); err != nil {
			return nil, NewLoadError(path, "unmarshal", err)
		}
		return result, nil

	default:
		return nil, NewLoadError(path, "detect",
			fmt.Errorf("unsupported config format %q — expected .toml, .json, or .jsonc", ext))
	}
}

// ────────────────────────────────────────────────────────────────
// TOML Loading
// ────────────────────────────────────────────────────────────────

// LoadFile decodes a TOML file into the provided struct.
// Returns structured LoadError on failure.
func LoadFile(path string, v any) error {
	_, err := toml.DecodeFile(path, v)
	if err != nil {
		return NewLoadError(path, "parse", fmt.Errorf("toml: %w", err))
	}
	return nil
}

// LoadBytes decodes TOML bytes into the provided struct.
// Returns structured LoadError on failure.
func LoadBytes(data []byte, v any) error {
	if err := toml.Unmarshal(data, v); err != nil {
		return NewLoadError("(bytes)", "parse", fmt.Errorf("toml: %w", err))
	}
	return nil
}

// LoadTOMLFile reads a TOML file and unmarshals into a typed result.
// Generic companion to LoadJSONCFile[T] in jsonc.go — same pattern, TOML format.
func LoadTOMLFile[T any](path string) (*T, error) {
	if !FileExists(path) {
		return nil, NewLoadError(path, "stat", fmt.Errorf("file not found"))
	}

	var result T
	if _, err := toml.DecodeFile(path, &result); err != nil {
		return nil, NewLoadError(path, "parse", fmt.Errorf("toml: %w", err))
	}

	return &result, nil
}

// ────────────────────────────────────────────────────────────────
// Config Discovery
// ────────────────────────────────────────────────────────────────

// FindAndLoad searches for a config file and loads it into the provided struct.
// configName is the file name (e.g., "template-config.toml")
// searchPaths are directories to search in order.
//
// Currently TOML-only for struct decoding. For format-agnostic map loading,
// use FindConfig + LoadConfigFile.
func FindAndLoad(configName string, searchPaths []string, v any) error {
	path, err := FindConfig(configName, searchPaths)
	if err != nil {
		return err
	}
	return LoadFile(path, v)
}

// FindConfig searches for a config file in multiple locations.
// Returns the first path where the file exists.
func FindConfig(configName string, searchPaths []string) (string, error) {
	for _, dir := range searchPaths {
		path := filepath.Join(dir, configName)
		if FileExists(path) {
			return path, nil
		}
	}
	return "", NewLoadError(configName, "find",
		fmt.Errorf("not found in %d search paths", len(searchPaths)))
}

// FindConfigDir searches for a config directory containing the given file.
// Returns the directory path, not the file path.
func FindConfigDir(configName string, searchPaths []string) (string, error) {
	for _, dir := range searchPaths {
		path := filepath.Join(dir, configName)
		if FileExists(path) {
			return dir, nil
		}
	}
	return "", NewLoadError(configName, "find",
		fmt.Errorf("directory not found in %d search paths", len(searchPaths)))
}

// ────────────────────────────────────────────────────────────────
// Search Path Builders
// ────────────────────────────────────────────────────────────────

// SkillConfigPaths returns standard search paths for skill configs.
// skillName is the skill folder name (e.g., "create-from-template").
func SkillConfigPaths(skillName string) []string {
	var paths []string

	// Check CLAUDE_PLUGIN_ROOT first (highest priority)
	if root := os.Getenv("CLAUDE_PLUGIN_ROOT"); root != "" {
		paths = append(paths, filepath.Join(root, "skills", skillName, "config"))
	}

	// Relative paths
	paths = append(paths,
		"config",
		"../config",
		"../../config",
		filepath.Join("claude-global", "skills", skillName, "config"),
	)

	// Add paths relative to executable
	if exe, err := os.Executable(); err == nil {
		exeDir := filepath.Dir(exe)
		paths = append(paths,
			filepath.Join(exeDir, "..", "..", "config"),
			filepath.Join(exeDir, "..", "config"),
		)
	}

	// Add home-based paths
	if home, err := os.UserHomeDir(); err == nil {
		paths = append(paths,
			filepath.Join(home, ".claude", "cpi-si", "skills", skillName, "config"),
			filepath.Join(home, ".claude", "skills", skillName, "config"),
		)
	}

	return paths
}

// SystemConfigPaths returns standard search paths for system configs.
func SystemConfigPaths() []string {
	paths := []string{
		"config",
		"../config",
	}

	if home, err := os.UserHomeDir(); err == nil {
		paths = append(paths,
			filepath.Join(home, ".claude", "cpi-si", "system", "config"),
			filepath.Join(home, ".claude", "system", "config"),
		)
	}

	return paths
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// "In all thy ways acknowledge him, and he shall direct thy paths."
// — Proverbs 3:5-6
//
// The loader acknowledges each file's format before trying to parse it.
// TOML, JSONC — each gets its proper parser. The path is directed by
// the extension, not by assumption.
//
// ============================================================================
// END CLOSING
// ============================================================================
