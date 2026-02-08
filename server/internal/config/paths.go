// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-config-paths
// Purpose: Multi-project path resolution for CWS server services
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package config

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"os"
	"path/filepath"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// BuilderPaths holds resolved filesystem paths for the builder dashboard service.
type BuilderPaths struct {
	ProjectDir string // company-docs/ root
	ConfigPath string // company-docs/build/build.config.yaml
	BuilderCLI string // company-docs/build/builder/dist/cli.js
	WebRoot    string // company-docs/build/dashboard/src-web/
}

// ResolveBuildPaths resolves all builder-related paths from a project directory.
func ResolveBuilderPaths(projectDir string) (*BuilderPaths, error) {
	abs, err := filepath.Abs(projectDir)
	if err != nil {
		return nil, fmt.Errorf("resolve project dir: %w", err)
	}

	configPath := filepath.Join(abs, "build", "build.config.yaml")
	if _, err := os.Stat(configPath); err != nil {
		return nil, fmt.Errorf("config not found at %s: %w", configPath, err)
	}

	return &BuilderPaths{
		ProjectDir: abs,
		ConfigPath: configPath,
		BuilderCLI: filepath.Join(abs, "build", "builder", "dist", "cli.js"),
		WebRoot:    filepath.Join(abs, "build", "dashboard", "src-web"),
	}, nil
}

// FindProjectDir walks up from startDir looking for build/build.config.yaml.
func FindProjectDir(startDir string) (string, error) {
	dir, err := filepath.Abs(startDir)
	if err != nil {
		return "", err
	}

	for {
		configPath := filepath.Join(dir, "build", "build.config.yaml")
		if _, err := os.Stat(configPath); err == nil {
			return dir, nil
		}

		parent := filepath.Dir(dir)
		if parent == dir {
			return "", fmt.Errorf("could not find build/build.config.yaml walking up from %s", startDir)
		}
		dir = parent
	}
}

// PreferencesDir returns the XDG-compliant preferences directory.
func PreferencesDir() string {
	xdg := os.Getenv("XDG_CONFIG_HOME")
	if xdg == "" {
		home, _ := os.UserHomeDir()
		xdg = filepath.Join(home, ".config")
	}
	return filepath.Join(xdg, "cws-dashboard")
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
