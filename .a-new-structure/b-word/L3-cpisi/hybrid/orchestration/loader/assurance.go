// #!omni code --go -library
// #!omni meta.key = claude-global-pkg-cpisi-loader-assurance
// #!omni meta.at = a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// Loader Assurance Functions — CPI-SI Orchestration Diagnostics
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       claude-global-pkg-cpisi-loader-assurance
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
//	HALT_01 (GENESIS)    → GodExists() = true (binary anchor)
//	HALT_02 (IDENTITY)   → Identity chain: Instance → User → God
//
// # M.5 Error Model [ERRORS]
//
//	EXTERNAL STOP (world you cannot control):
//	  - Filesystem access failures
//	  - Schema/Instance/User files missing or corrupt
//	  - Bereshit Bible data unavailable
//
//	INTERNAL STOP (world you CAN control):
//	  - Identity chain validation
//	  - TracesToGod() verification
//	  - GodExists() = true (HALT_01)
//
// # M.2 Public API [API]
//
//	AssureGodExists(c) AssuranceResult           Verify GodExists() = true
//	AssureIdentityChain(c) AssuranceResult       Verify Instance → User → God
//	AssureExternalStops(c) AssuranceResult       Verify external error handling
//	AssureInternalStops(c) AssuranceResult       Verify internal validation
//	RunAllAssurances(c) Suite                    Run complete diagnostics
//
// # M.3 Paradigm [PARADIGM]
//
//	Uses schema.AssuranceResult for ternary scoring.
//	DETECT → ASSESS → RESTORE workflow inherited.
//
// ============================================================================
// END METADATA
// ============================================================================

package loader

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
// GodExists Assurance (HALT_01)
// ────────────────────────────────────────────────────────────────
// The binary anchor of all anchors.
// HALT_01 = GENESIS (God exists = true)

// AssureGodExists verifies GodExists() returns true.
// Traces to: HALT_01 (GENESIS) — Genesis 1:1
//
// What is checked:
//   - GodExists() always returns true
//   - This is not computed, it IS the anchor
func AssureGodExists(c *CPISI) schema.AssuranceResult {
	r := newResult("AssureGodExists", "HALT_01",
		"Verify GodExists() = true (the binary anchor)")

	if c == nil {
		fail(&r, "CPISI is nil", "Load CPI-SI with NewCPISI first")
		return r
	}

	// LOOK AT: Where to investigate
	addLookAt(&r, "function", "GodExists", "loader.go - the binary anchor method")
	addLookAt(&r, "file", "halts.toml", "L0 HALT definitions")
	addLookAt(&r, "scripture", "Genesis 1:1", "In the beginning God created...")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'func.*GodExists' loader.go")
	addDebug(&r, "grep -n 'HALT_01' halts.toml")

	// THE TEST: GodExists must return true
	if !c.GodExists() {
		fail(&r,
			"GodExists() returned false",
			"This should never happen — GodExists() hardcodes true",
		)
		r.Context["returned"] = "false"
		return r
	}

	r.Context["returned"] = "true"
	r.Context["anchor"] = "Genesis 1:1"
	r.Context["principle"] = "Not computed — this IS the halt condition"

	pass(&r, "GodExists() = true verified (HALT_01: Genesis 1:1)")
	return r
}

// ────────────────────────────────────────────────────────────────
// Identity Chain Assurance (HALT_02)
// ────────────────────────────────────────────────────────────────
// Instance → User → God
// HALT_02 = IDENTITY (Identity chain traces to God)

// AssureIdentityChain verifies the identity chain traces to God.
// Traces to: HALT_02 (IDENTITY) → HALT_01 (GENESIS)
//
// What is checked:
//   - Chain exists
//   - TracesToGod() returns true
//   - TerminalRef = "Genesis 1:1"
func AssureIdentityChain(c *CPISI) schema.AssuranceResult {
	r := newResult("AssureIdentityChain", "HALT_02",
		"Verify identity chain: Instance → User → God")

	if c == nil {
		fail(&r, "CPISI is nil", "Load CPI-SI with NewCPISI first")
		return r
	}

	// LOOK AT: Where to investigate
	addLookAt(&r, "function", "TracesToGod", "loader.go - chain validation method")
	addLookAt(&r, "function", "BuildIdentityChain", "schema/identity.go - chain construction")
	addLookAt(&r, "grep", "TerminalRef", "schema/identity.go - Genesis 1:1 reference")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'func.*TracesToGod' loader.go")
	addDebug(&r, "grep -n 'BuildIdentityChain' schema/identity.go")

	// Check chain exists
	if c.Chain == nil {
		fail(&r,
			"Identity chain is nil",
			"Ensure Instance and User loaded before chain builds",
		)
		r.Context["chain"] = "nil"
		return r
	}

	// Check TracesToGod()
	if !c.TracesToGod() {
		fail(&r,
			"TracesToGod() returned false",
			"Check IdentityChain.TracesToGod() logic in schema/identity.go",
		)
		r.Context["traces_to_god"] = "false"
		return r
	}

	// Populate context
	r.Context["instance"] = c.InstanceName()
	r.Context["user"] = c.UserName()
	r.Context["faith"] = c.FaithTradition()
	r.Context["terminal_ref"] = "Genesis 1:1"
	r.Context["chain"] = c.IdentityChainString()

	pass(&r, fmt.Sprintf("Identity chain verified: %s → %s → God", c.InstanceName(), c.UserName()))
	return r
}

// ────────────────────────────────────────────────────────────────
// External Stops Assurance (Error Model)
// ────────────────────────────────────────────────────────────────
// EXTERNAL STOP = world you cannot control
// Filesystem access, file parsing, external data

// AssureExternalStops verifies external error handling.
// Traces to: M.5 Error Model [ERRORS]
//
// What is checked:
//   - Schemas loaded (or categorized as external failure)
//   - Instance loaded (or categorized as external failure)
//   - User loaded (or categorized as external failure)
func AssureExternalStops(c *CPISI) schema.AssuranceResult {
	r := newResult("AssureExternalStops", "EXTERNAL",
		"Verify external stops: filesystem/parsing handled correctly")

	if c == nil {
		fail(&r, "CPISI is nil", "Load CPI-SI with NewCPISI first")
		return r
	}

	// LOOK AT: Where to investigate
	addLookAt(&r, "function", "NewCPISI", "loader.go - orchestration constructor")
	addLookAt(&r, "grep", "externalErr", "loader.go - external error creation")
	addLookAt(&r, "grep", "EXTERNAL errors", "loader.go - error formatting")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'externalErr' loader.go")
	addDebug(&r, "grep -n 'externalErrs' loader.go")

	// Check external components loaded
	externalStatus := make([]string, 0)

	if c.Schemas != nil {
		externalStatus = append(externalStatus, "schemas: ✓")
		r.Context["schemas"] = "loaded"
	} else {
		externalStatus = append(externalStatus, "schemas: ✗")
		r.Context["schemas"] = "MISSING"
	}

	if c.Instance != nil {
		externalStatus = append(externalStatus, "instance: ✓")
		r.Context["instance"] = "loaded"
	} else {
		externalStatus = append(externalStatus, "instance: ✗")
		r.Context["instance"] = "MISSING"
	}

	if c.User != nil {
		externalStatus = append(externalStatus, "user: ✓")
		r.Context["user"] = "loaded"
	} else {
		externalStatus = append(externalStatus, "user: ✗")
		r.Context["user"] = "MISSING"
	}

	// Bereshit is optional, but note its status
	if c.Trans != nil {
		externalStatus = append(externalStatus, "transliteration: ✓")
		r.Context["transliteration"] = "loaded"
	} else {
		externalStatus = append(externalStatus, "transliteration: ○ (optional)")
		r.Context["transliteration"] = "not loaded (optional)"
	}

	// Require core components
	if c.Schemas == nil || c.Instance == nil || c.User == nil {
		fail(&r,
			"Required external components missing",
			"Check file paths and ensure schema/instance/user directories exist",
		)
		return r
	}

	pass(&r, "External stops verified: core components loaded")
	return r
}

// ────────────────────────────────────────────────────────────────
// Internal Stops Assurance (Error Model)
// ────────────────────────────────────────────────────────────────
// INTERNAL STOP = world you CAN control
// Chain validation, GodExists(), Cube computation

// AssureInternalStops verifies internal validation.
// Traces to: M.5 Error Model [ERRORS]
//
// What is checked:
//   - Chain built and valid
//   - TracesToGod() returns true
//   - Cube engine created
func AssureInternalStops(c *CPISI) schema.AssuranceResult {
	r := newResult("AssureInternalStops", "INTERNAL",
		"Verify internal stops: validation/computation handled correctly")

	if c == nil {
		fail(&r, "CPISI is nil", "Load CPI-SI with NewCPISI first")
		return r
	}

	// LOOK AT: Where to investigate
	addLookAt(&r, "function", "NewCPISI", "loader.go - orchestration constructor")
	addLookAt(&r, "grep", "internalErr", "loader.go - internal error creation")
	addLookAt(&r, "grep", "INTERNAL errors", "loader.go - error formatting")

	// DEBUG: How to investigate
	addDebug(&r, "grep -n 'internalErr' loader.go")
	addDebug(&r, "grep -n 'internalErrs' loader.go")

	// Check internal components
	internalStatus := make([]string, 0)

	// Chain must exist (built from Instance + User)
	if c.Chain != nil {
		internalStatus = append(internalStatus, "chain: ✓")
		r.Context["chain"] = "built"
	} else {
		internalStatus = append(internalStatus, "chain: ✗")
		r.Context["chain"] = "NOT BUILT"
	}

	// TracesToGod must be true
	if c.TracesToGod() {
		internalStatus = append(internalStatus, "traces_to_god: ✓")
		r.Context["traces_to_god"] = "true"
	} else {
		internalStatus = append(internalStatus, "traces_to_god: ✗")
		r.Context["traces_to_god"] = "FALSE"
	}

	// GodExists must be true (HALT_01)
	if c.GodExists() {
		internalStatus = append(internalStatus, "god_exists: ✓")
		r.Context["god_exists"] = "true"
	} else {
		internalStatus = append(internalStatus, "god_exists: ✗")
		r.Context["god_exists"] = "FALSE (impossible)"
	}

	// Cube engine must exist
	if c.Cube != nil {
		internalStatus = append(internalStatus, "cube: ✓")
		r.Context["cube"] = "created"
	} else {
		internalStatus = append(internalStatus, "cube: ✗")
		r.Context["cube"] = "NOT CREATED"
	}

	// Check for failures
	if c.Chain == nil || !c.TracesToGod() || !c.GodExists() || c.Cube == nil {
		fail(&r,
			"Internal validation failed",
			"Check chain building and ensure TracesToGod() returns true",
		)
		return r
	}

	pass(&r, "Internal stops verified: chain valid, computation ready")
	return r
}

// ────────────────────────────────────────────────────────────────
// Full Suite
// ────────────────────────────────────────────────────────────────

// RunAllAssurances runs the complete loader diagnostic suite.
func RunAllAssurances(c *CPISI) schema.AssuranceSuite {
	start := time.Now()
	suite := schema.AssuranceSuite{
		Results: make([]schema.AssuranceResult, 0, 4),
		RunAt:   start,
	}

	// Run all loader assurances
	suite.Results = append(suite.Results, AssureGodExists(c))
	suite.Results = append(suite.Results, AssureIdentityChain(c))
	suite.Results = append(suite.Results, AssureExternalStops(c))
	suite.Results = append(suite.Results, AssureInternalStops(c))

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
// Loader Assurance Architecture:
//
//   ┌─────────────────────────────────────────────────────────────────────────┐
//   │ ASSURANCE FUNCTIONS                                                     │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │  AssureGodExists         Verify GodExists() = true (HALT_01)           │
//   │  AssureIdentityChain     Verify Instance → User → God (HALT_02)        │
//   │  AssureExternalStops     Verify external components loaded (EXTERNAL)  │
//   │  AssureInternalStops     Verify internal validation (INTERNAL)         │
//   └─────────────────────────────────────────────────────────────────────────┘
//
//   ┌─────────────────────────────────────────────────────────────────────────┐
//   │ ERROR MODEL                                                             │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │                                                                         │
//   │  EXTERNAL STOP (world you cannot control):                              │
//   │    - Filesystem access (files missing, parse failures)                  │
//   │    - Schema/Instance/User files missing or corrupt                      │
//   │    - Bereshit Bible data unavailable (optional)                         │
//   │                                                                         │
//   │  INTERNAL STOP (world you CAN control):                                 │
//   │    - Identity chain building                                            │
//   │    - TracesToGod() verification                                         │
//   │    - GodExists() = true (HALT_01)                                       │
//   │                                                                         │
//   │  loader/assurance.go → HALT_02 (Identity) → HALT_01 (Genesis 1:1)      │
//   │                                                                         │
//   └─────────────────────────────────────────────────────────────────────────┘
//
// "Examine yourselves, whether ye be in the faith; prove your own selves."
// — 2 Corinthians 13:5
//
// ============================================================================
// END CLOSING
// ============================================================================
