// ============================================================================
// METADATA
// ============================================================================
// Hook: TeammateIdle
// Fires: When an agent team teammate is about to go idle
// Can block: Exit code 2 = send feedback and keep teammate working
//
// Agent Teams Integration (v2.1.33):
//   - Teammate finished current work and is about to go idle
//   - Quality gate: can reject idle and send feedback to keep working
//   - Tracks teammate activity patterns for team coordination
//
// Mental Construct Integration:
//   - Teammate idle = delegation checkpoint
//   - Opportunity to redirect or confirm completion

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

// TeammateIdleInput from Claude Code
type TeammateIdleInput struct {
	SessionID     string `json:"session_id"`
	HookEventName string `json:"hook_event_name,omitempty"`
	TeammateName  string `json:"teammate_name,omitempty"`
}

// TeammateIdleOutput to control behavior
// Exit code 2 with "reason" sends feedback to teammate and keeps them working
type TeammateIdleOutput struct {
	Decision string `json:"decision"` // allow, block
	Reason   string `json:"reason,omitempty"`
}

// ============================================================================
// BODY
// ============================================================================

// TeammateIdle handles the TeammateIdle hook
func TeammateIdle() {
	log := logging.New("teammate")
	log.SetMode(logging.ModeCompact)

	var input TeammateIdleInput
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

	// --- TEAMMATE IDLE: Track team activity ---
	state, stateErr := statemachine.LoadRuntimeState()
	currentSection := "B.1"

	if stateErr == nil && state != nil {
		currentSection = state.TrajectorySection
	}

	// Record teammate idle event in path
	if path, err := statemachine.LoadRuntimePath(); err == nil {
		path.RecordEvent("teammate_idle", input.TeammateName, currentSection)
		_ = statemachine.SaveRuntimePath(path)
	}

	log.Debug("Teammate idle", map[string]string{
		"session_id": input.SessionID,
		"teammate":   input.TeammateName,
		"trajectory": currentSection,
	})
	if catLog != nil {
		catLog.Success("teammate_idle", "Teammate going idle", map[string]string{
			"teammate":   input.TeammateName,
			"trajectory": currentSection,
		})
	}

	// Default: allow idle (exit 0)
	// To keep teammate working: exit code 2 with reason
	output := TeammateIdleOutput{Decision: "allow"}
	json.NewEncoder(os.Stdout).Encode(output)
}

// ============================================================================
// CLOSING
// ============================================================================
