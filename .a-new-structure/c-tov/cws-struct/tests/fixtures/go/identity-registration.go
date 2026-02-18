//omni:key B-test-fixture-identity-registration
//omni:code --go -library
//omni:version a-01.00

package fixture

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-fixture-identity-registration
// Purpose: Test fixture — has Pragma identity var but no init() or register call.
//          Should produce identity/register info.
//

var Pragma = [][2]string{
	{"I1.key", "B-test-fixture-identity-registration"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.omni"},
	{"I1.at", "a-01.00"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I3.file", "identity-registration.go"},
	{"I3.title", "Identity Registration Fixture"},
}

var Metadata = [][2]string{
	{"C1.version", "a-01.00"},
	{"C1.status", "draft"},
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C3.scripture", "Genesis 1:1"},
	{"C4.requires.stdlib", "none"},
	{"C4.consumers", "tests/handlers/go_test.ts"},
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

type Config struct {
	Name string
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

func NewConfig() Config {
	return Config{Name: "default"}
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
