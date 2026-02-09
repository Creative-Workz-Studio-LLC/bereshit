// ============================================================================
// METADATA - Integration Config Package
// ============================================================================
// Purpose: Load and manage integration configuration
// Biblical: Proverbs 4:7 - "Wisdom is the principal thing"

package config

import (
	pkgtoml "cws.studio/pkg/util/fs/toml"
)

// ============================================================================
// SETUP - Types
// ============================================================================

// Config holds the integration configuration
type Config struct {
	Paths   PathsConfig            `toml:"paths"`
	Levels  map[string]LevelConfig `toml:"levels"`
	Quality QualityConfig          `toml:"quality"`
	Process ProcessConfig          `toml:"process"`
}

// PathsConfig defines file locations
type PathsConfig struct {
	KnowledgeBase  string `toml:"knowledge_base"`
	Journals       string `toml:"journals"`
	Patterns       string `toml:"patterns"`
	GlobalClaudeMD string `toml:"global_claude_md"`
	InstanceBio    string `toml:"instance_bio"`
	InstanceConfig string `toml:"instance_config"`
}

// LevelConfig defines an integration level
type LevelConfig struct {
	Name        string   `toml:"name"`
	Description string   `toml:"description"`
	Target      string   `toml:"target"`
	Requires    []string `toml:"requires"`
	Examples    []string `toml:"examples"`
}

// QualityConfig defines quality criteria
type QualityConfig struct {
	MinObservations int      `toml:"min_observations"`
	EvidenceTypes   []string `toml:"evidence_types"`
	WarningSigns    []string `toml:"warning_signs"`
}

// ProcessConfig defines the integration process
type ProcessConfig struct {
	Steps     []string `toml:"steps"`
	Questions []string `toml:"questions"`
}

// ============================================================================
// BODY - Core Operations
// ============================================================================

// Load reads the configuration from file
func Load() (*Config, error) {
	var cfg Config
	searchPaths := pkgtoml.SkillConfigPaths("integrate-learning")
	if err := pkgtoml.FindAndLoad("integration-config.toml", searchPaths, &cfg); err != nil {
		return nil, err
	}
	return &cfg, nil
}

// GetLevel returns a specific integration level
func (c *Config) GetLevel(name string) (*LevelConfig, bool) {
	level, ok := c.Levels[name]
	if !ok {
		return nil, false
	}
	return &level, true
}

// GetLevelNames returns all level names
func (c *Config) GetLevelNames() []string {
	names := make([]string, 0, len(c.Levels))
	for name := range c.Levels {
		names = append(names, name)
	}
	return names
}
