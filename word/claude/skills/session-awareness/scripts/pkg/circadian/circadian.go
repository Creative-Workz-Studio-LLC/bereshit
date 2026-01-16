// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: session-awareness-pkg-circadian
// Purpose: Circadian awareness and time-of-day analysis
// Biblical: Lamentations 3:22-23 - "His compassions fail not"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
//
// ═══════════════════════════════════════════════════════════════════════════

package circadian

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"time"

	"github.com/creativeworkzstudio/claude-global/skills/session-awareness/scripts/pkg/config"
)

// CircadianResult holds the result of a circadian check
type CircadianResult struct {
	CurrentTime  time.Time
	Hour         int
	DayOfWeek    string
	WindowName   string
	Phase        string
	Productivity string
	Note         string
	IsWorkHours  bool
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Helpers ---

// isTypicalWorkHours checks if time is in typical work hours (weekday 8-18)
func isTypicalWorkHours(t time.Time) bool {
	dow := t.Weekday()
	hour := t.Hour()

	// Weekend = not typical work hours (but may work anyway)
	if dow == time.Saturday || dow == time.Sunday {
		return false
	}

	// Typical work hours 8am-6pm
	return hour >= 8 && hour < 18
}

// --- Core Operations ---

// Check performs a circadian awareness check
func Check(cfg *config.Config) *CircadianResult {
	now := time.Now()
	hour := now.Hour()
	window := cfg.GetCircadianWindow(hour)

	result := &CircadianResult{
		CurrentTime: now,
		Hour:        hour,
		DayOfWeek:   now.Weekday().String(),
		IsWorkHours: isTypicalWorkHours(now),
	}

	if window != nil {
		result.WindowName = window.Name
		result.Phase = window.Phase
		result.Productivity = window.Productivity
		result.Note = window.Note
	} else {
		result.WindowName = "unknown"
		result.Phase = "unknown"
		result.Productivity = "unknown"
		result.Note = "Outside configured windows"
	}

	return result
}

// GetGuidance returns guidance based on circadian state
func GetGuidance(result *CircadianResult) string {
	switch result.Phase {
	case "morning_peak":
		return "Peak cognitive hours - ideal for deep work"
	case "afternoon_stable":
		return "Sustained work - good for steady progress"
	case "evening_wind_down":
		return "Consider wrapping up at next natural milestone"
	case "night_owl":
		return "Seanje's productive zone - work with confidence"
	case "night_rest":
		return "Rest time - consider stopping unless urgent"
	default:
		return "Work as natural flow dictates"
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
