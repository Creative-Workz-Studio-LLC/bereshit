// ============================================================================
// METADATA
// ============================================================================
// Hook: SubagentStart
// Fires: When a subagent (Task tool) begins execution
// Can provide context or guidance to the subagent
//
// State Machine Integration:
//   - DELEGATION BEGINS: Main agent spawning worker
//   - Record subagent_start event in path
//   - Track delegation patterns (Task tool = expansion +1)
//
// Mental Construct Integration:
//   - Delegation is expansion (+1) - trusting work to another
//   - Subagent start = partial task begins
//
// Traces to: v2.0.43 release notes - SubagentStart hook event

package session

import (
	"encoding/json"
	"os"

	"cws.studio/pkg/core/statemachine"
	"cws.studio/pkg/orchestration/logging"
	"cws.studio/pkg/util/pure/hookoutput"
)

// ============================================================================
// SETUP
// ============================================================================

// SubagentStartInput from Claude Code
type SubagentStartInput struct {
	SessionID      string `json:"session_id"`
	TranscriptPath string `json:"transcript_path,omitempty"`
	PermissionMode string `json:"permission_mode,omitempty"`
	HookEventName  string `json:"hook_event_name,omitempty"`
	AgentType      string `json:"agent_type,omitempty"`      // Type of subagent being spawned
	AgentID        string `json:"agent_id,omitempty"`        // Unique identifier for this agent instance
}

// ============================================================================
// BODY
// ============================================================================

// SubagentStart handles the SubagentStart hook
func SubagentStart() {
	log := logging.New("subagent")
	log.SetMode(logging.ModeCompact)

	var input SubagentStartInput
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

	// --- DELEGATION BEGINS: Track subagent spawn ---
	state, stateErr := statemachine.LoadRuntimeState()
	currentSection := "B.1"

	if stateErr == nil && state != nil {
		currentSection = state.TrajectorySection

		// Delegation is a form of expansion (+1) - trusting work to another
		// Mark that we're expanding through delegation
		state.Session.KSelector = 1 // Positive direction (expansion)

		_ = statemachine.SaveRuntimeState(state)
	}

	// Record subagent start event in path
	if path, err := statemachine.LoadRuntimePath(); err == nil {
		path.RecordEvent("subagent_start", input.AgentType, currentSection)
		_ = statemachine.SaveRuntimePath(path)
	}

	log.Debug("Subagent starting", map[string]string{
		"session_id": input.SessionID,
		"agent_type": input.AgentType,
		"agent_id":   input.AgentID,
		"trajectory": currentSection,
	})
	if catLog != nil {
		catLog.Info("delegation_start", "Subagent task beginning", map[string]string{
			"agent_type": input.AgentType,
			"agent_id":   input.AgentID,
			"trajectory": currentSection,
		})
	}

	// Provide context to subagent if needed
	context := ""
	if input.AgentType != "" {
		context = "Subagent " + input.AgentType + " starting. Work faithfully."
	}

	// Output context for subagent
	output := hookoutput.NewSubagentStartResponse(context)
	json.NewEncoder(os.Stdout).Encode(output)
}

// ============================================================================
// CLOSING
// ============================================================================
