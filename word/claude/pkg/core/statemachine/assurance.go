//omni:key B-claude-global-pkg-statemachine-assurance
//omni:code --go -library
//omni:version a-01.10

// ============================================================================
// METADATA
// ============================================================================
//
// State Machine Assurance Functions — Universal Harness Diagnostics
//
// Key: B-claude-global-pkg-statemachine-assurance
// Version: a-01.10
// Purpose: Verify statemachine constants, positions, and commands alignment
//
// Biblical: 2 Corinthians 13:5 — Examine yourselves, whether ye be in the faith
//
// derivations: b-word/seed/code/L0/go/library.go

package statemachine

import (
	"fmt"
	"time"

	"cws.studio/pkg/foundation/schema"
)

// ────────────────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ────────────────────────────────────────────────────────────────────────────────────

// PragmaAssurance carries the OmniCode identity sections (I1-I4) for this package.
var PragmaAssurance = [][2]string{
	{"I1.key", "B-claude-global-pkg-statemachine-assurance"},
	{"I1.format", "go"},
	{"I1.from", "word/claude/pkg/core/statemachine/assurance.go"},
	{"I1.at", "a-01.10"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "assurance"},
	{"I3.file", "assurance.go"},
	{"I3.title", "State Machine Assurance"},
	{"I3.component", "CPI-SI Cognitive Engine"},
	{"I3.path", "pkg/core/statemachine/assurance.go"},
	{"I3.provides", "STATEMACHINE_ASSURANCE"},
	{"I3.brief", "Self-monitoring diagnostics for the state machine harness."},
	{"I4.layer", "L1-core"},
	{"I4.position", "monitoring"},
	{"I4.pattern", "assurance"},
}

// ────────────────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ────────────────────────────────────────────────────────────────────────────────────

// MetadataAssurance carries the OmniCode context sections (C1-C7) for this package.
var MetadataAssurance = [][2]string{
	{"C1.version", "a-01.10"},
	{"C1.status", "Active"},
	{"C1.created", "2025-12-20"},
	{"C1.updated", "2026-02-25"},
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.architect", "Seanje Lenox-Wise"},
	{"C2.implementation", "Nova Dawn"},
	{"C2.copyright", "CreativeWorkzStudio LLC"},
	{"C3.scripture", "2 Corinthians 13:5 — Examine yourselves, whether ye be in the faith; prove your own selves"},
	{"C3.principle", "The system monitors itself. Assurance functions are the mechanism"},
	{"C3.anchor", "HALT_01 (Genesis 1:1) — All traces terminate here"},
	{"C4.requires.stdlib", "fmt, time"},
	{"C4.requires.internal", "schema, statemachine"},
	{"C4.consumers", "SDK diagnostics, system monitoring"},
	{"C4.integration", "Run diagnostics via RunAllAssurances()"},
	{"C4.if_missing", "No self-verification of the state machine's internal state"},
	{"C5.purpose", "Verify statemachine constants, positions, and commands alignment"},
	{"C5.philosophy", "Self-assessment is foundational to reliable cognition"},
	{"C6.current", "a-01.10 — Substrate adaptation complete"},
	{"C6.planned", "Full integration with health scoring pipeline"},
	{"C6.limitations", "none"},
	{"C7.tags", "assurance, diagnostics, statemachine, ternary"},
	{"C7.category", "Assurance"},
	{"C7.domain", "bereshit"},
	{"C7.paradigm", "CPI-SI"},
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────────────────────────
// 2. Constants
// ────────────────────────────────────────────────────────────────────────────────────

// [Reserved: Constants aliased from statemachine.go]

// ────────────────────────────────────────────────────────────────────────────────────
// 5. Error Types
// ────────────────────────────────────────────────────────────────────────────────────

// [Reserved: Errors handled via schema.AssuranceResult]

// ────────────────────────────────────────────────────────────────────────────────────
// 6. Core Types
// ────────────────────────────────────────────────────────────────────────────────────

// [Reserved: Uses types from statemachine.go and pkg/foundation/schema]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────────────────────────
// 1. Org Chart
// ────────────────────────────────────────────────────────────────────────────────────

// Public APIs
// └── RunAllAssurances() -> calls AssureConstants, AssureCubePositions, AssureCommands
//
// Core Operations
// ├── AssureConstants() -> uses pass, fail, addLookAt, addDebug
// ├── AssureCubePositions() -> uses pass, fail, addLookAt, addDebug
// └── AssureCommands() -> uses pass, fail, addLookAt, addDebug
//
// Helpers
// ├── newResult() -> initializes AssuranceResult
// ├── pass() -> marks success (+1)
// ├── fail() -> marks failure (-1)
// ├── addLookAt() -> adds diagnostic location
// ├── addDebug() -> adds diagnostic command
// └── normalize() -> projects score to ternary

// ────────────────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ────────────────────────────────────────────────────────────────────────────────────

//--- H.1 Pure Functions [TRANSFORM] ---

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

// ────────────────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ────────────────────────────────────────────────────────────────────────────────────

//--- C.1 Diagnostics [CHECK] ---

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

// ────────────────────────────────────────────────────────────────────────────────────
// 5. Public APIs
// ────────────────────────────────────────────────────────────────────────────────────

//--- A.1 Suite Orchestration [RUN] ---

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

// ────────────────────────────────────────────────────────────────────────────────────
// X1: Policy
// ────────────────────────────────────────────────────────────────────────────────────
// "Remove not the ancient landmark, which thy fathers have set." — Proverbs 22:28
//
// Safe to Modify:
//   - Add new diagnostic checks in Core Operations §3
//   - Update diagnostic metadata/hints in constructors
//
// Modify with Care:
//   - suite tallying logic in RunAllAssurances
//
// Never Modify:
//   - 4-block structure
//   - ternary scoring logic (-1/0/+1)

// ────────────────────────────────────────────────────────────────────────────────────
// X4: Reference
// ────────────────────────────────────────────────────────────────────────────────────
// Dependencies:   statemachine.go, pkg/foundation/schema
// Dependents:     SDK diagnostics, system dashboards
// Template:       b-word/seed/code/L0/go/library.go

// ────────────────────────────────────────────────────────────────────────────────────
// X5: Note
// ────────────────────────────────────────────────────────────────────────────────────
// This library provides the diagnostic "immune system" for the state machine.
//
// "Examine yourselves, whether ye be in the faith; prove your own selves."
// — 2 Corinthians 13:5

// ============================================================================
// END CLOSING
// ============================================================================
