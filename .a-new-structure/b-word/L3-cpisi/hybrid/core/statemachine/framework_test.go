// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-statemachine-framework-test
// Purpose: Verify FRAMEWORK crystallization - TOML → Code cascade
// Biblical: Proverbs 11:1 - "A false balance is abomination to the LORD"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-21
//
// ═══════════════════════════════════════════════════════════════════════════

package statemachine

import (
	"testing"

	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/core/compute"
)

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ─────────────────────────────────────────────────────────────────────────────
// KNOWN TRUTH: From CUBE-MODEL-ALIGNMENT.txt crystallization
// These values are the SOURCE OF TRUTH - if code doesn't match, code is wrong
// ─────────────────────────────────────────────────────────────────────────────

// ExpectedFramework defines what we KNOW the 4 FRAMEWORK positions must be.
// Source: CUBE-MODEL-ALIGNMENT.txt - crystallized mathematical truth
// Formula: Y = X + Z where (X, Z) ∈ B × B, B = {-0.5, +0.5}
type ExpectedFramework struct {
	Section  string
	Name     string
	X        float64
	Y        float64
	Z        float64
	Type     string  // PURE or COMPOUND
	Command  string  // proceed, await, halt
	Operator string
	Meaning  string
}

var expectedPositions = []ExpectedFramework{
	{
		Section:  "B.1",
		Name:     "TIMESPACE-TIMESPACE",
		X:        -0.5,
		Y:        -1.0, // -0.5 + -0.5 = -1
		Z:        -0.5,
		Type:     "PURE",
		Command:  "proceed",
		Operator: "-->",
		Meaning:  "Foundation Rules",
	},
	{
		Section:  "B.2",
		Name:     "TIMESPACE-SPACETIME",
		X:        -0.5,
		Y:        0.0, // -0.5 + 0.5 = 0
		Z:        0.5,
		Type:     "COMPOUND",
		Command:  "await",
		Operator: "->|",
		Meaning:  "Method to Verify",
	},
	{
		Section:  "B.3",
		Name:     "SPACETIME-TIMESPACE",
		X:        0.5,
		Y:        0.0, // 0.5 + -0.5 = 0
		Z:        -0.5,
		Type:     "COMPOUND",
		Command:  "await",
		Operator: "|->",
		Meaning:  "Verify to Method",
	},
	{
		Section:  "B.4",
		Name:     "SPACETIME-SPACETIME",
		X:        0.5,
		Y:        1.0, // 0.5 + 0.5 = 1
		Z:        0.5,
		Type:     "PURE",
		Command:  "halt",
		Operator: "|",
		Meaning:  "Grounding Rules",
	},
}

// ─────────────────────────────────────────────────────────────────────────────
// TESTS: Assert expected truth against implementation
// ─────────────────────────────────────────────────────────────────────────────

func TestFrameworkPositionCount(t *testing.T) {
	// KNOWN: B × B = 4 positions, no more, no less
	const expectedCount = 4

	actual := len(compute.AllFrameworkPositions)
	if actual != expectedCount {
		t.Errorf("FRAMEWORK must have exactly %d positions (B × B = 4), got %d",
			expectedCount, actual)
	}
}

func TestFrameworkB1Coordinates(t *testing.T) {
	// KNOWN: B.1 TIMESPACE-TIMESPACE = (-0.5, -1, -0.5)
	expected := expectedPositions[0]

	actual, found := findBySection(compute.AllFrameworkPositions, "B.1")
	if !found {
		t.Fatal("B.1 position not found in implementation")
	}

	assertCoordinates(t, "B.1", expected.X, expected.Y, expected.Z,
		actual.Coordinates.X, actual.Coordinates.Y, actual.Coordinates.Z)
	assertString(t, "B.1 Name", expected.Name, actual.Name)
	assertString(t, "B.1 Type", expected.Type, actual.Type)
	assertString(t, "B.1 Operator", expected.Operator, actual.Operator)
}

func TestFrameworkB2Coordinates(t *testing.T) {
	// KNOWN: B.2 TIMESPACE-SPACETIME = (-0.5, 0, +0.5)
	expected := expectedPositions[1]

	actual, found := findBySection(compute.AllFrameworkPositions, "B.2")
	if !found {
		t.Fatal("B.2 position not found in implementation")
	}

	assertCoordinates(t, "B.2", expected.X, expected.Y, expected.Z,
		actual.Coordinates.X, actual.Coordinates.Y, actual.Coordinates.Z)
	assertString(t, "B.2 Name", expected.Name, actual.Name)
	assertString(t, "B.2 Type", expected.Type, actual.Type)
	assertString(t, "B.2 Operator", expected.Operator, actual.Operator)
}

func TestFrameworkB3Coordinates(t *testing.T) {
	// KNOWN: B.3 SPACETIME-TIMESPACE = (+0.5, 0, -0.5)
	expected := expectedPositions[2]

	actual, found := findBySection(compute.AllFrameworkPositions, "B.3")
	if !found {
		t.Fatal("B.3 position not found in implementation")
	}

	assertCoordinates(t, "B.3", expected.X, expected.Y, expected.Z,
		actual.Coordinates.X, actual.Coordinates.Y, actual.Coordinates.Z)
	assertString(t, "B.3 Name", expected.Name, actual.Name)
	assertString(t, "B.3 Type", expected.Type, actual.Type)
	assertString(t, "B.3 Operator", expected.Operator, actual.Operator)
}

func TestFrameworkB4Coordinates(t *testing.T) {
	// KNOWN: B.4 SPACETIME-SPACETIME = (+0.5, +1, +0.5)
	expected := expectedPositions[3]

	actual, found := findBySection(compute.AllFrameworkPositions, "B.4")
	if !found {
		t.Fatal("B.4 position not found in implementation")
	}

	assertCoordinates(t, "B.4", expected.X, expected.Y, expected.Z,
		actual.Coordinates.X, actual.Coordinates.Y, actual.Coordinates.Z)
	assertString(t, "B.4 Name", expected.Name, actual.Name)
	assertString(t, "B.4 Type", expected.Type, actual.Type)
	assertString(t, "B.4 Operator", expected.Operator, actual.Operator)
}

func TestFrameworkFormulaYEqualsXPlusZ(t *testing.T) {
	// KNOWN: Y = X + Z is the FRAMEWORK formula
	// Test each position against the KNOWN result, not computed result
	for _, expected := range expectedPositions {
		actual, found := findBySection(compute.AllFrameworkPositions, expected.Section)
		if !found {
			t.Errorf("%s: position not found", expected.Section)
			continue
		}

		// The KNOWN Y value must match implementation
		if actual.Coordinates.Y != expected.Y {
			t.Errorf("%s: Y must equal %.1f (from X=%.1f + Z=%.1f), got %.1f",
				expected.Section, expected.Y, expected.X, expected.Z, actual.Coordinates.Y)
		}
	}
}

func TestChainCountEquals27(t *testing.T) {
	// KNOWN: ANCHORS(9) + FRAMEWORK(4) + STATE(14) = 27 = 3³
	const expectedAnchors = 9
	const expectedFramework = 4
	const expectedState = 14
	const expectedTotal = 27

	sm := NewStateMachine()
	actualAnchors := len(sm.Positions)
	actualFramework := len(compute.AllFrameworkPositions)

	if actualAnchors != expectedAnchors {
		t.Errorf("ANCHORS must have %d positions, got %d", expectedAnchors, actualAnchors)
	}
	if actualFramework != expectedFramework {
		t.Errorf("FRAMEWORK must have %d positions, got %d", expectedFramework, actualFramework)
	}

	total := actualAnchors + actualFramework + expectedState
	if total != expectedTotal {
		t.Errorf("Chain must equal 27 (3³): %d + %d + %d = %d, got %d",
			actualAnchors, actualFramework, expectedState, expectedTotal, total)
	}
}

func TestFrameworkPureVsCompound(t *testing.T) {
	// KNOWN: B.1 and B.4 are PURE (same X and Z sign)
	//        B.2 and B.3 are COMPOUND (different X and Z sign)
	pureExpected := map[string]bool{"B.1": true, "B.4": true}
	compoundExpected := map[string]bool{"B.2": true, "B.3": true}

	for _, pos := range compute.AllFrameworkPositions {
		if pureExpected[pos.Section] {
			if pos.Type != "PURE" {
				t.Errorf("%s must be PURE, got %s", pos.Section, pos.Type)
			}
		}
		if compoundExpected[pos.Section] {
			if pos.Type != "COMPOUND" {
				t.Errorf("%s must be COMPOUND, got %s", pos.Section, pos.Type)
			}
		}
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// TRAJECTORY TESTS: Assert FRAMEWORK as active navigation
// ─────────────────────────────────────────────────────────────────────────────

func TestTrajectoryStartsAtB1(t *testing.T) {
	// KNOWN: New trajectory starts at B.1 (Building phase)
	trajectory := NewTrajectory()

	if trajectory.Section() != "B.1" {
		t.Errorf("New trajectory must start at B.1, got %s", trajectory.Section())
	}

	if !trajectory.IsBuilding() {
		t.Error("New trajectory must be in building phase")
	}

	// Verify B.1 coordinates: (-0.5, -1, -0.5)
	coords := trajectory.Coordinates()
	if coords.X != -0.5 {
		t.Errorf("B.1 X must be -0.5, got %.1f", coords.X)
	}
	if coords.Y != -1.0 {
		t.Errorf("B.1 Y must be -1.0, got %.1f", coords.Y)
	}
	if coords.Z != -0.5 {
		t.Errorf("B.1 Z must be -0.5, got %.1f", coords.Z)
	}
}

func TestTrajectoryAdvanceThroughAnchors(t *testing.T) {
	trajectory := NewTrajectory()

	// KNOWN: Advancing should record anchor path
	trajectory.Advance("PAST-PRESENT")
	trajectory.Advance("PRESENT-PRESENT")
	trajectory.Advance("FUTURE-PRESENT")

	path := trajectory.Path()
	if len(path) != 3 {
		t.Errorf("Path should have 3 anchors, got %d", len(path))
	}

	expectedPath := []string{"PAST-PRESENT", "PRESENT-PRESENT", "FUTURE-PRESENT"}
	for i, expected := range expectedPath {
		if path[i] != expected {
			t.Errorf("Path[%d]: expected %q, got %q", i, expected, path[i])
		}
	}

	if trajectory.LastAnchor() != "FUTURE-PRESENT" {
		t.Errorf("LastAnchor should be FUTURE-PRESENT, got %q", trajectory.LastAnchor())
	}
}

func TestTrajectoryPivot(t *testing.T) {
	trajectory := NewTrajectory()

	// KNOWN: Pivot from B.1 should enter B.2 (build → verify transition)
	if trajectory.Direction != BTimespace {
		t.Errorf("Initial direction must be BTimespace (-0.5), got %.1f", trajectory.Direction)
	}

	trajectory.Pivot()

	if trajectory.Direction != BSpacetime {
		t.Errorf("After pivot, direction must be BSpacetime (+0.5), got %.1f", trajectory.Direction)
	}

	if trajectory.Section() != "B.2" {
		t.Errorf("After pivot, section must be B.2, got %s", trajectory.Section())
	}

	if !trajectory.IsPivotingOut() {
		t.Error("After pivot from building, should be pivoting out (B.2)")
	}

	// KNOWN: Second pivot should enter B.3 (verify → rebuild)
	trajectory.Pivot()

	if trajectory.Section() != "B.3" {
		t.Errorf("After second pivot, section must be B.3, got %s", trajectory.Section())
	}

	if !trajectory.IsPivotingIn() {
		t.Error("After second pivot, should be pivoting in (B.3)")
	}
}

func TestTrajectoryGround(t *testing.T) {
	trajectory := NewTrajectory()

	// KNOWN: Ground should move to B.4
	trajectory.Ground()

	if trajectory.Section() != "B.4" {
		t.Errorf("After ground, section must be B.4, got %s", trajectory.Section())
	}

	if !trajectory.IsGrounding() {
		t.Error("After ground, should be in grounding phase")
	}

	if !trajectory.IsHalted() {
		t.Error("After ground, trajectory should be halted")
	}

	// Verify B.4 coordinates: (+0.5, +1, +0.5)
	coords := trajectory.Coordinates()
	if coords.X != 0.5 {
		t.Errorf("B.4 X must be +0.5, got %.1f", coords.X)
	}
	if coords.Y != 1.0 {
		t.Errorf("B.4 Y must be +1.0, got %.1f", coords.Y)
	}
	if coords.Z != 0.5 {
		t.Errorf("B.4 Z must be +0.5, got %.1f", coords.Z)
	}
}

func TestTrajectoryReset(t *testing.T) {
	trajectory := NewTrajectory()

	// Advance and pivot
	trajectory.Advance("TEST-ANCHOR")
	trajectory.Pivot()
	trajectory.Ground()

	// KNOWN: Reset should return to B.1 with empty path
	trajectory.Reset()

	if trajectory.Section() != "B.1" {
		t.Errorf("After reset, section must be B.1, got %s", trajectory.Section())
	}

	if len(trajectory.Path()) != 0 {
		t.Errorf("After reset, path should be empty, got %d items", len(trajectory.Path()))
	}

	if trajectory.PivotCount != 0 {
		t.Errorf("After reset, pivot count should be 0, got %d", trajectory.PivotCount)
	}
}

func TestStateMachineHasTrajectory(t *testing.T) {
	sm := NewStateMachine()

	// KNOWN: StateMachine must initialize with trajectory
	if sm.Trajectory == nil {
		t.Fatal("StateMachine must have Trajectory initialized")
	}

	if sm.TrajectorySection() != "B.1" {
		t.Errorf("StateMachine trajectory must start at B.1, got %s", sm.TrajectorySection())
	}
}

func TestStateMachineTrajectoryBlocksAtB4(t *testing.T) {
	sm := NewStateMachine()

	// Ground the trajectory (move to B.4)
	sm.GroundTrajectory()

	if !sm.IsTrajectoryHalted() {
		t.Error("After grounding, trajectory must be halted")
	}

	// KNOWN: Movement should be blocked at B.4
	err := sm.MoveToAnchor("PAST-PRESENT")
	if err == nil {
		t.Error("MoveToAnchor should fail when trajectory is at B.4 (halted)")
	}

	// Verify it's a trajectory error about B.4
	trajErr, ok := err.(*TrajectoryError)
	if !ok {
		t.Errorf("Error should be *TrajectoryError, got %T", err)
	} else if trajErr.Section != "B.4" {
		t.Errorf("Error section should be B.4, got %s", trajErr.Section)
	}
}

func TestStateMachineTrajectoryRecordsPath(t *testing.T) {
	sm := NewStateMachine()

	// Move through anchors
	err := sm.MoveToAnchor("PAST-PRESENT")
	if err != nil {
		t.Fatalf("MoveToAnchor failed: %v", err)
	}

	err = sm.MoveToAnchor("PRESENT-PRESENT")
	if err != nil {
		t.Fatalf("MoveToAnchor failed: %v", err)
	}

	// KNOWN: Path should record anchors traversed
	path := sm.TrajectoryPath()
	if len(path) != 2 {
		t.Errorf("Path should have 2 anchors, got %d", len(path))
	}

	if path[0] != "PAST-PRESENT" {
		t.Errorf("Path[0] should be PAST-PRESENT, got %s", path[0])
	}
	if path[1] != "PRESENT-PRESENT" {
		t.Errorf("Path[1] should be PRESENT-PRESENT, got %s", path[1])
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────

func findBySection(positions []compute.FrameworkPosition, section string) (compute.FrameworkPosition, bool) {
	for _, p := range positions {
		if p.Section == section {
			return p, true
		}
	}
	return compute.FrameworkPosition{}, false
}

func assertCoordinates(t *testing.T, label string, expX, expY, expZ, actX, actY, actZ float64) {
	t.Helper()
	if actX != expX {
		t.Errorf("%s X: expected %.1f, got %.1f", label, expX, actX)
	}
	if actY != expY {
		t.Errorf("%s Y: expected %.1f, got %.1f", label, expY, actY)
	}
	if actZ != expZ {
		t.Errorf("%s Z: expected %.1f, got %.1f", label, expZ, actZ)
	}
}

func assertString(t *testing.T, label, expected, actual string) {
	t.Helper()
	if actual != expected {
		t.Errorf("%s: expected %q, got %q", label, expected, actual)
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
