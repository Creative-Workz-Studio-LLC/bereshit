//omni:code --go -demo-test
//omni:key B-L0-hybrid-format-test
//omni:version b-02.00
//omni:pragma TestFilePragma
//omni:metadata TestFileMetadata

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-hybrid-format-test
// Purpose: Comprehensive tests for the format package — 48 tests covering
//          identity, duration, path, number, byte size, string, and model formatting
// Biblical: 1 Thessalonians 5:21 — Prove all things; hold fast that which is good
// Version: b-02.00

// Package format tests verify all formatting functions produce correct output.
//
// Same-package tests (white-box) — access to unexported identifiers.
// 48 tests organized by domain: identity, duration, path, number,
// byte size, string formatting, and model name shortening.
//
// # OmniCode Identity
//
//	Variable          Sections  Purpose                          Query
//	TestFilePragma    I1-I4     Identity — who this file is      TestFilePragmaGet
//	TestFileMetadata  C1-C7     Context — when, where, why, how  TestFileMetadataGet
//
// Note: Same-package test — Pragma/Metadata names differ from source file
// to avoid redeclaration. Source file owns Pragma/Metadata; tests own
// TestFilePragma/TestFileMetadata.
package format

// ──────────────────────────────────────────────────────────────────────────
// Metadata Imports
// ──────────────────────────────────────────────────────────────────────────

import (
	"testing"
	"time"
)

// ──────────────────────────────────────────────────────────────────────────
// Identity (I1-I4)
// ──────────────────────────────────────────────────────────────────────────

// TestFilePragma carries the OmniCode identity for this test file.
// Named TestFilePragma (not Pragma) because same-package tests share
// the namespace — format.go already declares Pragma.
//
// # Example
//
//	key := TestFilePragmaGet("I1.key") // returns "B-L0-hybrid-format-test"
var TestFilePragma = [][2]string{
	// I1: Core
	{"I1.key", "B-L0-hybrid-format-test"},
	{"I1.format", "go"},
	{"I1.from", "b-word/seed/code/L0/go/demo-test.go"},
	{"I1.at", "b-02.00"},
	// I2: Family
	{"I2.type", "code"},
	{"I2.structure", "4-block"},
	{"I2.subtype", "demo-test"},
	{"I2.role", "derived"},
	// I3: Instance
	{"I3.file", "format_test.go"},
	{"I3.title", "Format Package Tests"},
	{"I3.component", "48 tests: identity, duration, path, number, byte size, string, model"},
	{"I3.path", "L0-universal/c-hybrid/services/format/go/format_test.go"},
	{"I3.provides", "FORMAT_PACKAGE_TESTS"},
	{"I3.brief", "Comprehensive format function tests. Same-package white-box."},
	// I4: Architecture
	{"I4.layer", "L0"},
	{"I4.position", "hybrid/services/format"},
	{"I4.pattern", "go test -v ./L0-universal/c-hybrid/services/format/go/..."},
}

// ──────────────────────────────────────────────────────────────────────────
// Context (C1-C7)
// ──────────────────────────────────────────────────────────────────────────

// TestFileMetadata carries the OmniCode context for this test file.
//
// # Example
//
//	version := TestFileMetadataGet("C1.version") // returns "b-02.00"
var TestFileMetadata = [][2]string{
	// C1: State
	{"C1.version", "b-02.00"},
	{"C1.status", "Active"},
	{"C1.created", "2026-02-19"},
	{"C1.updated", "2026-02-19"},
	// C2: Attribution
	{"C2.organization", "CreativeWorkzStudio LLC"},
	{"C2.architect", "Nova Dawn (CPI-SI)"},
	{"C2.implementation", "Nova Dawn (CPI-SI)"},
	{"C2.copyright", "2025-2026 CreativeWorkzStudio LLC. All rights reserved."},
	// C3: Grounding
	{"C3.scripture", "1 Thessalonians 5:21 — Prove all things; hold fast that which is good"},
	{"C3.principle", "Testing proves correctness — hold fast to what works"},
	{"C3.anchor", "Genesis 1:1 — In the beginning God created the heaven and the earth"},
	// C4: Dependencies
	{"C4.requires.stdlib", "testing, time"},
	{"C4.requires.external", "none"},
	{"C4.requires.internal", "format (package under test, same-package access)"},
	{"C4.consumers", "go test runner, CI pipeline"},
	{"C4.integration", "go test -v ./L0-universal/c-hybrid/services/format/go/..."},
	{"C4.if_missing", "No verification of format functions — silent regressions in output formatting"},
	// C5: Intent
	{"C5.purpose", "Verify all format functions produce correct output across edge cases"},
	{"C5.philosophy", "Each function tested at boundaries — zero, normal, edge. No mocks needed."},
	// C6: Roadmap
	{"C6.current", "b-02.00 — 48 tests covering all 18 public functions"},
	{"C6.planned", "Fuzz tests for string formatting, locale-aware number tests"},
	// C7: Classification
	{"C7.tags", "test, format, duration, path, number, string, model, white-box"},
	{"C7.category", "Foundation"},
	{"C7.domain", "format-testing"},
	{"C7.paradigm", "CPI-SI"},
}

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order (dependency chain):
//   §01 Imports — see METADATA > Metadata Imports (Go constraint)
//   §02-§10    — not needed (pure test file, no fixtures)

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────
//
// [See: METADATA > Metadata Imports — Go requires all imports before
// declarations, so imports live with the identity vars they precede]
//
// --- Standard Library ---
// testing (test framework), time (duration test inputs)
//
// --- Package Under Test ---
// format (same-package — white-box access to unexported identifiers)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: 1 → 5 (ascending numeric)
// Flow: org chart → test functions
//
// Sections 2-4 reserved — no fixtures, no shared infrastructure, no
// custom error handling needed. Standard testing.T assertions throughout.

// ──────────────────────────────────────────────────────────────────────────
// 1. Org Chart
// ──────────────────────────────────────────────────────────────────────────
//
// Test functions organized by domain (48 total):
//
//   Identity (8)      — PragmaGet, MetadataGet, missing keys, slice checks
//   Duration (9)      — hours, minutes, seconds, zero, short, ms, Minutes()
//   Path (5)          — ShortenPath empty/short, BaseName simple/no-slash/trailing
//   Number (6)        — Itoa +/0/-, Percent normal/zero-total/full
//   ByteSize (7)      — zero, small, KB, KB-frac, MB, GB, GB-frac
//   String (11)       — Truncate 4, PadRight 2, PadLeft 2, Count 3
//   Model (4)         — known, empty, long, alias
//
// APUs: 48 test functions, 0 helpers, 0 fixtures

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────
//
// [Reserved: No test helpers — uses standard testing assertions only]

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────
//
// [Reserved: No shared test infrastructure — each test is self-contained]

// ──────────────────────────────────────────────────────────────────────────
// 4. Error Handling
// ──────────────────────────────────────────────────────────────────────────
//
// [Reserved: Uses t.Errorf for all assertions — no custom error handling]

// ──────────────────────────────────────────────────────────────────────────
// 5. Test Functions
// ──────────────────────────────────────────────────────────────────────────

// ═══ Identity ═══

func TestPragmaGetKey(t *testing.T) {
	got := PragmaGet("I1.key")
	if got != "B-L0-hybrid-format" {
		t.Errorf("PragmaGet(I1.key) = %q, want %q", got, "B-L0-hybrid-format")
	}
}

func TestPragmaGetFormat(t *testing.T) {
	got := PragmaGet("I1.format")
	if got != "go" {
		t.Errorf("PragmaGet(I1.format) = %q, want %q", got, "go")
	}
}

func TestPragmaGetMissing(t *testing.T) {
	got := PragmaGet("I99.nonexistent")
	if got != "" {
		t.Errorf("PragmaGet(missing) = %q, want empty", got)
	}
}

func TestMetadataGetVersion(t *testing.T) {
	got := MetadataGet("C1.version")
	if got != "b-02.00" {
		t.Errorf("MetadataGet(C1.version) = %q, want %q", got, "b-02.00")
	}
}

func TestMetadataGetScripture(t *testing.T) {
	got := MetadataGet("C3.scripture")
	if got == "" {
		t.Error("MetadataGet(C3.scripture) returned empty")
	}
}

func TestMetadataGetMissing(t *testing.T) {
	got := MetadataGet("C99.nonexistent")
	if got != "" {
		t.Errorf("MetadataGet(missing) = %q, want empty", got)
	}
}

func TestPragmaSliceNotEmpty(t *testing.T) {
	if len(Pragma) == 0 {
		t.Error("Pragma slice is empty")
	}
}

func TestMetadataSliceNotEmpty(t *testing.T) {
	if len(Metadata) == 0 {
		t.Error("Metadata slice is empty")
	}
}

// ═══ Duration Formatting ═══

func TestDurationHours(t *testing.T) {
	d := 2*time.Hour + 30*time.Minute
	got := Duration(d)
	want := "2h 30m"
	if got != want {
		t.Errorf("Duration(%v) = %q, want %q", d, got, want)
	}
}

func TestDurationMinutes(t *testing.T) {
	d := 5*time.Minute + 42*time.Second
	got := Duration(d)
	want := "5m 42s"
	if got != want {
		t.Errorf("Duration(%v) = %q, want %q", d, got, want)
	}
}

func TestDurationSeconds(t *testing.T) {
	d := 7 * time.Second
	got := Duration(d)
	want := "7s"
	if got != want {
		t.Errorf("Duration(%v) = %q, want %q", d, got, want)
	}
}

func TestDurationZero(t *testing.T) {
	got := Duration(0)
	want := "0s"
	if got != want {
		t.Errorf("Duration(0) = %q, want %q", got, want)
	}
}

func TestDurationShortHours(t *testing.T) {
	d := 3*time.Hour + 15*time.Minute
	got := DurationShort(d)
	want := "3h15m"
	if got != want {
		t.Errorf("DurationShort(%v) = %q, want %q", d, got, want)
	}
}

func TestDurationShortMinutes(t *testing.T) {
	d := 42 * time.Minute
	got := DurationShort(d)
	want := "42m"
	if got != want {
		t.Errorf("DurationShort(%v) = %q, want %q", d, got, want)
	}
}

func TestDurationMSUnder1000(t *testing.T) {
	got := DurationMS(500)
	want := "<1s"
	if got != want {
		t.Errorf("DurationMS(500) = %q, want %q", got, want)
	}
}

func TestDurationMSOver1000(t *testing.T) {
	got := DurationMS(65000)
	want := "1m 5s"
	if got != want {
		t.Errorf("DurationMS(65000) = %q, want %q", got, want)
	}
}

func TestMinutes(t *testing.T) {
	got := Minutes(90)
	want := "1h 30m"
	if got != want {
		t.Errorf("Minutes(90) = %q, want %q", got, want)
	}
}

// ═══ Path Formatting ═══

func TestShortenPathEmpty(t *testing.T) {
	got := ShortenPath("")
	if got != "" {
		t.Errorf("ShortenPath(\"\") = %q, want empty", got)
	}
}

func TestShortenPathShort(t *testing.T) {
	got := ShortenPath("/usr/local/bin")
	if got != "/usr/local/bin" {
		t.Errorf("ShortenPath short path should be unchanged, got %q", got)
	}
}

func TestBaseNameSimple(t *testing.T) {
	got := BaseName("/path/to/file.go")
	want := "file.go"
	if got != want {
		t.Errorf("BaseName = %q, want %q", got, want)
	}
}

func TestBaseNameNoSlash(t *testing.T) {
	got := BaseName("file.go")
	want := "file.go"
	if got != want {
		t.Errorf("BaseName = %q, want %q", got, want)
	}
}

func TestBaseNameTrailingSlash(t *testing.T) {
	got := BaseName("/path/to/dir/")
	if got != "" {
		t.Errorf("BaseName trailing slash = %q, want empty", got)
	}
}

// ═══ Number Formatting ═══

func TestItoaPositive(t *testing.T) {
	got := Itoa(42)
	if got != "42" {
		t.Errorf("Itoa(42) = %q, want %q", got, "42")
	}
}

func TestItoaZero(t *testing.T) {
	got := Itoa(0)
	if got != "0" {
		t.Errorf("Itoa(0) = %q, want %q", got, "0")
	}
}

func TestItoaNegative(t *testing.T) {
	got := Itoa(-7)
	if got != "-7" {
		t.Errorf("Itoa(-7) = %q, want %q", got, "-7")
	}
}

func TestPercentNormal(t *testing.T) {
	got := Percent(75, 100)
	if got != "75%" {
		t.Errorf("Percent(75, 100) = %q, want %q", got, "75%")
	}
}

func TestPercentZeroTotal(t *testing.T) {
	got := Percent(50, 0)
	if got != "0%" {
		t.Errorf("Percent(50, 0) = %q, want %q", got, "0%")
	}
}

func TestPercentFull(t *testing.T) {
	got := Percent(100, 100)
	if got != "100%" {
		t.Errorf("Percent(100, 100) = %q, want %q", got, "100%")
	}
}

// ═══ ByteSize Formatting ═══

func TestByteSizeZero(t *testing.T) {
	got := ByteSize(0)
	if got != "0 B" {
		t.Errorf("ByteSize(0) = %q, want %q", got, "0 B")
	}
}

func TestByteSizeSmall(t *testing.T) {
	got := ByteSize(512)
	if got != "<1 KB" {
		t.Errorf("ByteSize(512) = %q, want %q", got, "<1 KB")
	}
}

func TestByteSizeKB(t *testing.T) {
	got := ByteSize(1024)
	if got != "1 KB" {
		t.Errorf("ByteSize(1024) = %q, want %q", got, "1 KB")
	}
}

func TestByteSizeKBFrac(t *testing.T) {
	got := ByteSize(1536)
	if got != "1.5 KB" {
		t.Errorf("ByteSize(1536) = %q, want %q", got, "1.5 KB")
	}
}

func TestByteSizeMB(t *testing.T) {
	got := ByteSize(1024 * 1024)
	if got != "1 MB" {
		t.Errorf("ByteSize(1MB) = %q, want %q", got, "1 MB")
	}
}

func TestByteSizeGB(t *testing.T) {
	got := ByteSize(1024 * 1024 * 1024)
	if got != "1 GB" {
		t.Errorf("ByteSize(1GB) = %q, want %q", got, "1 GB")
	}
}

func TestByteSizeGBFrac(t *testing.T) {
	got := ByteSize(1024*1024*1024 + 512*1024*1024)
	if got != "1.5 GB" {
		t.Errorf("ByteSize(1.5GB) = %q, want %q", got, "1.5 GB")
	}
}

// ═══ String Formatting ═══

func TestTruncateShort(t *testing.T) {
	got := Truncate("hello", 10)
	if got != "hello" {
		t.Errorf("Truncate short = %q, want %q", got, "hello")
	}
}

func TestTruncateLong(t *testing.T) {
	got := Truncate("hello world", 8)
	want := "hello..."
	if got != want {
		t.Errorf("Truncate long = %q, want %q", got, want)
	}
}

func TestTruncateTiny(t *testing.T) {
	got := Truncate("hello", 2)
	want := "he"
	if got != want {
		t.Errorf("Truncate tiny = %q, want %q", got, want)
	}
}

func TestTruncateExact(t *testing.T) {
	got := Truncate("hello", 5)
	if got != "hello" {
		t.Errorf("Truncate exact = %q, want %q", got, "hello")
	}
}

func TestPadRightShort(t *testing.T) {
	got := PadRight("hi", 5)
	want := "hi   "
	if got != want {
		t.Errorf("PadRight = %q, want %q", got, want)
	}
}

func TestPadRightAlreadyWide(t *testing.T) {
	got := PadRight("hello", 3)
	if got != "hello" {
		t.Errorf("PadRight already wide = %q, want %q", got, "hello")
	}
}

func TestPadLeftShort(t *testing.T) {
	got := PadLeft("42", 5)
	want := "   42"
	if got != want {
		t.Errorf("PadLeft = %q, want %q", got, want)
	}
}

func TestPadLeftAlreadyWide(t *testing.T) {
	got := PadLeft("hello", 3)
	if got != "hello" {
		t.Errorf("PadLeft already wide = %q, want %q", got, "hello")
	}
}

func TestCountSingular(t *testing.T) {
	got := Count(1, "file", "files")
	want := "1 file"
	if got != want {
		t.Errorf("Count(1) = %q, want %q", got, want)
	}
}

func TestCountPlural(t *testing.T) {
	got := Count(3, "file", "files")
	want := "3 files"
	if got != want {
		t.Errorf("Count(3) = %q, want %q", got, want)
	}
}

func TestCountZero(t *testing.T) {
	got := Count(0, "file", "files")
	want := "0 files"
	if got != want {
		t.Errorf("Count(0) = %q, want %q", got, want)
	}
}

// ═══ Model Formatting ═══

func TestShortModelNameKnown(t *testing.T) {
	tests := []struct {
		input string
		want  string
	}{
		{"Claude Opus 4.6", "Opus 4.6"},
		{"Claude Sonnet 4", "Sonnet 4"},
		{"Claude 3.5 Haiku", "Haiku 3.5"},
		{"Opus", "Opus"},
	}
	for _, tc := range tests {
		got := ShortModelName(tc.input)
		if got != tc.want {
			t.Errorf("ShortModelName(%q) = %q, want %q", tc.input, got, tc.want)
		}
	}
}

func TestShortModelNameEmpty(t *testing.T) {
	got := ShortModelName("")
	if got != "Unknown" {
		t.Errorf("ShortModelName(\"\") = %q, want %q", got, "Unknown")
	}
}

func TestShortModelNameLong(t *testing.T) {
	got := ShortModelName("Some Very Long Model Name Here")
	if len(got) > 18 { // 15 + "..."
		t.Errorf("ShortModelName long = %q, too long", got)
	}
}

func TestGetShortModelNameAlias(t *testing.T) {
	got := GetShortModelName("Opus")
	want := ShortModelName("Opus")
	if got != want {
		t.Errorf("GetShortModelName != ShortModelName: %q vs %q", got, want)
	}
}

// ═══ Test File Identity ═══

func TestTestFilePragmaGetKey(t *testing.T) {
	for _, pair := range TestFilePragma {
		if pair[0] == "I1.key" {
			if pair[1] != "B-L0-hybrid-format-test" {
				t.Errorf("TestFilePragma I1.key = %q, want %q", pair[1], "B-L0-hybrid-format-test")
			}
			return
		}
	}
	t.Error("TestFilePragma missing I1.key")
}

func TestTestFileMetadataGetVersion(t *testing.T) {
	for _, pair := range TestFileMetadata {
		if pair[0] == "C1.version" {
			if pair[1] != "b-02.00" {
				t.Errorf("TestFileMetadata C1.version = %q, want %q", pair[1], "b-02.00")
			}
			return
		}
	}
	t.Error("TestFileMetadata missing C1.version")
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────
//
// Testing:
//   - go test -v ./L0-universal/c-hybrid/services/format/go/...
//   - 50 tests covering all 18 public functions + test file identity
//   - No mocks, no fixtures — pure function input/output verification
//
// Build Verification:
//   - go build ./L0-universal/c-hybrid/services/format/go/...
//   - go vet ./L0-universal/c-hybrid/services/format/go/...

// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Test file — executed via go test, not direct invocation.
// Same-package (white-box) — tests can access unexported identifiers.

// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// Pure function tests — no resources to clean up. No files, no connections,
// no goroutines. Each test is stateless and independent.

// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// "Remove not the ancient landmark, which thy fathers have set."
// — Proverbs 22:28
//
// Safe to Modify:
//   - Add new test functions for new format functions
//   - Add table-driven tests for expanded coverage
//   - Add fuzz tests for string formatting
//
// Modify with Care:
//   - Existing test expectations — may indicate API contract
//   - TestFilePragma/TestFileMetadata — identity contract
//
// Never Modify:
//   - 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   - Test function naming convention (TestXxx)

// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// 50 tests proving every format function produces correct output at
// boundaries. Each domain (duration, path, number, byte size, string,
// model) tested independently. Pure function tests — deterministic,
// no external dependencies, no mocks.
//
// "Prove all things; hold fast that which is good."
// — 1 Thessalonians 5:21

// ============================================================================
// END CLOSING
// ============================================================================
