//go:build ignore

//omni:key B-test-derived-build-ignore
//omni:code --go -library
//omni:version a-01.00

// Package fixture provides test fixtures for the Go 4-block linter.
package fixture

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-derived-build-ignore
// Purpose: Derived file that incorrectly has //go:build ignore.
//          Triggers CWS-R50-041 (derived/build-ignore).
//

var Pragma = [][2]string{
	{"I1.key", "B-test-derived-build-ignore"},
	{"I1.format", "go"},
	{"I1.from", "tests/fixtures/go/classify/derived-with-build-ignore.go"},
	{"I1.at", "a-01.00"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I3.file", "derived-with-build-ignore.go"},
	{"I3.title", "Derived With Build Ignore"},
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

// ============================================================================
// BODY
// ============================================================================

// 4. Core Logic
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
// This file is a test fixture for derived/build-ignore detection.
//
// ============================================================================
