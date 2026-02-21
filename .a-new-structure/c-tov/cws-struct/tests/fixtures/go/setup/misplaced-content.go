//omni:key B-test-fixture-misplaced-content
//omni:code --go -library
//omni:version a-01.00

package fixture

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-fixture-misplaced-content
// Purpose: Test fixture — SETUP subsections with misplaced content.
//          Type declaration in Constants, const in CoreTypes.
//          Linter should emit info diagnostics about placement.
//

var Pragma = [][2]string{
	{"I1.key", "B-test-fixture-misplaced-content"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.omni"},
	{"I1.at", "a-01.00"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I3.file", "misplaced-content.go"},
	{"I3.title", "Misplaced Content Fixture"},
}

var Metadata = [][2]string{
	{"C1.version", "a-01.00"},
	{"C1.status", "draft"},
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C3.scripture", "Let all things be done decently and in order. — 1 Corinthians 14:40"},
	{"C4.requires.stdlib", "none"},
	{"C4.consumers", "tests/handlers/go_test.ts"},
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Constants
// ──────────────────────────────────────────────────────────────────────────

const MaxWidgets = 100

// MISPLACED: type declaration belongs in CoreTypes, not Constants
type Widget struct {
	Name string
}

// ──────────────────────────────────────────────────────────────────────────
// 2. Core Types
// ──────────────────────────────────────────────────────────────────────────

// MISPLACED: const belongs in Constants, not CoreTypes
const DefaultName = "widget"

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

func NewWidget() Widget {
	return Widget{Name: DefaultName}
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
// ============================================================================
