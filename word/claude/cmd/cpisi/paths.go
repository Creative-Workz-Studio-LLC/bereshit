// ============================================================================
// METADATA - CPI-SI CLI Path Resolution
// ============================================================================
//
// Purpose: Path resolution, module definitions, config discovery
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

// Paths holds all resolved system paths.
type Paths struct {
	SourceDir  string // Canonical source root (has go.work)
	DeployDir  string // Symlink target (~/.claude/)
	InstallDir string // Binary target (~/.local/share/claude/)
	HomeDir    string // User home directory
}

// Module describes a Go module in the workspace.
type Module struct {
	Name        string // Short name for CLI (e.g. "pkg", "hooks")
	Path        string // Relative path from source root
	Description string // Human-readable description
	HasBinary   bool   // Produces an installable binary
	BinaryName  string // Output binary name (if HasBinary)
	BinaryCmd   string // Relative path to cmd/ main package
	HasMakefile bool   // Has its own Makefile for building
}

// AllModules defines every module in the workspace.
var AllModules = []Module{
	{
		Name:        "pkg",
		Path:        "pkg",
		Description: "Shared Go packages (cognition, statemachine, dashboard, types)",
	},
	{
		Name:        "hooks",
		Path:        "hooks",
		Description: "Session and tool lifecycle hooks",
		HasBinary:   true,
		HasMakefile: true,
	},
	{
		Name:        "statusline",
		Path:        "statusline",
		Description: "Status line binary (WezTerm OSC integration)",
		HasBinary:   true,
		BinaryName:  "statusline",
		BinaryCmd:   "cmd/statusline",
	},
	{
		Name:        "system",
		Path:        "system",
		Description: "System runtime libraries and commands",
		HasBinary:   true,
	},
	{
		Name:        "skills",
		Path:        "skills",
		Description: "Skill Go script implementations",
	},
	{
		Name:        "dashboard",
		Path:        "dashboard",
		Description: "Web dashboard (Go server + TypeScript frontend)",
		HasBinary:   true,
		BinaryName:  "dashboard",
		BinaryCmd:   "cmd/dashboard",
	},
	{
		Name:        "dashboard-tui",
		Path:        "dashboard/tui",
		Description: "Terminal dashboard (Bubble Tea)",
		HasBinary:   true,
		BinaryName:  "dashboard-tui",
		BinaryCmd:   "cmd/dashboard-tui",
	},
	{
		Name:        "dashboard-gtk",
		Path:        "dashboard/gtk",
		Description: "Native desktop dashboard (GTK4)",
		HasBinary:   true,
		BinaryName:  "dashboard-gtk",
		BinaryCmd:   "cmd/dashboard-gtk",
	},
}

// ResolvePaths discovers and validates system paths.
func ResolvePaths(sourceOverride, installOverride, deployOverride string) (*Paths, error) {
	home, err := os.UserHomeDir()
	if err != nil {
		return nil, fmt.Errorf("cannot determine home directory: %w", err)
	}

	p := &Paths{
		HomeDir:    home,
		DeployDir:  filepath.Join(home, ".claude"),
		InstallDir: filepath.Join(home, ".local", "share", "claude"),
	}

	// Apply overrides
	if deployOverride != "" {
		p.DeployDir = deployOverride
	}
	if installOverride != "" {
		p.InstallDir = installOverride
	}

	// Resolve source directory
	if sourceOverride != "" {
		p.SourceDir = sourceOverride
	} else {
		p.SourceDir, err = detectSourceDir(home)
		if err != nil {
			return nil, err
		}
	}

	return p, nil
}

// detectSourceDir finds the source root by multiple strategies.
func detectSourceDir(home string) (string, error) {
	// Strategy 1: Check if CWD contains go.work
	if cwd, err := os.Getwd(); err == nil {
		if _, err := os.Stat(filepath.Join(cwd, "go.work")); err == nil {
			return cwd, nil
		}
	}

	// Strategy 2: CPISI_SOURCE environment variable
	if dir := os.Getenv("CPISI_SOURCE"); dir != "" {
		if _, err := os.Stat(filepath.Join(dir, "go.work")); err == nil {
			return dir, nil
		}
		return "", fmt.Errorf("CPISI_SOURCE=%s does not contain go.work", dir)
	}

	// Strategy 3: Follow ~/.claude/CLAUDE.md symlink to find source
	deployDir := filepath.Join(home, ".claude")
	claudeMD := filepath.Join(deployDir, "CLAUDE.md")
	if target, err := os.Readlink(claudeMD); err == nil {
		dir := filepath.Dir(target)
		if _, err := os.Stat(filepath.Join(dir, "go.work")); err == nil {
			return dir, nil
		}
	}

	// Strategy 4: Follow ~/.claude/pkg symlink
	pkgLink := filepath.Join(deployDir, "pkg")
	if target, err := os.Readlink(pkgLink); err == nil {
		dir := filepath.Dir(target)
		if _, err := os.Stat(filepath.Join(dir, "go.work")); err == nil {
			return dir, nil
		}
	}

	return "", fmt.Errorf("cannot find CPI-SI source directory\n\n" +
		"Try one of:\n" +
		"  1. Run cpisi from the source directory (contains go.work)\n" +
		"  2. Set CPISI_SOURCE environment variable\n" +
		"  3. Install symlinks with: make install-symlinks")
}

// FindModule looks up a module by name.
func FindModule(name string) *Module {
	for i := range AllModules {
		if AllModules[i].Name == name {
			return &AllModules[i]
		}
	}
	return nil
}

// ModuleNames returns all module names as a comma-separated string.
func ModuleNames() string {
	names := make([]string, len(AllModules))
	for i, m := range AllModules {
		names[i] = m.Name
	}
	return strings.Join(names, ", ")
}

// BinaryModules returns only modules that produce binaries.
func BinaryModules() []Module {
	var result []Module
	for _, m := range AllModules {
		if m.HasBinary {
			result = append(result, m)
		}
	}
	return result
}

// ============================================================================
// CLOSING
// ============================================================================
