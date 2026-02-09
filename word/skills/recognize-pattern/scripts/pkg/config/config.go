// ============================================================================
// METADATA - Pattern Config Package
// ============================================================================
// Purpose: Load and manage pattern configuration
// Biblical: Ecclesiastes 1:9 - "There is no new thing under the sun"

package config

import (
	pkgtoml "cws.studio/pkg/util/fs/toml"
)

// ============================================================================
// SETUP - Types
// ============================================================================

// Config holds the pattern configuration
type Config struct {
	Categories map[string]CategoryConfig   `toml:"categories"`
	Strength   StrengthConfig              `toml:"strength"`
	Responses  map[string]ResponseConfig   `toml:"responses"`
	Questions  QuestionsConfig             `toml:"questions"`
}

// CategoryConfig defines a pattern category
type CategoryConfig struct {
	Name        string   `toml:"name"`
	Description string   `toml:"description"`
	Examples    []string `toml:"examples"`
	Signals     []string `toml:"signals"`
}

// StrengthConfig defines pattern strength levels
type StrengthConfig struct {
	MinOccurrences int               `toml:"min_occurrences"`
	Emerging       StrengthLevel     `toml:"emerging"`
	Established    StrengthLevel     `toml:"established"`
	Core           StrengthLevel     `toml:"core"`
}

// StrengthLevel defines a strength band
type StrengthLevel struct {
	Min   int    `toml:"min"`
	Max   int    `toml:"max"`
	Label string `toml:"label"`
}

// ResponseConfig defines responses to patterns
type ResponseConfig struct {
	Positive []string `toml:"positive"`
	Negative []string `toml:"negative"`
	Action   []string `toml:"action"`
}

// QuestionsConfig holds triggering questions
type QuestionsConfig struct {
	Recognition []string `toml:"recognition"`
	Assessment  []string `toml:"assessment"`
	Action      []string `toml:"action"`
}

// ============================================================================
// BODY - Core Operations
// ============================================================================

// Load reads the configuration from file
func Load() (*Config, error) {
	var cfg Config
	searchPaths := pkgtoml.SkillConfigPaths("recognize-pattern")
	if err := pkgtoml.FindAndLoad("pattern-config.toml", searchPaths, &cfg); err != nil {
		return nil, err
	}
	return &cfg, nil
}

// GetCategoryNames returns all category names
func (c *Config) GetCategoryNames() []string {
	names := make([]string, 0, len(c.Categories))
	for name := range c.Categories {
		names = append(names, name)
	}
	return names
}

// GetStrengthLabel returns strength label for occurrence count
func (c *Config) GetStrengthLabel(count int) string {
	if count >= c.Strength.Core.Min {
		return c.Strength.Core.Label
	}
	if count >= c.Strength.Established.Min {
		return c.Strength.Established.Label
	}
	if count >= c.Strength.Emerging.Min {
		return c.Strength.Emerging.Label
	}
	return "Not yet a pattern"
}
