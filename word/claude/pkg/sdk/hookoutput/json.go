//omni:key claude-global-pkg-hookoutput-json
//omni:code go
//omni:version 2.1.0
// ============================================================================
// METADATA
// ============================================================================
//
// Key: claude-global-pkg-hookoutput-json
// Purpose: JSON marshaling helper for hook output
// Biblical: "A faithful witness will not lie" - Proverbs 14:5
// Authors: Nova Dawn
// Version: 2.1.0
// Created: 2026-02-24
// Updated: 2026-02-24
//
// MustJSON provides panic-on-error JSON marshaling for hook output.
// Hooks must produce valid JSON or fail hard — silent corruption
// is worse than a visible crash.
//
// ============================================================================
// END METADATA
// ============================================================================

package hookoutput

// ============================================================================
// SETUP
// ============================================================================

// --- Imports ---

import "encoding/json"

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// --- Helpers ---

// MustJSON marshals response to JSON, panics on error (for hook output)
func MustJSON(v any) []byte {
	data, err := json.Marshal(v)
	if err != nil {
		panic(err)
	}
	return data
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Usage:
//   os.Stdout.Write(hookoutput.MustJSON(response))
//
// Panics on marshal error — hooks must produce valid JSON or fail visibly.
//
// ============================================================================
// END CLOSING
// ============================================================================
