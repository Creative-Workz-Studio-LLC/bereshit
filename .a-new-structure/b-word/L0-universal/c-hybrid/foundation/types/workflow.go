//omni:code --go -library
//omni:key B-L0-foundation-types-workflow
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-foundation-types-workflow
// Purpose: Universal workflow operation types for step-based execution
// Biblical: Proverbs 16:9 - "A man's heart deviseth his way"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-16
//
// Extracted from L3-cpisi types.go — these are generic workflow tracking
// types usable by any layer. CPI-SI specific types (Choice, Halt, Tendency,
// Hebrew states, K:ALIGN) remain in L3.
//
// ============================================================================

package types

// ============================================================================
// SETUP
// ============================================================================

// WorkflowOperation represents the type of operation being performed.
// Maps to workflow/*.jsonc files.
type WorkflowOperation string

const (
	WorkflowNone         WorkflowOperation = ""              // No active workflow
	WorkflowFileCreation WorkflowOperation = "file-creation" // Creating new files from templates
	WorkflowAlignment    WorkflowOperation = "alignment"     // Aligning documentation with reality
	WorkflowBlockUpdate  WorkflowOperation = "block-update"  // Updating existing files block by block
	WorkflowCommit       WorkflowOperation = "commit"        // Git commit workflow
)

// WorkflowStep represents a step within a workflow
type WorkflowStep struct {
	Index       int    `json:"index"`       // Step number (1-based)
	Action      string `json:"action"`      // What to do
	Description string `json:"description"` // Human-readable description
	Completed   bool   `json:"completed"`   // Whether step is done
}

// RuntimeWorkflow tracks current workflow state
type RuntimeWorkflow struct {
	Operation   WorkflowOperation `json:"operation"`    // Current operation type
	CurrentStep int               `json:"current_step"` // Current step index (1-based)
	TotalSteps  int               `json:"total_steps"`  // Total steps in workflow
	StartedAt   string            `json:"started_at"`   // When workflow started
	Steps       []WorkflowStep    `json:"steps"`        // All steps with completion status
}

// ============================================================================
// BODY
// ============================================================================

// IsActive returns whether a workflow is currently in progress
func (w *RuntimeWorkflow) IsActive() bool {
	return w.Operation != WorkflowNone
}

// IsComplete returns whether all steps are done
func (w *RuntimeWorkflow) IsComplete() bool {
	return w.CurrentStep > w.TotalSteps && w.TotalSteps > 0
}

// Progress returns completion percentage (0-100)
func (w *RuntimeWorkflow) Progress() int {
	if w.TotalSteps == 0 {
		return 0
	}
	completed := 0
	for _, s := range w.Steps {
		if s.Completed {
			completed++
		}
	}
	return completed * 100 / w.TotalSteps
}

// ============================================================================
// CLOSING
// ============================================================================
// Generic workflow types — extracted from L3, usable by all layers.
