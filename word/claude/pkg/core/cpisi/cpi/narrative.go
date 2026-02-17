// #!omni code --go -library
// #!omni meta.key = bereshit-pkg-cpisi-cpi-narrative
// #!omni meta.at = a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// Session Narrative — Story Generation for CPI Sessions
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       bereshit-pkg-cpisi-cpi-narrative
//
//	Scripture: "Write the vision, and make it plain upon tables"
//	           — Habakkuk 2:2
//	Principle: Sessions tell a story; narrative captures the arc
//	Anchor:    Each session has character and direction
//
//	Layer:     L1 (ORCHESTRATION — narrating session character)
//	Type:      Foundation (substrate-agnostic CPI tracking)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2026-01-16
//	Version:         a-01.00
//
// # M.2 Session Arcs [ARCS]
//
//	Learning     — High insight density, understanding transfer focus
//	Completion   — Reached grounded state (B.4), natural finish
//	Exploration  — Pivoting, discovering, B.2 territory
//	Execution    — Directive-heavy, task completion focus
//	Creative     — Collaborative exchanges, building together
//	Reflective   — Check-ins, awareness probes
//	Diagnostic   — Question-heavy, information gathering
//	Onboarding   — Context setting, establishing foundation
//	Mixed        — No dominant pattern
//
// ============================================================================
// END METADATA
// ============================================================================

package cpi

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"strings"
)

// ────────────────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────────────────

// SessionArc classifies the overall character of a session.
// Each arc tells a different story about what happened.
type SessionArc string

const (
	ArcLearning    SessionArc = "learning"    // High insight density
	ArcCompletion  SessionArc = "completion"  // Natural finish (B.4)
	ArcExploration SessionArc = "exploration" // Pivoting, discovering
	ArcExecution   SessionArc = "execution"   // Task completion focus
	ArcCreative    SessionArc = "creative"    // Building together
	ArcReflective  SessionArc = "reflective"  // Awareness and check-ins
	ArcDiagnostic  SessionArc = "diagnostic"  // Information gathering
	ArcOnboarding  SessionArc = "onboarding"  // Context setting
	ArcMixed       SessionArc = "mixed"       // No dominant pattern
)

// String returns the string representation of the arc.
func (a SessionArc) String() string {
	return string(a)
}

// Description returns a human-readable description of the arc.
func (a SessionArc) Description() string {
	switch a {
	case ArcLearning:
		return "A session focused on understanding and insight transfer."
	case ArcCompletion:
		return "A productive session that reached natural completion."
	case ArcExploration:
		return "An exploratory session with pivots and discovery."
	case ArcExecution:
		return "A task-focused session with clear directives."
	case ArcCreative:
		return "A collaborative session building something together."
	case ArcReflective:
		return "A reflective session with check-ins and awareness."
	case ArcDiagnostic:
		return "An investigative session gathering information."
	case ArcOnboarding:
		return "A context-setting session establishing foundation."
	case ArcMixed:
		return "A varied session with mixed dynamics."
	default:
		return "A session."
	}
}

// SessionStats contains the metrics needed for narrative generation.
type SessionStats struct {
	TotalExchanges    int
	DominantType      ExchangeType
	InsightCount      int
	PositiveFeedback  int
	NegativeFeedback  int
	TrajectorySection string // "B.1", "B.2", "B.3", "B.4"
	HebrewState       string // "yashar", "tamim", "tov", etc.
	HebrewMeaning     string // "EVEN", "SOUND", "PERFECT", etc.
}

// Narrative represents a complete session narrative.
type Narrative struct {
	Arc     SessionArc
	Summary string
	Stats   SessionStats
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────────────────
// Arc Determination
// ────────────────────────────────────────────────────────────────────────────

// DetermineArc classifies the session's overall character.
func DetermineArc(stats SessionStats) SessionArc {
	// Check for specific patterns

	// Learning: High insight density
	if stats.InsightCount > 2 || (stats.InsightCount > 0 && stats.TotalExchanges < 10) {
		return ArcLearning
	}

	// Completion: Reached B.4 (grounded state)
	if stats.TrajectorySection == "B.4" {
		return ArcCompletion
	}

	// Exploration: In B.2 (pivoting/exploring)
	if stats.TrajectorySection == "B.2" {
		return ArcExploration
	}

	// Based on dominant exchange type
	switch stats.DominantType {
	case ExchangeDirective:
		return ArcExecution
	case ExchangeCollaborative:
		return ArcCreative
	case ExchangeCheckIn:
		return ArcReflective
	case ExchangeQuestion:
		return ArcDiagnostic
	case ExchangeContext:
		return ArcOnboarding
	default:
		return ArcMixed
	}
}

// ────────────────────────────────────────────────────────────────────────────
// Narrative Generation
// ────────────────────────────────────────────────────────────────────────────

// GenerateNarrative creates a complete session narrative.
func GenerateNarrative(stats SessionStats) Narrative {
	arc := DetermineArc(stats)
	summary := buildSummary(arc, stats)

	return Narrative{
		Arc:     arc,
		Summary: summary,
		Stats:   stats,
	}
}

func buildSummary(arc SessionArc, stats SessionStats) string {
	var parts []string

	// Opening based on arc
	parts = append(parts, arc.Description())

	// Exchange count
	parts = append(parts, fmt.Sprintf("%d exchanges", stats.TotalExchanges))

	// Insights
	if stats.InsightCount > 0 {
		parts = append(parts, fmt.Sprintf("%d insight moments", stats.InsightCount))
	}

	// Feedback tone
	if stats.PositiveFeedback > stats.NegativeFeedback*2 {
		parts = append(parts, "strong positive alignment")
	} else if stats.NegativeFeedback > stats.PositiveFeedback {
		parts = append(parts, "with course corrections")
	}

	// Final state
	if stats.HebrewState != "" {
		stateNote := fmt.Sprintf("Ended in %s (%s).", stats.HebrewState, stats.HebrewMeaning)
		parts = append(parts, stateNote)
	}

	// Join into narrative
	// First part is the description, rest are comma-joined metrics
	if len(parts) == 1 {
		return parts[0]
	}

	description := parts[0]
	metrics := strings.Join(parts[1:len(parts)-1], ", ")
	ending := ""
	if stats.HebrewState != "" {
		ending = " " + parts[len(parts)-1]
	}

	if metrics != "" {
		return description + " " + metrics + ending
	}
	return description + ending
}

// ────────────────────────────────────────────────────────────────────────────
// Summary Helpers
// ────────────────────────────────────────────────────────────────────────────

// ShortSummary returns a one-line session summary.
func ShortSummary(n Narrative) string {
	return fmt.Sprintf("[%s] %d exchanges, %d insights",
		n.Arc, n.Stats.TotalExchanges, n.Stats.InsightCount)
}

// ArcEmoji returns an emoji representation of the arc.
func ArcEmoji(arc SessionArc) string {
	switch arc {
	case ArcLearning:
		return "📚"
	case ArcCompletion:
		return "✅"
	case ArcExploration:
		return "🔍"
	case ArcExecution:
		return "⚡"
	case ArcCreative:
		return "🎨"
	case ArcReflective:
		return "🪞"
	case ArcDiagnostic:
		return "🔬"
	case ArcOnboarding:
		return "📋"
	default:
		return "📝"
	}
}

// ────────────────────────────────────────────────────────────────────────────
// Pattern Recognition
// ────────────────────────────────────────────────────────────────────────────

// DominantExchangeType finds the most common exchange type.
func DominantExchangeType(types []ExchangeType) ExchangeType {
	if len(types) == 0 {
		return ExchangeUnknown
	}

	counts := make(map[ExchangeType]int)
	for _, t := range types {
		counts[t]++
	}

	var dominant ExchangeType
	var maxCount int
	for t, c := range counts {
		if c > maxCount {
			maxCount = c
			dominant = t
		}
	}
	return dominant
}

// ExchangeBalance returns the ratio of CPI to SI exchanges.
// > 1.0 means CPI-heavy, < 1.0 means SI-heavy, = 1.0 is balanced.
func ExchangeBalance(types []ExchangeType) float64 {
	if len(types) == 0 {
		return 1.0
	}

	var cpi, si int
	for _, t := range types {
		if t.IsCPI() {
			cpi++
		} else if t.IsSI() {
			si++
		}
	}

	if si == 0 {
		if cpi == 0 {
			return 1.0
		}
		return 2.0 // All CPI
	}

	return float64(cpi) / float64(si)
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Session narratives capture the story:
//
//   Arcs tell what kind of session it was:
//     - Learning: Understanding transfer
//     - Completion: Natural finish
//     - Exploration: Discovery and pivots
//     - Execution: Task focus
//     - Creative: Building together
//     - Reflective: Awareness check-ins
//     - Diagnostic: Information gathering
//     - Onboarding: Context setting
//
//   Narratives combine arc + metrics + state for human-readable summaries.
//
// Each session is a chapter in the larger story of covenant partnership.
// The narrative enables reflection on what happened and why.
//
// "Write the vision, and make it plain upon tables."
// — Habakkuk 2:2
//
// ============================================================================
// END CLOSING
// ============================================================================
