// ============================================================================
// METADATA
// ============================================================================
// Hook: PermissionRequest
// Fires: When user is shown a permission dialog
// Matchers: Same as PreToolUse tools

package permission

import (
	"encoding/json"
	"fmt"
	"os"
)

// ============================================================================
// SETUP
// ============================================================================

// RequestInput from Claude Code (similar to PreToolUse for permission dialogs)
type RequestInput struct {
	SessionID      string                 `json:"session_id"`
	TranscriptPath string                 `json:"transcript_path,omitempty"`
	CWD            string                 `json:"cwd,omitempty"`
	PermissionMode string                 `json:"permission_mode,omitempty"`
	HookEventName  string                 `json:"hook_event_name,omitempty"`
	ToolName       string                 `json:"tool_name"`
	ToolInput      map[string]interface{} `json:"tool_input"`
	ToolUseID      string                 `json:"tool_use_id,omitempty"`
}

// RequestOutput to control behavior
type RequestOutput struct {
	Decision      string                 `json:"decision"`                 // allow, deny
	Reason        string                 `json:"reason,omitempty"`         // Required if deny
	ModifiedInput map[string]interface{} `json:"modified_input,omitempty"` // Optional when allowing
}

// ============================================================================
// BODY
// ============================================================================

// Request handles the PermissionRequest hook
func Request() {
	var input RequestInput
	if err := json.NewDecoder(os.Stdin).Decode(&input); err != nil {
		fmt.Fprintf(os.Stderr, "Failed to decode input: %v\n", err)
		os.Exit(1)
	}

	// TODO: Implement permission request logic
	// - Auto-allow safe operations
	// - Auto-deny dangerous operations
	// - Modify input when allowing
	// - Custom deny messages

	// Default: don't intercept (let user decide)
	// Return empty to pass through to user
}

// ============================================================================
// CLOSING
// ============================================================================
