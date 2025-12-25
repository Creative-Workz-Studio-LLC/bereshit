// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-cmd-cpisi-tui
// Purpose: TUI mode — interactive terminal interface
// Biblical: Proverbs 16:23 - "The heart of the wise teacheth his mouth"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-20
//
// Human-friendly interactive interface for development and exploration.
// Requires TTY. Color-coded positions. Keyboard navigation.
//
// ═══════════════════════════════════════════════════════════════════════════

package main

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"strings"

	"github.com/charmbracelet/bubbles/textinput"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/charmbracelet/lipgloss"

	"creativeworkzstudio.com/bereshit/word/work/pkg/core/coordinates"
	"creativeworkzstudio.com/bereshit/word/work/pkg/core/statemachine"
)

// --- Styles ---

var (
	titleStyle = lipgloss.NewStyle().
			Bold(true).
			Foreground(lipgloss.Color("205")).
			MarginBottom(1)

	boxStyle = lipgloss.NewStyle().
			Border(lipgloss.RoundedBorder()).
			Padding(1, 2).
			MarginRight(2)

	selectedStyle = lipgloss.NewStyle().
			Foreground(lipgloss.Color("170")).
			Bold(true)

	haltStyle = lipgloss.NewStyle().
			Foreground(lipgloss.Color("196"))

	awaitStyle = lipgloss.NewStyle().
			Foreground(lipgloss.Color("220"))

	proceedStyle = lipgloss.NewStyle().
			Foreground(lipgloss.Color("82"))

	dimStyle = lipgloss.NewStyle().
			Foreground(lipgloss.Color("240"))

	infoStyle = lipgloss.NewStyle().
			Foreground(lipgloss.Color("86"))
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- TUI Model ---

type tuiModel struct {
	sm          *statemachine.StateMachine
	bible       *coordinates.BibleIndex
	positions   []string
	cursor      int
	nInput      textinput.Model
	mode        string
	psiResult   string
	bibleResult string
}

// --- TUI Entry ---

func runTUI() {
	sm := statemachine.NewStateMachine()
	bible := coordinates.NewBibleIndex("/media/seanje-lenox-wise/Project/Bereshit/word/scripture")

	// Load existing state
	if state, err := loadState(); err == nil {
		sm.MoveTo(state.Position)
		sm.SetAnchor(state.Anchor)
	}

	positions := []string{
		"PAST-PAST", "PAST-PRESENT", "PAST-FUTURE",
		"PRESENT-PAST", "PRESENT-PRESENT", "PRESENT-FUTURE",
		"FUTURE-PAST", "FUTURE-PRESENT", "FUTURE-FUTURE",
	}

	ti := textinput.New()
	ti.Placeholder = "Enter N..."
	ti.CharLimit = 20
	ti.Width = 20

	cursor := 4
	for i, p := range positions {
		if p == sm.Current.Position.Name {
			cursor = i
			break
		}
	}

	m := tuiModel{
		sm:        sm,
		bible:     bible,
		positions: positions,
		cursor:    cursor,
		nInput:    ti,
		mode:      "navigate",
	}

	p := tea.NewProgram(m, tea.WithAltScreen())
	if _, err := p.Run(); err != nil {
		fmt.Printf("Error: %v\n", err)
	}
}

// --- Bubble Tea Interface ---

func (m tuiModel) Init() tea.Cmd { return nil }

func (m tuiModel) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	switch msg := msg.(type) {
	case tea.KeyMsg:
		key := msg.String()

		// Global keys (work in any mode)
		switch key {
		case "ctrl+c":
			return m, tea.Quit
		case "esc":
			// ESC always returns to navigate mode
			m.mode = "navigate"
			m.nInput.Blur()
			return m, nil
		case "tab":
			// Tab cycles modes: navigate → psi → bible → navigate
			switch m.mode {
			case "navigate":
				m.mode = "psi"
				m.nInput.Focus()
			case "psi":
				m.mode = "bible"
				m.nInput.Blur()
				m.showBibleAnchors()
			case "bible":
				m.mode = "navigate"
			}
			return m, nil
		}

		// Mode-specific handling
		switch m.mode {
		case "navigate":
			switch key {
			case "q":
				return m, tea.Quit
			case "up", "k":
				if m.cursor > 0 {
					m.cursor--
				}
			case "down", "j":
				if m.cursor < len(m.positions)-1 {
					m.cursor++
				}
			case "enter":
				m.sm.MoveTo(m.positions[m.cursor])
				saveState(m.sm, "tui:navigate")
			case "h":
				m.sm.SetAnchor("")
				saveState(m.sm, "tui:halt")
			case "p":
				// Quick switch to psi
				m.mode = "psi"
				m.nInput.Focus()
			case "b":
				// Quick switch to bible
				m.mode = "bible"
				m.showBibleAnchors()
			}

		case "psi":
			switch key {
			case "enter":
				var n float64
				fmt.Sscanf(m.nInput.Value(), "%f", &n)
				coords := m.sm.Psi(n)
				m.psiResult = fmt.Sprintf(
					"Ψ(%.2f) = (%.6f, %.6f, %.6f) | Sum: %.10f",
					n, coords.X, coords.Y, coords.Z, coords.X+coords.Y+coords.Z)
			default:
				// Pass ALL other keys (including numbers!) to text input
				var cmd tea.Cmd
				m.nInput, cmd = m.nInput.Update(msg)
				return m, cmd
			}

		case "bible":
			switch key {
			case "q":
				return m, tea.Quit
			case "enter", " ":
				// Refresh anchor list
				m.showBibleAnchors()
			}
		}
	}
	return m, nil
}

func (m *tuiModel) showBibleAnchors() {
	anchors := coordinates.GetAnchorVerses()
	var sb strings.Builder
	sb.WriteString("Anchor Verses:\n\n")
	for _, a := range anchors {
		coords, _ := m.bible.ToCoords(a.Ref)
		sb.WriteString(fmt.Sprintf("  %s %d:%d\n", a.Ref.Book, a.Ref.Chapter, a.Ref.Verse))
		sb.WriteString(fmt.Sprintf("    (%.4f, %.4f, %.4f) - %s\n\n",
			coords.BookNorm, coords.ChapterNorm, coords.VerseNorm, a.Purpose))
	}
	m.bibleResult = sb.String()
}

// --- View Rendering ---

func (m tuiModel) View() string {
	var sb strings.Builder

	sb.WriteString(titleStyle.Render("CPI-SI State Machine - Kingdom Technology"))
	sb.WriteString("\n\n")

	// Mode tabs - Tab cycles, p/b for quick switch in navigate mode
	tabs := []struct{ key, name, mode string }{
		{"", "Navigate", "navigate"},
		{"p", "Ψ(N)", "psi"},
		{"b", "Bible", "bible"},
	}
	for _, tab := range tabs {
		label := tab.name
		if tab.key != "" {
			label = "[" + tab.key + "]" + tab.name
		}
		if m.mode == tab.mode {
			sb.WriteString(selectedStyle.Render("▶ " + label))
		} else {
			sb.WriteString(dimStyle.Render("  " + label))
		}
		sb.WriteString("  ")
	}
	sb.WriteString(dimStyle.Render("  |  Tab:cycle  Esc:back  q:quit"))
	sb.WriteString("\n\n")

	switch m.mode {
	case "navigate":
		sb.WriteString(m.viewNavigate())
	case "psi":
		sb.WriteString(m.viewPsi())
	case "bible":
		sb.WriteString(boxStyle.Render(m.bibleResult))
	}

	return sb.String()
}

func (m tuiModel) viewNavigate() string {
	var left, right strings.Builder

	left.WriteString("Positions:\n\n")
	for i, name := range m.positions {
		p, _ := m.sm.GetPosition(name)
		var style lipgloss.Style
		switch p.Command {
		case statemachine.CmdHalt:
			style = haltStyle
		case statemachine.CmdAwait:
			style = awaitStyle
		case statemachine.CmdProceed:
			style = proceedStyle
		}

		cursor := "  "
		if i == m.cursor {
			cursor = "▶ "
		}
		left.WriteString(style.Render(fmt.Sprintf("%s%s [%s]\n", cursor, name, p.Command.String())))
	}

	current := m.sm.Current.Position
	right.WriteString(fmt.Sprintf("Current: %s\n\n", selectedStyle.Render(current.Name)))
	right.WriteString(fmt.Sprintf("Command:  %s\n", current.Command.String()))
	right.WriteString(fmt.Sprintf("Operator: %s\n", string(current.Op)))
	right.WriteString(fmt.Sprintf("Meaning:  %s\n", current.Meaning))
	right.WriteString(fmt.Sprintf("Coords:   (%s, %s, %.1f)\n", current.X.String(), current.Y.String(), current.Z))
	right.WriteString(fmt.Sprintf("Anchor:   %s\n", m.sm.Current.Anchor))

	return lipgloss.JoinHorizontal(lipgloss.Top,
		boxStyle.Render(left.String()),
		boxStyle.Render(right.String()))
}

func (m tuiModel) viewPsi() string {
	content := fmt.Sprintf(`Ψ(N) Master Equation

Formula: Ψ(N) = (cos(θ/3), cos(θ/3-φ), cos(θ/3+φ))
  θ = arccos(-tanh(λN)), λ≈0.5493, φ=2π/3

Guarantee: X + Y + Z = 0  ∀N

Enter N: %s

%s

Formula Modes:
  Z = -(X + Y)
  Y = -(X + Z)
  X = -(Y + Z)
`, m.nInput.View(), infoStyle.Render(m.psiResult))

	return boxStyle.Render(content)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// TUI is the human-friendly interactive interface:
// - Mode tabs: Navigate, Ψ(N), Bible
// - Color-coded positions (HALT=red, AWAIT=yellow, PROCEED=green)
// - Keyboard navigation (j/k, arrows, Tab, Esc)
// - Real-time state updates
//
// ═══════════════════════════════════════════════════════════════════════════
