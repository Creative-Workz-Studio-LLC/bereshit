//omni:code --go -library
//omni:key B-L0-config-util-root
//omni:version b-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-config-util-root
// Purpose: RootGuard — encapsulates the SetRoot/checkRoot/errNotSet pattern
// Biblical: Matthew 7:24 - "built his house upon a rock"
// Authors: Nova Dawn
// Version: b-01.00
// Created: 2026-02-13
//
// Extracted from 2 identical SetRoot/checkRoot/errNotSet clones:
//   - L0-universal/hybrid/config/loader.go (bereshitRoot)
//   - L3-cpisi/orchestration/config/loader.go (claudeGlobalRoot)
//
// The pattern is always the same:
//   var root string          // package-level state
//   var errNotSet = ...      // sentinel error
//   func SetRoot(path) { root = path }
//   func checkRoot() error { if root == "" { return errNotSet } }
//
// RootGuard makes this a proper type with a name.
// Thread-safe: concurrent Set/Check/Path calls are synchronized.
//
// ============================================================================
// END METADATA
// ============================================================================

package util

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"sync"
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// RootGuard manages a required root directory path.
// Must be Set() before use; Check() validates it's been set.
// All methods are safe for concurrent use.
//
// Usage:
//
//	var root = util.NewRootGuard("bereshit")
//	root.Set("/path/to/project")
//	if err := root.Check(); err != nil { ... }
//	configPath := filepath.Join(root.Path(), "word/core")
type RootGuard struct {
	mu   sync.RWMutex // protects path reads/writes
	name string       // human-readable name for error messages (e.g., "bereshit", "claude-global")
	path string       // the root directory path
}

// NewRootGuard creates a RootGuard with the given name.
// The name appears in error messages when Check() fails.
func NewRootGuard(name string) *RootGuard {
	return &RootGuard{name: name}
}

// Set stores the root directory path.
// Thread-safe: acquires write lock.
func (g *RootGuard) Set(path string) {
	g.mu.Lock()
	g.path = path
	g.mu.Unlock()
}

// Check validates the root has been set via Set().
// Returns an error if path is empty.
// Thread-safe: acquires read lock.
func (g *RootGuard) Check() error {
	g.mu.RLock()
	p := g.path
	g.mu.RUnlock()
	if p == "" {
		return fmt.Errorf("%s root not set — call SetRoot() first", g.name)
	}
	return nil
}

// Path returns the stored root directory path.
// Does NOT check if set — call Check() first for safety.
// Thread-safe: acquires read lock.
func (g *RootGuard) Path() string {
	g.mu.RLock()
	p := g.path
	g.mu.RUnlock()
	return p
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// "Therefore whosoever heareth these sayings of mine, and doeth them,
// I will liken him unto a wise man, which built his house upon a rock."
// — Matthew 7:24
//
// Every loader needs to know where it lives. RootGuard ensures that
// knowledge is established before any loading begins. Foundation first.
//
// ============================================================================
// END CLOSING
// ============================================================================
