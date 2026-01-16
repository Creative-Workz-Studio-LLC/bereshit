package coordinates_test

import (
	"testing"

	"github.com/creativeworkzstudio/claude-global/pkg/core/coordinates"
	"github.com/creativeworkzstudio/claude-global/pkg/foundation/schema"
)

// ============================================================================
// METADATA
// ============================================================================
//
// Coordinates Assurance Tests — Wrappers for go test compatibility
//
// Philosophy: Tests with 1 solution designed to fail become specifications.
//
//   DETECT:  The specification (what MUST be true)
//   ASSESS:  What was found (current state)
//   RESTORE: The path from current to required (empty if already there)
//
// The logging format IS the filing system. Consistent. Parseable. Growable.
//
// Traces to:
//   HALT_01 (ANCHOR_GENESIS) — Genesis 1:1 = origin of all coordinates
//   HALT_07 (MECHANISM_POSITION) — Position IS computation
//
// ============================================================================

// Test root - the claude-global project root
const testRoot = "/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/claude-global"

// Bereshit root - the Bereshit project root (separate from claude-global)
const bereshitRoot = "/media/seanje-lenox-wise/Project/Bereshit"

// Test identities
const (
	testInstance = "nova_dawn"
	testUser     = "seanje-lenox-wise"
)

// ────────────────────────────────────────────────────────────────
// Individual Assurance Tests
// ────────────────────────────────────────────────────────────────
// Each test:
//   1. Calls the assurance function (returns structured result)
//   2. Logs the full D/A/R structure (the filing system)
//   3. Converts score to test pass/fail

func TestAssureTypeAliases(t *testing.T) {
	// DETECT: Types should be aliased from schema
	// ASSESS: Verify coordinates types match schema types
	result := schema.AssuranceResult{
		Name:    "Coordinates Type Aliases",
		HALT:    "HALT_07",
		Detect:  "coordinates.Perspective must alias schema.Perspective",
		Score:   schema.ScorePass,
	}

	// Verify Perspective constants match
	if coordinates.PerspectivePast != schema.PerspectivePast {
		result.Score = schema.ScoreFail
		result.Assess = "PerspectivePast mismatch"
	} else if coordinates.PerspectivePresent != schema.PerspectivePresent {
		result.Score = schema.ScoreFail
		result.Assess = "PerspectivePresent mismatch"
	} else if coordinates.PerspectiveFuture != schema.PerspectiveFuture {
		result.Score = schema.ScoreFail
		result.Assess = "PerspectiveFuture mismatch"
	} else {
		result.Assess = "All Perspective constants match schema"
	}

	logDAR(t, result)
	requirePass(t, result)
}

func TestAssureBibleIndex(t *testing.T) {
	// DETECT: BibleIndex should load 66 books
	result := schema.AssuranceResult{
		Name:    "Bible Index Loading",
		HALT:    "HALT_01",
		Detect:  "BibleIndex must contain 66 books (Old + New Testament)",
		Score:   schema.ScorePass,
	}

	bi := coordinates.NewBibleIndex(testRoot + "/bereshit/word/scripture/KJV")
	bookCount := len(bi.Books)

	if bookCount != 66 {
		result.Score = schema.ScoreFail
		result.Assess = "Book count mismatch"
		result.Context = map[string]string{"expected": "66", "actual": string(rune(bookCount))}
	} else {
		result.Assess = "All 66 books loaded correctly"
	}

	logDAR(t, result)
	requirePass(t, result)
}

func TestAssureGenesisOrigin(t *testing.T) {
	// DETECT: Genesis 1:1 must be the coordinate origin
	result := schema.AssuranceResult{
		Name:    "Genesis Origin",
		HALT:    "HALT_01",
		Detect:  "Genesis 1:1 in PRESENT perspective must be (0, 0, 0) origin",
		Score:   schema.ScorePass,
	}

	bi := coordinates.NewBibleIndex(testRoot + "/bereshit/word/scripture/KJV")
	bi.SetPerspective(coordinates.PerspectivePresent)

	ref := coordinates.BibleReference{Book: "Genesis", Chapter: 1, Verse: 1}
	coords, err := bi.ToCoords(ref)

	if err != nil {
		result.Score = schema.ScoreFail
		result.Assess = "Failed to get Genesis 1:1 coordinates: " + err.Error()
	} else if !coords.IsGenesis() {
		result.Score = schema.ScoreFail
		result.Assess = "Genesis 1:1 does not map to origin"
	} else {
		result.Assess = "Genesis 1:1 correctly maps to coordinate origin"
	}

	logDAR(t, result)
	requirePass(t, result)
}

func TestAssureBereshitLoader(t *testing.T) {
	// Call the actual assurance function using Bereshit project root
	result := coordinates.AssureBereshitLoader(bereshitRoot)

	logDAR(t, result)
	requirePass(t, result)
}

// ────────────────────────────────────────────────────────────────
// Full Suite Test
// ────────────────────────────────────────────────────────────────

func TestRunAllAssurances(t *testing.T) {
	// Header
	t.Logf("═══════════════════════════════════════════════════════════")
	t.Logf(" COORDINATES ASSURANCE SUITE")
	t.Logf("═══════════════════════════════════════════════════════════")

	// Run individual tests (they log their own D/A/R)
	t.Run("TypeAliases", TestAssureTypeAliases)
	t.Run("BibleIndex", TestAssureBibleIndex)
	t.Run("GenesisOrigin", TestAssureGenesisOrigin)
	t.Run("BereshitLoader", TestAssureBereshitLoader)

	// Summary
	t.Logf("───────────────────────────────────────────────────────────")
	t.Logf(" Coordinates Assurance Suite Complete")
	t.Logf("═══════════════════════════════════════════════════════════")
}

// ────────────────────────────────────────────────────────────────
// Logging — The Filing System Foundation
// ────────────────────────────────────────────────────────────────
// This format will grow into file-based logging.
// Structure is consistent. Parseable. Machine-readable.

// logDAR logs the full Detect/Assess/Restore structure.
// DETECT is ALWAYS logged because it IS the specification.
// LookAt and Debug give machine/human investigation paths.
func logDAR(t *testing.T, r schema.AssuranceResult) {
	t.Helper()

	// Header: [HALT] Score Name
	t.Logf("[%s] %s %s", r.HALT, r.Score.Emoji(), r.Name)

	// DETECT: The specification (what must be true)
	t.Logf("  DETECT:  %s", r.Detect)

	// ASSESS: What was found
	t.Logf("  ASSESS:  %s", r.Assess)

	// RESTORE: Recovery path (shows gap between current and required)
	if r.Restore != "" {
		t.Logf("  RESTORE: %s", r.Restore)
	} else {
		t.Logf("  RESTORE: (none needed)")
	}

	// LOOK AT: Where to investigate (typed for machine parsing)
	if len(r.LookAt) > 0 {
		t.Logf("  LOOK AT:")
		for _, entry := range r.LookAt {
			t.Logf("    [%s] %s — %s", entry.Type, entry.Path, entry.Hint)
		}
	}

	// DEBUG: How to investigate further
	if len(r.Debug) > 0 {
		t.Logf("  DEBUG:")
		for _, cmd := range r.Debug {
			t.Logf("    $ %s", cmd)
		}
	}

	// Context: Key-value diagnostics
	if len(r.Context) > 0 {
		t.Logf("  CONTEXT:")
		for k, v := range r.Context {
			t.Logf("    %s: %s", k, v)
		}
	}
}

// ────────────────────────────────────────────────────────────────
// Assertion — Score to Test Result Conversion
// ────────────────────────────────────────────────────────────────

// requirePass fails the test if the result is not a pass.
// The logging already happened in logDAR. This just converts score to test result.
func requirePass(t *testing.T, r schema.AssuranceResult) {
	t.Helper()
	if r.Score != schema.ScorePass {
		t.Fail() // Logging already done - just fail the test
	}
}
