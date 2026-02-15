//omni:code --go -test
//omni:key B-L2-git-test
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-git-test
// Purpose: Unit tests for git status utilities
// Biblical: Proverbs 27:23 — "Be thou diligent to know the state"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-15
//
// ═══════════════════════════════════════════════════════════════════════════

package git

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"os"
	"os/exec"
	"path/filepath"
	"testing"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// itoa — internal integer-to-string helper
// ───────────────────────────────────────────────────────────────────────────

func TestItoaZero(t *testing.T) {
	if got := itoa(0); got != "0" {
		t.Errorf("itoa(0) = %q, want %q", got, "0")
	}
}

func TestItoaSingleDigit(t *testing.T) {
	if got := itoa(7); got != "7" {
		t.Errorf("itoa(7) = %q, want %q", got, "7")
	}
}

func TestItoaMultiDigit(t *testing.T) {
	if got := itoa(42); got != "42" {
		t.Errorf("itoa(42) = %q, want %q", got, "42")
	}
}

func TestItoaLargeNumber(t *testing.T) {
	if got := itoa(12345); got != "12345" {
		t.Errorf("itoa(12345) = %q, want %q", got, "12345")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// GetStatus — empty/invalid directory
// ───────────────────────────────────────────────────────────────────────────

func TestGetStatusEmptyDir(t *testing.T) {
	status := GetStatus("")
	if status.IsRepo {
		t.Error("empty dir should not be a repo")
	}
}

func TestGetStatusNonexistentDir(t *testing.T) {
	status := GetStatus("/tmp/nonexistent-git-dir-99999")
	if status.IsRepo {
		t.Error("nonexistent dir should not be a repo")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// IsRepo — verify non-repo directories
// ───────────────────────────────────────────────────────────────────────────

func TestIsRepoFalseForTmp(t *testing.T) {
	dir := t.TempDir()
	if IsRepo(dir) {
		t.Error("temp dir should not be a git repo")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// GetBranch — non-repo returns empty
// ───────────────────────────────────────────────────────────────────────────

func TestGetBranchNonRepo(t *testing.T) {
	dir := t.TempDir()
	if got := GetBranch(dir); got != "" {
		t.Errorf("GetBranch(non-repo) = %q, want empty", got)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// GetChangedFiles — non-repo returns 0
// ───────────────────────────────────────────────────────────────────────────

func TestGetChangedFilesNonRepo(t *testing.T) {
	dir := t.TempDir()
	if got := GetChangedFiles(dir); got != 0 {
		t.Errorf("GetChangedFiles(non-repo) = %d, want 0", got)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// GetDisplay — non-repo yields HasInfo=false
// ───────────────────────────────────────────────────────────────────────────

func TestGetDisplayNonRepo(t *testing.T) {
	dir := t.TempDir()
	d := GetDisplay(dir)
	if d.HasInfo {
		t.Error("GetDisplay(non-repo) should have HasInfo=false")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Integration — real git repo in temp dir
// ───────────────────────────────────────────────────────────────────────────

func initTestRepo(t *testing.T) string {
	t.Helper()
	dir := t.TempDir()

	// Initialize git repo.
	cmd := exec.Command("git", "init", dir)
	cmd.Env = append(os.Environ(),
		"GIT_CONFIG_GLOBAL=/dev/null",
		"GIT_CONFIG_SYSTEM=/dev/null",
	)
	if out, err := cmd.CombinedOutput(); err != nil {
		t.Fatalf("git init: %v\n%s", err, out)
	}

	// Configure user for commits.
	for _, args := range [][]string{
		{"config", "user.email", "test@test.com"},
		{"config", "user.name", "Test"},
	} {
		cmd = exec.Command("git", append([]string{"-C", dir}, args...)...)
		if out, err := cmd.CombinedOutput(); err != nil {
			t.Fatalf("git %v: %v\n%s", args, err, out)
		}
	}

	// Create initial commit.
	readme := filepath.Join(dir, "README.md")
	if err := os.WriteFile(readme, []byte("# test"), 0644); err != nil {
		t.Fatal(err)
	}
	cmd = exec.Command("git", "-C", dir, "add", ".")
	if out, err := cmd.CombinedOutput(); err != nil {
		t.Fatalf("git add: %v\n%s", err, out)
	}
	cmd = exec.Command("git", "-C", dir, "commit", "--no-verify", "-m", "[test] INIT: Test repo")
	if out, err := cmd.CombinedOutput(); err != nil {
		t.Fatalf("git commit: %v\n%s", err, out)
	}

	return dir
}

func TestRealRepoIsRepo(t *testing.T) {
	dir := initTestRepo(t)
	if !IsRepo(dir) {
		t.Error("initialized repo should be detected as git repo")
	}
}

func TestRealRepoGetStatus(t *testing.T) {
	dir := initTestRepo(t)
	status := GetStatus(dir)

	if !status.IsRepo {
		t.Fatal("should be a repo")
	}
	if status.IsDirty {
		t.Error("clean repo should not be dirty")
	}
	if status.ChangedFiles != 0 {
		t.Errorf("clean repo changed files = %d, want 0", status.ChangedFiles)
	}
}

func TestRealRepoDirtyStatus(t *testing.T) {
	dir := initTestRepo(t)

	// Create an untracked file.
	if err := os.WriteFile(filepath.Join(dir, "new.txt"), []byte("new"), 0644); err != nil {
		t.Fatal(err)
	}

	status := GetStatus(dir)
	if !status.IsDirty {
		t.Error("repo with untracked file should be dirty")
	}
	if status.ChangedFiles < 1 {
		t.Errorf("expected at least 1 changed file, got %d", status.ChangedFiles)
	}
}

func TestRealRepoGetDisplay(t *testing.T) {
	dir := initTestRepo(t)
	d := GetDisplay(dir)

	if !d.HasInfo {
		t.Fatal("real repo display should have info")
	}
	if d.Branch == "" {
		t.Error("branch should not be empty")
	}
	if d.Icon == "" {
		t.Error("icon should not be empty")
	}
	if d.Color == "" {
		t.Error("color should not be empty")
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
