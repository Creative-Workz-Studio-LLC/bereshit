// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: format-lookup-pkg-config
// Purpose: Configuration loader for format-lookup skill
// Biblical: Ecclesiastes 3:1 - "To every thing there is a season"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
//
// Config: ../../../config/format-mappings.toml
//
// ═══════════════════════════════════════════════════════════════════════════

package config

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	pkgtoml "cws.studio/pkg/util/fs/toml"
)

// FormatInfo describes a supported format
type FormatInfo struct {
	Name        string `toml:"name"`
	Flag        string `toml:"flag"`
	Type        string `toml:"type"`
	Description string `toml:"description"`
	Status      string `toml:"status"`
}

// Mapping represents a single OmniCode to format mapping
type Mapping struct {
	OmniCode   string `toml:"omnicode"`
	Target     string `toml:"target"`
	Example    string `toml:"example"`
	Block      string `toml:"block,omitempty"`      // e.g., "METADATA", "SETUP", "BODY", "CLOSING"
	Section    string `toml:"section,omitempty"`    // e.g., "Imports", "Constants", "Helpers"
	Subsection string `toml:"subsection,omitempty"` // e.g., "stdlib", "internal", "external"
	Priority   int    `toml:"priority,omitempty"`   // Import priority: 1=stdlib, 0=internal, -1=external
}

// BlockSection describes the sections within a block
type BlockSection struct {
	Order       []string `toml:"order"`
	Description string   `toml:"description"`
}

// BlockStructure describes a block type (code, documentation, data)
type BlockStructure struct {
	Blocks         []string                `toml:"blocks"`
	TemplateSource string                  `toml:"template_source"`
	Sections       map[string]BlockSection `toml:"sections"`
}

// Config holds all format mappings and block structures
type Config struct {
	Formats         map[string]FormatInfo     `toml:"formats"`
	Mappings        map[string][]Mapping      `toml:"mappings"`
	BlockStructures map[string]BlockStructure `toml:"block_structures"`
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// Load reads and parses the format mappings configuration
func Load() (*Config, error) {
	var cfg Config
	searchPaths := pkgtoml.SkillConfigPaths("format-lookup")
	if err := pkgtoml.FindAndLoad("format-mappings.toml", searchPaths, &cfg); err != nil {
		return nil, err
	}
	return &cfg, nil
}

// GetFormat returns info about a specific format
func (c *Config) GetFormat(name string) (FormatInfo, bool) {
	info, ok := c.Formats[name]
	return info, ok
}

// GetMappings returns all mappings for a format
func (c *Config) GetMappings(format string) []Mapping {
	return c.Mappings[format]
}

// ListFormats returns all format names
func (c *Config) ListFormats() []string {
	var names []string
	for name := range c.Formats {
		names = append(names, name)
	}
	return names
}

// ListFormatsByType returns formats of a specific type (code, documentation, data)
func (c *Config) ListFormatsByType(formatType string) []FormatInfo {
	var result []FormatInfo
	for _, info := range c.Formats {
		if info.Type == formatType {
			result = append(result, info)
		}
	}
	return result
}

// IsValidFormat checks if a format name is valid
func (c *Config) IsValidFormat(name string) bool {
	_, ok := c.Formats[name]
	return ok
}

// GetBlockStructure returns the block structure for a type (code, documentation, data)
func (c *Config) GetBlockStructure(structType string) (BlockStructure, bool) {
	bs, ok := c.BlockStructures[structType]
	return bs, ok
}

// GetSectionOrder returns the section order for a block within a structure type
func (c *Config) GetSectionOrder(structType, block string) []string {
	bs, ok := c.BlockStructures[structType]
	if !ok {
		return nil
	}
	section, ok := bs.Sections[block]
	if !ok {
		return nil
	}
	return section.Order
}

// GetMappingsByBlock returns mappings for a format filtered by block
func (c *Config) GetMappingsByBlock(format, block string) []Mapping {
	var result []Mapping
	for _, m := range c.Mappings[format] {
		if m.Block == block {
			result = append(result, m)
		}
	}
	return result
}

// GetMappingsBySection returns mappings for a format filtered by block and section
func (c *Config) GetMappingsBySection(format, block, section string) []Mapping {
	var result []Mapping
	for _, m := range c.Mappings[format] {
		if m.Block == block && m.Section == section {
			result = append(result, m)
		}
	}
	return result
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
