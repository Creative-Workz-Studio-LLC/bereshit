// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-toml
// Purpose: Shared TOML configuration loading utilities
// Biblical: Proverbs 3:5-6 - "In all thy ways acknowledge him, and he shall direct thy paths"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-10
//
// Used by: All skills with TOML configuration
//
// ═══════════════════════════════════════════════════════════════════════════

package toml

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/BurntSushi/toml"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// LoadFile decodes a TOML file into the provided struct
func LoadFile(path string, v interface{}) error {
	_, err := toml.DecodeFile(path, v)
	if err != nil {
		return fmt.Errorf("failed to parse TOML: %w", err)
	}
	return nil
}

// LoadBytes decodes TOML bytes into the provided struct
func LoadBytes(data []byte, v interface{}) error {
	if err := toml.Unmarshal(data, v); err != nil {
		return fmt.Errorf("failed to parse TOML: %w", err)
	}
	return nil
}

// FindAndLoad searches for a config file and loads it
// configName is the file name (e.g., "template-config.toml")
// searchPaths are directories to search in order
func FindAndLoad(configName string, searchPaths []string, v interface{}) error {
	path, err := FindConfig(configName, searchPaths)
	if err != nil {
		return err
	}
	return LoadFile(path, v)
}

// FindConfig searches for a config file in multiple locations
func FindConfig(configName string, searchPaths []string) (string, error) {
	for _, dir := range searchPaths {
		path := filepath.Join(dir, configName)
		if _, err := os.Stat(path); err == nil {
			return path, nil
		}
	}
	return "", fmt.Errorf("config %s not found in search paths", configName)
}

// FindConfigDir searches for a config directory containing the given file
// Returns the directory path, not the file path
func FindConfigDir(configName string, searchPaths []string) (string, error) {
	for _, dir := range searchPaths {
		path := filepath.Join(dir, configName)
		if _, err := os.Stat(path); err == nil {
			return dir, nil
		}
	}
	return "", fmt.Errorf("config directory containing %s not found", configName)
}

// SkillConfigPaths returns standard search paths for skill configs
// skillName is the skill folder name (e.g., "create-from-template")
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

// SystemConfigPaths returns standard search paths for system configs
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

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
