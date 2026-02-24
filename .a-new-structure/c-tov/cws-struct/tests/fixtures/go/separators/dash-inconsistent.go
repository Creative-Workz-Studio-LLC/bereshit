//omni:key B-test-dash-inconsistent
//omni:code --go -library
//omni:version a-01.00

// Package fixture provides test fixtures.
package fixture

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-dash-inconsistent
// Purpose: Go fixture with inconsistent ASCII dash separators.
//          Triggers CWS-R50-034 (style/dash-separator-width).
//

var Pragma = [][2]string{
	{"I1.key", "B-test-dash-inconsistent"},
	{"I1.format", "go"},
	{"I1.from", "tests/fixtures/go/separators/dash-inconsistent.go"},
	{"I1.at", "a-01.00"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I3.file", "dash-inconsistent.go"},
	{"I3.title", "Dash Inconsistent Separators"},
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

// --------------------------------------------------------------------------
// 1. Imports
// --------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 2. Constants
// ---------------------------------------------------------------------------

// ============================================================================
// BODY
// ============================================================================

// --------------------------------------------------------------------------
// 4. Core Logic
// --------------------------------------------------------------------------

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
// This file tests inconsistent dash separator width detection.
//
// ============================================================================
