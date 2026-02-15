//omni:code --go -library
//omni:key B-L0-config-util-must
//omni:version b-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-config-util-must
// Purpose: Generic panic-on-error wrapper — Must[T] for config loading
// Biblical: Joshua 1:9 - "Be strong and of a good courage"
// Authors: Nova Dawn
// Version: b-01.00
// Created: 2026-02-13
//
// Replaces 7 identical MustLoad* wrappers across 4 files.
// Go 1.18+ generics: one function serves all return types.
//
// Usage:
//   func MustLoadSchemas(dir string) *Schemas { return util.Must(LoadSchemas(dir)) }
//
// ============================================================================
// END METADATA
// ============================================================================

package util

// ============================================================================
// SETUP
// ============================================================================

import "fmt"

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// (T, error) Pattern
// ────────────────────────────────────────────────────────────────

// Must panics if err is non-nil, otherwise returns val.
// Use for config loading in init() or startup where failure is fatal.
//
// Go 1.18+ generics — works with any return type.
//
//	schemas := util.Must(LoadSchemas(dir))
func Must[T any](val T, err error) T {
	if err != nil {
		panic(fmt.Sprintf("config: must: %v", err))
	}
	return val
}

// MustNoErr panics if err is non-nil. Use for operations that return
// only an error (no value) where failure is fatal.
//
//	util.MustNoErr(os.MkdirAll(dir, 0755))
func MustNoErr(err error) {
	if err != nil {
		panic(fmt.Sprintf("config: must: %v", err))
	}
}

// ────────────────────────────────────────────────────────────────
// (T, bool) Pattern
// ────────────────────────────────────────────────────────────────

// MustVal panics with msg if ok is false, otherwise returns val.
// Use for map lookups or type assertions where absence is fatal.
//
//	system := util.MustVal(index[name], ok, "system not found: "+name)
func MustVal[T any](val T, ok bool, msg string) T {
	if !ok {
		panic(fmt.Sprintf("config: must: %s", msg))
	}
	return val
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// "Have not I commanded thee? Be strong and of a good courage; be not
// afraid, neither be thou dismayed." — Joshua 1:9
//
// Must is courage in code. If the config isn't there at startup,
// don't limp along — halt immediately and name the problem.
//
// ============================================================================
// END CLOSING
// ============================================================================
