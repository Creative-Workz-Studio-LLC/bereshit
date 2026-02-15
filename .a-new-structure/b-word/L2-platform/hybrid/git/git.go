//omni:code --go -library
//omni:key B-L2-util-vcs-git
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-util-vcs-git
// Purpose: Shared git status utilities
// Biblical: Proverbs 27:23 - "Be thou diligent to know the state"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-10
//
// Used by: statusline, hooks
//
// ═══════════════════════════════════════════════════════════════════════════

package git

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"os/exec"
	"strings"

	"creativeworkzstudio.com/bereshit/L2-platform/hybrid/terminal/display"
)

// Status holds git repository status
type Status struct {
	IsRepo       bool
	Branch       string
	IsDirty      bool
	ChangedFiles int
	Ahead        int
	Behind       int
}

// Display holds formatted git display info
type Display struct {
	Branch        string
	DisplayString string
	Icon          string
	Color         string
	HasInfo       bool
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// GetStatus returns git status for directory
func GetStatus(dir string) Status {
	if dir == "" {
		return Status{IsRepo: false}
	}

	// Check if in git repo
	cmd := exec.Command("git", "-C", dir, "rev-parse", "--is-inside-work-tree")
	if err := cmd.Run(); err != nil {
		return Status{IsRepo: false}
	}

	status := Status{IsRepo: true}

	// Get branch
	cmd = exec.Command("git", "-C", dir, "branch", "--show-current")
	if out, err := cmd.Output(); err == nil {
		status.Branch = strings.TrimSpace(string(out))
	}

	// Get dirty status
	cmd = exec.Command("git", "-C", dir, "status", "--porcelain")
	if out, err := cmd.Output(); err == nil {
		lines := strings.Split(strings.TrimSpace(string(out)), "\n")
		if len(lines) > 0 && lines[0] != "" {
			status.ChangedFiles = len(lines)
			status.IsDirty = true
		}
	}

	return status
}

// GetDisplay returns formatted display info for git status
func GetDisplay(dir string) Display {
	status := GetStatus(dir)

	if !status.IsRepo {
		return Display{HasInfo: false}
	}

	displayStr := status.Branch
	icon := "🌿"
	color := display.Green

	if status.IsDirty {
		displayStr = status.Branch + " *" + itoa(status.ChangedFiles)
		icon = "📝"
		color = display.Yellow
	}

	return Display{
		Branch:        status.Branch,
		DisplayString: displayStr,
		Icon:          icon,
		Color:         color,
		HasInfo:       true,
	}
}

// IsRepo checks if directory is a git repository
func IsRepo(dir string) bool {
	cmd := exec.Command("git", "-C", dir, "rev-parse", "--is-inside-work-tree")
	return cmd.Run() == nil
}

// GetBranch returns current branch name
func GetBranch(dir string) string {
	cmd := exec.Command("git", "-C", dir, "branch", "--show-current")
	if out, err := cmd.Output(); err == nil {
		return strings.TrimSpace(string(out))
	}
	return ""
}

// GetChangedFiles returns count of changed files
func GetChangedFiles(dir string) int {
	cmd := exec.Command("git", "-C", dir, "status", "--porcelain")
	if out, err := cmd.Output(); err == nil {
		lines := strings.Split(strings.TrimSpace(string(out)), "\n")
		if len(lines) > 0 && lines[0] != "" {
			return len(lines)
		}
	}
	return 0
}

func itoa(i int) string {
	if i == 0 {
		return "0"
	}
	var digits []byte
	for i > 0 {
		digits = append([]byte{byte('0' + i%10)}, digits...)
		i /= 10
	}
	return string(digits)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
