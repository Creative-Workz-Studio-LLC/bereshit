// ============================================================================
// METADATA
// ============================================================================
//
// Display Messages Primitive - Status Message Formatters
//
// Biblical Foundation: See format.go (rails pattern applies to all primitives)
// CPI-SI Identity: RAIL PRIMITIVE (orthogonal infrastructure component)
// Component Type: Single-line message formatters with icons and colors
//
// Purpose: Provides Success, Failure, Warning, and Info message formatting
//
// Authorship: Nova Dawn (extracted 2025-11-21 from format.go v2.0.0)
// Version: 1.0.0
//
// HEALTH SCORING MAP (Total = 100):
//   Success() (25): Validate → apply green + checkmark → return formatted
//   Failure() (25): Validate → apply red + X → return formatted
//   Warning() (25): Validate → apply yellow + warning → return formatted
//   Info() (25): Validate → apply cyan + info → return formatted
//
package display

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt" // Formatted string construction
)

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Message Formatters
// ────────────────────────────────────────────────────────────────
// Single-line status messages with icon and color. Empty messages return empty
// strings (validation through self-evidence - missing output signals invalid input).

// Success formats a success message with green checkmark.
//
// What It Does:
//   - Adds green color and ✓ icon to message
//   - Returns formatted string with ANSI codes
//   - Empty message returns empty string (self-evident validation)
//
// Parameters:
//   - message: The success message to format
//
// Returns:
//   - Formatted string: "✓ message" in green, or "" if message empty
//
// Example:
//   fmt.Println(Success("Configuration validated"))
//   // Output: ✓ Configuration validated (in green)
func Success(message string) string {
	defer recoverFromPanic()
	if message == "" {
		return "" // Self-evident validation: empty input = empty output
	}

	// Config with tripwires - fall back to constants if config invalid
	cfg := GetConfig()

	iconSuccess := cfg.Icons.Status.Success
	if iconSuccess == "" {
		iconSuccess = IconSuccess
	}

	colorGreen := cfg.Colors.Foreground.Green
	if colorGreen == "" {
		colorGreen = Green
	}

	colorReset := cfg.Colors.Basic.Reset
	if colorReset == "" {
		colorReset = Reset
	}

	return fmt.Sprintf("%s%s %s%s", colorGreen, iconSuccess, message, colorReset)
}

// Failure formats a failure message with red X.
//
// What It Does:
//   - Adds red color and ✗ icon to message
//   - Returns formatted string with ANSI codes
//   - Empty message returns empty string (self-evident validation)
//
// Parameters:
//   - message: The failure message to format
//
// Returns:
//   - Formatted string: "✗ message" in red, or "" if message empty
//
// Example:
//   fmt.Println(Failure("Validation failed"))
//   // Output: ✗ Validation failed (in red)
func Failure(message string) string {
	defer recoverFromPanic()
	if message == "" {
		return "" // Self-evident validation: empty input = empty output
	}

	// Config with tripwires
	cfg := GetConfig()

	iconFailure := cfg.Icons.Status.Failure
	if iconFailure == "" {
		iconFailure = IconFailure
	}

	colorRed := cfg.Colors.Foreground.Red
	if colorRed == "" {
		colorRed = Red
	}

	colorReset := cfg.Colors.Basic.Reset
	if colorReset == "" {
		colorReset = Reset
	}

	return fmt.Sprintf("%s%s %s%s", colorRed, iconFailure, message, colorReset)
}

// Warning formats a warning message with yellow warning symbol.
//
// What It Does:
//   - Adds yellow color and ⚠ icon to message
//   - Returns formatted string with ANSI codes
//   - Empty message returns empty string (self-evident validation)
//
// Parameters:
//   - message: The warning message to format
//
// Returns:
//   - Formatted string: "⚠ message" in yellow, or "" if message empty
//
// Example:
//   fmt.Println(Warning("Configuration using defaults"))
//   // Output: ⚠ Configuration using defaults (in yellow)
func Warning(message string) string {
	defer recoverFromPanic()
	if message == "" {
		return "" // Self-evident validation: empty input = empty output
	}

	// Config with tripwires
	cfg := GetConfig()

	iconWarning := cfg.Icons.Status.Warning
	if iconWarning == "" {
		iconWarning = IconWarning
	}

	colorYellow := cfg.Colors.Foreground.Yellow
	if colorYellow == "" {
		colorYellow = Yellow
	}

	colorReset := cfg.Colors.Basic.Reset
	if colorReset == "" {
		colorReset = Reset
	}

	return fmt.Sprintf("%s%s %s%s", colorYellow, iconWarning, message, colorReset)
}

// Info formats an informational message with cyan info symbol.
//
// What It Does:
//   - Adds cyan color and i icon to message
//   - Returns formatted string with ANSI codes
//   - Empty message returns empty string (self-evident validation)
//
// Parameters:
//   - message: The informational message to format
//
// Returns:
//   - Formatted string: "i message" in cyan, or "" if message empty
//
// Example:
//   fmt.Println(Info("System initialized"))
//   // Output: i System initialized (in cyan)
func Info(message string) string {
	defer recoverFromPanic()
	if message == "" {
		return "" // Self-evident validation: empty input = empty output
	}

	// Config with tripwires
	cfg := GetConfig()

	iconInfo := cfg.Icons.Status.Info
	if iconInfo == "" {
		iconInfo = IconInfo
	}

	colorCyan := cfg.Colors.Foreground.Cyan
	if colorCyan == "" {
		colorCyan = Cyan
	}

	colorReset := cfg.Colors.Basic.Reset
	if colorReset == "" {
		colorReset = Reset
	}

	return fmt.Sprintf("%s%s %s%s", colorCyan, iconInfo, message, colorReset)
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
//   ✅ Safe: Adding new message formatters (follow existing pattern)
//   ⚠️ Care: Changing function signatures (breaks all calling code)
//   ❌ Never: Removing validation (violates self-evidence principle)
//
// Quick Reference:
//   fmt.Println(Success("Done"))
//   fmt.Println(Failure("Error"))
//   fmt.Println(Warning("Caution"))
//   fmt.Println(Info("Notice"))
