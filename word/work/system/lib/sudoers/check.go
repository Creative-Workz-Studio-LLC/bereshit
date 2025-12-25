// ============================================================================
// METADATA
// ============================================================================
// Sudoers Validation Library - CPI-SI Interactive Terminal System
// Purpose: Validate sudoers configuration installation and correctness
// Non-blocking: Returns status without interrupting workflow
//
// HEALTH SCORING MAP (TRUE SCORE):
// ----------------------------------
// Check() function validates installed sudoers configuration:
//   Action 1/3: File exists check (+50 or -50) - CRITICAL
//     Foundation - without file, nothing else possible
//
//   Action 2/3: Permissions check (+35 or -35)
//     Security requirement - wrong perms can break sudo or create vulnerabilities
//
//   Action 3/3: Syntax validation (+45 or -45) - CRITICAL
//     Ensures sudo won't break on next reload
//
//   Total Possible (Check): 130 points
//   Normalization: (cumulative_health / 130) × 100
//
// CheckSourceFile() validates source file before installation:
//   Action 1/2: Source file exists (+55 or -55) - CRITICAL
//   Action 2/2: Source syntax validation (+60 or -60) - CRITICAL
//     Prevents installing broken config
//
//   Total Possible (CheckSourceFile): 115 points
//   Normalization: (cumulative_health / 115) × 100

package sudoers

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"

	"system/lib/debugging"
	"system/lib/logging"
)

// Status represents the sudoers configuration status
type Status struct {
	Installed       bool
	FileExists      bool
	CorrectPerms    bool
	SyntaxValid     bool
	FilePath        string
	Permissions     string
	ValidationError string
}

// ============================================================================
// BODY
// ============================================================================

// Check validates the sudoers configuration installation
func Check() Status {
	logger := logging.NewLogger("sudoers")
	inspector := debugging.NewInspector("sudoers")
	inspector.Enable() // Enable debugging to capture HOW data

	status := Status{
		FilePath: "/etc/sudoers.d/90-cpi-si-safe-operations",
	}

	// DEBUGGING: Capture initial state
	inspector.Snapshot("check-start", map[string]any{
		"expected_path": status.FilePath,
		"expected_perms": "0440",
	})

	// Action 1/3: Check if file exists (+50 or -50) - CRITICAL
	if _, err := os.Stat(status.FilePath); err == nil {
		status.FileExists = true
		logger.Check("file-exists", true, 50, map[string]any{
			"file": status.FilePath,
		})
	} else {
		logger.Check("file-exists", false, -50, map[string]any{
			"file":  status.FilePath,
			"error": err.Error(),
		})
		return status
	}

	// Action 2/3: Check permissions (+35 or -35)
	info, err := os.Stat(status.FilePath)
	if err != nil {
		status.ValidationError = fmt.Sprintf("stat error: %v", err)
		logger.Check("permissions-check", false, -35, map[string]any{
			"error": err.Error(),
		})
		return status
	}

	perms := info.Mode().Perm()
	status.Permissions = fmt.Sprintf("%04o", perms)

	// DEBUGGING: Capture permission check state
	inspector.ExpectedState("permissions-check", "0440", status.Permissions, map[string]any{
		"file":          status.FilePath,
		"is_correct":    perms == 0440,
		"perms_decimal": perms,
	})

	// 440 = r--r----- (readable by owner and group, not writable)
	if perms == 0440 {
		status.CorrectPerms = true
		logger.Check("permissions-check", true, 35, map[string]any{
			"permissions": status.Permissions,
			"expected":    "0440",
		})
	} else {
		logger.Check("permissions-check", false, -35, map[string]any{
			"permissions": status.Permissions,
			"expected":    "0440",
		})
	}

	// Action 3/3: Validate syntax using visudo (+45 or -45) - CRITICAL
	// DEBUGGING: Capture state before visudo call
	inspector.Snapshot("visudo-call", map[string]any{
		"file":    status.FilePath,
		"command": "pkexec visudo -c -f",
	})

	cmd := exec.Command("pkexec", "visudo", "-c", "-f", status.FilePath)
	output, err := cmd.CombinedOutput()

	// DEBUGGING: Capture visudo result
	inspector.ExpectedState("visudo-result", "parsed OK", string(output), map[string]any{
		"has_error":     err != nil,
		"output_length": len(output),
		"contains_ok":   strings.Contains(string(output), "parsed OK"),
	})

	if err != nil {
		status.ValidationError = fmt.Sprintf("visudo validation failed: %s", string(output))
		logger.Check("syntax-validation", false, -45, map[string]any{
			"error":  status.ValidationError,
			"output": string(output),
		})
		return status
	}

	// Check if visudo reports success
	if strings.Contains(string(output), "parsed OK") || err == nil {
		status.SyntaxValid = true
		logger.Check("syntax-validation", true, 45, map[string]any{
			"validation": "passed",
			"output":     string(output),
		})
	} else {
		logger.Check("syntax-validation", false, -45, map[string]any{
			"output": string(output),
		})
	}

	// Overall installation status
	status.Installed = status.FileExists && status.CorrectPerms && status.SyntaxValid

	// DEBUGGING: Capture final validation state
	inspector.ExpectedState("check-complete", true, status.Installed, map[string]any{
		"file_exists":   status.FileExists,
		"correct_perms": status.CorrectPerms,
		"syntax_valid":  status.SyntaxValid,
	})

	// Log final status
	if status.Installed {
		logger.Success("Sudoers configuration validated successfully", 0, map[string]any{
			"installed": true,
		})
	} else {
		logger.Failure("Sudoers configuration has issues", "see status for details", 0, map[string]any{
			"installed":    false,
			"file_exists":  status.FileExists,
			"correct_perms": status.CorrectPerms,
			"syntax_valid":  status.SyntaxValid,
		})
	}

	return status
}

// CheckSourceFile validates the source sudoers file before installation
func CheckSourceFile(path string) (bool, error) {
	logger := logging.NewLogger("sudoers")
	inspector := debugging.NewInspector("sudoers")
	inspector.Enable() // Enable debugging to capture HOW data

	// DEBUGGING: Capture initial state
	inspector.Snapshot("check-source-start", map[string]any{
		"path": path,
	})

	// Action 1/2: Check if source file exists (+55 or -55) - CRITICAL
	if _, err := os.Stat(path); err != nil {
		logger.Check("source-file-exists", false, -55, map[string]any{
			"path":  path,
			"error": err.Error(),
		})
		// DEBUGGING: Capture file not found state
		inspector.ExpectedState("source-file-exists", "exists", "not found", map[string]any{
			"path":  path,
			"error": err.Error(),
		})
		return false, fmt.Errorf("source file not found: %v", err)
	}
	logger.Check("source-file-exists", true, 55, map[string]any{
		"path": path,
	})

	// Action 2/2: Validate source syntax (+60 or -60) - CRITICAL
	// DEBUGGING: Capture state before visudo call
	inspector.Snapshot("source-visudo-call", map[string]any{
		"path":    path,
		"command": "pkexec visudo -c -f",
	})

	cmd := exec.Command("pkexec", "visudo", "-c", "-f", path)
	output, err := cmd.CombinedOutput()

	// DEBUGGING: Capture visudo result
	inspector.ExpectedState("source-visudo-result", "parsed OK", string(output), map[string]any{
		"has_error":     err != nil,
		"output_length": len(output),
		"contains_ok":   strings.Contains(string(output), "parsed OK"),
	})

	if err != nil {
		logger.Check("source-syntax-validation", false, -60, map[string]any{
			"path":   path,
			"error":  err.Error(),
			"output": string(output),
		})
		return false, fmt.Errorf("syntax validation failed: %s", string(output))
	}

	logger.Check("source-syntax-validation", true, 60, map[string]any{
		"path":   path,
		"output": string(output),
	})
	logger.Success("Source file validated successfully", 0, map[string]any{
		"path": path,
	})

	return true, nil
}

// GetConfigPath returns the path to the sudoers configuration file
func GetConfigPath() string {
	return "/etc/sudoers.d/90-cpi-si-safe-operations"
}

// GetSourcePath returns the path to the source sudoers file
func GetSourcePath() string {
	home, _ := os.UserHomeDir()
	return filepath.Join(home, ".claude", "system", "sudoers", "90-cpi-si-safe-operations")
}

// GetInstallScript returns the path to the installation script
func GetInstallScript() string {
	home, _ := os.UserHomeDir()
	return filepath.Join(home, ".claude", "system", "scripts", "sudoers", "install.sh")
}

// ============================================================================
// CLOSING
// ============================================================================

// IsHealthy returns true if sudoers configuration is fully operational
func IsHealthy(status Status) bool {
	return status.Installed
}

// GetRecommendation returns a recommendation based on status
func GetRecommendation(status Status) string {
	if status.Installed {
		return "Sudoers configuration is healthy"
	}

	if !status.FileExists {
		return "Run: cd ~/.claude/system && ./scripts/sudoers/install.sh"
	}

	if !status.CorrectPerms {
		return fmt.Sprintf("Fix permissions: sudo chmod 440 %s", status.FilePath)
	}

	if !status.SyntaxValid {
		return fmt.Sprintf("Syntax error: %s", status.ValidationError)
	}

	return "Unknown issue - manual investigation needed"
}
