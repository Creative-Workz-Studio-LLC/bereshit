// ============================================================================
// METADATA
// ============================================================================
// Hook: SubagentStart
// Fires: When a subagent (Task tool) begins execution
// Added: Claude Code v2.0.43
//
// Input fields:
//   - session_id: string - Parent session ID
//   - agent_type: string - Type of subagent being started
//   - agent_id: string - Unique ID for this subagent run
//   - prompt: string - The prompt/task given to the subagent
//   - hook_event_name: string - "SubagentStart"
//
// Integration:
//   - Logs subagent spawn for observability
//   - Can track subagent cascade depth
//   - Enables monitoring of parallel subagent execution
//
// Traces to: settings.json hooks.SubagentStart

package session

import (
	"encoding/json"
	"os"

	"github.com/creativeworkzstudio/claude-global/pkg/orchestration/logging"
)

// ============================================================================
// SETUP
// ============================================================================

// SubagentStartInput from Claude Code
type SubagentStartInput struct {
	SessionID     string `json:"session_id"`
	AgentType     string `json:"agent_type"`
	AgentID       string `json:"agent_id"`
	Prompt        string `json:"prompt,omitempty"`
	HookEventName string `json:"hook_event_name,omitempty"`
}

// ============================================================================
// BODY
// ============================================================================

// SubagentStart handles the SubagentStart hook
func SubagentStart() {
	log := logging.New("session")
	log.SetMode(logging.ModeCompact)

	var input SubagentStartInput
	if err := json.NewDecoder(os.Stdin).Decode(&input); err != nil {
		log.Error("Failed to decode SubagentStart input", map[string]string{"error": err.Error()})
		os.Exit(1)
	}

	// Create CategoryLogger for file output
	catLog, catErr := logging.NewCategoryLogger(logging.CategorySession, input.SessionID)
	if catErr != nil {
		log.Warn("CategoryLogger unavailable", map[string]string{
			"error": catErr.Error(),
		})
	}
	defer func() {
		if catLog != nil {
			catLog.Close()
		}
	}()

	// Log subagent spawn
	logFields := map[string]string{
		"session_id": input.SessionID,
		"agent_type": input.AgentType,
		"agent_id":   input.AgentID,
	}

	// Truncate prompt for logging (keep first 100 chars)
	if len(input.Prompt) > 100 {
		logFields["prompt_preview"] = input.Prompt[:100] + "..."
	} else if input.Prompt != "" {
		logFields["prompt_preview"] = input.Prompt
	}

	log.Info("Subagent starting", logFields)
	if catLog != nil {
		catLog.Info("subagent_start", "Subagent starting", logFields)
	}

	// TODO: Track in database when RecordSubagentEvent is implemented
	// For now, logging provides observability

	// No output needed - this is an observability hook
}

// ============================================================================
// CLOSING
// ============================================================================
