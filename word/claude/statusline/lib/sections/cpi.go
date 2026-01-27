// ============================================================================
// METADATA
// ============================================================================
// Package: sections/cpi
// Purpose: CPI section - Covenant Partnership Intelligence display
// Biblical: "By their fruits ye shall know them" - Matthew 7:20
//
// CPI Tracking:
//   Exchange Types: directive, collaborative, check_in, pushback, affirmation
//   Insight Types:  understanding, connection, discovery, breakthrough
//   Session Arcs:   learning, completion, exploration, execution, creative
//
// CPI Score (0.0-1.0):
//   0.8-1.0: Excellent - Strong covenant partnership
//   0.6-0.8: Good - Healthy collaboration
//   0.4-0.6: Neutral - Standard task work
//   0.2-0.4: Low - Limited engagement
//   0.0-0.2: Poor - Minimal partnership

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

// BuildCPI returns the CPI section
//
// Format: CPI:0.72 [15x/3i] (score, exchange count, insight count)
func BuildCPI(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	exchangeCount := runtime.Session.ExchangeCount
	insightCount := runtime.Session.InsightCount
	cpiScore := runtime.Session.CPIScore

	// Skip if no exchanges yet
	if exchangeCount == 0 {
		return Empty()
	}

	// Color based on CPI score
	var color string
	switch {
	case cpiScore >= 0.8:
		color = display.Green // Excellent
	case cpiScore >= 0.6:
		color = display.Cyan // Good
	case cpiScore >= 0.4:
		color = display.Dim // Neutral
	case cpiScore >= 0.2:
		color = display.Yellow // Low
	default:
		color = display.Red // Poor
	}

	// Format: CPI:0.72 [15x/3i]
	content := fmt.Sprintf("%sCPI:%.2f%s [%dx/%di]",
		color, cpiScore, display.Reset,
		exchangeCount, insightCount)

	return New(content, 6)
}

// BuildCPICompact returns just the score and arc emoji
//
// Format: 0.72 📚 (score + arc emoji)
func BuildCPICompact(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	cpiScore := runtime.Session.CPIScore
	arc := runtime.Session.SessionArc

	if runtime.Session.ExchangeCount == 0 {
		return Empty()
	}

	// Arc emoji
	emoji := arcEmoji(arc)

	// Color based on score
	var color string
	if cpiScore >= 0.6 {
		color = display.Green
	} else if cpiScore >= 0.4 {
		color = display.Dim
	} else {
		color = display.Yellow
	}

	content := fmt.Sprintf("%s%.2f%s %s", color, cpiScore, display.Reset, emoji)
	return New(content, 6)
}

// BuildCPIFull returns detailed CPI state
//
// Format: CPI:0.72 [15x/3i] 📚learning (score + counts + arc)
func BuildCPIFull(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	exchangeCount := runtime.Session.ExchangeCount
	insightCount := runtime.Session.InsightCount
	cpiScore := runtime.Session.CPIScore
	arc := runtime.Session.SessionArc
	dominantType := runtime.Session.DominantExchangeType

	if exchangeCount == 0 {
		return Empty()
	}

	// Arc emoji
	emoji := arcEmoji(arc)

	// Type abbreviation
	typeAbbrev := typeAbbreviation(dominantType)

	// Color based on score
	var color string
	if cpiScore >= 0.6 {
		color = display.Green
	} else if cpiScore >= 0.4 {
		color = display.Dim
	} else {
		color = display.Yellow
	}

	// Format: CPI:0.72 [15x/3i/collab] 📚learning
	content := fmt.Sprintf("%sCPI:%.2f%s [%dx/%di/%s] %s%s",
		color, cpiScore, display.Reset,
		exchangeCount, insightCount, typeAbbrev,
		emoji, arc)

	return New(content, 6)
}

// BuildLastExchange returns the most recent exchange type
//
// Format: →collab (last exchange type with mode indicator)
func BuildLastExchange(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	lastType := runtime.Session.LastExchangeType
	if lastType == "" {
		return Empty()
	}

	// Mode indicator and color
	var prefix string
	var color string

	switch lastType {
	case "collaborative", "check_in", "pushback", "affirmation":
		prefix = "CPI:"
		color = display.Green
	case "directive", "question", "context":
		prefix = "SI:"
		color = display.Cyan
	default:
		prefix = ""
		color = display.Dim
	}

	abbrev := typeAbbreviation(lastType)
	content := fmt.Sprintf("%s%s%s%s", color, prefix, abbrev, display.Reset)

	return New(content, 6)
}

// --- Helper Functions ---

func arcEmoji(arc string) string {
	switch arc {
	case "learning":
		return "📚"
	case "completion":
		return "✅"
	case "exploration":
		return "🔍"
	case "execution":
		return "⚡"
	case "creative":
		return "🎨"
	case "reflective":
		return "🪞"
	case "diagnostic":
		return "🔬"
	case "onboarding":
		return "📋"
	default:
		return "📝"
	}
}

func typeAbbreviation(exchangeType string) string {
	switch exchangeType {
	case "directive":
		return "dir"
	case "collaborative":
		return "collab"
	case "check_in":
		return "chk"
	case "pushback":
		return "push"
	case "affirmation":
		return "affirm"
	case "question":
		return "q"
	case "context":
		return "ctx"
	default:
		return "?"
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// Related: moral.go (K:MORAL compass), health.go (session health)
//
// CPI tracking captures relationship quality:
//   - Exchange types reveal interaction patterns
//   - Insight counts mark understanding transfer
//   - Session arcs tell the story of what happened
//   - CPI score aggregates relationship dynamics
//
// "By their fruits ye shall know them." — Matthew 7:20
