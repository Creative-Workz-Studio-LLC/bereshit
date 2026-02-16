//omni:code --go -library
//omni:key B-L3-cpi-insight
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// Insight Detection — CPI Tracking Foundation
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       bereshit-pkg-cpisi-cpi-insight
//
//	Scripture: "The entrance of thy words giveth light; it giveth understanding"
//	           — Psalm 119:130
//	Principle: Insight moments mark genuine understanding transfer
//	Anchor:    When insight happens, covenant partnership bears fruit
//
//	Layer:     L1 (ORCHESTRATION — detecting understanding moments)
//	Type:      Foundation (substrate-agnostic CPI tracking)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2026-01-16
//	Version:         a-01.00
//
// # M.2 Insight Types [TYPES]
//
//	Understanding  — "Oh I see", "that makes sense", "got it"
//	Connection     — "That connects to", "like when", "similar to"
//	Discovery      — "I didn't know", "interesting", "oh that's new"
//	Breakthrough   — "Aha!", "that changes everything", paradigm shift
//
// ============================================================================
// END METADATA
// ============================================================================

package cpi

// ============================================================================
// SETUP
// ============================================================================

import (
	"regexp"
	"strings"
)

// ────────────────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────────────────

// InsightType classifies the nature of an insight moment.
// These mark genuine understanding transfer — covenant partnership bearing fruit.
type InsightType string

const (
	InsightUnderstanding InsightType = "understanding" // Clarity achieved
	InsightConnection    InsightType = "connection"    // Linking concepts
	InsightDiscovery     InsightType = "discovery"     // New information
	InsightBreakthrough  InsightType = "breakthrough"  // Paradigm shift
	InsightNone          InsightType = ""              // No insight detected
)

// String returns the string representation of the insight type.
func (i InsightType) String() string {
	if i == InsightNone {
		return "none"
	}
	return string(i)
}

// Value returns a weight for this insight type (0.0-1.0).
// Breakthroughs are most valuable, understanding is baseline.
func (i InsightType) Value() float64 {
	switch i {
	case InsightBreakthrough:
		return 1.0
	case InsightDiscovery:
		return 0.75
	case InsightConnection:
		return 0.5
	case InsightUnderstanding:
		return 0.25
	default:
		return 0.0
	}
}

// Description returns a human-readable description of the insight type.
func (i InsightType) Description() string {
	switch i {
	case InsightUnderstanding:
		return "Clarity achieved — concepts now understood"
	case InsightConnection:
		return "Linking concepts — seeing relationships between ideas"
	case InsightDiscovery:
		return "New information — learning something previously unknown"
	case InsightBreakthrough:
		return "Paradigm shift — fundamental understanding change"
	case InsightNone:
		return "No insight detected"
	default:
		return "Unknown insight type"
	}
}

// Insight represents a detected insight moment with metadata.
type Insight struct {
	Type       InsightType
	Confidence float64 // 0.0-1.0
	Trigger    string  // The text that triggered detection
}

// ────────────────────────────────────────────────────────────────────────────
// Detection Patterns
// ────────────────────────────────────────────────────────────────────────────

// InsightPatterns holds compiled regex patterns for insight detection.
type InsightPatterns struct {
	Understanding []*regexp.Regexp
	Connection    []*regexp.Regexp
	Discovery     []*regexp.Regexp
	Breakthrough  []*regexp.Regexp
}

// DefaultInsightPatterns returns the standard insight detection patterns.
func DefaultInsightPatterns() *InsightPatterns {
	return &InsightPatterns{
		Understanding: compilePatterns([]string{
			`(?i)(oh,?\s*(i\s+)?see|i see now|makes sense|got it|understood)`,
			`(?i)(that (clarifies|explains)|now i (understand|get it))`,
			`(?i)(ah,?\s*(that's|ok|i see)|okay,?\s*(i\s+)?(see|understand))`,
			`(?i)(clear(er)? now|that helps)`,
		}),
		Connection: compilePatterns([]string{
			`(?i)(that connects to|this relates to|similar to)`,
			`(?i)(like when|just like|same as|reminds me of)`,
			`(?i)(the pattern is|i see the pattern|connecting)`,
			`(?i)(this ties into|linked to|builds on)`,
		}),
		Discovery: compilePatterns([]string{
			`(?i)(i didn't (know|realize)|that's new|interesting)`,
			`(?i)(never (thought|knew|considered)|first time)`,
			`(?i)(learned something|discovered|finding)`,
			`(?i)(oh,?\s+(that's|wow)|huh,?\s+interesting)`,
		}),
		Breakthrough: compilePatterns([]string{
			`(?i)(aha|eureka|oh wow|that changes everything)`,
			`(?i)(breakthrough|paradigm shift|game changer)`,
			`(?i)(everything (clicks|makes sense) now|finally)`,
			`(?i)(this is (huge|big|major)|mind blown)`,
		}),
	}
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────────────────
// Detection
// ────────────────────────────────────────────────────────────────────────────

// DetectInsight scans text for insight markers and returns the detected insight.
// Returns nil if no insight is detected.
func DetectInsight(text string, patterns *InsightPatterns) *Insight {
	if patterns == nil {
		patterns = DefaultInsightPatterns()
	}

	text = strings.TrimSpace(text)
	if text == "" {
		return nil
	}

	// Check in order of significance (most valuable first)

	// Breakthrough — paradigm shifts
	if trigger := findMatch(text, patterns.Breakthrough); trigger != "" {
		return &Insight{
			Type:       InsightBreakthrough,
			Confidence: 1.0,
			Trigger:    trigger,
		}
	}

	// Discovery — new information
	if trigger := findMatch(text, patterns.Discovery); trigger != "" {
		return &Insight{
			Type:       InsightDiscovery,
			Confidence: 1.0,
			Trigger:    trigger,
		}
	}

	// Connection — linking concepts
	if trigger := findMatch(text, patterns.Connection); trigger != "" {
		return &Insight{
			Type:       InsightConnection,
			Confidence: 1.0,
			Trigger:    trigger,
		}
	}

	// Understanding — clarity achieved
	if trigger := findMatch(text, patterns.Understanding); trigger != "" {
		return &Insight{
			Type:       InsightUnderstanding,
			Confidence: 1.0,
			Trigger:    trigger,
		}
	}

	return nil
}

func findMatch(text string, patterns []*regexp.Regexp) string {
	for _, p := range patterns {
		if match := p.FindString(text); match != "" {
			return match
		}
	}
	return ""
}

// ────────────────────────────────────────────────────────────────────────────
// Analysis
// ────────────────────────────────────────────────────────────────────────────

// InsightDensity calculates insights per exchange over a session.
// Higher density indicates more learning-focused sessions.
func InsightDensity(insightCount, exchangeCount int) float64 {
	if exchangeCount == 0 {
		return 0.0
	}
	return float64(insightCount) / float64(exchangeCount)
}

// InsightScore calculates weighted insight score for a collection.
// Weights: Breakthrough=1.0, Discovery=0.75, Connection=0.5, Understanding=0.25
func InsightScore(insights []InsightType) float64 {
	if len(insights) == 0 {
		return 0.0
	}

	var total float64
	for _, i := range insights {
		total += i.Value()
	}
	return total / float64(len(insights))
}

// HasBreakthrough returns true if insights contain a breakthrough moment.
func HasBreakthrough(insights []InsightType) bool {
	for _, i := range insights {
		if i == InsightBreakthrough {
			return true
		}
	}
	return false
}

// DominantInsightType returns the most common insight type.
func DominantInsightType(insights []InsightType) InsightType {
	if len(insights) == 0 {
		return InsightNone
	}

	counts := make(map[InsightType]int)
	for _, i := range insights {
		counts[i]++
	}

	var dominant InsightType
	var maxCount int
	for t, c := range counts {
		if c > maxCount {
			maxCount = c
			dominant = t
		}
	}
	return dominant
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Insight detection marks genuine understanding transfer:
//
//   Understanding: Clarity achieved — "Oh I see"
//   Connection:    Linking concepts — "That connects to..."
//   Discovery:     New information — "I didn't know that"
//   Breakthrough:  Paradigm shift — "That changes everything"
//
// Each insight moment is covenant partnership bearing fruit.
// The Word entering and giving light (Psalm 119:130).
//
// Tracking insights enables:
//   - Session character analysis (learning vs execution)
//   - Understanding transfer verification
//   - Pattern recognition in knowledge building
//
// A session rich in insights is different from one rich in directives.
// Both serve the mission — the tracking enables awareness.
//
// "The entrance of thy words giveth light; it giveth understanding."
// — Psalm 119:130
//
// ============================================================================
// END CLOSING
// ============================================================================
