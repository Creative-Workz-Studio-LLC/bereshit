// ============================================================================
// METADATA
// ============================================================================
// Validate Command - CPI-SI Interactive Terminal System
// Purpose: Validate system installation and configuration
// Non-blocking: Checks status without modifying system
// Usage: ./bin/validate
//
// HEALTH SCORING MAP (TRUE SCORE):
// ----------------------------------
// Setup (4 actions = 25 points):
//   Action 1/4: Initialize logger (+10 or -10)
//   Action 2/4: Start operation (+5 or -5)
//   Action 3/4: Snapshot state (+8 or -8)
//   Action 4/4: Display header (+2 or -2)
//
// Sudoers Validation (6 actions = 128 points) - CRITICAL:
//   Action 1/6: Access sudoers library (+5 or -5)
//   Action 2/6: Check file exists (+40 or -40) - Foundation for everything
//   Action 3/6: Check permissions correct (+30 or -30) - Security requirement
//   Action 4/6: Check syntax valid (+40 or -40) - Could break sudo entirely
//   Action 5/6: Determine installed status (+10 or -10)
//   Action 6/6: Display validation results (+3 or -3)
//
// Environment Validation (5 actions = 78 points) - CRITICAL:
//   Action 1/5: Access environment library (+5 or -5)
//   Action 2/5: Check shell integration (+25 or -25) - Required for env to work
//   Action 3/5: Check all variables correct (+35 or -35) - Main validation
//   Action 4/5: Determine healthy status (+10 or -10)
//   Action 5/5: Display validation results (+3 or -3)
//
// Summary & Results (4 actions = 28 points):
//   Action 1/4: Determine overall status (+10 or -10)
//   Action 2/4: Display appropriate message (+8 or -8)
//   Action 3/4: Show recommendations (+5 or -5)
//   Action 4/4: Log final result (+5 or -5)
//
// Total Possible: 259 points
// Normalization: (cumulative_health / 259) × 100

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"os"
	"system/lib/debugging"
	"system/lib/display"
	"system/lib/environment"
	"system/lib/logging"
	"system/lib/sudoers"
)

// ============================================================================
// BODY
// ============================================================================

func validateSudoers(logger *logging.Logger) bool {
	inspector := debugging.NewInspector("validate")
	inspector.Enable() // Enable debugging to capture HOW data
	fmt.Print(display.Subheader("Sudoers Configuration"))

	// DEBUGGING: Capture validation start
	inspector.Snapshot("sudoers-validation-start", map[string]any{
		"component":    "sudoers",
		"checks":       []string{"file exists", "permissions", "syntax"},
		"expected_all": true,
	})

	// Access sudoers library (+5 or -5)
	status := sudoers.Check()
	logger.Check("sudoers-library-access", true, 5, map[string]any{
		"library": "sudoers",
	})

	// Check file exists (+40 or -40) - Foundation for everything
	fmt.Println(display.StatusLine(status.FileExists, "Configuration file exists"))
	healthImpact := 40
	if !status.FileExists {
		healthImpact = -40
	}
	logger.Check("sudoers-file-exists", status.FileExists, healthImpact,
		map[string]any{"file": "/etc/sudoers.d/90-cpi-si-safe-operations"})

	// Check permissions correct (+30 or -30) - Security requirement
	fmt.Println(display.StatusLine(status.CorrectPerms, fmt.Sprintf("Correct permissions (%s)", status.Permissions)))
	healthImpact = 30
	if !status.CorrectPerms {
		healthImpact = -30
	}
	logger.Check("sudoers-permissions", status.CorrectPerms, healthImpact,
		map[string]any{"permissions": status.Permissions})

	// Check syntax valid (+40 or -40) - Could break sudo entirely
	fmt.Println(display.StatusLine(status.SyntaxValid, "Syntax validation passed"))
	healthImpact = 40
	if !status.SyntaxValid {
		healthImpact = -40
	}
	logger.Check("sudoers-syntax-valid", status.SyntaxValid, healthImpact,
		map[string]any{"syntax": "visudo check"})

	// Determine installed status (+10 or -10)
	healthImpact = 10
	if !status.Installed {
		healthImpact = -10
	}
	logger.Check("sudoers-installed-status", status.Installed, healthImpact,
		map[string]any{"installed": status.Installed})

	if !status.Installed {
		fmt.Println()
		fmt.Println(display.Warning(sudoers.GetRecommendation(status)))
	}

	// Display validation results (+3 or -3)
	logger.Check("sudoers-display-results", true, 3, map[string]any{
		"displayed": "sudoers validation results",
	})

	// DEBUGGING: Capture validation result
	inspector.ExpectedState("sudoers-validation-complete", true, status.Installed, map[string]any{
		"file_exists":   status.FileExists,
		"correct_perms": status.CorrectPerms,
		"syntax_valid":  status.SyntaxValid,
		"all_passed":    status.Installed,
	})

	return status.Installed
}

func validateEnvironment(logger *logging.Logger) bool {
	inspector := debugging.NewInspector("validate")
	inspector.Enable() // Enable debugging to capture HOW data
	fmt.Print(display.Subheader("Environment Configuration"))

	// DEBUGGING: Capture validation start
	inspector.Snapshot("environment-validation-start", map[string]any{
		"component":    "environment",
		"checks":       []string{"shell integration", "variables"},
		"expected_all": true,
	})

	// Access environment library (+5 or -5)
	status := environment.Check()
	logger.Check("environment-library-access", true, 5, map[string]any{
		"library": "environment",
	})

	// Check shell integration (+25 or -25) - Required for env to work
	fmt.Println(display.StatusLine(status.ShellIntegrated, "Shell integration (.bashrc)"))
	healthImpact := 25
	if !status.ShellIntegrated {
		healthImpact = -25
	}
	logger.Check("shell-integration", status.ShellIntegrated, healthImpact,
		map[string]any{"file": ".bashrc"})

	// Check all variables correct (+35 total) - Main validation
	allVarsCorrect := true
	correctCount := 0
	totalVars := len(status.Variables)

	for _, v := range status.Variables {
		msg := fmt.Sprintf("%s = %s", v.Name, v.Actual)
		if !v.IsSet {
			msg = fmt.Sprintf("%s (not set)", v.Name)
		} else if !v.IsCorrect {
			msg = fmt.Sprintf("%s = %s (expected: %s)", v.Name, v.Actual, v.Expected)
		}
		fmt.Println(display.StatusLine(v.IsCorrect, msg))

		if v.IsCorrect {
			correctCount++
		} else {
			allVarsCorrect = false
		}
	}

	// Proportional scoring for variables: 35 points distributed
	if totalVars > 0 {
		varScore := (correctCount * 35) / totalVars
		if !allVarsCorrect {
			varScore = varScore - 35 // Subtract the full 35, add back the proportional
		}
		logger.Check("environment-variables", allVarsCorrect, varScore, map[string]any{
			"correct": correctCount,
			"total":   totalVars,
		})
	} else {
		logger.Check("environment-variables", true, 35, map[string]any{
			"total": 0,
		})
	}

	// Determine healthy status (+10 or -10)
	healthy := environment.IsHealthy(status)
	healthImpact = 10
	if !healthy {
		healthImpact = -10
	}
	logger.Check("environment-healthy-status", healthy, healthImpact,
		map[string]any{"healthy": healthy})

	if !healthy {
		fmt.Println()
		fmt.Println(display.Warning(environment.GetRecommendation(status)))
	}

	// Display validation results (+3 or -3)
	logger.Check("environment-display-results", true, 3, map[string]any{
		"displayed": "environment validation results",
	})

	// DEBUGGING: Capture validation result
	inspector.ExpectedState("environment-validation-complete", true, healthy, map[string]any{
		"shell_integrated": status.ShellIntegrated,
		"all_vars_correct": allVarsCorrect,
		"correct_count":    correctCount,
		"total_vars":       totalVars,
		"all_passed":       healthy,
	})

	return healthy
}

func showSummary(logger *logging.Logger, sudoersOK, envOK bool) {
	fmt.Print(display.Header("Validation Summary"))

	allOK := sudoersOK && envOK

	// Determine overall status (+10 or -10)
	logger.Check("overall-status-determined", true, 10, map[string]any{
		"sudoers_ok": sudoersOK,
		"env_ok":     envOK,
		"overall":    allOK,
	})

	// Display appropriate message (+8 or -8)
	if allOK {
		fmt.Println(display.Success("Interactive Terminal System is fully operational"))
		fmt.Println()
		fmt.Println(display.Info("You can now use passwordless sudo for safe operations"))
		fmt.Println(display.Info("Environment variables are configured for non-interactive use"))
	} else {
		fmt.Println(display.Failure("Interactive Terminal System has issues"))
		fmt.Println()

		if !sudoersOK {
			fmt.Println(display.Warning("Sudoers configuration needs attention"))
		}

		if !envOK {
			fmt.Println(display.Warning("Environment configuration needs attention"))
		}

		fmt.Println()
		fmt.Println(display.Info("Run './bin/diagnose' for detailed troubleshooting"))
	}
	logger.Check("summary-message-displayed", true, 8, map[string]any{
		"message_type": map[bool]string{true: "success", false: "failure"}[allOK],
	})

	// Show recommendations (+5 or -5)
	logger.Check("recommendations-shown", true, 5, map[string]any{
		"shown": "next steps and recommendations",
	})
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup Action 1/4: Initialize logger (+10 or -10)
	logger := logging.NewLogger("validate")
	logger.DeclareHealthTotal(259)  // Total possible points from health scoring map
	inspector := debugging.NewInspector("validate")
	inspector.Enable() // Enable debugging to capture HOW data

	// DEBUGGING: Capture command start
	inspector.Snapshot("validate-start", map[string]any{
		"command":     "validate",
		"purpose":     "system configuration validation",
		"validations": []string{"sudoers", "environment"},
	})

	logger.Check("logger-initialized", true, 10, map[string]any{
		"component": "validate",
	})

	// Setup Action 2/4: Start operation (+5 or -5)
	logger.Operation("validate", 5, "system configuration validation")

	// Setup Action 3/4: Snapshot state (+8 or -8)
	logger.SnapshotState("before-validation", 8)

	// Setup Action 4/4: Display header (+2 or -2)
	fmt.Print(display.Header("CPI-SI Interactive Terminal System - Validation"))
	logger.Check("header-displayed", true, 2, map[string]any{
		"header": "validation",
	})

	// Perform validations
	sudoersOK := validateSudoers(logger)
	envOK := validateEnvironment(logger)

	// Show summary
	showSummary(logger, sudoersOK, envOK)

	// DEBUGGING: Capture overall validation result
	allOK := sudoersOK && envOK
	inspector.ExpectedState("validate-complete", true, allOK, map[string]any{
		"sudoers_ok":        sudoersOK,
		"environment_ok":    envOK,
		"all_passed":        allOK,
		"exit_code":         map[bool]int{true: 0, false: 1}[allOK],
	})

	// Log final result (+5 or -5)
	if allOK {
		logger.Success("Validation completed - system fully operational", 5, map[string]any{
			"sudoers_ok":     sudoersOK,
			"environment_ok": envOK,
			"exit_code":      0,
		})
	} else {
		logger.Failure("Validation failed - system has issues", "components not operational", -5, map[string]any{
			"sudoers_ok":     sudoersOK,
			"environment_ok": envOK,
			"exit_code":      1,
		})
		os.Exit(1)
	}
}
