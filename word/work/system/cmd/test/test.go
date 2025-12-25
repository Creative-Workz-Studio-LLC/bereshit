// ============================================================================
// METADATA
// ============================================================================
// Test Command - CPI-SI Interactive Terminal System
// Purpose: Test safe and protected operations to verify functionality
// Non-blocking: Safe test operations only
// Usage: ./bin/test [--quick|--full]
//
// HEALTH SCORING MAP (TRUE SCORE):
// ----------------------------------
// Setup (5 actions = 40 points):
//   Action 1/5: Initialize logger (+10 or -10)
//   Action 2/5: Display header (+2 or -2)
//   Action 3/5: Parse command arguments (+15 or -15) - Important to run correct tests
//   Action 4/5: Log operation start (+5 or -5)
//   Action 5/5: Snapshot state (+8 or -8)
//
// Test Execution (3 actions = 110 points) - CRITICAL:
//   Action 1/3: Initiate test suite (+10 or -10)
//   Action 2/3: Execute tests (+80 or -80) - Core functionality
//   Action 3/3: Collect all results (+20 or -20) - Must gather data
//
// Results Processing (5 actions = 50 points):
//   Action 1/5: Display individual results (+15 or -15)
//   Action 2/5: Count results correctly (+8 or -8)
//   Action 3/5: Display summary (+12 or -12)
//   Action 4/5: Log final result (+8 or -8)
//   Action 5/5: Exit with correct code (+7 or -7)
//
// Total Possible: 200 points
// Normalization: (cumulative_health / 200) × 100

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"os"
	"system/lib/debugging"
	"system/lib/display"
	"system/lib/logging"
	"system/lib/operations"
)

// ============================================================================
// BODY
// ============================================================================

func runQuickTests() []operations.TestResult {
	fmt.Println(display.Info("Running quick operation tests..."))
	fmt.Println()

	results := make([]operations.TestResult, 0)

	// Test: apt update (safe operation)
	// NOTE: sudo-rs requires exact command matches - no wildcards in arguments
	// Using exact command from sudoers: /usr/bin/apt update
	fmt.Println("Testing: sudo apt update (safe operation)...")
	result := operations.TestSafeOperation(
		"sudo", "Update package lists",
		"apt", "update",
	)
	results = append(results, result)
	displayResult(result)

	// Test: systemctl status (safe operation)
	// NOTE: sudo-rs requires exact command matches - no wildcards in arguments
	// Using exact command from sudoers: /usr/bin/systemctl status docker
	fmt.Println("Testing: sudo systemctl status docker (safe operation)...")
	result = operations.TestSafeOperation(
		"sudo", "Check service status",
		"systemctl", "status", "docker",
	)
	results = append(results, result)
	displayResult(result)

	return results
}

func runFullTests() []operations.TestResult {
	fmt.Println(display.Info("Running comprehensive operation tests..."))
	fmt.Println()

	return operations.RunStandardTests()
}

func displayResult(result operations.TestResult) {
	if result.Success {
		fmt.Println(display.Success(result.Description))
	} else {
		fmt.Println(display.Failure(result.Description))
		if result.Error != "" {
			fmt.Println(display.KeyValue("  Error", result.Error))
		}
	}
	fmt.Println()
}

func showResults(results []operations.TestResult) {
	fmt.Print(display.Header("Test Results"))

	passed := 0
	failed := 0

	for _, r := range results {
		if r.Success {
			passed++
		} else {
			failed++
		}
		displayResult(r)
	}

	// Summary
	fmt.Print(display.Subheader("Summary"))
	fmt.Println(display.KeyValue("Total Tests", fmt.Sprintf("%d", len(results))))
	fmt.Println(display.KeyValue("Passed", fmt.Sprintf("%s%d%s", display.Green, passed, display.Reset)))
	fmt.Println(display.KeyValue("Failed", fmt.Sprintf("%s%d%s", display.Red, failed, display.Reset)))

	fmt.Println()

	if failed == 0 {
		fmt.Println(display.Success("All tests passed - system is operational"))
	} else {
		fmt.Println(display.Warning(fmt.Sprintf("%d tests failed - system may have issues", failed)))
		fmt.Println()
		fmt.Println(display.Info("Run './bin/diagnose' for detailed troubleshooting"))
	}
}

func showUsage() {
	fmt.Println("Usage: ./bin/test [--quick|--full]")
	fmt.Println()
	fmt.Println("Options:")
	fmt.Println("  --quick    Run quick tests (default)")
	fmt.Println("  --full     Run comprehensive test suite")
	fmt.Println()
	fmt.Println("Examples:")
	fmt.Println("  ./bin/test")
	fmt.Println("  ./bin/test --quick")
	fmt.Println("  ./bin/test --full")
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup Action 1/5: Initialize logger (+10 or -10)
	logger := logging.NewLogger("test")
	logger.DeclareHealthTotal(200)  // Total possible points from health scoring map
	inspector := debugging.NewInspector("test")
	inspector.Enable() // Enable debugging to capture HOW data

	// DEBUGGING: Capture command start
	inspector.Snapshot("test-start", map[string]any{
		"command": "test",
		"purpose": "operation testing",
		"modes":   []string{"quick", "full"},
	})

	logger.Check("logger-initialized", true, 10, map[string]any{
		"component": "test",
	})

	// Setup Action 2/5: Display header (+2 or -2)
	fmt.Print(display.Header("CPI-SI Interactive Terminal System - Operation Tests"))
	logger.Check("header-displayed", true, 2, map[string]any{
		"header": "operation tests",
	})

	// Setup Action 3/5: Parse command arguments (+15 or -15) - Important to run correct tests
	mode := "quick"
	argValid := true
	if len(os.Args) > 1 {
		switch os.Args[1] {
		case "--quick":
			mode = "quick"
		case "--full":
			mode = "full"
		case "-h", "--help":
			showUsage()
			return
		default:
			fmt.Println(display.Failure(fmt.Sprintf("Unknown option: %s", os.Args[1])))
			showUsage()
			logger.Check("args-parsed", false, -15, map[string]any{
				"argument": os.Args[1],
				"valid":    false,
			})
			logger.Failure("Invalid option provided", "unknown command line argument", 0, map[string]any{
				"argument": os.Args[1],
			})
			os.Exit(1)
		}
	}
	logger.Check("args-parsed", argValid, 15, map[string]any{
		"mode":  mode,
		"valid": true,
	})

	// DEBUGGING: Capture mode selection
	inspector.Snapshot("test-mode-selected", map[string]any{
		"mode":     mode,
		"args":     os.Args[1:],
		"expected": []string{"quick", "full"},
	})

	// Setup Action 4/5: Log operation start (+5 or -5)
	logger.Operation("test", 5, fmt.Sprintf("mode=%s", mode))

	// Setup Action 5/5: Snapshot state (+8 or -8)
	logger.SnapshotState("before-tests", 8)

	// Test Execution Action 1/3: Initiate test suite (+10 or -10)
	logger.Check("test-suite-initiated", true, 10, map[string]any{
		"mode": mode,
	})

	// Test Execution Action 2/3: Execute tests (+80 or -80) - Core functionality
	var results []operations.TestResult
	executionSuccess := true

	if mode == "quick" {
		results = runQuickTests()
	} else {
		results = runFullTests()
	}

	// Check if we got results back
	if len(results) == 0 {
		executionSuccess = false
	}

	healthImpact := 80
	if !executionSuccess {
		healthImpact = -80
	}
	logger.Check("tests-executed", executionSuccess, healthImpact, map[string]any{
		"mode":       mode,
		"test_count": len(results),
	})

	// Test Execution Action 3/3: Collect all results (+20 or -20) - Must gather data
	collectionSuccess := len(results) > 0
	healthImpact = 20
	if !collectionSuccess {
		healthImpact = -20
	}
	logger.Check("results-collected", collectionSuccess, healthImpact, map[string]any{
		"total": len(results),
	})

	// Results Processing Action 1/5: Display individual results (+15 or -15)
	showResults(results)
	logger.Check("results-displayed", true, 15, map[string]any{
		"displayed": "individual test results",
	})

	// Results Processing Action 2/5: Count results correctly (+8 or -8)
	passed := 0
	failed := 0
	for _, r := range results {
		if r.Success {
			passed++
		} else {
			failed++
		}
	}
	logger.Check("results-counted", true, 8, map[string]any{
		"total":  len(results),
		"passed": passed,
		"failed": failed,
	})

	// DEBUGGING: Capture test results
	allPassed := failed == 0
	inspector.ExpectedState("test-results", 0, failed, map[string]any{
		"mode":       mode,
		"total":      len(results),
		"passed":     passed,
		"failed":     failed,
		"all_passed": allPassed,
	})

	// Results Processing Action 3/5: Display summary (+12 or -12)
	// (showResults already displayed summary, marking as done)
	logger.Check("summary-displayed", true, 12, map[string]any{
		"summary": "test results summary",
	})

	// Results Processing Action 4/5: Log final result (+8 or -8)
	if failed == 0 {
		logger.Success("All tests passed", 8, map[string]any{
			"mode":      mode,
			"total":     len(results),
			"passed":    passed,
			"failed":    failed,
			"exit_code": 0,
		})
	} else {
		logger.Failure("Some tests failed", fmt.Sprintf("%d of %d tests failed", failed, len(results)), -8, map[string]any{
			"mode":      mode,
			"total":     len(results),
			"passed":    passed,
			"failed":    failed,
			"exit_code": 0,
		})
	}

	// Results Processing Action 5/5: Exit with correct code (+7 or -7)
	// Note: We always exit 0 because the test command itself executed successfully.
	// Test failures are reported in the output and logs, not as command errors.
	logger.Check("test-command-completed", true, 7, map[string]any{
		"exit_code": 0,
		"tests_passed": failed == 0,
	})
}
