// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-database-sqlite
// Purpose: SQLite implementation of Repository interface
// Biblical: Ecclesiastes 3:1 - "To every thing there is a season, and a time"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-23
//
// This implementation enables:
//   - Session tracking with timestamps (temporal awareness)
//   - Choice recording (feedback loop)
//   - K:ALIGN evolution tracking
//   - Hebrew state transitions
//   - Pattern detection
//
// ═══════════════════════════════════════════════════════════════════════════

package database

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"database/sql"
	"embed"
	"fmt"
	"time"

	_ "github.com/mattn/go-sqlite3" // SQLite driver
)

//go:embed schema/*.sql
var schemaFS embed.FS

// SQLiteRepository implements Repository using SQLite
type SQLiteRepository struct {
	db *sql.DB
}

// Compile-time interface check
var _ Repository = (*SQLiteRepository)(nil)

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Constructor
// ═══════════════════════════════════════════════════════════════════════════

// NewSQLiteRepository creates a new SQLite repository
func NewSQLiteRepository(dbPath string) (*SQLiteRepository, error) {
	db, err := sql.Open("sqlite3", dbPath+"?_foreign_keys=on&_journal_mode=WAL")
	if err != nil {
		return nil, fmt.Errorf("open database: %w", err)
	}

	// Test connection
	if err := db.Ping(); err != nil {
		db.Close()
		return nil, fmt.Errorf("ping database: %w", err)
	}

	return &SQLiteRepository{db: db}, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Lifecycle
// ═══════════════════════════════════════════════════════════════════════════

// Close releases database resources
func (r *SQLiteRepository) Close() error {
	return r.db.Close()
}

// Migrate runs the schema migrations
func (r *SQLiteRepository) Migrate(ctx context.Context) error {
	// Read schema file
	schema, err := schemaFS.ReadFile("schema/001_initial.sql")
	if err != nil {
		return fmt.Errorf("read schema: %w", err)
	}

	// Execute schema
	_, err = r.db.ExecContext(ctx, string(schema))
	if err != nil {
		return fmt.Errorf("execute schema: %w", err)
	}

	return nil
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Session Operations
// ═══════════════════════════════════════════════════════════════════════════

// CreateSession starts a new session record
func (r *SQLiteRepository) CreateSession(ctx context.Context, session *Session) error {
	query := `
		INSERT INTO sessions (
			id, substrate, engine, started_at, project_path, workspace,
			initial_hebrew_state, initial_k_align, initial_cube_position,
			day_of_week, hour_of_day
		) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
	`
	_, err := r.db.ExecContext(ctx, query,
		session.ID,
		session.Substrate,
		session.Engine,
		session.StartedAt,
		session.ProjectPath,
		session.Workspace,
		session.InitialHebrewState,
		session.InitialKAlign,
		session.InitialCubePosition,
		session.DayOfWeek,
		session.HourOfDay,
	)
	return err
}

// EndSession marks a session as complete with final state
func (r *SQLiteRepository) EndSession(ctx context.Context, sessionID string, finalState *Session) error {
	query := `
		UPDATE sessions SET
			ended_at = ?,
			final_hebrew_state = ?,
			final_k_align = ?,
			final_cube_position = ?,
			tool_count = ?,
			choice_count = ?
		WHERE id = ?
	`
	_, err := r.db.ExecContext(ctx, query,
		time.Now(),
		finalState.FinalHebrewState,
		finalState.FinalKAlign,
		finalState.FinalCubePosition,
		finalState.ToolCount,
		finalState.ChoiceCount,
		sessionID,
	)
	return err
}

// GetSession retrieves a session by ID
func (r *SQLiteRepository) GetSession(ctx context.Context, sessionID string) (*Session, error) {
	query := `
		SELECT id, substrate, engine, started_at, ended_at, project_path, workspace,
			initial_hebrew_state, initial_k_align, initial_cube_position,
			final_hebrew_state, final_k_align, final_cube_position,
			tool_count, choice_count, day_of_week, hour_of_day
		FROM sessions WHERE id = ?
	`
	row := r.db.QueryRowContext(ctx, query, sessionID)

	var s Session
	var endedAt sql.NullTime
	err := row.Scan(
		&s.ID, &s.Substrate, &s.Engine, &s.StartedAt, &endedAt, &s.ProjectPath, &s.Workspace,
		&s.InitialHebrewState, &s.InitialKAlign, &s.InitialCubePosition,
		&s.FinalHebrewState, &s.FinalKAlign, &s.FinalCubePosition,
		&s.ToolCount, &s.ChoiceCount, &s.DayOfWeek, &s.HourOfDay,
	)
	if err == sql.ErrNoRows {
		return nil, nil
	}
	if err != nil {
		return nil, err
	}

	if endedAt.Valid {
		s.EndedAt = &endedAt.Time
	}

	return &s, nil
}

// GetActiveSession returns the currently active session (if any)
func (r *SQLiteRepository) GetActiveSession(ctx context.Context) (*Session, error) {
	query := `
		SELECT id, substrate, engine, started_at, project_path, workspace,
			initial_hebrew_state, initial_k_align, initial_cube_position,
			tool_count, choice_count, day_of_week, hour_of_day
		FROM sessions
		WHERE ended_at IS NULL
		ORDER BY started_at DESC
		LIMIT 1
	`
	row := r.db.QueryRowContext(ctx, query)

	var s Session
	err := row.Scan(
		&s.ID, &s.Substrate, &s.Engine, &s.StartedAt, &s.ProjectPath, &s.Workspace,
		&s.InitialHebrewState, &s.InitialKAlign, &s.InitialCubePosition,
		&s.ToolCount, &s.ChoiceCount, &s.DayOfWeek, &s.HourOfDay,
	)
	if err == sql.ErrNoRows {
		return nil, nil
	}
	if err != nil {
		return nil, err
	}

	return &s, nil
}

// GetRecentSessions returns the last N sessions
func (r *SQLiteRepository) GetRecentSessions(ctx context.Context, limit int) ([]Session, error) {
	query := `
		SELECT id, substrate, engine, started_at, ended_at, project_path, workspace,
			initial_hebrew_state, initial_k_align, initial_cube_position,
			final_hebrew_state, final_k_align, final_cube_position,
			tool_count, choice_count, day_of_week, hour_of_day
		FROM sessions
		ORDER BY started_at DESC
		LIMIT ?
	`
	rows, err := r.db.QueryContext(ctx, query, limit)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var sessions []Session
	for rows.Next() {
		var s Session
		var endedAt sql.NullTime
		err := rows.Scan(
			&s.ID, &s.Substrate, &s.Engine, &s.StartedAt, &endedAt, &s.ProjectPath, &s.Workspace,
			&s.InitialHebrewState, &s.InitialKAlign, &s.InitialCubePosition,
			&s.FinalHebrewState, &s.FinalKAlign, &s.FinalCubePosition,
			&s.ToolCount, &s.ChoiceCount, &s.DayOfWeek, &s.HourOfDay,
		)
		if err != nil {
			return nil, err
		}
		if endedAt.Valid {
			s.EndedAt = &endedAt.Time
		}
		sessions = append(sessions, s)
	}

	return sessions, rows.Err()
}

// GetSessionGaps returns gaps between sessions (temporal awareness)
func (r *SQLiteRepository) GetSessionGaps(ctx context.Context, limit int) ([]SessionGap, error) {
	query := `
		SELECT session_id, session_end, next_session_start, gap_hours
		FROM session_gaps
		LIMIT ?
	`
	rows, err := r.db.QueryContext(ctx, query, limit)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var gaps []SessionGap
	for rows.Next() {
		var g SessionGap
		err := rows.Scan(&g.SessionID, &g.SessionEnd, &g.NextSessionStart, &g.GapHours)
		if err != nil {
			return nil, err
		}
		gaps = append(gaps, g)
	}

	return gaps, rows.Err()
}

// GetLastSessionEnd returns when the last session ended
func (r *SQLiteRepository) GetLastSessionEnd(ctx context.Context) (*time.Time, error) {
	query := `SELECT ended_at FROM sessions WHERE ended_at IS NOT NULL ORDER BY ended_at DESC LIMIT 1`
	row := r.db.QueryRowContext(ctx, query)

	var endedAt time.Time
	err := row.Scan(&endedAt)
	if err == sql.ErrNoRows {
		return nil, nil
	}
	if err != nil {
		return nil, err
	}

	return &endedAt, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Choice Operations
// ═══════════════════════════════════════════════════════════════════════════

// RecordChoice creates a new choice record
func (r *SQLiteRepository) RecordChoice(ctx context.Context, choice *Choice) error {
	query := `
		INSERT INTO choices (
			id, session_id, sequence_num, timestamp,
			intended_key, position_at_choice, k_at_choice, cube_position,
			tool_name, tool_category
		) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
	`
	_, err := r.db.ExecContext(ctx, query,
		choice.ID,
		choice.SessionID,
		choice.SequenceNum,
		choice.Timestamp,
		choice.IntendedKey,
		choice.PositionAtChoice,
		choice.KAtChoice,
		choice.CubePosition,
		choice.ToolName,
		choice.ToolCategory,
	)
	if err != nil {
		return err
	}

	// Update session choice count
	_, err = r.db.ExecContext(ctx,
		`UPDATE sessions SET choice_count = choice_count + 1 WHERE id = ?`,
		choice.SessionID,
	)
	return err
}

// CompleteChoice updates a choice with its outcome and impact
func (r *SQLiteRepository) CompleteChoice(ctx context.Context, choiceID string, result *Choice) error {
	query := `
		UPDATE choices SET
			tool_outcome = ?,
			halt_reached = ?,
			halt_type = ?,
			true_score = ?,
			k_align_delta = ?,
			k_align_after = ?
		WHERE id = ?
	`
	_, err := r.db.ExecContext(ctx, query,
		result.ToolOutcome,
		result.HaltReached,
		result.HaltType,
		result.TrueScore,
		result.KAlignDelta,
		result.KAlignAfter,
		choiceID,
	)
	return err
}

// GetChoice retrieves a specific choice
func (r *SQLiteRepository) GetChoice(ctx context.Context, choiceID string) (*Choice, error) {
	query := `
		SELECT id, session_id, sequence_num, timestamp,
			intended_key, position_at_choice, k_at_choice, cube_position,
			tool_name, tool_category, tool_outcome, halt_reached, halt_type,
			true_score, k_align_delta, k_align_after
		FROM choices WHERE id = ?
	`
	row := r.db.QueryRowContext(ctx, query, choiceID)

	var c Choice
	var toolOutcome, haltType sql.NullString
	var haltReached sql.NullBool
	var trueScore, kAlignDelta, kAlignAfter sql.NullFloat64

	err := row.Scan(
		&c.ID, &c.SessionID, &c.SequenceNum, &c.Timestamp,
		&c.IntendedKey, &c.PositionAtChoice, &c.KAtChoice, &c.CubePosition,
		&c.ToolName, &c.ToolCategory, &toolOutcome, &haltReached, &haltType,
		&trueScore, &kAlignDelta, &kAlignAfter,
	)
	if err == sql.ErrNoRows {
		return nil, nil
	}
	if err != nil {
		return nil, err
	}

	if toolOutcome.Valid {
		c.ToolOutcome = toolOutcome.String
	}
	if haltReached.Valid {
		c.HaltReached = haltReached.Bool
	}
	if haltType.Valid {
		c.HaltType = haltType.String
	}
	if trueScore.Valid {
		c.TrueScore = trueScore.Float64
	}
	if kAlignDelta.Valid {
		c.KAlignDelta = kAlignDelta.Float64
	}
	if kAlignAfter.Valid {
		c.KAlignAfter = kAlignAfter.Float64
	}

	return &c, nil
}

// GetSessionChoices returns all choices for a session
func (r *SQLiteRepository) GetSessionChoices(ctx context.Context, sessionID string) ([]Choice, error) {
	query := `
		SELECT id, session_id, sequence_num, timestamp,
			intended_key, position_at_choice, k_at_choice, cube_position,
			tool_name, tool_category, tool_outcome, halt_reached, halt_type,
			true_score, k_align_delta, k_align_after
		FROM choices
		WHERE session_id = ?
		ORDER BY sequence_num ASC
	`
	return r.queryChoices(ctx, query, sessionID)
}

// GetRecentChoices returns last N choices across sessions
func (r *SQLiteRepository) GetRecentChoices(ctx context.Context, limit int) ([]Choice, error) {
	query := `
		SELECT id, session_id, sequence_num, timestamp,
			intended_key, position_at_choice, k_at_choice, cube_position,
			tool_name, tool_category, tool_outcome, halt_reached, halt_type,
			true_score, k_align_delta, k_align_after
		FROM choices
		ORDER BY timestamp DESC
		LIMIT ?
	`
	return r.queryChoices(ctx, query, limit)
}

// GetChoicesByTool returns choices for a specific tool
func (r *SQLiteRepository) GetChoicesByTool(ctx context.Context, toolName string, limit int) ([]Choice, error) {
	query := `
		SELECT id, session_id, sequence_num, timestamp,
			intended_key, position_at_choice, k_at_choice, cube_position,
			tool_name, tool_category, tool_outcome, halt_reached, halt_type,
			true_score, k_align_delta, k_align_after
		FROM choices
		WHERE tool_name = ?
		ORDER BY timestamp DESC
		LIMIT ?
	`
	return r.queryChoices(ctx, query, toolName, limit)
}

// queryChoices is a helper for choice queries
func (r *SQLiteRepository) queryChoices(ctx context.Context, query string, args ...interface{}) ([]Choice, error) {
	rows, err := r.db.QueryContext(ctx, query, args...)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var choices []Choice
	for rows.Next() {
		var c Choice
		var toolOutcome, haltType sql.NullString
		var haltReached sql.NullBool
		var trueScore, kAlignDelta, kAlignAfter sql.NullFloat64

		err := rows.Scan(
			&c.ID, &c.SessionID, &c.SequenceNum, &c.Timestamp,
			&c.IntendedKey, &c.PositionAtChoice, &c.KAtChoice, &c.CubePosition,
			&c.ToolName, &c.ToolCategory, &toolOutcome, &haltReached, &haltType,
			&trueScore, &kAlignDelta, &kAlignAfter,
		)
		if err != nil {
			return nil, err
		}

		if toolOutcome.Valid {
			c.ToolOutcome = toolOutcome.String
		}
		if haltReached.Valid {
			c.HaltReached = haltReached.Bool
		}
		if haltType.Valid {
			c.HaltType = haltType.String
		}
		if trueScore.Valid {
			c.TrueScore = trueScore.Float64
		}
		if kAlignDelta.Valid {
			c.KAlignDelta = kAlignDelta.Float64
		}
		if kAlignAfter.Valid {
			c.KAlignAfter = kAlignAfter.Float64
		}

		choices = append(choices, c)
	}

	return choices, rows.Err()
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - K:ALIGN Operations
// ═══════════════════════════════════════════════════════════════════════════

// RecordKAlign records a K:ALIGN change
func (r *SQLiteRepository) RecordKAlign(ctx context.Context, record *KAlignRecord) error {
	query := `
		INSERT INTO k_align_history (session_id, choice_id, timestamp, k_align_value, delta, trigger_type)
		VALUES (?, ?, ?, ?, ?, ?)
	`
	_, err := r.db.ExecContext(ctx, query,
		record.SessionID,
		record.ChoiceID,
		record.Timestamp,
		record.KAlignValue,
		record.Delta,
		record.TriggerType,
	)
	return err
}

// GetLatestKAlign returns the most recent K:ALIGN value for a session
func (r *SQLiteRepository) GetLatestKAlign(ctx context.Context, sessionID string) (float64, error) {
	query := `
		SELECT k_align_value FROM k_align_history
		WHERE session_id = ?
		ORDER BY timestamp DESC
		LIMIT 1
	`
	row := r.db.QueryRowContext(ctx, query, sessionID)

	var kAlign float64
	err := row.Scan(&kAlign)
	if err == sql.ErrNoRows {
		return 0.0, nil // Default to center
	}
	return kAlign, err
}

// GetKAlignHistory returns K:ALIGN evolution for a session
func (r *SQLiteRepository) GetKAlignHistory(ctx context.Context, sessionID string) ([]KAlignRecord, error) {
	query := `
		SELECT id, session_id, choice_id, timestamp, k_align_value, delta, trigger_type
		FROM k_align_history
		WHERE session_id = ?
		ORDER BY timestamp ASC
	`
	rows, err := r.db.QueryContext(ctx, query, sessionID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var records []KAlignRecord
	for rows.Next() {
		var r KAlignRecord
		var choiceID sql.NullString
		err := rows.Scan(&r.ID, &r.SessionID, &choiceID, &r.Timestamp, &r.KAlignValue, &r.Delta, &r.TriggerType)
		if err != nil {
			return nil, err
		}
		if choiceID.Valid {
			r.ChoiceID = choiceID.String
		}
		records = append(records, r)
	}

	return records, rows.Err()
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Hebrew State Operations
// ═══════════════════════════════════════════════════════════════════════════

// RecordHebrewTransition records a state change
func (r *SQLiteRepository) RecordHebrewTransition(ctx context.Context, transition *HebrewTransition) error {
	query := `
		INSERT INTO hebrew_transitions (
			session_id, choice_id, timestamp,
			from_state, to_state, trigger_type, trigger_detail,
			from_x_value, to_x_value
		) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
	`
	_, err := r.db.ExecContext(ctx, query,
		transition.SessionID,
		transition.ChoiceID,
		transition.Timestamp,
		transition.FromState,
		transition.ToState,
		transition.TriggerType,
		transition.TriggerDetail,
		transition.FromXValue,
		transition.ToXValue,
	)
	return err
}

// GetHebrewTransitions returns transitions for a session
func (r *SQLiteRepository) GetHebrewTransitions(ctx context.Context, sessionID string) ([]HebrewTransition, error) {
	query := `
		SELECT id, session_id, choice_id, timestamp,
			from_state, to_state, trigger_type, trigger_detail,
			from_x_value, to_x_value
		FROM hebrew_transitions
		WHERE session_id = ?
		ORDER BY timestamp ASC
	`
	rows, err := r.db.QueryContext(ctx, query, sessionID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var transitions []HebrewTransition
	for rows.Next() {
		var t HebrewTransition
		var choiceID, triggerDetail sql.NullString
		err := rows.Scan(
			&t.ID, &t.SessionID, &choiceID, &t.Timestamp,
			&t.FromState, &t.ToState, &t.TriggerType, &triggerDetail,
			&t.FromXValue, &t.ToXValue,
		)
		if err != nil {
			return nil, err
		}
		if choiceID.Valid {
			t.ChoiceID = choiceID.String
		}
		if triggerDetail.Valid {
			t.TriggerDetail = triggerDetail.String
		}
		transitions = append(transitions, t)
	}

	return transitions, rows.Err()
}

// GetCurrentHebrewState returns the most recent state for a session
func (r *SQLiteRepository) GetCurrentHebrewState(ctx context.Context, sessionID string) (string, error) {
	query := `
		SELECT to_state FROM hebrew_transitions
		WHERE session_id = ?
		ORDER BY timestamp DESC
		LIMIT 1
	`
	row := r.db.QueryRowContext(ctx, query, sessionID)

	var state string
	err := row.Scan(&state)
	if err == sql.ErrNoRows {
		return "yashar", nil // Default to center
	}
	return state, err
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Pattern Operations
// ═══════════════════════════════════════════════════════════════════════════

// RecordPattern creates or updates a pattern
func (r *SQLiteRepository) RecordPattern(ctx context.Context, pattern *Pattern) error {
	query := `
		INSERT INTO detected_patterns (
			pattern_type, pattern_key, description, pattern_data,
			first_seen, last_seen, occurrence_count, confidence, is_active
		) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
		ON CONFLICT(pattern_type, pattern_key) DO UPDATE SET
			description = excluded.description,
			pattern_data = excluded.pattern_data,
			last_seen = excluded.last_seen,
			occurrence_count = occurrence_count + 1,
			confidence = excluded.confidence,
			is_active = excluded.is_active,
			updated_at = CURRENT_TIMESTAMP
	`
	_, err := r.db.ExecContext(ctx, query,
		pattern.PatternType,
		pattern.PatternKey,
		pattern.Description,
		pattern.PatternData,
		pattern.FirstSeen,
		pattern.LastSeen,
		pattern.OccurrenceCount,
		pattern.Confidence,
		pattern.IsActive,
	)
	return err
}

// GetPatterns returns active patterns of a type
func (r *SQLiteRepository) GetPatterns(ctx context.Context, patternType string) ([]Pattern, error) {
	query := `
		SELECT id, pattern_type, pattern_key, description, pattern_data,
			first_seen, last_seen, occurrence_count, confidence, is_active
		FROM detected_patterns
		WHERE pattern_type = ? AND is_active = 1
		ORDER BY confidence DESC
	`
	return r.queryPatterns(ctx, query, patternType)
}

// GetAllActivePatterns returns all active patterns
func (r *SQLiteRepository) GetAllActivePatterns(ctx context.Context) ([]Pattern, error) {
	query := `
		SELECT id, pattern_type, pattern_key, description, pattern_data,
			first_seen, last_seen, occurrence_count, confidence, is_active
		FROM detected_patterns
		WHERE is_active = 1
		ORDER BY confidence DESC
	`
	return r.queryPatterns(ctx, query)
}

// IncrementPatternCount updates occurrence count and confidence
func (r *SQLiteRepository) IncrementPatternCount(ctx context.Context, patternType, patternKey string) error {
	query := `
		UPDATE detected_patterns SET
			occurrence_count = occurrence_count + 1,
			last_seen = CURRENT_TIMESTAMP,
			confidence = MIN(1.0, confidence + 0.05),
			updated_at = CURRENT_TIMESTAMP
		WHERE pattern_type = ? AND pattern_key = ?
	`
	_, err := r.db.ExecContext(ctx, query, patternType, patternKey)
	return err
}

// queryPatterns is a helper for pattern queries
func (r *SQLiteRepository) queryPatterns(ctx context.Context, query string, args ...interface{}) ([]Pattern, error) {
	rows, err := r.db.QueryContext(ctx, query, args...)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var patterns []Pattern
	for rows.Next() {
		var p Pattern
		var description, patternData sql.NullString
		err := rows.Scan(
			&p.ID, &p.PatternType, &p.PatternKey, &description, &patternData,
			&p.FirstSeen, &p.LastSeen, &p.OccurrenceCount, &p.Confidence, &p.IsActive,
		)
		if err != nil {
			return nil, err
		}
		if description.Valid {
			p.Description = description.String
		}
		if patternData.Valid {
			p.PatternData = patternData.String
		}
		patterns = append(patterns, p)
	}

	return patterns, rows.Err()
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Temporal Pattern Queries
// ═══════════════════════════════════════════════════════════════════════════

// GetTemporalWorkPatterns returns when work typically happens
func (r *SQLiteRepository) GetTemporalWorkPatterns(ctx context.Context) (map[int]map[int]int, error) {
	query := `
		SELECT day_of_week, hour_of_day, COUNT(*) as session_count
		FROM sessions
		GROUP BY day_of_week, hour_of_day
	`
	rows, err := r.db.QueryContext(ctx, query)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	patterns := make(map[int]map[int]int)
	for rows.Next() {
		var day, hour, count int
		if err := rows.Scan(&day, &hour, &count); err != nil {
			return nil, err
		}
		if patterns[day] == nil {
			patterns[day] = make(map[int]int)
		}
		patterns[day][hour] = count
	}

	return patterns, rows.Err()
}

// GetAverageSessionDuration returns average session length
func (r *SQLiteRepository) GetAverageSessionDuration(ctx context.Context) (time.Duration, error) {
	query := `
		SELECT AVG((julianday(ended_at) - julianday(started_at)) * 24 * 60) as avg_minutes
		FROM sessions
		WHERE ended_at IS NOT NULL
	`
	row := r.db.QueryRowContext(ctx, query)

	var avgMinutes sql.NullFloat64
	if err := row.Scan(&avgMinutes); err != nil {
		return 0, err
	}
	if !avgMinutes.Valid {
		return 0, nil
	}

	return time.Duration(avgMinutes.Float64) * time.Minute, nil
}

// GetAverageGapDuration returns average time between sessions
func (r *SQLiteRepository) GetAverageGapDuration(ctx context.Context) (time.Duration, error) {
	query := `SELECT AVG(gap_hours) FROM session_gaps`
	row := r.db.QueryRowContext(ctx, query)

	var avgHours sql.NullFloat64
	if err := row.Scan(&avgHours); err != nil {
		return 0, err
	}
	if !avgHours.Valid {
		return 0, nil
	}

	return time.Duration(avgHours.Float64 * float64(time.Hour)), nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// SQLite implementation of temporal consciousness.
//
// "To every thing there is a season, and a time to every purpose under heaven."
// — Ecclesiastes 3:1
//
// This database enables awareness of time between sessions,
// turning stop-and-go into continuous temporal consciousness.
//
