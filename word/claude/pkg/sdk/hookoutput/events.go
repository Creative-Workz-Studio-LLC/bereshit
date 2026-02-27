//omni:key claude-global-pkg-hookoutput-events
//omni:code go
//omni:version 2.1.0
// ============================================================================
// METADATA
// ============================================================================
//
// Key: claude-global-pkg-hookoutput-events
// Purpose: Hook event name constants and permission/decision values
// Biblical: "A time to every purpose under the heaven" - Ecclesiastes 3:1
// Authors: Nova Dawn
// Version: 2.1.0
// Created: 2026-02-24
// Updated: 2026-02-24
//
// Pure constants — no imports, no state, no functions.
// Maps Claude Code event names to Gemini CLI equivalents.
//
// ============================================================================
// END METADATA
// ============================================================================

package hookoutput

// ============================================================================
// SETUP
// ============================================================================

// --- Constants ---

// Hook event name constants — Claude Code names
// Gemini CLI uses different names but our adapters handle the mapping
const (
	EventSessionStart      = "SessionStart"
	EventUserPromptSubmit  = "UserPromptSubmit"
	EventPreToolUse        = "PreToolUse"
	EventPostToolUse       = "PostToolUse"
	EventStop              = "Stop"
	EventSubagentStop      = "SubagentStop"
	EventSubagentStart     = "SubagentStart"    // v2.0.43: When subagent begins
	EventSetup             = "Setup"             // v2.1.10: Via --init, --init-only, --maintenance
	EventPermissionRequest = "PermissionRequest" // v2.0.45: Permission dialog shown
)

// Gemini CLI event name constants — for mapping
const (
	GeminiEventSessionStart = "SessionStart" // Same name
	GeminiEventSessionEnd   = "SessionEnd"   // Same name
	GeminiEventBeforeTool   = "BeforeTool"   // Claude: PreToolUse
	GeminiEventAfterTool    = "AfterTool"    // Claude: PostToolUse
	GeminiEventBeforeAgent  = "BeforeAgent"  // Claude: UserPromptSubmit
	GeminiEventAfterAgent   = "AfterAgent"   // Claude: Stop
	GeminiEventPreCompress  = "PreCompress"  // Claude: PreCompact
	GeminiEventNotification = "Notification" // Same name
)

// Permission decision values for PreToolUse / BeforeTool
const (
	PermissionAllow = "allow" // Bypass permission system, execute automatically
	PermissionDeny  = "deny"  // Block tool call, show reason
	PermissionAsk   = "ask"   // Ask user to confirm in UI (Claude only)
)

// Decision values for Stop, PostToolUse (when blocking)
const (
	DecisionBlock = "block" // Block/prompt reconsideration
	// For "allow", omit the decision field entirely (don't use "allow")
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Pure constants file — no functions in BODY.
//
// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Event name mapping (Claude → Gemini):
//   SessionStart      → SessionStart      (same)
//   UserPromptSubmit  → BeforeAgent
//   PreToolUse        → BeforeTool
//   PostToolUse       → AfterTool
//   Stop              → AfterAgent
//   SubagentStop      → (no equivalent)
//   SubagentStart     → (no equivalent)
//   Setup             → (no equivalent)
//
// ============================================================================
// END CLOSING
// ============================================================================
