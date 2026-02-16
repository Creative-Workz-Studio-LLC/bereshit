// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cpisi-db-growth-sqlite
// Purpose: SQLite implementation of growth Repository interface
// Biblical: Isaiah 28:10 - "Line upon line, precept upon precept"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package growth

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"database/sql"
	"embed"
	"fmt"

	l2db "creativeworkzstudio.com/bereshit/L2-platform/hybrid/database"
)

//go:embed schema/*.sql
var schemaFS embed.FS

// SQLiteRepository implements growth.Repository using SQLite
type SQLiteRepository struct {
	db *sql.DB
}

// Compile-time interface check
var _ Repository = (*SQLiteRepository)(nil)

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Constructor & Lifecycle
// ═══════════════════════════════════════════════════════════════════════════

// NewSQLiteRepository creates a new growth SQLite repository
func NewSQLiteRepository(dbPath string) (*SQLiteRepository, error) {
	db, err := l2db.OpenDefault(dbPath)
	if err != nil {
		return nil, fmt.Errorf("open growth db: %w", err)
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
		return fmt.Errorf("read growth schema: %w", err)
	}
	_, err = r.db.ExecContext(ctx, string(schema))
	if err != nil {
		return fmt.Errorf("execute growth schema: %w", err)
	}
	return nil
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Pattern Operations
// ═══════════════════════════════════════════════════════════════════════════

// RecordPattern creates or updates a detected behavioral pattern
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
// BODY - Learning Operations
// ═══════════════════════════════════════════════════════════════════════════

// RecordLearning creates or updates a learning entry
func (r *SQLiteRepository) RecordLearning(ctx context.Context, learning *Learning) error {
	query := `
		INSERT INTO learnings (
			source_patterns, source_type, learning_type, learning_key,
			learning_content, confidence, weight, integrated,
			integrated_at, integration_target,
			first_seen, last_reinforced, reinforcement_count
		) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
		ON CONFLICT(learning_type, learning_key) DO UPDATE SET
			learning_content = excluded.learning_content,
			confidence = excluded.confidence,
			last_reinforced = CURRENT_TIMESTAMP,
			reinforcement_count = reinforcement_count + 1,
			updated_at = CURRENT_TIMESTAMP
	`
	_, err := r.db.ExecContext(ctx, query,
		learning.SourcePatterns,
		learning.SourceType,
		learning.LearningType,
		learning.LearningKey,
		learning.LearningContent,
		learning.Confidence,
		learning.Weight,
		learning.Integrated,
		learning.IntegratedAt,
		learning.IntegrationTarget,
		learning.FirstSeen,
		learning.LastReinforced,
		learning.ReinforcementCount,
	)
	return err
}

// GetLearnings returns learnings of a type
func (r *SQLiteRepository) GetLearnings(ctx context.Context, learningType string) ([]Learning, error) {
	query := `
		SELECT id, source_patterns, source_type, learning_type, learning_key,
			learning_content, confidence, weight, integrated,
			integrated_at, integration_target,
			first_seen, last_reinforced, reinforcement_count
		FROM learnings
		WHERE learning_type = ?
		ORDER BY confidence DESC
	`
	return r.queryLearnings(ctx, query, learningType)
}

// GetIntegrationCandidates returns high-confidence unintegrated learnings
func (r *SQLiteRepository) GetIntegrationCandidates(ctx context.Context) ([]Learning, error) {
	query := `
		SELECT id, source_patterns, source_type, learning_type, learning_key,
			learning_content, confidence, weight, integrated,
			integrated_at, integration_target,
			first_seen, last_reinforced, reinforcement_count
		FROM learnings
		WHERE integrated = 0 AND confidence >= 0.7
		ORDER BY confidence DESC, reinforcement_count DESC
	`
	return r.queryLearnings(ctx, query)
}

// queryLearnings is a helper for learning queries
func (r *SQLiteRepository) queryLearnings(ctx context.Context, query string, args ...interface{}) ([]Learning, error) {
	rows, err := r.db.QueryContext(ctx, query, args...)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var learnings []Learning
	for rows.Next() {
		var l Learning
		var sourcePatterns, integrationTarget sql.NullString
		var integratedAt sql.NullTime

		err := rows.Scan(
			&l.ID, &sourcePatterns, &l.SourceType, &l.LearningType, &l.LearningKey,
			&l.LearningContent, &l.Confidence, &l.Weight, &l.Integrated,
			&integratedAt, &integrationTarget,
			&l.FirstSeen, &l.LastReinforced, &l.ReinforcementCount,
		)
		if err != nil {
			return nil, err
		}
		if sourcePatterns.Valid {
			l.SourcePatterns = sourcePatterns.String
		}
		if integratedAt.Valid {
			l.IntegratedAt = &integratedAt.Time
		}
		if integrationTarget.Valid {
			l.IntegrationTarget = integrationTarget.String
		}
		learnings = append(learnings, l)
	}
	return learnings, rows.Err()
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Memory Pattern Operations
// ═══════════════════════════════════════════════════════════════════════════

// RecordMemoryPattern stores an instance memory pattern from JSONC files
func (r *SQLiteRepository) RecordMemoryPattern(ctx context.Context, mp *MemoryPattern) error {
	query := `
		INSERT INTO memory_patterns (
			pattern_id, instance_id, pattern_text, category,
			first_observed, last_updated, confidence, evidence,
			evolution, serves_me, challenges_me, related_patterns, extensions
		) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
		ON CONFLICT(pattern_id) DO UPDATE SET
			pattern_text = excluded.pattern_text,
			last_updated = excluded.last_updated,
			confidence = excluded.confidence,
			evidence = excluded.evidence,
			evolution = excluded.evolution
	`
	_, err := r.db.ExecContext(ctx, query,
		mp.PatternID, mp.InstanceID, mp.PatternText, mp.Category,
		mp.FirstObserved, mp.LastUpdated, mp.Confidence, mp.Evidence,
		mp.Evolution, mp.ServesMe, mp.ChallengesMe, mp.RelatedPatterns, mp.Extensions,
	)
	return err
}

// GetMemoryPatterns returns all instance memory patterns
func (r *SQLiteRepository) GetMemoryPatterns(ctx context.Context) ([]MemoryPattern, error) {
	query := `
		SELECT id, pattern_id, instance_id, pattern_text, category,
			first_observed, last_updated, confidence, evidence,
			evolution, serves_me, challenges_me, related_patterns, extensions
		FROM memory_patterns
		ORDER BY last_updated DESC
	`
	rows, err := r.db.QueryContext(ctx, query)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var patterns []MemoryPattern
	for rows.Next() {
		var mp MemoryPattern
		err := rows.Scan(
			&mp.ID, &mp.PatternID, &mp.InstanceID, &mp.PatternText, &mp.Category,
			&mp.FirstObserved, &mp.LastUpdated, &mp.Confidence, &mp.Evidence,
			&mp.Evolution, &mp.ServesMe, &mp.ChallengesMe, &mp.RelatedPatterns, &mp.Extensions,
		)
		if err != nil {
			return nil, err
		}
		patterns = append(patterns, mp)
	}
	return patterns, rows.Err()
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Journal Operations
// ═══════════════════════════════════════════════════════════════════════════

// RecordJournalEntry tracks journal metadata
func (r *SQLiteRepository) RecordJournalEntry(ctx context.Context, entry *JournalEntry) error {
	query := `
		INSERT INTO journal_entries (journal_type, file_path, title, date, tags)
		VALUES (?, ?, ?, ?, ?)
	`
	_, err := r.db.ExecContext(ctx, query,
		entry.JournalType, entry.FilePath, entry.Title, entry.Date, entry.Tags,
	)
	return err
}

// GetJournalEntries returns journal entries of a type
func (r *SQLiteRepository) GetJournalEntries(ctx context.Context, journalType string, limit int) ([]JournalEntry, error) {
	query := `
		SELECT id, journal_type, file_path, title, date, tags
		FROM journal_entries
		WHERE journal_type = ?
		ORDER BY date DESC
		LIMIT ?
	`
	rows, err := r.db.QueryContext(ctx, query, journalType, limit)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var entries []JournalEntry
	for rows.Next() {
		var e JournalEntry
		var title, tags sql.NullString
		err := rows.Scan(&e.ID, &e.JournalType, &e.FilePath, &title, &e.Date, &tags)
		if err != nil {
			return nil, err
		}
		if title.Valid {
			e.Title = title.String
		}
		if tags.Valid {
			e.Tags = tags.String
		}
		entries = append(entries, e)
	}
	return entries, rows.Err()
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
