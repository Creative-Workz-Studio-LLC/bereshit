// ============================================================================
// METADATA
// ============================================================================
// Hook: Stop
// Fires: When main Claude agent finishes responding
// Can block and request continuation
//
// State Machine Integration:
//   - HALT EVALUATION: Check if work is truly complete
//   - Record stop event in path
//   - Track halt_reached in state
//   - Consider trajectory (B.4 = completion phase)
//   - Consider todos (all completed = natural stopping point)
//
// Mental Construct Integration:
//   - This is the moment the DOOR CLOSES
//   - Natural stopping points should align with trajectory B.4 or all todos complete
//   - Incomplete work detected = could request continuation

package session

import (
	"encoding/json"
	"fmt"
	"os"

	"creativeworkzstudio.com/bereshit/word/work/pkg/util/pure/hookoutput"
	"creativeworkzstudio.com/bereshit/word/work/pkg/orchestration/logging"
	"creativeworkzstudio.com/bereshit/word/work/pkg/core/statemachine"
)

// ============================================================================
// SETUP
// ============================================================================

// StopInput from Claude Code
type StopInput struct {
	SessionID      string `json:"session_id"`
	TranscriptPath string `json:"transcript_path,omitempty"`
	PermissionMode string `json:"permission_mode,omitempty"`
	HookEventName  string `json:"hook_event_name,omitempty"`
	StopHookActive bool   `json:"stop_hook_active"`
}

// StopOutput uses hookoutput.StopResponse for correct schema
// Old: {"decision": "allow"} - INVALID, "allow" not recognized
// New: {} for allow (empty), {"decision": "block", "reason": "..."} for block

// ============================================================================
// BODY
// ============================================================================

// Stop handles the Stop hook
func Stop() {
	log := logging.New("stop")
	log.SetMode(logging.ModeCompact)

	var input StopInput
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

	// --- HALT EVALUATION: Assess completion state ---
	state, stateErr := statemachine.LoadRuntimeState()
	currentSection := "B.1"
	haltType := "natural" // natural, incomplete, forced

	if stateErr == nil && state != nil {
		currentSection = state.TrajectorySection

		// Evaluate halt conditions
		haltType = evaluateHaltType(state)

		// Track halt in state
		state.Session.LastHaltReached = true
		state.Session.LastHaltType = haltType
		state.Session.LastHaltTrigger = "agent_stop"

		_ = statemachine.SaveRuntimeState(state)
	}

	// Record stop event in path
	if path, err := statemachine.LoadRuntimePath(); err == nil {
		path.RecordEvent("agent_stop", haltType, currentSection)
		_ = statemachine.SaveRuntimePath(path)
	}

	log.Debug("Stop - door closing", map[string]string{
		"session_id": input.SessionID,
		"trajectory": currentSection,
		"halt_type":  haltType,
	})
	if catLog != nil {
		catLog.Info("door_closing", "Agent stop - halt evaluation", map[string]string{
			"trajectory": currentSection,
			"halt_type":  haltType,
		})
	}

	// Check stop conditions
	shouldBlock, reason := evaluateStop(log, state, input)

	// Use correct Claude Code schema
	var output *hookoutput.StopResponse
	if shouldBlock {
		output = hookoutput.NewStopBlock(reason)
		log.Warn("Stop blocked", map[string]string{"reason": reason})
		if catLog != nil {
			catLog.Warn("stop_blocked", "Stop blocked - work incomplete", map[string]string{
				"reason": reason,
			})
		}
	} else {
		output = hookoutput.NewStopAllow()
		if catLog != nil {
			catLog.Success("stop_allowed", "Natural stopping point", map[string]string{
				"trajectory": currentSection,
			})
		}
	}

	json.NewEncoder(os.Stdout).Encode(output)
}

// evaluateHaltType determines the type of halt based on state
func evaluateHaltType(state *statemachine.RuntimeState) string {
	if state == nil {
		return "unknown"
	}

	tasks := state.Session.Tasks
	trajectory := state.TrajectorySection

	// B.4 + all todos complete = natural completion
	if trajectory == "B.4" && tasks.Total > 0 && tasks.Completed == tasks.Total {
		return "complete" // Work finished, natural halt
	}

	// B.4 but todos incomplete = near completion
	if trajectory == "B.4" {
		return "near_complete"
	}

	// All todos done regardless of trajectory = task-complete halt
	if tasks.Total > 0 && tasks.Completed == tasks.Total {
		return "task_complete"
	}

	// No todos and any trajectory = natural pause
	if tasks.Total == 0 {
		return "natural"
	}

	// Todos exist but incomplete = incomplete halt
	if tasks.Pending > 0 || tasks.InProgress > 0 {
		return "incomplete"
	}

	return "natural"
}

// evaluateStop determines whether to block the stop
// Returns: (shouldBlock bool, reason string)
func evaluateStop(log *logging.Logger, state *statemachine.RuntimeState, input StopInput) (bool, string) {
	if state == nil {
		return false, "" // No state = allow
	}

	tasks := state.Session.Tasks

	// If there are in-progress todos, consider blocking
	// But only if this is a reasonable intervention point
	if tasks.InProgress > 0 && tasks.Total > 2 {
		// Check if we've been working long enough to justify blocking
		if state.Session.HooksFired > 10 {
			// Could block here, but for now just log the state
			log.Warn("Incomplete work detected", map[string]string{
				"in_progress": fmt.Sprintf("%d", tasks.InProgress),
				"pending":     fmt.Sprintf("%d", tasks.Pending),
				"completed":   fmt.Sprintf("%d", tasks.Completed),
			})
			// Uncomment to enable blocking:
			// return true, fmt.Sprintf("Work in progress: %d tasks incomplete", tasks.InProgress+tasks.Pending)
		}
	}

	// Default: allow stop
	return false, ""
}

// ============================================================================
// CLOSING
// ============================================================================
