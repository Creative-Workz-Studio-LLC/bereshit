// ============================================================================
// METADATA
// ============================================================================
//
// Display Recovery Primitive - Panic Recovery for Self-Evident Failure
//
// Biblical Foundation: See format.go (rails pattern applies to all primitives)
// CPI-SI Identity: RAIL PRIMITIVE (orthogonal infrastructure component)
// Component Type: Error recovery mechanism for display rail
//
// Purpose: Provides panic recovery for all display formatting functions,
//          implementing rails self-evidence pattern (silent recovery, broken
//          output IS the failure signal)
//
// Authorship: Nova Dawn (extracted 2025-11-21 from format.go v2.0.0)
// Version: 1.0.0
//
// HEALTH SCORING MAP (Total = 100):
//   Panic Recovery (100): recover() captures panic → function returns zero value
//
package display

// ============================================================================
// SETUP
// ============================================================================

// No imports needed - stdlib recovery mechanism only

// ============================================================================
// BODY
// ============================================================================

// recoverFromPanic recovers from panics in format functions.
//
// Rails Pattern: Silent recovery is appropriate because rails are self-evident.
// If a display function panics, the output will be visibly broken or missing -
// this IS the failure signal. No logging needed because the failure is immediately
// obvious to anyone viewing the output.
//
// What It Does:
//   - Recovers from panic in defer chains
//   - Silently consumes panic value (broken output is the signal)
//   - Allows function to return zero value (empty string for most formatters)
//
// Health Impact (Rails Self-Evidence):
//   - Panic recovery = function returns empty string
//   - Empty string in output = visibly obvious failure
//   - No health tracking needed (failure is self-evident)
//
// Example:
//   defer recoverFromPanic()  // Placed at start of public functions
func recoverFromPanic() {
	if r := recover(); r != nil {
		// Silent recovery - panic in formatting is self-evident through broken output
		// No logging needed - rails don't track themselves
		_ = r
	}
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Code Validation: Compile with format.go (go build ./display)
// Code Execution: Library primitive (imported by format.go)
// Code Cleanup: None needed (stateless function)
//
// Modification Policy:
//   ✅ Safe: N/A (single primitive function)
//   ⚠️ Care: Changing recovery behavior (affects all display functions)
//   ❌ Never: Adding logging or tracking (violates rails self-evidence)
//
// Quick Reference:
//   defer recoverFromPanic()  // At start of every public display function
