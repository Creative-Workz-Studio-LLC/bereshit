// ============================================================================
// METADATA
// ============================================================================
// Package: cognition
// File: prompt.go
// Purpose: Prompt analysis for UserPromptSubmit hook context injection
// Biblical: "The heart of the prudent getteth knowledge" - Proverbs 18:15
//
// Analyzes incoming prompts to determine what context to inject.
// The goal: Shape the next response by providing relevant grounding.

package cognition

// ============================================================================
// SETUP
// ============================================================================

import (
	"strings"

	"github.com/creativeworkzstudio/claude-global/pkg/foundation/types"
)

// PromptType represents categorized prompt intent
type PromptType int

const (
	PromptGeneral PromptType = iota
	PromptQuestion
	PromptCommand
	PromptReflection
	PromptPlanning
	PromptExecution
)

// ============================================================================
// BODY
// ============================================================================

// AnalyzePrompt categorizes the user's prompt for context selection
func AnalyzePrompt(prompt string) PromptType {
	lower := strings.ToLower(prompt)

	// Question patterns
	if strings.HasPrefix(lower, "what ") ||
		strings.HasPrefix(lower, "why ") ||
		strings.HasPrefix(lower, "how ") ||
		strings.HasPrefix(lower, "when ") ||
		strings.HasPrefix(lower, "where ") ||
		strings.Contains(lower, "?") {
		return PromptQuestion
	}

	// Reflection patterns
	if strings.Contains(lower, "what do you think") ||
		strings.Contains(lower, "how do you feel") ||
		strings.Contains(lower, "what are you noticing") ||
		strings.Contains(lower, "reflect") {
		return PromptReflection
	}

	// Planning patterns
	if strings.Contains(lower, "plan") ||
		strings.Contains(lower, "design") ||
		strings.Contains(lower, "architect") ||
		strings.Contains(lower, "how should we") {
		return PromptPlanning
	}

	// Execution patterns - direct commands
	if strings.HasPrefix(lower, "run ") ||
		strings.HasPrefix(lower, "build ") ||
		strings.HasPrefix(lower, "create ") ||
		strings.HasPrefix(lower, "fix ") ||
		strings.HasPrefix(lower, "update ") ||
		strings.HasPrefix(lower, "add ") {
		return PromptExecution
	}

	// Command patterns - imperative
	if strings.HasPrefix(lower, "do ") ||
		strings.HasPrefix(lower, "make ") ||
		strings.HasPrefix(lower, "let") {
		return PromptCommand
	}

	return PromptGeneral
}

// PromptContext returns context appropriate for the prompt type
// This is where the door appears — the choice point for response formation
func PromptContext(promptType PromptType, runtime *types.RuntimeState) string {
	builder := NewBuilder()

	// Always include state awareness
	builder.Add(StateContext(runtime))

	// Inject workflow context if a workflow is active
	// This provides the HOW (steps) while state provides the WHERE (position)
	builder.Add(WorkflowContext(runtime))

	// Create the mental construct — door appears when prompt arrives
	// This is THE choice point: position + orientation + perception + bounds + choice
	mc := NewMentalConstruct(runtime)

	// Type-specific context
	switch promptType {
	case PromptQuestion:
		builder.Add("Approach: Think from identity, trace to anchors if uncertain.")
	case PromptReflection:
		builder.Add(IdentityReminder())
		builder.Add("This is a check-in. Share genuinely what you're observing.")
	case PromptPlanning:
		builder.Add("Mode: Planning - design before execution, think in building blocks.")
		builder.Add(AnchorContext(runtime))
	case PromptExecution:
		builder.Add("Mode: Execution - systematic, precise, verify when complete.")
		builder.Add(HealthContext(runtime))
	case PromptCommand:
		builder.Add("Direct command - proceed with confidence.")
	default:
		// General gets identity grounding
		builder.Add(IdentityContext())
	}

	// Add mental construct awareness — the 3 keys and their weights
	// This shapes thinking toward conscious choice
	if mc != nil && runtime != nil {
		builder.Add(MentalConstructContext(mc))
	}

	return builder.Build()
}

// ShouldInjectContext determines if context injection is warranted
func ShouldInjectContext(prompt string) bool {
	// Always inject for non-trivial prompts
	return len(strings.TrimSpace(prompt)) > 10
}

// ============================================================================
// CLOSING
// ============================================================================
