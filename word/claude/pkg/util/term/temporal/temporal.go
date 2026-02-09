// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-temporal
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

	"cws.studio/pkg/util/term/display"
)

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

// GetTimeOfDay classifies a time into time-of-day category
func GetTimeOfDay(t time.Time) TimeOfDay {
	hour := t.Hour()

	switch {
	case hour >= 5 && hour < 9:
		return TimeOfDay{
			Name:  "morning",
			Icon:  "🌅",
			Label: "morning",
			Color: display.Yellow,
			Hour:  hour,
		}
	case hour >= 9 && hour < 12:
		return TimeOfDay{
			Name:  "mid-morning",
			Icon:  "☀️",
			Label: "mid-morning",
			Color: display.BrightYellow,
			Hour:  hour,
		}
	case hour >= 12 && hour < 14:
		return TimeOfDay{
			Name:  "midday",
			Icon:  "🌞",
			Label: "midday",
			Color: display.Yellow,
			Hour:  hour,
		}
	case hour >= 14 && hour < 17:
		return TimeOfDay{
			Name:  "afternoon",
			Icon:  "🌤️",
			Label: "afternoon",
			Color: display.Cyan,
			Hour:  hour,
		}
	case hour >= 17 && hour < 20:
		return TimeOfDay{
			Name:  "evening",
			Icon:  "🌆",
			Label: "evening",
			Color: display.Blue,
			Hour:  hour,
		}
	case hour >= 20 && hour < 23:
		return TimeOfDay{
			Name:  "night",
			Icon:  "🌙",
			Label: "night",
			Color: display.Magenta,
			Hour:  hour,
		}
	default: // 23-5
		return TimeOfDay{
			Name:  "late-night",
			Icon:  "🌑",
			Label: "late night",
			Color: display.Dim + display.Blue,
			Hour:  hour,
		}
	}
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

// GetSessionPhase classifies session duration
func GetSessionPhase(elapsed time.Duration) SessionPhase {
	minutes := int(elapsed.Minutes())

	switch {
	case minutes < 15:
		return SessionPhase{
			Name:     "fresh",
			Duration: elapsed,
			Color:    display.Green,
		}
	case minutes < 60:
		return SessionPhase{
			Name:     "active",
			Duration: elapsed,
			Color:    display.Cyan,
		}
	case minutes < 120:
		return SessionPhase{
			Name:     "sustained",
			Duration: elapsed,
			Color:    display.Yellow,
		}
	case minutes < 240:
		return SessionPhase{
			Name:     "long",
			Duration: elapsed,
			Color:    display.Blue,
		}
	default:
		return SessionPhase{
			Name:     "extended",
			Duration: elapsed,
			Color:    display.Magenta,
		}
	}
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

// GetCircadianPhase returns circadian phase for hour
func GetCircadianPhase(hour int) CircadianPhase {
	switch {
	case hour >= 5 && hour < 9:
		return CircadianPhase{
			Name:        "rising",
			Productivity: "increasing",
			Note:        "cortisol rising, alertness building",
		}
	case hour >= 9 && hour < 12:
		return CircadianPhase{
			Name:        "peak",
			Productivity: "high",
			Note:        "peak cognitive performance",
		}
	case hour >= 12 && hour < 14:
		return CircadianPhase{
			Name:        "dip",
			Productivity: "lower",
			Note:        "post-lunch dip normal",
		}
	case hour >= 14 && hour < 17:
		return CircadianPhase{
			Name:        "recovery",
			Productivity: "moderate",
			Note:        "second wind building",
		}
	case hour >= 17 && hour < 21:
		return CircadianPhase{
			Name:        "evening",
			Productivity: "variable",
			Note:        "creativity often high",
		}
	case hour >= 21 || hour < 5:
		return CircadianPhase{
			Name:        "rest",
			Productivity: "low",
			Note:        "body preparing for rest",
		}
	default:
		return CircadianPhase{
			Name:        "normal",
			Productivity: "normal",
			Note:        "",
		}
	}
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

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
