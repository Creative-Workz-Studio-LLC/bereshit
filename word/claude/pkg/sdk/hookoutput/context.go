//omni:key claude-global-pkg-hookoutput-context
//omni:code go
//omni:version 2.1.0
// ============================================================================
// METADATA
// ============================================================================
//
// Key: claude-global-pkg-hookoutput-context
// Purpose: Context injection hook types and constructors
// Biblical: "The entrance of thy words giveth light" - Psalm 119:130
// Authors: Nova Dawn
// Version: 2.1.0
// Created: 2026-02-24
// Updated: 2026-02-24
//
// Context hooks inject additional information into the substrate's
// conversation. Used by SessionStart, UserPromptSubmit, PostToolUse,
// Setup, and SubagentStart events.
//
// ============================================================================
// END METADATA
// ============================================================================

package hookoutput

// ============================================================================
// SETUP
// ============================================================================

// --- Types ---

// ContextHookOutput is the hookSpecificOutput for context injection hooks.
// Used by SessionStart, UserPromptSubmit, PostToolUse, Setup, SubagentStart.
type ContextHookOutput struct {
	HookEventName     string `json:"hookEventName,omitempty"` // Required for Claude, optional for Gemini
	AdditionalContext string `json:"additionalContext"`
}

// ContextResponse wraps hookSpecificOutput for context hooks.
// Supports both Claude Code and Gemini CLI response schemas.
type ContextResponse struct {
	Decision           string             `json:"decision,omitempty"`           // "block" only, omit for allow
	Reason             string             `json:"reason,omitempty"`
	SystemMessage      string             `json:"systemMessage,omitempty"`      // Gemini: user-facing feedback
	Continue           *bool              `json:"continue,omitempty"`           // Gemini: whether to continue
	HookSpecificOutput *ContextHookOutput `json:"hookSpecificOutput,omitempty"`
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// --- Public APIs ---

// NewContextResponse creates a substrate-aware response with context injection
func NewContextResponse(hookEvent, context string) *ContextResponse {
	if context == "" {
		return &ContextResponse{}
	}

	resp := &ContextResponse{
		HookSpecificOutput: &ContextHookOutput{
			AdditionalContext: context,
		},
	}

	// Claude Code requires hookEventName; Gemini ignores it but it doesn't hurt
	resp.HookSpecificOutput.HookEventName = hookEvent

	return resp
}

// NewSessionStartResponse creates a SessionStart response (both substrates)
func NewSessionStartResponse(context string) *ContextResponse {
	return NewContextResponse(EventSessionStart, context)
}

// NewUserPromptSubmitResponse creates a UserPromptSubmit/BeforeAgent response
func NewUserPromptSubmitResponse(context string) *ContextResponse {
	if IsGemini() {
		return NewContextResponse(GeminiEventBeforeAgent, context)
	}
	return NewContextResponse(EventUserPromptSubmit, context)
}

// NewPostToolUseResponse creates a PostToolUse/AfterTool response
func NewPostToolUseResponse(context string) *ContextResponse {
	if IsGemini() {
		return NewContextResponse(GeminiEventAfterTool, context)
	}
	return NewContextResponse(EventPostToolUse, context)
}

// NewBlockedPromptResponse creates a response that blocks the prompt
func NewBlockedPromptResponse(reason string) *ContextResponse {
	if IsGemini() {
		// Gemini uses systemMessage + continue=false to block
		cont := false
		return &ContextResponse{
			SystemMessage: reason,
			Continue:      &cont,
		}
	}
	return &ContextResponse{
		Decision: DecisionBlock,
		Reason:   reason,
	}
}

// NewSetupResponse creates a Setup hook response (Claude-only, v2.1.10)
func NewSetupResponse(context string) *ContextResponse {
	return NewContextResponse(EventSetup, context)
}

// NewSubagentStartResponse creates a SubagentStart hook response (Claude-only, v2.0.43)
func NewSubagentStartResponse(context string) *ContextResponse {
	return NewContextResponse(EventSubagentStart, context)
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Context hook constructors:
//   NewContextResponse(hookEvent, context)  — generic, substrate-aware
//   NewSessionStartResponse(context)        — SessionStart
//   NewUserPromptSubmitResponse(context)     — UserPromptSubmit / BeforeAgent
//   NewPostToolUseResponse(context)          — PostToolUse / AfterTool
//   NewBlockedPromptResponse(reason)         — Block the prompt
//   NewSetupResponse(context)                — Setup (Claude-only)
//   NewSubagentStartResponse(context)        — SubagentStart (Claude-only)
//
// ============================================================================
// END CLOSING
// ============================================================================
