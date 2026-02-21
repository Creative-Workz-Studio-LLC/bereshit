//omni:key B-test-fixture-valid-executable
//omni:code --go -executable
//omni:version a-01.00

// Package main provides a test fixture for Go executable validation.
package main

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-fixture-valid-executable
// Purpose: Test fixture — valid Go executable with main() in CLOSING Ce.
//

// Pragma carries OmniCode identity fields (I1-I4).
var Pragma = [][2]string{
	{"I1.key", "B-test-fixture-valid-executable"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/executable.omni"},
	{"I1.at", "a-01.00"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "executable"},
	{"I3.file", "valid-executable.go"},
	{"I3.title", "Valid Executable Fixture"},
}

// Metadata carries OmniCode context fields (C1-C7).
var Metadata = [][2]string{
	{"C1.version", "a-01.00"},
	{"C1.status", "draft"},
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C3.scripture", "In the beginning God created the heaven and the earth. — Genesis 1:1"},
	{"C4.requires.stdlib", "fmt"},
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

func run() {
	fmt.Println("running")
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// Ce — Entry Point
// ──────────────────────────────────────────────────────────────────────────

func main() {
	run()
}

// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
// Valid executable fixture — main() correctly placed in CLOSING Ce zone.

// ============================================================================
// END CLOSING
// ============================================================================
