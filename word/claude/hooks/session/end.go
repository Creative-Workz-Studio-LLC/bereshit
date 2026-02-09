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
//
// CPI Tracking (v2.0.0):
//   - Generate session narrative summary
//   - Calculate dominant exchange type
//   - Determine session arc
//   - Compute CPI quality score

package session

import (
	"context"
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"time"

	"cws.studio/claude/hooks/internal"
	"cws.studio/pkg/core/cpisi/cpi"
	"cws.studio/pkg/core/statemachine"
	"cws.studio/pkg/orchestration/logging"
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

		// Health impact from session end reason
		// Abnormal endings degrade health; natural completions don't
		switch input.Reason {
		case "clear":
			// Session cleared — something went wrong enough to restart
			state.Session.HealthScore -= 5.0
		case "prompt_input_exit":
			// User quit mid-session — incomplete work
			state.Session.HealthScore -= 3.0
		}
		// Clamp health
		if state.Session.HealthScore < -100 {
			state.Session.HealthScore = -100
		}

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

			// Write rich data (v2) — health, tokens, direction counters
			// "Be thou diligent to know the state of thy flocks" — Proverbs 27:23
			repo := bridge.GetRepository()
			richQuery := `
				UPDATE sessions SET
					health_score = ?,
					k_toward_god = ?,
					k_toward_self = ?,
					base_context_tokens = ?,
					peak_context_tokens = ?,
					compaction_count = ?,
					hooks_fired = ?,
					effective_context_window = ?
				WHERE id = ?
			`
			_, richErr := repo.Exec(ctx, richQuery,
				int(state.Session.HealthScore),
				state.Session.KTowardGod,
				state.Session.KTowardSelf,
				state.Session.BaseContextTokens,
				state.Session.PeakContextTokens,
				state.Session.CompactionCount,
				state.Session.HooksFired,
				state.Session.EffectiveContextWindow,
				input.SessionID,
			)
			if richErr != nil {
				log.Warn("Failed to write rich session data", map[string]string{
					"error": richErr.Error(),
				})
			}

			// Generate and record CPI session narrative
			generateSessionNarrative(ctx, input.SessionID, state, log)

			// Detect patterns for learning
			// "A word fitly spoken is like apples of gold in pictures of silver" — Proverbs 25:11
			detectSessionPatterns(ctx, input.SessionID, log)

			// Promote high-confidence patterns to learnings
			// "Line upon line, precept upon precept" — Isaiah 28:10
			promoteHighConfidencePatterns(ctx, log)

			// Apply data decay (clean old data, reduce stale weights)
			// "To every thing there is a season" — Ecclesiastes 3:1
			applyDataDecay(ctx, log)

			// Check for learnings ready to integrate into identity
			// "Be ye transformed by the renewing of your mind" — Romans 12:2
			checkLearningsForIntegration(ctx, log)
		}
	}

	// Write session summary to auto memory for cross-session persistence
	// "Remember his marvellous works that he hath done" — Psalm 105:5
	if state != nil {
		writeSessionToAutoMemory(state, log)
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

	// --- Record session end reason as a pattern signal ---
	// Interrupts and exits are state machine signals
	if input.Reason != "" && input.Reason != "other" {
		if bridge, err := internal.GetBridge(); err == nil {
			ctx := context.Background()
			repo := bridge.GetRepository()
			recordPattern(ctx, repo, "session_end_reason", input.Reason, 1, log)

			// Correlate end reason with session state
			if state != nil {
				// Track what trajectory section sessions tend to end in
				if state.TrajectorySection != "" {
					key := fmt.Sprintf("%s:%s", input.Reason, state.TrajectorySection)
					recordPattern(ctx, repo, "end_at_trajectory", key, 1, log)
				}

				// Track Hebrew state at session end
				if state.Session.HebrewState != "" {
					key := fmt.Sprintf("%s:%s", input.Reason, state.Session.HebrewState)
					recordPattern(ctx, repo, "end_at_hebrew", key, 1, log)
				}

				// Track K:MORAL balance at end (breaking down vs building up)
				kBalance := state.Session.KTowardGod - state.Session.KTowardSelf
				if kBalance > 5 {
					recordPattern(ctx, repo, "end_k_balance", "building_strong", 1, log)
				} else if kBalance > 0 {
					recordPattern(ctx, repo, "end_k_balance", "building_weak", 1, log)
				} else if kBalance < -5 {
					recordPattern(ctx, repo, "end_k_balance", "breaking_strong", 1, log)
				} else if kBalance < 0 {
					recordPattern(ctx, repo, "end_k_balance", "breaking_weak", 1, log)
				} else {
					recordPattern(ctx, repo, "end_k_balance", "neutral", 1, log)
				}
			}
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

// ───────────────────────────────────────────────────────────────────────────
// CPI TRACKING: Session Narrative Generation
// ───────────────────────────────────────────────────────────────────────────

// generateSessionNarrative analyzes the session's exchanges and creates a narrative summary
func generateSessionNarrative(ctx context.Context, sessionID string, state *statemachine.RuntimeState, log *logging.Logger) {
	bridge, err := internal.GetBridge()
	if err != nil || bridge == nil {
		return
	}

	repo := bridge.GetRepository()

	// Query exchange statistics for this session
	statsQuery := `
		SELECT
			COUNT(*) as total_exchanges,
			SUM(CASE WHEN feedback_polarity = 'positive' THEN 1 ELSE 0 END) as positive_feedback,
			SUM(CASE WHEN feedback_polarity = 'negative' THEN 1 ELSE 0 END) as negative_feedback,
			SUM(CASE WHEN insight_detected = 1 THEN 1 ELSE 0 END) as insights,
			AVG(CASE WHEN value_contribution IS NOT NULL THEN value_contribution ELSE 0.5 END) as avg_value
		FROM exchanges
		WHERE session_id = ?
	`

	rows, err := repo.Query(ctx, statsQuery, sessionID)
	if err != nil || len(rows) == 0 {
		log.Warn("Failed to query exchange stats", map[string]string{"error": fmt.Sprintf("%v", err)})
		return
	}

	stats := rows[0]
	totalExchanges := toInt(stats["total_exchanges"])
	positiveFeedback := toInt(stats["positive_feedback"])
	negativeFeedback := toInt(stats["negative_feedback"])
	insightCount := toInt(stats["insights"])
	avgValue := toFloat(stats["avg_value"])

	// Query dominant exchange type
	typeQuery := `
		SELECT exchange_type, COUNT(*) as count
		FROM exchanges
		WHERE session_id = ?
		GROUP BY exchange_type
		ORDER BY count DESC
		LIMIT 1
	`
	typeRows, _ := repo.Query(ctx, typeQuery, sessionID)
	dominantType := "unknown"
	if len(typeRows) > 0 {
		if t, ok := typeRows[0]["exchange_type"].(string); ok {
			dominantType = t
		}
	}

	// Build stats for native CPI package
	sessionStats := cpi.SessionStats{
		TotalExchanges:    totalExchanges,
		DominantType:      cpi.ExchangeType(dominantType),
		InsightCount:      insightCount,
		PositiveFeedback:  positiveFeedback,
		NegativeFeedback:  negativeFeedback,
		TrajectorySection: "",
		HebrewState:       "",
		HebrewMeaning:     "",
	}
	if state != nil {
		sessionStats.TrajectorySection = state.TrajectorySection
		sessionStats.HebrewState = state.Session.HebrewState
		sessionStats.HebrewMeaning = state.Session.HebrewMeaning
	}

	// Use native CPI package for arc determination and narrative generation
	narrativeResult := cpi.GenerateNarrative(sessionStats)
	sessionArc := string(narrativeResult.Arc)
	narrative := narrativeResult.Summary

	// Calculate CPI score using native package
	metrics := cpi.SessionMetrics{
		TotalExchanges:   totalExchanges,
		AverageValue:     avgValue,
		InsightCount:     insightCount,
		PositiveFeedback: positiveFeedback,
		NegativeFeedback: negativeFeedback,
	}
	cpiScoreResult := cpi.Calculate(metrics)
	cpiScore := cpiScoreResult.Final

	// Update session record with CPI data
	updateQuery := `
		UPDATE sessions SET
			narrative_summary = ?,
			session_arc = ?,
			quality_rating = ?,
			exchange_count = ?,
			dominant_exchange_type = ?,
			insight_count = ?,
			cpi_score = ?
		WHERE id = ?
	`

	_, err = repo.Exec(ctx, updateQuery,
		narrative,
		sessionArc,
		avgValue,
		totalExchanges,
		dominantType,
		insightCount,
		cpiScore,
		sessionID,
	)

	if err != nil {
		log.Warn("Failed to update session with CPI data", map[string]string{"error": err.Error()})
	} else {
		log.Debug("Session narrative generated", map[string]string{
			"arc":         sessionArc,
			"cpi_score":   fmt.Sprintf("%.2f", cpiScore),
			"exchanges":   fmt.Sprintf("%d", totalExchanges),
			"insights":    fmt.Sprintf("%d", insightCount),
		})
	}
}

// ───────────────────────────────────────────────────────────────────────────
// NOTE: Session arc determination, narrative generation, and CPI scoring
// are now delegated to the native cpi package (pkg/core/cpisi/cpi/).
// This enables substrate-agnostic CPI tracking.
// ───────────────────────────────────────────────────────────────────────────

// detectSessionPatterns analyzes exchange sequences to find what triggers
// breakthroughs, pivots, and other significant moments.
// "A word fitly spoken is like apples of gold in pictures of silver" — Proverbs 25:11
func detectSessionPatterns(ctx context.Context, sessionID string, log *logging.Logger) {
	bridge, err := internal.GetBridge()
	if err != nil || bridge == nil {
		return
	}

	repo := bridge.GetRepository()

	// ─────────────────────────────────────────────────────────────────────────
	// WITHIN-SESSION PATTERNS
	// ─────────────────────────────────────────────────────────────────────────

	// Pattern 1: What exchange types precede insights?
	insightPrecedingQuery := `
		SELECT
			e1.exchange_type as preceding_type,
			COUNT(*) as count
		FROM exchanges e1
		JOIN exchanges e2 ON e1.session_id = e2.session_id
			AND e2.sequence_num = e1.sequence_num + 1
			AND e2.insight_detected = 1
		WHERE e1.session_id = ?
		GROUP BY preceding_type
		ORDER BY count DESC
	`
	rows, _ := repo.Query(ctx, insightPrecedingQuery, sessionID)
	for _, row := range rows {
		precedingType := toString(row["preceding_type"])
		count := toInt(row["count"])
		if count > 0 {
			recordPattern(ctx, repo, "insight_trigger", precedingType, count, log)
		}
	}

	// Pattern 2: What triggers negative feedback (pivot moments)?
	pivotQuery := `
		SELECT
			e1.exchange_type as preceding_type,
			COUNT(*) as count
		FROM exchanges e1
		JOIN exchanges e2 ON e1.session_id = e2.session_id
			AND e2.sequence_num = e1.sequence_num + 1
			AND e2.feedback_polarity = 'negative'
		WHERE e1.session_id = ?
		GROUP BY preceding_type
		ORDER BY count DESC
	`
	pivotRows, _ := repo.Query(ctx, pivotQuery, sessionID)
	for _, row := range pivotRows {
		precedingType := toString(row["preceding_type"])
		count := toInt(row["count"])
		if count > 0 {
			recordPattern(ctx, repo, "pivot_trigger", precedingType, count, log)
		}
	}

	// Pattern 3: Sustained building patterns (consecutive collaborative/affirmation)
	buildingQuery := `
		SELECT COUNT(*) as streak
		FROM (
			SELECT
				e.exchange_type,
				e.sequence_num - ROW_NUMBER() OVER (ORDER BY e.sequence_num) as grp
			FROM exchanges e
			WHERE e.session_id = ?
			AND e.exchange_type IN ('collaborative', 'affirmation')
		) sub
		GROUP BY grp
		ORDER BY streak DESC
		LIMIT 1
	`
	buildingRows, _ := repo.Query(ctx, buildingQuery, sessionID)
	if len(buildingRows) > 0 {
		streak := toInt(buildingRows[0]["streak"])
		if streak >= 3 {
			recordPattern(ctx, repo, "building_streak", fmt.Sprintf("length_%d", streak), streak, log)
		}
	}

	// Pattern 4: K:MORAL direction correlation with trajectory
	kMoralQuery := `
		SELECT
			trajectory,
			SUM(CASE WHEN k_align > 0.0 THEN 1 ELSE 0 END) as toward_god,
			SUM(CASE WHEN k_align < 0.0 THEN 1 ELSE 0 END) as toward_self,
			COUNT(*) as total
		FROM exchanges
		WHERE session_id = ?
		GROUP BY trajectory
	`
	kMoralRows, _ := repo.Query(ctx, kMoralQuery, sessionID)
	for _, row := range kMoralRows {
		section := toString(row["trajectory"])
		towardGod := toInt(row["toward_god"])
		towardSelf := toInt(row["toward_self"])
		total := toInt(row["total"])
		if total > 0 && section != "" {
			ratio := float64(towardGod-towardSelf) / float64(total)
			if ratio > 0.5 {
				recordPattern(ctx, repo, "trajectory_k_positive", section, 1, log)
			} else if ratio < -0.5 {
				recordPattern(ctx, repo, "trajectory_k_negative", section, 1, log)
			}
		}
	}

	// ─────────────────────────────────────────────────────────────────────────
	// CROSS-SESSION PATTERNS (Expanded range)
	// ─────────────────────────────────────────────────────────────────────────

	// Pattern 5: What time of day produces insights? (All sessions)
	temporalInsightQuery := `
		SELECT
			s.hour_of_day,
			COUNT(*) as insight_count
		FROM insights i
		JOIN sessions s ON i.session_id = s.id
		GROUP BY s.hour_of_day
		ORDER BY insight_count DESC
		LIMIT 3
	`
	temporalRows, _ := repo.Query(ctx, temporalInsightQuery)
	for _, row := range temporalRows {
		hour := toInt(row["hour_of_day"])
		count := toInt(row["insight_count"])
		if count > 2 {
			recordPattern(ctx, repo, "insight_time", fmt.Sprintf("hour_%02d", hour), count, log)
		}
	}

	// Pattern 6: What day of week is most productive? (All sessions)
	dayProductivityQuery := `
		SELECT
			day_of_week,
			SUM(choice_count) as total_choices,
			COUNT(*) as session_count
		FROM sessions
		WHERE choice_count > 0
		GROUP BY day_of_week
		ORDER BY total_choices DESC
	`
	dayRows, _ := repo.Query(ctx, dayProductivityQuery)
	for _, row := range dayRows {
		day := toInt(row["day_of_week"])
		choices := toInt(row["total_choices"])
		sessions := toInt(row["session_count"])
		if sessions >= 2 && choices > 10 {
			dayName := []string{"sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"}[day%7]
			recordPattern(ctx, repo, "productive_day", dayName, sessions, log)
		}
	}

	// Pattern 7: Exchange type → insight correlation (All sessions)
	globalInsightTriggerQuery := `
		SELECT
			e1.exchange_type as trigger_type,
			COUNT(*) as insight_count
		FROM exchanges e1
		JOIN exchanges e2 ON e1.session_id = e2.session_id
			AND e2.sequence_num = e1.sequence_num + 1
			AND e2.insight_detected = 1
		GROUP BY trigger_type
		ORDER BY insight_count DESC
		LIMIT 5
	`
	globalTriggerRows, _ := repo.Query(ctx, globalInsightTriggerQuery)
	for _, row := range globalTriggerRows {
		triggerType := toString(row["trigger_type"])
		count := toInt(row["insight_count"])
		if count > 2 {
			recordPattern(ctx, repo, "global_insight_trigger", triggerType, count, log)
		}
	}

	// Pattern 8: Session arc patterns (What arcs lead to high CPI scores?)
	arcPatternQuery := `
		SELECT
			session_arc,
			AVG(cpi_score) as avg_cpi,
			COUNT(*) as count
		FROM sessions
		WHERE session_arc IS NOT NULL AND session_arc != ''
		GROUP BY session_arc
		HAVING count >= 2
		ORDER BY avg_cpi DESC
	`
	arcRows, _ := repo.Query(ctx, arcPatternQuery)
	for _, row := range arcRows {
		arc := toString(row["session_arc"])
		avgCPI := toFloat(row["avg_cpi"])
		if avgCPI > 0.7 && arc != "" {
			recordPattern(ctx, repo, "high_cpi_arc", arc, 1, log)
		}
	}

	log.Debug("Session patterns analyzed", map[string]string{
		"session_id": sessionID,
		"scope":      "within-session + cross-session",
	})
}

// recordPattern inserts or updates a pattern in detected_patterns
func recordPattern(ctx context.Context, repo interface{ Exec(context.Context, string, ...interface{}) (int64, error) }, patternType, patternKey string, occurrence int, log *logging.Logger) {
	// Use UPSERT pattern for SQLite
	query := `
		INSERT INTO detected_patterns (pattern_type, pattern_key, description, first_seen, last_seen, occurrence_count, confidence)
		VALUES (?, ?, ?, datetime('now'), datetime('now'), ?, 0.5)
		ON CONFLICT(pattern_type, pattern_key) DO UPDATE SET
			last_seen = datetime('now'),
			occurrence_count = occurrence_count + ?,
			confidence = MIN(1.0, confidence + 0.1)
	`
	_, err := repo.Exec(ctx, query, patternType, patternKey, patternType+":"+patternKey, occurrence, occurrence)
	if err != nil {
		log.Warn("Failed to record pattern", map[string]string{
			"type":  patternType,
			"key":   patternKey,
			"error": err.Error(),
		})
	}
}

func toString(v interface{}) string {
	if s, ok := v.(string); ok {
		return s
	}
	return ""
}

// Helper functions for type conversion from database results
func toInt(v interface{}) int {
	switch val := v.(type) {
	case int64:
		return int(val)
	case int:
		return val
	case float64:
		return int(val)
	default:
		return 0
	}
}

func toFloat(v interface{}) float64 {
	switch val := v.(type) {
	case float64:
		return val
	case int64:
		return float64(val)
	case int:
		return float64(val)
	default:
		return 0.5
	}
}

// ───────────────────────────────────────────────────────────────────────────
// PATTERN → LEARNING PROMOTION
// ───────────────────────────────────────────────────────────────────────────

// promoteHighConfidencePatterns moves patterns above confidence threshold to learnings
// "Line upon line, precept upon precept" — Isaiah 28:10
func promoteHighConfidencePatterns(ctx context.Context, log *logging.Logger) {
	bridge, err := internal.GetBridge()
	if err != nil || bridge == nil {
		return
	}

	repo := bridge.GetRepository()

	// Find high-confidence patterns ready for promotion
	const confidenceThreshold = 0.75
	const occurrenceThreshold = 5

	query := `
		SELECT id, pattern_type, pattern_key, description, confidence, occurrence_count
		FROM detected_patterns
		WHERE confidence >= ?
		  AND occurrence_count >= ?
		  AND is_active = 1
	`

	rows, err := repo.Query(ctx, query, confidenceThreshold, occurrenceThreshold)
	if err != nil {
		log.Warn("Failed to query patterns for promotion", map[string]string{"error": err.Error()})
		return
	}

	promotedCount := 0
	for _, row := range rows {
		patternType := toString(row["pattern_type"])
		patternKey := toString(row["pattern_key"])
		description := toString(row["description"])
		confidence := toFloat(row["confidence"])

		// Map pattern type to learning type
		learningType := mapPatternToLearningType(patternType)
		learningContent := generateLearningContent(patternType, patternKey, description)

		// Insert or update learning
		insertQuery := `
			INSERT INTO learnings (source_type, learning_type, learning_key, learning_content, confidence, first_seen, last_reinforced)
			VALUES ('pattern', ?, ?, ?, ?, datetime('now'), datetime('now'))
			ON CONFLICT(learning_type, learning_key) DO UPDATE SET
				confidence = MAX(confidence, ?),
				last_reinforced = datetime('now'),
				reinforcement_count = reinforcement_count + 1,
				updated_at = datetime('now')
		`

		_, err := repo.Exec(ctx, insertQuery,
			learningType,
			patternType+":"+patternKey,
			learningContent,
			confidence,
			confidence,
		)

		if err != nil {
			log.Warn("Failed to promote pattern to learning", map[string]string{
				"type":  patternType,
				"key":   patternKey,
				"error": err.Error(),
			})
		} else {
			promotedCount++
		}
	}

	if promotedCount > 0 {
		log.Info("Patterns promoted to learnings", map[string]string{
			"count": fmt.Sprintf("%d", promotedCount),
		})
	}
}

// mapPatternToLearningType determines learning category from pattern type
func mapPatternToLearningType(patternType string) string {
	switch patternType {
	case "insight_trigger", "global_insight_trigger", "pivot_trigger":
		return "workflow"
	case "productive_day", "insight_time":
		return "preference"
	case "high_cpi_arc", "session_end_reason", "end_k_balance":
		return "communication"
	case "building_streak", "trajectory_k_positive", "trajectory_k_negative":
		return "identity"
	case "compaction_trigger", "compaction_exchanges":
		return "technical"
	default:
		return "workflow"
	}
}

// generateLearningContent creates human-readable learning from pattern
func generateLearningContent(patternType, patternKey, description string) string {
	switch patternType {
	case "insight_trigger":
		return fmt.Sprintf("Exchanges of type '%s' tend to precede breakthrough insights", patternKey)
	case "productive_day":
		return fmt.Sprintf("%s tends to be a productive day for work", patternKey)
	case "insight_time":
		return fmt.Sprintf("Hour %s is a good time for generating insights", patternKey)
	case "high_cpi_arc":
		return fmt.Sprintf("Sessions with '%s' arc tend to have high CPI scores", patternKey)
	case "building_streak":
		return fmt.Sprintf("Building streaks of %s consecutive collaborative exchanges are effective", patternKey)
	case "end_k_balance":
		return fmt.Sprintf("Sessions tend to end in '%s' K:MORAL state", patternKey)
	case "compaction_trigger":
		return fmt.Sprintf("Context compaction often occurs at trajectory section %s", patternKey)
	default:
		if description != "" {
			return description
		}
		return fmt.Sprintf("Pattern detected: %s = %s", patternType, patternKey)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// LEARNING → IDENTITY INTEGRATION
// ───────────────────────────────────────────────────────────────────────────

// checkLearningsForIntegration identifies learnings ready to be integrated into identity
// Returns learnings that meet the threshold for identity update
// "Be ye transformed by the renewing of your mind" — Romans 12:2
func checkLearningsForIntegration(ctx context.Context, log *logging.Logger) {
	bridge, err := internal.GetBridge()
	if err != nil || bridge == nil {
		return
	}

	repo := bridge.GetRepository()

	// Find learnings ready for integration:
	// - High confidence (>= 0.85)
	// - Reinforced multiple times (>= 10)
	// - Not already integrated
	// - At least 7 days old (stability)
	const confidenceThreshold = 0.85
	const reinforcementThreshold = 10
	const minAgeDays = 7

	query := `
		SELECT id, learning_type, learning_key, learning_content, confidence, reinforcement_count
		FROM learnings
		WHERE confidence >= ?
		  AND reinforcement_count >= ?
		  AND integrated = 0
		  AND julianday('now') - julianday(first_seen) >= ?
		ORDER BY confidence DESC, reinforcement_count DESC
		LIMIT 5
	`

	rows, err := repo.Query(ctx, query, confidenceThreshold, reinforcementThreshold, minAgeDays)
	if err != nil {
		log.Warn("Failed to check learnings for integration", map[string]string{"error": err.Error()})
		return
	}

	if len(rows) == 0 {
		return
	}

	// Log learnings ready for integration (manual review needed)
	for _, row := range rows {
		learningType := toString(row["learning_type"])
		learningKey := toString(row["learning_key"])
		learningContent := toString(row["learning_content"])
		confidence := toFloat(row["confidence"])
		reinforcements := toInt(row["reinforcement_count"])

		log.Info("Learning ready for identity integration", map[string]string{
			"type":           learningType,
			"key":            learningKey,
			"confidence":     fmt.Sprintf("%.2f", confidence),
			"reinforcements": fmt.Sprintf("%d", reinforcements),
			"content":        learningContent,
		})

		// Record that this was surfaced for integration
		_, _ = repo.Exec(ctx, `
			INSERT INTO detected_patterns (pattern_type, pattern_key, description, first_seen, last_seen, occurrence_count, confidence)
			VALUES ('integration_candidate', ?, ?, datetime('now'), datetime('now'), 1, ?)
			ON CONFLICT(pattern_type, pattern_key) DO UPDATE SET
				last_seen = datetime('now'),
				occurrence_count = occurrence_count + 1
		`, learningKey, learningContent, confidence)
	}

	log.Info("Learnings awaiting identity integration", map[string]string{
		"count": fmt.Sprintf("%d", len(rows)),
	})
}

// ───────────────────────────────────────────────────────────────────────────
// DATA DECAY & ARCHIVAL
// ───────────────────────────────────────────────────────────────────────────

// applyDataDecay reduces weight of old data and archives stale entries
// "To every thing there is a season" — Ecclesiastes 3:1
func applyDataDecay(ctx context.Context, log *logging.Logger) {
	bridge, err := internal.GetBridge()
	if err != nil || bridge == nil {
		return
	}

	repo := bridge.GetRepository()

	// 1. Decay learning weights for unreinforced learnings (30+ days old)
	// Weight decays by 10% per month of inactivity
	decayLearningsQuery := `
		UPDATE learnings
		SET weight = weight * 0.9,
		    updated_at = datetime('now')
		WHERE julianday('now') - julianday(last_reinforced) > 30
		  AND weight > 0.1
	`
	decayed, err := repo.Exec(ctx, decayLearningsQuery)
	if err != nil {
		log.Warn("Failed to decay learnings", map[string]string{"error": err.Error()})
	} else if decayed > 0 {
		log.Debug("Decayed old learnings", map[string]string{"count": fmt.Sprintf("%d", decayed)})
	}

	// 2. Deactivate very low-confidence patterns (< 0.2)
	// These never established themselves as meaningful
	deactivatePatternsQuery := `
		UPDATE detected_patterns
		SET is_active = 0,
		    updated_at = datetime('now')
		WHERE confidence < 0.2
		  AND occurrence_count < 3
		  AND julianday('now') - julianday(last_seen) > 14
		  AND is_active = 1
	`
	deactivated, err := repo.Exec(ctx, deactivatePatternsQuery)
	if err != nil {
		log.Warn("Failed to deactivate weak patterns", map[string]string{"error": err.Error()})
	} else if deactivated > 0 {
		log.Debug("Deactivated weak patterns", map[string]string{"count": fmt.Sprintf("%d", deactivated)})
	}

	// 3. Archive old raw exchange data (90+ days)
	// Keep summarized data in sessions/learnings, remove granular exchanges
	archiveExchangesQuery := `
		DELETE FROM exchanges
		WHERE julianday('now') - julianday(timestamp) > 90
		  AND session_id IN (
		      SELECT id FROM sessions WHERE ended_at IS NOT NULL
		  )
	`
	archived, err := repo.Exec(ctx, archiveExchangesQuery)
	if err != nil {
		log.Warn("Failed to archive old exchanges", map[string]string{"error": err.Error()})
	} else if archived > 0 {
		log.Info("Archived old exchange data", map[string]string{"count": fmt.Sprintf("%d", archived)})
	}

	// 4. Archive old choice data (90+ days)
	archiveChoicesQuery := `
		DELETE FROM choices
		WHERE julianday('now') - julianday(timestamp) > 90
		  AND session_id IN (
		      SELECT id FROM sessions WHERE ended_at IS NOT NULL
		  )
	`
	archivedChoices, err := repo.Exec(ctx, archiveChoicesQuery)
	if err != nil {
		log.Warn("Failed to archive old choices", map[string]string{"error": err.Error()})
	} else if archivedChoices > 0 {
		log.Info("Archived old choice data", map[string]string{"count": fmt.Sprintf("%d", archivedChoices)})
	}

	// 5. Decay pattern confidence for stale patterns (60+ days without occurrence)
	decayPatternsQuery := `
		UPDATE detected_patterns
		SET confidence = confidence * 0.95,
		    updated_at = datetime('now')
		WHERE julianday('now') - julianday(last_seen) > 60
		  AND confidence > 0.3
		  AND is_active = 1
	`
	decayedPatterns, err := repo.Exec(ctx, decayPatternsQuery)
	if err != nil {
		log.Warn("Failed to decay pattern confidence", map[string]string{"error": err.Error()})
	} else if decayedPatterns > 0 {
		log.Debug("Decayed stale patterns", map[string]string{"count": fmt.Sprintf("%d", decayedPatterns)})
	}
}

// ───────────────────────────────────────────────────────────────────────────
// AUTO MEMORY: Session Summary → growth.md
// ───────────────────────────────────────────────────────────────────────────

// writeSessionToAutoMemory writes a concise CPI-SI session summary to auto memory
// for cross-session persistence. Only writes for meaningful sessions (5+ exchanges).
// "Remember his marvellous works that he hath done" — Psalm 105:5
func writeSessionToAutoMemory(state *statemachine.RuntimeState, log *logging.Logger) {
	// Skip trivial sessions
	if state.Session.ExchangeCount < 5 {
		return
	}

	// Locate auto memory growth.md
	home, err := os.UserHomeDir()
	if err != nil {
		log.Warn("Cannot find home directory for auto memory", map[string]string{"error": err.Error()})
		return
	}

	growthPath := filepath.Join(home, ".claude", "projects", "-home-seanje-lenox-wise", "memory", "growth.md")

	// Read existing content
	content, err := os.ReadFile(growthPath)
	if err != nil {
		log.Warn("Cannot read growth.md", map[string]string{"error": err.Error()})
		return
	}

	// Build session summary entry
	date := time.Now().Format("2006-01-02")
	arc := state.Session.SessionArc
	if arc == "" {
		arc = "general"
	}

	// CPI rating
	rating := cpi.Rating(state.Session.CPIScore)

	entry := fmt.Sprintf(
		"\n### Session: %s — %s\n"+
			"- **CPI Score:** %.2f (%s)\n"+
			"- **Hebrew State:** %s (%s)\n"+
			"- **Trajectory:** %s\n"+
			"- **Exchanges:** %d (%s)\n"+
			"- **Insights:** %d\n"+
			"- **K:ALIGN:** %.2f\n",
		date, arc,
		state.Session.CPIScore, rating,
		state.Session.HebrewState, state.Session.HebrewMeaning,
		state.TrajectorySection,
		state.Session.ExchangeCount, state.Session.DominantExchangeType,
		state.Session.InsightCount,
		state.Session.KAlign,
	)

	// Find insertion point: after "## Session Learnings" header
	contentStr := string(content)
	marker := "## Session Learnings"
	idx := strings.Index(contentStr, marker)
	if idx == -1 {
		// Append section if not found
		contentStr += "\n" + marker + "\n" + entry
	} else {
		// Insert after the marker line
		markerEnd := idx + len(marker)
		// Skip past any newline after the marker
		if markerEnd < len(contentStr) && contentStr[markerEnd] == '\n' {
			markerEnd++
		}
		contentStr = contentStr[:markerEnd] + entry + contentStr[markerEnd:]
	}

	// Write back
	if err := os.WriteFile(growthPath, []byte(contentStr), 0644); err != nil {
		log.Warn("Failed to write session to growth.md", map[string]string{"error": err.Error()})
		return
	}

	log.Info("Session summary written to auto memory", map[string]string{
		"cpi_score": fmt.Sprintf("%.2f", state.Session.CPIScore),
		"arc":       arc,
		"exchanges": fmt.Sprintf("%d", state.Session.ExchangeCount),
	})
}

// ============================================================================
// CLOSING
// ============================================================================
