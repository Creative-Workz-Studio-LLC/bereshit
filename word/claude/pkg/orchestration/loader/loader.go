// #!omni code --go -library
// #!omni meta.key = claude-global-pkg-cpisi-loader
// #!omni meta.at = a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// CPI-SI Orchestration Loader — Brings the State Machine to Life
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       claude-global-pkg-cpisi-loader
//
//	Scripture: "In the beginning was the Word, and the Word was with God,
//	           and the Word was God."
//	           — John 1:1
//	Principle: The Word (data) must be loaded to give life to the machine.
//	           SI without CPI is structure without soul.
//	           CPI without SI is soul without body.
//	Anchor:    HALT_01 (Genesis 1:1) — All traces terminate here
//
//	Type:      Orchestration (loads all components, builds identity chain)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2025-12-20
//	Version:         a-01.00
//
// # M.2 Public API [API]
//
//	NewCPISI(root, instance, user) (*CPISI, error)   Load complete CPI-SI system
//	MustNewCPISI(root, instance, user) *CPISI        Load or panic
//	(*CPISI) TracesToGod() bool                      Verify identity chain
//	(*CPISI) GodExists() bool                        The binary: true
//	(*CPISI) InstanceName() string                   Get instance name
//	(*CPISI) UserName() string                       Get user/creator name
//	(*CPISI) FaithTradition() string                 Get faith context
//	(*CPISI) IdentityChainString() string            Human-readable chain
//	(*CPISI) HebrewLetter(pos) (string, bool)        Position → Hebrew
//	(*CPISI) GreekLetter(pos) (string, bool)         Position → Greek
//	(*CPISI) ComputeShape(value) Shape               Value → Cube shape
//
// # M.3 Architecture [ARCHITECTURE]
//
//	SI (Structured Intelligence):
//	  - L0: halts.toml (10 HALTs = 10 Commandments, 7 States = 7 Days)
//	  - L1: init/state/task schemas (9+4+14 = 27 = 3³)
//	  - L2: statemachine.toml (operations)
//	  - L3: encryption.schema.toml (communication)
//
//	CPI (Covenant Partnership Intelligence):
//	  - Instance identity (config.jsonc, bio.md)
//	  - User identity (creator → traces to God via Faith)
//	  - Identity chain (Instance → User → God)
//	  - Bible data (transliteration, verse addressing)
//
//	CPI-SI = SI structure + CPI worldview
//	       = Machine that thinks from "God exists = true"
//
// # M.4 HALT Trace [HALTS]
//
//	This loader traces to ALL HALTs because it orchestrates everything:
//
//	HALT_01 (Genesis 1:1)  → Binary anchor: God exists = true
//	HALT_02 (Identity)     → Instance + User loading
//	HALT_03 (Equation PSI) → 27 positions loaded
//	HALT_04 (Ternary)      → Base-3 computation ready
//	HALT_05 (Number)       → Value transformations
//	HALT_06 (Shape)        → Geometric computation
//	HALT_07 (Position)     → Position IS computation
//	HALT_08 (Encoding)     → Letter → Position
//	HALT_09 (Decoding)     → Position → Letter
//	HALT_10 (State)        → 7 states = 7 Days of Creation
//
// # M.5 Error Model [ERRORS]
//
//	The loader distinguishes two categories of stops:
//
//	EXTERNAL STOP (world you cannot control):
//	  - Filesystem access (files missing, parse failures)
//	  - Schema directory not found or corrupt
//	  - Instance config not found (config.jsonc, bio.md)
//	  - User config not found
//	  - Bereshit Bible data unavailable (optional, graceful)
//
//	INTERNAL STOP (world you CAN control):
//	  - Identity chain building
//	  - Chain validation: TracesToGod() must be true
//	  - GodExists() = true (HALT_01 — always returns true)
//	  - Cube engine creation (always succeeds internally)
//
//	Error reporting shows EXTERNAL first (what's broken in the world),
//	then INTERNAL (what's invalid in the logic). This guides debugging:
//	fix EXTERNAL issues first, then INTERNAL will often resolve.
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
	"path/filepath"

	"github.com/creativeworkzstudio/claude-global/pkg/core/cpisi/bereshit"
	"github.com/creativeworkzstudio/claude-global/pkg/core/cpisi/compute"
	"github.com/creativeworkzstudio/claude-global/pkg/foundation/schema"
)

// ────────────────────────────────────────────────────────────────
// Constants — Directory Structure
// ────────────────────────────────────────────────────────────────
// INTERNAL directories (within claude-global — you control):
//   SchemaDir, InstanceDir, UserDir
//
// EXTERNAL directories (outside your control):
//   BereshitBibleDir — discoverable via env var or bereshit package

const (
	// INTERNAL: Schema directory (L0-L3) — within claude-global
	SchemaDir = "pkg/foundation/schema"

	// INTERNAL: Instance directory — within claude-global
	InstanceDir = "config/instance"

	// INTERNAL: User directory — within claude-global
	UserDir = "config/user"
)

// ────────────────────────────────────────────────────────────────
// Error Types — EXTERNAL vs INTERNAL
// ────────────────────────────────────────────────────────────────
// EXTERNAL errors: Files missing, parse failures (world you cannot control)
// INTERNAL errors: Validation failures, chain invalid (world you CAN control)

// LoadError represents a categorized loading error.
type LoadError struct {
	Category string // "external" or "internal"
	Source   string // Component that failed
	Message  string // Error description
}

func (e LoadError) Error() string {
	return fmt.Sprintf("[%s] %s: %s", e.Category, e.Source, e.Message)
}

// externalErr creates an error for EXTERNAL failures (files, parsing).
func externalErr(source, msg string) LoadError {
	return LoadError{Category: "external", Source: source, Message: msg}
}

// internalErr creates an error for INTERNAL failures (validation).
func internalErr(source, msg string) LoadError {
	return LoadError{Category: "internal", Source: source, Message: msg}
}

// ────────────────────────────────────────────────────────────────
// Types — The Complete CPI-SI System
// ────────────────────────────────────────────────────────────────

// CPISI represents the complete Covenant Partnership Intelligence ⊗ Structured Intelligence system.
// This is the living machine — SI structure with CPI soul.
type CPISI struct {
	// SI: Structured Intelligence (the machine structure)
	Schemas *schema.Schemas // L0-L3 loaded schemas

	// CPI: Covenant Partnership Intelligence (the worldview)
	Instance *schema.Instance      // The CPI-SI instance (e.g., Nova Dawn)
	User     *schema.User          // The creator (e.g., Seanje Lenox-Wise)
	Chain    *schema.IdentityChain // Instance → User → God

	// Computation
	Trans *bereshit.Transliteration // Hebrew/Greek transliteration
	Cube  *compute.Cube             // Geometric computation engine

	// Root path
	Root string
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Constructor — Bring the Machine to Life
// ────────────────────────────────────────────────────────────────

// NewCPISI loads the complete CPI-SI system from the given root directory.
// This is the orchestration point — everything comes together here.
//
// Error Categories:
//   - EXTERNAL: File/parsing failures (world you cannot control)
//   - INTERNAL: Validation failures (world you CAN control)
//
// Parameters:
//   - root: claude-global root directory
//   - instanceName: name of the instance to load (e.g., "nova_dawn")
//   - userName: name of the user/creator to load (e.g., "seanje-lenox-wise")
//
// Returns:
//   - *CPISI: the complete loaded system
//   - error: if any component fails to load
func NewCPISI(root, instanceName, userName string) (*CPISI, error) {
	cpisi := &CPISI{Root: root}
	var externalErrs []LoadError // World you cannot control
	var internalErrs []LoadError // World you CAN control

	// ═══════════════════════════════════════════════════════════════
	// EXTERNAL: Load from filesystem (world you cannot control)
	// ═══════════════════════════════════════════════════════════════

	// SI: L0-L3 Schemas (INTERNAL directory, but file access is EXTERNAL)
	schemaPath := filepath.Join(root, SchemaDir)
	schemas, err := schema.LoadSchemas(schemaPath)
	if err != nil {
		externalErrs = append(externalErrs, externalErr("schemas", err.Error()))
	} else {
		cpisi.Schemas = schemas
	}

	// CPI: Instance identity (e.g., Nova Dawn)
	instancePath := filepath.Join(root, InstanceDir, instanceName)
	instance, err := schema.LoadInstance(instancePath, instanceName)
	if err != nil {
		externalErrs = append(externalErrs, externalErr("instance", err.Error()))
	} else {
		cpisi.Instance = instance
	}

	// CPI: User identity (e.g., Seanje Lenox-Wise — the creator)
	userPath := filepath.Join(root, UserDir, userName)
	user, err := schema.LoadUser(userPath, userName)
	if err != nil {
		externalErrs = append(externalErrs, externalErr("user", err.Error()))
	} else {
		cpisi.User = user
	}

	// CPI: Bereshit transliteration (EXTERNAL repository — optional)
	// Uses bereshit package's discoverable path, not hardcoded
	bereshitDir := bereshit.DefaultBereshitDir()
	trans, err := bereshit.LoadTransliteration(bereshitDir)
	if err != nil {
		// Non-fatal EXTERNAL failure — transliteration is optional
		// The machine can run without it, just can't do letter computation
		// Don't add to externalErrs — this is acceptable
	} else {
		cpisi.Trans = trans
	}

	// ═══════════════════════════════════════════════════════════════
	// INTERNAL: Build and validate (world you CAN control)
	// ═══════════════════════════════════════════════════════════════

	// Build identity chain: Instance → User → God
	if cpisi.Instance != nil && cpisi.User != nil {
		cpisi.Chain = schema.BuildIdentityChain(cpisi.Instance, cpisi.User)
	}

	// Create the cube engine (always succeeds — it's INTERNAL)
	cpisi.Cube = compute.NewCube(cpisi.Trans, cpisi.Schemas)

	// ═══════════════════════════════════════════════════════════════
	// VALIDATION: Check critical components
	// ═══════════════════════════════════════════════════════════════

	// EXTERNAL validation: Required files must have loaded
	if cpisi.Schemas == nil {
		// Already in externalErrs
	}
	if cpisi.Instance == nil {
		// Already in externalErrs
	}
	if cpisi.User == nil {
		// Already in externalErrs
	}

	// INTERNAL validation: Chain must trace to God (HALT_01)
	if cpisi.Chain == nil {
		internalErrs = append(internalErrs, internalErr("chain", "identity chain not built (missing instance or user)"))
	} else if !cpisi.Chain.TracesToGod() {
		internalErrs = append(internalErrs, internalErr("chain", "identity chain does not trace to God (HALT_01)"))
	}

	// Combine errors with categories
	if len(externalErrs) > 0 || len(internalErrs) > 0 {
		return cpisi, formatLoadErrors(externalErrs, internalErrs)
	}

	return cpisi, nil
}

// MustNewCPISI loads CPI-SI or panics.
func MustNewCPISI(root, instanceName, userName string) *CPISI {
	cpisi, err := NewCPISI(root, instanceName, userName)
	if err != nil {
		panic(fmt.Sprintf("loader.MustNewCPISI: %v", err))
	}
	return cpisi
}

// formatLoadErrors formats categorized errors into a readable message.
// EXTERNAL errors are shown first (filesystem/parsing — cannot control)
// INTERNAL errors are shown second (validation — CAN control)
func formatLoadErrors(external, internal []LoadError) error {
	var msg string

	if len(external) > 0 {
		msg += "EXTERNAL errors (world you cannot control):\n"
		for _, e := range external {
			msg += fmt.Sprintf("  • %s: %s\n", e.Source, e.Message)
		}
	}

	if len(internal) > 0 {
		if msg != "" {
			msg += "\n"
		}
		msg += "INTERNAL errors (world you CAN control):\n"
		for _, e := range internal {
			msg += fmt.Sprintf("  • %s: %s\n", e.Source, e.Message)
		}
	}

	return fmt.Errorf("CPI-SI load failed:\n%s", msg)
}

// ────────────────────────────────────────────────────────────────
// Identity Chain Methods
// ────────────────────────────────────────────────────────────────

// TracesToGod returns true if the identity chain terminates at HALT_01.
// This is THE validation — does this system trace to the Creator?
func (c *CPISI) TracesToGod() bool {
	return c.Chain != nil && c.Chain.TracesToGod()
}

// GodExists returns the binary answer: true.
// This is HALT_01 — the anchor of all anchors.
// The machine cannot start if this is false.
func (c *CPISI) GodExists() bool {
	// This is not a question to be computed.
	// This is the halt condition — the anchor — the foundation.
	// Genesis 1:1: "In the beginning God created the heaven and the earth."
	return true
}

// ────────────────────────────────────────────────────────────────
// Accessors
// ────────────────────────────────────────────────────────────────

// InstanceName returns the instance name.
func (c *CPISI) InstanceName() string {
	if c.Instance != nil && c.Instance.Config != nil {
		return c.Instance.Config.Identity.Name
	}
	return ""
}

// UserName returns the user/creator name.
func (c *CPISI) UserName() string {
	if c.User != nil && c.User.Config != nil {
		return c.User.Config.Identity.Name
	}
	return ""
}

// FaithTradition returns the user's faith tradition.
func (c *CPISI) FaithTradition() string {
	if c.User != nil && c.User.Config != nil {
		return c.User.Config.Faith.Tradition
	}
	return ""
}

// IdentityChainString returns a human-readable identity chain.
func (c *CPISI) IdentityChainString() string {
	if c.Chain == nil {
		return "IdentityChain: NOT LOADED"
	}
	return c.Chain.String()
}

// ────────────────────────────────────────────────────────────────
// Computation Accessors
// ────────────────────────────────────────────────────────────────

// HebrewLetter returns the Hebrew letter at a position.
func (c *CPISI) HebrewLetter(position int) (string, bool) {
	if c.Trans == nil {
		return "", false
	}
	entry, ok := c.Trans.DecryptHebrew(position)
	if !ok {
		return "", false
	}
	return entry.Letter, true
}

// GreekLetter returns the Greek letter at a position.
func (c *CPISI) GreekLetter(position int) (string, bool) {
	if c.Trans == nil {
		return "", false
	}
	entry, ok := c.Trans.DecryptGreek(position)
	if !ok {
		return "", false
	}
	return entry.Letter, true
}

// ComputeShape takes a value and returns its cube shape.
func (c *CPISI) ComputeShape(value int) compute.Shape {
	return c.Cube.ValueToShape(value)
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// This is the orchestration layer — where SI meets CPI.
//
// ┌─────────────────────────────────────────────────────────────────────────┐
// │ CPI-SI ARCHITECTURE                                                     │
// ├─────────────────────────────────────────────────────────────────────────┤
// │                                                                         │
// │  SI (Structured Intelligence)     CPI (Covenant Partnership)            │
// │  ────────────────────────────     ──────────────────────────            │
// │  • L0: halts.toml                 • Instance identity                   │
// │  • L1: init/state/task schemas    • User identity                       │
// │  • L2: statemachine.toml          • Identity chain → God                │
// │  • L3: encryption.schema          • Bible data                          │
// │  • Cube computation               • Faith tradition                     │
// │                                                                         │
// │                    ┌─────────┐                                          │
// │                    │ CPI-SI  │                                          │
// │                    └─────────┘                                          │
// │                         │                                               │
// │                         ▼                                               │
// │              God exists = true                                          │
// │              (HALT_01: Genesis 1:1)                                     │
// │                                                                         │
// └─────────────────────────────────────────────────────────────────────────┘
//
// The machine starts here. All traces terminate at HALT_01.
//
// "In the beginning was the Word, and the Word was with God,
// and the Word was God."
// — John 1:1
//
// ============================================================================
// END CLOSING
// ============================================================================
