//omni:key claude-global-pkg-hookoutput-pretool
//omni:code go
//omni:version 2.1.0
// ============================================================================
// METADATA
// ============================================================================
//
// Key: claude-global-pkg-hookoutput-pretool
// Purpose: PreToolUse / BeforeTool hook types and constructors
// Biblical: "Try the spirits whether they are of God" - 1 John 4:1
// Authors: Nova Dawn
// Version: 2.1.0
// Created: 2026-02-24
// Updated: 2026-02-24
//
// PreToolUse hooks control whether a tool call proceeds.
// Claude Code uses hookSpecificOutput.permissionDecision.
// Gemini CLI uses top-level decision + exit code 2 to block.
//
// ============================================================================
// END METADATA
// ============================================================================

package hookoutput

// ============================================================================
// SETUP
// ============================================================================

// --- Types ---

// PreToolHookOutput is the hookSpecificOutput for PreToolUse.
// Controls whether a tool call proceeds.
type PreToolHookOutput struct {
	HookEventName            string         `json:"hookEventName,omitempty"`
	PermissionDecision       string         `json:"permissionDecision"`
	PermissionDecisionReason string         `json:"permissionDecisionReason,omitempty"`
	UpdatedInput             map[string]any `json:"updatedInput,omitempty"`
	ToolInput                map[string]any `json:"tool_input,omitempty"` // Gemini: modified tool input
}

// PreToolResponse wraps hookSpecificOutput for PreToolUse/BeforeTool.
// Claude uses hookSpecificOutput.permissionDecision; Gemini uses top-level decision.
type PreToolResponse struct {
	Decision           string             `json:"decision,omitempty"`           // Gemini: top-level decision
	Reason             string             `json:"reason,omitempty"`             // Gemini: top-level reason
	HookSpecificOutput *PreToolHookOutput `json:"hookSpecificOutput,omitempty"` // Claude: nested output
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// --- Public APIs ---

// NewPreToolResponse creates a substrate-aware PreToolUse response
func NewPreToolResponse(decision, reason string) *PreToolResponse {
	if IsGemini() {
		return &PreToolResponse{
			Decision: decision,
			Reason:   reason,
		}
	}
	return &PreToolResponse{
		HookSpecificOutput: &PreToolHookOutput{
			HookEventName:            EventPreToolUse,
			PermissionDecision:       decision,
			PermissionDecisionReason: reason,
		},
	}
}

// NewPreToolAllow creates an "allow" response (bypass permission)
func NewPreToolAllow() *PreToolResponse {
	if IsGemini() {
		return &PreToolResponse{} // Empty = allow on Gemini
	}
	return NewPreToolResponse(PermissionAllow, "")
}

// NewPreToolDeny creates a "deny" response (block tool)
func NewPreToolDeny(reason string) *PreToolResponse {
	return NewPreToolResponse(PermissionDeny, reason)
}

// NewPreToolAsk creates an "ask" response (prompt user — Claude only)
func NewPreToolAsk(reason string) *PreToolResponse {
	if IsGemini() {
		// Gemini doesn't have "ask" — treat as deny with explanation
		return NewPreToolResponse(PermissionDeny, reason)
	}
	return NewPreToolResponse(PermissionAsk, reason)
}

// NewPreToolModified creates an "allow" response with modified inputs
func NewPreToolModified(reason string, updatedInput map[string]any) *PreToolResponse {
	if IsGemini() {
		return &PreToolResponse{
			HookSpecificOutput: &PreToolHookOutput{
				ToolInput: updatedInput,
			},
		}
	}
	return &PreToolResponse{
		HookSpecificOutput: &PreToolHookOutput{
			HookEventName:            EventPreToolUse,
			PermissionDecision:       PermissionAllow,
			PermissionDecisionReason: reason,
			UpdatedInput:             updatedInput,
		},
	}
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// PreToolUse constructors:
//   NewPreToolResponse(decision, reason) — generic, substrate-aware
//   NewPreToolAllow()                    — bypass permission
//   NewPreToolDeny(reason)               — block tool call
//   NewPreToolAsk(reason)                — prompt user (Claude only)
//   NewPreToolModified(reason, input)    — allow with modified inputs
//
// ============================================================================
// END CLOSING
// ============================================================================
