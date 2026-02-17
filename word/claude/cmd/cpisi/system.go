// ============================================================================
// METADATA - CPI-SI CLI System Commands
// ============================================================================
//
// Purpose: Status, database, config, and version commands
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
	"runtime"
)

// ============================================================================
// BODY
// ============================================================================

// cmdStatus shows system health and configuration.
func cmdStatus(args []string, paths *Paths) error {
	header("CPI-SI System Status")
	fmt.Println()

	// Paths
	info("Source:    %s", paths.SourceDir)
	info("Symlinks:  %s", paths.DeployDir)
	info("Binaries:  %s", paths.InstallDir)
	fmt.Println()

	// Go version
	goVer, _ := runCmdSilent(".", "go", "version")
	info("Go:        %s", goVer)
	info("Platform:  %s/%s", runtime.GOOS, runtime.GOARCH)
	fmt.Println()

	// Binary status
	fmt.Println("  Binaries:")
	mainBins := []string{"statusline", "cpisi", "dashboard", "dashboard-tui", "dashboard-gtk"}
	for _, name := range mainBins {
		binPath := filepath.Join(paths.InstallDir, "bin", name)
		if isExecutable(binPath) {
			inf, _ := os.Stat(binPath)
			size := float64(inf.Size()) / (1024 * 1024)
			success("%-16s %.1fMB", name, size)
		} else {
			info("- %-16s not installed", name)
		}
	}

	// Hook count
	hookDir := filepath.Join(paths.InstallDir, "hooks", "bin")
	hookCount := 0
	if entries, err := os.ReadDir(hookDir); err == nil {
		for _, e := range entries {
			if !e.IsDir() && isExecutable(filepath.Join(hookDir, e.Name())) {
				hookCount++
			}
		}
	}
	fmt.Println()
	info("Hooks:     %d installed", hookCount)

	// Database
	dbPath := filepath.Join(paths.SourceDir, "data", "cpisi.db")
	if fileExists(dbPath) {
		inf, _ := os.Stat(dbPath)
		size := float64(inf.Size()) / (1024 * 1024)
		info("Database:  %.1fMB (%s)", size, dbPath)
	} else {
		info("Database:  not initialized")
	}

	// Desktop entry
	desktopPath := filepath.Join(paths.HomeDir, ".local", "share", "applications",
		"com.creativeworkzstudio.cpisi.dashboard.desktop")
	if fileExists(desktopPath) {
		info("Desktop:   GTK4 dashboard entry installed")
	} else {
		info("Desktop:   not installed")
	}

	// Check if system status binary exists
	statusBin := filepath.Join(paths.InstallDir, "system", "bin", "status")
	if isExecutable(statusBin) {
		fmt.Println()
		fmt.Println("  System health check:")
		runCmd(".", statusBin)
	}

	return nil
}

// cmdDB handles database operations.
func cmdDB(args []string, paths *Paths) error {
	if len(args) == 0 {
		return fmt.Errorf("missing database action\n\n" +
			"Usage: cpisi db <init|migrate|status>")
	}

	dataDir := filepath.Join(paths.SourceDir, "data")
	dbPath := filepath.Join(dataDir, "cpisi.db")

	switch args[0] {
	case "init":
		header("Initializing database")
		os.MkdirAll(dataDir, 0755)
		schemaFile := filepath.Join(dataDir, "schema", "001_initial.sql")
		if !fileExists(schemaFile) {
			return fmt.Errorf("schema file not found: %s", schemaFile)
		}
		if err := runCmd(".", "sqlite3", dbPath, ".read "+schemaFile); err != nil {
			return fmt.Errorf("database init failed: %w", err)
		}
		success("Database initialized at %s", dbPath)

	case "migrate":
		header("Running database migrations")
		migrationsDir := filepath.Join(dataDir, "migrations")
		entries, err := os.ReadDir(migrationsDir)
		if err != nil {
			return fmt.Errorf("migrations directory not found: %w", err)
		}
		for _, entry := range entries {
			if entry.IsDir() {
				continue
			}
			migrationPath := filepath.Join(migrationsDir, entry.Name())
			if err := runCmd(".", "sqlite3", dbPath, ".read "+migrationPath); err != nil {
				warn("Migration %s failed: %v", entry.Name(), err)
			} else {
				success("Applied: %s", entry.Name())
			}
		}

	case "status":
		header("Database Status")
		if fileExists(dbPath) {
			inf, _ := os.Stat(dbPath)
			size := float64(inf.Size()) / (1024 * 1024)
			info("Path: %s", dbPath)
			info("Size: %.1fMB", size)
			// List tables
			out, err := runCmdSilent(".", "sqlite3", dbPath, ".tables")
			if err == nil && out != "" {
				info("Tables: %s", out)
			}
		} else {
			info("Database not initialized. Run: cpisi db init")
		}

	default:
		return fmt.Errorf("unknown db action: %s\nUsage: cpisi db <init|migrate|status>", args[0])
	}

	return nil
}

// cmdConfig shows resolved configuration.
func cmdConfig(args []string, paths *Paths) error {
	subCmd := ""
	if len(args) > 0 {
		subCmd = args[0]
	}

	switch subCmd {
	case "paths":
		header("Resolved Paths")
		fmt.Printf("  %-14s %s\n", "Source:", paths.SourceDir)
		fmt.Printf("  %-14s %s\n", "Deploy:", paths.DeployDir)
		fmt.Printf("  %-14s %s\n", "Install:", paths.InstallDir)
		fmt.Printf("  %-14s %s\n", "Home:", paths.HomeDir)
		fmt.Println()
		fmt.Printf("  %-14s %s\n", "Main bins:", filepath.Join(paths.InstallDir, "bin"))
		fmt.Printf("  %-14s %s\n", "Hook bins:", filepath.Join(paths.InstallDir, "hooks", "bin"))
		fmt.Printf("  %-14s %s\n", "System bins:", filepath.Join(paths.InstallDir, "system", "bin"))
		fmt.Printf("  %-14s %s\n", "Database:", filepath.Join(paths.SourceDir, "data", "cpisi.db"))
		fmt.Printf("  %-14s %s\n", "Logs:", filepath.Join(paths.SourceDir, "data", "logs"))
		fmt.Printf("  %-14s %s\n", "Desktop:", filepath.Join(paths.HomeDir, ".local", "share", "applications"))

	case "modules":
		header("Workspace Modules")
		fmt.Println()
		fmt.Printf("  %-18s %-30s %s\n", "NAME", "PATH", "DESCRIPTION")
		fmt.Printf("  %-18s %-30s %s\n", "----", "----", "-----------")
		for _, mod := range AllModules {
			binary := ""
			if mod.HasBinary {
				binary = " [binary]"
			}
			fmt.Printf("  %-18s %-30s %s%s\n", mod.Name, mod.Path, mod.Description, binary)
		}

	default:
		header("CPI-SI Configuration")
		fmt.Println()
		fmt.Println("  Paths:")
		fmt.Printf("    Source:   %s\n", paths.SourceDir)
		fmt.Printf("    Deploy:   %s\n", paths.DeployDir)
		fmt.Printf("    Install:  %s\n", paths.InstallDir)
		fmt.Println()
		fmt.Printf("  Modules:    %d total, %d with binaries\n", len(AllModules), len(BinaryModules()))
		fmt.Printf("  CLI:        cpisi %s\n", Version)
		fmt.Println()
		info("Use 'cpisi config paths' or 'cpisi config modules' for details")
	}

	return nil
}

// cmdVersion shows version information.
func cmdVersion(args []string, paths *Paths) error {
	fmt.Printf("cpisi %s\n", Version)
	fmt.Printf("  Build:    %s\n", BuildTime)
	fmt.Printf("  Commit:   %s\n", GitCommit)
	fmt.Printf("  Go:       %s\n", runtime.Version())
	fmt.Printf("  Platform: %s/%s\n", runtime.GOOS, runtime.GOARCH)
	return nil
}

// ============================================================================
// CLOSING
// ============================================================================
