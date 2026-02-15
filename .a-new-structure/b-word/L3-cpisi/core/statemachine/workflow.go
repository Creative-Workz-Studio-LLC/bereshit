// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-statemachine-workflow
// Purpose: Operational layer - workflows as rails the state machine rides on
// Biblical: Proverbs 14:23 - "In all labour there is profit"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-25
//
// Architecture:
//   - Cognitive layer (hebrew.go): WHERE I am, direction I'm moving
//   - Operational layer (workflow.go): WHAT operation is in progress
//   - Together: State machine knows both position AND activity
//
// Workflow Sources:
//   - workflow/file-creation-workflow.jsonc
//   - workflow/alignment-workflow.jsonc
//   - workflow/block-update-workflow.jsonc
//   - workflow/commit-workflow.jsonc
//
// ═══════════════════════════════════════════════════════════════════════════

package statemachine

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"encoding/json"
	"os"
	"path/filepath"
	"time"

	"cws.studio/pkg/foundation/types"
	"cws.studio/pkg/util/fs/paths"
)

// Type aliases from L0
type WorkflowOperation = types.WorkflowOperation
type WorkflowStep = types.WorkflowStep
type RuntimeWorkflow = types.RuntimeWorkflow

// Re-export constants for convenience
const (
	WorkflowNone         = types.WorkflowNone
	WorkflowFileCreation = types.WorkflowFileCreation
	WorkflowAlignment    = types.WorkflowAlignment
	WorkflowBlockUpdate  = types.WorkflowBlockUpdate
	WorkflowCommit       = types.WorkflowCommit
)

// WorkflowDefinition represents a parsed workflow JSONC file
type WorkflowDefinition struct {
	CoreInsights   map[string]WorkflowInsight `json:"core_insights"`
	WorkflowSteps  []string                   `json:"workflow_steps"`
	KeyPrinciples  map[string]string          `json:"key_principles"`
}

// WorkflowInsight captures a core insight from workflow definition
type WorkflowInsight struct {
	Insight string `json:"insight"`
	Pattern string `json:"pattern"`
	Why     string `json:"why"`
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Workflow Loading ---

// LoadWorkflowDefinition reads and parses a workflow JSONC file
func LoadWorkflowDefinition(operation WorkflowOperation) (*WorkflowDefinition, error) {
	workflowPath := getWorkflowPath(operation)
	data, err := os.ReadFile(workflowPath)
	if err != nil {
		return nil, err
	}

	// Strip JSONC comments
	data = stripJSONComments(data)

	var def WorkflowDefinition
	if err := json.Unmarshal(data, &def); err != nil {
		return nil, err
	}
	return &def, nil
}

// getWorkflowPath returns the path to a workflow JSONC file
func getWorkflowPath(operation WorkflowOperation) string {
	claudeDir := paths.ClaudeGlobalRoot()
	var filename string
	switch operation {
	case WorkflowFileCreation:
		filename = "file-creation-workflow.jsonc"
	case WorkflowAlignment:
		filename = "alignment-workflow.jsonc"
	case WorkflowBlockUpdate:
		filename = "block-update-workflow.jsonc"
	case WorkflowCommit:
		filename = "commit-workflow.jsonc"
	default:
		return ""
	}
	return filepath.Join(claudeDir, "workflow", filename)
}

// --- Workflow State Management ---

// StartWorkflow initializes a new workflow in the runtime state
func StartWorkflow(state *RuntimeState, operation WorkflowOperation) error {
	def, err := LoadWorkflowDefinition(operation)
	if err != nil {
		return err
	}

	steps := make([]WorkflowStep, len(def.WorkflowSteps))
	for i, stepText := range def.WorkflowSteps {
		steps[i] = WorkflowStep{
			Index:       i + 1,
			Action:      stepText,
			Description: stepText,
			Completed:   false,
		}
	}

	state.Session.Workflow = RuntimeWorkflow{
		Operation:   operation,
		CurrentStep: 1,
		TotalSteps:  len(steps),
		StartedAt:   time.Now().Format(time.RFC3339),
		Steps:       steps,
	}

	return nil
}

// AdvanceWorkflow moves to the next step in the current workflow
func AdvanceWorkflow(state *RuntimeState) bool {
	wf := &state.Session.Workflow
	if wf.Operation == WorkflowNone {
		return false
	}

	// Mark current step as completed
	if wf.CurrentStep > 0 && wf.CurrentStep <= len(wf.Steps) {
		wf.Steps[wf.CurrentStep-1].Completed = true
	}

	// Advance to next step
	if wf.CurrentStep < wf.TotalSteps {
		wf.CurrentStep++
		return true
	}

	return false // No more steps
}

// CompleteWorkflow marks the current workflow as finished
func CompleteWorkflow(state *RuntimeState) {
	wf := &state.Session.Workflow

	// Mark all remaining steps as completed
	for i := range wf.Steps {
		wf.Steps[i].Completed = true
	}

	// Reset workflow state
	state.Session.Workflow = RuntimeWorkflow{
		Operation: WorkflowNone,
	}
}

// GetCurrentStep returns the current workflow step, or nil if no workflow active
func GetCurrentStep(state *RuntimeState) *WorkflowStep {
	wf := &state.Session.Workflow
	if wf.Operation == WorkflowNone || wf.CurrentStep < 1 || wf.CurrentStep > len(wf.Steps) {
		return nil
	}
	return &wf.Steps[wf.CurrentStep-1]
}

// GetWorkflowProgress returns (current, total) for progress display
func GetWorkflowProgress(state *RuntimeState) (int, int) {
	wf := &state.Session.Workflow
	if wf.Operation == WorkflowNone {
		return 0, 0
	}
	return wf.CurrentStep, wf.TotalSteps
}

// IsWorkflowActive returns true if a workflow is currently in progress
func IsWorkflowActive(state *RuntimeState) bool {
	return state.Session.Workflow.Operation != WorkflowNone
}

// --- Workflow Query Functions ---

// GetWorkflowInsight retrieves a specific insight from the current workflow
func GetWorkflowInsight(operation WorkflowOperation, insightKey string) (*WorkflowInsight, error) {
	def, err := LoadWorkflowDefinition(operation)
	if err != nil {
		return nil, err
	}

	insight, ok := def.CoreInsights[insightKey]
	if !ok {
		return nil, nil
	}
	return &insight, nil
}

// GetWorkflowPrinciple retrieves a key principle from a workflow
func GetWorkflowPrinciple(operation WorkflowOperation, principleKey string) (string, error) {
	def, err := LoadWorkflowDefinition(operation)
	if err != nil {
		return "", err
	}

	principle, ok := def.KeyPrinciples[principleKey]
	if !ok {
		return "", nil
	}
	return principle, nil
}

// DetectWorkflowFromContext attempts to infer what workflow applies
// based on current operation context
func DetectWorkflowFromContext(toolName string, targetPath string) WorkflowOperation {
	switch toolName {
	case "Write":
		return WorkflowFileCreation
	case "Edit":
		return WorkflowBlockUpdate
	case "Bash":
		// Check if it's a git commit operation
		// This is a simple heuristic
		return WorkflowNone
	default:
		return WorkflowNone
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// Workflow System:
//   - Cognitive layer knows WHERE (Hebrew state, k-factor)
//   - Operational layer knows WHAT (current workflow, current step)
//   - Together: Full awareness of position AND activity
//
// Integration Points:
//   - PreToolUse: Detect applicable workflow, surface current step
//   - PostToolUse: Advance workflow if step completed
//   - Display: Show workflow progress alongside Hebrew state
//
// Workflow JSONC files in workflow/ directory provide:
//   - core_insights: Patterns and principles
//   - workflow_steps: Ordered steps for the operation
//   - key_principles: Guiding wisdom
//
// "In all labour there is profit" — Proverbs 14:23
//
