// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-statemachine-runtime-db
// Purpose: Database-aware runtime state initialization with temporal continuity
// Biblical: Ecclesiastes 3:1 - "To every thing there is a season, and a time"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-23
//
// Design: Provides database-aware initialization that:
//   - Checks last session end time for temporal gap calculation
//   - Continues K:ALIGN from previous session if gap is small
//   - Resets to defaults if gap is large or database unavailable
//
// ═══════════════════════════════════════════════════════════════════════════

package statemachine

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"time"

	"cws.studio/pkg/foundation/database"
)

// ───────────────────────────────────────────────────────────────────────────
// TYPES
// ───────────────────────────────────────────────────────────────────────────

// InitializationContext contains context from database-aware initialization
type InitializationContext struct {
	HasTemporalContext    bool          // True if we have previous session data
	GapSinceLastSession   time.Duration // Time since last session ended
	LastKAlign            float64       // K:ALIGN from last session
	LastHebrewState       string        // Hebrew state from last session
	ContinuedFromPrevious bool          // True if we're continuing previous state
}

// DatabaseBridge wraps the database repository for state machine use
type DatabaseBridge interface {
	// StartSession records a new session start
	StartSession(ctx context.Context, sessionID string, state *RuntimeState) error
	// EndSession records session end
	EndSession(ctx context.Context, sessionID string, state *RuntimeState) error
	// GetLastSessionEnd returns when the last session ended
	GetLastSessionEnd(ctx context.Context) (*time.Time, error)
	// RecordChoice records a choice in the database
	RecordChoice(ctx context.Context, record interface{}, state *RuntimeState) error
	// CompleteChoice updates a choice with its outcome
	CompleteChoice(ctx context.Context, record interface{}, state *RuntimeState) error
	// GetRepository returns the underlying repository for pattern detection
	GetRepository() database.Repository
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// DATABASE-AWARE INITIALIZATION
// ───────────────────────────────────────────────────────────────────────────

// InitializeWithDatabase creates runtime state with temporal continuity awareness
// If database is available, it checks the gap since last session and potentially
// continues K:ALIGN and Hebrew state from the previous session.
func InitializeWithDatabase(ctx context.Context, sessionID, substrate, engine string, bridge DatabaseBridge) (*RuntimeState, *InitializationContext, error) {
	// Start with basic initialization
	state := InitializeRuntimeState(sessionID, substrate, engine)
	initCtx := &InitializationContext{}

	// If no bridge, return basic state
	if bridge == nil {
		return state, initCtx, nil
	}

	// Try to get last session end time for gap calculation
	lastEnd, err := bridge.GetLastSessionEnd(ctx)
	if err != nil || lastEnd == nil {
		// No previous session data
		return state, initCtx, nil
	}

	// Calculate gap
	gap := time.Since(*lastEnd)
	initCtx.HasTemporalContext = true
	initCtx.GapSinceLastSession = gap

	// If gap is small (< 4 hours), continue from previous state
	// This provides temporal continuity across sessions
	if gap < 4*time.Hour {
		initCtx.ContinuedFromPrevious = true
		// The state was already initialized with defaults
		// Patterns will be applied separately to modify it
	}

	return state, initCtx, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// This file provides database-aware initialization for temporal continuity.
// The actual learning happens through pattern detection (patterns.go).
//
// Related: patterns.go, runtime.go, interface.go
//
