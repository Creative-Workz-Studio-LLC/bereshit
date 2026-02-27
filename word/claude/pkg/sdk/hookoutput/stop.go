//omni:key claude-global-pkg-hookoutput-stop
//omni:code go
//omni:version 2.1.0
// ============================================================================
// METADATA
// ============================================================================
//
// Key: claude-global-pkg-hookoutput-stop
// Purpose: Stop / SubagentStop hook types and constructors
// Biblical: "To every thing there is a season, and a time to every purpose" - Ecclesiastes 3:1
// Authors: Nova Dawn
// Version: 2.1.0
// Created: 2026-02-24
// Updated: 2026-02-24
//
// Stop hooks control whether a session or subagent termination proceeds.
// Claude Code uses decision="block" to prevent stopping.
// Gemini CLI uses stopReason to explain why stopping was blocked.
//
// ============================================================================
// END METADATA
// ============================================================================

package hookoutput

// ============================================================================
// SETUP
// ============================================================================

// --- Types ---

// StopResponse is the output for Stop/SubagentStop/AfterAgent hooks.
// Controls whether the session/subagent stop proceeds.
type StopResponse struct {
	Decision   string `json:"decision,omitempty"`   // "block" only, omit for allow
	Reason     string `json:"reason,omitempty"`     // Required with block
	StopReason string `json:"stopReason,omitempty"` // Gemini: explicit stop reason
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// --- Public APIs ---

// NewStopAllow creates a response that allows the stop (empty = proceed)
func NewStopAllow() *StopResponse {
	return &StopResponse{}
}

// NewStopBlock creates a response that blocks the stop
func NewStopBlock(reason string) *StopResponse {
	if IsGemini() {
		return &StopResponse{
			StopReason: reason,
		}
	}
	return &StopResponse{
		Decision: DecisionBlock,
		Reason:   reason,
	}
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Stop hook constructors:
//   NewStopAllow()       — allow stop (empty response)
//   NewStopBlock(reason) — block stop with reason
//
// ============================================================================
// END CLOSING
// ============================================================================
