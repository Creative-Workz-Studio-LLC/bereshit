// ============================================================================
// METADATA
// ============================================================================
//
// Display Visual Primitive - Complex Visual Components
//
// Biblical Foundation: See format.go (rails pattern applies to all primitives)
// CPI-SI Identity: RAIL PRIMITIVE (orthogonal infrastructure component)
// Component Type: Complex formatted output with tables, progress bars, boxes
//
// Purpose: Provides Table, ProgressBar, and Box visual components
//
// Authorship: Nova Dawn (extracted 2025-11-21 from format.go v2.0.0)
// Version: 1.0.0
//
// HEALTH SCORING MAP (Total = 100):
//   Table.Render() (40): Validate → calculate widths → render headers/rows
//   ProgressBar() (30): Validate → calculate percentage → render bar
//   Box() (30): Validate → calculate max width → render borders
//
package display

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"     // Formatted string construction
	"strings" // String manipulation and repetition
)

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────

// Table creates formatted multi-column tables with headers and optional column colors.
//
// Automatically calculates column widths based on content, ensuring alignment
// across all rows. Supports optional per-column coloring for visual categorization.
// Returns empty string for invalid configurations (no headers, no rows).
//
// Layout algorithm:
//   1. Calculate max width per column (header vs all row cells)
//   2. Add 2-space padding after each column
//   3. Render: bold headers → separator line (─) → colored rows
//
// Memory: ~(rows × cols × avg_cell_length) bytes for content
// Performance: O(rows × cols) for width calculation and rendering
//
// Example usage:
//
//	table := &display.Table{
//	    Headers: []string{"Name", "Status", "Score"},
//	    Rows: [][]string{
//	        {"Alice", "Active", "95"},
//	        {"Bob", "Inactive", "72"},
//	    },
//	    Colors: []string{"", display.Green, display.Cyan},
//	}
//	output := table.Render()
type Table struct {
	Headers []string   // Column headers displayed in bold at top of table
	Rows    [][]string // Table data rows - each row is slice of cell values
	Colors  []string   // Optional ANSI color codes per column (empty string = no color)
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Visual Components
// ────────────────────────────────────────────────────────────────
// Complex formatted output requiring width calculations and multi-line rendering.

// Render renders the table to a formatted string with aligned columns.
//
// What It Does:
//   - Calculates column widths from headers and cell content
//   - Renders bold header row with separator line
//   - Renders data rows with optional per-column coloring
//   - Handles mismatched row lengths gracefully (defensive bounds checking)
//   - Empty table or missing headers returns empty string (self-evident validation)
//
// Parameters:
//   - t: Table receiver with Headers, Rows, and optional Colors fields
//
// Returns:
//   - Multi-line formatted table string, or "" if invalid (empty/no headers)
//
// Health Impact:
//   - Invalid input (empty table, no headers) = returns "" (visibly obvious)
//   - Mismatched row lengths = handled defensively (no panic)
//
// Example:
//   table := &Table{
//       Headers: []string{"Component", "Status"},
//       Rows: [][]string{
//           {"Logging", "healthy"},
//           {"Display", "healthy"},
//       },
//       Colors: []string{Green, ""},
//   }
//   fmt.Println(table.Render())
//   // Output:
//   // Component    Status
//   // ──────────────────
//   // Logging      healthy  (in green)
//   // Display      healthy
func (t *Table) Render() string {
	defer recoverFromPanic()

	// Validation: empty table or no headers (self-evident - returns empty string)
	if len(t.Rows) == 0 || len(t.Headers) == 0 {
		return ""
	}

	// Calculate column widths from headers and all cells
	widths := make([]int, len(t.Headers))
	for i, h := range t.Headers {
		widths[i] = len(h)
	}

	for _, row := range t.Rows {
		for i, cell := range row {
			// Defensive: handle rows with more/fewer cells than headers
			if i < len(widths) && len(cell) > widths[i] {
				widths[i] = len(cell)
			}
		}
	}

	var result strings.Builder

	// Config with tripwires - layout
	cfg := GetConfig()

	columnPadding := cfg.Layout.Table.ColumnPadding
	if columnPadding == 0 {
		columnPadding = TableColumnPadding
	}

	// Config with tripwires - colors
	colorBold := cfg.Colors.Basic.Bold
	if colorBold == "" {
		colorBold = Bold
	}

	colorReset := cfg.Colors.Basic.Reset
	if colorReset == "" {
		colorReset = Reset
	}

	// Render header row (bold)
	result.WriteString(colorBold)
	for i, h := range t.Headers {
		result.WriteString(fmt.Sprintf("%-*s", widths[i]+columnPadding, h))
	}
	result.WriteString(colorReset + "\n")

	// Render separator line
	for i := range t.Headers {
		result.WriteString(strings.Repeat("─", widths[i]+columnPadding))
	}
	result.WriteString("\n")

	// Render data rows with optional per-column coloring
	for _, row := range t.Rows {
		for i, cell := range row {
			// Apply color if specified for this column
			if i < len(t.Colors) && t.Colors[i] != "" {
				result.WriteString(t.Colors[i])
			}
			// Defensive: only render if within calculated widths
			if i < len(widths) {
				result.WriteString(fmt.Sprintf("%-*s", widths[i]+columnPadding, cell))
			}
			// Reset color if it was applied
			if i < len(t.Colors) && t.Colors[i] != "" {
				result.WriteString(colorReset)
			}
		}
		result.WriteString("\n")
	}

	return result.String()
}

// ProgressBar creates a visual progress bar with percentage.
//
// What It Does:
//   - Calculates percentage as current/total
//   - Renders bar with filled (█) and empty (░) characters
//   - Displays numeric progress and percentage: [████░░░░] 4/8 (50%)
//   - Validates inputs and clamps filled count to valid range [0, width]
//   - Invalid inputs (total=0, negative values) return empty string (self-evident validation)
//
// Parameters:
//   - current: Current progress value (e.g., completed items)
//   - total: Total target value (e.g., total items)
//   - width: Visual width of the bar in characters
//
// Returns:
//   - Formatted progress bar string, or "" if invalid inputs
//
// Health Impact:
//   - Divide by zero (total=0) = returns "" (visibly obvious)
//   - Negative values = returns "" (visibly obvious)
//   - Clamping prevents visual corruption from calculation errors
//
// Configuration Note:
//   - Bar characters (█, ░) will be configurable in Phase 7 via formatting.jsonc
//
// Example:
//   fmt.Println(ProgressBar(3, 10, 20))
//   // Output: [██████░░░░░░░░░░░░░░] 3/10 (30%)
func ProgressBar(current, total int, width int) string {
	defer recoverFromPanic()

	// Validation: prevent divide by zero
	if total == 0 {
		return "" // Self-evident: empty output signals invalid input
	}

	// Validation: prevent negative values
	if current < 0 || total < 0 || width < 0 {
		return "" // Self-evident: empty output signals invalid input
	}

	// Calculate percentage and filled character count
	percentage := float64(current) / float64(total)
	filled := int(percentage * float64(width))

	// Clamp filled to valid range [0, width] to prevent visual corruption
	if filled < 0 {
		filled = 0
	}
	if filled > width {
		filled = width
	}

	// Construct bar with filled and empty characters
	bar := strings.Repeat("█", filled) + strings.Repeat("░", width-filled)
	return fmt.Sprintf("[%s] %d/%d (%.0f%%)", bar, current, total, percentage*100)
}

// Box creates a boxed message with title and border.
//
// What It Does:
//   - Calculates max width from title and all message lines
//   - Renders Unicode box border (┌─┐│└┘) around content
//   - Title displayed in bold cyan on top line
//   - Message lines padded to max width for clean alignment
//   - Empty title AND message returns empty string (self-evident validation)
//   - Newlines in title are removed (single-line title enforcement)
//
// Parameters:
//   - title: Single-line box title (newlines stripped)
//   - message: Multi-line message content (preserves newlines)
//
// Returns:
//   - Multi-line boxed output with borders, or "" if both inputs empty
//
// Health Impact:
//   - Both inputs empty = returns "" (visibly obvious)
//   - Title with newlines = newlines stripped (defensive handling)
//
// Configuration Note:
//   - Box characters (┌─┐│└┘) will be configurable in Phase 7 via formatting.jsonc
//
// Example:
//   fmt.Println(Box("System Status", "All services running\nNo errors detected"))
//   // Output:
//   // ┌────────────────────────┐
//   // │ System Status          │
//   // ├────────────────────────┤
//   // │ All services running   │
//   // │ No errors detected     │
//   // └────────────────────────┘
func Box(title, message string) string {
	defer recoverFromPanic()

	// Validation: both inputs empty (self-evident validation)
	if title == "" && message == "" {
		return ""
	}

	// Defensive: strip newlines from title (enforce single-line title)
	title = strings.ReplaceAll(title, "\n", " ")

	// Split message into lines and calculate max width
	lines := strings.Split(message, "\n")
	maxWidth := len(title)

	for _, line := range lines {
		if len(line) > maxWidth {
			maxWidth = len(line)
		}
	}

	// Config with tripwires - layout
	cfg := GetConfig()

	borderPadding := cfg.Layout.Box.WidthPadding
	if borderPadding == 0 {
		borderPadding = BoxBorderPadding
	}

	// Config with tripwires - colors
	colorBold := cfg.Colors.Basic.Bold
	if colorBold == "" {
		colorBold = Bold
	}

	colorBoldCyan := cfg.Colors.BoldForeground.BoldCyan
	if colorBoldCyan == "" {
		colorBoldCyan = BoldCyan
	}

	colorReset := cfg.Colors.Basic.Reset
	if colorReset == "" {
		colorReset = Reset
	}

	// Calculate box width (content + border padding for borders and internal padding)
	width := maxWidth + borderPadding
	top := "┌" + strings.Repeat("─", width-2) + "┐"
	bottom := "└" + strings.Repeat("─", width-2) + "┘"
	separator := "├" + strings.Repeat("─", width-2) + "┤"
	titleLine := fmt.Sprintf("│ %s%-*s%s │", colorBold, maxWidth, title, colorReset)

	// Build box output
	var result strings.Builder
	result.WriteString(colorBoldCyan + top + colorReset + "\n")
	result.WriteString(colorBoldCyan + titleLine + colorBoldCyan + colorReset + "\n")
	result.WriteString(colorBoldCyan + separator + colorReset + "\n")

	for _, line := range lines {
		result.WriteString(fmt.Sprintf("│ %-*s │\n", maxWidth, line))
	}

	result.WriteString(colorBoldCyan + bottom + colorReset + "\n")

	return result.String()
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
//   ✅ Safe: Adding new visual components (follow existing pattern)
//   ⚠️ Care: Changing function signatures or Table struct (breaks calling code)
//   ❌ Never: Removing validation (violates self-evidence principle)
//
// Quick Reference:
//   table := &Table{Headers: []string{"A"}, Rows: [][]string{{"B"}}}
//   fmt.Println(table.Render())
//   fmt.Println(ProgressBar(5, 10, 20))
//   fmt.Println(Box("Title", "Message"))
