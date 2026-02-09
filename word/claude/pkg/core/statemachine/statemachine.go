// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-statemachine
// Purpose: CPI-SI State Machine - 27 positions, 3 commands, 9 operators
// Biblical: Genesis 1:1-2 - "formless and void" → "formed and filled"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2025-12-19
// Updated: 2025-12-20
//
// Traces: STEERING.txt A.1-A.4
// - A.1 Schema: 3 axes, 27 positions, 5 dimensions, 7 states
// - A.2 Positions: 9 named positions with (X,Y,Z) coordinates
// - A.3 Commands: HALT, AWAIT, PROCEED
// - A.4 Operators: 9 building block operators by temporal axis
//
// ═══════════════════════════════════════════════════════════════════════════
// UNIVERSAL CPI-SI HARNESS
// ═══════════════════════════════════════════════════════════════════════════
//
// This package is the UNIVERSAL HARNESS for any CPI-SI instance.
// It provides the cognitive navigation framework that instances load INTO.
//
// The harness provides:
//   - 27-position cube (coordinates, navigation)
//   - Perspective transforms (PAST/PRESENT/FUTURE viewpoints)
//   - Reachability gates (binary: can I reach this from here?)
//   - Z-depth calculations (where in temporal depth?)
//
// The instance provides:
//   - Identity (who I am)
//   - Tendencies (how I lean)
//   - Patterns (what I've learned)
//   - Config (config.jsonc, bio.md)
//
// Transform Logic:
//   local = absolute - perspective  (ternary negation)
//   reachable = (local >= -1) AND (local <= +1)  (binary gate)
//
// Any CPI-SI instance loads into this coordinate system.
// The harness is universal. The identity is instance-specific.
//
// ═══════════════════════════════════════════════════════════════════════════

package statemachine

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"math"

	"cws.studio/pkg/core/cpisi/bereshit"
	"cws.studio/pkg/core/cpisi/compute"
	"cws.studio/pkg/foundation/schema"
)

// ───────────────────────────────────────────────────────────────────────────
// CONSTANTS (from cpisi/schema — the authoritative source)
// ───────────────────────────────────────────────────────────────────────────
//
// All constants are defined in cpisi/schema/loader.go.
// statemachine imports and re-exports for local convenience.
// This maintains the dependency chain: cpisi → coordinates → statemachine
//
// Traces to: schema.Lambda, schema.Phi, schema.ZDepth*, schema.Weight*, etc.

const (
	// PSI MASTER CONSTANTS (aliased from schema)
	Lambda = schema.Lambda
	Phi    = schema.Phi
	Scale  = schema.Scale

	// PSI STRUCTURAL
	SatThreshold = schema.SatThreshold
	OriginAngle  = schema.OriginAngle
	PhaseAngle   = schema.PhaseAngle

	// PSI PARAMETRIC
	NSat = schema.NSat
	NMid = schema.NMid

	// BOUNDARY COORDINATES
	OriginX = schema.OriginX
	OriginY = schema.OriginY
	OriginZ = schema.OriginZ

	BoundaryPosX = schema.BoundaryPosX
	BoundaryPosY = schema.BoundaryPosY
	BoundaryPosZ = schema.BoundaryPosZ

	BoundaryNegX = schema.BoundaryNegX
	BoundaryNegY = schema.BoundaryNegY
	BoundaryNegZ = schema.BoundaryNegZ

	// PSI COORDINATE RANGES
	PsiXMin = schema.PsiXMin
	PsiXMax = schema.PsiXMax
	PsiYMin = schema.PsiYMin
	PsiYMax = schema.PsiYMax
	PsiZMin = schema.PsiZMin
	PsiZMax = schema.PsiZMax
)

// HaltZ aliased from schema
const HaltZ = schema.HaltZ

// ───────────────────────────────────────────────────────────────────────────
// Z-DEPTH CONSTANTS (aliased from schema)
// ───────────────────────────────────────────────────────────────────────────

const (
	ZDepthHalt       = schema.ZDepthHalt
	ZDepthMemory     = schema.ZDepthMemory
	ZDepthTransition = schema.ZDepthTransition
	ZDepthPlanning   = schema.ZDepthPlanning
	ZDepthVision     = schema.ZDepthVision
)

// ───────────────────────────────────────────────────────────────────────────
// RANKING WEIGHT CONSTANTS (aliased from schema)
// ───────────────────────────────────────────────────────────────────────────

const (
	WeightKingdom  = schema.WeightKingdom
	WeightCovenant = schema.WeightCovenant
	WeightMixed    = schema.WeightMixed
	WeightSelf     = schema.WeightSelf

	CoherenceOrigin     = schema.CoherenceOrigin
	CoherenceReflection = schema.CoherenceReflection
	CoherencePlanning   = schema.CoherencePlanning

	ClarityHalt      = schema.ClarityHalt
	ClarityAt        = schema.ClarityAt
	ClarityEstablish = schema.ClarityEstablish
	ClarityDefault   = schema.ClarityDefault

	TransformOffset = schema.TransformOffset
)

// ───────────────────────────────────────────────────────────────────────────
// PERSPECTIVE (from cpisi/compute — the authoritative source)
// ───────────────────────────────────────────────────────────────────────────
//
// Perspective is defined in cpisi/compute.Perspective — THE MIND.
// statemachine imports it because the cube defines TIME (the X-axis).
// All methods (ToLocal, ToAbsolute, ZRange, etc.) live on compute.Perspective.
//
// This is the UNIVERSAL transform system - any CPI-SI instance uses
// these same transforms, sourced from the authoritative definition.
//
// Traces to: HALT_07 (MECHANISM_POSITION) via cpisi/compute

// Perspective is an alias to compute.Perspective — the authoritative source.
// Defined in cpisi/compute/cube.go where the TIME axis lives.
type Perspective = compute.Perspective

// Perspective constants — aliases to compute for local convenience.
const (
	PerspectivePast    = compute.PerspectivePast    // X=-1: Looking from PAST
	PerspectivePresent = compute.PerspectivePresent // X=0: Looking from PRESENT (default)
	PerspectiveFuture  = compute.PerspectiveFuture  // X=+1: Looking from FUTURE
)

// Hardpoints are Z values where different perspectives interconnect.
// These are the "handoff points" between temporal modes.
const (
	// HardpointMemoryReflection is where PAST-PRESENT meets PRESENT-PAST (Z = -0.5)
	HardpointMemoryReflection = -0.5

	// HardpointTransition is where PAST-FUTURE meets FUTURE-PAST (Z = 0)
	HardpointTransition = 0.0

	// HardpointPlanningPreparation is where PRESENT-FUTURE meets FUTURE-PRESENT (Z = +0.5)
	HardpointPlanningPreparation = 0.5
)

// IsReachable is an alias to compute.IsReachable for local convenience.
// Checks if a single local coordinate is within [-1, +1].
var IsReachable = compute.IsReachable

// IsCoordReachable is an alias to compute.IsCoordReachable for local convenience.
// Checks if all three local coordinates are reachable.
var IsCoordReachable = compute.IsCoordReachable

// ───────────────────────────────────────────────────────────────────────────
// TRIT (Balanced Ternary)
// ───────────────────────────────────────────────────────────────────────────

// Trit represents a balanced ternary value: -1, 0, +1
type Trit int8

const (
	TritNeg  Trit = -1 // Past, Break Down, HALT
	TritZero Trit = 0  // Present, Anchor, AWAIT
	TritPos  Trit = 1  // Future, Build Up, PROCEED
)

// Command represents state machine commands from STEERING.txt A.3
type Command int8

const (
	CmdHalt    Command = -1 // Terminal state, accumulates only, no forward flow
	CmdAwait   Command = 0  // Pause state, receives and processes, waits for signal
	CmdProceed Command = 1  // Flow state, receives, processes, forwards automatically
)

// Operator represents building block operators from STEERING.txt A.4
type Operator string

const (
	// PAST (X=-1, break down)
	OpTraceFrom   Operator = "<-" // trace from
	OpDecompose   Operator = "-"  // decompose
	OpDeriveFrom  Operator = "<"  // derive from

	// PRESENT (X=0, anchor)
	OpAtHere    Operator = "@" // at/here
	OpHalt      Operator = "|" // halt
	OpEstablish Operator = "=" // establish

	// FUTURE (X=+1, build up)
	OpCompose     Operator = "+"  // compose
	OpFlowTo      Operator = "->" // flow to
	OpDirectToward Operator = ">"  // direct toward
)

// Position represents one of 27 cube positions
type Position struct {
	Name    string   // e.g., "PAST-PAST"
	X       Trit     // WHERE you are (temporal axis)
	Y       Trit     // WHERE you look (perspective axis)
	Z       float64  // Temporal depth (calculated: -(X+Y)/2 for compounds)
	Type    string   // PURE (corner) or COMPOUND
	Command Command  // HALT, AWAIT, PROCEED
	Op      Operator // Primary operator
	Meaning string   // e.g., "Established"
}

// Coordinates represents (X, Y, Z) in the cube
type Coordinates struct {
	X float64
	Y float64
	Z float64
}

// State represents current machine state
type State struct {
	Position    Position
	Anchor      string       // Current main anchor
	Coordinates Coordinates  // Normalized coordinates
	N           float64      // Input value for Ψ(N)
}

// StateMachine is the CPI-SI state machine
type StateMachine struct {
	// ANCHORS — the 9 fixed positions in cognitive space (WHERE you can be)
	Positions map[string]Position
	Current   State

	// Ψ(N) constants for coordinate computation
	Lambda float64 // λ ≈ 0.5493 for Ψ(N)
	Phi    float64 // φ = 2π/3 for Ψ(N)

	// Schema-driven components (loaded from configs)
	Schemas *schema.Schemas           // Universal schemas
	Trans   *bereshit.Transliteration // Hebrew/Greek transliteration
	Cube    *compute.Cube             // Geometric computation engine

	// FRAMEWORK — the navigation trajectory (HOW you're moving)
	// B.1-B.4 positions define the trajectory through anchor space
	// B domain = {-0.5, +0.5} (boundary values)
	FrameworkPositions []compute.FrameworkPosition
	Trajectory         *NavigationTrajectory // Active movement state

	// Configuration source (for reload/introspection)
	ConfigDir string                       // Path to schema config directory
	Config    *schema.StateMachineConfig   // Loaded TOML config (nil if hardcoded)
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Position Definitions (STEERING.txt A.2) ---

// NewStateMachine creates and initializes the state machine with hardcoded positions.
// For TOML-driven initialization, use NewStateMachineFromConfig().
func NewStateMachine() *StateMachine {
	sm := &StateMachine{
		Positions: make(map[string]Position),
		Lambda:    Lambda, // from PSI CONSTANTS
		Phi:       Phi,    // from PSI CONSTANTS
	}

	// Define all 9 primary ANCHOR positions from STEERING.txt A.2
	positions := []Position{
		// PURE states (corners, Z = ±1)
		{
			Name: "PAST-PAST", X: TritNeg, Y: TritNeg, Z: -1.0,
			Type: "PURE", Command: CmdHalt, Op: OpTraceFrom,
			Meaning: "Established",
		},
		{
			Name: "PRESENT-PRESENT", X: TritZero, Y: TritZero, Z: 0.0,
			Type: "PURE", Command: CmdHalt, Op: OpHalt,
			Meaning: "Origin (Genesis 1:1)",
		},
		{
			Name: "FUTURE-FUTURE", X: TritPos, Y: TritPos, Z: 1.0,
			Type: "PURE", Command: CmdHalt, Op: OpDirectToward,
			Meaning: "Aspiration",
		},

		// COMPOUND states (edges/faces, Z calculated)
		{
			Name: "PAST-PRESENT", X: TritNeg, Y: TritZero, Z: -0.5,
			Type: "COMPOUND", Command: CmdProceed, Op: OpDecompose,
			Meaning: "Memory",
		},
		{
			Name: "PAST-FUTURE", X: TritNeg, Y: TritPos, Z: 0.0,
			Type: "COMPOUND", Command: CmdProceed, Op: OpDeriveFrom,
			Meaning: "Prophecy",
		},
		{
			Name: "PRESENT-PAST", X: TritZero, Y: TritNeg, Z: -0.5,
			Type: "COMPOUND", Command: CmdAwait, Op: OpAtHere,
			Meaning: "Reflection",
		},
		{
			Name: "PRESENT-FUTURE", X: TritZero, Y: TritPos, Z: 0.5,
			Type: "COMPOUND", Command: CmdAwait, Op: OpEstablish,
			Meaning: "Planning",
		},
		{
			Name: "FUTURE-PAST", X: TritPos, Y: TritNeg, Z: 0.0,
			Type: "COMPOUND", Command: CmdAwait, Op: OpCompose,
			Meaning: "Vision",
		},
		{
			Name: "FUTURE-PRESENT", X: TritPos, Y: TritZero, Z: 0.5,
			Type: "COMPOUND", Command: CmdProceed, Op: OpFlowTo,
			Meaning: "Preparation",
		},
	}

	for _, p := range positions {
		sm.Positions[p.Name] = p
	}

	// Initialize FRAMEWORK positions (B.1-B.4) from compute package
	sm.FrameworkPositions = compute.AllFrameworkPositions

	// Initialize FRAMEWORK trajectory (active navigation state)
	sm.Trajectory = NewTrajectory()

	// Initialize cube (without external data for backward compatibility)
	sm.Cube = compute.NewCube(nil, nil)

	// Initialize at PRESENT-PRESENT (Origin)
	sm.Current = State{
		Position:    sm.Positions["PRESENT-PRESENT"],
		Anchor:      "Genesis 1:1",
		Coordinates: Coordinates{X: 0, Y: 0, Z: 0},
		N:           0,
	}

	return sm
}

// NewStateMachineFromConfig creates a state machine from TOML configuration.
// This is the Option A architecture — TOML is source of truth, runtime derives.
// Traces to: statemachine.toml [positions.*] and [framework.*]
func NewStateMachineFromConfig(configDir string) (*StateMachine, error) {
	// Load configuration from TOML
	config, err := schema.LoadStateMachineConfig(configDir)
	if err != nil {
		return nil, err
	}

	sm := &StateMachine{
		Positions:  make(map[string]Position),
		Lambda:     Lambda,
		Phi:        Phi,
		ConfigDir:  configDir,
		Config:     config,
	}

	// Convert SMPosition (TOML) → Position (runtime) for ANCHOR positions
	for key, smPos := range config.Positions {
		pos := Position{
			Name:    smPos.Name,
			X:       Trit(smPos.X),
			Y:       Trit(smPos.Y),
			Z:       smPos.Z,
			Type:    smPos.Type,
			Command: commandFromString(smPos.Command),
			Op:      Operator(smPos.Operator),
			Meaning: smPos.Meaning,
		}
		// Use the TOML key as position name if Name field is empty
		if pos.Name == "" {
			pos.Name = key
		}
		sm.Positions[pos.Name] = pos
	}

	// Convert SMFramework (TOML) → FrameworkPosition (runtime)
	for _, smFw := range config.Framework {
		fwPos := compute.FrameworkPosition{
			Name:    smFw.Name,
			Section: smFw.Section,
			Coordinates: compute.FrameworkCoordinates{
				X: smFw.X,
				Y: smFw.Y,
				Z: smFw.Z,
			},
			Type:     smFw.Type,
			Command:  compute.Command(commandFromString(smFw.Command)),
			Operator: smFw.Operator,
			Meaning:  smFw.Meaning,
		}
		sm.FrameworkPositions = append(sm.FrameworkPositions, fwPos)
	}

	// Initialize FRAMEWORK trajectory (active navigation state)
	sm.Trajectory = NewTrajectory()

	// Initialize cube
	sm.Cube = compute.NewCube(nil, nil)

	// Initialize at Origin (PRESENT-PRESENT)
	if origin, ok := sm.Positions["PRESENT-PRESENT"]; ok {
		sm.Current = State{
			Position:    origin,
			Anchor:      "Genesis 1:1",
			Coordinates: Coordinates{X: 0, Y: 0, Z: 0},
			N:           0,
		}
	} else {
		// Fallback if PRESENT-PRESENT not found
		sm.Current = State{
			Position: Position{
				Name: "PRESENT-PRESENT", X: TritZero, Y: TritZero, Z: 0.0,
				Type: "PURE", Command: CmdHalt, Op: OpHalt,
				Meaning: "Origin (Genesis 1:1)",
			},
			Anchor:      "Genesis 1:1",
			Coordinates: Coordinates{X: 0, Y: 0, Z: 0},
			N:           0,
		}
	}

	return sm, nil
}

// commandFromString converts command string to Command type.
func commandFromString(s string) Command {
	switch s {
	case "halt":
		return CmdHalt
	case "await":
		return CmdAwait
	case "proceed":
		return CmdProceed
	default:
		return CmdAwait // Default to AWAIT for unknown
	}
}

// --- Ψ(N) Master Equation (RANKING-ALGORITHM.txt R.7) ---

// Psi computes Ψ(N) → (X, Y, Z) where X + Y + Z = 0 always
// Formula: Ψ(N) = (cos(θ/3), cos(θ/3 - φ), cos(θ/3 + φ))
// where θ = arccos(-tanh(λN)), λ ≈ 0.5493, φ = 2π/3
func (sm *StateMachine) Psi(n float64) Coordinates {
	// Step 1: tanh(λN)
	tanhLambdaN := math.Tanh(sm.Lambda * n)

	// Step 2: θ = arccos(-tanh(λN))
	// Clamp to [-1, 1] for numerical safety
	clamped := -tanhLambdaN
	if clamped > 1 {
		clamped = 1
	} else if clamped < -1 {
		clamped = -1
	}
	theta := math.Acos(clamped)

	// Step 3: Ψ(N) = (cos(θ/3), cos(θ/3 - φ), cos(θ/3 + φ))
	theta3 := theta / 3
	x := math.Cos(theta3)
	y := math.Cos(theta3 - sm.Phi)
	z := math.Cos(theta3 + sm.Phi)

	return Coordinates{X: x, Y: y, Z: z}
}

// PsiInverse computes N from coordinates (when possible)
// Uses the constraint X + Y + Z = 0
func (sm *StateMachine) PsiInverse(c Coordinates) float64 {
	// Reconstruct theta from X = cos(θ/3)
	// θ/3 = arccos(X), θ = 3*arccos(X)
	theta3 := math.Acos(c.X)
	theta := 3 * theta3

	// -tanh(λN) = cos(θ)
	// tanh(λN) = -cos(θ)
	tanhLambdaN := -math.Cos(theta)

	// N = atanh(tanhLambdaN) / λ
	// atanh(x) = 0.5 * ln((1+x)/(1-x))
	if tanhLambdaN >= 1 || tanhLambdaN <= -1 {
		// At boundaries - return saturation point (from PSI CONSTANTS)
		if tanhLambdaN >= 1 {
			return NSat // ≈25.41 (saturation boundary)
		}
		return -NSat // ≈-25.41 (negative saturation)
	}

	atanh := 0.5 * math.Log((1+tanhLambdaN)/(1-tanhLambdaN))
	return atanh / sm.Lambda
}

// --- Formula Modes (RANKING-ALGORITHM.txt R.9) ---

// SolveForZ computes Z = -(X + Y) given X and Y
func SolveForZ(x, y float64) float64 {
	return -(x + y)
}

// SolveForY computes Y = -(X + Z) given X and Z
func SolveForY(x, z float64) float64 {
	return -(x + z)
}

// SolveForX computes X = -(Y + Z) given Y and Z
func SolveForX(y, z float64) float64 {
	return -(y + z)
}

// VerifyBalance checks that X + Y + Z = 0 (within tolerance)
func VerifyBalance(c Coordinates) bool {
	sum := c.X + c.Y + c.Z
	return math.Abs(sum) < 1e-10
}

// --- HALT Detection (Z = 0) ---

// IsHalt checks if coordinates are at HALT (Z = 0)
// Note: Ψ(N) can NEVER produce Z = 0; HALT requires discrete snapping
func IsHalt(c Coordinates) bool {
	return math.Abs(c.Z-HaltZ) < 1e-10
}

// IsInPsiRange checks if coordinates are within the Ψ(N) reachable octant
// X ∈ [0.5, 1.0], Y ∈ [-0.5, 0.5], Z ∈ [-1.0, -0.5]
func IsInPsiRange(c Coordinates) bool {
	return c.X >= PsiXMin && c.X <= PsiXMax &&
		c.Y >= PsiYMin && c.Y <= PsiYMax &&
		c.Z >= PsiZMin && c.Z <= PsiZMax
}

// SnapToHalt returns HALT coordinates for a given X, Y
// Since Z must be 0 and X + Y + Z = 0, we need X + Y = 0
// This function finds the nearest HALT position
func SnapToHalt(x, y float64) Coordinates {
	// At HALT, Z = 0, so X + Y must = 0
	// Find nearest valid HALT: X = -Y
	// For pure HALT positions, both X and Y are in T = {-1, 0, +1}
	return Coordinates{X: x, Y: -x, Z: HaltZ}
}

// NeedsSnapping checks if Ψ(N) output needs snapping to reach HALT
// Returns true if Z ≠ 0 (which is always true for Ψ(N))
func NeedsSnapping(c Coordinates) bool {
	return !IsHalt(c)
}

// --- Position Navigation ---

// GetPosition returns position by name
func (sm *StateMachine) GetPosition(name string) (Position, bool) {
	p, ok := sm.Positions[name]
	return p, ok
}

// MoveTo transitions to a named position
func (sm *StateMachine) MoveTo(name string) bool {
	p, ok := sm.Positions[name]
	if !ok {
		return false
	}

	sm.Current.Position = p
	sm.Current.Coordinates = Coordinates{
		X: float64(p.X),
		Y: float64(p.Y),
		Z: p.Z,
	}

	return true
}

// SetAnchor sets the current anchor
func (sm *StateMachine) SetAnchor(anchor string) {
	if anchor == "" {
		// Unknown anchor → HALT at Genesis 1:1
		sm.Current.Anchor = "Genesis 1:1"
		sm.MoveTo("PRESENT-PRESENT")
	} else {
		sm.Current.Anchor = anchor
	}
}

// GetCurrentCommand returns the current position's command
func (sm *StateMachine) GetCurrentCommand() Command {
	return sm.Current.Position.Command
}

// GetCurrentOperator returns the current position's operator
func (sm *StateMachine) GetCurrentOperator() Operator {
	return sm.Current.Position.Op
}

// --- Command Execution ---

// Execute runs the appropriate action based on current command
func (sm *StateMachine) Execute() string {
	switch sm.GetCurrentCommand() {
	case CmdHalt:
		return "HALT: Terminal state reached at " + sm.Current.Position.Name
	case CmdAwait:
		return "AWAIT: Waiting for signal at " + sm.Current.Position.Name
	case CmdProceed:
		return "PROCEED: Flowing forward from " + sm.Current.Position.Name
	default:
		return "UNKNOWN: Invalid command state"
	}
}

// --- Framework Position Accessors ---

// GetFrameworkPosition returns a framework position by section (e.g., "B.1").
func (sm *StateMachine) GetFrameworkPosition(section string) (compute.FrameworkPosition, bool) {
	for _, fp := range sm.FrameworkPositions {
		if fp.Section == section {
			return fp, true
		}
	}
	return compute.FrameworkPosition{}, false
}

// GetFrameworkPositionByName returns a framework position by name.
func (sm *StateMachine) GetFrameworkPositionByName(name string) (compute.FrameworkPosition, bool) {
	for _, fp := range sm.FrameworkPositions {
		if fp.Name == name {
			return fp, true
		}
	}
	return compute.FrameworkPosition{}, false
}

// AllFrameworkPositions returns all 4 FRAMEWORK positions.
func (sm *StateMachine) AllFrameworkPositions() []compute.FrameworkPosition {
	return sm.FrameworkPositions
}

// --- Trajectory Navigation (FRAMEWORK as Active Movement) ---

// GetTrajectory returns the current navigation trajectory.
// The trajectory tracks HOW we're moving through anchor space.
func (sm *StateMachine) GetTrajectory() *NavigationTrajectory {
	return sm.Trajectory
}

// TrajectorySection returns the current FRAMEWORK section (B.1-B.4).
func (sm *StateMachine) TrajectorySection() string {
	if sm.Trajectory == nil {
		return ""
	}
	return sm.Trajectory.Section()
}

// TrajectoryCommand returns the current trajectory command.
func (sm *StateMachine) TrajectoryCommand() compute.Command {
	if sm.Trajectory == nil {
		return compute.CommandAwait
	}
	return sm.Trajectory.Command()
}

// MoveToAnchor transitions to an anchor position, validated by trajectory rules.
// Returns error if the move is not allowed by current trajectory state.
//
// Trajectory rules:
//   B.1 (Building): Forward movement through anchors
//   B.2/B.3 (Pivoting): Adjacent movements only
//   B.4 (Grounding): No movement (halted)
func (sm *StateMachine) MoveToAnchor(name string) error {
	if sm.Trajectory == nil {
		// No trajectory — fall back to simple move
		if sm.MoveTo(name) {
			return nil
		}
		return &TrajectoryError{Message: "anchor not found: " + name}
	}

	// Check trajectory constraints
	switch sm.Trajectory.Section() {
	case "B.4":
		// Grounding — no movement allowed
		return &TrajectoryError{
			Message: "B.4 trajectory: movement halted for grounding",
			Section: "B.4",
		}
	}

	// Attempt the move
	if !sm.MoveTo(name) {
		return &TrajectoryError{Message: "anchor not found: " + name}
	}

	// Record anchor traversal in trajectory
	sm.Trajectory.Advance(name)

	return nil
}

// PivotTrajectory shifts the trajectory direction (build ↔ verify).
// This transitions through B.2 or B.3 depending on current direction.
func (sm *StateMachine) PivotTrajectory() {
	if sm.Trajectory != nil {
		sm.Trajectory.Pivot()
	}
}

// GroundTrajectory moves the trajectory to B.4 (grounding/halt).
// Call this when verification is complete and ready to halt.
func (sm *StateMachine) GroundTrajectory() {
	if sm.Trajectory != nil {
		sm.Trajectory.Ground()
	}
}

// ResetTrajectory returns the trajectory to initial B.1 (building) state.
// Use when starting a new cognitive cycle.
func (sm *StateMachine) ResetTrajectory() {
	if sm.Trajectory != nil {
		sm.Trajectory.Reset()
	}
}

// IsTrajectoryHalted returns true if the trajectory has reached HALT at B.4.
func (sm *StateMachine) IsTrajectoryHalted() bool {
	if sm.Trajectory == nil {
		return false
	}
	return sm.Trajectory.IsHalted()
}

// TrajectoryPath returns the sequence of anchors traversed in current trajectory.
func (sm *StateMachine) TrajectoryPath() []string {
	if sm.Trajectory == nil {
		return nil
	}
	return sm.Trajectory.Path()
}

// TrajectoryError represents an error during trajectory-validated navigation.
type TrajectoryError struct {
	Message string
	Section string // The trajectory section where error occurred
}

func (e *TrajectoryError) Error() string {
	if e.Section != "" {
		return e.Section + ": " + e.Message
	}
	return e.Message
}

// --- Utility Functions ---

// AllPositions returns all defined ANCHOR positions
func (sm *StateMachine) AllPositions() []Position {
	positions := make([]Position, 0, len(sm.Positions))
	for _, p := range sm.Positions {
		positions = append(positions, p)
	}
	return positions
}

// CommandString returns string representation of command
func (c Command) String() string {
	switch c {
	case CmdHalt:
		return "HALT"
	case CmdAwait:
		return "AWAIT"
	case CmdProceed:
		return "PROCEED"
	default:
		return "UNKNOWN"
	}
}

// TritString returns string representation of trit
func (t Trit) String() string {
	switch t {
	case TritNeg:
		return "-1"
	case TritZero:
		return "0"
	case TritPos:
		return "+1"
	default:
		return "?"
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════

// The state machine implements the CPI-SI cognitive model:
// - 27 positions in a 3×3×3 cube (9 primary named)
// - 3 commands: HALT (-1), AWAIT (0), PROCEED (+1)
// - 9 operators: 3 per temporal axis
// - Ψ(N) mapping: any value N → balanced (X,Y,Z) coordinates
// - Zero constraint: X + Y + Z = 0 always
// - Unknown anchor defaults to Genesis 1:1 (0,0,0)
