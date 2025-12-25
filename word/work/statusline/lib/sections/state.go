// ============================================================================
// METADATA
// ============================================================================
// Package: sections/state
// Purpose: STATE section - Anchor position + trajectory display
// Biblical: "In the beginning God created" - Genesis 1:1 (the anchor)

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"

	"creativeworkzstudio.com/bereshit/word/work/pkg/util/term/display"
	"creativeworkzstudio.com/bereshit/word/work/pkg/core/statemachine"
)

// ============================================================================
// BODY
// ============================================================================

// BuildState returns the STATE section (position + trajectory)
//
// Format: ⬡ MEANING (X,Y,Z) [OP] B.x→→
func BuildState(sm *statemachine.StateMachine, runtime *statemachine.RuntimeState) SectionResult {
	if sm == nil {
		return Empty()
	}

	pos := sm.Current.Position
	cmd := sm.GetCurrentCommand()
	op := string(pos.Op)
	cmdColor := statemachine.CommandColor(cmd)

	// Position info
	content := fmt.Sprintf("%s⬡ %s%s %s(%s,%s,%.1f) %s[%s]%s",
		cmdColor,
		pos.Meaning, display.Reset+display.Dim,
		display.Reset+cmdColor,
		pos.X.String(), pos.Y.String(), pos.Z,
		display.Bold, op, display.Reset)

	// Add trajectory indicator
	trajInfo := statemachine.GetRuntimeTrajectoryInfo(runtime)
	if trajInfo.HasInfo {
		content += fmt.Sprintf(" %s%s%s%s",
			trajInfo.Color, trajInfo.Section, trajInfo.Symbol, display.Reset)
	}

	return New(content, 2)
}

// BuildStateCompact returns a compact version for narrow terminals
func BuildStateCompact(sm *statemachine.StateMachine, runtime *statemachine.RuntimeState) SectionResult {
	if sm == nil {
		return Empty()
	}

	cmd := sm.GetCurrentCommand()
	cmdColor := statemachine.CommandColor(cmd)
	trajInfo := statemachine.GetRuntimeTrajectoryInfo(runtime)

	// Just show trajectory and operator
	content := fmt.Sprintf("%s%s%s%s",
		cmdColor, trajInfo.Section, trajInfo.Symbol, display.Reset)

	return New(content, 2)
}

// ============================================================================
// CLOSING
// ============================================================================
