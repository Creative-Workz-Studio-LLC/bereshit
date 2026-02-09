// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: session-awareness-pkg-config
// Purpose: Configuration loader for session-awareness skill
// Biblical: Lamentations 3:22-23 - "His compassions fail not"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
//
// Config: ../../../config/session-config.toml
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

// Paths configures where session data lives
type Paths struct {
	SessionDir     string `toml:"session_dir"`
	CurrentSession string `toml:"current_session"`
	SessionLog     string `toml:"session_log"`
	PatternFile    string `toml:"pattern_file"`
}

// DurationCategory defines a session duration category
type DurationCategory struct {
	Name        string `toml:"name"`
	MinMinutes  int    `toml:"min_minutes"`
	MaxMinutes  int    `toml:"max_minutes"`
	Description string `toml:"description"`
	Guidance    string `toml:"guidance"`
}

// Duration holds all duration configuration
type Duration struct {
	Order       []string                    `toml:"order"`
	QuickCheck  DurationCategory            `toml:"quick_check"`
	NormalWork  DurationCategory            `toml:"normal_work"`
	DeepWork    DurationCategory            `toml:"deep_work"`
	LongSession DurationCategory            `toml:"long_session"`
	Categories  map[string]DurationCategory `toml:"-"` // Built after load
}

// TimeStates configures idle detection
type TimeStates struct {
	IdleThresholdMinutes int `toml:"idle_threshold_minutes"`
}

// CircadianWindow defines a time window
type CircadianWindow struct {
	Name         string `toml:"name"`
	StartHour    int    `toml:"start_hour"`
	EndHour      int    `toml:"end_hour"`
	Phase        string `toml:"phase"`
	Productivity string `toml:"productivity"`
	Note         string `toml:"note"`
}

// Circadian holds circadian configuration
type Circadian struct {
	Phases  map[string]string `toml:"phases"`
	Windows []CircadianWindow `toml:"windows"`
}

// Output configures output formatting
type Output struct {
	TimeFormat      string `toml:"time_format"`
	ShortTimeFormat string `toml:"short_time_format"`
	DateFormat      string `toml:"date_format"`
}

// Principles holds guiding principles
type Principles struct {
	AwarenessPurpose string   `toml:"awareness_purpose"`
	IdentitySource   string   `toml:"identity_source"`
	RenewalSource    string   `toml:"renewal_source"`
	Avoid            []string `toml:"avoid"`
}

// Config holds all session awareness configuration
type Config struct {
	Paths      Paths      `toml:"paths"`
	Duration   Duration   `toml:"duration"`
	TimeStates TimeStates `toml:"time_states"`
	Circadian  Circadian  `toml:"circadian"`
	Output     Output     `toml:"output"`
	Principles Principles `toml:"principles"`
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Helpers ---

// buildDurationCategories populates the Categories map from named fields
func (c *Config) buildDurationCategories() {
	c.Duration.Categories = map[string]DurationCategory{
		"quick_check":  c.Duration.QuickCheck,
		"normal_work":  c.Duration.NormalWork,
		"deep_work":    c.Duration.DeepWork,
		"long_session": c.Duration.LongSession,
	}
}

// --- Core Operations ---

// Load reads and parses the session configuration
func Load() (*Config, error) {
	var cfg Config
	searchPaths := pkgtoml.SkillConfigPaths("session-awareness")
	if err := pkgtoml.FindAndLoad("session-config.toml", searchPaths, &cfg); err != nil {
		return nil, err
	}

	// Build derived data
	cfg.buildDurationCategories()

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

// GetDurationCategory returns the category for a given duration in minutes
func (c *Config) GetDurationCategory(minutes int) DurationCategory {
	for _, name := range c.Duration.Order {
		cat, ok := c.Duration.Categories[name]
		if !ok {
			continue
		}
		if minutes >= cat.MinMinutes && minutes < cat.MaxMinutes {
			return cat
		}
	}
	// Default to long session
	return c.Duration.LongSession
}

// GetCircadianWindow returns the circadian window for a given hour
func (c *Config) GetCircadianWindow(hour int) *CircadianWindow {
	for _, window := range c.Circadian.Windows {
		// Handle overnight windows (e.g., 21-02)
		if window.StartHour > window.EndHour {
			// Overnight window
			if hour >= window.StartHour || hour < window.EndHour {
				return &window
			}
		} else {
			// Normal window
			if hour >= window.StartHour && hour < window.EndHour {
				return &window
			}
		}
	}
	return nil
}

// IsIdleThresholdExceeded checks if gap exceeds idle threshold
func (c *Config) IsIdleThresholdExceeded(gapMinutes int) bool {
	return gapMinutes >= c.TimeStates.IdleThresholdMinutes
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
