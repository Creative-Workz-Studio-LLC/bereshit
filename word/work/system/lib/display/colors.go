// ============================================================================
// METADATA
// ============================================================================
//
// Display Colors Primitive - ANSI Escape Sequences
//
// Biblical Foundation: See format.go (rails pattern applies to all primitives)
// CPI-SI Identity: RAIL PRIMITIVE (orthogonal infrastructure component)
// Component Type: ANSI color code constants for terminal formatting
//
// Purpose: Provides ANSI escape sequence constants for colored terminal output
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
// ANSI Color Constants
// ────────────────────────────────────────────────────────────────
//
// ANSI Color Format: \033[<attributes>m where attributes are numeric codes:
//   0 = reset all formatting
//   1 = bold text weight
//   2 = dim text intensity
//   31-37 = foreground colors (red, green, yellow, blue, magenta, cyan, gray)
//   1;3X = bold + foreground color combination
//
// Future Configuration (Phase 7+):
//   - Load from system/data/config/display/formatting.jsonc
//   - Support 256-color (\033[38;5;Nm) and RGB (\033[38;2;R;G;Bm)
//   - Automatic terminal capability detection for mode selection

const (
	//--- Text Modifiers ---

	// Reset clears all formatting returning terminal to default state.
	// Applied at end of every formatted string to prevent color bleeding.
	Reset = "\033[0m"

	// Bold increases text weight for emphasis in headers and important text.
	// Used in Header(), Subheader(), and Table headers.
	Bold = "\033[1m"

	// Dim reduces text intensity for de-emphasized content.
	// Applied to KeyValue() keys showing they're labels not values.
	Dim = "\033[2m"

	//--- Foreground Colors ---

	// Red indicates errors, failures, and critical issues.
	Red = "\033[31m" // Failure(), negative StatusLine(), critical messages

	// Green indicates success, completion, and positive states.
	Green = "\033[32m" // Success(), positive StatusLine(), completion indicators

	// Yellow indicates warnings and caution requiring attention.
	Yellow = "\033[33m" // Warning(), non-critical issues, review needed

	// Blue provides neutral informational coloring.
	Blue = "\033[34m" // Currently unused - reserved for future info categories

	// Magenta provides distinct categorization separate from status.
	Magenta = "\033[35m" // Currently unused - reserved for special categories

	// Cyan indicates general information and structural elements.
	Cyan = "\033[36m" // Info(), general informational messages

	// Gray provides muted text for secondary information.
	Gray = "\033[37m" // Currently unused - available for low-priority content

	//--- Bold Foreground Colors ---

	// BoldRed emphasizes critical failures and errors demanding attention.
	BoldRed = "\033[1;31m" // Currently unused - available for critical emphasis

	// BoldGreen emphasizes important successes and achievements.
	BoldGreen = "\033[1;32m" // Currently unused - available for success emphasis

	// BoldYellow emphasizes important warnings requiring immediate review.
	BoldYellow = "\033[1;33m" // Currently unused - available for warning emphasis

	// BoldBlue provides emphasized informational hierarchy.
	BoldBlue = "\033[1;34m" // Currently unused - reserved for emphasized info

	// BoldMagenta provides emphasized special categorization.
	BoldMagenta = "\033[1;35m" // Currently unused - reserved for special emphasis

	// BoldCyan emphasizes structural headers and important information.
	BoldCyan = "\033[1;36m" // Header(), Box() borders - visual structure emphasis
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
//   ✅ Safe: Adding new color constants (extend palette)
//   ⚠️ Care: Changing existing constant values (affects all output)
//   ❌ Never: Removing constants in use (breaks calling code)
//
// Quick Reference:
//   coloredText := Green + "Success" + Reset
//   boldHeader := BoldCyan + "HEADER" + Reset
