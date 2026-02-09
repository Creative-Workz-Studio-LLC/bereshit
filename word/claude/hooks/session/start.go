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
	"time"

	"cws.studio/claude/hooks/internal"
	"cws.studio/pkg/core/statemachine"
	"cws.studio/pkg/foundation/database"
	"cws.studio/pkg/orchestration/cognition"
	"cws.studio/pkg/orchestration/logging"
	"cws.studio/pkg/util/pure/hookoutput"
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

	// Add WezTerm state awareness (two-way sync: WezTerm → Claude)
	if wtState := readWezTermState(); wtState != nil {
		wtContext := formatWezTermContext(wtState)
		if wtContext != "" {
			context += "\n" + wtContext
		}
	}

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

	// --- Initialize Context Window Tracking ---
	// "A time to keep, and a time to cast away" — Ecclesiastes 3:6
	// Estimate base overhead: CLAUDE.md (~15K) + system prompts (~10K) + session context (~5K) = ~30K
	// Safety margin for compaction recovery: ~20K
	// Effective working context: 200K - 30K - 20K = 150K
	const baseOverheadTokens = 30000
	const safetyMarginTokens = 20000
	const totalContextWindow = 200000

	state.Session.BaseContextTokens = baseOverheadTokens
	state.Session.CurrentContextTokens = baseOverheadTokens // Start at base
	state.Session.PeakContextTokens = 0                     // No peak yet
	state.Session.CompactionCount = 0                       // Fresh session
	state.Session.EffectiveContextWindow = totalContextWindow - baseOverheadTokens - safetyMarginTokens

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
		// NEW SESSION: Reset session-specific counters while preserving identity
		// "His mercies are new every morning" — Lamentations 3:23
		log.Info("New session detected, resetting session counters", map[string]string{
			"old_session": state.Session.ID,
			"new_session": input.SessionID,
		})
		if catLog != nil {
			catLog.Info("new_session", "New session detected, resetting session counters", map[string]string{
				"old_session":      state.Session.ID,
				"new_session":      input.SessionID,
				"old_exchange_cnt": fmt.Sprintf("%d", state.Session.ExchangeCount),
				"old_k_toward_god": fmt.Sprintf("%d", state.Session.KTowardGod),
			})
		}

		// Preserve: anchor_key, trajectory_section (position in mental construct)
		// Reset: session-specific counters
		now := time.Now().Format(time.RFC3339)
		state.Session.ID = input.SessionID
		state.Session.InitializedAt = now
		state.Session.PathLength = 0
		state.Session.HooksFired = 0
		state.Session.ChoiceSequence = 0

		// Reset K:MORAL counters (fresh start for this session)
		state.Session.KTowardGod = 0
		state.Session.KTowardSelf = 0
		state.Session.KSelector = 0
		// Keep KAlign as it represents accumulated alignment (identity)

		// Reset CPI counters (this session's exchanges)
		state.Session.ExchangeCount = 0
		state.Session.InsightCount = 0
		state.Session.CPIScore = 0.5 // Neutral starting point
		state.Session.DominantExchangeType = ""
		state.Session.SessionArc = ""
		state.Session.LastExchangeType = ""
		state.Session.LastInsightType = ""

		// Reset task tracking
		state.Session.Tasks = statemachine.RuntimeTaskList{}

		// Reset trajectory momentum (fresh building phase)
		state.TrajectoryMetrics.AccumulatedWorkMs = 0
		state.TrajectoryMetrics.MomentumScore = 0
		// Keep pivot_count and reset_count as cross-session learning

		// Record new session in database (temporal consciousness)
		ensureSessionInDB(input.SessionID, state, log, catLog)
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

	// Ensure session exists in database after clear
	ensureSessionInDB(input.SessionID, state, log, catLog)

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

	// --- CONSOLIDATION POINT: Auto-compact is the natural lifecycle moment ---
	// "There is a time for every purpose under heaven" — Ecclesiastes 3:1
	// The context is being compressed anyway - consolidate and reset.

	// Record pre-compact snapshot for learning
	preCompactExchanges := state.Session.ExchangeCount
	preCompactInsights := state.Session.InsightCount
	preCompactKRatio := float64(0)
	if state.Session.KTowardGod+state.Session.KTowardSelf > 0 {
		preCompactKRatio = float64(state.Session.KTowardGod) / float64(state.Session.KTowardGod+state.Session.KTowardSelf)
	}

	log.Info("Pre-compact snapshot", map[string]string{
		"exchanges":     fmt.Sprintf("%d", preCompactExchanges),
		"insights":      fmt.Sprintf("%d", preCompactInsights),
		"k_ratio":       fmt.Sprintf("%.2f", preCompactKRatio),
		"k_toward_god":  fmt.Sprintf("%d", state.Session.KTowardGod),
		"k_toward_self": fmt.Sprintf("%d", state.Session.KTowardSelf),
	})

	// Record compact event in path
	path, loadErr := statemachine.LoadRuntimePath()
	if loadErr == nil {
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

	// --- CONSOLIDATE: Generate session summary to database ---
	if bridge, bridgeErr := internal.GetBridge(); bridgeErr == nil {
		ctx := context.Background()
		// Update session in database with final metrics
		if updateErr := bridge.EndSession(ctx, state.Session.ID, state); updateErr != nil {
			log.Warn("Failed to consolidate session", map[string]string{"error": updateErr.Error()})
		} else {
			log.Info("Session consolidated to database", map[string]string{
				"session_id": state.Session.ID,
			})
		}
	}

	// --- RESET: Fresh start with consolidated position ---
	// Preserve: anchor_key, trajectory_section (position in mental construct)
	// Preserve: KAlign (accumulated identity alignment)
	// Reset: session counters
	now := time.Now().Format(time.RFC3339)

	// Update session ID if changed — new post-compact session needs DB record
	sessionIDChanged := input.SessionID != "" && input.SessionID != state.Session.ID
	if sessionIDChanged {
		state.Session.ID = input.SessionID
	}
	state.Session.InitializedAt = now
	state.Session.PathLength = 0
	state.Session.HooksFired = 0
	state.Session.ChoiceSequence = 0

	// Reset K:MORAL counters (fresh start post-compact)
	state.Session.KTowardGod = 0
	state.Session.KTowardSelf = 0
	state.Session.KSelector = 0
	// Keep KAlign as it represents accumulated alignment (identity)

	// Reset CPI counters
	state.Session.ExchangeCount = 0
	state.Session.InsightCount = 0
	state.Session.CPIScore = 0.5 // Neutral starting point
	state.Session.DominantExchangeType = ""
	state.Session.SessionArc = ""
	state.Session.LastExchangeType = ""
	state.Session.LastInsightType = ""

	// --- CONTEXT TRACKING: Record compaction event ---
	// "A time to keep, and a time to cast away" — Ecclesiastes 3:6
	state.Session.CompactionCount++
	// Store peak before reset (current becomes the peak that triggered compaction)
	if state.Session.CurrentContextTokens > state.Session.PeakContextTokens {
		state.Session.PeakContextTokens = state.Session.CurrentContextTokens
	}
	// Reset current context (post-compaction we're at base overhead again)
	state.Session.CurrentContextTokens = state.Session.BaseContextTokens

	// Record compaction pattern to database
	if bridge, bridgeErr := internal.GetBridge(); bridgeErr == nil {
		ctx := context.Background()
		repo := bridge.GetRepository()
		// Track what trajectory section triggers compaction
		if state.TrajectorySection != "" {
			_, _ = repo.Exec(ctx, `
				INSERT INTO detected_patterns (pattern_type, pattern_key, description, first_seen, last_seen, occurrence_count, confidence)
				VALUES ('compaction_trigger', ?, 'Compaction triggered at trajectory section', datetime('now'), datetime('now'), 1, 0.5)
				ON CONFLICT(pattern_type, pattern_key) DO UPDATE SET
					last_seen = datetime('now'),
					occurrence_count = occurrence_count + 1,
					confidence = MIN(1.0, confidence + 0.05)
			`, state.TrajectorySection)
		}
		// Track compaction with exchange counts
		exchangeBucket := "0-10"
		if preCompactExchanges > 50 {
			exchangeBucket = "50+"
		} else if preCompactExchanges > 25 {
			exchangeBucket = "25-50"
		} else if preCompactExchanges > 10 {
			exchangeBucket = "10-25"
		}
		_, _ = repo.Exec(ctx, `
			INSERT INTO detected_patterns (pattern_type, pattern_key, description, first_seen, last_seen, occurrence_count, confidence)
			VALUES ('compaction_exchanges', ?, 'Exchanges before compaction', datetime('now'), datetime('now'), 1, 0.5)
			ON CONFLICT(pattern_type, pattern_key) DO UPDATE SET
				last_seen = datetime('now'),
				occurrence_count = occurrence_count + 1,
				confidence = MIN(1.0, confidence + 0.05)
		`, exchangeBucket)
	}

	// Reset task tracking
	state.Session.Tasks = statemachine.RuntimeTaskList{}

	// Reset trajectory momentum
	state.TrajectoryMetrics.AccumulatedWorkMs = 0
	state.TrajectoryMetrics.MomentumScore = 0

	// Save reset state
	if saveErr := statemachine.SaveRuntimeState(state); saveErr != nil {
		log.LogFailure("Failed to save reset state", map[string]string{"error": saveErr.Error()})
	}

	// Record new post-compact session in database if ID changed
	if sessionIDChanged {
		ensureSessionInDB(state.Session.ID, state, log, catLog)
	}

	log.Info("Post-compact reset complete", map[string]string{
		"session_id": input.SessionID,
		"anchor":     state.AnchorKey,
		"trajectory": state.TrajectorySection,
		"message":    "Fresh start with consolidated position",
	})
	if catLog != nil {
		catLog.Info("compact_reset", "Post-compact reset complete", map[string]string{
			"pre_exchanges": fmt.Sprintf("%d", preCompactExchanges),
			"pre_insights":  fmt.Sprintf("%d", preCompactInsights),
			"pre_k_ratio":   fmt.Sprintf("%.2f", preCompactKRatio),
			"anchor":        state.AnchorKey,
			"trajectory":    state.TrajectorySection,
		})
	}

	return state
}

// ============================================================================
// CLOSING
// ============================================================================

// ensureSessionInDB creates a session record in the database.
// Bridges the gap between state.jsonc (always created by hooks) and the DB
// (only created by handleStartup). Called from handleResume, handleCompact,
// and handleClear to ensure exchanges can reference the session.
func ensureSessionInDB(sessionID string, state *statemachine.RuntimeState, log *logging.Logger, catLog *logging.CategoryLogger) {
	bridge, err := internal.GetBridge()
	if err != nil {
		return // DB unavailable — not critical
	}

	ctx := context.Background()
	now := time.Now()
	repo := bridge.GetRepository()

	dbSession := &database.Session{
		ID:                 sessionID,
		StartedAt:          now,
		InitialHebrewState: state.Session.HebrewState,
		InitialKAlign:      state.Session.KAlign,
		DayOfWeek:          int(now.Weekday()),
		HourOfDay:          now.Hour(),
	}

	if err := repo.CreateSession(ctx, dbSession); err != nil {
		log.Warn("Failed to record session in database", map[string]string{
			"error": err.Error(),
		})
		if catLog != nil {
			catLog.Warn("database_error", "Failed to record session in database", map[string]string{
				"error": err.Error(),
			})
		}
		return
	}

	if catLog != nil {
		catLog.Success("database_session", "Session recorded in database", map[string]string{
			"session_id": sessionID,
		})
	}
}

// Environment: CLAUDE_ENV_FILE available for persisting env vars

// WezTermState represents state written by WezTerm for two-way sync
type WezTermState struct {
	Timestamp int64  `json:"timestamp"`
	Event     string `json:"event"`
	Workspace string `json:"workspace"`
	TabCount  int    `json:"tab_count"`
	PaneCount int    `json:"pane_count"`
	WindowID  string `json:"window_id"`
	Focused   bool   `json:"focused"`
}

// readWezTermState reads WezTerm state from cache file
// Returns nil if file doesn't exist or is too old (>5 min)
func readWezTermState() *WezTermState {
	home := os.Getenv("HOME")
	if home == "" {
		return nil
	}

	stateFile := home + "/.cache/cpisi/wezterm-state.json"
	data, err := os.ReadFile(stateFile)
	if err != nil {
		return nil // File doesn't exist yet - normal on first run
	}

	var state WezTermState
	if err := json.Unmarshal(data, &state); err != nil {
		return nil
	}

	// Check if state is stale (>5 minutes old)
	age := time.Now().Unix() - state.Timestamp
	if age > 300 {
		return nil // Stale state
	}

	return &state
}

// formatWezTermContext builds context string from WezTerm state
func formatWezTermContext(wt *WezTermState) string {
	if wt == nil {
		return ""
	}

	parts := []string{}

	// Workspace awareness
	if wt.Workspace != "" && wt.Workspace != "default" {
		parts = append(parts, fmt.Sprintf("WezTerm workspace: %s", wt.Workspace))
	}

	// Layout awareness
	if wt.TabCount > 1 || wt.PaneCount > 1 {
		parts = append(parts, fmt.Sprintf("Terminal layout: %d tabs, %d panes", wt.TabCount, wt.PaneCount))
	}

	// Focus awareness
	if !wt.Focused {
		parts = append(parts, "Terminal was unfocused (returning to session)")
	}

	// Recent event
	switch wt.Event {
	case "focus_gained":
		parts = append(parts, "Terminal focus just restored")
	case "config_reloaded":
		parts = append(parts, "WezTerm config was reloaded")
	}

	if len(parts) == 0 {
		return ""
	}

	return "**Terminal Context:** " + fmt.Sprintf("%v", parts)
}
