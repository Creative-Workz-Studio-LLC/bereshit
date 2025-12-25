// ============================================================================
// METADATA
// ============================================================================
// JSONC Parsing Utilities - Foundation Primitive Library
//
// BIBLICAL FOUNDATION:
// Scripture: Proverbs 25:2 (WEB) - "It is the glory of God to conceal a thing,
//            but the glory of kings is to search out a matter."
// Principle: Discovery Through Removal - Comments conceal, stripping reveals
// Anchor: Matthew 13:52 (WEB) - "Therefore every scribe who has been made a
//         disciple in the Kingdom of Heaven is like a man who is a householder,
//         who brings out of his treasure new and old things."
//         (Comments are treasure - preserve meaning, remove syntax)
//
// CPI-SI IDENTITY:
// Type: FOUNDATION PRIMITIVE (lowest-level utility)
// Role: Shared JSONC parsing for all config-loading libraries
// Paradigm: Extract-once pattern - single source of truth
//
// PURPOSE & FUNCTION:
// Provides robust JSONC comment stripping and parsing utilities used by all
// system libraries that load configuration from JSONC files. Consolidates
// duplicated comment-stripping logic from 5 libraries into one primitive.
//
// AUTHORSHIP & LINEAGE:
// Version: 1.0.0
// Created: 2025-11-15
// Author: Nova Dawn (CPI-SI instance)
// History: Extracted from display/format.go, config/config.go, calendar/calendar.go,
//          privacy/privacy.go, validation/syntax.go during Phase 10 architectural
//          refinement. Combines best approaches from all implementations.
//
// BLOCKING STATUS:
// Blocks: None (foundation primitive)
// Blocked By: None (stdlib-only)
//
// USAGE & INTEGRATION:
// Used by any library loading JSONC configuration files:
//   data, err := os.ReadFile(path)
//   cleaned := jsonc.StripComments(data)
//   json.Unmarshal(cleaned, &config)
//
// Or use convenience function:
//   err := jsonc.Load(path, &config)
//
// DEPENDENCIES:
// Standard Library: encoding/json, os, strings
// System Libraries: None (foundation primitive)
//
// HEALTH SCORING MAP (Total = 100):
// - StripComments function: 60 pts
//   * Correct string tracking: +20
//   * Handle // comments: +15
//   * Handle /* */ comments: +15
//   * Preserve valid content: +10
// - Load function: 40 pts
//   * File read: +15
//   * Comment stripping: +10 (delegates to StripComments)
//   * JSON unmarshal: +15

// ============================================================================
// SETUP
// ============================================================================

package jsonc

import (
	"encoding/json" // JSON unmarshaling after comment stripping
	"fmt"           // Error formatting
	"os"            // File reading for Load function
	"strings"       // String manipulation for comment stripping
)

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Comment Stripping - Core Functionality
// ────────────────────────────────────────────────────────────────

// StripComments removes JSONC-style comments (// and /* */) from data.
//
// Correctly handles:
//   - Single-line comments: // comment text
//   - Multi-line comments: /* comment text */
//   - String context: Doesn't strip // or /* inside strings
//   - Escape sequences: Handles \" inside strings
//
// What It Does:
//   - Processes data line by line
//   - Tracks string context (inside quotes or not)
//   - Removes // comments only when not in strings
//   - Removes /* */ comments spanning multiple lines
//   - Returns cleaned JSON ready for unmarshaling
//
// Parameters:
//   - data: Raw JSONC data with comments
//
// Returns:
//   - Cleaned JSON data (comments stripped)
//
// Health Scoring: 60 points total
//   - Correct string tracking: +20
//   - Handle // comments: +15
//   - Handle /* */ comments: +15
//   - Preserve valid content: +10
func StripComments(data []byte) []byte {
	lines := strings.Split(string(data), "\n")
	var result []string
	inMultiLineComment := false

	for _, line := range lines {
		// Track multi-line comment state
		if inMultiLineComment {
			// Look for end of multi-line comment
			if idx := strings.Index(line, "*/"); idx >= 0 {
				line = line[idx+2:] // Keep content after */
				inMultiLineComment = false
			} else {
				continue // Skip entire line (still in comment)
			}
		}

		// Check for start of multi-line comment (not in string)
		var cleaned strings.Builder
		inString := false
		escaped := false
		i := 0

		for i < len(line) {
			ch := line[i]

			// Handle escape sequences
			if escaped {
				cleaned.WriteByte(ch)
				escaped = false
				i++
				continue
			}

			if ch == '\\' {
				cleaned.WriteByte(ch)
				escaped = true
				i++
				continue
			}

			// Track string boundaries
			if ch == '"' {
				inString = !inString
				cleaned.WriteByte(ch)
				i++
				continue
			}

			// Check for comments (only when not in string)
			if !inString {
				// Check for single-line comment
				if i < len(line)-1 && ch == '/' && line[i+1] == '/' {
					break // Rest of line is comment
				}

				// Check for multi-line comment start
				if i < len(line)-1 && ch == '/' && line[i+1] == '*' {
					// Find end of comment on same line
					endIdx := strings.Index(line[i+2:], "*/")
					if endIdx >= 0 {
						// Comment ends on same line
						i += endIdx + 4 // Skip past */
						continue
					} else {
						// Comment continues to next line
						inMultiLineComment = true
						break
					}
				}
			}

			// Normal character - keep it
			cleaned.WriteByte(ch)
			i++
		}

		// Keep line (even if empty - valid in JSON)
		result = append(result, cleaned.String())
	}

	return []byte(strings.Join(result, "\n"))
}

// ────────────────────────────────────────────────────────────────
// Convenience Functions - Higher-Level Utilities
// ────────────────────────────────────────────────────────────────

// Load reads a JSONC file, strips comments, and unmarshals into v.
//
// What It Does:
//   - Reads file from disk
//   - Strips JSONC comments using StripComments
//   - Unmarshals cleaned JSON into provided struct
//
// Parameters:
//   - path: File path to JSONC file
//   - v: Pointer to struct to unmarshal into
//
// Returns:
//   - error: File read error, comment stripping error, or unmarshal error
//
// Health Scoring: 40 points total
//   - File read: +15
//   - Comment stripping: +10
//   - JSON unmarshal: +15
//
// Example:
//   var config MyConfig
//   err := jsonc.Load("/path/to/config.jsonc", &config)
func Load(path string, v interface{}) error {
	// Read file
	data, err := os.ReadFile(path)
	if err != nil {
		return fmt.Errorf("failed to read JSONC file: %w", err)
	}

	// Strip comments
	cleaned := StripComments(data)

	// Unmarshal JSON
	if err := json.Unmarshal(cleaned, v); err != nil {
		return fmt.Errorf("failed to unmarshal JSONC: %w", err)
	}

	return nil
}

// Parse strips comments from JSONC data and unmarshals into v.
//
// What It Does:
//   - Strips JSONC comments from provided data
//   - Unmarshals cleaned JSON into provided struct
//
// Parameters:
//   - data: Raw JSONC data
//   - v: Pointer to struct to unmarshal into
//
// Returns:
//   - error: Comment stripping error or unmarshal error
//
// Example:
//   data := []byte(`{"key": "value" // comment}`)
//   var config MyConfig
//   err := jsonc.Parse(data, &config)
func Parse(data []byte, v interface{}) error {
	// Strip comments
	cleaned := StripComments(data)

	// Unmarshal JSON
	if err := json.Unmarshal(cleaned, v); err != nil {
		return fmt.Errorf("failed to unmarshal JSONC: %w", err)
	}

	return nil
}

// ============================================================================
// CLOSING
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Code Validation
// ────────────────────────────────────────────────────────────────

// Build Requirements:
// - Go 1.16+ (for os.ReadFile)
// - No external dependencies
// - Compiles with: go build
//
// Testing:
// - Test with sample JSONC files containing:
//   * Single-line comments
//   * Multi-line comments
//   * Comments inside strings (should not strip)
//   * Escaped quotes in strings
//   * Mixed comment types

// ────────────────────────────────────────────────────────────────
// Code Execution
// ────────────────────────────────────────────────────────────────

// Library Pattern:
// This is a library, not an executable. Import and use functions:
//   import "system/lib/jsonc"
//   cleaned := jsonc.StripComments(data)
//   err := jsonc.Load(path, &config)

// ────────────────────────────────────────────────────────────────
// Code Cleanup
// ────────────────────────────────────────────────────────────────

// Resource Management:
// - No resources to clean up (stateless functions)
// - Garbage collector handles all memory

// ────────────────────────────────────────────────────────────────
// Library Overview & Integration Summary
// ────────────────────────────────────────────────────────────────

// JSONC Parsing Primitive
//
// Foundation-level utility consolidating JSONC comment stripping from 5
// libraries into single source of truth. Provides robust handling of both
// single-line and multi-line comments with correct string context tracking.
//
// Used By:
// - display/format.go (config loading)
// - config/config.go (user/instance/project configs)
// - calendar/calendar.go (calendar config loading)
// - privacy/privacy.go (privacy filter loading)
// - validation/syntax.go (validation config loading)
//
// Integration: Import and use StripComments, Load, or Parse functions

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────

// SAFE:
// - Adding new convenience functions (e.g., LoadWithFallback)
// - Adding validation (e.g., ValidateJSON before unmarshal)
// - Adding optional parameters (e.g., ParseWithOptions)

// CARE:
// - Changing StripComments logic (affects all 5 dependent libraries)
// - Changing function signatures (breaks existing usage)
// - Adding dependencies (keep stdlib-only)

// NEVER:
// - Remove or rename public functions (breaks dependents)
// - Change comment stripping behavior without testing all 5 libraries
// - Add non-stdlib dependencies (must remain foundation primitive)

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────

// Ladder Position: FOUNDATION PRIMITIVE (lowest rung)
// - No system dependencies
// - Stdlib-only
// - Used by config-loading libraries at all levels
//
// Baton Flow:
//   File path → Load() → os.ReadFile → StripComments → json.Unmarshal → Struct
//
// Not a rail (has specific purpose, not universal infrastructure)

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────

// Adding new convenience function:
//   1. Add function in "Convenience Functions" section
//   2. Follow existing patterns (Load, Parse)
//   3. Document health scoring
//   4. Update "Library Overview" section
//
// Improving comment stripping:
//   1. Modify StripComments function
//   2. Add comprehensive test cases
//   3. Test with all 5 dependent libraries
//   4. Verify no regressions

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────

// Approximate Processing Units (APU):
// - StripComments: ~100 APU (line iteration + string tracking)
// - Load: ~120 APU (file read + StripComments + unmarshal)
// - Parse: ~110 APU (StripComments + unmarshal)
//
// Optimization:
// - Uses strings.Builder for efficient string construction
// - Single-pass line processing
// - Minimal allocations

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────

// Problem: Comments inside strings are stripped
// Cause: String tracking failed (escaped quote handling)
// Solution: Verify escape sequence handling in StripComments
//
// Problem: Multi-line comments not removed
// Cause: inMultiLineComment state not tracked correctly
// Solution: Check /* and */ detection logic
//
// Problem: Valid JSON marked invalid after stripping
// Cause: Over-aggressive comment removal
// Solution: Test with edge cases (/* inside strings, etc.)

// ────────────────────────────────────────────────────────────────
// Related Components & Dependencies
// ────────────────────────────────────────────────────────────────

// Depends On:
// - encoding/json (stdlib)
// - os (stdlib)
// - strings (stdlib)
//
// Used By:
// - display (config loading)
// - config (user/instance/project loading)
// - calendar (calendar config)
// - privacy (filter loading)
// - validation (validation config)
//
// Replaces:
// - display/format.go loadConfig() inline logic (regexp approach)
// - config/config.go stripJSONComments() (line filtering)
// - calendar/calendar.go stripJSONCComments() (line filtering)
// - privacy/privacy.go stripJSONComments() (unknown)
// - validation/syntax.go stripJSONCComments() (unknown)

// ────────────────────────────────────────────────────────────────
// Future Expansions & Roadmap
// ────────────────────────────────────────────────────────────────

// Version 1.1.0:
// - Add LoadWithFallback (load with default on error)
// - Add ValidateJSON (pre-validation before unmarshal)
// - Add schema validation support
//
// Version 1.2.0:
// - Add streaming support for large JSONC files
// - Add line/column error reporting
// - Add partial parsing (continue on error)
//
// Version 2.0.0:
// - Add JSONC-to-JSON conversion tool
// - Add minification support (strip whitespace + comments)
// - Add pretty-printing after comment stripping

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────

// Biblical Anchor: Proverbs 25:2 - "It is the glory of God to conceal a thing,
//                  but the glory of kings is to search out a matter."
//
// Comments conceal meaning in syntax. This library reveals the treasure beneath.
//
// Design Guarantee: Stdlib-only foundation primitive with robust string handling.
// Comments are sacred - preserve their intent while removing their syntax.

// ────────────────────────────────────────────────────────────────
// Quick Reference: Usage Examples
// ────────────────────────────────────────────────────────────────

// Example 1: Strip comments from data
//   data := []byte(`{"key": "value" // comment}`)
//   cleaned := jsonc.StripComments(data)
//   // cleaned = []byte(`{"key": "value" }`)
//
// Example 2: Load JSONC file
//   type Config struct {
//       Key string `json:"key"`
//   }
//   var config Config
//   err := jsonc.Load("/path/to/config.jsonc", &config)
//
// Example 3: Parse JSONC data
//   data := os.ReadFile("config.jsonc")
//   var config Config
//   err := jsonc.Parse(data, &config)
//
// Example 4: Handle comments inside strings
//   data := []byte(`{"url": "http://example.com//path"}`)
//   cleaned := jsonc.StripComments(data)
//   // cleaned preserves // inside string
