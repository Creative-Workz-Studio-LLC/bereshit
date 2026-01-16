// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: recognize-stopping-point-pkg-config
// Purpose: Configuration loader for recognize-stopping-point skill
// Biblical: Ecclesiastes 3:1 - "To every thing there is a season"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
//
// Config: ../../../config/stopping-config.toml
//
// ═══════════════════════════════════════════════════════════════════════════

package config

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"os"
	"path/filepath"

	pkgtoml "github.com/creativeworkzstudio/claude-global/pkg/util/fs/toml"
)

// Paths configures where session data lives
type Paths struct {
	SessionDir     string `toml:"session_dir"`
	CurrentSession string `toml:"current_session"`
	SessionLog     string `toml:"session_log"`
}

// StoppingType defines a category of stopping point
type StoppingType struct {
	Name        string   `toml:"name"`
	Weight      int      `toml:"weight"`
	Description string   `toml:"description"`
	Indicators  []string `toml:"indicators"`
	Decision    string   `toml:"decision"`
}

// Duration holds duration-related thresholds
type Duration struct {
	QuickCheckMax     int `toml:"quick_check_max"`
	NormalWorkMax     int `toml:"normal_work_max"`
	DeepWorkMax       int `toml:"deep_work_max"`
	SoftLimitDeepWork int `toml:"soft_limit_deep_work"`
	SoftLimitNormal   int `toml:"soft_limit_normal"`
	HardLimitDeepWork int `toml:"hard_limit_deep_work"`
	HardLimitNormal   int `toml:"hard_limit_normal"`
}

// Circadian holds time-of-day thresholds
type Circadian struct {
	EveningStartHour        int `toml:"evening_start_hour"`
	LateEveningHour         int `toml:"late_evening_hour"`
	EarlyMorningEnd         int `toml:"early_morning_end"`
	NightOwlProductiveUntil int `toml:"night_owl_productive_until"`
}

// Quality holds quality indicator lists
type Quality struct {
	DipIndicators         []string `toml:"dip_indicators"`
	HighQualityIndicators []string `toml:"high_quality_indicators"`
}

// EndReasons holds valid end reasons
type EndReasons struct {
	ValidReasons []string `toml:"valid_reasons"`
}

// Output configures output formatting
type Output struct {
	TimeFormat      string `toml:"time_format"`
	ShortTimeFormat string `toml:"short_time_format"`
}

// Principles holds guiding principles
type Principles struct {
	QualityOverDuration  string   `toml:"quality_over_duration"`
	NaturalOverArbitrary string   `toml:"natural_over_arbitrary"`
	StoppingIsWisdom     string   `toml:"stopping_is_wisdom"`
	IdentitySource       string   `toml:"identity_source"`
	Avoid                []string `toml:"avoid"`
	Embrace              []string `toml:"embrace"`
}

// Config holds all stopping point configuration
type Config struct {
	Paths         Paths                   `toml:"paths"`
	StoppingTypes map[string]StoppingType `toml:"stopping_types"`
	Duration      Duration                `toml:"duration"`
	Circadian     Circadian               `toml:"circadian"`
	Quality       Quality                 `toml:"quality"`
	EndReasons    EndReasons              `toml:"end_reasons"`
	Output        Output                  `toml:"output"`
	Principles    Principles              `toml:"principles"`
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Core Operations ---

// Load reads and parses the stopping point configuration
func Load() (*Config, error) {
	var cfg Config
	searchPaths := pkgtoml.SkillConfigPaths("recognize-stopping-point")
	if err := pkgtoml.FindAndLoad("stopping-config.toml", searchPaths, &cfg); err != nil {
		return nil, err
	}
	return &cfg, nil
}

// GetSessionPath returns the full path to session directory
func (c *Config) GetSessionPath() string {
	home, _ := os.UserHomeDir()
	return filepath.Join(home, c.Paths.SessionDir)
}

// GetCurrentSessionFile returns the full path to current session file
func (c *Config) GetCurrentSessionFile() string {
	return filepath.Join(c.GetSessionPath(), c.Paths.CurrentSession)
}

// GetSessionLogFile returns the full path to session log file
func (c *Config) GetSessionLogFile() string {
	return filepath.Join(c.GetSessionPath(), c.Paths.SessionLog)
}

// GetStoppingType returns a specific stopping type by key
func (c *Config) GetStoppingType(key string) (StoppingType, bool) {
	st, ok := c.StoppingTypes[key]
	return st, ok
}

// IsEndOfDayHour checks if the given hour is in evening/night window
func (c *Config) IsEndOfDayHour(hour int) bool {
	return hour >= c.Circadian.EveningStartHour || hour < c.Circadian.EarlyMorningEnd
}

// IsNightOwlProductiveHour checks if hour is in Seanje's productive night window
func (c *Config) IsNightOwlProductiveHour(hour int) bool {
	// Night owl productive: after evening start but before night owl limit
	return hour >= c.Circadian.EveningStartHour || hour < c.Circadian.NightOwlProductiveUntil
}

// IsDurationLimitReached checks duration against limits
func (c *Config) IsDurationLimitReached(minutes int, isDeepWork bool) (soft bool, hard bool) {
	if isDeepWork {
		soft = minutes >= c.Duration.SoftLimitDeepWork
		hard = minutes >= c.Duration.HardLimitDeepWork
	} else {
		soft = minutes >= c.Duration.SoftLimitNormal
		hard = minutes >= c.Duration.HardLimitNormal
	}
	return soft, hard
}

// IsValidEndReason checks if a reason is valid
func (c *Config) IsValidEndReason(reason string) bool {
	for _, r := range c.EndReasons.ValidReasons {
		if r == reason {
			return true
		}
	}
	return false
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
