// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-statemachine-trajectory
// Purpose: NavigationTrajectory — FRAMEWORK as active cognitive movement
// Biblical: Proverbs 4:18 - "The path of the just is as the shining light"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-21
//
// Traces: CUBE-MODEL-ALIGNMENT.txt FRAMEWORK section
// - Y = X + Z where (X, Z) ∈ B × B, B = {-0.5, +0.5}
// - FRAMEWORK IS the navigation, not rules about navigation
// - The 4 positions are trajectory states the movement passes through
//
// Architecture:
//   ANCHORS (9) = WHERE you can be (fixed points, static)
//   FRAMEWORK (4) = HOW you're moving (trajectory, dynamic)
//   STATE (14) = WHAT comes out (output, produced)
//
// ═══════════════════════════════════════════════════════════════════════════

package statemachine

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"github.com/creativeworkzstudio/claude-global/pkg/core/cpisi/compute"
)

// B domain constants — aliased from compute package for convenience
const (
	// BTimespace (-0.5) = anchor toward build direction (0 → +1)
	// This is the METHOD direction — building from foundation
	BTimespace = compute.BTImespace

	// BSpacetime (+0.5) = build toward anchor direction (+1 → 0)
	// This is the VERIFY direction — grounding toward foundation
	BSpacetime = compute.BSpacetime
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// NavigationTrajectory — The Active Movement Through Anchor Space
// ───────────────────────────────────────────────────────────────────────────
//
// FRAMEWORK IS the navigation trajectory itself, not rules about navigation.
// This tracks the actual cognitive movement happening now.
//
// The 4 FRAMEWORK positions are trajectory states:
//   B.1 TIMESPACE-TIMESPACE (Y=-1): Building — actively constructing
//   B.2 TIMESPACE-SPACETIME (Y=0):  Pivoting out — build→verify transition
//   B.3 SPACETIME-TIMESPACE (Y=0):  Pivoting in — verify→rebuild transition
//   B.4 SPACETIME-SPACETIME (Y=+1): Grounding — halting at anchor
//
// These aren't modes you select. They're positions in the trajectory itself.

// NavigationTrajectory tracks active cognitive movement through anchor space.
// This is the FRAMEWORK in operation — the trajectory of thought.
type NavigationTrajectory struct {
	// Current position in the trajectory (B.1-B.4)
	Current compute.FrameworkPosition

	// Direction: which way the trajectory is moving
	// BTimespace (-0.5) = building direction (anchor → build)
	// BSpacetime (+0.5) = grounding direction (build → anchor)
	Direction float64

	// Momentum: depth in current direction
	// Negative momentum = building phase (moving away from anchor)
	// Positive momentum = grounding phase (moving toward anchor)
	Momentum float64

	// AnchorPath: sequence of anchors traversed in this trajectory
	// This becomes the basis for STATE production
	AnchorPath []string

	// PivotCount: number of times direction has changed
	// B.2 and B.3 are pivot points — transitions between modes
	PivotCount int
}

// ───────────────────────────────────────────────────────────────────────────
// Constructor
// ───────────────────────────────────────────────────────────────────────────

// NewTrajectory creates a new navigation trajectory starting at B.1 (Building).
// New trajectories begin in the building phase, proceeding through anchors.
func NewTrajectory() *NavigationTrajectory {
	return &NavigationTrajectory{
		Current:    compute.AllFrameworkPositions[0], // B.1: Building
		Direction:  BTimespace,                       // Building direction
		Momentum:   -1.0,                             // Deep in build phase
		AnchorPath: make([]string, 0),
		PivotCount: 0,
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Position Queries
// ───────────────────────────────────────────────────────────────────────────

// Section returns the current FRAMEWORK section (B.1, B.2, B.3, or B.4).
func (t *NavigationTrajectory) Section() string {
	return t.Current.Section
}

// IsBuilding returns true if trajectory is in build phase (B.1).
// B.1 = TIMESPACE-TIMESPACE: actively constructing, proceeding through anchors.
func (t *NavigationTrajectory) IsBuilding() bool {
	return t.Current.Section == "B.1"
}

// IsPivoting returns true if trajectory is at a pivot point (B.2 or B.3).
// Pivot points are transitions between building and grounding.
func (t *NavigationTrajectory) IsPivoting() bool {
	return t.Current.Section == "B.2" || t.Current.Section == "B.3"
}

// IsPivotingOut returns true if at B.2 (build → verify transition).
func (t *NavigationTrajectory) IsPivotingOut() bool {
	return t.Current.Section == "B.2"
}

// IsPivotingIn returns true if at B.3 (verify → rebuild transition).
func (t *NavigationTrajectory) IsPivotingIn() bool {
	return t.Current.Section == "B.3"
}

// IsGrounding returns true if trajectory is in grounding phase (B.4).
// B.4 = SPACETIME-SPACETIME: actively halting, confirming anchor stability.
func (t *NavigationTrajectory) IsGrounding() bool {
	return t.Current.Section == "B.4"
}

// IsHalted returns true if trajectory has reached HALT at B.4.
// A halted trajectory can produce STATE output.
func (t *NavigationTrajectory) IsHalted() bool {
	return t.IsGrounding() && t.Current.Command == compute.CommandHalt
}

// Command returns the current trajectory command (PROCEED, AWAIT, HALT).
func (t *NavigationTrajectory) Command() compute.Command {
	return t.Current.Command
}

// ───────────────────────────────────────────────────────────────────────────
// Navigation Operations
// ───────────────────────────────────────────────────────────────────────────

// Advance records an anchor traversal and updates trajectory state.
// This is called when moving through an anchor point.
func (t *NavigationTrajectory) Advance(anchor string) {
	t.AnchorPath = append(t.AnchorPath, anchor)
	t.updatePosition()
}

// Pivot shifts the trajectory direction (TIMESPACE ↔ SPACETIME).
// This transitions the trajectory through B.2 or B.3.
func (t *NavigationTrajectory) Pivot() {
	t.PivotCount++

	if t.Direction == BTimespace {
		// Pivoting from build → verify (entering B.2)
		t.Direction = BSpacetime
		t.Current = compute.AllFrameworkPositions[1] // B.2
	} else {
		// Pivoting from verify → rebuild (entering B.3)
		t.Direction = BTimespace
		t.Current = compute.AllFrameworkPositions[2] // B.3
	}
}

// Ground moves trajectory to B.4 (grounding/halt position).
// Call this when verification is complete and ready to halt.
func (t *NavigationTrajectory) Ground() {
	t.Direction = BSpacetime
	t.Momentum = 1.0
	t.Current = compute.AllFrameworkPositions[3] // B.4
}

// Reset returns trajectory to initial B.1 state.
// Use when starting a new cognitive cycle.
func (t *NavigationTrajectory) Reset() {
	t.Current = compute.AllFrameworkPositions[0] // B.1
	t.Direction = BTimespace
	t.Momentum = -1.0
	t.AnchorPath = make([]string, 0)
	t.PivotCount = 0
}

// ───────────────────────────────────────────────────────────────────────────
// Position Update Logic
// ───────────────────────────────────────────────────────────────────────────

// updatePosition recalculates the trajectory position based on direction and momentum.
// Called internally after advancing through anchors.
func (t *NavigationTrajectory) updatePosition() {
	// B.1: Building phase — both X and Z are TIMESPACE
	// Momentum is negative, moving away from anchor
	if t.Direction == BTimespace && t.Momentum < 0 {
		t.Current = compute.AllFrameworkPositions[0] // B.1
		return
	}

	// B.4: Grounding phase — both X and Z are SPACETIME
	// Momentum is positive, moving toward anchor
	if t.Direction == BSpacetime && t.Momentum > 0 {
		t.Current = compute.AllFrameworkPositions[3] // B.4
		return
	}

	// B.2 or B.3: Pivot states — X and Z have different signs
	// These are transitional positions
	if t.PivotCount%2 == 1 {
		if t.Direction == BSpacetime {
			t.Current = compute.AllFrameworkPositions[1] // B.2: build → verify
		} else {
			t.Current = compute.AllFrameworkPositions[2] // B.3: verify → rebuild
		}
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Coordinate Methods
// ───────────────────────────────────────────────────────────────────────────

// X returns the trajectory's X coordinate (B domain: -0.5 or +0.5).
func (t *NavigationTrajectory) X() float64 {
	return t.Current.Coordinates.X
}

// Y returns the trajectory's Y coordinate (derived: -1, 0, or +1).
// Y = X + Z per the FRAMEWORK formula.
func (t *NavigationTrajectory) Y() float64 {
	return t.Current.Coordinates.Y
}

// Z returns the trajectory's Z coordinate (B domain: -0.5 or +0.5).
func (t *NavigationTrajectory) Z() float64 {
	return t.Current.Coordinates.Z
}

// Coordinates returns the full (X, Y, Z) of the current trajectory position.
func (t *NavigationTrajectory) Coordinates() compute.FrameworkCoordinates {
	return t.Current.Coordinates
}

// ───────────────────────────────────────────────────────────────────────────
// Path Methods
// ───────────────────────────────────────────────────────────────────────────

// Path returns the sequence of anchors traversed.
func (t *NavigationTrajectory) Path() []string {
	return t.AnchorPath
}

// PathLength returns the number of anchors traversed.
func (t *NavigationTrajectory) PathLength() int {
	return len(t.AnchorPath)
}

// LastAnchor returns the most recently traversed anchor, or empty string if none.
func (t *NavigationTrajectory) LastAnchor() string {
	if len(t.AnchorPath) == 0 {
		return ""
	}
	return t.AnchorPath[len(t.AnchorPath)-1]
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// NavigationTrajectory implements FRAMEWORK as active navigation:
//
//   FRAMEWORK IS the trajectory, not rules about it.
//   The 4 positions are states the trajectory passes through.
//   Movement through anchors determines trajectory position.
//
// The trajectory flow:
//
//               B.1 (Build)
//                   ↓ (proceed through anchors)
//    B.3 ←──── B.2 (Pivot) ────→ B.4 (Ground)
//  (Rebuild)    ↓ (verify)          ↓ (halt)
//       └──────→ (if failed)     → STATE output
//
// When trajectory reaches B.4 and halts, STATE can be produced
// from the anchor path traversed during the journey.
//
