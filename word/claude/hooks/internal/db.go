// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-hooks-internal-db
// Purpose: Lazy singletons for database access — legacy Bridge + MultiBridge
// Biblical: Ecclesiastes 4:12 - "A threefold cord is not quickly broken"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-23
// Updated: 2026-02-08
//
// Provides two access patterns:
//   GetBridge()      — Legacy Bridge backed by MultiDB via LegacyAdapter
//                      (existing hook code works unchanged)
//   GetMultiBridge() — Direct domain-specific access via MultiBridge
//                      (new code targeting specific domain databases)
//
// ═══════════════════════════════════════════════════════════════════════════

package internal

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"sync"

	"cws.studio/pkg/foundation/database"
	"cws.studio/pkg/util/fs/paths"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

var (
	// Multi-DB infrastructure (shared by both access patterns)
	multiDB     *database.MultiDB
	multiOnce   sync.Once
	multiErr    error

	// Legacy Bridge (backed by LegacyAdapter → MultiDB)
	bridge     *database.Bridge
	bridgeOnce sync.Once
	bridgeErr  error

	// MultiBridge (direct domain access)
	multiBridge     *database.MultiBridge
	multiBridgeOnce sync.Once
	multiBridgeErr  error
)

// ensureMultiDB initializes the multi-database infrastructure.
// Called by both GetBridge and GetMultiBridge.
func ensureMultiDB() (*database.MultiDB, error) {
	multiOnce.Do(func() {
		if err := paths.EnsureDatabaseDir(); err != nil {
			multiErr = err
			return
		}

		ctx := context.Background()
		mdb, err := database.OpenAll(ctx)
		if err != nil {
			multiErr = err
			return
		}

		multiDB = mdb
	})
	return multiDB, multiErr
}

// GetBridge returns the shared legacy Bridge instance.
// Backed by MultiDB via LegacyAdapter — existing hook code works unchanged.
// Thread-safe via sync.Once.
func GetBridge() (*database.Bridge, error) {
	bridgeOnce.Do(func() {
		mdb, err := ensureMultiDB()
		if err != nil {
			bridgeErr = err
			return
		}

		// LegacyAdapter implements Repository by dispatching to MultiDB
		adapter := database.NewLegacyAdapter(mdb)

		// Bridge wraps any Repository — adapter routes to domain DBs
		bridge = database.NewBridge(adapter)
	})

	return bridge, bridgeErr
}

// GetMultiBridge returns the shared MultiBridge instance.
// Provides direct domain-specific database access.
// Thread-safe via sync.Once.
func GetMultiBridge() (*database.MultiBridge, error) {
	multiBridgeOnce.Do(func() {
		mdb, err := ensureMultiDB()
		if err != nil {
			multiBridgeErr = err
			return
		}

		multiBridge = database.NewMultiBridge(mdb)
	})

	return multiBridge, multiBridgeErr
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// Legacy usage (unchanged):
//   bridge, err := internal.GetBridge()
//   if err != nil { /* handle */ }
//   bridge.StartSession(ctx, sessionID, state)
//   repo := bridge.GetRepository() // LegacyAdapter dispatches to domain DBs
//
// New domain-specific usage:
//   mb, err := internal.GetMultiBridge()
//   if err != nil { /* handle */ }
//   mb.StartSession(ctx, sessionID, state)       // → sessions.db
//   mb.RecordChoice(ctx, record, state)           // → cognition.db
//   mb.RecordPattern(ctx, pattern)                // → growth.db
//
