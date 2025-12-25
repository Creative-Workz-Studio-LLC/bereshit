// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-wezterm-osc
// Purpose: WezTerm OSC sequence emission for terminal integration
// Biblical: Proverbs 25:11 - "A word fitly spoken is like apples of gold"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-20
//
// OSC Sequences Supported:
//   OSC 1  - Set tab title (icon name)
//   OSC 2  - Set window title
//   OSC 7  - Set current working directory
//   OSC 9  - iTerm2 notification
//   OSC 52 - Clipboard manipulation
//   OSC 777 - rxvt notification
//   OSC 1337 - User variables (iTerm2 protocol)
//
// Reference: https://wezterm.org/escape-sequences.html
//
// ═══════════════════════════════════════════════════════════════════════════

package wezterm

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"encoding/base64"
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

// OSC escape sequence constants
const (
	ESC = "\x1b"  // Escape character
	BEL = "\x07"  // Bell (OSC terminator)
	ST  = "\x1b\\" // String Terminator (alternative OSC terminator)
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ─────────────────────────────────────────────────────────────────────────────
// OSC 1 & 2: Title Setting
// ─────────────────────────────────────────────────────────────────────────────

// SetTabTitle sets the tab title (OSC 1)
// This appears in WezTerm's tab bar
func SetTabTitle(title string) {
	fmt.Fprintf(os.Stderr, "%s]1;%s%s", ESC, title, BEL)
}

// SetWindowTitle sets the window title (OSC 2)
// This appears in the window title bar
func SetWindowTitle(title string) {
	fmt.Fprintf(os.Stderr, "%s]2;%s%s", ESC, title, BEL)
}

// SetBothTitles sets both tab and window title (OSC 0)
// Clears icon name and sets window title
func SetBothTitles(title string) {
	fmt.Fprintf(os.Stderr, "%s]0;%s%s", ESC, title, BEL)
}

// ─────────────────────────────────────────────────────────────────────────────
// OSC 7: Current Working Directory
// ─────────────────────────────────────────────────────────────────────────────

// SetCWD emits OSC 7 to tell WezTerm the current working directory
// This enables spawning new panes/tabs with the same directory
func SetCWD(path string) {
	hostname, _ := os.Hostname()
	if hostname == "" {
		hostname = "localhost"
	}
	// Format: file://hostname/path
	fmt.Fprintf(os.Stderr, "%s]7;file://%s%s%s", ESC, hostname, path, BEL)
}

// ─────────────────────────────────────────────────────────────────────────────
// OSC 9 & 777: Notifications
// ─────────────────────────────────────────────────────────────────────────────

// Notify sends an iTerm2-style notification (OSC 9)
func Notify(message string) {
	fmt.Fprintf(os.Stderr, "%s]9;%s%s", ESC, message, BEL)
}

// NotifyWithTitle sends an rxvt-style notification with title (OSC 777)
func NotifyWithTitle(title, body string) {
	fmt.Fprintf(os.Stderr, "%s]777;notify;%s;%s%s", ESC, title, body, BEL)
}

// ─────────────────────────────────────────────────────────────────────────────
// OSC 1337: User Variables
// ─────────────────────────────────────────────────────────────────────────────

// SetUserVar sets a user variable that can be read by WezTerm Lua config
// Values are base64 encoded per iTerm2 protocol
func SetUserVar(name, value string) {
	encoded := base64.StdEncoding.EncodeToString([]byte(value))
	fmt.Fprintf(os.Stderr, "%s]1337;SetUserVar=%s=%s%s", ESC, name, encoded, BEL)
}

// SetUserVars sets multiple user variables at once
func SetUserVars(vars map[string]string) {
	for name, value := range vars {
		SetUserVar(name, value)
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// OSC 52: Clipboard
// ─────────────────────────────────────────────────────────────────────────────

// SetClipboard sets the system clipboard content (OSC 52)
// target can be: "c" (clipboard), "p" (primary), "s" (select), etc.
func SetClipboard(target, content string) {
	encoded := base64.StdEncoding.EncodeToString([]byte(content))
	fmt.Fprintf(os.Stderr, "%s]52;%s;%s%s", ESC, target, encoded, BEL)
}

// CopyToClipboard is a convenience function for copying to system clipboard
func CopyToClipboard(content string) {
	SetClipboard("c", content)
}

// ─────────────────────────────────────────────────────────────────────────────
// OSC 10/11/12: Dynamic Terminal Colors
// ─────────────────────────────────────────────────────────────────────────────

// Command state colors (hex format for OSC 10/11/12)
// These reflect the state machine command visually
var commandColors = map[string]struct {
	Foreground string // OSC 10
	Background string // OSC 11
	Cursor     string // OSC 12
}{
	"HALT": {
		Foreground: "#ff6b6b", // Red-ish for terminal/stop
		Background: "#1a1a2e", // Dark with red undertone
		Cursor:     "#ff4757", // Bright red cursor
	},
	"AWAIT": {
		Foreground: "#ffd93d", // Yellow for waiting/caution
		Background: "#1a1a2e", // Dark with yellow undertone
		Cursor:     "#feca57", // Bright yellow cursor
	},
	"PROCEED": {
		Foreground: "#6bcb77", // Green for flowing/go
		Background: "#1a1a2e", // Dark with green undertone
		Cursor:     "#26de81", // Bright green cursor
	},
}

// SetForegroundColor sets terminal foreground color (OSC 10)
func SetForegroundColor(hexColor string) {
	fmt.Fprintf(os.Stderr, "%s]10;%s%s", ESC, hexColor, BEL)
}

// SetBackgroundColor sets terminal background color (OSC 11)
func SetBackgroundColor(hexColor string) {
	fmt.Fprintf(os.Stderr, "%s]11;%s%s", ESC, hexColor, BEL)
}

// SetCursorColor sets terminal cursor color (OSC 12)
func SetCursorColor(hexColor string) {
	fmt.Fprintf(os.Stderr, "%s]12;%s%s", ESC, hexColor, BEL)
}

// ResetTerminalColors resets to default colors (OSC 104/110/111/112)
func ResetTerminalColors() {
	fmt.Fprintf(os.Stderr, "%s]104%s", ESC, BEL)  // Reset all colors
	fmt.Fprintf(os.Stderr, "%s]110%s", ESC, BEL)  // Reset foreground
	fmt.Fprintf(os.Stderr, "%s]111%s", ESC, BEL)  // Reset background
	fmt.Fprintf(os.Stderr, "%s]112%s", ESC, BEL)  // Reset cursor
}

// SetCommandColors sets terminal colors based on command state
func SetCommandColors(command string) {
	colors, ok := commandColors[strings.ToUpper(command)]
	if !ok {
		return // Unknown command, don't change colors
	}
	SetForegroundColor(colors.Foreground)
	SetBackgroundColor(colors.Background)
	SetCursorColor(colors.Cursor)
}

// ─────────────────────────────────────────────────────────────────────────────
// CPI-SI State Machine Integration
// ─────────────────────────────────────────────────────────────────────────────

// StateVars holds state machine variables for WezTerm
type StateVars struct {
	Position string  // e.g., "PRESENT-PRESENT"
	Meaning  string  // e.g., "Origin (Genesis 1:1)"
	Command  string  // "HALT", "AWAIT", "PROCEED"
	Operator string  // "|", "@", "->", etc.
	Anchor   string  // e.g., "Genesis 1:1"
	X        string  // X coordinate
	Y        string  // Y coordinate
	Z        float64 // Z depth
}

// stateFilePath returns the path to the state persistence file
func stateFilePath() string {
	// Prefer XDG_RUNTIME_DIR, fall back to /tmp
	runtimeDir := os.Getenv("XDG_RUNTIME_DIR")
	if runtimeDir == "" {
		runtimeDir = "/tmp"
	}
	return filepath.Join(runtimeDir, "cpisi-wezterm-state")
}

// ReadPreviousCommand reads the previous command from state file
func ReadPreviousCommand() string {
	data, err := os.ReadFile(stateFilePath())
	if err != nil {
		return "" // No previous state
	}
	return strings.TrimSpace(string(data))
}

// WritePreviousCommand persists current command for next invocation
func WritePreviousCommand(command string) {
	_ = os.WriteFile(stateFilePath(), []byte(command), 0600)
}

// EmitStateVars sends all state machine variables to WezTerm
func EmitStateVars(sv StateVars) {
	SetUserVar("SM_POSITION", sv.Position)
	SetUserVar("SM_MEANING", sv.Meaning)
	SetUserVar("SM_COMMAND", sv.Command)
	SetUserVar("SM_OPERATOR", sv.Operator)
	SetUserVar("SM_ANCHOR", sv.Anchor)
	SetUserVar("SM_X", sv.X)
	SetUserVar("SM_Y", sv.Y)
	SetUserVar("SM_Z", fmt.Sprintf("%.1f", sv.Z))
}

// EmitStateTitles sets tab and window titles based on state
func EmitStateTitles(sv StateVars, instanceName string) {
	// Tab: Position + Command with operator
	tabTitle := fmt.Sprintf("⬡ %s %s %s", sv.Position, sv.Operator, sv.Command)
	SetTabTitle(tabTitle)

	// Window: Instance @ Anchor
	windowTitle := fmt.Sprintf("%s @ %s", instanceName, sv.Anchor)
	SetWindowTitle(windowTitle)
}

// EmitStateNotification sends a notification on state change
func EmitStateNotification(oldCmd, newCmd, position string) {
	if oldCmd != newCmd && oldCmd != "" {
		title := fmt.Sprintf("CPI-SI: %s", newCmd)
		body := fmt.Sprintf("Transitioned to %s at %s", newCmd, position)
		NotifyWithTitle(title, body)
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// Full State Emission (All-in-One)
// ─────────────────────────────────────────────────────────────────────────────

// EmitFullState performs complete WezTerm state integration
//
// This is the primary entry point for statusline integration:
//   - Reads previous command state
//   - Emits user variables (OSC 1337)
//   - Sets tab/window titles (OSC 1/2)
//   - Sets terminal colors based on command (OSC 10/11/12)
//   - Sends notification if command changed (OSC 777)
//   - Persists current command for next invocation
func EmitFullState(sv StateVars, instanceName, workdir string) {
	// Read previous state for transition detection
	prevCmd := ReadPreviousCommand()

	// Emit all user variables for WezTerm Lua access
	EmitStateVars(sv)

	// Set tab and window titles
	EmitStateTitles(sv, instanceName)

	// Set terminal colors based on command state
	SetCommandColors(sv.Command)

	// Emit CWD for new pane/tab spawning
	if workdir != "" {
		SetCWD(workdir)
	}

	// Notify on state transition
	EmitStateNotification(prevCmd, sv.Command, sv.Position)

	// Persist current command for next invocation
	WritePreviousCommand(sv.Command)
}

// ─────────────────────────────────────────────────────────────────────────────
// Health State Emission
// ─────────────────────────────────────────────────────────────────────────────

// EmitHealth sends health score information to WezTerm
// User variables set:
//   HEALTH_SCORE - Numeric score (-100 to +100)
//   HEALTH_STATE - Hebrew state name (SHAVAR, SHALEM, TAMIM, etc.)
//   HEALTH_LEVEL - Level name (broken, even, perfect, etc.)
func EmitHealth(score float64, hebrewState, level string) {
	SetUserVar("HEALTH_SCORE", fmt.Sprintf("%.0f", score))
	SetUserVar("HEALTH_STATE", hebrewState)
	SetUserVar("HEALTH_LEVEL", level)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
