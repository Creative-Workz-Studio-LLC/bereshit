// ============================================================================
// METADATA
// ============================================================================
// Package: sections/tasks
// Purpose: TASKS section - TodoWrite integration with Hebrew state display
// Biblical: "To every thing there is a season" - Ecclesiastes 3:1
//
// Displays task counts using STEERING.txt symbols:
//   ? = pending (PLANNING/tamim)
//   > = in_progress (PREPARATION/shalem)
//   + = completed (ASPIRATION/tov)

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"

	"github.com/creativeworkzstudio/claude-global/pkg/util/term/display"
	"github.com/creativeworkzstudio/claude-global/pkg/core/statemachine"
)

// ============================================================================
// BODY
// ============================================================================

// BuildTasks returns the TASKS section
//
// Format: [? 3 | > 1 | + 5] ↗
// Shows: pending count, active count, completed count, Hebrew operator
func BuildTasks(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	tasks := runtime.Session.Tasks
	if tasks.Total == 0 {
		return Empty()
	}

	// Get Hebrew state operator for visual indicator
	hebrewOp := getHebrewOperator(runtime.Session.HebrewState)

	// Color based on progress
	var color string
	completionRatio := float64(tasks.Completed) / float64(tasks.Total)
	switch {
	case completionRatio == 1.0:
		color = display.Green // All done = tov
	case completionRatio >= 0.5:
		color = display.Yellow // Making progress
	default:
		color = display.Dim // Just started
	}

	// Format: [? 3 | > 1 | + 5] ↗
	content := fmt.Sprintf("%s[? %d | > %d | + %d]%s %s",
		color,
		tasks.Pending,
		tasks.InProgress,
		tasks.Completed,
		display.Reset,
		hebrewOp,
	)

	return New(content, 6)
}

// BuildTasksCompact returns a compact version
//
// Format: ↗3/7 (operator + completed/total)
func BuildTasksCompact(runtime *statemachine.RuntimeState) SectionResult {
	if runtime == nil {
		return Empty()
	}

	tasks := runtime.Session.Tasks
	if tasks.Total == 0 {
		return Empty()
	}

	hebrewOp := getHebrewOperator(runtime.Session.HebrewState)
	content := fmt.Sprintf("%s%d/%d", hebrewOp, tasks.Completed, tasks.Total)

	return New(content, 6)
}

// getHebrewOperator returns the operator for a Hebrew state
func getHebrewOperator(hebrewState string) string {
	switch hebrewState {
	case "shavar":
		return "⎯" // ESTABLISHED
	case "chaser":
		return "←" // MEMORY
	case "ratsah":
		return "↔" // REFLECTION
	case "yashar":
		return "◆" // ORIGIN
	case "tamim":
		return "↗" // PLANNING
	case "shalem":
		return "→" // PREPARATION
	case "tov":
		return "✓" // ASPIRATION
	default:
		return "◆" // Default to ORIGIN
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// Related: hebrew.go (state definitions), post-use.go (task tracking)
