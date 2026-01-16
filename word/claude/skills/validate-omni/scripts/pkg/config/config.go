// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: validate-omni-pkg-config
// Purpose: Load validation configuration from TOML files
// Biblical: Proverbs 24:3 - "Through wisdom is an house builded"
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
	"path/filepath"

	pkgtoml "github.com/creativeworkzstudio/claude-global/pkg/util/fs/toml"
)

// BlockStructures holds block definitions loaded from config
type BlockStructures struct {
	Code          BlockSpec            `toml:"code"`
	Documentation BlockSpec            `toml:"documentation"`
	Interface     BlockSpec            `toml:"interface"`
	Folder        BlockSpec            `toml:"folder"`
	Data          BlockSpec            `toml:"data"`
	Template      BlockSpecWithAlts    `toml:"template"`
}

// BlockSpec defines a block structure
type BlockSpec struct {
	Count       int      `toml:"count"`
	Blocks      []string `toml:"blocks"`
	Description string   `toml:"description"`
}

// BlockSpecWithAlts includes alternate structures (for templates)
type BlockSpecWithAlts struct {
	BlockSpec
	Alternates map[string][]string `toml:"alternates"`
}

// PragmaTypes holds pragma definitions loaded from config
type PragmaTypes struct {
	Types   map[string]PragmaType `toml:"types"`
	Formats map[string]string     `toml:"formats"`
}

// PragmaType defines a single pragma type
type PragmaType struct {
	Description string   `toml:"description"`
	BlockCount  int      `toml:"block_count"`
	Formats     []string `toml:"formats"`
}

// RequiredSections holds section requirements loaded from config
type RequiredSections struct {
	Sections map[string]Section `toml:"sections"`
	Summary  SectionSummary     `toml:"summary"`
}

// Section defines a required section
type Section struct {
	Name        string   `toml:"name"`
	Markers     []string `toml:"markers"`
	Description string   `toml:"description"`
	Example     string   `toml:"example"`
	Required    bool     `toml:"required"`
}

// SectionSummary provides quick reference info
type SectionSummary struct {
	TotalRequired int      `toml:"total_required"`
	AllMarkers    []string `toml:"all_markers"`
}

// Config holds all loaded configuration
type Config struct {
	BlockStructures  BlockStructures
	PragmaTypes      PragmaTypes
	RequiredSections RequiredSections
	ConfigDir        string
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// Load reads all configuration files from the config directory
func Load() (*Config, error) {
	searchPaths := pkgtoml.SkillConfigPaths("validate-omni")

	// Find config directory by looking for block-structures.toml
	configDir, err := pkgtoml.FindConfigDir("block-structures.toml", searchPaths)
	if err != nil {
		return nil, err
	}

	cfg := &Config{ConfigDir: configDir}

	// Load block structures
	if err := pkgtoml.LoadFile(filepath.Join(configDir, "block-structures.toml"), &cfg.BlockStructures); err != nil {
		return nil, err
	}

	// Load pragma types
	if err := pkgtoml.LoadFile(filepath.Join(configDir, "pragma-types.toml"), &cfg.PragmaTypes); err != nil {
		return nil, err
	}

	// Load required sections
	if err := pkgtoml.LoadFile(filepath.Join(configDir, "required-sections.toml"), &cfg.RequiredSections); err != nil {
		return nil, err
	}

	return cfg, nil
}

// GetBlockSpec returns the block spec for a pragma type
func (c *Config) GetBlockSpec(pragmaType string) (BlockSpec, bool) {
	switch pragmaType {
	case "code":
		return c.BlockStructures.Code, true
	case "documentation":
		return c.BlockStructures.Documentation, true
	case "interface":
		return c.BlockStructures.Interface, true
	case "folder":
		return c.BlockStructures.Folder, true
	case "data":
		return c.BlockStructures.Data, true
	case "template":
		return c.BlockStructures.Template.BlockSpec, true
	default:
		return BlockSpec{}, false
	}
}

// GetTemplateAlternates returns alternate block structures for templates
func (c *Config) GetTemplateAlternates() map[string][]string {
	return c.BlockStructures.Template.Alternates
}

// IsValidPragmaType checks if a pragma type is valid
func (c *Config) IsValidPragmaType(pragmaType string) bool {
	_, ok := c.PragmaTypes.Types[pragmaType]
	return ok
}

// GetValidFormats returns valid format flags for a pragma type
func (c *Config) GetValidFormats(pragmaType string) []string {
	if pt, ok := c.PragmaTypes.Types[pragmaType]; ok {
		return pt.Formats
	}
	return nil
}

// GetRequiredMarkers returns all markers that satisfy required sections
func (c *Config) GetRequiredMarkers() []string {
	return c.RequiredSections.Summary.AllMarkers
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
