//omni:code --go -library
//omni:key B-L3-database-multidb
//omni:version a-01.00
// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cpisi-db-multidb
// Purpose: MultiDB coordinator - opens, migrates, and manages 5 domain databases
// Biblical: Nehemiah 3 - Each builder worked on the section nearest their house
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// Architecture:
//   5 domain databases, each serving a specific concern:
//   - sessions.db  : Session & exchange lifecycle
//   - cognition.db : Mental construct, choices, Hebrew states, K:ALIGN
//   - growth.db    : Learning, pattern recognition, identity integration
//   - temporal.db  : Time awareness, calendars, work rhythms
//   - projects.db  : Work tracking, milestones, project-session linking
//
// ═══════════════════════════════════════════════════════════════════════════

package database

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"fmt"
	"os"
	"path/filepath"

	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/database/cognition"
	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/database/growth"
	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/database/projects"
	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/database/sessions"
	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/database/temporal"
	"creativeworkzstudio.com/bereshit/L0-universal/hybrid/paths"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY - MultiDB
// ═══════════════════════════════════════════════════════════════════════════

// MultiDB coordinates all 5 domain databases
// Each domain handles its own concern — no cross-domain foreign keys.
// session_id (TEXT) is the correlation key across domains.
type MultiDB struct {
	Sessions  sessions.Repository
	Cognition cognition.Repository
	Growth    growth.Repository
	Temporal  temporal.Repository
	Projects  projects.Repository
}

// OpenAll opens and migrates all 5 domain databases
func OpenAll(ctx context.Context) (*MultiDB, error) {
	basePath := paths.DatabaseDir()

	// Ensure data directory exists
	if err := os.MkdirAll(basePath, 0755); err != nil {
		return nil, fmt.Errorf("create data dir %s: %w", basePath, err)
	}

	mdb := &MultiDB{}
	var err error

	// Open each domain database
	mdb.Sessions, err = sessions.NewSQLiteRepository(filepath.Join(basePath, "sessions.db"))
	if err != nil {
		return nil, fmt.Errorf("open sessions: %w", err)
	}

	mdb.Cognition, err = cognition.NewSQLiteRepository(filepath.Join(basePath, "cognition.db"))
	if err != nil {
		mdb.Sessions.Close()
		return nil, fmt.Errorf("open cognition: %w", err)
	}

	mdb.Growth, err = growth.NewSQLiteRepository(filepath.Join(basePath, "growth.db"))
	if err != nil {
		mdb.Sessions.Close()
		mdb.Cognition.Close()
		return nil, fmt.Errorf("open growth: %w", err)
	}

	mdb.Temporal, err = temporal.NewSQLiteRepository(filepath.Join(basePath, "temporal.db"))
	if err != nil {
		mdb.Sessions.Close()
		mdb.Cognition.Close()
		mdb.Growth.Close()
		return nil, fmt.Errorf("open temporal: %w", err)
	}

	mdb.Projects, err = projects.NewSQLiteRepository(filepath.Join(basePath, "projects.db"))
	if err != nil {
		mdb.Sessions.Close()
		mdb.Cognition.Close()
		mdb.Growth.Close()
		mdb.Temporal.Close()
		return nil, fmt.Errorf("open projects: %w", err)
	}

	// Run migrations on all databases
	if err := mdb.MigrateAll(ctx); err != nil {
		mdb.Close()
		return nil, fmt.Errorf("migrate: %w", err)
	}

	return mdb, nil
}

// MigrateAll runs schema migrations on all 5 databases
func (m *MultiDB) MigrateAll(ctx context.Context) error {
	if err := m.Sessions.Migrate(ctx); err != nil {
		return fmt.Errorf("sessions: %w", err)
	}
	if err := m.Cognition.Migrate(ctx); err != nil {
		return fmt.Errorf("cognition: %w", err)
	}
	if err := m.Growth.Migrate(ctx); err != nil {
		return fmt.Errorf("growth: %w", err)
	}
	if err := m.Temporal.Migrate(ctx); err != nil {
		return fmt.Errorf("temporal: %w", err)
	}
	if err := m.Projects.Migrate(ctx); err != nil {
		return fmt.Errorf("projects: %w", err)
	}
	return nil
}

// Close releases all database resources
func (m *MultiDB) Close() error {
	var firstErr error

	if m.Sessions != nil {
		if err := m.Sessions.Close(); err != nil && firstErr == nil {
			firstErr = fmt.Errorf("close sessions: %w", err)
		}
	}
	if m.Cognition != nil {
		if err := m.Cognition.Close(); err != nil && firstErr == nil {
			firstErr = fmt.Errorf("close cognition: %w", err)
		}
	}
	if m.Growth != nil {
		if err := m.Growth.Close(); err != nil && firstErr == nil {
			firstErr = fmt.Errorf("close growth: %w", err)
		}
	}
	if m.Temporal != nil {
		if err := m.Temporal.Close(); err != nil && firstErr == nil {
			firstErr = fmt.Errorf("close temporal: %w", err)
		}
	}
	if m.Projects != nil {
		if err := m.Projects.Close(); err != nil && firstErr == nil {
			firstErr = fmt.Errorf("close projects: %w", err)
		}
	}

	return firstErr
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
