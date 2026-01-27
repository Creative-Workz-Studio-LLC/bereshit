// ============================================================================
// METADATA
// ============================================================================
// Package: sections/method
// Purpose: METHOD section - Shows current cognitive direction clearly
// Biblical: "A time to break down, and a time to build up" - Ecclesiastes 3:3
//
// Displays the building block method direction in human-readable terms:
//   🔨 BUILD  - Creating new structure (+1 key dominant)
//   ⬇ BREAK  - Completing, cleaning, halting (-1 key dominant)
//   🔄 PIVOT  - Transitioning between modes (recent direction change)
//   ◆ STEADY - Lateral maintenance (0 key dominant)

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"

	"github.com/creativeworkzstudio/claude-global/pkg/core/statemachine"
	"github.com/creativeworkzstudio/claude-global/pkg/util/term/display"
)

// ============================================================================
// BODY
// ============================================================================

// BuildMethod returns the current cognitive method indicator
//
// Format: 🔨 BUILD or ⬇ BREAK or 🔄 PIVOT or ◆ STEADY
// Based on: LastKeyChosen and KTowardGod/KTowardSelf pattern
func BuildMethod(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	// Get the last key chosen (most recent direction)
	lastKey := runtime.Session.LastKeyChosen

	// Get the overall pattern (which way are we trending?)
	towardGod := runtime.Session.KTowardGod   // +1 choices (building)
	towardSelf := runtime.Session.KTowardSelf // -1 choices (breaking down)

	// Determine the method based on last key and pattern
	var icon, label, color string

	switch lastKey {
	case 1: // Building up
		icon = "🔨"
		label = "BUILD"
		color = display.Green

	case -1: // Breaking down / completing
		icon = "⬇"
		label = "BREAK"
		color = display.Yellow

	case 0: // Lateral / maintenance
		// Check if we recently pivoted (pattern shows mixed direction)
		if towardGod > 0 && towardSelf > 0 {
			// Both directions have been used - we're pivoting
			ratio := float64(towardGod) / float64(towardGod+towardSelf)
			if ratio > 0.4 && ratio < 0.6 {
				// Close to 50/50 - genuine pivot
				icon = "🔄"
				label = "PIVOT"
				color = display.Cyan
			} else {
				// Leaning one way but currently lateral
				icon = "◆"
				label = "STEADY"
				color = display.Dim
			}
		} else {
			icon = "◆"
			label = "STEADY"
			color = display.Dim
		}

	default:
		// Unknown state - show steady
		icon = "◆"
		label = "STEADY"
		color = display.Dim
	}

	content := fmt.Sprintf("%s%s %s%s", color, icon, label, display.Reset)
	return New(content, 80) // High priority - method is important
}

// BuildMethodCompact returns a compact method indicator (just icon)
//
// Format: 🔨 or ⬇ or 🔄 or ◆
func BuildMethodCompact(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	lastKey := runtime.Session.LastKeyChosen

	var icon, color string
	switch lastKey {
	case 1:
		icon = "🔨"
		color = display.Green
	case -1:
		icon = "⬇"
		color = display.Yellow
	default:
		icon = "◆"
		color = display.Dim
	}

	content := fmt.Sprintf("%s%s%s", color, icon, display.Reset)
	return New(content, 80)
}

// ============================================================================
// CLOSING
// ============================================================================
