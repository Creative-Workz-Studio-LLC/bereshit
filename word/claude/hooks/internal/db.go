// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-hooks-internal-db
// Purpose: Lazy singleton for database Bridge - shared across all hooks
// Biblical: Ecclesiastes 4:12 - "A threefold cord is not quickly broken"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-23
//
// This provides a single Bridge instance for all hooks to use.
// Lazy initialization - only connects when first needed.
//
// ═══════════════════════════════════════════════════════════════════════════

package internal

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"sync"

	"github.com/creativeworkzstudio/claude-global/pkg/foundation/database"
	"github.com/creativeworkzstudio/claude-global/pkg/util/fs/paths"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

var (
	bridge     *database.Bridge
	bridgeOnce sync.Once
	bridgeErr  error
)

// GetBridge returns the shared Bridge instance, initializing if needed.
// Thread-safe via sync.Once - only initializes once regardless of calls.
func GetBridge() (*database.Bridge, error) {
	bridgeOnce.Do(func() {
		// Ensure data directory exists
		if err := paths.EnsureDatabaseDir(); err != nil {
			bridgeErr = err
			return
		}

		// Create SQLite repository
		dbPath := paths.DatabasePath()
		repo, err := database.NewSQLiteRepository(dbPath)
		if err != nil {
			bridgeErr = err
			return
		}

		// Run migrations (schema creation)
		ctx := context.Background()
		if err := repo.Migrate(ctx); err != nil {
			bridgeErr = err
			return
		}

		// Create Bridge with repository
		bridge = database.NewBridge(repo)
	})

	return bridge, bridgeErr
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// Usage:
//   bridge, err := internal.GetBridge()
//   if err != nil { /* handle */ }
//   bridge.StartSession(ctx, sessionID, state)
//
