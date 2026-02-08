// ============================================================================
// METADATA - CPI-SI Dashboard TUI Command Bar
// ============================================================================
//
// Purpose: Bottom keyboard shortcut hints
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
	"strings"

	"github.com/charmbracelet/lipgloss"
)

// ============================================================================
// BODY
// ============================================================================

// CommandBar renders keyboard shortcut hints at the bottom.
type CommandBar struct {
	showHelp bool
}

// NewCommandBar creates a new command bar.
func NewCommandBar() CommandBar {
	return CommandBar{}
}

// ToggleHelp shows/hides the expanded help.
func (c *CommandBar) ToggleHelp() {
	c.showHelp = !c.showHelp
}

// View renders the command bar.
func (c CommandBar) View(width int) string {
	barStyle := lipgloss.NewStyle().
		Background(lipgloss.Color("#0d1117")).
		Width(width)

	keyStyle := lipgloss.NewStyle().
		Foreground(colorGold).
		Bold(true)

	descStyle := lipgloss.NewStyle().
		Foreground(colorMuted)

	sep := descStyle.Render("  ")

	if c.showHelp {
		// Expanded help
		lines := []string{
			keyStyle.Render("j/k") + descStyle.Render(" scroll") + sep +
				keyStyle.Render("g/G") + descStyle.Render(" top/bottom") + sep +
				keyStyle.Render("Tab") + descStyle.Render(" focus") + sep +
				keyStyle.Render("q") + descStyle.Render(" quit") + sep +
				keyStyle.Render("?") + descStyle.Render(" close help"),
		}
		return barStyle.Render(strings.Join(lines, "\n"))
	}

	// Compact hints
	hints := keyStyle.Render("j/k") + descStyle.Render(" scroll") + sep +
		keyStyle.Render("Tab") + descStyle.Render(" focus") + sep +
		keyStyle.Render("q") + descStyle.Render(" quit") + sep +
		keyStyle.Render("?") + descStyle.Render(" help")

	return barStyle.Render(hints)
}

// ============================================================================
// CLOSING
// ============================================================================
