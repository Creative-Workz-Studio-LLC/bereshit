//omni:code --go -library
//omni:key B-L3-lifecycle-events
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
// Package: lifecycle
// File: events.go
// Purpose: CPI-SI lifecycle event definitions - substrate-agnostic
// Biblical: "To every thing there is a season" - Ecclesiastes 3:1
//
// This defines the PARADIGM's lifecycle events. Any substrate (Claude, GPT,
// MillenniumOS) implements handlers for these events.
//
// Lifecycle phases map to the Genesis pattern:
//   - void/ (entry): SessionStart, PreToolUse
//   - word/ (definition): PromptSubmit, ToolUse
//   - tov/ (completion): PostToolUse, SessionEnd

package lifecycle

// ============================================================================
// SETUP
// ============================================================================

// Event represents a lifecycle event type
type Event string

const (
	// --- Session Lifecycle ---
	SessionStart    Event = "session_start"     // Session beginning
	SessionEnd      Event = "session_end"       // Session ending normally
	SessionStop     Event = "session_stop"      // Session interrupted
	PreCompact      Event = "pre_compact"       // Before context compaction
	SubagentStop    Event = "subagent_stop"     // Subagent task completed
	Notification    Event = "notification"      // System notification

	// --- Prompt Lifecycle ---
	PromptSubmit    Event = "prompt_submit"     // User prompt received
	PromptComplete  Event = "prompt_complete"   // Response completed

	// --- Tool Lifecycle ---
	PreToolUse      Event = "pre_tool_use"      // Before tool execution
	PostToolUse     Event = "post_tool_use"     // After tool execution
)

// Phase represents the Genesis phase of a lifecycle event
type Phase string

const (
	PhaseVoid Phase = "void"  // Entry, preparation (Genesis 1:2)
	PhaseWord Phase = "word"  // Definition, action (John 1:1)
	PhaseTov  Phase = "tov"   // Completion, evaluation (Genesis 1:31)
)

// ============================================================================
// BODY
// ============================================================================

// EventInfo describes a lifecycle event
type EventInfo struct {
	Event       Event  // The event type
	Phase       Phase  // Genesis phase mapping
	Description string // Human-readable description
	Reversible  bool   // Can the action be undone?
}

// EventCatalog maps events to their info
var EventCatalog = map[Event]EventInfo{
	SessionStart: {
		Event:       SessionStart,
		Phase:       PhaseVoid,
		Description: "Session beginning - state initialization",
		Reversible:  false,
	},
	SessionEnd: {
		Event:       SessionEnd,
		Phase:       PhaseTov,
		Description: "Session ending normally - state persistence",
		Reversible:  false,
	},
	SessionStop: {
		Event:       SessionStop,
		Phase:       PhaseTov,
		Description: "Session interrupted - graceful shutdown",
		Reversible:  false,
	},
	PreCompact: {
		Event:       PreCompact,
		Phase:       PhaseWord,
		Description: "Before context compaction - preserve essential state",
		Reversible:  false,
	},
	SubagentStop: {
		Event:       SubagentStop,
		Phase:       PhaseTov,
		Description: "Subagent task completed",
		Reversible:  false,
	},
	Notification: {
		Event:       Notification,
		Phase:       PhaseWord,
		Description: "System notification received",
		Reversible:  true,
	},
	PromptSubmit: {
		Event:       PromptSubmit,
		Phase:       PhaseVoid,
		Description: "User prompt received - pre-processing",
		Reversible:  false,
	},
	PromptComplete: {
		Event:       PromptComplete,
		Phase:       PhaseTov,
		Description: "Response completed - post-processing",
		Reversible:  false,
	},
	PreToolUse: {
		Event:       PreToolUse,
		Phase:       PhaseVoid,
		Description: "Before tool execution - validation",
		Reversible:  true,
	},
	PostToolUse: {
		Event:       PostToolUse,
		Phase:       PhaseTov,
		Description: "After tool execution - evaluation",
		Reversible:  false,
	},
}

// GetPhase returns the Genesis phase for an event
func (e Event) GetPhase() Phase {
	if info, ok := EventCatalog[e]; ok {
		return info.Phase
	}
	return PhaseWord // Default to word phase
}

// IsReversible returns whether the event's effects can be undone
func (e Event) IsReversible() bool {
	if info, ok := EventCatalog[e]; ok {
		return info.Reversible
	}
	return false
}

// ============================================================================
// CLOSING
// ============================================================================
// This file defines the WHAT of lifecycle events.
// Substrate-specific handlers implement the HOW.
//
// "To every thing there is a season, and a time to every purpose
// under the heaven" — Ecclesiastes 3:1
