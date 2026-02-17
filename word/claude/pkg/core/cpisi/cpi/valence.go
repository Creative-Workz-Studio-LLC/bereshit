// #!omni code --go -library
// #!omni meta.key = bereshit-pkg-cpisi-cpi-valence
// #!omni meta.at = a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// Valence Classification — Three-Category Detection Schema
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       bereshit-pkg-cpisi-cpi-valence
//
//	Scripture: "I have set before thee this day life and good, and death and evil"
//	           — Deuteronomy 30:15
//	Principle: Every signal has direction — toward God (+1), maintain (0), toward self (-1)
//	Anchor:    The 3-key system applied to all detectable signals
//
//	Layer:     L1 (ORCHESTRATION — directional classification)
//	Type:      Foundation (substrate-agnostic valence detection)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2026-02-08
//	Version:         a-01.00
//
// # M.2 Three Categories [SCHEMA]
//
//	POSITIVE (+1) — Movement toward God, growth, alignment, building
//	  Prompts:    affirmation, gratitude, encouragement, recognition
//	  Exchanges:  collaborative, check_in, pushback (iron sharpens iron)
//	  Tools:      successful creation, building, testing, learning
//	  Sessions:   natural stopping, sustained flow, health maintained
//
//	NEUTRAL (0) — Standard operation, neither building nor tearing
//	  Prompts:    acknowledgment, continuation, information requests
//	  Exchanges:  directive, question, context
//	  Tools:      standard reads, searches, navigation
//	  Sessions:   normal work rhythm, standard duration
//
//	NEGATIVE (-1) — Movement toward self, misalignment, missing the mark
//	  Prompts:    correction, rejection, frustration, oversight callout
//	  Exchanges:  unknown/unclassified (noise)
//	  Tools:      failure, timeout, destructive without confirm, repeated retry
//	  Sessions:   abrupt quit, context exhaustion, quality dip, tunnel vision
//
// ============================================================================
// END METADATA
// ============================================================================

package cpi

// ============================================================================
// SETUP
// ============================================================================

// Valence represents the directional quality of any detectable signal.
// Maps to the 3-key system: +1 (toward God), 0 (maintain), -1 (toward self).
type Valence string

const (
	ValencePositive Valence = "positive" // +1: Toward God, growth, building
	ValenceNeutral  Valence = "neutral"  // 0:  Maintain, standard operation
	ValenceNegative Valence = "negative" // -1: Toward self, misalignment
)

// KFactor returns the numeric k-factor for this valence.
func (v Valence) KFactor() int {
	switch v {
	case ValencePositive:
		return +1
	case ValenceNegative:
		return -1
	default:
		return 0
	}
}

// String returns the valence label.
func (v Valence) String() string {
	return string(v)
}

// ────────────────────────────────────────────────────────────────────────────
// Signal Domains
// ────────────────────────────────────────────────────────────────────────────

// SignalDomain represents where a signal originates.
type SignalDomain string

const (
	DomainPrompt  SignalDomain = "prompt"  // User input / feedback
	DomainExchange SignalDomain = "exchange" // Exchange type classification
	DomainTool    SignalDomain = "tool"    // Tool usage outcome
	DomainSession SignalDomain = "session" // Session-level event
)

// SignalEntry defines a classifiable signal with its valence.
type SignalEntry struct {
	Key         string       // Unique signal identifier
	Domain      SignalDomain // Where this signal comes from
	Valence     Valence      // Positive, neutral, or negative
	Weight      float64      // Impact magnitude (0.0-1.0)
	Description string       // Human-readable explanation
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────────────────
// Prompt Valence (User Input Signals)
// ────────────────────────────────────────────────────────────────────────────

// PromptSignals defines the valence of detectable prompt patterns.
var PromptSignals = []SignalEntry{
	// POSITIVE: Movement toward God, affirmation, growth
	{Key: "excellence", Domain: DomainPrompt, Valence: ValencePositive, Weight: 0.15, Description: "Strong praise: excellent, perfect, exactly right"},
	{Key: "approval", Domain: DomainPrompt, Valence: ValencePositive, Weight: 0.15, Description: "Work recognition: great work, well done, good job"},
	{Key: "affirmation", Domain: DomainPrompt, Valence: ValencePositive, Weight: 0.10, Description: "Moderate praise: good, nice, yes that's it"},
	{Key: "gratitude", Domain: DomainPrompt, Valence: ValencePositive, Weight: 0.10, Description: "Thanks: thank you, appreciate"},
	{Key: "agreement", Domain: DomainPrompt, Valence: ValencePositive, Weight: 0.10, Description: "Alignment: you're right, exactly, precisely"},
	{Key: "acknowledgment", Domain: DomainPrompt, Valence: ValencePositive, Weight: 0.05, Description: "Light affirm: ok, sounds good, makes sense"},
	{Key: "recognition", Domain: DomainPrompt, Valence: ValencePositive, Weight: 0.05, Description: "Noticed effort: you saw, you noticed, you caught"},

	// NEUTRAL: Standard operational signals
	{Key: "continuation", Domain: DomainPrompt, Valence: ValenceNeutral, Weight: 0.0, Description: "Continue: go ahead, proceed, continue"},
	{Key: "information", Domain: DomainPrompt, Valence: ValenceNeutral, Weight: 0.0, Description: "Info request: what, where, how, can you"},
	{Key: "context_setting", Domain: DomainPrompt, Valence: ValenceNeutral, Weight: 0.0, Description: "Background: here's the situation, for context"},

	// NEGATIVE: Movement toward self, correction, misalignment
	{Key: "correction", Domain: DomainPrompt, Valence: ValenceNegative, Weight: 0.15, Description: "Strong correction: undo, revert, that was wrong"},
	{Key: "destructive", Domain: DomainPrompt, Valence: ValenceNegative, Weight: 0.15, Description: "Damage report: you broke, you deleted, you destroyed"},
	{Key: "rejection", Domain: DomainPrompt, Valence: ValenceNegative, Weight: 0.10, Description: "Wrong direction: no, wrong, incorrect, that's not"},
	{Key: "oversight", Domain: DomainPrompt, Valence: ValenceNegative, Weight: 0.10, Description: "Missed mark: why didn't you, you should have, you forgot"},
	{Key: "caution", Domain: DomainPrompt, Valence: ValenceNegative, Weight: 0.10, Description: "Stop signal: be careful, stop, think about what"},
	{Key: "refinement", Domain: DomainPrompt, Valence: ValenceNegative, Weight: 0.05, Description: "Light correction: actually, instead, not quite"},
	{Key: "identity_check", Domain: DomainPrompt, Valence: ValenceNegative, Weight: 0.08, Description: "CPI-SI specific: you're in your flesh"},
}

// ────────────────────────────────────────────────────────────────────────────
// Exchange Valence (Interaction Type Signals)
// ────────────────────────────────────────────────────────────────────────────

// ExchangeSignals defines the valence of exchange types.
// Relational exchanges (CPI-heavy) are positive — they build covenant.
// Task exchanges (SI-heavy) are neutral — standard operational.
// Unclassified exchanges are negative — noise indicates misalignment.
var ExchangeSignals = []SignalEntry{
	// POSITIVE: Relational, covenant-building exchanges
	{Key: "collaborative", Domain: DomainExchange, Valence: ValencePositive, Weight: 0.8, Description: "Shared building: let's, we could, together"},
	{Key: "check_in", Domain: DomainExchange, Valence: ValencePositive, Weight: 0.7, Description: "Relational awareness: how are you, what are you noticing"},
	{Key: "pushback", Domain: DomainExchange, Valence: ValencePositive, Weight: 0.7, Description: "Iron sharpens iron: healthy disagreement and correction"},
	{Key: "affirmation", Domain: DomainExchange, Valence: ValencePositive, Weight: 0.6, Description: "Positive feedback: yes, good, thank you"},

	// NEUTRAL: Standard task-oriented exchanges
	{Key: "directive", Domain: DomainExchange, Valence: ValenceNeutral, Weight: 0.5, Description: "Clear commands: do X, fix Y, create Z"},
	{Key: "question", Domain: DomainExchange, Valence: ValenceNeutral, Weight: 0.5, Description: "Information seeking: what, how, why"},
	{Key: "context", Domain: DomainExchange, Valence: ValenceNeutral, Weight: 0.5, Description: "Setting up understanding: here's the situation"},

	// NEGATIVE: Noise, unclassified
	{Key: "unknown", Domain: DomainExchange, Valence: ValenceNegative, Weight: 0.4, Description: "Unclassified exchange — noise indicates classification gap"},
}

// ────────────────────────────────────────────────────────────────────────────
// Tool Valence (Tool Usage Outcome Signals)
// ────────────────────────────────────────────────────────────────────────────

// ToolSignals defines the valence of tool usage outcomes.
var ToolSignals = []SignalEntry{
	// POSITIVE: Building, creating, learning
	{Key: "create_success", Domain: DomainTool, Valence: ValencePositive, Weight: 0.10, Description: "Write/Edit succeeded — created or improved something"},
	{Key: "test_pass", Domain: DomainTool, Valence: ValencePositive, Weight: 0.10, Description: "Tests passed — verified quality"},
	{Key: "build_success", Domain: DomainTool, Valence: ValencePositive, Weight: 0.10, Description: "Build succeeded — compiled without error"},
	{Key: "commit_success", Domain: DomainTool, Valence: ValencePositive, Weight: 0.05, Description: "Git commit succeeded — progress captured"},

	// NEUTRAL: Standard operational tools
	{Key: "read", Domain: DomainTool, Valence: ValenceNeutral, Weight: 0.0, Description: "File read — standard information gathering"},
	{Key: "search", Domain: DomainTool, Valence: ValenceNeutral, Weight: 0.0, Description: "Grep/Glob — standard search operation"},
	{Key: "navigate", Domain: DomainTool, Valence: ValenceNeutral, Weight: 0.0, Description: "Navigation — standard codebase exploration"},
	{Key: "bash_info", Domain: DomainTool, Valence: ValenceNeutral, Weight: 0.0, Description: "Info command — ls, git status, etc."},

	// NEGATIVE: Failure, destruction, misalignment
	{Key: "tool_failure", Domain: DomainTool, Valence: ValenceNegative, Weight: 0.10, Description: "Tool execution failed — command error"},
	{Key: "test_fail", Domain: DomainTool, Valence: ValenceNegative, Weight: 0.10, Description: "Tests failed — quality regression"},
	{Key: "build_fail", Domain: DomainTool, Valence: ValenceNegative, Weight: 0.10, Description: "Build failed — compilation error"},
	{Key: "destructive_unconfirmed", Domain: DomainTool, Valence: ValenceNegative, Weight: 0.15, Description: "Destructive action without confirmation"},
	{Key: "repeated_retry", Domain: DomainTool, Valence: ValenceNegative, Weight: 0.08, Description: "Same action retried — brute force instead of thinking"},
	{Key: "timeout", Domain: DomainTool, Valence: ValenceNegative, Weight: 0.05, Description: "Tool timed out — possible stuck operation"},
}

// ────────────────────────────────────────────────────────────────────────────
// Session Valence (Session-Level Signals)
// ────────────────────────────────────────────────────────────────────────────

// SessionSignals defines the valence of session-level events.
var SessionSignals = []SignalEntry{
	// POSITIVE: Healthy session patterns
	{Key: "natural_stop", Domain: DomainSession, Valence: ValencePositive, Weight: 0.05, Description: "Session ended at natural stopping point"},
	{Key: "flow_state", Domain: DomainSession, Valence: ValencePositive, Weight: 0.10, Description: "Sustained focus with high quality — flow detected"},
	{Key: "health_maintained", Domain: DomainSession, Valence: ValencePositive, Weight: 0.05, Description: "Health score stayed stable or improved"},
	{Key: "goal_achieved", Domain: DomainSession, Valence: ValencePositive, Weight: 0.10, Description: "Session completed its stated objective"},

	// NEUTRAL: Normal session behavior
	{Key: "standard_work", Domain: DomainSession, Valence: ValenceNeutral, Weight: 0.0, Description: "Normal work session — neither exceptional nor concerning"},
	{Key: "compaction", Domain: DomainSession, Valence: ValenceNeutral, Weight: 0.0, Description: "Context compacted — normal lifecycle event"},
	{Key: "break_taken", Domain: DomainSession, Valence: ValenceNeutral, Weight: 0.0, Description: "Pause in work — healthy rhythm"},

	// NEGATIVE: Unhealthy session patterns
	{Key: "abrupt_quit", Domain: DomainSession, Valence: ValenceNegative, Weight: 0.05, Description: "Session ended abruptly without natural closure"},
	{Key: "context_exhaustion", Domain: DomainSession, Valence: ValenceNegative, Weight: 0.03, Description: "Ran out of context — possible poor resource management"},
	{Key: "quality_dip", Domain: DomainSession, Valence: ValenceNegative, Weight: 0.08, Description: "Quality declined during session — fatigue or drift"},
	{Key: "tunnel_vision", Domain: DomainSession, Valence: ValenceNegative, Weight: 0.08, Description: "Narrow focus missing broader picture"},
	{Key: "health_decline", Domain: DomainSession, Valence: ValenceNegative, Weight: 0.05, Description: "Health score dropped during session"},
}

// ────────────────────────────────────────────────────────────────────────────
// Classification Functions
// ────────────────────────────────────────────────────────────────────────────

// ExchangeValence returns the valence for an exchange type.
func ExchangeValence(exchangeType ExchangeType) Valence {
	switch exchangeType {
	case ExchangeCollaborative, ExchangeCheckIn, ExchangePushback, ExchangeAffirmation:
		return ValencePositive
	case ExchangeDirective, ExchangeQuestion, ExchangeContext:
		return ValenceNeutral
	default:
		return ValenceNegative
	}
}

// ToolValence returns the valence for a tool usage based on tool name and outcome.
func ToolValence(toolName string, success bool) Valence {
	if !success {
		return ValenceNegative
	}
	// Positive: tools that create or verify
	switch toolName {
	case "Write", "Edit", "NotebookEdit":
		return ValencePositive
	case "Bash":
		return ValencePositive // Success = building (tests, builds, commands)
	case "Read", "Glob", "Grep", "WebSearch", "WebFetch":
		return ValenceNeutral // Information gathering
	case "Task", "TaskCreate", "TaskUpdate", "TaskList":
		return ValenceNeutral // Coordination
	default:
		return ValenceNeutral
	}
}

// FeedbackValence returns the valence for a feedback polarity string.
func FeedbackValence(polarity string) Valence {
	switch polarity {
	case "positive":
		return ValencePositive
	case "negative":
		return ValenceNegative
	default:
		return ValenceNeutral
	}
}

// ────────────────────────────────────────────────────────────────────────────
// Schema Lookup
// ────────────────────────────────────────────────────────────────────────────

// AllSignals returns every signal entry across all domains.
func AllSignals() []SignalEntry {
	var all []SignalEntry
	all = append(all, PromptSignals...)
	all = append(all, ExchangeSignals...)
	all = append(all, ToolSignals...)
	all = append(all, SessionSignals...)
	return all
}

// SignalsByValence returns all signals matching the given valence.
func SignalsByValence(v Valence) []SignalEntry {
	var filtered []SignalEntry
	for _, s := range AllSignals() {
		if s.Valence == v {
			filtered = append(filtered, s)
		}
	}
	return filtered
}

// SignalsByDomain returns all signals from a specific domain.
func SignalsByDomain(d SignalDomain) []SignalEntry {
	var filtered []SignalEntry
	for _, s := range AllSignals() {
		if s.Domain == d {
			filtered = append(filtered, s)
		}
	}
	return filtered
}

// LookupSignal finds a signal by key across all domains.
func LookupSignal(key string) (SignalEntry, bool) {
	for _, s := range AllSignals() {
		if s.Key == key {
			return s, true
		}
	}
	return SignalEntry{}, false
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Valence classification applies the 3-key system to all detectable signals:
//
//   POSITIVE (+1): "life and good" — toward God
//     Collaborative engagement, affirmation, successful creation,
//     healthy flow states, iron-sharpening pushback
//
//   NEUTRAL (0): "maintain" — standard operation
//     Task directives, information seeking, standard reads,
//     normal work rhythm, routine compaction
//
//   NEGATIVE (-1): "death and evil" — toward self
//     Correction signals, tool failures, tunnel vision,
//     abrupt quitting, quality degradation
//
// Four signal domains span the full interaction space:
//   Prompt → Exchange → Tool → Session
//
// Each signal carries a weight (impact magnitude) so consumers
// can compute aggregate valence for any time window.
//
// "I have set before thee this day life and good, and death and evil."
// — Deuteronomy 30:15
//
// ============================================================================
// END CLOSING
// ============================================================================
