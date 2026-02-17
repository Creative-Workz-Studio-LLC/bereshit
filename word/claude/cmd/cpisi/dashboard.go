// ============================================================================
// METADATA - CPI-SI CLI Dashboard Launcher
// ============================================================================
//
// Purpose: Launch web, TUI, or GTK4 dashboards
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"os"
	"path/filepath"
)

// Dashboard defines a launchable dashboard variant.
type Dashboard struct {
	Name        string
	Binary      string
	Description string
	NeedsGTK    bool
	NeedsTTY    bool
}

var dashboards = []Dashboard{
	{
		Name:        "web",
		Binary:      "dashboard",
		Description: "Web dashboard — browser-based, port 8470",
	},
	{
		Name:        "tui",
		Binary:      "dashboard-tui",
		Description: "Terminal dashboard — Bubble Tea, needs real TTY",
		NeedsTTY:    true,
	},
	{
		Name:        "gtk",
		Binary:      "dashboard-gtk",
		Description: "Native desktop dashboard — GTK4, window manager",
		NeedsGTK:    true,
	},
}

// ============================================================================
// BODY
// ============================================================================

// cmdDashboard handles dashboard subcommands.
func cmdDashboard(args []string, paths *Paths) error {
	if len(args) == 0 {
		return fmt.Errorf("missing dashboard type\n\n" +
			"Usage: cpisi dashboard <web|tui|gtk|list>\n\n" +
			"  web [--port N]    Web dashboard (default port 8470)\n" +
			"  tui [--log file]  Terminal dashboard (Bubble Tea)\n" +
			"  gtk               Native desktop dashboard (GTK4)\n" +
			"  list              List available dashboards")
	}

	mode := args[0]
	passArgs := args[1:]

	if mode == "list" {
		return listDashboards(paths)
	}

	// Find the dashboard
	var dash *Dashboard
	for i := range dashboards {
		if dashboards[i].Name == mode {
			dash = &dashboards[i]
			break
		}
	}
	if dash == nil {
		return fmt.Errorf("unknown dashboard: %s\nAvailable: web, tui, gtk", mode)
	}

	// Find the binary
	binPath := filepath.Join(paths.InstallDir, "bin", dash.Binary)
	if !isExecutable(binPath) {
		// Try source bin/ as fallback
		binPath = filepath.Join(paths.SourceDir, "bin", dash.Binary)
		if !fileExists(binPath) {
			return fmt.Errorf("%s dashboard binary not found\n"+
				"Run 'cpisi build' to build it, then 'cpisi install --bin' to install",
				dash.Name)
		}
	}

	// Launch
	fmt.Printf("Launching %s dashboard...\n", dash.Name)
	return execBinary(binPath, passArgs)
}

// listDashboards shows available dashboards and their status.
func listDashboards(paths *Paths) error {
	header("Available Dashboards")
	fmt.Println()

	for _, dash := range dashboards {
		binPath := filepath.Join(paths.InstallDir, "bin", dash.Binary)
		status := "not installed"
		if isExecutable(binPath) {
			info, _ := os.Stat(binPath)
			size := float64(info.Size()) / (1024 * 1024)
			status = fmt.Sprintf("installed (%.1fMB)", size)
		} else if fileExists(filepath.Join(paths.SourceDir, "bin", dash.Binary)) {
			status = "built (not installed)"
		}

		fmt.Printf("  %-6s  %-12s  %s\n", dash.Name, status, dash.Description)
	}

	fmt.Println()
	info("Launch with: cpisi dashboard <web|tui|gtk>")
	return nil
}

// ============================================================================
// CLOSING
// ============================================================================
