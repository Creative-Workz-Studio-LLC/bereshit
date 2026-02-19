//omni:code --go -library
//omni:key B-L0-hybrid-output
//omni:version b-02.00
//omni:pragma Pragma
//omni:metadata Metadata

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-hybrid-output
// Purpose: Terminal output formatting for CPI-SI tools — headers, sections,
//          key-value pairs, tables, status indicators, and JSON helpers
// Biblical: Colossians 4:6 — Let your speech be alway with grace, seasoned
//           with salt, that ye may know how ye ought to answer every man
// Version: b-02.00

// Package output provides terminal output formatting for CPI-SI CLI tools.
//
// Headers, sections, key-value pairs, tables, status indicators, and JSON
// helpers — all writing through io.Writer for testability. Pure stdlib.
//
// # Design
//
// Writer-based testability. Every output function has two layers:
//
//  1. A private write* function that takes io.Writer and does the real work
//  2. A public function that delegates to os.Stdout
//
// Tests exercise the write functions directly against bytes.Buffer,
// verifying exact output without touching stdout.
//
// # Usage
//
//	output.Header("System Status")
//	output.KeyValue("Uptime", "3h 42m")
//	output.Success("All checks passed")
//	output.TableHeader("Name", "Status", "Score")
//
// # OmniCode Identity
//
//	Variable    Sections  Purpose                          Query
//	Pragma      I1-I4     Identity — who this file is      PragmaGet
//	Metadata    C1-C7     Context — when, where, why, how  MetadataGet
//
// See Pragma (I1-I4) and Metadata (C1-C7) for OmniCode identity.
package output

// ──────────────────────────────────────────────────────────────────────────
// Metadata Imports
// ──────────────────────────────────────────────────────────────────────────
//
// Go requires all imports before any declarations. Since Pragma/Metadata
// vars live in METADATA, ALL imports must be here — not in SETUP.

import (
	"fmt"     // Fprintf, Fprintln
	"io"      // Writer interface
	"os"      // Stdout
	"strings" // Repeat, Join
)

import (
	"creativeworkzstudio.com/bereshit/L0-universal/b-spiral/identity" // Register — package identity registry
)

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

// Pragma carries the OmniCode identity sections (I1-I4) for this package.
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
//	key := PragmaGet("I1.key") // returns "B-L0-hybrid-output"
var Pragma = [][2]string{
	// I1: Core
	{"I1.key", "B-L0-hybrid-output"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/library.go"},
	{"I1.at", "b-02.00"},
	// I2: Family
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "library"},
	{"I2.role", "derived"},
	// I3: Instance
	{"I3.file", "output.go"},
	{"I3.title", "Output Formatting Library"},
	{"I3.component", "Headers, sections, key-value, tables, status, JSON output"},
	{"I3.path", "L0-universal/c-hybrid/services/output/go/output.go"},
	{"I3.provides", "OUTPUT_FORMATTING_LIBRARY"},
	{"I3.brief", "Writer-based terminal output. Testable. Pure stdlib."},
	// I4: Architecture
	{"I4.layer", "L0"},
	{"I4.position", "hybrid/services/output"},
	{"I4.pattern", "output.Header(title), output.KeyValue(k, v), output.Success(msg)"},
}

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

// Metadata carries the OmniCode context sections (C1-C7) for this package.
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
	{"C3.scripture", "Colossians 4:6 — Let your speech be alway with grace, seasoned with salt"},
	{"C3.principle", "Graceful output — structured, clear, readable by humans"},
	{"C3.anchor", "Genesis 1:1 — In the beginning God created the heaven and the earth"},
	// C4: Dependencies
	{"C4.requires.stdlib", "fmt (formatting), io (Writer), os (Stdout), strings (Repeat, Join)"},
	{"C4.requires.external", "none"},
	{"C4.requires.internal", "L0-universal/b-spiral/identity"},
	{"C4.consumers", "all CPI-SI skill CLI commands for formatted terminal output"},
	{"C4.integration", "import creativeworkzstudio.com/bereshit/L0-universal/c-hybrid/services/output/go/output"},
	{"C4.if_missing", "No consistent terminal output — each CLI tool reimplements headers, tables, status"},
	// C5: Intent
	{"C5.purpose", "Consistent terminal output across all CPI-SI tools"},
	{"C5.philosophy", "Print functions handle layout — callers handle logic. Writer-based for testability."},
	// C6: Roadmap
	{"C6.current", "b-02.00 — I/C metadata, writer-based testability, 22 output functions"},
	{"C6.planned", "Color/ANSI support, progress bars, spinner output"},
	{"C6.limitations", "No color, no progress indicators, stdout only by default"},
	// C7: Classification
	{"C7.tags", "output, headers, tables, json, status, writer, testable, stdlib-only"},
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
// All functions write through io.Writer.
//
// Section order (dependency chain):
//   §01 Imports   — see METADATA > Metadata Imports (Go constraint)
//   §02-§10       — not needed (pure output library)

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────
//
// [See: METADATA > Metadata Imports — Go requires all imports before
// declarations, so imports live with the identity vars they precede]
//
// --- Standard Library ---
// fmt (formatting), io (Writer), os (Stdout), strings (Repeat, Join)
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
//
// Pattern: Private write*(w io.Writer, ...) does the work.
//          Public Func(...) calls write*(os.Stdout, ...).
//          Tests call write* against bytes.Buffer.

// ──────────────────────────────────────────────────────────────────────────
// 1. Org Chart
// ──────────────────────────────────────────────────────────────────────────
//
// Ladder Structure:
//
//   Public APIs (Exported — delegates to write* via os.Stdout)
//   ├── Headers:   Header, Section, SubSection, Divider
//   ├── Formatted: KeyValue, KeyValueWidth, Bullet, Numbered, Indent, BlankLine
//   ├── Tables:    TableHeader, TableRow
//   ├── Status:    Success, Info, Tip, Warning, Error
//   ├── JSON:      JSONStart, JSONEnd, JSONField, JSONFieldLast,
//   │              JSONObjectStart, JSONObjectEnd, JSONObjectEndLast
//   └── Identity:  PragmaGet, MetadataGet
//
//   Helpers (Private — write to any io.Writer)
//   ├── writeHeader, writeSection, writeSubSection, writeDivider
//   ├── writeKeyValue, writeKeyValueWidth, writeBullet, writeNumbered,
//   │   writeIndent, writeBlankLine
//   ├── writeTableHeader, writeTableRow
//   ├── writeSuccess, writeInfo, writeTip, writeWarning, writeError
//   └── writeJSONField, writeJSONObjectStart
//
// APUs: 41 functions (19 write helpers, 22 public APIs including identity)

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────
//
// Private write functions that take io.Writer for testability.
// Each corresponds to a public API function.

// --- Headers ---

func writeHeader(w io.Writer, title string) {
	width := max(len(title), 40)
	fmt.Fprintln(w, title)
	fmt.Fprintln(w, strings.Repeat("=", width))
	fmt.Fprintln(w)
}

func writeSection(w io.Writer, title string) {
	fmt.Fprintln(w, title)
	fmt.Fprintln(w, strings.Repeat("-", len(title)))
	fmt.Fprintln(w)
}

func writeSubSection(w io.Writer, title string) {
	fmt.Fprintf(w, "## %s\n\n", title)
}

func writeDivider(w io.Writer, char string, width int) {
	fmt.Fprintln(w, strings.Repeat(char, width))
}

// --- Formatted Output ---

func writeKeyValue(w io.Writer, key string, value any) {
	fmt.Fprintf(w, "%-12s %v\n", key+":", value)
}

func writeKeyValueWidth(w io.Writer, key string, value any, width int) {
	format := fmt.Sprintf("%%-%ds %%v\n", width)
	fmt.Fprintf(w, format, key+":", value)
}

func writeBullet(w io.Writer, text string) {
	fmt.Fprintf(w, "  - %s\n", text)
}

func writeNumbered(w io.Writer, n int, text string) {
	fmt.Fprintf(w, "  %d. %s\n", n, text)
}

func writeIndent(w io.Writer, text string, level int) {
	prefix := strings.Repeat("  ", level)
	fmt.Fprintf(w, "%s%s\n", prefix, text)
}

func writeBlankLine(w io.Writer) {
	fmt.Fprintln(w)
}

// --- Tables ---

func writeTableHeader(w io.Writer, columns ...string) {
	var widths []int
	for _, col := range columns {
		widths = append(widths, len(col)+2)
	}
	writeTableRow(w, widths, columns...)
	var dashes []string
	for _, wd := range widths {
		dashes = append(dashes, strings.Repeat("-", wd))
	}
	fmt.Fprintln(w, strings.Join(dashes, " "))
}

func writeTableRow(w io.Writer, widths []int, values ...string) {
	var parts []string
	for i, v := range values {
		width := 10
		if i < len(widths) {
			width = widths[i]
		}
		format := fmt.Sprintf("%%-%ds", width)
		parts = append(parts, fmt.Sprintf(format, v))
	}
	fmt.Fprintln(w, strings.Join(parts, " "))
}

// --- Status ---

func writeSuccess(w io.Writer, msg string) {
	fmt.Fprintf(w, "OK %s\n", msg)
}

func writeInfo(w io.Writer, msg string) {
	fmt.Fprintf(w, "INFO %s\n", msg)
}

func writeTip(w io.Writer, msg string) {
	fmt.Fprintf(w, "TIP %s\n", msg)
}

func writeWarning(w io.Writer, msg string) {
	fmt.Fprintf(w, "WARN %s\n", msg)
}

func writeError(w io.Writer, msg string) {
	fmt.Fprintf(w, "ERR %s\n", msg)
}

// --- JSON ---

func writeJSONField(w io.Writer, key string, value any, last bool) {
	comma := ","
	if last {
		comma = ""
	}
	switch v := value.(type) {
	case string:
		fmt.Fprintf(w, "  %q: %q%s\n", key, v, comma)
	case bool:
		fmt.Fprintf(w, "  %q: %t%s\n", key, v, comma)
	case int, int64, float64:
		fmt.Fprintf(w, "  %q: %v%s\n", key, v, comma)
	default:
		fmt.Fprintf(w, "  %q: %v%s\n", key, v, comma)
	}
}

func writeJSONObjectStart(w io.Writer, key string) {
	fmt.Fprintf(w, "  %q: {\n", key)
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

// ═══ Headers and Sections ═══

// Header prints a title with a double-line separator beneath it.
// Minimum separator width is 40 characters.
func Header(title string) {
	writeHeader(os.Stdout, title)
}

// Section prints a section title with a single-line separator beneath it.
func Section(title string) {
	writeSection(os.Stdout, title)
}

// SubSection prints a subsection title with ## prefix.
func SubSection(title string) {
	writeSubSection(os.Stdout, title)
}

// Divider prints a line of the specified character repeated to width.
func Divider(char string, width int) {
	writeDivider(os.Stdout, char, width)
}

// ═══ Formatted Output ═══

// KeyValue prints a key-value pair with the key left-aligned to 12 characters.
func KeyValue(key string, value any) {
	writeKeyValue(os.Stdout, key, value)
}

// KeyValueWidth prints a key-value pair with a custom key column width.
func KeyValueWidth(key string, value any, width int) {
	writeKeyValueWidth(os.Stdout, key, value, width)
}

// Bullet prints a bulleted list item with "  - " prefix.
func Bullet(text string) {
	writeBullet(os.Stdout, text)
}

// Numbered prints a numbered list item with "  N. " prefix.
func Numbered(n int, text string) {
	writeNumbered(os.Stdout, n, text)
}

// Indent prints text indented by the given level (2 spaces per level).
func Indent(text string, level int) {
	writeIndent(os.Stdout, text, level)
}

// BlankLine prints an empty line.
func BlankLine() {
	writeBlankLine(os.Stdout)
}

// ═══ Tables ═══

// TableHeader prints a table header row with auto-sized columns
// followed by a dashed separator line.
func TableHeader(columns ...string) {
	writeTableHeader(os.Stdout, columns...)
}

// TableRow prints a table row with values aligned to specified column widths.
// Columns without a corresponding width default to 10 characters.
func TableRow(widths []int, values ...string) {
	writeTableRow(os.Stdout, widths, values...)
}

// ═══ Status Indicators ═══

// Success prints a success message prefixed with "OK ".
func Success(msg string) {
	writeSuccess(os.Stdout, msg)
}

// Info prints an informational message prefixed with "INFO ".
func Info(msg string) {
	writeInfo(os.Stdout, msg)
}

// Tip prints a guidance message prefixed with "TIP ".
func Tip(msg string) {
	writeTip(os.Stdout, msg)
}

// Warning prints a warning message prefixed with "WARN ".
func Warning(msg string) {
	writeWarning(os.Stdout, msg)
}

// Error prints an error message prefixed with "ERR ".
func Error(msg string) {
	writeError(os.Stdout, msg)
}

// ═══ JSON Output Helpers ═══

// JSONStart prints the opening brace of a JSON object.
func JSONStart() {
	fmt.Fprintln(os.Stdout, "{")
}

// JSONEnd prints the closing brace of a JSON object.
func JSONEnd() {
	fmt.Fprintln(os.Stdout, "}")
}

// JSONField prints a JSON field with a trailing comma.
// Handles string, bool, and numeric types with appropriate formatting.
func JSONField(key string, value any) {
	writeJSONField(os.Stdout, key, value, false)
}

// JSONFieldLast prints a JSON field without a trailing comma.
// Use for the last field in a JSON object.
func JSONFieldLast(key string, value any) {
	writeJSONField(os.Stdout, key, value, true)
}

// JSONObjectStart prints the opening of a nested JSON object.
func JSONObjectStart(key string) {
	writeJSONObjectStart(os.Stdout, key)
}

// JSONObjectEnd prints the closing of a nested JSON object with trailing comma.
func JSONObjectEnd() {
	fmt.Fprintln(os.Stdout, "  },")
}

// JSONObjectEndLast prints the closing of a nested JSON object without trailing comma.
func JSONObjectEndLast() {
	fmt.Fprintln(os.Stdout, "  }")
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
//   - go test -v ./L0-universal/c-hybrid/services/output/go/...
//   - 50+ tests covering all output functions via bytes.Buffer
//   - Writer-based tests — deterministic, no stdout dependency
//
// Build Verification:
//   - go build ./L0-universal/c-hybrid/services/output/go/...
//   - go vet ./L0-universal/c-hybrid/services/output/go/...

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
// Terminal output only — no resources to clean up. No connections, no files,
// no goroutines. Garbage collected when process exits.

// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Remove not the ancient landmark, which thy fathers have set."
// — Proverbs 22:28
//
// Safe:
//   - Add new output functions (progress, color, spinner variants)
//   - Add new JSON helper variants
//   - Add new table formatting options
//
// Careful:
//   - Existing function signatures — breaks callers
//   - Output format strings — callers may parse output
//   - write* helper signatures — test suite depends on them
//
// Never:
//   - 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   - Writer-based testability pattern (write* + public wrapper)
//   - init() identity registration pattern
//   - [][2]string identity format (I1-I4, C1-C7)

// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// Note: Terminal output formatting for CPI-SI tools. Every function writes
// through io.Writer — callers handle logic, this package handles layout.
// Tests verify exact output via bytes.Buffer without touching stdout.
//
// Scripture: "Let your speech be alway with grace, seasoned with salt."
// — Colossians 4:6

// ============================================================================
// END CLOSING
// ============================================================================
