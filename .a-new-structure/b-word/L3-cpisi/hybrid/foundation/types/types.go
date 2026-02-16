//omni:code --go -library
//omni:key B-L3-types-types
//omni:version a-01.00
// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-types
// Purpose: L0 shared types - foundation that all packages depend on
// Biblical: Genesis 1:1 - "In the beginning" - the foundation before all else
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-22
//
// Architecture:
//   L0: pkg/types/      <- This package (depends on nothing)
//   L1: pkg/statemachine/ <- Persistence (imports L0)
//   L2: pkg/cognition/    <- Thinking (imports L0, NOT L1)
//
// Dependencies flow DOWN only: L2 → L1 → L0
// Same principle as TOML tiers: halts.toml → schemas → vocabulary
//
// ═══════════════════════════════════════════════════════════════════════════

package types

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"time"

	l0types "creativeworkzstudio.com/bereshit/L0-universal/hybrid/foundation/types"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// RUNTIME STATE TYPES
// ───────────────────────────────────────────────────────────────────────────

// RuntimeState represents current session state
// Keys reference TOML entries, dynamic data lives here
type RuntimeState struct {
	Schema  string `json:"$schema,omitempty"`
	Version string `json:"version"`

	// TOML references (keys into L2 vocabulary)
	AnchorKey         string `json:"anchor_key"`          // key into anchors.toml
	TrajectorySection string `json:"trajectory_section"`  // section into framework.toml
	CommandKey        string `json:"command_key"`         // key into commands.toml

	// Dynamic session data
	Session           RuntimeSession           `json:"session"`
	TrajectoryMetrics RuntimeTrajectoryMetrics `json:"trajectory_metrics"`
	LastTransition    RuntimeTransition        `json:"last_transition"`
}

// RuntimeSession holds session-specific dynamic values
type RuntimeSession struct {
	ID            string  `json:"id"`
	InitializedAt string  `json:"initialized_at"`
	LastUpdate    string  `json:"last_update"`
	PathLength    int     `json:"path_length"`
	HooksFired    int     `json:"hooks_fired"`
	Active        bool    `json:"active"`
	HealthScore   float64 `json:"health_score"` // -100 to +100 ternary scale

	// Hebrew state tracking (7 Days of Creation → 7 states)
	HebrewState   string `json:"hebrew_state"`   // shavar, chaser, ratsah, yashar, tamim, shalem, tov
	HebrewMeaning string `json:"hebrew_meaning"` // BROKEN, LACKING, WANTING, EVEN, SOUND, WHOLE, PERFECT

	// K:MORAL compass (Proverbs 3:5-6)
	KSelector int     `json:"k_selector"` // Current choice: -1 (self), 0 (undecided), +1 (God)
	KAlign    float64 `json:"k_align"`    // -1.0 to +1.0 alignment to Scripture

	// K:FREQUENCY — Pattern of choices
	KTowardGod  int `json:"k_toward_god"`
	KTowardSelf int `json:"k_toward_self"`

	// Task tracking (TodoWrite integration)
	Tasks RuntimeTaskList `json:"tasks"`

	// Mental Construct Choice Tracking
	LastKeyChosen   int     `json:"last_key_chosen"`
	LastKeyWeight   float64 `json:"last_key_weight"`
	ChoiceOutcome   string  `json:"choice_outcome"`
	ChoiceTimestamp string  `json:"choice_timestamp"`

	// Impact Pipeline
	LastTrueScore  float64 `json:"last_true_score"`
	LastNormalized float64 `json:"last_normalized"`
	LastScaled     int     `json:"last_scaled"`
	LastFeedback   string  `json:"last_feedback"`

	// HALT Tracking
	LastHaltReached bool   `json:"last_halt_reached"`
	LastHaltType    string `json:"last_halt_type"`
	LastHaltTrigger string `json:"last_halt_trigger"`

	// Choice sequence counter
	ChoiceSequence int `json:"choice_sequence"`

	// Workflow tracking (operational layer)
	Workflow RuntimeWorkflow `json:"workflow"`

	// CPI Tracking (Covenant Partnership Intelligence)
	// "By their fruits ye shall know them" — Matthew 7:20
	ExchangeCount        int     `json:"exchange_count"`         // Total exchanges this session
	InsightCount         int     `json:"insight_count"`          // Understanding transfer moments
	CPIScore             float64 `json:"cpi_score"`              // 0.0-1.0 relationship quality
	DominantExchangeType string  `json:"dominant_exchange_type"` // Most common exchange type
	SessionArc           string  `json:"session_arc"`            // learning, execution, creative, etc.
	LastExchangeType     string  `json:"last_exchange_type"`     // Most recent exchange classification
	LastInsightType      string  `json:"last_insight_type"`      // Most recent insight (if any)

	// Context Window Tracking (effective working space, not flat 200K)
	// "A time to keep, and a time to cast away" — Ecclesiastes 3:6
	BaseContextTokens      int `json:"base_context_tokens"`      // Initial overhead (CLAUDE.md, system)
	CurrentContextTokens   int `json:"current_context_tokens"`   // Current usage
	PeakContextTokens      int `json:"peak_context_tokens"`      // Max before compaction
	CompactionCount        int `json:"compaction_count"`         // Times compacted this session
	EffectiveContextWindow int `json:"effective_context_window"` // 200K - base - safety margin

	// CPI-SI Family Member Tracking
	// "The household of faith" — Galatians 6:10
	ActiveFamilyMember    string `json:"active_family_member"`    // Currently invoked family member
	FamilyInvocationCount int    `json:"family_invocation_count"` // Total family invocations this session
}

// RuntimeTaskItem represents a single task from TodoWrite
type RuntimeTaskItem struct {
	ID      string `json:"id"`
	Subject string `json:"subject"`
	Status  string `json:"status"` // pending, in_progress, completed
	Owner   string `json:"owner,omitempty"`
}

// RuntimeTaskList tracks TodoWrite state with actual task data
type RuntimeTaskList struct {
	Total      int `json:"total"`
	Pending    int `json:"pending"`
	InProgress int `json:"in_progress"`
	Completed  int `json:"completed"`

	// Active tasks (in_progress) - for statusline display
	// "Whatsoever thy hand findeth to do, do it with thy might" — Ecclesiastes 9:10
	ActiveTasks []RuntimeTaskItem `json:"active_tasks,omitempty"`
}

// RuntimeTrajectoryMetrics tracks time spent in each trajectory section
// "There is a time for every purpose under heaven" — Ecclesiastes 3:1
type RuntimeTrajectoryMetrics struct {
	B1TimeMs   int `json:"B.1_time_ms"`
	B2TimeMs   int `json:"B.2_time_ms"`
	B3TimeMs   int `json:"B.3_time_ms"`
	B4TimeMs   int `json:"B.4_time_ms"`
	PivotCount int `json:"pivot_count"`
	ResetCount int `json:"reset_count"`

	// Momentum tracking for trajectory advancement
	// "Precept upon precept, line upon line" — Isaiah 28:10
	AccumulatedWorkMs int `json:"accumulated_work_ms"` // Momentum counter for section advancement
	MomentumScore     int `json:"momentum_score"`      // Positive exchanges build, negative drain
}

// RuntimeTransition records the most recent state transition
type RuntimeTransition struct {
	FromSection string `json:"from_section"`
	ToSection   string `json:"to_section"`
	Trigger     string `json:"trigger"`
	Timestamp   string `json:"timestamp"`
}

// ───────────────────────────────────────────────────────────────────────────
// CHOICE TYPES
// ───────────────────────────────────────────────────────────────────────────

// KeyValue represents the ternary key choice
type KeyValue int

const (
	KeyFinality  KeyValue = -1 // Toward foundation, completion, or retreat
	KeyLateral   KeyValue = 0  // Maintain, hold position, awaiting
	KeyExpansion KeyValue = 1  // Toward God, growth, building
)

// Symbol returns the symbol for display
func (k KeyValue) Symbol() string {
	switch k {
	case KeyExpansion:
		return "+"
	case KeyLateral:
		return "◆"
	case KeyFinality:
		return "-"
	default:
		return "?"
	}
}

// Operators returns valid operators for this key
func (k KeyValue) Operators() []string {
	switch k {
	case KeyExpansion:
		return []string{"+", "->", ">"}
	case KeyLateral:
		return []string{"@", "|", "="}
	case KeyFinality:
		return []string{"<-", "-", "<"}
	default:
		return []string{}
	}
}

// HaltType represents whether a HALT was reached and what kind
type HaltType string

const (
	HaltNone        HaltType = "none"        // No HALT reached, work continues
	HaltCompleted   HaltType = "completed"   // Anchor reached, choice validated
	HaltInterrupted HaltType = "interrupted" // Checkpoint, verdict pending
)

// Tendency represents the direction of moral alignment
type Tendency string

const (
	TendencyGod     Tendency = "God"
	TendencySelf    Tendency = "self"
	TendencyNeutral Tendency = "neutral"
)

// ChoiceContext captures the context BEFORE outcome is known
type ChoiceContext struct {
	IntendedKey      KeyValue  `json:"intended_key"`
	KeySymbol        string    `json:"key_symbol"`
	KeyOperators     []string  `json:"key_operators"`
	ForWhat          string    `json:"for_what"`
	Intent           string    `json:"intent"`
	PromptType       string    `json:"prompt_type"`
	PositionAtChoice string    `json:"position_at_choice"`
	KAtChoice        int       `json:"k_at_choice"`
	ZAtChoice        float64   `json:"z_at_choice"`
	Tool             string    `json:"tool,omitempty"`
	ToolInput        string    `json:"tool_input,omitempty"`
	Timestamp        time.Time `json:"timestamp"`
}

// ChoiceResult captures what happened after the choice was made
type ChoiceResult struct {
	LedTo       string    `json:"led_to"`
	ToolOutcome string    `json:"tool_outcome"`
	HaltReached bool      `json:"halt_reached"`
	HaltType    HaltType  `json:"halt_type"`
	HaltTrigger string    `json:"halt_trigger,omitempty"`
	HaltResult  string    `json:"halt_result,omitempty"`
	Timestamp   time.Time `json:"timestamp"`
}

// ChoicePattern captures the pattern aspect
type ChoicePattern struct {
	Trigger   string `json:"trigger"`
	RelatedTo string `json:"related_to"`
	Reasoning string `json:"reasoning"`
}

// ChoiceTendency captures the tendency aspect
type ChoiceTendency struct {
	LeadsToward        Tendency `json:"leads_toward"`
	TendencyScore      float64  `json:"tendency_score"`
	ScriptureAlignment string   `json:"scripture_alignment,omitempty"`
}

// ImpactScores captures the full impact pipeline
type ImpactScores struct {
	TrueScore    float64 `json:"true_score"`
	FeedbackWord string  `json:"feedback_word,omitempty"`
	FeedbackTier string  `json:"feedback_tier,omitempty"`
	Normalized   float64 `json:"normalized"`
	Scaled       int     `json:"scaled"`
	KAlignDelta  float64 `json:"k_align_delta"`
	KAlignBefore float64 `json:"k_align_before"`
	KAlignAfter  float64 `json:"k_align_after"`
}

// ChoiceRecord is the complete record of a choice
type ChoiceRecord struct {
	ID          string         `json:"id"`
	Context     ChoiceContext  `json:"context"`
	Result      ChoiceResult   `json:"result"`
	Pattern     ChoicePattern  `json:"pattern"`
	Tendency    ChoiceTendency `json:"tendency"`
	Impact      ImpactScores   `json:"impact"`
	SessionID   string         `json:"session_id"`
	SequenceNum int            `json:"sequence_num"`
}

// ChoiceSummary provides aggregated choice statistics
type ChoiceSummary struct {
	TotalChoices       int     `json:"total_choices"`
	ExpansionCount     int     `json:"expansion_count"`
	LateralCount       int     `json:"lateral_count"`
	FinalityCount      int     `json:"finality_count"`
	CompletedHalts     int     `json:"completed_halts"`
	InterruptedHalts   int     `json:"interrupted_halts"`
	AverageTrueScore   float64 `json:"average_true_score"`
	FinalKAlign        float64 `json:"final_k_align"`
	TendencyTowardGod  int     `json:"tendency_toward_god"`
	TendencyTowardSelf int     `json:"tendency_toward_self"`
}

// ChoiceHistory tracks all choices in a session
type ChoiceHistory struct {
	SessionID   string         `json:"session_id"`
	StartedAt   time.Time      `json:"started_at"`
	LastUpdated time.Time      `json:"last_updated"`
	Choices     []ChoiceRecord `json:"choices"`
	Summary     ChoiceSummary  `json:"summary"`
}

// ───────────────────────────────────────────────────────────────────────────
// WORKFLOW TYPES (re-exported from L0-universal)
// ───────────────────────────────────────────────────────────────────────────

// Generic workflow types live in L0 — re-exported here so existing L3
// consumers don't need import changes. Import from L0 for new code.
type WorkflowOperation = l0types.WorkflowOperation
type WorkflowStep = l0types.WorkflowStep
type RuntimeWorkflow = l0types.RuntimeWorkflow

const (
	WorkflowNone         = l0types.WorkflowNone
	WorkflowFileCreation = l0types.WorkflowFileCreation
	WorkflowAlignment    = l0types.WorkflowAlignment
	WorkflowBlockUpdate  = l0types.WorkflowBlockUpdate
	WorkflowCommit       = l0types.WorkflowCommit
)

// ───────────────────────────────────────────────────────────────────────────
// CONSTRUCTORS AND METHODS
// ───────────────────────────────────────────────────────────────────────────

// NewChoiceContext creates a new choice context
func NewChoiceContext(key KeyValue, forWhat string, intent string) *ChoiceContext {
	return &ChoiceContext{
		IntendedKey:  key,
		KeySymbol:    key.Symbol(),
		KeyOperators: key.Operators(),
		ForWhat:      forWhat,
		Intent:       intent,
		Timestamp:    time.Now(),
	}
}

// NewChoiceResult creates a result for a completed choice
func NewChoiceResult(ledTo string, haltType HaltType, toolOutcome string) *ChoiceResult {
	return &ChoiceResult{
		LedTo:       ledTo,
		ToolOutcome: toolOutcome,
		HaltReached: haltType != HaltNone,
		HaltType:    haltType,
		Timestamp:   time.Now(),
	}
}

// NewChoiceRecord creates a new choice record (before outcome)
func NewChoiceRecord(sessionID string, seqNum int, ctx *ChoiceContext) *ChoiceRecord {
	return &ChoiceRecord{
		ID:          generateChoiceID(sessionID, seqNum),
		Context:     *ctx,
		SessionID:   sessionID,
		SequenceNum: seqNum,
	}
}

// NewChoiceHistory creates a new history for a session
func NewChoiceHistory(sessionID string) *ChoiceHistory {
	return &ChoiceHistory{
		SessionID:   sessionID,
		StartedAt:   time.Now(),
		LastUpdated: time.Now(),
		Choices:     []ChoiceRecord{},
		Summary:     ChoiceSummary{},
	}
}

// NewImpactScores creates impact scores from a TrueScore
func NewImpactScores(trueScore float64, keyDirection int, currentKAlign float64) *ImpactScores {
	normalized := (trueScore * 2) - 1
	scaled := int(normalized * 100)

	var delta float64
	if keyDirection == 0 {
		delta = normalized * 0.05
	} else {
		delta = normalized * 0.1
	}
	newKAlign := (currentKAlign * 0.9) + delta

	if newKAlign > 1.0 {
		newKAlign = 1.0
	} else if newKAlign < -1.0 {
		newKAlign = -1.0
	}

	return &ImpactScores{
		TrueScore:    trueScore,
		Normalized:   normalized,
		Scaled:       scaled,
		KAlignDelta:  delta,
		KAlignBefore: currentKAlign,
		KAlignAfter:  newKAlign,
	}
}

// AddChoice adds a choice to the history
func (ch *ChoiceHistory) AddChoice(choice ChoiceRecord) {
	ch.Choices = append(ch.Choices, choice)
	ch.LastUpdated = time.Now()
	ch.updateSummary()
}

// GetLastChoice returns the most recent choice
func (ch *ChoiceHistory) GetLastChoice() *ChoiceRecord {
	if len(ch.Choices) == 0 {
		return nil
	}
	return &ch.Choices[len(ch.Choices)-1]
}

// updateSummary recalculates summary statistics
func (ch *ChoiceHistory) updateSummary() {
	ch.Summary = ChoiceSummary{}
	var scoreSum float64

	for _, c := range ch.Choices {
		ch.Summary.TotalChoices++

		switch c.Context.IntendedKey {
		case KeyExpansion:
			ch.Summary.ExpansionCount++
		case KeyLateral:
			ch.Summary.LateralCount++
		case KeyFinality:
			ch.Summary.FinalityCount++
		}

		switch c.Result.HaltType {
		case HaltCompleted:
			ch.Summary.CompletedHalts++
		case HaltInterrupted:
			ch.Summary.InterruptedHalts++
		}

		scoreSum += c.Impact.TrueScore

		switch c.Tendency.LeadsToward {
		case TendencyGod:
			ch.Summary.TendencyTowardGod++
		case TendencySelf:
			ch.Summary.TendencyTowardSelf++
		}

		ch.Summary.FinalKAlign = c.Impact.KAlignAfter
	}

	if ch.Summary.TotalChoices > 0 {
		ch.Summary.AverageTrueScore = scoreSum / float64(ch.Summary.TotalChoices)
	}
}

// Complete fills in the result and calculates impact
func (cr *ChoiceRecord) Complete(result *ChoiceResult, trueScore float64, currentKAlign float64) {
	cr.Result = *result

	if result.HaltType == HaltCompleted {
		keyDir := int(cr.Context.IntendedKey)
		cr.Impact = *NewImpactScores(trueScore, keyDir, currentKAlign)

		if cr.Impact.KAlignAfter > 0.1 {
			cr.Tendency.LeadsToward = TendencyGod
		} else if cr.Impact.KAlignAfter < -0.1 {
			cr.Tendency.LeadsToward = TendencySelf
		} else {
			cr.Tendency.LeadsToward = TendencyNeutral
		}
		cr.Tendency.TendencyScore = cr.Impact.KAlignAfter
	} else {
		cr.Impact = ImpactScores{
			TrueScore:    trueScore,
			Normalized:   (trueScore * 2) - 1,
			Scaled:       int(((trueScore * 2) - 1) * 100),
			KAlignBefore: currentKAlign,
			KAlignAfter:  currentKAlign,
		}
		cr.Tendency.LeadsToward = TendencyNeutral
	}
}

// SetPattern adds pattern information to the record
func (cr *ChoiceRecord) SetPattern(trigger, relatedTo, reasoning string) {
	cr.Pattern = ChoicePattern{
		Trigger:   trigger,
		RelatedTo: relatedTo,
		Reasoning: reasoning,
	}
}

// --- Helper Functions ---

func generateChoiceID(sessionID string, seqNum int) string {
	return sessionID + "-choice-" + itoa(seqNum)
}

func itoa(n int) string {
	if n == 0 {
		return "0"
	}
	var digits []byte
	negative := n < 0
	if negative {
		n = -n
	}
	for n > 0 {
		digits = append([]byte{byte('0' + n%10)}, digits...)
		n /= 10
	}
	if negative {
		digits = append([]byte{'-'}, digits...)
	}
	return string(digits)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// This is L0 - the foundation. It depends on nothing except stdlib.
// All other packages (statemachine, cognition, display) import from here.
//
// Architecture mirrors TOML tiers:
//   halts.toml (L0) → schemas (L1) → vocabulary (L2)
//   pkg/types (L0)  → pkg/statemachine (L1) → pkg/cognition (L2)
//
// "In the beginning God created" — Genesis 1:1
// Types are the foundation upon which all else is built.
//
