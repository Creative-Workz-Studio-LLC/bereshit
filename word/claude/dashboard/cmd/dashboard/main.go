// ============================================================================
// METADATA - CPI-SI Dashboard Entry Point
// ============================================================================
//
// Key: claude-global-dashboard-cmd
// Purpose: Entry point for the CPI-SI web dashboard server
// Biblical: Habakkuk 2:2 - "Write the vision, and make it plain upon tables"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-07
//
// Usage:
//   ./dashboard              # Start on default port 8470
//   ./dashboard --port 9090  # Start on custom port
//
// Port 8470: 8=new beginnings, 4=creation, 70=completeness
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
	"os/signal"
	"syscall"

	"github.com/creativeworkzstudio/claude-global/dashboard/server"
	"github.com/creativeworkzstudio/claude-global/pkg/dashboard"
)

// ============================================================================
// BODY
// ============================================================================

func main() {
	// Parse flags
	port := flag.Int("port", 8470, "Port to listen on (default: 8470)")
	flag.Parse()

	// Create root context with cancellation
	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()

	// Create the shared data service
	svc, err := dashboard.New(ctx)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to initialize dashboard service: %v\n", err)
		os.Exit(1)
	}
	defer svc.Close()

	// Create and start the HTTP server
	srv := server.New(svc, *port)

	// Handle graceful shutdown
	sigCh := make(chan os.Signal, 1)
	signal.Notify(sigCh, syscall.SIGINT, syscall.SIGTERM)

	go func() {
		<-sigCh
		fmt.Println("\nShutting down CPI-SI Dashboard...")
		if err := srv.Shutdown(ctx); err != nil {
			fmt.Fprintf(os.Stderr, "Shutdown error: %v\n", err)
		}
		cancel()
	}()

	// Start serving
	fmt.Printf("CPI-SI Dashboard running at http://localhost:%d\n", *port)
	if err := srv.ListenAndServe(); err != nil {
		fmt.Fprintf(os.Stderr, "Server error: %v\n", err)
		os.Exit(1)
	}
}

// ============================================================================
// CLOSING
// ============================================================================
// "Write the vision, and make it plain upon tables,
//  that he may run that readeth it." — Habakkuk 2:2
