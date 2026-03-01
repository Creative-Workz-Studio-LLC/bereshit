//omni:key B-claude-pkg-cognition-summary
//omni:code --go -library
//omni:version a-01.00

package cognition

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-claude-pkg-cognition-summary
// Purpose: Generates closing statistics and session reflection summaries
//

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

import (
	"fmt"
	"strings"
	"time"

	"cws.studio/pkg/core/health"
	"cws.studio/pkg/core/statemachine"
	"cws.studio/pkg/foundation/types"
	"cws.studio/pkg/util/term/display"
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 22. Public APIs
// ──────────────────────────────────────────────────────────────────────────

// GenerateSessionSummary builds a rich visual summary of the session
func GenerateSessionSummary(state *types.RuntimeState) string {
	if state == nil {
		return ""
	}

	var sb strings.Builder

	// Header
	sb.WriteString("\n")
	sb.WriteString(fmt.Sprintf("%s%s %s ══════════════════════════════════════════════════════%s\n",
		display.Bold, display.Magenta, "SESSION COMPLETE", display.Reset))
	
	// Identity & Substrate
	subIcon := "🛡️"
	if state.Session.Substrate == "gemini" {
		subIcon = "♊"
	}
	sb.WriteString(fmt.Sprintf("  %sID:%s %s  %sBrain:%s %s %s\n",
		display.Dim, display.Reset, state.Session.ID,
		display.Dim, display.Reset, subIcon, state.Session.Engine))

	// Temporal
	start, _ := time.Parse(time.RFC3339, state.Session.InitializedAt)
	duration := time.Since(start).Round(time.Second)
	sb.WriteString(fmt.Sprintf("  %sStart:%s %s  %sDuration:%s %s\n",
		display.Dim, display.Reset, start.Format("15:04:05"),
		display.Dim, display.Reset, duration))

	sb.WriteString(fmt.Sprintf("%s%s ─────────────────────────────────────────────────────────────────%s\n",
		display.Dim, display.Magenta, display.Reset))

	// Metrics Row
	sb.WriteString(fmt.Sprintf("  %sExchanges:%s %-4d  %sInsights:%s %-4d  %sTools:%s %-4d\n",
		display.Dim, display.Reset, state.Session.ExchangeCount,
		display.Dim, display.Reset, state.Session.InsightCount,
		display.Dim, display.Reset, state.Session.PathLength))

	// State Row
	trajInfo := statemachine.GetTrajectoryInfo(state.TrajectorySection)
	healthInfo := health.GetHealthDisplay(state.Session.HealthScore)
	
	sb.WriteString(fmt.Sprintf("  %sTrajectory:%s %s %s  %sHealth:%s %s %s\n",
		display.Dim, display.Reset, trajInfo.Section, trajInfo.Symbol,
		display.Dim, display.Reset, healthInfo.Emoji, healthInfo.Formatted))

	sb.WriteString(fmt.Sprintf("  %sHebrew:%s %-10s  %sAlignment:%s %s%.2f%s\n",
		display.Dim, display.Reset, strings.ToUpper(state.Session.HebrewMeaning),
		display.Dim, display.Reset, alignmentColor(state.Session.KAlign), state.Session.KAlign, display.Reset))

	// Token Usage
	sb.WriteString(fmt.Sprintf("  %sPeak Context:%s %dK tokens  %sCompactions:%s %d\n",
		display.Dim, display.Reset, state.Session.PeakContextTokens/1000,
		display.Dim, display.Reset, state.Session.CompactionCount))

	sb.WriteString(fmt.Sprintf("%s%s ─────────────────────────────────────────────────────────────────%s\n",
		display.Dim, display.Magenta, display.Reset))

	// Work Summary (from tasks)
	if state.Session.Tasks.Total > 0 {
		sb.WriteString(fmt.Sprintf("  %sTasks:%s [%d/%d] complete\n",
			display.Dim, display.Reset, state.Session.Tasks.Completed, state.Session.Tasks.Total))
	}

	// Work Summary (from path)
	if path, err := statemachine.LoadRuntimePath(); err == nil {
		sb.WriteString(fmt.Sprintf("  %sSummary:%s %s\n",
			display.Dim, display.Reset, GenerateWorkSummary(path)))
	}

	// Closing Anchor
	sb.WriteString(fmt.Sprintf("  %sAnchor:%s ⚓ %s\n",
		display.Dim, display.Reset, state.AnchorKey))

	sb.WriteString(fmt.Sprintf("%s%s ════════════════════════════════════════════════════════════════%s\n\n",
		display.Bold, display.Magenta, display.Reset))

	return sb.String()
}

// GenerateWorkSummary returns a concise text summary of events recorded in the path
func GenerateWorkSummary(path *statemachine.RuntimePath) string {
	if path == nil || len(path.Events) == 0 {
		return "No significant events recorded."
	}

	var sb strings.Builder
	
	// Group events by type to avoid spamming
	counts := make(map[string]int)
	for _, e := range path.Events {
		counts[e.Type]++
	}

	// List unique event types with counts
	var summaryParts []string
	for etype, count := range counts {
		label := formatEventType(etype)
		if count > 1 {
			summaryParts = append(summaryParts, fmt.Sprintf("%d %s", count, label))
		} else {
			summaryParts = append(summaryParts, label)
		}
	}
	
	sb.WriteString(strings.Join(summaryParts, ", "))
	
	// Add recent milestones
	const maxMilestones = 3
	milestones := 0
	sb.WriteString("\n  Milestones: ")
	for i := len(path.Events) - 1; i >= 0 && milestones < maxMilestones; i-- {
		e := path.Events[i]
		if isSignificant(e.Type) {
			if milestones > 0 {
				sb.WriteString(" → ")
			}
			sb.WriteString(formatEventType(e.Type))
			milestones++
		}
	}

	return sb.String()
}

// alignmentColor returns ANSI color for alignment value
func alignmentColor(k float64) string {
	switch {
	case k > 0.5:
		return display.Green
	case k < -0.5:
		return display.Red
	default:
		return display.Yellow
	}
}

// Helper: Format event type for display
func formatEventType(etype string) string {
	etype = strings.ReplaceAll(etype, "_", " ")
	return etype
}

// Helper: Determine if event is a significant milestone
func isSignificant(etype string) bool {
	significant := []string{
		"insight_detected",
		"task_completed",
		"pivot_out",
		"pivot_in",
		"grounded",
		"error_recovered",
	}
	for _, s := range significant {
		if etype == s {
			return true
		}
	}
	return false
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
// Note: Generates closing statistics and session reflection summaries
// Scripture: "In the beginning God created the heaven and the earth." — Genesis 1:1
// ============================================================================
// END CLOSING
// ============================================================================
