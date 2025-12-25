// ============================================================================
// METADATA
// ============================================================================
// Divergence Demo - CPI-SI Immune System Test
// Purpose: Intentionally create health divergences to test debugger detection
// Non-blocking: Educational demonstration only
//
// HEALTH SCORING MAP (TRUE SCORE):
// ----------------------------------
// Test 1: Partial Success (+30 expected, +10 actual) - medium severity
// Test 2: Complete Failure (+40 expected, 0 actual) - critical severity
// Test 3: Unexpected Failure (+20 expected, -5 actual) - high severity
// Test 4: Perfect Match (+15 expected, +15 actual) - no divergence
// Test 5: Over-performance (+10 expected, +25 actual) - medium severity
//
// Total Possible: 115 points
// Normalization: (cumulative_health / 115) × 100

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"system/lib/logging"
)

// ============================================================================
// BODY
// ============================================================================

func main() {
	logger := logging.NewLogger("divergence-demo")
	logger.DeclareHealthTotal(115)

	// Test 1: Partial Success - got 10 instead of expected 30
	// Using standard Check for backward compatibility test
	logger.Check("partial-success-test", true, 10, map[string]any{
		"expected": 30,
		"actual":   10,
		"note":     "intentional divergence for testing",
	})

	// Test 2: Complete Failure with semantic metadata - got 0 instead of expected 40
	logger.CheckWithMetadata("complete-failure-test", false, 0, map[string]any{
		"expected": 40,
		"actual":   0,
		"note":     "intentional complete failure",
	}, logging.Metadata{
		OperationType:    "file_validation",
		OperationSubtype: "permission_check",
		ErrorType:        "permission_denied",
		ErrorDetails: map[string]any{
			"file":          "/etc/sudoers.d/test-file",
			"required_mode": "0440",
			"actual_mode":   "0000",
		},
		RecoveryHint:     "automated_fix",
		RecoveryStrategy: "fix_file_permissions",
		RecoveryParams: map[string]any{
			"file":         "/etc/sudoers.d/test-file",
			"target_mode":  "0440",
			"target_owner": "root:root",
		},
	})

	// Test 3: Unexpected Failure with semantic metadata - got -5 instead of expected 20
	logger.FailureWithMetadata("unexpected-failure-test", "intentional failure", -5, map[string]any{
		"expected": 20,
		"actual":   -5,
	}, logging.Metadata{
		OperationType:    "system_operation",
		OperationSubtype: "package_install",
		ErrorType:        "missing_dependency",
		ErrorDetails: map[string]any{
			"package":      "test-package",
			"dependency":   "test-dependency",
			"apt_error":    "Package not found",
		},
		RecoveryHint:     "install_dependency",
		RecoveryStrategy: "install_package",
		RecoveryParams: map[string]any{
			"package":       "test-dependency",
			"check_command": "dpkg -l test-dependency",
		},
	})

	// Test 4: Perfect Match with semantic metadata - got 15 as expected
	logger.CheckWithMetadata("perfect-match-test", true, 15, map[string]any{
		"expected": 15,
		"actual":   15,
		"note":     "no divergence expected",
	}, logging.Metadata{
		OperationType:    "file_validation",
		OperationSubtype: "syntax_check",
	})

	// Test 5: Over-performance - got 25 instead of expected 10
	// Using standard Check for backward compatibility test
	logger.Check("over-performance-test", true, 25, map[string]any{
		"expected": 10,
		"actual":   25,
		"note":     "exceeds expected health",
	})

	logger.Success("Divergence demo complete", 5, map[string]any{
		"divergences_created": 4,
		"perfect_matches":     1,
	})
}

// ============================================================================
// CLOSING
// ============================================================================
