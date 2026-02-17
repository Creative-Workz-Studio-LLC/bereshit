//omni:code --go -library
//omni:key B-L0-hybrid-format
//omni:version b-01.01
//omni:pragma _pragma
//omni:metadata _metadata

// ============================================================================
// METADATA
// ============================================================================

// Package format provides shared formatting utilities for CPI-SI CLI output.
//
// Duration, path, number, string, and model name formatting — all pure stdlib.
// No external dependencies. Used by statusline, session skills, and any CLI
// tool that needs consistent human-readable output.
//
// Usage:
//
//	format.Duration(elapsed)           // "3m 42s"
//	format.ShortenPath("/long/path")   // "~/.../last/two"
//	format.Truncate(longStr, 30)       // "first 27 chars..."
//	format.Percent(75, 100)            // "75%"
//
// See _pragma and _metadata for OmniCode identity.
package format

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

//--- I.1 Standard Library [IMPORT] (1) ---
// Path, string, and time utilities
import (
	"os"      // UserHomeDir
	"strings" // path splitting, padding
	"time"    // Duration type
)

//--- I.3 Internal Packages [IMPORT] (0) ---
// L0 identity registry — package self-awareness
import (
	"creativeworkzstudio.com/bereshit/L0-universal/spiral/identity" // Register — package identity registry
)

// ────────────────────────────────────────────────────────────────
// Pragma
// ────────────────────────────────────────────────────────────────

// _pragma — OmniCode file identity (P1-P5). Runtime-queryable.
var _pragma = map[string]string{
	// P1: Core Identity
	"P1.key":    "B-L0-hybrid-format",
	"P1.type":   "code",
	"P1.format": "go",
	"P1.style":  "library",
	// P2: Structure
	"P2.blocks": "4-block",
	// P3: Derivation
	"P3.from":    "b-word/seed/code/L0/go/library.go",
	"P3.derives": "B-L0-seed-go-library",
	// P4: Version
	"P4.at": "b-01.01",
	// P5: Summary
	"P5.title":   "Format Utilities Library",
	"P5.summary": "Shared formatting for durations, paths, numbers, and strings",
}

// ────────────────────────────────────────────────────────────────
// Metadata
// ────────────────────────────────────────────────────────────────

// _metadata — OmniCode file metadata (M1-M10). Runtime-queryable.
var _metadata = map[string]string{

	// ── M1: Core Identity ─────────────────────────────────────────
	"M1.key":            "B-L0-hybrid-format",
	"M1.component_type": "Foundation",
	"M1.architect":      "Seanje Lenox-Wise",
	"M1.implementation": "Nova Dawn",
	"M1.created":        "2025-12-10",

	// ── M2: Version History ───────────────────────────────────────
	"M2.b-01.01": "2026-02-14 — REORG: promoted from hybrid/util/ to hybrid/ top-level domain",
	"M2.b-01.00": "2026-02-14 — BASELINE: 4-block, identity registration, pragma/metadata",
	"M2.a-01.00": "2025-12-10 — Initial creation",

	// ── M3: Interface ─────────────────────────────────────────────
	"M3.requires.stdlib":   "os, strings, time",
	"M3.requires.external": "none",
	"M3.requires.internal": "L0-universal/spiral/identity",
	"M3.used_by":           "statusline, session skills, CLI output formatting",
	"M3.import":            "creativeworkzstudio.com/bereshit/L0-universal/hybrid/format",
	"M3.pattern":           "format.Duration(d), format.ShortenPath(p), format.Truncate(s, n)",

	// ── M4: Public API ────────────────────────────────────────────
	"M4.duration": "Duration, DurationShort, DurationMS, Minutes",
	"M4.path":     "ShortenPath, BaseName",
	"M4.number":   "Itoa, Percent",
	"M4.string":   "Truncate, PadRight, PadLeft",
	"M4.model":    "ShortModelName, GetShortModelName",
	"M4.self":     "Pragma, Metadata",

	// ── M5: Operational ───────────────────────────────────────────
	"M5.blocking": "no — pure value formatting only",
	"M5.health":   "Foundation | granted: formatted | deferred: n/a | denied: n/a",

	// ── M6: Classification ────────────────────────────────────────
	"M6.tags":     "format, duration, path, number, string, pure, stdlib-only",
	"M6.category": "Foundation",
	"M6.domain":   "output-formatting",
	"M6.layer":    "L0-universal",
	"M6.paradigm": "CPI-SI",

	// ── M7: Intent ────────────────────────────────────────────────
	"M7.purpose":    "Consistent human-readable formatting across all CPI-SI tools",
	"M7.philosophy": "Pure functions, no side effects — format values, never print them",
	"M7.provides":   "Duration, path, number, string, and model name formatting",

	// ── M8: Grounding ─────────────────────────────────────────────
	"M8.scripture": "Proverbs 4:26 — Ponder the path of thy feet",
	"M8.principle": "Clarity in presentation — format data so humans can ponder it",
	"M8.anchor":    "Genesis 1:1 — In the beginning God created the heaven and the earth",

	// ── M9: Dependencies ──────────────────────────────────────────
	"M9.needs.stdlib":   "os (home dir), strings (manipulation), time (Duration type)",
	"M9.needs.external": "none",
	"M9.needs.internal": "L0-universal/spiral/identity",
	"M9.used_by":        "statusline, session skills, all CPI-SI CLI commands",
	"M9.layer_deps":     "L0 only — no L1+ dependencies",

	// ── M10: Roadmap ──────────────────────────────────────────────
	"M10.current":     "b-01.00 — BASELINE: standard formatting with identity",
	"M10.planned":     "Byte size formatting, color/ANSI formatting, locale awareness",
	"M10.limitations": "No color output, no locale support, no streaming formatters",
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// No types, constants, or package-level variables beyond pragma/metadata.
// All functions are pure — no shared state.

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: Identity → Helpers → Duration → Path → Number → String → Model

// ────────────────────────────────────────────────────────────────
// Identity — Package Self-Registration
// ────────────────────────────────────────────────────────────────

// init registers this package in the OmniCode identity registry.
func init() {
	identity.Register(_pragma["P1.key"], _pragma, _metadata)
}

// Pragma returns this package's OmniCode pragma identity (P1-P5).
func Pragma() map[string]string { return _pragma }

// Metadata returns this package's OmniCode metadata (M1-M10).
func Metadata() map[string]string { return _metadata }

// ────────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────────

// itoa converts an integer to a string without importing strconv.
// Keeps this package free of strconv dependency for minimal footprint.
func itoa(i int) string {
	if i == 0 {
		return "0"
	}
	if i < 0 {
		return "-" + itoa(-i)
	}

	var digits []byte
	for i > 0 {
		digits = append([]byte{byte('0' + i%10)}, digits...)
		i /= 10
	}
	return string(digits)
}

// ────────────────────────────────────────────────────────────────
// Duration Formatting
// ────────────────────────────────────────────────────────────────

// Duration formats a time.Duration as a human-readable string.
// Returns "Xh Ym" for hours+, "Xm Ys" for minutes+, "Xs" for seconds.
func Duration(d time.Duration) string {
	hours := int(d.Hours())
	minutes := int(d.Minutes()) % 60
	seconds := int(d.Seconds()) % 60

	if hours > 0 {
		return itoa(hours) + "h " + itoa(minutes) + "m"
	}
	if minutes > 0 {
		return itoa(minutes) + "m " + itoa(seconds) + "s"
	}
	return itoa(seconds) + "s"
}

// DurationShort formats a duration compactly without spaces.
// Returns "XhYm" for hours+, "Xm" for minutes.
func DurationShort(d time.Duration) string {
	hours := int(d.Hours())
	minutes := int(d.Minutes()) % 60

	if hours > 0 {
		return itoa(hours) + "h" + itoa(minutes) + "m"
	}
	return itoa(minutes) + "m"
}

// DurationMS formats a millisecond count as a human-readable duration.
// Returns "<1s" for values under 1000ms.
func DurationMS(ms int) string {
	if ms < 1000 {
		return "<1s"
	}
	return Duration(time.Duration(ms) * time.Millisecond)
}

// Minutes formats a minute count as a human-readable duration.
func Minutes(m int) string {
	return Duration(time.Duration(m) * time.Minute)
}

// ────────────────────────────────────────────────────────────────
// Path Formatting
// ────────────────────────────────────────────────────────────────

// ShortenPath reduces a path for display by replacing the home directory
// with ~ and truncating long paths to show only the last two components.
func ShortenPath(path string) string {
	if path == "" {
		return ""
	}

	// Replace home with ~
	home, _ := os.UserHomeDir()
	if strings.HasPrefix(path, home) {
		path = "~" + path[len(home):]
	}

	// If still too long, truncate with ...
	if len(path) > 50 {
		parts := strings.Split(path, "/")
		if len(parts) > 3 {
			return ".../" + strings.Join(parts[len(parts)-2:], "/")
		}
	}

	return path
}

// BaseName returns the last component of a slash-separated path.
func BaseName(path string) string {
	parts := strings.Split(path, "/")
	if len(parts) > 0 {
		return parts[len(parts)-1]
	}
	return path
}

// ────────────────────────────────────────────────────────────────
// Number Formatting
// ────────────────────────────────────────────────────────────────

// Itoa converts an integer to its string representation.
// Exported wrapper around internal itoa to avoid strconv dependency.
func Itoa(i int) string {
	return itoa(i)
}

// Percent formats a value/total ratio as a percentage string.
// Returns "0%" if total is zero.
func Percent(value, total float64) string {
	if total == 0 {
		return "0%"
	}
	pct := (value / total) * 100
	return itoa(int(pct)) + "%"
}

// ────────────────────────────────────────────────────────────────
// String Formatting
// ────────────────────────────────────────────────────────────────

// Truncate shortens a string to max length, appending "..." if truncated.
// If max <= 3, truncates without ellipsis.
func Truncate(s string, max int) string {
	if len(s) <= max {
		return s
	}
	if max <= 3 {
		return s[:max]
	}
	return s[:max-3] + "..."
}

// PadRight pads a string with trailing spaces to reach the specified width.
// Returns the string unchanged if already at or beyond width.
func PadRight(s string, width int) string {
	if len(s) >= width {
		return s
	}
	return s + strings.Repeat(" ", width-len(s))
}

// PadLeft pads a string with leading spaces to reach the specified width.
// Returns the string unchanged if already at or beyond width.
func PadLeft(s string, width int) string {
	if len(s) >= width {
		return s
	}
	return strings.Repeat(" ", width-len(s)) + s
}

// ────────────────────────────────────────────────────────────────
// Model Formatting
// ────────────────────────────────────────────────────────────────

// ShortModelName extracts a short model name for display from a full
// Claude model display name. Returns "Unknown" for empty strings.
func ShortModelName(displayName string) string {
	if displayName == "" {
		return "Unknown"
	}

	// Common mappings
	mappings := map[string]string{
		"Claude 3.5 Sonnet": "Sonnet 3.5",
		"Claude 3.5 Haiku":  "Haiku 3.5",
		"Claude 3 Opus":     "Opus 3",
		"Claude 3 Sonnet":   "Sonnet 3",
		"Claude Opus 4":     "Opus 4",
		"Claude Opus 4.5":   "Opus 4.5",
		"Claude Sonnet 4":   "Sonnet 4",
		"Sonnet":            "Sonnet",
		"Opus":              "Opus",
		"Haiku":             "Haiku",
	}

	if short, ok := mappings[displayName]; ok {
		return short
	}

	// Default: return as-is but truncate if too long
	if len(displayName) > 15 {
		return displayName[:15] + "..."
	}
	return displayName
}

// GetShortModelName is an alias for ShortModelName.
// Deprecated: use ShortModelName directly.
func GetShortModelName(displayName string) string {
	return ShortModelName(displayName)
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ────────────────────────────────────────────────────────────────
// Library Overview
// ────────────────────────────────────────────────────────────────
//
// Pure formatting utilities — take values in, return strings out.
// No side effects, no I/O (except UserHomeDir in ShortenPath),
// no shared state. Safe for concurrent use.
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   - Add new formatting functions (duration, path, number, string variants)
//   - Add new model name mappings to ShortModelName
//   - Improve truncation/padding with rune-aware versions
//
// Modify with Care:
//   - Existing function signatures — breaks callers
//   - itoa implementation — used by multiple functions
//
// Never Modify:
//   - 4-block structure
//   - init() identity registration pattern
//
// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Dependencies:  stdlib only (os, strings, time) + L0 identity
// Dependents:    statusline, session skills, CPI-SI CLI commands
// Parallel:      util/output (printing), util/cli (flag parsing)
//
// "Ponder the path of thy feet." — Proverbs 4:26
//
// ============================================================================
// END CLOSING
// ============================================================================
