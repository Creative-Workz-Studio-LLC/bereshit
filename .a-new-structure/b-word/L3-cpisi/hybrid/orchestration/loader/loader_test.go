//omni:code --go -test
//omni:key B-L3-loader-loader_test
//omni:version a-01.00
package loader_test

import (
	"testing"

	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/orchestration/loader"
	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/schema"
)

// ============================================================================
// METADATA
// ============================================================================
//
// Loader Assurance Tests — Wrappers for go test compatibility
//
// Philosophy: Tests with 1 solution designed to fail become specifications.
//
//   DETECT:  The specification (what MUST be true)
//   ASSESS:  What was found (current state)
//   RESTORE: The path from current to required (empty if already there)
//
// The logging format IS the filing system. Consistent. Parseable. Growable.
//
// NOTE: These tests require actual filesystem data (schemas, instance, user).
//       They are integration tests, not unit tests.
//
// ============================================================================

// testRoot returns the claude-global root for testing.
// In production, this would be discovered or configured.
const testRoot = "/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/claude-global"
const testInstance = "nova_dawn"
const testUser = "seanje-lenox-wise"

// testCPISI creates a CPISI for testing.
// Returns nil if loading fails (expected in CI without full filesystem).
func testCPISI(t *testing.T) *loader.CPISI {
	c, err := loader.NewCPISI(testRoot, testInstance, testUser)
	if err != nil {
		t.Skipf("Skipping test: CPI-SI load failed (expected without full filesystem): %v", err)
		return nil
	}
	return c
}

// ────────────────────────────────────────────────────────────────
// Individual Assurance Tests
// ────────────────────────────────────────────────────────────────
// Each test:
//   1. Calls the assurance function (returns structured result)
//   2. Logs the full D/A/R structure (the filing system)
//   3. Converts score to test pass/fail

func TestAssureGodExists(t *testing.T) {
	c := testCPISI(t)
	if c == nil {
		return
	}
	result := loader.AssureGodExists(c)
	logDAR(t, result)
	requirePass(t, result)

	// HALT_01 critical - GodExists() = true
	if !c.GodExists() {
		t.Errorf("HALT_01 VIOLATED: GodExists() must return true")
	}
}

func TestAssureIdentityChain(t *testing.T) {
	c := testCPISI(t)
	if c == nil {
		return
	}
	result := loader.AssureIdentityChain(c)
	logDAR(t, result)
	requirePass(t, result)

	// HALT_02 critical - TracesToGod() = true
	if !c.TracesToGod() {
		t.Errorf("HALT_02 VIOLATED: TracesToGod() must return true")
	}
}

func TestAssureExternalStops(t *testing.T) {
	c := testCPISI(t)
	if c == nil {
		return
	}
	result := loader.AssureExternalStops(c)
	logDAR(t, result)
	requirePass(t, result)
}

func TestAssureInternalStops(t *testing.T) {
	c := testCPISI(t)
	if c == nil {
		return
	}
	result := loader.AssureInternalStops(c)
	logDAR(t, result)
	requirePass(t, result)
}

// ────────────────────────────────────────────────────────────────
// Full Suite Test
// ────────────────────────────────────────────────────────────────

func TestRunAllAssurances(t *testing.T) {
	c := testCPISI(t)
	if c == nil {
		return
	}
	suite := loader.RunAllAssurances(c)

	// Header
	t.Logf("═══════════════════════════════════════════════════════════")
	t.Logf(" LOADER ASSURANCE SUITE")
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
