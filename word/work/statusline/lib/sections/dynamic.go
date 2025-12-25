// ============================================================================
// METADATA
// ============================================================================
// Package: sections/dynamic
// Purpose: Dynamic section that changes based on command state
// Biblical: "To every thing there is a season" - Ecclesiastes 3:1
//
// HALT:    Shows accumulated totals, terminal state info
// AWAIT:   Shows what's pending, at-position awareness
// PROCEED: Shows flow metrics, forward motion indicators

package sections

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"strings"

	"creativeworkzstudio.com/bereshit/word/work/pkg/util/term/display"
	"creativeworkzstudio.com/bereshit/word/work/pkg/core/statemachine"
	"creativeworkzstudio.com/bereshit/word/work/pkg/foundation/types"
	"creativeworkzstudio.com/bereshit/word/work/statusline/lib/session"
)

// ============================================================================
// BODY
// ============================================================================

// BuildDynamic returns command-aware content
func BuildDynamic(ctx types.SessionContext, sm *statemachine.StateMachine) SectionResult {
	if sm == nil {
		return Empty()
	}

	cmd := sm.GetCurrentCommand()
	pos := sm.Current.Position
	var parts []string

	switch cmd {
	case statemachine.CmdHalt:
		parts = buildHaltSection(ctx, pos)
	case statemachine.CmdAwait:
		parts = buildAwaitSection(ctx, pos)
	case statemachine.CmdProceed:
		parts = buildProceedSection(ctx, pos)
	}

	if len(parts) == 0 {
		return Empty()
	}

	return New(strings.Join(parts, " "), 7)
}

// buildHaltSection shows accumulated totals at terminal state
func buildHaltSection(ctx types.SessionContext, pos statemachine.Position) []string {
	var parts []string

	// Lines modified
	linesDisplay := session.GetLinesModifiedDisplay(ctx)
	if linesDisplay.HasInfo {
		parts = append(parts, fmt.Sprintf("%s∑ %d lines%s",
			display.Red, linesDisplay.TotalLines, display.Reset))
	}

	// Duration with API efficiency
	if ctx.Cost.TotalDurationMS > 0 {
		dur := formatDuration(ctx.Cost.TotalDurationMS)
		apiPct := ctx.APIEfficiency()
		parts = append(parts, fmt.Sprintf("%s⏱ %s (API:%.0f%%)%s",
			display.Red, dur, apiPct, display.Reset))
	}

	// Final cost at HALT
	if ctx.Cost.TotalCostUSD > 0 {
		parts = append(parts, fmt.Sprintf("%s💰 $%.2f%s",
			display.Red, ctx.Cost.TotalCostUSD, display.Reset))
	}

	return parts
}

// buildAwaitSection shows what we're waiting for
func buildAwaitSection(ctx types.SessionContext, pos statemachine.Position) []string {
	var parts []string

	// At position
	parts = append(parts, fmt.Sprintf("%s@ %s%s",
		display.Yellow, pos.Meaning, display.Reset))

	// Pending lines
	linesDisplay := session.GetLinesModifiedDisplay(ctx)
	if linesDisplay.HasInfo {
		parts = append(parts, fmt.Sprintf("%s%d pending%s",
			display.Yellow, linesDisplay.TotalLines, display.Reset))
	}

	// Time spent waiting
	if ctx.Cost.TotalDurationMS > 0 {
		dur := formatDuration(ctx.Cost.TotalDurationMS)
		parts = append(parts, fmt.Sprintf("%s⏳ %s%s",
			display.Yellow, dur, display.Reset))
	}

	return parts
}

// buildProceedSection shows flow metrics
func buildProceedSection(ctx types.SessionContext, pos statemachine.Position) []string {
	var parts []string

	// Lines (with flow arrow)
	linesDisplay := session.GetLinesModifiedDisplay(ctx)
	if linesDisplay.HasInfo {
		parts = append(parts, fmt.Sprintf("%s→ %d lines%s",
			display.Green, linesDisplay.TotalLines, display.Reset))
	}

	// Duration with API efficiency
	if ctx.Cost.TotalDurationMS > 0 {
		dur := formatDuration(ctx.Cost.TotalDurationMS)
		apiPct := ctx.APIEfficiency()
		parts = append(parts, fmt.Sprintf("%s⏱ %s (API:%.0f%%)%s",
			display.Green, dur, apiPct, display.Reset))
	}

	// Cost (flowing state - show if significant)
	if ctx.Cost.TotalCostUSD > 0.01 {
		parts = append(parts, fmt.Sprintf("%s💰 $%.2f%s",
			display.Green, ctx.Cost.TotalCostUSD, display.Reset))
	}

	return parts
}

// formatDuration formats milliseconds into human-readable duration
func formatDuration(ms int) string {
	if ms < 1000 {
		return "<1s"
	}

	seconds := ms / 1000
	minutes := seconds / 60
	hours := minutes / 60

	if hours > 0 {
		return fmt.Sprintf("%dh%dm", hours, minutes%60)
	}
	if minutes > 0 {
		return fmt.Sprintf("%dm%ds", minutes, seconds%60)
	}
	return fmt.Sprintf("%ds", seconds)
}

// ============================================================================
// CLOSING
// ============================================================================
