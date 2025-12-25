// ============================================================================
// METADATA
// ============================================================================
// Package: statemachine/display
// Purpose: Command state display helpers (colors, icons)
// Biblical: "Let all things be done decently and in order" - 1 Corinthians 14:40
//
// CPI-SI command states have consistent visual representation:
//   HALT   (|)  = Red    = Terminal/stop
//   AWAIT  (@)  = Yellow = Waiting/caution
//   PROCEED (->) = Green  = Flowing/go

package statemachine

// ============================================================================
// SETUP
// ============================================================================

import (
	"creativeworkzstudio.com/bereshit/word/work/pkg/util/term/display"
)

// CommandInfo contains display information for a command state
type CommandInfo struct {
	Name     string // HALT, AWAIT, PROCEED
	Operator string // |, @, ->
	Icon     string // Visual icon
	Color    string // ANSI color
}

// ============================================================================
// BODY
// ============================================================================

// CommandColor returns the ANSI color for a command state
func CommandColor(cmd Command) string {
	switch cmd {
	case CmdHalt:
		return display.Red // Terminal - stop
	case CmdAwait:
		return display.Yellow // Waiting - caution
	case CmdProceed:
		return display.Green // Flowing - go
	default:
		return display.Gray
	}
}

// CommandIcon returns the visual icon for a command state
func CommandIcon(cmd Command) string {
	switch cmd {
	case CmdHalt:
		return "⬡" // Hexagon - halted
	case CmdAwait:
		return "◇" // Diamond - waiting
	case CmdProceed:
		return "▷" // Triangle - proceeding
	default:
		return "○"
	}
}

// CommandOperator returns the operator symbol for a command state
func CommandOperator(cmd Command) string {
	switch cmd {
	case CmdHalt:
		return "|"
	case CmdAwait:
		return "@"
	case CmdProceed:
		return "->"
	default:
		return ""
	}
}

// GetCommandInfo returns complete command display information
func GetCommandInfo(cmd Command) CommandInfo {
	return CommandInfo{
		Name:     cmd.String(),
		Operator: CommandOperator(cmd),
		Icon:     CommandIcon(cmd),
		Color:    CommandColor(cmd),
	}
}

// --- Hex Color Codes for WezTerm/Lua ---

// These match the Go ANSI colors for consistent appearance
const (
	HexRed    = "#ff6b6b" // HALT
	HexYellow = "#ffd93d" // AWAIT
	HexGreen  = "#6bcb77" // PROCEED
	HexGray   = "#888888" // Default

	// Background colors (darker variants)
	HexBgRed    = "#2a1a1a"
	HexBgYellow = "#2a2a1a"
	HexBgGreen  = "#1a2a1a"
)

// CommandHexColor returns the hex color for a command state
// Used for WezTerm OSC sequences
func CommandHexColor(cmd Command) string {
	switch cmd {
	case CmdHalt:
		return HexRed
	case CmdAwait:
		return HexYellow
	case CmdProceed:
		return HexGreen
	default:
		return HexGray
	}
}

// CommandHexBgColor returns the background hex color for a command state
func CommandHexBgColor(cmd Command) string {
	switch cmd {
	case CmdHalt:
		return HexBgRed
	case CmdAwait:
		return HexBgYellow
	case CmdProceed:
		return HexBgGreen
	default:
		return ""
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// Related: pkg/util/term/display/ (colors)
//          pkg/util/term/wezterm/osc.go (terminal integration)
