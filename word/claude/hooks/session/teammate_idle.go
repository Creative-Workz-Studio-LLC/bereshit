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
	"fmt"
	"os"

	"cws.studio/pkg/orchestration/logging"
	"cws.studio/pkg/core/statemachine"
	"cws.studio/pkg/sdk/hookoutput"
	"cws.studio/pkg/sdk/substrate"
	"cws.studio/claude/hooks/internal/status"
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
	rawInput, _ := os.ReadFile("/dev/stdin")
	if err := json.Unmarshal(rawInput, &input); err != nil {
		log.Error("Failed to decode input", map[string]string{"error": err.Error()})
		os.Exit(1)
	}

	// --- Load Substrate Maps ---
	schemaBase := "/media/seanje-lenox-wise/Project/Bereshit/word/core/schemas/substrate"
	for _, sub := range []string{"gemini", "claude", "cpisi"} {
		substrate.LoadMap(fmt.Sprintf("%s/%s.toml", schemaBase, sub))
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

	// --- Render Response via Substrate SDK ---
	subName := "claude"
	if hookoutput.IsGemini() {
		subName = "gemini"
	}

	rendered, err := substrate.RenderOutput(subName, "teammate_idle", "allow", nil)
	if err == nil {
		fmt.Print(rendered)
	} else {
		// Fallback to legacy behavior
		output := TeammateIdleOutput{Decision: "allow"}
		json.NewEncoder(os.Stdout).Encode(output)
	}

	// Update statusline and terminal state
	status.Emit(input.SessionID)
}

// ============================================================================
// CLOSING
// ============================================================================
