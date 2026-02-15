//omni:code --go -test
//omni:key B-L2-builder-test
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-builder-test
// Purpose: Unit tests for builder path resolution
// Biblical: Proverbs 4:26 — "Ponder the path of thy feet"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-15
//
// ═══════════════════════════════════════════════════════════════════════════

package builder

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"os"
	"path/filepath"
	"strings"
	"testing"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// ResolvePaths — error cases (valid paths need a real project dir)
// ───────────────────────────────────────────────────────────────────────────

func TestResolvePathsMissingConfig(t *testing.T) {
	_, err := ResolvePaths("/tmp/nonexistent-project-dir-12345")
	if err == nil {
		t.Fatal("expected error for missing config, got nil")
	}
	if !strings.Contains(err.Error(), "config not found") {
		t.Errorf("unexpected error: %v", err)
	}
}

func TestResolvePathsPopulatesAllFields(t *testing.T) {
	// Create a minimal project structure in /tmp.
	dir := t.TempDir()
	configDir := filepath.Join(dir, "build")
	configPath := filepath.Join(configDir, "build.config.yaml")

	if err := mkdirAll(configDir); err != nil {
		t.Fatal(err)
	}
	if err := writeFile(configPath, "# test config"); err != nil {
		t.Fatal(err)
	}

	p, err := ResolvePaths(dir)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	if p.ProjectDir == "" {
		t.Error("ProjectDir should not be empty")
	}
	if p.ConfigPath == "" {
		t.Error("ConfigPath should not be empty")
	}
	if p.BuilderCLI == "" {
		t.Error("BuilderCLI should not be empty")
	}
	if p.WebRoot == "" {
		t.Error("WebRoot should not be empty")
	}

	// Verify paths are relative to project dir.
	if !strings.HasPrefix(p.ConfigPath, p.ProjectDir) {
		t.Errorf("ConfigPath %q should be under ProjectDir %q", p.ConfigPath, p.ProjectDir)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// FindProjectDir — upward-walk algorithm
// ───────────────────────────────────────────────────────────────────────────

func TestFindProjectDirSuccess(t *testing.T) {
	// Create nested structure: root/build/build.config.yaml, start from root/sub/deep.
	root := t.TempDir()
	configDir := filepath.Join(root, "build")
	deepDir := filepath.Join(root, "sub", "deep")

	if err := mkdirAll(configDir); err != nil {
		t.Fatal(err)
	}
	if err := mkdirAll(deepDir); err != nil {
		t.Fatal(err)
	}
	if err := writeFile(filepath.Join(configDir, "build.config.yaml"), "# test"); err != nil {
		t.Fatal(err)
	}

	found, err := FindProjectDir(deepDir)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if found != root {
		t.Errorf("got %q, want %q", found, root)
	}
}

func TestFindProjectDirNotFound(t *testing.T) {
	_, err := FindProjectDir("/tmp")
	if err == nil {
		t.Fatal("expected error for /tmp, got nil")
	}
	if !strings.Contains(err.Error(), "could not find") {
		t.Errorf("unexpected error: %v", err)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Helpers — filesystem utilities for tests
// ───────────────────────────────────────────────────────────────────────────

func mkdirAll(path string) error {
	return os.MkdirAll(path, 0755)
}

func writeFile(path, content string) error {
	return os.WriteFile(path, []byte(content), 0644)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
