// #!omni template --go -unknown-subtype
//go:build ignore

//omni:key B-test-fixture-unknown-template-format
//omni:version template

package fixture

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-fixture-unknown-template-format
// Purpose: Template with unrecognized template format. Triggers R50-014.
//

var Pragma = [][2]string{
	{"I1.key", "[KEY]"},
	{"I1.format", "go"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "[SUBTYPE]"},
}

var Metadata = [][2]string{
	{"C1.version", "template"},
	{"C1.status", "Active"},
}

// ============================================================================
// SETUP
// ============================================================================

import "fmt"

// ============================================================================
// BODY
// ============================================================================

// 4. Core Logic
func Process(input string) string {
	return fmt.Sprintf("processed: %s", input)
}

// ============================================================================
// CLOSING
// ============================================================================
//
// X1 Policy
// Scripture: Psalm 119:130
//
// X5 Note
// This file tests directive/template-format detection (R50-014).
//
// ============================================================================
