//omni:code --go -test
//omni:key B-L1-omnicode-hybrid-pragma-test
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L1-omnicode-hybrid-pragma-test
// Purpose: Tests for L1 pragma package — reader, methods, extensions, dispatch
// Biblical: Proverbs 27:17 - "Iron sharpeneth iron"
//           The Go tests are the second witness — same contracts, different language.
// Authors: Nova Dawn
// Version: a-01.00
// Created: 2026-02-14
//
// Covers:
//   A. pragma.go — stripCommentPrefix, parseLine1, parseMetaLine
//   B. pragma.go — Parse, ParseString, ParseReader
//   C. pragma.go — Pragma methods (IsConfig, IsNativeOmni, IsFolder, BlockPattern, String)
//   D. extensions.go — LookupExt, IsRegisteredExt, AllExts
//   E. extensions.go — ExtsByCategory, ExtsByPhase, ExtsByState
//   F. extensions.go — Format bridge (InternalFormatForExt, BlockPatternForExt, DefaultTypeForExt)
//   G. dispatch.go — IsOmniCodeFile
//
// ============================================================================
// END METADATA
// ============================================================================

package pragma

// ============================================================================
// SETUP
// ============================================================================

import (
	"os"
	"path/filepath"
	"strings"
	"testing"

	"creativeworkzstudio.com/bereshit/L0-universal/hybrid/config/util"
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// A. stripCommentPrefix
// ────────────────────────────────────────────────────────────────

func TestStripCommentPrefix_BarePragma(t *testing.T) {
	got := stripCommentPrefix("#!omni code --go")
	if got != "#!omni code --go" {
		t.Errorf("bare pragma: got %q, want %q", got, "#!omni code --go")
	}
}

func TestStripCommentPrefix_GoComment(t *testing.T) {
	got := stripCommentPrefix("// #!omni code --go -library")
	if got != "#!omni code --go -library" {
		t.Errorf("go comment: got %q, want %q", got, "#!omni code --go -library")
	}
}

func TestStripCommentPrefix_TOMLComment(t *testing.T) {
	got := stripCommentPrefix("# #!omni data --toml")
	if got != "#!omni data --toml" {
		t.Errorf("toml comment: got %q, want %q", got, "#!omni data --toml")
	}
}

func TestStripCommentPrefix_HTMLComment(t *testing.T) {
	got := stripCommentPrefix("<!-- #!omni documentation --md -->")
	if got != "#!omni documentation --md" {
		t.Errorf("html comment: got %q, want %q", got, "#!omni documentation --md")
	}
}

func TestStripCommentPrefix_NonPragmaLines(t *testing.T) {
	cases := []string{
		"package main",
		"// just a comment",
		"# not a pragma",
		"",
		"<!-- not omni -->",
	}
	for _, line := range cases {
		got := stripCommentPrefix(line)
		if got != "" {
			t.Errorf("non-pragma %q: got %q, want empty", line, got)
		}
	}
}

func TestStripCommentPrefix_Whitespace(t *testing.T) {
	got := stripCommentPrefix("   // #!omni code --go   ")
	if got != "#!omni code --go" {
		t.Errorf("whitespace: got %q, want %q", got, "#!omni code --go")
	}
}

// ────────────────────────────────────────────────────────────────
// B. parseLine1
// ────────────────────────────────────────────────────────────────

func TestParseLine1_Full(t *testing.T) {
	ft, fmt, m := parseLine1("#!omni code --go -library")
	if ft != TypeCode {
		t.Errorf("type: got %q, want %q", ft, TypeCode)
	}
	if fmt != FormatGo {
		t.Errorf("format: got %q, want %q", fmt, FormatGo)
	}
	if m != ModLibrary {
		t.Errorf("modifier: got %q, want %q", m, ModLibrary)
	}
}

func TestParseLine1_FormatOnly(t *testing.T) {
	ft, fmt, m := parseLine1("#!omni data --toml")
	if ft != TypeData {
		t.Errorf("type: got %q, want %q", ft, TypeData)
	}
	if fmt != FormatTOML {
		t.Errorf("format: got %q, want %q", fmt, FormatTOML)
	}
	if m != ModNone {
		t.Errorf("modifier: got %q, want %q", m, ModNone)
	}
}

func TestParseLine1_DefaultsToOmni(t *testing.T) {
	_, fmt, _ := parseLine1("#!omni code")
	if fmt != FormatOmni {
		t.Errorf("format: got %q, want %q", fmt, FormatOmni)
	}
}

func TestParseLine1_BarePrefix(t *testing.T) {
	_, fmt, _ := parseLine1("#!omni")
	if fmt != FormatOmni {
		t.Errorf("bare #!omni: format got %q, want %q", fmt, FormatOmni)
	}
}

// ────────────────────────────────────────────────────────────────
// C. parseMetaLine
// ────────────────────────────────────────────────────────────────

func TestParseMetaLine_Key(t *testing.T) {
	field, value := parseMetaLine("#!omni meta.key = B-L1-test")
	if field != "key" || value != "B-L1-test" {
		t.Errorf("key: got (%q, %q), want (\"key\", \"B-L1-test\")", field, value)
	}
}

func TestParseMetaLine_From(t *testing.T) {
	field, value := parseMetaLine("#!omni meta.from = bereshit/word/omni/syntax.omni")
	if field != "from" || value != "bereshit/word/omni/syntax.omni" {
		t.Errorf("from: got (%q, %q)", field, value)
	}
}

func TestParseMetaLine_At(t *testing.T) {
	field, value := parseMetaLine("#!omni meta.at = a-01.00")
	if field != "at" || value != "a-01.00" {
		t.Errorf("at: got (%q, %q)", field, value)
	}
}

func TestParseMetaLine_NonMetaLine(t *testing.T) {
	field, _ := parseMetaLine("#!omni code --go")
	if field != "" {
		t.Errorf("non-meta line should return empty field, got %q", field)
	}
}

// ────────────────────────────────────────────────────────────────
// D. ParseString / ParseReader
// ────────────────────────────────────────────────────────────────

func TestParseString_GoFile(t *testing.T) {
	content := `// #!omni code --go -library
// #!omni meta.key = B-L1-pragma
// #!omni meta.from = bereshit/word/omni/syntax.omni
// #!omni meta.at = a-01.00

package pragma
`
	p, err := ParseString(content)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if p.FileType != TypeCode {
		t.Errorf("type: got %q, want %q", p.FileType, TypeCode)
	}
	if p.Format != FormatGo {
		t.Errorf("format: got %q, want %q", p.Format, FormatGo)
	}
	if p.Modifier != ModLibrary {
		t.Errorf("modifier: got %q, want %q", p.Modifier, ModLibrary)
	}
	if p.Key != "B-L1-pragma" {
		t.Errorf("key: got %q, want %q", p.Key, "B-L1-pragma")
	}
	if p.From != "bereshit/word/omni/syntax.omni" {
		t.Errorf("from: got %q", p.From)
	}
	if p.At != "a-01.00" {
		t.Errorf("at: got %q", p.At)
	}
	if len(p.RawLines) != 4 {
		t.Errorf("raw lines: got %d, want 4", len(p.RawLines))
	}
}

func TestParseString_TOMLFile(t *testing.T) {
	content := `# #!omni data --toml
# #!omni meta.key = B-L0-config
# #!omni meta.at = b-02.00

[section]
key = "value"
`
	p, err := ParseString(content)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if p.FileType != TypeData {
		t.Errorf("type: got %q", p.FileType)
	}
	if p.Format != FormatTOML {
		t.Errorf("format: got %q", p.Format)
	}
	if p.Key != "B-L0-config" {
		t.Errorf("key: got %q", p.Key)
	}
	if p.At != "b-02.00" {
		t.Errorf("at: got %q", p.At)
	}
}

func TestParseString_HTMLFile(t *testing.T) {
	content := `<!-- #!omni documentation --md -->
<!-- #!omni meta.key = B-doc-readme -->

# README
`
	p, err := ParseString(content)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if p.FileType != TypeDocumentation {
		t.Errorf("type: got %q", p.FileType)
	}
	if p.Format != FormatMD {
		t.Errorf("format: got %q", p.Format)
	}
	if p.Key != "B-doc-readme" {
		t.Errorf("key: got %q", p.Key)
	}
}

func TestParseString_BareOmniFile(t *testing.T) {
	content := `#!omni template --omni
#!omni meta.key = B-L0-seed
#!omni meta.at = a-01.00
`
	p, err := ParseString(content)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if p.FileType != TypeTemplate {
		t.Errorf("type: got %q", p.FileType)
	}
	if p.Format != FormatOmni {
		t.Errorf("format: got %q", p.Format)
	}
	if p.Key != "B-L0-seed" {
		t.Errorf("key: got %q", p.Key)
	}
}

func TestParseString_NoPragmaError(t *testing.T) {
	content := "package main\n\nfunc main() {}\n"
	_, err := ParseString(content)
	if err == nil {
		t.Fatal("expected error for no pragma, got nil")
	}
	if !strings.Contains(err.Error(), "no #!omni pragma") {
		t.Errorf("error should mention missing pragma, got: %v", err)
	}
}

func TestParseString_ContiguousPragmaOnly(t *testing.T) {
	// Pragma lines must be contiguous — non-pragma between them stops scanning
	content := `// #!omni code --go
// not a pragma
// #!omni meta.key = should-not-be-read
`
	p, err := ParseString(content)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if p.Key != "" {
		t.Errorf("key should be empty (non-contiguous), got %q", p.Key)
	}
	if len(p.RawLines) != 1 {
		t.Errorf("raw lines: got %d, want 1", len(p.RawLines))
	}
}

func TestParseString_PreservesRawLines(t *testing.T) {
	content := `// #!omni code --go -library
// #!omni meta.key = test
`
	p, err := ParseString(content)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if len(p.RawLines) != 2 {
		t.Fatalf("raw lines: got %d, want 2", len(p.RawLines))
	}
	if p.RawLines[0] != "#!omni code --go -library" {
		t.Errorf("raw[0]: got %q", p.RawLines[0])
	}
	if p.RawLines[1] != "#!omni meta.key = test" {
		t.Errorf("raw[1]: got %q", p.RawLines[1])
	}
}

func TestParse_RealFile(t *testing.T) {
	// Create a temp file with pragma
	dir := t.TempDir()
	path := filepath.Join(dir, "test.go")
	content := `// #!omni code --go -library
// #!omni meta.key = B-test

package test
`
	if err := os.WriteFile(path, []byte(content), 0644); err != nil {
		t.Fatalf("write: %v", err)
	}

	p, err := Parse(path)
	if err != nil {
		t.Fatalf("parse: %v", err)
	}
	if p.FileType != TypeCode {
		t.Errorf("type: got %q", p.FileType)
	}
	if p.Key != "B-test" {
		t.Errorf("key: got %q", p.Key)
	}
}

func TestParse_MissingFile(t *testing.T) {
	_, err := Parse("/nonexistent/file.go")
	if err == nil {
		t.Fatal("expected error for missing file")
	}
}

// ────────────────────────────────────────────────────────────────
// E. Pragma Methods
// ────────────────────────────────────────────────────────────────

func TestPragma_IsConfig(t *testing.T) {
	cases := []struct {
		name   string
		pragma Pragma
		want   bool
	}{
		{"data+toml", Pragma{FileType: TypeData, Format: FormatTOML}, true},
		{"data+json", Pragma{FileType: TypeData, Format: FormatJSON}, true},
		{"data+yaml", Pragma{FileType: TypeData, Format: FormatYAML}, true},
		{"folder+toml", Pragma{FileType: TypeFolder, Format: FormatTOML}, true},
		{"data+omni", Pragma{FileType: TypeData, Format: FormatOmni}, false},
		{"code+toml", Pragma{FileType: TypeCode, Format: FormatTOML}, false},
	}
	for _, tc := range cases {
		t.Run(tc.name, func(t *testing.T) {
			got := tc.pragma.IsConfig()
			if got != tc.want {
				t.Errorf("IsConfig: got %v, want %v", got, tc.want)
			}
		})
	}
}

func TestPragma_IsNativeOmni(t *testing.T) {
	if !(&Pragma{Format: FormatOmni}).IsNativeOmni() {
		t.Error("format=omni should be native")
	}
	if (&Pragma{Format: FormatGo}).IsNativeOmni() {
		t.Error("format=go should not be native")
	}
}

func TestPragma_IsFolder(t *testing.T) {
	if !(&Pragma{FileType: TypeFolder}).IsFolder() {
		t.Error("type=folder should be folder")
	}
	if (&Pragma{FileType: TypeCode}).IsFolder() {
		t.Error("type=code should not be folder")
	}
}

func TestPragma_BlockPattern(t *testing.T) {
	cases := []struct {
		fileType Type
		want     string
	}{
		{TypeData, "3-block"},
		{TypeInterface, "3-block"},
		{TypeFolder, "3-block"},
		{TypeCode, "4-block"},
		{TypeTemplate, "4-block"},
		{TypeUniversal, "4-block"},
		{TypeDocumentation, "5-block"},
		{TypeSpec, "unknown"},
	}
	for _, tc := range cases {
		t.Run(string(tc.fileType), func(t *testing.T) {
			p := &Pragma{FileType: tc.fileType}
			got := p.BlockPattern()
			if got != tc.want {
				t.Errorf("BlockPattern(%s): got %q, want %q", tc.fileType, got, tc.want)
			}
		})
	}
}

func TestPragma_String(t *testing.T) {
	p := &Pragma{
		FileType: TypeCode,
		Format:   FormatGo,
		Modifier: ModLibrary,
		Key:      "B-test",
		At:       "a-01.00",
	}
	s := p.String()
	if !strings.Contains(s, "code") {
		t.Errorf("String should contain type: %s", s)
	}
	if !strings.Contains(s, "--go") {
		t.Errorf("String should contain format: %s", s)
	}
	if !strings.Contains(s, "-library") {
		t.Errorf("String should contain modifier: %s", s)
	}
	if !strings.Contains(s, "key=B-test") {
		t.Errorf("String should contain key: %s", s)
	}
	if !strings.Contains(s, "at=a-01.00") {
		t.Errorf("String should contain at: %s", s)
	}
}

func TestPragma_String_OmniDefault(t *testing.T) {
	// When format is omni (default), should not appear in string
	p := &Pragma{FileType: TypeCode, Format: FormatOmni}
	s := p.String()
	if strings.Contains(s, "--omni") {
		t.Errorf("String should omit default format omni: %s", s)
	}
}

// ────────────────────────────────────────────────────────────────
// F. Extension Registry
// ────────────────────────────────────────────────────────────────

func TestLookupExt_Known(t *testing.T) {
	ext, ok := LookupExt(ExtOmni)
	if !ok {
		t.Fatal(".omni should be registered")
	}
	if ext.Name != "OmniCode Source" {
		t.Errorf("name: got %q", ext.Name)
	}
	if ext.Category != CatOmniSuite {
		t.Errorf("category: got %q", ext.Category)
	}
}

func TestLookupExt_Unknown(t *testing.T) {
	_, ok := LookupExt(".xyz")
	if ok {
		t.Error(".xyz should not be registered")
	}
}

func TestIsRegisteredExt(t *testing.T) {
	if !IsRegisteredExt(ExtOmni) {
		t.Error(".omni should be registered")
	}
	if !IsRegisteredExt(ExtGenesis) {
		t.Error(".gen should be registered")
	}
	if IsRegisteredExt(".unknown") {
		t.Error(".unknown should not be registered")
	}
}

func TestAllExts_Count(t *testing.T) {
	all := AllExts()
	if len(all) != 15 {
		t.Errorf("AllExts: got %d, want 15", len(all))
	}
}

// ────────────────────────────────────────────────────────────────
// G. Extension Filtering
// ────────────────────────────────────────────────────────────────

func TestExtsByCategory_OmniSuite(t *testing.T) {
	exts := ExtsByCategory(CatOmniSuite)
	if len(exts) != 5 {
		t.Errorf("omni-suite count: got %d, want 5", len(exts))
	}
}

func TestExtsByCategory_Biblical(t *testing.T) {
	exts := ExtsByCategory(CatBiblical)
	if len(exts) != 7 {
		t.Errorf("biblical count: got %d, want 7", len(exts))
	}
}

func TestExtsByCategory_Cognitive(t *testing.T) {
	exts := ExtsByCategory(CatCognitive)
	if len(exts) != 3 {
		t.Errorf("cognitive count: got %d, want 3", len(exts))
	}
}

func TestExtsByCategory_SumsToAll(t *testing.T) {
	omni := len(ExtsByCategory(CatOmniSuite))
	biblical := len(ExtsByCategory(CatBiblical))
	cognitive := len(ExtsByCategory(CatCognitive))
	total := omni + biblical + cognitive
	if total != len(AllExts()) {
		t.Errorf("category sum %d != AllExts %d", total, len(AllExts()))
	}
}

func TestExtsByPhase_A(t *testing.T) {
	exts := ExtsByPhase("a")
	if len(exts) == 0 {
		t.Error("phase a should have extensions")
	}
	for _, e := range exts {
		if e.Phase != "a" {
			t.Errorf("extension %s has phase %q, want \"a\"", e.Ext, e.Phase)
		}
	}
}

func TestExtsByPhase_B(t *testing.T) {
	exts := ExtsByPhase("b")
	if len(exts) == 0 {
		t.Error("phase b should have extensions")
	}
	for _, e := range exts {
		if e.Phase != "b" {
			t.Errorf("extension %s has phase %q, want \"b\"", e.Ext, e.Phase)
		}
	}
}

func TestExtsByPhase_SumsToAll(t *testing.T) {
	a := len(ExtsByPhase("a"))
	b := len(ExtsByPhase("b"))
	if a+b != len(AllExts()) {
		t.Errorf("phase sum %d != AllExts %d", a+b, len(AllExts()))
	}
}

func TestExtsByState_Halt(t *testing.T) {
	exts := ExtsByState(StateHalt)
	if len(exts) == 0 {
		t.Error("HALT should have extensions")
	}
	for _, e := range exts {
		if e.StateMap != StateHalt {
			t.Errorf("extension %s has state %q, want HALT", e.Ext, e.StateMap)
		}
	}
}

func TestExtsByState_Proceed(t *testing.T) {
	exts := ExtsByState(StateProceed)
	if len(exts) == 0 {
		t.Error("PROCEED should have extensions")
	}
}

// ────────────────────────────────────────────────────────────────
// H. Format Bridge
// ────────────────────────────────────────────────────────────────

func TestInternalFormatForExt(t *testing.T) {
	cases := []struct {
		ext  string
		want Format
	}{
		{ExtOmniFold, FormatTOML},
		{ExtWitness, FormatJSONC},  // TOML spec: internal_format = "jsonc"
		{ExtOmni, FormatOmni},
		{ExtGenesis, FormatBinary}, // TOML spec: internal_format = "binary"
		{ExtReveal, FormatText},    // TOML spec: internal_format = "text"
		{ExtScroll, FormatJSONC},   // TOML spec: internal_format = "jsonc"
	}
	for _, tc := range cases {
		t.Run(tc.ext, func(t *testing.T) {
			got := InternalFormatForExt(tc.ext)
			if got != tc.want {
				t.Errorf("InternalFormat(%s): got %q, want %q", tc.ext, got, tc.want)
			}
		})
	}
}

func TestInternalFormatForExt_Unknown(t *testing.T) {
	got := InternalFormatForExt(".xyz")
	if got != "" {
		t.Errorf("unknown ext: got %q, want empty", got)
	}
}

func TestBlockPatternForExt(t *testing.T) {
	cases := []struct {
		ext  string
		want string
	}{
		{ExtOmniFold, "3-block"},
		{ExtOmni, "varies"},
		{ExtOmniInterface, "4-block"},
		{ExtGenesis, "none"},
	}
	for _, tc := range cases {
		t.Run(tc.ext, func(t *testing.T) {
			got := BlockPatternForExt(tc.ext)
			if got != tc.want {
				t.Errorf("BlockPattern(%s): got %q, want %q", tc.ext, got, tc.want)
			}
		})
	}
}

func TestDefaultTypeForExt(t *testing.T) {
	cases := []struct {
		ext  string
		want Type
	}{
		{ExtOmni, TypeCode},
		{ExtOmniFold, TypeFolder},
		{ExtSeed, TypeTemplate},
		{ExtOmniInterface, TypeInterface},
	}
	for _, tc := range cases {
		t.Run(tc.ext, func(t *testing.T) {
			got := DefaultTypeForExt(tc.ext)
			if got != tc.want {
				t.Errorf("DefaultType(%s): got %q, want %q", tc.ext, got, tc.want)
			}
		})
	}
}

// ────────────────────────────────────────────────────────────────
// I. IsOmniCodeFile
// ────────────────────────────────────────────────────────────────

func TestIsOmniCodeFile(t *testing.T) {
	cases := []struct {
		path string
		want bool
	}{
		{"config.omni", true},
		{"root.ofd", true},
		{"main.go", false},
		{"config.toml", false},
		{"README.md", false},
	}
	for _, tc := range cases {
		t.Run(tc.path, func(t *testing.T) {
			got := IsOmniCodeFile(tc.path)
			if got != tc.want {
				t.Errorf("IsOmniCodeFile(%q): got %v, want %v", tc.path, got, tc.want)
			}
		})
	}
}

// ────────────────────────────────────────────────────────────────
// J. Spec-Driven Registry
// ────────────────────────────────────────────────────────────────

func TestIsSpecLoaded(t *testing.T) {
	// On this workstation with the full repo, extensions.toml should be on disk.
	// IsSpecLoaded tells us whether the registry loaded from the spec or fell
	// back to hardcoded defaults.
	loaded := IsSpecLoaded()
	t.Logf("IsSpecLoaded: %v", loaded)

	// If the spec file exists, it MUST have been loaded
	specPath := resolveExtensionsSpec()
	if specPath != "" && !loaded {
		t.Error("extensions.toml found on disk but spec was NOT loaded — this is a bug")
	}
}

func TestSpecDriven_AllExtensionsMatch(t *testing.T) {
	// Verify that spec-loaded and default registries agree on all 15 extensions.
	// This is the assurance test: if someone edits the TOML, the default
	// registry must also be updated (or vice versa).
	defaults := defaultRegistry()
	active := getRegistry()

	if len(active) != len(defaults) {
		t.Errorf("active registry has %d entries, default has %d", len(active), len(defaults))
	}

	for ext, def := range defaults {
		act, ok := active[ext]
		if !ok {
			t.Errorf("extension %s in defaults but missing from active registry", ext)
			continue
		}

		// Check the fields that MUST match between spec and defaults
		if act.Category != def.Category {
			t.Errorf("%s category: active=%q default=%q", ext, act.Category, def.Category)
		}
		if act.Phase != def.Phase {
			t.Errorf("%s phase: active=%q default=%q", ext, act.Phase, def.Phase)
		}
		if act.Operation != def.Operation {
			t.Errorf("%s operation: active=%q default=%q", ext, act.Operation, def.Operation)
		}
		if act.InternalFormat != def.InternalFormat {
			t.Errorf("%s format: active=%q default=%q", ext, act.InternalFormat, def.InternalFormat)
		}
		if act.BlockPattern != def.BlockPattern {
			t.Errorf("%s block: active=%q default=%q", ext, act.BlockPattern, def.BlockPattern)
		}
		if act.DefaultType != def.DefaultType {
			t.Errorf("%s type: active=%q default=%q", ext, act.DefaultType, def.DefaultType)
		}
		if act.StateMap != def.StateMap {
			t.Errorf("%s state: active=%q default=%q", ext, act.StateMap, def.StateMap)
		}
	}
}

func TestSpecDriven_CrossCheckSpecLookup(t *testing.T) {
	// The TOML spec has lookup tables (lookup.all, lookup.by_category, etc.)
	// that should be consistent with what we loaded into the registry.
	// This test loads the spec directly and cross-references.
	specPath := resolveExtensionsSpec()
	if specPath == "" {
		t.Skip("extensions.toml not on disk — cross-check skipped")
	}

	spec, err := util.LoadTOMLFile[extensionsSpec](specPath)
	if err != nil {
		t.Fatalf("load spec: %v", err)
	}

	// Verify lookup.all.count matches loaded registry
	reg := getRegistry()
	if spec.Lookup.All.Count != len(reg) {
		t.Errorf("spec lookup.all.count=%d, registry has %d entries",
			spec.Lookup.All.Count, len(reg))
	}

	// Verify every extension in lookup.all is in the registry
	for _, ext := range spec.Lookup.All.Extensions {
		if _, ok := reg[ext]; !ok {
			t.Errorf("extension %s in spec lookup.all but missing from registry", ext)
		}
	}

	// Verify phase counts match
	phaseA := ExtsByPhase("a")
	if spec.Lookup.PhaseA.Count != len(phaseA) {
		t.Errorf("spec phase_a.count=%d, registry has %d phase-a extensions",
			spec.Lookup.PhaseA.Count, len(phaseA))
	}
	phaseB := ExtsByPhase("b")
	if spec.Lookup.PhaseB.Count != len(phaseB) {
		t.Errorf("spec phase_b.count=%d, registry has %d phase-b extensions",
			spec.Lookup.PhaseB.Count, len(phaseB))
	}

	// Verify category groupings match
	omniSuite := ExtsByCategory(CatOmniSuite)
	if len(spec.Lookup.ByCategory.OmniSuite) != len(omniSuite) {
		t.Errorf("spec by_category.omni_suite has %d, registry has %d",
			len(spec.Lookup.ByCategory.OmniSuite), len(omniSuite))
	}
	biblical := ExtsByCategory(CatBiblical)
	if len(spec.Lookup.ByCategory.Biblical) != len(biblical) {
		t.Errorf("spec by_category.biblical has %d, registry has %d",
			len(spec.Lookup.ByCategory.Biblical), len(biblical))
	}
	cognitive := ExtsByCategory(CatCognitive)
	if len(spec.Lookup.ByCategory.Cognitive) != len(cognitive) {
		t.Errorf("spec by_category.cognitive has %d, registry has %d",
			len(spec.Lookup.ByCategory.Cognitive), len(cognitive))
	}

	// Verify format_bridge.mappings match loaded InternalFormat values
	for ext, specFmt := range spec.FormatBridge.Mappings {
		got := InternalFormatForExt(ext)
		if string(got) != specFmt {
			t.Errorf("format_bridge %s: spec=%q, registry=%q", ext, specFmt, got)
		}
	}

	// Verify format_bridge.block_patterns match loaded BlockPattern values
	for ext, specPat := range spec.FormatBridge.BlockPatterns {
		got := BlockPatternForExt(ext)
		if got != specPat {
			t.Errorf("block_pattern %s: spec=%q, registry=%q", ext, specPat, got)
		}
	}
}

func TestParseStateMapping(t *testing.T) {
	cases := []struct {
		input string
		want  StateMapping
	}{
		{"PROCEED — active content, ready for processing", StateProceed},
		{"HALT — verification is a stopping point, truth established", StateHalt},
		{"AWAIT — interface declared, awaiting implementation", StateAwait},
		{"CLEAR — health assessment resets/recalibrates", StateClear},
		{"PIVOT — positional awareness enables direction change", StatePivot},
		{"PROCEED", StateProceed},     // No description
		{"UNKNOWN — something", StateProceed}, // Unknown defaults to PROCEED
	}
	for _, tc := range cases {
		t.Run(tc.input, func(t *testing.T) {
			got := parseStateMapping(tc.input)
			if got != tc.want {
				t.Errorf("parseStateMapping(%q): got %q, want %q", tc.input, got, tc.want)
			}
		})
	}
}

// ────────────────────────────────────────────────────────────────
// K. Caching
// ────────────────────────────────────────────────────────────────

func TestPragmaCache_HitOnSecondParse(t *testing.T) {
	// Write a test file, parse it twice — second call should use cache.
	dir := t.TempDir()
	path := filepath.Join(dir, "cache-test.toml")
	writeTestFile(t, path, "# #!omni data --toml\n# #!omni meta.key = cache-test\n\n[section]\nkey = \"val\"\n")

	// Clear cache to ensure clean state
	InvalidatePragmaCache()

	p1, err := Parse(path)
	if err != nil {
		t.Fatalf("first parse: %v", err)
	}

	p2, err := Parse(path)
	if err != nil {
		t.Fatalf("second parse: %v", err)
	}

	// Both should return the same pointer (cached)
	if p1 != p2 {
		t.Error("expected cached pointer on second Parse call")
	}
	if p1.Key != "cache-test" {
		t.Errorf("key: got %q, want %q", p1.Key, "cache-test")
	}
}

func TestPragmaCache_InvalidateEntry(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "invalidate-test.toml")
	writeTestFile(t, path, "# #!omni data --toml\n# #!omni meta.key = v1\n\n[s]\nk = \"v\"\n")

	InvalidatePragmaCache()

	p1, err := Parse(path)
	if err != nil {
		t.Fatalf("parse v1: %v", err)
	}
	if p1.Key != "v1" {
		t.Fatalf("expected v1, got %q", p1.Key)
	}

	// Modify the file and invalidate the entry
	writeTestFile(t, path, "# #!omni data --toml\n# #!omni meta.key = v2\n\n[s]\nk = \"v\"\n")
	InvalidatePragmaCacheEntry(path)

	p2, err := Parse(path)
	if err != nil {
		t.Fatalf("parse v2: %v", err)
	}
	if p2.Key != "v2" {
		t.Errorf("expected v2 after invalidation, got %q", p2.Key)
	}
}

func TestDispatchCache_LoadConfigMap(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "dispatch-cache.toml")
	writeTestFile(t, path, "# #!omni data --toml\n# #!omni meta.key = dispatch-cache\n\n[info]\nname = \"cached\"\n")

	InvalidateDispatchCache()

	p1, data1, err := LoadConfigMap(path)
	if err != nil {
		t.Fatalf("first load: %v", err)
	}
	if p1.Key != "dispatch-cache" {
		t.Errorf("key: got %q, want %q", p1.Key, "dispatch-cache")
	}
	if data1["info"] == nil {
		t.Error("expected info key in data")
	}

	// Second call should return cached result
	p2, data2, err := LoadConfigMap(path)
	if err != nil {
		t.Fatalf("second load: %v", err)
	}
	if p2.Key != p1.Key {
		t.Error("cached pragma key mismatch")
	}
	if data2["info"] == nil {
		t.Error("cached data missing info key")
	}
}

// ────────────────────────────────────────────────────────────────
// L. Structured Error Types
// ────────────────────────────────────────────────────────────────

func TestDispatch_LoadError_Type(t *testing.T) {
	// Loading a nonexistent file should return a *util.LoadError
	_, _, err := LoadConfig[map[string]any]("/nonexistent/path/to/file.toml")
	if err == nil {
		t.Fatal("expected error for nonexistent file")
	}

	loadErr := util.AsLoadError(err)
	if loadErr == nil {
		t.Errorf("expected *util.LoadError, got %T: %v", err, err)
	} else {
		if loadErr.Op != "pragma" {
			t.Errorf("expected op='pragma', got %q", loadErr.Op)
		}
		if loadErr.File != "/nonexistent/path/to/file.toml" {
			t.Errorf("expected file in error, got %q", loadErr.File)
		}
	}
}

func TestDispatch_LoadError_UnsupportedFormat(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "unsupported.omni")
	writeTestFile(t, path, "#!omni code --go\n")

	InvalidateDispatchCache()

	_, _, err := LoadConfig[map[string]any](path)
	if err == nil {
		t.Fatal("expected error for unsupported format in dispatch")
	}

	loadErr := util.AsLoadError(err)
	if loadErr == nil {
		t.Errorf("expected *util.LoadError, got %T: %v", err, err)
	} else {
		if loadErr.Op != "dispatch" {
			t.Errorf("expected op='dispatch', got %q", loadErr.Op)
		}
	}
}

// writeTestFile is a helper — writes content to path, fails test on error.
func writeTestFile(t *testing.T, path, content string) {
	t.Helper()
	if err := os.MkdirAll(filepath.Dir(path), 0o755); err != nil {
		t.Fatal(err)
	}
	if err := os.WriteFile(path, []byte(content), 0o644); err != nil {
		t.Fatal(err)
	}
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// The Go tests are the second witness for L1's pragma contracts.
// Rust tests (68 passing) are the first witness — same truth, different language.
//
// Together they verify:
//   - Comment prefix stripping works identically in both languages
//   - Pragma parsing produces the same results
//   - Extension registry has the same 15 entries with same properties
//   - Format bridge returns consistent values
//   - Methods behave equivalently
//
// Proverbs 27:17 — "Iron sharpeneth iron; so a man sharpeneth
// the countenance of his friend."
//
// ============================================================================
// END CLOSING
// ============================================================================
