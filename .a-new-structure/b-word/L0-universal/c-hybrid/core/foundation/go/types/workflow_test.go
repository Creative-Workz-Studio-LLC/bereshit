//omni:code --go -demo-test
//omni:key B-L0-foundation-types-workflow-test
//omni:version a-02.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-foundation-types-workflow-test
// Purpose: Comprehensive tests for workflow operation types
// Biblical: Proverbs 16:9 — "A man's heart deviseth his way"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2026-02-18
// Updated: 2026-02-18

package types

// ──────────────────────────────────────────────────────────────────────────
// Imports (in METADATA — Go requires all imports before any declarations)
// ──────────────────────────────────────────────────────────────────────────

import (
	"encoding/json"
	"testing"
)

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// section-order: [s01]-imports
// (imports declared in METADATA section — Go language constraint)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Org Chart
// ──────────────────────────────────────────────────────────────────────────
//
// Test Functions (18 tests in Cv zone):
//   WorkflowOperation:  TestWorkflowNone_SerializesAsNone,
//                       TestWorkflowNone_DeserializesFromNone,
//                       TestWorkflowOperation_String, TestWorkflowOperation_IsNone
//   RuntimeWorkflow:    TestNewRuntimeWorkflow, TestNewRuntimeWorkflow_Empty,
//                       TestRuntimeWorkflow_IsActive,
//                       TestRuntimeWorkflow_IsComplete_UsesSteps,
//                       TestRuntimeWorkflow_IsComplete_EmptyNoOperation,
//                       TestRuntimeWorkflow_IsComplete_EmptyWithOperation,
//                       TestRuntimeWorkflow_Progress,
//                       TestRuntimeWorkflow_ProgressEmpty,
//                       TestRuntimeWorkflow_CompletedAndRemaining,
//                       TestRuntimeWorkflow_CompleteStep_OutOfRange,
//                       TestRuntimeWorkflow_CompleteStep_AdvancesCurrentStep,
//                       TestRuntimeWorkflow_String
//   WorkflowStep:       TestWorkflowStep_String
//   JSON:               TestRuntimeWorkflow_JSONRoundtrip

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: No test helpers needed — all tests use package constructors directly]

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: Test logic lives in Cv zone — test functions ARE the operations]

// ──────────────────────────────────────────────────────────────────────────
// 4. Error Handling
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: Tests validate errors from main package, no custom test errors]

// ──────────────────────────────────────────────────────────────────────────
// 5. Test Functions
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: All Test* functions in Cv zone below]

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────
//
// 18 tests covering:
//   - WorkflowOperation: serialization, deserialization, String, IsNone
//   - RuntimeWorkflow: construction, IsActive, IsComplete (steps-based),
//     Progress, CompletedCount, RemainingCount, CompleteStep edge cases
//   - WorkflowStep: string representation
//   - JSON roundtrip: RuntimeWorkflow marshal/unmarshal

// --- WorkflowOperation Tests ---

func TestWorkflowNone_SerializesAsNone(t *testing.T) {
	type wrapper struct {
		Op WorkflowOperation `json:"op"`
	}
	w := wrapper{Op: WorkflowNone}
	data, err := json.Marshal(w)
	if err != nil {
		t.Fatal(err)
	}
	expected := `{"op":"none"}`
	if string(data) != expected {
		t.Errorf("got %s, want %s", string(data), expected)
	}
}

func TestWorkflowNone_DeserializesFromNone(t *testing.T) {
	type wrapper struct {
		Op WorkflowOperation `json:"op"`
	}
	var w wrapper
	if err := json.Unmarshal([]byte(`{"op":"none"}`), &w); err != nil {
		t.Fatal(err)
	}
	if w.Op != WorkflowNone {
		t.Errorf("got %q, want %q", w.Op, WorkflowNone)
	}
}

func TestWorkflowOperation_String(t *testing.T) {
	tests := []struct {
		op   WorkflowOperation
		want string
	}{
		{WorkflowNone, "none"},
		{WorkflowFileCreation, "file-creation"},
		{WorkflowAlignment, "alignment"},
		{WorkflowBlockUpdate, "block-update"},
		{WorkflowCommit, "commit"},
	}
	for _, tc := range tests {
		if got := tc.op.String(); got != tc.want {
			t.Errorf("String() = %q, want %q", got, tc.want)
		}
	}
}

func TestWorkflowOperation_IsNone(t *testing.T) {
	if !WorkflowNone.IsNone() {
		t.Error("WorkflowNone.IsNone() should be true")
	}
	if WorkflowFileCreation.IsNone() {
		t.Error("WorkflowFileCreation.IsNone() should be false")
	}
}

// --- RuntimeWorkflow Tests ---

func TestNewRuntimeWorkflow(t *testing.T) {
	steps := []WorkflowStep{
		{Action: "first", Description: "do first"},
		{Action: "second", Description: "do second"},
		{Action: "third", Description: "do third"},
	}
	wf := NewRuntimeWorkflow(WorkflowFileCreation, "2026-02-18T00:00:00Z", steps)

	if wf.Operation != WorkflowFileCreation {
		t.Errorf("Operation = %s, want file-creation", wf.Operation)
	}
	if wf.CurrentStep != 1 {
		t.Errorf("CurrentStep = %d, want 1", wf.CurrentStep)
	}
	if wf.TotalSteps != 3 {
		t.Errorf("TotalSteps = %d, want 3", wf.TotalSteps)
	}
	if len(wf.Steps) != 3 {
		t.Fatalf("Steps length = %d, want 3", len(wf.Steps))
	}
	// Verify re-indexing
	for i, s := range wf.Steps {
		if s.Index != i+1 {
			t.Errorf("Step[%d].Index = %d, want %d", i, s.Index, i+1)
		}
	}
}

func TestNewRuntimeWorkflow_Empty(t *testing.T) {
	wf := NewRuntimeWorkflow(WorkflowNone, "", nil)

	if wf.TotalSteps != 0 {
		t.Errorf("TotalSteps = %d, want 0", wf.TotalSteps)
	}
	if len(wf.Steps) != 0 {
		t.Errorf("Steps length = %d, want 0", len(wf.Steps))
	}
}

func TestRuntimeWorkflow_IsActive(t *testing.T) {
	active := RuntimeWorkflow{Operation: WorkflowCommit}
	inactive := RuntimeWorkflow{Operation: WorkflowNone}

	if !active.IsActive() {
		t.Error("should be active with non-none operation")
	}
	if inactive.IsActive() {
		t.Error("should be inactive with none operation")
	}
}

func TestRuntimeWorkflow_IsComplete_UsesSteps(t *testing.T) {
	wf := NewRuntimeWorkflow(WorkflowAlignment, "now", []WorkflowStep{
		{Action: "a"}, {Action: "b"},
	})

	if wf.IsComplete() {
		t.Error("should not be complete with uncompleted steps")
	}

	wf.CompleteStep(1)
	if wf.IsComplete() {
		t.Error("should not be complete with one remaining step")
	}

	wf.CompleteStep(2)
	if !wf.IsComplete() {
		t.Error("should be complete when all steps done")
	}
}

func TestRuntimeWorkflow_IsComplete_EmptyNoOperation(t *testing.T) {
	wf := RuntimeWorkflow{Operation: WorkflowNone}
	if !wf.IsComplete() {
		t.Error("empty workflow with no operation should be complete")
	}
}

func TestRuntimeWorkflow_IsComplete_EmptyWithOperation(t *testing.T) {
	wf := RuntimeWorkflow{Operation: WorkflowCommit}
	if wf.IsComplete() {
		t.Error("empty workflow with active operation should not be complete")
	}
}

func TestRuntimeWorkflow_Progress(t *testing.T) {
	wf := NewRuntimeWorkflow(WorkflowCommit, "now", []WorkflowStep{
		{Action: "a"}, {Action: "b"}, {Action: "c"}, {Action: "d"},
	})

	if wf.Progress() != 0 {
		t.Errorf("Progress = %d, want 0", wf.Progress())
	}

	wf.CompleteStep(1)
	if wf.Progress() != 25 {
		t.Errorf("Progress = %d, want 25", wf.Progress())
	}

	wf.CompleteStep(2)
	wf.CompleteStep(3)
	if wf.Progress() != 75 {
		t.Errorf("Progress = %d, want 75", wf.Progress())
	}

	wf.CompleteStep(4)
	if wf.Progress() != 100 {
		t.Errorf("Progress = %d, want 100", wf.Progress())
	}
}

func TestRuntimeWorkflow_ProgressEmpty(t *testing.T) {
	wf := RuntimeWorkflow{}
	if wf.Progress() != 0 {
		t.Errorf("Progress on empty = %d, want 0", wf.Progress())
	}
}

func TestRuntimeWorkflow_CompletedAndRemaining(t *testing.T) {
	wf := NewRuntimeWorkflow(WorkflowCommit, "now", []WorkflowStep{
		{Action: "a"}, {Action: "b"}, {Action: "c"},
	})

	if wf.CompletedCount() != 0 {
		t.Errorf("CompletedCount = %d, want 0", wf.CompletedCount())
	}
	if wf.RemainingCount() != 3 {
		t.Errorf("RemainingCount = %d, want 3", wf.RemainingCount())
	}

	wf.CompleteStep(2)
	if wf.CompletedCount() != 1 {
		t.Errorf("CompletedCount = %d, want 1", wf.CompletedCount())
	}
	if wf.RemainingCount() != 2 {
		t.Errorf("RemainingCount = %d, want 2", wf.RemainingCount())
	}
}

func TestRuntimeWorkflow_CompleteStep_OutOfRange(t *testing.T) {
	wf := NewRuntimeWorkflow(WorkflowCommit, "now", []WorkflowStep{
		{Action: "a"},
	})

	if wf.CompleteStep(0) {
		t.Error("CompleteStep(0) should return false")
	}
	if wf.CompleteStep(2) {
		t.Error("CompleteStep(2) should return false for 1-step workflow")
	}
	if wf.CompleteStep(-1) {
		t.Error("CompleteStep(-1) should return false")
	}
}

func TestRuntimeWorkflow_CompleteStep_AdvancesCurrentStep(t *testing.T) {
	wf := NewRuntimeWorkflow(WorkflowCommit, "now", []WorkflowStep{
		{Action: "a"}, {Action: "b"}, {Action: "c"},
	})

	if wf.CurrentStep != 1 {
		t.Fatalf("initial CurrentStep = %d, want 1", wf.CurrentStep)
	}

	wf.CompleteStep(1)
	if wf.CurrentStep != 2 {
		t.Errorf("after completing step 1, CurrentStep = %d, want 2", wf.CurrentStep)
	}

	// Complete step 3 (skip 2)
	wf.CompleteStep(3)
	// CurrentStep should still be 2 (first incomplete)
	if wf.CurrentStep != 2 {
		t.Errorf("after completing 1,3, CurrentStep = %d, want 2", wf.CurrentStep)
	}

	// Complete step 2 — now all done
	wf.CompleteStep(2)
	if wf.CurrentStep != 4 {
		t.Errorf("after completing all, CurrentStep = %d, want 4 (past end)", wf.CurrentStep)
	}
}

func TestRuntimeWorkflow_String(t *testing.T) {
	// Inactive
	inactive := RuntimeWorkflow{Operation: WorkflowNone}
	if got := inactive.String(); got != "none (inactive)" {
		t.Errorf("inactive String() = %q", got)
	}

	// In progress
	wf := NewRuntimeWorkflow(WorkflowCommit, "now", []WorkflowStep{
		{Action: "a"}, {Action: "b"}, {Action: "c"}, {Action: "d"}, {Action: "e"},
	})
	wf.CompleteStep(1)
	wf.CompleteStep(2)
	expected := "commit: 2/5 steps (40%)"
	if got := wf.String(); got != expected {
		t.Errorf("in-progress String() = %q, want %q", got, expected)
	}

	// Complete
	wf.CompleteStep(3)
	wf.CompleteStep(4)
	wf.CompleteStep(5)
	expected = "commit: complete (5/5 steps)"
	if got := wf.String(); got != expected {
		t.Errorf("complete String() = %q, want %q", got, expected)
	}
}

// --- WorkflowStep Tests ---

func TestWorkflowStep_String(t *testing.T) {
	incomplete := WorkflowStep{Index: 1, Action: "build", Completed: false}
	if got := incomplete.String(); got != "[ ] 1. build" {
		t.Errorf("incomplete step String() = %q", got)
	}

	complete := WorkflowStep{Index: 2, Action: "test", Completed: true}
	if got := complete.String(); got != "[x] 2. test" {
		t.Errorf("complete step String() = %q", got)
	}
}

// --- JSON Roundtrip Tests ---

func TestRuntimeWorkflow_JSONRoundtrip(t *testing.T) {
	wf := NewRuntimeWorkflow(WorkflowFileCreation, "2026-02-18T00:00:00Z", []WorkflowStep{
		{Action: "create", Description: "Create file"},
		{Action: "fill", Description: "Fill content"},
	})
	wf.CompleteStep(1)

	data, err := json.Marshal(wf)
	if err != nil {
		t.Fatal(err)
	}

	var decoded RuntimeWorkflow
	if err := json.Unmarshal(data, &decoded); err != nil {
		t.Fatal(err)
	}

	if decoded.Operation != wf.Operation {
		t.Errorf("Operation = %s, want %s", decoded.Operation, wf.Operation)
	}
	if decoded.CurrentStep != wf.CurrentStep {
		t.Errorf("CurrentStep = %d, want %d", decoded.CurrentStep, wf.CurrentStep)
	}
	if len(decoded.Steps) != len(wf.Steps) {
		t.Fatalf("Steps length = %d, want %d", len(decoded.Steps), len(wf.Steps))
	}
	if !decoded.Steps[0].Completed {
		t.Error("Step 1 should be completed after roundtrip")
	}
	if decoded.Steps[1].Completed {
		t.Error("Step 2 should not be completed after roundtrip")
	}
}

//
// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// go test -v -run TestWorkflow ./L0-universal/c-hybrid/core/foundation/go/types/
// go test -v -run TestRuntimeWorkflow ./L0-universal/c-hybrid/core/foundation/go/types/
//
// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// No resources to clean up. Pure value-type tests.
//
// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// Never:
//   - Skip WorkflowNone serialization test (serde contract)
//   - Remove IsComplete steps-based verification (correctness guarantee)
//
// Careful:
//   - Changing expected String() output (format is a contract)
//   - Modifying CompleteStep advancement tests (behavioral contract)
//
// Safe:
//   - Adding new test cases for edge conditions
//   - Adding tests for new WorkflowOperation values
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// Note: Test suite for workflow types in types package — verifies
// WorkflowOperation serialization, RuntimeWorkflow construction/queries/
// mutations, WorkflowStep string representation, and JSON roundtrip.
// Steps slice is the source of truth for completion, not CurrentStep.
//
// Scripture: Proverbs 16:9 — "A man's heart deviseth his way: but the
// LORD directeth his steps."
//
// ============================================================================
// END CLOSING
// ============================================================================
