// ============================================================================
// METADATA
// ============================================================================
// Package: sections/anchor
// Purpose: ANCHOR section - Kingdom grounding display
// Biblical: "In the beginning God created" - Genesis 1:1

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"

	"github.com/creativeworkzstudio/claude-global/pkg/util/term/display"
	"github.com/creativeworkzstudio/claude-global/pkg/core/statemachine"
	"github.com/creativeworkzstudio/claude-global/statusline/lib/features"
)

// ============================================================================
// BODY
// ============================================================================

// BuildAnchor returns the ANCHOR section
//
// Format: ⚓ Genesis 1:1
// Shows prominently at HALT, dimmed otherwise
func BuildAnchor(sm *statemachine.StateMachine, sessionID string) SectionResult {
	if sm == nil {
		return Empty()
	}

	cmd := sm.GetCurrentCommand()
	anchor := sm.Current.Anchor
	if anchor == "" {
		anchor = "Genesis 1:1"
	}

	// Show anchor more prominently at HALT, or based on reminder settings
	showAnchor := cmd == statemachine.CmdHalt || features.ShouldShowReminder(sessionID)
	if !showAnchor {
		return Empty()
	}

	anchorColor := display.Dim + display.Cyan
	if cmd == statemachine.CmdHalt {
		anchorColor = display.Cyan // Brighter at HALT
	}

	content := fmt.Sprintf("%s⚓ %s%s",
		anchorColor, anchor, display.Reset)

	return New(content, 8)
}

// BuildAnchorAlways returns anchor section regardless of command state
func BuildAnchorAlways(sm *statemachine.StateMachine) SectionResult {
	anchor := "Genesis 1:1"
	if sm != nil && sm.Current.Anchor != "" {
		anchor = sm.Current.Anchor
	}

	content := fmt.Sprintf("%s⚓ %s%s",
		display.Cyan, anchor, display.Reset)

	return New(content, 8)
}

// ============================================================================
// CLOSING
// ============================================================================
