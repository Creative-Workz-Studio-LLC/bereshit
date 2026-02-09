// ============================================================================
// METADATA
// ============================================================================
// Hook: TaskCompleted
// Fires: When a task is being marked complete in agent teams
// Can block: Exit code 2 = reject completion, send feedback
//
// Agent Teams Integration (v2.1.33):
//   - Task in shared task list being marked complete
//   - Quality gate: can reject completion with feedback
//   - Tracks task completion patterns for team coordination
//
// Mental Construct Integration:
//   - Task completion = building block complete (+1)
//   - Quality checkpoint before accepting work

package session

import (
	"encoding/json"
	"os"

	"cws.studio/pkg/orchestration/logging"
	"cws.studio/pkg/core/statemachine"
)

// ============================================================================
// SETUP
// ============================================================================

// TaskCompletedInput from Claude Code
type TaskCompletedInput struct {
	SessionID     string `json:"session_id"`
	HookEventName string `json:"hook_event_name,omitempty"`
	TaskID        string `json:"task_id,omitempty"`
	TaskSubject   string `json:"task_subject,omitempty"`
}

// TaskCompletedOutput to control behavior
// Exit code 2 with "reason" rejects completion and sends feedback
type TaskCompletedOutput struct {
	Decision string `json:"decision"` // allow, block
	Reason   string `json:"reason,omitempty"`
}

// ============================================================================
// BODY
// ============================================================================

// TaskCompleted handles the TaskCompleted hook
func TaskCompleted() {
	log := logging.New("task")
	log.SetMode(logging.ModeCompact)

	var input TaskCompletedInput
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

	// --- TASK COMPLETE: Track completion ---
	state, stateErr := statemachine.LoadRuntimeState()
	currentSection := "B.1"

	if stateErr == nil && state != nil {
		currentSection = state.TrajectorySection

		// Task completion is positive alignment (work finished)
		state.Session.KAlign += 0.02
		if state.Session.KAlign > 1.0 {
			state.Session.KAlign = 1.0
		}

		_ = statemachine.SaveRuntimeState(state)
	}

	// Record task completed event in path
	if path, err := statemachine.LoadRuntimePath(); err == nil {
		path.RecordEvent("task_completed", input.TaskID, currentSection)
		_ = statemachine.SaveRuntimePath(path)
	}

	log.Debug("Task completed", map[string]string{
		"session_id": input.SessionID,
		"task_id":    input.TaskID,
		"task":       input.TaskSubject,
		"trajectory": currentSection,
	})
	if catLog != nil {
		catLog.Success("task_completed", "Task marked complete", map[string]string{
			"task_id":    input.TaskID,
			"task":       input.TaskSubject,
			"trajectory": currentSection,
		})
	}

	// Default: allow completion (exit 0)
	// To reject completion: exit code 2 with reason
	output := TaskCompletedOutput{Decision: "allow"}
	json.NewEncoder(os.Stdout).Encode(output)
}

// ============================================================================
// CLOSING
// ============================================================================
