// ============================================================================
// METADATA
// ============================================================================
// File System Utilities - Shared file system operations library
// Provides common file system checks for hooks, statusline, and other components
// Non-blocking: Returns boolean results, doesn't print or format output

package fs

// ============================================================================
// SETUP
// ============================================================================
import (
	"os"
)

// ============================================================================
// BODY
// ============================================================================

// PathExists checks if a file or directory exists
func PathExists(path string) bool {
	_, err := os.Stat(path)
	return err == nil
}

// FileIsNewer returns true if file1 was modified after file2
func FileIsNewer(file1, file2 string) bool {
	info1, err1 := os.Stat(file1)
	info2, err2 := os.Stat(file2)
	if err1 != nil || err2 != nil {
		return false
	}
	return info1.ModTime().After(info2.ModTime())
}

// ============================================================================
// CLOSING
// ============================================================================
// Function-based library - no execution needed
