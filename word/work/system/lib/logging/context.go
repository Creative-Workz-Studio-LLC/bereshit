// ============================================================================
// METADATA
// ============================================================================
// System Context Capture - Logging Library
//
// Biblical Foundation
//
// Scripture: "The eyes of the LORD are in every place, beholding the evil and the good" (Proverbs 15:3, KJV)
// Principle: Complete observation enables wisdom. Capturing full system context provides the truth needed for discernment and diagnosis.
// Anchor: Context is WHO (user), WHERE (location), WHAT (environment), WHEN (time), WHY (purpose) - the complete picture for understanding what happened.
//
// CPI-SI Identity
//
// Component Type: Context capture module within Rails infrastructure
// Role: Capture complete system state for logging (Detection layer of immune system)
// Paradigm: CPI-SI framework component
//
// Authorship & Lineage
//
// Architect: Seanje Lenox-Wise, Nova Dawn
// Implementation: Nova Dawn
// Creation Date: 2025-11-18
// Version: 1.0.0
// Last Modified: 2025-11-18 - Extracted from monolithic logger.go
//
// Purpose & Function
//
// Purpose: Capture complete system environment state for log entries. Provides WHO, WHERE, WHAT context needed for debugging and understanding execution environment.
//
// Core Design: Graceful degradation pattern - attempt all captures, never fail, return "unknown" when information unavailable.
//
// Key Features:
//   - User identity (username, hostname, PID)
//   - Shell context (type, interactive/non-interactive, login/non-login)
//   - Environment state (automation variables, CPI-SI framework vars)
//   - Sudoers configuration status (installed, valid permissions)
//   - System metrics (CPU load, memory usage, disk usage)
//   - Current working directory
//
// Blocking Status
//
// Non-blocking: All context capture fails gracefully with "unknown" default values. Never blocks logging if system information unavailable.
// Mitigation: Every capture operation has fallback to unknownValue constant.
//
// Usage & Integration
//
// Usage:
//
//	import "system/runtime/lib/logging"
//
// Integration Pattern:
//   1. Logger.CaptureContext() orchestrates complete capture
//   2. Individual capture functions provide specific pieces
//   3. Helper functions (getCurrentUser, getHostname, getCWD) provide identity
//   4. All functions fail gracefully with unknownValue
//
// Public API:
//
//   CaptureContext() *SystemContext - Main orchestration (Logger method)
//   captureSystemMetrics() SystemMetrics - Resource usage snapshot
//   captureShellContext() ShellContext - Shell type and mode
//   captureEnvState() map[string]string - Environment variables
//   captureSudoersContext() SudoersContext - Sudoers configuration
//
// Dependencies
//
// Dependencies (What This Needs):
//   Standard Library: fmt, os, os/exec, path/filepath, runtime, strings
//   Package Files: None (context capture is foundation)
//
// Dependents (What Uses This):
//   Internal: logger.go (all logging methods call CaptureContext)
//
// Health Scoring
//
// Base100 scoring algorithm (CPSI-ALG-001).
//
// Context Capture Operations (30 pts):
//   - Shell detection (interactive/login): +5 (success), +2 (partial), 0 (failure)
//   - Sudoers validation: +5 (correct), +2 (wrong perms), 0 (missing)
//   - Environment state capture: +10 (all vars), +5 (some vars), 0 (none)
//   - System metrics (CPU/Memory/Disk): +10 (all three), +5 (some), 0 (none)
//
// Note: This module's health is about capture completeness, not system health itself.

package logging

// ============================================================================
// SETUP
// ============================================================================

// Imports

import (
	"fmt"           // String formatting for metrics output
	"os"            // File operations, environment variables, process info
	"os/exec"       // System command execution (df command)
	"path/filepath" // Path manipulation for shell basename extraction
	"runtime"       // OS detection (Linux-specific paths)
	"strings"       // String processing for parsing system files
)

// Constants

const (
	//--- System File Paths ---
	// Linux system files for context capture.

	sudoersFilePath = "/etc/sudoers.d/90-cpi-si-safe-operations" // CPI-SI sudoers configuration file
	procLoadAvgPath = "/proc/loadavg"                             // Linux CPU load averages file
	procMeminfoPath = "/proc/meminfo"                             // Linux memory info file

	//--- File Permissions ---
	// Required permissions for security-sensitive files.

	sudoersValidPerms = 0440          // Required permissions for sudoers file (octal)
	permissionsFormat = "%04o"        // Octal format for permissions display

	//--- Format Strings ---
	// Output formatting for system metrics.

	loadAvgFormat     = "%s, %s, %s"  // CPU load averages format
	memoryUsageFormat = "%dMB / %dMB" // Memory usage format
	diskUsageFormat   = "%s / %s (%s)" // Disk usage format

	//--- Environment Variables ---
	// Shell and framework environment variable names.

	frameworkEnvPrefix = "CPI_SI_" // Prefix for CPI-SI framework environment variables
	shellLvlEnvVar     = "SHLVL"   // Shell level environment variable
	shellArgEnvVar     = "0"       // Shell $0 argument variable
	loginShellPrefix   = "-"       // Login shell prefix in $0
	loginShellLevel    = "1"       // SHLVL value for login shells

	//--- System Commands ---
	// External commands and their arguments.

	dfCommand   = "df" // Disk free command
	dfHumanFlag = "-h" // Human-readable flag for df

	//--- Graceful Failure Values ---
	// Default values when context capture fails.

	unknownValue = "unknown" // Graceful failure return value for context capture

	//--- Numeric Constants ---
	// Mathematical constants and conversion factors.

	kbToMbDivisor = 1024 // Divisor to convert KB to MB
)

// Types

// ShellContext captures which shell is running and how it's running.
//
// Used by SystemContext to record shell environment during context capture.
// Interactive/login flags determine shell behavior (prompts, profile loading).
type ShellContext struct {
	Type        string // Shell program (bash, zsh, sh, etc.)
	Interactive bool   // Interactive mode (true = terminal with prompts, false = script execution)
	Login       bool   // Login shell (true = full profile loaded, false = lightweight sub-shell)
}

// SudoersContext captures whether passwordless sudo is configured correctly.
//
// Used by SystemContext to verify safe operations configuration. Tracks both
// file existence and correct permissions (must be 0440 for sudoers.d files).
type SudoersContext struct {
	Installed   bool   // File installed (true = exists at /etc/sudoers.d/90-cpi-si-safe-operations, false = missing)
	Valid       bool   // Permissions valid (true = correct 0440, false = wrong permissions)
	Permissions string // Actual permissions (octal string)
}

// SystemMetrics captures how busy the computer is at this exact moment.
//
// Used by SystemContext to record system load snapshot. Provides debugging
// context for performance-related issues.
type SystemMetrics struct {
	Load   string // CPU load averages (1min, 5min, 15min from /proc/loadavg)
	Memory string // RAM usage (used/total MB from /proc/meminfo)
	Disk   string // Disk space (used/total with % from df command)
}

// SystemContext captures everything about the system at this exact moment.
//
// Composes all building blocks (ShellContext, SudoersContext, SystemMetrics)
// into complete environment snapshot. Used by LogEntry for full context capture.
type SystemContext struct {
	User     string            // Username running process
	Host     string            // Computer hostname
	PID      int               // Process ID
	Shell    ShellContext      // Shell configuration
	CWD      string            // Current working directory
	EnvState map[string]string // Relevant environment variables
	Sudoers  SudoersContext    // Sudo configuration
	System   SystemMetrics     // Resource usage snapshot
}

// Type Methods

// Format converts ShellContext to human-readable string for logs.
//
// Returns formatted string like "bash (interactive, login)" for log display.
func (s ShellContext) Format() string {
	interactive := "non-interactive" // Default mode string
	if s.Interactive {                // Terminal mode
		interactive = "interactive" // Override with interactive string
	}
	login := "non-login" // Default login string
	if s.Login {          // Login shell
		login = "login" // Override with login string
	}
	return fmt.Sprintf("%s (%s, %s)", s.Type, interactive, login) // Combine: "bash (interactive, login)"
}

// ToMap converts SudoersContext to map format for structured logging.
//
// Returns map with installed status, validity, and permissions for debugging output.
func (s SudoersContext) ToMap() map[string]string {
	return map[string]string{
		"installed":   fmt.Sprintf("%v", s.Installed),   // Convert bool to string
		"valid":       fmt.Sprintf("%v", s.Valid),       // Convert bool to string
		"permissions": s.Permissions,                    // Already string
	}
}

// ToMap converts SystemMetrics to map format for structured logging.
//
// Returns map with CPU load, memory usage, and disk usage for debugging output.
func (m SystemMetrics) ToMap() map[string]string {
	return map[string]string{
		"load":   m.Load,   // CPU load averages
		"memory": m.Memory, // RAM usage
		"disk":   m.Disk,   // Disk usage
	}
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Helpers - Foundation Functions
// ────────────────────────────────────────────────────────────────

// getCurrentUser retrieves the current username from $USER environment variable.
func getCurrentUser() string {
	if user := os.Getenv("USER"); user != "" { // $USER environment variable set
		return user // Return actual username
	}
	return unknownValue // Fail gracefully with constant
}

// getHostname retrieves the system hostname.
func getHostname() string {
	if host, err := os.Hostname(); err == nil { // Hostname lookup succeeded
		return host // Return actual hostname
	}
	return unknownValue // Fail gracefully with constant
}

// getCWD retrieves the current working directory.
func getCWD() string {
	if cwd, err := os.Getwd(); err == nil { // CWD lookup succeeded
		return cwd // Return actual working directory
	}
	return unknownValue // Fail gracefully with constant
}

// isTerminal checks if a file descriptor is a terminal (TTY).
func isTerminal(file *os.File) bool {
	stat, err := file.Stat()                   // Get file stat info
	if err != nil {                             // Stat failed
		return false                            // Not a terminal
	}
	return (stat.Mode() & os.ModeCharDevice) != 0 // Check character device bit
}

// ────────────────────────────────────────────────────────────────
// Core Operations - Context Capture
// ────────────────────────────────────────────────────────────────

// captureShellContext captures shell type and execution mode.
func captureShellContext() ShellContext {
	shell := os.Getenv("SHELL") // Get shell path from environment
	if shell == "" {             // $SHELL not set
		shell = unknownValue // Use constant for graceful failure
	}

	// Determine shell type from path
	shellType := filepath.Base(shell) // Extract basename (e.g., /bin/bash → bash)

	// Interactive if stdin is a terminal
	interactive := isTerminal(os.Stdin) // Check if stdin is TTY

	// Login shell typically has - prefix in $0 or SHLVL=1
	login := strings.HasPrefix(os.Getenv(shellArgEnvVar), loginShellPrefix) || os.Getenv(shellLvlEnvVar) == loginShellLevel // Login shell detection

	return ShellContext{ // Return populated shell context
		Type:        shellType,   // Shell type (bash, zsh, etc.)
		Interactive: interactive, // Interactive vs non-interactive
		Login:       login,       // Login vs non-login
	}
}

// captureEnvState captures automation and CPI-SI framework environment variables.
func captureEnvState() map[string]string {
	envVars := make(map[string]string)

	// Capture non-interactive environment variables
	relevantVars := []string{
		"DEBIAN_FRONTEND",
		"NEEDRESTART_MODE",
		"NEEDRESTART_SUSPEND",
		"PIP_NO_INPUT",
		"NPM_CONFIG_YES",
		"GIT_EDITOR",
		"EDITOR",
		"VISUAL",
	}

	for _, varName := range relevantVars {         // Iterate relevant variables
		if value := os.Getenv(varName); value != "" { // Variable is set
			envVars[varName] = value // Add to map
		}
	}

	// Capture all CPI_SI_* framework variables
	for _, env := range os.Environ() {                // Iterate all environment variables
		if strings.HasPrefix(env, frameworkEnvPrefix) { // Matches CPI_SI_* prefix
			parts := strings.SplitN(env, "=", 2) // Split on first = only
			if len(parts) == 2 {                  // Valid key=value format
				envVars[parts[0]] = parts[1] // Add framework var to map
			}
		}
	}

	return envVars // Return collected environment state
}

// captureSudoersContext captures sudoers configuration state (existence and permissions).
func captureSudoersContext() SudoersContext {
	// Direct file system check to avoid circular dependency with sudoers library
	permissions := unknownValue // Default to unknown if file doesn't exist
	installed := false           // Assume not installed until verified
	valid := false               // Assume not valid until verified

	if info, err := os.Stat(sudoersFilePath); err == nil { // Sudoers file exists
		installed = true                                              // Mark as installed
		permissions = fmt.Sprintf(permissionsFormat, info.Mode().Perm()) // Capture actual permissions

		// Quick validity check without calling sudoers.Check()
		// (Just check if file exists and has correct permissions)
		if info.Mode().Perm() == sudoersValidPerms { // Permissions match expected 0440
			valid = true // Mark as valid
		}
	}

	return SudoersContext{ // Return sudoers state
		Installed:   installed,   // Whether file exists
		Valid:       valid,       // Whether permissions are correct
		Permissions: permissions, // Actual permissions or "unknown"
	}
}

// captureLoadAvg captures CPU load averages from /proc/loadavg.
func captureLoadAvg() string {
	// Read /proc/loadavg on Linux
	if runtime.GOOS == "linux" { // Linux-specific implementation
		if data, err := os.ReadFile(procLoadAvgPath); err == nil { // Read succeeded
			fields := strings.Fields(string(data))     // Parse space-separated fields
			if len(fields) >= 3 {                       // At least 3 fields (1min, 5min, 15min)
				return fmt.Sprintf(loadAvgFormat, fields[0], fields[1], fields[2]) // Return formatted load
			}
		}
	}
	return unknownValue // Fail gracefully with constant
}

// captureMemoryUsage captures RAM usage from /proc/meminfo.
func captureMemoryUsage() string {
	// Read /proc/meminfo on Linux
	if runtime.GOOS == "linux" { // Linux-specific implementation
		if data, err := os.ReadFile(procMeminfoPath); err == nil { // Read succeeded
			lines := strings.Split(string(data), "\n") // Split into lines
			var total, available int64                  // Memory values in KB

			for _, line := range lines {   // Parse each line
				fields := strings.Fields(line) // Split on whitespace
				if len(fields) >= 2 {           // Valid key-value line
					switch fields[0] { // Check field name
					case "MemTotal:": // Total RAM line
						fmt.Sscanf(fields[1], "%d", &total) // Parse KB value
					case "MemAvailable:": // Available RAM line
						fmt.Sscanf(fields[1], "%d", &available) // Parse KB value
					}
					if total > 0 && available > 0 { // Both values found
						break // Stop parsing remaining lines
					}
				}
			}

			if total > 0 && available > 0 {    // Both values parsed successfully
				used := total - available       // Calculate used memory
				return fmt.Sprintf(memoryUsageFormat, used/kbToMbDivisor, total/kbToMbDivisor) // Format as MB
			}
		}
	}
	return unknownValue // Fail gracefully with constant
}

// captureDiskUsage captures disk usage for current working directory filesystem using df.
func captureDiskUsage() string {
	// Get disk usage for current working directory
	cwd := getCWD() // Get current directory (or "unknown")

	if runtime.GOOS == "linux" {                        // Linux-specific implementation
		cmd := exec.Command(dfCommand, dfHumanFlag, cwd) // df -h for human-readable output
		if output, err := cmd.Output(); err == nil {     // Command succeeded
			lines := strings.Split(string(output), "\n") // Split output into lines
			if len(lines) >= 2 {                          // Has header + data line
				fields := strings.Fields(lines[1])        // Parse data line (second line)
				if len(fields) >= 5 {                      // Has filesystem, size, used, avail, use%, mount
					return fmt.Sprintf(diskUsageFormat, fields[2], fields[1], fields[4]) // Format: used / total (percentage)
				}
			}
		}
	}
	return unknownValue // Fail gracefully with constant
}

// captureSystemMetrics orchestrates complete system resource metrics capture.
func captureSystemMetrics() SystemMetrics {
	return SystemMetrics{ // Orchestrate metric capture
		Load:   captureLoadAvg(),    // CPU load averages
		Memory: captureMemoryUsage(), // RAM usage
		Disk:   captureDiskUsage(),   // Disk usage for CWD
	}
}

// ────────────────────────────────────────────────────────────────
// Logger Methods - Context Orchestration
// ────────────────────────────────────────────────────────────────

// CaptureContext orchestrates complete system state capture (WHO, WHERE, WHY).
func (l *Logger) CaptureContext() *SystemContext {
	return &SystemContext{ // Orchestrate complete context capture
		User:     l.username,             // Pre-computed username (captured once at initialization)
		Host:     l.hostname,             // Pre-computed hostname (captured once at initialization)
		PID:      l.pid,                  // Pre-computed PID (captured once at initialization)
		Shell:    captureShellContext(),  // Shell type and mode (dynamic - can change)
		CWD:      getCWD(),                // Current working directory (dynamic - can change)
		EnvState: captureEnvState(),       // Environment variables (dynamic - can change)
		Sudoers:  captureSudoersContext(), // Sudoers configuration (dynamic - can change)
		System:   captureSystemMetrics(),  // System resource metrics (dynamic - constantly changing)
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// Library module (no entry point). Import: "system/runtime/lib/logging"
//
// ============================================================================
// END CLOSING
// ============================================================================
