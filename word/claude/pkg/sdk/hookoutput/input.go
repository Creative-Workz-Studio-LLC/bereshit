//omni:key claude-global-pkg-hookoutput-input
//omni:code go
//omni:version 2.1.0
// ============================================================================
// METADATA
// ============================================================================
//
// Key: claude-global-pkg-hookoutput-input
// Purpose: Universal hook input parsing — substrate-agnostic input struct
// Biblical: "Hear, and your soul shall live" - Isaiah 55:3
// Authors: Nova Dawn
// Version: 2.1.0
// Created: 2026-02-24
// Updated: 2026-02-24
//
// HookInput is the universal struct for parsing JSON input from stdin.
// Fields from both Claude Code and Gemini CLI are represented.
// Unknown fields are silently ignored during JSON unmarshaling.
//
// ============================================================================
// END METADATA
// ============================================================================

package hookoutput

// ============================================================================
// SETUP
// ============================================================================

// --- Types ---

// HookInput is a substrate-agnostic hook input struct.
// Fields from both Claude Code and Gemini CLI are represented.
// Unknown fields are silently ignored during JSON unmarshaling.
type HookInput struct {
	// Common fields (both substrates)
	SessionID      string `json:"session_id"`
	TranscriptPath string `json:"transcript_path,omitempty"`
	CWD            string `json:"cwd,omitempty"`
	HookEventName  string `json:"hook_event_name,omitempty"`
	Timestamp      string `json:"timestamp,omitempty"`

	// SessionStart
	Source string `json:"source,omitempty"` // startup, resume, clear, compact

	// Tool hooks
	ToolName  string         `json:"tool_name,omitempty"`
	ToolInput map[string]any `json:"tool_input,omitempty"`

	// Claude-specific
	ToolUseID      string `json:"tool_use_id,omitempty"`
	PermissionMode string `json:"permission_mode,omitempty"`

	// Gemini-specific
	Prompt         string `json:"prompt,omitempty"`          // BeforeAgent
	PromptResponse string `json:"prompt_response,omitempty"` // AfterAgent
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Pure data struct — no functions in BODY.
// Unmarshal stdin JSON into HookInput:
//
//   var input hookoutput.HookInput
//   json.NewDecoder(os.Stdin).Decode(&input)
//
// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// HookInput field sources:
//   Common:  session_id, transcript_path, cwd, hook_event_name, timestamp
//   Session: source (startup, resume, clear, compact)
//   Tool:    tool_name, tool_input
//   Claude:  tool_use_id, permission_mode
//   Gemini:  prompt, prompt_response
//
// ============================================================================
// END CLOSING
// ============================================================================
