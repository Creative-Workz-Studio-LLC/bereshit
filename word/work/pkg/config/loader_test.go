// #!omni code --go -test
// ═══════════════════════════════════════════════════════════════════════════
// Config Loader Tests
// Key: B-word-work-pkg-config-loader-test
// ═══════════════════════════════════════════════════════════════════════════
//
// derives_from: bereshit/word/seed/code/go/demo-test.go
// See: standards/code/4-block/
//
// ═══════════════════════════════════════════════════════════════════════════

// Package config_test provides unit tests for the config loader.
//
// # Core Identity
//
//	Key:     B-word-work-pkg-config-loader-test
//	Type:    Ladder (validates foundation Phase 3+ builds upon)
//	Version: a-01.50 (2025-12-14)
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
//   - a-01.50 (2025-12-14) — PhD rigor + Bible accessibility refinement
//   - a-01.00 (2025-12-13) — Initial test file, 10 test functions
//
// # Interface
//
//	Needs:   testing, os, path/filepath | pkg/config (under test)
//	Tests:   loader.go — SetRoot, LoadAll, LoadSystem, LoadSpec, validation
//	Run:     BERESHIT_ROOT=/path go test -v ./pkg/config/...
//
// # Test Functions
//
// Setup Validation:
//
//	TestLoadAllWithoutRoot                           Error without SetRoot
//
// Primary Loading:
//
//	TestLoadAll                                      All systems via index
//	TestLoadSpec                                     Single spec by name
//
// Typed Loaders:
//
//	TestLoadTypes, TestLoadMath, TestLoadBible       System-specific loading
//
// Validation:
//
//	TestDiscoverAndCompare                           Manifest vs disk
//	TestValidateDependencyGraph                      Dependency tree check
//
// # Operational
//
//	Blocking: No — file I/O only
//	Health:   Test file (validates health, not self-tracking)
package config_test

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Imports → Types → Type Methods → Constants → Variables → Package-Level State
// See: bereshit/word/seed/code/go/demo-test.go > SETUP

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

//--- Standard Library ---
// Test framework, path operations
import (
	"os"            // Environment variable for bereshit root
	"path/filepath" // Path construction
	"testing"       // Test framework
)

//--- External Packages ---
// [Reserved: No external dependencies for tests]

//--- Internal Packages ---
// Package under test
import (
	"creativeworkzstudio.com/bereshit/word/work/pkg/config"
)

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Uses types from config package — no custom test types needed]

// ────────────────────────────────────────────────────────────────
// Type Methods
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No custom types defined above]

// ────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Uses BERESHIT_ROOT env var — no hardcoded paths]

// ────────────────────────────────────────────────────────────────
// Variables
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Test functions are self-contained — no package-level state]

// ────────────────────────────────────────────────────────────────
// Package-Level State (Rails Pattern)
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Test file — no Rails infrastructure needed]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: Org Chart → Helpers → Core Operations → Error Handling → Public APIs
// See: bereshit/word/seed/code/go/demo-test.go > BODY

// ────────────────────────────────────────────────────────────────
// Organizational Chart
// ────────────────────────────────────────────────────────────────
//
// Navigation map for BODY. When lost in tests, return here.
// Shows WHERE tests live (layers), HOW they connect (flow), WHAT exists (count).
//
// Subsections: Ladder Structure, Baton Flow, APU Inventory

//--- Ladder Structure ---
// Vertical dependency layers. Bottom supports top. Read bottom-up for foundations.
//
//   Public APIs (Top Rungs - Test Functions)
//   ├── Setup Validation:
//   │   └── TestLoadAllWithoutRoot()    → error without SetRoot
//   ├── Primary Loading:
//   │   ├── TestLoadAll()               → all systems via index
//   │   └── TestLoadSpec()              → single spec by name
//   ├── Typed Loaders:
//   │   ├── TestLoadTypes()             → types system
//   │   ├── TestLoadMath()              → math system
//   │   └── TestLoadBible()             → bible system
//   └── Validation:
//       ├── TestDiscoverAndCompare()    → manifest vs disk
//       └── TestValidateDependencyGraph() → dependency tree
//
//   Helpers (Bottom Rungs - Foundations)
//   └── getBereshitRoot() → derives root from env var or file location

//--- Baton Flow ---
// Horizontal execution path. Entry → processing → Exit.
//
//   go test → Test*()
//     ↓
//   getBereshitRoot() → config.SetRoot()
//     ↓
//   config.Load*() or config.Validate*()
//     ↓
//   verify result → Exit (pass/fail)

//--- APU Inventory ---
// Count of Available Processing Units by category.
//
//   Total: 11 functions
//   ├── Helpers: 2 functions
//   │   ├── getBereshitRoot()   → derives bereshit root
//   │   └── setupTest()         → test setup with cleanup (CLOSING)
//   └── Test Functions: 9 tests
//       ├── Setup: TestLoadAllWithoutRoot
//       ├── Loading: TestLoadAll, TestLoadSpec, TestLoadTypes,
//       │            TestLoadMath, TestLoadBible
//       ├── Validation: TestDiscoverAndCompare, TestValidateDependencyGraph
//       └── Diagnostic: TestDiagnostic (CLOSING — comprehensive health check)

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities - Internal Support
// ────────────────────────────────────────────────────────────────
//
// Foundation layer for tests. Test functions depend on these.
// See: Org Chart > Ladder Structure > Helpers
//
// Subsections: Setup Helpers

//--- Setup Helpers ---
// Test setup utilities. Called at start of each test.

// getBereshitRoot derives the bereshit root path for tests.
//
// Parameters:
//   - t: test context for fatal errors
//
// Returns:
//   - string: absolute path to bereshit repository root
//
// Resolution order:
//  1. BERESHIT_ROOT env var (explicit override)
//  2. Derived from test file location (4 levels up)
func getBereshitRoot(t *testing.T) string {
	t.Helper()

	// Primary: explicit environment variable
	if root := os.Getenv("BERESHIT_ROOT"); root != "" {
		return root
	}

	// Fallback: derive from this file's location
	// Path: bereshit/word/work/pkg/config/loader_test.go
	// Root is 4 directories up: config → pkg → work → word → bereshit
	wd, err := os.Getwd()
	if err != nil {
		t.Fatalf("failed to get working directory: %v", err)
	}

	root := filepath.Join(wd, "..", "..", "..", "..")
	absRoot, err := filepath.Abs(root)
	if err != nil {
		t.Fatalf("failed to get absolute path: %v", err)
	}

	return absRoot
}

// ────────────────────────────────────────────────────────────────
// Core Operations - Test Infrastructure
// ────────────────────────────────────────────────────────────────
//
// Middle rung. Depends on Helpers below, used by Test Functions above.
// See: Org Chart > Ladder Structure (tests have flat structure currently)
//
// Subsections: Test Fixtures, Table-Driven Data
//
// [Reserved: Tests consume config package directly — no complex fixtures needed]
// Future: Add table-driven test data, shared assertions, mock manifests

// ────────────────────────────────────────────────────────────────
// Error Handling - Test Assertions
// ────────────────────────────────────────────────────────────────
//
// Test failure patterns. How tests report issues.
// See: Org Chart > Test Functions for which tests use which assertions
//
// Subsections: Assertion Helpers, Cleanup Patterns
//
// [Reserved: Uses t.Fatal/t.Error directly — no custom assertion helpers needed]
// Future: Add assertLoadResult, assertTripwireWarning, assertSystemPresent

// ────────────────────────────────────────────────────────────────
// Test Functions - Public APIs
// ────────────────────────────────────────────────────────────────
//
// Top rung. Each test verifies a specific config loader function.
// See: Org Chart > Ladder Structure > Public APIs
//
// Subsections: Setup Validation, Primary Loading, Typed Loaders, Validation Tests
//
// TRIPWIRE-AWARE TESTING:
//   Tests should distinguish between:
//   - PASS:      Loaded via index.toml (primary path)
//   - HALF PASS: Loaded via fallback (tripwire triggered) — functional but degraded
//   - FAIL:      Could not load at all
//
//   Current tests only check Valid — need to evolve to detect fallback mode.

//--- Setup Validation ---
// Tests that verify preconditions and error handling.

// TestLoadAllWithoutRoot verifies LoadAll fails when SetRoot not called.
func TestLoadAllWithoutRoot(t *testing.T) {
	// Don't call SetRoot - test error case
	result := config.LoadAll()

	if result.Valid {
		t.Error("LoadAll should fail when root not set")
	}

	if len(result.Errors) == 0 {
		t.Error("LoadAll should return error when root not set")
	}
}

//--- Primary Loading ---
// Tests for main loading entry points.

// TestLoadAll verifies all configs load successfully from bereshit root.
//
// TRIPWIRE-AWARE: Tests primary path first, reports which path succeeded.
//   - PASS (+1):      LoadAllFromIndex() succeeds (index.toml works)
//   - HALF PASS (0):  LoadAllFromIndex() fails but LoadAll() succeeds (fallback)
//   - FAIL (-1):      Neither path works
func TestLoadAll(t *testing.T) {
	root := getBereshitRoot(t)
	config.SetRoot(root)

	// Expected systems (from index.toml manifest)
	expectedSystems := []string{"math", "types", "language", "bible", "health", "permission", "identity", "network", "filesystem"}

	// ═══════════════════════════════════════════════════════════════════════
	// TRIPWIRE CHECK: Try primary path first (index.toml)
	// ═══════════════════════════════════════════════════════════════════════
	primaryResult := config.LoadAllFromIndex()

	if primaryResult.Valid {
		// PASS (+1): Primary path works — index.toml is single source of truth
		t.Log("✓ PASS: Loaded via index.toml (primary path)")

		// Verify all systems present
		for _, sys := range expectedSystems {
			if _, ok := primaryResult.Configs[sys]; !ok {
				t.Errorf("expected system %q in result.Configs", sys)
			}
		}
		return // Primary succeeded, test complete
	}

	// Primary failed — check WHY before trying fallback
	t.Log("⚠ Primary path (index.toml) failed:")
	for _, err := range primaryResult.Errors {
		t.Logf("  - %v", err)
	}

	// ═══════════════════════════════════════════════════════════════════════
	// FALLBACK CHECK: Try LoadAll() which includes fallback logic
	// ═══════════════════════════════════════════════════════════════════════
	fallbackResult := config.LoadAll()

	if fallbackResult.Valid {
		// HALF PASS (0): Fallback works but primary doesn't — degraded state
		t.Log("⚠ HALF PASS: Loaded via fallback (tripwire triggered)")
		t.Log("  Action: Create/fix word/core/index.toml to restore primary path")

		// Verify all systems present even in fallback
		for _, sys := range expectedSystems {
			if _, ok := fallbackResult.Configs[sys]; !ok {
				t.Errorf("expected system %q in fallback result", sys)
			}
		}
		return // Fallback succeeded, test passes with warning
	}

	// FAIL (-1): Neither path works
	t.Log("✗ FAIL: Neither primary nor fallback path works")
	for _, err := range fallbackResult.Errors {
		t.Errorf("LoadAll error: %v", err)
	}
	t.Fatal("LoadAll failed - see errors above")
}

// TestLoadSpec verifies single spec loading works correctly.
//
// NOTE: LoadSpec has internal tripwire — uses index.toml if available,
// falls back to hardcoded paths if not. Test passes on either path.
// See TestLoadAll for explicit tripwire detection.
func TestLoadSpec(t *testing.T) {
	root := getBereshitRoot(t)
	config.SetRoot(root)

	cfg, err := config.LoadSpec("types", "primitives.toml")
	if err != nil {
		t.Fatalf("LoadSpec failed: %v", err)
	}

	// Verify structural correctness
	if cfg.Name != "primitives.toml" {
		t.Errorf("expected Name 'primitives.toml', got %q", cfg.Name)
	}

	if cfg.Path == "" {
		t.Error("expected Path to be populated")
	}

	if len(cfg.Keys) == 0 {
		t.Error("expected Keys to be populated")
	}

	// Verify expected content from primitives.toml
	// These sections should exist in word/core/types/primitives.toml
	expectedSections := []string{"trit", "trit5", "int9", "int27"}
	for _, section := range expectedSections {
		if _, ok := cfg.Data[section]; !ok {
			t.Errorf("expected section %q in primitives.toml", section)
		}
	}
}

//--- Typed Loaders ---
// Tests for system-specific loading functions.

// TestLoadTypes verifies types system loads correctly.
//
// NOTE: Has internal tripwire. See TestLoadAll for tripwire detection.
func TestLoadTypes(t *testing.T) {
	root := getBereshitRoot(t)
	config.SetRoot(root)

	configs, err := config.LoadTypes()
	if err != nil {
		t.Fatalf("LoadTypes failed: %v", err)
	}

	// Expected files from index.toml: primitives.toml, composition.toml, validation.toml
	expectedFiles := map[string]bool{
		"primitives.toml":  false,
		"composition.toml": false,
		"validation.toml":  false,
	}

	for _, cfg := range configs {
		if _, expected := expectedFiles[cfg.Name]; expected {
			expectedFiles[cfg.Name] = true
		}
	}

	for file, found := range expectedFiles {
		if !found {
			t.Errorf("expected types spec %q not found", file)
		}
	}
}

// TestLoadMath verifies math system loads correctly.
//
// NOTE: Has internal tripwire. See TestLoadAll for tripwire detection.
func TestLoadMath(t *testing.T) {
	root := getBereshitRoot(t)
	config.SetRoot(root)

	configs, err := config.LoadMath()
	if err != nil {
		t.Fatalf("LoadMath failed: %v", err)
	}

	// Expected: ternary.toml (foundation anchor, order=0)
	found := false
	for _, cfg := range configs {
		if cfg.Name == "ternary.toml" {
			found = true
			// Verify it has expected content
			if _, ok := cfg.Data["trit"]; !ok {
				t.Error("ternary.toml missing [trit] section")
			}
			break
		}
	}
	if !found {
		t.Error("expected ternary.toml in math system")
	}
}

// TestLoadBible verifies bible system loads correctly.
//
// NOTE: Has internal tripwire. See TestLoadAll for tripwire detection.
func TestLoadBible(t *testing.T) {
	root := getBereshitRoot(t)
	config.SetRoot(root)

	configs, err := config.LoadBible()
	if err != nil {
		t.Fatalf("LoadBible failed: %v", err)
	}

	// Expected files from index.toml (5 specs)
	expectedFiles := map[string]bool{
		"scripture-text.toml": false,
		"addressing.toml":     false,
		"encoding.toml":       false,
		"decoding.toml":       false,
		"translation.toml":    false,
	}

	for _, cfg := range configs {
		if _, expected := expectedFiles[cfg.Name]; expected {
			expectedFiles[cfg.Name] = true
		}
	}

	for file, found := range expectedFiles {
		if !found {
			t.Errorf("expected bible spec %q not found", file)
		}
	}
}

//--- Validation Tests ---
// Tests for tripwire and dependency validation.

// TestDiscoverAndCompare verifies manifest/disk comparison.
//
// This IS the tripwire — compares what index.toml declares vs what's on disk.
// REQUIRES index.toml (no fallback). If index.toml missing, test fails.
func TestDiscoverAndCompare(t *testing.T) {
	root := getBereshitRoot(t)
	config.SetRoot(root)

	result, err := config.DiscoverAndCompare()
	if err != nil {
		t.Fatalf("DiscoverAndCompare failed: %v", err)
	}

	// Verify discovered files exist
	if len(result.Discovered) == 0 {
		t.Error("expected discovered files on disk")
	}

	// Verify manifest files exist
	if len(result.Manifest) == 0 {
		t.Error("expected manifest files from index.toml")
	}

	// Tripwire check: no files should be missing
	if !result.Valid {
		t.Error("TRIPWIRE: Files declared in manifest but missing from disk:")
		for _, missing := range result.Missing {
			t.Errorf("  - %s", missing)
		}
	}

	// Warning check: unexpected files (on disk but not in manifest)
	if len(result.Unexpected) > 0 {
		t.Log("WARNING: Files on disk but not in manifest:")
		for _, unexpected := range result.Unexpected {
			t.Logf("  - %s", unexpected)
		}
	}
}

// TestValidateDependencyGraph verifies dependency validation.
//
// Validates the dependency DAG: no cycles, no missing references.
// REQUIRES index.toml (no fallback). If index.toml missing, test fails.
func TestValidateDependencyGraph(t *testing.T) {
	root := getBereshitRoot(t)
	config.SetRoot(root)

	depErrors, err := config.ValidateDependencyGraph()
	if err != nil {
		t.Fatalf("ValidateDependencyGraph failed: %v", err)
	}

	// Should have no dependency errors
	if len(depErrors) > 0 {
		t.Error("DEPENDENCY ERRORS:")
		for _, depErr := range depErrors {
			t.Errorf("  - %v", depErr)
		}
	}

	// Verify dependency tree is accessible
	tree, treeErr := config.GetDependencyTree()
	if treeErr != nil {
		t.Fatalf("GetDependencyTree failed: %v", treeErr)
	}
	if len(tree) == 0 {
		t.Error("expected non-empty dependency tree")
	}
}


// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// Section order: [GROUP 1: CODING] Validation → Execution → Cleanup →
//                [GROUP 2: DOCUMENTATION] Overview → Policy → Troubleshooting → Quick Reference
// See: bereshit/word/seed/code/go/demo-test.go > CLOSING

// ════════════════════════════════════════════════════════════════
// GROUP 1: CODING - Verify, Execute, Clean
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// Code Validation - Diagnostic API
// ────────────────────────────────────────────────────────────────
//
// Comprehensive diagnostic test. Run this to check config system health.
// Produces detailed output showing system state.
//
// Usage: go test -v -run TestDiagnostic ./pkg/config/...

// TestDiagnostic runs comprehensive config system diagnostics.
//
// This is the DIAGNOSTIC API - not just a test, but a health check tool.
// Outputs detailed state information for debugging config issues.
//
// Diagnostic Output:
//   - PASS (+1):      All systems healthy, primary path working
//   - HALF PASS (0):  Systems functional but using fallback (tripwire)
//   - FAIL (-1):      System broken, see errors
func TestDiagnostic(t *testing.T) {
	t.Log("════════════════════════════════════════════════════════════════")
	t.Log("CONFIG SYSTEM DIAGNOSTIC")
	t.Log("════════════════════════════════════════════════════════════════")

	root, cleanup := setupTest(t)
	defer cleanup()
	t.Logf("Bereshit Root: %s", root)

	// ─────────────────────────────────────────────────────────────────
	// PHASE 1: Index Health (Primary Path)
	// ─────────────────────────────────────────────────────────────────
	t.Log("")
	t.Log("── PHASE 1: Index Health ──")

	primaryResult := config.LoadAllFromIndex()
	if primaryResult.Valid {
		t.Log("✓ index.toml: HEALTHY (primary path working)")
		t.Logf("  Systems loaded: %d", len(primaryResult.Configs))
		for sys, configs := range primaryResult.Summary {
			t.Logf("    %s: %d specs", sys, len(configs))
		}
	} else {
		t.Log("⚠ index.toml: DEGRADED (errors encountered)")
		for _, err := range primaryResult.Errors {
			t.Logf("    ERROR: %v", err)
		}
	}

	// ─────────────────────────────────────────────────────────────────
	// PHASE 2: Tripwire Check (Manifest vs Disk)
	// ─────────────────────────────────────────────────────────────────
	t.Log("")
	t.Log("── PHASE 2: Tripwire Check ──")

	discovery, err := config.DiscoverAndCompare()
	if err != nil {
		t.Logf("✗ Tripwire: FAILED (%v)", err)
	} else if discovery.Valid {
		t.Log("✓ Tripwire: ALIGNED (manifest matches disk)")
		t.Logf("  Manifest files: %d", len(discovery.Manifest))
		t.Logf("  Discovered files: %d", len(discovery.Discovered))
		if len(discovery.Unexpected) > 0 {
			t.Logf("  ⚠ Unexpected (on disk, not in manifest): %d", len(discovery.Unexpected))
			for _, f := range discovery.Unexpected {
				t.Logf("      %s", f)
			}
		}
	} else {
		t.Log("✗ Tripwire: MISALIGNED (files missing from disk)")
		for _, f := range discovery.Missing {
			t.Logf("    MISSING: %s", f)
		}
	}

	// ─────────────────────────────────────────────────────────────────
	// PHASE 3: Dependency Health
	// ─────────────────────────────────────────────────────────────────
	t.Log("")
	t.Log("── PHASE 3: Dependency Health ──")

	depErrors, err := config.ValidateDependencyGraph()
	if err != nil {
		t.Logf("✗ Dependencies: FAILED (%v)", err)
	} else if len(depErrors) == 0 {
		t.Log("✓ Dependencies: HEALTHY (no cycles, no missing refs)")
		tree, _ := config.GetDependencyTree()
		t.Logf("  Nodes in graph: %d", len(tree))
	} else {
		t.Log("✗ Dependencies: BROKEN")
		for _, depErr := range depErrors {
			t.Logf("    ERROR: %v", depErr)
		}
	}

	// ─────────────────────────────────────────────────────────────────
	// PHASE 4: Overall Assessment
	// ─────────────────────────────────────────────────────────────────
	t.Log("")
	t.Log("── OVERALL ASSESSMENT ──")

	if primaryResult.Valid && discovery.Valid && len(depErrors) == 0 {
		t.Log("✓ CONFIG SYSTEM: HEALTHY (+1)")
		t.Log("  All systems operational via primary path")
	} else if !primaryResult.Valid {
		// Check if fallback works
		fallbackResult := config.LoadAll()
		if fallbackResult.Valid {
			t.Log("⚠ CONFIG SYSTEM: DEGRADED (0)")
			t.Log("  Primary path broken, fallback operational")
			t.Log("  Action: Fix index.toml to restore primary path")
		} else {
			t.Log("✗ CONFIG SYSTEM: BROKEN (-1)")
			t.Log("  Neither primary nor fallback path working")
		}
	} else {
		t.Log("⚠ CONFIG SYSTEM: DEGRADED (0)")
		t.Log("  Primary path working but issues detected above")
	}

	t.Log("")
	t.Log("════════════════════════════════════════════════════════════════")
}

// ────────────────────────────────────────────────────────────────
// Code Execution - Test Entry Point
// ────────────────────────────────────────────────────────────────
//
// TestMain provides test setup and teardown.
// Currently minimal - extend for global fixtures if needed.

// [Reserved: TestMain for global setup/teardown]
// func TestMain(m *testing.M) {
//     // Setup: could validate bereshit structure exists
//     // Run tests
//     code := m.Run()
//     // Teardown: cleanup if needed
//     os.Exit(code)
// }

// ────────────────────────────────────────────────────────────────
// Code Cleanup - Resource Management
// ────────────────────────────────────────────────────────────────
//
// Cleanup patterns for tests that create resources.

// setupTest is a helper that sets up test environment and returns root + cleanup.
// Use with defer cleanup() for automatic resource cleanup.
//
// Usage:
//
//	func TestSomething(t *testing.T) {
//	    root, cleanup := setupTest(t)
//	    defer cleanup()
//	    t.Logf("Root: %s", root)
//	    // ... test code ...
//	}
func setupTest(t *testing.T) (string, func()) {
	t.Helper()
	root := getBereshitRoot(t)
	config.SetRoot(root)

	// Return root and cleanup function
	return root, func() {
		// Currently no cleanup needed - config loader is stateless per-call
		// Future: reset package state, close connections, etc.
	}
}

// ════════════════════════════════════════════════════════════════
// GROUP 2: DOCUMENTATION - Reference
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// Test Overview
// ────────────────────────────────────────────────────────────────
//
// Purpose: Verify config loader reads TOML specs from word/core/
//          AND serve as diagnostic API for config-driven systems.
//
// Test Functions (9 total):
//   - TestLoadAllWithoutRoot: Error handling without SetRoot
//   - TestLoadAll: Tripwire-aware loading (primary vs fallback)
//   - TestLoadSpec: Single spec loading with content verification
//   - TestLoadTypes: Types system (primitives, composition, validation)
//   - TestLoadMath: Math system (ternary.toml foundation)
//   - TestLoadBible: Bible system (5 specs)
//   - TestDiscoverAndCompare: Manifest vs disk tripwire
//   - TestValidateDependencyGraph: Dependency DAG validation
//   - TestDiagnostic: Comprehensive health check (run for debugging)
//
// Helper Functions (2):
//   - getBereshitRoot: Derives bereshit root from working directory
//   - setupTest: Test setup with cleanup function
//
// Execution: go test -v ./pkg/config/...
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   ✅ Add new test functions for new Load* functions
//   ✅ Add more assertions to existing tests
//   ✅ Update getBereshitRoot helper if path derivation changes
//
// Modify with Care:
//   ⚠️ Test function names - affects test filtering
//   ⚠️ Error messages - may affect CI/CD parsing
//
// NEVER Modify:
//   ❌ 4-block structure
//   ❌ Remove existing passing tests without replacement
//
// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart" for complete structure.
//
// Quick summary:
// - 9 test functions depend on 2 helpers (getBereshitRoot, setupTest)
// - Each test calls SetRoot → Load* → verify LoadResult
// - Baton: test framework → Test* → getBereshitRoot → config.SetRoot → config.Load*
//
// Ternary Results:
//   +1 (PASS)      - Primary path (index.toml) works
//    0 (HALF PASS) - Fallback works (tripwire triggered)
//   -1 (FAIL)      - Neither path works
//
// ────────────────────────────────────────────────────────────────
// Surgical Update Points (Extension Guide)
// ────────────────────────────────────────────────────────────────
//
// To add a new test for a new Load* function:
//   1. Add TestLoadNewThing following existing pattern
//   2. Call getBereshitRoot → SetRoot → LoadNewThing
//   3. Verify result.Valid == true and len(result.Configs) > 0
//   4. Update Organizational Chart in BODY
//
// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Unit tests - performance not primary concern]
//
// Note: Tests read actual TOML files from disk. If tests become slow,
// consider using t.TempDir() with minimal fixture files.
//
// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: "failed to get absolute path" or "working directory" errors
//   - Cause: Test run from unexpected location
//   - Solution: Set BERESHIT_ROOT environment variable
//   - Example: BERESHIT_ROOT=/path/to/bereshit go test ./pkg/config/...
//
// Problem: result.Valid == false
//   - Cause: TOML file missing or malformed
//   - Solution: Check word/core/ directory for expected .toml files
//   - Debug: Run TestDiagnostic for comprehensive health check
//
// Problem: Tests affect each other (flaky tests)
//   - Cause: SetRoot() modifies package-level state
//   - Solution: Run with go test -count=1 for isolation
//
// ────────────────────────────────────────────────────────────────
// Related Components & Dependencies
// ────────────────────────────────────────────────────────────────
//
// Tests: loader.go (same package)
//
// Key dependencies:
//   - os, path/filepath: Path resolution
//   - testing: Test framework
//   - config package: SetRoot, Load* functions
//
// Config manifest: word/core/index.toml (single source of truth)
//
// Systems tested (9 total, in dependency order):
//   0: math/          - ternary.toml (foundation)
//   1: types/         - primitives.toml, composition.toml, validation.toml
//   2: language/      - keywords.toml, syntax.toml
//   3: bible/         - scripture-text.toml, addressing.toml, encoding.toml,
//                       decoding.toml, translation.toml
//   4: os/health/     - score.toml, diagnostics.toml, log.toml, provider.toml
//   5: os/permission/ - access.toml
//   6: identity/      - model.toml, contract.toml
//   7: network/       - message.toml, timestamp.toml, contract.toml
//   8: filesystem/    - types.toml, contract.toml
//
// ────────────────────────────────────────────────────────────────
// Future Expansions & Roadmap
// ────────────────────────────────────────────────────────────────
//
// Planned Tests:
//   ⏳ Error case tests (malformed TOML, missing files)
//   ⏳ Edge case tests (empty files, large files)
//   ⏳ Config value validation tests
//
// Version History:
//   a-01.00 (2025-12-13) - Initial test file
//         - 8 test functions covering all Load* APIs
//         - 1 helper for bereshit root resolution
//   a-01.50 (2025-12-14) - Tripwire-aware + Diagnostic API
//         - 9 test functions (added TestDiagnostic)
//         - 2 helpers (added setupTest)
//         - Ternary results: +1 PASS / 0 HALF PASS / -1 FAIL
//         - Index.toml as manifest (config-driven system)
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This test file verifies the config loader reads TOML specs from
// bereshit/word/core/ via index.toml manifest. Tests cover all public
// Load* functions AND serve as diagnostic API for config-driven systems.
//
// "Prove all things; hold fast that which is good." - 1 Thessalonians 5:21
//
// ────────────────────────────────────────────────────────────────
// Quick Reference: Usage Examples
// ────────────────────────────────────────────────────────────────
//
// Run all config tests:
//   go test -v ./pkg/config/...
//
// Run diagnostic health check:
//   go test -v -run TestDiagnostic ./pkg/config/...
//
// Run single test:
//   go test -v -run TestLoadMath ./pkg/config/...
//
// Run with explicit root:
//   BERESHIT_ROOT=/path/to/bereshit go test -v ./pkg/config/...
//
// Run isolated (avoid state sharing):
//   go test -v -count=1 ./pkg/config/...

// ============================================================================
// END CLOSING
// ============================================================================
