//omni:code --rust -library
//omni:key B-L0-foundation-workflow-rs
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

//! # Workflow Types (Rust)
//!
//! Universal workflow operation types for step-based execution.
//!
//! Extracted from L3-cpisi — these are generic workflow tracking types
//! usable by any layer. CPI-SI specific types remain in L3.

// ============================================================================
// SETUP
// ============================================================================

use serde::{Deserialize, Serialize};
use std::fmt;

// ────────────────────────────────────────────────────────────────
// S.1 Types
// ────────────────────────────────────────────────────────────────

/// Type of workflow operation being performed.
#[derive(Debug, Clone, Default, PartialEq, Eq, Serialize, Deserialize)]
pub enum WorkflowOperation {
    /// No active workflow.
    #[serde(rename = "")]
    #[default]
    None,
    /// Creating new files from templates.
    #[serde(rename = "file-creation")]
    FileCreation,
    /// Aligning documentation with reality.
    #[serde(rename = "alignment")]
    Alignment,
    /// Updating existing files block by block.
    #[serde(rename = "block-update")]
    BlockUpdate,
    /// Git commit workflow.
    #[serde(rename = "commit")]
    Commit,
}

/// A step within a workflow.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct WorkflowStep {
    /// Step number (1-based).
    pub index: usize,
    /// What to do.
    pub action: String,
    /// Human-readable description.
    pub description: String,
    /// Whether step is done.
    pub completed: bool,
}

/// Tracks current workflow state.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct RuntimeWorkflow {
    /// Current operation type.
    pub operation: WorkflowOperation,
    /// Current step index (1-based).
    pub current_step: usize,
    /// Total steps in workflow.
    pub total_steps: usize,
    /// When workflow started.
    #[serde(default)]
    pub started_at: String,
    /// All steps with completion status.
    #[serde(default)]
    pub steps: Vec<WorkflowStep>,
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 WorkflowOperation
// ────────────────────────────────────────────────────────────────

// Default derived on enum declaration above (None variant)

impl fmt::Display for WorkflowOperation {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::None => write!(f, "none"),
            Self::FileCreation => write!(f, "file-creation"),
            Self::Alignment => write!(f, "alignment"),
            Self::BlockUpdate => write!(f, "block-update"),
            Self::Commit => write!(f, "commit"),
        }
    }
}

// ────────────────────────────────────────────────────────────────
// B.2 RuntimeWorkflow
// ────────────────────────────────────────────────────────────────

impl RuntimeWorkflow {
    /// Whether a workflow is currently in progress.
    pub fn is_active(&self) -> bool {
        self.operation != WorkflowOperation::None
    }

    /// Whether all steps are done.
    pub fn is_complete(&self) -> bool {
        self.total_steps > 0 && self.current_step > self.total_steps
    }

    /// Completion percentage (0-100).
    pub fn progress(&self) -> usize {
        if self.total_steps == 0 {
            return 0;
        }
        let completed = self.steps.iter().filter(|s| s.completed).count();
        completed * 100 / self.total_steps
    }
}

impl Default for RuntimeWorkflow {
    fn default() -> Self {
        Self {
            operation: WorkflowOperation::None,
            current_step: 0,
            total_steps: 0,
            started_at: String::new(),
            steps: Vec::new(),
        }
    }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Generic workflow types — extracted from L3, usable by all layers.
// "A man's heart deviseth his way" — Proverbs 16:9

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn default_inactive() {
        let w = RuntimeWorkflow::default();
        assert!(!w.is_active());
        assert!(!w.is_complete());
        assert_eq!(w.progress(), 0);
    }

    #[test]
    fn progress_calculation() {
        let w = RuntimeWorkflow {
            operation: WorkflowOperation::FileCreation,
            current_step: 2,
            total_steps: 4,
            started_at: String::new(),
            steps: vec![
                WorkflowStep { index: 1, action: "a".into(), description: "".into(), completed: true },
                WorkflowStep { index: 2, action: "b".into(), description: "".into(), completed: true },
                WorkflowStep { index: 3, action: "c".into(), description: "".into(), completed: false },
                WorkflowStep { index: 4, action: "d".into(), description: "".into(), completed: false },
            ],
        };
        assert!(w.is_active());
        assert!(!w.is_complete());
        assert_eq!(w.progress(), 50);
    }

    #[test]
    fn operation_display() {
        assert_eq!(WorkflowOperation::Commit.to_string(), "commit");
        assert_eq!(WorkflowOperation::None.to_string(), "none");
    }
}
