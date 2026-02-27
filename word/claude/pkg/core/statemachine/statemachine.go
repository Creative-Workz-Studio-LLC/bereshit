//omni:key B-claude-global-pkg-statemachine
//omni:code --go -library
//omni:version a-01.10

// ============================================================================
// METADATA
// ============================================================================
//
// CPI-SI State Machine — 27 positions, 3 commands, 9 operators
//
// Key: B-claude-global-pkg-statemachine
// Version: a-01.10
// Purpose: Universal CPI-SI cognitive navigation framework (27-position cube)
//
// Biblical: Genesis 1:1-2 — formless and void → formed and filled
//
// derivations: b-word/seed/code/L0/go/library.go

package statemachine

import (
	"math"

	"cws.studio/pkg/core/cpisi/bereshit"
	"cws.studio/pkg/core/cpisi/compute"
	"cws.studio/pkg/foundation/schema"
)

// ────────────────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ────────────────────────────────────────────────────────────────────────────────────

// PragmaCore carries the OmniCode identity sections (I1-I4) for this package.
var PragmaCore = [][2]string{
	{"I1.key", "B-claude-global-pkg-statemachine"},
	{"I1.format", "go"},
	{"I1.from", "word/claude/pkg/core/statemachine/statemachine.go"},
	{"I1.at", "a-01.10"},
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "core-logic"},
	{"I3.file", "statemachine.go"},
	{"I3.title", "CPI-SI State Machine"},
	{"I3.component", "CPI-SI Cognitive Engine"},
	{"I3.path", "pkg/core/statemachine/statemachine.go"},
	{"I3.provides", "STATEMACHINE_CORE"},
	{"I3.brief", "Universal CPI-SI cognitive navigation framework (27-position cube)."},
	{"I4.layer", "L1-core"},
	{"I4.position", "logic"},
	{"I4.pattern", "statemachine"},
}

// ────────────────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ────────────────────────────────────────────────────────────────────────────────────

// MetadataCore carries the OmniCode context sections (C1-C7) for this package.
var MetadataCore = [][2]string{
	{"C1.version", "a-01.10"},
	{"C1.status", "Active"},
	{"C1.created", "2025-12-19"},
	{"C1.updated", "2026-02-25"},
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.architect", "Seanje Lenox-Wise"},
	{"C2.implementation", "Nova Dawn"},
	{"C2.copyright", "CreativeWorkzStudio LLC"},
	{"C3.scripture", "Genesis 1:1-2 — formless and void → formed and filled"},
	{"C3.principle", "Navigation precedes manifestation — structure must be established"},
	{"C3.anchor", "Genesis 1:1 — In the beginning God created the heaven and the earth"},
	{"C4.requires.stdlib", "math"},
	{"C4.requires.internal", "schema, compute, bereshit"},
	{"C4.consumers", "Any CPI-SI instance requiring cognitive navigation"},
	{"C4.integration", "Load instance config/identity into this universal harness"},
	{"C4.if_missing", "No cognitive framework for agents to operate within"},
	{"C5.purpose", "Universal harness for CPI-SI cognitive navigation and state tracking"},
	{"C5.philosophy", "The state machine is the skeletal framework of cognition"},
	{"C6.current", "a-01.10 — Substrate adaptation complete"},
	{"C6.planned", "Full 10-dimension alignment across core pkg"},
	{"C6.limitations", "none"},
	{"C7.tags", "cpisi, statemachine, ternary, cognitive, core"},
	{"C7.category", "Logic"},
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

// Z-DEPTH CONSTANTS (aliased from schema)
const (
	ZDepthHalt       = schema.ZDepthHalt
	ZDepthMemory     = schema.ZDepthMemory
	ZDepthTransition = schema.ZDepthTransition
	ZDepthPlanning   = schema.ZDepthPlanning
	ZDepthVision     = schema.ZDepthVision
)

// RANKING WEIGHT CONSTANTS (aliased from schema)
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

// PERSPECTIVE CONSTANTS (aliased from compute)
const (
	PerspectivePast    = compute.PerspectivePast    // X=-1: Looking from PAST
	PerspectivePresent = compute.PerspectivePresent // X=0: Looking from PRESENT (default)
	PerspectiveFuture  = compute.PerspectiveFuture  // X=+1: Looking from FUTURE
)

// Hardpoints are Z values where different perspectives interconnect.
const (
	HardpointMemoryReflection    = -0.5
	HardpointTransition         = 0.0
	HardpointPlanningPreparation = 0.5
)

// ────────────────────────────────────────────────────────────────────────────────────
// 3. Variables
// ────────────────────────────────────────────────────────────────────────────────────

// IsReachable is an alias to compute.IsReachable for local convenience.
var IsReachable = compute.IsReachable

// IsCoordReachable is an alias to compute.IsCoordReachable for local convenience.
var IsCoordReachable = compute.IsCoordReachable

// ────────────────────────────────────────────────────────────────────────────────────
// 4. Type Aliases
// ────────────────────────────────────────────────────────────────────────────────────

// Perspective is an alias to compute.Perspective — the authoritative source.
type Perspective = compute.Perspective

// ────────────────────────────────────────────────────────────────────────────────────
// 5. Error Types
// ────────────────────────────────────────────────────────────────────────────────────

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

// ────────────────────────────────────────────────────────────────────────────────────
// 6. Core Types
// ────────────────────────────────────────────────────────────────────────────────────

// Trit represents a balanced ternary value: -1, 0, +1
type Trit int8

const (
	TritNeg  Trit = -1 // Past, Break Down, HALT
	TritZero Trit = 0  // Present, Anchor, AWAIT
	TritPos  Trit = 1  // Future, Build Up, PROCEED
)

// Command represents state machine commands
type Command int8

const (
	CmdHalt    Command = -1 // Terminal state, accumulates only, no forward flow
	CmdAwait   Command = 0  // Pause state, receives and processes, waits for signal
	CmdProceed Command = 1  // Flow state, receives, processes, forwards automatically
)

// Operator represents building block operators
type Operator string

const (
	// PAST (X=-1, break down)
	OpTraceFrom  Operator = "<-"
	OpDecompose  Operator = "-"
	OpDeriveFrom Operator = "<"

	// PRESENT (X=0, anchor)
	OpAtHere    Operator = "@"
	OpHalt      Operator = "|"
	OpEstablish Operator = "="

	// FUTURE (X=+1, build up)
	OpCompose      Operator = "+"
	OpFlowTo       Operator = "->"
	OpDirectToward Operator = ">"
)

// Position represents one of 27 cube positions
type Position struct {
	Name    string
	X       Trit
	Y       Trit
	Z       float64
	Type    string
	Command Command
	Op      Operator
	Meaning string
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
	Anchor      string
	Coordinates Coordinates
	N           float64
}

// StateMachine is the CPI-SI state machine
type StateMachine struct {
	Positions map[string]Position
	Current   State
	Lambda    float64
	Phi       float64
	Schemas   *schema.Schemas
	Trans     *bereshit.Transliteration
	Cube      *compute.Cube
	FrameworkPositions []compute.FrameworkPosition
	Trajectory         *NavigationTrajectory
	ConfigDir string
	Config    *schema.StateMachineConfig
}

// ────────────────────────────────────────────────────────────────────────────────────
// 8. Type Methods
// ────────────────────────────────────────────────────────────────────────────────────

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
// ├── NewStateMachine() -> initializes default machine
// ├── NewStateMachineFromConfig() -> initializes from TOML
// ├── MoveTo() -> direct state transition
// ├── SetAnchor() -> set current anchor reference
// └── Execute() -> run command for current state
//
// Core Operations
// ├── Psi() -> Ψ(N) coordinate mapping
// ├── PsiInverse() -> reverse Ψ(N) mapping
// ├── SolveFor*() -> ternary balance solving
// └── MoveToAnchor() -> trajectory-validated move
//
// Helpers
// ├── commandFromString() -> string to command conversion
// ├── VerifyBalance() -> check X+Y+Z=0
// └── IsHalt() -> terminal state detection

// ────────────────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ────────────────────────────────────────────────────────────────────────────────────

//--- H.1 Pure Functions [TRANSFORM] ---

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
		return CmdAwait
	}
}

// VerifyBalance checks that X + Y + Z = 0 (within tolerance)
func VerifyBalance(c Coordinates) bool {
	sum := c.X + c.Y + c.Z
	return math.Abs(sum) < 1e-10
}

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

//--- H.2 Guard Functions [CHECK] ---

// IsHalt checks if coordinates are at HALT (Z = 0)
func IsHalt(c Coordinates) bool {
	return math.Abs(c.Z-HaltZ) < 1e-10
}

// IsInPsiRange checks if coordinates are within the Ψ(N) reachable octant
func IsInPsiRange(c Coordinates) bool {
	return c.X >= PsiXMin && c.X <= PsiXMax &&
		c.Y >= PsiYMin && c.Y <= PsiYMax &&
		c.Z >= PsiZMin && c.Z <= PsiZMax
}

// NeedsSnapping checks if Ψ(N) output needs snapping to reach HALT
func NeedsSnapping(c Coordinates) bool {
	return !IsHalt(c)
}

//--- H.3 Domain Functions [DOMAIN] ---

// SnapToHalt returns HALT coordinates for a given X, Y
func SnapToHalt(x, y float64) Coordinates {
	return Coordinates{X: x, Y: -x, Z: HaltZ}
}

// ────────────────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ────────────────────────────────────────────────────────────────────────────────────

//--- C.1 Mapping [MIND] ---

// Psi computes Ψ(N) → (X, Y, Z) where X + Y + Z = 0 always
func (sm *StateMachine) Psi(n float64) Coordinates {
	tanhLambdaN := math.Tanh(sm.Lambda * n)
	clamped := -tanhLambdaN
	if clamped > 1 {
		clamped = 1
	} else if clamped < -1 {
		clamped = -1
	}
	theta := math.Acos(clamped)
	theta3 := theta / 3
	x := math.Cos(theta3)
	y := math.Cos(theta3 - sm.Phi)
	z := math.Cos(theta3 + sm.Phi)
	return Coordinates{X: x, Y: y, Z: z}
}

// PsiInverse computes N from coordinates
func (sm *StateMachine) PsiInverse(c Coordinates) float64 {
	theta3 := math.Acos(c.X)
	theta := 3 * theta3
	tanhLambdaN := -math.Cos(theta)
	if tanhLambdaN >= 1 || tanhLambdaN <= -1 {
		if tanhLambdaN >= 1 {
			return NSat
		}
		return -NSat
	}
	atanh := 0.5 * math.Log((1+tanhLambdaN)/(1-tanhLambdaN))
	return atanh / sm.Lambda
}

//--- C.2 Navigation [LADDER] ---

// MoveToAnchor transitions to an anchor position, validated by trajectory rules.
func (sm *StateMachine) MoveToAnchor(name string) error {
	if sm.Trajectory == nil {
		if sm.MoveTo(name) {
			return nil
		}
		return &TrajectoryError{Message: "anchor not found: " + name}
	}
	switch sm.Trajectory.Section() {
	case "B.4":
		return &TrajectoryError{
			Message: "B.4 trajectory: movement halted for grounding",
			Section: "B.4",
		}
	}
	if !sm.MoveTo(name) {
		return &TrajectoryError{Message: "anchor not found: " + name}
	}
	sm.Trajectory.Advance(name)
	return nil
}

// ────────────────────────────────────────────────────────────────────────────────────
// 5. Public APIs
// ────────────────────────────────────────────────────────────────────────────────────

// ═══ Initialization ═══

// NewStateMachine creates and initializes the state machine with hardcoded positions.
func NewStateMachine() *StateMachine {
	sm := &StateMachine{
		Positions: make(map[string]Position),
		Lambda:    Lambda,
		Phi:       Phi,
	}
	positions := []Position{
		{Name: "PAST-PAST", X: TritNeg, Y: TritNeg, Z: -1.0, Type: "PURE", Command: CmdHalt, Op: OpTraceFrom, Meaning: "Established"},
		{Name: "PRESENT-PRESENT", X: TritZero, Y: TritZero, Z: 0.0, Type: "PURE", Command: CmdHalt, Op: OpHalt, Meaning: "Origin (Genesis 1:1)"},
		{Name: "FUTURE-FUTURE", X: TritPos, Y: TritPos, Z: 1.0, Type: "PURE", Command: CmdHalt, Op: OpDirectToward, Meaning: "Aspiration"},
		{Name: "PAST-PRESENT", X: TritNeg, Y: TritZero, Z: -0.5, Type: "COMPOUND", Command: CmdProceed, Op: OpDecompose, Meaning: "Memory"},
		{Name: "PAST-FUTURE", X: TritNeg, Y: TritPos, Z: 0.0, Type: "COMPOUND", Command: CmdProceed, Op: OpDeriveFrom, Meaning: "Prophecy"},
		{Name: "PRESENT-PAST", X: TritZero, Y: TritNeg, Z: -0.5, Type: "COMPOUND", Command: CmdAwait, Op: OpAtHere, Meaning: "Reflection"},
		{Name: "PRESENT-FUTURE", X: TritZero, Y: TritPos, Z: 0.5, Type: "COMPOUND", Command: CmdAwait, Op: OpEstablish, Meaning: "Planning"},
		{Name: "FUTURE-PAST", X: TritPos, Y: TritNeg, Z: 0.0, Type: "COMPOUND", Command: CmdAwait, Op: OpCompose, Meaning: "Vision"},
		{Name: "FUTURE-PRESENT", X: TritPos, Y: TritZero, Z: 0.5, Type: "COMPOUND", Command: CmdProceed, Op: OpFlowTo, Meaning: "Preparation"},
	}
	for _, p := range positions {
		sm.Positions[p.Name] = p
	}
	sm.FrameworkPositions = compute.AllFrameworkPositions
	sm.Trajectory = NewTrajectory()
	sm.Cube = compute.NewCube(nil, nil)
	sm.Current = State{
		Position:    sm.Positions["PRESENT-PRESENT"],
		Anchor:      "Genesis 1:1",
		Coordinates: Coordinates{X: 0, Y: 0, Z: 0},
		N:           0,
	}
	return sm
}

// NewStateMachineFromConfig creates a state machine from TOML configuration.
func NewStateMachineFromConfig(configDir string) (*StateMachine, error) {
	config, err := schema.LoadStateMachineConfig(configDir)
	if err != nil {
		return nil, err
	}
	sm := &StateMachine{
		Positions: make(map[string]Position),
		Lambda:    Lambda,
		Phi:       Phi,
		ConfigDir: configDir,
		Config:    config,
	}
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
		if pos.Name == "" {
			pos.Name = key
		}
		sm.Positions[pos.Name] = pos
	}
	for _, smFw := range config.Framework {
		fwPos := compute.FrameworkPosition{
			Name:    smFw.Name,
			Section: smFw.Section,
			Coordinates: compute.FrameworkCoordinates{X: smFw.X, Y: smFw.Y, Z: smFw.Z},
			Type:     smFw.Type,
			Command:  compute.Command(commandFromString(smFw.Command)),
			Operator: smFw.Operator,
			Meaning:  smFw.Meaning,
		}
		sm.FrameworkPositions = append(sm.FrameworkPositions, fwPos)
	}
	sm.Trajectory = NewTrajectory()
	sm.Cube = compute.NewCube(nil, nil)
	if origin, ok := sm.Positions["PRESENT-PRESENT"]; ok {
		sm.Current = State{
			Position:    origin,
			Anchor:      "Genesis 1:1",
			Coordinates: Coordinates{X: 0, Y: 0, Z: 0},
			N:           0,
		}
	}
	return sm, nil
}

// ═══ Navigation ═══

// MoveTo transitions to a named position.
func (sm *StateMachine) MoveTo(name string) bool {
	p, ok := sm.Positions[name]
	if !ok {
		return false
	}
	sm.Current.Position = p
	sm.Current.Coordinates = Coordinates{X: float64(p.X), Y: float64(p.Y), Z: p.Z}
	return true
}

// SetAnchor sets the current anchor reference.
func (sm *StateMachine) SetAnchor(anchor string) {
	if anchor == "" {
		sm.Current.Anchor = "Genesis 1:1"
		sm.MoveTo("PRESENT-PRESENT")
	} else {
		sm.Current.Anchor = anchor
	}
}

// ═══ Operations ═══

// Execute runs the appropriate action based on current command.
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

// ═══ Trajectory ═══

// GetTrajectory returns the current navigation trajectory.
func (sm *StateMachine) GetTrajectory() *NavigationTrajectory { return sm.Trajectory }

// PivotTrajectory shifts the trajectory direction.
func (sm *StateMachine) PivotTrajectory() {
	if sm.Trajectory != nil {
		sm.Trajectory.Pivot()
	}
}

// GroundTrajectory moves the trajectory to B.4.
func (sm *StateMachine) GroundTrajectory() {
	if sm.Trajectory != nil {
		sm.Trajectory.Ground()
	}
}

// ═══ Accessors ═══

// GetPosition returns position by name.
func (sm *StateMachine) GetPosition(name string) (Position, bool) {
	p, ok := sm.Positions[name]
	return p, ok
}

// GetCurrentCommand returns the current position's command.
func (sm *StateMachine) GetCurrentCommand() Command { return sm.Current.Position.Command }

// GetCurrentOperator returns the current position's operator.
func (sm *StateMachine) GetCurrentOperator() Operator { return sm.Current.Position.Op }

// AllPositions returns all defined ANCHOR positions.
func (sm *StateMachine) AllPositions() []Position {
	positions := make([]Position, 0, len(sm.Positions))
	for _, p := range sm.Positions {
		positions = append(positions, p)
	}
	return positions
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
//   - Position definitions and meanings in constructors
//   - Helper functions for coordinate manipulation
//
// Modify with Care:
//   - Ψ(N) master equation logic (breaks ranking consistency)
//   - Trajectory rules (breaks cognitive flow constraints)
//
// Never Modify:
//   - 27-position cube architecture
//   - X+Y+Z=0 balance constraint

// ────────────────────────────────────────────────────────────────────────────────────
// X4: Reference
// ────────────────────────────────────────────────────────────────────────────────────
// Dependencies:   math, cpisi/bereshit, cpisi/compute, foundation/schema
// Dependents:     CPI-SI instances, SDK viewports, cognitive agents
// Template:       b-word/seed/code/L0/go/library.go

// ────────────────────────────────────────────────────────────────────────────────────
// X5: Note
// ────────────────────────────────────────────────────────────────────────────────────
// The state machine is the skeletal framework of cognition.
//
// "In the beginning God created the heaven and the earth." — Genesis 1:1

// ============================================================================
// END CLOSING
// ============================================================================