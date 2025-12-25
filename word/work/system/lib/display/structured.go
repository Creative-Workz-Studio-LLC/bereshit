// ============================================================================
// METADATA
// ============================================================================
//
// Display Structured Primitive - Headers and Key-Value Formatters
//
// Biblical Foundation: See format.go (rails pattern applies to all primitives)
// CPI-SI Identity: RAIL PRIMITIVE (orthogonal infrastructure component)
// Component Type: Structured output formatters with alignment and separators
//
// Purpose: Provides Header, Subheader, KeyValue, and StatusLine formatting
//
// Authorship: Nova Dawn (extracted 2025-11-21 from format.go v2.0.0)
// Version: 1.0.0
//
// HEALTH SCORING MAP (Total = 100):
//   Header() (25): Validate → calculate separator → format with colors
//   Subheader() (25): Validate → format bold title with colon
//   KeyValue() (25): Validate → apply column alignment → format
//   StatusLine() (25): Validate → select icon/color based on status → format
//
package display

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"     // Formatted string construction
	"strings" // String repetition for separators
)

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Structured Output Formatters
// ────────────────────────────────────────────────────────────────
// Multi-line formatted output with headers, separators, and alignment.

// Header formats a major section header with top and bottom separators.
//
// What It Does:
//   - Creates bold cyan title with separator lines above and below
//   - Separator length = title length + padding (visual breathing room)
//   - Empty title returns empty string (self-evident validation)
//
// Parameters:
//   - title: The section header text
//
// Returns:
//   - Three-line formatted header with separators, or "" if title empty
//
// Example:
//   fmt.Println(Header("SYSTEM STATUS"))
//   // Output:
//   // ────────────────
//   //  SYSTEM STATUS
//   // ────────────────
func Header(title string) string {
	defer recoverFromPanic()
	if title == "" {
		return "" // Self-evident validation: empty input = empty output
	}

	// Config with tripwires - layout
	cfg := GetConfig()

	padding := cfg.Layout.Header.Padding
	if padding == 0 {
		padding = HeaderPadding
	}

	// Config with tripwires - colors
	colorBoldCyan := cfg.Colors.BoldForeground.BoldCyan
	if colorBoldCyan == "" {
		colorBoldCyan = BoldCyan
	}

	colorReset := cfg.Colors.Basic.Reset
	if colorReset == "" {
		colorReset = Reset
	}

	separator := strings.Repeat("─", len(title)+padding)
	return fmt.Sprintf("\n%s%s%s\n%s %s %s\n%s%s%s\n",
		colorBoldCyan, separator, colorReset,
		colorBoldCyan, title, colorReset,
		colorBoldCyan, separator, colorReset,
	)
}

// Subheader formats a minor subsection header.
//
// What It Does:
//   - Creates bold title with colon (less prominent than Header)
//   - Empty title returns empty string (self-evident validation)
//
// Parameters:
//   - title: The subsection header text
//
// Returns:
//   - Single-line formatted subheader, or "" if title empty
//
// Example:
//   fmt.Println(Subheader("Environment Details"))
//   // Output:
//   // Environment Details: (in bold)
func Subheader(title string) string {
	defer recoverFromPanic()
	if title == "" {
		return "" // Self-evident validation: empty input = empty output
	}

	// Config with tripwires - colors
	cfg := GetConfig()

	colorBold := cfg.Colors.Basic.Bold
	if colorBold == "" {
		colorBold = Bold
	}

	colorReset := cfg.Colors.Basic.Reset
	if colorReset == "" {
		colorReset = Reset
	}

	return fmt.Sprintf("\n%s%s:%s\n", colorBold, title, colorReset)
}

// KeyValue formats a key-value pair with fixed-width alignment.
//
// What It Does:
//   - Displays key in dim text, left-aligned in configured column width
//   - Value in normal text, left-aligned after key column
//   - Indented for visual hierarchy
//   - Empty key returns empty string (self-evident validation)
//
// Parameters:
//   - key: The label/key (will have colon appended)
//   - value: The value to display
//
// Returns:
//   - Formatted "  key:            value" with fixed key column, or "" if key empty
//
// Example:
//   fmt.Println(KeyValue("Status", "healthy"))
//   fmt.Println(KeyValue("Version", "2.0.0"))
//   // Output:
//   //   Status:              healthy
//   //   Version:             2.0.0
func KeyValue(key, value string) string {
	defer recoverFromPanic()
	if key == "" {
		return "" // Self-evident validation: empty key = empty output
	}

	// Config with tripwires - layout
	cfg := GetConfig()

	indent := cfg.Layout.Indentation.KeyValue
	if indent == "" {
		indent = IndentSpaces
	}

	columnWidth := cfg.Layout.KeyValue.ColumnWidth
	if columnWidth == 0 {
		columnWidth = KeyColumnWidth
	}

	// Config with tripwires - colors
	colorDim := cfg.Colors.Basic.Dim
	if colorDim == "" {
		colorDim = Dim
	}

	colorReset := cfg.Colors.Basic.Reset
	if colorReset == "" {
		colorReset = Reset
	}

	return fmt.Sprintf("%s%s%-*s%s %s", indent, colorDim, columnWidth, key+":", colorReset, value)
}

// StatusLine formats a status line with success/failure icon.
//
// What It Does:
//   - Displays green ◉ for success (ok=true) or red ◯ for failure (ok=false)
//   - Indented for visual hierarchy
//   - Empty message returns empty string (self-evident validation)
//
// Parameters:
//   - ok: true for success (green ◉), false for failure (red ◯)
//   - message: The status message to display
//
// Returns:
//   - Formatted "  ◉ message" (green) or "  ◯ message" (red), or "" if message empty
//
// Example:
//   fmt.Println(StatusLine(true, "Configuration valid"))
//   fmt.Println(StatusLine(false, "Missing required file"))
//   // Output:
//   //   ◉ Configuration valid (green)
//   //   ◯ Missing required file (red)
func StatusLine(ok bool, message string) string {
	defer recoverFromPanic()
	if message == "" {
		return "" // Self-evident validation: empty message = empty output
	}

	// Config with tripwires - layout
	cfg := GetConfig()

	indent := cfg.Layout.Indentation.StatusLine
	if indent == "" {
		indent = IndentSpaces
	}

	// Config with tripwires - icons and colors (depends on ok status)
	var icon, color string
	if ok {
		icon = cfg.Icons.Status.Check
		if icon == "" {
			icon = IconCheck
		}

		color = cfg.Colors.Foreground.Green
		if color == "" {
			color = Green
		}
	} else {
		icon = cfg.Icons.Status.Cross
		if icon == "" {
			icon = IconCross
		}

		color = cfg.Colors.Foreground.Red
		if color == "" {
			color = Red
		}
	}

	colorReset := cfg.Colors.Basic.Reset
	if colorReset == "" {
		colorReset = Reset
	}

	return fmt.Sprintf("%s%s%s%s %s", indent, color, icon, colorReset, message)
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Code Validation: Compile with format.go (go build ./display)
// Code Execution: Library primitives (imported by format.go)
// Code Cleanup: None needed (stateless functions)
//
// Modification Policy:
//   ✅ Safe: Adding new structured formatters (follow existing pattern)
//   ⚠️ Care: Changing function signatures (breaks all calling code)
//   ❌ Never: Removing validation (violates self-evidence principle)
//
// Quick Reference:
//   fmt.Println(Header("SECTION"))
//   fmt.Println(Subheader("Details"))
//   fmt.Println(KeyValue("Key", "Value"))
//   fmt.Println(StatusLine(true, "OK"))
