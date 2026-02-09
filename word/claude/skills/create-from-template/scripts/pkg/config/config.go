// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: create-from-template-pkg-config
// Purpose: Configuration loading for template skill
// Biblical: Exodus 25:40 - "Look that thou make them after their pattern"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-09
//
// ═══════════════════════════════════════════════════════════════════════════

package config

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"os"
	"path/filepath"

	pkgtoml "cws.studio/pkg/util/fs/toml"
)

// --- Types ---

// Config holds all template configuration
type Config struct {
	Paths           PathsConfig              `toml:"paths"`
	Categories      map[string]Category      `toml:"categories"`
	BlockStructures map[string][]string      `toml:"block_structures"`
	Discovery       DiscoveryConfig          `toml:"discovery"`
}

// PathsConfig holds template path configuration
type PathsConfig struct {
	BereshitSeed           string `toml:"bereshit_seed"`
	CodeTemplates          string `toml:"code_templates"`
	DataTemplates          string `toml:"data_templates"`
	DocumentationTemplates string `toml:"documentation_templates"`
	OmniSeed               string `toml:"omni_seed"`
}

// Category represents a template category
type Category struct {
	Name           string                      `toml:"name"`
	Description    string                      `toml:"description"`
	BlockStructure string                      `toml:"block_structure"`
	Path           string                      `toml:"path"`
	Languages      map[string]LanguageConfig   `toml:"languages"`
	Formats        map[string]FormatConfig     `toml:"formats"`
}

// LanguageConfig for code templates
type LanguageConfig struct {
	Extensions []string `toml:"extensions"`
	Types      []string `toml:"types"`
}

// FormatConfig for data/doc templates
type FormatConfig struct {
	Extensions []string `toml:"extensions"`
	Types      []string `toml:"types"`
}

// DiscoveryConfig for template discovery
type DiscoveryConfig struct {
	TemplateMarkers []string `toml:"template_markers"`
	SkipDirs        []string `toml:"skip_dirs"`
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Core Operations ---

// Load reads and parses the configuration file
func Load() (*Config, error) {
	var cfg Config
	searchPaths := pkgtoml.SkillConfigPaths("create-from-template")
	if err := pkgtoml.FindAndLoad("template-config.toml", searchPaths, &cfg); err != nil {
		return nil, err
	}
	return &cfg, nil
}

// GetBereshitRoot returns the bereshit root path
func (c *Config) GetBereshitRoot() string {
	// Check environment first
	if root := os.Getenv("BERESHIT_ROOT"); root != "" {
		return root
	}

	// Default to workspace-relative
	return c.Paths.BereshitSeed
}

// GetTemplatePath returns full path for a category
func (c *Config) GetTemplatePath(category string) string {
	root := c.GetBereshitRoot()
	if cat, ok := c.Categories[category]; ok {
		return filepath.Join(root, cat.Path)
	}
	return ""
}

// GetBlockStructure returns blocks for a structure type
func (c *Config) GetBlockStructure(structType string) []string {
	if blocks, ok := c.BlockStructures[structType]; ok {
		return blocks
	}
	return nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
