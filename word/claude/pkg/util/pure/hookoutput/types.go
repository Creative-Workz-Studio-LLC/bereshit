// ============================================================================
// METADATA
// ============================================================================
//
// Key: claude-global-pkg-hookoutput
// Purpose: Claude Code hook output types - exact schemas Claude Code expects
// Biblical: "A word fitly spoken is like apples of gold in pictures of silver" - Proverbs 25:11
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-21
//
// These types match Claude Code's expected hook output schemas exactly.
// Using incorrect schemas results in JSON validation errors and lost context.
//
// Reference: Claude Code Hooks Documentation
// - hookSpecificOutput.hookEventName is REQUIRED for context injection
// - Different hooks have different valid fields
//
// ============================================================================

package hookoutput

// ============================================================================
// SETUP
// ============================================================================

import "encoding/json"

// Hook event name constants - must match exactly
const (
	EventSessionStart      = "SessionStart"
	EventUserPromptSubmit  = "UserPromptSubmit"
	EventPreToolUse        = "PreToolUse"
	EventPostToolUse       = "PostToolUse"
	EventStop              = "Stop"
	EventSubagentStop      = "SubagentStop"
	EventSubagentStart     = "SubagentStart"     // v2.0.43: When subagent begins
	EventSetup             = "Setup"             // v2.1.10: Via --init, --init-only, --maintenance
	EventPermissionRequest = "PermissionRequest" // v2.0.45: Permission dialog shown
)

// Permission decision values for PreToolUse
const (
	PermissionAllow = "allow" // Bypass permission system, execute automatically
	PermissionDeny  = "deny"  // Block tool call, show reason to Claude
	PermissionAsk   = "ask"   // Ask user to confirm in UI
)

// Decision values for Stop, PostToolUse (when blocking)
const (
	DecisionBlock = "block" // Block/prompt reconsideration
	// For "allow", omit the decision field entirely (don't use "allow")
)

// ============================================================================
// BODY
// ============================================================================

// --- Context Hooks (SessionStart, UserPromptSubmit, PostToolUse) ---

// ContextHookOutput is the hookSpecificOutput for context injection hooks
type ContextHookOutput struct {
	HookEventName     string `json:"hookEventName"`
	AdditionalContext string `json:"additionalContext"`
}

// ContextResponse wraps hookSpecificOutput for context hooks
type ContextResponse struct {
	Decision           string             `json:"decision,omitempty"` // "block" only, omit for allow
	Reason             string             `json:"reason,omitempty"`
	HookSpecificOutput *ContextHookOutput `json:"hookSpecificOutput,omitempty"`
}

// NewContextResponse creates a response with context injection
func NewContextResponse(hookEvent, context string) *ContextResponse {
	if context == "" {
		return &ContextResponse{}
	}
	return &ContextResponse{
		HookSpecificOutput: &ContextHookOutput{
			HookEventName:     hookEvent,
			AdditionalContext: context,
		},
	}
}

// NewSessionStartResponse creates a SessionStart response
func NewSessionStartResponse(context string) *ContextResponse {
	return NewContextResponse(EventSessionStart, context)
}

// NewUserPromptSubmitResponse creates a UserPromptSubmit response
func NewUserPromptSubmitResponse(context string) *ContextResponse {
	return NewContextResponse(EventUserPromptSubmit, context)
}

// NewPostToolUseResponse creates a PostToolUse response
func NewPostToolUseResponse(context string) *ContextResponse {
	return NewContextResponse(EventPostToolUse, context)
}

// NewBlockedPromptResponse creates a response that blocks the prompt
func NewBlockedPromptResponse(reason string) *ContextResponse {
	return &ContextResponse{
		Decision: DecisionBlock,
		Reason:   reason,
	}
}

// NewSetupResponse creates a Setup hook response (v2.1.10)
func NewSetupResponse(context string) *ContextResponse {
	return NewContextResponse(EventSetup, context)
}

// NewSubagentStartResponse creates a SubagentStart hook response (v2.0.43)
func NewSubagentStartResponse(context string) *ContextResponse {
	return NewContextResponse(EventSubagentStart, context)
}

// --- PreToolUse Hook ---

// PreToolHookOutput is the hookSpecificOutput for PreToolUse
type PreToolHookOutput struct {
	HookEventName            string                 `json:"hookEventName"`
	PermissionDecision       string                 `json:"permissionDecision"`
	PermissionDecisionReason string                 `json:"permissionDecisionReason,omitempty"`
	UpdatedInput             map[string]interface{} `json:"updatedInput,omitempty"`
}

// PreToolResponse wraps hookSpecificOutput for PreToolUse
type PreToolResponse struct {
	HookSpecificOutput *PreToolHookOutput `json:"hookSpecificOutput,omitempty"`
}

// NewPreToolResponse creates a PreToolUse response
func NewPreToolResponse(decision, reason string) *PreToolResponse {
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
	return NewPreToolResponse(PermissionAllow, "")
}

// NewPreToolDeny creates a "deny" response (block tool)
func NewPreToolDeny(reason string) *PreToolResponse {
	return NewPreToolResponse(PermissionDeny, reason)
}

// NewPreToolAsk creates an "ask" response (prompt user)
func NewPreToolAsk(reason string) *PreToolResponse {
	return NewPreToolResponse(PermissionAsk, reason)
}

// NewPreToolModified creates an "allow" response with modified inputs
func NewPreToolModified(reason string, updatedInput map[string]interface{}) *PreToolResponse {
	return &PreToolResponse{
		HookSpecificOutput: &PreToolHookOutput{
			HookEventName:            EventPreToolUse,
			PermissionDecision:       PermissionAllow,
			PermissionDecisionReason: reason,
			UpdatedInput:             updatedInput,
		},
	}
}

// --- Stop Hook ---

// StopResponse is the output for Stop/SubagentStop hooks
type StopResponse struct {
	Decision string `json:"decision,omitempty"` // "block" only, omit for allow
	Reason   string `json:"reason,omitempty"`   // Required with block
}

// NewStopAllow creates a response that allows the stop (empty = proceed)
func NewStopAllow() *StopResponse {
	return &StopResponse{}
}

// NewStopBlock creates a response that blocks the stop
func NewStopBlock(reason string) *StopResponse {
	return &StopResponse{
		Decision: DecisionBlock,
		Reason:   reason,
	}
}

// --- Output Helpers ---

// MustJSON marshals response to JSON, panics on error (for hook output)
func MustJSON(v interface{}) []byte {
	data, err := json.Marshal(v)
	if err != nil {
		panic(err)
	}
	return data
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Usage by hook:
//
// SessionStart:     hookoutput.NewSessionStartResponse(context)
// UserPromptSubmit: hookoutput.NewUserPromptSubmitResponse(context)
// PostToolUse:      hookoutput.NewPostToolUseResponse(context)
// PreToolUse:       hookoutput.NewPreToolAllow() / NewPreToolDeny(reason) / NewPreToolAsk(reason)
// Stop:             hookoutput.NewStopAllow() / NewStopBlock(reason)
//
// All responses are JSON-serializable and match Claude Code's expected schemas.
//
