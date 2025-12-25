// ============================================================================
// METADATA
// ============================================================================
// Hook: SessionEnd
// Fires: When Claude Code session ends
// Cannot block termination
//
// State Machine Integration:
//   - Mark session inactive
//   - Finalize path (set ended_at, compute summary)
//   - Record session_end event

package session

import (
	"context"
	"encoding/json"
	"os"
	"time"

	"cws.studio/claude/hooks/internal"
	"creativeworkzstudio.com/bereshit/word/work/pkg/core/statemachine"
	"creativeworkzstudio.com/bereshit/word/work/pkg/orchestration/logging"
)

// ============================================================================
// SETUP
// ============================================================================

// EndInput from Claude Code
type EndInput struct {
	SessionID      string `json:"session_id"`
	TranscriptPath string `json:"transcript_path,omitempty"`
	PermissionMode string `json:"permission_mode,omitempty"`
	HookEventName  string `json:"hook_event_name,omitempty"`
	Reason         string `json:"reason"` // clear, logout, prompt_input_exit, other
}

// ============================================================================
// BODY
// ============================================================================

// End handles the SessionEnd hook
func End() {
	log := logging.New("session")
	log.SetMode(logging.ModeCompact)

	var input EndInput
	if err := json.NewDecoder(os.Stdin).Decode(&input); err != nil {
		log.Error("Failed to decode input", map[string]string{"error": err.Error()})
		os.Exit(1)
	}

	// Create CategoryLogger for file output (append to data/logs/session/)
	catLog, catErr := logging.NewCategoryLogger(logging.CategorySession, input.SessionID)
	if catErr != nil {
		log.Warn("CategoryLogger unavailable, using stderr only", map[string]string{
			"error": catErr.Error(),
		})
	}
	defer func() {
		if catLog != nil {
			catLog.Close()
		}
	}()

	// --- Finalize state machine ---
	now := time.Now().Format(time.RFC3339)

	// Mark session inactive
	var state *statemachine.RuntimeState
	var stateErr error
	if state, stateErr = statemachine.LoadRuntimeState(); stateErr == nil {
		state.Session.Active = false
		_ = statemachine.SaveRuntimeState(state)

		// Record session end in database (temporal consciousness)
		if bridge, err := internal.GetBridge(); err == nil {
			ctx := context.Background()
			if err := bridge.EndSession(ctx, input.SessionID, state); err != nil {
				log.Warn("Failed to record session end in database", map[string]string{
					"error": err.Error(),
				})
				if catLog != nil {
					catLog.Warn("database_error", "Failed to record session end in database", map[string]string{
						"error": err.Error(),
					})
				}
			} else if catLog != nil {
				catLog.Success("database_session_end", "Session end recorded in database", map[string]string{
					"session_id": input.SessionID,
				})
			}
		}
	}

	// Finalize path
	var path *statemachine.RuntimePath
	if p, err := statemachine.LoadRuntimePath(); err == nil {
		path = p
		path.EndedAt = now
		path.RecordEvent("session_end", "", "")

		// Compute unique anchors
		seen := make(map[string]bool)
		for _, a := range path.Anchors {
			seen[a.AnchorKey] = true
		}
		path.Summary.UniqueAnchors = len(seen)

		_ = statemachine.SaveRuntimePath(path)
	}

	// Record session to history for cross-session learning
	if state != nil && path != nil {
		if err := statemachine.RecordSessionSnapshot(input.SessionID, state, path); err != nil {
			log.Warn("Failed to record session in history", map[string]string{
				"error": err.Error(),
			})
			if catLog != nil {
				catLog.Warn("history_error", "Failed to record session in history", map[string]string{
					"error": err.Error(),
				})
			}
		} else if catLog != nil {
			catLog.Success("history_recorded", "Session recorded in history", map[string]string{
				"session_id": input.SessionID,
			})
		}
	}

	log.Info("Session ended", map[string]string{
		"session_id": input.SessionID,
		"reason":     input.Reason,
	})
	if catLog != nil {
		catLog.Info("session_ended", "Session ended", map[string]string{
			"session_id": input.SessionID,
			"reason":     input.Reason,
		})
	}

	// Output nothing - cannot block termination
}

// ============================================================================
// CLOSING
// ============================================================================
