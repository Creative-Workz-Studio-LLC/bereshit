//omni:code --go -test
//omni:key B-L0-config-util-test
//omni:version b-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-config-util-test
// Purpose: Tests for config/util — production-grade verification of all utilities
// Biblical: Proverbs 27:17 - "Iron sharpeneth iron"
// Authors: Nova Dawn
// Version: b-01.00
// Created: 2026-02-13
//
// Tests everything the parent loader_test.go doesn't:
//   - exists.go:  FileExists bug fix, IsFile, EnsureDir
//   - errors.go:  ValidationError.Value in output, AsLoadError, AsValidationError
//   - formats.go: RegisterFormat, RegisterConfigFormat, AllKnownExts, AllConfigFormats
//   - must.go:    MustNoErr, MustVal
//   - tripwire.go: TripwireWarningTo, TripwireBannerTo, TripwireBannerOnce
//   - jsonc.go:   ParseJSONC edge cases, LoadJSONCFile, LoadJSONCMap
//   - loader.go:  LoadConfigFile format dispatch
//   - root.go:    RootGuard concurrent safety
//
// ============================================================================
// END METADATA
// ============================================================================

package util

// ============================================================================
// SETUP
// ============================================================================

import (
	"bytes"
	"errors"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"sync"
	"testing"
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// A. exists.go
// ────────────────────────────────────────────────────────────────

func TestFileExistsCorrectness(t *testing.T) {
	// Real file → true
	tmp := t.TempDir()
	f := filepath.Join(tmp, "exists.txt")
	os.WriteFile(f, []byte("hello"), 0644)

	if !FileExists(f) {
		t.Errorf("FileExists(%q) = false, want true", f)
	}

	// Missing file → false
	if FileExists(filepath.Join(tmp, "nope.txt")) {
		t.Error("FileExists should return false for missing file")
	}

	// Directory → true (FileExists checks existence, not type)
	if !FileExists(tmp) {
		t.Error("FileExists should return true for directories")
	}
}

func TestIsFile(t *testing.T) {
	tmp := t.TempDir()
	f := filepath.Join(tmp, "regular.txt")
	os.WriteFile(f, []byte("data"), 0644)

	if !IsFile(f) {
		t.Error("IsFile should return true for regular file")
	}
	if IsFile(tmp) {
		t.Error("IsFile should return false for directory")
	}
	if IsFile(filepath.Join(tmp, "nope")) {
		t.Error("IsFile should return false for missing path")
	}
}

func TestEnsureDir(t *testing.T) {
	tmp := t.TempDir()

	// Create nested directory
	nested := filepath.Join(tmp, "a", "b", "c")
	if err := EnsureDir(nested); err != nil {
		t.Fatalf("EnsureDir(%q) failed: %v", nested, err)
	}
	if !DirExists(nested) {
		t.Error("EnsureDir should have created the directory")
	}

	// No-op on existing directory
	if err := EnsureDir(nested); err != nil {
		t.Errorf("EnsureDir on existing dir should be no-op, got: %v", err)
	}

	// Error when path is a file
	f := filepath.Join(tmp, "file.txt")
	os.WriteFile(f, []byte("x"), 0644)
	if err := EnsureDir(f); err == nil {
		t.Error("EnsureDir should fail when path is a regular file")
	}
}

// ────────────────────────────────────────────────────────────────
// B. errors.go
// ────────────────────────────────────────────────────────────────

func TestLoadErrorUnwrap(t *testing.T) {
	inner := fmt.Errorf("permission denied")
	le := NewLoadError("config.toml", "read", inner)

	// Unwrap chain works
	if !errors.Is(le, inner) {
		t.Error("LoadError.Unwrap should allow errors.Is to find inner error")
	}

	// Error format
	got := le.Error()
	want := "load config.toml: read: permission denied"
	if got != want {
		t.Errorf("LoadError.Error() = %q, want %q", got, want)
	}
}

func TestValidationErrorValue(t *testing.T) {
	// Without value — no "(got: ...)" suffix
	ve := NewValidationError("types/primitives.toml", "int9", "min_value", "expected int")
	got := ve.Error()
	if strings.Contains(got, "got:") {
		t.Errorf("Error without Value should not contain 'got:', got %q", got)
	}

	// With value — includes "(got: ...)"
	veVal := NewValidationErrorWithValue("types/primitives.toml", "int9", "min_value", "expected int", "hello")
	got = veVal.Error()
	if !strings.Contains(got, "(got: hello)") {
		t.Errorf("Error with Value should contain '(got: hello)', got %q", got)
	}

	// Tiered formatting — section only
	veSec := NewValidationError("config.toml", "database", "", "missing required fields")
	got = veSec.Error()
	want := "config.toml [database]: missing required fields"
	if got != want {
		t.Errorf("Section-level error = %q, want %q", got, want)
	}

	// Tiered formatting — file only
	veFile := NewValidationError("config.toml", "", "", "empty file")
	got = veFile.Error()
	want = "config.toml: empty file"
	if got != want {
		t.Errorf("File-level error = %q, want %q", got, want)
	}
}

func TestAsLoadError(t *testing.T) {
	le := NewLoadError("x.toml", "parse", fmt.Errorf("bad"))

	// Direct
	if got := AsLoadError(le); got == nil {
		t.Error("AsLoadError should find direct LoadError")
	}

	// Wrapped
	wrapped := fmt.Errorf("outer: %w", le)
	if got := AsLoadError(wrapped); got == nil {
		t.Error("AsLoadError should find wrapped LoadError")
	} else if got.File != "x.toml" {
		t.Errorf("AsLoadError.File = %q, want 'x.toml'", got.File)
	}

	// Not present
	if got := AsLoadError(fmt.Errorf("plain error")); got != nil {
		t.Error("AsLoadError should return nil for non-LoadError")
	}
}

func TestAsValidationError(t *testing.T) {
	ve := NewValidationError("a.toml", "s", "f", "bad")

	if got := AsValidationError(ve); got == nil {
		t.Error("AsValidationError should find direct ValidationError")
	}

	wrapped := fmt.Errorf("context: %w", ve)
	if got := AsValidationError(wrapped); got == nil {
		t.Error("AsValidationError should find wrapped ValidationError")
	}

	if got := AsValidationError(fmt.Errorf("nope")); got != nil {
		t.Error("AsValidationError should return nil for non-ValidationError")
	}
}

// ────────────────────────────────────────────────────────────────
// C. formats.go
// ────────────────────────────────────────────────────────────────

func TestRegisterFormat(t *testing.T) {
	// Register a new extension
	RegisterFormat(".omni", "omni")
	defer func() {
		// Clean up — remove the test registration
		mu.Lock()
		delete(StandardFormats, ".omni")
		mu.Unlock()
	}()

	got := FormatFromExt(".omni")
	if got != "omni" {
		t.Errorf("FormatFromExt('.omni') after Register = %q, want 'omni'", got)
	}

	if !IsKnownExt(".omni") {
		t.Error("IsKnownExt('.omni') should be true after registration")
	}
}

func TestRegisterConfigFormat(t *testing.T) {
	RegisterConfigFormat("omni-config")
	defer func() {
		mu.Lock()
		delete(ConfigFormats, "omni-config")
		mu.Unlock()
	}()

	if !IsConfigFormat("omni-config") {
		t.Error("IsConfigFormat('omni-config') should be true after registration")
	}
}

func TestAllKnownExts(t *testing.T) {
	exts := AllKnownExts()
	if len(exts) == 0 {
		t.Fatal("AllKnownExts returned empty")
	}

	// Should be sorted
	for i := 1; i < len(exts); i++ {
		if exts[i] < exts[i-1] {
			t.Errorf("AllKnownExts not sorted: %q before %q", exts[i-1], exts[i])
		}
	}

	// Should include known extensions
	found := make(map[string]bool)
	for _, ext := range exts {
		found[ext] = true
	}
	for _, required := range []string{".go", ".toml", ".json", ".rs", ".c"} {
		if !found[required] {
			t.Errorf("AllKnownExts missing %q", required)
		}
	}
}

func TestAllConfigFormats(t *testing.T) {
	formats := AllConfigFormats()
	if len(formats) < 4 {
		t.Errorf("AllConfigFormats returned %d, expected at least 4", len(formats))
	}

	// Should be sorted
	for i := 1; i < len(formats); i++ {
		if formats[i] < formats[i-1] {
			t.Errorf("AllConfigFormats not sorted: %q before %q", formats[i-1], formats[i])
		}
	}
}

// ────────────────────────────────────────────────────────────────
// D. must.go
// ────────────────────────────────────────────────────────────────

func TestMustNoErr(t *testing.T) {
	// Success — no panic
	MustNoErr(nil)

	// Panic case
	defer func() {
		r := recover()
		if r == nil {
			t.Error("MustNoErr with error should panic")
		}
		msg := fmt.Sprintf("%v", r)
		if !strings.Contains(msg, "config: must:") {
			t.Errorf("panic message should contain 'config: must:', got %q", msg)
		}
	}()
	MustNoErr(fmt.Errorf("boom"))
}

func TestMustVal(t *testing.T) {
	// Success
	got := MustVal(42, true, "should not see this")
	if got != 42 {
		t.Errorf("MustVal(42, true, ...) = %d, want 42", got)
	}

	// Panic case
	defer func() {
		r := recover()
		if r == nil {
			t.Error("MustVal with ok=false should panic")
		}
		msg := fmt.Sprintf("%v", r)
		if !strings.Contains(msg, "not found") {
			t.Errorf("panic message should contain 'not found', got %q", msg)
		}
	}()
	_ = MustVal(0, false, "not found")
}

// ────────────────────────────────────────────────────────────────
// E. tripwire.go
// ────────────────────────────────────────────────────────────────

func TestTripwireWarningTo(t *testing.T) {
	var buf bytes.Buffer
	TripwireWarningTo(&buf, "config", "/fallback/path", "index.toml missing")

	got := buf.String()
	if !strings.Contains(got, "WARNING [config]") {
		t.Errorf("warning should contain 'WARNING [config]', got %q", got)
	}
	if !strings.Contains(got, "index.toml missing") {
		t.Errorf("warning should contain reason, got %q", got)
	}
	if !strings.Contains(got, "/fallback/path") {
		t.Errorf("warning should contain fallback path, got %q", got)
	}
}

func TestTripwireBannerTo(t *testing.T) {
	var buf bytes.Buffer
	TripwireBannerTo(&buf, "INDEX MISSING", []string{
		"Expected: word/core/index.toml",
		"Fallback: hardcoded system list",
	})

	got := buf.String()

	// Should contain the title
	if !strings.Contains(got, "TRIPWIRE: INDEX MISSING") {
		t.Errorf("banner should contain title, got %q", got)
	}

	// Should contain body lines
	if !strings.Contains(got, "Expected: word/core/index.toml") {
		t.Error("banner should contain body lines")
	}

	// Should have 3 borders (top, after title, bottom)
	borderChar := "\u2550" // ═
	borderCount := strings.Count(got, borderChar)
	// 64 chars × 3 borders = 192 border chars minimum
	if borderCount < 192 {
		t.Errorf("banner should have 3 borders (192+ ═ chars), got %d", borderCount)
	}
}

func TestTripwireBannerOnce(t *testing.T) {
	// Reset the seen map for test isolation
	bannerMu.Lock()
	origSeen := bannerSeen
	bannerSeen = make(map[string]bool)
	bannerMu.Unlock()

	defer func() {
		bannerMu.Lock()
		bannerSeen = origSeen
		bannerMu.Unlock()
	}()

	// Capture stdout — TripwireBannerOnce writes to os.Stdout via TripwireBanner.
	// We can't easily capture stdout in a test, so instead verify the dedup logic
	// by checking the seen map directly.
	TripwireBannerOnce("TEST BANNER", []string{"line 1"})

	bannerMu.Lock()
	seen1 := bannerSeen["TEST BANNER"]
	bannerMu.Unlock()
	if !seen1 {
		t.Error("first call should mark banner as seen")
	}

	// Second call — should be silently skipped (seen map already has it)
	TripwireBannerOnce("TEST BANNER", []string{"line 1"})

	// Different title — should fire
	TripwireBannerOnce("OTHER BANNER", []string{"line 2"})

	bannerMu.Lock()
	seen2 := bannerSeen["OTHER BANNER"]
	bannerMu.Unlock()
	if !seen2 {
		t.Error("different title should also be marked as seen")
	}
}

// ────────────────────────────────────────────────────────────────
// F. jsonc.go
// ────────────────────────────────────────────────────────────────

func TestParseJSONCComments(t *testing.T) {
	tests := []struct {
		name  string
		input string
		valid bool
	}{
		{
			name:  "single line comment on own line",
			input: "// this is a comment\n{\"key\": \"value\"}",
			valid: true,
		},
		{
			name:  "multi-line comments",
			input: "/* header */\n{\"key\": \"value\"}",
			valid: true,
		},
		{
			name:  "trailing comma object",
			input: `{"a": 1, "b": 2,}`,
			valid: true,
		},
		{
			name:  "trailing comma array",
			input: `{"arr": [1, 2, 3,]}`,
			valid: true,
		},
		{
			name:  "all features combined",
			input: "// top comment\n{\"key\": \"val\", /* inline */ \"arr\": [1,2,],}",
			valid: true,
		},
		{
			name:  "invalid json after stripping",
			input: `{not json at all`,
			valid: false,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			cleaned, err := ParseJSONC([]byte(tt.input))
			if tt.valid {
				if err != nil {
					t.Fatalf("ParseJSONC failed: %v", err)
				}
				if len(cleaned) == 0 {
					t.Error("ParseJSONC returned empty bytes")
				}
			} else {
				if err == nil {
					t.Error("ParseJSONC should have failed for invalid input")
				}
			}
		})
	}
}

func TestLoadJSONCFile(t *testing.T) {
	tmp := t.TempDir()

	// Write a JSONC file
	content := `{
	// This is a comment
	"name": "test",
	"version": 1,
}`
	path := filepath.Join(tmp, "test.jsonc")
	os.WriteFile(path, []byte(content), 0644)

	type Config struct {
		Name    string `json:"name"`
		Version int    `json:"version"`
	}

	result, err := LoadJSONCFile[Config](path)
	if err != nil {
		t.Fatalf("LoadJSONCFile failed: %v", err)
	}
	if result.Name != "test" {
		t.Errorf("Name = %q, want 'test'", result.Name)
	}
	if result.Version != 1 {
		t.Errorf("Version = %d, want 1", result.Version)
	}

	// Missing file
	_, err = LoadJSONCFile[Config]("/nonexistent.jsonc")
	if err == nil {
		t.Error("LoadJSONCFile should error on missing file")
	}
	le := AsLoadError(err)
	if le == nil {
		t.Error("error should be a *LoadError")
	} else if le.Op != "stat" {
		t.Errorf("LoadError.Op = %q, want 'stat'", le.Op)
	}
}

func TestLoadJSONCMap(t *testing.T) {
	tmp := t.TempDir()
	path := filepath.Join(tmp, "map.jsonc")
	os.WriteFile(path, []byte(`{"a": 1, "b": "two"}`), 0644)

	m, err := LoadJSONCMap(path)
	if err != nil {
		t.Fatalf("LoadJSONCMap failed: %v", err)
	}
	if m["a"] != float64(1) {
		t.Errorf("m['a'] = %v, want 1", m["a"])
	}
	if m["b"] != "two" {
		t.Errorf("m['b'] = %v, want 'two'", m["b"])
	}
}

// ────────────────────────────────────────────────────────────────
// G. loader.go (util) — format dispatch
// ────────────────────────────────────────────────────────────────

func TestLoadConfigFileTOML(t *testing.T) {
	tmp := t.TempDir()
	path := filepath.Join(tmp, "config.toml")
	os.WriteFile(path, []byte("[section]\nkey = \"value\"\n"), 0644)

	m, err := LoadConfigFile(path)
	if err != nil {
		t.Fatalf("LoadConfigFile(.toml) failed: %v", err)
	}
	section, ok := m["section"].(map[string]interface{})
	if !ok {
		t.Fatal("expected 'section' to be a map")
	}
	if section["key"] != "value" {
		t.Errorf("section.key = %v, want 'value'", section["key"])
	}
}

func TestLoadConfigFileJSONC(t *testing.T) {
	tmp := t.TempDir()
	path := filepath.Join(tmp, "config.jsonc")
	os.WriteFile(path, []byte(`{
	// comment
	"name": "test",
}`), 0644)

	m, err := LoadConfigFile(path)
	if err != nil {
		t.Fatalf("LoadConfigFile(.jsonc) failed: %v", err)
	}
	if m["name"] != "test" {
		t.Errorf("name = %v, want 'test'", m["name"])
	}
}

func TestLoadConfigFileJSON(t *testing.T) {
	tmp := t.TempDir()
	path := filepath.Join(tmp, "config.json")
	os.WriteFile(path, []byte(`{"count": 42}`), 0644)

	m, err := LoadConfigFile(path)
	if err != nil {
		t.Fatalf("LoadConfigFile(.json) failed: %v", err)
	}
	if m["count"] != float64(42) {
		t.Errorf("count = %v, want 42", m["count"])
	}
}

func TestLoadConfigFileUnsupported(t *testing.T) {
	tmp := t.TempDir()
	path := filepath.Join(tmp, "config.xml")
	os.WriteFile(path, []byte("<config/>"), 0644)

	_, err := LoadConfigFile(path)
	if err == nil {
		t.Error("LoadConfigFile(.xml) should return error for unsupported format")
	}
	le := AsLoadError(err)
	if le == nil {
		t.Error("error should be a *LoadError")
	} else if le.Op != "detect" {
		t.Errorf("LoadError.Op = %q, want 'detect'", le.Op)
	}
}

func TestLoadConfigFileMissing(t *testing.T) {
	_, err := LoadConfigFile("/nonexistent/config.toml")
	if err == nil {
		t.Error("LoadConfigFile should error on missing file")
	}
	le := AsLoadError(err)
	if le == nil {
		t.Error("error should be a *LoadError")
	} else if le.Op != "stat" {
		t.Errorf("LoadError.Op = %q, want 'stat'", le.Op)
	}
}

// ────────────────────────────────────────────────────────────────
// H. root.go — concurrent safety
// ────────────────────────────────────────────────────────────────

func TestRootGuardBasic(t *testing.T) {
	g := NewRootGuard("test")

	// Check before Set → error
	if err := g.Check(); err == nil {
		t.Error("Check before Set should return error")
	}

	// Set + Check → success
	g.Set("/some/path")
	if err := g.Check(); err != nil {
		t.Errorf("Check after Set failed: %v", err)
	}
	if got := g.Path(); got != "/some/path" {
		t.Errorf("Path() = %q, want '/some/path'", got)
	}
}

func TestRootGuardConcurrent(t *testing.T) {
	g := NewRootGuard("concurrent")
	g.Set("/initial")

	var wg sync.WaitGroup
	errs := make(chan error, 100)

	// 50 goroutines reading
	for i := 0; i < 50; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			if err := g.Check(); err != nil {
				errs <- err
			}
			_ = g.Path()
		}()
	}

	// 10 goroutines writing
	for i := 0; i < 10; i++ {
		wg.Add(1)
		go func(n int) {
			defer wg.Done()
			g.Set(fmt.Sprintf("/path/%d", n))
		}(i)
	}

	wg.Wait()
	close(errs)

	for err := range errs {
		t.Errorf("concurrent error: %v", err)
	}
}

// ────────────────────────────────────────────────────────────────
// I. doc.go — package identity
// ────────────────────────────────────────────────────────────────

func TestPackageIdentity(t *testing.T) {
	pragma := Pragma()
	if pragma == nil {
		t.Fatal("Pragma() returned nil")
	}
	if pragma["P1.key"] != "B-L0-config-util" {
		t.Errorf("Pragma P1.key = %q, want 'B-L0-config-util'", pragma["P1.key"])
	}

	meta := Metadata()
	if meta == nil {
		t.Fatal("Metadata() returned nil")
	}
	if meta["M1.key"] != "B-L0-config-util" {
		t.Errorf("Metadata M1.key = %q, want 'B-L0-config-util'", meta["M1.key"])
	}

	// Verify pragma and metadata return copies (not the original map)
	p1 := Pragma()
	p2 := Pragma()
	p1["P1.key"] = "mutated"
	if p2["P1.key"] == "mutated" {
		t.Error("Pragma() should return a copy, not the original map")
	}
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// "Iron sharpeneth iron; so a man sharpeneth the countenance of his friend."
// — Proverbs 27:17
//
// Tests are the iron. They sharpen the code by proving it does what it claims.
// Every function in util/ earns its place by passing these.
//
// ============================================================================
// END CLOSING
// ============================================================================
