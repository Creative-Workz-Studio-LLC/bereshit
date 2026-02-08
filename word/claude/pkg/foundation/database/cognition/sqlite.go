// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cpisi-db-cognition-sqlite
// Purpose: SQLite implementation of cognition Repository interface
// Biblical: Romans 12:2 - "Be ye transformed by the renewing of your mind"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package cognition

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"database/sql"
	"embed"
	"fmt"

	_ "modernc.org/sqlite"
)

//go:embed schema/*.sql
var schemaFS embed.FS

// SQLiteRepository implements cognition.Repository using SQLite
type SQLiteRepository struct {
	db *sql.DB
}

// Compile-time interface check
var _ Repository = (*SQLiteRepository)(nil)

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Constructor & Lifecycle
// ═══════════════════════════════════════════════════════════════════════════

// NewSQLiteRepository creates a new cognition SQLite repository
func NewSQLiteRepository(dbPath string) (*SQLiteRepository, error) {
	db, err := sql.Open("sqlite", dbPath+"?_pragma=foreign_keys(1)&_pragma=journal_mode(WAL)")
	if err != nil {
		return nil, fmt.Errorf("open cognition db: %w", err)
	}
	if err := db.Ping(); err != nil {
		db.Close()
		return nil, fmt.Errorf("ping cognition db: %w", err)
	}
	return &SQLiteRepository{db: db}, nil
}

// Close releases database resources
func (r *SQLiteRepository) Close() error {
	return r.db.Close()
}

// Migrate runs the schema migrations in order
func (r *SQLiteRepository) Migrate(ctx context.Context) error {
	migrations := []string{
		"schema/001_initial.sql",
		"schema/002_rich_data.sql",
	}

	for _, file := range migrations {
		schema, err := schemaFS.ReadFile(file)
		if err != nil {
			return fmt.Errorf("read %s: %w", file, err)
		}
		_, err = r.db.ExecContext(ctx, string(schema))
		if err != nil {
			continue // Ignore duplicate column errors
		}
	}
	return nil
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
			tool_name, tool_category, health_score
		) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
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
		choice.HealthScore,
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
		return 0.0, nil
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
		var rec KAlignRecord
		var choiceID sql.NullString
		err := rows.Scan(&rec.ID, &rec.SessionID, &choiceID, &rec.Timestamp, &rec.KAlignValue, &rec.Delta, &rec.TriggerType)
		if err != nil {
			return nil, err
		}
		if choiceID.Valid {
			rec.ChoiceID = choiceID.String
		}
		records = append(records, rec)
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
		return "yashar", nil
	}
	return state, err
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Anchor & Trajectory Operations
// ═══════════════════════════════════════════════════════════════════════════

// RecordAnchorVisit records an anchor traversal
func (r *SQLiteRepository) RecordAnchorVisit(ctx context.Context, visit *AnchorVisit) error {
	query := `
		INSERT INTO anchor_visits (session_id, anchor_key, visit_timestamp, from_section, trigger)
		VALUES (?, ?, ?, ?, ?)
	`
	_, err := r.db.ExecContext(ctx, query,
		visit.SessionID,
		visit.AnchorKey,
		visit.VisitTimestamp,
		visit.FromSection,
		visit.Trigger,
	)
	return err
}

// RecordTrajectoryTransition records B.1→B.4 movement
func (r *SQLiteRepository) RecordTrajectoryTransition(ctx context.Context, transition *TrajectoryTransition) error {
	query := `
		INSERT INTO trajectory_history (session_id, from_section, to_section, transition_timestamp, trigger, duration_ms)
		VALUES (?, ?, ?, ?, ?, ?)
	`
	_, err := r.db.ExecContext(ctx, query,
		transition.SessionID,
		transition.FromSection,
		transition.ToSection,
		transition.TransitionTimestamp,
		transition.Trigger,
		transition.DurationMs,
	)
	return err
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Raw SQL Operations
// ═══════════════════════════════════════════════════════════════════════════

// Exec executes a raw SQL statement and returns rows affected
func (r *SQLiteRepository) Exec(ctx context.Context, query string, args ...interface{}) (int64, error) {
	result, err := r.db.ExecContext(ctx, query, args...)
	if err != nil {
		return 0, fmt.Errorf("exec: %w", err)
	}
	rowsAffected, err := result.RowsAffected()
	if err != nil {
		return 0, fmt.Errorf("rows affected: %w", err)
	}
	return rowsAffected, nil
}

// Query executes a raw SQL query and returns rows as maps
func (r *SQLiteRepository) Query(ctx context.Context, query string, args ...interface{}) ([]map[string]interface{}, error) {
	rows, err := r.db.QueryContext(ctx, query, args...)
	if err != nil {
		return nil, fmt.Errorf("query: %w", err)
	}
	defer rows.Close()

	columns, err := rows.Columns()
	if err != nil {
		return nil, fmt.Errorf("columns: %w", err)
	}

	var results []map[string]interface{}
	for rows.Next() {
		values := make([]interface{}, len(columns))
		valuePtrs := make([]interface{}, len(columns))
		for i := range values {
			valuePtrs[i] = &values[i]
		}
		if err := rows.Scan(valuePtrs...); err != nil {
			return nil, fmt.Errorf("scan: %w", err)
		}
		row := make(map[string]interface{})
		for i, col := range columns {
			row[col] = values[i]
		}
		results = append(results, row)
	}
	return results, rows.Err()
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
