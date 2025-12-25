// ============================================================================
// METADATA
// ============================================================================
//
// Display Layout Primitive - Spacing and Padding Constants
//
// Biblical Foundation: See format.go (rails pattern applies to all primitives)
// CPI-SI Identity: RAIL PRIMITIVE (orthogonal infrastructure component)
// Component Type: Layout calculation constants for consistent formatting
//
// Purpose: Provides spacing, padding, and width constants for aligned output
//
// Authorship: Nova Dawn (extracted 2025-11-21 from format.go v2.0.0)
// Version: 1.0.0
//
// HEALTH SCORING MAP: N/A (constants only, no execution)
//
package display

// ============================================================================
// SETUP
// ============================================================================

// No imports needed - pure constants

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Layout Constants
// ────────────────────────────────────────────────────────────────
//
// Future Configuration (Phase 7+):
//   - Load from system/data/config/display/formatting.jsonc
//   - Allow runtime adjustment without recompilation

const (
	// IndentSpaces provides consistent indentation for hierarchical content.
	//
	// Used in: KeyValue(), StatusLine() for visual nesting (2 spaces)
	// Matches: formatting.jsonc layout.indentation.key_value and status_line
	IndentSpaces = "  "

	// KeyColumnWidth defines fixed-width column for key-value pair keys.
	//
	// Used in: KeyValue() for left-aligned key column (20 characters)
	// Matches: formatting.jsonc layout.key_value.column_width
	// Rationale: Wide enough for most labels, maintains clean vertical alignment
	KeyColumnWidth = 20

	// HeaderPadding defines extra characters added to header separator width.
	//
	// Used in: Header() to extend separator beyond title (title length + 4)
	// Matches: formatting.jsonc layout.header.padding
	// Rationale: Visual breathing room around centered title
	HeaderPadding = 4

	// TableColumnPadding defines spacing between table columns.
	//
	// Used in: Table.Render() for column separation (2 spaces after each column)
	// Matches: formatting.jsonc layout.table.column_padding
	// Rationale: Prevents visual crowding while maintaining compact layout
	TableColumnPadding = 2

	// BoxBorderPadding defines space added for box borders and internal padding.
	//
	// Used in: Box() to calculate total box width (content width + 4)
	// Matches: formatting.jsonc layout.box.width_padding
	// Breakdown: 2 chars for left border+space (│ ), 2 chars for right space+border ( │)
	// Rationale: Ensures content doesn't touch borders, maintains readability
	BoxBorderPadding = 4
)

// ============================================================================
// CLOSING
// ============================================================================
//
// Code Validation: Compile with format.go (go build ./display)
// Code Execution: Constants available at compile time
// Code Cleanup: None needed (constants have no runtime state)
//
// Modification Policy:
//   ✅ Safe: Adding new layout constants (extend layout options)
//   ⚠️ Care: Changing existing constant values (affects all output alignment)
//   ❌ Never: Removing constants in use (breaks calling code)
//
// Quick Reference:
//   indentedText := IndentSpaces + "Content"
//   keyValue := fmt.Sprintf("%-*s %s", KeyColumnWidth, "Key:", "Value")
