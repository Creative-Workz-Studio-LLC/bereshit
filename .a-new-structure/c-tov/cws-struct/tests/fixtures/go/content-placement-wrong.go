//omni:key B-test-fixture-content-placement-wrong
//omni:code --go -library
//omni:version a-01.00

package fixture

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-fixture-content-placement-wrong
// Purpose: Test fixture — func_decl in SETUP, import in BODY.
//

var Pragma = [][2]string{
	{"I1.key", "B-test-fixture-content-placement-wrong"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.omni"},
	{"I1.at", "a-01.00"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I3.file", "content-placement-wrong.go"},
	{"I3.title", "Content Placement Wrong Fixture"},
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

import "fmt"

type Config struct {
	Name string
}

func wrongFuncInSetup() string {
	return "I should be in BODY"
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

import "os"

type WrongTypeInBody struct {
	Path string
}

func correctFunc() string {
	return fmt.Sprintf("correct in BODY")
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
