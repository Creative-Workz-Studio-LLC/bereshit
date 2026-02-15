// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cpisi-db-sessions-interface
// Purpose: Repository interface for session & exchange lifecycle domain
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
	"time"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Types
// ═══════════════════════════════════════════════════════════════════════════

// Session represents a work session
type Session struct {
	ID        string
	StartedAt time.Time
	EndedAt   *time.Time

	ProjectPath string
	Workspace   string

	InitialHebrewState  string
	InitialKAlign       float64
	InitialCubePosition string
	FinalHebrewState    string
	FinalKAlign         float64
	FinalCubePosition   string

	ToolCount   int
	ChoiceCount int
	DayOfWeek   int
	HourOfDay   int

	ExchangeCount        int
	InsightCount         int
	CPIScore             float64
	DominantExchangeType string
	SessionArc           string
	NarrativeSummary     string
	QualityRating        float64
}

// Exchange represents a single prompt/response exchange
type Exchange struct {
	ID                 int64
	SessionID          string
	Timestamp          time.Time
	SequenceNum        int
	ExchangeType       string
	Initiative         string
	PromptLength       int
	FeedbackDetected   bool
	FeedbackPolarity   string
	FeedbackCategories string
	ValueContribution  float64
	DepthLevel         string
	InsightDetected    bool
	InsightType        string
	HebrewState        string
	KAlign             float64
	Trajectory         string

	// Rich data (v2)
	HealthScore   *int
	ContextTokens *int

	// Message text (v3)
	UserMessageText string
	ResponseSummary string
	Valence         string

	// Cognition (v4) — the reasoning behind the response
	ThinkingText string
}

// Insight represents a moment of understanding transfer
type Insight struct {
	ID         int64
	SessionID  string
	ExchangeID *int64
	Timestamp  time.Time
	Type       string
	Confidence float64
	TopicArea  string
}

// SessionGap represents time between sessions
type SessionGap struct {
	SessionID        string
	SessionEnd       time.Time
	NextSessionStart time.Time
	GapHours         float64
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Repository Interface
// ═══════════════════════════════════════════════════════════════════════════

// Repository defines the contract for session & exchange persistence
type Repository interface {
	// Session lifecycle
	CreateSession(ctx context.Context, session *Session) error
	EndSession(ctx context.Context, sessionID string, finalState *Session) error
	GetSession(ctx context.Context, sessionID string) (*Session, error)
	GetActiveSession(ctx context.Context) (*Session, error)
	GetRecentSessions(ctx context.Context, limit int) ([]Session, error)
	GetSessionGaps(ctx context.Context, limit int) ([]SessionGap, error)
	GetLastSessionEnd(ctx context.Context) (*time.Time, error)

	// Exchange operations
	RecordExchange(ctx context.Context, exchange *Exchange) error
	RecordInsight(ctx context.Context, insight *Insight) error
	GetRecentExchanges(ctx context.Context, sessionID string, limit int) ([]Exchange, error)
	UpdateExchangeResponse(ctx context.Context, sessionID string, responseText string) error
	UpdateExchangeThinking(ctx context.Context, sessionID string, thinkingText string) error

	// Temporal queries
	GetTemporalWorkPatterns(ctx context.Context) (map[int]map[int]int, error)
	GetAverageSessionDuration(ctx context.Context) (time.Duration, error)
	GetAverageGapDuration(ctx context.Context) (time.Duration, error)

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
