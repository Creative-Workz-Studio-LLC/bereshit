// ============================================================================
// METADATA
// ============================================================================
//
// Display Icons Primitive - Unicode Status Symbols
//
// Biblical Foundation: See format.go (rails pattern applies to all primitives)
// CPI-SI Identity: RAIL PRIMITIVE (orthogonal infrastructure component)
// Component Type: Unicode icon constants for status indicators
//
// Purpose: Provides Unicode symbols for success, failure, warning, and status
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
// Status Icon Constants
// ────────────────────────────────────────────────────────────────
//
// Future Configuration (Phase 7+):
//   - Unicode mode (current): Full Unicode symbol set
//   - ASCII fallback: ✓→+, ✗→x, ⚠→!, i→i, ◉→*, ◯→o
//   - Emoji variant: ✓→✅, ✗→❌, ⚠→⚠️, etc.

const (
	// IconSuccess (✓) indicates successful operation completion.
	//
	// Unicode: U+2713 CHECK MARK
	// Used in: Success() for positive status messages
	IconSuccess = "✓"

	// IconFailure (✗) indicates operation failure or error condition.
	//
	// Unicode: U+2717 BALLOT X
	// Used in: Failure() for negative status messages
	IconFailure = "✗"

	// IconWarning (⚠) indicates caution or attention required.
	//
	// Unicode: U+26A0 WARNING SIGN
	// Used in: Warning() for non-critical issues requiring review
	IconWarning = "⚠"

	// IconInfo (i) indicates general informational message.
	//
	// Unicode: U+0069 LATIN SMALL LETTER I (standard lowercase i)
	// Used in: Info() for neutral informational output
	IconInfo = "i"

	// IconCheck (◉) indicates active/enabled state in status lines.
	//
	// Unicode: U+25C9 FISHEYE
	// Used in: StatusLine(true, ...) for positive boolean states
	IconCheck = "◉"

	// IconCross (◯) indicates inactive/disabled state in status lines.
	//
	// Unicode: U+25CB WHITE CIRCLE
	// Used in: StatusLine(false, ...) for negative boolean states
	IconCross = "◯"
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
//   ✅ Safe: Adding new icon constants (extend icon set)
//   ⚠️ Care: Changing existing constant values (affects all output)
//   ❌ Never: Removing constants in use (breaks calling code)
//
// Quick Reference:
//   successMsg := IconSuccess + " Operation complete"
//   failureMsg := IconFailure + " Operation failed"
