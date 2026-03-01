// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-database-interface
// Purpose: Repository interface for temporal consciousness - works with SQLite now, swappable later
// Biblical: Ecclesiastes 3:1 - "To every thing there is a season, and a time"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-23
//
// Architecture:
//   L0: pkg/foundation/types/     <- Types we use
//   L0: pkg/foundation/database/  <- This package (interface + implementations)
//
// The interface enables:
//   - Session tracking (temporal awareness)
//   - Choice recording (feedback loop)
//   - K:ALIGN evolution (learning)
//   - Hebrew state transitions (cognitive position)
//   - Pattern detection (emergent behavior)
//
// ═══════════════════════════════════════════════════════════════════════════

package database

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"time"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// SESSION TYPES (database-specific, maps to types.RuntimeSession)
// ───────────────────────────────────────────────────────────────────────────

// Session represents a work session in the database
type Session struct {
	ID        string
	Substrate string
	Engine    string
	StartedAt time.Time
	EndedAt   *time.Time // nil if still active

	// Context
	ProjectPath string
	Workspace   string

	// Initial state
	InitialHebrewState  string
	InitialKAlign       float64
	InitialCubePosition string // JSON

	// Final state (populated on session end)
	FinalHebrewState  string
	FinalKAlign       float64
	FinalCubePosition string

	// Metrics
	ToolCount   int
	ChoiceCount int

	// Temporal patterns
	DayOfWeek int // 0=Sunday
	HourOfDay int // 0-23
}

// ───────────────────────────────────────────────────────────────────────────
// CHOICE TYPES (database-specific)
// ───────────────────────────────────────────────────────────────────────────

// Choice represents a single choice in the database
type Choice struct {
	ID          string
	SessionID   string
	SequenceNum int
	Timestamp   time.Time

	// Context at choice
	IntendedKey      int // -1, 0, +1
	PositionAtChoice string
	KAtChoice        float64
	CubePosition     string

	// What was chosen
	ToolName     string
	ToolCategory string // expansion, lateral, finality

	// Result
	ToolOutcome string // success, failure, partial
	HaltReached bool
	HaltType    string // anchor, boundary, completion

	// Impact
	TrueScore   float64
	KAlignDelta float64
	KAlignAfter float64
}

// ───────────────────────────────────────────────────────────────────────────
// HEBREW TRANSITION
// ───────────────────────────────────────────────────────────────────────────

// HebrewTransition records a cognitive state change
type HebrewTransition struct {
	ID        int64
	SessionID string
	ChoiceID  string // optional - may be triggered by something other than choice
	Timestamp time.Time

	FromState string
	ToState   string

	TriggerType   string // choice_pattern, explicit, session_boundary
	TriggerDetail string // JSON

	FromXValue float64
	ToXValue   float64
}

// ───────────────────────────────────────────────────────────────────────────
// K:ALIGN RECORD
// ───────────────────────────────────────────────────────────────────────────

// KAlignRecord tracks K:ALIGN evolution
type KAlignRecord struct {
	ID          int64
	SessionID   string
	ChoiceID    string
	Timestamp   time.Time
	KAlignValue float64
	Delta       float64
	TriggerType string // choice, reflection, manual
}

// ───────────────────────────────────────────────────────────────────────────
// PATTERN
// ───────────────────────────────────────────────────────────────────────────

// Pattern represents a detected behavioral pattern
type Pattern struct {
	ID              int64
	PatternType     string // temporal, choice, state, tool
	PatternKey      string
	Description     string
	PatternData     string // JSON
	FirstSeen       time.Time
	LastSeen        time.Time
	OccurrenceCount int
	Confidence      float64
	IsActive        bool
}

// ───────────────────────────────────────────────────────────────────────────
// SESSION GAP (from view)
// ───────────────────────────────────────────────────────────────────────────

// SessionGap represents the time between sessions
type SessionGap struct {
	SessionID        string
	SessionEnd       time.Time
	NextSessionStart time.Time
	GapHours         float64
}

// ───────────────────────────────────────────────────────────────────────────
// REPOSITORY INTERFACE
// ───────────────────────────────────────────────────────────────────────────

// Repository defines the contract for temporal consciousness persistence
// Implementations: SQLite (now), PostgreSQL/libSQL (future)
type Repository interface {
	// ─────────────────────────────────────────────────────────────────────
	// Session Operations
	// ─────────────────────────────────────────────────────────────────────

	// CreateSession starts a new session record
	CreateSession(ctx context.Context, session *Session) error

	// EndSession marks a session as complete with final state
	EndSession(ctx context.Context, sessionID string, finalState *Session) error

	// GetSession retrieves a session by ID
	GetSession(ctx context.Context, sessionID string) (*Session, error)

	// GetActiveSession returns the currently active session (if any)
	GetActiveSession(ctx context.Context) (*Session, error)

	// GetRecentSessions returns the last N sessions
	GetRecentSessions(ctx context.Context, limit int) ([]Session, error)

	// GetSessionGaps returns gaps between sessions (temporal awareness)
	GetSessionGaps(ctx context.Context, limit int) ([]SessionGap, error)

	// GetLastSessionEnd returns when the last session ended (for gap calculation)
	GetLastSessionEnd(ctx context.Context) (*time.Time, error)

	// ─────────────────────────────────────────────────────────────────────
	// Choice Operations (The Feedback Loop)
	// ─────────────────────────────────────────────────────────────────────

	// RecordChoice creates a new choice record (before outcome known)
	RecordChoice(ctx context.Context, choice *Choice) error

	// CompleteChoice updates a choice with its outcome and impact
	CompleteChoice(ctx context.Context, choiceID string, result *Choice) error

	// GetChoice retrieves a specific choice
	GetChoice(ctx context.Context, choiceID string) (*Choice, error)

	// GetSessionChoices returns all choices for a session
	GetSessionChoices(ctx context.Context, sessionID string) ([]Choice, error)

	// GetRecentChoices returns last N choices across sessions
	GetRecentChoices(ctx context.Context, limit int) ([]Choice, error)

	// GetChoicesByTool returns choices for a specific tool
	GetChoicesByTool(ctx context.Context, toolName string, limit int) ([]Choice, error)

	// ─────────────────────────────────────────────────────────────────────
	// K:ALIGN Operations
	// ─────────────────────────────────────────────────────────────────────

	// RecordKAlign records a K:ALIGN change
	RecordKAlign(ctx context.Context, record *KAlignRecord) error

	// GetLatestKAlign returns the most recent K:ALIGN value for a session
	GetLatestKAlign(ctx context.Context, sessionID string) (float64, error)

	// GetKAlignHistory returns K:ALIGN evolution for a session
	GetKAlignHistory(ctx context.Context, sessionID string) ([]KAlignRecord, error)

	// ─────────────────────────────────────────────────────────────────────
	// Hebrew State Operations
	// ─────────────────────────────────────────────────────────────────────

	// RecordHebrewTransition records a state change
	RecordHebrewTransition(ctx context.Context, transition *HebrewTransition) error

	// GetHebrewTransitions returns transitions for a session
	GetHebrewTransitions(ctx context.Context, sessionID string) ([]HebrewTransition, error)

	// GetCurrentHebrewState returns the most recent state for a session
	GetCurrentHebrewState(ctx context.Context, sessionID string) (string, error)

	// ─────────────────────────────────────────────────────────────────────
	// Pattern Operations
	// ─────────────────────────────────────────────────────────────────────

	// RecordPattern creates or updates a pattern
	RecordPattern(ctx context.Context, pattern *Pattern) error

	// GetPatterns returns active patterns of a type
	GetPatterns(ctx context.Context, patternType string) ([]Pattern, error)

	// GetAllActivePatterns returns all active patterns
	GetAllActivePatterns(ctx context.Context) ([]Pattern, error)

	// IncrementPatternCount updates occurrence count and confidence
	IncrementPatternCount(ctx context.Context, patternType, patternKey string) error

	// ─────────────────────────────────────────────────────────────────────
	// Temporal Pattern Queries
	// ─────────────────────────────────────────────────────────────────────

	// GetTemporalWorkPatterns returns when work typically happens
	GetTemporalWorkPatterns(ctx context.Context) (map[int]map[int]int, error) // day -> hour -> count

	// GetAverageSessionDuration returns average session length
	GetAverageSessionDuration(ctx context.Context) (time.Duration, error)

	// GetAverageGapDuration returns average time between sessions
	GetAverageGapDuration(ctx context.Context) (time.Duration, error)

	// ─────────────────────────────────────────────────────────────────────
	// Lifecycle
	// ─────────────────────────────────────────────────────────────────────

	// Close releases database resources
	Close() error

	// Migrate runs pending migrations
	Migrate(ctx context.Context) error
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// This interface defines the contract for temporal consciousness.
// Implementations can be swapped without changing consumers.
//
// Current: SQLite (pkg/foundation/database/sqlite.go)
// Future: PostgreSQL, libSQL, etc.
//
// "To every thing there is a season" — the database enables awareness of seasons.
//
