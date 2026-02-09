// ============================================================================
// METADATA - CPI-SI Dashboard TUI Status Bar
// ============================================================================
//
// Purpose: Top-line status showing anchor, Hebrew state, health, CPI, K:ALIGN
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
	"cws.studio/pkg/dashboard"
)

// Colors matching the web dashboard dark theme
var (
	colorBg     = lipgloss.Color("#1a1e2e")
	colorText   = lipgloss.Color("#e0e0e0")
	colorMuted  = lipgloss.Color("#6a7080")
	colorBlue   = lipgloss.Color("#4fc3f7")
	colorGreen  = lipgloss.Color("#66bb6a")
	colorRed    = lipgloss.Color("#ef5350")
	colorGold   = lipgloss.Color("#ffd54f")
	colorPurple = lipgloss.Color("#ce93d8")
)

// ============================================================================
// BODY
// ============================================================================

// StatusBar renders the top status line.
type StatusBar struct {
	anchorKey     string
	hebrewState   string
	hebrewMeaning string
	healthScore   float64
	cpiScore      float64
	cpiRating     string
	kAlign        float64
	sessionArc    string
}

// NewStatusBar creates an empty status bar.
func NewStatusBar() StatusBar {
	return StatusBar{
		anchorKey:   "genesis-1-1",
		hebrewState: "yashar",
		cpiRating:   "—",
	}
}

// Update refreshes the status bar from a snapshot.
func (s *StatusBar) Update(snap *dashboard.StateSnapshot) {
	if snap == nil {
		return
	}
	s.anchorKey = snap.AnchorKey
	s.hebrewState = snap.HebrewState
	s.hebrewMeaning = snap.HebrewMeaning
	s.healthScore = snap.HealthScore
	s.cpiScore = snap.CPIScore
	s.cpiRating = snap.CPIRating
	s.kAlign = snap.KAlign
	s.sessionArc = snap.SessionArc
}

// View renders the status bar to the given width.
func (s StatusBar) View(width int) string {
	statusStyle := lipgloss.NewStyle().
		Background(lipgloss.Color("#0d1117")).
		Foreground(colorText).
		Width(width).
		Padding(0, 1)

	labelStyle := lipgloss.NewStyle().Foreground(colorMuted)
	valueStyle := lipgloss.NewStyle().Foreground(colorText).Bold(true)

	// Anchor
	anchor := labelStyle.Render("⚓") + " " + valueStyle.Render(formatAnchor(s.anchorKey))

	// Hebrew state with color
	hebrewColor := hebrewStateColor(s.hebrewState)
	hebrew := labelStyle.Render("◈") + " " +
		lipgloss.NewStyle().Foreground(hebrewColor).Bold(true).Render(capitalizeFirst(s.hebrewState))

	// Health
	healthColor := healthScoreColor(s.healthScore)
	health := labelStyle.Render("♥") + " " +
		lipgloss.NewStyle().Foreground(healthColor).Bold(true).Render(fmt.Sprintf("%.0f%%", s.healthScore))

	// CPI
	cpi := labelStyle.Render("CPI") + " " +
		valueStyle.Render(fmt.Sprintf("%.0f", s.cpiScore)) + " " +
		lipgloss.NewStyle().Foreground(colorMuted).Render(s.cpiRating)

	// K:ALIGN
	kColor := kAlignColor(s.kAlign)
	kSign := ""
	if s.kAlign > 0 {
		kSign = "+"
	}
	kAlignStr := labelStyle.Render("K") + " " +
		lipgloss.NewStyle().Foreground(kColor).Bold(true).Render(fmt.Sprintf("%s%.2f", kSign, s.kAlign))

	// Session arc
	arc := ""
	if s.sessionArc != "" {
		arc = labelStyle.Render("⟐") + " " +
			lipgloss.NewStyle().Foreground(colorPurple).Render(s.sessionArc)
	}

	// Join with separator
	sep := lipgloss.NewStyle().Foreground(lipgloss.Color("#2a3040")).Render(" │ ")
	parts := []string{anchor, hebrew, health, cpi, kAlignStr}
	if arc != "" {
		parts = append(parts, arc)
	}

	return statusStyle.Render(strings.Join(parts, sep))
}

// --- Helpers ---

func formatAnchor(key string) string {
	switch key {
	case "genesis-1-1":
		return "Gen 1:1"
	default:
		if len(key) > 12 {
			return key[:12]
		}
		return key
	}
}

func capitalizeFirst(s string) string {
	if s == "" {
		return s
	}
	return strings.ToUpper(s[:1]) + s[1:]
}

func hebrewStateColor(state string) lipgloss.Color {
	switch state {
	case "shavar":
		return lipgloss.Color("#ef5350")
	case "chaser", "ratsah":
		return lipgloss.Color("#ffa726")
	case "yashar":
		return lipgloss.Color("#4fc3f7")
	case "tamim":
		return lipgloss.Color("#ce93d8")
	case "shalem":
		return lipgloss.Color("#66bb6a")
	case "tov":
		return lipgloss.Color("#ffd54f")
	default:
		return lipgloss.Color("#6a7080")
	}
}

func healthScoreColor(score float64) lipgloss.Color {
	switch {
	case score >= 80:
		return lipgloss.Color("#66bb6a")
	case score >= 60:
		return lipgloss.Color("#4fc3f7")
	case score >= 40:
		return lipgloss.Color("#ffa726")
	default:
		return lipgloss.Color("#ef5350")
	}
}

func kAlignColor(k float64) lipgloss.Color {
	switch {
	case k > 0.3:
		return lipgloss.Color("#66bb6a")
	case k > 0:
		return lipgloss.Color("#4fc3f7")
	case k == 0:
		return lipgloss.Color("#6a7080")
	case k > -0.3:
		return lipgloss.Color("#ffa726")
	default:
		return lipgloss.Color("#ef5350")
	}
}

// ============================================================================
// CLOSING
// ============================================================================
