// ============================================================================
// METADATA
// ============================================================================
// Environment Validation Library - CPI-SI Interactive Terminal System
// Purpose: Validate environment variable configuration
// Non-blocking: Returns status without interrupting workflow
//
// HEALTH SCORING MAP (TRUE SCORE):
// ----------------------------------
// Check() function validates environment configuration:
//   Action 1/6: Shell integration check (+60 or -60) - CRITICAL
//     Foundation for all variables - without this, config doesn't load
//
//   Action 2/6: DEBIAN_FRONTEND variable (+20 or -20)
//     Package manager non-interactive - high usage frequency
//
//   Action 3/6: NEEDRESTART_MODE variable (+20 or -20)
//     Automatic restart decisions - prevents hanging prompts
//
//   Action 4/6: NEEDRESTART_SUSPEND variable (+20 or -20)
//     Suspend restart prompts - pairs with NEEDRESTART_MODE
//
//   Action 5/6: PIP_NO_INPUT variable (+10 or -10)
//     Python package non-interactive - moderate importance
//
//   Action 6/6: NPM_CONFIG_YES variable (+10 or -10)
//     Node package non-interactive - moderate importance
//
// Total Possible: 140 points
// Normalization: (cumulative_health / 140) × 100

package environment

// ============================================================================
// SETUP
// ============================================================================

import (
	"bufio"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"

	"system/lib/debugging"
	"system/lib/logging"
)

// Variable represents a single environment variable
type Variable struct {
	Name     string
	Expected string
	Actual   string
	IsSet    bool
	IsCorrect bool
}

// Status represents the environment configuration status
type Status struct {
	ShellIntegrated bool
	Variables       []Variable
	ConfigPath      string
	ShellRCPath     string
}

// ============================================================================
// BODY
// ============================================================================

// RequiredVariables defines the critical environment variables
var RequiredVariables = []struct {
	Name     string
	Expected string
}{
	{"DEBIAN_FRONTEND", "noninteractive"},
	{"NEEDRESTART_MODE", "a"},
	{"NEEDRESTART_SUSPEND", "1"},
	{"PIP_NO_INPUT", "1"},
	{"NPM_CONFIG_YES", "true"},
}

// Check validates environment variable configuration
func Check() Status {
	logger := logging.NewLogger("environment")
	inspector := debugging.NewInspector("environment")
	inspector.Enable() // Enable debugging to capture HOW data

	home, _ := os.UserHomeDir()
	status := Status{
		ConfigPath:  filepath.Join(home, ".claude", "system", "env", "non-interactive.conf"),
		ShellRCPath: filepath.Join(home, ".bashrc"),
		Variables:   make([]Variable, 0),
	}

	// DEBUGGING: Capture initial state
	inspector.Snapshot("check-start", map[string]any{
		"config_path":   status.ConfigPath,
		"shell_rc_path": status.ShellRCPath,
		"required_vars": len(RequiredVariables),
	})

	// Action 1/5: Check shell integration (+60 or -60) - CRITICAL
	status.ShellIntegrated = checkShellIntegration(status.ShellRCPath, status.ConfigPath)

	// DEBUGGING: Capture shell integration result
	inspector.ExpectedState("shell-integration", true, status.ShellIntegrated, map[string]any{
		"shell_rc":    status.ShellRCPath,
		"config_path": status.ConfigPath,
	})

	if status.ShellIntegrated {
		logger.Check("shell-integration", true, 60, map[string]any{
			"shell_rc":    status.ShellRCPath,
			"config_path": status.ConfigPath,
		})
	} else {
		logger.Check("shell-integration", false, -60, map[string]any{
			"shell_rc":    status.ShellRCPath,
			"config_path": status.ConfigPath,
		})
	}

	// Actions 2-5: Check each required variable
	variablePoints := map[string]int{
		"DEBIAN_FRONTEND":  25, // High usage frequency
		"NEEDRESTART_MODE": 25, // Prevents hanging prompts
		"PIP_NO_INPUT":     15, // Moderate importance
		"NPM_CONFIG_YES":   15, // Moderate importance
	}

	for _, req := range RequiredVariables {
		variable := Variable{
			Name:     req.Name,
			Expected: req.Expected,
			Actual:   os.Getenv(req.Name),
		}

		variable.IsSet = variable.Actual != ""
		variable.IsCorrect = variable.Actual == req.Expected

		// DEBUGGING: Capture variable check result
		inspector.ExpectedState(fmt.Sprintf("variable-%s", strings.ToLower(req.Name)), req.Expected, variable.Actual, map[string]any{
			"name":       req.Name,
			"is_set":     variable.IsSet,
			"is_correct": variable.IsCorrect,
		})

		points := variablePoints[req.Name]
		if variable.IsCorrect {
			logger.Check(fmt.Sprintf("variable-%s", strings.ToLower(req.Name)), true, points, map[string]any{
				"name":     req.Name,
				"expected": req.Expected,
				"actual":   variable.Actual,
			})
		} else {
			logger.Check(fmt.Sprintf("variable-%s", strings.ToLower(req.Name)), false, -points, map[string]any{
				"name":     req.Name,
				"expected": req.Expected,
				"actual":   variable.Actual,
				"is_set":   variable.IsSet,
			})
		}

		status.Variables = append(status.Variables, variable)
	}

	// DEBUGGING: Capture final environment validation state
	incorrectCountDebug := 0
	for _, v := range status.Variables {
		if !v.IsCorrect {
			incorrectCountDebug++
		}
	}
	inspector.ExpectedState("check-complete", 0, incorrectCountDebug, map[string]any{
		"shell_integrated": status.ShellIntegrated,
		"total_vars":       len(status.Variables),
		"is_healthy":       IsHealthy(status),
	})

	// Log final status
	if IsHealthy(status) {
		logger.Success("Environment configuration validated successfully", 0, map[string]any{
			"shell_integrated": status.ShellIntegrated,
			"all_vars_correct": true,
		})
	} else {
		incorrectCount := 0
		for _, v := range status.Variables {
			if !v.IsCorrect {
				incorrectCount++
			}
		}
		logger.Failure("Environment configuration has issues", "see status for details", 0, map[string]any{
			"shell_integrated": status.ShellIntegrated,
			"incorrect_vars":   incorrectCount,
		})
	}

	return status
}

// checkShellIntegration checks if .bashrc sources the config file
func checkShellIntegration(shellRC, configPath string) bool {
	file, err := os.Open(shellRC)
	if err != nil {
		return false
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		// Look for the integration marker or direct source
		if strings.Contains(line, "CPI-SI Interactive Terminal System") ||
			strings.Contains(line, configPath) {
			return true
		}
	}

	return false
}

// CheckInCurrentShell verifies variables in the current shell session
func CheckInCurrentShell() map[string]string {
	result := make(map[string]string)

	for _, req := range RequiredVariables {
		result[req.Name] = os.Getenv(req.Name)
	}

	return result
}

// GetConfigPath returns the path to the environment configuration file
func GetConfigPath() string {
	home, _ := os.UserHomeDir()
	return filepath.Join(home, ".claude", "system", "env", "non-interactive.conf")
}

// GetIntegrationScript returns the path to the integration script
func GetIntegrationScript() string {
	home, _ := os.UserHomeDir()
	return filepath.Join(home, ".claude", "system", "scripts", "env", "integrate.sh")
}

// ============================================================================
// CLOSING
// ============================================================================

// IsHealthy returns true if environment is properly configured
func IsHealthy(status Status) bool {
	if !status.ShellIntegrated {
		return false
	}

	for _, v := range status.Variables {
		if !v.IsCorrect {
			return false
		}
	}

	return true
}

// GetRecommendation returns a recommendation based on status
func GetRecommendation(status Status) string {
	if !status.ShellIntegrated {
		return "Run: cd ~/.claude/system && ./scripts/env/integrate.sh && source ~/.bashrc"
	}

	incorrect := 0
	for _, v := range status.Variables {
		if !v.IsCorrect {
			incorrect++
		}
	}

	if incorrect > 0 {
		return fmt.Sprintf("%d variables incorrect - source ~/.bashrc or restart shell", incorrect)
	}

	return "Environment configuration is healthy"
}

// TestShellCommand executes a command in a fresh shell to test environment
func TestShellCommand(command string) (string, error) {
	cmd := exec.Command("bash", "-c", fmt.Sprintf("source %s && %s", GetConfigPath(), command))
	output, err := cmd.CombinedOutput()
	return string(output), err
}
