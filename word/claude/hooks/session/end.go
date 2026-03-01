// ============================================================================
// METADATA
// ============================================================================
// Hook: SessionEnd
// Fires: When Claude Code session ends (app exit or /clear)
// Purpose: Session cleanup, state persistence, and performance logging
//
// This hook enables:
//   - Finalizing the temporal record in the database
//   - Recording final state (K:ALIGN, Hebrew state)
//   - Performance summary emission
//
// Traces to: v1.0.0 architecture - Session lifecycle foundation

package session

import (
	"context"
	"encoding/json"
	"fmt"
	"os"
	"time"

	"cws.studio/claude/hooks/internal"
	"cws.studio/pkg/core/statemachine"
	"cws.studio/pkg/orchestration/cognition"
	"cws.studio/pkg/orchestration/logging"
	"cws.studio/pkg/sdk/hookoutput"
	"cws.studio/pkg/sdk/substrate"
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
}

// ============================================================================
// BODY
// ============================================================================

// End handles the SessionEnd hook
func End() {
	log := logging.New("session")
	log.SetMode(logging.ModeCompact)

	var input EndInput
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

	// --- Process Universal Event via Rust Engine ---
	subName := "claude"
	if hookoutput.IsGemini() {
		subName = "gemini"
	}
	universalJSON, err := substrate.ProcessEvent(subName, "session_end", string(rawInput))
	if err == nil {
		log.Debug("Universal Event mapped", map[string]string{"universal": universalJSON})
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

	// --- SESSION END: Finalize state and record outcomes ---
	state, stateErr := statemachine.LoadRuntimeState()
	if stateErr != nil {
		log.Warn("Could not load state for session end", map[string]string{"error": stateErr.Error()})
	}

	if state != nil {
		// Mark as inactive
		state.Session.Active = false
		state.Session.EndedAt = time.Now().Format(time.RFC3339)

		// Record in database via Bridge
		bridge, err := internal.GetBridge()
		if err == nil {
			_ = bridge.EndSession(context.Background(), input.SessionID, state)
		}

		_ = statemachine.SaveRuntimeState(state)
	}

	log.Info("Session ended", map[string]string{
		"session_id": input.SessionID,
		"exchanges":  fmt.Sprintf("%d", state.Session.ExchangeCount),
	})

	if catLog != nil {
		catLog.Info("session_end", "Session finalized", map[string]string{
			"exchanges": fmt.Sprintf("%d", state.Session.ExchangeCount),
		})
	}

	// --- Visual Summary Stat Screen ---
	if state != nil {
		fmt.Fprint(os.Stderr, cognition.GenerateSessionSummary(state))

		// --- Generate Sabbath Artifacts (.adoc/.json) ---
		testimony := substrate.SessionTestimony{
			SessionID:       state.Session.ID,
			Substrate:       state.Session.Substrate,
			Engine:          state.Session.Engine,
			StartedAt:       state.Session.InitializedAt,
			EndedAt:         state.Session.EndedAt,
			DurationSeconds: 0, // Duration calculation can be added here
			Metrics: substrate.SessionMetrics{
				Exchanges:         uint32(state.Session.ExchangeCount),
				Insights:          uint32(state.Session.InsightCount),
				ToolsUsed:         uint32(state.Session.PathLength),
				PeakContextTokens: uint32(state.Session.PeakContextTokens),
				Compactions:       uint32(state.Session.CompactionCount),
			},
			State: substrate.FinalState{
				Trajectory:  state.TrajectorySection,
				Health:      state.Session.HealthScore,
				HebrewState: state.Session.HebrewState,
				Alignment:   state.Session.KAlign,
				Anchor:      state.AnchorKey,
			},
			Summary: cognition.GenerateWorkSummary(nil), // Path loading could be optimized here
		}

		if err := substrate.GenerateSabbathArtifacts(testimony); err != nil {
			log.Warn("Failed to generate mission artifacts", map[string]string{
				"error": err.Error(),
			})
		} else {
			log.Info("Sabbath artifacts generated", nil)
		}
	}
}

// ============================================================================
// CLOSING
// ============================================================================
