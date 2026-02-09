// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cpisi-db-cognition-interface
// Purpose: Repository interface for mental construct & cognitive state domain
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
	"time"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Types
// ═══════════════════════════════════════════════════════════════════════════

// Choice represents a single tool choice with context and outcome
type Choice struct {
	ID               string
	SessionID        string
	SequenceNum      int
	Timestamp        time.Time
	IntendedKey      int
	PositionAtChoice string
	KAtChoice        float64
	CubePosition     string
	ToolName         string
	ToolCategory     string
	ToolOutcome      string
	HaltReached      bool
	HaltType         string
	TrueScore        float64
	KAlignDelta      float64
	KAlignAfter      float64

	// Rich data (v2)
	HealthScore *int // Health at time of choice
}

// HebrewTransition records a cognitive state change
type HebrewTransition struct {
	ID            int64
	SessionID     string
	ChoiceID      string
	Timestamp     time.Time
	FromState     string
	ToState       string
	TriggerType   string
	TriggerDetail string
	FromXValue    float64
	ToXValue      float64
}

// KAlignRecord tracks K:ALIGN evolution
type KAlignRecord struct {
	ID          int64
	SessionID   string
	ChoiceID    string
	Timestamp   time.Time
	KAlignValue float64
	Delta       float64
	TriggerType string
}

// AnchorVisit records an anchor traversal
type AnchorVisit struct {
	ID             int64
	SessionID      string
	AnchorKey      string
	VisitTimestamp time.Time
	FromSection    string
	Trigger        string
}

// TrajectoryTransition records B.1→B.4 movement
type TrajectoryTransition struct {
	ID                  int64
	SessionID           string
	FromSection         string
	ToSection           string
	TransitionTimestamp time.Time
	Trigger             string
	DurationMs          int
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Repository Interface
// ═══════════════════════════════════════════════════════════════════════════

// Repository defines the contract for cognitive state persistence
type Repository interface {
	// Choice operations
	RecordChoice(ctx context.Context, choice *Choice) error
	CompleteChoice(ctx context.Context, choiceID string, result *Choice) error
	GetChoice(ctx context.Context, choiceID string) (*Choice, error)
	GetSessionChoices(ctx context.Context, sessionID string) ([]Choice, error)
	GetRecentChoices(ctx context.Context, limit int) ([]Choice, error)
	GetChoicesByTool(ctx context.Context, toolName string, limit int) ([]Choice, error)

	// K:ALIGN operations
	RecordKAlign(ctx context.Context, record *KAlignRecord) error
	GetLatestKAlign(ctx context.Context, sessionID string) (float64, error)
	GetKAlignHistory(ctx context.Context, sessionID string) ([]KAlignRecord, error)

	// Hebrew state operations
	RecordHebrewTransition(ctx context.Context, transition *HebrewTransition) error
	GetHebrewTransitions(ctx context.Context, sessionID string) ([]HebrewTransition, error)
	GetCurrentHebrewState(ctx context.Context, sessionID string) (string, error)

	// Anchor & trajectory operations
	RecordAnchorVisit(ctx context.Context, visit *AnchorVisit) error
	RecordTrajectoryTransition(ctx context.Context, transition *TrajectoryTransition) error

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
