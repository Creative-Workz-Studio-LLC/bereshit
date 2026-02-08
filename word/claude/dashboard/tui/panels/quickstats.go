// ============================================================================
// METADATA - CPI-SI Dashboard TUI Quick Stats
// ============================================================================
//
// Purpose: Right sidebar — trajectory, workflow, tasks, session metrics
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ============================================================================

package panels

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"strings"

	"github.com/charmbracelet/lipgloss"
	"github.com/creativeworkzstudio/claude-global/pkg/dashboard"
)

// ============================================================================
// BODY
// ============================================================================

// QuickStats is the right sidebar showing key metrics.
type QuickStats struct {
	snapshot *dashboard.StateSnapshot
}

// NewQuickStats creates an empty quick stats panel.
func NewQuickStats() QuickStats {
	return QuickStats{}
}

// Update refreshes the stats from a snapshot.
func (q *QuickStats) Update(snap *dashboard.StateSnapshot) {
	q.snapshot = snap
}

// View renders the quick stats sidebar.
func (q QuickStats) View(width, height int, focused bool) string {
	borderColor := colorMuted
	if focused {
		borderColor = colorBlue
	}

	panelStyle := lipgloss.NewStyle().
		Border(lipgloss.RoundedBorder()).
		BorderForeground(borderColor).
		Width(width - 2).
		Height(height - 2)

	if q.snapshot == nil {
		emptyStyle := lipgloss.NewStyle().Foreground(colorMuted).Italic(true)
		return panelStyle.Render(emptyStyle.Render("  Waiting for state..."))
	}

	s := q.snapshot
	contentWidth := width - 6

	var sections []string

	// --- Trajectory ---
	sections = append(sections, q.renderSection("Trajectory", contentWidth,
		q.renderTrajectoryBar(s),
		fmt.Sprintf("Section: %s", highlightValue(s.TrajectorySection)),
		fmt.Sprintf("Pivots: %d  Resets: %d", s.TrajectoryMetrics.PivotCount, s.TrajectoryMetrics.ResetCount),
		fmt.Sprintf("Momentum: %d", s.TrajectoryMetrics.MomentumScore),
	))

	// --- Session ---
	sections = append(sections, q.renderSection("Session", contentWidth,
		fmt.Sprintf("Exchanges: %s", highlightValue(fmt.Sprintf("%d", s.ExchangeCount))),
		fmt.Sprintf("Choices:   %s", highlightValue(fmt.Sprintf("%d", s.ChoiceSequence))),
		fmt.Sprintf("Hooks:     %s", highlightValue(fmt.Sprintf("%d", s.HooksFired))),
		fmt.Sprintf("Insights:  %s", highlightValue(fmt.Sprintf("%d", s.InsightCount))),
	))

	// --- K-Factor ---
	sections = append(sections, q.renderSection("K-Factor", contentWidth,
		fmt.Sprintf("Selector: %s", formatKSelector(s.KSelector)),
		fmt.Sprintf("→ God:    %s", highlightValue(fmt.Sprintf("%d", s.KTowardGod))),
		fmt.Sprintf("→ Self:   %s", highlightValue(fmt.Sprintf("%d", s.KTowardSelf))),
	))

	// --- Tasks ---
	if s.Tasks.Total > 0 {
		sections = append(sections, q.renderSection("Tasks", contentWidth,
			q.renderTaskBar(s),
			fmt.Sprintf("Done: %d/%d  Active: %d",
				s.Tasks.Completed, s.Tasks.Total, s.Tasks.InProgress),
		))
	}

	// --- Workflow ---
	if s.Workflow.Operation != "" {
		stepStr := fmt.Sprintf("%d/%d", s.Workflow.CurrentStep, s.Workflow.TotalSteps)
		sections = append(sections, q.renderSection("Workflow", contentWidth,
			fmt.Sprintf("Op: %s", highlightValue(s.Workflow.Operation)),
			fmt.Sprintf("Step: %s", highlightValue(stepStr)),
		))
	}

	// --- Context ---
	sections = append(sections, q.renderSection("Context", contentWidth,
		fmt.Sprintf("Tokens: %s", highlightValue(formatTokens(s.CurrentTokens))),
		fmt.Sprintf("Peak:   %s", highlightValue(formatTokens(s.PeakTokens))),
		fmt.Sprintf("Compactions: %d", s.CompactionCount),
	))

	// --- Family ---
	if s.ActiveFamilyMember != "" {
		sections = append(sections, q.renderSection("Family", contentWidth,
			fmt.Sprintf("Active: %s", lipgloss.NewStyle().Foreground(colorPurple).Bold(true).Render(s.ActiveFamilyMember)),
			fmt.Sprintf("Invocations: %d", s.FamilyInvocationCount),
		))
	}

	content := strings.Join(sections, "\n")
	return panelStyle.Render(content)
}

// --- Rendering helpers ---

func (q QuickStats) renderSection(title string, width int, lines ...string) string {
	titleStyle := lipgloss.NewStyle().
		Foreground(colorBlue).
		Bold(true).
		Padding(0, 1)

	lineStyle := lipgloss.NewStyle().
		Foreground(colorText).
		Padding(0, 1)

	result := titleStyle.Render("─ " + title)
	for _, line := range lines {
		result += "\n" + lineStyle.Render(line)
	}
	return result
}

func (q QuickStats) renderTrajectoryBar(s *dashboard.StateSnapshot) string {
	sections := []struct {
		name string
		time int
	}{
		{"B.1", s.TrajectoryMetrics.B1TimeMs},
		{"B.2", s.TrajectoryMetrics.B2TimeMs},
		{"B.3", s.TrajectoryMetrics.B3TimeMs},
		{"B.4", s.TrajectoryMetrics.B4TimeMs},
	}

	var parts []string
	for _, sec := range sections {
		style := lipgloss.NewStyle().Foreground(colorMuted)
		if s.TrajectorySection == sec.name || strings.HasPrefix(s.TrajectorySection, sec.name) {
			style = lipgloss.NewStyle().Foreground(colorGreen).Bold(true)
		}
		parts = append(parts, style.Render(sec.name))
	}

	return strings.Join(parts, " → ")
}

func (q QuickStats) renderTaskBar(s *dashboard.StateSnapshot) string {
	total := s.Tasks.Total
	if total == 0 {
		return ""
	}

	barWidth := 16
	completed := int(float64(s.Tasks.Completed) / float64(total) * float64(barWidth))
	inProgress := int(float64(s.Tasks.InProgress) / float64(total) * float64(barWidth))
	remaining := barWidth - completed - inProgress

	if remaining < 0 {
		remaining = 0
	}

	bar := lipgloss.NewStyle().Foreground(colorGreen).Render(strings.Repeat("█", completed)) +
		lipgloss.NewStyle().Foreground(colorGold).Render(strings.Repeat("▓", inProgress)) +
		lipgloss.NewStyle().Foreground(colorMuted).Render(strings.Repeat("░", remaining))

	return bar
}

func highlightValue(s string) string {
	return lipgloss.NewStyle().Foreground(colorText).Bold(true).Render(s)
}

func formatKSelector(k int) string {
	switch k {
	case 1:
		return lipgloss.NewStyle().Foreground(colorGreen).Bold(true).Render("+1 (toward God)")
	case 0:
		return lipgloss.NewStyle().Foreground(colorBlue).Render(" 0 (maintain)")
	case -1:
		return lipgloss.NewStyle().Foreground(colorRed).Bold(true).Render("-1 (toward self)")
	default:
		return fmt.Sprintf("%d", k)
	}
}

func formatTokens(n int) string {
	if n >= 1000000 {
		return fmt.Sprintf("%.1fM", float64(n)/1000000)
	}
	if n >= 1000 {
		return fmt.Sprintf("%.1fK", float64(n)/1000)
	}
	return fmt.Sprintf("%d", n)
}

// ============================================================================
// CLOSING
// ============================================================================
