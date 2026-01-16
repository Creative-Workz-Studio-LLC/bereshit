// ============================================================================
// METADATA
// ============================================================================
// Hook: SessionStart
// Fires: When Claude Code starts a new session or resumes
// Matchers: startup, resume, clear, compact
//
// State Machine Integration:
//   - startup: Creates fresh runtime state (state.jsonc, path.jsonc)
//   - resume: Loads existing runtime state, marks as active
//   - clear: Resets runtime state to initial values
//   - compact: Snapshots state to history before compaction
//
// Health Integration:
//   - Logging uses health loader (config-driven from TOML)
//   - Health starts at 0 (even balance) per ternary system
//   - Each log entry tracks health changes (+1 success, -1 failure)
//
// Config-Driven Design:
//   - Runtime JSONCs contain REFERENCES (keys) to TOML entries
//   - anchor_key → pkg/cpisi/schema/config/l2/anchors.toml
//   - trajectory_section → pkg/cpisi/schema/config/l2/framework.toml
//   - command_key → pkg/cpisi/schema/config/l2/commands.toml
//   - health levels → pkg/cpisi/schema/config/l2/health/score.toml
//
// Traces to: hooks.jsonc SessionStart operations

package session

import (
	"context"
	"encoding/json"
	"fmt"
	"os"

	"cws.studio/claude/hooks/internal"
	"github.com/creativeworkzstudio/claude-global/pkg/core/statemachine"
	"github.com/creativeworkzstudio/claude-global/pkg/orchestration/cognition"
	"github.com/creativeworkzstudio/claude-global/pkg/orchestration/logging"
	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/hookoutput"
)

// ============================================================================
// SETUP
// ============================================================================

// StartInput from Claude Code
type StartInput struct {
	SessionID      string `json:"session_id"`
	TranscriptPath string `json:"transcript_path,omitempty"`
	PermissionMode string `json:"permission_mode,omitempty"`
	HookEventName  string `json:"hook_event_name,omitempty"`
	Source         string `json:"source"` // startup, resume, clear, compact
}

// StartOutput uses hookoutput.ContextResponse for correct schema
// Old: {"additionalContext": "..."}
// New: {"hookSpecificOutput": {"hookEventName": "SessionStart", "additionalContext": "..."}}

// ============================================================================
// BODY
// ============================================================================

// Start handles the SessionStart hook
func Start() {
	log := logging.New("session")
	log.SetMode(logging.ModeCompact)

	var input StartInput
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

	log.Info("Session starting", map[string]string{
		"session_id": input.SessionID,
		"source":     input.Source,
	})
	if catLog != nil {
		catLog.Info("session_start", "Session starting", map[string]string{
			"session_id": input.SessionID,
			"source":     input.Source,
		})
	}

	// Initialize session based on source type and get runtime state
	var state *statemachine.RuntimeState
	switch input.Source {
	case "startup":
		state = handleStartup(log, catLog, input)
	case "resume":
		state = handleResume(log, catLog, input)
	case "clear":
		state = handleClear(log, catLog, input)
	case "compact":
		state = handleCompact(log, catLog, input)
	default:
		log.Warn("Unknown source", map[string]string{"source": input.Source})
	}

	// Build and emit cognition context to shape session
	workdir, _ := os.Getwd()
	context := cognition.SessionContext(state, workdir)

	// Add post-compact checkpoint when resuming from auto-compact
	if input.Source == "compact" {
		context += "\n" + cognition.PostCompactAwareness()
	}

	// Use correct Claude Code schema with hookSpecificOutput
	output := hookoutput.NewSessionStartResponse(context)
	json.NewEncoder(os.Stdout).Encode(output)
}

// handleStartup initializes a fresh session
func handleStartup(log *logging.Logger, catLog *logging.CategoryLogger, input StartInput) *statemachine.RuntimeState {
	// Initialize state machine runtime
	state := statemachine.InitializeRuntimeState(input.SessionID)
	path := statemachine.InitializeRuntimePath(input.SessionID)

	// Save state to disk
	if err := statemachine.SaveRuntimeState(state); err != nil {
		log.LogFailure("Failed to save runtime state", map[string]string{
			"error": err.Error(),
		})
		if catLog != nil {
			catLog.Failure("state_save_error", "Failed to save runtime state", map[string]string{
				"error": err.Error(),
			})
		}
	}

	if err := statemachine.SaveRuntimePath(path); err != nil {
		log.LogFailure("Failed to save runtime path", map[string]string{
			"error": err.Error(),
		})
		if catLog != nil {
			catLog.Failure("path_save_error", "Failed to save runtime path", map[string]string{
				"error": err.Error(),
			})
		}
	}

	// Record session start in database (temporal consciousness)
	if bridge, err := internal.GetBridge(); err == nil {
		ctx := context.Background()
		if err := bridge.StartSession(ctx, input.SessionID, state); err != nil {
			log.Warn("Failed to record session in database", map[string]string{
				"error": err.Error(),
			})
			if catLog != nil {
				catLog.Warn("database_error", "Failed to record session in database", map[string]string{
					"error": err.Error(),
				})
			}
		} else if catLog != nil {
			catLog.Success("database_session", "Session recorded in database", map[string]string{
				"session_id": input.SessionID,
			})
		}
	}

	// Log successful initialization (+1 health)
	log.LogSuccess("Session initialized", map[string]string{
		"session_id":  input.SessionID,
		"anchor":      state.AnchorKey,
		"trajectory":  state.TrajectorySection,
		"command":     state.CommandKey,
		"health_init": "0", // even balance
	})
	if catLog != nil {
		catLog.Success("session_initialized", "Session initialized", map[string]string{
			"session_id":  input.SessionID,
			"anchor":      state.AnchorKey,
			"trajectory":  state.TrajectorySection,
			"command":     state.CommandKey,
			"health_init": "0",
		})
	}

	return state
}

// handleResume continues an existing session
func handleResume(log *logging.Logger, catLog *logging.CategoryLogger, input StartInput) *statemachine.RuntimeState {
	// Try to load existing state
	state, err := statemachine.LoadRuntimeState()
	if err != nil {
		// No existing state - initialize fresh
		log.Warn("No existing state, initializing fresh", map[string]string{
			"error": err.Error(),
		})
		if catLog != nil {
			catLog.Warn("no_state", "No existing state, initializing fresh", map[string]string{
				"error": err.Error(),
			})
		}
		return handleStartup(log, catLog, input)
	}

	// Update session ID to match Claude Code's session
	// This is critical - Claude Code generates new session IDs, state must track them
	if input.SessionID != "" && input.SessionID != state.Session.ID {
		state.Session.ID = input.SessionID
	}

	// Mark as active
	state.Session.Active = true
	state.Session.HooksFired++

	if err := statemachine.SaveRuntimeState(state); err != nil {
		log.LogFailure("Failed to save runtime state", map[string]string{
			"error": err.Error(),
		})
		if catLog != nil {
			catLog.Failure("state_save_error", "Failed to save runtime state", map[string]string{
				"error": err.Error(),
			})
		}
	}

	// Record resume event in path
	path, err := statemachine.LoadRuntimePath()
	if err == nil {
		path.RecordEvent("session_resume", "", state.TrajectorySection)
		_ = statemachine.SaveRuntimePath(path)
	}

	log.LogSuccess("Session resumed", map[string]string{
		"session_id": input.SessionID,
		"anchor":     state.AnchorKey,
		"trajectory": state.TrajectorySection,
		"command":    state.CommandKey,
	})
	if catLog != nil {
		catLog.Success("session_resumed", "Session resumed", map[string]string{
			"session_id": input.SessionID,
			"anchor":     state.AnchorKey,
			"trajectory": state.TrajectorySection,
			"command":    state.CommandKey,
		})
	}

	return state
}

// handleClear handles session clear event
func handleClear(log *logging.Logger, catLog *logging.CategoryLogger, input StartInput) *statemachine.RuntimeState {
	// Reset to initial state (preserves session ID)
	state := statemachine.InitializeRuntimeState(input.SessionID)

	if err := statemachine.SaveRuntimeState(state); err != nil {
		log.LogFailure("Failed to reset runtime state", map[string]string{
			"error": err.Error(),
		})
		if catLog != nil {
			catLog.Failure("state_reset_error", "Failed to reset runtime state", map[string]string{
				"error": err.Error(),
			})
		}
	}

	// Record clear event in path (don't reset path - keep audit trail)
	path, err := statemachine.LoadRuntimePath()
	if err == nil {
		path.RecordEvent("session_clear", "", state.TrajectorySection)
		_ = statemachine.SaveRuntimePath(path)
	}

	log.Info("Session cleared, state reset", map[string]string{
		"session_id": input.SessionID,
		"anchor":     state.AnchorKey,
		"trajectory": state.TrajectorySection,
	})
	if catLog != nil {
		catLog.Info("session_cleared", "Session cleared, state reset", map[string]string{
			"session_id": input.SessionID,
			"anchor":     state.AnchorKey,
			"trajectory": state.TrajectorySection,
		})
	}

	return state
}

// handleCompact handles context compaction
func handleCompact(log *logging.Logger, catLog *logging.CategoryLogger, input StartInput) *statemachine.RuntimeState {
	// Load current state for snapshot
	state, err := statemachine.LoadRuntimeState()
	if err != nil {
		log.Warn("No state to snapshot", map[string]string{
			"error": err.Error(),
		})
		if catLog != nil {
			catLog.Warn("no_state_snapshot", "No state to snapshot", map[string]string{
				"error": err.Error(),
			})
		}
		return nil
	}

	// Update session ID to match Claude Code's session
	if input.SessionID != "" && input.SessionID != state.Session.ID {
		state.Session.ID = input.SessionID
		_ = statemachine.SaveRuntimeState(state)
	}

	// Record compact event in path
	path, err := statemachine.LoadRuntimePath()
	if err == nil {
		path.RecordEvent("context_compact", "", state.TrajectorySection)
		_ = statemachine.SaveRuntimePath(path)
	}

	// Write to history.jsonc for cross-session learning
	if histErr := statemachine.RecordSessionSnapshot(input.SessionID, state, path); histErr != nil {
		log.Warn("Failed to record snapshot in history", map[string]string{
			"error": histErr.Error(),
		})
		if catLog != nil {
			catLog.Warn("history_error", "Failed to record snapshot in history", map[string]string{
				"error": histErr.Error(),
			})
		}
	}

	log.Info("Context compacted, state snapshot recorded", map[string]string{
		"session_id":  input.SessionID,
		"anchor":      state.AnchorKey,
		"trajectory":  state.TrajectorySection,
		"path_length": fmt.Sprintf("%d", state.Session.PathLength),
	})
	if catLog != nil {
		catLog.Info("context_compacted", "Context compacted, state snapshot recorded", map[string]string{
			"session_id":  input.SessionID,
			"anchor":      state.AnchorKey,
			"trajectory":  state.TrajectorySection,
			"path_length": fmt.Sprintf("%d", state.Session.PathLength),
		})
	}

	return state
}

// ============================================================================
// CLOSING
// ============================================================================
// Environment: CLAUDE_ENV_FILE available for persisting env vars
