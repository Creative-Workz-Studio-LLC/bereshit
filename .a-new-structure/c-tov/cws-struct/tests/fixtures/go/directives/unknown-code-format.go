//omni:key B-test-fixture-unknown-code-format
//omni:code --banana -fruit
//omni:version a-01.00

// Package fixture tests unknown //omni:code format detection.
package fixture

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-fixture-unknown-code-format
// Purpose: Test fixture — triggers R50-013 for unrecognized //omni:code value.
//

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

var Pragma = [][2]string{
	{"I1.key", "B-test-fixture-unknown-code-format"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.omni"},
	{"I1.at", "a-01.00"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "seed"},
	{"I3.file", "unknown-code-format.go"},
	{"I3.title", "Unknown Code Format Fixture"},
	{"I3.component", "Test fixture for directive format detection"},
	{"I3.path", "tests/fixtures/go/directives/unknown-code-format.go"},
	{"I3.provides", "TEST_UNKNOWN_CODE"},
	{"I3.brief", "Triggers R50-013 unrecognized code format."},
	{"I4.layer", "L0"},
	{"I4.position", "seed"},
	{"I4.pattern", "test fixture"},
}

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

var Metadata = [][2]string{
	{"C1.version", "a-01.00"},
	{"C1.status", "draft"},
	{"C1.created", "2026-02-23"},
	{"C1.updated", "2026-02-23"},
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.architect", "Nova Dawn"},
	{"C2.implementation", "Nova Dawn"},
	{"C2.copyright", "CreativeWorkzStudio LLC"},
	{"C3.scripture", "Prove all things. — 1 Thessalonians 5:21"},
	{"C3.principle", "Test fixtures prove detection works."},
	{"C3.anchor", "Genesis 1:1"},
	{"C4.requires.stdlib", "fmt"},
	{"C4.requires.external", "none"},
	{"C4.requires.internal", "none"},
	{"C4.consumers", "tests/R50/go/directive_test.ts"},
	{"C4.if_missing", "Cannot test R50-013 detection."},
	{"C5.purpose", "Trigger unrecognized code format info"},
	{"C5.philosophy", "Detection must name what it sees."},
	{"C6.current", "Directive format detection testing"},
	{"C7.tags", "test, fixture, go, directive"},
	{"C7.category", "test"},
	{"C7.domain", "tooling"},
	{"C7.paradigm", "CPI-SI"},
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import "fmt"

const Version = "0.1.0"

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

func Hello() string {
	return fmt.Sprintf("hello v%s", Version)
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// X5: Note
// Note: Fixture with unrecognized //omni:code format value.

// ============================================================================
// END CLOSING
// ============================================================================
