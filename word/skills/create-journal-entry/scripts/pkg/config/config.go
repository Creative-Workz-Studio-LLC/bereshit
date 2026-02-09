// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: create-journal-entry-pkg-config
// Purpose: Configuration loading for journal entry skill
// Biblical: Psalm 119:11 - "Thy word have I hid in mine heart"
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

	pkgtoml "cws.studio/pkg/util/fs/toml"
)

// --- Types ---

// Config holds all journal configuration
type Config struct {
	Paths     PathsConfig          `toml:"paths"`
	Types     map[string]JournalType `toml:"types"`
	Templates TemplatesConfig      `toml:"templates"`
}

// PathsConfig holds journal path configuration
type PathsConfig struct {
	KnowledgeBase string `toml:"knowledge_base"`
	JournalsRoot  string `toml:"journals_root"`
	BibleStudy    string `toml:"bible_study"`
	Personal      string `toml:"personal"`
	Instance      string `toml:"instance"`
	Universal     string `toml:"universal"`
}

// JournalType represents a journal type configuration
type JournalType struct {
	Name            string         `toml:"name"`
	Description     string         `toml:"description"`
	Visibility      string         `toml:"visibility"`
	Transfers       interface{}    `toml:"transfers"` // bool or string
	FilenamePattern string         `toml:"filename_pattern"`
	Sections        SectionsConfig `toml:"sections"`
}

// SectionsConfig for journal entry sections
type SectionsConfig struct {
	Required []string `toml:"required"`
	Optional []string `toml:"optional"`
}

// TemplatesConfig for entry templates
type TemplatesConfig struct {
	DateFormat string `toml:"date_format"`
	Header     string `toml:"header"`
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Core Operations ---

// Load reads and parses the configuration file
func Load() (*Config, error) {
	var cfg Config
	searchPaths := pkgtoml.SkillConfigPaths("create-journal-entry")
	if err := pkgtoml.FindAndLoad("journal-config.toml", searchPaths, &cfg); err != nil {
		return nil, err
	}
	return &cfg, nil
}

// GetJournalPath returns the full path for a journal type
func (c *Config) GetJournalPath(journalType string) string {
	base := filepath.Join(c.Paths.KnowledgeBase, c.Paths.JournalsRoot)

	switch journalType {
	case "bible-study":
		return filepath.Join(base, c.Paths.BibleStudy)
	case "personal":
		return filepath.Join(base, c.Paths.Personal)
	case "instance":
		return filepath.Join(base, c.Paths.Instance)
	case "universal":
		return filepath.Join(base, c.Paths.Universal)
	default:
		return ""
	}
}

// GetJournalType returns the configuration for a journal type
func (c *Config) GetJournalType(typeName string) (*JournalType, bool) {
	jt, ok := c.Types[typeName]
	return &jt, ok
}

// GetJournalTypes returns all available journal type names
func (c *Config) GetJournalTypes() []string {
	var types []string
	for name := range c.Types {
		types = append(types, name)
	}
	return types
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
