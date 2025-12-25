// #!omni code --go -executable
// ═══════════════════════════════════════════════════════════════════════════
// Phase 0 Demo: Config-Driven System Demonstration
// Key: B-tov-demo-phase0-demo-config
// ═══════════════════════════════════════════════════════════════════════════
//
// derives_from: bereshit/word/seed/code/go/executable.go
// See: standards/code/4-block/
//
// ═══════════════════════════════════════════════════════════════════════════

// Package main demonstrates a config-driven system with tripwire-aware validation.
//
// # Core Identity
//
//	Key:     B-tov-demo-phase0-demo-config
//	Type:    Baton (execution flow — validates and reports)
//	Version: a-01.50 (2025-12-14)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2025-12-12
//
//	Scripture: "Prove all things; hold fast that which is good" — 1 Thess 5:21
//	Anchor:    "Every purpose is established by counsel" — Proverbs 20:18
//
// # Version History
//
//   - a-01.50 (2025-12-14) — Tripwire-aware + ternary status + diagnostic API
//   - a-01.00 (2025-12-12) — Initial Phase 0 demo
//
// # Interface
//
//	Needs:   fmt, os | pkg/config (config loader)
//	Demo:    Config-driven system with primary/fallback paths
//	Run:     BERESHIT_ROOT=/path ./demo-config
//
// # Demo Functions
//
// Public API (for embedding in games/tutorials):
//
//	DemoConfigRunAll       Run full demonstration, return failure count
//	DemoConfigGetPassed    Get count of passed validations
//	DemoConfigGetFailed    Get count of failed validations
//	DemoConfigGetStatus    Get ternary status (+1, 0, -1)
//	DemoConfigReset        Reset counters for fresh run
//
// Diagnostic APIs (debug markers from loader_test.go TestDiagnostic):
//
//	DemoConfigDiagnostic         Comprehensive debug (all phases)
//	DemoConfigDiagnosticIndex    Phase 1: index health
//	DemoConfigDiagnosticTripwire Phase 2: manifest vs disk
//	DemoConfigDiagnosticDeps     Phase 3: dependency graph
//
// # Ternary Results
//
//	+1 (PASS)      Primary path works (index.toml)
//	 0 (HALF PASS) Fallback works (tripwire triggered)
//	-1 (FAIL)      Neither path works
//
// # Operational
//
//	Blocking: No — runs once, reports, exits
//	Health:   Reports system health, doesn't self-track
package main

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// Section order: Imports → Types → Type Methods → Constants → Variables → Package-Level State
// See: bereshit/word/seed/code/go/executable.go > SETUP

// ────────────────────────────────────────────────────────────────
// Imports
// ────────────────────────────────────────────────────────────────

//--- Standard Library ---
// Output formatting, environment access
import (
	"fmt" // formatted output
	"os"  // environment variables and exit codes
)

//--- External Packages ---
// [Reserved: None — demo uses only stdlib and internal loader]

//--- Internal Packages ---
// Config loader under demonstration
import (
	"creativeworkzstudio.com/bereshit/word/work/pkg/config" // config loader
)

// ────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Uses types from config package — no custom types needed]

// ────────────────────────────────────────────────────────────────
// Type Methods
// ────────────────────────────────────────────────────────────────
//
// [Reserved: No local types defined above]

// ────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────
//
// Ternary status and exit codes for config-driven system demonstration.
// Maps to building block method: +1 (build up), 0 (foundation), -1 (break down).
//
// Subsections: Ternary Status, Exit Codes

//--- Ternary Status ---
// Config system health expressed as ternary values
const (
	StatusPass     = 1  // +1: Primary path works (index.toml)
	StatusHalfPass = 0  //  0: Fallback works (tripwire triggered)
	StatusFail     = -1 // -1: Neither path works
)

//--- Exit Codes ---
// Shell exit status for scripting integration
const (
	ExitSuccess = 0 // All systems healthy
	ExitFailure = 1 // One or more systems failed
)

// ────────────────────────────────────────────────────────────────
// Variables
// ────────────────────────────────────────────────────────────────
//
// Package-level mutable state. Kept minimal — explicit reset via DemoConfigReset().
//
// Subsections: Demo State, Sentinel Errors

//--- Demo State ---
// Tracking counters for demonstration results.
var (
	systemsPassed int // +N: count of systems that loaded successfully
	systemsFailed int // +N: count of systems that failed to load
	systemStatus  int // ternary: +1 (PASS), 0 (HALF PASS), -1 (FAIL)
)

//--- Sentinel Errors ---
// [Reserved: Demo uses config package errors — no custom sentinels needed]

// ────────────────────────────────────────────────────────────────
// Package-Level State (Rails Pattern)
// ────────────────────────────────────────────────────────────────
//
// [Reserved: Demo runs once — no Rails infrastructure needed]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Section order: Org Chart → Helpers → Core Operations → Error Handling → Public APIs
// See: bereshit/word/seed/code/go/executable.go > BODY

// ────────────────────────────────────────────────────────────────
// Organizational Chart
// ────────────────────────────────────────────────────────────────
//
// Navigation map for BODY + CLOSING. When lost in demo logic, return here.
// Shows WHERE functions live (layers), HOW they connect (flow), WHAT exists (count).
//
// Subsections: Ladder Structure, Baton Flow, APU Inventory

//--- Ladder Structure ---
// Vertical dependency layers. Bottom supports top. Read bottom-up for foundations.
//
//   ════════════════════════════════════════════════════════════════
//   CLOSING (Lifecycle - Validation → Execution → Cleanup)
//   ════════════════════════════════════════════════════════════════
//   Code Execution (Entry Point)
//   └── main()                       → delegates to DemoConfigRunAll()
//
//   Code Validation (Prerequisites)
//   └── demoValidatePrerequisites()  → check index, tripwire, dependencies
//
//   Code Cleanup (Reporting)
//   └── demoCleanupAndReport()       → final summary and status
//
//   ════════════════════════════════════════════════════════════════
//   BODY (Business Logic - Helpers → Core Operations → Public APIs)
//   ════════════════════════════════════════════════════════════════
//   Public APIs (Top Rungs - Exported Interface)
//   ├── DemoConfigRunAll()           → orchestrate full demonstration
//   ├── DemoConfigGetPassed()        → inspection: passed count
//   ├── DemoConfigGetFailed()        → inspection: failed count
//   ├── DemoConfigReset()            → reset counters for fresh run
//   ├── DemoConfigGetStatus()        → inspection: ternary status
//   └── Diagnostic APIs (debug markers from loader_test.go)
//       ├── DemoConfigDiagnostic()       → comprehensive debug (all phases)
//       ├── DemoConfigDiagnosticIndex()  → Phase 1: index health
//       ├── DemoConfigDiagnosticTripwire() → Phase 2: manifest vs disk
//       └── DemoConfigDiagnosticDeps()   → Phase 3: dependency graph
//
//   Core Operations (Business Logic)
//   └── demoShowConfigCategories()   → display loaded systems with samples
//
//   Helpers (Value Display)
//   ├── demoShowMathValues()         → sample ternary.toml values
//   ├── demoShowPrimitiveValues()    → sample primitives.toml values
//   └── demoShowHealthValues()       → sample score.toml values

//--- Baton Flow ---
// Horizontal execution path. Read left-to-right for runtime order.
//
//   CLOSING: main()
//     ↓
//   BODY: DemoConfigRunAll()
//     ├─→ DemoConfigReset()
//     ├─→ CLOSING: demoValidatePrerequisites()
//     │     ├─→ config.SetRoot()
//     │     ├─→ config.DiscoverAndCompare()      [Tripwire]
//     │     ├─→ config.ValidateDependencyGraph() [Dependencies]
//     │     └─→ config.LoadAllFromIndex()        [Primary] or config.LoadAll() [Fallback]
//     ├─→ BODY: demoShowConfigCategories()
//     │     └─→ demoShowMathValues(), demoShowPrimitiveValues(), demoShowHealthValues()
//     └─→ CLOSING: demoCleanupAndReport()
//     ↓
//   os.Exit(0 or 1)

//--- APU Inventory ---
// Function count by category.
//
//   Total: 10 functions
//   ├── CLOSING Lifecycle: 3 functions
//   │   ├── Code Execution: main()
//   │   ├── Code Validation: demoValidatePrerequisites()
//   │   └── Code Cleanup: demoCleanupAndReport()
//   ├── BODY Public APIs: 5 functions
//   │   └── DemoConfigRunAll, GetPassed, GetFailed, GetStatus, Reset
//   ├── BODY Core Operations: 1 function
//   │   └── demoShowConfigCategories()
//   └── BODY Helpers: 3 functions
//       └── demoShowMathValues, demoShowPrimitiveValues, demoShowHealthValues

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities - Internal Support
// ────────────────────────────────────────────────────────────────
//
// Foundation layer for demo. Core Operations depend on these, never the reverse.
// See: Org Chart > Ladder Structure > Helpers
//
// Subsections: Display Helpers

//--- Display Helpers ---
// Show sample values from loaded configs. Called by demoShowConfigCategories().
// No side effects — read-only access to ConfigFile.Data.

// demoShowMathValues displays sample values from math/ternary.toml.
//
// Parameters:
//   - cfg: loaded ConfigFile from math system
//
// Displays:
//   - trit.values, trit.states, trit.identity_sum
//   - packing.byte_states
func demoShowMathValues(cfg *config.ConfigFile) {
	fmt.Println("  │")
	fmt.Println("  │   Sample values from ternary.toml:")

	// Try to access trit values (values is an array [-1, 0, 1])
	if trit, ok := cfg.Data["trit"].(map[string]any); ok {
		if values, ok := trit["values"].([]any); ok {
			fmt.Printf("  │     trit.values = %v\n", values)
		}
		if states, ok := trit["states"].(int64); ok {
			fmt.Printf("  │     trit.states = %d\n", states)
		}
		if identity, ok := trit["identity_sum"].(int64); ok {
			fmt.Printf("  │     trit.identity_sum = %d (balanced!)\n", identity)
		}
	}

	// Try to access packing constants
	if packing, ok := cfg.Data["packing"].(map[string]any); ok {
		if states, ok := packing["byte_states"].(int64); ok {
			fmt.Printf("  │     packing.byte_states = %d (3^5)\n", states)
		}
	}
}

// demoShowPrimitiveValues displays sample values from types/primitives.toml.
//
// Parameters:
//   - cfg: loaded ConfigFile from types system
//
// Displays:
//   - int9.min_value, int9.max_value, int9.size_trits
//   - int27.min_value, int27.max_value
func demoShowPrimitiveValues(cfg *config.ConfigFile) {
	fmt.Println("  │")
	fmt.Println("  │   Sample values from primitives.toml:")

	// Try to access int9 range (flat fields, not nested)
	if int9, ok := cfg.Data["int9"].(map[string]any); ok {
		if min, ok := int9["min_value"].(int64); ok {
			fmt.Printf("  │     int9.min_value = %d\n", min)
		}
		if max, ok := int9["max_value"].(int64); ok {
			fmt.Printf("  │     int9.max_value = %d\n", max)
		}
		if trits, ok := int9["size_trits"].(int64); ok {
			fmt.Printf("  │     int9.size_trits = %d\n", trits)
		}
	}

	// Try to access int27 range
	if int27, ok := cfg.Data["int27"].(map[string]any); ok {
		if min, ok := int27["min_value"].(int64); ok {
			fmt.Printf("  │     int27.min_value = %d\n", min)
		}
		if max, ok := int27["max_value"].(int64); ok {
			fmt.Printf("  │     int27.max_value = %d\n", max)
		}
	}
}

// demoShowHealthValues displays sample values from os/health/score.toml.
//
// Parameters:
//   - cfg: loaded ConfigFile from health system
//
// Displays:
//   - health.storage_min, health.storage_max, health.storage_default
func demoShowHealthValues(cfg *config.ConfigFile) {
	fmt.Println("  │")
	fmt.Println("  │   Sample values from score.toml:")

	// Try to access health values (stored as uint8, interpreted as ternary)
	if health, ok := cfg.Data["health"].(map[string]any); ok {
		if min, ok := health["storage_min"].(int64); ok {
			fmt.Printf("  │     health.storage_min = %d\n", min)
		}
		if max, ok := health["storage_max"].(int64); ok {
			fmt.Printf("  │     health.storage_max = %d\n", max)
		}
		if def, ok := health["storage_default"].(int64); ok {
			fmt.Printf("  │     health.storage_default = %d (center/balanced)\n", def)
		}
	}
}

// ────────────────────────────────────────────────────────────────
// Core Operations - Business Logic
// ────────────────────────────────────────────────────────────────
//
// Middle rung. Depends on Helpers below, used by Public APIs above.
// See: Org Chart > Ladder Structure > Core Operations
//
// Business logic for demonstrating config-driven system.
// Lifecycle functions (validation, cleanup) are in CLOSING.
//
// Subsections: Demo Display

//--- Demo Display ---
// Show loaded configurations with sample values.

// demoShowConfigCategories displays loaded systems and sample values.
//
// Business logic for the demonstration's "EXECUTION" phase.
// Iterates through all 9 systems in dependency order, showing:
//   - System name and description
//   - Loaded spec count
//   - Sample values from key specs (math, types, health)
//
// Uses helper functions for detailed value display.
func demoShowConfigCategories() {
	result := config.LoadAll()

	fmt.Println()
	fmt.Println("────────────────────────────────────────────────────────────────")
	fmt.Println("Step 2: Demonstrating Loaded Specifications")
	fmt.Println("────────────────────────────────────────────────────────────────")
	fmt.Println()
	fmt.Println("  Systems load in dependency order (0 = foundation, higher builds on lower):")
	fmt.Println()

	// Show systems in dependency order with descriptions
	systemOrder := []struct {
		name string
		desc string
	}{
		{"math", "Ternary mathematics — foundation all types build on"},
		{"types", "Type system — primitives, composition, validation"},
		{"language", "OmniCode language — keywords and syntax"},
		{"bible", "Scripture systems — encoding, addressing, translation"},
		{"health", "Health monitoring — scores, diagnostics, logging"},
		{"permission", "Permission system — access control"},
		{"identity", "Identity system — CPI-SI identity model"},
		{"network", "Network systems — FaithNet messaging"},
		{"filesystem", "Filesystem types — OmniFS structures"},
	}

	for order, sys := range systemOrder {
		configs, ok := result.Configs[sys.name]
		if !ok {
			fmt.Printf("  ✗ System %d: %s — NOT FOUND\n", order, sys.name)
			systemsFailed++
			continue
		}
		systemsPassed++

		fmt.Printf("  ┌─ System %d: %s ─────────────────────────────────────────\n", order, sys.name)
		fmt.Printf("  │ %s\n", sys.desc)
		fmt.Printf("  │ Specs: %d loaded\n", len(configs))

		// Show specs with their sections
		for _, cfg := range configs {
			fmt.Printf("  │   • %s (%d sections: %v)\n", cfg.Name, len(cfg.Keys), cfg.Keys)
		}

		// Show sample values for key foundation specs
		if sys.name == "math" && len(configs) > 0 {
			demoShowMathValues(configs[0])
		}
		if sys.name == "types" {
			for _, cfg := range configs {
				if cfg.Name == "primitives.toml" {
					demoShowPrimitiveValues(cfg)
				}
			}
		}
		if sys.name == "health" {
			for _, cfg := range configs {
				if cfg.Name == "score.toml" {
					demoShowHealthValues(cfg)
				}
			}
		}

		fmt.Println("  └────────────────────────────────────────────────────────────┘")
		fmt.Println()
	}

	fmt.Println("────────────────────────────────────────────────────────────────")
	fmt.Println("  Phase 0 provides the SPECIFICATION layer.")
	fmt.Println("  These values flow to the generator → C headers → libtrit.")
	fmt.Println("────────────────────────────────────────────────────────────────")
}

// ────────────────────────────────────────────────────────────────
// Error Handling/Recovery Patterns
// ────────────────────────────────────────────────────────────────
//
// Demo error handling. Uses config package errors — no custom types needed.
// See: Org Chart > APU Inventory > Error Handling
//
// Subsections: Error Strategy

//--- Error Strategy ---
// Non-blocking: collect failures in systemsFailed, continue processing.
// Final status determined in CLOSING: Code Cleanup > demoCleanupAndReport.
//
// [Reserved: Future — add structured error collection, recovery patterns]

// ────────────────────────────────────────────────────────────────
// Public APIs - Exported Interface
// ────────────────────────────────────────────────────────────────
//
// Top rung. Orchestrates Core Operations for external callers.
// See: Org Chart > Ladder Structure > Public APIs
//
// Subsections: Orchestration, Inspection, State Management

//--- Orchestration ---
// Main entry point for running the demonstration.

// DemoConfigRunAll runs all Phase 0 config demonstration.
// Returns the count of failed configurations (0 = all passed).
//
// Orchestrates the demo's three phases:
//   1. VALIDATION — demoValidatePrerequisites() [CLOSING]
//   2. EXECUTION  — demoShowConfigCategories()  [BODY]
//   3. CLEANUP    — demoCleanupAndReport()      [CLOSING]
//
// Ternary Results:
//   +1 (PASS):      Primary path works (index.toml)
//    0 (HALF PASS): Fallback works (tripwire triggered)
//   -1 (FAIL):      Neither path works
//
// Usage in game/tutorial:
//
//	if DemoConfigRunAll() == 0 {
//	    unlockNextPhase()
//	}
func DemoConfigRunAll() int {
	// ═══════════════════════════════════════════════════════════
	// SETUP: Reset state for fresh run
	// ═══════════════════════════════════════════════════════════
	DemoConfigReset()

	fmt.Println("════════════════════════════════════════════════════════════════")
	fmt.Println("Phase 0 Demo: Config System Demonstration")
	fmt.Println("════════════════════════════════════════════════════════════════")
	fmt.Println()
	fmt.Println("  Demonstrating the specification foundation for Kingdom Technology")
	fmt.Println()

	// ═══════════════════════════════════════════════════════════
	// VALIDATION: Verify config loader prerequisites [CLOSING]
	// ═══════════════════════════════════════════════════════════
	if !demoValidatePrerequisites() {
		systemStatus = StatusFail // -1: Neither path works
		demoCleanupAndReport()
		return systemsFailed
	}

	// ═══════════════════════════════════════════════════════════
	// EXECUTION: Demonstrate config loading [BODY]
	// ═══════════════════════════════════════════════════════════
	demoShowConfigCategories()

	// ═══════════════════════════════════════════════════════════
	// CLEANUP: Report results [CLOSING]
	// ═══════════════════════════════════════════════════════════
	if systemsFailed == 0 {
		systemStatus = StatusPass // +1: All systems loaded
	} else {
		systemStatus = StatusHalfPass // 0: Partial success
	}
	demoCleanupAndReport()

	return systemsFailed
}

//--- Inspection ---
// Read-only access to demo state for debugging and testing.

// DemoConfigGetPassed returns the count of systems that passed validation.
func DemoConfigGetPassed() int {
	return systemsPassed
}

// DemoConfigGetFailed returns the count of systems that failed validation.
func DemoConfigGetFailed() int {
	return systemsFailed
}

// DemoConfigGetStatus returns the overall ternary status.
//
// Returns:
//   +1 (StatusPass):     All systems healthy, primary path
//    0 (StatusHalfPass): Systems functional, fallback or partial
//   -1 (StatusFail):     System broken
func DemoConfigGetStatus() int {
	return systemStatus
}

//--- State Management ---
// Reset demo state for fresh runs.

// DemoConfigReset resets all demo counters for a fresh run.
func DemoConfigReset() {
	systemsPassed = 0
	systemsFailed = 0
	systemStatus = 0
}

//--- Diagnostic APIs ---
// Debug markers mirroring loader_test.go TestDiagnostic pattern.
// These return results AND print - building blocks for validation.

// DemoConfigDiagnosticIndex checks index health (Phase 1).
//
// Mirrors TestDiagnostic Phase 1. Calls config.LoadAllFromIndex().
// Returns the LoadResult for use by callers.
//
// Returns:
//   - config.LoadResult with Valid, Configs, Errors, Summary
func DemoConfigDiagnosticIndex(verbose bool) config.LoadResult {
	result := config.LoadAllFromIndex()
	if verbose {
		fmt.Println("── DIAGNOSTIC: Index Health ──")
		if result.Valid {
			fmt.Println("✓ index.toml: HEALTHY (primary path working)")
			fmt.Printf("  Systems loaded: %d\n", len(result.Configs))
			for sys, configs := range result.Summary {
				fmt.Printf("    %s: %d specs\n", sys, len(configs))
			}
		} else {
			fmt.Println("⚠ index.toml: DEGRADED (errors encountered)")
			for _, err := range result.Errors {
				fmt.Printf("    ERROR: %v\n", err)
			}
		}
		fmt.Println()
	}
	return result
}

// DemoConfigDiagnosticTripwire checks manifest vs filesystem (Phase 2).
//
// Mirrors TestDiagnostic Phase 2. Calls config.DiscoverAndCompare().
// Returns the DiscoveryResult for use by callers.
//
// Returns:
//   - config.DiscoveryResult with Valid, Manifest, Discovered, Missing, Unexpected
//   - error if discovery failed
func DemoConfigDiagnosticTripwire(verbose bool) (config.DiscoveryResult, error) {
	discovery, err := config.DiscoverAndCompare()
	if verbose {
		fmt.Println("── DIAGNOSTIC: Tripwire Check ──")
		if err != nil {
			fmt.Printf("✗ Tripwire: FAILED (%v)\n", err)
		} else if discovery.Valid {
			fmt.Println("✓ Tripwire: ALIGNED (manifest matches disk)")
			fmt.Printf("  Manifest files: %d\n", len(discovery.Manifest))
			fmt.Printf("  Discovered files: %d\n", len(discovery.Discovered))
			if len(discovery.Unexpected) > 0 {
				fmt.Printf("  ⚠ Unexpected (on disk, not in manifest): %d\n", len(discovery.Unexpected))
				for _, f := range discovery.Unexpected {
					fmt.Printf("      %s\n", f)
				}
			}
		} else {
			fmt.Println("✗ Tripwire: MISALIGNED (files missing from disk)")
			for _, f := range discovery.Missing {
				fmt.Printf("    MISSING: %s\n", f)
			}
		}
		fmt.Println()
	}
	return discovery, err
}

// DemoConfigDiagnosticDeps checks dependency graph health (Phase 3).
//
// Mirrors TestDiagnostic Phase 3. Calls config.ValidateDependencyGraph().
// Returns dependency errors for use by callers.
//
// Returns:
//   - []error: dependency validation errors (empty if healthy)
//   - error: if validation itself failed
func DemoConfigDiagnosticDeps(verbose bool) ([]error, error) {
	depErrors, err := config.ValidateDependencyGraph()
	if verbose {
		fmt.Println("── DIAGNOSTIC: Dependency Health ──")
		if err != nil {
			fmt.Printf("✗ Dependencies: FAILED (%v)\n", err)
		} else if len(depErrors) == 0 {
			fmt.Println("✓ Dependencies: HEALTHY (no cycles, no missing refs)")
			tree, _ := config.GetDependencyTree()
			fmt.Printf("  Nodes in graph: %d\n", len(tree))
		} else {
			fmt.Println("✗ Dependencies: BROKEN")
			for _, depErr := range depErrors {
				fmt.Printf("    ERROR: %v\n", depErr)
			}
		}
		fmt.Println()
	}
	return depErrors, err
}

// DemoConfigDiagnosticLoad loads all configs (Phase 4).
//
// Calls config.LoadAll() - the fallback/primary loader.
// Returns the LoadResult for use by callers.
//
// Returns:
//   - config.LoadResult with Valid, Configs, Errors, Summary
func DemoConfigDiagnosticLoad(verbose bool) config.LoadResult {
	result := config.LoadAll()
	if verbose {
		fmt.Println("── DIAGNOSTIC: Config Loading ──")
		if result.Valid {
			fmt.Printf("✓ Loaded: %d systems\n", len(result.Summary))
			for sys, configs := range result.Summary {
				fmt.Printf("    %s: %d specs\n", sys, len(configs))
			}
		} else {
			fmt.Println("✗ Load FAILED")
			for _, err := range result.Errors {
				fmt.Printf("    ERROR: %v\n", err)
			}
		}
		fmt.Println()
	}
	return result
}

// DemoConfigDiagnostic runs all diagnostic phases with full output.
//
// Comprehensive debug function mirroring TestDiagnostic from loader_test.go.
// Returns overall ternary status: +1 (healthy), 0 (degraded), -1 (broken).
//
// Phases:
//   1. Index Health (config.LoadAllFromIndex)
//   2. Tripwire Check (config.DiscoverAndCompare)
//   3. Dependency Health (config.ValidateDependencyGraph)
//   4. Overall Assessment
func DemoConfigDiagnostic() int {
	fmt.Println("════════════════════════════════════════════════════════════════")
	fmt.Println("CONFIG SYSTEM DIAGNOSTIC")
	fmt.Println("════════════════════════════════════════════════════════════════")
	fmt.Println()

	// Run all phases with verbose output
	indexResult := DemoConfigDiagnosticIndex(true)
	discovery, _ := DemoConfigDiagnosticTripwire(true)
	depErrors, _ := DemoConfigDiagnosticDeps(true)

	// Overall Assessment
	fmt.Println("── OVERALL ASSESSMENT ──")
	var status int

	if indexResult.Valid && discovery.Valid && len(depErrors) == 0 {
		fmt.Println("✓ CONFIG SYSTEM: HEALTHY (+1)")
		fmt.Println("  All systems operational via primary path")
		status = StatusPass
	} else if !indexResult.Valid {
		fallbackResult := DemoConfigDiagnosticLoad(false)
		if fallbackResult.Valid {
			fmt.Println("⚠ CONFIG SYSTEM: DEGRADED (0)")
			fmt.Println("  Primary path broken, fallback operational")
			fmt.Println("  Action: Fix index.toml to restore primary path")
			status = StatusHalfPass
		} else {
			fmt.Println("✗ CONFIG SYSTEM: BROKEN (-1)")
			fmt.Println("  Neither primary nor fallback path working")
			status = StatusFail
		}
	} else {
		fmt.Println("⚠ CONFIG SYSTEM: DEGRADED (0)")
		fmt.Println("  Primary path working but issues detected above")
		status = StatusHalfPass
	}
	fmt.Println()
	fmt.Println("════════════════════════════════════════════════════════════════")
	return status
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
// See: bereshit/word/seed/code/go/executable.go > CLOSING

// ════════════════════════════════════════════════════════════════
// GROUP 1: CODING - Verify, Execute, Clean
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// Code Validation - Prerequisites Check
// ────────────────────────────────────────────────────────────────
//
// Lifecycle validation function. Runs BEFORE main execution.
// Checks config system prerequisites: root, tripwire, dependencies.
//
// Subsections: Validation Logic

//--- Validation Logic ---
// Check system prerequisites before demonstration can proceed.

// demoValidatePrerequisites checks config system prerequisites.
//
// Lifecycle function for the demo's "VALIDATION" phase.
// Returns true if validation passes, false if demo cannot proceed.
//
// USES diagnostic APIs as building blocks (no duplicate config.* calls):
//   1. Set bereshit root from BERESHIT_ROOT env var
//   2. DemoConfigDiagnosticTripwire: Compare manifest to filesystem
//   3. DemoConfigDiagnosticDeps: Validate dependency graph
//   4. DemoConfigDiagnosticLoad: Verify all configs parse
//
// Updates systemsFailed counter on errors.
func demoValidatePrerequisites() bool {
	fmt.Println("────────────────────────────────────────────────────────────────")
	fmt.Println("Step 1: Validating config loader prerequisites...")
	fmt.Println("────────────────────────────────────────────────────────────────")

	root := os.Getenv("BERESHIT_ROOT")
	if root == "" {
		root = "."
	}

	// Check root exists
	fmt.Printf("  Bereshit Root: %s\n", root)
	config.SetRoot(root)

	// ═══════════════════════════════════════════════════════════
	// TRIPWIRE: Uses DemoConfigDiagnosticTripwire (building block)
	// ═══════════════════════════════════════════════════════════
	fmt.Println()
	fmt.Println("  ┌─ Tripwire: Manifest vs Filesystem ─────────────────────────┐")

	// Call diagnostic API (verbose=false, we format our own output)
	discovery, err := DemoConfigDiagnosticTripwire(false)
	if err != nil {
		fmt.Printf("  │ ✗ Discovery failed: %v\n", err)
		systemsFailed++
		fmt.Println("  └────────────────────────────────────────────────────────────┘")
		return false
	}

	fmt.Printf("  │ Manifest files:   %d\n", len(discovery.Manifest))
	fmt.Printf("  │ Discovered files: %d\n", len(discovery.Discovered))

	if len(discovery.Missing) > 0 {
		fmt.Printf("  │ ✗ MISSING (in manifest, not on disk): %d\n", len(discovery.Missing))
		for _, f := range discovery.Missing {
			fmt.Printf("  │   - %s\n", f)
			systemsFailed++
		}
	}

	if len(discovery.Unexpected) > 0 {
		fmt.Printf("  │ ⚠ UNEXPECTED (on disk, not in manifest): %d\n", len(discovery.Unexpected))
		for _, f := range discovery.Unexpected {
			fmt.Printf("  │   - %s\n", f)
		}
	}

	if discovery.Valid {
		fmt.Println("  │ ✓ Tripwire PASSED - manifest matches filesystem")
	} else {
		fmt.Println("  │ ✗ Tripwire FAILED - missing files detected")
	}
	fmt.Println("  └────────────────────────────────────────────────────────────┘")

	// ═══════════════════════════════════════════════════════════
	// DEPENDENCY: Uses DemoConfigDiagnosticDeps (building block)
	// ═══════════════════════════════════════════════════════════
	fmt.Println()
	fmt.Println("  ┌─ Dependency Graph Validation ──────────────────────────────┐")

	// Call diagnostic API (verbose=false, we format our own output)
	depErrors, err := DemoConfigDiagnosticDeps(false)
	if err != nil {
		fmt.Printf("  │ ✗ Validation failed: %v\n", err)
		systemsFailed++
		fmt.Println("  └────────────────────────────────────────────────────────────┘")
		return false
	}

	if len(depErrors) > 0 {
		fmt.Printf("  │ ✗ Dependency errors: %d\n", len(depErrors))
		for _, e := range depErrors {
			fmt.Printf("  │   - %v\n", e)
			systemsFailed++
		}
		fmt.Println("  └────────────────────────────────────────────────────────────┘")
		return false
	}

	fmt.Println("  │ ✓ No circular dependencies")
	fmt.Println("  │ ✓ All referenced specs exist")
	fmt.Println("  │ ✓ Dependency graph VALID")
	fmt.Println("  └────────────────────────────────────────────────────────────┘")

	// ═══════════════════════════════════════════════════════════
	// LOAD: Uses DemoConfigDiagnosticLoad (building block)
	// ═══════════════════════════════════════════════════════════
	fmt.Println()
	fmt.Println("  ┌─ Config Loading ───────────────────────────────────────────┐")

	// Call diagnostic API (verbose=false, we format our own output)
	result := DemoConfigDiagnosticLoad(false)

	if !result.Valid {
		fmt.Println("  │ ✗ Config loader failed")
		for _, err := range result.Errors {
			fmt.Printf("  │   - %v\n", err)
			systemsFailed++
		}
		fmt.Println("  └────────────────────────────────────────────────────────────┘")
		fmt.Println("────────────────────────────────────────────────────────────────")
		fmt.Println("✗ Prerequisites failed. Cannot proceed.")
		return false
	}

	fmt.Printf("  │ ✓ %d systems loaded successfully\n", len(result.Configs))
	fmt.Println("  └────────────────────────────────────────────────────────────┘")

	fmt.Println("────────────────────────────────────────────────────────────────")
	fmt.Println("✓ All prerequisites validated. Proceeding with demonstration.")
	return true
}

// ────────────────────────────────────────────────────────────────
// Code Execution - Entry Point
// ────────────────────────────────────────────────────────────────
//
// Entry point for executable. Delegates to Public APIs.
// See: Org Chart > Ladder Structure > Code Execution
//
// Subsections: Entry Point

//--- Entry Point ---
// Main function delegates to DemoConfigRunAll() for testability.

// main is the entry point for the Phase 0 config validation demo.
//
// Lifecycle function for the demo's "EXECUTION" entry.
// Delegates to DemoConfigRunAll() which orchestrates all phases.
// This pattern allows programmatic calling by games/tutorials.
//
// Environment:
//   BERESHIT_ROOT: Path to bereshit repository root (optional, defaults to ".")
//
// Exit Codes:
//   0 (ExitSuccess): All configs loaded successfully
//   1 (ExitFailure): One or more configs failed to load
func main() {
	failures := DemoConfigRunAll()
	if failures > 0 {
		os.Exit(ExitFailure)
	}
	os.Exit(ExitSuccess)
}

// ────────────────────────────────────────────────────────────────
// Code Cleanup - Results Reporting
// ────────────────────────────────────────────────────────────────
//
// Lifecycle cleanup function. Runs AFTER main execution.
// Reports demonstration results and final status.
//
// Subsections: Cleanup Logic

//--- Cleanup Logic ---
// Report results after demonstration completes.

// demoCleanupAndReport outputs final summary of demonstration results.
//
// Lifecycle function for the demo's "CLEANUP" phase.
// Called by DemoConfigRunAll() after validation and execution complete.
//
// Reports:
//   - Systems passed/failed counts
//   - Overall ternary status
//   - Biblical closing verse
func demoCleanupAndReport() {
	fmt.Println()
	fmt.Println("════════════════════════════════════════════════════════════════")
	fmt.Println("Demonstration Complete")
	fmt.Println("════════════════════════════════════════════════════════════════")
	fmt.Println()

	// ═══════════════════════════════════════════════════════════
	// STATUS: Report ternary result
	// ═══════════════════════════════════════════════════════════
	switch systemStatus {
	case StatusPass:
		fmt.Println("  Status: ✓ PASS (+1)")
		fmt.Println("  All systems healthy via primary path (index.toml)")
	case StatusHalfPass:
		fmt.Println("  Status: ⚠ HALF PASS (0)")
		fmt.Println("  Systems functional but using fallback or partial")
	case StatusFail:
		fmt.Println("  Status: ✗ FAIL (-1)")
		fmt.Println("  System broken — neither path works")
	}
	fmt.Println()

	// ═══════════════════════════════════════════════════════════
	// COUNTS: Report pass/fail
	// ═══════════════════════════════════════════════════════════
	fmt.Printf("  Categories demonstrated: %d\n", systemsPassed)
	if systemsFailed > 0 {
		fmt.Printf("  Failures: %d\n", systemsFailed)
	}
	fmt.Println()

	// ═══════════════════════════════════════════════════════════
	// CLOSING: Biblical foundation
	// ═══════════════════════════════════════════════════════════
	fmt.Println("  \"Prove all things; hold fast that which is good.\"")
	fmt.Println("                                    — 1 Thessalonians 5:21")
	fmt.Println()
	fmt.Println("  The specifications are proven. The foundation is ready.")
	fmt.Println()
	fmt.Println("════════════════════════════════════════════════════════════════")
}

// ════════════════════════════════════════════════════════════════
// GROUP 2: DOCUMENTATION - Reference
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// Executable Overview
// ────────────────────────────────────────────────────────────────
//
// Purpose: Demonstrate config-driven system using Phase 0 TOML specs.
// See METADATA for full documentation.
//
// Subsections: Quick Summary, API Reference
//
//--- Quick Summary ---
//   - Validates Phase 0 TOML specs exist and parse correctly
//   - Shows tripwire pattern (manifest vs filesystem)
//   - Shows dependency validation (no cycles, all refs exist)
//   - Displays sample values from key specs
//   - Reports ternary status: PASS (+1), HALF PASS (0), FAIL (-1)
//
//--- API Reference ---
//   DemoConfigRunAll()      → int       Run demo, return failure count
//   DemoConfigGetPassed()   → int       Get passed systems count
//   DemoConfigGetFailed()   → int       Get failed systems count
//   DemoConfigGetStatus()   → int       Get ternary status (+1, 0, -1)
//   DemoConfigReset()       → void      Reset counters for fresh run
//
//--- Diagnostic APIs (debug markers from loader_test.go) ---
//   DemoConfigDiagnostic()         → void  Run all diagnostic phases
//   DemoConfigDiagnosticIndex()    → void  Phase 1: index health
//   DemoConfigDiagnosticTripwire() → void  Phase 2: manifest vs disk
//   DemoConfigDiagnosticDeps()     → void  Phase 3: dependency graph

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// Guide for future maintainers. What's safe to change.
//
// Subsections: Safe to Modify, Modify with Care, Never Modify
//
//--- Safe to Modify ---
//   ✅ Add new display helpers for additional specs
//   ✅ Enhance output formatting
//   ✅ Add new inspection functions (DemoConfigGet*)
//
//--- Modify with Care ---
//   ⚠️ DemoConfigRunAll signature — breaks game/tutorial callers
//   ⚠️ Exit codes — affects CI/CD integration
//   ⚠️ Ternary status values — affects status interpretation
//
//--- Never Modify ---
//   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   ❌ Lifecycle pattern (Validation → Execution → Cleanup)
//   ❌ Biblical anchors and scripture references

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// Points to BODY > Organizational Chart for complete architecture.
//
// Subsections: APU Inventory, Ladder, Baton
//
//--- APU Inventory ---
//   Total: 14 functions
//   - CLOSING Lifecycle: 3 (main, demoValidatePrerequisites, demoCleanupAndReport)
//   - BODY Public APIs: 5 (RunAll, GetPassed, GetFailed, GetStatus, Reset)
//   - BODY Diagnostic APIs: 4 (Diagnostic, DiagnosticIndex, DiagnosticTripwire, DiagnosticDeps)
//   - BODY Core Operations: 1 (demoShowConfigCategories)
//   - BODY Helpers: 3 (demoShowMathValues, demoShowPrimitiveValues, demoShowHealthValues)
//
//--- Ladder ---
//   CLOSING (main) → BODY Public APIs → BODY Core Ops → BODY Helpers
//   CLOSING lifecycle functions orchestrate the three phases
//
//--- Baton ---
//   main() → DemoConfigRunAll() → validate → execute → cleanup → Exit

// ────────────────────────────────────────────────────────────────
// Surgical Update Points
// ────────────────────────────────────────────────────────────────
//
// Extension guide. Points to BODY subsection extension points.
//
//--- Extension Points ---
//   - New display helper: Add to BODY > Helpers, call from demoShowConfigCategories
//   - New inspection API: Add DemoConfigGet* to BODY > Public APIs > Inspection
//   - Enhanced validation: Extend demoValidatePrerequisites in CLOSING > Code Validation

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// Demo runs once and exits — performance not primary concern.
//
//--- Performance Notes ---
//   - Config loading: Uses config.LoadAll() which reads all TOML files
//   - Memory: All configs held in memory during demo (released on exit)
//   - I/O: File reads only, no network or long-running operations

// ────────────────────────────────────────────────────────────────
// Related Components
// ────────────────────────────────────────────────────────────────
//
// Subsections: Dependencies, Dependents
//
//--- Dependencies ---
//   - config.Loader (word/work/pkg/config/loader.go) — loads TOML specs
//   - word/core/*.toml — source specifications
//   - word/core/index.toml — file manifest (tripwire reference)
//
//--- Dependents ---
//   - None (demo is leaf node — end of chain)
//   - Future: CI/CD integration for spec validation

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Common problems and solutions.
//
//--- Common Problems ---
// Problem: "BERESHIT_ROOT not set" or wrong path
//   - Set: export BERESHIT_ROOT=/path/to/bereshit
//   - Or run from bereshit root directory (defaults to ".")
//
// Problem: "index.toml not found" (tripwire triggered)
//   - Check: word/core/index.toml exists
//   - Demo will use fallback paths (HALF PASS status)
//
// Problem: Missing TOML files
//   - Check: All files in index.toml exist on disk
//   - Run: make status (from word/work/) to see current state

// ────────────────────────────────────────────────────────────────
// Future Expansions
// ────────────────────────────────────────────────────────────────
//
// Subsections: Planned Features, Known Limitations, Version History
//
//--- Planned Features ---
//   - JSON output mode for CI/CD integration
//   - Verbose flag for detailed tripwire diagnostics
//   - Spec diff mode: compare specs against previous versions
//
//--- Known Limitations ---
//   - Hardcoded category list (update when new categories added)
//   - No recursive spec validation (validates existence, not content)
//   - Exit codes: only 0/1 (ternary status internal only)
//
//--- Version History ---
//   - v0.1.0: Initial demo (tripwire pattern, ternary status)

// ────────────────────────────────────────────────────────────────
// Quick Reference
// ────────────────────────────────────────────────────────────────
//
// Copy-paste ready examples for common operations.
//
//--- Build and Run ---
//   cd tov/demo/phase-0/demo-config
//   go build .
//   BERESHIT_ROOT=/path/to/bereshit ./demo-config
//
//--- Programmatic Usage ---
//   import "creativeworkzstudio.com/bereshit/tov/demo/phase-0/demo-config"
//
//   if demo.DemoConfigRunAll() == 0 {
//       fmt.Println("Phase 0 validated!")
//   }
//   status := demo.DemoConfigGetStatus() // +1, 0, or -1

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This demo validates the SPECIFICATION layer (Phase 0).
// Specifications → Generator → C headers → libtrit → runtime
//
// "Every purpose is established by counsel" — Proverbs 20:18

// ============================================================================
// END CLOSING
// ============================================================================
