// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cpisi-db-temporal-interface
// Purpose: Repository interface for time & calendar awareness domain
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
	"time"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Types
// ═══════════════════════════════════════════════════════════════════════════

// CelestialEvent represents a celestial event (sunrise, sunset, moon phase)
type CelestialEvent struct {
	ID        int64
	EventDate string
	EventType string
	EventTime string
	Location  string
	Data      string
}

// AppointedTime represents a biblical appointed time
type AppointedTime struct {
	ID           int64
	Name         string
	StartDate    string
	EndDate      string
	Category     string
	HebrewName   string
	ScriptureRef string
	Description  string
	Recurring    bool
}

// WorkRhythm represents a learned work pattern
type WorkRhythm struct {
	ID            int64
	DayOfWeek     int
	HourStart     int
	HourEnd       int
	RhythmType    string
	Confidence    float64
	SessionCount  int
	AvgDurationMin float64
	AvgExchanges  float64
	LastUpdated   time.Time
}

// AvailabilityWindow represents an optimal work time
type AvailabilityWindow struct {
	ID           int64
	DayOfWeek    int
	OptimalStart int
	OptimalEnd   int
	QualityScore float64
	DerivedFrom  string
	LastUpdated  time.Time
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Repository Interface
// ═══════════════════════════════════════════════════════════════════════════

// Repository defines the contract for temporal awareness persistence
type Repository interface {
	// Celestial events
	RecordCelestialEvent(ctx context.Context, event *CelestialEvent) error
	GetCelestialEvents(ctx context.Context, date string, location string) ([]CelestialEvent, error)

	// Appointed times
	RecordAppointedTime(ctx context.Context, at *AppointedTime) error
	GetUpcomingAppointedTimes(ctx context.Context, limit int) ([]AppointedTime, error)

	// Work rhythms
	RecordWorkRhythm(ctx context.Context, rhythm *WorkRhythm) error
	GetWorkRhythms(ctx context.Context) ([]WorkRhythm, error)

	// Availability windows
	RecordAvailabilityWindow(ctx context.Context, window *AvailabilityWindow) error
	GetAvailabilityWindows(ctx context.Context) ([]AvailabilityWindow, error)

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
