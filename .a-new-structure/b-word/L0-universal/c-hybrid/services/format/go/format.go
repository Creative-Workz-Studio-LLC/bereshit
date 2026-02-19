//omni:code --go -library
//omni:key B-L0-hybrid-format
//omni:version b-02.00
//omni:pragma Pragma
//omni:metadata Metadata

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-hybrid-format
// Purpose: Shared formatting utilities for CPI-SI CLI output — duration, path,
//          number, string, and model name formatting
// Biblical: Proverbs 4:26 — Ponder the path of thy feet, and let all thy ways
//           be established
// Version: b-02.00

// Package format provides shared formatting utilities for CPI-SI CLI output.
//
// Duration, path, number, string, byte size, and model name formatting — all
// pure stdlib. No external dependencies. Used by statusline, session skills,
// and any CLI tool that needs consistent human-readable output.
//
// # Design
//
// Pure functions — take values in, return strings out. No side effects,
// no I/O (except UserHomeDir in ShortenPath), no shared state. Safe for
// concurrent use.
//
// # Usage
//
//	format.Duration(elapsed)           // "3m 42s"
//	format.ShortenPath("/long/path")   // "~/.../last/two"
//	format.Truncate(longStr, 30)       // "first 27 chars..."
//	format.Percent(75, 100)            // "75%"
//	format.ByteSize(1536)              // "1.5 KB"
//	format.Count(3, "file", "files")   // "3 files"
//
// # OmniCode Identity
//
//	Variable    Sections  Purpose                          Query
//	Pragma      I1-I4     Identity — who this file is      PragmaGet
//	Metadata    C1-C7     Context — when, where, why, how  MetadataGet
//
// See Pragma (I1-I4) and Metadata (C1-C7) for OmniCode identity.
package format

// ──────────────────────────────────────────────────────────────────────────
// Metadata Imports
// ──────────────────────────────────────────────────────────────────────────
//
// Go requires all imports before any declarations. Since Pragma/Metadata
// vars live in METADATA, ALL imports must be here — not in SETUP.
// This is a Go language constraint, not a structural choice.

import (
	"os"      // UserHomeDir for ShortenPath
	"strings" // path splitting, padding
	"time"    // Duration type
)

import (
	"creativeworkzstudio.com/bereshit/L0-universal/b-spiral/identity" // Register — package identity registry
)

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

// Pragma carries the OmniCode identity sections (I1-I4) for this package.
//
// These fields establish WHO this file is in the project graph.
// Initialized once at package load, read-only thereafter.
//
// # Sections
//
//	Section  Purpose       Required                 Defined
//	I1       Core          key, format, from, at    —
//	I2       Family        type, structure          subtype, role
//	I3       Instance      file, title              component, path, provides, brief
//	I4       Architecture  —                        layer, position, pattern
//
// # Example
//
//	key := PragmaGet("I1.key") // returns "B-L0-hybrid-format"
var Pragma = [][2]string{
	// I1: Core
	{"I1.key", "B-L0-hybrid-format"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.go"},
	{"I1.at", "b-02.00"},
	// I2: Family
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "derived"},
	// I3: Instance
	{"I3.file", "format.go"},
	{"I3.title", "Format Utilities Library"},
	{"I3.component", "Duration, path, number, string, byte size, and model name formatting"},
	{"I3.path", "L0-universal/c-hybrid/services/format/go/format.go"},
	{"I3.provides", "FORMAT_UTILITIES_LIBRARY"},
	{"I3.brief", "Pure stdlib formatting. No side effects. Concurrent-safe."},
	// I4: Architecture
	{"I4.layer", "L0"},
	{"I4.position", "hybrid/services/format"},
	{"I4.pattern", "format.Duration(d), format.ShortenPath(p), format.Truncate(s, n)"},
}

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

// Metadata carries the OmniCode context sections (C1-C7) for this package.
//
// These fields establish WHEN, WHERE, WHY, and HOW for this file.
// Initialized once at package load, read-only thereafter.
//
// # Sections
//
//	Section  Purpose         Required             Defined
//	C1       State           version, status      created, updated
//	C2       Attribution     organization         architect, implementation, copyright
//	C3       Grounding       scripture            principle, anchor
//	C4       Dependencies    requires, consumers  integration, if_missing
//	C5       Intent          —                    purpose, philosophy
//	C6       Roadmap         —                    current, planned, limitations
//	C7       Classification  —                    tags, category, domain, paradigm
//
// # Example
//
//	version := MetadataGet("C1.version") // returns "b-02.00"
var Metadata = [][2]string{
	// C1: State
	{"C1.version", "b-02.00"},
	{"C1.status", "Active"},
	{"C1.created", "2025-12-10"},
	{"C1.updated", "2026-02-19"},
	// C2: Attribution
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.architect", "Seanje Lenox-Wise"},
	{"C2.implementation", "Nova Dawn (CPI-SI)"},
	{"C2.copyright", "2025-2026 CreativeWorkzStudio LLC. All rights reserved."},
	// C3: Grounding
	{"C3.scripture", "Proverbs 4:26 — Ponder the path of thy feet, and let all thy ways be established"},
	{"C3.principle", "Clarity in presentation — format data so humans can ponder it"},
	{"C3.anchor", "Genesis 1:1 — In the beginning God created the heaven and the earth"},
	// C4: Dependencies
	{"C4.requires.stdlib", "os (home dir), strings (manipulation), time (Duration type)"},
	{"C4.requires.external", "none"},
	{"C4.requires.internal", "L0-universal/b-spiral/identity"},
	{"C4.consumers", "statusline, session skills, all CPI-SI CLI commands"},
	{"C4.integration", "import creativeworkzstudio.com/bereshit/L0-universal/c-hybrid/services/format/go/format"},
	{"C4.if_missing", "No consistent formatting across CPI-SI tools — each reimplements duration/path/string formatting"},
	// C5: Intent
	{"C5.purpose", "Consistent human-readable formatting across all CPI-SI tools"},
	{"C5.philosophy", "Pure functions, no side effects — format values, never print them"},
	// C6: Roadmap
	{"C6.current", "b-02.00 — I/C metadata, identity registration, 16 pure formatting functions"},
	{"C6.planned", "Color/ANSI formatting, locale awareness, rune-aware truncation"},
	{"C6.limitations", "No color output, no locale support, byte-not-rune truncation"},
	// C7: Classification
	{"C7.tags", "format, duration, path, number, string, byte-size, pure, stdlib-only"},
	{"C7.category", "Foundation"},
	{"C7.domain", "output-formatting"},
	{"C7.paradigm", "CPI-SI"},
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
//
// Section order (dependency chain):
//   §01 Imports   — see METADATA > Metadata Imports (Go constraint)
//   §02-§10       — not needed (pure function library)

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────
//
// [See: METADATA > Metadata Imports — Go requires all imports before
// declarations, so imports live with the identity vars they precede]
//
// --- Standard Library ---
// os (UserHomeDir), strings (manipulation), time (Duration type)
//
// --- External Packages ---
// [Pure library — no external dependencies at L0]
//
// --- Internal Packages ---
// L0-universal/b-spiral/identity (package registry)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: 1 → 2 → 3 → 5 (ascending numeric)
// Flow: org chart → helpers → core → public APIs

// ──────────────────────────────────────────────────────────────────────────
// 1. Org Chart
// ──────────────────────────────────────────────────────────────────────────
//
// Ladder Structure:
//
//   Public APIs (Top Rungs — Exported)
//   ├── Duration(d)          → uses itoa()
//   ├── DurationShort(d)     → uses itoa()
//   ├── DurationMS(ms)       → uses Duration()
//   ├── Minutes(m)           → uses Duration()
//   ├── ShortenPath(path)    → pure
//   ├── BaseName(path)       → pure
//   ├── Itoa(i)              → uses itoa()
//   ├── Percent(v, t)        → uses itoa()
//   ├── ByteSize(bytes)      → pure
//   ├── Truncate(s, max)     → pure
//   ├── PadRight(s, w)       → pure
//   ├── PadLeft(s, w)        → pure
//   ├── Count(n, sg, pl)     → uses itoa()
//   ├── ShortModelName(name) → pure
//   ├── PragmaGet(key)       → reads Pragma slice
//   └── MetadataGet(key)     → reads Metadata slice
//
//   Helpers (Bottom Rungs — Unexported)
//   └── itoa(i)              → pure integer-to-string
//
// APUs: 17 functions (1 helper, 16 public APIs including identity)

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────

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

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────

// init registers this package in the OmniCode identity registry.
func init() {
	identity.Register(PragmaGet("I1.key"), Pragma, Metadata)
}

// ──────────────────────────────────────────────────────────────────────────
// 5. Public APIs
// ──────────────────────────────────────────────────────────────────────────

// ═══ Identity ═══

// PragmaGet returns a single value from this package's Pragma by key.
// Returns empty string if key not found.
func PragmaGet(key string) string {
	for _, pair := range Pragma {
		if pair[0] == key {
			return pair[1]
		}
	}
	return ""
}

// MetadataGet returns a single value from this package's Metadata by key.
// Returns empty string if key not found.
func MetadataGet(key string) string {
	for _, pair := range Metadata {
		if pair[0] == key {
			return pair[1]
		}
	}
	return ""
}

// ═══ Duration Formatting ═══

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

// ═══ Path Formatting ═══

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

// ═══ Number Formatting ═══

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

// ByteSize formats a byte count as a human-readable size string.
// Uses binary units: KB (1024), MB (1024^2), GB (1024^3).
// Returns "0 B" for zero, "<1 KB" for 1-1023 bytes.
func ByteSize(bytes uint64) string {
	if bytes == 0 {
		return "0 B"
	}
	if bytes < 1024 {
		return "<1 KB"
	}

	const (
		kb = 1024
		mb = 1024 * 1024
		gb = 1024 * 1024 * 1024
	)

	if bytes >= gb {
		whole := bytes / gb
		frac := (bytes % gb) * 10 / gb
		if frac > 0 {
			return itoa(int(whole)) + "." + itoa(int(frac)) + " GB"
		}
		return itoa(int(whole)) + " GB"
	}
	if bytes >= mb {
		whole := bytes / mb
		frac := (bytes % mb) * 10 / mb
		if frac > 0 {
			return itoa(int(whole)) + "." + itoa(int(frac)) + " MB"
		}
		return itoa(int(whole)) + " MB"
	}
	// KB
	whole := bytes / kb
	frac := (bytes % kb) * 10 / kb
	if frac > 0 {
		return itoa(int(whole)) + "." + itoa(int(frac)) + " KB"
	}
	return itoa(int(whole)) + " KB"
}

// ═══ String Formatting ═══

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

// Count formats a number with the appropriate singular or plural noun.
// Example: Count(1, "file", "files") returns "1 file".
func Count(n int, singular, plural string) string {
	if n == 1 {
		return "1 " + singular
	}
	return itoa(n) + " " + plural
}

// ═══ Model Formatting ═══

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
		"Claude Opus 4.6":   "Opus 4.6",
		"Claude Sonnet 4":   "Sonnet 4",
		"Claude Sonnet 4.5": "Sonnet 4.5",
		"Claude Sonnet 4.6": "Sonnet 4.6",
		"Claude Haiku 4.5":  "Haiku 4.5",
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
// ──────────────────────────────────────────────────────────────────────────
// CLOSING Zones Overview
// ──────────────────────────────────────────────────────────────────────────
//
// 3 Code Zones (operations):  Cv → Ce → Cc
// 5 Doc Sections (guidance):  X1 → X2 → X3 → X4 → X5

// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────
//
// Testing:
//   - go test -v ./L0-universal/c-hybrid/services/format/go/...
//   - 35+ tests covering all formatting functions and identity
//   - Pure function tests — deterministic, no I/O (except ShortenPath)
//
// Build Verification:
//   - go build ./L0-universal/c-hybrid/services/format/go/...
//   - go vet ./L0-universal/c-hybrid/services/format/go/...

// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library — imported, not executed. Registration via identity.Register()
// in init(). No main(), no CLI.

// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// Pure formatting — no resources to clean up. No connections, no files,
// no goroutines. Garbage collected when process exits.

// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Remove not the ancient landmark, which thy fathers have set."
// — Proverbs 22:28
//
// Safe:
//   - Add new formatting functions (follow pure function pattern)
//   - Add new model name mappings to ShortModelName
//   - Improve truncation/padding with rune-aware versions
//
// Careful:
//   - Existing function signatures — breaks callers
//   - itoa implementation — used by multiple functions
//   - Duration output format — downstream may parse it
//
// Never:
//   - 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   - init() identity registration pattern
//   - [][2]string identity format (I1-I4, C1-C7)

// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// Note: Pure formatting utilities — take values in, return strings out.
// No side effects (except UserHomeDir in ShortenPath), no shared state.
// Safe for concurrent use. The clarity of output honors the clarity of truth.
//
// Scripture: "Ponder the path of thy feet, and let all thy ways be
// established." — Proverbs 4:26

// ============================================================================
// END CLOSING
// ============================================================================
