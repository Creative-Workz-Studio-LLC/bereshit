// ============================================================================
// METADATA
// ============================================================================
// Check Awareness - Session duration, circadian awareness, and work patterns
// Part of: session-awareness skill system
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-03
// Purpose: CLI wrapper providing formatted session awareness information
//
// Usage:
//   check-awareness duration    # Check session duration only
//   check-awareness circadian   # Check circadian awareness only
//   check-awareness full        # Full session status (default)
//   check-awareness patterns    # Show learned work patterns
//
// Dependencies: session-time, session-log, session-patterns binaries
// Health Scoring: Base100 - Command execution and formatting

package main

// ============================================================================
// SETUP - Imports, Dependencies, Globals
// ============================================================================

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"time"
)

const (
	BinDir = ".claude/system/bin"
)

// ============================================================================
// BODY - Core Functionality
// ============================================================================

// runCommand executes a system command and returns output
func runCommand(command string, args ...string) (string, error) {
	cmd := exec.Command(command, args...)
	output, err := cmd.CombinedOutput()
	if err != nil {
		return "", fmt.Errorf("command failed: %s - %v", command, err)
	}
	return strings.TrimSpace(string(output)), nil
}

// getBinPath returns absolute path to a binary in system/bin
func getBinPath(binary string) string {
	homeDir, err := os.UserHomeDir()
	if err != nil {
		return filepath.Join("/home", os.Getenv("USER"), BinDir, binary)
	}
	return filepath.Join(homeDir, BinDir, binary)
}

// getTimeOfDay returns current time category
func getTimeOfDay() string {
	hour := time.Now().Hour()

	if hour >= 5 && hour < 12 {
		return "morning"
	}
	if hour >= 12 && hour < 17 {
		return "afternoon"
	}
	if hour >= 17 && hour < 21 {
		return "evening"
	}
	return "night"
}

// isWorkHours returns true if current time is typical work hours
func isWorkHours() bool {
	now := time.Now()
	hour := now.Hour()
	day := now.Weekday()

	// Weekday 9am-5pm (learned pattern)
	if day >= time.Monday && day <= time.Friday {
		return hour >= 9 && hour < 17
	}

	// Weekend - flexible
	return false
}

// formatSessionCheck creates formatted output with session status and circadian awareness
func formatSessionCheck() (string, error) {
	// Get session log status
	sessionStatus, err := runCommand(getBinPath("session-log"), "status")
	if err != nil {
		return "", fmt.Errorf("failed to get session status: %w", err)
	}

	// Get circadian check
	circadianCheck, err := runCommand(getBinPath("session-patterns"), "check")
	if err != nil {
		return "", fmt.Errorf("failed to get circadian awareness: %w", err)
	}

	// Get session time
	sessionTime, err := runCommand(getBinPath("session-time"), "check")
	if err != nil {
		// Non-critical if this fails
		sessionTime = "Session time unavailable"
	}

	timeOfDay := getTimeOfDay()
	workHoursStatus := "⚠️  No"
	if isWorkHours() {
		workHoursStatus = "✅ Yes"
	}

	output := fmt.Sprintf(`
🕐 Session Status
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
%s

📊 Session Time
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
%s

🌙 Circadian Awareness
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Time of Day: %s
Typical Work Hours: %s

%s
`, sessionStatus, sessionTime, timeOfDay, workHoursStatus, circadianCheck)

	return output, nil
}

// printUsage displays command usage information
func printUsage() {
	fmt.Println(`
Usage: check-awareness [command]

Commands:
    duration    Check session duration only
    circadian   Check circadian awareness only
    full        Full session status (default)
    patterns    Show learned work patterns
    help        Show this help message

Examples:
    check-awareness duration
    check-awareness circadian
    check-awareness full
    check-awareness patterns
`)
}

// ============================================================================
// CLOSING - Main Entry Point
// ============================================================================

func main() {
	// Parse command argument
	command := "full"
	if len(os.Args) > 1 {
		command = os.Args[1]
	}

	var output string
	var err error

	switch command {
	case "duration":
		output, err = runCommand(getBinPath("session-time"), "check")

	case "circadian":
		output, err = runCommand(getBinPath("session-patterns"), "check")

	case "patterns":
		output, err = runCommand(getBinPath("session-patterns"), "show")

	case "full":
		output, err = formatSessionCheck()

	case "help", "--help", "-h":
		printUsage()
		os.Exit(0)

	default:
		fmt.Fprintf(os.Stderr, "Unknown command: %s\n", command)
		printUsage()
		os.Exit(1)
	}

	if err != nil {
		fmt.Fprintf(os.Stderr, "❌ Error: %v\n", err)
		os.Exit(1)
	}

	fmt.Println(output)
	os.Exit(0)
}
