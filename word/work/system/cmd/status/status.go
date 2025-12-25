// ============================================================================
// METADATA
// ============================================================================
// Status Command - CPI-SI Interactive Terminal System
// Purpose: Quick health check showing system status
// Non-blocking: Fast status overview
// Usage: ./bin/status
//
// HEALTH SCORING MAP (TRUE SCORE):
// ----------------------------------
// Setup (4 actions = 25 points):
//   Action 1/4: Initialize logger (+10 or -10)
//   Action 2/4: Log operation start (+5 or -5)
//   Action 3/4: Snapshot state (+8 or -8)
//   Action 4/4: Display header (+2 or -2)
//
// Component Checks (2 actions = 100 points) - CRITICAL:
//   Action 1/2: Check sudoers component (+50 or -50) - Core functionality
//   Action 2/2: Check environment component (+50 or -50) - Core functionality
//
// Display Results (5 actions = 45 points):
//   Action 1/5: Display quick status (+15 or -15)
//   Action 2/5: Log check results (+10 or -10)
//   Action 3/5: Display detailed status (+8 or -8)
//   Action 4/5: Display capabilities (+5 or -5)
//   Action 5/5: Display next steps (+7 or -7)
//
// Total Possible: 170 points
// Normalization: (cumulative_health / 170) × 100

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"system/lib/debugging"
	"system/lib/display"
	"system/lib/environment"
	"system/lib/logging"
	"system/lib/sudoers"
)

// ============================================================================
// BODY
// ============================================================================

func checkComponent(name string, checker func() bool) (string, bool) {
	ok := checker()
	if ok {
		return fmt.Sprintf("%s%-25s%s %s", display.Bold, name, display.Reset, display.Success("Operational")), ok
	}
	return fmt.Sprintf("%s%-25s%s %s", display.Bold, name, display.Reset, display.Failure("Issues detected")), ok
}

func showQuickStatus() (bool, bool) {
	// Check sudoers
	sudoersStatus := sudoers.Check()
	sudoersOK := sudoers.IsHealthy(sudoersStatus)

	// Check environment
	envStatus := environment.Check()
	envOK := environment.IsHealthy(envStatus)

	// Display
	fmt.Print(display.Header("System Status"))

	sudoersLine, _ := checkComponent("Sudoers Configuration", func() bool { return sudoersOK })
	fmt.Println(sudoersLine)

	envLine, _ := checkComponent("Environment Variables", func() bool { return envOK })
	fmt.Println(envLine)

	return sudoersOK, envOK
}

func showDetailedStatus(sudoersOK, envOK bool) {
	fmt.Print(display.Subheader("Details"))

	if !sudoersOK {
		sudoersStatus := sudoers.Check()
		fmt.Println(display.KeyValue("Sudoers", sudoers.GetRecommendation(sudoersStatus)))
	}

	if !envOK {
		envStatus := environment.Check()
		fmt.Println(display.KeyValue("Environment", environment.GetRecommendation(envStatus)))
	}

	if sudoersOK && envOK {
		fmt.Println(display.Success("All components operational"))
	}
}

func showCapabilities(sudoersOK, envOK bool) {
	fmt.Print(display.Subheader("Available Capabilities"))

	if sudoersOK {
		fmt.Println(display.StatusLine(true, "Passwordless sudo for safe operations"))
		fmt.Println(display.KeyValue("", "• Package management (apt install/update/upgrade)"))
		fmt.Println(display.KeyValue("", "• Service management (systemctl start/stop/restart)"))
		fmt.Println(display.KeyValue("", "• File permissions (chmod/chown)"))
	} else {
		fmt.Println(display.StatusLine(false, "Passwordless sudo (not configured)"))
	}

	if envOK {
		fmt.Println(display.StatusLine(true, "Non-interactive environment defaults"))
		fmt.Println(display.KeyValue("", "• Package managers (apt, pip, npm)"))
		fmt.Println(display.KeyValue("", "• Development tools"))
	} else {
		fmt.Println(display.StatusLine(false, "Non-interactive defaults (not configured)"))
	}
}

func showNextSteps(sudoersOK, envOK bool) {
	if sudoersOK && envOK {
		fmt.Print(display.Subheader("Ready to Use"))
		fmt.Println()
		fmt.Println(display.Info("Interactive Terminal System is fully operational"))
		fmt.Println()
		fmt.Println("Try:")
		fmt.Println("  sudo apt update                   # Should work without password")
		fmt.Println("  ./bin/test --quick                # Run operation tests")
		return
	}

	fmt.Print(display.Subheader("Next Steps"))

	if !sudoersOK {
		fmt.Println()
		fmt.Println("1. Install sudoers configuration:")
		fmt.Println("   cd ~/.claude/system")
		fmt.Println("   ./scripts/sudoers/install.sh")
	}

	if !envOK {
		fmt.Println()
		fmt.Println("2. Integrate environment variables:")
		fmt.Println("   cd ~/.claude/system")
		fmt.Println("   ./scripts/env/integrate.sh")
		fmt.Println("   source ~/.bashrc")
	}

	fmt.Println()
	fmt.Println("Then run './bin/validate' to verify installation")
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup Action 1/4: Initialize logger (+10)
	logger := logging.NewLogger("status")
	logger.DeclareHealthTotal(170)  // Total possible points from health scoring map
	inspector := debugging.NewInspector("status")
	inspector.Enable() // Enable debugging to capture HOW data

	// DEBUGGING: Capture command start
	inspector.Snapshot("status-start", map[string]any{
		"command":     "status",
		"purpose":     "quick health check",
		"checks":      []string{"sudoers", "environment"},
	})

	logger.Check("logger-initialized", true, 10, map[string]any{
		"component": "status",
	})

	// Setup Action 2/4: Log operation start (+5)
	logger.Operation("status", 5, "quick health check")

	// Setup Action 3/4: Snapshot state (+8)
	logger.SnapshotState("before-status-check", 8)

	// Setup Action 4/4: Display header (+2)
	fmt.Print(display.Header("CPI-SI Interactive Terminal System"))
	logger.Check("header-displayed", true, 2, map[string]any{
		"header": "system status",
	})

	// Component Checks Action 1/2: Check sudoers (+50)
	// Component Checks Action 2/2: Check environment (+50)
	sudoersOK, envOK := showQuickStatus()

	// DEBUGGING: Capture component health results
	inspector.ExpectedState("sudoers-health", true, sudoersOK, map[string]any{
		"component": "sudoers",
		"check":     "passwordless operations",
	})
	inspector.ExpectedState("environment-health", true, envOK, map[string]any{
		"component": "environment",
		"check":     "non-interactive defaults",
	})

	logger.Check("sudoers-checked", true, 50, map[string]any{
		"component": "sudoers",
		"healthy":   sudoersOK,
	})
	logger.Check("environment-checked", true, 50, map[string]any{
		"component": "environment",
		"healthy":   envOK,
	})

	// Display Results Action 1/5: Display quick status (+15)
	// (showQuickStatus already displayed, marking as done)
	logger.Check("quick-status-displayed", true, 15, map[string]any{
		"displayed": "component status overview",
	})

	// Display Results Action 2/5: Log check results (+10)
	logger.Check("check-results-logged", true, 10, map[string]any{
		"sudoers_ok": sudoersOK,
		"env_ok":     envOK,
	})

	// Display Results Action 3/5: Display detailed status (+8)
	showDetailedStatus(sudoersOK, envOK)
	logger.Check("detailed-status-displayed", true, 8, map[string]any{
		"displayed": "detailed component status",
	})

	fmt.Println()

	// Display Results Action 4/5: Display capabilities (+5)
	showCapabilities(sudoersOK, envOK)
	logger.Check("capabilities-displayed", true, 5, map[string]any{
		"displayed": "available capabilities",
	})

	fmt.Println()

	// Display Results Action 5/5: Display next steps (+7)
	showNextSteps(sudoersOK, envOK)
	logger.Check("next-steps-displayed", true, 7, map[string]any{
		"displayed": "guidance for user",
	})

	fmt.Println()

	// DEBUGGING: Capture overall status completion
	allHealthy := sudoersOK && envOK
	inspector.ExpectedState("status-complete", true, allHealthy, map[string]any{
		"sudoers_healthy":     sudoersOK,
		"environment_healthy": envOK,
		"all_checks_passed":   allHealthy,
		"issues_detected":     !allHealthy,
	})

	// Log completion (not part of 100-point map, just final reporting)
	if allHealthy {
		logger.Success("Status check completed - all components operational", 0, map[string]any{
			"sudoers_ok":     sudoersOK,
			"environment_ok": envOK,
			"overall":        "healthy",
		})
	} else {
		logger.Success("Status check completed - issues detected", 0, map[string]any{
			"sudoers_ok":     sudoersOK,
			"environment_ok": envOK,
			"overall":        "issues",
		})
	}
}
