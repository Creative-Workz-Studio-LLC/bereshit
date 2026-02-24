//omni:key B-test-inconsistent-separators
//omni:code --go -library
//omni:version a-01.00

// Package fixture provides test fixtures.
package fixture

// ===========================================================================
// METADATA
// ===========================================================================
//
// Key:     B-test-inconsistent-separators
// Purpose: Go fixture with inconsistent separator widths.
//          METADATA uses 75-char = separators, other blocks use 76-char.
//          Triggers CWS-R50-030 (style/eq-separator-width).
//

var Pragma = [][2]string{
	{"I1.key", "B-test-inconsistent-separators"},
	{"I1.format", "go"},
	{"I1.from", "tests/fixtures/go/separators/inconsistent-widths.go"},
	{"I1.at", "a-01.00"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I3.file", "inconsistent-widths.go"},
	{"I3.title", "Inconsistent Separator Widths"},
}

var Metadata = [][2]string{
	{"C1.version", "a-01.00"},
	{"C1.status", "Active"},
	{"C1.created", "2026-02-23"},
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C3.scripture", "Psalm 119:130"},
	{"C4.requires", "none"},
	{"C4.consumers", "tests"},
}

// ============================================================================
// SETUP
// ============================================================================

import "fmt"

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

// ───────────────────────────────────────────────────────────────────────────────
// 2. Constants
// ───────────────────────────────────────────────────────────────────────────────

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────

func Process() string {
	return fmt.Sprintf("hello")
}

// ============================================================================
// CLOSING
// ============================================================================
//
// X1 Policy
// Scripture: Psalm 119:130
//
// X5 Note
// This file tests inconsistent separator width detection.
//
// ============================================================================
