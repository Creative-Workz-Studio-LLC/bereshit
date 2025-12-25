// ============================================================================
// METADATA
// ============================================================================
// Package: sections/health
// Purpose: HEALTH section - CPI-SI health score display
// Biblical: 7 Hebrew states from Shavar (broken) to Tamim (perfect)

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"

	"creativeworkzstudio.com/bereshit/word/work/pkg/util/term/display"
	"creativeworkzstudio.com/bereshit/word/work/pkg/core/health"
	"creativeworkzstudio.com/bereshit/word/work/pkg/core/statemachine"
)

// ============================================================================
// BODY
// ============================================================================

// BuildHealth returns the HEALTH section
//
// Format: ❤️ +75 SHALEM
func BuildHealth(runtime *statemachine.RuntimeState, cfg *health.HealthConfig) SectionResult {
	// Get health score from runtime state
	score := float64(0) // Default to balanced
	if runtime != nil {
		score = runtime.Session.HealthScore
	}

	// Get display info
	info := health.GetHealthDisplayFromConfig(score, cfg)
	if !info.HasInfo {
		return Empty()
	}

	content := fmt.Sprintf("%s%s %s%s",
		info.Color, info.Emoji, info.Formatted, display.Reset)

	return New(content, 6)
}

// BuildHealthCompact returns just emoji and score
func BuildHealthCompact(runtime *statemachine.RuntimeState) SectionResult {
	score := float64(0)
	if runtime != nil {
		score = runtime.Session.HealthScore
	}

	info := health.GetHealthDisplay(score)

	content := fmt.Sprintf("%s%s%.0f%s",
		info.Color, info.Emoji, score, display.Reset)

	return New(content, 6)
}

// ============================================================================
// CLOSING
// ============================================================================
