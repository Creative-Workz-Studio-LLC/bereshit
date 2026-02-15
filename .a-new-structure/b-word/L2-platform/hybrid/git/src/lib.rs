//omni:code --rust -library
//omni:key B-L2-git
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L2-git
// Purpose: Shared git status utilities
// Biblical: Proverbs 27:23 — "Be thou diligent to know the state"
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-15
//
// Used by: statusline, hooks
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────
// S.1 Imports
// ────────────────────────────────────────────────────────────────

use std::process::Command;

// ────────────────────────────────────────────────────────────────
// S.2 Core Types
// ────────────────────────────────────────────────────────────────

/// Git repository status information.
#[derive(Debug, Clone, Default)]
pub struct Status {
    pub is_repo: bool,
    pub branch: String,
    pub is_dirty: bool,
    pub changed_files: usize,
    pub ahead: usize,
    pub behind: usize,
}

/// Formatted git display info for statusline rendering.
#[derive(Debug, Clone, Default)]
pub struct Display {
    pub branch: String,
    pub display_string: String,
    pub icon: String,
    pub color: String,
    pub has_info: bool,
}

// ============================================================================
// BODY
// ============================================================================

/// Returns git status for the given directory.
///
/// Calls `git rev-parse`, `git branch --show-current`, and
/// `git status --porcelain` to gather repository information.
pub fn get_status(dir: &str) -> Status {
    if dir.is_empty() {
        return Status::default();
    }

    // Check if in git repo
    let Ok(output) = Command::new("git")
        .args(["-C", dir, "rev-parse", "--is-inside-work-tree"])
        .output()
    else {
        return Status::default();
    };

    if !output.status.success() {
        return Status::default();
    }

    let mut status = Status {
        is_repo: true,
        ..Default::default()
    };

    // Get branch
    if let Ok(output) = Command::new("git")
        .args(["-C", dir, "branch", "--show-current"])
        .output()
    {
        if output.status.success() {
            status.branch = String::from_utf8_lossy(&output.stdout).trim().to_string();
        }
    }

    // Get dirty status
    if let Ok(output) = Command::new("git")
        .args(["-C", dir, "status", "--porcelain"])
        .output()
    {
        if output.status.success() {
            let text = String::from_utf8_lossy(&output.stdout);
            let trimmed = text.trim();
            if !trimmed.is_empty() {
                let lines: Vec<&str> = trimmed.lines().collect();
                status.changed_files = lines.len();
                status.is_dirty = true;
            }
        }
    }

    status
}

/// Returns formatted display info for git status.
pub fn get_display(dir: &str) -> Display {
    let status = get_status(dir);

    if !status.is_repo {
        return Display::default();
    }

    let (display_string, icon, color) = if status.is_dirty {
        (
            format!("{} *{}", status.branch, status.changed_files),
            "\u{1f4dd}".to_string(), // memo emoji
            bereshit_l2_display::YELLOW.to_string(),
        )
    } else {
        (
            status.branch.clone(),
            "\u{1f33f}".to_string(), // herb emoji
            bereshit_l2_display::GREEN.to_string(),
        )
    };

    Display {
        branch: status.branch,
        display_string,
        icon,
        color,
        has_info: true,
    }
}

/// Checks if the given directory is a git repository.
pub fn is_repo(dir: &str) -> bool {
    Command::new("git")
        .args(["-C", dir, "rev-parse", "--is-inside-work-tree"])
        .output()
        .map(|o| o.status.success())
        .unwrap_or(false)
}

/// Returns the current branch name for the given directory.
pub fn get_branch(dir: &str) -> String {
    Command::new("git")
        .args(["-C", dir, "branch", "--show-current"])
        .output()
        .ok()
        .filter(|o| o.status.success())
        .map(|o| String::from_utf8_lossy(&o.stdout).trim().to_string())
        .unwrap_or_default()
}

/// Returns the count of changed files in the given directory.
pub fn get_changed_files(dir: &str) -> usize {
    Command::new("git")
        .args(["-C", dir, "status", "--porcelain"])
        .output()
        .ok()
        .filter(|o| o.status.success())
        .map(|o| {
            let text = String::from_utf8_lossy(&o.stdout);
            let trimmed = text.trim();
            if trimmed.is_empty() {
                0
            } else {
                trimmed.lines().count()
            }
        })
        .unwrap_or(0)
}

// ============================================================================
// CLOSING
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn empty_dir_returns_no_repo() {
        let status = get_status("");
        assert!(!status.is_repo);
    }

    #[test]
    fn nonexistent_dir_returns_no_repo() {
        let status = get_status("/nonexistent/path/that/does/not/exist");
        assert!(!status.is_repo);
    }

    #[test]
    fn display_default_has_no_info() {
        let display = Display::default();
        assert!(!display.has_info);
    }

    #[test]
    fn is_repo_false_for_nonexistent() {
        assert!(!is_repo("/nonexistent/path/that/does/not/exist"));
    }
}
