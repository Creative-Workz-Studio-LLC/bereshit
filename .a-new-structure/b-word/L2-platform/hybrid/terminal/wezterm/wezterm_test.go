//omni:code --go -test
//omni:key B-L2-wezterm-test
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-wezterm-test
// Purpose: Unit tests for WezTerm OSC sequence emission and state management
// Biblical: Proverbs 25:11 — "A word fitly spoken is like apples of gold"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-15
//
// ═══════════════════════════════════════════════════════════════════════════

package wezterm

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
// Constants — verify OSC escape codes
// ───────────────────────────────────────────────────────────────────────────

func TestESCConstant(t *testing.T) {
	if ESC != "\x1b" {
		t.Errorf("ESC = %q, want \\x1b", ESC)
	}
}

func TestBELConstant(t *testing.T) {
	if BEL != "\x07" {
		t.Errorf("BEL = %q, want \\x07", BEL)
	}
}

func TestSTConstant(t *testing.T) {
	if ST != "\x1b\\" {
		t.Errorf("ST = %q, want \\x1b\\\\", ST)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// cfg.CommandColors — verify all command states have color definitions
// ───────────────────────────────────────────────────────────────────────────

func TestCommandColorsHALT(t *testing.T) {
	colors, ok := cfg.CommandColors["HALT"]
	if !ok {
		t.Fatal("HALT should have color definitions")
	}
	if colors.Foreground == "" || colors.Background == "" || colors.Cursor == "" {
		t.Error("HALT should have all three color fields")
	}
}

func TestCommandColorsAWAIT(t *testing.T) {
	if _, ok := cfg.CommandColors["AWAIT"]; !ok {
		t.Error("AWAIT should have color definitions")
	}
}

func TestCommandColorsPROCEED(t *testing.T) {
	if _, ok := cfg.CommandColors["PROCEED"]; !ok {
		t.Error("PROCEED should have color definitions")
	}
}

func TestCommandColorsCount(t *testing.T) {
	if len(cfg.CommandColors) != 3 {
		t.Errorf("expected 3 command colors, got %d", len(cfg.CommandColors))
	}
}

// ───────────────────────────────────────────────────────────────────────────
// StateVars — verify struct fields
// ───────────────────────────────────────────────────────────────────────────

func TestStateVarsConstruction(t *testing.T) {
	sv := StateVars{
		Position: "PRESENT-PRESENT",
		Meaning:  "Origin (Genesis 1:1)",
		Command:  "HALT",
		Operator: "|",
		Anchor:   "Genesis 1:1",
		X:        "PRESENT",
		Y:        "PRESENT",
		Z:        0.0,
	}

	if sv.Position != "PRESENT-PRESENT" {
		t.Errorf("Position = %q", sv.Position)
	}
	if sv.Command != "HALT" {
		t.Errorf("Command = %q", sv.Command)
	}
	if sv.Z != 0.0 {
		t.Errorf("Z = %f", sv.Z)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// State file — read/write persistence
// ───────────────────────────────────────────────────────────────────────────

func TestReadPreviousCommandMissing(t *testing.T) {
	// Use temp dir to avoid reading real state.
	dir := t.TempDir()
	t.Setenv("XDG_RUNTIME_DIR", dir)

	got := ReadPreviousCommand()
	if got != "" {
		t.Errorf("missing state file should return empty, got %q", got)
	}
}

func TestWriteAndReadPreviousCommand(t *testing.T) {
	dir := t.TempDir()
	t.Setenv("XDG_RUNTIME_DIR", dir)

	WritePreviousCommand("PROCEED")

	got := ReadPreviousCommand()
	if got != "PROCEED" {
		t.Errorf("ReadPreviousCommand() = %q, want %q", got, "PROCEED")
	}
}

func TestStateFilePath(t *testing.T) {
	dir := t.TempDir()
	t.Setenv("XDG_RUNTIME_DIR", dir)

	path := stateFilePath()
	expected := filepath.Join(dir, "cpisi-wezterm-state")
	if path != expected {
		t.Errorf("stateFilePath() = %q, want %q", path, expected)
	}
}

func TestStateFilePathFallback(t *testing.T) {
	t.Setenv("XDG_RUNTIME_DIR", "")

	path := stateFilePath()
	if filepath.Dir(path) != "/tmp" {
		t.Errorf("stateFilePath() with empty XDG should use /tmp, got %q", path)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// SetCommandColors — unknown command is safe no-op
// ───────────────────────────────────────────────────────────────────────────

func TestSetCommandColorsUnknownIsSafe(t *testing.T) {
	// Should not panic.
	SetCommandColors("NONEXISTENT")
}

// ───────────────────────────────────────────────────────────────────────────
// State file overwrite
// ───────────────────────────────────────────────────────────────────────────

func TestWriteOverwritesPrevious(t *testing.T) {
	dir := t.TempDir()
	t.Setenv("XDG_RUNTIME_DIR", dir)

	WritePreviousCommand("HALT")
	WritePreviousCommand("AWAIT")

	got := ReadPreviousCommand()
	if got != "AWAIT" {
		t.Errorf("after overwrite: got %q, want %q", got, "AWAIT")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// State file permissions
// ───────────────────────────────────────────────────────────────────────────

func TestWriteStateFilePermissions(t *testing.T) {
	dir := t.TempDir()
	t.Setenv("XDG_RUNTIME_DIR", dir)

	WritePreviousCommand("HALT")

	info, err := os.Stat(filepath.Join(dir, "cpisi-wezterm-state"))
	if err != nil {
		t.Fatalf("state file not created: %v", err)
	}
	// Should be owner-only (0600).
	perm := info.Mode().Perm()
	if perm != 0600 {
		t.Errorf("state file permissions = %o, want 0600", perm)
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
