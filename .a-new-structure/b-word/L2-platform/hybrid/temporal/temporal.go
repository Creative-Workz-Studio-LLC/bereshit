//omni:code --go -library
//omni:key B-L2-util-term-temporal
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-util-term-temporal
// Purpose: Shared temporal awareness utilities
// Biblical: Ecclesiastes 3:1 - "To every thing there is a season"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-10
//
// Used by: statusline, session-awareness, hooks
//
// ═══════════════════════════════════════════════════════════════════════════

package temporal

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"time"

	"creativeworkzstudio.com/bereshit/L2-platform/hybrid/registry"
	"creativeworkzstudio.com/bereshit/L2-platform/hybrid/terminal/display"
)

// ────────────────────────────────────────────────────────────────────────────
// S.1 Config — database-backed time boundaries with hardcoded fallback
// ────────────────────────────────────────────────────────────────────────────

// TimeRange defines a named time period with display properties.
type TimeRange struct {
	Name       string
	StartHour  int
	EndHour    int
	Icon       string
	Color      string
}

// SessionRange defines a named session duration boundary.
type SessionRange struct {
	Name       string
	MaxMinutes int // upper bound in minutes (0 = unbounded)
	Color      string
}

// CircadianRange defines a circadian phase boundary.
type CircadianRange struct {
	Name         string
	StartHour    int
	EndHour      int
	Productivity string
	Note         string
}

// Config holds all temporal boundary definitions.
type Config struct {
	TimeOfDay []TimeRange
	Session   []SessionRange
	Circadian []CircadianRange
}

// DefaultConfig returns hardcoded temporal boundaries matching boundaries.toml.
func DefaultConfig() Config {
	return Config{
		TimeOfDay: []TimeRange{
			{"morning", 5, 9, "🌅", display.Yellow},
			{"mid-morning", 9, 12, "☀️", display.BrightYellow},
			{"midday", 12, 14, "🌞", display.Yellow},
			{"afternoon", 14, 17, "🌤️", display.Cyan},
			{"evening", 17, 20, "🌆", display.Blue},
			{"night", 20, 23, "🌙", display.Magenta},
			{"late-night", 23, 5, "🌑", display.Dim + display.Blue},
		},
		Session: []SessionRange{
			{"fresh", 15, display.Green},
			{"active", 60, display.Cyan},
			{"sustained", 120, display.Yellow},
			{"long", 240, display.Blue},
			{"extended", 0, display.Magenta},
		},
		Circadian: []CircadianRange{
			{"rising", 5, 9, "increasing", "cortisol rising, alertness building"},
			{"peak", 9, 12, "high", "peak cognitive performance"},
			{"dip", 12, 14, "lower", "post-lunch dip normal"},
			{"recovery", 14, 17, "moderate", "second wind building"},
			{"evening", 17, 21, "variable", "creativity often high"},
			{"rest", 21, 5, "low", "body preparing for rest"},
		},
	}
}

// package-level config
var cfg = DefaultConfig()

// Init sets the package configuration. Pass nil to keep defaults.
func Init(c *Config) {
	if c != nil {
		cfg = *c
	}
}

// ────────────────────────────────────────────────────────────────────────────
// S.2 Types
// ────────────────────────────────────────────────────────────────────────────

// TimeOfDay represents time-of-day classification
type TimeOfDay struct {
	Name  string // morning, afternoon, evening, night
	Icon  string
	Label string
	Color string
	Hour  int
}

// TimeOfDayDisplay for statusline rendering (includes HasInfo)
type TimeOfDayDisplay struct {
	Icon    string
	Label   string
	Color   string
	HasInfo bool
}

// SessionPhase represents session duration classification
type SessionPhase struct {
	Name     string // fresh, active, sustained, long, extended
	Duration time.Duration
	Color    string
}

// SessionPhaseDisplay for statusline rendering
type SessionPhaseDisplay struct {
	Phase    string
	Duration string
	Color    string
	HasInfo  bool
}

// CircadianPhase represents circadian rhythm phase
type CircadianPhase struct {
	Name        string
	Productivity string
	Note        string
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Time of Day ---

// GetTimeOfDay classifies a time into time-of-day category.
// Uses config-driven time ranges from the database (or defaults).
func GetTimeOfDay(t time.Time) TimeOfDay {
	hour := t.Hour()

	for _, tr := range cfg.TimeOfDay {
		if inHourRange(hour, tr.StartHour, tr.EndHour) {
			return TimeOfDay{
				Name:  tr.Name,
				Icon:  tr.Icon,
				Label: tr.Name,
				Color: tr.Color,
				Hour:  hour,
			}
		}
	}

	// Fallback — should never reach here with valid config
	return TimeOfDay{Name: "unknown", Icon: "❓", Label: "unknown", Color: display.White, Hour: hour}
}

// inHourRange checks if hour is within [start, end), handling midnight wrap.
func inHourRange(hour, start, end int) bool {
	if start < end {
		return hour >= start && hour < end
	}
	// Wraps midnight: e.g., 23-5 means hour >= 23 OR hour < 5
	return hour >= start || hour < end
}

// GetTimeOfDayNow returns current time-of-day
func GetTimeOfDayNow() TimeOfDay {
	return GetTimeOfDay(time.Now())
}

// GetTimeOfDayDisplay returns display-ready time-of-day info
func GetTimeOfDayDisplay(t time.Time) TimeOfDayDisplay {
	tod := GetTimeOfDay(t)
	return TimeOfDayDisplay{
		Icon:    tod.Icon,
		Label:   tod.Label,
		Color:   tod.Color,
		HasInfo: true,
	}
}

// GetTimeOfDayDisplayNow returns current time-of-day display
func GetTimeOfDayDisplayNow() TimeOfDayDisplay {
	return GetTimeOfDayDisplay(time.Now())
}

// --- Session Phase ---

// GetSessionPhase classifies session duration.
// Uses config-driven session boundaries from the database (or defaults).
func GetSessionPhase(elapsed time.Duration) SessionPhase {
	minutes := int(elapsed.Minutes())

	for _, sr := range cfg.Session {
		if sr.MaxMinutes == 0 || minutes < sr.MaxMinutes {
			return SessionPhase{
				Name:     sr.Name,
				Duration: elapsed,
				Color:    sr.Color,
			}
		}
	}

	// Fallback — last session range should have MaxMinutes=0
	last := cfg.Session[len(cfg.Session)-1]
	return SessionPhase{Name: last.Name, Duration: elapsed, Color: last.Color}
}

// GetSessionPhaseSince returns phase based on start time
func GetSessionPhaseSince(start time.Time) SessionPhase {
	return GetSessionPhase(time.Since(start))
}

// GetSessionPhaseDisplay returns display-ready session phase info
func GetSessionPhaseDisplay(startTime time.Time) SessionPhaseDisplay {
	if startTime.IsZero() {
		return SessionPhaseDisplay{HasInfo: false}
	}

	phase := GetSessionPhaseSince(startTime)
	return SessionPhaseDisplay{
		Phase:    phase.Name,
		Duration: FormatDuration(phase.Duration),
		Color:    phase.Color,
		HasInfo:  true,
	}
}

// --- Circadian Awareness ---

// GetCircadianPhase returns circadian phase for hour.
// Uses config-driven circadian boundaries from the database (or defaults).
func GetCircadianPhase(hour int) CircadianPhase {
	for _, cr := range cfg.Circadian {
		if inHourRange(hour, cr.StartHour, cr.EndHour) {
			return CircadianPhase{
				Name:        cr.Name,
				Productivity: cr.Productivity,
				Note:        cr.Note,
			}
		}
	}

	return CircadianPhase{Name: "normal", Productivity: "normal", Note: ""}
}

// GetCircadianPhaseNow returns current circadian phase
func GetCircadianPhaseNow() CircadianPhase {
	return GetCircadianPhase(time.Now().Hour())
}

// --- Duration Formatting ---

// FormatDuration formats duration as human-readable string
func FormatDuration(d time.Duration) string {
	hours := int(d.Hours())
	minutes := int(d.Minutes()) % 60

	if hours > 0 {
		return itoa(hours) + "h " + itoa(minutes) + "m"
	}
	return itoa(minutes) + "m"
}

// FormatDurationShort formats duration compactly
func FormatDurationShort(d time.Duration) string {
	hours := int(d.Hours())
	minutes := int(d.Minutes()) % 60

	if hours > 0 {
		return itoa(hours) + "h" + itoa(minutes) + "m"
	}
	return itoa(minutes) + "m"
}

func itoa(i int) string {
	if i == 0 {
		return "0"
	}
	var digits []byte
	for i > 0 {
		digits = append([]byte{byte('0' + i%10)}, digits...)
		i /= 10
	}
	return string(digits)
}

// ────────────────────────────────────────────────────────────────────────────
// B.5 Registry Integration
// ────────────────────────────────────────────────────────────────────────────

// ConfigFromRegistry builds a Config from the L2 platform database
// via the cross-layer registry. Falls back to defaults for any field
// that can't be loaded.
func ConfigFromRegistry(r *registry.Registry) Config {
	c := DefaultConfig()

	// Time of day ranges
	if items, err := r.Platform.TemporalByCategory("time_of_day"); err == nil && len(items) > 0 {
		ranges := make([]TimeRange, 0, len(items))
		for _, t := range items {
			ranges = append(ranges, TimeRange{
				Name:      t.Name,
				StartHour: t.StartValue,
				EndHour:   t.EndValue,
				Icon:      t.Icon,
				Color:     t.Color,
			})
		}
		c.TimeOfDay = ranges
	}

	// Session phase ranges
	if items, err := r.Platform.TemporalByCategory("session_phase"); err == nil && len(items) > 0 {
		ranges := make([]SessionRange, 0, len(items))
		for _, t := range items {
			ranges = append(ranges, SessionRange{
				Name:       t.Name,
				MaxMinutes: t.EndValue,
				Color:      t.Color,
			})
		}
		c.Session = ranges
	}

	// Circadian ranges
	if items, err := r.Platform.TemporalByCategory("circadian"); err == nil && len(items) > 0 {
		ranges := make([]CircadianRange, 0, len(items))
		for _, t := range items {
			ranges = append(ranges, CircadianRange{
				Name:         t.Name,
				StartHour:    t.StartValue,
				EndHour:      t.EndValue,
				Productivity: t.Extra,
				Note:         t.Description,
			})
		}
		c.Circadian = ranges
	}

	return c
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
