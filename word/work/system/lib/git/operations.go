// ============================================================================
// METADATA
// ============================================================================
// Git Operations - Shared git repository interaction library
// Provides raw git data for consumption by hooks, statusline, and other components
// Non-blocking: Returns data structures, doesn't print or format output

package git

// ============================================================================
// SETUP
// ============================================================================
import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

// ============================================================================
// BODY
// ============================================================================

// Info contains comprehensive git repository status
type Info struct {
	Branch         string   // Current branch name or commit hash (detached HEAD)
	Dirty          bool     // True if uncommitted changes exist
	Ahead          int      // Number of commits ahead of remote
	Behind         int      // Number of commits behind remote
	Stashes        int      // Number of stashed changes
	Conflicts      []string // List of files with merge conflicts
	UncommittedCount int    // Number of uncommitted changes
}

// GetBranch reads the current git branch from .git/HEAD
func GetBranch(dir string) string {
	// Try to read .git/HEAD to get current branch
	gitHeadPath := filepath.Join(dir, ".git", "HEAD")
	data, err := os.ReadFile(gitHeadPath)
	if err != nil {
		return ""
	}

	content := strings.TrimSpace(string(data))
	if strings.HasPrefix(content, "ref: refs/heads/") {
		return strings.TrimPrefix(content, "ref: refs/heads/")
	}

	// Detached HEAD - show short commit hash
	if len(content) >= 7 {
		return content[:7]
	}

	return ""
}

// IsGitRepository checks if the given directory is a git repository
func IsGitRepository(dir string) bool {
	gitDir := filepath.Join(dir, ".git")
	_, err := os.Stat(gitDir)
	return err == nil
}

// GetInfo retrieves comprehensive git repository status
func GetInfo(dir string) Info {
	info := Info{
		Branch: GetBranch(dir),
	}

	// If not a git repository or no branch, return empty info
	if info.Branch == "" {
		return info
	}

	// Check for uncommitted changes
	cmd := exec.Command("git", "status", "--porcelain")
	cmd.Dir = dir
	if output, err := cmd.Output(); err == nil {
		trimmed := strings.TrimSpace(string(output))
		if len(trimmed) > 0 {
			info.Dirty = true
			lines := strings.Split(trimmed, "\n")
			info.UncommittedCount = len(lines)
		}
	}

	// Check ahead/behind status relative to upstream
	cmd = exec.Command("git", "rev-list", "--left-right", "--count", "HEAD...@{upstream}")
	cmd.Dir = dir
	if output, err := cmd.Output(); err == nil {
		parts := strings.Fields(string(output))
		if len(parts) == 2 {
			// parts[0] is ahead, parts[1] is behind
			if parts[0] != "0" {
				var ahead int
				if _, err := fmt.Sscanf(parts[0], "%d", &ahead); err == nil {
					info.Ahead = ahead
				}
			}
			if parts[1] != "0" {
				var behind int
				if _, err := fmt.Sscanf(parts[1], "%d", &behind); err == nil {
					info.Behind = behind
				}
			}
		}
	}

	// Check for stashed changes
	cmd = exec.Command("git", "stash", "list")
	cmd.Dir = dir
	if output, err := cmd.Output(); err == nil && len(output) > 0 {
		lines := strings.Split(strings.TrimSpace(string(output)), "\n")
		info.Stashes = len(lines)
	}

	// Check for merge conflicts
	cmd = exec.Command("git", "diff", "--name-only", "--diff-filter=U")
	cmd.Dir = dir
	if output, err := cmd.Output(); err == nil && len(output) > 0 {
		info.Conflicts = strings.Split(strings.TrimSpace(string(output)), "\n")
	}

	return info
}

// ============================================================================
// CLOSING
// ============================================================================
// Function-based library - no execution needed
