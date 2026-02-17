// ============================================================================
// METADATA - CPI-SI CLI Install Commands
// ============================================================================
//
// Purpose: Install, uninstall, and verify system deployment
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
	"strings"
)

// Directories to symlink from source to deploy dir.
var symlinkDirs = []string{
	"agents", "commands", "config", "data", "divisions", "docs",
	"hooks", "output-styles", "pkg", "plans", "rules", "skills",
	"statusline", "system", "workflow",
}

// Files to symlink from source to deploy dir.
var symlinkFiles = []string{
	"CLAUDE.md", "SYSTEM-REFERENCE.md", "config.toml", "instance.jsonc",
	"Makefile", "go.work", "settings.json",
}

// Stale directories from old setups that should be removed.
var staleDirs = []string{"cpi-si", "bin"}

// ============================================================================
// BODY
// ============================================================================

// cmdInstall handles system installation.
func cmdInstall(args []string, paths *Paths) error {
	// Parse install flags
	fresh := false
	binOnly := false
	symlinksOnly := false

	for _, arg := range args {
		switch arg {
		case "--fresh":
			fresh = true
		case "--bin":
			binOnly = true
		case "--symlinks":
			symlinksOnly = true
		default:
			return fmt.Errorf("unknown flag: %s\nUsage: cpisi install [--fresh|--bin|--symlinks]", arg)
		}
	}

	fmt.Println()
	fmt.Println("  CPI-SI System Installation")
	fmt.Println()
	info("Source:   %s", paths.SourceDir)
	info("Symlinks: %s", paths.DeployDir)
	info("Binaries: %s", paths.InstallDir)
	fmt.Println()

	if fresh {
		header("Cleaning previous installation")
		cmdClean(nil, paths)
		cleanStale(paths)
	}

	if binOnly {
		return installBinaries(paths)
	}

	if symlinksOnly {
		return installSymlinks(paths)
	}

	// Full install: build + symlinks + binaries

	header("[1/4] Building all modules")
	if err := cmdBuild(nil, paths); err != nil {
		return fmt.Errorf("build failed: %w", err)
	}

	header("[2/4] Creating symlinks")
	if err := installSymlinks(paths); err != nil {
		return fmt.Errorf("symlinks failed: %w", err)
	}

	header("[3/4] Installing binaries")
	if err := installBinaries(paths); err != nil {
		return fmt.Errorf("binary install failed: %w", err)
	}

	header("[4/4] Installing desktop entry")
	installDesktopEntry(paths)

	fmt.Println()
	success("Installation complete")
	info("Run 'cpisi verify' to check everything")
	return nil
}

// installSymlinks creates symlinks from source to deploy dir.
func installSymlinks(paths *Paths) error {
	header("Installing symlinks to " + paths.DeployDir)
	os.MkdirAll(paths.DeployDir, 0755)

	// Directory symlinks
	for _, dir := range symlinkDirs {
		srcPath := filepath.Join(paths.SourceDir, dir)
		dstPath := filepath.Join(paths.DeployDir, dir)

		if !fileExists(srcPath) {
			if isVerbose() {
				warn("%s not in source (skipping)", dir)
			}
			continue
		}

		// Remove existing symlink or back up existing directory
		if isSymlink(dstPath) {
			os.Remove(dstPath)
		} else if fileExists(dstPath) {
			warn("Backing up existing %s to %s.bak", dir, dir)
			os.Rename(dstPath, dstPath+".bak")
		}

		if err := os.Symlink(srcPath, dstPath); err != nil {
			fail("%s: %v", dir, err)
		} else {
			success("%s", dir)
		}
	}

	// File symlinks
	for _, file := range symlinkFiles {
		srcPath := filepath.Join(paths.SourceDir, file)
		dstPath := filepath.Join(paths.DeployDir, file)

		if !fileExists(srcPath) {
			if isVerbose() {
				warn("%s not in source (skipping)", file)
			}
			continue
		}

		if isSymlink(dstPath) {
			os.Remove(dstPath)
		} else if fileExists(dstPath) {
			warn("Backing up existing %s to %s.bak", file, file)
			os.Rename(dstPath, dstPath+".bak")
		}

		if err := os.Symlink(srcPath, dstPath); err != nil {
			fail("%s: %v", file, err)
		} else {
			success("%s", file)
		}
	}

	return nil
}

// installBinaries copies built binaries to the install dir with execute permissions.
func installBinaries(paths *Paths) error {
	header("Installing binaries to " + paths.InstallDir)

	// Create target directories
	os.MkdirAll(filepath.Join(paths.InstallDir, "bin"), 0755)
	os.MkdirAll(filepath.Join(paths.InstallDir, "hooks", "bin"), 0755)
	os.MkdirAll(filepath.Join(paths.InstallDir, "system", "bin"), 0755)

	// Main binaries
	mainBins := []string{"statusline", "cpisi", "dashboard", "dashboard-tui", "dashboard-gtk"}
	for _, name := range mainBins {
		src := filepath.Join(paths.SourceDir, "bin", name)
		dst := filepath.Join(paths.InstallDir, "bin", name)
		if fileExists(src) {
			if err := copyFile(src, dst, 0755); err != nil {
				fail("%s: %v", name, err)
			} else {
				success("%s", name)
			}
		}
	}

	// Hook binaries
	hookBinDir := filepath.Join(paths.SourceDir, "hooks", "bin")
	if entries, err := os.ReadDir(hookBinDir); err == nil {
		for _, entry := range entries {
			if entry.IsDir() {
				continue
			}
			src := filepath.Join(hookBinDir, entry.Name())
			dst := filepath.Join(paths.InstallDir, "hooks", "bin", entry.Name())
			if err := copyFile(src, dst, 0755); err != nil {
				fail("hook %s: %v", entry.Name(), err)
			} else {
				success("hook: %s", entry.Name())
			}
		}
	}

	// System runtime binaries
	systemBinDir := filepath.Join(paths.SourceDir, "system", "runtime", "bin")
	if entries, err := os.ReadDir(systemBinDir); err == nil {
		for _, entry := range entries {
			if entry.IsDir() {
				continue
			}
			src := filepath.Join(systemBinDir, entry.Name())
			dst := filepath.Join(paths.InstallDir, "system", "bin", entry.Name())
			if err := copyFile(src, dst, 0755); err != nil {
				if isVerbose() {
					warn("system %s: %v", entry.Name(), err)
				}
			}
		}
		success("system commands")
	}

	return nil
}

// installDesktopEntry installs the .desktop file for the GTK4 dashboard.
func installDesktopEntry(paths *Paths) {
	desktopIn := filepath.Join(paths.SourceDir, "dashboard", "gtk", "data",
		"com.creativeworkzstudio.cpisi.dashboard.desktop.in")

	if !fileExists(desktopIn) {
		if isVerbose() {
			info("No .desktop.in template found (skipping)")
		}
		return
	}

	// Read template and substitute
	data, err := os.ReadFile(desktopIn)
	if err != nil {
		warn("desktop entry: %v", err)
		return
	}

	content := string(data)
	content = strings.ReplaceAll(content, "@BINDIR@", filepath.Join(paths.InstallDir, "bin"))
	content = strings.ReplaceAll(content, "@ICONDIR@", filepath.Join(paths.SourceDir, "dashboard", "gtk", "data"))

	// Write to ~/.local/share/applications/
	appsDir := filepath.Join(paths.HomeDir, ".local", "share", "applications")
	os.MkdirAll(appsDir, 0755)

	dstPath := filepath.Join(appsDir, "com.creativeworkzstudio.cpisi.dashboard.desktop")
	if err := os.WriteFile(dstPath, []byte(content), 0644); err != nil {
		warn("desktop entry: %v", err)
	} else {
		success("desktop entry installed")
	}
}

// cmdUninstall removes installed components.
func cmdUninstall(args []string, paths *Paths) error {
	binOnly := false
	symlinksOnly := false

	for _, arg := range args {
		switch arg {
		case "--bin":
			binOnly = true
		case "--symlinks":
			symlinksOnly = true
		default:
			return fmt.Errorf("unknown flag: %s", arg)
		}
	}

	if binOnly {
		return uninstallBinaries(paths)
	}
	if symlinksOnly {
		return uninstallSymlinks(paths)
	}

	// Full uninstall
	uninstallSymlinks(paths)
	uninstallBinaries(paths)
	uninstallDesktopEntry(paths)
	success("Full uninstall complete")
	return nil
}

func uninstallSymlinks(paths *Paths) error {
	header("Removing symlinks from " + paths.DeployDir)
	for _, dir := range symlinkDirs {
		dstPath := filepath.Join(paths.DeployDir, dir)
		if isSymlink(dstPath) {
			os.Remove(dstPath)
			success("Removed %s", dir)
		}
	}
	for _, file := range symlinkFiles {
		dstPath := filepath.Join(paths.DeployDir, file)
		if isSymlink(dstPath) {
			os.Remove(dstPath)
			success("Removed %s", file)
		}
	}
	return nil
}

func uninstallBinaries(paths *Paths) error {
	header("Removing binaries from " + paths.InstallDir)
	os.RemoveAll(filepath.Join(paths.InstallDir, "bin"))
	os.RemoveAll(filepath.Join(paths.InstallDir, "hooks", "bin"))
	os.RemoveAll(filepath.Join(paths.InstallDir, "system", "bin"))
	success("Binaries removed")
	return nil
}

func uninstallDesktopEntry(paths *Paths) {
	dstPath := filepath.Join(paths.HomeDir, ".local", "share", "applications",
		"com.creativeworkzstudio.cpisi.dashboard.desktop")
	if fileExists(dstPath) {
		os.Remove(dstPath)
		success("Desktop entry removed")
	}
}

// cleanStale removes known stale directories from old setups.
func cleanStale(paths *Paths) {
	for _, dir := range staleDirs {
		dstPath := filepath.Join(paths.DeployDir, dir)
		if isSymlink(dstPath) {
			os.Remove(dstPath)
			success("Removed stale symlink: %s", dir)
		} else if fileExists(dstPath) {
			os.RemoveAll(dstPath)
			success("Removed stale directory: %s", dir)
		}
	}
}

// cmdVerify checks installation integrity.
func cmdVerify(args []string, paths *Paths) error {
	header("Verifying CPI-SI Installation")
	fmt.Println()
	info("Source:   %s", paths.SourceDir)
	info("Symlinks: %s", paths.DeployDir)
	info("Binaries: %s", paths.InstallDir)

	errors := 0

	// Check source directories
	fmt.Println()
	fmt.Println("  Source directories:")
	for _, dir := range symlinkDirs {
		srcPath := filepath.Join(paths.SourceDir, dir)
		if fileExists(srcPath) {
			success("%s", dir)
		} else {
			fail("%s MISSING", dir)
			errors++
		}
	}

	// Check source files
	fmt.Println()
	fmt.Println("  Source files:")
	for _, file := range symlinkFiles {
		srcPath := filepath.Join(paths.SourceDir, file)
		if fileExists(srcPath) {
			success("%s", file)
		} else {
			fail("%s MISSING", file)
			errors++
		}
	}

	// Check symlinks
	fmt.Println()
	fmt.Println("  Symlinks:")
	for _, dir := range symlinkDirs {
		dstPath := filepath.Join(paths.DeployDir, dir)
		checkSymlink(dstPath, filepath.Join(paths.SourceDir, dir), dir, &errors)
	}
	for _, file := range symlinkFiles {
		dstPath := filepath.Join(paths.DeployDir, file)
		checkSymlink(dstPath, filepath.Join(paths.SourceDir, file), file, &errors)
	}

	// Check installed binaries
	fmt.Println()
	fmt.Println("  Main binaries:")
	mainBins := []string{"statusline", "cpisi", "dashboard", "dashboard-tui", "dashboard-gtk"}
	for _, name := range mainBins {
		binPath := filepath.Join(paths.InstallDir, "bin", name)
		if isExecutable(binPath) {
			success("%s", name)
		} else if fileExists(binPath) {
			fail("%s exists but NOT executable", name)
			errors++
		} else {
			info("- %s not installed", name)
		}
	}

	// Check hook binaries
	fmt.Println()
	fmt.Println("  Hook binaries:")
	hookNames := []string{
		"start", "end", "stop", "submit", "pre-use", "post-use",
		"pre-compact", "setup", "request", "notification",
		"permission-request", "hooks", "subagent-start", "subagent-stop",
		"task-completed", "teammate-idle",
	}
	for _, name := range hookNames {
		binPath := filepath.Join(paths.InstallDir, "hooks", "bin", name)
		if isExecutable(binPath) {
			success("%s", name)
		} else if fileExists(binPath) {
			fail("%s NOT executable", name)
			errors++
		} else {
			info("- %s not installed", name)
		}
	}

	// Check for stale directories
	fmt.Println()
	fmt.Println("  Stale check:")
	for _, dir := range staleDirs {
		dstPath := filepath.Join(paths.DeployDir, dir)
		if fileExists(dstPath) || isSymlink(dstPath) {
			warn("%s is stale (run: cpisi install --fresh)", dir)
			errors++
		} else {
			success("%s not present (good)", dir)
		}
	}

	// Check go.work
	fmt.Println()
	fmt.Println("  Go workspace:")
	goWork := filepath.Join(paths.SourceDir, "go.work")
	if fileExists(goWork) {
		success("go.work exists")
		if _, err := runCmdSilent(paths.SourceDir, "go", "work", "sync"); err == nil {
			success("go.work syncs successfully")
		} else {
			fail("go.work sync failed")
			errors++
		}
	} else {
		fail("go.work missing")
		errors++
	}

	// Desktop entry
	fmt.Println()
	fmt.Println("  Desktop entry:")
	desktopPath := filepath.Join(paths.HomeDir, ".local", "share", "applications",
		"com.creativeworkzstudio.cpisi.dashboard.desktop")
	if fileExists(desktopPath) {
		success("GTK4 dashboard desktop entry installed")
	} else {
		info("- GTK4 desktop entry not installed")
	}

	// Summary
	fmt.Println()
	if errors == 0 {
		success("Verification PASSED")
	} else {
		fail("Verification FAILED: %d error(s)", errors)
		info("Run 'cpisi install --fresh' to fix")
		return fmt.Errorf("%d verification error(s)", errors)
	}
	return nil
}

// checkSymlink verifies a symlink points to the expected target.
func checkSymlink(path, expectedTarget, name string, errors *int) {
	if isSymlink(path) {
		target, err := filepath.EvalSymlinks(path)
		if err == nil && target == expectedTarget {
			success("%s", name)
		} else {
			warn("%s points to wrong target", name)
			(*errors)++
		}
	} else if fileExists(path) {
		fail("%s is not a symlink", name)
		(*errors)++
	} else {
		info("- %s not installed", name)
	}
}

// ============================================================================
// CLOSING
// ============================================================================
