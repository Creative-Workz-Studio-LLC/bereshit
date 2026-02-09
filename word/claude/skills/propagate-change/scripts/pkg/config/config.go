// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: propagate-change-pkg-config
// Purpose: Configuration loader for propagate-change skill
// Biblical: Galatians 5:9 - "A little leaven leaveneth the whole lump"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
//
// Config: ../../../config/chain-config.toml
//
// ═══════════════════════════════════════════════════════════════════════════

package config

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	pkgtoml "cws.studio/pkg/util/fs/toml"
)

// Hierarchy defines the chain hierarchy
type Hierarchy struct {
	Levels       []string          `toml:"levels"`
	Descriptions map[string]string `toml:"descriptions"`
}

// Locations defines where to find templates
type Locations struct {
	OmniSeed    string   `toml:"omni_seed"`
	FormatSeed  string   `toml:"format_seed"`
	SearchPaths []string `toml:"search_paths"`
	FilePatterns []string `toml:"file_patterns"`
}

// Template defines a specialized template
type Template struct {
	Path            string   `toml:"path"`
	BlockCount      int      `toml:"block_count"`
	Formats         []string `toml:"formats"`
	FormatTemplates []string `toml:"format_templates"`
}

// Patterns defines regex patterns for chain detection
type Patterns struct {
	DerivesFrom []string `toml:"derives_from"`
	RootMarkers []string `toml:"root_markers"`
}

// Propagation defines what each level affects
type Propagation struct {
	SyntaxSpecAffects  []string `toml:"syntax_spec_affects"`
	UniversalAffects   []string `toml:"universal_affects"`
	SpecializedAffects []string `toml:"specialized_affects"`
	FormatAffects      []string `toml:"format_affects"`
	DocumentAffects    []string `toml:"document_affects"`
}

// Output defines report formatting
type Output struct {
	Sections    []string `toml:"sections"`
	MaxDirect   int      `toml:"max_direct"`
	MaxIndirect int      `toml:"max_indirect"`
}

// Config holds all chain configuration
type Config struct {
	Hierarchy   Hierarchy            `toml:"hierarchy"`
	Locations   Locations            `toml:"locations"`
	Templates   map[string]Template  `toml:"templates"`
	Patterns    Patterns             `toml:"patterns"`
	Propagation Propagation          `toml:"propagation"`
	Output      Output               `toml:"output"`
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// Load reads and parses the chain configuration
func Load() (*Config, error) {
	var cfg Config
	searchPaths := pkgtoml.SkillConfigPaths("propagate-change")
	if err := pkgtoml.FindAndLoad("chain-config.toml", searchPaths, &cfg); err != nil {
		return nil, err
	}
	return &cfg, nil
}

// GetTemplate returns info about a specific template type
func (c *Config) GetTemplate(name string) (Template, bool) {
	tmpl, ok := c.Templates[name]
	return tmpl, ok
}

// GetLevelIndex returns the index of a level in the hierarchy
func (c *Config) GetLevelIndex(level string) int {
	for i, l := range c.Hierarchy.Levels {
		if l == level {
			return i
		}
	}
	return -1
}

// IsRootMarker checks if a value indicates chain root
func (c *Config) IsRootMarker(value string) bool {
	for _, marker := range c.Patterns.RootMarkers {
		if value == marker {
			return true
		}
	}
	return false
}

// GetAffectedLevels returns what levels are affected by changes at a given level
func (c *Config) GetAffectedLevels(level string) []string {
	switch level {
	case "syntax-spec":
		return c.Propagation.SyntaxSpecAffects
	case "universal":
		return c.Propagation.UniversalAffects
	case "specialized":
		return c.Propagation.SpecializedAffects
	case "format":
		return c.Propagation.FormatAffects
	case "document":
		return c.Propagation.DocumentAffects
	default:
		return nil
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
