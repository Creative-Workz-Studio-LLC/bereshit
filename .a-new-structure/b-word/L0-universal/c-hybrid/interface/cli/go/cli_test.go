//omni:code --go -demo-test
//omni:key B-L0-hybrid-cli-test
//omni:version b-03.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-hybrid-cli-test
// Purpose: Comprehensive tests for CLI scaffolding library
// Biblical: 1 Thessalonians 5:21 — Prove all things; hold fast that which is good
// Authors: Nova Dawn
// Version: 3.0.0
// Created: 2026-02-18
// Updated: 2026-02-18

package cli

// ──────────────────────────────────────────────────────────────────────────
// Imports (in METADATA — Go requires all imports before any declarations)
// ──────────────────────────────────────────────────────────────────────────

import (
	"strings"
	"testing"
)

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// section-order: [s01]-imports
// (imports declared in METADATA section — Go language constraint)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Org Chart
// ──────────────────────────────────────────────────────────────────────────
//
// Test Functions (35 tests in Cv zone):
//   Identity:       TestPragmaGet, TestPragmaGet_Missing,
//                   TestMetadataGet, TestMetadataGet_Missing
//   Constructor:    TestNew, TestNew_HasFlags
//   Configuration:  TestWithBiblical, TestWithVersion, TestWithJSON, TestFlag
//   Parsing:        TestParse_Empty, TestParse_WithFlags, TestParse_WithArgs
//   Queries:        TestWantsHelp_Default, TestWantsHelp_Long, TestWantsHelp_Short,
//                   TestWantsVersion_Default, TestWantsVersion_Long, TestWantsVersion_Short,
//                   TestWantsJSON_Default, TestWantsJSON_Set,
//                   TestVersion_Default, TestVersion_Set,
//                   TestArgs, TestFirstArg, TestFirstArg_Empty,
//                   TestHasArgs, TestHasArgs_Empty, TestArgCount
//   Usage Builder:  TestNewUsage, TestAddSection, TestWithStandardFlags,
//                   TestWithStandardFlags_WithVersion,
//                   TestUsageWithVersion, TestUsageString,
//                   TestUsageString_WithVersion

// ──────────────────────────────────────────────────────────────────────────
// 2. Helpers
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: No test helpers needed — all tests use package constructors directly]

// ──────────────────────────────────────────────────────────────────────────
// 3. Core Operations
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: Test logic lives in Cv zone — test functions ARE the operations]

// ──────────────────────────────────────────────────────────────────────────
// 4. Error Handling
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: Uses t.Error/t.Errorf/t.Fatal — no custom recovery]

// ──────────────────────────────────────────────────────────────────────────
// 5. Test Functions
// ──────────────────────────────────────────────────────────────────────────
// [Reserved: All Test* functions in Cv zone below]

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────
//
// 35 tests covering:
//   - Identity: PragmaGet, MetadataGet (found and missing)
//   - Constructor: New creates App with name, desc, help flags
//   - Configuration: builder methods (WithBiblical, WithVersion, WithJSON, Flag)
//   - Parsing: empty args, flag args, positional args
//   - Queries: all query methods return correct values
//   - Usage: builder, sections, standard flags, version, String()

// --- Identity Tests ---

func TestPragmaGet(t *testing.T) {
	val, ok := PragmaGet("I1.key")
	if !ok {
		t.Fatal("PragmaGet(I1.key) should return true")
	}
	if val != "B-L0-hybrid-cli" {
		t.Errorf("PragmaGet(I1.key) = %q, want %q", val, "B-L0-hybrid-cli")
	}
}

func TestPragmaGet_Missing(t *testing.T) {
	val, ok := PragmaGet("Z99.nonexistent")
	if ok {
		t.Error("PragmaGet for missing key should return false")
	}
	if val != "" {
		t.Errorf("PragmaGet missing should return empty, got %q", val)
	}
}

func TestMetadataGet(t *testing.T) {
	val, ok := MetadataGet("C1.version")
	if !ok {
		t.Fatal("MetadataGet(C1.version) should return true")
	}
	if val != "b-03.00" {
		t.Errorf("MetadataGet(C1.version) = %q, want %q", val, "b-03.00")
	}
}

func TestMetadataGet_Missing(t *testing.T) {
	val, ok := MetadataGet("Z99.nonexistent")
	if ok {
		t.Error("MetadataGet for missing key should return false")
	}
	if val != "" {
		t.Errorf("MetadataGet missing should return empty, got %q", val)
	}
}

// --- Constructor Tests ---

func TestNew(t *testing.T) {
	app := New("test", "test tool")
	if app.Name != "test" {
		t.Errorf("Name = %q, want %q", app.Name, "test")
	}
	if app.Description != "test tool" {
		t.Errorf("Description = %q, want %q", app.Description, "test tool")
	}
}

func TestNew_HasFlags(t *testing.T) {
	app := New("test", "test tool")
	if app.flagSet == nil {
		t.Fatal("flagSet should not be nil")
	}
	if app.helpFlag == nil {
		t.Error("helpFlag should be set by New()")
	}
	if app.hFlag == nil {
		t.Error("hFlag should be set by New()")
	}
}

// --- Configuration Tests ---

func TestWithBiblical(t *testing.T) {
	app := New("test", "desc")
	ret := app.WithBiblical("Proverbs 15:28")

	if app.Biblical != "Proverbs 15:28" {
		t.Errorf("Biblical = %q, want %q", app.Biblical, "Proverbs 15:28")
	}
	if ret != app {
		t.Error("WithBiblical should return same app (builder pattern)")
	}
}

func TestWithVersion(t *testing.T) {
	app := New("test", "desc")
	ret := app.WithVersion("1.2.3")

	if app.version != "1.2.3" {
		t.Errorf("version = %q, want %q", app.version, "1.2.3")
	}
	if app.versionFlag == nil {
		t.Error("versionFlag should be set by WithVersion()")
	}
	if app.vFlag == nil {
		t.Error("vFlag should be set by WithVersion()")
	}
	if ret != app {
		t.Error("WithVersion should return same app (builder pattern)")
	}
}

func TestWithJSON(t *testing.T) {
	app := New("test", "desc")
	ret := app.WithJSON()

	if app.jsonFlag == nil {
		t.Error("jsonFlag should be set by WithJSON()")
	}
	if ret != app {
		t.Error("WithJSON should return same app (builder pattern)")
	}
}

func TestFlag(t *testing.T) {
	app := New("test", "desc")
	fs := app.Flag()

	if fs == nil {
		t.Fatal("Flag() should return non-nil FlagSet")
	}
	if fs != app.flagSet {
		t.Error("Flag() should return the app's FlagSet")
	}
}

// --- Parsing Tests ---

func TestParse_Empty(t *testing.T) {
	app := New("test", "desc")
	if err := app.Parse([]string{}); err != nil {
		t.Fatalf("Parse empty args: %v", err)
	}
	if !app.parsed {
		t.Error("parsed should be true after Parse()")
	}
}

func TestParse_WithFlags(t *testing.T) {
	app := New("test", "desc")
	app.WithVersion("1.0.0")
	if err := app.Parse([]string{"-help"}); err != nil {
		t.Fatalf("Parse with -help: %v", err)
	}
	if !app.WantsHelp() {
		t.Error("WantsHelp() should be true after parsing -help")
	}
}

func TestParse_WithArgs(t *testing.T) {
	app := New("test", "desc")
	if err := app.Parse([]string{"file.go", "file2.go"}); err != nil {
		t.Fatalf("Parse with args: %v", err)
	}
	args := app.Args()
	if len(args) != 2 {
		t.Errorf("Args length = %d, want 2", len(args))
	}
	if args[0] != "file.go" {
		t.Errorf("Args[0] = %q, want %q", args[0], "file.go")
	}
}

// --- Query Tests ---

func TestWantsHelp_Default(t *testing.T) {
	app := New("test", "desc")
	_ = app.Parse([]string{})
	if app.WantsHelp() {
		t.Error("WantsHelp() should be false by default")
	}
}

func TestWantsHelp_Long(t *testing.T) {
	app := New("test", "desc")
	_ = app.Parse([]string{"-help"})
	if !app.WantsHelp() {
		t.Error("WantsHelp() should be true after -help")
	}
}

func TestWantsHelp_Short(t *testing.T) {
	app := New("test", "desc")
	_ = app.Parse([]string{"-h"})
	if !app.WantsHelp() {
		t.Error("WantsHelp() should be true after -h")
	}
}

func TestWantsVersion_Default(t *testing.T) {
	app := New("test", "desc")
	app.WithVersion("1.0.0")
	_ = app.Parse([]string{})
	if app.WantsVersion() {
		t.Error("WantsVersion() should be false by default")
	}
}

func TestWantsVersion_Long(t *testing.T) {
	app := New("test", "desc")
	app.WithVersion("1.0.0")
	_ = app.Parse([]string{"-version"})
	if !app.WantsVersion() {
		t.Error("WantsVersion() should be true after -version")
	}
}

func TestWantsVersion_Short(t *testing.T) {
	app := New("test", "desc")
	app.WithVersion("1.0.0")
	_ = app.Parse([]string{"-V"})
	if !app.WantsVersion() {
		t.Error("WantsVersion() should be true after -V")
	}
}

func TestWantsVersion_NoWithVersion(t *testing.T) {
	app := New("test", "desc")
	_ = app.Parse([]string{})
	if app.WantsVersion() {
		t.Error("WantsVersion() should be false when WithVersion not called")
	}
}

func TestWantsJSON_Default(t *testing.T) {
	app := New("test", "desc")
	app.WithJSON()
	_ = app.Parse([]string{})
	if app.WantsJSON() {
		t.Error("WantsJSON() should be false by default")
	}
}

func TestWantsJSON_Set(t *testing.T) {
	app := New("test", "desc")
	app.WithJSON()
	_ = app.Parse([]string{"-json"})
	if !app.WantsJSON() {
		t.Error("WantsJSON() should be true after -json")
	}
}

func TestWantsJSON_NoWithJSON(t *testing.T) {
	app := New("test", "desc")
	_ = app.Parse([]string{})
	if app.WantsJSON() {
		t.Error("WantsJSON() should be false when WithJSON not called")
	}
}

func TestVersion_Default(t *testing.T) {
	app := New("test", "desc")
	if app.Version() != "" {
		t.Errorf("Version() = %q, want empty", app.Version())
	}
}

func TestVersion_Set(t *testing.T) {
	app := New("test", "desc")
	app.WithVersion("2.5.0")
	if app.Version() != "2.5.0" {
		t.Errorf("Version() = %q, want %q", app.Version(), "2.5.0")
	}
}

func TestArgs(t *testing.T) {
	app := New("test", "desc")
	_ = app.Parse([]string{"a", "b", "c"})
	args := app.Args()
	if len(args) != 3 {
		t.Errorf("Args length = %d, want 3", len(args))
	}
}

func TestFirstArg(t *testing.T) {
	app := New("test", "desc")
	_ = app.Parse([]string{"first", "second"})
	arg, ok := app.FirstArg()
	if !ok {
		t.Error("FirstArg() should return true when args present")
	}
	if arg != "first" {
		t.Errorf("FirstArg() = %q, want %q", arg, "first")
	}
}

func TestFirstArg_Empty(t *testing.T) {
	app := New("test", "desc")
	_ = app.Parse([]string{})
	arg, ok := app.FirstArg()
	if ok {
		t.Error("FirstArg() should return false when no args")
	}
	if arg != "" {
		t.Errorf("FirstArg() = %q, want empty", arg)
	}
}

func TestHasArgs(t *testing.T) {
	app := New("test", "desc")
	_ = app.Parse([]string{"arg"})
	if !app.HasArgs() {
		t.Error("HasArgs() should be true when args present")
	}
}

func TestHasArgs_Empty(t *testing.T) {
	app := New("test", "desc")
	_ = app.Parse([]string{})
	if app.HasArgs() {
		t.Error("HasArgs() should be false when no args")
	}
}

func TestArgCount(t *testing.T) {
	app := New("test", "desc")
	_ = app.Parse([]string{"a", "b"})
	if app.ArgCount() != 2 {
		t.Errorf("ArgCount() = %d, want 2", app.ArgCount())
	}
}

func TestArgCount_Empty(t *testing.T) {
	app := New("test", "desc")
	_ = app.Parse([]string{})
	if app.ArgCount() != 0 {
		t.Errorf("ArgCount() = %d, want 0", app.ArgCount())
	}
}

// --- Usage Builder Tests ---

func TestNewUsage(t *testing.T) {
	app := New("mytool", "does things")
	app.WithBiblical("Proverbs 15:28")
	usage := NewUsage(app)

	if usage.Name != "mytool" {
		t.Errorf("Name = %q", usage.Name)
	}
	if usage.Description != "does things" {
		t.Errorf("Description = %q", usage.Description)
	}
	if usage.Biblical != "Proverbs 15:28" {
		t.Errorf("Biblical = %q", usage.Biblical)
	}
}

func TestNewUsage_CarriesVersion(t *testing.T) {
	app := New("mytool", "desc")
	app.WithVersion("3.0.0")
	usage := NewUsage(app)

	if usage.Version != "3.0.0" {
		t.Errorf("Version = %q, want %q", usage.Version, "3.0.0")
	}
}

func TestAddSection(t *testing.T) {
	app := New("test", "desc")
	usage := NewUsage(app)
	ret := usage.AddSection("USAGE", "test [options] <file>")

	if len(usage.Sections) != 1 {
		t.Fatalf("Sections length = %d, want 1", len(usage.Sections))
	}
	if usage.Sections[0].Title != "USAGE" {
		t.Errorf("Section title = %q", usage.Sections[0].Title)
	}
	if ret != usage {
		t.Error("AddSection should return same usage (builder pattern)")
	}
}

func TestWithStandardFlags(t *testing.T) {
	app := New("test", "desc")
	usage := NewUsage(app)
	usage.WithStandardFlags()

	if len(usage.Sections) != 1 {
		t.Fatalf("Sections length = %d, want 1", len(usage.Sections))
	}
	if usage.Sections[0].Title != "FLAGS" {
		t.Errorf("Section title = %q, want FLAGS", usage.Sections[0].Title)
	}

	// Should have help and json lines (no version — not set)
	lines := usage.Sections[0].Lines
	if len(lines) != 2 {
		t.Errorf("FLAGS lines = %d, want 2 (help + json, no version)", len(lines))
	}
}

func TestWithStandardFlags_WithVersion(t *testing.T) {
	app := New("test", "desc")
	app.WithVersion("1.0.0")
	usage := NewUsage(app)
	usage.WithStandardFlags()

	lines := usage.Sections[0].Lines
	if len(lines) != 3 {
		t.Errorf("FLAGS lines = %d, want 3 (help + version + json)", len(lines))
	}
}

func TestUsageWithVersion(t *testing.T) {
	app := New("test", "desc")
	usage := NewUsage(app)
	ret := usage.WithVersion("4.0.0")

	if usage.Version != "4.0.0" {
		t.Errorf("Version = %q, want %q", usage.Version, "4.0.0")
	}
	if ret != usage {
		t.Error("WithVersion should return same usage (builder pattern)")
	}
}

func TestUsageString(t *testing.T) {
	app := New("mytool", "does things")
	app.WithBiblical("Proverbs 15:28")
	usage := NewUsage(app)
	usage.AddSection("USAGE", "mytool [options] <file>")

	s := usage.String()

	// Header — no version
	if !strings.HasPrefix(s, "mytool - does things\n") {
		t.Errorf("header wrong, got: %q", s[:40])
	}
	// Section present
	if !strings.Contains(s, "USAGE:\n") {
		t.Error("should contain USAGE section")
	}
	if !strings.Contains(s, "  mytool [options] <file>") {
		t.Error("should contain usage line")
	}
	// Biblical anchor
	if !strings.Contains(s, "Biblical: Proverbs 15:28") {
		t.Error("should contain biblical reference")
	}
}

func TestUsageString_WithVersion(t *testing.T) {
	app := New("mytool", "does things")
	app.WithVersion("2.0.0")
	usage := NewUsage(app)

	s := usage.String()

	if !strings.HasPrefix(s, "mytool 2.0.0 - does things\n") {
		t.Errorf("versioned header wrong, got: %q", s[:40])
	}
}

func TestUsageString_NoBiblical(t *testing.T) {
	app := New("mytool", "desc")
	usage := NewUsage(app)
	s := usage.String()

	if strings.Contains(s, "Biblical:") {
		t.Error("should not contain Biblical line when not set")
	}
}

//
// ──────────────────────────────────────────────────────────────────────────
// Ce — Execution
// ──────────────────────────────────────────────────────────────────────────
//
// go test -v ./L0-universal/c-hybrid/interface/cli/go/
// go test -run TestPragmaGet ./L0-universal/c-hybrid/interface/cli/go/
//
// ──────────────────────────────────────────────────────────────────────────
// Cc — Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// No resources to clean up. Pure value-type tests.
//
// ──────────────────────────────────────────────────────────────────────────
// X1: Policy
// ──────────────────────────────────────────────────────────────────────────
//
// Never:
//   - Skip identity tests (contract verification)
//   - Skip builder pattern return tests (API contract)
//
// Careful:
//   - Changing expected String() output (downstream may depend on format)
//   - Changing flag names in parsing tests (must match cli.go)
//
// Safe:
//   - Adding new test cases for edge conditions
//   - Adding tests for new App/Usage methods
//
// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
//
// Note: Test suite for CLI scaffolding — verifies identity, constructor,
// configuration, parsing, queries, and usage builder. Same-package tests
// for white-box access to unexported fields.
//
// Scripture: 1 Thessalonians 5:21 — "Prove all things; hold fast that
// which is good."
//
// ============================================================================
// END CLOSING
// ============================================================================
