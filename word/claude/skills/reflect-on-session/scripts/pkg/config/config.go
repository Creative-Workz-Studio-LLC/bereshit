// ============================================================================
// METADATA - Reflection Config Package
// ============================================================================
// Purpose: Load and manage reflection configuration
// Biblical: Psalm 77:12 - "I will meditate also of all thy work"

package config

import (
	pkgtoml "github.com/creativeworkzstudio/claude-global/pkg/util/fs/toml"
)

// ============================================================================
// SETUP - Types
// ============================================================================

// Config holds the reflection configuration
type Config struct {
	Dimensions  map[string]DimensionConfig `toml:"dimensions"`
	Depth       map[string]DepthConfig     `toml:"depth"`
	Triggers    TriggersConfig             `toml:"triggers"`
	Templates   map[string]TemplateConfig  `toml:"templates"`
	Integration IntegrationConfig          `toml:"integration"`
}

// DimensionConfig defines a reflection dimension
type DimensionConfig struct {
	Name        string   `toml:"name"`
	Description string   `toml:"description"`
	Questions   []string `toml:"questions"`
}

// DepthConfig defines a reflection depth level
type DepthConfig struct {
	Name        string   `toml:"name"`
	Description string   `toml:"description"`
	Dimensions  []string `toml:"dimensions"`
	Output      string   `toml:"output"`
}

// TriggersConfig defines when to reflect
type TriggersConfig struct {
	TimeBased  []string `toml:"time_based"`
	EventBased []string `toml:"event_based"`
	Prompted   []string `toml:"prompted"`
}

// TemplateConfig holds template format
type TemplateConfig struct {
	Format string `toml:"format"`
}

// IntegrationConfig defines integration signals
type IntegrationConfig struct {
	Signals  []string `toml:"signals"`
	SkipWhen []string `toml:"skip_when"`
}

// ============================================================================
// BODY - Core Operations
// ============================================================================

// Load reads the configuration from file
func Load() (*Config, error) {
	var cfg Config
	searchPaths := pkgtoml.SkillConfigPaths("reflect-on-session")
	if err := pkgtoml.FindAndLoad("reflection-config.toml", searchPaths, &cfg); err != nil {
		return nil, err
	}
	return &cfg, nil
}

// GetDimensionNames returns all dimension names
func (c *Config) GetDimensionNames() []string {
	names := make([]string, 0, len(c.Dimensions))
	for name := range c.Dimensions {
		names = append(names, name)
	}
	return names
}

// GetDepthNames returns all depth level names
func (c *Config) GetDepthNames() []string {
	names := make([]string, 0, len(c.Depth))
	for name := range c.Depth {
		names = append(names, name)
	}
	return names
}
