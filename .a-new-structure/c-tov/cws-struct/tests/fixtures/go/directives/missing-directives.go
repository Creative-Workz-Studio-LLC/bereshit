// Package fixture provides test fixtures.
package fixture

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-missing-directives
// Purpose: Go fixture with NO //omni: directives at all.
//          Derived file missing required and recommended directives.
//          Triggers CWS-R50-011 (directive/{{directive}})
//          and CWS-R50-012 (directive/{{directive}}/recommended).
//

var Pragma = [][2]string{
	{"I1.key", "B-test-missing-directives"},
	{"I1.format", "go"},
	{"I1.from", "tests/fixtures/go/directives/missing-directives.go"},
	{"I1.at", "a-01.00"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I3.file", "missing-directives.go"},
	{"I3.title", "Missing Directives"},
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
// This file tests missing directive detection.
//
// ============================================================================
