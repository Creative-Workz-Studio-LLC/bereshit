//omni:code --go -library
//omni:key B-L3-database-growth-interface
//omni:version a-01.00
// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cpisi-db-growth-interface
// Purpose: Repository interface for learning & pattern recognition domain
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
	"time"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Types
// ═══════════════════════════════════════════════════════════════════════════

// Pattern represents a detected behavioral pattern
type Pattern struct {
	ID              int64
	PatternType     string
	PatternKey      string
	Description     string
	PatternData     string
	FirstSeen       time.Time
	LastSeen        time.Time
	OccurrenceCount int
	Confidence      float64
	IsActive        bool
}

// Learning represents a pattern promoted to knowledge
type Learning struct {
	ID                 int64
	SourcePatterns     string
	SourceType         string
	LearningType       string
	LearningKey        string
	LearningContent    string
	Confidence         float64
	Weight             float64
	Integrated         bool
	IntegratedAt       *time.Time
	IntegrationTarget  string
	FirstSeen          time.Time
	LastReinforced     time.Time
	ReinforcementCount int
}

// MemoryPattern represents an instance memory pattern (from JSONC files)
type MemoryPattern struct {
	ID              int64
	PatternID       string
	InstanceID      string
	PatternText     string
	Category        string
	FirstObserved   string
	LastUpdated     string
	Confidence      string
	Evidence        string
	Evolution       string
	ServesMe        string
	ChallengesMe    string
	RelatedPatterns string
	Extensions      string
}

// JournalEntry tracks journal metadata
type JournalEntry struct {
	ID          int64
	JournalType string
	FilePath    string
	Title       string
	Date        string
	Tags        string
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Repository Interface
// ═══════════════════════════════════════════════════════════════════════════

// Repository defines the contract for growth & learning persistence
type Repository interface {
	// Pattern operations
	RecordPattern(ctx context.Context, pattern *Pattern) error
	GetPatterns(ctx context.Context, patternType string) ([]Pattern, error)
	GetAllActivePatterns(ctx context.Context) ([]Pattern, error)
	IncrementPatternCount(ctx context.Context, patternType, patternKey string) error

	// Learning operations
	RecordLearning(ctx context.Context, learning *Learning) error
	GetLearnings(ctx context.Context, learningType string) ([]Learning, error)
	GetIntegrationCandidates(ctx context.Context) ([]Learning, error)

	// Memory pattern operations
	RecordMemoryPattern(ctx context.Context, mp *MemoryPattern) error
	GetMemoryPatterns(ctx context.Context) ([]MemoryPattern, error)

	// Journal operations
	RecordJournalEntry(ctx context.Context, entry *JournalEntry) error
	GetJournalEntries(ctx context.Context, journalType string, limit int) ([]JournalEntry, error)

	// Raw SQL
	Exec(ctx context.Context, query string, args ...interface{}) (int64, error)
	Query(ctx context.Context, query string, args ...interface{}) ([]map[string]interface{}, error)

	// Lifecycle
	Close() error
	Migrate(ctx context.Context) error
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
