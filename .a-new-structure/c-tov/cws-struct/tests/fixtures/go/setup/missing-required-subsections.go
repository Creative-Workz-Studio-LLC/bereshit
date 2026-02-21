//omni:key B-test-fixture-missing-required-setup
//omni:code --go -library
//omni:version a-01.00

package fixture

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-fixture-missing-required-setup
// Purpose: Test fixture — library subtype missing required SETUP subsections.
//          Has Imports but missing ErrorTypes (S5) and CoreTypes (S6) which
//          the schema marks as requiredIn: ["library"]. Should trigger
//          setup/required-subsection warnings.
//

var Pragma = [][2]string{
	{"I1.key", "B-test-fixture-missing-required-setup"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.omni"},
	{"I1.at", "a-01.00"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I3.file", "missing-required-subsections.go"},
	{"I3.title", "Missing Required SETUP Subsections"},
}

var Metadata = [][2]string{
	{"C1.version", "a-01.00"},
	{"C1.status", "draft"},
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C3.scripture", "Genesis 1:1"},
	{"C4.requires.stdlib", "fmt"},
	{"C4.consumers", "tests/handlers/go_test.ts"},
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

import "fmt"

// ──────────────────────────────────────────────────────────────────────────
// 2. Constants
// ──────────────────────────────────────────────────────────────────────────

const Version = "1.0.0"

// NOTE: Missing S5 ErrorTypes and S6 CoreTypes — both required for library.

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

func Hello() string {
	return fmt.Sprintf("hello %s", Version)
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
// Never: remove 4-block structure
// Safe: add subsections

// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
// Note: Library missing required SETUP subsections for testing.
// Scripture: "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21

// ============================================================================
// END CLOSING
// ============================================================================
