// ============================================================================
// METADATA
// ============================================================================
// Package: cognition
// File: workflow.go
// Purpose: Workflow context injection - brings workflow content into prompts
// Biblical: "In all labour there is profit" - Proverbs 14:23
//
// When a workflow is active (detected by state machine), this injects
// the workflow steps into Claude's context so it knows WHAT to do next.
// The state machine tracks WHERE we are; this provides the HOW.

package cognition

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"strings"

	"github.com/creativeworkzstudio/claude-global/pkg/core/statemachine"
	"github.com/creativeworkzstudio/claude-global/pkg/foundation/types"
)

// ============================================================================
// BODY
// ============================================================================

// WorkflowContext returns context for active workflow
// If no workflow is active, returns empty string
func WorkflowContext(runtime *types.RuntimeState) string {
	if runtime == nil {
		return ""
	}

	wf := runtime.Session.Workflow
	if wf.Operation == types.WorkflowNone {
		return ""
	}

	// Load the full workflow definition for principles/insights
	def, err := statemachine.LoadWorkflowDefinition(wf.Operation)
	if err != nil {
		// Fallback to just state-based guidance
		return formatWorkflowState(wf)
	}

	return formatWorkflowFull(wf, def)
}

// formatWorkflowState returns basic workflow state when definition unavailable
func formatWorkflowState(wf types.RuntimeWorkflow) string {
	opName := workflowOperationName(wf.Operation)
	return fmt.Sprintf("**Active Workflow:** %s (Step %d/%d)",
		opName, wf.CurrentStep, wf.TotalSteps)
}

// formatWorkflowFull returns comprehensive workflow context
func formatWorkflowFull(wf types.RuntimeWorkflow, def *statemachine.WorkflowDefinition) string {
	var parts []string

	opName := workflowOperationName(wf.Operation)

	// Header with current position
	parts = append(parts, fmt.Sprintf("**Workflow:** %s [%d/%d]",
		opName, wf.CurrentStep, wf.TotalSteps))

	// Current step (what to do NOW)
	if wf.CurrentStep > 0 && wf.CurrentStep <= len(wf.Steps) {
		currentStep := wf.Steps[wf.CurrentStep-1]
		parts = append(parts, fmt.Sprintf("**Current Step:** %s", currentStep.Description))
	}

	// Next 2 upcoming steps (what's coming)
	upcoming := getUpcomingSteps(wf, 2)
	if len(upcoming) > 0 {
		parts = append(parts, fmt.Sprintf("**Next:** %s", strings.Join(upcoming, " → ")))
	}

	// Key principle for this workflow (first one, brief)
	if len(def.KeyPrinciples) > 0 {
		for key, value := range def.KeyPrinciples {
			// Just include first principle to avoid context bloat
			parts = append(parts, fmt.Sprintf("**Principle (%s):** %s", key, value))
			break
		}
	}

	return strings.Join(parts, "\n")
}

// getUpcomingSteps returns the next N steps after current
func getUpcomingSteps(wf types.RuntimeWorkflow, count int) []string {
	var upcoming []string
	for i := wf.CurrentStep; i < wf.TotalSteps && len(upcoming) < count; i++ {
		if i < len(wf.Steps) {
			// Truncate long step descriptions
			desc := wf.Steps[i].Description
			if len(desc) > 50 {
				desc = desc[:47] + "..."
			}
			upcoming = append(upcoming, desc)
		}
	}
	return upcoming
}

// workflowOperationName returns human-readable workflow name
func workflowOperationName(op types.WorkflowOperation) string {
	switch op {
	case types.WorkflowFileCreation:
		return "File Creation"
	case types.WorkflowAlignment:
		return "Alignment"
	case types.WorkflowBlockUpdate:
		return "Block Update"
	case types.WorkflowCommit:
		return "Commit"
	default:
		return "Unknown"
	}
}

// ============================================================================
// CLOSING
// ============================================================================
