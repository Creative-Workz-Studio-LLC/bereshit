// ============================================================================
// METADATA
// ============================================================================
// Diagnose Command - CPI-SI Interactive Terminal System
// Purpose: Detailed diagnostics and troubleshooting information
// Non-blocking: Comprehensive system analysis
// Usage: ./bin/diagnose
//
// HEALTH SCORING MAP (TRUE SCORE):
// ----------------------------------
// Setup (4 actions = 25 points):
//   Action 1/4: Initialize logger (+10 or -10)
//   Action 2/4: Log operation start (+5 or -5)
//   Action 3/4: Snapshot state (+8 or -8)
//   Action 4/4: Display header (+2 or -2)
//
// Diagnostic Actions (7 actions = 163 points) - CRITICAL:
//   Action 1/7: Check system info (+15 or -15)
//   Action 2/7: Diagnose sudoers (+50 or -50) - Core system component
//   Action 3/7: Log sudoers diagnosis (+8 or -8)
//   Action 4/7: Diagnose environment (+50 or -50) - Core system component
//   Action 5/7: Log environment diagnosis (+8 or -8)
//   Action 6/7: Check filesystem paths (+18 or -18) - Essential for functionality
//   Action 7/7: Check binaries (+14 or -14) - Tools must exist
//
// Results & Guidance (2 actions = 32 points):
//   Action 1/2: Display troubleshooting (+25 or -25) - Primary value to user
//   Action 2/2: Log completion (+7 or -7)
//
// Total Possible: 220 points
// Normalization: (cumulative_health / 220) × 100

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"os"
	"os/exec"
	"os/user"
	"path/filepath"
	"system/lib/debugging"
	"system/lib/display"
	"system/lib/environment"
	"system/lib/logging"
	"system/lib/sudoers"
)

// ============================================================================
// BODY
// ============================================================================

func checkSystemInfo() {
	fmt.Print(display.Subheader("System Information"))

	// User info
	currentUser, _ := user.Current()
	fmt.Println(display.KeyValue("User", currentUser.Username))
	fmt.Println(display.KeyValue("Home", currentUser.HomeDir))

	// Shell
	shell := os.Getenv("SHELL")
	fmt.Println(display.KeyValue("Shell", shell))

	// Working directory
	wd, _ := os.Getwd()
	fmt.Println(display.KeyValue("Working Dir", wd))

	fmt.Println()
}

func diagnoseSudoers() {
	fmt.Print(display.Subheader("Sudoers Diagnosis"))

	status := sudoers.Check()

	fmt.Println(display.KeyValue("Config Path", status.FilePath))
	fmt.Println(display.KeyValue("File Exists", fmt.Sprintf("%t", status.FileExists)))

	if status.FileExists {
		fmt.Println(display.KeyValue("Permissions", status.Permissions))
		fmt.Println(display.KeyValue("Syntax Valid", fmt.Sprintf("%t", status.SyntaxValid)))

		if status.ValidationError != "" {
			fmt.Println(display.KeyValue("Error", status.ValidationError))
		}
	}

	// Check source file
	sourcePath := sudoers.GetSourcePath()
	if _, err := os.Stat(sourcePath); err == nil {
		fmt.Println(display.KeyValue("Source File", "Found"))

		valid, err := sudoers.CheckSourceFile(sourcePath)
		if err != nil {
			fmt.Println(display.KeyValue("Source Valid", fmt.Sprintf("No: %v", err)))
		} else {
			fmt.Println(display.KeyValue("Source Valid", fmt.Sprintf("%t", valid)))
		}
	} else {
		fmt.Println(display.KeyValue("Source File", "Not found"))
	}

	// Check sudo group membership
	cmd := exec.Command("groups")
	output, _ := cmd.Output()
	fmt.Println(display.KeyValue("Groups", string(output)))

	// Recommendation
	fmt.Println()
	fmt.Println(display.Info(sudoers.GetRecommendation(status)))

	fmt.Println()
}

func diagnoseEnvironment() {
	fmt.Print(display.Subheader("Environment Diagnosis"))

	status := environment.Check()

	fmt.Println(display.KeyValue("Config Path", status.ConfigPath))

	// Check config file exists
	if _, err := os.Stat(status.ConfigPath); err == nil {
		fmt.Println(display.KeyValue("Config File", "Found"))
	} else {
		fmt.Println(display.KeyValue("Config File", "Not found"))
	}

	// Check shell integration
	fmt.Println(display.KeyValue("Shell RC", status.ShellRCPath))
	fmt.Println(display.KeyValue("Integrated", fmt.Sprintf("%t", status.ShellIntegrated)))

	// Check each variable
	fmt.Println()
	fmt.Println(display.Bold + "Environment Variables:" + display.Reset)

	for _, v := range status.Variables {
		statusIcon := display.IconSuccess
		color := display.Green
		if !v.IsCorrect {
			statusIcon = display.IconFailure
			color = display.Red
		}

		fmt.Printf("  %s%s%s %s\n", color, statusIcon, display.Reset, v.Name)
		fmt.Println(display.KeyValue("    Expected", v.Expected))
		if v.IsSet {
			fmt.Println(display.KeyValue("    Actual", v.Actual))
		} else {
			fmt.Println(display.KeyValue("    Actual", "(not set)"))
		}
	}

	// Integration script
	integrationScript := environment.GetIntegrationScript()
	if _, err := os.Stat(integrationScript); err == nil {
		fmt.Println()
		fmt.Println(display.KeyValue("Integration Script", integrationScript))
	}

	// Recommendation
	fmt.Println()
	fmt.Println(display.Info(environment.GetRecommendation(status)))

	fmt.Println()
}

func checkPaths() {
	fmt.Print(display.Subheader("File System Check"))

	home, _ := os.UserHomeDir()
	systemPath := filepath.Join(home, ".claude", "system")

	directories := []string{
		systemPath,
		filepath.Join(systemPath, "sudoers"),
		filepath.Join(systemPath, "env"),
		filepath.Join(systemPath, "wrappers"),
		filepath.Join(systemPath, "docs"),
		filepath.Join(systemPath, "bin"),
	}

	for _, dir := range directories {
		if info, err := os.Stat(dir); err == nil {
			if info.IsDir() {
				fmt.Println(display.StatusLine(true, dir))
			} else {
				fmt.Println(display.StatusLine(false, fmt.Sprintf("%s (not a directory)", dir)))
			}
		} else {
			fmt.Println(display.StatusLine(false, fmt.Sprintf("%s (not found)", dir)))
		}
	}

	fmt.Println()
}

func checkBinaries() {
	fmt.Print(display.Subheader("Binary Check"))

	home, _ := os.UserHomeDir()
	binPath := filepath.Join(home, ".claude", "system", "bin")

	binaries := []string{"validate", "test", "status", "diagnose"}

	for _, bin := range binaries {
		path := filepath.Join(binPath, bin)
		if info, err := os.Stat(path); err == nil {
			perms := info.Mode().Perm()
			executable := (perms & 0111) != 0
			fmt.Println(display.StatusLine(executable, fmt.Sprintf("%s (%04o)", bin, perms)))
		} else {
			fmt.Println(display.StatusLine(false, fmt.Sprintf("%s (not found)", bin)))
		}
	}

	fmt.Println()
}

func showTroubleshooting() {
	fmt.Print(display.Header("Troubleshooting Recommendations"))

	fmt.Println("Common issues and solutions:")
	fmt.Println()

	fmt.Println(display.Bold + "1. Sudoers not working:" + display.Reset)
	fmt.Println("   • Logout and login again")
	fmt.Println("   • Check file permissions: pkexec visudo -c")
	fmt.Println("   • Verify ownership: ls -l /etc/sudoers.d/90-cpi-si-safe-operations")
	fmt.Println()

	fmt.Println(display.Bold + "2. Environment variables not set:" + display.Reset)
	fmt.Println("   • Source .bashrc: source ~/.bashrc")
	fmt.Println("   • Check integration: grep 'CPI-SI' ~/.bashrc")
	fmt.Println("   • Restart shell or Claude Code session")
	fmt.Println()

	fmt.Println(display.Bold + "3. Binaries not found:" + display.Reset)
	fmt.Println("   • Run build script: cd ~/.claude/system && ./scripts/build.sh")
	fmt.Println("   • Check build errors in output")
	fmt.Println()
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
	// Setup Action 1/4: Initialize logger (+10 or -10)
	logger := logging.NewLogger("diagnose")
	logger.DeclareHealthTotal(220)  // Total possible points from health scoring map
	inspector := debugging.NewInspector("diagnose")
	inspector.Enable() // Enable debugging to capture HOW data

	// DEBUGGING: Capture diagnostic command start
	inspector.Snapshot("diagnose-start", map[string]any{
		"command": "diagnose",
		"purpose": "comprehensive system diagnostics",
		"checks":  []string{"system info", "sudoers", "environment", "paths", "binaries"},
	})

	logger.Check("logger-initialized", true, 10, map[string]any{
		"component": "diagnose",
	})

	// Setup Action 2/4: Log operation start (+5 or -5)
	logger.Operation("diagnose", 5, "comprehensive system diagnostics")

	// Setup Action 3/4: Snapshot state (+8 or -8)
	logger.SnapshotState("diagnostic-start", 8)

	// Setup Action 4/4: Display header (+2 or -2)
	fmt.Print(display.Header("CPI-SI Interactive Terminal System - Diagnostics"))
	logger.Check("header-displayed", true, 2, map[string]any{
		"header": "diagnostics",
	})

	// Diagnostic Action 1/7: Check system info (+15 or -15)
	checkSystemInfo()
	logger.Check("system-info-checked", true, 15, map[string]any{
		"checked": "user, shell, working directory",
	})

	// Diagnostic Action 2/7: Diagnose sudoers (+50 or -50) - Core system component
	diagnoseSudoers()
	logger.Check("sudoers-diagnosed", true, 50, map[string]any{
		"diagnostic": "sudoers configuration",
	})

	// Diagnostic Action 3/7: Log sudoers diagnosis (+8 or -8)
	sudoersStatus := sudoers.Check()
	logger.Check("sudoers-diagnosis-logged", true, 8, map[string]any{
		"file_exists":  sudoersStatus.FileExists,
		"syntax_valid": sudoersStatus.SyntaxValid,
		"permissions":  sudoersStatus.Permissions,
	})

	// Diagnostic Action 4/7: Diagnose environment (+50 or -50) - Core system component
	diagnoseEnvironment()
	logger.Check("environment-diagnosed", true, 50, map[string]any{
		"diagnostic": "environment configuration",
	})

	// Diagnostic Action 5/7: Log environment diagnosis (+8 or -8)
	envStatus := environment.Check()
	logger.Check("environment-diagnosis-logged", true, 8, map[string]any{
		"shell_integrated": envStatus.ShellIntegrated,
		"config_path":      envStatus.ConfigPath,
	})

	// Diagnostic Action 6/7: Check filesystem paths (+18 or -18) - Essential for functionality
	checkPaths()
	logger.Check("paths-checked", true, 18, map[string]any{
		"checked": "system directories",
	})

	// Diagnostic Action 7/7: Check binaries (+14 or -14) - Tools must exist
	checkBinaries()
	logger.Check("binaries-checked", true, 14, map[string]any{
		"checked": "validate, test, status, diagnose",
	})

	// Results & Guidance Action 1/2: Display troubleshooting (+25 or -25) - Primary value to user
	showTroubleshooting()
	logger.Check("troubleshooting-displayed", true, 25, map[string]any{
		"displayed": "troubleshooting recommendations",
	})

	// Results & Guidance Action 2/2: Log completion (+7 or -7)
	allHealthy := sudoers.IsHealthy(sudoersStatus) && environment.IsHealthy(envStatus)

	// DEBUGGING: Capture overall diagnostic results
	sudoersOK := sudoers.IsHealthy(sudoersStatus)
	envOK := environment.IsHealthy(envStatus)
	inspector.ExpectedState("diagnose-complete", true, allHealthy, map[string]any{
		"sudoers_healthy":     sudoersOK,
		"environment_healthy": envOK,
		"all_healthy":         allHealthy,
		"issues_found":        !allHealthy,
	})

	if allHealthy {
		logger.Success("Diagnostics completed - system healthy", 7, map[string]any{
			"sudoers_ok":     sudoersOK,
			"environment_ok": envOK,
		})
	} else {
		logger.Success("Diagnostics completed - issues found", 7, map[string]any{
			"sudoers_ok":     sudoersOK,
			"environment_ok": envOK,
		})
	}
}
