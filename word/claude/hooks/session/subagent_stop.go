// ============================================================================
// METADATA
// ============================================================================
// Hook: SubagentStop
// Fires: When a subagent (Task tool) finishes responding
// Can block and request continuation
//
// State Machine Integration:
//   - DELEGATION COMPLETE: Subagent finished its work
//   - Record subagent_stop event in path
//   - Track delegation patterns (Task tool = expansion +1)
//
// Mental Construct Integration:
//   - Delegation is expansion (+1) - trusting work to another
//   - Subagent completion = partial task complete

package session

import (
	"encoding/json"
	"os"

	"cws.studio/pkg/orchestration/logging"
	"cws.studio/pkg/core/statemachine"
	"cws.studio/pkg/sdk/hookoutput"
)

// ============================================================================
// SETUP
// ============================================================================

// SubagentStopInput from Claude Code
// Updated for CC v2.1.47: added agent_id, agent_transcript_path, last_assistant_message
type SubagentStopInput struct {
	SessionID            string `json:"session_id"`
	TranscriptPath       string `json:"transcript_path,omitempty"`
	PermissionMode       string `json:"permission_mode,omitempty"`
	HookEventName        string `json:"hook_event_name,omitempty"`
	StopHookActive       bool   `json:"stop_hook_active"`
	AgentID              string `json:"agent_id,omitempty"`              // v2.0.42: which agent stopped
	AgentTranscriptPath  string `json:"agent_transcript_path,omitempty"` // v2.0.42: agent's transcript file
	LastAssistantMessage string `json:"last_assistant_message,omitempty"` // v2.1.47: final response text
}

// Output: use hookoutput.StopResponse (shared with Stop hook)

// ============================================================================
// BODY
// ============================================================================

// SubagentStop handles the SubagentStop hook
func SubagentStop() {
	log := logging.New("subagent")
	log.SetMode(logging.ModeCompact)

	var input SubagentStopInput
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

	// --- DELEGATION COMPLETE: Track subagent outcome ---
	state, stateErr := statemachine.LoadRuntimeState()
	currentSection := "B.1"

	if stateErr == nil && state != nil {
		currentSection = state.TrajectorySection

		// Delegation completion is a form of success (expansion worked)
		// Slight positive alignment (delegation succeeded)
		state.Session.KAlign += 0.01
		if state.Session.KAlign > 1.0 {
			state.Session.KAlign = 1.0
		}

		_ = statemachine.SaveRuntimeState(state)
	}

	// Record subagent stop event in path
	if path, err := statemachine.LoadRuntimePath(); err == nil {
		path.RecordEvent("subagent_stop", "", currentSection)
		_ = statemachine.SaveRuntimePath(path)
	}

	log.Debug("Subagent complete", map[string]string{
		"session_id": input.SessionID,
		"trajectory": currentSection,
		"agent_id":   input.AgentID,
	})
	if catLog != nil {
		catLog.Success("delegation_complete", "Subagent task finished", map[string]string{
			"trajectory":       currentSection,
			"agent_id":         input.AgentID,
			"transcript_path":  input.AgentTranscriptPath,
		})
	}

	// Default: allow stop (use hookoutput.StopResponse for correct schema)
	output := hookoutput.NewStopAllow()
	json.NewEncoder(os.Stdout).Encode(output)
}

// ============================================================================
// CLOSING
// ============================================================================
