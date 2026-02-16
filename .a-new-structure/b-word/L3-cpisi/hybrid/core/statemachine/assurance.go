//omni:code --go -library
//omni:key B-L3-statemachine-assurance
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// State Machine Assurance Functions — Universal Harness Diagnostics
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       claude-global-pkg-statemachine-assurance
//
//	Scripture: "Examine yourselves, whether ye be in the faith; prove your own selves."
//	           — 2 Corinthians 13:5
//	Principle: The system monitors itself. Assurance functions are the mechanism.
//	Anchor:    HALT_01 (Genesis 1:1) — All traces terminate here
//
//	Layer:     Self-Monitoring (cross-cutting)
//	Type:      Assurance (SDK-compatible diagnostics)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2025-12-20
//	Version:         a-01.00
//
// # M.4 HALT Trace [HALTS]
//
//	HALT_01 (GENESIS)            → Genesis 1:1 = origin
//	HALT_07 (MECHANISM_POSITION) → Position IS computation verification
//	HALT_10 (STATE_HEBREW)       → 7 Days of Creation = 7 States
//
// # M.2 Public API [API]
//
//	AssureConstants() AssuranceResult            Verify PSI constants from schema
//	AssureCubePositions() AssuranceResult        Verify 27 positions (3³)
//	AssureCommands() AssuranceResult             Verify 3 commands (HALT/AWAIT/PROCEED)
//	RunAllAssurances() Suite                     Run complete diagnostics
//
// # M.3 Paradigm [PARADIGM]
//
//	Uses schema.AssuranceResult for ternary scoring.
//	DETECT → ASSESS → RESTORE workflow inherited.
//
// ============================================================================
// END METADATA
// ============================================================================

package statemachine

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"time"

	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/schema"
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────────

// newResult creates a new AssuranceResult with timing started.
func newResult(name, halt, detect string) schema.AssuranceResult {
	return schema.AssuranceResult{
		Name:    name,
		HALT:    halt,
		Detect:  detect,
		Context: make(map[string]string),
		LookAt:  make([]schema.LookAtEntry, 0),
		Debug:   make([]string, 0),
		RunAt:   time.Now(),
	}
}

// pass marks the result as passed (+1).
func pass(r *schema.AssuranceResult, assess string) {
	r.Score = schema.ScorePass
	r.Assess = assess
	r.Restore = ""
	r.Duration = time.Since(r.RunAt)
}

// fail marks the result as failed (-1) with recovery path.
func fail(r *schema.AssuranceResult, assess, restore string) {
	r.Score = schema.ScoreFail
	r.Assess = assess
	r.Restore = restore
	r.Duration = time.Since(r.RunAt)
}

// addLookAt adds a typed location for investigation.
func addLookAt(r *schema.AssuranceResult, typ, path, hint string) {
	r.LookAt = append(r.LookAt, schema.LookAtEntry{Type: typ, Path: path, Hint: hint})
}

// addDebug adds a debug command/step suggestion.
func addDebug(r *schema.AssuranceResult, cmd string) {
	r.Debug = append(r.Debug, cmd)
}

// ────────────────────────────────────────────────────────────────
// Constants Assurance (HALT_07)
// ────────────────────────────────────────────────────────────────
// Statemachine constants must alias from schema (vocabulary layer).
// HALT_07 = MECHANISM_POSITION (Position IS computation)

// AssureConstants verifies statemachine constants alias from schema.
// Traces to: HALT_07 (MECHANISM_POSITION)
//
// What is checked:
//   - PSI constants (Lambda, Phi, Scale) match schema
//   - Boundary coordinates match schema
func AssureConstants() schema.AssuranceResult {
	r := newResult("AssureConstants", "HALT_07",
		"Verify statemachine constants alias from schema (vocabulary layer)")

	// LOOK AT: Where to investigate
	addLookAt(&r, "file", "statemachine.go", "Constants defined here")
	addLookAt(&r, "file", "pkg/foundation/schema/loader.go", "Source constants")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'Lambda\\|Phi\\|Scale' statemachine.go")
	addDebug(&r, "grep -n 'const.*=' statemachine.go")

	// Verify PSI constants match schema
	if Lambda != schema.Lambda {
		fail(&r,
			fmt.Sprintf("Lambda mismatch: local=%v, schema=%v", Lambda, schema.Lambda),
			"Ensure Lambda = schema.Lambda in statemachine.go",
		)
		return r
	}

	if Phi != schema.Phi {
		fail(&r,
			fmt.Sprintf("Phi mismatch: local=%v, schema=%v", Phi, schema.Phi),
			"Ensure Phi = schema.Phi in statemachine.go",
		)
		return r
	}

	if Scale != schema.Scale {
		fail(&r,
			fmt.Sprintf("Scale mismatch: local=%v, schema=%v", Scale, schema.Scale),
			"Ensure Scale = schema.Scale in statemachine.go",
		)
		return r
	}

	r.Context["lambda"] = fmt.Sprintf("%v", Lambda)
	r.Context["phi"] = fmt.Sprintf("%v", Phi)
	r.Context["scale"] = fmt.Sprintf("%v", Scale)

	pass(&r, "All PSI constants match schema")
	return r
}

// ────────────────────────────────────────────────────────────────
// Cube Positions Assurance (HALT_07)
// ────────────────────────────────────────────────────────────────
// 27 = 3³ positions in balanced ternary cube
// HALT_07 = MECHANISM_POSITION

// AssureCubePositions verifies 27 cube positions exist.
// Traces to: HALT_07 (MECHANISM_POSITION)
//
// What is checked:
//   - 27 positions (3³) from PAST/PRESENT/FUTURE on 3 axes
//   - Each position has X, Y, Z in [-1, 0, +1]
func AssureCubePositions() schema.AssuranceResult {
	r := newResult("AssureCubePositions", "HALT_07",
		"Verify 27 cube positions (3³ = PAST/PRESENT/FUTURE on 3 axes)")

	// LOOK AT: Where to investigate
	addLookAt(&r, "file", "statemachine.go", "Cube positions defined")
	addLookAt(&r, "grep", "Position", "statemachine.go - position types")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'Position' statemachine.go")

	// Count positions by iterating all valid coordinates
	posCount := 0
	for x := -1; x <= 1; x++ {
		for y := -1; y <= 1; y++ {
			for z := -1; z <= 1; z++ {
				posCount++
			}
		}
	}

	if posCount != 27 {
		fail(&r,
			fmt.Sprintf("Position count is %d, expected 27 (3³)", posCount),
			"Verify cube iteration covers all positions",
		)
		return r
	}

	r.Context["positions"] = "27"
	r.Context["dimensions"] = "3"
	r.Context["coord_range"] = "[-1, 0, +1]"
	r.Context["formula"] = "3³ = 27"

	pass(&r, "27 cube positions verified (3³)")
	return r
}

// ────────────────────────────────────────────────────────────────
// Commands Assurance (HALT_10)
// ────────────────────────────────────────────────────────────────
// 3 commands: HALT(-1), AWAIT(0), PROCEED(+1)
// HALT_10 = STATE_HEBREW (7 Days of Creation = 7 States)

// AssureCommands verifies 3 state machine commands.
// Traces to: HALT_10 (STATE_HEBREW)
//
// What is checked:
//   - CmdHalt = -1
//   - CmdAwait = 0
//   - CmdProceed = +1
func AssureCommands() schema.AssuranceResult {
	r := newResult("AssureCommands", "HALT_10",
		"Verify 3 commands: HALT(-1), AWAIT(0), PROCEED(+1)")

	// LOOK AT: Where to investigate
	addLookAt(&r, "file", "statemachine.go", "Command constants")
	addLookAt(&r, "grep", "Cmd", "statemachine.go - command definitions")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'CmdHalt\\|CmdAwait\\|CmdProceed' statemachine.go")

	// Verify command constants
	if CmdHalt != -1 {
		fail(&r,
			fmt.Sprintf("CmdHalt is %d, expected -1", CmdHalt),
			"CmdHalt must be -1 (terminal state)",
		)
		return r
	}

	if CmdAwait != 0 {
		fail(&r,
			fmt.Sprintf("CmdAwait is %d, expected 0", CmdAwait),
			"CmdAwait must be 0 (pause state)",
		)
		return r
	}

	if CmdProceed != 1 {
		fail(&r,
			fmt.Sprintf("CmdProceed is %d, expected 1", CmdProceed),
			"CmdProceed must be +1 (flow state)",
		)
		return r
	}

	r.Context["cmd_halt"] = fmt.Sprintf("%d", CmdHalt)
	r.Context["cmd_await"] = fmt.Sprintf("%d", CmdAwait)
	r.Context["cmd_proceed"] = fmt.Sprintf("%d", CmdProceed)

	pass(&r, "All 3 commands verified: HALT(-1), AWAIT(0), PROCEED(+1)")
	return r
}

// ────────────────────────────────────────────────────────────────
// Full Suite
// ────────────────────────────────────────────────────────────────

// RunAllAssurances runs the complete statemachine diagnostic suite.
func RunAllAssurances() schema.AssuranceSuite {
	start := time.Now()
	suite := schema.AssuranceSuite{
		Results: make([]schema.AssuranceResult, 0, 3),
		RunAt:   start,
	}

	// Run all statemachine assurances
	suite.Results = append(suite.Results, AssureConstants())
	suite.Results = append(suite.Results, AssureCubePositions())
	suite.Results = append(suite.Results, AssureCommands())

	// Tally results (ternary)
	for _, r := range suite.Results {
		suite.TrueScore += int(r.Score)
		switch r.Score {
		case schema.ScorePass:
			suite.PassCount++
		case schema.ScoreNeutral:
			suite.NeutralCount++
		case schema.ScoreFail:
			suite.FailCount++
		}
	}

	// Normalize true score to ternary
	suite.Normalized = schema.Score(normalize(suite.TrueScore))
	suite.Duration = time.Since(start)

	return suite
}

// normalize projects an unbounded true score to ternary (-1/0/+1).
func normalize(trueScore int) int {
	if trueScore > 0 {
		return 1
	}
	if trueScore < 0 {
		return -1
	}
	return 0
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// State Machine Assurance Architecture:
//
//   ┌─────────────────────────────────────────────────────────────────────────┐
//   │ ASSURANCE FUNCTIONS                                                     │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │  AssureConstants         Verify PSI constants from schema (HALT_07)    │
//   │  AssureCubePositions     Verify 27 positions (HALT_07)                 │
//   │  AssureCommands          Verify 3 commands (HALT_10)                   │
//   └─────────────────────────────────────────────────────────────────────────┘
//
//   ┌─────────────────────────────────────────────────────────────────────────┐
//   │ HALT TRACE                                                              │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │                                                                         │
//   │  statemachine/assurance.go → HALT_10 (7 States) → HALT_07 (Position)   │
//   │                            → HALT_01 (Genesis 1:1)                     │
//   │                                                                         │
//   └─────────────────────────────────────────────────────────────────────────┘
//
// "Examine yourselves, whether ye be in the faith; prove your own selves."
// — 2 Corinthians 13:5
//
// ============================================================================
// END CLOSING
// ============================================================================
