// ============================================================================
// METADATA - CPI-SI CLI Build Commands
// ============================================================================
//
// Purpose: Build, test, clean, format, vet, tidy operations
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

// ============================================================================
// BODY
// ============================================================================

// cmdBuild builds specified modules (or all).
func cmdBuild(args []string, paths *Paths) error {
	modules := resolveModules(args)

	header("Building CPI-SI modules")
	info("Source: %s", paths.SourceDir)
	fmt.Println()

	var errors []string

	for _, mod := range modules {
		fmt.Printf("  Building %s...\n", mod.Name)

		switch mod.Name {
		case "hooks":
			// Hooks have their own Makefile
			if err := runCmd(filepath.Join(paths.SourceDir, "hooks"), "make", "build"); err != nil {
				errors = append(errors, mod.Name)
				fail("%s: %v", mod.Name, err)
			} else {
				success("%s", mod.Name)
			}

		case "system":
			// System has split cmd + lib
			cmdDir := filepath.Join(paths.SourceDir, "system", "runtime", "cmd")
			libDir := filepath.Join(paths.SourceDir, "system", "runtime", "lib")
			failed := false
			if err := runGo(cmdDir, "build", "./..."); err != nil {
				failed = true
			}
			if err := runGo(libDir, "build", "./..."); err != nil {
				failed = true
			}
			if failed {
				errors = append(errors, mod.Name)
				fail("%s", mod.Name)
			} else {
				success("%s", mod.Name)
			}

		case "skills":
			// Skills are pattern-based: skills/*/scripts/
			if err := buildSkills(paths.SourceDir); err != nil {
				errors = append(errors, mod.Name)
				fail("%s: %v", mod.Name, err)
			} else {
				success("%s", mod.Name)
			}

		case "dashboard":
			// Dashboard needs frontend built first
			if err := buildDashboardFrontend(paths.SourceDir); err != nil {
				warn("frontend build failed: %v (continuing with Go only)", err)
			}
			modDir := filepath.Join(paths.SourceDir, mod.Path)
			if err := runGo(modDir, "build", "./..."); err != nil {
				errors = append(errors, mod.Name)
				fail("%s", mod.Name)
			} else {
				success("%s", mod.Name)
			}

		default:
			modDir := filepath.Join(paths.SourceDir, mod.Path)
			if err := runGo(modDir, "build", "./..."); err != nil {
				errors = append(errors, mod.Name)
				fail("%s: %v", mod.Name, err)
			} else {
				success("%s", mod.Name)
			}
		}
	}

	fmt.Println()
	if len(errors) > 0 {
		return fmt.Errorf("build failed for: %s", strings.Join(errors, ", "))
	}

	// Build binaries
	header("Building binaries")
	if err := buildBinaries(paths); err != nil {
		return err
	}

	fmt.Println()
	success("All modules built successfully")
	info("Run 'cpisi install --bin' to deploy binaries to %s", paths.InstallDir)
	return nil
}

// buildBinaries builds all binary outputs to bin/.
func buildBinaries(paths *Paths) error {
	binDir := filepath.Join(paths.SourceDir, "bin")
	os.MkdirAll(binDir, 0755)

	ldflags := "-s -w"
	var errors []string

	// Statusline
	statuslineDir := filepath.Join(paths.SourceDir, "statusline")
	out := filepath.Join(binDir, "statusline")
	if err := runGo(statuslineDir, "build", "-ldflags="+ldflags, "-o", out, "./cmd/statusline"); err != nil {
		errors = append(errors, "statusline")
		fail("statusline")
	} else {
		success("bin/statusline")
	}

	// cpisi (this CLI)
	cpisiDir := filepath.Join(paths.SourceDir, "cmd", "cpisi")
	out = filepath.Join(binDir, "cpisi")
	buildLdflags := fmt.Sprintf("-s -w -X main.Version=%s -X main.BuildTime=%s",
		Version, buildTimeNow())
	if err := runGo(cpisiDir, "build", "-ldflags="+buildLdflags, "-o", out, "."); err != nil {
		errors = append(errors, "cpisi")
		fail("cpisi")
	} else {
		success("bin/cpisi")
	}

	// Dashboard (web)
	dashDir := filepath.Join(paths.SourceDir, "dashboard")
	out = filepath.Join(binDir, "dashboard")
	if err := runGo(dashDir, "build", "-ldflags="+ldflags, "-o", out, "./cmd/dashboard"); err != nil {
		errors = append(errors, "dashboard")
		fail("dashboard")
	} else {
		success("bin/dashboard")
	}

	// Dashboard TUI
	tuiDir := filepath.Join(paths.SourceDir, "dashboard", "tui")
	out = filepath.Join(binDir, "dashboard-tui")
	if err := runGo(tuiDir, "build", "-ldflags="+ldflags, "-o", out, "./cmd/dashboard-tui"); err != nil {
		errors = append(errors, "dashboard-tui")
		fail("dashboard-tui")
	} else {
		success("bin/dashboard-tui")
	}

	// Dashboard GTK
	gtkDir := filepath.Join(paths.SourceDir, "dashboard", "gtk")
	out = filepath.Join(binDir, "dashboard-gtk")
	if err := runGo(gtkDir, "build", "-ldflags="+ldflags, "-o", out, "./cmd/dashboard-gtk"); err != nil {
		errors = append(errors, "dashboard-gtk")
		fail("dashboard-gtk")
	} else {
		success("bin/dashboard-gtk")
	}

	// Hooks (via their own Makefile)
	hooksDir := filepath.Join(paths.SourceDir, "hooks")
	if fileExists(filepath.Join(hooksDir, "Makefile")) {
		if err := runCmd(hooksDir, "make", "build"); err != nil {
			errors = append(errors, "hooks")
			fail("hook binaries")
		} else {
			success("hooks/bin/*")
		}
	}

	// System runtime commands
	systemCmdDir := filepath.Join(paths.SourceDir, "system", "runtime", "cmd")
	systemBinDir := filepath.Join(paths.SourceDir, "system", "runtime", "bin")
	os.MkdirAll(systemBinDir, 0755)
	entries, _ := os.ReadDir(systemCmdDir)
	for _, entry := range entries {
		if !entry.IsDir() {
			continue
		}
		name := entry.Name()
		cmdPath := filepath.Join(systemCmdDir, name)
		// Check if it has a Go source file
		if !hasGoSource(cmdPath) {
			continue
		}
		out := filepath.Join(systemBinDir, name)
		if err := runGo(cmdPath, "build", "-ldflags="+ldflags, "-o", out, "."); err != nil {
			if isVerbose() {
				warn("system cmd %s: %v", name, err)
			}
		}
	}
	success("system/runtime/bin/*")

	if len(errors) > 0 {
		return fmt.Errorf("binary build failed for: %s", strings.Join(errors, ", "))
	}
	return nil
}

// buildDashboardFrontend builds the TypeScript frontend on ext4 (/tmp).
func buildDashboardFrontend(sourceDir string) error {
	frontendDir := filepath.Join(sourceDir, "dashboard", "frontend")
	if !fileExists(filepath.Join(frontendDir, "package.json")) {
		return fmt.Errorf("frontend not found at %s", frontendDir)
	}

	info("Building dashboard frontend (TypeScript)...")

	// exFAT doesn't support symlinks, so npm install needs ext4
	tempBuild := "/tmp/cpisi-dashboard-build"
	os.RemoveAll(tempBuild)

	// Copy frontend source to temp
	if err := runCmd(".", "cp", "-r", frontendDir, tempBuild); err != nil {
		return fmt.Errorf("copy to temp: %w", err)
	}
	defer os.RemoveAll(tempBuild)

	// npm install and build
	if err := runCmd(tempBuild, "npm", "install", "--silent"); err != nil {
		return fmt.Errorf("npm install: %w", err)
	}
	if err := runCmd(tempBuild, "npx", "tsc", "--noEmit"); err != nil {
		return fmt.Errorf("typecheck: %w", err)
	}
	if err := runCmd(tempBuild, "npx", "vite", "build"); err != nil {
		return fmt.Errorf("vite build: %w", err)
	}

	// Copy output back
	staticDir := filepath.Join(sourceDir, "dashboard", "server", "static")
	os.RemoveAll(staticDir)
	if err := runCmd(".", "cp", "-r", "/tmp/server/static", staticDir); err != nil {
		return fmt.Errorf("copy static: %w", err)
	}
	os.RemoveAll("/tmp/server")

	success("dashboard frontend")
	return nil
}

// buildSkills builds all skill script modules.
func buildSkills(sourceDir string) error {
	skillsDir := filepath.Join(sourceDir, "skills")
	entries, err := os.ReadDir(skillsDir)
	if err != nil {
		return err
	}

	for _, entry := range entries {
		if !entry.IsDir() {
			continue
		}
		scriptsDir := filepath.Join(skillsDir, entry.Name(), "scripts")
		if !fileExists(filepath.Join(scriptsDir, "go.mod")) {
			continue
		}
		if err := runGo(scriptsDir, "build", "./..."); err != nil {
			return fmt.Errorf("skill %s: %w", entry.Name(), err)
		}
	}
	return nil
}

// cmdTest runs tests for specified modules (or all).
func cmdTest(args []string, paths *Paths) error {
	modules := resolveModules(args)

	header("Testing CPI-SI modules")
	var errors []string

	for _, mod := range modules {
		fmt.Printf("  Testing %s...\n", mod.Name)

		switch mod.Name {
		case "system":
			cmdDir := filepath.Join(paths.SourceDir, "system", "runtime", "cmd")
			libDir := filepath.Join(paths.SourceDir, "system", "runtime", "lib")
			runGo(cmdDir, "test", "./...", "-v")
			runGo(libDir, "test", "./...", "-v")
			success("%s", mod.Name)

		case "skills":
			info("(skills have no tests)")

		case "hooks":
			modDir := filepath.Join(paths.SourceDir, mod.Path)
			if err := runGo(modDir, "test", "./...", "-v"); err != nil {
				errors = append(errors, mod.Name)
			} else {
				success("%s", mod.Name)
			}

		default:
			modDir := filepath.Join(paths.SourceDir, mod.Path)
			if err := runGo(modDir, "test", "./...", "-v"); err != nil {
				errors = append(errors, mod.Name)
			} else {
				success("%s", mod.Name)
			}
		}
	}

	fmt.Println()
	if len(errors) > 0 {
		return fmt.Errorf("tests failed for: %s", strings.Join(errors, ", "))
	}
	success("All tests passed")
	return nil
}

// cmdClean removes build artifacts.
func cmdClean(args []string, paths *Paths) error {
	header("Cleaning build artifacts")

	// Remove bin/
	binDir := filepath.Join(paths.SourceDir, "bin")
	os.RemoveAll(binDir)
	success("Removed bin/")

	// Remove dashboard/server/static
	staticDir := filepath.Join(paths.SourceDir, "dashboard", "server", "static")
	os.RemoveAll(staticDir)
	success("Removed dashboard/server/static/")

	// Go clean each module
	for _, mod := range AllModules {
		switch mod.Name {
		case "system":
			runGo(filepath.Join(paths.SourceDir, "system", "runtime", "cmd"), "clean", "./...")
			runGo(filepath.Join(paths.SourceDir, "system", "runtime", "lib"), "clean", "./...")
		case "skills":
			// Skip — handled individually
		default:
			modDir := filepath.Join(paths.SourceDir, mod.Path)
			runGo(modDir, "clean", "./...")
		}
	}

	success("Clean complete")
	return nil
}

// cmdFmt formats all Go code.
func cmdFmt(args []string, paths *Paths) error {
	header("Formatting Go code")
	for _, mod := range AllModules {
		fmtModule(paths.SourceDir, mod)
	}
	success("Format complete")
	return nil
}

// cmdVet runs go vet on all modules.
func cmdVet(args []string, paths *Paths) error {
	header("Running go vet")
	var errors []string
	for _, mod := range AllModules {
		if err := vetModule(paths.SourceDir, mod); err != nil {
			errors = append(errors, mod.Name)
		}
	}
	if len(errors) > 0 {
		return fmt.Errorf("vet failed for: %s", strings.Join(errors, ", "))
	}
	success("Vet passed")
	return nil
}

// cmdTidy tidies all go.mod files.
func cmdTidy(args []string, paths *Paths) error {
	header("Tidying go.mod files")
	for _, mod := range AllModules {
		tidyModule(paths.SourceDir, mod)
	}
	// Also tidy skill modules
	skillsDir := filepath.Join(paths.SourceDir, "skills")
	entries, _ := os.ReadDir(skillsDir)
	for _, entry := range entries {
		if !entry.IsDir() {
			continue
		}
		scriptsDir := filepath.Join(skillsDir, entry.Name(), "scripts")
		if fileExists(filepath.Join(scriptsDir, "go.mod")) {
			runGo(scriptsDir, "mod", "tidy")
		}
	}
	success("Tidy complete")
	return nil
}

// Module operation helpers.

func fmtModule(sourceDir string, mod Module) {
	switch mod.Name {
	case "system":
		runGo(filepath.Join(sourceDir, "system", "runtime", "cmd"), "fmt", "./...")
		runGo(filepath.Join(sourceDir, "system", "runtime", "lib"), "fmt", "./...")
	case "skills":
		// Skip
	default:
		runGo(filepath.Join(sourceDir, mod.Path), "fmt", "./...")
	}
}

func vetModule(sourceDir string, mod Module) error {
	switch mod.Name {
	case "system":
		if err := runGo(filepath.Join(sourceDir, "system", "runtime", "cmd"), "vet", "./..."); err != nil {
			return err
		}
		return runGo(filepath.Join(sourceDir, "system", "runtime", "lib"), "vet", "./...")
	case "skills":
		return nil
	default:
		return runGo(filepath.Join(sourceDir, mod.Path), "vet", "./...")
	}
}

func tidyModule(sourceDir string, mod Module) {
	switch mod.Name {
	case "system":
		runGo(filepath.Join(sourceDir, "system", "runtime", "cmd"), "mod", "tidy")
		runGo(filepath.Join(sourceDir, "system", "runtime", "lib"), "mod", "tidy")
	case "skills":
		// Handled separately
	default:
		runGo(filepath.Join(sourceDir, mod.Path), "mod", "tidy")
	}
}

// resolveModules maps CLI args to Module structs, or returns all modules.
func resolveModules(args []string) []Module {
	if len(args) == 0 {
		return AllModules
	}
	var result []Module
	for _, name := range args {
		mod := FindModule(name)
		if mod == nil {
			fmt.Fprintf(os.Stderr, "cpisi: unknown module: %s\n", name)
			fmt.Fprintf(os.Stderr, "Available modules: %s\n", ModuleNames())
			os.Exit(1)
		}
		result = append(result, *mod)
	}
	return result
}

// hasGoSource checks if a directory contains .go files.
func hasGoSource(dir string) bool {
	entries, err := os.ReadDir(dir)
	if err != nil {
		return false
	}
	for _, e := range entries {
		if !e.IsDir() && strings.HasSuffix(e.Name(), ".go") {
			return true
		}
	}
	return false
}

// buildTimeNow returns the current time as a build timestamp string.
func buildTimeNow() string {
	// Use go's time formatting if we need it; for ldflags we keep it simple
	return "now"
}

// ============================================================================
// CLOSING
// ============================================================================
