// #!omni code --go -library
// #!omni meta.key = claude-global-pkg-cpisi-schema-assurance
// #!omni meta.at = a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// Schema Assurance Functions — Self-Monitoring Diagnostics
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       claude-global-pkg-cpisi-schema-assurance
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
//	HALT_02 (IDENTITY)     → Instance/User loading verification
//	HALT_01 (GENESIS)      → TracesToGod verification
//
// # M.2 Public API [API]
//
//	AssureInstance(root, name) AssuranceResult    Verify instance loads
//	AssureUser(root, name) AssuranceResult        Verify user loads
//	AssureIdentityChain(root, inst, user) Result  Verify chain traces to God
//	AssureSchemas(root) AssuranceResult           Verify L0→L3 loads
//	RunAllAssurances(root, inst, user) Suite      Run complete diagnostics
//
// # M.3 Paradigm [PARADIGM]
//
//	TERNARY ATOMIC SCORING:
//	  -1 = FAIL    (problem detected)
//	   0 = NEUTRAL (cannot determine / needs manual assessment)
//	  +1 = PASS    (healthy)
//
//	DETECT → ASSESS → RESTORE:
//	  Detect:  What was checked (the assertion)
//	  Assess:  What was found (diagnostics)
//	  Restore: What to do if failed (recovery path)
//
// ============================================================================
// END METADATA
// ============================================================================

package schema

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"path/filepath"
	"time"
)

// ────────────────────────────────────────────────────────────────
// Types — Ternary Assurance Score
// ────────────────────────────────────────────────────────────────

// Score is the ternary atomic result: -1 (fail), 0 (neutral), +1 (pass)
type Score int

const (
	ScoreFail    Score = -1 // Problem detected
	ScoreNeutral Score = 0  // Cannot determine / needs assessment
	ScorePass    Score = 1  // Healthy
)

// String returns the ternary score as a string.
func (s Score) String() string {
	switch s {
	case ScoreFail:
		return "FAIL"
	case ScoreNeutral:
		return "NEUTRAL"
	case ScorePass:
		return "PASS"
	default:
		return "UNKNOWN"
	}
}

// Emoji returns the ternary score as an emoji.
func (s Score) Emoji() string {
	switch s {
	case ScoreFail:
		return "✗"
	case ScoreNeutral:
		return "?"
	case ScorePass:
		return "✓"
	default:
		return "?"
	}
}

// ────────────────────────────────────────────────────────────────
// Types — Detect / Assess / Restore
// ────────────────────────────────────────────────────────────────

// LookAtEntry provides typed location for machine and human investigation.
// The Type field tells the machine HOW to interpret Path.
type LookAtEntry struct {
	Type string // "file", "function", "line", "grep", "config", "log"
	Path string // The location (file path, function name, grep pattern)
	Hint string // What to look for there (human-readable)
}

// AssuranceResult is the structured output of an assurance check.
// Follows Detect → Assess → Restore workflow.
//
// Designed for BOTH machine and human consumption:
//   - Machine: Parses LookAt entries by Type, runs Debug commands
//   - Human: Reads Detect/Assess/Restore narrative, follows LookAt hints
type AssuranceResult struct {
	// Identity
	Name string // e.g., "AssureInstance", "AssureIdentityChain"
	HALT string // Which HALT this traces to (e.g., "HALT_02")

	// Ternary Score
	Score Score // -1 (fail), 0 (neutral), +1 (pass)

	// DETECT: What was checked (THE SPECIFICATION)
	Detect string // Description of what was checked

	// ASSESS: What was found
	Assess  string            // Summary of findings
	Context map[string]string // Related context (key-value diagnostics)

	// LOOK AT: Where to investigate (typed for machine parsing)
	LookAt []LookAtEntry // Files, functions, patterns to examine

	// DEBUG: How to investigate further
	Debug []string // Commands, steps, suggestions to try

	// RESTORE: What to do if failed
	Restore string // Recovery path (empty if passed)

	// Timing
	Duration time.Duration
	RunAt    time.Time
}

// AssuranceSuite holds results from multiple assurance checks.
type AssuranceSuite struct {
	Results []AssuranceResult

	// Ternary aggregation
	TrueScore  int   // Sum of all scores (unbounded)
	Normalized Score // Projected to -1/0/+1

	// Counts
	PassCount    int
	NeutralCount int
	FailCount    int

	// Timing
	Duration time.Duration
	RunAt    time.Time
}

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
func newResult(name, halt, detect string) AssuranceResult {
	return AssuranceResult{
		Name:    name,
		HALT:    halt,
		Detect:  detect,
		Context: make(map[string]string),
		LookAt:  make([]LookAtEntry, 0),
		Debug:   make([]string, 0),
		RunAt:   time.Now(),
	}
}

// pass marks the result as passed (+1).
func (r *AssuranceResult) pass(assess string) {
	r.Score = ScorePass
	r.Assess = assess
	r.Restore = "" // No recovery needed
	r.Duration = time.Since(r.RunAt)
}

// fail marks the result as failed (-1) with recovery path.
func (r *AssuranceResult) fail(assess, restore string) {
	r.Score = ScoreFail
	r.Assess = assess
	r.Restore = restore
	r.Duration = time.Since(r.RunAt)
}

// neutral marks the result as neutral (0) - cannot determine.
func (r *AssuranceResult) neutral(assess string) {
	r.Score = ScoreNeutral
	r.Assess = assess
	r.Restore = "Manual assessment required"
	r.Duration = time.Since(r.RunAt)
}

// addLookAt adds a typed location for investigation.
func (r *AssuranceResult) addLookAt(typ, path, hint string) {
	r.LookAt = append(r.LookAt, LookAtEntry{Type: typ, Path: path, Hint: hint})
}

// addDebug adds a debug command/step suggestion.
func (r *AssuranceResult) addDebug(cmd string) {
	r.Debug = append(r.Debug, cmd)
}

// normalize projects an unbounded true score to ternary (-1/0/+1).
func normalize(trueScore int) Score {
	if trueScore > 0 {
		return ScorePass
	}
	if trueScore < 0 {
		return ScoreFail
	}
	return ScoreNeutral
}

// ────────────────────────────────────────────────────────────────
// Instance Assurance (HALT_02)
// ────────────────────────────────────────────────────────────────
// InstanceConfig has 14 sections = 14 STATE layer positions.
// Required for chain: BiblicalFoundation (→HALT_01), Identity (→HALT_02), Covenant (→User→God)

// AssureInstance verifies an instance loads correctly.
// Traces to: HALT_02 (IDENTITY)
//
// Required sections for identity chain:
//   - BiblicalFoundation.Scripture → HALT_01 (Genesis 1:1)
//   - Identity.Name → HALT_02 (who is this instance)
//   - Covenant.Creator → links to User → imago Dei → God
func AssureInstance(root, instanceName string) AssuranceResult {
	r := newResult("AssureInstance", "HALT_02",
		fmt.Sprintf("Verify instance '%s' loads with valid identity (14-section config)", instanceName))
	r.Context["root"] = root
	r.Context["instance"] = instanceName

	instanceDir := filepath.Join(root, "config/instance", instanceName)
	configPath := filepath.Join(instanceDir, "config.jsonc")
	bioPath := filepath.Join(instanceDir, "bio.md")

	// LOOK AT: Where to investigate (typed for machine parsing)
	r.addLookAt("config", configPath, "14-section instance identity (config.jsonc)")
	r.addLookAt("file", bioPath, "Narrative identity (bio.md)")
	r.addLookAt("grep", "biblical_foundation", "Section 1 → HALT_01")
	r.addLookAt("grep", "identity", "Section 2 → HALT_02")
	r.addLookAt("grep", "covenant", "Section 11 → links to User → God")

	// DEBUG: How to investigate
	r.addDebug(fmt.Sprintf("cat %s | jq '.biblical_foundation'", configPath))
	r.addDebug(fmt.Sprintf("cat %s | jq '.identity'", configPath))
	r.addDebug(fmt.Sprintf("cat %s | jq '.covenant'", configPath))
	r.addDebug(fmt.Sprintf("ls -la %s", instanceDir))

	instance, err := LoadInstance(instanceDir, instanceName)
	if err != nil {
		r.fail(
			fmt.Sprintf("LoadInstance failed: %v", err),
			fmt.Sprintf("Check instance config at %s", configPath),
		)
		return r
	}

	// Verify required fields for identity chain
	if instance.Config == nil {
		r.fail("Instance.Config is nil", "Instance config.jsonc may be malformed or missing")
		return r
	}

	// Section 1: BiblicalFoundation → HALT_01 (Genesis 1:1)
	if instance.Config.BiblicalFoundation.Scripture == "" {
		r.fail(
			"BiblicalFoundation.Scripture is empty (cannot trace to HALT_01)",
			"Add biblical_foundation.scripture to config.jsonc",
		)
		return r
	}

	// Section 2: Identity → HALT_02 (who is this instance)
	if instance.Config.Identity.Name == "" {
		r.fail(
			"Identity.Name is empty (HALT_02 requires identity)",
			"Add identity.name to config.jsonc",
		)
		return r
	}

	// Section 11: Covenant → links to User → imago Dei → God
	if instance.Config.Covenant.Creator == "" {
		r.fail(
			"Covenant.Creator is empty (cannot link to User → God)",
			"Add covenant.creator to config.jsonc (name of human creator)",
		)
		return r
	}

	// Populate context for successful load
	r.Context["identity.name"] = instance.Config.Identity.Name
	r.Context["biblical.scripture"] = instance.Config.BiblicalFoundation.Scripture
	r.Context["covenant.creator"] = instance.Config.Covenant.Creator
	r.Context["bio.loaded"] = fmt.Sprintf("%d chars", len(instance.Bio))

	r.pass(fmt.Sprintf("Instance '%s' loaded: Creator=%s, Scripture=%s",
		instance.Config.Identity.Name,
		instance.Config.Covenant.Creator,
		instance.Config.BiblicalFoundation.Scripture))
	return r
}

// ────────────────────────────────────────────────────────────────
// User Assurance (HALT_02)
// ────────────────────────────────────────────────────────────────
// UserConfig represents a human being.
// ALL humans bear the image of God (Genesis 1:27) - this is the theological anchor.
// Faith tradition is CONTEXT, not a gate - no one is excluded from imago Dei.

// AssureUser verifies a user loads correctly.
// Traces to: HALT_02 (IDENTITY) → HALT_01 via imago Dei
//
// Theological foundation:
//   - All humans are made in God's image (Genesis 1:27)
//   - This is NOT a faith requirement - it's ontological reality
//   - Faith tradition provides context but does not gate the chain
func AssureUser(root, userName string) AssuranceResult {
	r := newResult("AssureUser", "HALT_02",
		fmt.Sprintf("Verify user '%s' loads (human made in God's image - Genesis 1:27)", userName))
	r.Context["root"] = root
	r.Context["user"] = userName

	userDir := filepath.Join(root, "config/user", userName)
	configPath := filepath.Join(userDir, "config.jsonc")

	// LOOK AT: Where to investigate (typed for machine parsing)
	r.addLookAt("config", configPath, "User identity configuration")
	r.addLookAt("grep", "identity", "User must have identity section with name")
	r.addLookAt("grep", "faith", "Faith is context, not gate (imago Dei is the anchor)")
	r.addLookAt("grep", "calling", "User's calling/purpose if specified")

	// DEBUG: How to investigate
	r.addDebug(fmt.Sprintf("cat %s | jq '.identity'", configPath))
	r.addDebug(fmt.Sprintf("cat %s | jq '.faith'", configPath))
	r.addDebug(fmt.Sprintf("ls -la %s", userDir))

	user, err := LoadUser(userDir, userName)
	if err != nil {
		r.fail(
			fmt.Sprintf("LoadUser failed: %v", err),
			fmt.Sprintf("Check user config at %s", configPath),
		)
		return r
	}

	// Verify required fields
	if user.Config == nil {
		r.fail("User.Config is nil", "User config.jsonc may be malformed")
		return r
	}
	if user.Config.Identity.Name == "" {
		r.fail("Identity.Name is empty", "Add identity.name to config.jsonc")
		return r
	}

	// Faith tradition is CONTEXT, not a gate
	// All humans bear God's image (Genesis 1:27) - this is the anchor
	// A user with no specified faith still bears imago Dei
	faithContext := user.Config.Faith.Tradition
	if faithContext == "" {
		faithContext = "imago Dei" // Default: human made in God's image
	}

	r.Context["identity.name"] = user.Config.Identity.Name
	r.Context["faith.tradition"] = faithContext
	r.Context["imago_dei"] = "true" // All humans bear God's image
	r.pass(fmt.Sprintf("User '%s' loaded (imago Dei): %s",
		user.Config.Identity.Name, faithContext))
	return r
}

// ────────────────────────────────────────────────────────────────
// Identity Chain Assurance (HALT_01)
// ────────────────────────────────────────────────────────────────
// THE CHAIN: Instance → User (imago Dei) → God (Genesis 1:1)
//
// This is the HALT_01 assurance - the anchor of all anchors.
// The chain traces through:
//   1. Instance has Covenant.Creator (link to human)
//   2. User exists and has Identity.Name (the human)
//   3. User bears imago Dei → traces to God (Genesis 1:27 → Genesis 1:1)
//
// Note: Faith tradition is CONTEXT, not a gate.
// All humans bear God's image - this is the theological anchor.

// AssureIdentityChain verifies the identity chain traces to God.
// Traces to: HALT_01 (ANCHOR_GENESIS)
//
// Chain validation:
//   - Instance.Covenant.Creator must match User.Identity.Name
//   - User bears imago Dei (Genesis 1:27) → traces to HALT_01 (Genesis 1:1)
//   - This chain must be unbroken for identity to be grounded
func AssureIdentityChain(root, instanceName, userName string) AssuranceResult {
	r := newResult("AssureIdentityChain", "HALT_01",
		"Verify identity chain: Instance → User (imago Dei) → God (Genesis 1:1)")
	r.Context["root"] = root
	r.Context["instance"] = instanceName
	r.Context["user"] = userName

	instanceDir := filepath.Join(root, "config/instance", instanceName)
	instanceConfigPath := filepath.Join(instanceDir, "config.jsonc")
	userDir := filepath.Join(root, "config/user", userName)
	userConfigPath := filepath.Join(userDir, "config.jsonc")

	// LOOK AT: Where to investigate (typed for machine parsing)
	r.addLookAt("config", instanceConfigPath, "Instance config - check covenant.creator")
	r.addLookAt("config", userConfigPath, "User config - check identity.name matches")
	r.addLookAt("grep", "covenant.creator", "Must match user identity.name")
	r.addLookAt("grep", "biblical_foundation.scripture", "Should trace to Genesis 1:1")
	r.addLookAt("function", "BuildIdentityChain", "loader.go - chain construction logic")

	// DEBUG: How to investigate
	r.addDebug(fmt.Sprintf("cat %s | jq '.covenant.creator'", instanceConfigPath))
	r.addDebug(fmt.Sprintf("cat %s | jq '.identity.name'", userConfigPath))
	r.addDebug("# Verify: covenant.creator == identity.name (the link)")
	r.addDebug("grep -n 'BuildIdentityChain' loader.go")

	// Load instance
	instance, err := LoadInstance(instanceDir, instanceName)
	if err != nil {
		r.fail(
			fmt.Sprintf("LoadInstance failed: %v", err),
			"Fix instance loading first (run AssureInstance)",
		)
		return r
	}

	// Load user
	user, err := LoadUser(userDir, userName)
	if err != nil {
		r.fail(
			fmt.Sprintf("LoadUser failed: %v", err),
			"Fix user loading first (run AssureUser)",
		)
		return r
	}

	// Build chain
	chain := BuildIdentityChain(instance, user)

	// Populate context for diagnostics
	r.Context["instance.name"] = instance.Config.Identity.Name
	r.Context["instance.covenant.creator"] = instance.Config.Covenant.Creator
	r.Context["user.identity.name"] = user.Config.Identity.Name

	// Verify chain validity (Instance → User link)
	if !chain.Valid {
		r.fail(
			fmt.Sprintf("Identity chain broken: Instance.Covenant.Creator (%s) != User.Identity.Name (%s)",
				instance.Config.Covenant.Creator, user.Config.Identity.Name),
			"Ensure Instance.Covenant.Creator matches User.Identity.Name exactly",
		)
		r.Context["chain.valid"] = "false"
		return r
	}

	// THE CRITICAL CHECK: Does it trace to God?
	// Chain: Instance → User (imago Dei) → God
	// All humans bear imago Dei (Genesis 1:27) → traces to God (Genesis 1:1)
	if !chain.TracesToGod() {
		r.fail(
			"Identity chain does NOT trace to God (HALT_01 violated)",
			"Chain broken: Instance.Covenant.Creator must match User.Identity.Name",
		)
		r.Context["traces_to_god"] = "false"
		return r
	}

	r.Context["chain.valid"] = "true"
	r.Context["traces_to_god"] = "true"
	r.Context["chain.summary"] = chain.String()
	r.Context["faith.context"] = chain.FaithTrace
	r.pass(fmt.Sprintf("Identity chain verified: %s → %s (imago Dei) → God",
		instance.Config.Identity.Name, user.Config.Identity.Name))
	return r
}

// ────────────────────────────────────────────────────────────────
// Schema Loading Assurance (L0→L3)
// ────────────────────────────────────────────────────────────────
// Schema layers map to CPI-SI architecture:
//   L0: HALTs (foundation - HALT_01 = Genesis 1:1)
//   L1: Mini State Machine (init, state, task)
//   L2: Operations (cube geometry - 27 positions)
//   L3: Communication (encryption)
//
// All layers must load for the schema system to function.

// AssureSchemas verifies all schema layers load correctly.
// Traces to: HALT_02 (IDENTITY) → L0→L1→L2→L3
//
// Layer structure:
//   - L0 (config/l0/): halts.toml - Foundation anchors including HALT_01
//   - L1 (config/l1/): init, state, task schemas - Mini state machine
//   - L2 (config/l2/): cube.schema.toml - 27-position geometry
//   - L3 (config/l3/): encryption.schema.toml - Communication encoding
func AssureSchemas(schemaDir string) AssuranceResult {
	r := newResult("AssureSchemas", "HALT_02",
		"Verify schema layers load: L0 (halts) → L1 (state machine) → L2 (cube) → L3 (encryption)")
	r.Context["schema_dir"] = schemaDir

	// LOOK AT: Where to investigate (typed for machine parsing)
	r.addLookAt("file", filepath.Join(schemaDir, "config/l0/halts.toml"), "L0 Foundation - HALT_01 lives here")
	r.addLookAt("file", filepath.Join(schemaDir, "config/l1/init.schema.toml"), "L1 Init schema")
	r.addLookAt("file", filepath.Join(schemaDir, "config/l1/state.schema.toml"), "L1 State schema (27 positions)")
	r.addLookAt("file", filepath.Join(schemaDir, "config/l1/task.schema.toml"), "L1 Task schema")
	r.addLookAt("file", filepath.Join(schemaDir, "config/l2/cube.schema.toml"), "L2 Cube geometry")
	r.addLookAt("file", filepath.Join(schemaDir, "config/l3/encryption.schema.toml"), "L3 Encryption")

	// DEBUG: How to investigate
	r.addDebug(fmt.Sprintf("ls -la %s/config/l*/", schemaDir))
	r.addDebug(fmt.Sprintf("cat %s/config/l0/halts.toml | head -20", schemaDir))
	r.addDebug("make verify-config  # Makefile target")

	schemas, err := LoadSchemas(schemaDir)
	if err != nil {
		r.fail(
			fmt.Sprintf("LoadSchemas failed: %v", err),
			fmt.Sprintf("Check schema files in %s/config/l0, l1, l2, l3 directories", schemaDir),
		)
		return r
	}

	// Verify L0 (HALTs) - Foundation layer with HALT_01
	if schemas.Halts == nil {
		r.fail("L0 Halts not loaded", "Check config/l0/halts.toml exists and is valid TOML")
		return r
	}
	r.Context["l0.halts"] = "loaded"

	// Verify L1 (Mini State Machine)
	if schemas.Init == nil {
		r.fail("L1 Init schema not loaded", "Check config/l1/init.schema.toml")
		return r
	}
	r.Context["l1.init"] = "loaded"

	if schemas.State == nil {
		r.fail("L1 State schema not loaded", "Check config/l1/state.schema.toml")
		return r
	}
	r.Context["l1.state"] = "loaded"

	if schemas.Task == nil {
		r.fail("L1 Task schema not loaded", "Check config/l1/task.schema.toml")
		return r
	}
	r.Context["l1.task"] = "loaded"

	// Verify L2 (Cube) - 27-position geometry
	if schemas.Cube == nil {
		r.fail("L2 Cube schema not loaded", "Check config/l2/cube.schema.toml")
		return r
	}
	r.Context["l2.cube"] = "loaded"

	// Verify L3 (Encrypt) - Communication layer
	if schemas.Encrypt == nil {
		r.fail("L3 Encrypt schema not loaded", "Check config/l3/encryption.schema.toml")
		return r
	}
	r.Context["l3.encrypt"] = "loaded"

	r.pass("All schema layers loaded: L0→L1→L2→L3")
	return r
}

// ────────────────────────────────────────────────────────────────
// Full Suite
// ────────────────────────────────────────────────────────────────

// RunAllAssurances runs the complete diagnostic suite.
// This is what the self-monitoring system calls.
func RunAllAssurances(root, instanceName, userName string) AssuranceSuite {
	start := time.Now()
	suite := AssuranceSuite{
		Results: make([]AssuranceResult, 0, 4),
		RunAt:   start,
	}

	// Run all assurances
	schemaDir := filepath.Join(root, "pkg/foundation/schema")

	suite.Results = append(suite.Results, AssureSchemas(schemaDir))
	suite.Results = append(suite.Results, AssureInstance(root, instanceName))
	suite.Results = append(suite.Results, AssureUser(root, userName))
	suite.Results = append(suite.Results, AssureIdentityChain(root, instanceName, userName))

	// Tally results (ternary)
	for _, r := range suite.Results {
		suite.TrueScore += int(r.Score)
		switch r.Score {
		case ScorePass:
			suite.PassCount++
		case ScoreNeutral:
			suite.NeutralCount++
		case ScoreFail:
			suite.FailCount++
		}
	}

	// Normalize true score to ternary
	suite.Normalized = normalize(suite.TrueScore)
	suite.Duration = time.Since(start)

	return suite
}

// ────────────────────────────────────────────────────────────────
// Result Methods
// ────────────────────────────────────────────────────────────────

// String returns a human-readable summary of the result.
func (r AssuranceResult) String() string {
	return fmt.Sprintf("[%s] %s %s (%s): %s",
		r.HALT, r.Score.Emoji(), r.Name, r.Score.String(), r.Assess)
}

// RecoveryNeeded returns true if this result requires recovery.
func (r AssuranceResult) RecoveryNeeded() bool {
	return r.Score == ScoreFail
}

// String returns a human-readable summary of the suite.
func (s AssuranceSuite) String() string {
	return fmt.Sprintf("Assurance Suite: %s (true=%d, pass=%d, neutral=%d, fail=%d) in %v",
		s.Normalized.String(), s.TrueScore, s.PassCount, s.NeutralCount, s.FailCount, s.Duration)
}

// RecoveryNeeded returns how many assurances need recovery.
func (s AssuranceSuite) RecoveryNeeded() int {
	return s.FailCount
}

// AllPass returns true if all assurances passed (+1).
func (s AssuranceSuite) AllPass() bool {
	return s.FailCount == 0 && s.NeutralCount == 0
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Self-Monitoring Architecture — Detect / Assess / Restore:
//
//   ┌─────────────────────────────────────────────────────────────────────────┐
//   │ TERNARY ATOMIC SCORING                                                  │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │  -1 = FAIL     Problem detected, recovery path provided                 │
//   │   0 = NEUTRAL  Cannot determine, manual assessment required             │
//   │  +1 = PASS     Healthy, no action needed                                │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │ TRUE SCORE     Unbounded sum of all atomic scores                       │
//   │ NORMALIZED     Projected to -1/0/+1 for quick status                    │
//   └─────────────────────────────────────────────────────────────────────────┘
//
//   ┌─────────────────────────────────────────────────────────────────────────┐
//   │ DETECT → ASSESS → RESTORE                                               │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │  DETECT:   What was checked (the assertion)                             │
//   │  ASSESS:   What was found (diagnostics, details)                        │
//   │  RESTORE:  What to do if failed (recovery path)                         │
//   └─────────────────────────────────────────────────────────────────────────┘
//
//   ┌─────────────────────────────────────────────────────────────────────────┐
//   │ USAGE                                                                   │
//   ├─────────────────────────────────────────────────────────────────────────┤
//   │  SDK:          result := schema.AssureIdentityChain(root, inst, user)  │
//   │  Self-Monitor: suite := schema.RunAllAssurances(root, inst, user)      │
//   │  go test:      Uses assurance functions via loader_test.go wrapper     │
//   │  CLI:          cpisi-health --assure-all                               │
//   └─────────────────────────────────────────────────────────────────────────┘
//
// "Examine yourselves, whether ye be in the faith; prove your own selves."
// — 2 Corinthians 13:5
//
// ============================================================================
// END CLOSING
// ============================================================================
