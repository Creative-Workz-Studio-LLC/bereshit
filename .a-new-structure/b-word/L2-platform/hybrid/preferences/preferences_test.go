//omni:code --go -test
//omni:key B-L2-preferences-test
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-preferences-test
// Purpose: Unit tests for XDG-compliant preferences read/write
// Biblical: Psalm 119:30 — "I have chosen the way of truth"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-15
//
// ═══════════════════════════════════════════════════════════════════════════

package preferences

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"os"
	"path/filepath"
	"testing"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Dir — verify XDG compliance
// ───────────────────────────────────────────────────────────────────────────

func TestDirRespectsXDGConfigHome(t *testing.T) {
	dir := t.TempDir()
	t.Setenv("XDG_CONFIG_HOME", dir)

	got := Dir()
	want := filepath.Join(dir, "cws-dashboard")
	if got != want {
		t.Errorf("Dir() = %q, want %q", got, want)
	}
}

func TestDirFallsBackToHome(t *testing.T) {
	t.Setenv("XDG_CONFIG_HOME", "")

	got := Dir()
	// Should end with .config/cws-dashboard.
	if filepath.Base(got) != "cws-dashboard" {
		t.Errorf("Dir() = %q, should end with cws-dashboard", got)
	}
	if filepath.Base(filepath.Dir(got)) != ".config" {
		t.Errorf("Dir() = %q, parent should be .config", got)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Read — missing file returns empty map, not error
// ───────────────────────────────────────────────────────────────────────────

func TestReadMissingFileReturnsEmptyMap(t *testing.T) {
	dir := t.TempDir()
	t.Setenv("XDG_CONFIG_HOME", dir)

	prefs, err := Read()
	if err != nil {
		t.Fatalf("Read: unexpected error: %v", err)
	}
	if prefs == nil {
		t.Fatal("Read should return non-nil map")
	}
	if len(prefs) != 0 {
		t.Errorf("Read should return empty map, got %d entries", len(prefs))
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Write + Read — round-trip preservation
// ───────────────────────────────────────────────────────────────────────────

func TestWriteAndReadRoundTrip(t *testing.T) {
	dir := t.TempDir()
	t.Setenv("XDG_CONFIG_HOME", dir)

	original := map[string]any{
		"theme":    "dark",
		"fontSize": float64(14),
	}

	if err := Write(original); err != nil {
		t.Fatalf("Write: %v", err)
	}

	// Verify file exists on disk.
	prefsPath := filepath.Join(dir, "cws-dashboard", prefsFilename)
	if _, err := os.Stat(prefsPath); err != nil {
		t.Fatalf("preferences file not created: %v", err)
	}

	// Read back.
	got, err := Read()
	if err != nil {
		t.Fatalf("Read: %v", err)
	}

	if got["theme"] != "dark" {
		t.Errorf("theme = %v, want %q", got["theme"], "dark")
	}
	if got["fontSize"] != float64(14) {
		t.Errorf("fontSize = %v, want 14", got["fontSize"])
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Write — creates directory structure
// ───────────────────────────────────────────────────────────────────────────

func TestWriteCreatesDirectory(t *testing.T) {
	dir := t.TempDir()
	t.Setenv("XDG_CONFIG_HOME", dir)

	// cws-dashboard/ shouldn't exist yet.
	dashDir := filepath.Join(dir, "cws-dashboard")
	if _, err := os.Stat(dashDir); err == nil {
		t.Fatal("dashboard dir should not exist before Write")
	}

	if err := Write(map[string]any{"key": "val"}); err != nil {
		t.Fatalf("Write: %v", err)
	}

	if _, err := os.Stat(dashDir); err != nil {
		t.Errorf("Write should create dashboard dir: %v", err)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Read — corrupt JSON returns error
// ───────────────────────────────────────────────────────────────────────────

func TestReadCorruptJSON(t *testing.T) {
	dir := t.TempDir()
	t.Setenv("XDG_CONFIG_HOME", dir)

	// Create malformed preferences file.
	dashDir := filepath.Join(dir, "cws-dashboard")
	if err := os.MkdirAll(dashDir, 0755); err != nil {
		t.Fatal(err)
	}
	if err := os.WriteFile(filepath.Join(dashDir, prefsFilename), []byte("{bad json"), 0644); err != nil {
		t.Fatal(err)
	}

	_, err := Read()
	if err == nil {
		t.Error("Read should return error for corrupt JSON")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Write — overwrite preserves only latest
// ───────────────────────────────────────────────────────────────────────────

func TestWriteOverwrite(t *testing.T) {
	dir := t.TempDir()
	t.Setenv("XDG_CONFIG_HOME", dir)

	if err := Write(map[string]any{"old": "data"}); err != nil {
		t.Fatal(err)
	}
	if err := Write(map[string]any{"new": "data"}); err != nil {
		t.Fatal(err)
	}

	got, err := Read()
	if err != nil {
		t.Fatal(err)
	}

	if _, ok := got["old"]; ok {
		t.Error("old key should not exist after overwrite")
	}
	if got["new"] != "data" {
		t.Errorf("new = %v, want %q", got["new"], "data")
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
