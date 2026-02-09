// ============================================================================
// METADATA
// ============================================================================
// Hook: Setup
// Fires: Via --init, --init-only, or --maintenance CLI flags
// Purpose: Repository setup and maintenance operations
//
// This hook enables:
//   - One-time repository initialization
//   - Periodic maintenance tasks
//   - Environment validation before work begins
//
// Traces to: v2.1.10 release notes - Setup hook event

package session

import (
	"encoding/json"
	"fmt"
	"os"
	"strings"

	"cws.studio/pkg/orchestration/logging"
	"cws.studio/pkg/util/pure/hookoutput"
)

// ============================================================================
// SETUP
// ============================================================================

// SetupInput from Claude Code
type SetupInput struct {
	SessionID      string `json:"session_id"`
	TranscriptPath string `json:"transcript_path,omitempty"`
	CWD            string `json:"cwd,omitempty"`
	PermissionMode string `json:"permission_mode,omitempty"`
	HookEventName  string `json:"hook_event_name,omitempty"`
	TriggerFlag    string `json:"trigger_flag,omitempty"` // init, init-only, maintenance
}

// ============================================================================
// BODY
// ============================================================================

// Setup handles the Setup hook
func Setup() {
	log := logging.New("setup")
	log.SetMode(logging.ModeCompact)

	var input SetupInput
	if err := json.NewDecoder(os.Stdin).Decode(&input); err != nil {
		log.Error("Failed to decode input", map[string]string{"error": err.Error()})
		os.Exit(1)
	}

	// Create CategoryLogger for file output
	catLog, catErr := logging.NewCategoryLogger(logging.CategorySession, input.SessionID)
	if catErr != nil {
		log.Warn("CategoryLogger unavailable", map[string]string{"error": catErr.Error()})
	}
	defer func() {
		if catLog != nil {
			catLog.Close()
		}
	}()

	log.Info("Setup hook triggered", map[string]string{
		"session_id":   input.SessionID,
		"trigger":      input.TriggerFlag,
		"working_dir":  input.CWD,
	})

	// Repository validation
	var issues []string
	var info []string

	// Check for CLAUDE.md (identity loading)
	if !fileExists(input.CWD + "/CLAUDE.md") {
		issues = append(issues, "Missing CLAUDE.md - identity loading may be incomplete")
	} else {
		info = append(info, "CLAUDE.md present")
	}

	// Check for .omni files (OmniCode structure)
	if fileExists(input.CWD + "/.omni") {
		info = append(info, "OmniCode .omni present")
	}

	// Check for go.mod (Go project)
	if fileExists(input.CWD + "/go.mod") {
		info = append(info, "Go module detected")
	}

	// Check for go.work (workspace)
	if fileExists(input.CWD + "/go.work") {
		info = append(info, "Go workspace detected")
	}

	// Check for Makefile (build automation)
	if fileExists(input.CWD + "/Makefile") {
		info = append(info, "Makefile present")
	}

	// Check for word/ directory (Bereshit structure)
	if dirExists(input.CWD + "/word") {
		info = append(info, "Bereshit structure (word/) detected")
	}

	// Build context message
	var contextParts []string
	contextParts = append(contextParts, fmt.Sprintf("Setup complete for: %s", input.CWD))

	if len(info) > 0 {
		contextParts = append(contextParts, "Structure: "+strings.Join(info, ", "))
	}

	if len(issues) > 0 {
		contextParts = append(contextParts, "⚠️ Issues: "+strings.Join(issues, "; "))
	}

	context := strings.Join(contextParts, "\n")

	if catLog != nil {
		catLog.Success("setup_complete", "Setup hook executed", map[string]string{
			"trigger":     input.TriggerFlag,
			"working_dir": input.CWD,
		})
	}

	// Output context for Claude
	output := hookoutput.NewSetupResponse(context)
	json.NewEncoder(os.Stdout).Encode(output)
}

// ============================================================================
// CLOSING
// ============================================================================

// fileExists checks if a file exists at the given path
func fileExists(path string) bool {
	info, err := os.Stat(path)
	if err != nil {
		return false
	}
	return !info.IsDir()
}

// dirExists checks if a directory exists at the given path
func dirExists(path string) bool {
	info, err := os.Stat(path)
	if err != nil {
		return false
	}
	return info.IsDir()
}
