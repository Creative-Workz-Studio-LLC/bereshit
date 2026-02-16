//omni:code --go -library
//omni:key B-L3-cognition-workflow
//omni:version a-01.00
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

	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/core/statemachine"
	"creativeworkzstudio.com/bereshit/L3-cpisi/hybrid/foundation/types"
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
	opDesc := workflowOperationDescription(wf.Operation)
	return fmt.Sprintf("**%s** — Step %d/%d\n*%s*",
		opName, wf.CurrentStep, wf.TotalSteps, opDesc)
}

// formatWorkflowFull returns comprehensive workflow context
func formatWorkflowFull(wf types.RuntimeWorkflow, def *statemachine.WorkflowDefinition) string {
	var parts []string

	opName := workflowOperationName(wf.Operation)

	// Header with what we're doing and progress
	// Format: ✏️ Editing Blocks — Step 3/8
	parts = append(parts, fmt.Sprintf("**%s** — Step %d/%d",
		opName, wf.CurrentStep, wf.TotalSteps))

	// Current step (what to do NOW) - make it prominent
	if wf.CurrentStep > 0 && wf.CurrentStep <= len(wf.Steps) {
		currentStep := wf.Steps[wf.CurrentStep-1]
		parts = append(parts, fmt.Sprintf("**NOW:** %s", currentStep.Description))
	}

	// Next step only (keep it focused)
	upcoming := getUpcomingSteps(wf, 1)
	if len(upcoming) > 0 {
		parts = append(parts, fmt.Sprintf("**NEXT:** %s", upcoming[0]))
	}

	// Key principle (if relevant)
	if len(def.KeyPrinciples) > 0 {
		for key, value := range def.KeyPrinciples {
			parts = append(parts, fmt.Sprintf("*(%s: %s)*", key, value))
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

// workflowOperationName returns human-readable workflow name with description
func workflowOperationName(op types.WorkflowOperation) string {
	switch op {
	case types.WorkflowFileCreation:
		return "📄 Creating File"
	case types.WorkflowAlignment:
		return "🎯 Aligning"
	case types.WorkflowBlockUpdate:
		return "✏️ Editing Blocks"
	case types.WorkflowCommit:
		return "📦 Committing"
	default:
		return "⚙️ Working"
	}
}

// workflowOperationDescription returns what the workflow means
func workflowOperationDescription(op types.WorkflowOperation) string {
	switch op {
	case types.WorkflowFileCreation:
		return "Creating a new file from template"
	case types.WorkflowAlignment:
		return "Aligning code/docs with standards"
	case types.WorkflowBlockUpdate:
		return "Editing file block-by-block"
	case types.WorkflowCommit:
		return "Preparing git commit"
	default:
		return "Processing..."
	}
}

// ============================================================================
// CLOSING
// ============================================================================
