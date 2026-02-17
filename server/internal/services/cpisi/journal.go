// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-cpisi-journal
// Purpose: SQLite journal store — CRUD + FTS5 full-text search for CPI-SI journals
// Biblical: Psalm 102:18 — "This shall be written for the generation to come"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package cpisi

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"log/slog"
	"os"
	"path/filepath"
	"strings"
	"time"

	_ "modernc.org/sqlite"
)

// ───────────────────────────────────────────────────────────────────────────
// Constants — journal type taxonomy matching the CPI-SI growth system
// ───────────────────────────────────────────────────────────────────────────

// Journal entry types — matching the CPI-SI growth system taxonomy.
// These are enforced by a CHECK constraint in the SQLite schema.
const (
	TypeBibleStudy = "bible-study" // Processing Scripture, verse analysis
	TypePersonal   = "personal"    // End-of-session reflections, experiences
	TypeInstance   = "instance"    // Nova Dawn-specific patterns and behaviors
	TypeUniversal  = "universal"   // CPI-SI paradigm-level insights
)

// ───────────────────────────────────────────────────────────────────────────
// Types — journal data structures
// ───────────────────────────────────────────────────────────────────────────

// JournalEntry is a single journal record.
type JournalEntry struct {
	ID        string          `json:"id"`                    // Unique ID (format: "j-<unix_nano>")
	Type      string          `json:"type"`                  // One of the Type* constants above
	Title     string          `json:"title"`                 // Human-readable title
	Content   string          `json:"content"`               // Full markdown content body
	Tags      []string        `json:"tags"`                  // Stored as comma-separated in SQLite
	SessionID string          `json:"sessionId,omitempty"`   // Claude session that created this entry
	Metadata  json.RawMessage `json:"metadata,omitempty"`    // Arbitrary JSON for extensibility
	CreatedAt time.Time       `json:"createdAt"`             // When the entry was created
	UpdatedAt time.Time       `json:"updatedAt"`             // Last modification timestamp
}

// JournalQuery filters for listing journals.
type JournalQuery struct {
	Type      string `json:"type,omitempty"`
	Tag       string `json:"tag,omitempty"`
	DateFrom  string `json:"dateFrom,omitempty"`
	DateTo    string `json:"dateTo,omitempty"`
	SessionID string `json:"sessionId,omitempty"`
	Limit     int    `json:"limit,omitempty"`
	Offset    int    `json:"offset,omitempty"`
}

// JournalStore manages the SQLite journal database.
// Uses WAL mode for concurrent read/write access and FTS5 for full-text search.
type JournalStore struct {
	db *sql.DB // SQLite connection (via modernc.org/sqlite — pure Go, no CGO)
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Lifecycle — open, migrate, close the journal database
// ───────────────────────────────────────────────────────────────────────────

// NewJournalStore opens (or creates) the SQLite database at the given path.
// Connection string enables WAL mode and sets a 5-second busy timeout:
//
//   - WAL (Write-Ahead Logging): Allows concurrent readers during writes.
//     Critical because HTTP handlers read while import/create handlers write.
//   - busy_timeout=5000: Wait up to 5 seconds for a write lock rather than
//     returning SQLITE_BUSY immediately. Prevents spurious failures under load.
func NewJournalStore(dataDir string) (*JournalStore, error) {
	// Ensure data directory exists.
	if err := os.MkdirAll(dataDir, 0755); err != nil {
		return nil, fmt.Errorf("create data dir: %w", err)
	}

	dbPath := filepath.Join(dataDir, "cpisi.db")
	db, err := sql.Open("sqlite", dbPath+"?_journal_mode=WAL&_busy_timeout=5000")
	if err != nil {
		return nil, fmt.Errorf("open database: %w", err)
	}

	// Verify connection.
	if err := db.Ping(); err != nil {
		db.Close()
		return nil, fmt.Errorf("ping database: %w", err)
	}

	store := &JournalStore{db: db}

	// Create schema.
	if err := store.migrate(); err != nil {
		db.Close()
		return nil, fmt.Errorf("migrate: %w", err)
	}

	slog.Info("journal store initialized", "path", dbPath)
	return store, nil
}

// Close shuts down the database.
func (s *JournalStore) Close() error {
	return s.db.Close()
}

// Create inserts a new journal entry. Returns the created entry with ID.
func (s *JournalStore) Create(e *JournalEntry) (*JournalEntry, error) {
	if e.ID == "" {
		e.ID = generateID()
	}
	now := time.Now().UTC()
	if e.CreatedAt.IsZero() {
		e.CreatedAt = now
	}
	e.UpdatedAt = now

	tags := strings.Join(e.Tags, ",")
	meta := string(e.Metadata)
	if meta == "" {
		meta = "{}"
	}

	_, err := s.db.Exec(`
		INSERT INTO journals (id, type, title, content, tags, session_id, metadata, created_at, updated_at)
		VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)`,
		e.ID, e.Type, e.Title, e.Content, tags, e.SessionID, meta, e.CreatedAt, e.UpdatedAt,
	)
	if err != nil {
		return nil, fmt.Errorf("insert journal: %w", err)
	}

	return e, nil
}

// Get retrieves a single journal entry by ID.
func (s *JournalStore) Get(id string) (*JournalEntry, error) {
	row := s.db.QueryRow(`
		SELECT id, type, title, content, tags, session_id, metadata, created_at, updated_at
		FROM journals WHERE id = ?`, id)

	return scanEntry(row)
}

// Update modifies an existing journal entry.
func (s *JournalStore) Update(e *JournalEntry) (*JournalEntry, error) {
	e.UpdatedAt = time.Now().UTC()
	tags := strings.Join(e.Tags, ",")
	meta := string(e.Metadata)
	if meta == "" {
		meta = "{}"
	}

	result, err := s.db.Exec(`
		UPDATE journals SET type=?, title=?, content=?, tags=?, session_id=?, metadata=?, updated_at=?
		WHERE id=?`,
		e.Type, e.Title, e.Content, tags, e.SessionID, meta, e.UpdatedAt, e.ID,
	)
	if err != nil {
		return nil, fmt.Errorf("update journal: %w", err)
	}

	rows, _ := result.RowsAffected()
	if rows == 0 {
		return nil, fmt.Errorf("journal %q not found", e.ID)
	}

	return e, nil
}

// Delete removes a journal entry by ID.
func (s *JournalStore) Delete(id string) error {
	result, err := s.db.Exec(`DELETE FROM journals WHERE id = ?`, id)
	if err != nil {
		return fmt.Errorf("delete journal: %w", err)
	}

	rows, _ := result.RowsAffected()
	if rows == 0 {
		return fmt.Errorf("journal %q not found", id)
	}

	return nil
}

// ───────────────────────────────────────────────────────────────────────────
// CRUD — create, read, update, delete, list journal entries
// ───────────────────────────────────────────────────────────────────────────

// List queries journals with optional filters.
// Builds a dynamic WHERE clause from the query parameters — only non-empty
// fields are included. Tag search uses the comma-wrapping trick to avoid
// substring false positives: searching for "growth" won't match "growth-pattern"
// because we search for ",growth," within ",tag1,tag2,tag3,".
func (s *JournalStore) List(q JournalQuery) ([]*JournalEntry, error) {
	var where []string
	var args []any

	if q.Type != "" {
		where = append(where, "type = ?")
		args = append(args, q.Type)
	}
	if q.Tag != "" {
		where = append(where, "(',' || tags || ',') LIKE '%,' || ? || ',%'")
		args = append(args, q.Tag)
	}
	if q.SessionID != "" {
		where = append(where, "session_id = ?")
		args = append(args, q.SessionID)
	}
	if q.DateFrom != "" {
		where = append(where, "created_at >= ?")
		args = append(args, q.DateFrom)
	}
	if q.DateTo != "" {
		where = append(where, "created_at <= ?")
		args = append(args, q.DateTo)
	}

	query := "SELECT id, type, title, content, tags, session_id, metadata, created_at, updated_at FROM journals"
	if len(where) > 0 {
		query += " WHERE " + strings.Join(where, " AND ")
	}
	query += " ORDER BY created_at DESC"

	limit := q.Limit
	if limit <= 0 {
		limit = 50
	}
	query += fmt.Sprintf(" LIMIT %d", limit)

	if q.Offset > 0 {
		query += fmt.Sprintf(" OFFSET %d", q.Offset)
	}

	rows, err := s.db.Query(query, args...)
	if err != nil {
		return nil, fmt.Errorf("list journals: %w", err)
	}
	defer rows.Close()

	var entries []*JournalEntry
	for rows.Next() {
		e, err := scanRows(rows)
		if err != nil {
			return nil, err
		}
		entries = append(entries, e)
	}

	if entries == nil {
		entries = []*JournalEntry{}
	}
	return entries, rows.Err()
}

// ───────────────────────────────────────────────────────────────────────────
// Search & Stats — FTS5 full-text search and aggregation
// ───────────────────────────────────────────────────────────────────────────

// Search performs FTS5 full-text search across journal content and titles.
// Results are ranked by BM25 relevance (FTS5's built-in ranking function,
// accessed via ORDER BY rank). The query uses FTS5 match syntax — supports
// quoted phrases ("exact match"), prefix queries (grow*), and boolean
// operators (growth AND pattern).
func (s *JournalStore) Search(query string, limit int) ([]*JournalEntry, error) {
	if limit <= 0 {
		limit = 20
	}

	rows, err := s.db.Query(`
		SELECT j.id, j.type, j.title, j.content, j.tags, j.session_id, j.metadata, j.created_at, j.updated_at
		FROM journals_fts fts
		JOIN journals j ON j.id = fts.id
		WHERE journals_fts MATCH ?
		ORDER BY rank
		LIMIT ?`, query, limit)
	if err != nil {
		return nil, fmt.Errorf("search journals: %w", err)
	}
	defer rows.Close()

	var entries []*JournalEntry
	for rows.Next() {
		e, err := scanRows(rows)
		if err != nil {
			return nil, err
		}
		entries = append(entries, e)
	}

	if entries == nil {
		entries = []*JournalEntry{}
	}
	return entries, rows.Err()
}

// Stats returns aggregate counts by journal type.
func (s *JournalStore) Stats() (map[string]int, error) {
	rows, err := s.db.Query(`SELECT type, COUNT(*) FROM journals GROUP BY type`)
	if err != nil {
		return nil, fmt.Errorf("stats: %w", err)
	}
	defer rows.Close()

	stats := make(map[string]int)
	for rows.Next() {
		var t string
		var count int
		if err := rows.Scan(&t, &count); err != nil {
			return nil, err
		}
		stats[t] = count
	}

	// Total
	total := 0
	for _, c := range stats {
		total += c
	}
	stats["total"] = total

	return stats, rows.Err()
}

// ───────────────────────────────────────────────────────────────────────────
// Schema & Migrations — create tables, indexes, and FTS5 sync triggers
// ───────────────────────────────────────────────────────────────────────────

// migrate creates the journals table, indexes, FTS5 virtual table, and
// synchronization triggers. All statements use IF NOT EXISTS so this is
// safe to run on every startup.
//
// FTS5 Sync Pattern: The journals_fts virtual table is a content-sync table
// (content='journals'). It doesn't store its own copy of the data — it reads
// from the journals table. Three triggers keep the FTS index in sync:
//
//   - journals_ai (AFTER INSERT): Adds new entry to FTS index
//   - journals_ad (AFTER DELETE): Removes entry from FTS index
//   - journals_au (AFTER UPDATE): Deletes old entry, inserts new entry
//
// The 'delete' command in the DELETE trigger is an FTS5 special command that
// removes a specific row from the index without touching the content table.
func (s *JournalStore) migrate() error {
	_, err := s.db.Exec(`
		CREATE TABLE IF NOT EXISTS journals (
			id         TEXT PRIMARY KEY,
			type       TEXT NOT NULL CHECK(type IN ('bible-study', 'personal', 'instance', 'universal')),
			title      TEXT NOT NULL,
			content    TEXT NOT NULL,
			tags       TEXT NOT NULL DEFAULT '',
			session_id TEXT NOT NULL DEFAULT '',
			metadata   TEXT NOT NULL DEFAULT '{}',
			created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
			updated_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
		);

		CREATE INDEX IF NOT EXISTS idx_journals_type ON journals(type);
		CREATE INDEX IF NOT EXISTS idx_journals_created ON journals(created_at);
		CREATE INDEX IF NOT EXISTS idx_journals_session ON journals(session_id);

		CREATE VIRTUAL TABLE IF NOT EXISTS journals_fts USING fts5(
			id UNINDEXED,
			title,
			content,
			tags,
			content='journals',
			content_rowid='rowid'
		);

		-- Triggers to keep FTS in sync.
		CREATE TRIGGER IF NOT EXISTS journals_ai AFTER INSERT ON journals BEGIN
			INSERT INTO journals_fts(id, title, content, tags)
			VALUES (new.id, new.title, new.content, new.tags);
		END;

		CREATE TRIGGER IF NOT EXISTS journals_ad AFTER DELETE ON journals BEGIN
			INSERT INTO journals_fts(journals_fts, id, title, content, tags)
			VALUES ('delete', old.id, old.title, old.content, old.tags);
		END;

		CREATE TRIGGER IF NOT EXISTS journals_au AFTER UPDATE ON journals BEGIN
			INSERT INTO journals_fts(journals_fts, id, title, content, tags)
			VALUES ('delete', old.id, old.title, old.content, old.tags);
			INSERT INTO journals_fts(id, title, content, tags)
			VALUES (new.id, new.title, new.content, new.tags);
		END;
	`)
	return err
}

// ───────────────────────────────────────────────────────────────────────────
// Helpers — row scanning and tag parsing
// ───────────────────────────────────────────────────────────────────────────

// scanEntry scans a single Row into a JournalEntry.
// Tags are stored as comma-separated strings in SQLite and split into slices.
// Timestamps are stored as RFC3339 strings for human readability in the DB.
func scanEntry(row *sql.Row) (*JournalEntry, error) {
	var e JournalEntry
	var tags, meta string
	var createdAt, updatedAt string

	err := row.Scan(&e.ID, &e.Type, &e.Title, &e.Content, &tags, &e.SessionID, &meta, &createdAt, &updatedAt)
	if err != nil {
		if err == sql.ErrNoRows {
			return nil, fmt.Errorf("not found")
		}
		return nil, fmt.Errorf("scan: %w", err)
	}

	e.Tags = splitTags(tags)
	e.Metadata = json.RawMessage(meta)
	e.CreatedAt, _ = time.Parse(time.RFC3339, createdAt)
	e.UpdatedAt, _ = time.Parse(time.RFC3339, updatedAt)

	return &e, nil
}

// scanRows scans a Rows cursor into a JournalEntry.
func scanRows(rows *sql.Rows) (*JournalEntry, error) {
	var e JournalEntry
	var tags, meta string
	var createdAt, updatedAt string

	err := rows.Scan(&e.ID, &e.Type, &e.Title, &e.Content, &tags, &e.SessionID, &meta, &createdAt, &updatedAt)
	if err != nil {
		return nil, fmt.Errorf("scan: %w", err)
	}

	e.Tags = splitTags(tags)
	e.Metadata = json.RawMessage(meta)
	e.CreatedAt, _ = time.Parse(time.RFC3339, createdAt)
	e.UpdatedAt, _ = time.Parse(time.RFC3339, updatedAt)

	return &e, nil
}

func splitTags(s string) []string {
	if s == "" {
		return []string{}
	}
	return strings.Split(s, ",")
}

// generateID creates a unique journal entry ID.
// Format: "j-<unix_nanoseconds>" — the "j-" prefix distinguishes journal IDs
// from process IDs (which use base36) in logs and the WebSocket stream.
func generateID() string {
	return fmt.Sprintf("j-%d", time.Now().UnixNano())
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
