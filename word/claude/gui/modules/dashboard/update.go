// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-modules-dashboard-update
//omni:code  --go -package->dashboard
//omni:version a-00.10
//
// Purpose: StateSnapshot -> ViewModel translation. This is where the dashboard's
//          data becomes structured presentation. The renderer never sees
//          StateSnapshot — only ViewModels.
// Biblical: Daniel 5:25 - "MENE, MENE, TEKEL, UPHARSIN" — weigh and display
//
// ============================================================================
// END METADATA

package dashboard

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"strings"

	"cws.studio/gui/render"
	"cws.studio/pkg/dashboard"
)

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

// BuildOverviewView creates the full overview ViewModel from a state snapshot.
func BuildOverviewView(snap *dashboard.StateSnapshot) render.View {
	return render.View{
		ID:    ViewOverview,
		Title: "Overview",
		Icon:  "dashboard",
		Sections: []render.Section{
			buildAnchorSection(snap),
			buildHebrewSection(snap),
			buildHealthSection(snap),
			buildCPISection(snap),
			buildKAlignSection(snap),
			buildTrajectorySection(snap),
			buildSessionSection(snap),
			buildContextSection(snap),
			buildTasksSection(snap),
			buildWorkflowSection(snap),
			buildFamilySection(snap),
		},
	}
}

// --- Section builders ---

func buildAnchorSection(snap *dashboard.StateSnapshot) render.Section {
	return render.Section{
		Title: "Anchor Position",
		Fields: []render.Field{
			{Key: "Anchor", Value: snap.AnchorKey, Style: render.StyleEmphasized},
			{Key: "Command", Value: snap.CommandKey, Style: render.StyleCode},
		},
	}
}

func buildHebrewSection(snap *dashboard.StateSnapshot) render.Section {
	style := hebrewStyle(snap.HebrewState)
	return render.Section{
		Title: "Hebrew State",
		Fields: []render.Field{
			{Key: "State", Value: snap.HebrewState, Style: style},
			{Key: "Meaning", Value: snap.HebrewMeaning, Style: render.StyleMuted},
		},
	}
}

func buildHealthSection(snap *dashboard.StateSnapshot) render.Section {
	score := snap.HealthScore
	label := fmt.Sprintf("%.0f/100", score)
	style := healthStyle(score)

	return render.Section{
		Title: "Health",
		Fields: []render.Field{
			{
				Key:   "Score",
				Value: label,
				Style: style,
				Progress: &render.Progress{
					Fraction: score / 100.0,
					Label:    label,
					Style:    "health",
				},
			},
		},
	}
}

func buildCPISection(snap *dashboard.StateSnapshot) render.Section {
	score := snap.CPIScore
	label := fmt.Sprintf("%.2f — %s", score, snap.CPIRating)

	return render.Section{
		Title: "CPI",
		Fields: []render.Field{
			{
				Key:   "Score",
				Value: label,
				Style: render.StyleEmphasized,
				Progress: &render.Progress{
					Fraction: score,
					Label:    label,
					Style:    "cpi",
				},
			},
		},
	}
}

func buildKAlignSection(snap *dashboard.StateSnapshot) render.Section {
	kLabel := fmt.Sprintf("%.3f", snap.KAlign)
	kStyle := render.StyleNormal
	if snap.KAlign > 0 {
		kStyle = render.StyleSuccess
	} else if snap.KAlign < 0 {
		kStyle = render.StyleError
	}

	total := float64(snap.KTowardGod + snap.KTowardSelf + snap.KSelector)
	if total == 0 {
		total = 1 // avoid division by zero
	}

	return render.Section{
		Title: "K:ALIGN",
		Fields: []render.Field{
			{Key: "Alignment", Value: kLabel, Style: kStyle},
			{Key: "Toward God (+1)", Value: fmt.Sprintf("%d", snap.KTowardGod), Style: render.StyleSuccess},
			{Key: "Maintain (0)", Value: fmt.Sprintf("%d", snap.KSelector), Style: render.StyleMuted},
			{Key: "Toward Self (-1)", Value: fmt.Sprintf("%d", snap.KTowardSelf), Style: render.StyleError},
			{
				Key:   "Distribution",
				Value: "",
				Bar: &render.Bar{
					Segments: []render.BarSegment{
						{Width: float64(snap.KTowardGod) / total, Style: "valence-positive", Label: "+1"},
						{Width: float64(snap.KSelector) / total, Style: "valence-neutral", Label: "0"},
						{Width: float64(snap.KTowardSelf) / total, Style: "valence-negative", Label: "-1"},
					},
				},
			},
		},
	}
}

func buildTrajectorySection(snap *dashboard.StateSnapshot) render.Section {
	phases := []struct {
		id   string
		name string
	}{
		{"B.1", "BUILDING"},
		{"B.2", "EXECUTING"},
		{"B.3", "VERIFYING"},
		{"B.4", "COMPLETING"},
	}

	fields := make([]render.Field, 0, len(phases))
	for _, p := range phases {
		style := render.StyleMuted
		if snap.TrajectorySection == p.id {
			style = render.StyleSuccess
		}
		fields = append(fields, render.Field{
			Key:   p.id,
			Value: p.name,
			Style: style,
		})
	}

	return render.Section{
		Title:  "Trajectory",
		Fields: fields,
	}
}

func buildSessionSection(snap *dashboard.StateSnapshot) render.Section {
	return render.Section{
		Title: "Session",
		Fields: []render.Field{
			{Key: "ID", Value: shortID(snap.SessionID), Style: render.StyleCode},
			{Key: "Started", Value: snap.SessionStart, Style: render.StyleMuted},
			{Key: "Exchanges", Value: fmt.Sprintf("%d", snap.ExchangeCount), Style: render.StyleNormal},
			{Key: "Insights", Value: fmt.Sprintf("%d", snap.InsightCount), Style: render.StyleEmphasized},
			{Key: "Arc", Value: snap.SessionArc, Style: render.StyleNormal},
			{Key: "Path Length", Value: fmt.Sprintf("%d", snap.PathLength), Style: render.StyleNormal},
		},
	}
}

func buildContextSection(snap *dashboard.StateSnapshot) render.Section {
	fraction := 0.0
	if snap.PeakTokens > 0 {
		fraction = float64(snap.CurrentTokens) / float64(snap.PeakTokens)
	}

	return render.Section{
		Title: "Context Window",
		Fields: []render.Field{
			{
				Key:   "Tokens",
				Value: fmt.Sprintf("%d / %d", snap.CurrentTokens, snap.PeakTokens),
				Style: render.StyleNormal,
				Progress: &render.Progress{
					Fraction: fraction,
					Label:    fmt.Sprintf("%d tokens", snap.CurrentTokens),
					Style:    "tokens",
				},
			},
			{Key: "Compactions", Value: fmt.Sprintf("%d", snap.CompactionCount), Style: render.StyleMuted},
		},
	}
}

func buildTasksSection(snap *dashboard.StateSnapshot) render.Section {
	t := snap.Tasks
	total := t.Total
	if total == 0 {
		return render.Section{
			Title: "Tasks",
			Fields: []render.Field{
				{Key: "Status", Value: "No tasks", Style: render.StyleMuted},
			},
		}
	}

	fraction := float64(t.Completed) / float64(total)

	return render.Section{
		Title: "Tasks",
		Fields: []render.Field{
			{
				Key:   "Progress",
				Value: fmt.Sprintf("%d/%d completed", t.Completed, total),
				Style: render.StyleNormal,
				Progress: &render.Progress{
					Fraction: fraction,
					Label:    fmt.Sprintf("%d/%d", t.Completed, total),
					Style:    "health",
				},
			},
			{Key: "Pending", Value: fmt.Sprintf("%d", t.Pending), Style: render.StyleWarning},
			{Key: "In Progress", Value: fmt.Sprintf("%d", t.InProgress), Style: render.StyleEmphasized},
		},
	}
}

func buildWorkflowSection(snap *dashboard.StateSnapshot) render.Section {
	w := snap.Workflow
	progress := ""
	if w.TotalSteps > 0 {
		progress = fmt.Sprintf("%d / %d", w.CurrentStep, w.TotalSteps)
	}
	return render.Section{
		Title: "Workflow",
		Fields: []render.Field{
			{Key: "Operation", Value: w.Operation, Style: render.StyleEmphasized},
			{Key: "Progress", Value: progress, Style: render.StyleNormal},
			{Key: "Started", Value: w.StartedAt, Style: render.StyleMuted},
		},
	}
}

func buildFamilySection(snap *dashboard.StateSnapshot) render.Section {
	return render.Section{
		Title: "Family",
		Fields: []render.Field{
			{Key: "Hooks Fired", Value: fmt.Sprintf("%d", snap.HooksFired), Style: render.StyleNormal},
			{Key: "Dominant", Value: snap.DominantExchange, Style: render.StyleEmphasized},
		},
	}
}

// --- Helpers ---

// hebrewStyle returns the FieldStyle for a Hebrew state name.
func hebrewStyle(state string) render.FieldStyle {
	lower := strings.ToLower(state)
	switch {
	case strings.Contains(lower, "tov"):
		return render.StyleSuccess
	case strings.Contains(lower, "shalem"), strings.Contains(lower, "tamim"):
		return render.StyleEmphasized
	case strings.Contains(lower, "yashar"):
		return render.StyleNormal
	case strings.Contains(lower, "shavar"):
		return render.StyleError
	default:
		return render.StyleWarning
	}
}

// healthStyle returns the FieldStyle based on health score.
func healthStyle(score float64) render.FieldStyle {
	switch {
	case score >= 80:
		return render.StyleSuccess
	case score >= 60:
		return render.StyleEmphasized
	case score >= 40:
		return render.StyleWarning
	default:
		return render.StyleError
	}
}

// shortID truncates a session ID for display.
func shortID(id string) string {
	if len(id) > 12 {
		return id[:12] + "..."
	}
	return id
}

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
