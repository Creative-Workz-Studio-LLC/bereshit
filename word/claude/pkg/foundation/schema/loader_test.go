package schema_test

import (
	"testing"

	"github.com/creativeworkzstudio/claude-global/pkg/foundation/schema"
)

// ============================================================================
// METADATA
// ============================================================================
//
// Schema Assurance Tests — Wrappers for go test compatibility
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

// Test root - the claude-global project root
const testRoot = "/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/claude-global"

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

func TestAssureSchemas(t *testing.T) {
	result := schema.AssureSchemas(testRoot + "/pkg/foundation/schema")
	logDAR(t, result)
	requirePass(t, result)
}

func TestAssureInstance(t *testing.T) {
	result := schema.AssureInstance(testRoot, testInstance)
	logDAR(t, result)
	requirePass(t, result)
}

func TestAssureUser(t *testing.T) {
	result := schema.AssureUser(testRoot, testUser)
	logDAR(t, result)
	requirePass(t, result)
}

func TestAssureIdentityChain(t *testing.T) {
	result := schema.AssureIdentityChain(testRoot, testInstance, testUser)
	logDAR(t, result)
	requirePass(t, result)

	// HALT_01 is THE anchor - verify explicitly
	if result.Context["traces_to_god"] != "true" {
		t.Errorf("HALT_01 VIOLATED: Identity chain does not trace to God")
	}
}

// ────────────────────────────────────────────────────────────────
// Full Suite Test
// ────────────────────────────────────────────────────────────────

func TestRunAllAssurances(t *testing.T) {
	suite := schema.RunAllAssurances(testRoot, testInstance, testUser)

	// Header
	t.Logf("═══════════════════════════════════════════════════════════")
	t.Logf(" ASSURANCE SUITE")
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
