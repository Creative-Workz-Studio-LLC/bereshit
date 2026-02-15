//omni:code --go -library
//omni:key B-L0-config-util-exists
//omni:version b-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L0-config-util-exists
// Purpose: File and directory existence checks — single source of truth
// Biblical: Psalm 139:7-8 - "Whither shall I go from thy spirit?"
// Authors: Nova Dawn
// Version: b-01.00
// Created: 2026-02-13
//
// Extracted from: 4 identical fileExists() copies across L0 and L3 loaders.
// Single source of truth. Every loader calls these instead of re-implementing.
//
// ============================================================================
// END METADATA
// ============================================================================

package util

// ============================================================================
// SETUP
// ============================================================================

import "os"

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Existence Checks
// ────────────────────────────────────────────────────────────────

// FileExists returns true if a file or directory exists at path.
// Returns false for ANY stat error — permission denied, broken symlink, etc.
func FileExists(path string) bool {
	_, err := os.Stat(path)
	return err == nil
}

// DirExists returns true if path exists AND is a directory.
func DirExists(path string) bool {
	info, err := os.Stat(path)
	if err != nil {
		return false
	}
	return info.IsDir()
}

// IsFile returns true if path exists AND is a regular file (not a directory).
func IsFile(path string) bool {
	info, err := os.Stat(path)
	if err != nil {
		return false
	}
	return !info.IsDir()
}

// ────────────────────────────────────────────────────────────────
// Directory Operations
// ────────────────────────────────────────────────────────────────

// EnsureDir creates the directory at path (and parents) if it doesn't exist.
// No-op if the directory already exists. Returns error if path exists as a file,
// or if creation fails.
func EnsureDir(path string) error {
	info, err := os.Stat(path)
	if err == nil {
		if info.IsDir() {
			return nil // already exists
		}
		return &os.PathError{Op: "mkdir", Path: path, Err: os.ErrExist}
	}
	return os.MkdirAll(path, 0755)
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// "Whither shall I go from thy spirit? or whither shall I flee from thy
// presence? If I ascend up into heaven, thou art there: if I make my bed
// in hell, behold, thou art there." — Psalm 139:7-8
//
// Existence checks. Simple. Honest. Is it there or not?
//
// ============================================================================
// END CLOSING
// ============================================================================
