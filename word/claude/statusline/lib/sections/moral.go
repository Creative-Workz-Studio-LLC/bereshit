// ============================================================================
// METADATA
// ============================================================================
// Package: sections/moral
// Purpose: K:MORAL section - Alignment compass display
// Biblical: "He shall direct thy paths" - Proverbs 3:6
//
// K:MORAL Compass:
//   k > 0: Moving TOWARD God (tov/ASPIRATION) — inherently safe
//          "Mine eyes are ever toward the LORD" — Psalm 25:15
//   k < 0: Moving TOWARD self (shavar/BROKEN) — needs validation
//          "Lean not unto thine own understanding" — Proverbs 3:5
//   k = 0: At center (yashar/ORIGIN) — balanced, no directional movement
//          "In all thy ways acknowledge him, and he shall direct thy paths" — Proverbs 3:6
//
// K:SCRIPTURE = fixed reference (10 Commandments in code)
// K:ALIGN = sliding measurement (current alignment -1.0 to +1.0)
//
// Thresholds grounded in Hebrew state Z values:
//   align >= +0.5: tamim/shalem level (SOUND/WHOLE) — justified
//   align >= 0.0:  yashar level (EVEN) — neutral, proceed with discernment
//   align < 0.0:   chaser/shavar level (LACKING/BROKEN) — unjustified

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"

	"github.com/creativeworkzstudio/claude-global/pkg/util/term/display"
	"github.com/creativeworkzstudio/claude-global/pkg/core/statemachine"
)

// ============================================================================
// BODY
// ============================================================================

// BuildMoral returns the K:MORAL section
//
// Format: k→0.8 (k direction, alignment score)
// k→ = toward God (k > 0), k← = toward self (k < 0), k◆ = centered (k = 0)
func BuildMoral(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	k := runtime.Session.KSelector
	align := runtime.Session.KAlign

	// Skip if no moral direction set
	if k == 0 && align == 0.0 {
		return Empty()
	}

	// Direction symbol based on k selector
	var dirSymbol string
	var color string

	switch {
	case k > 0:
		// Toward God (tov/ASPIRATION) - always safe
		dirSymbol = "→"
		color = display.Green
	case k < 0:
		// Toward self (shavar/BROKEN) - color based on alignment (needs validation)
		// Thresholds from Hebrew state Z values: +0.5 = tamim/shalem, 0.0 = yashar
		dirSymbol = "←"
		if align >= 0.5 {
			color = display.Green // tamim/shalem level - justified backward movement
		} else if align >= 0.0 {
			color = display.Yellow // yashar level - neutral, proceed with discernment
		} else {
			color = display.Red // chaser/shavar level - unjustified, moving toward brokenness
		}
	default:
		dirSymbol = "◆" // Centered (yashar/ORIGIN)
		color = display.Dim
	}

	// Format: k→0.8
	content := fmt.Sprintf("%sk%s%.1f%s", color, dirSymbol, align, display.Reset)

	return New(content, 7)
}

// BuildMoralCompact returns just the direction symbol
//
// Format: → (toward God) or ← (toward self) or ◆ (centered)
func BuildMoralCompact(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	k := runtime.Session.KSelector

	var symbol string
	switch {
	case k > 0:
		symbol = "→" // Toward God (tov)
	case k < 0:
		symbol = "←" // Toward self (shavar)
	default:
		symbol = "◆" // Centered (yashar)
	}

	return New(symbol, 7)
}

// BuildMoralFull returns detailed moral state
//
// Format: k→0.8 (tamim/SOUND) - direction + alignment + Hebrew state
func BuildMoralFull(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	k := runtime.Session.KSelector
	align := runtime.Session.KAlign
	hebrewState := runtime.Session.HebrewState
	hebrewMeaning := runtime.Session.HebrewMeaning

	// Direction symbol based on k selector
	var dirSymbol string
	switch {
	case k > 0:
		dirSymbol = "→" // Toward God (tov)
	case k < 0:
		dirSymbol = "←" // Toward self (shavar)
	default:
		dirSymbol = "◆" // Centered (yashar)
	}

	// Full format: k→0.8 (tamim/SOUND)
	content := fmt.Sprintf("k%s%.1f (%s/%s)", dirSymbol, align, hebrewState, hebrewMeaning)

	return New(content, 7)
}

// BuildMoralPattern returns the k frequency pattern
//
// Format: →3/←1 (3 choices toward God, 1 toward self)
// "By their fruits ye shall know them" (Matthew 7:16)
func BuildMoralPattern(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	towardGod := runtime.Session.KTowardGod
	towardSelf := runtime.Session.KTowardSelf

	// Skip if no pattern yet
	if towardGod == 0 && towardSelf == 0 {
		return Empty()
	}

	// Color based on dominant pattern
	var color string
	if towardGod > towardSelf {
		color = display.Green // Pattern toward God
	} else if towardSelf > towardGod {
		color = display.Yellow // Pattern toward self (needs attention)
	} else {
		color = display.Dim // Balanced
	}

	// Format: →3/←1
	content := fmt.Sprintf("%s→%d/←%d%s", color, towardGod, towardSelf, display.Reset)

	return New(content, 7)
}

// ============================================================================
// CLOSING
// ============================================================================
// Related: hebrew.go (K:SCRIPTURE references, K:MORAL evaluation)
