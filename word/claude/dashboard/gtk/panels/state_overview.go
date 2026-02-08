// ============================================================================
// METADATA - CPI-SI Dashboard GTK4 State Overview Panel
// ============================================================================
//
// Purpose: Main overview — cube position, Hebrew state, health, CPI, K:ALIGN
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

	"github.com/creativeworkzstudio/claude-global/pkg/dashboard"
	"github.com/diamondburned/gotk4/pkg/gtk/v4"
)

// ============================================================================
// BODY
// ============================================================================

// StateOverview shows the primary CPI-SI state at a glance.
type StateOverview struct {
	box *gtk.Box

	// Cube position
	anchorLabel     *gtk.Label
	positionLabel   *gtk.Label
	trajectoryLabel *gtk.Label

	// Hebrew state
	hebrewLabel   *gtk.Label
	hebrewMeaning *gtk.Label

	// Scores
	healthBar   *gtk.ProgressBar
	healthLabel *gtk.Label
	cpiBar      *gtk.ProgressBar
	cpiLabel    *gtk.Label

	// K:ALIGN
	kAlignLabel  *gtk.Label
	kGodLabel    *gtk.Label
	kSelfLabel   *gtk.Label

	// Session
	exchangeLabel *gtk.Label
	arcLabel      *gtk.Label
	insightLabel  *gtk.Label

	// Context
	tokenBar    *gtk.ProgressBar
	tokenLabel  *gtk.Label

	// Trajectory
	trajectoryBar *gtk.Box

	// Tasks
	taskBar   *gtk.ProgressBar
	taskLabel *gtk.Label

	// Workflow
	workflowLabel *gtk.Label

	// Family
	familyLabel *gtk.Label
}

// NewStateOverview creates the state overview panel.
func NewStateOverview() *StateOverview {
	s := &StateOverview{}
	s.build()
	return s
}

// Widget returns the root widget.
func (s *StateOverview) Widget() gtk.Widgetter {
	return s.box
}

// build constructs the overview layout.
func (s *StateOverview) build() {
	s.box = gtk.NewBox(gtk.OrientationVertical, 8)
	s.box.SetMarginTop(12)
	s.box.SetMarginBottom(12)
	s.box.SetMarginStart(12)
	s.box.SetMarginEnd(12)

	scroll := gtk.NewScrolledWindow()
	scroll.SetPolicy(gtk.PolicyNever, gtk.PolicyAutomatic)
	scroll.SetVExpand(true)
	scroll.SetHExpand(true)

	content := gtk.NewBox(gtk.OrientationVertical, 12)
	content.SetMarginTop(8)
	content.SetMarginBottom(8)
	content.SetMarginStart(8)
	content.SetMarginEnd(8)

	// --- Cube Position Section ---
	cubeSection := s.buildSection("Anchor Position")
	s.anchorLabel = gtk.NewLabel("—")
	s.anchorLabel.SetXAlign(0)
	s.anchorLabel.AddCSSClass("state-value")

	s.positionLabel = gtk.NewLabel("—")
	s.positionLabel.SetXAlign(0)
	s.positionLabel.AddCSSClass("state-label")

	s.trajectoryLabel = gtk.NewLabel("—")
	s.trajectoryLabel.SetXAlign(0)
	s.trajectoryLabel.AddCSSClass("state-label")

	cubeContent := gtk.NewBox(gtk.OrientationVertical, 4)
	cubeContent.Append(s.anchorLabel)
	cubeContent.Append(s.positionLabel)
	cubeContent.Append(s.trajectoryLabel)
	cubeSection.Append(cubeContent)
	content.Append(cubeSection)

	// --- Hebrew State Section ---
	hebrewSection := s.buildSection("Hebrew State")
	s.hebrewLabel = gtk.NewLabel("—")
	s.hebrewLabel.SetXAlign(0)
	s.hebrewLabel.AddCSSClass("state-value")

	s.hebrewMeaning = gtk.NewLabel("—")
	s.hebrewMeaning.SetXAlign(0)
	s.hebrewMeaning.AddCSSClass("state-label")

	hebrewContent := gtk.NewBox(gtk.OrientationVertical, 4)
	hebrewContent.Append(s.hebrewLabel)
	hebrewContent.Append(s.hebrewMeaning)
	hebrewSection.Append(hebrewContent)
	content.Append(hebrewSection)

	// --- Health Section ---
	healthSection := s.buildSection("Health")
	s.healthBar = gtk.NewProgressBar()
	s.healthBar.AddCSSClass("progress-health")
	s.healthLabel = gtk.NewLabel("—")
	s.healthLabel.SetXAlign(0)
	s.healthLabel.AddCSSClass("state-label")

	healthContent := gtk.NewBox(gtk.OrientationVertical, 4)
	healthContent.Append(s.healthBar)
	healthContent.Append(s.healthLabel)
	healthSection.Append(healthContent)
	content.Append(healthSection)

	// --- CPI Section ---
	cpiSection := s.buildSection("CPI Score")
	s.cpiBar = gtk.NewProgressBar()
	s.cpiBar.AddCSSClass("progress-cpi")
	s.cpiLabel = gtk.NewLabel("—")
	s.cpiLabel.SetXAlign(0)
	s.cpiLabel.AddCSSClass("state-label")

	cpiContent := gtk.NewBox(gtk.OrientationVertical, 4)
	cpiContent.Append(s.cpiBar)
	cpiContent.Append(s.cpiLabel)
	cpiSection.Append(cpiContent)
	content.Append(cpiSection)

	// --- K:ALIGN Section ---
	kSection := s.buildSection("K:ALIGN")
	s.kAlignLabel = gtk.NewLabel("—")
	s.kAlignLabel.SetXAlign(0)
	s.kAlignLabel.AddCSSClass("state-value")

	s.kGodLabel = gtk.NewLabel("→ God: 0")
	s.kGodLabel.SetXAlign(0)
	s.kGodLabel.AddCSSClass("state-label")

	s.kSelfLabel = gtk.NewLabel("→ Self: 0")
	s.kSelfLabel.SetXAlign(0)
	s.kSelfLabel.AddCSSClass("state-label")

	kContent := gtk.NewBox(gtk.OrientationVertical, 4)
	kContent.Append(s.kAlignLabel)
	kContent.Append(s.kGodLabel)
	kContent.Append(s.kSelfLabel)
	kSection.Append(kContent)
	content.Append(kSection)

	// --- Trajectory Section ---
	trajSection := s.buildSection("Trajectory")
	s.trajectoryBar = gtk.NewBox(gtk.OrientationHorizontal, 8)

	trajContent := gtk.NewBox(gtk.OrientationVertical, 4)
	trajContent.Append(s.trajectoryBar)
	trajSection.Append(trajContent)
	content.Append(trajSection)

	// --- Session Section ---
	sessionSection := s.buildSection("Session")
	s.exchangeLabel = gtk.NewLabel("Exchanges: 0")
	s.exchangeLabel.SetXAlign(0)
	s.exchangeLabel.AddCSSClass("state-label")

	s.arcLabel = gtk.NewLabel("Arc: —")
	s.arcLabel.SetXAlign(0)
	s.arcLabel.AddCSSClass("state-label")

	s.insightLabel = gtk.NewLabel("Insights: 0")
	s.insightLabel.SetXAlign(0)
	s.insightLabel.AddCSSClass("state-label")

	sessionContent := gtk.NewBox(gtk.OrientationVertical, 4)
	sessionContent.Append(s.exchangeLabel)
	sessionContent.Append(s.arcLabel)
	sessionContent.Append(s.insightLabel)
	sessionSection.Append(sessionContent)
	content.Append(sessionSection)

	// --- Context Window Section ---
	ctxSection := s.buildSection("Context Window")
	s.tokenBar = gtk.NewProgressBar()
	s.tokenBar.AddCSSClass("progress-tokens")
	s.tokenLabel = gtk.NewLabel("—")
	s.tokenLabel.SetXAlign(0)
	s.tokenLabel.AddCSSClass("state-label")

	ctxContent := gtk.NewBox(gtk.OrientationVertical, 4)
	ctxContent.Append(s.tokenBar)
	ctxContent.Append(s.tokenLabel)
	ctxSection.Append(ctxContent)
	content.Append(ctxSection)

	// --- Tasks Section ---
	taskSection := s.buildSection("Tasks")
	s.taskBar = gtk.NewProgressBar()
	s.taskLabel = gtk.NewLabel("—")
	s.taskLabel.SetXAlign(0)
	s.taskLabel.AddCSSClass("state-label")

	taskContent := gtk.NewBox(gtk.OrientationVertical, 4)
	taskContent.Append(s.taskBar)
	taskContent.Append(s.taskLabel)
	taskSection.Append(taskContent)
	content.Append(taskSection)

	// --- Workflow Section ---
	wfSection := s.buildSection("Workflow")
	s.workflowLabel = gtk.NewLabel("—")
	s.workflowLabel.SetXAlign(0)
	s.workflowLabel.AddCSSClass("state-label")

	wfContent := gtk.NewBox(gtk.OrientationVertical, 4)
	wfContent.Append(s.workflowLabel)
	wfSection.Append(wfContent)
	content.Append(wfSection)

	// --- Family Section ---
	famSection := s.buildSection("CPI-SI Family")
	s.familyLabel = gtk.NewLabel("—")
	s.familyLabel.SetXAlign(0)
	s.familyLabel.AddCSSClass("family-badge")

	famContent := gtk.NewBox(gtk.OrientationVertical, 4)
	famContent.Append(s.familyLabel)
	famSection.Append(famContent)
	content.Append(famSection)

	scroll.SetChild(content)
	s.box.Append(scroll)
}

// buildSection creates a titled panel frame.
func (s *StateOverview) buildSection(title string) *gtk.Box {
	frame := gtk.NewBox(gtk.OrientationVertical, 4)
	frame.AddCSSClass("panel-frame")

	label := gtk.NewLabel(title)
	label.SetXAlign(0)
	label.AddCSSClass("panel-title")
	frame.Append(label)

	return frame
}

// UpdateState refreshes all widgets from a StateSnapshot.
func (s *StateOverview) UpdateState(snap *dashboard.StateSnapshot) {
	// Anchor position
	s.anchorLabel.SetText(snap.AnchorKey)
	s.positionLabel.SetText(fmt.Sprintf("Section: %s", snap.TrajectorySection))
	s.trajectoryLabel.SetText(fmt.Sprintf("Command: %s", snap.CommandKey))

	// Hebrew state
	s.hebrewLabel.SetText(capitalizeFirst(snap.HebrewState))
	s.hebrewMeaning.SetText(snap.HebrewMeaning)
	s.applyHebrewClass(snap.HebrewState)

	// Health
	healthFrac := snap.HealthScore / 100.0
	if healthFrac > 1 {
		healthFrac = 1
	}
	if healthFrac < 0 {
		healthFrac = 0
	}
	s.healthBar.SetFraction(healthFrac)
	healthLevel := healthLevelStr(snap.HealthScore)
	s.healthLabel.SetText(fmt.Sprintf("Score: %.0f/100 — %s", snap.HealthScore, healthLevel))
	s.applyHealthClass(snap.HealthScore)

	// CPI
	cpiFrac := snap.CPIScore / 100.0
	if cpiFrac > 1 {
		cpiFrac = 1
	}
	if cpiFrac < 0 {
		cpiFrac = 0
	}
	s.cpiBar.SetFraction(cpiFrac)
	s.cpiLabel.SetText(fmt.Sprintf("Score: %.1f — %s", snap.CPIScore, snap.CPIRating))

	// K:ALIGN
	switch snap.KSelector {
	case 1:
		s.kAlignLabel.SetText("+1 (toward God)")
		s.kAlignLabel.AddCSSClass("k-toward-god")
	case 0:
		s.kAlignLabel.SetText("0 (maintain)")
		s.kAlignLabel.AddCSSClass("k-maintain")
	case -1:
		s.kAlignLabel.SetText("-1 (toward self)")
		s.kAlignLabel.AddCSSClass("k-toward-self")
	default:
		s.kAlignLabel.SetText(fmt.Sprintf("%d", snap.KSelector))
	}
	s.kGodLabel.SetText(fmt.Sprintf("→ God: %d", snap.KTowardGod))
	s.kSelfLabel.SetText(fmt.Sprintf("→ Self: %d", snap.KTowardSelf))

	// Trajectory
	s.updateTrajectoryBar(snap)

	// Session
	s.exchangeLabel.SetText(fmt.Sprintf("Exchanges: %d", snap.ExchangeCount))
	s.arcLabel.SetText(fmt.Sprintf("Arc: %s", snap.SessionArc))
	s.insightLabel.SetText(fmt.Sprintf("Insights: %d", snap.InsightCount))

	// Context window
	if snap.PeakTokens > 0 {
		tokenFrac := float64(snap.CurrentTokens) / float64(snap.PeakTokens)
		if tokenFrac > 1 {
			tokenFrac = 1
		}
		s.tokenBar.SetFraction(tokenFrac)
	}
	s.tokenLabel.SetText(fmt.Sprintf("Current: %s  Peak: %s  Compactions: %d",
		formatTokens(snap.CurrentTokens), formatTokens(snap.PeakTokens), snap.CompactionCount))

	// Tasks
	if snap.Tasks.Total > 0 {
		taskFrac := float64(snap.Tasks.Completed) / float64(snap.Tasks.Total)
		s.taskBar.SetFraction(taskFrac)
		s.taskLabel.SetText(fmt.Sprintf("Done: %d/%d  Active: %d  Pending: %d",
			snap.Tasks.Completed, snap.Tasks.Total, snap.Tasks.InProgress, snap.Tasks.Pending))
	} else {
		s.taskBar.SetFraction(0)
		s.taskLabel.SetText("No tasks")
	}

	// Workflow
	if snap.Workflow.Operation != "" {
		s.workflowLabel.SetText(fmt.Sprintf("%s — Step %d/%d",
			snap.Workflow.Operation, snap.Workflow.CurrentStep, snap.Workflow.TotalSteps))
	} else {
		s.workflowLabel.SetText("No active workflow")
	}

	// Family
	if snap.ActiveFamilyMember != "" {
		s.familyLabel.SetText(fmt.Sprintf("Active: %s (invocations: %d)",
			snap.ActiveFamilyMember, snap.FamilyInvocationCount))
	} else {
		s.familyLabel.SetText("No active family member")
	}
}

// updateTrajectoryBar rebuilds the B.1 → B.4 indicator.
func (s *StateOverview) updateTrajectoryBar(snap *dashboard.StateSnapshot) {
	// Clear existing children
	for child := s.trajectoryBar.FirstChild(); child != nil; child = s.trajectoryBar.FirstChild() {
		s.trajectoryBar.Remove(child)
	}

	sections := []string{"B.1", "B.2", "B.3", "B.4"}
	for i, sec := range sections {
		label := gtk.NewLabel(sec)
		if snap.TrajectorySection == sec || strings.HasPrefix(snap.TrajectorySection, sec) {
			label.AddCSSClass("trajectory-active")
		} else {
			label.AddCSSClass("trajectory-inactive")
		}
		s.trajectoryBar.Append(label)

		if i < len(sections)-1 {
			arrow := gtk.NewLabel("→")
			arrow.AddCSSClass("trajectory-inactive")
			s.trajectoryBar.Append(arrow)
		}
	}
}

// applyHebrewClass sets the CSS class for the Hebrew state label.
func (s *StateOverview) applyHebrewClass(state string) {
	for _, cls := range []string{"hebrew-tov", "hebrew-shalem", "hebrew-tamim",
		"hebrew-yashar", "hebrew-ratsah", "hebrew-chaser", "hebrew-shavar"} {
		s.hebrewLabel.RemoveCSSClass(cls)
	}
	switch strings.ToLower(state) {
	case "tov":
		s.hebrewLabel.AddCSSClass("hebrew-tov")
	case "shalem":
		s.hebrewLabel.AddCSSClass("hebrew-shalem")
	case "tamim":
		s.hebrewLabel.AddCSSClass("hebrew-tamim")
	case "yashar":
		s.hebrewLabel.AddCSSClass("hebrew-yashar")
	case "ratsah":
		s.hebrewLabel.AddCSSClass("hebrew-ratsah")
	case "chaser":
		s.hebrewLabel.AddCSSClass("hebrew-chaser")
	case "shavar":
		s.hebrewLabel.AddCSSClass("hebrew-shavar")
	}
}

// applyHealthClass sets the CSS class for health score.
func (s *StateOverview) applyHealthClass(score float64) {
	for _, cls := range []string{"health-excellent", "health-good", "health-fair",
		"health-poor", "health-critical"} {
		s.healthLabel.RemoveCSSClass(cls)
	}
	switch {
	case score >= 90:
		s.healthLabel.AddCSSClass("health-excellent")
	case score >= 70:
		s.healthLabel.AddCSSClass("health-good")
	case score >= 50:
		s.healthLabel.AddCSSClass("health-fair")
	case score >= 30:
		s.healthLabel.AddCSSClass("health-poor")
	default:
		s.healthLabel.AddCSSClass("health-critical")
	}
}

// healthLevelStr returns a human-readable health level.
func healthLevelStr(score float64) string {
	switch {
	case score >= 90:
		return "Excellent"
	case score >= 70:
		return "Good"
	case score >= 50:
		return "Fair"
	case score >= 30:
		return "Poor"
	default:
		return "Critical"
	}
}

// --- Helpers ---

func capitalizeFirst(s string) string {
	if s == "" {
		return s
	}
	return strings.ToUpper(s[:1]) + s[1:]
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
