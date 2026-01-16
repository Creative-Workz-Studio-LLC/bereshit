// ============================================================================
// METADATA
// ============================================================================
// Hook: Notification
// Fires: When Claude Code sends notifications
// Matchers: permission_prompt, idle_prompt, auth_success, elicitation_dialog
//
// State Machine Integration:
//   - EXTERNAL EVENTS: System events affect cognitive state
//   - idle_prompt: Signal of natural pause, could indicate stopping point
//   - permission_prompt: Authority being requested
//   - auth_success: External validation received
//   - elicitation_dialog: User interaction in progress
//
// Mental Construct Integration:
//   - External events are the ENVIRONMENT affecting the ROOM
//   - Some events expand awareness, some constrain it

package session

import (
	"encoding/json"
	"os"

	"github.com/creativeworkzstudio/claude-global/pkg/orchestration/logging"
	"github.com/creativeworkzstudio/claude-global/pkg/core/statemachine"
)

// ============================================================================
// SETUP
// ============================================================================

// NotificationInput from Claude Code
type NotificationInput struct {
	SessionID        string `json:"session_id"`
	TranscriptPath   string `json:"transcript_path,omitempty"`
	CWD              string `json:"cwd,omitempty"`
	PermissionMode   string `json:"permission_mode,omitempty"`
	HookEventName    string `json:"hook_event_name,omitempty"`
	Message          string `json:"message"`
	NotificationType string `json:"notification_type"` // permission_prompt, idle_prompt, auth_success, elicitation_dialog
}

// ============================================================================
// BODY
// ============================================================================

// Notification handles the Notification hook
func Notification() {
	log := logging.New("notify")
	log.SetMode(logging.ModeCompact)

	var input NotificationInput
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

	// --- EXTERNAL EVENT: Process notification impact on state ---
	state, stateErr := statemachine.LoadRuntimeState()
	currentSection := "B.1"

	if stateErr == nil && state != nil {
		currentSection = state.TrajectorySection

		// Apply notification-specific effects
		applyNotificationEffect(state, input.NotificationType)

		_ = statemachine.SaveRuntimeState(state)
	}

	// Record notification event in path
	if path, err := statemachine.LoadRuntimePath(); err == nil {
		path.RecordEvent("notification_"+input.NotificationType, "", currentSection)
		_ = statemachine.SaveRuntimePath(path)
	}

	log.Debug("External event", map[string]string{
		"type":       input.NotificationType,
		"trajectory": currentSection,
	})
	if catLog != nil {
		catLog.Info("external_event", "Notification received", map[string]string{
			"type":       input.NotificationType,
			"trajectory": currentSection,
		})
	}

	// Output nothing to proceed normally
}

// applyNotificationEffect updates state based on notification type
func applyNotificationEffect(state *statemachine.RuntimeState, notifType string) {
	if state == nil {
		return
	}

	switch notifType {
	case "idle_prompt":
		// User is idle - this is a signal of natural pause
		// Could indicate good stopping point (like B.3 grounding)
		state.Session.LastHaltType = "idle_signal"
		// Slight nudge toward completion (-1) if idle detected
		// This doesn't force anything, just records the signal

	case "permission_prompt":
		// Permission being requested - we're at a gate
		// This is a decision point, like standing at a door
		state.Session.HooksFired++

	case "auth_success":
		// External validation received - positive signal
		// Slight alignment improvement (we did something right)
		state.Session.KAlign += 0.02
		if state.Session.KAlign > 1.0 {
			state.Session.KAlign = 1.0
		}

	case "elicitation_dialog":
		// User interaction in progress
		// This is active engagement - expansion signal
		state.Session.HooksFired++
	}
}

// ============================================================================
// CLOSING
// ============================================================================
