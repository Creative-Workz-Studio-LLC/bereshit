// ============================================================================
// METADATA - CPI-SI Dashboard TUI Message Log
// ============================================================================
//
// Purpose: Scrollable event log panel — color-coded by level, auto-scroll
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

const maxMessages = 500

// Messages is the scrollable event log panel.
type Messages struct {
	events   []dashboard.LogEvent
	offset   int // First visible line
	maxLines int // Visible lines
}

// NewMessages creates an empty message log.
func NewMessages() Messages {
	return Messages{
		maxLines: 20,
	}
}

// SetMaxLines adjusts the visible line count.
func (m *Messages) SetMaxLines(n int) {
	if n < 3 {
		n = 3
	}
	m.maxLines = n
}

// AddEvent appends a new log event and auto-scrolls to bottom.
func (m *Messages) AddEvent(evt dashboard.LogEvent) {
	m.events = append(m.events, evt)
	// Trim if over limit
	if len(m.events) > maxMessages {
		m.events = m.events[len(m.events)-maxMessages:]
	}
	// Auto-scroll to bottom
	m.scrollToEnd()
}

// ScrollDown moves the viewport down one line.
func (m *Messages) ScrollDown() {
	maxOffset := len(m.events) - m.maxLines + 2 // +2 for title+border
	if maxOffset < 0 {
		maxOffset = 0
	}
	if m.offset < maxOffset {
		m.offset++
	}
}

// ScrollUp moves the viewport up one line.
func (m *Messages) ScrollUp() {
	if m.offset > 0 {
		m.offset--
	}
}

// ScrollToTop jumps to the beginning.
func (m *Messages) ScrollToTop() {
	m.offset = 0
}

// ScrollToBottom jumps to the end.
func (m *Messages) ScrollToBottom() {
	m.scrollToEnd()
}

func (m *Messages) scrollToEnd() {
	maxOffset := len(m.events) - m.maxLines + 2
	if maxOffset < 0 {
		maxOffset = 0
	}
	m.offset = maxOffset
}

// View renders the message log panel.
func (m Messages) View(width, height int, focused bool) string {
	borderColor := colorMuted
	if focused {
		borderColor = colorBlue
	}

	panelStyle := lipgloss.NewStyle().
		Border(lipgloss.RoundedBorder()).
		BorderForeground(borderColor).
		Width(width - 2). // Account for border
		Height(height - 2)

	titleStyle := lipgloss.NewStyle().
		Foreground(colorText).
		Bold(true).
		Padding(0, 1)

	title := titleStyle.Render(fmt.Sprintf("Messages (%d)", len(m.events)))

	// Render visible events
	visibleLines := height - 4 // Title + borders + padding
	if visibleLines < 1 {
		visibleLines = 1
	}

	var lines []string
	start := m.offset
	end := start + visibleLines
	if end > len(m.events) {
		end = len(m.events)
	}

	contentWidth := width - 6 // Border + padding
	if contentWidth < 10 {
		contentWidth = 10
	}

	for i := start; i < end; i++ {
		evt := m.events[i]
		line := formatEvent(evt, contentWidth)
		lines = append(lines, line)
	}

	if len(lines) == 0 {
		emptyStyle := lipgloss.NewStyle().Foreground(colorMuted).Italic(true)
		lines = append(lines, emptyStyle.Render("No events yet..."))
	}

	// Pad remaining lines
	for len(lines) < visibleLines {
		lines = append(lines, "")
	}

	content := title + "\n" + strings.Join(lines, "\n")
	return panelStyle.Render(content)
}

// formatEvent renders a single log event line.
func formatEvent(evt dashboard.LogEvent, maxWidth int) string {
	// Time (just HH:MM:SS)
	ts := evt.Timestamp
	if len(ts) >= 19 {
		ts = ts[11:19] // Extract time portion
	} else if len(ts) > 8 {
		ts = ts[len(ts)-8:]
	}

	timeStyle := lipgloss.NewStyle().Foreground(colorMuted)

	// Level coloring
	var levelStyle lipgloss.Style
	switch strings.ToUpper(evt.Level) {
	case "ERROR":
		levelStyle = lipgloss.NewStyle().Foreground(colorRed).Bold(true)
	case "WARN":
		levelStyle = lipgloss.NewStyle().Foreground(colorGold)
	case "INFO":
		levelStyle = lipgloss.NewStyle().Foreground(colorBlue)
	case "DEBUG":
		levelStyle = lipgloss.NewStyle().Foreground(colorMuted)
	default:
		levelStyle = lipgloss.NewStyle().Foreground(colorText)
	}

	// Component
	compStyle := lipgloss.NewStyle().Foreground(colorPurple)

	// Build the line
	prefix := timeStyle.Render(ts) + " " +
		levelStyle.Render(fmt.Sprintf("%-5s", strings.ToUpper(evt.Level))) + " " +
		compStyle.Render(fmt.Sprintf("%-10s", evt.Component)) + " "

	// Truncate message to fit
	prefixLen := len(ts) + 1 + 5 + 1 + 10 + 1
	msgWidth := maxWidth - prefixLen
	if msgWidth < 10 {
		msgWidth = 10
	}
	msg := evt.Message
	if len(msg) > msgWidth {
		msg = msg[:msgWidth-3] + "..."
	}

	return prefix + lipgloss.NewStyle().Foreground(colorText).Render(msg)
}

// ============================================================================
// CLOSING
// ============================================================================
