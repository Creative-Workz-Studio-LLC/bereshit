//omni:key B-test-fixture-content-placement-correct
//omni:code --go -library
//omni:version a-01.00

package fixture

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-fixture-content-placement-correct
// Purpose: Test fixture — all constructs in their correct blocks.
//          Types/imports/consts in SETUP, funcs in BODY.
//

var Pragma = [][2]string{
	{"I1.key", "B-test-fixture-content-placement-correct"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.omni"},
	{"I1.at", "a-01.00"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I3.file", "content-placement-correct.go"},
	{"I3.title", "Content Placement Correct Fixture"},
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

const Version = "1.0.0"

var debug = false

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

func NewConfig(name string) Config {
	return Config{Name: fmt.Sprintf("cfg-%s", name)}
}

func (c Config) String() string {
	if debug {
		return fmt.Sprintf("Config{%s}", c.Name)
	}
	return c.Name
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
