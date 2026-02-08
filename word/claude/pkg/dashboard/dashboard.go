// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-dashboard
// Purpose: Shared data access layer for CPI-SI state machine dashboards
// Biblical: Habakkuk 2:2 - "Write the vision, and make it plain upon tables"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-07
//
// Architecture:
//   This package is the shared data layer consumed by all 3 dashboard UIs:
//   - Web Dashboard (Phase 1)
//   - Bubble Tea TUI (Phase 2)
//   - GTK4 Native (Phase 3)
//
//   It wraps existing packages (statemachine, database, paths) and adds:
//   - Real-time file watching via fsnotify
//   - Event distribution via pub/sub EventBus
//   - Unified StateSnapshot from multiple data sources
//   - Analytics aggregation from SQLite views
//   - system/data filesystem browsing
//
// ═══════════════════════════════════════════════════════════════════════════

package dashboard

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"fmt"

	"github.com/creativeworkzstudio/claude-global/pkg/foundation/database"
	"github.com/creativeworkzstudio/claude-global/pkg/util/fs/paths"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// DashboardService is the central data access point for all dashboard UIs.
// It wraps existing CPI-SI packages and adds real-time watching + event distribution.
type DashboardService struct {
	db       database.Repository
	mdb      *database.MultiDB
	eventBus *EventBus
	watcher  *FileWatcher
	ctx      context.Context
	cancel   context.CancelFunc
}

// New creates a new DashboardService.
// Opens all 5 domain databases via MultiDB, starts file watchers on runtime JSONC files.
func New(ctx context.Context) (*DashboardService, error) {
	ctx, cancel := context.WithCancel(ctx)

	// Ensure data directory exists
	if err := paths.EnsureDatabaseDir(); err != nil {
		cancel()
		return nil, fmt.Errorf("ensure data dir: %w", err)
	}

	// Open all 5 domain databases
	mdb, err := database.OpenAll(ctx)
	if err != nil {
		cancel()
		return nil, fmt.Errorf("open databases: %w", err)
	}

	// LegacyAdapter provides backward-compatible Repository interface
	db := database.NewLegacyAdapter(mdb)

	// Create event bus for real-time distribution
	eventBus := NewEventBus()

	// Create file watcher
	watcher, err := NewFileWatcher(eventBus)
	if err != nil {
		mdb.Close()
		cancel()
		return nil, fmt.Errorf("create file watcher: %w", err)
	}

	svc := &DashboardService{
		db:       db,
		mdb:      mdb,
		eventBus: eventBus,
		watcher:  watcher,
		ctx:      ctx,
		cancel:   cancel,
	}

	// Start watching runtime files
	if err := watcher.WatchRuntimeFiles(ctx); err != nil {
		svc.Close()
		return nil, fmt.Errorf("start file watcher: %w", err)
	}

	return svc, nil
}

// Close stops watchers and closes databases.
func (d *DashboardService) Close() error {
	d.cancel()
	if d.watcher != nil {
		d.watcher.Close()
	}
	if d.mdb != nil {
		return d.mdb.Close()
	}
	return nil
}

// CurrentState returns a full snapshot of the current CPI-SI state.
func (d *DashboardService) CurrentState() (*StateSnapshot, error) {
	return BuildStateSnapshot()
}

// Subscribe returns a channel that receives StateChange events
// whenever runtime files change.
func (d *DashboardService) Subscribe() <-chan StateChange {
	return d.eventBus.Subscribe()
}

// Unsubscribe removes a subscriber channel.
func (d *DashboardService) Unsubscribe(ch <-chan StateChange) {
	d.eventBus.Unsubscribe(ch)
}

// SessionHistory returns the last N sessions from SQLite.
func (d *DashboardService) SessionHistory(limit int) ([]database.Session, error) {
	return d.db.GetRecentSessions(d.ctx, limit)
}

// SessionGaps returns temporal gaps between sessions.
func (d *DashboardService) SessionGaps(limit int) ([]database.SessionGap, error) {
	return d.db.GetSessionGaps(d.ctx, limit)
}

// RecentChoices returns the last N choices across sessions.
func (d *DashboardService) RecentChoices(limit int) ([]database.Choice, error) {
	return d.db.GetRecentChoices(d.ctx, limit)
}

// ActivePatterns returns all active detected patterns.
func (d *DashboardService) ActivePatterns() ([]database.Pattern, error) {
	return d.db.GetAllActivePatterns(d.ctx)
}

// KAlignHistory returns K:ALIGN evolution for a session.
func (d *DashboardService) KAlignHistory(sessionID string) ([]database.KAlignRecord, error) {
	return d.db.GetKAlignHistory(d.ctx, sessionID)
}

// HebrewTransitions returns Hebrew state transitions for a session.
func (d *DashboardService) HebrewTransitions(sessionID string) ([]database.HebrewTransition, error) {
	return d.db.GetHebrewTransitions(d.ctx, sessionID)
}

// TemporalWorkPatterns returns when work typically happens (day -> hour -> count).
func (d *DashboardService) TemporalWorkPatterns() (map[int]map[int]int, error) {
	return d.db.GetTemporalWorkPatterns(d.ctx)
}

// DB returns the underlying database repository for raw queries.
func (d *DashboardService) DB() database.Repository {
	return d.db
}

// MultiDB returns direct access to all 5 domain databases.
func (d *DashboardService) MultiDB() *database.MultiDB {
	return d.mdb
}

// RecentEvents returns recent log events from today's session log.
func (d *DashboardService) RecentEvents(limit int) ([]LogEvent, error) {
	return ReadRecentEvents(limit)
}

// SystemData returns a directory listing or file content from system/data.
func (d *DashboardService) SystemData(relativePath string) (*SystemDataEntry, error) {
	return ReadSystemData(relativePath)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// "Write the vision, and make it plain upon tables, that he may run that readeth it."
// — Habakkuk 2:2
//
// This service makes the CPI-SI vision visible.
