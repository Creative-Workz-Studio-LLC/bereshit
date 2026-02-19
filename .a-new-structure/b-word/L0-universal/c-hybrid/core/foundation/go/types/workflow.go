//omni:code --go -library
//omni:key B-L0-foundation-types-workflow
//omni:version a-02.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-foundation-types-workflow
// Purpose: Universal workflow operation types for step-based execution
// Biblical: Proverbs 16:9 - "A man's heart deviseth his way"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2026-02-16
// Updated: 2026-02-18
//
// Extracted from L3-cpisi types.go — these are generic workflow tracking
// types usable by any layer. CPI-SI specific types (Choice, Halt, Tendency,
// Hebrew states, K:ALIGN) remain in L3.
//
// ============================================================================

package types

// ──────────────────────────────────────────────────────────────────────────
// Imports (in METADATA — Go requires all imports before any declarations)
// ──────────────────────────────────────────────────────────────────────────

import (
	"fmt"

	"creativeworkzstudio.com/bereshit/L0-universal/b-spiral/identity"
)

// Pragma — OmniCode file identity (I1-I4). Runtime-queryable via PragmaGet().
//
// NOTE: This file carries the identity vars for the entire types package.
// Go requires unique package-level names, and sessioncontext.go shares this
// package. The Pragma/Metadata here represent workflow.go specifically;
// sessioncontext.go registers its own identity in init_sessioncontext.go
// or documents its key in its METADATA comment block.
var Pragma = [][2]string{
	// I1: Core — key, format, from, at
	{"I1.key", "B-L0-foundation-types-workflow"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.go"},
	{"I1.at", "a-02.00"},
	// I2: Family — type, structure, subtype, role
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "foundation"},
	// I3: Instance — file, title, component, path, provides, brief
	{"I3.file", "workflow.go"},
	{"I3.title", "Workflow Operation Types"},
	{"I3.component", "types"},
	{"I3.path", "L0-universal/c-hybrid/core/foundation/go/types/workflow.go"},
	{"I3.provides", "Universal workflow operation types for step-based execution"},
	{"I3.brief", "Workflow operations, steps, runtime tracking, progress"},
	// I4: Architecture — layer, position, pattern
	{"I4.layer", "L0-universal"},
	{"I4.position", "c-hybrid/core/foundation"},
	{"I4.pattern", "value-types"},
}

// Metadata — OmniCode file metadata (C1-C7). Runtime-queryable via MetadataGet().
var Metadata = [][2]string{
	// C1: State — version, status, created, updated
	{"C1.version", "a-02.00"},
	{"C1.status", "Active"},
	{"C1.created", "2026-02-16"},
	{"C1.updated", "2026-02-18"},
	// C2: Attribution
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.architect", "Seanje Lenox-Wise"},
	{"C2.implementation", "Nova Dawn"},
	{"C2.copyright", "2025-2026 CreativeWorkzStudio LLC"},
	// C3: Grounding
	{"C3.scripture", "Proverbs 16:9 — A man's heart deviseth his way"},
	{"C3.principle", "Workflow steps trace a path — the LORD directs it"},
	{"C3.anchor", "Genesis 1:1 — In the beginning God created the heaven and the earth"},
	// C4: Dependencies
	{"C4.requires.stdlib", "fmt"},
	{"C4.requires.external", "none"},
	{"C4.requires.internal", "L0-universal/b-spiral/identity"},
	{"C4.consumers", "workflow engine, CLI tools, hooks"},
	{"C4.integration", "import creativeworkzstudio.com/bereshit/L0-universal/c-hybrid/core/foundation/go/types"},
	{"C4.if_missing", "callers must define their own workflow tracking types"},
	// C5: Intent
	{"C5.purpose", "Standardized workflow tracking across all layers"},
	{"C5.philosophy", "Steps are the source of truth — CurrentStep/TotalSteps are convenience"},
	// C6: Roadmap
	{"C6.current", "a-02.00 — Workflow ops, steps, runtime tracking, progress, Display"},
	{"C6.planned", "Workflow persistence, step dependencies, rollback"},
	{"C6.limitations", "No persistence — callers handle I/O"},
	// C7: Classification
	{"C7.tags", "workflow, operation, steps, progress, runtime, types"},
	{"C7.category", "Foundation"},
	{"C7.domain", "types"},
	{"C7.paradigm", "CPI-SI"},
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// --- Constants ---

// WorkflowOperation represents the type of operation being performed.
// Maps to workflow/*.jsonc files.
type WorkflowOperation string

const (
	WorkflowNone         WorkflowOperation = "none"          // No active workflow
	WorkflowFileCreation WorkflowOperation = "file-creation" // Creating new files from templates
	WorkflowAlignment    WorkflowOperation = "alignment"     // Aligning documentation with reality
	WorkflowBlockUpdate  WorkflowOperation = "block-update"  // Updating existing files block by block
	WorkflowCommit       WorkflowOperation = "commit"        // Git commit workflow
)

// --- Core Types ---

// WorkflowStep represents a step within a workflow.
type WorkflowStep struct {
	Index       int    `json:"index"`       // Step number (1-based)
	Action      string `json:"action"`      // What to do
	Description string `json:"description"` // Human-readable description
	Completed   bool   `json:"completed"`   // Whether step is done
}

// RuntimeWorkflow tracks current workflow state.
//
// Steps is the source of truth for completion — not CurrentStep/TotalSteps.
// CurrentStep indicates which step is actively being worked on.
// TotalSteps is derived from len(Steps) for convenience.
type RuntimeWorkflow struct {
	Operation   WorkflowOperation `json:"operation"`    // Current operation type
	CurrentStep int               `json:"current_step"` // Current step index (1-based)
	TotalSteps  int               `json:"total_steps"`  // Total steps in workflow
	StartedAt   string            `json:"started_at"`   // When workflow started (ISO 8601)
	Steps       []WorkflowStep    `json:"steps"`        // All steps with completion status
}

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
// Identity:
//   init, PragmaGet, MetadataGet
//
// Constructors:  NewRuntimeWorkflow
// Queries:       IsActive, IsComplete, Progress, CompletedCount, RemainingCount
// Mutators:      CompleteStep
//
// Flow: NewRuntimeWorkflow → IsActive → (CompleteStep → Progress) → IsComplete

// --- Identity Access ---

// init registers this package in the OmniCode identity registry.
func init() {
	key, ok := PragmaGet("I1.key")
	if !ok {
		return // identity not registerable without key
	}
	identity.Register(key, Pragma, Metadata)
}

// PragmaGet looks up a value from the Pragma identity.
// Returns the value and true if found, empty string and false otherwise.
func PragmaGet(key string) (string, bool) {
	for _, pair := range Pragma {
		if pair[0] == key {
			return pair[1], true
		}
	}
	return "", false
}

// MetadataGet looks up a value from the Metadata.
// Returns the value and true if found, empty string and false otherwise.
func MetadataGet(key string) (string, bool) {
	for _, pair := range Metadata {
		if pair[0] == key {
			return pair[1], true
		}
	}
	return "", false
}

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: No internal helpers — types package uses only stdlib]

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────

// NewRuntimeWorkflow creates a workflow from an operation and a list of steps.
//
// Steps are indexed 1-based. TotalSteps is derived from len(steps).
// CurrentStep starts at 1 (first step).
func NewRuntimeWorkflow(op WorkflowOperation, startedAt string, steps []WorkflowStep) RuntimeWorkflow {
	indexed := make([]WorkflowStep, len(steps))
	for i, s := range steps {
		indexed[i] = WorkflowStep{
			Index:       i + 1,
			Action:      s.Action,
			Description: s.Description,
			Completed:   s.Completed,
		}
	}
	return RuntimeWorkflow{
		Operation:   op,
		CurrentStep: 1,
		TotalSteps:  len(indexed),
		StartedAt:   startedAt,
		Steps:       indexed,
	}
}

// --- Queries ---

// IsNone returns true if this is the "none" operation.
func (op WorkflowOperation) IsNone() bool {
	return op == WorkflowNone
}

// IsActive returns whether a workflow is currently in progress.
func (w *RuntimeWorkflow) IsActive() bool {
	return !w.Operation.IsNone()
}

// IsComplete returns whether all steps are done.
//
// Steps slice is the source of truth — every step must be marked Completed.
// An empty steps slice with no active operation is considered complete.
func (w *RuntimeWorkflow) IsComplete() bool {
	if len(w.Steps) == 0 {
		return !w.IsActive()
	}
	for _, s := range w.Steps {
		if !s.Completed {
			return false
		}
	}
	return true
}

// Progress returns completion percentage (0-100).
//
// Based on the Steps slice, not CurrentStep/TotalSteps.
func (w *RuntimeWorkflow) Progress() int {
	if len(w.Steps) == 0 {
		return 0
	}
	return w.CompletedCount() * 100 / len(w.Steps)
}

// CompletedCount returns the number of completed steps.
func (w *RuntimeWorkflow) CompletedCount() int {
	count := 0
	for _, s := range w.Steps {
		if s.Completed {
			count++
		}
	}
	return count
}

// RemainingCount returns the number of incomplete steps.
func (w *RuntimeWorkflow) RemainingCount() int {
	return len(w.Steps) - w.CompletedCount()
}

// --- Mutators ---

// CompleteStep marks a step as completed by its 1-based index.
//
// Returns false if the index is out of range.
// Advances CurrentStep to the next incomplete step.
func (w *RuntimeWorkflow) CompleteStep(index int) bool {
	if index < 1 || index > len(w.Steps) {
		return false
	}
	w.Steps[index-1].Completed = true

	// Advance CurrentStep past completed steps
	for w.CurrentStep <= len(w.Steps) && w.Steps[w.CurrentStep-1].Completed {
		w.CurrentStep++
	}
	return true
}

// ──────────────────────────────────────────────────────────────────────────
// 4. Error Handling
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: Types package — CompleteStep returns bool for out-of-range]

// ──────────────────────────────────────────────────────────────────────────
// 5. Public APIs
// ──────────────────────────────────────────────────────────────────────────

// --- String Methods ---

// String returns the workflow operation name.
func (op WorkflowOperation) String() string {
	return string(op)
}

// String returns a human-readable representation of the step.
func (s WorkflowStep) String() string {
	mark := " "
	if s.Completed {
		mark = "x"
	}
	return fmt.Sprintf("[%s] %d. %s", mark, s.Index, s.Action)
}

// String returns a human-readable summary of the workflow.
//
//	"none (inactive)"
//	"file-creation: 2/5 steps (40%)"
//	"alignment: complete (5/5 steps)"
func (w *RuntimeWorkflow) String() string {
	if !w.IsActive() {
		return fmt.Sprintf("%s (inactive)", w.Operation)
	}
	if w.IsComplete() {
		return fmt.Sprintf("%s: complete (%d/%d steps)", w.Operation, w.CompletedCount(), len(w.Steps))
	}
	return fmt.Sprintf("%s: %d/%d steps (%d%%)", w.Operation, w.CompletedCount(), len(w.Steps), w.Progress())
}

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
// Testing: go test -v -run TestWorkflow ./L0-universal/.../types/
//
// Verify:
//   - WorkflowNone serializes as "none" (not empty string)
//   - IsComplete uses Steps slice, not CurrentStep > TotalSteps
//   - CompleteStep advances CurrentStep correctly
//   - Progress matches CompletedCount / len(Steps) * 100
//
// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library — imported, not executed.
// import "creativeworkzstudio.com/bereshit/L0-universal/c-hybrid/core/foundation/go/types"
//
// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// No resources to clean up. Pure value types.
//
// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// Never:
//   - Change WorkflowNone from "none" to empty string — breaks serde
//   - Use CurrentStep > TotalSteps for completion — use Steps slice
//   - Add filesystem I/O to this package — types only
//
// Careful:
//   - Adding new WorkflowOperation values — update Rust mirror
//   - Changing WorkflowStep fields — breaks JSON contract
//
// Safe:
//   - Adding new methods on existing types
//   - Adding new query functions
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// Note: Generic workflow types extracted from L3-cpisi. Steps slice is the
// source of truth for completion. CurrentStep/TotalSteps are convenience fields.
//
// Scripture: Proverbs 16:9 — "A man's heart deviseth his way: but the LORD
// directeth his steps."
//
// ============================================================================
// END CLOSING
// ============================================================================
