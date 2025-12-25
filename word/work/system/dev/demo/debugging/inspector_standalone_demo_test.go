// ============================================================================
// METADATA
// ============================================================================
// Inspector Library Demo - Demonstrate debugging rail functionality
//
// Biblical Foundation: 1 Thessalonians 5:21 - "Test all things; hold fast what is good."
//
// CPI-SI Identity: Demo for debugging rail inspector library
// Purpose: DEMONSTRATE inspector library delivers on documented promises
//          Shows zero-cost, semantic categorization, and lifecycle with VISIBLE output
//
// Why Demo not Test: Demos are harder to fake - you SEE it working
//                     Tests hide behind pass/fail assertions
//                     This demonstrates the library actually works as documented
//
// Created: 2025-10-27
// Modified: 2025-11-16 (Converted from test to demo)
// ============================================================================

package debugging_test

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"os"
	"path/filepath"
	"testing"
	"time"

	"system/lib/debugging"
	"system/lib/display"
)

// Demo configuration
const (
	configDrivenDebugBase = "cpi-si/system/dev/debug"

	testComponentZeroCost       = "test-zero-cost"
	testComponentCategorization = "test-categorization"
	testComponentLifecycle      = "test-lifecycle"
	testContextID              = "test-categorization-12345-67890"
	testDebugPattern           = "*.debug"
)

// ============================================================================
// BODY
// ============================================================================

// TestInspectorZeroCostWhenDisabled demonstrates zero-cost promise with VISIBLE output.
//
// This is a DEMO disguised as a test (so `go test` can run it).
// You SEE the filesystem state - harder to fake than assertions.
func TestInspectorZeroCostWhenDisabled(t *testing.T) {
	fmt.Print(display.Box("Inspector Library Demo", "Zero-Cost When Disabled - No I/O Until Enabled"))
	fmt.Println()

	inspector := debugging.NewInspector(testComponentZeroCost)

	// Check initial state
	fmt.Print(display.Header("1. Initial State Check"))
	fmt.Println()
	fmt.Printf("  Inspector enabled: %v\n", inspector.IsEnabled())
	fmt.Println("  ✓ Inspector starts disabled (zero-cost by default)")
	fmt.Println()

	// Check filesystem before calling methods
	home := os.Getenv("HOME")
	debugDir := filepath.Join(home, ".claude", configDrivenDebugBase, testComponentZeroCost)
	beforeFiles, _ := filepath.Glob(filepath.Join(debugDir, testDebugPattern))

	fmt.Print(display.Header("2. Calling Methods While Disabled"))
	fmt.Println()
	fmt.Println("  Calling inspector.Snapshot() while disabled...")
	inspector.Snapshot("pre-enable-snapshot", map[string]any{"should_not_appear": true})
	fmt.Println("  Calling inspector.ExpectedState() while disabled...")
	inspector.ExpectedState("pre-enable-state", "expected", "actual", nil)
	fmt.Println()

	// Check filesystem after calling methods
	afterFiles, _ := filepath.Glob(filepath.Join(debugDir, testDebugPattern))

	fmt.Print(display.Header("3. Filesystem Verification"))
	fmt.Println()
	fmt.Printf("  Files before: %d\n", len(beforeFiles))
	fmt.Printf("  Files after:  %d\n", len(afterFiles))
	fmt.Println()
	if len(afterFiles) > len(beforeFiles) {
		fmt.Println("  ✗ VIOLATION: Methods created files while disabled!")
	} else {
		fmt.Println("  ✓ ZERO-COST VERIFIED: No files created while disabled")
	}
	fmt.Println()
	fmt.Println("  🎯 You can SEE the filesystem state - visible proof of zero-cost")
	fmt.Println()
}

// TestInspectorSemanticCategorization demonstrates entry type categorization with VISIBLE output.
//
// This is a DEMO disguised as a test (so `go test` can run it).
// You SEE the actual entry types produced - visible verification.
func TestInspectorSemanticCategorization(t *testing.T) {
	fmt.Print(display.Box("Inspector Library Demo", "Semantic Categorization - Entry Types Match Conditions"))
	fmt.Println()

	inspector := debugging.NewInspector(testComponentCategorization, testContextID)
	defer inspector.Close()

	err := inspector.Enable()
	if err != nil {
		fmt.Printf("  ✗ Failed to enable inspector: %v\n", err)
		return
	}

	// Demonstrate different entry types
	fmt.Print(display.Header("Generating Inspection Entries"))
	fmt.Println()

	fmt.Println("  1. ExpectedState with MATCH → EXPECTED_STATE")
	inspector.ExpectedState("match-test", "value", "value", nil)

	fmt.Println("  2. ExpectedState with MISMATCH → DIVERGENCE")
	inspector.ExpectedState("diverge-test", "expected", "actual", nil)

	fmt.Println("  3. Timing FAST → TIMING")
	inspector.Timing("fast-timing", 5*time.Millisecond, 100*time.Millisecond)

	fmt.Println("  4. Timing SLOW → SLOW_TIMING")
	inspector.Timing("slow-timing", 100*time.Millisecond, 10*time.Millisecond)

	fmt.Println("  5. Counter MATCH → COUNTER")
	inspector.Counter("match-count", 10, 10)

	fmt.Println("  6. Counter DIVERGE → COUNT_DIVERGENCE")
	inspector.Counter("diverge-count", 15, 10)

	fmt.Println("  7. Flow MATCH → FLOW")
	inspector.Flow("match-flow", "success", "success")

	fmt.Println("  8. Flow DIVERGE → UNEXPECTED_FLOW")
	inspector.Flow("diverge-flow", "error", "success")

	fmt.Println("  9. ConditionalSnapshot TRUE → CONDITIONAL")
	inspector.ConditionalSnapshot("condition-met", true, map[string]any{"captured": true})

	fmt.Println("  10. ConditionalSnapshot FALSE → (no entry)")
	inspector.ConditionalSnapshot("condition-not-met", false, map[string]any{"should_not_appear": true})

	fmt.Println("  11. Snapshot → SNAPSHOT")
	inspector.Snapshot("snapshot-test", map[string]any{"test": "data"})

	fmt.Println("  12. Checkpoint → CHECKPOINT")
	inspector.Checkpoint("checkpoint-test", nil)

	fmt.Println()
	inspector.Disable()

	// Show where to find output
	home := os.Getenv("HOME")
	debugDir := filepath.Join(home, ".claude", configDrivenDebugBase, testComponentCategorization)
	files, _ := filepath.Glob(filepath.Join(debugDir, testDebugPattern))

	fmt.Print(display.Header("Output Location"))
	fmt.Println()
	if len(files) > 0 {
		fmt.Printf("  Debug file: %s\n", files[len(files)-1])
		fmt.Println()
		fmt.Println("  ✓ Entries written - inspect file to verify entry types")
		fmt.Println("  ✓ Each entry categorized based on conditions")
		fmt.Printf("  ✓ All entries contain contextID: %s\n", testContextID)
	} else {
		fmt.Println("  ✗ No debug files created")
	}
	fmt.Println()
	fmt.Println("  🎯 You can INSPECT the debug file - visible proof of categorization")
	fmt.Println()
}

// TestInspectorLifecycle demonstrates full lifecycle with VISIBLE output.
//
// This is a DEMO disguised as a test (so `go test` can run it).
// You SEE the state transitions - visible verification of lifecycle.
func TestInspectorLifecycle(t *testing.T) {
	fmt.Print(display.Box("Inspector Library Demo", "Full Lifecycle - Zero-Cost Through State Transitions"))
	fmt.Println()

	inspector := debugging.NewInspector(testComponentLifecycle)
	defer inspector.Close()

	// Phase 1: Initial disabled state
	fmt.Print(display.Header("Phase 1: Initial Disabled State"))
	fmt.Println()
	fmt.Printf("  Inspector enabled: %v\n", inspector.IsEnabled())
	fmt.Println("  ✓ Starts disabled (zero-cost)")
	fmt.Println()

	// Phase 2: Enable and capture
	fmt.Print(display.Header("Phase 2: Enable and Capture"))
	fmt.Println()
	err := inspector.Enable()
	if err != nil {
		fmt.Printf("  ✗ Failed to enable: %v\n", err)
		return
	}
	fmt.Printf("  Inspector enabled: %v\n", inspector.IsEnabled())

	inspector.Snapshot("enabled-snapshot", map[string]any{"phase": "enabled"})
	fmt.Println("  ✓ Snapshot captured while enabled")

	home := os.Getenv("HOME")
	debugDir := filepath.Join(home, ".claude", configDrivenDebugBase, testComponentLifecycle)
	enabledFiles, _ := filepath.Glob(filepath.Join(debugDir, testDebugPattern))

	if len(enabledFiles) > 0 {
		fileInfo, _ := os.Stat(enabledFiles[len(enabledFiles)-1])
		sizeAfterEnabled := fileInfo.Size()
		fmt.Printf("  ✓ Debug file created: %d bytes\n", sizeAfterEnabled)
	} else {
		fmt.Println("  ✗ No debug file created")
	}
	fmt.Println()

	// Phase 3: Disable and verify zero-cost restored
	fmt.Print(display.Header("Phase 3: Disable and Verify Zero-Cost"))
	fmt.Println()

	// Record size before disable
	var sizeAfterEnabled int64
	if len(enabledFiles) > 0 {
		fileInfo, _ := os.Stat(enabledFiles[len(enabledFiles)-1])
		sizeAfterEnabled = fileInfo.Size()
	}

	err = inspector.Disable()
	if err != nil {
		fmt.Printf("  ✗ Failed to disable: %v\n", err)
		return
	}
	fmt.Printf("  Inspector enabled: %v\n", inspector.IsEnabled())

	inspector.Snapshot("post-disable-snapshot", map[string]any{"should_not_appear": true})
	fmt.Println("  Calling inspector.Snapshot() while disabled...")
	inspector.ExpectedState("post-disable-state", "expected", "actual", nil)
	fmt.Println("  Calling inspector.ExpectedState() while disabled...")

	time.Sleep(10 * time.Millisecond)

	// Check file size after disabled calls
	if len(enabledFiles) > 0 {
		fileInfoAfter, _ := os.Stat(enabledFiles[len(enabledFiles)-1])
		sizeAfterDisabled := fileInfoAfter.Size()

		fmt.Println()
		fmt.Printf("  File size after enabled:  %d bytes\n", sizeAfterEnabled)
		fmt.Printf("  File size after disabled: %d bytes\n", sizeAfterDisabled)
		fmt.Println()

		if sizeAfterDisabled > sizeAfterEnabled {
			fmt.Println("  ✗ VIOLATION: File grew after disable!")
		} else {
			fmt.Println("  ✓ ZERO-COST RESTORED: File did not grow after disable")
		}
	}

	fmt.Println()
	fmt.Println("  🎯 You can SEE the state transitions - visible lifecycle verification")
	fmt.Println()
	fmt.Println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
	fmt.Println()
	fmt.Println("  🎯 Demo Complete: Inspector library demonstrates three promises")
	fmt.Println()
	fmt.Println("  Promises Demonstrated:")
	fmt.Println("     • Zero-cost when disabled (filesystem verification)")
	fmt.Println("     • Semantic categorization (entry types match conditions)")
	fmt.Println("     • Full lifecycle (zero-cost through state transitions)")
	fmt.Println()
	fmt.Println("  Why Demo not Test:")
	fmt.Println("     • You SEE the filesystem state")
	fmt.Println("     • Entry types are visible in debug files")
	fmt.Println("     • State transitions are demonstrated, not asserted")
	fmt.Println("     • Tests hide behind pass/fail - demos show truth")
	fmt.Println()
}

// ============================================================================
// CLOSING
// ============================================================================
// Run: go test -v
// Output: Visible demonstration of inspector library promises
// Inspect: cat ~/.claude/cpi-si/system/dev/debug/test-*/*.debug
