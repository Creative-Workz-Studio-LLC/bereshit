// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cpisi-db-temporal-sqlite
// Purpose: SQLite implementation of temporal Repository interface
// Biblical: Genesis 1:14 - "Let there be lights...for signs, and for seasons"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package temporal

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

// SQLiteRepository implements temporal.Repository using SQLite
type SQLiteRepository struct {
	db *sql.DB
}

// Compile-time interface check
var _ Repository = (*SQLiteRepository)(nil)

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Constructor & Lifecycle
// ═══════════════════════════════════════════════════════════════════════════

// NewSQLiteRepository creates a new temporal SQLite repository
func NewSQLiteRepository(dbPath string) (*SQLiteRepository, error) {
	db, err := sql.Open("sqlite", dbPath+"?_pragma=foreign_keys(1)&_pragma=journal_mode(WAL)")
	if err != nil {
		return nil, fmt.Errorf("open temporal db: %w", err)
	}
	if err := db.Ping(); err != nil {
		db.Close()
		return nil, fmt.Errorf("ping temporal db: %w", err)
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
		return fmt.Errorf("read temporal schema: %w", err)
	}
	_, err = r.db.ExecContext(ctx, string(schema))
	if err != nil {
		return fmt.Errorf("execute temporal schema: %w", err)
	}
	return nil
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Celestial Events
// ═══════════════════════════════════════════════════════════════════════════

// RecordCelestialEvent stores a celestial event (sunrise, sunset, moon phase)
func (r *SQLiteRepository) RecordCelestialEvent(ctx context.Context, event *CelestialEvent) error {
	query := `
		INSERT INTO celestial_events (event_date, event_type, event_time, location, data)
		VALUES (?, ?, ?, ?, ?)
		ON CONFLICT(event_date, event_type, location) DO UPDATE SET
			event_time = excluded.event_time,
			data = excluded.data
	`
	_, err := r.db.ExecContext(ctx, query,
		event.EventDate, event.EventType, event.EventTime, event.Location, event.Data,
	)
	return err
}

// GetCelestialEvents returns events for a date and location
func (r *SQLiteRepository) GetCelestialEvents(ctx context.Context, date string, location string) ([]CelestialEvent, error) {
	query := `
		SELECT id, event_date, event_type, event_time, location, data
		FROM celestial_events
		WHERE event_date = ? AND location = ?
		ORDER BY event_time ASC
	`
	rows, err := r.db.QueryContext(ctx, query, date, location)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var events []CelestialEvent
	for rows.Next() {
		var e CelestialEvent
		var eventTime, data sql.NullString
		err := rows.Scan(&e.ID, &e.EventDate, &e.EventType, &eventTime, &e.Location, &data)
		if err != nil {
			return nil, err
		}
		if eventTime.Valid {
			e.EventTime = eventTime.String
		}
		if data.Valid {
			e.Data = data.String
		}
		events = append(events, e)
	}
	return events, rows.Err()
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Appointed Times
// ═══════════════════════════════════════════════════════════════════════════

// RecordAppointedTime stores a biblical appointed time
func (r *SQLiteRepository) RecordAppointedTime(ctx context.Context, at *AppointedTime) error {
	query := `
		INSERT INTO appointed_times (
			name, start_date, end_date, category,
			hebrew_name, scripture_ref, description, recurring
		) VALUES (?, ?, ?, ?, ?, ?, ?, ?)
	`
	_, err := r.db.ExecContext(ctx, query,
		at.Name, at.StartDate, at.EndDate, at.Category,
		at.HebrewName, at.ScriptureRef, at.Description, at.Recurring,
	)
	return err
}

// GetUpcomingAppointedTimes returns future appointed times
func (r *SQLiteRepository) GetUpcomingAppointedTimes(ctx context.Context, limit int) ([]AppointedTime, error) {
	query := `
		SELECT id, name, start_date, end_date, category,
			hebrew_name, scripture_ref, description, recurring
		FROM appointed_times
		WHERE start_date >= date('now')
		ORDER BY start_date ASC
		LIMIT ?
	`
	rows, err := r.db.QueryContext(ctx, query, limit)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var times []AppointedTime
	for rows.Next() {
		var at AppointedTime
		var endDate, hebrewName, scriptureRef, description sql.NullString
		err := rows.Scan(
			&at.ID, &at.Name, &at.StartDate, &endDate, &at.Category,
			&hebrewName, &scriptureRef, &description, &at.Recurring,
		)
		if err != nil {
			return nil, err
		}
		if endDate.Valid {
			at.EndDate = endDate.String
		}
		if hebrewName.Valid {
			at.HebrewName = hebrewName.String
		}
		if scriptureRef.Valid {
			at.ScriptureRef = scriptureRef.String
		}
		if description.Valid {
			at.Description = description.String
		}
		times = append(times, at)
	}
	return times, rows.Err()
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Work Rhythms
// ═══════════════════════════════════════════════════════════════════════════

// RecordWorkRhythm stores a learned work pattern
func (r *SQLiteRepository) RecordWorkRhythm(ctx context.Context, rhythm *WorkRhythm) error {
	query := `
		INSERT INTO work_rhythms (
			day_of_week, hour_start, hour_end, rhythm_type,
			confidence, session_count, avg_duration_min, avg_exchanges
		) VALUES (?, ?, ?, ?, ?, ?, ?, ?)
		ON CONFLICT(day_of_week, hour_start, rhythm_type) DO UPDATE SET
			hour_end = excluded.hour_end,
			confidence = excluded.confidence,
			session_count = excluded.session_count,
			avg_duration_min = excluded.avg_duration_min,
			avg_exchanges = excluded.avg_exchanges,
			last_updated = CURRENT_TIMESTAMP
	`
	_, err := r.db.ExecContext(ctx, query,
		rhythm.DayOfWeek, rhythm.HourStart, rhythm.HourEnd, rhythm.RhythmType,
		rhythm.Confidence, rhythm.SessionCount, rhythm.AvgDurationMin, rhythm.AvgExchanges,
	)
	return err
}

// GetWorkRhythms returns all learned work patterns
func (r *SQLiteRepository) GetWorkRhythms(ctx context.Context) ([]WorkRhythm, error) {
	query := `
		SELECT id, day_of_week, hour_start, hour_end, rhythm_type,
			confidence, session_count, avg_duration_min, avg_exchanges, last_updated
		FROM work_rhythms
		ORDER BY confidence DESC
	`
	rows, err := r.db.QueryContext(ctx, query)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var rhythms []WorkRhythm
	for rows.Next() {
		var wr WorkRhythm
		var avgDuration, avgExchanges sql.NullFloat64
		err := rows.Scan(
			&wr.ID, &wr.DayOfWeek, &wr.HourStart, &wr.HourEnd, &wr.RhythmType,
			&wr.Confidence, &wr.SessionCount, &avgDuration, &avgExchanges, &wr.LastUpdated,
		)
		if err != nil {
			return nil, err
		}
		if avgDuration.Valid {
			wr.AvgDurationMin = avgDuration.Float64
		}
		if avgExchanges.Valid {
			wr.AvgExchanges = avgExchanges.Float64
		}
		rhythms = append(rhythms, wr)
	}
	return rhythms, rows.Err()
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Availability Windows
// ═══════════════════════════════════════════════════════════════════════════

// RecordAvailabilityWindow stores an optimal work time window
func (r *SQLiteRepository) RecordAvailabilityWindow(ctx context.Context, window *AvailabilityWindow) error {
	query := `
		INSERT INTO availability_windows (
			day_of_week, optimal_start, optimal_end,
			quality_score, derived_from
		) VALUES (?, ?, ?, ?, ?)
	`
	_, err := r.db.ExecContext(ctx, query,
		window.DayOfWeek, window.OptimalStart, window.OptimalEnd,
		window.QualityScore, window.DerivedFrom,
	)
	return err
}

// GetAvailabilityWindows returns all availability windows
func (r *SQLiteRepository) GetAvailabilityWindows(ctx context.Context) ([]AvailabilityWindow, error) {
	query := `
		SELECT id, day_of_week, optimal_start, optimal_end,
			quality_score, derived_from, last_updated
		FROM availability_windows
		ORDER BY day_of_week, optimal_start
	`
	rows, err := r.db.QueryContext(ctx, query)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var windows []AvailabilityWindow
	for rows.Next() {
		var w AvailabilityWindow
		var derivedFrom sql.NullString
		err := rows.Scan(
			&w.ID, &w.DayOfWeek, &w.OptimalStart, &w.OptimalEnd,
			&w.QualityScore, &derivedFrom, &w.LastUpdated,
		)
		if err != nil {
			return nil, err
		}
		if derivedFrom.Valid {
			w.DerivedFrom = derivedFrom.String
		}
		windows = append(windows, w)
	}
	return windows, rows.Err()
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
