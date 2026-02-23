//omni:key B-test-fixture-valid-library
//omni:code --go -library
//omni:version a-01.00

// Package fixture provides test fixtures for the Go 4-block linter.
package fixture

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-test-fixture-valid-library
// Purpose: Test fixture — complete valid Go library with all I/C fields.
// Biblical: Prove all things; hold fast that which is good. — 1 Thessalonians 5:21
//

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

// Pragma carries OmniCode identity fields (I1-I4).
var Pragma = [][2]string{
	// I1: Core
	{"I1.key", "B-test-fixture-valid-library"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.omni"},
	{"I1.at", "a-01.00"},
	// I2: Family
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "seed"},
	// I3: Instance
	{"I3.file", "valid-library.go"},
	{"I3.title", "Valid Library Fixture"},
	{"I3.component", "Test fixture for Go linter"},
	{"I3.path", "tests/fixtures/go/valid-library.go"},
	{"I3.provides", "TEST_VALID_LIBRARY"},
	{"I3.brief", "Complete valid library for linter testing."},
	// I4: Architecture
	{"I4.layer", "L0"},
	{"I4.position", "seed"},
	{"I4.pattern", "test fixture"},
}

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

// Metadata carries OmniCode context fields (C1-C7).
var Metadata = [][2]string{
	// C1: State
	{"C1.version", "a-01.00"},
	{"C1.status", "draft"},
	{"C1.created", "2026-02-17"},
	{"C1.updated", "2026-02-22"},
	// C2: Attribution
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.architect", "Nova Dawn"},
	{"C2.implementation", "Nova Dawn"},
	{"C2.copyright", "CreativeWorkzStudio LLC"},
	// C3: Grounding
	{"C3.scripture", "Prove all things; hold fast that which is good. — 1 Thessalonians 5:21"},
	{"C3.principle", "Tests prove truth. Fixtures embody it."},
	{"C3.anchor", "Genesis 1:1"},
	// C4: Dependencies
	{"C4.requires.stdlib", "fmt"},
	{"C4.requires.external", "none"},
	{"C4.requires.internal", "none"},
	{"C4.consumers", "tests/handlers/go_test.ts"},
	{"C4.if_missing", "Go linter tests have no valid fixture to test against."},
	// C5: Design
	{"C5.purpose", "Test the Go 4-block linter happy path"},
	{"C5.philosophy", "A valid fixture must exercise every check without triggering any."},
	// C6: Roadmap
	{"C6.current", "Fixture for initial Go linter test suite"},
	// C7: Discovery
	{"C7.tags", "test, fixture, go, linter"},
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

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

import "fmt"

// ──────────────────────────────────────────────────────────────────────────
// 2. Modules
// ──────────────────────────────────────────────────────────────────────────

// No submodules — single-file library fixture.

// ──────────────────────────────────────────────────────────────────────────
// 3. Constants
// ──────────────────────────────────────────────────────────────────────────

const Version = "0.1.0"

// ──────────────────────────────────────────────────────────────────────────
// 6. Error Types
// ──────────────────────────────────────────────────────────────────────────

type ConfigError struct {
	Field   string
	Message string
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────

type Config struct {
	Name    string
	Version string
}

// ──────────────────────────────────────────────────────────────────────────
// 11. Variables
// ──────────────────────────────────────────────────────────────────────────

var defaultConfig = Config{Name: "test", Version: Version}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 0. Org Chart
// ──────────────────────────────────────────────────────────────────────────

// fixture — test library with Config type.
// Provides NewConfig, GetVersion, formatConfig.

// ──────────────────────────────────────────────────────────────────────────
// 1. Identity Access
// ──────────────────────────────────────────────────────────────────────────

func Key() string     { return Pragma[0][1] }
func Format() string  { return Pragma[1][1] }

// ──────────────────────────────────────────────────────────────────────────
// 2. Trait Implementations
// ──────────────────────────────────────────────────────────────────────────

func (e *ConfigError) Error() string {
	return fmt.Sprintf("config error: %s — %s", e.Field, e.Message)
}

// ──────────────────────────────────────────────────────────────────────────
// 3. Constructors
// ──────────────────────────────────────────────────────────────────────────

func NewConfig(name string) Config {
	return Config{Name: name, Version: Version}
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────

func formatConfig(c Config) string {
	return fmt.Sprintf("%s v%s", c.Name, c.Version)
}

// ──────────────────────────────────────────────────────────────────────────
// 5. Queries
// ──────────────────────────────────────────────────────────────────────────

func (c Config) GetName() string { return c.Name }

// ──────────────────────────────────────────────────────────────────────────
// 6. Output Display
// ──────────────────────────────────────────────────────────────────────────

func (c Config) String() string {
	return fmt.Sprintf("Config{%s}", c.Name)
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Free Functions
// ──────────────────────────────────────────────────────────────────────────

func GetVersion() string {
	return Version
}

// ──────────────────────────────────────────────────────────────────────────
// 20. Core Operations
// ──────────────────────────────────────────────────────────────────────────

func ValidateConfig(c Config) error {
	if c.Name == "" {
		return &ConfigError{Field: "Name", Message: "cannot be empty"}
	}
	return nil
}

// ──────────────────────────────────────────────────────────────────────────
// 21. Error Handling
// ──────────────────────────────────────────────────────────────────────────

func WrapConfigError(field string, err error) *ConfigError {
	return &ConfigError{Field: field, Message: err.Error()}
}

// ──────────────────────────────────────────────────────────────────────────
// 22. Public APIs
// ──────────────────────────────────────────────────────────────────────────

func LoadConfig(name string) (Config, error) {
	c := NewConfig(name)
	if err := ValidateConfig(c); err != nil {
		return Config{}, err
	}
	return c, nil
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────

// No tests in library — tested externally.

// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
// Never: remove 4-block structure
// Careful: change I/C field values without updating consumers
// Safe: add new functions, types, constants

// ──────────────────────────────────────────────────────────────────────────
// X4: Reference
// ──────────────────────────────────────────────────────────────────────────
// Related: tests/handlers/go_test.ts

// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
// Note: Valid Go library test fixture. Exercises every linter check without
//   triggering errors or warnings.
// Scripture: "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21

// ============================================================================
// END CLOSING
// ============================================================================
