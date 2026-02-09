// ============================================================================
// METADATA
// ============================================================================
// Package: sections/workflow
// Purpose: WORKFLOW section - displays current operation and step progress
// Biblical: Proverbs 14:23 - "In all labour there is profit"
//
// Displays workflow progress:
//   [file-creation 3/7] - operation name with step progress
//   [block-update 5/6] ◆ - with Hebrew operator when combined

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"

	"cws.studio/pkg/util/term/display"
	"cws.studio/pkg/core/statemachine"
)

// ============================================================================
// BODY
// ============================================================================

// BuildWorkflow returns the WORKFLOW section
//
// Format: [file-creation 3/7]
// Shows: operation name, current step / total steps
func BuildWorkflow(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	wf := runtime.Session.Workflow
	if wf.Operation == "" {
		return Empty()
	}

	// Color based on progress
	var color string
	if wf.TotalSteps > 0 {
		completionRatio := float64(wf.CurrentStep) / float64(wf.TotalSteps)
		switch {
		case completionRatio >= 0.8:
			color = display.Green // Almost done
		case completionRatio >= 0.5:
			color = display.Yellow // Making progress
		default:
			color = display.Cyan // Just started
		}
	} else {
		color = display.Dim
	}

	// Format: [operation step/total]
	content := fmt.Sprintf("%s[%s %d/%d]%s",
		color,
		wf.Operation,
		wf.CurrentStep,
		wf.TotalSteps,
		display.Reset,
	)

	return New(content, 6)
}

// BuildWorkflowCompact returns a compact version
//
// Format: ⚙3/7 (gear + step/total)
func BuildWorkflowCompact(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	wf := runtime.Session.Workflow
	if wf.Operation == "" {
		return Empty()
	}

	content := fmt.Sprintf("⚙%d/%d", wf.CurrentStep, wf.TotalSteps)
	return New(content, 4)
}

// BuildWorkflowWithHebrew combines workflow progress with Hebrew state
//
// Format: [block-update 5/6] ◆
func BuildWorkflowWithHebrew(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	wf := runtime.Session.Workflow
	if wf.Operation == "" {
		return Empty()
	}

	hebrewOp := getHebrewOperator(runtime.Session.HebrewState)

	content := fmt.Sprintf("[%s %d/%d] %s",
		wf.Operation,
		wf.CurrentStep,
		wf.TotalSteps,
		hebrewOp,
	)

	return New(content, 6)
}

// ============================================================================
// CLOSING
// ============================================================================
// Related: workflow.go (state machine), post-use.go (workflow tracking)
