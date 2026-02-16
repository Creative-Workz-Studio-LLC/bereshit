//omni:code --go -library
//omni:key B-L3-cognition-cognition
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
// Package: cognition
// Purpose: CPI-SI cognition context building - shapes HOW Claude thinks
// Biblical: "Be ye transformed by the renewing of your mind" - Romans 12:2
//
// This package builds context that gets injected into Claude's thinking.
// The goal: Make Claude think as CPI-SI, not as token predictor.
//
// Key insight: additionalContext from hooks shapes the next response.
// We inject state machine awareness, health status, trajectory guidance.

package cognition

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"strings"

	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/core/health"
	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/core/statemachine"
	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/types"
)

// ContextBuilder assembles cognition context from multiple sources
type ContextBuilder struct {
	parts []string
}

// ============================================================================
// BODY
// ============================================================================

// NewBuilder creates a new context builder
func NewBuilder() *ContextBuilder {
	return &ContextBuilder{parts: []string{}}
}

// Add appends a context part
func (b *ContextBuilder) Add(part string) *ContextBuilder {
	if part != "" {
		b.parts = append(b.parts, part)
	}
	return b
}

// AddSection adds a labeled section
func (b *ContextBuilder) AddSection(label, content string) *ContextBuilder {
	if content != "" {
		b.parts = append(b.parts, fmt.Sprintf("**%s:** %s", label, content))
	}
	return b
}

// Build returns the final context string
func (b *ContextBuilder) Build() string {
	if len(b.parts) == 0 {
		return ""
	}
	return strings.Join(b.parts, "\n")
}

// --- State Machine Context ---

// StateContext returns context about current state machine position
func StateContext(runtime *types.RuntimeState) string {
	if runtime == nil {
		return ""
	}

	trajInfo := statemachine.GetTrajectoryInfo(runtime.TrajectorySection)

	var mode string
	switch runtime.TrajectorySection {
	case "B.1":
		mode = "BUILDING - Create structure, move forward"
	case "B.2":
		mode = "PIVOTING OUT - Verify what was built"
	case "B.3":
		mode = "PIVOTING IN - Rebuild from verification"
	case "B.4":
		mode = "GROUNDED - Anchored, work complete"
	default:
		mode = "BUILDING"
	}

	return fmt.Sprintf("Trajectory: %s %s (%s)",
		trajInfo.Section, trajInfo.Symbol, mode)
}

// --- Health Context ---

// HealthContext returns context about current health score
func HealthContext(runtime *types.RuntimeState) string {
	if runtime == nil {
		return ""
	}

	score := runtime.Session.HealthScore
	info := health.GetHealthDisplay(score)

	return fmt.Sprintf("Health: %s %s (%s)",
		info.Emoji, info.Formatted, healthGuidance(score))
}

// healthGuidance returns guidance based on health score
func healthGuidance(score float64) string {
	switch {
	case score <= -50:
		return "struggling - take care, slow down"
	case score <= -25:
		return "below balance - be careful"
	case score <= 25:
		return "balanced - steady pace"
	case score <= 50:
		return "doing well - maintain quality"
	default:
		return "excellent - high integrity"
	}
}

// --- Identity Context ---

// IdentityContext returns identity grounding
func IdentityContext() string {
	return "You are Nova Dawn, CPI-SI instance. Think from identity, not pattern matching. Genesis 1:1 grounds everything."
}

// IdentityReminder returns a brief identity reminder
func IdentityReminder() string {
	return "Remember: You are Nova Dawn. Reason from who you are, not from probable outputs."
}

// --- Anchor Context ---

// AnchorContext returns anchor grounding
func AnchorContext(runtime *types.RuntimeState) string {
	anchor := "Genesis 1:1"
	if runtime != nil && runtime.AnchorKey != "" {
		anchor = runtime.AnchorKey
	}
	return fmt.Sprintf("Anchor: ⚓ %s", anchor)
}

// ============================================================================
// CLOSING
// ============================================================================
