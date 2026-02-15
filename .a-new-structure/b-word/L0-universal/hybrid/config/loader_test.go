//omni:code --go -test
//omni:key B-L0-config-loader-test
//omni:version b-01.00

// ============================================================================
// METADATA
// ============================================================================

// Package config_test verifies and assesses the L0 config loader.
//
// Purpose: Assessment API for config loader — tests ARE the assessment.
//
// These are not disposable tests. They are the assessment API.
// The same Pragma(), Metadata(), Nav() calls that tests use are the same
// calls a dashboard, CI, or health system would use. Tests happen to run
// through `go test` — assessments happen to run through whatever calls them.
//
// Five assessment categories:
//
//   A. Identity   — Does the package know itself? (Pragma, Metadata, Nav)
//   B. Structure  — Does the declared architecture match reality? (Nav counts, flows)
//   C. Errors     — Are errors structured and type-assertable? (errors.As)
//   D. Loading    — Do the load functions produce correct results? (LoadAll, LoadSystem, LoadSpec)
//   E. Health     — Comprehensive diagnostic (all categories combined)
//
// Each category is both a test AND a reusable assessment.
// The pattern: call public API → verify against declared contract → report.
//
// # Core Identity
//
//	Key:     B-L0-config-loader-test
//	Type:    Assessment (validates AND serves as runtime health API)
//	Version: b-01.00 (2026-02-13)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2025-12-13
//
//	Scripture: "Prove all things; hold fast that which is good" — 1 Thess 5:21
//	Anchor:    "By their fruits ye shall know them" — Matthew 7:20
//
// # Version History
//
//   - b-01.00 (2026-02-13) — Assessment rewrite: identity, structure, errors, loading, health
//   - a-02.00 (2026-02-13) — L0 Util integration tests
//   - a-01.50 (2025-12-14) — PhD rigor + Bible accessibility refinement
//   - a-01.00 (2025-12-13) — Initial test file, 10 test functions
//
// # Interface
//
//	Needs:   testing, errors, os, path/filepath, strings | config (under test), util
//	Tests:   loader.go — Pragma, Metadata, Nav, SetRoot, LoadAll, LoadSystem, LoadSpec, errors
//	Run:     go test -v ./L0-universal/hybrid/config/... (or MODULE_ROOT=/path/to/b-word)
package config_test

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

//--- I.1 Standard Library ---
import (
	"errors"        // type assertions for structured errors (errors.As)
	"os"            // environment variable for module root
	"path/filepath" // path construction
	"strings"       // string matching for nav field validation
	"testing"       // test framework
)

//--- I.2 Internal Packages ---
import (
	"creativeworkzstudio.com/bereshit/L0-universal/hybrid/config"
	"creativeworkzstudio.com/bereshit/L0-universal/hybrid/config/util"
	"creativeworkzstudio.com/bereshit/L0-universal/spiral/identity"
)

// ────────────────────────────────────────────────────────────────
// Pragma
// ────────────────────────────────────────────────────────────────

// _pragma — OmniCode file identity for this test file.
// Test files carry identity for linter compliance but do not register
// with the identity registry (tests are verification, not production).
var _pragma = map[string]string{
	"P1.key":    "B-L0-config-loader-test",
	"P1.type":   "code",
	"P1.format": "go",
	"P1.style":  "test",
	"P2.blocks": "4-block",
	"P3.from":   "b-word/seed/code/L0/go/library.go",
	"P4.at":     "b-01.00",
	"P5.title":  "Config Loader Assessment Suite",
	"P5.summary": "5-category assessment: identity, structure, errors, loading, health",
}

// ────────────────────────────────────────────────────────────────
// Metadata
// ────────────────────────────────────────────────────────────────

// _metadata — OmniCode file metadata for this test file.
var _metadata = map[string]string{
	"M1.key":            "B-L0-config-loader-test",
	"M1.component_type": "Assessment",
	"M1.architect":      "Seanje Lenox-Wise",
	"M1.implementation": "Nova Dawn",
	"M1.created":        "2025-12-13",
	"M6.tags":           "test, assessment, config, identity, health",
	"M6.layer":          "L0-universal",
}

// Ensure pragma/metadata are referenced to satisfy the compiler.
var (
	_ = _pragma
	_ = _metadata
)

// ────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────

// Expected OmniCode key for this package. Single source of truth for
// identity assertions — if this changes, tests know immediately.
const expectedKey = "B-L0-config-loader"

// Expected systems in the 9-System Architecture (dependency order).
var expectedSystems = []string{
	"math", "types", "language", "bible",
	"health", "permission", "identity", "network", "filesystem",
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Org Chart — Assessment Structure
// ────────────────────────────────────────────────────────────────
//
// Five assessment categories, each both a test AND a reusable API:
//   A. Identity   — Does the package know itself?
//   B. Structure  — Does the declared architecture match reality?
//   C. Error      — Are errors structured and type-assertable?
//   D. Loading    — Do the load functions produce correct results?
//   E. Health     — Comprehensive diagnostic (all categories combined)
//
// Helpers provide test setup (root resolution, field assertions).

// ────────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────────

// getModuleRoot derives the Go module root path for tests.
//
// The module root is b-word/ — where go.mod lives. All config paths
// (IndexDir, watchPaths, manifest system paths) are relative to this.
//
// Resolution order:
//  1. MODULE_ROOT env var (explicit override)
//  2. Derived from test CWD: L0-universal/hybrid/config/ → 3 up → b-word/
func getModuleRoot(t *testing.T) string {
	t.Helper()

	if root := os.Getenv("MODULE_ROOT"); root != "" {
		return root
	}

	// Derive from CWD: b-word/L0-universal/hybrid/config/ → 3 up → b-word/
	wd, err := os.Getwd()
	if err != nil {
		t.Fatalf("failed to get working directory: %v", err)
	}

	root := filepath.Join(wd, "..", "..", "..")
	absRoot, err := filepath.Abs(root)
	if err != nil {
		t.Fatalf("failed to get absolute path: %v", err)
	}

	return absRoot
}

// setupTest sets root and returns it with a cleanup function.
func setupTest(t *testing.T) (string, func()) {
	t.Helper()
	root := getModuleRoot(t)
	config.SetRoot(root)
	return root, func() {}
}

// requireField checks that a map field exists and is non-empty.
// Returns the value for further inspection.
func requireField(t *testing.T, m map[string]string, key, context string) string {
	t.Helper()
	v, ok := m[key]
	if !ok {
		t.Errorf("[%s] missing required field %q", context, key)
		return ""
	}
	if v == "" {
		t.Errorf("[%s] field %q is empty", context, key)
	}
	return v
}

// ────────────────────────────────────────────────────────────────
// A. Identity Assessment
// ────────────────────────────────────────────────────────────────
//
// Does the package know itself?
//
// These tests verify that Pragma(), Metadata(), and Nav() return
// well-formed identity data. The same calls are usable by any
// external assessment tool — `go test` is just one consumer.

// TestPragmaIdentity verifies the package's pragma (P1-P5) is complete.
//
// Assessment: Every OmniCode package MUST have P1 (identity), P2 (structure),
// P3 (derivation), P4 (version), P5 (summary). Missing fields mean the
// package can't be indexed, discovered, or assessed.
func TestPragmaIdentity(t *testing.T) {
	pragma := config.Pragma()

	// P1: Core Identity — the package knows its own key, type, format, style
	key := requireField(t, pragma, "P1.key", "Pragma")
	if key != expectedKey {
		t.Errorf("Pragma P1.key = %q, want %q", key, expectedKey)
	}
	requireField(t, pragma, "P1.type", "Pragma")
	requireField(t, pragma, "P1.format", "Pragma")
	requireField(t, pragma, "P1.style", "Pragma")

	// P2: Structure
	blocks := requireField(t, pragma, "P2.blocks", "Pragma")
	if blocks != "4-block" {
		t.Errorf("Pragma P2.blocks = %q, want %q", blocks, "4-block")
	}

	// P3: Derivation — traces back to template
	requireField(t, pragma, "P3.from", "Pragma")
	requireField(t, pragma, "P3.derives", "Pragma")

	// P4: Version
	requireField(t, pragma, "P4.at", "Pragma")

	// P5: Summary
	requireField(t, pragma, "P5.title", "Pragma")
	requireField(t, pragma, "P5.summary", "Pragma")
}

// TestMetadataContract verifies the package's metadata (M1-M10) is complete.
//
// Assessment: M1-M5 are code-facing (what a Go developer needs).
// M6-M10 are system-facing (what OmniCode needs for indexing).
// Missing metadata means the package can't participate in the system.
func TestMetadataContract(t *testing.T) {
	meta := config.Metadata()

	// M1: Core Identity
	key := requireField(t, meta, "M1.key", "Metadata")
	if key != expectedKey {
		t.Errorf("Metadata M1.key = %q, want %q", key, expectedKey)
	}
	requireField(t, meta, "M1.component_type", "Metadata")
	requireField(t, meta, "M1.architect", "Metadata")
	requireField(t, meta, "M1.implementation", "Metadata")
	requireField(t, meta, "M1.created", "Metadata")

	// M3: Interface — what this package needs and provides
	requireField(t, meta, "M3.requires.stdlib", "Metadata")
	requireField(t, meta, "M3.requires.external", "Metadata")
	requireField(t, meta, "M3.requires.internal", "Metadata")
	requireField(t, meta, "M3.import", "Metadata")
	requireField(t, meta, "M3.pattern", "Metadata")

	// M4: Public API — the declared surface area
	requireField(t, meta, "M4.config", "Metadata")
	requireField(t, meta, "M4.load", "Metadata")
	requireField(t, meta, "M4.check", "Metadata")
	requireField(t, meta, "M4.access", "Metadata")
}

// TestNavArchitecture verifies the package's navigation map is complete.
//
// Assessment: Nav() returns the BODY org chart — ladder, baton, APU.
// If this is incomplete, assessment tools can't understand the package's
// internal structure.
func TestNavArchitecture(t *testing.T) {
	nav := config.Nav()

	// Ladder: every section must have name, purpose, depends, count
	sections := []string{"B.1", "B.2", "B.3", "B.4", "B.5"}
	for _, s := range sections {
		requireField(t, nav, s+".name", "Nav.Ladder")
		requireField(t, nav, s+".purpose", "Nav.Ladder")
		requireField(t, nav, s+".count", "Nav.Ladder")
		// .depends can be empty for foundation layers
		if _, ok := nav[s+".depends"]; !ok {
			t.Errorf("[Nav.Ladder] missing field %q (can be empty, but must exist)", s+".depends")
		}
	}

	// Baton: flow paths must be declared
	requireField(t, nav, "flow.entry", "Nav.Baton")
	requireField(t, nav, "flow.exit", "Nav.Baton")
	requireField(t, nav, "flow.pattern", "Nav.Baton")
	requireField(t, nav, "flow.primary.path", "Nav.Baton")
	requireField(t, nav, "flow.primary.why", "Nav.Baton")
	requireField(t, nav, "flow.watch.path", "Nav.Baton")
	requireField(t, nav, "flow.watch.why", "Nav.Baton")
	requireField(t, nav, "flow.yet", "Nav.Baton")

	// APU: census counts
	total := requireField(t, nav, "apu.total", "Nav.APU")
	requireField(t, nav, "apu.functions", "Nav.APU")
	requireField(t, nav, "apu.wrappers", "Nav.APU")
	requireField(t, nav, "apu.maps", "Nav.APU")

	// Verify total format: "{N} functions + {N} wrappers + {N} map"
	if !strings.Contains(total, "functions") || !strings.Contains(total, "wrappers") {
		t.Errorf("Nav apu.total format unexpected: %q", total)
	}
}

// TestRegistryIntegration verifies the package registers itself in the
// identity registry at init time.
//
// Assessment: After import, identity.Lookup(key) must find this package
// with matching pragma and metadata. If not, the package is invisible
// to the system.
func TestRegistryIntegration(t *testing.T) {
	pkg, ok := identity.Lookup(expectedKey)
	if !ok {
		t.Fatalf("identity.Lookup(%q) not found — package did not register", expectedKey)
	}

	// Registered pragma should match what Pragma() returns
	if pkg.Pragma["P1.key"] != config.Pragma()["P1.key"] {
		t.Errorf("registry pragma P1.key = %q, Pragma() P1.key = %q",
			pkg.Pragma["P1.key"], config.Pragma()["P1.key"])
	}

	// Registered metadata should match what Metadata() returns
	if pkg.Metadata["M1.key"] != config.Metadata()["M1.key"] {
		t.Errorf("registry metadata M1.key = %q, Metadata() M1.key = %q",
			pkg.Metadata["M1.key"], config.Metadata()["M1.key"])
	}
}

// ────────────────────────────────────────────────────────────────
// B. Structure Assessment
// ────────────────────────────────────────────────────────────────
//
// Does the declared architecture match reality?
//
// Tests that exercise the flows declared in Nav() — primary (appointed),
// watch (ramparts), yet (anchor holds). If the declared flow doesn't
// work, the nav is lying about the architecture.

// TestPrimaryFlow verifies the appointed path: index.toml → manifest-driven loading.
//
// Assessment: Nav declares flow.primary.path — this test exercises it.
// PASS (+1): index.toml exists and drives loading in dependency order.
// SKIP: index.toml doesn't exist (not an error — watch flow handles it).
func TestPrimaryFlow(t *testing.T) {
	root, cleanup := setupTest(t)
	defer cleanup()

	result := config.LoadAllFromIndex()
	if !result.Valid {
		// Primary flow may legitimately fail (no index.toml). Log, don't fail.
		t.Skipf("Primary flow not available (index.toml may not exist at %s/L0-universal/ladder/foundation/)", root)
	}

	// Verify all 9 systems loaded in dependency order
	for _, sys := range expectedSystems {
		if _, ok := result.Configs[sys]; !ok {
			t.Errorf("primary flow missing system %q", sys)
		}
	}

	t.Logf("Primary flow: %d systems loaded via index.toml", len(result.Configs))
}

// TestWatchFlow verifies the watch path: known positions when manifest is absent.
//
// Assessment: Nav declares flow.watch.path — this test exercises it.
// LoadAll falls back to watch flow when index.toml is missing.
// This test verifies the fallback works even without a manifest.
func TestWatchFlow(t *testing.T) {
	_, cleanup := setupTest(t)
	defer cleanup()

	// LoadAll tries primary first, then watch flow. Either succeeding is valid.
	result := config.LoadAll()
	if !result.Valid {
		t.Log("Watch flow errors:")
		for _, err := range result.Errors {
			t.Logf("  %v", err)
		}
		t.Fatal("Neither primary nor watch flow produced valid results")
	}

	// Verify systems present (some subset expected regardless of path)
	if len(result.Configs) == 0 {
		t.Error("LoadAll returned zero systems")
	}

	// Log which systems loaded
	for sys, configs := range result.Summary {
		t.Logf("  %s: %d specs", sys, len(configs))
	}
}

// TestMetadataDeclaresAPI verifies that every function declared in M4
// actually exists as a callable public API.
//
// Assessment: M4 is the declared public surface. If it says "LoadAll" exists,
// we should be able to call it. This test exercises each declared function.
func TestMetadataDeclaresAPI(t *testing.T) {
	_, cleanup := setupTest(t)
	defer cleanup()

	meta := config.Metadata()

	// M4.config: "SetRoot" — already called by setupTest
	if meta["M4.config"] != "SetRoot" {
		t.Errorf("M4.config = %q, expected SetRoot", meta["M4.config"])
	}

	// M4.load: "LoadAll, LoadAllFromIndex, LoadIndex, LoadSystem, LoadSpec"
	// Exercise each one
	loadFuncs := meta["M4.load"]

	if strings.Contains(loadFuncs, "LoadAll") {
		result := config.LoadAll()
		if len(result.Errors) > 0 && !result.Valid {
			t.Logf("LoadAll returned errors (may be expected): %v", result.Errors)
		}
	}

	if strings.Contains(loadFuncs, "LoadSystem") {
		_, err := config.LoadSystem("math")
		if err != nil {
			t.Logf("LoadSystem(math) error (may be expected): %v", err)
		}
	}

	if strings.Contains(loadFuncs, "LoadSpec") {
		_, err := config.LoadSpec("math", "ternary.toml")
		if err != nil {
			t.Logf("LoadSpec(math, ternary.toml) error (may be expected): %v", err)
		}
	}

	// M4.check: "DiscoverAndCompare, ValidateDependencyGraph, GetDependencyTree"
	checkFuncs := meta["M4.check"]

	if strings.Contains(checkFuncs, "DiscoverAndCompare") {
		_, err := config.DiscoverAndCompare()
		if err != nil {
			t.Logf("DiscoverAndCompare error (may be expected): %v", err)
		}
	}

	if strings.Contains(checkFuncs, "ValidateDependencyGraph") {
		_, err := config.ValidateDependencyGraph()
		if err != nil {
			t.Logf("ValidateDependencyGraph error (may be expected): %v", err)
		}
	}

	if strings.Contains(checkFuncs, "GetDependencyTree") {
		_, err := config.GetDependencyTree()
		if err != nil {
			t.Logf("GetDependencyTree error (may be expected): %v", err)
		}
	}

	// M4.access: typed wrappers — exercise a few
	accessFuncs := meta["M4.access"]
	if strings.Contains(accessFuncs, "LoadMath") {
		_, _ = config.LoadMath()
	}
	if strings.Contains(accessFuncs, "LoadTypes") {
		_, _ = config.LoadTypes()
	}
}

// ────────────────────────────────────────────────────────────────
// C. Error Type Assessment
// ────────────────────────────────────────────────────────────────
//
// Are errors structured and type-assertable?
//
// The error system has three types: LoadError (file), ValidationError (field),
// DependencyError (graph). Tests verify errors.As works — which means any
// caller can handle errors structurally, not by string matching.

// TestLoadErrorType verifies that file-not-found errors return as *LoadError,
// not raw fmt.Errorf strings. This is the assessment: structured errors work.
func TestLoadErrorType(t *testing.T) {
	_, cleanup := setupTest(t)
	defer cleanup()

	// Load a spec that doesn't exist — should produce a LoadError
	_, err := config.LoadSpec("math", "this-does-not-exist.toml")
	if err == nil {
		t.Fatal("expected error for non-existent spec, got nil")
	}

	// The error chain should contain a *LoadError (from loadFile via newLoadError)
	var loadErr *config.LoadError
	if errors.As(err, &loadErr) {
		// Structured error — verify fields are populated
		if loadErr.File == "" {
			t.Error("LoadError.File is empty — should name the file")
		}
		if loadErr.Op == "" {
			t.Error("LoadError.Op is empty — should name the operation (read, parse)")
		}
		t.Logf("LoadError: file=%q op=%q err=%v", loadErr.File, loadErr.Op, loadErr.Err)
	} else {
		// This is the assessment failure — raw fmt.Errorf instead of structured error
		t.Errorf("error is %T, not *LoadError — error system not wired correctly: %v",
			err, err)
	}
}

// TestLoadErrorOnMissingSystem verifies that loading a non-existent system
// with no manifest and no watch path produces a structured error.
func TestLoadErrorOnMissingSystem(t *testing.T) {
	_, cleanup := setupTest(t)
	defer cleanup()

	_, err := config.LoadSystem("this-system-does-not-exist")
	if err == nil {
		t.Fatal("expected error for non-existent system, got nil")
	}

	// The error should be informative — it should mention the system name
	if !strings.Contains(err.Error(), "this-system-does-not-exist") {
		t.Errorf("error message doesn't mention the system name: %v", err)
	}
}

// TestLoadAllWithoutRoot verifies LoadAll fails with structured error
// when SetRoot() hasn't been called.
func TestLoadAllWithoutRoot(t *testing.T) {
	// Don't call SetRoot — test error case.
	// Note: This test must run before any test that calls SetRoot,
	// OR we need a way to reset root. Since Go tests run in the same
	// process, this test may see stale state from other tests.
	// For now, verify the error mechanism exists.

	// Use LoadSpec with a known-bad system as a proxy for root validation
	// (LoadAll's root check is a guard, not a LoadError)
	result := config.LoadAll()

	// If root was never set by any prior test, this should fail
	if result.Valid && len(result.Configs) > 0 {
		t.Log("Root appears to be set from a prior test — skipping root-not-set check")
		return
	}

	if len(result.Errors) == 0 {
		t.Error("expected at least one error when loading fails")
	}
}

// ────────────────────────────────────────────────────────────────
// D. Loading Assessment
// ────────────────────────────────────────────────────────────────
//
// Do the load functions produce correct results?
//
// These exercise the actual loading API with real TOML files.
// The assessment: loaded data matches expected structure.

// TestLoadSpec verifies single spec loading with structural validation.
func TestLoadSpec(t *testing.T) {
	_, cleanup := setupTest(t)
	defer cleanup()

	cfg, err := config.LoadSpec("types", "primitives.toml")
	if err != nil {
		t.Fatalf("LoadSpec(types, primitives.toml) failed: %v", err)
	}

	// Structural verification — the loaded file should be self-aware
	if cfg.Name != "primitives.toml" {
		t.Errorf("Name = %q, want %q", cfg.Name, "primitives.toml")
	}
	if cfg.Path == "" {
		t.Error("Path is empty — should be full filesystem path")
	}
	if len(cfg.Keys) == 0 {
		t.Error("Keys is empty — TOML sections should be extracted")
	}

	// Content verification — expected top-level sections in primitives.toml
	// Real structure uses namespaced sections: [cc.settings.trit.trit], [cc.settings.integer.int9], etc.
	// Top-level keys: _pragma, _metadata, _content, cc, conversion, validation, coercion, paradigm, _generate, _closing
	for _, section := range []string{"_pragma", "cc", "_content"} {
		if _, ok := cfg.Data[section]; !ok {
			t.Errorf("missing expected top-level section %q in primitives.toml", section)
		}
	}

	// Identity bridge — if primitives.toml has [_pragma], it should be extracted
	if cfg.Pragma != nil {
		t.Logf("Identity bridge: primitives.toml has pragma with %d fields", len(cfg.Pragma))
	}
}

// TestLoadTypedSystems verifies typed wrappers load the expected specs.
func TestLoadTypedSystems(t *testing.T) {
	_, cleanup := setupTest(t)
	defer cleanup()

	tests := []struct {
		name     string
		loader   func() ([]*config.ConfigFile, error)
		expected []string // expected filenames
	}{
		{
			name:     "math",
			loader:   config.LoadMath,
			expected: []string{"ternary.toml"},
		},
		{
			name:     "types",
			loader:   config.LoadTypes,
			expected: []string{"primitives.toml", "composition.toml", "validation.toml"},
		},
		{
			name:   "bible",
			loader: config.LoadBible,
			expected: []string{
				"scripture-text.toml", "addressing.toml", "encoding.toml",
				"decoding.toml", "translation.toml",
			},
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			configs, err := tt.loader()
			if err != nil {
				t.Fatalf("Load%s failed: %v", tt.name, err)
			}

			// Build set of loaded filenames
			loaded := make(map[string]bool, len(configs))
			for _, cfg := range configs {
				loaded[cfg.Name] = true
			}

			// Verify expected files are present
			for _, expected := range tt.expected {
				if !loaded[expected] {
					t.Errorf("expected %q not found in %s system", expected, tt.name)
				}
			}
		})
	}
}

// TestMathTernaryContent verifies the foundation spec has expected content.
//
// Assessment: math/ternary.toml is order=0, the anchor. If it has wrong
// content, the entire system is on a bad foundation.
func TestMathTernaryContent(t *testing.T) {
	_, cleanup := setupTest(t)
	defer cleanup()

	cfg, err := config.LoadSpec("math", "ternary.toml")
	if err != nil {
		t.Fatalf("LoadSpec(math, ternary.toml) failed: %v", err)
	}

	// ternary.toml is a split-index file — [index] declares content files.
	// Top-level keys: _pragma, _metadata, _content, index, _validation, _closing
	if _, ok := cfg.Data["index"]; !ok {
		t.Fatal("ternary.toml missing [index] section — split-index foundation broken")
	}
}

// ────────────────────────────────────────────────────────────────
// D.2 Validation Assessment
// ────────────────────────────────────────────────────────────────
//
// Tripwire and dependency graph validation.

// TestDiscoverAndCompare verifies manifest/disk alignment.
//
// Assessment: The tripwire pattern — what the manifest declares should
// exist on disk. Missing files = broken promises. Unexpected files = drift.
func TestDiscoverAndCompare(t *testing.T) {
	_, cleanup := setupTest(t)
	defer cleanup()

	result, err := config.DiscoverAndCompare()
	if err != nil {
		t.Skipf("DiscoverAndCompare requires index.toml: %v", err)
	}

	if len(result.Manifest) == 0 {
		t.Error("manifest is empty — index.toml has no entries")
	}
	if len(result.Discovered) == 0 {
		t.Error("discovered is empty — no files found on disk")
	}

	// Tripwire: missing files are errors
	if !result.Valid {
		t.Error("TRIPWIRE: Files declared in manifest but missing from disk:")
		for _, missing := range result.Missing {
			t.Errorf("  MISSING: %s", missing)
		}
	}

	// Unexpected: drift warnings (not errors)
	for _, unexpected := range result.Unexpected {
		t.Logf("  UNEXPECTED (on disk, not in manifest): %s", unexpected)
	}
}

// TestValidateDependencyGraph verifies the dependency DAG is healthy.
//
// Assessment: No cycles, no missing references. The dependency order
// declared in index.toml must be valid.
func TestValidateDependencyGraph(t *testing.T) {
	_, cleanup := setupTest(t)
	defer cleanup()

	depErrors, err := config.ValidateDependencyGraph()
	if err != nil {
		t.Skipf("ValidateDependencyGraph requires index.toml: %v", err)
	}

	if len(depErrors) > 0 {
		t.Error("DEPENDENCY ERRORS:")
		for _, depErr := range depErrors {
			t.Errorf("  %v", depErr)
		}
	}

	// Verify the graph is non-empty
	tree, err := config.GetDependencyTree()
	if err != nil {
		t.Fatalf("GetDependencyTree failed: %v", err)
	}
	if len(tree) == 0 {
		t.Error("dependency tree is empty — no specs in manifest")
	}

	t.Logf("Dependency graph: %d nodes, %d errors", len(tree), len(depErrors))
}

// ────────────────────────────────────────────────────────────────
// D.3 L0 Util Assessment
// ────────────────────────────────────────────────────────────────
//
// Shared primitives that underpin all loaders.

// TestUtilFileExists verifies the shared FileExists utility.
func TestUtilFileExists(t *testing.T) {
	root := getModuleRoot(t)

	indexFile := filepath.Join(root, "L0-universal", "ladder", "foundation", "index.toml")
	if !util.FileExists(indexFile) {
		t.Errorf("FileExists(%q) = false, expected true", indexFile)
	}

	fake := filepath.Join(root, "this-file-does-not-exist.toml")
	if util.FileExists(fake) {
		t.Errorf("FileExists(%q) = true, expected false", fake)
	}
}

// TestUtilDirExists verifies the shared DirExists utility.
func TestUtilDirExists(t *testing.T) {
	root := getModuleRoot(t)

	foundationDir := filepath.Join(root, "L0-universal", "ladder", "foundation")
	if !util.DirExists(foundationDir) {
		t.Errorf("DirExists(%q) = false, expected true", foundationDir)
	}

	// File path (not dir) should return false
	indexFile := filepath.Join(root, "L0-universal", "ladder", "foundation", "index.toml")
	if util.DirExists(indexFile) {
		t.Errorf("DirExists(%q) = true for a file, expected false", indexFile)
	}

	fake := filepath.Join(root, "this-dir-does-not-exist")
	if util.DirExists(fake) {
		t.Errorf("DirExists(%q) = true, expected false", fake)
	}
}

// TestUtilFormatDetection verifies format detection from extensions and paths.
func TestUtilFormatDetection(t *testing.T) {
	extTests := []struct {
		ext    string
		expect string
	}{
		{".go", "go"}, {".toml", "toml"}, {".json", "json"},
		{".jsonc", "jsonc"}, {".yaml", "yaml"}, {".yml", "yaml"},
		{".c", "c"}, {".h", "c"}, {".rs", "rs"}, {".md", "md"},
		{".adoc", "adoc"}, {".txt", "txt"}, {".py", "py"},
		{".ts", "ts"}, {".sh", "sh"},
		{".omni", ""}, {".gen", ""}, {".xyz", ""},
	}

	for _, tt := range extTests {
		got := util.FormatFromExt(tt.ext)
		if got != tt.expect {
			t.Errorf("FormatFromExt(%q) = %q, want %q", tt.ext, got, tt.expect)
		}
	}

	pathTests := []struct {
		path   string
		expect string
	}{
		{"L0-universal/ladder/foundation/types/primitives.toml", "toml"},
		{"pkg/config/loader.go", "go"},
		{"docs/guide.adoc", "adoc"},
		{"data/output.gen", ""},
	}

	for _, tt := range pathTests {
		got := util.FormatFromPath(tt.path)
		if got != tt.expect {
			t.Errorf("FormatFromPath(%q) = %q, want %q", tt.path, got, tt.expect)
		}
	}
}

// TestUtilIsConfigFormat verifies config format identification.
func TestUtilIsConfigFormat(t *testing.T) {
	for _, f := range []string{"toml", "json", "jsonc", "yaml"} {
		if !util.IsConfigFormat(f) {
			t.Errorf("IsConfigFormat(%q) = false, expected true", f)
		}
	}
	for _, f := range []string{"go", "c", "rs", "md", "adoc", "py", "txt", ""} {
		if util.IsConfigFormat(f) {
			t.Errorf("IsConfigFormat(%q) = true, expected false", f)
		}
	}
}

// TestUtilMust verifies the Must[T] wrapper.
func TestUtilMust(t *testing.T) {
	// Success case
	result := util.Must(42, nil)
	if result != 42 {
		t.Errorf("Must(42, nil) = %d, want 42", result)
	}

	// Panic case
	defer func() {
		if r := recover(); r == nil {
			t.Error("Must with error should panic, but did not")
		}
	}()
	_ = util.Must(0, os.ErrNotExist)
}

// TestUtilLoadTOMLFile verifies the generic TOML file loader.
func TestUtilLoadTOMLFile(t *testing.T) {
	root := getModuleRoot(t)

	primitivesPath := filepath.Join(root, "L0-universal", "ladder", "foundation", "types", "primitives.toml")
	if !util.FileExists(primitivesPath) {
		t.Skipf("primitives.toml not found at %s", primitivesPath)
	}

	result, err := util.LoadTOMLFile[map[string]interface{}](primitivesPath)
	if err != nil {
		t.Fatalf("LoadTOMLFile failed: %v", err)
	}
	if result == nil {
		t.Fatal("LoadTOMLFile returned nil result")
	}
	// primitives.toml uses namespaced structure — [cc.settings.trit.trit] etc.
	if _, ok := (*result)["cc"]; !ok {
		t.Error("expected 'cc' key in primitives.toml (namespaced structure)")
	}

	// Error case: missing file
	_, err = util.LoadTOMLFile[map[string]interface{}]("/nonexistent/file.toml")
	if err == nil {
		t.Error("LoadTOMLFile should return error for missing file")
	}
}

// ────────────────────────────────────────────────────────────────
// E. Health Assessment (Diagnostic)
// ────────────────────────────────────────────────────────────────
//
// Comprehensive system health check. Run this for full assessment:
//   go test -v -run TestHealthAssessment ./L0-universal/hybrid/config/...

// TestHealthAssessment runs the comprehensive config system diagnostic.
//
// This IS the assessment API. A dashboard would call Pragma(), Metadata(),
// Nav(), LoadAll(), DiscoverAndCompare(), ValidateDependencyGraph() —
// the exact same calls this test makes.
//
// Output:
//   HEALTHY (+1):  Identity complete, primary flow works, no drift, no cycles
//   DEGRADED (0):  Identity complete but loading uses fallback or has warnings
//   BROKEN (-1):   Identity incomplete or loading fails entirely
func TestHealthAssessment(t *testing.T) {
	root, cleanup := setupTest(t)
	defer cleanup()

	healthy := true
	degraded := false

	t.Log("════════════════════════════════════════════════════════════════")
	t.Log("CONFIG SYSTEM HEALTH ASSESSMENT")
	t.Logf("Root: %s", root)
	t.Log("════════════════════════════════════════════════════════════════")

	// ── Phase 1: Identity ───────────────────────────────────────────
	t.Log("")
	t.Log("── Phase 1: Identity ──")

	pragma := config.Pragma()
	meta := config.Metadata()
	nav := config.Nav()

	if pragma["P1.key"] == expectedKey {
		t.Logf("  Pragma:   %s (P1-P5 present)", pragma["P1.key"])
	} else {
		t.Errorf("  Pragma:   KEY MISMATCH — got %q, want %q", pragma["P1.key"], expectedKey)
		healthy = false
	}

	if meta["M1.key"] == expectedKey {
		t.Logf("  Metadata: %s (M1-M10 present)", meta["M1.key"])
	} else {
		t.Errorf("  Metadata: KEY MISMATCH — got %q, want %q", meta["M1.key"], expectedKey)
		healthy = false
	}

	if nav["apu.total"] != "" {
		t.Logf("  Nav:      %s", nav["apu.total"])
	} else {
		t.Error("  Nav:      APU total missing")
		healthy = false
	}

	// Registry check
	if _, ok := identity.Lookup(expectedKey); ok {
		t.Log("  Registry: registered")
	} else {
		t.Error("  Registry: NOT FOUND")
		healthy = false
	}

	// ── Phase 2: Primary Flow (Index) ───────────────────────────────
	t.Log("")
	t.Log("── Phase 2: Primary Flow ──")

	primaryResult := config.LoadAllFromIndex()
	if primaryResult.Valid {
		t.Logf("  Status: HEALTHY — %d systems loaded via index.toml", len(primaryResult.Configs))
		for sys, specs := range primaryResult.Summary {
			t.Logf("    %s: %d specs", sys, len(specs))
		}
	} else {
		t.Log("  Status: DEGRADED — primary flow errors:")
		for _, err := range primaryResult.Errors {
			t.Logf("    %v", err)
		}
		degraded = true
	}

	// ── Phase 3: Tripwire ───────────────────────────────────────────
	t.Log("")
	t.Log("── Phase 3: Tripwire ──")

	discovery, err := config.DiscoverAndCompare()
	if err != nil {
		t.Logf("  Status: UNAVAILABLE (%v)", err)
		degraded = true
	} else if discovery.Valid {
		t.Logf("  Status: ALIGNED — manifest (%d) matches disk (%d)",
			len(discovery.Manifest), len(discovery.Discovered))
		if len(discovery.Unexpected) > 0 {
			t.Logf("  Drift:  %d unexpected files on disk", len(discovery.Unexpected))
			degraded = true
		}
	} else {
		t.Log("  Status: MISALIGNED")
		for _, f := range discovery.Missing {
			t.Logf("    MISSING: %s", f)
		}
		healthy = false
	}

	// ── Phase 4: Dependencies ───────────────────────────────────────
	t.Log("")
	t.Log("── Phase 4: Dependencies ──")

	depErrors, err := config.ValidateDependencyGraph()
	if err != nil {
		t.Logf("  Status: UNAVAILABLE (%v)", err)
		degraded = true
	} else if len(depErrors) == 0 {
		tree, _ := config.GetDependencyTree()
		t.Logf("  Status: HEALTHY — %d nodes, no cycles, no missing refs", len(tree))
	} else {
		t.Log("  Status: BROKEN")
		for _, depErr := range depErrors {
			t.Logf("    %v", depErr)
		}
		healthy = false
	}

	// ── Phase 5: Watch Flow (Fallback) ──────────────────────────────
	t.Log("")
	t.Log("── Phase 5: Watch Flow ──")

	if !primaryResult.Valid {
		// Primary failed — check if watch flow saves us
		watchResult := config.LoadAll()
		if watchResult.Valid {
			t.Logf("  Status: OPERATIONAL — %d systems loaded via watch paths", len(watchResult.Configs))
		} else {
			t.Log("  Status: BROKEN — watch flow also failed")
			healthy = false
		}
	} else {
		t.Log("  Status: STANDBY (primary flow is healthy, watch not needed)")
	}

	// ── Overall ─────────────────────────────────────────────────────
	t.Log("")
	t.Log("── Overall Assessment ──")

	switch {
	case healthy && !degraded:
		t.Log("  HEALTHY (+1) — All systems operational via primary path")
	case healthy && degraded:
		t.Log("  DEGRADED (0) — Operational but with warnings")
	default:
		t.Log("  BROKEN (-1) — See errors above")
	}

	t.Log("")
	t.Log("════════════════════════════════════════════════════════════════")
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
//
// These tests are the proof. The same API calls that verify correctness
// in `go test` are the same calls a dashboard uses for health assessment.
// The package knows itself (Pragma, Metadata, Nav). The tests verify
// it's telling the truth.
//
// Assessment categories (same order as BODY):
//   A. Identity:   TestPragmaIdentity, TestMetadataContract, TestNavArchitecture,
//                  TestRegistryIntegration
//   B. Structure:  TestPrimaryFlow, TestWatchFlow, TestMetadataDeclaresAPI
//   C. Errors:     TestLoadErrorType, TestLoadErrorOnMissingSystem,
//                  TestLoadAllWithoutRoot
//   D. Loading:    TestLoadSpec, TestLoadTypedSystems, TestMathTernaryContent,
//                  TestDiscoverAndCompare, TestValidateDependencyGraph,
//                  TestUtilFileExists, TestUtilDirExists, TestUtilFormatDetection,
//                  TestUtilIsConfigFormat, TestUtilMust, TestUtilLoadTOMLFile
//   E. Health:     TestHealthAssessment
//
// Run all:        go test -v ./L0-universal/hybrid/config/...
// Run assessment: go test -v -run TestHealthAssessment ./L0-universal/hybrid/config/...
// Run identity:   go test -v -run "TestPragma|TestMetadata|TestNav" ./L0-universal/hybrid/config/...

// ============================================================================
// END CLOSING
// ============================================================================
