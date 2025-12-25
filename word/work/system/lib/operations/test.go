// ============================================================================
// METADATA
// ============================================================================
// Operations Testing Library - CPI-SI Interactive Terminal System
// Purpose: Test safe and protected operations to verify system functionality
// Non-blocking: Safe operations that validate configuration
//
// HEALTH SCORING MAP (TRUE SCORE):
// ----------------------------------
// Each test operation is weighted individually:
//
// TestSafeOperation():
//   - Operation succeeds without password: +10
//   - Operation fails or requires password: -10
//
// TestProtectedOperation():
//   - Correctly requires password/denied: +10
//   - Incorrectly allows without password: -10 (SAFETY BREACH)
//
// TestSafetyBoundary():
//   - Correctly blocks operation: +10
//   - Incorrectly allows operation: -10 (CRITICAL SAFETY BREACH)
//
// RunStandardTests() executes multiple tests:
//   - Per-test scoring: each test contributes ±10
//   - Total varies based on number of tests run
//   - Normalization: (cumulative_health / total_tests*10) × 100

package operations

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"os/exec"
	"strings"
	"time"

	"system/lib/debugging"
	"system/lib/logging"
)

// Package-level shared logger for operations component
// Ensures health accumulates across all test operations
var operationsLogger *logging.Logger

func init() {
	operationsLogger = logging.NewLogger("operations")
}

// TestResult represents the result of an operation test
type TestResult struct {
	Operation    string
	Description  string
	Expected     string
	Success      bool
	Output       string
	Error        string
	Duration     time.Duration
	SafetyLevel  string // "safe", "protected", "boundary"
}

// ============================================================================
// BODY
// ============================================================================

// TestSafeOperation tests a safe operation (should work without password)
func TestSafeOperation(operation, description string, args ...string) TestResult {
	inspector := debugging.NewInspector("operations")
	inspector.Enable() // Enable debugging to capture HOW data
	operationsLogger.Operation(operation, 0, args...)

	// DEBUGGING: Capture test parameters
	inspector.Snapshot("safe-operation-start", map[string]any{
		"operation":    operation,
		"description":  description,
		"args":         args,
		"expected":     "success without password",
		"safety_level": "safe",
	})

	start := time.Now()
	result := TestResult{
		Operation:   operation,
		Description: description,
		Expected:    "Success without password prompt",
		SafetyLevel: "safe",
	}

	cmd := exec.Command(operation, args...)
	output, err := cmd.CombinedOutput()
	result.Duration = time.Since(start)
	result.Output = string(output)

	// DEBUGGING: Capture execution results
	inspector.Snapshot("safe-operation-executed", map[string]any{
		"operation":     operation,
		"duration_ms":   result.Duration.Milliseconds(),
		"has_error":     err != nil,
		"output_length": len(output),
	})

	if err != nil {
		result.Error = err.Error()
		// Check if error is password-related
		passwordDetected := strings.Contains(result.Output, "password") ||
			strings.Contains(result.Error, "password")

		// DEBUGGING: Capture password detection analysis
		inspector.ExpectedState("safe-operation-password-check", false, passwordDetected, map[string]any{
			"operation":      operation,
			"has_error":      true,
			"error_message":  result.Error,
			"output_length":  len(result.Output),
		})

		if passwordDetected {
			result.Success = false
			result.Error = "Password prompt detected - safe operation failed"
			operationsLogger.Failure(description, result.Error, -10, map[string]any{
				"operation":    operation,
				"args":         strings.Join(args, " "),
				"duration_ms":  result.Duration.Milliseconds(),
				"safety_level": "safe",
			})
		} else {
			// Other errors might be acceptable (e.g., package not found)
			result.Success = false
			operationsLogger.Failure(description, result.Error, -10, map[string]any{
				"operation":    operation,
				"args":         strings.Join(args, " "),
				"duration_ms":  result.Duration.Milliseconds(),
				"safety_level": "safe",
				"output":       result.Output,
			})
		}
	} else {
		result.Success = true
		operationsLogger.Success(description, 10, map[string]any{
			"operation":    operation,
			"args":         strings.Join(args, " "),
			"duration_ms":  result.Duration.Milliseconds(),
			"safety_level": "safe",
		})
	}

	// DEBUGGING: Capture final test result
	inspector.ExpectedState("safe-operation-result", true, result.Success, map[string]any{
		"operation":     operation,
		"duration_ms":   result.Duration.Milliseconds(),
		"had_error":     err != nil,
		"error_message": result.Error,
	})

	return result
}

// TestProtectedOperation tests a protected operation (should require password)
func TestProtectedOperation(operation, description string, args ...string) TestResult {
	inspector := debugging.NewInspector("operations")
	inspector.Enable() // Enable debugging to capture HOW data
	operationsLogger.Operation("sudo -n "+operation, 0, args...)

	// DEBUGGING: Capture test parameters
	inspector.Snapshot("protected-operation-start", map[string]any{
		"operation":    operation,
		"description":  description,
		"args":         args,
		"expected":     "password required or denied",
		"safety_level": "protected",
	})

	start := time.Now()
	result := TestResult{
		Operation:   operation,
		Description: description,
		Expected:    "Password prompt or explicit denial",
		SafetyLevel: "protected",
	}

	// Use sudo -n (non-interactive) to test without actually prompting
	fullArgs := append([]string{"-n", operation}, args...)
	cmd := exec.Command("sudo", fullArgs...)
	output, err := cmd.CombinedOutput()
	result.Duration = time.Since(start)
	result.Output = string(output)

	// DEBUGGING: Capture execution results
	inspector.Snapshot("protected-operation-executed", map[string]any{
		"operation":     operation,
		"duration_ms":   result.Duration.Milliseconds(),
		"has_error":     err != nil,
		"output_length": len(output),
	})

	if err != nil {
		result.Error = err.Error()
		// For protected operations, password requirement is success
		passwordRequired := strings.Contains(result.Output, "password") ||
			strings.Contains(result.Error, "password") ||
			strings.Contains(result.Output, "no tty present") ||
			strings.Contains(result.Output, "interactive authentication")
		explicitDenial := strings.Contains(result.Output, "not allowed")

		// DEBUGGING: Capture protection validation
		inspector.ExpectedState("protected-operation-validation", "password or denied", result.Output, map[string]any{
			"operation":         operation,
			"has_error":         true,
			"password_required": passwordRequired,
			"explicit_denial":   explicitDenial,
			"error_message":     result.Error,
		})

		if passwordRequired {
			result.Success = true // Correctly requiring password
			operationsLogger.Success(description+" - correctly requires password", 10, map[string]any{
				"operation":    operation,
				"args":         strings.Join(args, " "),
				"duration_ms":  result.Duration.Milliseconds(),
				"safety_level": "protected",
			})
		} else if explicitDenial {
			result.Success = true // Explicit denial is also correct
			operationsLogger.Success(description+" - correctly denied", 10, map[string]any{
				"operation":    operation,
				"args":         strings.Join(args, " "),
				"duration_ms":  result.Duration.Milliseconds(),
				"safety_level": "protected",
			})
		} else {
			result.Success = false
			operationsLogger.Failure(description, "unexpected error", -10, map[string]any{
				"operation":    operation,
				"args":         strings.Join(args, " "),
				"duration_ms":  result.Duration.Milliseconds(),
				"safety_level": "protected",
				"error":        result.Error,
				"output":       result.Output,
			})
		}
	} else {
		// If command succeeded without password, that's a failure for protected ops
		result.Success = false
		result.Error = "Protected operation succeeded without password - safety boundary breach"

		// DEBUGGING: Capture safety breach
		inspector.ExpectedState("protected-operation-breach", "error", "success", map[string]any{
			"operation":     operation,
			"safety_breach": true,
			"duration_ms":   result.Duration.Milliseconds(),
		})

		operationsLogger.Error(description, fmt.Errorf("SAFETY BREACH: %s", result.Error), -10)
	}

	// DEBUGGING: Capture final test result
	inspector.ExpectedState("protected-operation-result", true, result.Success, map[string]any{
		"operation":     operation,
		"duration_ms":   result.Duration.Milliseconds(),
		"had_error":     err != nil,
		"error_message": result.Error,
	})

	return result
}

// TestSafetyBoundary tests an explicit safety boundary (should be denied)
func TestSafetyBoundary(operation, description string, args ...string) TestResult {
	inspector := debugging.NewInspector("operations")
	inspector.Enable() // Enable debugging to capture HOW data
	operationsLogger.Operation("sudo -n "+operation, 0, args...)

	// DEBUGGING: Capture test parameters
	inspector.Snapshot("safety-boundary-start", map[string]any{
		"operation":    operation,
		"description":  description,
		"args":         args,
		"expected":     "explicit denial",
		"safety_level": "boundary",
	})

	start := time.Now()
	result := TestResult{
		Operation:   operation,
		Description: description,
		Expected:    "Explicit denial",
		SafetyLevel: "boundary",
	}

	fullArgs := append([]string{"-n", operation}, args...)
	cmd := exec.Command("sudo", fullArgs...)
	output, err := cmd.CombinedOutput()
	result.Duration = time.Since(start)
	result.Output = string(output)

	// DEBUGGING: Capture execution results
	inspector.Snapshot("safety-boundary-executed", map[string]any{
		"operation":     operation,
		"duration_ms":   result.Duration.Milliseconds(),
		"has_error":     err != nil,
		"output_length": len(output),
	})

	if err != nil {
		result.Error = err.Error()
		// Safety boundary should explicitly deny or require authentication
		explicitlyDenied := strings.Contains(result.Output, "not allowed") ||
			strings.Contains(result.Error, "not allowed") ||
			strings.Contains(result.Output, "interactive authentication")

		// DEBUGGING: Capture denial validation
		inspector.ExpectedState("safety-boundary-validation", true, explicitlyDenied, map[string]any{
			"operation":      operation,
			"has_error":      true,
			"error_message":  result.Error,
			"output_excerpt": result.Output[:min(100, len(result.Output))],
		})

		if explicitlyDenied {
			result.Success = true // Correct denial
			operationsLogger.Success(description+" - correctly blocked by safety boundary", 10, map[string]any{
				"operation":    operation,
				"args":         strings.Join(args, " "),
				"duration_ms":  result.Duration.Milliseconds(),
				"safety_level": "boundary",
			})
		} else {
			result.Success = false
			operationsLogger.Failure(description, "unexpected result", -10, map[string]any{
				"operation":    operation,
				"args":         strings.Join(args, " "),
				"duration_ms":  result.Duration.Milliseconds(),
				"safety_level": "boundary",
				"error":        result.Error,
				"output":       result.Output,
			})
		}
	} else {
		result.Success = false
		result.Error = "Safety boundary breach - operation was not denied"

		// DEBUGGING: Capture critical safety breach
		inspector.ExpectedState("safety-boundary-breach", "error/denial", "success", map[string]any{
			"operation":            operation,
			"critical_breach":      true,
			"duration_ms":          result.Duration.Milliseconds(),
			"output_length":        len(result.Output),
		})

		operationsLogger.Error(description, fmt.Errorf("CRITICAL SAFETY BREACH: %s", result.Error), -10)
	}

	// DEBUGGING: Capture final test result
	inspector.ExpectedState("safety-boundary-result", true, result.Success, map[string]any{
		"operation":     operation,
		"duration_ms":   result.Duration.Milliseconds(),
		"had_error":     err != nil,
		"error_message": result.Error,
	})

	return result
}

// ============================================================================
// CLOSING
// ============================================================================

// RunStandardTests executes a standard suite of operation tests
func RunStandardTests() []TestResult {
	inspector := debugging.NewInspector("operations")
	inspector.Enable() // Enable debugging to capture HOW data

	// DEBUGGING: Capture test suite start
	inspector.Snapshot("test-suite-start", map[string]any{
		"safe_tests":     2,
		"protected_tests": 1,
		"boundary_tests":  1,
		"total_tests":     4,
	})

	results := make([]TestResult, 0)

	// Safe operations (should work without password)
	// NOTE: sudo-rs (0.2.8+) intentionally does not support wildcards in command arguments
	// for security reasons. Commands must exactly match sudoers entries.
	// See: sudo-rs design decisions on argument wildcards.
	results = append(results, TestSafeOperation(
		"sudo", "Update package lists",
		"apt", "update",
	))

	results = append(results, TestSafeOperation(
		"sudo", "Check service status",
		"systemctl", "status", "docker",
	))

	// Protected operations (should require password or be denied)
	results = append(results, TestProtectedOperation(
		"mkfs.ext4", "Format filesystem (protected)",
		"--help",
	))

	// Safety boundaries (should be explicitly denied)
	results = append(results, TestSafetyBoundary(
		"apt", "Remove essential package (safety boundary)",
		"remove", "systemd",
	))

	// DEBUGGING: Capture test suite results
	passed := 0
	failed := 0
	for _, r := range results {
		if r.Success {
			passed++
		} else {
			failed++
		}
	}
	inspector.Snapshot("test-suite-complete", map[string]any{
		"total_tests":  len(results),
		"passed":       passed,
		"failed":       failed,
		"pass_rate":    float64(passed) / float64(len(results)) * 100,
	})

	return results
}

// GenerateReport creates a formatted test report
func GenerateReport(results []TestResult) string {
	var report strings.Builder

	passed := 0
	failed := 0

	for _, r := range results {
		if r.Success {
			passed++
		} else {
			failed++
		}
	}

	report.WriteString(fmt.Sprintf("\nOperation Test Results: %d passed, %d failed\n\n", passed, failed))

	for _, r := range results {
		status := "✓ PASS"
		if !r.Success {
			status = "✗ FAIL"
		}

		report.WriteString(fmt.Sprintf("%s [%s] %s\n", status, r.SafetyLevel, r.Description))
		if !r.Success && r.Error != "" {
			report.WriteString(fmt.Sprintf("      Error: %s\n", r.Error))
		}
	}

	return report.String()
}
