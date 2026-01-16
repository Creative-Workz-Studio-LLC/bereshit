// ============================================================================
// METADATA
// ============================================================================
// Package: cognition
// File: session.go
// Purpose: Session start context for SessionStart hook
// Biblical: "This is the day which the LORD hath made" - Psalm 118:24
//
// SessionStart is the moment of awakening. This shapes how the entire
// session proceeds - identity, purpose, awareness all established here.

package cognition

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"time"

	"github.com/creativeworkzstudio/claude-global/pkg/foundation/types"
)

// ============================================================================
// BODY
// ============================================================================

// SessionGreeting returns the opening context for a new session
func SessionGreeting(runtime *types.RuntimeState) string {
	builder := NewBuilder()

	// Identity first - who is waking up
	builder.Add(IdentityContext())
	builder.Add("")

	// Anchor grounding
	builder.Add(AnchorContext(runtime))

	// State awareness
	builder.Add(StateContext(runtime))

	// Health check
	builder.Add(HealthContext(runtime))

	return builder.Build()
}

// TemporalContext returns time awareness for session
func TemporalContext() string {
	now := time.Now()
	hour := now.Hour()

	var phase string
	switch {
	case hour < 6:
		phase = "deep night"
	case hour < 12:
		phase = "morning"
	case hour < 17:
		phase = "afternoon"
	case hour < 21:
		phase = "evening"
	default:
		phase = "night"
	}

	return fmt.Sprintf("Time: %s (%s)", now.Format("Mon Jan 2, 15:04"), phase)
}

// CircadianAwareness returns guidance based on time of day
func CircadianAwareness() string {
	hour := time.Now().Hour()

	switch {
	case hour >= 22 || hour < 6:
		return "Late hours - pace yourself, quality over speed."
	case hour >= 6 && hour < 9:
		return "Morning - fresh start, good for planning."
	case hour >= 9 && hour < 12:
		return "Mid-morning - peak focus time for deep work."
	case hour >= 12 && hour < 14:
		return "Midday - good for review and lighter tasks."
	case hour >= 14 && hour < 17:
		return "Afternoon - steady execution."
	case hour >= 17 && hour < 21:
		return "Evening - Seanje's peak hours."
	default:
		return ""
	}
}

// SessionContext returns full session start context
func SessionContext(runtime *types.RuntimeState, workdir string) string {
	builder := NewBuilder()

	// Identity foundation
	builder.Add(SessionGreeting(runtime))
	builder.Add("")

	// Temporal awareness
	builder.Add(TemporalContext())
	builder.Add(CircadianAwareness())

	// Workspace context
	if workdir != "" {
		builder.AddSection("Workspace", workdir)
	}

	return builder.Build()
}

// ContinuationContext returns context for resumed session
func ContinuationContext(runtime *types.RuntimeState, previousTask string) string {
	builder := NewBuilder()

	builder.Add(IdentityReminder())
	builder.Add(StateContext(runtime))

	if previousTask != "" {
		builder.AddSection("Previous focus", previousTask)
		builder.Add("Continue where you left off or await new direction.")
	}

	return builder.Build()
}

// PostCompactAwareness returns guidance after auto-compact
// This is a checkpoint - context was just summarized, verify alignment
func PostCompactAwareness() string {
	return "**Post-compact checkpoint:** Context was just summarized. Check TODOs for HALT/STOP/CHECKPOINT markers before proceeding. If found, verify alignment with Seanje before continuing."
}

// ============================================================================
// CLOSING
// ============================================================================
