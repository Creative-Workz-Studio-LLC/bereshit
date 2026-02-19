//omni:code --go -library
//omni:key B-L0-foundation-session
//omni:version a-02.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-foundation-session
// Purpose: Universal session management — ID, paths, logs, duration
// Biblical: Ecclesiastes 3:1 - "To every thing there is a season"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2026-02-16
// Updated: 2026-02-18
//
// Promoted from L3-cpisi to L0-universal as shared infrastructure.
// Any layer can track session state — this is not CPI-SI specific.
//
// TYPES ONLY — no filesystem I/O. Consumers handle persistence.
// This matches the Rust foundation crate design: types, constructors,
// queries, Display. Callers (hooks, CLI) do the reading/writing.
//
// ============================================================================

package session

// ──────────────────────────────────────────────────────────────────────────
// Imports (in METADATA — Go requires all imports before any declarations)
// ──────────────────────────────────────────────────────────────────────────

import (
	"fmt"
	"path/filepath"
	"time"

	"creativeworkzstudio.com/bereshit/L0-universal/b-spiral/identity"
)

// Pragma — OmniCode file identity (I1-I4). Runtime-queryable via PragmaGet().
var Pragma = [][2]string{
	// I1: Core — key, format, from, at
	{"I1.key", "B-L0-foundation-session"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.go"},
	{"I1.at", "a-02.00"},
	// I2: Family — type, structure, subtype, role
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "foundation"},
	// I3: Instance — file, title, component, path, provides, brief
	{"I3.file", "session.go"},
	{"I3.title", "Session Management Library"},
	{"I3.component", "session"},
	{"I3.path", "L0-universal/c-hybrid/core/foundation/go/session/session.go"},
	{"I3.provides", "Universal session management — ID, paths, logs, duration"},
	{"I3.brief", "Session types, log tracking, duration formatting"},
	// I4: Architecture — layer, position, pattern
	{"I4.layer", "L0-universal"},
	{"I4.position", "c-hybrid/core/foundation"},
	{"I4.pattern", "value-types"},
}

// Metadata — OmniCode file metadata (C1-C7). Runtime-queryable via MetadataGet().
var Metadata = [][2]string{
	// C1: State — version, status, created, updated
	{"C1.version", "a-02.00"},
	{"C1.status", "Active"},
	{"C1.created", "2026-02-16"},
	{"C1.updated", "2026-02-18"},
	// C2: Attribution
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.architect", "Seanje Lenox-Wise"},
	{"C2.implementation", "Nova Dawn"},
	{"C2.copyright", "2025-2026 CreativeWorkzStudio LLC"},
	// C3: Grounding
	{"C3.scripture", "Ecclesiastes 3:1 — To every thing there is a season"},
	{"C3.principle", "Every session has a season — awareness grounds sustainable rhythm"},
	{"C3.anchor", "Genesis 1:1 — In the beginning God created the heaven and the earth"},
	// C4: Dependencies
	{"C4.requires.stdlib", "fmt, path/filepath, time"},
	{"C4.requires.external", "none"},
	{"C4.requires.internal", "L0-universal/b-spiral/identity"},
	{"C4.consumers", "hooks, CLI tools, session-aware packages"},
	{"C4.integration", "import creativeworkzstudio.com/bereshit/L0-universal/c-hybrid/core/foundation/go/session"},
	{"C4.if_missing", "callers must define their own session types"},
	// C5: Intent
	{"C5.purpose", "Standardized session tracking across all layers"},
	{"C5.philosophy", "Types only — no filesystem I/O. Consumers handle persistence."},
	// C6: Roadmap
	{"C6.current", "a-02.00 — Session types, log tracking, duration formatting"},
	{"C6.planned", "Session lifecycle events, pattern detection"},
	{"C6.limitations", "No I/O — callers must read/write session files"},
	// C7: Classification
	{"C7.tags", "session, log, duration, paths, types"},
	{"C7.category", "Foundation"},
	{"C7.domain", "types"},
	{"C7.paradigm", "CPI-SI"},
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// --- Core Types ---

// Current represents the current session metadata.
type Current struct {
	SessionID   string    `json:"session_id"`
	StartTime   time.Time `json:"start_time"`
	Compactions int       `json:"compactions,omitempty"`
}

// LogEntry represents a single session log entry.
type LogEntry struct {
	Timestamp time.Time `json:"timestamp"`
	Type      string    `json:"type"`    // "task", "note", "tool", "milestone"
	Content   string    `json:"content"`
}

// Log represents the session log — ordered entries for a session.
type Log struct {
	SessionID string     `json:"session_id"`
	Entries   []LogEntry `json:"entries"`
}

// Paths holds session-related filesystem paths.
//
// Consumers create Paths via PathsFromDir() with an absolute directory.
// No home directory resolution — callers provide the full path.
type Paths struct {
	SessionDir     string `json:"session_dir"`
	CurrentSession string `json:"current_session"`
	SessionLog     string `json:"session_log"`
	PatternFile    string `json:"pattern_file"`
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Org Chart
// ──────────────────────────────────────────────────────────────────────────
//
// Identity:
//   init, PragmaGet, MetadataGet
//
// Constructors:
//   NewCurrent, NewLogEntry, NewLog, PathsFromDir
//
// Queries (Current):
//   Elapsed, ElapsedMinutes, HasCompacted
//
// Queries (Log):
//   EntryCount, IsEmpty, CountByType
//
// Mutators (Current):
//   RecordCompaction
//
// Mutators (Log):
//   AddEntry
//
// Functions:
//   FormatDuration, FormatElapsed

// --- Identity Access ---

// init registers this package in the OmniCode identity registry.
func init() {
	key, ok := PragmaGet("I1.key")
	if !ok {
		return // identity not registerable without key
	}
	identity.Register(key, Pragma, Metadata)
}

// PragmaGet looks up a value from the Pragma identity.
// Returns the value and true if found, empty string and false otherwise.
func PragmaGet(key string) (string, bool) {
	for _, pair := range Pragma {
		if pair[0] == key {
			return pair[1], true
		}
	}
	return "", false
}

// MetadataGet looks up a value from the Metadata.
// Returns the value and true if found, empty string and false otherwise.
func MetadataGet(key string) (string, bool) {
	for _, pair := range Metadata {
		if pair[0] == key {
			return pair[1], true
		}
	}
	return "", false
}

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: No internal helpers — pure constructors and accessors]

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────

// --- Constructors ---

// NewCurrent creates a new session with the given ID and start time.
func NewCurrent(sessionID string, startTime time.Time) Current {
	return Current{
		SessionID: sessionID,
		StartTime: startTime,
	}
}

// NewLogEntry creates a new log entry with the given details.
func NewLogEntry(timestamp time.Time, entryType, content string) LogEntry {
	return LogEntry{
		Timestamp: timestamp,
		Type:      entryType,
		Content:   content,
	}
}

// NewLog creates a new empty session log.
func NewLog(sessionID string) Log {
	return Log{
		SessionID: sessionID,
		Entries:   []LogEntry{},
	}
}

// PathsFromDir creates session paths from an absolute directory.
//
// The caller provides the full directory path — no home directory resolution.
// This keeps the types package substrate-agnostic.
func PathsFromDir(dir string) Paths {
	return Paths{
		SessionDir:     dir,
		CurrentSession: filepath.Join(dir, "current.json"),
		SessionLog:     filepath.Join(dir, "current-log.json"),
		PatternFile:    filepath.Join(dir, "patterns.json"),
	}
}

// --- Current Queries ---

// Elapsed returns duration since session start.
func (c *Current) Elapsed() time.Duration {
	return time.Since(c.StartTime)
}

// ElapsedMinutes returns minutes since session start.
func (c *Current) ElapsedMinutes() int {
	return int(c.Elapsed().Minutes())
}

// HasCompacted returns true if the session has been compacted at least once.
func (c *Current) HasCompacted() bool {
	return c.Compactions > 0
}

// --- Current Mutators ---

// RecordCompaction increments the compaction counter.
func (c *Current) RecordCompaction() {
	c.Compactions++
}

// --- Log Queries ---

// EntryCount returns the number of entries in the log.
func (l *Log) EntryCount() int {
	return len(l.Entries)
}

// IsEmpty returns true if the log has no entries.
func (l *Log) IsEmpty() bool {
	return len(l.Entries) == 0
}

// CountByType returns the number of entries matching the given type.
func (l *Log) CountByType(entryType string) int {
	count := 0
	for _, e := range l.Entries {
		if e.Type == entryType {
			count++
		}
	}
	return count
}

// --- Log Mutators ---

// AddEntry appends a log entry.
func (l *Log) AddEntry(entry LogEntry) {
	l.Entries = append(l.Entries, entry)
}

// --- Duration Formatting ---

// FormatDuration returns a human-readable duration string.
//
// Scales naturally:
//   - <60s:    "30s"
//   - 1-59m:   "5m"
//   - 1h+:     "2h 15m"
func FormatDuration(d time.Duration) string {
	totalSeconds := int(d.Seconds())

	if totalSeconds < 60 {
		return fmt.Sprintf("%ds", totalSeconds)
	}

	hours := totalSeconds / 3600
	minutes := (totalSeconds % 3600) / 60

	if hours > 0 {
		return fmt.Sprintf("%dh %dm", hours, minutes)
	}
	return fmt.Sprintf("%dm", minutes)
}

// FormatElapsed returns a formatted elapsed time since the given start.
func FormatElapsed(start time.Time) string {
	return FormatDuration(time.Since(start))
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Error Handling
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: Types package — no I/O, no errors. Constructors always succeed.]

// ──────────────────────────────────────────────────────────────────────────
// 5. Public APIs
// ──────────────────────────────────────────────────────────────────────────

// --- String Methods ---

// String returns a human-readable representation of the session.
//
//	"[abc-123] started 2026-02-18T15:30:00Z (compactions: 2)"
func (c Current) String() string {
	return fmt.Sprintf("[%s] started %s (compactions: %d)",
		c.SessionID,
		c.StartTime.Format(time.RFC3339),
		c.Compactions,
	)
}

// String returns a human-readable representation of the log entry.
//
//	"[2026-02-18T15:30:00Z] task: implemented feature X"
func (e LogEntry) String() string {
	return fmt.Sprintf("[%s] %s: %s",
		e.Timestamp.Format(time.RFC3339),
		e.Type,
		e.Content,
	)
}

// String returns a human-readable summary of the session log.
//
//	"Session abc-123 (5 entries)"
func (l Log) String() string {
	return fmt.Sprintf("Session %s (%d entries)", l.SessionID, len(l.Entries))
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────
//
// Testing: go test -v ./L0-universal/.../session/
//
// Verify:
//   - NewCurrent sets SessionID and StartTime, Compactions=0
//   - RecordCompaction increments counter
//   - HasCompacted false when new, true after RecordCompaction
//   - FormatDuration: 30s->"30s", 300s->"5m", 3900s->"1h 5m"
//   - FormatDuration(0) -> "0s" (edge case)
//   - AddEntry grows log, CountByType filters correctly
//   - JSON roundtrip preserves all fields
//
// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library — imported, not executed.
// import "creativeworkzstudio.com/bereshit/L0-universal/c-hybrid/core/foundation/go/session"
//
// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// No resources to clean up. Pure value types.
//
// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// Never:
//   - Add filesystem I/O (os.ReadFile, os.WriteFile) — types only
//   - Resolve home directory — callers provide absolute paths
//   - Add substrate-specific logic (Claude paths, etc.)
//
// Careful:
//   - Changing JSON field names — breaks session persistence
//   - Modifying FormatDuration thresholds — affects display
//
// Safe:
//   - Adding new query methods
//   - Adding new log entry types
//   - Extending Paths with new file paths
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// Note: Session management types promoted from L3 to L0. All filesystem I/O
// removed — consumers handle persistence. Matches Rust foundation design.
//
// Scripture: Ecclesiastes 3:1 — "To every thing there is a season, and a
// time to every purpose under the heaven."
//
// ============================================================================
// END CLOSING
// ============================================================================
