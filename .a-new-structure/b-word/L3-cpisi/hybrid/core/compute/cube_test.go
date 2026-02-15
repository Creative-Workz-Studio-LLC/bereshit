package compute_test

import (
	"testing"

	"cws.studio/pkg/core/cpisi/compute"
	"cws.studio/pkg/foundation/schema"
)

// ============================================================================
// METADATA
// ============================================================================
//
// Cube Assurance Tests — Wrappers for go test compatibility
//
// Philosophy: Tests with 1 solution designed to fail become specifications.
//
//   DETECT:  The specification (what MUST be true)
//   ASSESS:  What was found (current state)
//   RESTORE: The path from current to required (empty if already there)
//
// The logging format IS the filing system. Consistent. Parseable. Growable.
//
// ============================================================================

// testCube creates a cube for testing (no bereshit or schemas needed for basic tests)
func testCube() *compute.Cube {
	return compute.NewCube(nil, nil)
}

// ────────────────────────────────────────────────────────────────
// Individual Assurance Tests
// ────────────────────────────────────────────────────────────────
// Each test:
//   1. Calls the assurance function (returns structured result)
//   2. Logs the full D/A/R structure (the filing system)
//   3. Converts score to test pass/fail

func TestAssureCubeConstants(t *testing.T) {
	c := testCube()
	result := compute.AssureCubeConstants(c)
	logDAR(t, result)
	requirePass(t, result)

	// HALT_03 critical - Base = 3 (ternary)
	if c.Base != 3 {
		t.Errorf("HALT_03 VIOLATED: Expected Base=3, got %d", c.Base)
	}
	// HALT_05 critical - DataModulo = 243 (3^5)
	if c.DataModulo != 243 {
		t.Errorf("HALT_05 VIOLATED: Expected DataModulo=243, got %d", c.DataModulo)
	}
}

func TestAssureShapeTranslation(t *testing.T) {
	c := testCube()
	result := compute.AssureShapeTranslation(c)
	logDAR(t, result)
	requirePass(t, result)
}

func TestAssureCycleFormula(t *testing.T) {
	c := testCube()
	result := compute.AssureCycleFormula(c)
	logDAR(t, result)
	requirePass(t, result)

	// HALT_09 critical - Formula verification
	// (0 * 3 + 0) % 243 = 0
	if c.Compute(0, compute.PerspectivePresent) != 0 {
		t.Errorf("HALT_09 VIOLATED: Compute(0, Present) should be 0")
	}
}

func TestAssureTriteConversion(t *testing.T) {
	c := testCube()
	result := compute.AssureTriteConversion(c)
	logDAR(t, result)
	requirePass(t, result)
}

func TestAssureSevenDays(t *testing.T) {
	c := testCube()
	result := compute.AssureSevenDays(c)
	logDAR(t, result)
	requirePass(t, result)

	// HALT_10 critical - Origin = Day 4 (SEASONS)
	origin := compute.NewShape(0, 0, 0)
	if origin.CreationDay() != 4 {
		t.Errorf("HALT_10 VIOLATED: Origin should be Day 4 (SEASONS), got %d", origin.CreationDay())
	}
}

// ────────────────────────────────────────────────────────────────
// Full Suite Test
// ────────────────────────────────────────────────────────────────

func TestRunAllAssurances(t *testing.T) {
	c := testCube()
	suite := compute.RunAllAssurances(c)

	// Header
	t.Logf("═══════════════════════════════════════════════════════════")
	t.Logf(" CUBE ASSURANCE SUITE")
	t.Logf("═══════════════════════════════════════════════════════════")

	// Log each result with full D/A/R
	for _, r := range suite.Results {
		logDAR(t, r)
		t.Logf("") // Separator
	}

	// Summary
	t.Logf("───────────────────────────────────────────────────────────")
	t.Logf(" %s", suite.String())
	t.Logf("═══════════════════════════════════════════════════════════")

	// Convert suite result to test pass/fail
	if !suite.AllPass() {
		t.Errorf("Suite failed: %d failures, %d neutral", suite.FailCount, suite.NeutralCount)
	}
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
