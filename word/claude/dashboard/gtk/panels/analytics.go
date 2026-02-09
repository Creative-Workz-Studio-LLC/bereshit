// ============================================================================
// METADATA - CPI-SI Dashboard GTK4 Analytics Panel
// ============================================================================
//
// Purpose: Analytics view — session history, trends, metrics tables
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

	"cws.studio/pkg/dashboard"
	"github.com/diamondburned/gotk4/pkg/gtk/v4"
)

// ============================================================================
// BODY
// ============================================================================

// Analytics shows historical data, trends, and metrics.
type Analytics struct {
	box *gtk.Box

	// Session history
	sessionList *gtk.Box

	// Trend summaries
	kTrendLabel     *gtk.Label
	cpiTrendLabel   *gtk.Label
	healthTrendLabel *gtk.Label

	// Pattern list
	patternList *gtk.Box

	// Stats
	totalSessionsLabel *gtk.Label
	avgHealthLabel     *gtk.Label
	avgCPILabel        *gtk.Label
}

// NewAnalytics creates the analytics panel.
func NewAnalytics() *Analytics {
	a := &Analytics{}
	a.build()
	return a
}

// Widget returns the root widget.
func (a *Analytics) Widget() gtk.Widgetter {
	return a.box
}

// build constructs the analytics layout.
func (a *Analytics) build() {
	a.box = gtk.NewBox(gtk.OrientationVertical, 8)
	a.box.SetMarginTop(12)
	a.box.SetMarginBottom(12)
	a.box.SetMarginStart(12)
	a.box.SetMarginEnd(12)

	scroll := gtk.NewScrolledWindow()
	scroll.SetPolicy(gtk.PolicyNever, gtk.PolicyAutomatic)
	scroll.SetVExpand(true)
	scroll.SetHExpand(true)

	content := gtk.NewBox(gtk.OrientationVertical, 12)
	content.SetMarginTop(8)
	content.SetMarginBottom(8)
	content.SetMarginStart(8)
	content.SetMarginEnd(8)

	// --- Summary Stats ---
	statsSection := a.buildSection("Summary")
	statsGrid := gtk.NewGrid()
	statsGrid.SetRowSpacing(4)
	statsGrid.SetColumnSpacing(16)

	a.totalSessionsLabel = gtk.NewLabel("Sessions: —")
	a.totalSessionsLabel.SetXAlign(0)
	a.totalSessionsLabel.AddCSSClass("state-label")
	statsGrid.Attach(a.totalSessionsLabel, 0, 0, 1, 1)

	a.avgHealthLabel = gtk.NewLabel("Avg Health: —")
	a.avgHealthLabel.SetXAlign(0)
	a.avgHealthLabel.AddCSSClass("state-label")
	statsGrid.Attach(a.avgHealthLabel, 1, 0, 1, 1)

	a.avgCPILabel = gtk.NewLabel("Avg CPI: —")
	a.avgCPILabel.SetXAlign(0)
	a.avgCPILabel.AddCSSClass("state-label")
	statsGrid.Attach(a.avgCPILabel, 0, 1, 1, 1)

	statsSection.Append(statsGrid)
	content.Append(statsSection)

	// --- Trends ---
	trendSection := a.buildSection("Trends")
	a.kTrendLabel = gtk.NewLabel("K:ALIGN trend: —")
	a.kTrendLabel.SetXAlign(0)
	a.kTrendLabel.AddCSSClass("state-label")

	a.cpiTrendLabel = gtk.NewLabel("CPI trend: —")
	a.cpiTrendLabel.SetXAlign(0)
	a.cpiTrendLabel.AddCSSClass("state-label")

	a.healthTrendLabel = gtk.NewLabel("Health trend: —")
	a.healthTrendLabel.SetXAlign(0)
	a.healthTrendLabel.AddCSSClass("state-label")

	trendContent := gtk.NewBox(gtk.OrientationVertical, 4)
	trendContent.Append(a.kTrendLabel)
	trendContent.Append(a.cpiTrendLabel)
	trendContent.Append(a.healthTrendLabel)
	trendSection.Append(trendContent)
	content.Append(trendSection)

	// --- Recent Sessions ---
	sessionSection := a.buildSection("Recent Sessions")
	a.sessionList = gtk.NewBox(gtk.OrientationVertical, 2)

	emptyLabel := gtk.NewLabel("No session history available")
	emptyLabel.AddCSSClass("state-label")
	emptyLabel.SetXAlign(0)
	a.sessionList.Append(emptyLabel)

	sessionSection.Append(a.sessionList)
	content.Append(sessionSection)

	// --- Active Patterns ---
	patternSection := a.buildSection("Active Patterns")
	a.patternList = gtk.NewBox(gtk.OrientationVertical, 2)

	noPatterns := gtk.NewLabel("No patterns detected")
	noPatterns.AddCSSClass("state-label")
	noPatterns.SetXAlign(0)
	a.patternList.Append(noPatterns)

	patternSection.Append(a.patternList)
	content.Append(patternSection)

	scroll.SetChild(content)
	a.box.Append(scroll)
}

// buildSection creates a titled panel frame.
func (a *Analytics) buildSection(title string) *gtk.Box {
	frame := gtk.NewBox(gtk.OrientationVertical, 4)
	frame.AddCSSClass("panel-frame")

	label := gtk.NewLabel(title)
	label.SetXAlign(0)
	label.AddCSSClass("panel-title")
	frame.Append(label)

	return frame
}

// UpdateState refreshes analytics from a state snapshot.
func (a *Analytics) UpdateState(snap *dashboard.StateSnapshot) {
	// Update session count from exchange info
	a.totalSessionsLabel.SetText(fmt.Sprintf("Sessions: %d exchanges", snap.ExchangeCount))

	// Health
	a.avgHealthLabel.SetText(fmt.Sprintf("Health: %.0f", snap.HealthScore))

	// CPI
	a.avgCPILabel.SetText(fmt.Sprintf("CPI: %.1f — %s", snap.CPIScore, snap.CPIRating))

	// K:ALIGN trend
	kDir := "maintain"
	if snap.KTowardGod > snap.KTowardSelf {
		kDir = "toward God"
	} else if snap.KTowardSelf > snap.KTowardGod {
		kDir = "toward self"
	}
	a.kTrendLabel.SetText(fmt.Sprintf("K:ALIGN: +%d/-%d (%s)", snap.KTowardGod, snap.KTowardSelf, kDir))

	// CPI trend
	a.cpiTrendLabel.SetText(fmt.Sprintf("CPI: %.1f (%s)", snap.CPIScore, snap.CPIRating))

	// Health trend
	a.healthTrendLabel.SetText(fmt.Sprintf("Health: %.0f/100", snap.HealthScore))
}

// UpdateAnalytics refreshes from a full analytics bundle.
func (a *Analytics) UpdateAnalytics(bundle *dashboard.AnalyticsBundle) {
	if bundle == nil {
		return
	}

	// Summary from session count
	a.totalSessionsLabel.SetText(fmt.Sprintf("Sessions: %d", len(bundle.RecentSessions)))

	// Recent sessions
	for child := a.sessionList.FirstChild(); child != nil; child = a.sessionList.FirstChild() {
		a.sessionList.Remove(child)
	}

	if len(bundle.RecentSessions) == 0 {
		empty := gtk.NewLabel("No session history available")
		empty.AddCSSClass("state-label")
		empty.SetXAlign(0)
		a.sessionList.Append(empty)
	} else {
		for _, sess := range bundle.RecentSessions {
			row := gtk.NewBox(gtk.OrientationHorizontal, 8)

			idStr := sess.ID
			if len(idStr) > 8 {
				idStr = idStr[:8]
			}
			idLabel := gtk.NewLabel(idStr)
			idLabel.AddCSSClass("state-value")
			idLabel.SetXAlign(0)
			row.Append(idLabel)

			arcLabel := gtk.NewLabel(sess.SessionArc)
			arcLabel.AddCSSClass("state-label")
			row.Append(arcLabel)

			cpiLabel := gtk.NewLabel(fmt.Sprintf("CPI:%.2f", sess.CPIScore))
			cpiLabel.AddCSSClass("state-label")
			row.Append(cpiLabel)

			exchLabel := gtk.NewLabel(fmt.Sprintf("Ex:%d", sess.ExchangeCount))
			exchLabel.AddCSSClass("state-label")
			row.Append(exchLabel)

			a.sessionList.Append(row)
		}
	}

	// Active patterns
	for child := a.patternList.FirstChild(); child != nil; child = a.patternList.FirstChild() {
		a.patternList.Remove(child)
	}

	if len(bundle.ActivePatterns) == 0 {
		empty := gtk.NewLabel("No patterns detected")
		empty.AddCSSClass("state-label")
		empty.SetXAlign(0)
		a.patternList.Append(empty)
	} else {
		for _, pattern := range bundle.ActivePatterns {
			label := gtk.NewLabel(fmt.Sprintf("%s — %s (x%d, conf:%.0f%%)",
				pattern.PatternKey, pattern.Description,
				pattern.OccurrenceCount, pattern.Confidence*100))
			label.AddCSSClass("state-label")
			label.SetXAlign(0)
			label.SetWrap(true)
			a.patternList.Append(label)
		}
	}
}

// ============================================================================
// CLOSING
// ============================================================================
