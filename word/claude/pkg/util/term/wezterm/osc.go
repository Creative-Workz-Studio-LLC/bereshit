// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-wezterm-osc
// Purpose: WezTerm OSC sequence emission for terminal integration
// Biblical: Proverbs 25:11 - "A word fitly spoken is like apples of gold"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2025-12-20
// Updated: 2026-02-02
//
// Change: v1.1.0 - OSC output now goes to /dev/tty directly instead of stderr.
//         This bypasses Claude Code's process piping so WezTerm receives the
//         escape sequences even when statusline output is captured.
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
	"io"
	"os"
	"path/filepath"
	"strings"
)

// ttyWriter is the output destination for OSC sequences.
// Uses /dev/tty to bypass any process piping and reach the terminal directly.
var ttyWriter io.Writer

func init() {
	// Substrate-agnostic output selection
	// 1. Check for explicit override via CPISI_OSC_DEST
	dest := os.Getenv("CPISI_OSC_DEST")
	switch strings.ToLower(dest) {
	case "tty":
		if tty, err := os.OpenFile("/dev/tty", os.O_WRONLY, 0); err == nil {
			ttyWriter = tty
			return
		}
	case "stdout":
		ttyWriter = os.Stdout
		return
	case "stderr":
		ttyWriter = os.Stderr
		return
	case "none":
		ttyWriter = io.Discard
		return
	}

	// 2. Detection logic
	// If running in Gemini CLI, we should use Stderr to let the substrate handle rendering.
	// Writing to /dev/tty directly causes flickering in TUIs.
	if os.Getenv("GEMINI_CLI") == "1" || os.Getenv("GEMINI_SESSION_ID") != "" {
		ttyWriter = os.Stderr
		return
	}

	// 3. Legacy / Bypass mode (Claude Code)
	// Try to open /dev/tty for direct terminal access
	// This bypasses Claude Code's output capture
	tty, err := os.OpenFile("/dev/tty", os.O_WRONLY, 0)
	if err != nil {
		// Fall back to stderr if /dev/tty isn't available
		ttyWriter = os.Stderr
	} else {
		ttyWriter = tty
	}
}

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
	fmt.Fprintf(ttyWriter, "%s]1;%s%s", ESC, title, BEL)
}

// SetWindowTitle sets the window title (OSC 2)
// This appears in the window title bar
func SetWindowTitle(title string) {
	fmt.Fprintf(ttyWriter, "%s]2;%s%s", ESC, title, BEL)
}

// SetBothTitles sets both tab and window title (OSC 0)
// Clears icon name and sets window title
func SetBothTitles(title string) {
	fmt.Fprintf(ttyWriter, "%s]0;%s%s", ESC, title, BEL)
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
	fmt.Fprintf(ttyWriter, "%s]7;file://%s%s%s", ESC, hostname, path, BEL)
}

// ─────────────────────────────────────────────────────────────────────────────
// OSC 9 & 777: Notifications
// ─────────────────────────────────────────────────────────────────────────────

// Notify sends an iTerm2-style notification (OSC 9)
func Notify(message string) {
	fmt.Fprintf(ttyWriter, "%s]9;%s%s", ESC, message, BEL)
}

// NotifyWithTitle sends an rxvt-style notification with title (OSC 777)
func NotifyWithTitle(title, body string) {
	fmt.Fprintf(ttyWriter, "%s]777;notify;%s;%s%s", ESC, title, body, BEL)
}

// ─────────────────────────────────────────────────────────────────────────────
// OSC 1337: User Variables
// ─────────────────────────────────────────────────────────────────────────────

// SetUserVar sets a user variable that can be read by WezTerm Lua config
// Values are base64 encoded per iTerm2 protocol
func SetUserVar(name, value string) {
	encoded := base64.StdEncoding.EncodeToString([]byte(value))
	fmt.Fprintf(ttyWriter, "%s]1337;SetUserVar=%s=%s%s", ESC, name, encoded, BEL)
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
	fmt.Fprintf(ttyWriter, "%s]52;%s;%s%s", ESC, target, encoded, BEL)
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
	fmt.Fprintf(ttyWriter, "%s]10;%s%s", ESC, hexColor, BEL)
}

// SetBackgroundColor sets terminal background color (OSC 11)
func SetBackgroundColor(hexColor string) {
	fmt.Fprintf(ttyWriter, "%s]11;%s%s", ESC, hexColor, BEL)
}

// SetCursorColor sets terminal cursor color (OSC 12)
func SetCursorColor(hexColor string) {
	fmt.Fprintf(ttyWriter, "%s]12;%s%s", ESC, hexColor, BEL)
}

// ResetTerminalColors resets to default colors (OSC 104/110/111/112)
func ResetTerminalColors() {
	fmt.Fprintf(ttyWriter, "%s]104%s", ESC, BEL)  // Reset all colors
	fmt.Fprintf(ttyWriter, "%s]110%s", ESC, BEL)  // Reset foreground
	fmt.Fprintf(ttyWriter, "%s]111%s", ESC, BEL)  // Reset background
	fmt.Fprintf(ttyWriter, "%s]112%s", ESC, BEL)  // Reset cursor
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

// ─────────────────────────────────────────────────────────────────────────────
// Framework/Trajectory Emission
// ─────────────────────────────────────────────────────────────────────────────

// TrajectoryVars holds framework trajectory state
type TrajectoryVars struct {
	Section string // B.1, B.2, B.3, B.4
	Display string // →→, →|, |→, ||
	State   string // BUILDING, PIVOTING_OUT, PIVOTING_IN, GROUNDING
	PathLen int    // Number of anchors traversed
	X       string // Trajectory X position
	Y       string // Trajectory Y position
	Z       string // Trajectory Z position
}

// EmitTrajectory sends FRAMEWORK trajectory variables to WezTerm
// User variables set:
//   SM_TRAJ_SECTION  - Section identifier (B.1, B.2, etc.)
//   SM_TRAJ_DISPLAY  - Visual display (→→, →|, etc.)
//   SM_TRAJ_STATE    - State name (BUILDING, etc.)
//   SM_TRAJ_PATH_LEN - Anchors traversed count
//   SM_TRAJ_X/Y/Z    - Trajectory coordinates
func EmitTrajectory(tv TrajectoryVars) {
	SetUserVar("SM_TRAJ_SECTION", tv.Section)
	SetUserVar("SM_TRAJ_DISPLAY", tv.Display)
	SetUserVar("SM_TRAJ_STATE", tv.State)
	SetUserVar("SM_TRAJ_PATH_LEN", fmt.Sprintf("%d", tv.PathLen))
	if tv.X != "" {
		SetUserVar("SM_TRAJ_X", tv.X)
	}
	if tv.Y != "" {
		SetUserVar("SM_TRAJ_Y", tv.Y)
	}
	if tv.Z != "" {
		SetUserVar("SM_TRAJ_Z", tv.Z)
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// K:MORAL Emission
// ─────────────────────────────────────────────────────────────────────────────

// KMoralVars holds K:MORAL (moral direction) state
type KMoralVars struct {
	Selector    string // Current selector
	Align       string // Alignment indicator
	TowardGod   int    // Count of +k choices
	TowardSelf  int    // Count of -k choices
	HebrewState string // Current Hebrew state (yashar, tamim, etc.)
}

// EmitKMoral sends K:MORAL direction variables to WezTerm
// User variables set:
//   K_SELECTOR    - Current k selector
//   K_ALIGN       - Alignment indicator
//   K_TOWARD_GOD  - Count of toward-God choices
//   K_TOWARD_SELF - Count of toward-self choices
//   HEBREW_STATE  - Current Hebrew state
func EmitKMoral(km KMoralVars) {
	SetUserVar("K_SELECTOR", km.Selector)
	SetUserVar("K_ALIGN", km.Align)
	SetUserVar("K_TOWARD_GOD", fmt.Sprintf("%d", km.TowardGod))
	SetUserVar("K_TOWARD_SELF", fmt.Sprintf("%d", km.TowardSelf))
	SetUserVar("HEBREW_STATE", km.HebrewState)
}

// ─────────────────────────────────────────────────────────────────────────────
// CPI Emission
// ─────────────────────────────────────────────────────────────────────────────

// CPIVars holds Covenant Partnership Intelligence metrics
type CPIVars struct {
	Score     float64 // 0.0 to 1.0
	Exchanges int     // Number of exchanges
	Insights  int     // Number of insights
	Arc       string  // Session arc (starting, building, flowing, winding)
}

// EmitCPI sends CPI metrics to WezTerm
// User variables set:
//   CPI_SCORE     - CPI score (0.00 to 1.00)
//   CPI_EXCHANGES - Exchange count
//   CPI_INSIGHTS  - Insight count
//   CPI_ARC       - Session arc phase
func EmitCPI(cpi CPIVars) {
	SetUserVar("CPI_SCORE", fmt.Sprintf("%.2f", cpi.Score))
	SetUserVar("CPI_EXCHANGES", fmt.Sprintf("%d", cpi.Exchanges))
	SetUserVar("CPI_INSIGHTS", fmt.Sprintf("%d", cpi.Insights))
	SetUserVar("CPI_ARC", cpi.Arc)
}

// ─────────────────────────────────────────────────────────────────────────────
// Context Emission
// ─────────────────────────────────────────────────────────────────────────────

// ContextVars holds context window state
type ContextVars struct {
	CompactionCount  int // Number of compactions
	EffectiveWindow  int // Effective context window size
}

// EmitContext sends context awareness variables to WezTerm
// User variables set:
//   CTX_COMPACTION_COUNT  - Number of context compactions
//   CTX_EFFECTIVE_WINDOW  - Effective window percentage
func EmitContext(ctx ContextVars) {
	SetUserVar("CTX_COMPACTION_COUNT", fmt.Sprintf("%d", ctx.CompactionCount))
	SetUserVar("CTX_EFFECTIVE_WINDOW", fmt.Sprintf("%d", ctx.EffectiveWindow))
}

// ─────────────────────────────────────────────────────────────────────────────
// Momentum Emission
// ─────────────────────────────────────────────────────────────────────────────

// MomentumVars holds momentum tracking state
type MomentumVars struct {
	Net   int     // Net momentum (towardGod - towardSelf)
	Ratio float64 // Ratio of toward-God to total
}

// EmitMomentum sends momentum tracking variables to WezTerm
// User variables set:
//   MOMENTUM_NET   - Net k-momentum
//   MOMENTUM_RATIO - Ratio of positive momentum
func EmitMomentum(mom MomentumVars) {
	SetUserVar("MOMENTUM_NET", fmt.Sprintf("%d", mom.Net))
	SetUserVar("MOMENTUM_RATIO", fmt.Sprintf("%.2f", mom.Ratio))
}

// ─────────────────────────────────────────────────────────────────────────────
// Session Emission
// ─────────────────────────────────────────────────────────────────────────────

// SessionVars holds session tracking state
type SessionVars struct {
	StartTime  int64  // Unix timestamp of session start
	ActiveTask string // Currently active task name
}

// EmitSession sends session tracking variables to WezTerm
// User variables set:
//   SESSION_START - Unix timestamp of session start
//   ACTIVE_TASK   - Name of active task
func EmitSession(sess SessionVars) {
	SetUserVar("SESSION_START", fmt.Sprintf("%d", sess.StartTime))
	SetUserVar("ACTIVE_TASK", sess.ActiveTask)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
