// ============================================================================
// METADATA
// ============================================================================
// Hook: PermissionRequest
// Fires: When user is shown a permission dialog (after PreToolUse returns "ask")
// Matchers: Same as PreToolUse tools
//
// State Machine Integration:
//   - GATE: This is a decision point for authority
//   - Fires AFTER PreToolUse when PreToolUse returns "ask"
//   - Can auto-allow, auto-deny, or pass through to user
//
// "Every way of a man is right in his own eyes: but the LORD pondereth the hearts"
// — Proverbs 21:2

package permission

import (
	"encoding/json"
	"fmt"
	"os"
	"strings"

	"cws.studio/pkg/orchestration/logging"
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

	// Initialize logging
	log := logging.New("permission")
	catLog, _ := logging.NewCategoryLogger(logging.CategorySession, input.SessionID)

	// Check for auto-deny patterns first (safety)
	if reason := checkAutoDeny(input); reason != "" {
		output := RequestOutput{
			Decision: "deny",
			Reason:   reason,
		}
		if catLog != nil {
			catLog.Warn("auto_deny", "Permission auto-denied", map[string]string{
				"tool":   input.ToolName,
				"reason": reason,
			})
		}
		json.NewEncoder(os.Stdout).Encode(output)
		return
	}

	// Check for auto-allow patterns (convenience)
	if checkAutoAllow(input) {
		output := RequestOutput{
			Decision: "allow",
		}
		log.Debug("Auto-allowing", map[string]string{"tool": input.ToolName})
		json.NewEncoder(os.Stdout).Encode(output)
		return
	}

	// Default: pass through to user (return empty)
	// This lets the user see and decide on the permission prompt
	log.Debug("Passing to user", map[string]string{"tool": input.ToolName})
}

// checkAutoDeny returns a reason string if the operation should be auto-denied
// Empty string means no auto-deny
func checkAutoDeny(input RequestInput) string {
	switch input.ToolName {
	case "Bash":
		cmd, _ := input.ToolInput["command"].(string)
		cmd = strings.ToLower(cmd)

		// Dangerous system commands
		dangerousPatterns := []struct {
			pattern string
			reason  string
		}{
			{"rm -rf /", "Attempting to delete root filesystem"},
			{"rm -rf /*", "Attempting to delete root filesystem"},
			{":(){:|:&};:", "Fork bomb detected"},
			{"mkfs.", "Filesystem format command detected"},
			{"dd if=/dev/zero of=/dev/sd", "Disk wipe command detected"},
			{"> /dev/sd", "Direct disk write detected"},
			{"chmod -r 777 /", "Dangerous permission change on root"},
			{"wget http", "Downloading from insecure HTTP source"},
			{"curl http", "Downloading from insecure HTTP source"},
			{"| bash", "Piping to bash (potential code injection)"},
			{"| sh", "Piping to shell (potential code injection)"},
			{"eval \"$(curl", "Remote code execution pattern"},
			{"eval \"$(wget", "Remote code execution pattern"},
		}

		for _, dp := range dangerousPatterns {
			if strings.Contains(cmd, dp.pattern) {
				return dp.reason
			}
		}

	case "Write", "Edit":
		filePath, _ := input.ToolInput["file_path"].(string)

		// Protected system files
		protectedPaths := []string{
			"/etc/passwd",
			"/etc/shadow",
			"/etc/sudoers",
			"/etc/hosts",
			"/boot/",
			"/sys/",
			"/proc/",
		}

		for _, protected := range protectedPaths {
			if strings.HasPrefix(filePath, protected) {
				return fmt.Sprintf("Protected system file: %s", protected)
			}
		}

		// Credentials files
		if strings.Contains(filePath, ".ssh/") && strings.HasSuffix(filePath, "_rsa") {
			return "SSH private key modification"
		}
	}

	return ""
}

// checkAutoAllow returns true if the operation should be auto-allowed
func checkAutoAllow(input RequestInput) bool {
	switch input.ToolName {
	case "Read", "Glob", "Grep":
		// Read operations in workspace are generally safe
		return true

	case "WebSearch", "WebFetch":
		// Information gathering is safe
		return true

	case "Bash":
		cmd, _ := input.ToolInput["command"].(string)

		// Safe informational commands
		safeCommands := []string{
			"ls ", "pwd", "whoami", "date", "echo ",
			"cat ", "head ", "tail ", "wc ",
			"git status", "git log", "git diff", "git branch",
			"go version", "node --version", "npm --version",
			"which ", "type ", "file ",
		}

		cmdLower := strings.ToLower(cmd)
		for _, safe := range safeCommands {
			if strings.HasPrefix(cmdLower, safe) {
				return true
			}
		}
	}

	return false
}

// ============================================================================
// CLOSING
// ============================================================================
