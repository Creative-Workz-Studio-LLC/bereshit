//omni:code --rust -library
//omni:key B-L0-foundation-workflow-rs
//omni:version b-02.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-L0-foundation-workflow-rs
// Purpose: Universal workflow operation types for step-based execution
// Biblical: Proverbs 16:9 — A man's heart deviseth his way: but the LORD
//           directeth his steps
// Version: b-02.00

//! # Workflow Types
//!
//! Universal workflow operation types for step-based execution tracking.
//!
//! | Type | Models | When |
//! |------|--------|------|
//! | [`WorkflowOperation`] | Type of workflow in progress | Enum discriminant |
//! | [`WorkflowStep`] | A single step with action + status | Step tracking |
//! | [`RuntimeWorkflow`] | Full workflow state machine | Orchestration |
//!
//! Extracted from L3-cpisi — these are generic workflow tracking types
//! usable by any layer. CPI-SI specific types remain in L3.

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

use serde::{Deserialize, Serialize};
use std::fmt;

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────

/// Type of workflow operation being performed.
///
/// Each variant maps to a specific multi-step process. The serialized
/// form uses kebab-case strings for JSON readability.
///
/// # Examples
///
/// ```
/// use bereshit_l0_foundation::WorkflowOperation;
///
/// let op = WorkflowOperation::default();
/// assert_eq!(op, WorkflowOperation::None);
/// assert_eq!(op.to_string(), "none");
/// ```
#[derive(Debug, Clone, Default, PartialEq, Eq, Serialize, Deserialize)]
pub enum WorkflowOperation {
    /// No active workflow.
    #[serde(rename = "none")]
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
///
/// Steps are 1-indexed. Each step has an action (machine-readable verb)
/// and a description (human-readable explanation).
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct WorkflowStep {
    /// Step number (1-based).
    pub index: usize,
    /// Machine-readable action verb (e.g., "copy", "lint", "commit").
    pub action: String,
    /// Human-readable description of what this step does.
    pub description: String,
    /// Whether this step has been completed.
    pub completed: bool,
}

/// Tracks current workflow state — the state machine.
///
/// Holds the operation type, current position, and all steps with their
/// completion status. Progress is computed from the steps vec, not from
/// `current_step` — the steps are the source of truth.
///
/// # Examples
///
/// ```
/// use bereshit_l0_foundation::{RuntimeWorkflow, WorkflowOperation, WorkflowStep};
///
/// let mut w = RuntimeWorkflow::new(WorkflowOperation::Alignment, vec![
///     WorkflowStep { index: 1, action: "lint".into(), description: "Run linter".into(), completed: false },
///     WorkflowStep { index: 2, action: "fix".into(), description: "Apply fixes".into(), completed: false },
/// ]);
/// assert!(w.is_active());
/// assert_eq!(w.progress(), 0);
///
/// w.steps[0].completed = true;
/// w.current_step = 2;
/// assert_eq!(w.progress(), 50);
/// ```
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct RuntimeWorkflow {
    /// Current operation type.
    pub operation: WorkflowOperation,
    /// Current step index (1-based). 0 = not started.
    pub current_step: usize,
    /// Total steps in this workflow.
    pub total_steps: usize,
    /// ISO timestamp when workflow started (empty = not started).
    #[serde(default)]
    pub started_at: String,
    /// All steps with completion status.
    #[serde(default)]
    pub steps: Vec<WorkflowStep>,
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// Subsection order:
//   3. Constructors   — Creating workflow instances
//   5. Queries        — State inspection
//   6. Output         — Display formatting

// ──────────────────────────────────────────────────────────────────────────
// 3. Constructors
// ──────────────────────────────────────────────────────────────────────────

impl RuntimeWorkflow {
    /// Create a new workflow with the given operation and steps.
    ///
    /// Sets `total_steps` from the steps vec length. `current_step`
    /// starts at 1 (first step) if steps are provided, 0 otherwise.
    pub fn new(operation: WorkflowOperation, steps: Vec<WorkflowStep>) -> Self {
        let total = steps.len();
        Self {
            operation,
            current_step: if total > 0 { 1 } else { 0 },
            total_steps: total,
            started_at: String::new(),
            steps,
        }
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

// ──────────────────────────────────────────────────────────────────────────
// 5. Queries & Accessors
// ──────────────────────────────────────────────────────────────────────────

impl RuntimeWorkflow {
    /// Whether a workflow is currently in progress.
    pub fn is_active(&self) -> bool {
        self.operation != WorkflowOperation::None
    }

    /// Whether all steps are completed.
    ///
    /// A workflow is complete when every step in the steps vec is marked
    /// completed. An empty workflow (no steps) is not complete — it hasn't
    /// started.
    pub fn is_complete(&self) -> bool {
        !self.steps.is_empty() && self.steps.iter().all(|s| s.completed)
    }

    /// Completion percentage (0-100).
    ///
    /// Based on the number of completed steps in the steps vec.
    /// Returns 0 for empty workflows.
    pub fn progress(&self) -> usize {
        if self.total_steps == 0 {
            return 0;
        }
        let completed = self.steps.iter().filter(|s| s.completed).count();
        completed * 100 / self.total_steps
    }

    /// Number of completed steps.
    pub fn completed_count(&self) -> usize {
        self.steps.iter().filter(|s| s.completed).count()
    }

    /// Number of remaining (incomplete) steps.
    pub fn remaining_count(&self) -> usize {
        self.steps.iter().filter(|s| !s.completed).count()
    }
}

impl WorkflowOperation {
    /// Whether this is the `None` variant.
    pub fn is_none(&self) -> bool {
        *self == Self::None
    }
}

// ──────────────────────────────────────────────────────────────────────────
// 6. Output & Display
// ──────────────────────────────────────────────────────────────────────────

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

impl fmt::Display for RuntimeWorkflow {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if self.operation == WorkflowOperation::None {
            write!(f, "[idle]")
        } else {
            write!(
                f,
                "[{}] step {}/{} ({}%)",
                self.operation,
                self.current_step,
                self.total_steps,
                self.progress()
            )
        }
    }
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// "A man's heart deviseth his way: but the LORD directeth his steps."
// — Proverbs 16:9

// ──────────────────────────────────────────────────────────────────────────
// Cv — Closing Validation
// ──────────────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;

    // -- WorkflowOperation ------------------------------------------------

    #[test]
    fn operation_default_is_none() {
        assert_eq!(WorkflowOperation::default(), WorkflowOperation::None);
        assert!(WorkflowOperation::None.is_none());
    }

    #[test]
    fn operation_display() {
        assert_eq!(WorkflowOperation::None.to_string(), "none");
        assert_eq!(WorkflowOperation::FileCreation.to_string(), "file-creation");
        assert_eq!(WorkflowOperation::Alignment.to_string(), "alignment");
        assert_eq!(WorkflowOperation::BlockUpdate.to_string(), "block-update");
        assert_eq!(WorkflowOperation::Commit.to_string(), "commit");
    }

    #[test]
    fn operation_serde_roundtrip() {
        let ops = [
            WorkflowOperation::None,
            WorkflowOperation::FileCreation,
            WorkflowOperation::Alignment,
            WorkflowOperation::BlockUpdate,
            WorkflowOperation::Commit,
        ];
        for op in &ops {
            let json = serde_json::to_string(op).unwrap();
            let restored: WorkflowOperation = serde_json::from_str(&json).unwrap();
            assert_eq!(&restored, op);
        }
    }

    #[test]
    fn operation_none_serializes_as_none_string() {
        let json = serde_json::to_string(&WorkflowOperation::None).unwrap();
        assert_eq!(json, "\"none\"");
    }

    // -- RuntimeWorkflow --------------------------------------------------

    #[test]
    fn default_is_inactive() {
        let w = RuntimeWorkflow::default();
        assert!(!w.is_active());
        assert!(!w.is_complete());
        assert_eq!(w.progress(), 0);
        assert_eq!(w.completed_count(), 0);
        assert_eq!(w.remaining_count(), 0);
    }

    #[test]
    fn new_creates_active_workflow() {
        let steps = vec![
            WorkflowStep {
                index: 1,
                action: "a".into(),
                description: "first".into(),
                completed: false,
            },
            WorkflowStep {
                index: 2,
                action: "b".into(),
                description: "second".into(),
                completed: false,
            },
        ];
        let w = RuntimeWorkflow::new(WorkflowOperation::Alignment, steps);
        assert!(w.is_active());
        assert!(!w.is_complete());
        assert_eq!(w.current_step, 1);
        assert_eq!(w.total_steps, 2);
        assert_eq!(w.progress(), 0);
        assert_eq!(w.remaining_count(), 2);
    }

    #[test]
    fn progress_calculation() {
        let w = RuntimeWorkflow {
            operation: WorkflowOperation::FileCreation,
            current_step: 3,
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
        assert_eq!(w.completed_count(), 2);
        assert_eq!(w.remaining_count(), 2);
    }

    #[test]
    fn is_complete_when_all_steps_done() {
        let w = RuntimeWorkflow {
            operation: WorkflowOperation::Commit,
            current_step: 2,
            total_steps: 2,
            started_at: String::new(),
            steps: vec![
                WorkflowStep { index: 1, action: "a".into(), description: "".into(), completed: true },
                WorkflowStep { index: 2, action: "b".into(), description: "".into(), completed: true },
            ],
        };
        assert!(w.is_complete());
        assert_eq!(w.progress(), 100);
    }

    #[test]
    fn display_idle() {
        let w = RuntimeWorkflow::default();
        assert_eq!(w.to_string(), "[idle]");
    }

    #[test]
    fn display_active() {
        let w = RuntimeWorkflow {
            operation: WorkflowOperation::Alignment,
            current_step: 2,
            total_steps: 4,
            started_at: String::new(),
            steps: vec![
                WorkflowStep { index: 1, action: "a".into(), description: "".into(), completed: true },
                WorkflowStep { index: 2, action: "b".into(), description: "".into(), completed: false },
                WorkflowStep { index: 3, action: "c".into(), description: "".into(), completed: false },
                WorkflowStep { index: 4, action: "d".into(), description: "".into(), completed: false },
            ],
        };
        assert_eq!(w.to_string(), "[alignment] step 2/4 (25%)");
    }

    #[test]
    fn workflow_serde_roundtrip() {
        let w = RuntimeWorkflow::new(
            WorkflowOperation::BlockUpdate,
            vec![WorkflowStep {
                index: 1,
                action: "lint".into(),
                description: "Run linter".into(),
                completed: false,
            }],
        );
        let json = serde_json::to_string(&w).unwrap();
        let restored: RuntimeWorkflow = serde_json::from_str(&json).unwrap();
        assert_eq!(restored.operation, WorkflowOperation::BlockUpdate);
        assert_eq!(restored.total_steps, 1);
        assert_eq!(restored.steps.len(), 1);
    }
}

// ──────────────────────────────────────────────────────────────────────────
// Ce — Closing Execution
// ──────────────────────────────────────────────────────────────────────────
//
// Library module — no entry point.

// ──────────────────────────────────────────────────────────────────────────
// Cc — Closing Cleanup
// ──────────────────────────────────────────────────────────────────────────
//
// Serde types — no resources to clean up.

//
// ──────────────────────────────────────────────────────────────────────────
// X1: Modification Policy
// ──────────────────────────────────────────────────────────────────────────
//
// Safe to Modify:
//   - Add new WorkflowOperation variants (additive)
//   - Add new query methods on RuntimeWorkflow
//   - Add tests
//
// Modify with Care:
//   - Existing variant serde names — breaks JSON compatibility
//   - WorkflowStep fields — breaks construction
//   - RuntimeWorkflow fields — breaks deserialization
//
// Never Modify:
//   - WorkflowOperation::None as default

// ──────────────────────────────────────────────────────────────────────────
// X5: Closing Note
// ──────────────────────────────────────────────────────────────────────────
//
// Generic workflow types — extracted from L3, usable by all layers.
// "A man's heart deviseth his way: but the LORD directeth his steps."
// — Proverbs 16:9

// ============================================================================
// END CLOSING
// ============================================================================
