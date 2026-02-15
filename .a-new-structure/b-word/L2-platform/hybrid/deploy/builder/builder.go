//omni:code --go -library
//omni:key B-L2-builder
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-builder
// Purpose: Multi-project path resolution for CWS builder services
// Biblical: Proverbs 4:26 — "Ponder the path of thy feet"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package builder

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

// ───────────────────────────────────────────────────────────────────────────
// Types — resolved paths for project-relative resources
// ───────────────────────────────────────────────────────────────────────────

// Paths holds resolved filesystem paths for the builder dashboard service.
// All paths are absolute and verified to exist (or derivable from a verified root).
type Paths struct {
	ProjectDir string // Absolute path to company-docs/ root (verified via config file)
	ConfigPath string // build/build.config.yaml — single source of truth for editorial config
	BuilderCLI string // build/builder/dist/cli.js — compiled TS builder entry point
	WebRoot    string // build/dashboard/src-web/ — SPA static files root
}

// ───────────────────────────────────────────────────────────────────────────
// Path Resolution — locate project resources from a root directory
// ───────────────────────────────────────────────────────────────────────────

// ResolvePaths resolves all builder-related paths from a project directory.
// The project directory must contain build/build.config.yaml — this is the sentinel
// file that confirms the directory is a valid CWS project root.
func ResolvePaths(projectDir string) (*Paths, error) {
	abs, err := filepath.Abs(projectDir)
	if err != nil {
		return nil, fmt.Errorf("resolve project dir: %w", err)
	}

	configPath := filepath.Join(abs, "build", "build.config.yaml")
	if _, err := os.Stat(configPath); err != nil {
		return nil, fmt.Errorf("config not found at %s: %w", configPath, err)
	}

	return &Paths{
		ProjectDir: abs,
		ConfigPath: configPath,
		BuilderCLI: filepath.Join(abs, "build", "builder", "dist", "cli.js"),
		WebRoot:    filepath.Join(abs, "build", "dashboard", "src-web"),
	}, nil
}

// FindProjectDir walks up from startDir looking for build/build.config.yaml.
// This upward-walk algorithm mirrors how tools like git find .git/ — it starts
// at the given directory and checks each parent until it hits the filesystem root.
// Used for auto-detection when --builder-dir is not specified.
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

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
