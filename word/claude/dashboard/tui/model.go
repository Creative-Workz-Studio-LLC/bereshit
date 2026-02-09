// ============================================================================
// METADATA - CPI-SI Dashboard TUI Model
// ============================================================================
//
// Key: claude-global-dashboard-tui-model
// Purpose: Main Bubble Tea model — orchestrates panels and data flow
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ============================================================================

package tui

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"

	tea "github.com/charmbracelet/bubbletea"
	"github.com/charmbracelet/lipgloss"
	"cws.studio/dashboard/tui/panels"
	"cws.studio/pkg/dashboard"
)

// ============================================================================
// BODY
// ============================================================================

// --- Message types ---

// stateChangeMsg wraps a dashboard StateChange for the Bubble Tea Update loop.
type stateChangeMsg dashboard.StateChange

// initialStateMsg carries the first state snapshot loaded at startup.
type initialStateMsg struct {
	snapshot *dashboard.StateSnapshot
	events   []dashboard.LogEvent
	valence  map[string]int
}

// --- Focus tracking ---

// FocusArea tracks which panel has keyboard focus.
type FocusArea int

const (
	FocusMessages FocusArea = iota
	FocusStats
)

// --- Model ---

// Model is the root Bubble Tea model for the CPI-SI Dashboard TUI.
type Model struct {
	svc       *dashboard.DashboardService
	eventCh   <-chan dashboard.StateChange
	width     int
	height    int
	focus     FocusArea
	statusBar panels.StatusBar
	messages  panels.Messages
	stats     panels.QuickStats
	cmdBar    panels.CommandBar
	ready     bool
}

// NewModel creates a new TUI model connected to the dashboard service.
func NewModel(svc *dashboard.DashboardService) Model {
	eventCh := svc.Subscribe()

	return Model{
		svc:       svc,
		eventCh:   eventCh,
		focus:     FocusMessages,
		statusBar: panels.NewStatusBar(),
		messages:  panels.NewMessages(),
		stats:     panels.NewQuickStats(),
		cmdBar:    panels.NewCommandBar(),
	}
}

// Init returns the initial commands: load state + start listening for events.
func (m Model) Init() tea.Cmd {
	return tea.Batch(
		m.loadInitialState(),
		m.waitForEvent(),
	)
}

// Update handles all incoming messages.
func (m Model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	switch msg := msg.(type) {

	case tea.WindowSizeMsg:
		m.width = msg.Width
		m.height = msg.Height
		m.updatePanelSizes()
		m.ready = true
		return m, nil

	case tea.KeyMsg:
		switch msg.String() {
		case "q", "ctrl+c":
			m.svc.Unsubscribe(m.eventCh)
			return m, tea.Quit
		case "tab":
			m.cycleFocus()
			return m, nil
		case "j", "down":
			if m.focus == FocusMessages {
				m.messages.ScrollDown()
			}
			return m, nil
		case "k", "up":
			if m.focus == FocusMessages {
				m.messages.ScrollUp()
			}
			return m, nil
		case "g":
			if m.focus == FocusMessages {
				m.messages.ScrollToTop()
			}
			return m, nil
		case "G":
			if m.focus == FocusMessages {
				m.messages.ScrollToBottom()
			}
			return m, nil
		case "?":
			m.cmdBar.ToggleHelp()
			return m, nil
		}

	case initialStateMsg:
		if msg.snapshot != nil {
			m.statusBar.Update(msg.snapshot)
			m.stats.Update(msg.snapshot)
		}
		if msg.valence != nil {
			m.stats.UpdateValence(msg.valence)
		}
		for _, evt := range msg.events {
			m.messages.AddEvent(evt)
		}
		return m, nil

	case stateChangeMsg:
		change := dashboard.StateChange(msg)
		switch change.Type {
		case "state_update":
			if change.Snapshot != nil {
				m.statusBar.Update(change.Snapshot)
				m.stats.Update(change.Snapshot)
				// Refresh valence on state updates
				if change.Snapshot.SessionID != "" {
					if valence, err := m.svc.ValenceDistribution(change.Snapshot.SessionID); err == nil {
						m.stats.UpdateValence(valence)
					}
				}
			}
		case "log_event":
			if change.Event != nil {
				m.messages.AddEvent(*change.Event)
			}
		}
		// Re-subscribe for next event
		return m, m.waitForEvent()
	}

	return m, nil
}

// View renders the full TUI layout.
func (m Model) View() string {
	if !m.ready {
		return "Loading CPI-SI Dashboard..."
	}

	// Render each panel
	statusView := m.statusBar.View(m.width)

	// Calculate body dimensions
	statusHeight := lipgloss.Height(statusView)
	cmdHeight := 1
	bodyHeight := m.height - statusHeight - cmdHeight
	if bodyHeight < 3 {
		bodyHeight = 3
	}

	// Split: messages (70%) | stats (30%)
	msgWidth := int(float64(m.width) * 0.7)
	statsWidth := m.width - msgWidth
	if statsWidth < 20 {
		statsWidth = 20
		msgWidth = m.width - statsWidth
	}

	msgView := m.messages.View(msgWidth, bodyHeight, m.focus == FocusMessages)
	statsView := m.stats.View(statsWidth, bodyHeight, m.focus == FocusStats)

	body := lipgloss.JoinHorizontal(lipgloss.Top, msgView, statsView)
	cmdView := m.cmdBar.View(m.width)

	return lipgloss.JoinVertical(lipgloss.Left,
		statusView,
		body,
		cmdView,
	)
}

// --- Internal methods ---

func (m *Model) cycleFocus() {
	switch m.focus {
	case FocusMessages:
		m.focus = FocusStats
	case FocusStats:
		m.focus = FocusMessages
	}
}

func (m *Model) updatePanelSizes() {
	m.messages.SetMaxLines(m.height - 4) // Reserve for status + cmd
}

// waitForEvent returns a tea.Cmd that blocks on the EventBus channel.
func (m Model) waitForEvent() tea.Cmd {
	ch := m.eventCh
	return func() tea.Msg {
		change, ok := <-ch
		if !ok {
			return nil
		}
		return stateChangeMsg(change)
	}
}

// loadInitialState fetches the current state and recent events.
func (m Model) loadInitialState() tea.Cmd {
	svc := m.svc
	return func() tea.Msg {
		snap, _ := svc.CurrentState()
		events, _ := svc.RecentEvents(50)
		var valence map[string]int
		if snap != nil && snap.SessionID != "" {
			valence, _ = svc.ValenceDistribution(snap.SessionID)
		}
		return initialStateMsg{snapshot: snap, events: events, valence: valence}
	}
}

// ============================================================================
// CLOSING
// ============================================================================

// Ensure Model satisfies tea.Model
var _ tea.Model = Model{}

// formatDuration formats milliseconds into a human-readable duration.
func formatDuration(ms int) string {
	if ms < 1000 {
		return fmt.Sprintf("%dms", ms)
	}
	secs := ms / 1000
	if secs < 60 {
		return fmt.Sprintf("%ds", secs)
	}
	mins := secs / 60
	return fmt.Sprintf("%dm%ds", mins, secs%60)
}
