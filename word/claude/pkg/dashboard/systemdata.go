// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-dashboard-systemdata
// Purpose: Browse and read system/data files for dashboard display
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-07
//
// ═══════════════════════════════════════════════════════════════════════════

package dashboard

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"cws.studio/pkg/util/fs/paths"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// SystemDataEntry represents a file or directory in system/data.
type SystemDataEntry struct {
	Name     string            `json:"name"`
	Path     string            `json:"path"`     // Relative to system/data/
	IsDir    bool              `json:"isDir"`
	Size     int64             `json:"size,omitempty"`
	Children []SystemDataEntry `json:"children,omitempty"` // For directories
	Content  string            `json:"content,omitempty"`  // For files (when requested)
}

// ReadSystemData returns a directory listing or file content from system/data.
// Path is relative to system/data/ (e.g., "temporal/patterns" or "session/current.json").
// Path traversal is prevented.
func ReadSystemData(relativePath string) (*SystemDataEntry, error) {
	baseDir := filepath.Join(paths.ClaudeGlobalRoot(), "system", "data")

	// Prevent path traversal
	cleanPath := filepath.Clean(relativePath)
	if strings.Contains(cleanPath, "..") {
		return nil, fmt.Errorf("path traversal not allowed")
	}

	fullPath := filepath.Join(baseDir, cleanPath)

	// Verify the resolved path is still under baseDir
	if !strings.HasPrefix(fullPath, baseDir) {
		return nil, fmt.Errorf("path traversal not allowed")
	}

	info, err := os.Stat(fullPath)
	if err != nil {
		return nil, err
	}

	entry := &SystemDataEntry{
		Name:  info.Name(),
		Path:  relativePath,
		IsDir: info.IsDir(),
		Size:  info.Size(),
	}

	if info.IsDir() {
		// List directory contents
		dirEntries, err := os.ReadDir(fullPath)
		if err != nil {
			return nil, err
		}
		for _, de := range dirEntries {
			deInfo, err := de.Info()
			if err != nil {
				continue
			}
			child := SystemDataEntry{
				Name:  de.Name(),
				Path:  filepath.Join(relativePath, de.Name()),
				IsDir: de.IsDir(),
				Size:  deInfo.Size(),
			}
			entry.Children = append(entry.Children, child)
		}
	} else {
		// Read file content (limit to 1MB to prevent OOM)
		if info.Size() > 1024*1024 {
			entry.Content = fmt.Sprintf("[File too large: %d bytes]", info.Size())
		} else {
			data, err := os.ReadFile(fullPath)
			if err != nil {
				return nil, err
			}
			entry.Content = string(data)
		}
	}

	return entry, nil
}

// ListSystemDataCategories returns the top-level categories in system/data.
func ListSystemDataCategories() ([]SystemDataEntry, error) {
	entry, err := ReadSystemData("")
	if err != nil {
		return nil, err
	}
	return entry.Children, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
