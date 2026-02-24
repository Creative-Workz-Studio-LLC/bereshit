//omni:key B-test-nonstandard-widths
//omni:code --go -library
//omni:version a-01.00

// Package fixture provides test fixtures.
package fixture

// ========================================================================
// METADATA
// ========================================================================
//
// Key:     B-test-nonstandard-widths
// Purpose: Go fixture with consistent but non-standard separator widths.
//          All = separators are 72 chars (standard: 76).
//          All box separators are 70 chars (standard: 74).
//          Triggers CWS-R50-031 (style/eq-separator-standard)
//          and CWS-R50-033 (style/box-separator-standard).
//

var Pragma = [][2]string{
	{"I1.key", "B-test-nonstandard-widths"},
	{"I1.format", "go"},
	{"I1.from", "tests/fixtures/go/separators/nonstandard-widths.go"},
	{"I1.at", "a-01.00"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I3.file", "nonstandard-widths.go"},
	{"I3.title", "Nonstandard Separator Widths"},
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

// ========================================================================
// SETUP
// ========================================================================

import "fmt"

// ──────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────

// ========================================================================
// BODY
// ========================================================================

// ──────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────

func Process() string {
	return fmt.Sprintf("hello")
}

// ========================================================================
// CLOSING
// ========================================================================
//
// X1 Policy
// Scripture: Psalm 119:130
//
// X5 Note
// This file tests nonstandard but consistent separator width detection.
//
// ========================================================================
