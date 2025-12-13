// ═══════════════════════════════════════════════════════════════════════════
// Config Loader Test (4-Block Structure)
// Key: B-word-work-pkg-config-loader-test
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: DEPENDED (needs: pkg/config)
//   - Tests the config loader package
//   - Requires bereshit repository structure for integration tests
//
// derives_from: bereshit/word/seed/code/go/demo-test.go
// Derived from: Kingdom Technology 4-block code structure
//
// ═══════════════════════════════════════════════════════════════════════════

// Package config_test provides unit tests for the config loader.
//
// Config Loader Test - CPI-SI Bereshit Foundation
//
// ────────────────────────────────────────────────────────────────
// CORE IDENTITY (Required)
// ────────────────────────────────────────────────────────────────
//
// # Biblical Foundation
//
// Scripture: "Prove all things; hold fast that which is good."
//            — 1 Thessalonians 5:21 KJV
//
// Principle: Tests prove correctness before deployment. Config loading
//            must be validated to ensure specs exist and parse correctly.
//
// # CPI-SI Identity
//
// Component Type: Ladder (validates foundation that Phase 3+ builds upon)
//
// Role: Verify config loader functions work correctly
//
// # Authorship & Lineage
//
//   - Architect: Seanje Lenox-Wise
//   - Implementation: Nova Dawn
//   - Created: 2025-12-13
//   - Version: a-01.00
//
// # Purpose & Function
//
// Purpose: Unit tests for config loader package
//
// Key Features:
//
//   - SetRoot configuration
//   - LoadAll aggregation
//   - Individual loader functions
//   - Error handling for missing files
//
// ────────────────────────────────────────────────────────────────
// INTERFACE (Expected)
// ────────────────────────────────────────────────────────────────
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: testing, os, path/filepath
//   - Internal: bereshit/word/work/pkg/config
//
// # Usage & Integration
//
// Run Tests:
//
//	go test -v ./...                    # Run all tests
//	go test -v -run TestLoadAll         # Run specific test
//	go test -v -cover                   # Run with coverage
//
// ────────────────────────────────────────────────────────────────
// OPERATIONAL (Contextual)
// ────────────────────────────────────────────────────────────────
//
// [OMIT: Test file - validates health rather than tracks it]
//
package config_test

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Imports → Constants → Variables → Types → Type Methods → Package-Level State

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

import (
	"os"            // Environment variable for bereshit root
	"path/filepath" // Path construction
	"testing"       // Test framework

	"creativeworkzstudio.com/bereshit/word/work/pkg/config" // Package under test
)

// ────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────

// [Reserved: No constants needed - uses environment variable for root path]

// ────────────────────────────────────────────────────────────────
// Variables
// ────────────────────────────────────────────────────────────────

// [Reserved: No package-level variables - test functions are self-contained]

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────

// [Reserved: Uses types from config package - no custom test types needed]

// ────────────────────────────────────────────────────────────────
// Type Methods
// ────────────────────────────────────────────────────────────────

// [Reserved: No custom types defined above]

// ────────────────────────────────────────────────────────────────
// Package-Level State
// ────────────────────────────────────────────────────────────────

// [Reserved: Test file - no Rails infrastructure needed]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md
//
// -----------------------------------------------------------------------------
// BODY Sections Overview
// -----------------------------------------------------------------------------
//
// 1. ORGANIZATIONAL CHART (Internal Structure)
//    Purpose: Map dependencies and execution flow within this demo/test
//    Subsections: Test Structure → Demonstration Flow → Coverage Map
//
// 2. HELPERS/UTILITIES (Test Support)
//    Purpose: Test fixtures, setup/teardown, utility functions
//    Subsections: Test Fixtures → Mock Functions → Utility Functions
//
// 3. CORE OPERATIONS (Test/Demo Logic)
//    Purpose: Individual tests or demonstrations of functionality
//    Subsections: [Test Category 1] → [Test Category 2] → ... (organized by concern)
//
// 4. ERROR HANDLING/RECOVERY (Test Safety)
//    Purpose: Test failure handling, cleanup on error, panic recovery
//    Subsections: Cleanup Functions → Error Assertions → Recovery Patterns
//
// 5. PUBLIC APIs (Exported Tests/Demos)
//    Purpose: Test functions (Test*), benchmark functions (Benchmark*), examples
//    Subsections: Unit Tests → Integration Tests → Examples → Benchmarks
//
// Section order: Org Chart → Helpers → Core Operations → Error Handling → Public APIs
// This flows: understand structure → build fixtures → implement tests → handle failures → expose test interface
//
// Universal mapping (see standards for cross-language patterns):
//   Organizational Chart ≈ Test/Demo Structure Documentation
//   Helpers/Utilities ≈ Test Fixtures and Utilities
//   Core Operations ≈ Test/Demo Logic
//   Error Handling ≈ Test Failure Handling
//   Public APIs ≈ Exported Test Functions

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Internal Structure
// ────────────────────────────────────────────────────────────────
//
// Ladder Structure (Dependencies):
//
//   Public APIs (Top Rungs - Test Functions)
//   ├── TestLoadAllWithoutRoot() → tests error case
//   ├── TestLoadAll()            → uses getBereshitRoot(), tests full load
//   ├── TestLoadPrimitives()     → uses getBereshitRoot(), tests single file
//   ├── TestLoadTypes()          → uses getBereshitRoot(), tests single file
//   ├── TestLoadSchemas()        → uses getBereshitRoot(), tests directory
//   ├── TestLoadContracts()      → uses getBereshitRoot(), tests directory
//   ├── TestLoadBibleRail()      → uses getBereshitRoot(), tests directory
//   └── TestLoadConstants()      → uses getBereshitRoot(), tests directory
//
//   Helpers (Bottom Rungs - Foundations)
//   └── getBereshitRoot() → derives bereshit root from test file location
//
// Baton Flow (Execution Paths):
//
//   go test → TestLoadAll()
//     ↓
//   getBereshitRoot() → config.SetRoot()
//     ↓
//   config.LoadAll() → verify result
//     ↓
//   Exit → pass/fail
//
// APUs (Available Processing Units):
// - 9 functions total
// - 1 helper (getBereshitRoot)
// - 8 test functions (TestLoad*)

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities - Internal Support
// ────────────────────────────────────────────────────────────────

// getBereshitRoot derives the bereshit root path from test file location.
//
// Returns the absolute path to the bereshit repository root.
// Uses BERESHIT_ROOT env var if set, otherwise derives from file location.
func getBereshitRoot(t *testing.T) string {
	t.Helper()

	// Check environment variable first
	if root := os.Getenv("BERESHIT_ROOT"); root != "" {
		return root
	}

	// Derive from this file's location: word/work/pkg/config/ → bereshit root
	// This file is at: bereshit/word/work/pkg/config/loader_test.go
	// Root is 4 directories up
	wd, err := os.Getwd()
	if err != nil {
		t.Fatalf("failed to get working directory: %v", err)
	}

	// Go up 4 levels: config → pkg → work → word → bereshit
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

// [Reserved: Tests consume config package directly - no complex fixtures needed]

// ────────────────────────────────────────────────────────────────
// Error Handling - Test Assertions
// ────────────────────────────────────────────────────────────────

// [Reserved: Uses t.Fatal/t.Error directly - no custom assertion helpers needed]

// ────────────────────────────────────────────────────────────────
// Test Functions - Public APIs
// ────────────────────────────────────────────────────────────────

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

// TestLoadAll verifies all configs load successfully from bereshit root.
func TestLoadAll(t *testing.T) {
	root := getBereshitRoot(t)
	config.SetRoot(root)

	result := config.LoadAll()

	if !result.Valid {
		for _, err := range result.Errors {
			t.Errorf("LoadAll error: %v", err)
		}
		t.Fatal("LoadAll failed - see errors above")
	}

	// Verify expected categories present
	categories := []string{"core", "schemas", "contracts", "bible", "constants"}
	for _, cat := range categories {
		if _, ok := result.Configs[cat]; !ok {
			t.Errorf("expected category %q in result.Configs", cat)
		}
	}
}

// TestLoadPrimitives verifies primitives.toml loads correctly.
func TestLoadPrimitives(t *testing.T) {
	root := getBereshitRoot(t)
	config.SetRoot(root)

	cfg, err := config.LoadPrimitives()
	if err != nil {
		t.Fatalf("LoadPrimitives failed: %v", err)
	}

	if cfg.Name != "primitives.toml" {
		t.Errorf("expected Name 'primitives.toml', got %q", cfg.Name)
	}

	if len(cfg.Keys) == 0 {
		t.Error("expected Keys to be populated")
	}
}

// TestLoadTypes verifies types.toml loads correctly.
func TestLoadTypes(t *testing.T) {
	root := getBereshitRoot(t)
	config.SetRoot(root)

	cfg, err := config.LoadTypes()
	if err != nil {
		t.Fatalf("LoadTypes failed: %v", err)
	}

	if cfg.Name != "types.toml" {
		t.Errorf("expected Name 'types.toml', got %q", cfg.Name)
	}
}

// TestLoadSchemas verifies schema directory loads correctly.
func TestLoadSchemas(t *testing.T) {
	root := getBereshitRoot(t)
	config.SetRoot(root)

	schemas, err := config.LoadSchemas()
	if err != nil {
		t.Fatalf("LoadSchemas failed: %v", err)
	}

	// Should have at least one schema file
	if len(schemas) == 0 {
		t.Error("expected at least one schema file")
	}
}

// TestLoadContracts verifies contracts directory loads correctly.
func TestLoadContracts(t *testing.T) {
	root := getBereshitRoot(t)
	config.SetRoot(root)

	contracts, err := config.LoadContracts()
	if err != nil {
		t.Fatalf("LoadContracts failed: %v", err)
	}

	// Should have at least one contract file
	if len(contracts) == 0 {
		t.Error("expected at least one contract file")
	}
}

// TestLoadBibleRail verifies bible directory loads correctly.
func TestLoadBibleRail(t *testing.T) {
	root := getBereshitRoot(t)
	config.SetRoot(root)

	bible, err := config.LoadBibleRail()
	if err != nil {
		t.Fatalf("LoadBibleRail failed: %v", err)
	}

	// Should have at least one bible config file
	if len(bible) == 0 {
		t.Error("expected at least one bible config file")
	}
}

// TestLoadConstants verifies ternary-math.toml loads correctly.
func TestLoadConstants(t *testing.T) {
	root := getBereshitRoot(t)
	config.SetRoot(root)

	constants, err := config.LoadConstants()
	if err != nil {
		t.Fatalf("LoadConstants failed: %v", err)
	}

	// Should have ternary-math.toml
	if len(constants) == 0 {
		t.Error("expected at least one constants file")
	}

	// Verify ternary-math.toml is present
	found := false
	for _, c := range constants {
		if c.Name == "ternary-math.toml" {
			found = true
			break
		}
	}
	if !found {
		t.Error("expected ternary-math.toml in constants")
	}
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md
//
// -----------------------------------------------------------------------------
// CLOSING Sections Overview
// -----------------------------------------------------------------------------
//
// GROUP 1: CODING (Operations - Verify, Execute, Clean)
//
// 1. CODE VALIDATION (Test Execution)
//    Purpose: Run tests and verify correct behavior
//    Subsections: Test Execution → Coverage Requirements → Benchmark Execution
//
// 2. CODE EXECUTION (Test Runner)
//    Purpose: How tests are executed via go test
//    Subsections: Entry Point → Test Flow → Exit Codes
//
// 3. CODE CLEANUP (Test Cleanup)
//    Purpose: Test fixture cleanup and resource management
//    Subsections: Cleanup Patterns → t.Cleanup() Usage
//
// GROUP 2: FINAL DOCUMENTATION (Synthesis - Reference Back to Earlier Blocks)
//
// 4. TEST OVERVIEW (Summary with Back-References)
//    Purpose: High-level summary of what this test file covers
//    References: METADATA "Purpose & Function", tested component
//
// 5. MODIFICATION POLICY (Safe/Careful/Never)
//    Purpose: Guide future maintainers on what's safe to change
//    Subsections: Safe to Modify → Modify with Care → Never Modify
//
// 6. LADDER AND BATON FLOW (Back-Reference to BODY)
//    Purpose: Point to BODY Organizational Chart for test structure
//    References: BODY "Organizational Chart - Internal Structure"
//
// 7. SURGICAL UPDATE POINTS (Back-Reference to BODY)
//    Purpose: Point to BODY test organization for adding tests
//    References: BODY "Core Operations" test categories
//
// 8. PERFORMANCE CONSIDERATIONS (Benchmark Notes)
//    Purpose: Benchmark results and performance testing guidance
//    References: Benchmark functions in this file
//
// 9. TROUBLESHOOTING GUIDE (Test Failures)
//    Purpose: Common test failures and how to diagnose
//    References: BODY test functions with error conditions
//
// 10. RELATED COMPONENTS (Tested Components)
//     Purpose: Point to components being tested
//     References: METADATA "Dependencies" - what this tests
//
// 11. FUTURE EXPANSIONS (Test Coverage Roadmap)
//     Purpose: Planned test coverage, edge cases to add
//     Subsections: Coverage Gaps → Edge Cases → Integration Tests
//
// 12. CONTRIBUTION GUIDELINES (Adding Tests)
//     Purpose: How to add new tests to this file
//     Subsections: Test Naming → Test Structure → Assertions
//
// 13. QUICK REFERENCE (Test Commands)
//     Purpose: Copy-paste ready test commands
//     Subsections: Run All → Run Specific → Run Benchmarks → Coverage
//
// Section order: Validation → Execution → Cleanup → Overview → Policy → Ladder/Baton →
//                Surgical → Performance → Troubleshooting → Related → Future → Contribution → Reference
// This flows: run tests → clean up → document → guide test expansion
//
// ════════════════════════════════════════════════════════════════
// GROUP 1: CODING
// ════════════════════════════════════════════════════════════════
//
// ────────────────────────────────────────────────────────────────
// Code Validation: Config Loader Tests
// ────────────────────────────────────────────────────────────────
//
// Test Execution (from word/work/pkg/config/):
//
//   go test -v                        # Run all tests with verbose output
//   go test -v -run TestLoadAll       # Run specific test
//   go test -cover                    # Check coverage
//
// Build Verification:
//
//   go build ./...                    # Compiles without errors
//   go vet ./...                      # No warnings
//
// Expected Results:
//   - TestLoadAllWithoutRoot: PASS (verifies error handling)
//   - TestLoadAll: PASS (all categories load)
//   - TestLoadPrimitives: PASS (primitives.toml found)
//   - TestLoadTypes: PASS (types.toml found)
//   - TestLoadSchemas: PASS (at least 1 schema)
//   - TestLoadContracts: PASS (at least 1 contract)
//   - TestLoadBibleRail: PASS (at least 1 bible config)
//   - TestLoadConstants: PASS (ternary-math.toml found)
//
// ────────────────────────────────────────────────────────────────
// Code Execution: Config Loader Tests
// ────────────────────────────────────────────────────────────────
//
// Entry Point: go test (from word/work/pkg/config/)
//
// Execution Flow:
//   1. go test compiles test binary
//   2. getBereshitRoot() derives path from working directory
//   3. config.SetRoot() configures loader
//   4. Test functions call Load* functions and verify results
//   5. Results reported to stdout
//
// ────────────────────────────────────────────────────────────────
// Code Cleanup: Config Loader Tests
// ────────────────────────────────────────────────────────────────
//
// Resource Management:
//   - No file handles held open (config loader closes after read)
//   - No temp files created
//   - Package-level bereshitRoot state persists between tests
//
// Note: Tests may affect each other through SetRoot() - run with -count=1
// if isolation issues occur.
//
// ════════════════════════════════════════════════════════════════
// FINAL DOCUMENTATION
// ════════════════════════════════════════════════════════════════
//
// ────────────────────────────────────────────────────────────────
// Test Overview & Summary
// ────────────────────────────────────────────────────────────────
//
// Purpose: Verify config loader reads TOML files from bereshit/void/config/
//
// Quick summary:
//   - Tests all 6 Load* functions (LoadPrimitives through LoadConstants)
//   - Tests LoadAll orchestration (all configs in single call)
//   - Verifies SetRoot requirement (must be set before loading)
//
// Test Functions: See BODY "Test Functions" section above
//   TestLoadAllWithoutRoot, TestLoadAll, TestLoadPrimitives,
//   TestLoadTypes, TestLoadSchemas, TestLoadContracts,
//   TestLoadBibleRail, TestLoadConstants
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
// - 8 test functions depend on 1 helper (getBereshitRoot)
// - Each test calls SetRoot → Load* → verify LoadResult
// - Baton: test framework → Test* → getBereshitRoot → config.SetRoot → config.Load*
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
//   - Solution: Check void/config/ directory for expected .toml files
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
// Config files tested (in void/config/):
//   - primitives.toml, types.toml, schemas.toml
//   - contracts.toml, bible-rail.toml, constants.toml
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
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This test file verifies the config loader reads TOML configuration files
// from bereshit/void/config/. Tests cover all public Load* functions and
// verify the SetRoot requirement.
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
// Run single test:
//   go test -v -run TestLoadPrimitives ./pkg/config/...
//
// Run with explicit root:
//   BERESHIT_ROOT=/path/to/bereshit go test -v ./pkg/config/...
//
// Run isolated (avoid state sharing):
//   go test -v -count=1 ./pkg/config/...

// ============================================================================
// END CLOSING
// ============================================================================
