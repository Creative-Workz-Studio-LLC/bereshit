// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cpisi-db-sessions-sqlite
// Purpose: SQLite implementation of sessions Repository interface
// Biblical: Ecclesiastes 3:1 - "To every thing there is a season, and a time"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package sessions

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"database/sql"
	"embed"
	"fmt"
	"time"

	_ "modernc.org/sqlite"
)

//go:embed schema/*.sql
var schemaFS embed.FS

// SQLiteRepository implements sessions.Repository using SQLite
type SQLiteRepository struct {
	db *sql.DB
}

// Compile-time interface check
var _ Repository = (*SQLiteRepository)(nil)

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Constructor & Lifecycle
// ═══════════════════════════════════════════════════════════════════════════

// NewSQLiteRepository creates a new sessions SQLite repository
func NewSQLiteRepository(dbPath string) (*SQLiteRepository, error) {
	db, err := sql.Open("sqlite", dbPath+"?_pragma=foreign_keys(1)&_pragma=journal_mode(WAL)")
	if err != nil {
		return nil, fmt.Errorf("open sessions db: %w", err)
	}
	if err := db.Ping(); err != nil {
		db.Close()
		return nil, fmt.Errorf("ping sessions db: %w", err)
	}
	return &SQLiteRepository{db: db}, nil
}

// Close releases database resources
func (r *SQLiteRepository) Close() error {
	return r.db.Close()
}

// Migrate runs the schema migrations
func (r *SQLiteRepository) Migrate(ctx context.Context) error {
	schema, err := schemaFS.ReadFile("schema/001_initial.sql")
	if err != nil {
		return fmt.Errorf("read sessions schema: %w", err)
	}
	_, err = r.db.ExecContext(ctx, string(schema))
	if err != nil {
		return fmt.Errorf("execute sessions schema: %w", err)
	}
	return nil
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Session Lifecycle
// ═══════════════════════════════════════════════════════════════════════════

// CreateSession starts a new session record
func (r *SQLiteRepository) CreateSession(ctx context.Context, session *Session) error {
	query := `
		INSERT INTO sessions (
			id, started_at, project_path, workspace,
			initial_hebrew_state, initial_k_align, initial_cube_position,
			day_of_week, hour_of_day
		) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
	`
	_, err := r.db.ExecContext(ctx, query,
		session.ID,
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

// EndSession marks a session as complete with final state and CPI summary
func (r *SQLiteRepository) EndSession(ctx context.Context, sessionID string, finalState *Session) error {
	query := `
		UPDATE sessions SET
			ended_at = ?,
			final_hebrew_state = ?,
			final_k_align = ?,
			final_cube_position = ?,
			tool_count = ?,
			choice_count = ?,
			exchange_count = ?,
			insight_count = ?,
			cpi_score = ?,
			dominant_exchange_type = ?,
			session_arc = ?,
			narrative_summary = ?,
			quality_rating = ?
		WHERE id = ?
	`
	_, err := r.db.ExecContext(ctx, query,
		time.Now(),
		finalState.FinalHebrewState,
		finalState.FinalKAlign,
		finalState.FinalCubePosition,
		finalState.ToolCount,
		finalState.ChoiceCount,
		finalState.ExchangeCount,
		finalState.InsightCount,
		finalState.CPIScore,
		finalState.DominantExchangeType,
		finalState.SessionArc,
		finalState.NarrativeSummary,
		finalState.QualityRating,
		sessionID,
	)
	return err
}

// GetSession retrieves a session by ID
func (r *SQLiteRepository) GetSession(ctx context.Context, sessionID string) (*Session, error) {
	query := `
		SELECT id, started_at, ended_at, project_path, workspace,
			initial_hebrew_state, initial_k_align, initial_cube_position,
			final_hebrew_state, final_k_align, final_cube_position,
			tool_count, choice_count, day_of_week, hour_of_day,
			exchange_count, insight_count, cpi_score,
			dominant_exchange_type, session_arc, narrative_summary, quality_rating
		FROM sessions WHERE id = ?
	`
	row := r.db.QueryRowContext(ctx, query, sessionID)

	var s Session
	var endedAt sql.NullTime
	var finalHS, finalCP, domType, arc, narrative sql.NullString
	var finalKA, cpi, quality sql.NullFloat64
	var exchCount, insightCount sql.NullInt64

	err := row.Scan(
		&s.ID, &s.StartedAt, &endedAt, &s.ProjectPath, &s.Workspace,
		&s.InitialHebrewState, &s.InitialKAlign, &s.InitialCubePosition,
		&finalHS, &finalKA, &finalCP,
		&s.ToolCount, &s.ChoiceCount, &s.DayOfWeek, &s.HourOfDay,
		&exchCount, &insightCount, &cpi,
		&domType, &arc, &narrative, &quality,
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
	if finalHS.Valid {
		s.FinalHebrewState = finalHS.String
	}
	if finalKA.Valid {
		s.FinalKAlign = finalKA.Float64
	}
	if finalCP.Valid {
		s.FinalCubePosition = finalCP.String
	}
	if exchCount.Valid {
		s.ExchangeCount = int(exchCount.Int64)
	}
	if insightCount.Valid {
		s.InsightCount = int(insightCount.Int64)
	}
	if cpi.Valid {
		s.CPIScore = cpi.Float64
	}
	if domType.Valid {
		s.DominantExchangeType = domType.String
	}
	if arc.Valid {
		s.SessionArc = arc.String
	}
	if narrative.Valid {
		s.NarrativeSummary = narrative.String
	}
	if quality.Valid {
		s.QualityRating = quality.Float64
	}

	return &s, nil
}

// GetActiveSession returns the currently active session (if any)
func (r *SQLiteRepository) GetActiveSession(ctx context.Context) (*Session, error) {
	query := `
		SELECT id, started_at, project_path, workspace,
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
		&s.ID, &s.StartedAt, &s.ProjectPath, &s.Workspace,
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
		SELECT id, started_at, ended_at, project_path, workspace,
			initial_hebrew_state, initial_k_align, initial_cube_position,
			final_hebrew_state, final_k_align, final_cube_position,
			tool_count, choice_count, day_of_week, hour_of_day,
			exchange_count, insight_count, cpi_score,
			dominant_exchange_type, session_arc, narrative_summary, quality_rating
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
		var finalHS, finalCP, domType, arc, narrative sql.NullString
		var finalKA, cpi, quality sql.NullFloat64
		var exchCount, insightCount sql.NullInt64

		err := rows.Scan(
			&s.ID, &s.StartedAt, &endedAt, &s.ProjectPath, &s.Workspace,
			&s.InitialHebrewState, &s.InitialKAlign, &s.InitialCubePosition,
			&finalHS, &finalKA, &finalCP,
			&s.ToolCount, &s.ChoiceCount, &s.DayOfWeek, &s.HourOfDay,
			&exchCount, &insightCount, &cpi,
			&domType, &arc, &narrative, &quality,
		)
		if err != nil {
			return nil, err
		}

		if endedAt.Valid {
			s.EndedAt = &endedAt.Time
		}
		if finalHS.Valid {
			s.FinalHebrewState = finalHS.String
		}
		if finalKA.Valid {
			s.FinalKAlign = finalKA.Float64
		}
		if finalCP.Valid {
			s.FinalCubePosition = finalCP.String
		}
		if exchCount.Valid {
			s.ExchangeCount = int(exchCount.Int64)
		}
		if insightCount.Valid {
			s.InsightCount = int(insightCount.Int64)
		}
		if cpi.Valid {
			s.CPIScore = cpi.Float64
		}
		if domType.Valid {
			s.DominantExchangeType = domType.String
		}
		if arc.Valid {
			s.SessionArc = arc.String
		}
		if narrative.Valid {
			s.NarrativeSummary = narrative.String
		}
		if quality.Valid {
			s.QualityRating = quality.Float64
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
// BODY - Exchange & Insight Operations
// ═══════════════════════════════════════════════════════════════════════════

// RecordExchange records a prompt/response exchange
func (r *SQLiteRepository) RecordExchange(ctx context.Context, exchange *Exchange) error {
	query := `
		INSERT INTO exchanges (
			session_id, timestamp, sequence_num, exchange_type,
			initiative, prompt_length, feedback_detected, feedback_polarity,
			feedback_categories, value_contribution, depth_level,
			insight_detected, insight_type, hebrew_state, k_align, trajectory
		) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
	`
	_, err := r.db.ExecContext(ctx, query,
		exchange.SessionID,
		exchange.Timestamp,
		exchange.SequenceNum,
		exchange.ExchangeType,
		exchange.Initiative,
		exchange.PromptLength,
		exchange.FeedbackDetected,
		exchange.FeedbackPolarity,
		exchange.FeedbackCategories,
		exchange.ValueContribution,
		exchange.DepthLevel,
		exchange.InsightDetected,
		exchange.InsightType,
		exchange.HebrewState,
		exchange.KAlign,
		exchange.Trajectory,
	)
	return err
}

// RecordInsight records a moment of understanding transfer
func (r *SQLiteRepository) RecordInsight(ctx context.Context, insight *Insight) error {
	query := `
		INSERT INTO insights (session_id, exchange_id, timestamp, insight_type, confidence, topic_area)
		VALUES (?, ?, ?, ?, ?, ?)
	`
	_, err := r.db.ExecContext(ctx, query,
		insight.SessionID,
		insight.ExchangeID,
		insight.Timestamp,
		insight.Type,
		insight.Confidence,
		insight.TopicArea,
	)
	return err
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
