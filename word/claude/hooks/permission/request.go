// ============================================================================
// METADATA
// ============================================================================
// Hook: PermissionRequest
// Fires: When user is shown a permission dialog
// Added: Claude Code v2.0.45
// Matchers: Same as PreToolUse tools
//
// Output options:
//   - {"decision": "allow"} - Approve the request
//   - {"decision": "deny", "reason": "..."} - Deny with reason
//   - {} or no output - Let user decide (default behavior)
//
// New in v2.0.54: Can also apply permission updates (always allow suggestions)
// Traces to: settings.json hooks.PermissionRequest

package permission

import (
	"encoding/json"
	"os"

	"github.com/creativeworkzstudio/claude-global/pkg/orchestration/logging"
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
	log := logging.New("permission")
	log.SetMode(logging.ModeCompact)

	var input RequestInput
	if err := json.NewDecoder(os.Stdin).Decode(&input); err != nil {
		log.Error("Failed to decode PermissionRequest input", map[string]string{"error": err.Error()})
		os.Exit(1)
	}

	// Create CategoryLogger for file output
	catLog, catErr := logging.NewCategoryLogger(logging.CategorySession, input.SessionID)
	if catErr != nil {
		log.Warn("CategoryLogger unavailable", map[string]string{
			"error": catErr.Error(),
		})
	}
	defer func() {
		if catLog != nil {
			catLog.Close()
		}
	}()

	// Log the permission request
	logFields := map[string]string{
		"session_id": input.SessionID,
		"tool_name":  input.ToolName,
	}
	if input.ToolUseID != "" {
		logFields["tool_use_id"] = input.ToolUseID
	}

	log.Info("Permission requested", logFields)
	if catLog != nil {
		catLog.Info("permission_request", "Permission requested", logFields)
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
