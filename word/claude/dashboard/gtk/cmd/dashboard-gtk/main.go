// ============================================================================
// METADATA - CPI-SI Dashboard GTK4 Entry Point
// ============================================================================
//
// Purpose: Launch the GTK4 native dashboard application
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
// Biblical: Habakkuk 2:2 - "Write the vision, and make it plain upon tables"
//
// Usage:
//   dashboard-gtk              Launch the GTK4 dashboard
//   dashboard-gtk --version    Show version
//   dashboard-gtk --title T    Set window title
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

	"github.com/creativeworkzstudio/claude-global/dashboard/gtk/app"
	"github.com/creativeworkzstudio/claude-global/pkg/dashboard"
)

// Build-time variables
var (
	Version = "1.0.0"
)

// ============================================================================
// BODY
// ============================================================================

func main() {
	showVersion := flag.Bool("version", false, "Show version and exit")
	title := flag.String("title", "", "Override window title")
	flag.Parse()

	if *showVersion {
		fmt.Printf("dashboard-gtk %s\n", Version)
		os.Exit(0)
	}

	// Create dashboard service
	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()

	svc, err := dashboard.New(ctx)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to create dashboard service: %v\n", err)
		os.Exit(1)
	}
	defer svc.Close()

	// Create and run GTK4 application
	opts := app.Options{
		Title: *title,
	}
	dashApp := app.NewWithOptions(svc, opts)
	defer dashApp.Close()

	status := dashApp.Run(os.Args[:1]) // Pass only argv[0] to GTK
	os.Exit(status)
}

// ============================================================================
// CLOSING
// ============================================================================
