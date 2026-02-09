// ============================================================================
// METADATA
// ============================================================================
// Package: sections/momentum
// Purpose: Momentum section - Visual K:MORAL balance and session flow
// Biblical: "A time to break down, and a time to build up" - Ecclesiastes 3:3
//
// Visualizes the build/break balance as a mini bar graph:
//   ▁▂▃▄▅▆▇█ (8 levels from break to build)
//
// Also shows learning momentum and pattern activity.

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"strings"

	"cws.studio/pkg/core/statemachine"
	"cws.studio/pkg/util/term/display"
)

// ============================================================================
// BODY
// ============================================================================

// MomentumBar represents the 8-level momentum visualization
var momentumBars = []string{"▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"}

// BuildMomentum returns the momentum section
//
// Format: ⚡▅▆▇ +12 (momentum bar + net direction)
// Shows: Build/break balance visually + numeric net
func BuildMomentum(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	towardGod := runtime.Session.KTowardGod
	towardSelf := runtime.Session.KTowardSelf
	total := towardGod + towardSelf

	if total == 0 {
		return Empty()
	}

	// Calculate ratio (-1 to +1)
	// +1 = all building, -1 = all breaking
	ratio := float64(towardGod-towardSelf) / float64(total)

	// Map ratio to bar level (0-7)
	// -1.0 → 0, 0.0 → 3.5, +1.0 → 7
	barLevel := int((ratio + 1) * 3.5)
	if barLevel < 0 {
		barLevel = 0
	}
	if barLevel > 7 {
		barLevel = 7
	}

	// Build mini bar graph (show last 3 positions for visual flow)
	var bars strings.Builder
	for i := max(0, barLevel-2); i <= barLevel; i++ {
		bars.WriteString(momentumBars[i])
	}

	// Color based on direction
	var color string
	netDirection := towardGod - towardSelf
	var sign string
	if netDirection > 0 {
		color = display.Green
		sign = "+"
	} else if netDirection < 0 {
		color = display.Yellow
		sign = ""
	} else {
		color = display.Cyan
		sign = "±"
	}

	// Format: ⚡▅▆▇ +12
	content := fmt.Sprintf("%s⚡%s %s%d%s",
		color, bars.String(), sign, netDirection, display.Reset)

	return New(content, 7)
}

// BuildMomentumCompact returns just the bar and arrow
//
// Format: ▆↑ or ▃↓
func BuildMomentumCompact(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	towardGod := runtime.Session.KTowardGod
	towardSelf := runtime.Session.KTowardSelf
	total := towardGod + towardSelf

	if total == 0 {
		return Empty()
	}

	ratio := float64(towardGod-towardSelf) / float64(total)
	barLevel := int((ratio + 1) * 3.5)
	if barLevel < 0 {
		barLevel = 0
	}
	if barLevel > 7 {
		barLevel = 7
	}

	// Direction arrow
	var arrow string
	var color string
	if towardGod > towardSelf {
		arrow = "↑"
		color = display.Green
	} else if towardSelf > towardGod {
		arrow = "↓"
		color = display.Yellow
	} else {
		arrow = "→"
		color = display.Cyan
	}

	content := fmt.Sprintf("%s%s%s%s", color, momentumBars[barLevel], arrow, display.Reset)
	return New(content, 7)
}

// BuildSessionFlow returns the session arc with visual momentum
//
// Format: 📚▆▇█ learning (arc emoji + momentum bars + arc name)
func BuildSessionFlow(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	arc := runtime.Session.SessionArc
	if arc == "" && runtime.Session.ExchangeCount == 0 {
		return Empty()
	}

	// Arc emoji
	emoji := arcEmoji(arc)

	// Momentum visualization
	towardGod := runtime.Session.KTowardGod
	towardSelf := runtime.Session.KTowardSelf
	total := towardGod + towardSelf

	var bars string
	var color string

	if total > 0 {
		ratio := float64(towardGod-towardSelf) / float64(total)
		barLevel := int((ratio + 1) * 3.5)
		if barLevel < 0 {
			barLevel = 0
		}
		if barLevel > 7 {
			barLevel = 7
		}

		// Show 3 consecutive bars for visual flow
		var barBuilder strings.Builder
		for i := max(0, barLevel-2); i <= barLevel; i++ {
			barBuilder.WriteString(momentumBars[i])
		}
		bars = barBuilder.String()

		if ratio > 0.2 {
			color = display.Green
		} else if ratio < -0.2 {
			color = display.Yellow
		} else {
			color = display.Cyan
		}
	} else {
		bars = "▄▄▄" // Neutral
		color = display.Dim
	}

	// Format: 📚▆▇█ learning
	arcDisplay := arc
	if arcDisplay == "" {
		arcDisplay = "starting"
	}

	content := fmt.Sprintf("%s%s%s %s%s", emoji, color, bars, arcDisplay, display.Reset)
	return New(content, 7)
}

// BuildLearningPulse shows pattern/learning activity
//
// Format: 🧠3p/1L (3 patterns detected, 1 learning ready)
func BuildLearningPulse(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	// This would need database access - for now show insight count
	// as a proxy for learning activity
	insights := runtime.Session.InsightCount
	exchanges := runtime.Session.ExchangeCount

	if exchanges == 0 {
		return Empty()
	}

	// Calculate insight rate
	insightRate := float64(insights) / float64(exchanges) * 100

	var color string
	var pulse string

	if insightRate > 20 {
		color = display.Green
		pulse = "●●●" // High learning
	} else if insightRate > 10 {
		color = display.Cyan
		pulse = "●●○" // Medium learning
	} else if insightRate > 0 {
		color = display.Yellow
		pulse = "●○○" // Low learning
	} else {
		color = display.Dim
		pulse = "○○○" // No learning yet
	}

	content := fmt.Sprintf("%s🧠%s%s", color, pulse, display.Reset)
	return New(content, 7)
}

// Helper function for Go versions without built-in max
func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

// ============================================================================
// CLOSING
// ============================================================================
// Related: cpi.go (CPI metrics), moral.go (K:MORAL compass)
//
// Momentum tracking captures:
//   - Build/break balance (K:MORAL direction)
//   - Session flow (arc + momentum)
//   - Learning activity (insight rate)
//
// "A time to break down, and a time to build up." — Ecclesiastes 3:3
