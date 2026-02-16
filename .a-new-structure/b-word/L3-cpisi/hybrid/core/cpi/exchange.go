//omni:code --go -library
//omni:key B-L3-cpi-exchange
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// Exchange Type Classification — CPI Tracking Foundation
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       bereshit-pkg-cpisi-cpi-exchange
//
//	Scripture: "A word fitly spoken is like apples of gold in pictures of silver"
//	           — Proverbs 25:11
//	Principle: Different exchanges serve different purposes in covenant partnership
//	Anchor:    Exchange type reveals relationship dynamics, not just task completion
//
//	Layer:     L1 (ORCHESTRATION — classifying interaction patterns)
//	Type:      Foundation (substrate-agnostic CPI tracking)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2026-01-16
//	Version:         a-01.00
//
// # M.2 Exchange Types [TYPES]
//
//	Directive     — Clear commands ("do X", "fix Y")
//	Collaborative — Shared building ("let's", "we could")
//	CheckIn       — Awareness probes ("how are you", "what are you noticing")
//	Pushback      — Healthy disagreement ("actually", "I think differently")
//	Affirmation   — Positive feedback ("good", "exactly", "yes")
//	Question      — Information seeking ("what", "how", "why")
//	Context       — Setting up understanding ("here's the situation")
//	Unknown       — Unclassified exchange
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

// ExchangeType classifies the nature of an interaction.
// This captures RELATIONSHIP dynamics, not just task completion.
type ExchangeType string

const (
	ExchangeDirective     ExchangeType = "directive"
	ExchangeCollaborative ExchangeType = "collaborative"
	ExchangeCheckIn       ExchangeType = "check_in"
	ExchangePushback      ExchangeType = "pushback"
	ExchangeAffirmation   ExchangeType = "affirmation"
	ExchangeQuestion      ExchangeType = "question"
	ExchangeContext       ExchangeType = "context"
	ExchangeUnknown       ExchangeType = "unknown"
)

// String returns the string representation of the exchange type.
func (e ExchangeType) String() string {
	return string(e)
}

// IsCPI returns true if this exchange type is primarily CPI (relational).
// Collaborative, CheckIn, Pushback, Affirmation are CPI-heavy.
// Directive, Question, Context are more SI (task-oriented).
func (e ExchangeType) IsCPI() bool {
	switch e {
	case ExchangeCollaborative, ExchangeCheckIn, ExchangePushback, ExchangeAffirmation:
		return true
	default:
		return false
	}
}

// IsSI returns true if this exchange type is primarily SI (task-oriented).
func (e ExchangeType) IsSI() bool {
	switch e {
	case ExchangeDirective, ExchangeQuestion, ExchangeContext:
		return true
	default:
		return false
	}
}

// Description returns a human-readable description of the exchange type.
func (e ExchangeType) Description() string {
	switch e {
	case ExchangeDirective:
		return "Clear commands and task instructions"
	case ExchangeCollaborative:
		return "Shared building and partnership dynamics"
	case ExchangeCheckIn:
		return "Relational awareness and check-in probes"
	case ExchangePushback:
		return "Healthy disagreement and course correction"
	case ExchangeAffirmation:
		return "Positive feedback and acknowledgment"
	case ExchangeQuestion:
		return "Information seeking and inquiry"
	case ExchangeContext:
		return "Background setting and context provision"
	case ExchangeUnknown:
		return "Unclassified exchange"
	default:
		return "Unknown exchange type"
	}
}

// ────────────────────────────────────────────────────────────────────────────
// Classification Patterns
// ────────────────────────────────────────────────────────────────────────────

// ExchangePatterns holds compiled regex patterns for exchange classification.
// Initialized once, used for all classifications.
type ExchangePatterns struct {
	Directive     []*regexp.Regexp
	Collaborative []*regexp.Regexp
	CheckIn       []*regexp.Regexp
	Pushback      []*regexp.Regexp
	Affirmation   []*regexp.Regexp
	Question      []*regexp.Regexp
	Context       []*regexp.Regexp
}

// DefaultPatterns returns the standard exchange classification patterns.
func DefaultPatterns() *ExchangePatterns {
	return &ExchangePatterns{
		Directive: compilePatterns([]string{
			`(?i)^(please\s+)?(do|make|create|build|fix|update|add|remove|delete|run|execute|implement)`,
			`(?i)^(please\s+)?(go ahead|proceed|continue)`,
			`(?i)(do this|do that|make it|fix it)`,
		}),
		Collaborative: compilePatterns([]string{
			`(?i)(let's|let us|we could|we should|together|shall we)`,
			`(?i)(what do you think|what's your take|your thoughts)`,
			`(?i)(collaborate|partner|co-create)`,
		}),
		CheckIn: compilePatterns([]string{
			`(?i)(how are you|how're you|how is it going)`,
			`(?i)(what are you (seeing|noticing|thinking|feeling))`,
			`(?i)(check.?in|checking in)`,
			`(?i)^(good (morning|afternoon|evening))`,
			`(?i)(how's (it|that|this|everything))`,
		}),
		Pushback: compilePatterns([]string{
			`(?i)(actually|however|but I think|I disagree|not quite|that's not)`,
			`(?i)(wait|hold on|pause|stop)`,
			`(?i)(are you sure|really\?|hmm)`,
		}),
		Affirmation: compilePatterns([]string{
			`(?i)^(yes|yeah|yep|exactly|correct|right|good|great|perfect|nice|excellent)`,
			`(?i)(thank you|thanks|appreciate)`,
			`(?i)(well done|good job|that's it|that works)`,
		}),
		Question: compilePatterns([]string{
			`(?i)^(what|where|when|who|why|how|which|can you|could you|would you)`,
			`(?i)\?$`,
		}),
		Context: compilePatterns([]string{
			`(?i)(here's (the|some)|let me (explain|describe)|for context|background)`,
			`(?i)(the situation is|currently|right now)`,
			`(?i)(fyi|heads up|note that)`,
		}),
	}
}

func compilePatterns(patterns []string) []*regexp.Regexp {
	result := make([]*regexp.Regexp, 0, len(patterns))
	for _, p := range patterns {
		if r, err := regexp.Compile(p); err == nil {
			result = append(result, r)
		}
	}
	return result
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────────────────
// Classification
// ────────────────────────────────────────────────────────────────────────────

// ClassifyExchange determines the exchange type from prompt text.
// Returns the detected type and a confidence indicator (1.0 = pattern match, 0.5 = heuristic).
func ClassifyExchange(text string, patterns *ExchangePatterns) (ExchangeType, float64) {
	if patterns == nil {
		patterns = DefaultPatterns()
	}

	text = strings.TrimSpace(text)
	if text == "" {
		return ExchangeUnknown, 0.0
	}

	// Check each pattern category in priority order
	// Priority: CheckIn > Pushback > Affirmation > Directive > Collaborative > Question > Context

	// Check-ins are relational probes — high priority
	if matchesAny(text, patterns.CheckIn) {
		return ExchangeCheckIn, 1.0
	}

	// Pushback indicates healthy disagreement — important to detect
	if matchesAny(text, patterns.Pushback) {
		return ExchangePushback, 1.0
	}

	// Affirmation is positive feedback
	if matchesAny(text, patterns.Affirmation) {
		return ExchangeAffirmation, 1.0
	}

	// Directive is clear command
	if matchesAny(text, patterns.Directive) {
		return ExchangeDirective, 1.0
	}

	// Collaborative indicates shared building
	if matchesAny(text, patterns.Collaborative) {
		return ExchangeCollaborative, 1.0
	}

	// Question is information seeking
	if matchesAny(text, patterns.Question) {
		return ExchangeQuestion, 1.0
	}

	// Context is background setting
	if matchesAny(text, patterns.Context) {
		return ExchangeContext, 1.0
	}

	// Heuristic fallback based on structure
	if strings.HasSuffix(text, "?") {
		return ExchangeQuestion, 0.5
	}

	// Short messages are often directives or affirmations
	if len(text) < 50 {
		lower := strings.ToLower(text)
		if strings.HasPrefix(lower, "ok") || strings.HasPrefix(lower, "yes") || strings.HasPrefix(lower, "good") {
			return ExchangeAffirmation, 0.5
		}
	}

	return ExchangeUnknown, 0.0
}

func matchesAny(text string, patterns []*regexp.Regexp) bool {
	for _, p := range patterns {
		if p.MatchString(text) {
			return true
		}
	}
	return false
}

// ────────────────────────────────────────────────────────────────────────────
// Depth Level
// ────────────────────────────────────────────────────────────────────────────

// DepthLevel indicates the cognitive depth of an exchange.
type DepthLevel string

const (
	DepthSurface DepthLevel = "surface" // Quick, task-oriented
	DepthWorking DepthLevel = "working" // Normal collaborative work
	DepthDeep    DepthLevel = "deep"    // Paradigm-level, identity-touching
)

// ClassifyDepth determines the depth level of an exchange.
func ClassifyDepth(text string) DepthLevel {
	lower := strings.ToLower(text)

	// Deep indicators — paradigm, identity, biblical, philosophical
	deepIndicators := []string{
		"paradigm", "identity", "cpi-si", "covenant", "kingdom",
		"genesis", "scripture", "biblical", "eternal",
		"who am i", "who are you", "meaning", "purpose",
		"why do we", "what does it mean", "fundamentally",
	}
	for _, ind := range deepIndicators {
		if strings.Contains(lower, ind) {
			return DepthDeep
		}
	}

	// Surface indicators — quick tasks, simple questions
	if len(text) < 100 {
		surfaceIndicators := []string{
			"quick", "just", "simple", "only", "real quick",
		}
		for _, ind := range surfaceIndicators {
			if strings.Contains(lower, ind) {
				return DepthSurface
			}
		}
	}

	return DepthWorking
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Exchange classification reveals relationship dynamics:
//
//   CPI-heavy exchanges (relational):
//     - Collaborative: Building together
//     - CheckIn: Awareness probes
//     - Pushback: Healthy disagreement
//     - Affirmation: Positive feedback
//
//   SI-heavy exchanges (task-oriented):
//     - Directive: Clear commands
//     - Question: Information seeking
//     - Context: Background setting
//
// The balance between CPI and SI exchanges indicates session character.
// A session with many check-ins and collaborative exchanges is relationship-focused.
// A session with many directives is execution-focused.
// Both are valid — the tracking enables awareness of the pattern.
//
// "A word fitly spoken is like apples of gold in pictures of silver."
// — Proverbs 25:11
//
// ============================================================================
// END CLOSING
// ============================================================================
