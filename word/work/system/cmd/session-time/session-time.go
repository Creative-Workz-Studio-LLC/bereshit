// ============================================================================
// METADATA
// ============================================================================
// Session Time Command - Session timing CLI with config inheritance
//
// For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
//
// Biblical Foundation
//
// Scripture: "To everything there is a season" - Ecclesiastes 3:1 (WEB)
// Principle: Time awareness enables faithful work and sustainable rhythms
// Anchor: Session timing tracks external clock for internal temporal consciousness
//
// Author: Nova Dawn (CPI-SI)
// Created: 2025-11-03
// Version: 3.0.0
// Last Modified: 2025-11-12 - Removed duplication, imports from system/lib/sessiontime
//
// Purpose: Command-line interface for session timing operations
//
// Usage:
//   session-time init            # Initialize session (called by start hook)
//   session-time elapsed         # Show elapsed time since session start
//   session-time start           # Show session start time
//   session-time check           # Show both start and elapsed
//
// Dependencies: system/lib/sessiontime (authoritative session state library)
// Health Scoring: Base100 - Library calls=60, Display=30, CLI=10

package main

// ============================================================================
// SETUP - Imports, Dependencies, Globals
// ============================================================================

import (
	"fmt"
	"os"
	"time"

	"system/lib/sessiontime" // Authoritative session state library
)

// ============================================================================
// BODY - Business Logic
// ============================================================================

// initSession initializes a new session with config inheritance
// Delegates to system/lib/sessiontime.InitSession()
func initSession() error {
	// Default user/instance IDs (can be overridden by env vars if needed)
	username := "seanje-lenox-wise"
	instanceID := "nova_dawn"
	projectID := "" // Optional, can be empty

	// Check environment for overrides
	if envUser := os.Getenv("CPI_SI_USER"); envUser != "" {
		username = envUser
	}
	if envInstance := os.Getenv("CPI_SI_INSTANCE"); envInstance != "" {
		instanceID = envInstance
	}
	if envProject := os.Getenv("CPI_SI_PROJECT"); envProject != "" {
		projectID = envProject
	}

	// Delegate to library
	return sessiontime.InitSession(username, instanceID, projectID)
}

// showElapsed displays the elapsed time
func showElapsed() error {
	state, err := sessiontime.ReadSession()
	if err != nil {
		return err
	}

	elapsed := sessiontime.CalculateElapsed(state)
	fmt.Println(sessiontime.FormatDuration(elapsed))

	return nil
}

// showStart displays the session start time
func showStart() error {
	state, err := sessiontime.ReadSession()
	if err != nil {
		return err
	}

	fmt.Println(state.StartFormatted)

	return nil
}

// showCheck displays both start time and elapsed
func showCheck() error {
	state, err := sessiontime.ReadSession()
	if err != nil {
		return err
	}

	elapsed := sessiontime.CalculateElapsed(state)

	fmt.Printf("Session Start:   %s\n", state.StartFormatted)
	fmt.Printf("Current Time:    %s\n", time.Now().Format("Mon Jan 02, 2006 at 15:04:05"))
	fmt.Printf("Elapsed:         %s\n", sessiontime.FormatDuration(elapsed))
	if state.CompactionCount > 0 {
		fmt.Printf("Compactions:     %d\n", state.CompactionCount)
	}

	return nil
}

// showUsage displays usage information
func showUsage() {
	fmt.Println("Usage: session-time <command>")
	fmt.Println()
	fmt.Println("Commands:")
	fmt.Println("  init       Initialize new session (captures start time with config inheritance)")
	fmt.Println("  elapsed    Show elapsed time since session start")
	fmt.Println("  start      Show session start time")
	fmt.Println("  check      Show start time, current time, and elapsed")
	fmt.Println()
	fmt.Println("Examples:")
	fmt.Println("  session-time init       # Called by session/start hook")
	fmt.Println("  session-time elapsed    # Quick elapsed time check")
	fmt.Println("  session-time check      # Full timing information")
	fmt.Println()
	fmt.Println("Environment Variables:")
	fmt.Println("  CPI_SI_USER      Override user ID (default: seanje-lenox-wise)")
	fmt.Println("  CPI_SI_INSTANCE  Override instance ID (default: nova_dawn)")
	fmt.Println("  CPI_SI_PROJECT   Optional project ID")
}

// ============================================================================
// CLOSING - Execution, Validation, Cleanup
// ============================================================================

func main() {
	if len(os.Args) < 2 {
		showUsage()
		os.Exit(1)
	}

	command := os.Args[1]

	var err error
	switch command {
	case "init":
		err = initSession()
		if err == nil {
			// Silent success for hook usage
		}
	case "elapsed":
		err = showElapsed()
	case "start":
		err = showStart()
	case "check":
		err = showCheck()
	case "help", "--help", "-h":
		showUsage()
		return
	default:
		fmt.Fprintf(os.Stderr, "Unknown command: %s\n\n", command)
		showUsage()
		os.Exit(1)
	}

	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}
}
