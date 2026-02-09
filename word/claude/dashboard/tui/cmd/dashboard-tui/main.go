// ============================================================================
// METADATA - CPI-SI Dashboard TUI Entry Point
// ============================================================================
//
// Key: claude-global-dashboard-tui-cmd
// Purpose: Entry point for the CPI-SI Bubble Tea terminal dashboard
// Biblical: Habakkuk 2:2 - "Write the vision, and make it plain upon tables"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// Usage:
//   ./dashboard-tui            # Start TUI dashboard
//   ./dashboard-tui --log tui.log  # Start with debug logging
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"flag"
	"fmt"
	"os"

	tea "github.com/charmbracelet/bubbletea"
	"cws.studio/dashboard/tui"
	"cws.studio/pkg/dashboard"
)

// ============================================================================
// BODY
// ============================================================================

func main() {
	logFile := flag.String("log", "", "Log file for debug output")
	flag.Parse()

	// Optional debug logging
	if *logFile != "" {
		f, err := tea.LogToFile(*logFile, "dashboard-tui")
		if err != nil {
			fmt.Fprintf(os.Stderr, "Could not open log file: %v\n", err)
			os.Exit(1)
		}
		defer f.Close()
	}

	// Create root context
	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()

	// Create the shared data service
	svc, err := dashboard.New(ctx)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to initialize dashboard service: %v\n", err)
		os.Exit(1)
	}
	defer svc.Close()

	// Create and run the Bubble Tea program
	model := tui.NewModel(svc)
	p := tea.NewProgram(model, tea.WithAltScreen(), tea.WithMouseCellMotion())

	if _, err := p.Run(); err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// "Write the vision, and make it plain upon tables,
//  that he may run that readeth it." — Habakkuk 2:2
