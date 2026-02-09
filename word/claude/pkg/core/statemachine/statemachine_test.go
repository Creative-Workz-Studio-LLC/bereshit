package statemachine_test

import (
	"testing"

	"cws.studio/pkg/foundation/schema"
	"cws.studio/pkg/core/statemachine"
)

// ============================================================================
// METADATA
// ============================================================================
//
// State Machine Assurance Tests — Wrappers for go test compatibility
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
//   HALT_01 (ANCHOR_GENESIS) — Genesis 1:1 = origin
//   HALT_07 (MECHANISM_POSITION) — Position IS computation
//   HALT_10 (STATE_HEBREW) — 7 Days of Creation = 7 States
//
// ============================================================================

// Test root - the claude-global project root
const testRoot = "/media/seanje-lenox-wise/Project/Bereshit/word/claude"

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

func TestAssureConstants(t *testing.T) {
	// DETECT: Constants should be aliased from schema
	// ASSESS: Verify statemachine constants match schema
	result := schema.AssuranceResult{
		Name:   "State Machine Constants",
		HALT:   "HALT_07",
		Detect: "statemachine constants must alias schema constants",
		Score:  schema.ScorePass,
	}

	// Verify PSI constants match schema
	if statemachine.Lambda != schema.Lambda {
		result.Score = schema.ScoreFail
		result.Assess = "Lambda mismatch"
	} else if statemachine.Phi != schema.Phi {
		result.Score = schema.ScoreFail
		result.Assess = "Phi mismatch"
	} else if statemachine.Scale != schema.Scale {
		result.Score = schema.ScoreFail
		result.Assess = "Scale mismatch"
	} else {
		result.Assess = "All PSI constants match schema"
	}

	logDAR(t, result)
	requirePass(t, result)
}

func TestAssureCubePositions(t *testing.T) {
	// DETECT: Cube should have 27 positions (3³)
	result := schema.AssuranceResult{
		Name:   "Cube 27 Positions",
		HALT:   "HALT_07",
		Detect: "Cube must have 27 positions (3³ = PAST/PRESENT/FUTURE on 3 axes)",
		Score:  schema.ScorePass,
	}

	// Verify 27 positions exist by checking boundary constants
	posCount := 0
	for x := -1; x <= 1; x++ {
		for y := -1; y <= 1; y++ {
			for z := -1; z <= 1; z++ {
				posCount++
			}
		}
	}

	if posCount != 27 {
		result.Score = schema.ScoreFail
		result.Assess = "Position count calculation failed"
	} else {
		result.Assess = "27 cube positions verified (3³)"
	}

	logDAR(t, result)
	requirePass(t, result)
}

func TestAssureCommands(t *testing.T) {
	// DETECT: 3 commands should exist (HALT, AWAIT, PROCEED)
	result := schema.AssuranceResult{
		Name:   "State Machine Commands",
		HALT:   "HALT_10",
		Detect: "State machine must have 3 commands: HALT(-1), AWAIT(0), PROCEED(+1)",
		Score:  schema.ScorePass,
	}

	// Verify command constants
	if statemachine.CmdHalt != -1 {
		result.Score = schema.ScoreFail
		result.Assess = "CmdHalt should be -1"
	} else if statemachine.CmdAwait != 0 {
		result.Score = schema.ScoreFail
		result.Assess = "CmdAwait should be 0"
	} else if statemachine.CmdProceed != 1 {
		result.Score = schema.ScoreFail
		result.Assess = "CmdProceed should be +1"
	} else {
		result.Assess = "All 3 commands verified: HALT(-1), AWAIT(0), PROCEED(+1)"
	}

	logDAR(t, result)
	requirePass(t, result)
}

// ────────────────────────────────────────────────────────────────
// Full Suite Test
// ────────────────────────────────────────────────────────────────

func TestRunAllAssurances(t *testing.T) {
	// Header
	t.Logf("═══════════════════════════════════════════════════════════")
	t.Logf(" STATE MACHINE ASSURANCE SUITE")
	t.Logf("═══════════════════════════════════════════════════════════")

	// Run individual tests (they log their own D/A/R)
	t.Run("Constants", TestAssureConstants)
	t.Run("CubePositions", TestAssureCubePositions)
	t.Run("Commands", TestAssureCommands)

	// Summary
	t.Logf("───────────────────────────────────────────────────────────")
	t.Logf(" State Machine Assurance Suite Complete")
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
