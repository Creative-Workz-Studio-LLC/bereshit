//omni:code --go -library
//omni:key B-L2-util-term-display
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-util-term-display
// Purpose: Shared terminal display colors and formatting
// Biblical: Proverbs 25:11 - "A word fitly spoken is like apples of gold"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-10
//
// Used by: statusline, skills with CLI output
//
// ═══════════════════════════════════════════════════════════════════════════

package display

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

const (
	// Reset
	Reset = "\033[0m"

	// Styles
	Bold      = "\033[1m"
	Dim       = "\033[2m"
	Italic    = "\033[3m"
	Underline = "\033[4m"

	// Standard Colors
	Black   = "\033[30m"
	Red     = "\033[31m"
	Green   = "\033[32m"
	Yellow  = "\033[33m"
	Blue    = "\033[34m"
	Magenta = "\033[35m"
	Cyan    = "\033[36m"
	White   = "\033[37m"
	Gray    = "\033[90m"

	// Bright Colors
	BrightBlack   = "\033[90m"
	BrightRed     = "\033[91m"
	BrightGreen   = "\033[92m"
	BrightYellow  = "\033[93m"
	BrightBlue    = "\033[94m"
	BrightMagenta = "\033[95m"
	BrightCyan    = "\033[96m"
	BrightWhite   = "\033[97m"

	// Backgrounds
	BgBlack   = "\033[40m"
	BgRed     = "\033[41m"
	BgGreen   = "\033[42m"
	BgYellow  = "\033[43m"
	BgBlue    = "\033[44m"
	BgMagenta = "\033[45m"
	BgCyan    = "\033[46m"
	BgWhite   = "\033[47m"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// Colorize wraps text in color codes
func Colorize(text, color string) string {
	return color + text + Reset
}

// BoldColorize wraps text in bold + color
func BoldColorize(text, color string) string {
	return Bold + color + text + Reset
}

// Dim applies dim styling
func DimText(text string) string {
	return Dim + text + Reset
}

// Success formats as green
func Success(text string) string {
	return Green + text + Reset
}

// Warning formats as yellow
func Warning(text string) string {
	return Yellow + text + Reset
}

// Error formats as red
func Error(text string) string {
	return Red + text + Reset
}

// Info formats as cyan
func Info(text string) string {
	return Cyan + text + Reset
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
