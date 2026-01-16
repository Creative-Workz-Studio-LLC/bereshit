// ============================================================================
// METADATA - Meta-Awareness Config Package
// ============================================================================
// Purpose: Load and manage awareness configuration
// Biblical: Proverbs 4:26 - "Ponder the path of thy feet"

package config

import (
	pkgtoml "github.com/creativeworkzstudio/claude-global/pkg/util/fs/toml"
)

// ============================================================================
// SETUP - Types
// ============================================================================

// Config holds the awareness configuration
type Config struct {
	Prompts   map[string]PromptConfig   `toml:"prompts"`
	States    map[string]StateConfig    `toml:"states"`
	Triggers  TriggersConfig            `toml:"triggers"`
	Responses map[string]ResponseConfig `toml:"responses"`
}

// PromptConfig defines a checkpoint prompt category
type PromptConfig struct {
	Name        string   `toml:"name"`
	Description string   `toml:"description"`
	Questions   []string `toml:"questions"`
}

// StateConfig defines a recognizable cognitive state
type StateConfig struct {
	Name       string   `toml:"name"`
	Indicators []string `toml:"indicators"`
}

// TriggersConfig defines when to check
type TriggersConfig struct {
	TimeBased    []string `toml:"time_based"`
	EventBased   []string `toml:"event_based"`
	PatternBased []string `toml:"pattern_based"`
}

// ResponseConfig defines actions for a state
type ResponseConfig struct {
	Actions []string `toml:"actions"`
}

// ============================================================================
// BODY - Core Operations
// ============================================================================

// Load reads the configuration from file
func Load() (*Config, error) {
	var cfg Config
	searchPaths := pkgtoml.SkillConfigPaths("meta-awareness")
	if err := pkgtoml.FindAndLoad("awareness-config.toml", searchPaths, &cfg); err != nil {
		return nil, err
	}
	return &cfg, nil
}

// GetPromptCategories returns all prompt category names
func (c *Config) GetPromptCategories() []string {
	names := make([]string, 0, len(c.Prompts))
	for name := range c.Prompts {
		names = append(names, name)
	}
	return names
}

// GetStateNames returns all state names
func (c *Config) GetStateNames() []string {
	names := make([]string, 0, len(c.States))
	for name := range c.States {
		names = append(names, name)
	}
	return names
}
