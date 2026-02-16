//omni:code --go -library
//omni:key B-L3-health-display
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
// Package: health/display
// Purpose: Health score display helpers for statusline and terminal
// Biblical: 7 Hebrew states of wholeness (Shavar to Tamim)
//
// Ternary health scale:
//   -100 (SHAVAR/broken) → 0 (SHALEM/even) → +100 (TAMIM/perfect)

package health

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"

	"creativeworkzstudio.com/bereshit/L2-platform/hybrid/terminal/display"
)

// HealthDisplayInfo contains all display information for a health score
type HealthDisplayInfo struct {
	Score       float64 // -100 to +100
	Level       string  // broken, wanting, lacking, even, sound, whole, perfect
	Hebrew      string  // Hebrew word (SHAVAR, CHASER, etc.)
	Emoji       string  // Visual emoji
	Color       string  // ANSI color code
	HexColor    string  // Hex color for WezTerm
	Sign        string  // +, -, or empty
	Formatted   string  // e.g., "+75 SHALEM"
	HasInfo     bool
}

// ============================================================================
// BODY
// ============================================================================

// --- Level Determination ---

// GetLevel returns the health level name for a score
func GetLevel(score float64) string {
	switch {
	case score <= -75:
		return "broken"
	case score <= -50:
		return "wanting"
	case score <= -25:
		return "lacking"
	case score <= 25:
		return "even"
	case score <= 50:
		return "sound"
	case score <= 75:
		return "whole"
	default:
		return "perfect"
	}
}

// GetHebrew returns the Hebrew term for a health level
func GetHebrew(level string) string {
	hebrewTerms := map[string]string{
		"broken":  "SHAVAR",  // שָׁבַר - shattered
		"wanting": "RATSAH",  // רָצָה - wanting
		"lacking": "CHASER",  // חָסֵר - lacking
		"even":    "SHALEM",  // שָׁלֵם - complete/balanced
		"sound":   "YASHAR",  // יָשָׁר - upright
		"whole":   "TOV",     // טוֹב - good
		"perfect": "TAMIM",   // תָּמִים - perfect/complete
	}
	if hebrew, ok := hebrewTerms[level]; ok {
		return hebrew
	}
	return "SHALEM" // Default to balanced
}

// --- Color Helpers ---

// HealthColor returns ANSI color based on health score
func HealthColor(score float64) string {
	switch {
	case score <= -50:
		return display.Red // Broken/wanting
	case score <= -25:
		return display.Yellow // Lacking
	case score <= 25:
		return display.Cyan // Even (balanced)
	case score <= 50:
		return display.Green // Sound
	default:
		return display.Green + display.Bold // Whole/perfect
	}
}

// HealthHexColor returns hex color for WezTerm
func HealthHexColor(score float64) string {
	switch {
	case score <= -50:
		return "#ff6b6b" // Red
	case score <= -25:
		return "#ffd93d" // Yellow
	case score <= 25:
		return "#88ccff" // Cyan
	case score <= 50:
		return "#6bcb77" // Green
	default:
		return "#98fb98" // Bright green
	}
}

// --- Emoji Helpers ---

// HealthEmoji returns emoji for a health level
func HealthEmoji(level string) string {
	emojis := map[string]string{
		"broken":  "💔", // Shattered heart
		"wanting": "😔", // Sad
		"lacking": "😐", // Neutral
		"even":    "⚖️", // Balance scales
		"sound":   "💪", // Strong
		"whole":   "✨", // Sparkles
		"perfect": "👑", // Crown
	}
	if emoji, ok := emojis[level]; ok {
		return emoji
	}
	return "❤️" // Default heart
}

// --- Display Formatting ---

// FormatScore formats a health score with sign
func FormatScore(score float64) string {
	if score > 0 {
		return fmt.Sprintf("+%.0f", score)
	}
	return fmt.Sprintf("%.0f", score)
}

// GetHealthDisplay returns complete health display information
func GetHealthDisplay(score float64) HealthDisplayInfo {
	level := GetLevel(score)
	hebrew := GetHebrew(level)

	sign := ""
	if score > 0 {
		sign = "+"
	} else if score < 0 {
		sign = "-"
	}

	formatted := fmt.Sprintf("%s%.0f %s", sign, absFloat(score), hebrew)
	if score == 0 {
		formatted = fmt.Sprintf("0 %s", hebrew)
	}

	return HealthDisplayInfo{
		Score:     score,
		Level:     level,
		Hebrew:    hebrew,
		Emoji:     HealthEmoji(level),
		Color:     HealthColor(score),
		HexColor:  HealthHexColor(score),
		Sign:      sign,
		Formatted: formatted,
		HasInfo:   true,
	}
}

// GetHealthDisplayFromConfig uses HealthConfig for display (uses TOML definitions)
func GetHealthDisplayFromConfig(score float64, cfg *HealthConfig) HealthDisplayInfo {
	if cfg == nil {
		return GetHealthDisplay(score)
	}

	// Use config's display definitions if available
	level := GetLevel(score)
	info := GetHealthDisplay(score)

	// Override with config values if present
	var displayDef *DisplayDef
	switch level {
	case "broken":
		displayDef = &cfg.Display.Broken
	case "wanting":
		displayDef = &cfg.Display.Wanting
	case "lacking":
		displayDef = &cfg.Display.Lacking
	case "even":
		displayDef = &cfg.Display.Even
	case "sound":
		displayDef = &cfg.Display.Sound
	case "whole":
		displayDef = &cfg.Display.Whole
	case "perfect":
		displayDef = &cfg.Display.Perfect
	}

	if displayDef != nil && displayDef.Emoji != "" {
		info.Emoji = displayDef.Emoji
	}
	if displayDef != nil && displayDef.Color != "" {
		info.HexColor = displayDef.Color
	}

	return info
}

// --- Helper Functions ---

func absFloat(f float64) float64 {
	if f < 0 {
		return -f
	}
	return f
}

// ============================================================================
// CLOSING
// ============================================================================
// Related: pkg/health/loader.go (config loading)
//          pkg/wezterm/osc.go (terminal emission)
//          statusline/lib/sections/health.go (section builder)
