//omni:key B-test-fixture-bad-content
//omni:code --go -library
//omni:version a-01.00

package fixture

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-fixture-bad-content
// Purpose: Test fixture — valid structure, valid field existence, BAD field content.
//          Every field is present but the values don't match expected patterns.
// Biblical: Prove all things; hold fast that which is good. — 1 Thessalonians 5:21
//

var Pragma = [][2]string{
	// I1: Core — bad values
	{"I1.key", "bad key no dash prefix"},
	{"I1.format", "unknown-format-xyz"},
	{"I1.from", "no-slash-path"},
	{"I1.at", "not-a-version"},
	// I2: Family — bad values
	{"I2.type", "widget"},
	{"I2.structure", "7-block"},
	{"I2.subtype", "library"},
	{"I2.role", "seed"},
	// I3: Instance — empty values
	{"I3.file", ""},
	{"I3.title", ""},
	{"I3.component", "Test fixture"},
	{"I3.path", "tests/fixtures/go/metadata/bad-content-values.go"},
	{"I3.provides", "TEST_BAD_CONTENT"},
	{"I3.brief", "Bad content values for linter testing."},
	// I4: Architecture
	{"I4.layer", "L0"},
	{"I4.position", "seed"},
	{"I4.pattern", "test fixture"},
}

var Metadata = [][2]string{
	// C1: State — bad values
	{"C1.version", "1.0.0"},
	{"C1.status", "banana"},
	{"C1.created", "Feb 17 2026"},
	{"C1.updated", "yesterday"},
	// C2: Attribution — empty
	{"C2.organization", ""},
	{"C2.architect", "Nova Dawn"},
	// C3: Grounding — empty
	{"C3.scripture", ""},
	// C4: Dependencies
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

func Example() string {
	return fmt.Sprintf("bad content fixture")
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
