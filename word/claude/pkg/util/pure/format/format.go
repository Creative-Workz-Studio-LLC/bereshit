// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-format
// Purpose: Shared formatting utilities (durations, paths, numbers)
// Biblical: Proverbs 4:26 - "Ponder the path of thy feet"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-10
//
// Used by: statusline, session skills, any CLI with output formatting
//
// ═══════════════════════════════════════════════════════════════════════════

package format

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"os"
	"strings"
	"time"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Duration Formatting ---

// Duration formats a time.Duration as human-readable string
func Duration(d time.Duration) string {
	hours := int(d.Hours())
	minutes := int(d.Minutes()) % 60
	seconds := int(d.Seconds()) % 60

	if hours > 0 {
		return itoa(hours) + "h " + itoa(minutes) + "m"
	}
	if minutes > 0 {
		return itoa(minutes) + "m " + itoa(seconds) + "s"
	}
	return itoa(seconds) + "s"
}

// DurationShort formats duration compactly
func DurationShort(d time.Duration) string {
	hours := int(d.Hours())
	minutes := int(d.Minutes()) % 60

	if hours > 0 {
		return itoa(hours) + "h" + itoa(minutes) + "m"
	}
	return itoa(minutes) + "m"
}

// DurationMS formats milliseconds as duration
func DurationMS(ms int) string {
	if ms < 1000 {
		return "<1s"
	}
	return Duration(time.Duration(ms) * time.Millisecond)
}

// Minutes formats minutes as duration
func Minutes(m int) string {
	return Duration(time.Duration(m) * time.Minute)
}

// --- Path Formatting ---

// ShortenPath reduces path length for display
func ShortenPath(path string) string {
	if path == "" {
		return ""
	}

	// Replace home with ~
	home, _ := os.UserHomeDir()
	if strings.HasPrefix(path, home) {
		path = "~" + path[len(home):]
	}

	// If still too long, truncate with ...
	if len(path) > 50 {
		parts := strings.Split(path, "/")
		if len(parts) > 3 {
			return ".../" + strings.Join(parts[len(parts)-2:], "/")
		}
	}

	return path
}

// BaseName returns the last component of a path
func BaseName(path string) string {
	parts := strings.Split(path, "/")
	if len(parts) > 0 {
		return parts[len(parts)-1]
	}
	return path
}

// --- Number Formatting ---

// Itoa converts int to string without strconv
func Itoa(i int) string {
	return itoa(i)
}

func itoa(i int) string {
	if i == 0 {
		return "0"
	}
	if i < 0 {
		return "-" + itoa(-i)
	}

	var digits []byte
	for i > 0 {
		digits = append([]byte{byte('0' + i%10)}, digits...)
		i /= 10
	}
	return string(digits)
}

// Percent formats a percentage
func Percent(value, total float64) string {
	if total == 0 {
		return "0%"
	}
	pct := (value / total) * 100
	return itoa(int(pct)) + "%"
}

// --- String Formatting ---

// Truncate shortens string to max length with ellipsis
func Truncate(s string, max int) string {
	if len(s) <= max {
		return s
	}
	if max <= 3 {
		return s[:max]
	}
	return s[:max-3] + "..."
}

// PadRight pads string to width with spaces
func PadRight(s string, width int) string {
	if len(s) >= width {
		return s
	}
	return s + strings.Repeat(" ", width-len(s))
}

// PadLeft pads string to width with spaces on left
func PadLeft(s string, width int) string {
	if len(s) >= width {
		return s
	}
	return strings.Repeat(" ", width-len(s)) + s
}

// --- Model Formatting ---

// ShortModelName extracts short model name for display
func ShortModelName(displayName string) string {
	if displayName == "" {
		return "Unknown"
	}

	// Common mappings
	mappings := map[string]string{
		"Claude 3.5 Sonnet":     "Sonnet 3.5",
		"Claude 3.5 Haiku":      "Haiku 3.5",
		"Claude 3 Opus":         "Opus 3",
		"Claude 3 Sonnet":       "Sonnet 3",
		"Claude Opus 4":         "Opus 4",
		"Claude Opus 4.5":       "Opus 4.5",
		"Claude Sonnet 4":       "Sonnet 4",
		"Sonnet":                "Sonnet",
		"Opus":                  "Opus",
		"Haiku":                 "Haiku",
	}

	if short, ok := mappings[displayName]; ok {
		return short
	}

	// Default: return as-is but truncate if too long
	if len(displayName) > 15 {
		return displayName[:15] + "..."
	}
	return displayName
}

// GetShortModelName is an alias for ShortModelName
func GetShortModelName(displayName string) string {
	return ShortModelName(displayName)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
