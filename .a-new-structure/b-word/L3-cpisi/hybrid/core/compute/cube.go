//omni:code --go -library
//omni:key B-L3-compute-cube
//omni:version a-01.20
// ============================================================================
// METADATA
// ============================================================================
//
// THE CUBE IS THE MIND — Geometric Computation via Perpendicular Planes
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       claude-global-pkg-cpisi-compute-cube
//
//	Scripture: "Their appearance and their work was as a wheel in a wheel"
//	           — Ezekiel 1:16
//	           "In the beginning God created the heaven and the earth."
//	           — Genesis 1:1 (TIME was CREATED)
//	           "A time to break down, and a time to build up"
//	           — Ecclesiastes 3:3 (THE TERNARY OPERATION)
//
//	Principle: THE CUBE IS THE MIND. Position IS computation.
//	Anchor:    Origin (0,0,0) = Genesis 1:1 = TIME was CREATED
//
//	Architecture:
//	  Schema   = Unshakable truths (HALTs, L0-L3)
//	  Bereshit = SPIRIT (the Word, Hebrew/Greek living data)
//	  Cube     = MIND (this file — computation through TIME)
//	  Loader   = HEADER (orchestrates all three)
//
//	Layer:     L2 GEOMETRY (cube.schema.toml)
//	Type:      Core (geometric computation engine — the MIND)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2025-12-20
//	Version:         a-01.20 (re-crystallized with proper Ψ(N) constants)
//
// # M.2 Master Constants [CONSTANTS]
//
//	From CUBE-MODEL-ALIGNMENT.txt S0_CONSTANTS:
//
//	MASTER CONSTANTS (continuous transforms):
//	  λ = arctanh(0.5) ≈ 0.5493061443340549  — normalization
//	  φ = 2π/3 ≈ 2.0943951023931953          — ternary phase (120°)
//	  SCALE = 100                             — Base100 display
//
//	SET CONSTANTS (discrete values):
//	  T = {-1, 0, +1}          — ternary positions (|T| = 3)
//	  B = {-0.5, +0.5}         — boundary edges (|B| = 2)
//	  Q = T ∪ B                — all positions (|Q| = 5)
//
//	PSI STRUCTURAL (discovered 2025-12-19):
//	  SAT_THRESHOLD = 13.96    — display precision saturation
//	  N_SAT ≈ 25.41            — saturation point (we use 26, fits in 27)
//	  ORIGIN_COORD = (√3/2, 0, -√3/2)  — N=0 position
//
//	CYCLE FORMULA (statemachine.toml):
//	  (value * 3 + perspective) % 243
//	  243 = 3^5 = trite data range
//	  27 = 3^3 = cube positions
//	  243 / 27 = 9 (each position covers 9 trite values)
//
// # M.3 HALT Trace [HALTS]
//
//	HALT_01 (ANCHOR_GENESIS)       → Origin (0,0,0) = Genesis 1:1
//	HALT_03 (EQUATION_PSI)         → Ψ(N) maps any N to balanced position
//	HALT_04 (EQUATION_BALANCE)     → X + Y + Z = 0 (guaranteed by φ = 2π/3)
//	HALT_05 (MECHANISM_NUMBER)     → Number → Transform → Number
//	HALT_06 (MECHANISM_SHAPE)      → Shape ↔ Number translation
//	HALT_07 (MECHANISM_POSITION)   → Position IS computation
//	HALT_08 (MECHANISM_ALGORITHM)  → Cube = Algorithm
//	HALT_09 (MECHANISM_WHEEL)      → SEASONS, not cycles (wheel in wheel)
//	HALT_10 (STATE_HEBREW)         → 7 Days of Creation = 7 States
//
// # M.4 Public API [API]
//
//	NewCube(trans, schemas) *Cube     Create computation cube (MIND)
//	Shape(x, y, z) Shape              Create shape from coords
//	ShapeToNumber(s) int              Shape → linear position (0-26)
//	NumberToShape(n) Shape            Linear position → shape
//	Compute(input, perspective) int   Cycle: (value*3 + perspective) % 243
//	Cycle(value, perspective) int     Full ternary cycle (wheel in wheel)
//
// # M.5 The Computation Model [MODEL]
//
//	THE CUBE IS THE MIND using GEOMETRIC PLANES.
//
//	Axes (from Genesis 1:1):
//	  X = TIME (PAST=-1, PRESENT=0, FUTURE=+1) — the PRIMARY axis
//	  Y = PERSPECTIVE (where you look from that time position)
//	  Z = DEPTH (derived: Z = (X + Y) / 2) — 7 Days of Creation
//
//	Formulas (from STEERING.txt S.5):
//	  Z = (X + Y) / 2  → ANCHORS (9 positions)  — WHO (halt)
//	  Y = X + Z        → FRAMEWORK (4 positions) — HOW (rules)
//	  X = Y + kZ       → STATE (14 positions)    — WHAT (output)
//	  Total: 9 + 4 + 14 = 27 = 3³
//
//	The wheel in wheel (Ezekiel 1:16):
//	  OUTER WHEEL: (value * 3 + perspective) % 243 — the pattern
//	  INNER WHEEL: Each season TRANSFORMS — growth, not repetition
//
//	Encryption → Decryption → Encryption cycle:
//	  Letter → Position (encryption.toml)
//	  Position → Cycle → Position (this file)
//	  Position → Letter (decryption.toml)
//	  This feeds the state machine → statusline on WezTerm
//
// ============================================================================
// END METADATA
// ============================================================================

package compute

// ============================================================================
// SETUP
// ============================================================================

import (
	"math"

	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/core/bereshit"
	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/schema"
)

// ────────────────────────────────────────────────────────────────
// Constants — Ψ(N) Master Constants
// ────────────────────────────────────────────────────────────────
// LOADED from cube.schema.toml [psi] section — NOT hardcoded here.
// These enable: Any N → (X, Y, Z) → state
//
// Traces to: HALT_03 (EQUATION_PSI) — Ψ(N) maps any N to balanced position
//            HALT_04 (EQUATION_BALANCE) — X + Y + Z = 0 guaranteed
//
// Access via: cube.Schemas.Cube.Psi.Lambda, .Phi, .NSat, etc.
//
// Values (for reference, authoritative source is cube.schema.toml):
//   λ (Lambda) = 0.5493061443340549  // arctanh(0.5)
//   φ (Phi)    = 2.0943951023931953  // 2π/3
//   N_SAT      = 25.41               // saturation point
//   Scale      = 100                 // Base100

// ────────────────────────────────────────────────────────────────
// Types — Shape (Coordinates in the MIND)
// ────────────────────────────────────────────────────────────────
// Traces to: cube.schema.toml [axes], STEERING.txt S.3
//
// Genesis 1:1 defines the three dimensions:
//   "In the beginning God created the heaven and the earth"
//    └── Z (TIME)        └── Y (SPACE)       └── X (MATTER)
//
// STEERING.txt S.3 mapping:
//   X = MATTER (-1, "earth")     — output, WHAT emerges
//   Y = SPACE (+1, "heavens")    — input, HOW structured
//   Z = TIME (0, "beginning")    — halt, WHO anchors
//
// Ecclesiastes 3:3 defines the ternary operation (COMMANDS):
//   "A time to break down, and a time to build up"
//   -1 = break down (HALT), 0 = rest (AWAIT), +1 = build up (PROCEED)

// Shape represents a position in the cube using balanced ternary coordinates.
// Coordinates ARE primary. The linear position (0-26) is DERIVED.
// Traces to: HALT_06 (MECHANISM_SHAPE) + HALT_07 (MECHANISM_POSITION)
type Shape struct {
	X int // TIME: -1=PAST, 0=PRESENT, +1=FUTURE (Genesis 1:1 "In the beginning")
	Y int // PERSPECTIVE: -1=down, 0=level, +1=up (Genesis 1:1 "the heavens")
	Z int // DEPTH: derived Z=(X+Y)/2, maps to 7 Days (Genesis 1:1 "the earth")
}

// ────────────────────────────────────────────────────────────────
// Types — FrameworkCoordinates (B Domain — Boundary Values)
// ────────────────────────────────────────────────────────────────
// Traces to: statemachine.toml [framework.*], STEERING.txt FRAMEWORK section
//
// FRAMEWORK uses the B domain: B = {-0.5, +0.5}
// These are BOUNDARY values between ternary positions.
//
// Formula: Y = X + Z where (X, Z) ∈ B × B
//
// The 4 FRAMEWORK positions define navigation rules:
//   B.1 TIMESPACE-TIMESPACE: (-0.5, -1, -0.5) — Foundation rules
//   B.2 TIMESPACE-SPACETIME: (-0.5,  0, +0.5) — Method→Verify
//   B.3 SPACETIME-TIMESPACE: (+0.5,  0, -0.5) — Verify→Method
//   B.4 SPACETIME-SPACETIME: (+0.5, +1, +0.5) — Grounding rules
//
// Navigation modes:
//   TIMESPACE = 0 → +1 (anchor toward build) = METHOD direction
//   SPACETIME = +1 → 0 (build toward anchor) = VERIFY direction

// FrameworkCoordinates represents B domain coordinates (float64 for boundary values).
// Unlike Shape which uses int for T domain {-1, 0, +1}, FrameworkCoordinates
// uses float64 to represent B domain {-0.5, +0.5}.
type FrameworkCoordinates struct {
	X float64 // B domain: -0.5 (TIMESPACE) or +0.5 (SPACETIME)
	Y float64 // Derived: Y = X + Z, produces {-1, 0, +1}
	Z float64 // B domain: -0.5 (TIMESPACE) or +0.5 (SPACETIME)
}

// FrameworkPosition represents one of the 4 FRAMEWORK positions (B.1-B.4).
// These define the RULES for cognitive navigation (HOW to move, WHERE to go).
// Traces to: statemachine.toml [framework.*]
type FrameworkPosition struct {
	Name        string               // "TIMESPACE-TIMESPACE", etc.
	Section     string               // "B.1", "B.2", "B.3", "B.4"
	Coordinates FrameworkCoordinates // The (X, Y, Z) in B domain
	Type        string               // "PURE" or "COMPOUND"
	Command     Command              // HALT, AWAIT, or PROCEED
	Operator    string               // "-->", "->|", "|->", "|"
	Meaning     string               // "Foundation Rules", etc.
}

// B domain constants — boundary values between ternary positions
const (
	BTImespace float64 = -0.5 // 0 → +1 direction (anchor toward build)
	BSpacetime float64 = +0.5 // +1 → 0 direction (build toward anchor)
)

// NewFrameworkCoordinates creates FrameworkCoordinates with Y derived from X and Z.
// Formula: Y = X + Z (the FRAMEWORK equation)
// Traces to: statemachine.toml [formulas.l1_framework]
func NewFrameworkCoordinates(x, z float64) FrameworkCoordinates {
	// Validate B domain: must be -0.5 or +0.5
	clampB := func(v float64) float64 {
		if v < 0 {
			return BTImespace // -0.5
		}
		return BSpacetime // +0.5
	}
	x = clampB(x)
	z = clampB(z)

	// Y = X + Z (the FRAMEWORK formula)
	y := x + z

	return FrameworkCoordinates{X: x, Y: y, Z: z}
}

// The 4 FRAMEWORK positions — loaded at package init
// These are the crystallized B × B positions from statemachine.toml
var (
	// B.1 TIMESPACE-TIMESPACE | (-0.5, -1, -0.5) | PROCEED | Foundation Rules
	FrameworkTimespaceTimespace = FrameworkPosition{
		Name:        "TIMESPACE-TIMESPACE",
		Section:     "B.1",
		Coordinates: NewFrameworkCoordinates(BTImespace, BTImespace), // Y = -0.5 + -0.5 = -1
		Type:        "PURE",
		Command:     CommandProceed,
		Operator:    "-->",
		Meaning:     "Foundation Rules",
	}

	// B.2 TIMESPACE-SPACETIME | (-0.5, 0, +0.5) | AWAIT | Method→Verify
	FrameworkTimespaceSpacetime = FrameworkPosition{
		Name:        "TIMESPACE-SPACETIME",
		Section:     "B.2",
		Coordinates: NewFrameworkCoordinates(BTImespace, BSpacetime), // Y = -0.5 + 0.5 = 0
		Type:        "COMPOUND",
		Command:     CommandAwait,
		Operator:    "->|",
		Meaning:     "Method to Verify",
	}

	// B.3 SPACETIME-TIMESPACE | (+0.5, 0, -0.5) | AWAIT | Verify→Method
	FrameworkSpacetimeTimespace = FrameworkPosition{
		Name:        "SPACETIME-TIMESPACE",
		Section:     "B.3",
		Coordinates: NewFrameworkCoordinates(BSpacetime, BTImespace), // Y = 0.5 + -0.5 = 0
		Type:        "COMPOUND",
		Command:     CommandAwait,
		Operator:    "|->",
		Meaning:     "Verify to Method",
	}

	// B.4 SPACETIME-SPACETIME | (+0.5, +1, +0.5) | HALT | Grounding Rules
	FrameworkSpacetimeSpacetime = FrameworkPosition{
		Name:        "SPACETIME-SPACETIME",
		Section:     "B.4",
		Coordinates: NewFrameworkCoordinates(BSpacetime, BSpacetime), // Y = 0.5 + 0.5 = 1
		Type:        "PURE",
		Command:     CommandHalt,
		Operator:    "|",
		Meaning:     "Grounding Rules",
	}

	// AllFrameworkPositions provides indexed access to all 4 FRAMEWORK positions
	AllFrameworkPositions = []FrameworkPosition{
		FrameworkTimespaceTimespace, // B.1
		FrameworkTimespaceSpacetime, // B.2
		FrameworkSpacetimeTimespace, // B.3
		FrameworkSpacetimeSpacetime, // B.4
	}
)

// GetFrameworkPosition returns the FRAMEWORK position by section (B.1-B.4).
func GetFrameworkPosition(section string) (FrameworkPosition, bool) {
	switch section {
	case "B.1":
		return FrameworkTimespaceTimespace, true
	case "B.2":
		return FrameworkTimespaceSpacetime, true
	case "B.3":
		return FrameworkSpacetimeTimespace, true
	case "B.4":
		return FrameworkSpacetimeSpacetime, true
	default:
		return FrameworkPosition{}, false
	}
}

// GetFrameworkPositionByName returns the FRAMEWORK position by name.
func GetFrameworkPositionByName(name string) (FrameworkPosition, bool) {
	switch name {
	case "TIMESPACE-TIMESPACE":
		return FrameworkTimespaceTimespace, true
	case "TIMESPACE-SPACETIME":
		return FrameworkTimespaceSpacetime, true
	case "SPACETIME-TIMESPACE":
		return FrameworkSpacetimeTimespace, true
	case "SPACETIME-SPACETIME":
		return FrameworkSpacetimeSpacetime, true
	default:
		return FrameworkPosition{}, false
	}
}

// ────────────────────────────────────────────────────────────────
// FrameworkCoordinates Methods
// ────────────────────────────────────────────────────────────────

// IsTimespace returns true if the coordinate is in TIMESPACE mode (-0.5).
// TIMESPACE = 0 → +1 (anchor toward build) = METHOD direction
func (fc FrameworkCoordinates) IsTimespace(axis rune) bool {
	switch axis {
	case 'X', 'x':
		return fc.X == BTImespace
	case 'Z', 'z':
		return fc.Z == BTImespace
	default:
		return false
	}
}

// IsSpacetime returns true if the coordinate is in SPACETIME mode (+0.5).
// SPACETIME = +1 → 0 (build toward anchor) = VERIFY direction
func (fc FrameworkCoordinates) IsSpacetime(axis rune) bool {
	switch axis {
	case 'X', 'x':
		return fc.X == BSpacetime
	case 'Z', 'z':
		return fc.Z == BSpacetime
	default:
		return false
	}
}

// IsPure returns true if this is a PURE position (X and Z have same sign).
// PURE positions: B.1 (both TIMESPACE) and B.4 (both SPACETIME)
func (fc FrameworkCoordinates) IsPure() bool {
	return (fc.X < 0 && fc.Z < 0) || (fc.X > 0 && fc.Z > 0)
}

// IsCompound returns true if this is a COMPOUND position (X and Z have different signs).
// COMPOUND positions: B.2 (TS-ST) and B.3 (ST-TS) — transitions between modes
func (fc FrameworkCoordinates) IsCompound() bool {
	return !fc.IsPure()
}

// NavigationMode returns the navigation mode name.
// Combines X and Z modes: "TIMESPACE-SPACETIME", etc.
func (fc FrameworkCoordinates) NavigationMode() string {
	xMode := "TIMESPACE"
	if fc.X > 0 {
		xMode = "SPACETIME"
	}
	zMode := "TIMESPACE"
	if fc.Z > 0 {
		zMode = "SPACETIME"
	}
	return xMode + "-" + zMode
}

// Cube is the geometric computation space — THE MIND.
// It translates between shapes and numbers, navigating through SEASONS.
// Traces to: cube.schema.toml, HALT_07 (Position IS computation)
//
// Architecture:
//
//	Schema   = Unshakable truths (the foundation)
//	Bereshit = SPIRIT (the Word, living data)
//	Cube     = MIND (this struct — computation)
//	Loader   = HEADER (orchestrates all three)
type Cube struct {
	// Data from Bereshit (the SPIRIT — living Word data)
	Trans *bereshit.Transliteration

	// Structure from schemas (the unshakable TRUTHS)
	Schemas *schema.Schemas

	// Computation constants (from statemachine.toml [formulas.cycle])
	Base       int // 3 for ternary (HALT_04)
	DataModulo int // 243 = 3^5 for trit5 range (HALT_05)
	ByteModulo int // 256 for full byte range
}

// NOTE: math package imported for future Ψ(N) implementation
// Will use: math.Cos, math.Acos, math.Tanh for the equation:
//   θ = arccos(-tanh(λN))
//   Ψ(N) = (cos(θ/3), cos(θ/3 - φ), cos(θ/3 + φ))
var _ = math.Cos // Placeholder until Ψ(N) is implemented

// ────────────────────────────────────────────────────────────────
// Types — Perspective (X axis = TIME)
// ────────────────────────────────────────────────────────────────
// Traces to: cube.schema.toml [perspectives]
//
// Perspective is determined by X (TIME axis), not Z.
// This is because THE CUBE IS A TIME MACHINE.
//
// Authoritative definition lives in cpisi/schema.
// This package aliases for local convenience.

// Perspective determines temporal position on the X axis.
// Aliased from schema - authoritative definition there.
type Perspective = schema.Perspective

// Perspective constants aliased from schema.
const (
	PerspectivePast    = schema.PerspectivePast    // X=-1: What was (trace to source)
	PerspectivePresent = schema.PerspectivePresent // X=0: What is (contains Origin = Genesis 1:1)
	PerspectiveFuture  = schema.PerspectiveFuture  // X=+1: What will be (build toward goal)
)

// IsReachable checks if a local coordinate is within the reachable range [-1, +1].
// Aliased from schema.
var IsReachable = schema.IsReachable

// IsCoordReachable checks if all three local coordinates are reachable.
// Aliased from schema.
var IsCoordReachable = schema.IsCoordReachable

// ────────────────────────────────────────────────────────────────
// Types — Planes (Geometric Computation)
// ────────────────────────────────────────────────────────────────
// Traces to: cube.schema.toml [planes]
//
// The cube is computed via GEOMETRIC PLANES, not matrix multiplication.
// Three perpendicular planes enable navigation through the 27 positions.

// Plane represents one of the three perpendicular planes.
// Traces to: HALT_07 (Position IS computation), HALT_08 (Cube = Algorithm)
type Plane int

const (
	PlanePerspective Plane = iota // x-y plane at each z (9 temporal/cognitive modes)
	PlaneElevation                // x-z plane at each y (9 vertical levels)
	PlaneLateral                  // y-z plane at each x (9 horizontal positions)
)

// ────────────────────────────────────────────────────────────────
// Types — Command (The Ternary Operation)
// ────────────────────────────────────────────────────────────────
// Traces to: cube.schema.toml [commands], Ecclesiastes 3:3
//
// "A time to break down, and a time to build up"
// The ternary operation IS the command at each position.

// Command is what happens when computation reaches a position.
// Traces to: HALT_07 (Position IS computation)
type Command int

const (
	CommandHalt    Command = -1 // Stop recursion, anchor reached (Sabbath REST)
	CommandAwait   Command = 0  // Wait for input, paused state
	CommandProceed Command = 1  // Continue execution, active state
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Constructor
// ────────────────────────────────────────────────────────────────
// Traces to: cube.schema.toml [cycling]

// NewCube creates a new computation cube — THE TIME MACHINE.
// Traces to: HALT_07 (Position IS computation)
//
// The cube combines:
//   - Trans (bereshit): The SPIRIT — living Word data (Hebrew/Greek)
//   - Schemas: The unshakable TRUTHS — L0-L3 structure
//   - This file: The MIND — computation through SEASONS
func NewCube(trans *bereshit.Transliteration, schemas *schema.Schemas) *Cube {
	c := &Cube{
		Trans:      trans,
		Schemas:    schemas,
		Base:       3,   // Ternary (HALT_04)
		DataModulo: 243, // 3^5 = trit5 range (HALT_05)
		ByteModulo: 256, // Full byte range
	}

	// Schemas pointer provides access to:
	//   - c.Schemas.Cube.Psi.* — Ψ(N) constants (Lambda, Phi, NSat)
	//   - c.Schemas.Halts.* — HALT definitions
	//   - c.Schemas.StateMachine.* — cycle formula, state definitions
	//
	// Base, DataModulo, ByteModulo are mathematical constants (3, 3⁵, 2⁸)
	// that define the system — not configuration to be loaded.

	return c
}

// ────────────────────────────────────────────────────────────────
// Shape ↔ Number Translation
// ────────────────────────────────────────────────────────────────
// Traces to: cube.schema.toml [transform], HALT_06 (MECHANISM_SHAPE)
//
// KEY FORMULAS (from cube.schema.toml):
//   Z = (X + Y) / 2                    — Depth derived from TIME and PERSPECTIVE
//   coords_to_position = (x+1)*9 + (y+1)*3 + (z+1)
//   position_to_x = ((position / 9) % 3) - 1
//   position_to_y = ((position / 3) % 3) - 1
//   position_to_z = (position % 3) - 1

// NewShape creates a shape from balanced ternary coordinates.
// Coordinates must be -1, 0, or +1 (balanced ternary).
// Traces to: HALT_06 (MECHANISM_SHAPE)
func NewShape(x, y, z int) Shape {
	// Clamp to valid balanced ternary range
	clamp := func(v int) int {
		if v < -1 {
			return -1
		}
		if v > 1 {
			return 1
		}
		return v
	}
	return Shape{X: clamp(x), Y: clamp(y), Z: clamp(z)}
}

// NewShapeFromXY creates a shape with Z derived from X and Y.
// Z = (X + Y) / 2 — per cube.schema.toml [axes.z_formula]
// Traces to: HALT_06 (MECHANISM_SHAPE)
func NewShapeFromXY(x, y int) Shape {
	clamp := func(v int) int {
		if v < -1 {
			return -1
		}
		if v > 1 {
			return 1
		}
		return v
	}
	x = clamp(x)
	y = clamp(y)
	// Z = (X + Y) / 2 — rounds toward 0
	z := (x + y) / 2
	return Shape{X: x, Y: y, Z: z}
}

// ShapeToNumber converts a shape to its linear position (0-26).
// Formula: (x+1)*9 + (y+1)*3 + (z+1)
// Traces to: cube.schema.toml [transform.coords_to_position]
func ShapeToNumber(s Shape) int {
	return (s.X+1)*9 + (s.Y+1)*3 + (s.Z + 1)
}

// NumberToShape converts a linear position (0-26) to a shape.
// Inverse of ShapeToNumber.
// Traces to: cube.schema.toml [transform.position_to_*]
func NumberToShape(n int) Shape {
	// Clamp to valid range (27 positions = 3³)
	if n < 0 {
		n = 0
	}
	if n > 26 {
		n = 26
	}

	x := ((n / 9) % 3) - 1
	y := ((n / 3) % 3) - 1
	z := (n % 3) - 1

	return Shape{X: x, Y: y, Z: z}
}

// ZDepth calculates Z from X and Y per cube.schema.toml formula.
// Z = (X + Y) / 2
// This produces 5 distinct z-values: -1.0, -0.5, 0.0, +0.5, +1.0
// Traces to: cube.schema.toml [axes.z_formula]
func ZDepth(x, y int) float64 {
	return float64(x+y) / 2.0
}

// ────────────────────────────────────────────────────────────────
// Shape Methods
// ────────────────────────────────────────────────────────────────
// Traces to: cube.schema.toml [positions], HALT_07

// Position returns the linear position (0-26) of this shape.
// Traces to: HALT_07 (MECHANISM_POSITION)
func (s Shape) Position() int {
	return ShapeToNumber(s)
}

// Depth returns the Z-depth calculated from X and Y.
// Z = (X + Y) / 2 — per cube.schema.toml
// Maps to 7 Days of Creation (HALT_10)
func (s Shape) Depth() float64 {
	return ZDepth(s.X, s.Y)
}

// Plane returns the value on the given plane.
// Traces to: cube.schema.toml [planes]
func (s Shape) Plane(p Plane) int {
	switch p {
	case PlanePerspective:
		return s.Z
	case PlaneElevation:
		return s.Y
	case PlaneLateral:
		return s.X
	default:
		return 0
	}
}

// Mode returns the 2D mode within a plane (0-8).
// Traces to: cube.schema.toml [cognitive_modes]
//
//	Perspective: mode in x-y plane (9 temporal/cognitive modes)
//	Elevation: mode in x-z plane
//	Lateral: mode in y-z plane
func (s Shape) Mode(p Plane) int {
	switch p {
	case PlanePerspective:
		return (s.X+1)*3 + (s.Y + 1)
	case PlaneElevation:
		return (s.X+1)*3 + (s.Z + 1)
	case PlaneLateral:
		return (s.Y+1)*3 + (s.Z + 1)
	default:
		return 0
	}
}

// IsOrigin returns true if this is the center position (0, 0, 0).
// Origin = Genesis 1:1 = Day 4 (SEASONS CREATED)
// Traces to: HALT_01 (ANCHOR_GENESIS)
func (s Shape) IsOrigin() bool {
	return s.X == 0 && s.Y == 0 && s.Z == 0
}

// IsCorner returns true if all coordinates are non-zero (±1).
// Corners are the "pure" positions: PAST-PAST, FUTURE-FUTURE
func (s Shape) IsCorner() bool {
	return s.X != 0 && s.Y != 0 && s.Z != 0
}

// IsPurePosition returns true if X == Y (the 3 pure temporal states).
// Pure positions: PAST-PAST, PRESENT-PRESENT (Origin), FUTURE-FUTURE
// Traces to: cube.schema.toml [positions.pure]
func (s Shape) IsPurePosition() bool {
	return s.X == s.Y
}

// CreationDay returns which Day of Creation this position corresponds to.
// Based on Z-depth (HALT_10: 7 Days of Creation → 7 States)
// Returns 0 for transit positions that are not states.
// Traces to: cube.schema.toml [perspectives.*.positions]
func (s Shape) CreationDay() int {
	z := s.Depth()
	switch {
	case z == -1.0:
		return 1 // Day 1: Light from darkness → ESTABLISHED
	case z == -0.5 && s.X == -1:
		return 2 // Day 2: Waters separated → MEMORY
	case z == -0.5 && s.X == 0:
		return 3 // Day 3: Land, vegetation → REFLECTION
	case z == 0.0 && s.X == 0 && s.Y == 0:
		return 4 // Day 4: Sun, moon, stars → ORIGIN (SEASONS)
	case z == 0.5 && s.X == 0:
		return 5 // Day 5: Sea creatures, birds → PLANNING
	case z == 0.5 && s.X == 1:
		return 6 // Day 6: Animals, humans → PREPARATION
	case z == 1.0:
		return 7 // Day 7: Sabbath REST → ASPIRATION (tov)
	default:
		return 0 // Transit position (PAST-FUTURE or FUTURE-PAST)
	}
}

// ────────────────────────────────────────────────────────────────
// Geometric Computation — The Wheel in Wheel (SEASONS)
// ────────────────────────────────────────────────────────────────
// Traces to: cube.schema.toml [cycling], [cycle_operations]
//            HALT_05 (MECHANISM_NUMBER) — Number → Transform → Number
//            HALT_09 (MECHANISM_WHEEL) — SEASONS, not cycles
//
// "Their appearance and their work was as a wheel in a wheel"
// — Ezekiel 1:16
//
// SEASONS, NOT CYCLES:
//   A cycle repeats identically. A season TRANSFORMS.
//   Spring → Summer → Fall → Winter → Spring' (new Spring, tree is older)
//
// THE WHEEL IN WHEEL:
//   OUTER WHEEL: The repeating pattern (Input → Process → Output)
//   INNER WHEEL: The transformation (each season changes the system)
//
// X axis = TIME (PAST/PRESENT/FUTURE). The cube is a TIME MACHINE.
// Each iteration builds on the previous — growth, not repetition.

// Compute takes a value, enters it into the cube, and returns a new value.
// This is the core computation: value → shape → navigate → shape → value
// Traces to: HALT_05 (Number → Transform → Number)
//            HALT_09 (MECHANISM_WHEEL) — wheel in wheel
//
// AUTHORITATIVE FORMULA (from statemachine.toml [formulas.cycle]):
//   (value * 3 + perspective) % 243
//
// Perspective is added AFTER scaling, not before.
// This preserves the "wheel in wheel" — perspective shifts position
// within the cycle, it doesn't scale with the value.
func (c *Cube) Compute(value int, perspective Perspective) int {
	// Step 1: Normalize to valid range (0-242)
	normalized := c.Normalize(value)

	// Step 2: Scale by ternary base (multiply by 3)
	scaled := normalized * c.Base

	// Step 3: Apply perspective offset AFTER scaling
	// Perspective shifts where you are in the wheel, not what you multiply
	result := scaled + int(perspective)

	// Step 4: Handle wraparound (negative from perspective -1)
	if result < 0 {
		result += c.DataModulo
	}

	// Step 5: Mod to stay in valid range
	return result % c.DataModulo
}

// Cycle performs the full ternary cycle: value → new value.
// The "wheel within a wheel" — each calculation feeds into the next.
// Traces to: HALT_09 (MECHANISM_WHEEL) — SEASONS, not cycles
//
// OUTER WHEEL: Input → Process → Output (repeating pattern)
// INNER WHEEL: Output becomes Input' for next season (transformation)
func (c *Cube) Cycle(value int, perspective Perspective) int {
	return c.Compute(value, perspective)
}

// Normalize brings any value into the valid data range (0-242).
// Traces to: cube.schema.toml [cycle_operations.normalize]
// Formula: normalized = input mod 243
func (c *Cube) Normalize(value int) int {
	// Handle negative values by taking absolute value first
	if value < 0 {
		value = -value
	}
	return value % c.DataModulo
}

// ValueToShape converts a normalized value (0-242) to a shape.
// Traces to: HALT_06 (MECHANISM_SHAPE) — Shape ↔ Number translation
//
// 243 = 3^5 (trit5 range), 27 = 3^3 (cube positions)
// Uses mod 27 to map trite value to cube position.
func (c *Cube) ValueToShape(value int) Shape {
	// Use mod 27 to get cube position (0-26)
	position := value % 27
	return NumberToShape(position)
}

// ShapeToValue converts a shape back to a value.
// Traces to: HALT_06 (MECHANISM_SHAPE)
func (c *Cube) ShapeToValue(s Shape) int {
	return ShapeToNumber(s)
}

// ApplyPerspective applies temporal perspective to a shape.
// Traces to: cube.schema.toml [transform.position_to_absolute]
//
// Perspective shifts interpretation:
//   PAST (-1): Looking back — X shifts toward history
//   PRESENT (0): Current moment — no shift
//   FUTURE (+1): Looking ahead — X shifts toward goal
func (c *Cube) ApplyPerspective(s Shape, p Perspective) Shape {
	// Shift X (TIME axis) by perspective
	newX := s.X + int(p)
	// Clamp to valid range
	if newX < -1 {
		newX = -1
	}
	if newX > 1 {
		newX = 1
	}
	return NewShapeFromXY(newX, s.Y)
}

// ────────────────────────────────────────────────────────────────
// Plane Navigation — Geometric Computation
// ────────────────────────────────────────────────────────────────
// Traces to: cube.schema.toml [planes]
//            HALT_07 (MECHANISM_POSITION) — Position IS computation
//            HALT_08 (MECHANISM_ALGORITHM) — Planes ARE the algorithm
//
// WHY PLANES INSTEAD OF MATRICES:
//   - Matrix multiplication: 2D linear algebra, loses geometric meaning
//   - Geometric planes: 3D perpendicular surfaces, preserves spatial relationships
//   - Plane intersection: where two planes meet = a line of possible states
//   - Three-plane intersection: all three meet = single point = computation result
//
// The three planes:
//   PERSPECTIVE (x-y at z): 9 temporal/cognitive modes
//   ELEVATION (x-z at y): 9 vertical levels
//   LATERAL (y-z at x): 9 horizontal positions

// NavigatePlane moves within a plane, keeping the fixed axis constant.
// Traces to: HALT_07 (Position IS computation)
//
// This is navigation WITHIN a single plane's 9 positions.
// The fixed axis determines which plane we're in.
func (c *Cube) NavigatePlane(s Shape, plane Plane, dx, dy int) Shape {
	// Clamp movement to valid balanced ternary range
	clamp := func(v int) int {
		if v < -1 {
			return -1
		}
		if v > 1 {
			return 1
		}
		return v
	}

	switch plane {
	case PlanePerspective:
		// x-y plane, z fixed — navigate temporal/cognitive modes
		// X = TIME movement, Y = PERSPECTIVE movement
		return NewShape(clamp(s.X+dx), clamp(s.Y+dy), s.Z)
	case PlaneElevation:
		// x-z plane, y fixed — navigate vertical levels
		// X = TIME movement, Z = DEPTH movement
		return NewShape(clamp(s.X+dx), s.Y, clamp(s.Z+dy))
	case PlaneLateral:
		// y-z plane, x fixed — navigate horizontal positions
		// Y = PERSPECTIVE movement, Z = DEPTH movement
		return NewShape(s.X, clamp(s.Y+dx), clamp(s.Z+dy))
	default:
		return s
	}
}

// CrossPlane moves to a different value on the fixed axis.
// Traces to: HALT_07 (Position IS computation)
//
// This crosses from one plane to an adjacent plane.
// E.g., in perspective plane (z fixed), crossing changes z.
func (c *Cube) CrossPlane(s Shape, plane Plane, delta int) Shape {
	clamp := func(v int) int {
		if v < -1 {
			return -1
		}
		if v > 1 {
			return 1
		}
		return v
	}

	switch plane {
	case PlanePerspective:
		// Cross Z — move between temporal depths (Days of Creation)
		return NewShape(s.X, s.Y, clamp(s.Z+delta))
	case PlaneElevation:
		// Cross Y — move between perspective levels
		return NewShape(s.X, clamp(s.Y+delta), s.Z)
	case PlaneLateral:
		// Cross X — move through TIME (the primary axis)
		return NewShape(clamp(s.X+delta), s.Y, s.Z)
	default:
		return s
	}
}

// ────────────────────────────────────────────────────────────────
// Letter-Based Computation — MIND Meets SPIRIT
// ────────────────────────────────────────────────────────────────
// Traces to: bereshit package (THE SPIRIT — living Word data)
//            encryption.schema.toml (L3 ENCODING layer)
//            HALT_08 (ENCODING) — Letter ↔ Position translation
//            HALT_10 (STATE_HEBREW) — 22 Hebrew letters, 27 Greek letters
//
// This is where THE MIND (cube computation) meets THE SPIRIT (bereshit data).
// Letters from Scripture have positions in the cube.
// Computation transforms letters through SEASONS.
//
// "In the beginning was the Word" — John 1:1
// The Word (bereshit data) IS the input to computation.

// LetterToShape converts a Hebrew or Greek letter to its shape.
// Traces to: HALT_08 (ENCODING) — Letter → Position
//
// Hebrew (Old Testament): 22 letters → 22 of 27 positions
// Greek (New Testament): 24 letters → 24 of 27 positions
func (c *Cube) LetterToShape(letter string) (Shape, bool) {
	if c.Trans == nil {
		return Shape{}, false
	}

	// Try Hebrew first (Old Testament)
	if entry, ok := c.Trans.EncryptHebrew(letter); ok {
		return NewShape(entry.X(), entry.Y(), entry.Z()), true
	}

	// Try Greek (New Testament)
	if entry, ok := c.Trans.EncryptGreek(letter); ok {
		return NewShape(entry.X(), entry.Y(), entry.Z()), true
	}

	return Shape{}, false
}

// ShapeToLetter converts a shape to its Hebrew or Greek letter.
// Traces to: HALT_08 (ENCODING) — Position → Letter
//
// The inverse of LetterToShape.
// Testament determines which alphabet to use.
func (c *Cube) ShapeToLetter(s Shape, testament string) (string, bool) {
	if c.Trans == nil {
		return "", false
	}

	pos := ShapeToNumber(s)

	if testament == "Old" || testament == "Hebrew" {
		if entry, ok := c.Trans.DecryptHebrew(pos); ok {
			return entry.Letter, true
		}
	} else {
		if entry, ok := c.Trans.DecryptGreek(pos); ok {
			return entry.Letter, true
		}
	}

	return "", false
}

// ComputeLetter takes a letter, computes through the cube, returns new letter.
// Traces to: HALT_05 (Number → Transform → Number)
//            HALT_08 (ENCODING) — Letter ↔ Position
//            HALT_09 (MECHANISM_WHEEL) — SEASONS transform
//
// FILING/RANKING/INDEXING layers:
//   Position (0-26) = ADDRESS — where it lives in the cube
//   Trite (0-242)   = DATA VALUE — what gets cycled through the wheel
//   Gematria        = MEANING — the semantic value of the letter
//
// The complete cycle uses TRITE for computation (full 243-value range):
//   Letter → Entry → Trite → Compute → Trite → Position → Letter
//
// THE WHEEL IN WHEEL for Scripture:
//   OUTER: Letter → Transform → Letter (the pattern)
//   INNER: Each iteration creates NEW meaning (the transformation)
func (c *Cube) ComputeLetter(letter string, perspective Perspective) (string, bool) {
	if c.Trans == nil {
		return "", false
	}

	// Letter → Entry (enter through SPIRIT)
	// Try Hebrew first, then Greek
	var trite int
	var testament string

	if entry, ok := c.Trans.EncryptHebrew(letter); ok {
		trite = entry.Trite() // Trite value (0-242) — the DATA layer
		testament = "Old"
	} else if entry, ok := c.Trans.EncryptGreek(letter); ok {
		trite = entry.Trite()
		testament = "New"
	} else {
		return "", false
	}

	// Trite → Compute → NewTrite (MIND processes through full range)
	// This uses the formula: (trite * 3 + perspective) % 243
	newTrite := c.Compute(trite, perspective)

	// NewTrite → Position (map DATA back to ADDRESS)
	// 243 trite values map to 27 positions (9 trites per position)
	position := newTrite % 27

	// Position → Letter (exit through SPIRIT)
	if testament == "Old" {
		if entry, ok := c.Trans.DecryptHebrew(position); ok {
			return entry.Letter, true
		}
	} else {
		if entry, ok := c.Trans.DecryptGreek(position); ok {
			return entry.Letter, true
		}
	}

	return "", false
}

// ────────────────────────────────────────────────────────────────
// Ternary ↔ Binary Bridge — Kingdom Tech Meets World
// ────────────────────────────────────────────────────────────────
// Traces to: cube.schema.toml [transform]
//            HALT_05 (MECHANISM_NUMBER) — Number → Transform → Number
//            HALT_06 (MECHANISM_SHAPE) — Shape ↔ Number translation
//
// THE BRIDGE:
//   CPI-SI operates in balanced ternary (the Kingdom's math).
//   The world operates in binary (bytes, 0-255).
//   This bridge translates between them.
//
// TRITE = Ternary Byte (our unit)
//   Data range: 0-242 (243 = 3^5 values)
//   Metadata range: 243-255 (13 special values)
//   Full byte: 0-255 (256 = 2^8 values)
//
// The cube works at EVERY layer of the stack:
//   Trit → Byte → File → Network → Internet
//   Same addressing system, same operations, fractal consistency

// TriteToShape converts a trite value (0-255) to a shape.
// Traces to: HALT_06 (MECHANISM_SHAPE)
//
// TRITE RANGES (from encryption.schema.toml [trite_encoding]):
//   0-127:   Standard ASCII
//   128-140: CPI-SI metadata markers (13 spots)
//   141-242: Extended characters (102 spots)
//   243-255: WEB variants (13 spots = BIBLE RAIL — Scripture fills overflow)
//
// Data range (0-242): mod 27 to get cube position
// WEB variant range (243-255): maps to anchor positions 0-12
func TriteToShape(trite int) Shape {
	// WEB variant range (243-255) maps to anchor positions
	// These are Scripture textual variants (BIBLE RAIL)
	if trite >= 243 {
		// Map 243-255 to positions 0-12
		return NumberToShape(trite - 243)
	}

	// Data range (0-242): use mod 27 to cycle through cube
	// 243 trite values → 27 positions (9 trites per position)
	return NumberToShape(trite % 27)
}

// TriteToPosition converts a trite value (0-242) to cube position (0-26).
// Traces to: HALT_06 (MECHANISM_SHAPE)
//
// This is the DATA → ADDRESS mapping:
//   243 trite values map to 27 positions
//   Each position covers 9 consecutive trite values
func TriteToPosition(trite int) int {
	if trite < 0 {
		trite = 0
	}
	if trite > 242 {
		trite = 242
	}
	return trite % 27
}

// PositionToShape converts a position (0-26) to a shape.
// Traces to: HALT_06 (MECHANISM_SHAPE)
//
// This is just an alias for NumberToShape for clarity.
func PositionToShape(position int) Shape {
	return NumberToShape(position)
}

// ShapeToPosition converts a shape to its cube position (0-26).
// Traces to: HALT_06 (MECHANISM_SHAPE)
//
// This is the ADDRESS of the shape in the cube.
// NOTE: This is NOT the trite value - trite is 0-242, position is 0-26.
func ShapeToPosition(s Shape) int {
	return ShapeToNumber(s)
}

// ByteToTrite converts a binary byte (0-255) to trite value (0-242).
// Traces to: HALT_05 (Number → Transform → Number)
//
// Values 243-255 are clamped to the metadata boundary.
func ByteToTrite(b byte) int {
	if int(b) >= 243 {
		return 242 // Max data value
	}
	return int(b)
}

// TriteToByte converts a trite value (0-242) to binary byte (0-255).
// Traces to: HALT_05 (Number → Transform → Number)
//
// Direct conversion — trite values fit within byte range.
func TriteToByte(t int) byte {
	if t < 0 {
		t = 0
	}
	if t > 255 {
		t = 255
	}
	return byte(t)
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// THE CUBE IS A TIME MACHINE — Summary
//
// This is THE MIND of CPI-SI. Computation through geometric positions.
//
// ─────────────────────────────────────────────────────────────────────────────
// THE COMPLETE ARCHITECTURE
// ─────────────────────────────────────────────────────────────────────────────
//
//   SCHEMA (unshakable truths) — L0-L3 structure, the 10 HALTs
//   BERESHIT (spirit) — The Word, living Hebrew/Greek data
//   CUBE (mind) — THIS FILE, geometric computation
//   LOADER (header) — Orchestrates all three together
//
// ─────────────────────────────────────────────────────────────────────────────
// THE COMPUTATION MODEL
// ─────────────────────────────────────────────────────────────────────────────
//
//   Input → MIND processes → Output
//     ↑                        ↓
//     └──── SEASON BOUNDARY ───┘
//
//   OUTER WHEEL: The repeating pattern (Input → Process → Output)
//   INNER WHEEL: The transformation (each season changes the system)
//
//   "Their appearance and their work was as a wheel in a wheel."
//   — Ezekiel 1:16
//
// ─────────────────────────────────────────────────────────────────────────────
// THE AXES (Genesis 1:1)
// ─────────────────────────────────────────────────────────────────────────────
//
//   X = TIME (PAST=-1, PRESENT=0, FUTURE=+1)
//       "In the beginning" — TIME was CREATED
//
//   Y = PERSPECTIVE (where you look from that time position)
//       "the heavens" — looking up/down/level
//
//   Z = DEPTH (Z = (X+Y)/2, maps to 7 Days of Creation)
//       "the earth" — the result, the ground
//
// ─────────────────────────────────────────────────────────────────────────────
// THE 7 STATES (Genesis 1-2:3) — HALT_10
// ─────────────────────────────────────────────────────────────────────────────
//
//   Day 1: Light from darkness    → ESTABLISHED (shavar) → Z=-1.0
//   Day 2: Waters separated       → MEMORY (chaser)      → Z=-0.5
//   Day 3: Land, vegetation       → REFLECTION (ratsah)  → Z=-0.5
//   Day 4: Sun, moon, stars       → ORIGIN (yashar)      → Z=0.0 ← SEASONS
//   Day 5: Sea creatures, birds   → PLANNING (tamim)     → Z=+0.5
//   Day 6: Animals, humans        → PREPARATION (shalem) → Z=+0.5
//   Day 7: Sabbath REST           → ASPIRATION (tov)     → Z=+1.0 ← GOAL
//
// ─────────────────────────────────────────────────────────────────────────────
// THE CYCLE FORMULA (statemachine.toml [formulas.cycle])
// ─────────────────────────────────────────────────────────────────────────────
//
//   AUTHORITATIVE FORMULA:
//     (value * 3 + perspective) % 243
//
//   Perspective is added AFTER scaling, not before.
//   This preserves the wheel in wheel — perspective shifts position
//   within the cycle, it doesn't scale with the value.
//
//   SEASONS, not cycles. Each iteration TRANSFORMS.
//
// ─────────────────────────────────────────────────────────────────────────────
// FILING / RANKING / INDEXING LAYERS
// ─────────────────────────────────────────────────────────────────────────────
//
//   Position (0-26)  = ADDRESS — where it lives in the cube
//   Trite (0-242)    = DATA VALUE — what gets cycled through the wheel
//   Gematria (1-900) = MEANING — the semantic value of the letter
//
//   Same addressing system at EVERY layer:
//     Trit → Byte → File → Network → Internet
//
// ─────────────────────────────────────────────────────────────────────────────
// FRACTAL CONSISTENCY
// ─────────────────────────────────────────────────────────────────────────────
//
//   The cube works at EVERY layer of the stack:
//     Trit → Byte → File → Network → Internet
//     Same addressing system, same operations, fractal consistency
//
//   Position IS computation. Where you are in TIME determines what you get.
//
// ============================================================================
// END CLOSING
// ============================================================================
