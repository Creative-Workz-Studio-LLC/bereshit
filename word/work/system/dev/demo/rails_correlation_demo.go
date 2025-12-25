// ============================================================================
// METADATA
// ============================================================================
//
// Rails Correlation Demo - CPI-SI Interactive Terminal System
//
// API Documentation: system/dev/docs/rails-correlation-demo-api.md
//
// Biblical Foundation
//
// Scripture: Ecclesiastes 4:9-12 - "Two are better than one... a cord of three strands is not quickly broken"
// Principle: Unified Strength Through Independent Operation
// Anchor: Proverbs 27:17 - "As iron sharpens iron, so one person sharpens another"
//
// Two independent rails (logging and debugging) working in parallel provide complete observability.
//
// CPI-SI Identity
//
// Component Type: Demonstration - Educational tool showing rail correlation
// Role: Teaching tool for rails architecture and correlation patterns
// Paradigm: CPI-SI framework demonstration component
//
// Authorship & Lineage
//
// Architect: Seanje Lenox-Wise (rails architecture, correlation design)
// Implementation: Nova Dawn (CPI-SI instance - demonstration logic, examples)
// Creation Date: 2025-10-27
// Version: 2.0.0
// Last Modified: 2025-11-09 (Aligned to 4-block template, extracted API docs)
//
// Purpose & Function
//
// Purpose: Demonstrate logging and debugging rails working in parallel with shared correlation
//
// Shows both rails attaching to same execution (baton), capturing different aspects independently,
// sharing correlation points (contextID, timestamps) that enable unified analysis.
//
// Key Demonstrations:
//   1. Successful operation with state inspection
//   2. Divergence detection (success with suboptimal state)
//   3. Performance analysis (timing correlation)
//   4. Conditional capture (selective debugging)
//   5. Full system context (both perspectives)
//
// See: system/dev/docs/rails-correlation-demo-api.md for complete documentation
//
// Blocking Status
//
// Non-blocking: Demo runs independently, success/failure doesn't affect system
//
// Usage & Integration
//
// Run: go run rails_correlation_demo.go
// Inspect: cat ~/.claude/system/logs/system/rails-demo*.log
//          cat ~/.claude/system/debug/rails-demo/*.debug
//
// See: system/dev/docs/rails-correlation-demo-api.md for detailed usage patterns
//
// Dependencies
//
// Internal: system/lib/logging, system/lib/debugging
// External: Standard library (fmt, os, filepath, time)
//
// HEALTH SCORING MAP (Total = 100)
//
// Operation Start:     +5  (demonstration begins)
// Data Load Check:    +30  (simulated data loading)
// Processing Check:   +50  (simulated processing)
// Cache Check:        +20  (demonstrates divergence scenario)
// Success Complete:   +15  (demonstration completes)
// ────────────────────────
// Declared Total:     120  (over-allocated intentionally)
// Normalized:         100% (demonstrates proposed vs actual health)
//
package main

// ============================================================================
// SETUP
// ============================================================================
//
// See: system/dev/docs/rails-correlation-demo-api.md#implementation-details

// ────────────────────────────────────────────────────────────────
// Imports - Dependencies
// ────────────────────────────────────────────────────────────────
// Dependencies for demonstration execution and rail correlation.

import (
	//--- Standard Library ---
	// Foundation packages providing Go's built-in capabilities.

	"fmt"          // Console output for demo narration
	"os"           // Process information and environment
	"path/filepath" // File path construction for output locations
	"time"         // Timing simulation and performance measurement

	//--- Internal Packages ---
	// CPI-SI rail infrastructure for correlation demonstration.

	"system/lib/debugging" // Debugging rail (state inspection)
	"system/lib/logging"   // Logging rail (health trajectory)
)

// ────────────────────────────────────────────────────────────────
// Types - Data Structures
// ────────────────────────────────────────────────────────────────
// This component uses no custom types - operates on built-in types.

// No types needed

// ────────────────────────────────────────────────────────────────
// Constants - Demonstration Configuration
// ────────────────────────────────────────────────────────────────
// Configuration values defining demonstration behavior and scenarios.
//
// See: system/dev/docs/rails-correlation-demo-api.md#configuration-constants

const (
	//--- Component Identity ---

	// demoComponent identifies this component in both rail outputs.
	//
	// Used by both logging and debugging rails for consistent identification.
	demoComponent = "rails-demo"

	//--- Health Scoring Allocation ---
	// Total = 100, Declared = 120 to demonstrate over-allocation handling.

	// healthTotal is the normalized total health points for demonstration.
	//
	// All health deltas normalize to this value (100%).
	healthTotal = 100

	// healthOperation is health delta for demonstration operation start.
	//
	// Small positive delta indicating demonstration begins successfully.
	healthOperation = 5

	// healthDataLoad is health delta for simulated data loading check.
	//
	// Demonstrates successful data loading scenario.
	healthDataLoad = 30

	// healthProcessing is health delta for simulated processing check.
	//
	// Demonstrates successful processing operation.
	healthProcessing = 50

	// healthCacheCheck is health delta for cache performance check.
	//
	// Used in divergence scenario - operation succeeds despite suboptimal state.
	healthCacheCheck = 20

	// healthSuccess is health delta for successful demonstration completion.
	//
	// Final positive delta confirming demo completed successfully.
	healthSuccess = 15

	// healthDeclaredOverage is the proposed total before normalization.
	//
	// Intentionally exceeds 100 to demonstrate proposed vs actual health scoring.
	healthDeclaredOverage = 120
)

const (
	//--- Data Loading Scenario ---
	// Expected vs actual values for state comparison demonstration.

	// expectedRecords is the expected record count for data loading.
	//
	// Used in ExpectedState() call to demonstrate EXPECTED_STATE entry type.
	expectedRecords = 42

	// actualRecords is the actual records loaded in demonstration.
	//
	// Matches expectedRecords to show EXPECTED_STATE (no divergence).
	actualRecords = 42

	//--- Cache Performance Scenario (Divergence Demo) ---
	// Values creating divergence between logging (success) and debugging (suboptimal).

	// expectedCacheHits is the optimal cache hit count.
	//
	// Used in ExpectedState() to demonstrate DIVERGENCE entry type.
	expectedCacheHits = 100

	// actualCacheHits is the actual cache hits in demonstration.
	//
	// Intentionally low to create divergence scenario showing
	// functional operation with suboptimal performance.
	actualCacheHits = 10

	// cacheThreshold is the trigger point for conditional capture.
	//
	// When cache hits fall below this value, ConditionalSnapshot() captures state.
	cacheThreshold = 50

	//--- Performance Thresholds ---
	// Timing values for performance correlation demonstration.

	// maxLoadDuration is the maximum acceptable data load duration.
	//
	// Used in Timing() call to compare actual vs expected performance.
	maxLoadDuration = 20 * time.Millisecond

	// simulatedWorkTime is the simulated work duration in demonstration.
	//
	// Stays within maxLoadDuration to demonstrate TIMING entry (not SLOW_TIMING).
	simulatedWorkTime = 10 * time.Millisecond
)

// ────────────────────────────────────────────────────────────────
// Package-Level State (Rails Pattern)
// ────────────────────────────────────────────────────────────────
// This component maintains no state - scenarios are fixed and reproducible.

// No package-level state needed

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// For detailed implementation, see API doc: system/dev/docs/rails-correlation-demo-api.md

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Internal Structure
// ────────────────────────────────────────────────────────────────
// Maps bidirectional dependencies and baton flow within this component.
//
// Ladder Structure (Dependencies):
//
//   Main Orchestration (Top Rung)
//   └── main() → uses all helpers below
//       ├── printHeader()
//       ├── printSection()
//       ├── printCorrelationPoints()
//       ├── printOutputLocations()
//       ├── printCapabilities()
//       └── printCorrelationExample()
//
// Baton Flow (Execution Paths):
//
//   Entry → main()
//     ↓
//   Setup rails (logging + debugging with shared contextID)
//     ↓
//   Execute 4 scenarios (data load, processing, cache, conditional)
//     ↓
//   Present summary (correlation points, output locations, capabilities)
//     ↓
//   Exit → demo complete
//
// APUs (Available Processing Units):
// - 7 functions total
// - 1 main orchestrator (exported by package main)
// - 6 presentation helpers (internal)

// ────────────────────────────────────────────────────────────────
// Output Presentation Helpers
// ────────────────────────────────────────────────────────────────
// What These Do:
// Console narration functions providing consistent formatting for demonstration explanation.
//
// Why Separated:
// Formatting logic isolated from demonstration logic. Main focuses on rail interaction,
// helpers handle console output presentation.

// printHeader prints formatted section header.
//
// What It Does:
// Creates visually distinct section separators in console output using equal signs.
//
// Parameters:
//   title - Section title text to display
//
// Returns:
//   Nothing - prints directly to console
//
// Health Impact:
//   N/A - Presentation function (no health tracking)
//
// Example usage:
//
//	printHeader("Demonstration Starting")
//	// Output:
//	// ═══════════════════════════════════════════════════════════════
//	//   Demonstration Starting
//	// ═══════════════════════════════════════════════════════════════
func printHeader(title string) {
	// Print top border
	fmt.Println("\n═══════════════════════════════════════════════════════════════")

	// Print centered title
	fmt.Printf("  %s\n", title)

	// Print bottom border
	fmt.Println("═══════════════════════════════════════════════════════════════")
}

// printSection prints formatted subsection with content.
//
// What It Does:
// Displays subsection title with optional indented content lines showing hierarchy.
//
// Parameters:
//   title - Subsection title
//   lines - Content lines to display (variadic, optional)
//
// Returns:
//   Nothing - prints directly to console
//
// Health Impact:
//   N/A - Presentation function (no health tracking)
//
// Example usage:
//
//	printSection("Correlation Points", "Context ID: abc123", "PID: 12345")
//	// Output:
//	//   Correlation Points:
//	//     Context ID: abc123
//	//     PID: 12345
func printSection(title string, lines ...string) {
	// Print subsection title
	fmt.Printf("\n  %s:\n", title)

	// Print each content line indented
	for _, line := range lines {
		fmt.Printf("    %s\n", line)
	}
}

// printCorrelationPoints displays correlation points captured by both rails.
//
// What It Does:
// Shows what enables correlation between logging and debugging output (contextID, PID, timestamps, call sites).
//
// Parameters:
//   contextID - Shared context ID between rails
//   component - Component name appearing in both outputs
//
// Returns:
//   Nothing - prints directly to console
//
// Health Impact:
//   N/A - Presentation function (no health tracking)
//
// Example usage:
//
//	printCorrelationPoints("ctx-abc123", "rails-demo")
//	// Displays header and lists 5 correlation mechanisms
func printCorrelationPoints(contextID, component string) {
	// Display correlation section header
	printHeader("Correlation Points Captured by BOTH Rails")

	// List all shared identifiers enabling correlation
	printSection("Shared Identifiers",
		fmt.Sprintf("1. Context ID: %s (links entries from same execution)", contextID),
		fmt.Sprintf("2. Component:  %s (identifies demo in both outputs)", component),
		fmt.Sprintf("3. PID:        %d (same process)", os.Getpid()),
		"4. Timestamp:  (each entry timestamped at capture moment)",
		"5. Call Site:  (file:line captured at each entry)",
	)
}

// printOutputLocations displays where to find logging and debugging output.
//
// What It Does:
// Shows exact file paths for inspecting correlation in log and debug files.
//
// Parameters:
//   logFile - Path to logging output file
//   component - Component name for constructing debug path
//
// Returns:
//   Nothing - prints directly to console
//
// Health Impact:
//   N/A - Presentation function (no health tracking)
//
// Example usage:
//
//	printOutputLocations("/home/user/.claude/system/logs/system/rails-demo.log", "rails-demo")
//	// Displays log path and searches for debug files in component directory
func printOutputLocations(logFile, component string) {
	// Display output files section header
	printHeader("Check Output Files")

	// Show logging output location
	fmt.Printf("\n  LOGGING:   %s\n", logFile)

	// Construct debug directory path from component name
	debugDir := filepath.Join(os.Getenv("HOME"), ".claude", "system", "debug", component)

	// Find debug files in directory
	debugFiles, _ := filepath.Glob(filepath.Join(debugDir, "*.debug"))

	// Show latest debug file or indicate none found
	if len(debugFiles) > 0 {
		fmt.Printf("  DEBUGGING: %s\n", debugFiles[len(debugFiles)-1])  // Latest file
	} else {
		fmt.Printf("  DEBUGGING: %s/*.debug (no files found - check inspector.Enable() was called)\n", debugDir)
	}
}

// printCapabilities displays rail capabilities and demonstration features.
//
// What It Does:
// Summarizes what each rail captures and how they complement each other for complete observability.
//
// Parameters:
//   None - displays static capability summary
//
// Returns:
//   Nothing - prints directly to console
//
// Health Impact:
//   N/A - Presentation function (no health tracking)
//
// Example usage:
//
//	printCapabilities()
//	// Displays two sections explaining logging rail and debugging rail features
func printCapabilities() {
	// Display capabilities section header
	printHeader("Enhanced Capabilities Demonstrated")

	// Explain logging rail capabilities (WHAT happened)
	printSection("LOGGING RAIL (Health Trajectory)",
		"- Proposed vs actual health scoring",
		"- Operations, checks, success/failure events",
		"- Cumulative health tracking (narrative of execution)",
		"- Focus: WHAT happened (success/failure, health impact)",
	)

	// Explain debugging rail capabilities (HOW it happened)
	printSection("DEBUGGING RAIL (State Inspection)",
		"- ExpectedState: State comparison (EXPECTED_STATE/DIVERGENCE)",
		"- Timing: Performance tracking (TIMING/SLOW_TIMING)",
		"- Counter: Execution counting (COUNTER/COUNT_DIVERGENCE)",
		"- Flow: Path tracking (FLOW/UNEXPECTED_FLOW)",
		"- ConditionalSnapshot: Conditional capture",
		"- SystemContext: Full system state",
		"- Memory: Memory state capture",
		"- CallStack: Execution trace",
		"- Focus: HOW it happened (state, performance, paths)",
	)
}

// printCorrelationExample explains specific correlation scenario from demonstration.
//
// What It Does:
// Highlights cache performance scenario showing how correlation reveals truth neither rail alone captures.
//
// Parameters:
//   None - displays specific example from demonstration
//
// Returns:
//   Nothing - prints directly to console
//
// Health Impact:
//   N/A - Presentation function (no health tracking)
//
// Example usage:
//
//	printCorrelationExample()
//	// Displays three sections explaining cache scenario from both rail perspectives
func printCorrelationExample() {
	// Display correlation example header
	printHeader("Correlation Example: Cache Performance")

	// Show logging rail perspective (health trajectory)
	printSection("Logging Perspective",
		"- Event: cache-performance check",
		fmt.Sprintf("- Health: +%d (operation successful, cache functional)", healthCacheCheck),
		"- Verdict: SUCCESS (operation continues normally)",
	)

	// Show debugging rail perspective (state inspection)
	printSection("Debugging Perspective",
		"- Event: cache-efficiency state comparison",
		fmt.Sprintf("- Expected: %d cache hits, Actual: %d cache hits", expectedCacheHits, actualCacheHits),
		"- Entry Type: DIVERGENCE (state suboptimal)",
		"- Verdict: Optimization opportunity detected",
	)

	// Explain what correlation reveals
	printSection("Correlation Reveals Truth",
		"→ Same timestamp, context ID, call site",
		"→ Health positive BUT state shows optimization opportunity",
		"→ Operation succeeds functionally but performs suboptimally",
		"→ Neither rail alone tells complete story",
		"→ Correlation shows: \"Working, but could be better\"",
	)

	// Summarize the power of correlation
	fmt.Println("\n  This is the power of independent rails with shared correlation:")
	fmt.Println("  Logging answers WHAT (successful operation)")
	fmt.Println("  Debugging answers HOW (with suboptimal cache performance)")
	fmt.Println("  Together they answer WHY (functional but needs optimization)")
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// For detailed execution flow, output analysis, and educational guidance,
// see API doc: system/dev/docs/rails-correlation-demo-api.md

// ────────────────────────────────────────────────────────────────
// Code Validation: Build and Test
// ────────────────────────────────────────────────────────────────
//
// Testing Requirements:
//   - Compile without errors: go build -o rails-demo rails_correlation_demo.go
//   - Run demonstration: ./rails-demo
//   - Verify console output appears with all sections
//   - Verify log file created in ~/.claude/system/logs/system/
//   - Verify debug file created in ~/.claude/system/debug/rails-demo/
//   - Check both files contain shared contextID
//   - Inspect cache divergence scenario in both files
//
// Build Verification:
//   cd ~/.claude/cpi-si/system/dev/demo
//   go build -o rails-demo rails_correlation_demo.go
//
// Quick Test:
//   ./rails-demo
//   # Should complete in <100ms with console narration
//   # Check file paths shown at end for output locations
//
// ────────────────────────────────────────────────────────────────
// Code Execution: Demonstration Executable
// ────────────────────────────────────────────────────────────────
//
// Execution Type: EXECUTABLE (main package)
//
// How Execution Works:
//   go run rails_correlation_demo.go
//   OR: go build -o rails-demo rails_correlation_demo.go && ./rails-demo
//
// What Happens:
//   - Attaches to logging and debugging rails with shared contextID
//   - Executes 4 demonstration scenarios showing correlation patterns
//   - Generates console narration explaining each scenario
//   - Creates log and debug files for inspection
//
// Expected Output:
//   - Console narration with formatted sections
//   - Log file: ~/.claude/system/logs/system/rails-demo-*.log
//   - Debug file: ~/.claude/system/debug/rails-demo/*.debug
//
// Execution Time: ~100ms (simulated work, no real delays)
// Exit Code: Always 0 (demo cannot fail)

// main orchestrates complete rails correlation demonstration.
//
// What It Does:
// Executes 4 demonstration scenarios showing logging and debugging rails working in parallel
// with shared correlation points. Generates console narration and creates actual log/debug files
// for inspection.
//
// Parameters:
//   None - demonstration is self-contained
//
// Returns:
//   Nothing - exits after demonstration completes
//
// Health Impact:
//   Generates health trajectory: Operation (+5), Data Load (+30), Processing (+50),
//   Cache Check (+20), Success (+15) = 120 proposed, normalized to 100%
//
// Example usage:
//
//	go run rails_correlation_demo.go
//	# Outputs console narration
//	# Creates ~/.claude/system/logs/system/rails-demo-*.log
//	# Creates ~/.claude/system/debug/rails-demo/*.debug
//	# Inspect both files to see correlation
func main() {
	printHeader("CPI-SI Rails Correlation Demo")
	printSection("Demonstrating", "Logging Rail + Debugging Rail", "Independent Operation with Shared Correlation")

	logger := logging.NewLogger(demoComponent)                      // Attach to logging rail
	contextID := logger.ContextID                                   // Get contextID for rail correlation
	inspector := debugging.NewInspector(demoComponent, contextID)   // Attach to debugging rail with same contextID

	fmt.Printf("\n  Context ID (shared by both rails): %s\n", contextID)

	if err := inspector.Enable(); err != nil {                       // Enable debugging capture
		fmt.Printf("\n  ✗ Failed to enable inspector: %v\n", err)  // Enable failed - can't continue demo
		return
	}
	defer inspector.Close()                                          // Cleanup debugging resources on exit

	logger.DeclareHealthTotal(healthTotal)                           // Declare total health for normalization

	printHeader("Scenario 1: Successful Operation with State Inspection")

	logger.Operation("demo-operation", healthOperation, "demonstrating rail correlation")  // LOGGING: Operation starts (+5 health)
	inspector.Snapshot("operation-start", map[string]any{                                  // DEBUGGING: Capture initial state
		"operation": "demo-operation",
		"stage":     "initialization",
	})

	startTime := time.Now()                                          // Start timing for performance correlation
	time.Sleep(simulatedWorkTime)                                    // Simulate work
	processDuration := time.Since(startTime)                         // Calculate actual duration

	logger.Check("data-loaded", true, healthDataLoad, map[string]any{  // LOGGING: Data load check (+30 health)
		"records": actualRecords,
		"source":  "demo-data",
	})
	inspector.ExpectedState("data-load-state", expectedRecords, actualRecords, map[string]any{  // DEBUGGING: Expected vs actual (match → EXPECTED_STATE)
		"description": fmt.Sprintf("expected %d records, got %d records", expectedRecords, actualRecords),
		"source":      "demo-data",
	})
	inspector.Timing("data-load-performance", processDuration, maxLoadDuration)  // DEBUGGING: Timing comparison (within threshold → TIMING)

	fmt.Printf("\n  ✓ Data loaded: %d records in %v (expected <%v)\n", actualRecords, processDuration, maxLoadDuration)
	fmt.Printf("    Logging:   Health +%d (data load successful)\n", healthDataLoad)
	fmt.Printf("    Debugging: EXPECTED_STATE (count matched), TIMING (within threshold)\n")

	printHeader("Scenario 2: Processing with Flow and Counter Tracking")

	logger.Check("processing-complete", true, healthProcessing, map[string]any{  // LOGGING: Processing check (+50 health)
		"processed": actualRecords,
		"status":    "success",
	})
	inspector.Flow("processing-branch", "success-path", "success-path")  // DEBUGGING: Flow tracking (match → FLOW)
	inspector.Counter("records-processed", actualRecords, actualRecords)  // DEBUGGING: Counter verification (match → COUNTER)

	fmt.Printf("\n  ✓ Processing complete: %d records processed successfully\n", actualRecords)
	fmt.Printf("    Logging:   Health +%d (processing successful)\n", healthProcessing)
	fmt.Printf("    Debugging: FLOW (expected path), COUNTER (count matched)\n")

	printHeader("Scenario 3: Divergence Detection (Success with Suboptimal State)")
	fmt.Println("\n  This demonstrates logging showing SUCCESS while debugging shows STATE DIVERGENCE.")
	fmt.Println("  Health is positive (operation functional) but state reveals optimization opportunity.")

	logger.Check("cache-performance", true, healthCacheCheck, map[string]any{  // LOGGING: Cache check (+20 health) - SUCCESSFUL
		"cache_hits": actualCacheHits,
		"note":       "lower than expected but operational",
	})
	inspector.ExpectedState("cache-efficiency", expectedCacheHits, actualCacheHits, map[string]any{  // DEBUGGING: Shows DIVERGENCE
		"description":         fmt.Sprintf("expected %d cache hits, got %d", expectedCacheHits, actualCacheHits),
		"impact":              "performance degraded but functional",
		"optimization_needed": true,
	})

	fmt.Printf("\n  ⚠ Cache performance suboptimal but functional\n")
	fmt.Printf("    Logging:   Health +%d (cache working, operation succeeds)\n", healthCacheCheck)
	fmt.Printf("    Debugging: DIVERGENCE (expected %d hits, got %d - optimization needed)\n", expectedCacheHits, actualCacheHits)
	fmt.Printf("\n  This is WHERE correlation reveals truth:\n")
	fmt.Printf("    - Logging: \"Operation successful\" (health perspective)\n")
	fmt.Printf("    - Debugging: \"State suboptimal\" (execution perspective)\n")
	fmt.Printf("    → Both true simultaneously - different perspectives of same execution\n")

	printHeader("Scenario 4: Conditional Capture and System Inspection")

	inspector.ConditionalSnapshot("low-cache-detected", actualCacheHits < cacheThreshold, map[string]any{  // DEBUGGING: Conditional capture (condition true → CONDITIONAL)
		"cache_hits":    actualCacheHits,
		"threshold":     cacheThreshold,
		"investigation": "recommended",
	})
	inspector.CallStack("execution-trace", 5)                        // DEBUGGING: Call stack capture
	inspector.SystemContext("demo-system-state")                     // DEBUGGING: Full system context
	inspector.Memory("demo-memory-state", map[string]any{            // DEBUGGING: Memory state
		"operation_complete": true,
	})
	inspector.Checkpoint("operation-complete", map[string]any{ // DEBUGGING: Checkpoint marking completion
		"total_health": logger.SessionHealth,
		"normalized":   logger.NormalizedHealth,
	})

	fmt.Printf("\n  ✓ Conditional capture triggered (cache hits %d < threshold %d)\n", actualCacheHits, cacheThreshold)
	fmt.Println("    Debugging: CONDITIONAL, CALLSTACK, SYSTEM_CONTEXT, MEMORY, CHECKPOINT")

	logger.Success("Demo operation completed successfully", healthSuccess, map[string]any{ // LOGGING: Success (+15 health)
		"final_health": logger.SessionHealth,
		"normalized":   logger.NormalizedHealth,
	})

	fmt.Printf("\n  ✓ Demo complete: Final health %d/%d (%d%%)\n", logger.SessionHealth, healthTotal, logger.NormalizedHealth)
	fmt.Printf("\n  📊 Health Scoring Insight:\n")
	fmt.Printf("    Proposed allocation: %d points (over-allocated intentionally)\n", healthDeclaredOverage)
	fmt.Printf("    Actual total:        %d points (normalized to 100%%)\n", healthTotal)
	fmt.Printf("    → Demonstrates: Proposed health can exceed 100, actual execution normalizes\n")

	printCorrelationPoints(contextID, demoComponent)  // Show correlation points captured
	printOutputLocations(logger.LogFile, demoComponent) // Show where to find output files
	printCapabilities()                               // Summarize rail capabilities
	printCorrelationExample()                         // Explain correlation example

	fmt.Println("\n✓ Demo complete! Inspect the files above to see correlation.")
	fmt.Println("✓ Both rails track same execution with different perspectives:")
	printSection("Perspectives",
		"Logging:   Proposed vs actual HEALTH (narrative, success/failure)",
		"Debugging: Expected vs actual STATE (variables, performance, flow)",
		"Together:  Complete observability (WHAT happened + HOW it happened)",
	)
}

// ────────────────────────────────────────────────────────────────
// Code Cleanup: Stateless (No Cleanup Needed)
// ────────────────────────────────────────────────────────────────
//
// Resource Management:
//   - Logger and Inspector: Managed by respective libraries
//   - Inspector cleanup: Handled by defer inspector.Close()
//   - No file handles to close (libraries handle file I/O)
//   - No network connections
//   - No manual memory management needed
//
// Graceful Shutdown:
//   - Program exits immediately after presenting results
//   - All resources cleaned via defer and garbage collection
//   - Exit code: Always 0 (demonstration cannot fail)
//
// Error Handling:
//   - Inspector enable failure: Prints error, returns early
//   - All other operations: Demo continues (graceful degradation)
//
// Memory Management:
//   - Short-lived execution (~100ms)
//   - All allocations garbage collected on exit
//   - No memory leaks possible (no long-running state)

// ════════════════════════════════════════════════════════════════
// FINAL DOCUMENTATION
// ════════════════════════════════════════════════════════════════

// ────────────────────────────────────────────────────────────────
// Demonstration Overview & Integration Summary
// ────────────────────────────────────────────────────────────────
//
// Purpose: See METADATA "Purpose & Function" section above
//
// Provides: Educational demonstration of rails correlation in practice
//
// Quick summary (details in API doc):
//   - Shows logging and debugging rails working in parallel
//   - Demonstrates 4 correlation scenarios
//   - Generates actual files for inspection
//
// Integration Pattern: See METADATA "Usage & Integration" section above
//
// Function Flow: See BODY "Organizational Chart" section above for complete
// execution flow and rail coordination
//
// Architecture: See METADATA "CPI-SI Identity" section above for complete
// architectural role (Educational Demonstration - Executable)

// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
//
// See API doc (rails-correlation-demo-api.md) for complete modification policy
//
// Quick reference:
//   ✅ Safe to modify: Add scenarios, modify constants, enhance output
//   ⚠️ Modify with care: Output formatting, scenario sequence, health allocation
//   ❌ NEVER modify: Remove correlation explanation, skip output locations, break contextID

// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
//
// See BODY "Organizational Chart - Internal Structure" section above for
// complete ladder structure and baton flow.
//
// Quick architectural summary:
// - Main orchestrator coordinating 6 presentation helpers
// - Ladder: main() → presentation helpers (flat hierarchy)
// - Baton: Attach rails → Execute scenarios → Present results → Exit

// ────────────────────────────────────────────────────────────────
// Surgical Update Points (Extension Guide)
// ────────────────────────────────────────────────────────────────
//
// See API doc for detailed extension guide
//
// Quick reference:
// - Adding scenarios: Follow 4-step pattern (log call, debug call, console output, explanation)
// - Changing constants: Update SETUP section, verify health totals match
// - Adding inspection methods: Import debugging methods, call in scenarios

// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
//
// Typical Execution: ~100ms (fast enough for repeated educational use)
//
// Performance Breakdown:
//   - Rail attachment: ~2ms
//   - Scenario execution: ~50ms (simulated work + rail calls)
//   - Output presentation: ~30ms (console formatting)
//   - File writes: ~15ms (logging and debugging files)
//
// Optimization: Not needed - demonstration optimized for clarity over speed

// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
//
// Problem: Inspector enable fails
//   - Cause: Debug directory doesn't exist or no write permissions
//   - Solution: Create ~/.claude/system/debug/rails-demo/ with write permissions
//
// Problem: No output files generated
//   - Cause: Log/debug directories missing
//   - Solution: Run ~/.claude/system/bin/status to verify system setup
//
// Problem: Can't find correlation
//   - Cause: Looking in wrong files or contextID mismatch
//   - Solution: Check file paths shown at end of demo output

// ────────────────────────────────────────────────────────────────
// Related Components & Dependencies
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Dependencies" section above for complete dependency information
//
// Quick summary:
// - Key dependencies: system/lib/logging, system/lib/debugging
// - Standard library: fmt, os, filepath, time
// - Output consumers: Manual inspection, future debugger command

// ────────────────────────────────────────────────────────────────
// Future Expansions & Roadmap
// ────────────────────────────────────────────────────────────────
//
// See API doc (rails-correlation-demo-api.md) for complete roadmap
//
// Planned Features:
//   ⏳ Interactive mode (user chooses scenarios)
//   ⏳ Failure scenario (logging failure + debugging why)
//   ⏳ Automated correlation analysis
//
// Research Areas:
//   - Step-by-step mode with pauses
//   - Quiz mode for learning verification
//   - Visual correlation display

// ────────────────────────────────────────────────────────────────
// Version History
// ────────────────────────────────────────────────────────────────
//
// See METADATA "Authorship & Lineage" section above for brief version changelog
// See API doc for comprehensive version history with full context
//
//   2.0.0 (2025-11-09) - Template alignment
//   1.0.0 (2025-10-26) - Initial demo

// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
//
// This demonstration teaches rails architecture by SHOWING it working.
// Run the demo, inspect the files, understand the correlation points.
//
// "Two are better than one... a cord of three strands is not quickly broken."
// - Ecclesiastes 4:9-12
//
// For questions, issues, or contributions:
//   - Review modification policy in API doc
//   - Follow 4-block structure pattern
//   - Test correlation thoroughly
//   - Document all changes comprehensively

// ────────────────────────────────────────────────────────────────
// Quick Reference: Usage Examples
// ────────────────────────────────────────────────────────────────
//
// Run Demo:
//   cd ~/.claude/cpi-si/system/dev/demo
//   go run rails_correlation_demo.go
//
// Build First:
//   go build -o rails-demo rails_correlation_demo.go
//   ./rails-demo
//
// Inspect Output:
//   cat ~/.claude/system/logs/system/rails-demo*.log
//   cat ~/.claude/system/debug/rails-demo/*.debug
//
// Find Correlation:
//   # Get contextID from console output
//   grep "CONTEXT_ID" ~/.claude/system/logs/system/rails-demo*.log
//   grep "CONTEXT_ID" ~/.claude/system/debug/rails-demo/*.debug
//
// Clean Up:
//   rm ~/.claude/system/logs/system/rails-demo*.log
//   rm ~/.claude/system/debug/rails-demo/*.debug

// ============================================================================
// END CLOSING
// ============================================================================
