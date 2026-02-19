//omni:key B-test-fixture-missing-required-zones
//omni:code --go -library
//omni:version a-01.00

package fixture

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-fixture-missing-required-zones
// Purpose: Test fixture — CLOSING block present but missing required X1 and X5
//          documentation zones. Should trigger closing/required-X1 and
//          closing/required-X5 info rules.
//

var Pragma = [][2]string{
	{"I1.key", "B-test-fixture-missing-required-zones"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.omni"},
	{"I1.at", "a-01.00"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I3.file", "missing-required-zones.go"},
	{"I3.title", "Missing Required Zones Fixture"},
}

var Metadata = [][2]string{
	{"C1.version", "a-01.00"},
	{"C1.status", "draft"},
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C3.scripture", "Genesis 1:1"},
	{"C4.requires.stdlib", "none"},
	{"C4.consumers", "tests/handlers/go_test.ts"},
}

// ============================================================================
// END METADATA
// ============================================================================

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

func Hello() string {
	return fmt.Sprintf("hello")
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────

// No tests.

// ──────────────────────────────────────────────────────────────────────────
// X2: Changelog
// ──────────────────────────────────────────────────────────────────────────
// Nothing yet.

// NOTE: X1 (Policy) and X5 (Note) are intentionally MISSING.
// This should trigger closing/required-X1 and closing/required-X5.

// ============================================================================
// END CLOSING
// ============================================================================
