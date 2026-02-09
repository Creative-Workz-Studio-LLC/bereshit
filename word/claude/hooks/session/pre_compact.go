// ============================================================================
// METADATA
// ============================================================================
// Hook: PreCompact
// Fires: Before context compaction
// Matchers: manual, auto
//
// State Machine Integration:
//   - MEMORY TRANSITION: Context about to be compressed
//   - Snapshot state before compaction for continuity
//   - Record pre_compact event in path
//   - This is a form of death/rebirth - the "old context" dies
//
// Mental Construct Integration:
//   - Like sleep before waking - state persists through the transition
//   - The ROOM's contents change, but identity remains

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

// PreCompactInput from Claude Code
type PreCompactInput struct {
	SessionID          string `json:"session_id"`
	TranscriptPath     string `json:"transcript_path,omitempty"`
	PermissionMode     string `json:"permission_mode,omitempty"`
	HookEventName      string `json:"hook_event_name,omitempty"`
	Trigger            string `json:"trigger"`            // manual, auto
	CustomInstructions string `json:"custom_instructions,omitempty"`
}

// ============================================================================
// BODY
// ============================================================================

// PreCompact handles the PreCompact hook
func PreCompact() {
	log := logging.New("compact")
	log.SetMode(logging.ModeCompact)

	var input PreCompactInput
	if err := json.NewDecoder(os.Stdin).Decode(&input); err != nil {
		log.Error("Failed to decode input", map[string]string{"error": err.Error()})
		os.Exit(1)
	}

	// Create CategoryLogger for file output
	catLog, catErr := logging.NewCategoryLogger(logging.CategoryContext, input.SessionID)
	if catErr != nil {
		log.Warn("CategoryLogger unavailable", map[string]string{"error": catErr.Error()})
	}
	defer func() {
		if catLog != nil {
			catLog.Close()
		}
	}()

	// --- MEMORY TRANSITION: Snapshot state before compaction ---
	state, stateErr := statemachine.LoadRuntimeState()
	currentSection := "B.1"

	if stateErr == nil && state != nil {
		currentSection = state.TrajectorySection

		// Mark that compaction is happening
		state.Session.HooksFired++

		_ = statemachine.SaveRuntimeState(state)
	}

	// Record in path
	if path, err := statemachine.LoadRuntimePath(); err == nil {
		path.RecordEvent("pre_compact_"+input.Trigger, "", currentSection)
		_ = statemachine.SaveRuntimePath(path)
	}

	log.Debug("Memory transition", map[string]string{
		"trigger":    input.Trigger,
		"trajectory": currentSection,
	})
	if catLog != nil {
		catLog.Info("memory_transition", "Context compaction starting", map[string]string{
			"trigger":    input.Trigger,
			"trajectory": currentSection,
		})
	}

	// Output nothing to proceed with compaction
}

// ============================================================================
// CLOSING
// ============================================================================
