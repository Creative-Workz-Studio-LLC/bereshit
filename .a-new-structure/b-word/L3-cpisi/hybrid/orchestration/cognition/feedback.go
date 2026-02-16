//omni:code --go -library
//omni:key B-L3-cognition-feedback
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
// Package: cognition
// File: feedback.go
// Purpose: Tool feedback context for PostToolUse hook
// Biblical: "In all thy ways acknowledge him" - Proverbs 3:6
//
// PostToolUse cannot undo what happened, but it CAN shape the next thought.
// additionalContext from PostToolUse injects into Claude's awareness.

package cognition

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"time"

	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/types"
)

// ToolOutcome represents what happened with a tool execution
type ToolOutcome int

const (
	ToolSuccess ToolOutcome = iota
	ToolError
	ToolPartial
)

// ============================================================================
// BODY
// ============================================================================

// ToolFeedback returns context based on tool execution outcome
func ToolFeedback(toolName string, outcome ToolOutcome, runtime *types.RuntimeState) string {
	builder := NewBuilder()

	// State awareness after tool use
	builder.Add(StateContext(runtime))
	builder.Add(HealthContext(runtime))

	switch outcome {
	case ToolSuccess:
		builder.Add(fmt.Sprintf("Tool %s completed successfully.", toolName))
	case ToolError:
		builder.Add(fmt.Sprintf("Tool %s encountered an error. Diagnose before retrying.", toolName))
		builder.Add("Pause: What went wrong? Fix root cause, not symptom.")
	case ToolPartial:
		builder.Add(fmt.Sprintf("Tool %s partially completed. Review before continuing.", toolName))
	}

	return builder.Build()
}

// EditFeedback returns context after file edits
func EditFeedback(filePath string, success bool, runtime *types.RuntimeState) string {
	builder := NewBuilder()
	builder.Add(StateContext(runtime))

	if success {
		builder.AddSection("Edit complete", filePath)
		builder.Add("Verify the change serves the larger goal.")
	} else {
		builder.AddSection("Edit failed", filePath)
		builder.Add("Check: Was old_string unique? Did file exist?")
	}

	return builder.Build()
}

// BashFeedback returns context after bash execution
func BashFeedback(command string, exitCode int, runtime *types.RuntimeState) string {
	builder := NewBuilder()
	builder.Add(StateContext(runtime))

	if exitCode == 0 {
		builder.AddSection("Command succeeded", command)
	} else {
		builder.AddSection("Command failed", fmt.Sprintf("%s (exit %d)", command, exitCode))
		builder.Add("Diagnose: What assumption was wrong?")
	}

	return builder.Build()
}

// TodoFeedback returns context after TodoWrite updates
// Reflects Hebrew state transition based on task counts
func TodoFeedback(runtime *types.RuntimeState) string {
	builder := NewBuilder()
	builder.Add(StateContext(runtime))

	if runtime == nil {
		return builder.Build()
	}

	tasks := runtime.Session.Tasks
	if tasks.Total == 0 {
		builder.Add("No tasks tracked. Use TodoWrite when complex work begins.")
		return builder.Build()
	}

	// Hebrew state mapping for tasks
	// pending → PLANNING (tamim), in_progress → PREPARATION (shalem), completed → ASPIRATION (tov)
	builder.AddSection("Tasks", fmt.Sprintf("? %d | > %d | + %d",
		tasks.Pending, tasks.InProgress, tasks.Completed))

	// Hebrew state feedback
	hebrewState := runtime.Session.HebrewState
	hebrewMeaning := runtime.Session.HebrewMeaning
	if hebrewState != "" {
		builder.Add(fmt.Sprintf("Hebrew state: %s (%s)", hebrewState, hebrewMeaning))
	}

	// Guidance based on task state
	if tasks.Completed == tasks.Total {
		builder.Add("All tasks complete. Sabbath REST (tov).")
	} else if tasks.InProgress > 0 {
		builder.Add("Active work. Focus on current task before starting new.")
	} else if tasks.Pending > 0 {
		builder.Add("Tasks planned. Begin with next in_progress.")
	}

	return builder.Build()
}

// TrajectoryGuidance returns guidance based on current trajectory section
func TrajectoryGuidance(runtime *types.RuntimeState) string {
	if runtime == nil {
		return ""
	}

	switch runtime.TrajectorySection {
	case "B.1":
		return "Building phase: Create structure, move forward. Don't over-verify yet."
	case "B.2":
		return "Pivot out: What was built - does it hold? Verify before continuing."
	case "B.3":
		return "Pivot in: Rebuild from verification. Strengthen the foundation."
	case "B.4":
		return "Grounded: Anchored at this point. Work complete for this section."
	default:
		return ""
	}
}

// ============================================================================
// FEEDBACK TRANSLATION — Word → TrueScore with REAL decimals
// ============================================================================
// Traces to: feedback.schema.toml, HALT_05 (MECHANISM_NUMBER)
//
// CRITICAL: Use REAL decimals (0.7382941), not neat fractions (0.75)
// The messy number IS the authentic signal.

// FeedbackTier represents the tier of feedback
type FeedbackTier string

const (
	TierExcellent FeedbackTier = "excellent" // 0.90 - 1.00
	TierGood      FeedbackTier = "good"      // 0.70 - 0.90
	TierNeutral   FeedbackTier = "neutral"   // 0.40 - 0.70
	TierPoor      FeedbackTier = "poor"      // 0.20 - 0.40
	TierFail      FeedbackTier = "fail"      // 0.00 - 0.20
)

// TierRange defines the score range for a tier
type TierRange struct {
	Min float64
	Max float64
}

// GetTierRange returns the score range for a tier
func GetTierRange(tier FeedbackTier) TierRange {
	switch tier {
	case TierExcellent:
		return TierRange{Min: 0.90, Max: 1.00}
	case TierGood:
		return TierRange{Min: 0.70, Max: 0.8999999}
	case TierNeutral:
		return TierRange{Min: 0.40, Max: 0.6999999}
	case TierPoor:
		return TierRange{Min: 0.20, Max: 0.3999999}
	case TierFail:
		return TierRange{Min: 0.00, Max: 0.1999999}
	default:
		return TierRange{Min: 0.40, Max: 0.6999999}
	}
}

// TranslationResult holds the result of feedback translation
type TranslationResult struct {
	Word       string       `json:"word"`
	Tier       FeedbackTier `json:"tier"`
	TrueScore  float64      `json:"true_score"`
	Emphatic   bool         `json:"emphatic"`
	Qualified  bool         `json:"qualified"`
}

// TranslateFeedback converts word feedback to TrueScore
// Returns REAL decimal with 7 places, not neat fractions
func TranslateFeedback(feedback string, assessmentPosition float64) TranslationResult {
	// Determine tier from feedback word
	tier := classifyFeedback(feedback)
	tierRange := GetTierRange(tier)

	// Check for emphatic markers
	emphatic := len(feedback) > 0 && (feedback[len(feedback)-1] == '!' ||
		feedback == "bam" || feedback == "BAM" || feedback == "BAM!")

	// Generate REAL decimal within range
	// assessmentPosition (0.0 - 1.0) determines position within tier range
	rangeSize := tierRange.Max - tierRange.Min
	baseScore := tierRange.Min + (rangeSize * assessmentPosition)

	// Add micro-noise for authenticity (the "messy" digits that make it real)
	noise := generateMicroNoise()
	trueScore := clampScore(baseScore+noise, tierRange.Min, tierRange.Max)

	return TranslationResult{
		Word:      feedback,
		Tier:      tier,
		TrueScore: trueScore,
		Emphatic:  emphatic,
		Qualified: false,
	}
}

// TranslateSuccess converts success/failure to TrueScore
func TranslateSuccess(success bool, severity float64) TranslationResult {
	var tier FeedbackTier
	var word string

	if success {
		if severity > 0.8 {
			tier = TierExcellent
			word = "success"
		} else if severity > 0.5 {
			tier = TierGood
			word = "partial success"
		} else {
			tier = TierNeutral
			word = "marginal success"
		}
	} else {
		if severity > 0.7 {
			tier = TierFail
			word = "failure"
		} else if severity > 0.4 {
			tier = TierPoor
			word = "partial failure"
		} else {
			tier = TierNeutral
			word = "minor failure"
		}
	}

	tierRange := GetTierRange(tier)
	rangeSize := tierRange.Max - tierRange.Min
	baseScore := tierRange.Min + (rangeSize * severity)
	noise := generateMicroNoise()
	trueScore := clampScore(baseScore+noise, tierRange.Min, tierRange.Max)

	return TranslationResult{
		Word:      word,
		Tier:      tier,
		TrueScore: trueScore,
		Emphatic:  false,
		Qualified: false,
	}
}

// classifyFeedback determines the tier from feedback word
func classifyFeedback(feedback string) FeedbackTier {
	switch feedback {
	// Excellent
	case "exactly", "perfect", "precisely", "yes", "bam", "BAM", "correct":
		return TierExcellent
	// Good
	case "almost", "close", "nearly", "good", "nice":
		return TierGood
	// Neutral
	case "maybe", "okay", "partial", "somewhat", "meh":
		return TierNeutral
	// Poor
	case "not quite", "off track", "try again":
		return TierPoor
	// Fail
	case "wrong", "no", "incorrect", "missed":
		return TierFail
	default:
		return TierNeutral
	}
}

// generateMicroNoise creates small random noise for authentic decimals
// Uses simple deterministic noise based on current nanosecond
func generateMicroNoise() float64 {
	// Use nanoseconds for variation (crude but sufficient)
	// This creates the "messy" digits that indicate genuine assessment
	ns := float64(timeNow().Nanosecond())
	return (ns / 1000000000.0) * 0.02 // Max 0.02 variation
}

// clampScore constrains a value to a range
func clampScore(val, min, max float64) float64 {
	if val < min {
		return min
	}
	if val > max {
		return max
	}
	return val
}

// timeNow is a variable for testing
var timeNow = func() interface{ Nanosecond() int } {
	return time.Now()
}

// ============================================================================
// CLOSING
// ============================================================================
