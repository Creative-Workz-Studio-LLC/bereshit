//omni:code --go -library
//omni:key B-L0-hybrid-output
//omni:version b-01.01
//omni:pragma _pragma
//omni:metadata _metadata

// ============================================================================
// METADATA
// ============================================================================

// Package output provides shared output formatting utilities for CPI-SI CLIs.
//
// Headers, sections, key-value pairs, tables, status indicators, and JSON
// helpers — all writing to stdout via fmt. Pure stdlib, no external deps.
//
// Usage:
//
//	output.Header("System Status")
//	output.KeyValue("Uptime", "3h 42m")
//	output.Success("All checks passed")
//	output.TableHeader("Name", "Status", "Score")
//
// See _pragma and _metadata for OmniCode identity.
package output

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

//--- I.1 Standard Library [IMPORT] (1) ---
// Formatted output and string utilities
import (
	"fmt"     // Printf, Println
	"strings" // Repeat, Join
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
	"P1.key":    "B-L0-hybrid-output",
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
	"P5.title":   "Output Formatting Library",
	"P5.summary": "Shared output formatting for CPI-SI CLI commands",
}

// ────────────────────────────────────────────────────────────────
// Metadata
// ────────────────────────────────────────────────────────────────

// _metadata — OmniCode file metadata (M1-M10). Runtime-queryable.
var _metadata = map[string]string{

	// ── M1: Core Identity ─────────────────────────────────────────
	"M1.key":            "B-L0-hybrid-output",
	"M1.component_type": "Foundation",
	"M1.architect":      "Seanje Lenox-Wise",
	"M1.implementation": "Nova Dawn",
	"M1.created":        "2025-12-10",

	// ── M2: Version History ───────────────────────────────────────
	"M2.b-01.01": "2026-02-14 — REORG: promoted from hybrid/util/ to hybrid/ top-level domain",
	"M2.b-01.00": "2026-02-14 — BASELINE: 4-block, identity registration, pragma/metadata",
	"M2.a-01.00": "2025-12-10 — Initial creation",

	// ── M3: Interface ─────────────────────────────────────────────
	"M3.requires.stdlib":   "fmt, strings",
	"M3.requires.external": "none",
	"M3.requires.internal": "L0-universal/spiral/identity",
	"M3.used_by":           "all CPI-SI skill CLI commands for formatted terminal output",
	"M3.import":            "creativeworkzstudio.com/bereshit/L0-universal/hybrid/output",
	"M3.pattern":           "output.Header(title), output.KeyValue(k, v), output.Success(msg)",

	// ── M4: Public API ────────────────────────────────────────────
	"M4.headers":    "Header, Section, SubSection, Divider",
	"M4.formatted":  "KeyValue, KeyValueWidth, Bullet, Numbered, Indent",
	"M4.tables":     "TableHeader, TableRow",
	"M4.status":     "Success, Info, Tip",
	"M4.json":       "JSONStart, JSONEnd, JSONField, JSONFieldLast, JSONObjectStart, JSONObjectEnd, JSONObjectEndLast",
	"M4.self":       "Pragma, Metadata",

	// ── M5: Operational ───────────────────────────────────────────
	"M5.blocking": "no — stdout writes only",
	"M5.health":   "Foundation | granted: printed | deferred: n/a | denied: n/a",

	// ── M6: Classification ────────────────────────────────────────
	"M6.tags":     "output, headers, tables, json, status, pure, stdlib-only",
	"M6.category": "Foundation",
	"M6.domain":   "output-formatting",
	"M6.layer":    "L0-universal",
	"M6.paradigm": "CPI-SI",

	// ── M7: Intent ────────────────────────────────────────────────
	"M7.purpose":    "Consistent terminal output across all CPI-SI tools",
	"M7.philosophy": "Print functions handle layout — callers handle logic",
	"M7.provides":   "Headers, key-value pairs, tables, status indicators, JSON output",

	// ── M8: Grounding ─────────────────────────────────────────────
	"M8.scripture": "Colossians 4:6 — Let your speech be alway with grace",
	"M8.principle": "Graceful output — structured, clear, readable by humans",
	"M8.anchor":    "Genesis 1:1 — In the beginning God created the heaven and the earth",

	// ── M9: Dependencies ──────────────────────────────────────────
	"M9.needs.stdlib":   "fmt (printing), strings (Repeat, Join)",
	"M9.needs.external": "none",
	"M9.needs.internal": "L0-universal/spiral/identity",
	"M9.used_by":        "all CPI-SI skill CLI commands",
	"M9.layer_deps":     "L0 only — no L1+ dependencies",

	// ── M10: Roadmap ──────────────────────────────────────────────
	"M10.current":     "b-01.00 — BASELINE: standard output formatting with identity",
	"M10.planned":     "Color/ANSI support, progress bars, spinner output",
	"M10.limitations": "No color, no progress indicators, stdout only",
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// No types, constants, or package-level variables beyond pragma/metadata.
// All functions write directly to stdout.

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: Identity → Headers → Formatted Output → Tables → Status → JSON

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
// Headers and Sections
// ────────────────────────────────────────────────────────────────

// Header prints a title with a double-line separator beneath it.
// Minimum separator width is 40 characters.
func Header(title string) {
	width := max(len(title), 40)
	fmt.Println(title)
	fmt.Println(strings.Repeat("=", width))
	fmt.Println()
}

// Section prints a section title with a single-line separator beneath it.
func Section(title string) {
	fmt.Println(title)
	fmt.Println(strings.Repeat("-", len(title)))
	fmt.Println()
}

// SubSection prints a subsection title with ## prefix.
func SubSection(title string) {
	fmt.Printf("## %s\n\n", title)
}

// Divider prints a line of the specified character repeated to width.
func Divider(char string, width int) {
	fmt.Println(strings.Repeat(char, width))
}

// ────────────────────────────────────────────────────────────────
// Formatted Output
// ────────────────────────────────────────────────────────────────

// KeyValue prints a key-value pair with the key left-aligned to 12 characters.
func KeyValue(key string, value any) {
	fmt.Printf("%-12s %v\n", key+":", value)
}

// KeyValueWidth prints a key-value pair with a custom key column width.
func KeyValueWidth(key string, value any, width int) {
	format := fmt.Sprintf("%%-%ds %%v\n", width)
	fmt.Printf(format, key+":", value)
}

// Bullet prints a bulleted list item with "  - " prefix.
func Bullet(text string) {
	fmt.Printf("  - %s\n", text)
}

// Numbered prints a numbered list item with "  N. " prefix.
func Numbered(n int, text string) {
	fmt.Printf("  %d. %s\n", n, text)
}

// Indent prints text indented by the given level (2 spaces per level).
func Indent(text string, level int) {
	prefix := strings.Repeat("  ", level)
	fmt.Printf("%s%s\n", prefix, text)
}

// ────────────────────────────────────────────────────────────────
// Tables
// ────────────────────────────────────────────────────────────────

// TableHeader prints a table header row with auto-sized columns
// followed by a dashed separator line.
func TableHeader(columns ...string) {
	var widths []int
	for _, col := range columns {
		widths = append(widths, len(col)+2)
	}
	TableRow(widths, columns...)
	var dashes []string
	for _, w := range widths {
		dashes = append(dashes, strings.Repeat("-", w))
	}
	fmt.Println(strings.Join(dashes, " "))
}

// TableRow prints a table row with values aligned to specified column widths.
// Columns without a corresponding width default to 10 characters.
func TableRow(widths []int, values ...string) {
	var parts []string
	for i, v := range values {
		width := 10
		if i < len(widths) {
			width = widths[i]
		}
		format := fmt.Sprintf("%%-%ds", width)
		parts = append(parts, fmt.Sprintf(format, v))
	}
	fmt.Println(strings.Join(parts, " "))
}

// ────────────────────────────────────────────────────────────────
// Status Indicators
// ────────────────────────────────────────────────────────────────

// Success prints a success message prefixed with "OK ".
func Success(format string, args ...any) {
	fmt.Printf("OK "+format+"\n", args...)
}

// Info prints an informational message prefixed with "INFO ".
func Info(format string, args ...any) {
	fmt.Printf("INFO "+format+"\n", args...)
}

// Tip prints a guidance message prefixed with "TIP ".
func Tip(format string, args ...any) {
	fmt.Printf("TIP "+format+"\n", args...)
}

// ────────────────────────────────────────────────────────────────
// JSON Output Helpers
// ────────────────────────────────────────────────────────────────

// JSONStart prints the opening brace of a JSON object.
func JSONStart() {
	fmt.Println("{")
}

// JSONEnd prints the closing brace of a JSON object.
func JSONEnd() {
	fmt.Println("}")
}

// JSONField prints a JSON field with a trailing comma.
// Handles string, bool, and numeric types with appropriate formatting.
func JSONField(key string, value any) {
	switch v := value.(type) {
	case string:
		fmt.Printf("  %q: %q,\n", key, v)
	case bool:
		fmt.Printf("  %q: %t,\n", key, v)
	case int, int64, float64:
		fmt.Printf("  %q: %v,\n", key, v)
	default:
		fmt.Printf("  %q: %v,\n", key, v)
	}
}

// JSONFieldLast prints a JSON field without a trailing comma.
// Use for the last field in a JSON object.
func JSONFieldLast(key string, value any) {
	switch v := value.(type) {
	case string:
		fmt.Printf("  %q: %q\n", key, v)
	case bool:
		fmt.Printf("  %q: %t\n", key, v)
	case int, int64, float64:
		fmt.Printf("  %q: %v\n", key, v)
	default:
		fmt.Printf("  %q: %v\n", key, v)
	}
}

// JSONObjectStart prints the opening of a nested JSON object.
func JSONObjectStart(key string) {
	fmt.Printf("  %q: {\n", key)
}

// JSONObjectEnd prints the closing of a nested JSON object with trailing comma.
func JSONObjectEnd() {
	fmt.Println("  },")
}

// JSONObjectEndLast prints the closing of a nested JSON object without trailing comma.
func JSONObjectEndLast() {
	fmt.Println("  }")
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
// Terminal output formatting for CPI-SI tools. Every function writes
// to stdout via fmt — callers handle logic, this package handles layout.
// Pure stdlib. Safe for concurrent use (fmt is thread-safe).
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   - Add new output functions (progress, color, spinner variants)
//   - Add new JSON helper variants
//   - Add new table formatting options
//
// Modify with Care:
//   - Existing function signatures — breaks callers
//   - Output format strings — callers may parse output
//
// Never Modify:
//   - 4-block structure
//   - init() identity registration pattern
//
// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Dependencies:  stdlib only (fmt, strings) + L0 identity
// Dependents:    all CPI-SI skill CLI commands
// Parallel:      util/format (value formatting), util/cli (flag parsing)
//
// "Let your speech be alway with grace." — Colossians 4:6
//
// ============================================================================
// END CLOSING
// ============================================================================
