//omni:code --rust -library
//omni:key B-L3-types
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L3-types
// Purpose: CPI-SI specific types — runtime state, choice tracking, impact pipeline
// Biblical: Genesis 1:1 — "In the beginning God created the heaven and the earth"
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-16
//
// Port of L3-cpisi/hybrid/foundation/types/types.go to Rust.
// Runtime state, ternary key values, choice tracking, and impact scoring.
//
// Dependencies flow DOWN only: L3 -> L0
//   L3 types depend on L0 workflow types (re-exported here for convenience).
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

use serde::{Deserialize, Serialize};

// ────────────────────────────────────────────────────────────────
// S.1 Re-exports from L0 Foundation
// ────────────────────────────────────────────────────────────────

/// Generic workflow types live in L0 — re-exported here so existing L3
/// consumers don't need import changes. Import from L0 for new code.
pub use bereshit_l0_foundation::workflow::{RuntimeWorkflow, WorkflowOperation, WorkflowStep};

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Runtime State Types
// ────────────────────────────────────────────────────────────────

/// RuntimeState represents current session state.
/// Keys reference TOML entries, dynamic data lives here.
#[derive(Clone, Debug, Serialize, Deserialize, Default)]
pub struct RuntimeState {
    #[serde(rename = "$schema", default, skip_serializing_if = "String::is_empty")]
    pub schema: String,
    pub version: String,

    // TOML references (keys into L2 vocabulary)
    pub anchor_key: String,
    pub trajectory_section: String,
    pub command_key: String,

    // Dynamic session data
    pub session: RuntimeSession,
    pub trajectory_metrics: RuntimeTrajectoryMetrics,
    pub last_transition: RuntimeTransition,
}

/// RuntimeSession holds session-specific dynamic values.
#[derive(Clone, Debug, Serialize, Deserialize, Default)]
pub struct RuntimeSession {
    pub id: String,
    pub initialized_at: String,
    pub last_update: String,
    pub path_length: i32,
    pub hooks_fired: i32,
    pub active: bool,
    /// -100 to +100 ternary scale
    pub health_score: f64,

    // Hebrew state tracking (7 Days of Creation -> 7 states)
    /// shavar, chaser, ratsah, yashar, tamim, shalem, tov
    pub hebrew_state: String,
    /// BROKEN, LACKING, WANTING, EVEN, SOUND, WHOLE, PERFECT
    pub hebrew_meaning: String,

    // K:MORAL compass (Proverbs 3:5-6)
    /// Current choice: -1 (self), 0 (undecided), +1 (God)
    pub k_selector: i32,
    /// -1.0 to +1.0 alignment to Scripture
    pub k_align: f64,

    // K:FREQUENCY -- Pattern of choices
    pub k_toward_god: i32,
    pub k_toward_self: i32,

    // Task tracking (TodoWrite integration)
    pub tasks: RuntimeTaskList,

    // Mental Construct Choice Tracking
    pub last_key_chosen: i32,
    pub last_key_weight: f64,
    pub choice_outcome: String,
    pub choice_timestamp: String,

    // Impact Pipeline
    pub last_true_score: f64,
    pub last_normalized: f64,
    pub last_scaled: i32,
    pub last_feedback: String,

    // HALT Tracking
    pub last_halt_reached: bool,
    pub last_halt_type: String,
    pub last_halt_trigger: String,

    // Choice sequence counter
    pub choice_sequence: i32,

    // Workflow tracking (operational layer)
    pub workflow: RuntimeWorkflow,

    // CPI Tracking (Covenant Partnership Intelligence)
    // "By their fruits ye shall know them" -- Matthew 7:20
    pub exchange_count: i32,
    pub insight_count: i32,
    /// 0.0-1.0 relationship quality
    pub cpi_score: f64,
    pub dominant_exchange_type: String,
    pub session_arc: String,
    pub last_exchange_type: String,
    pub last_insight_type: String,

    // Context Window Tracking
    // "A time to keep, and a time to cast away" -- Ecclesiastes 3:6
    pub base_context_tokens: i32,
    pub current_context_tokens: i32,
    pub peak_context_tokens: i32,
    pub compaction_count: i32,
    pub effective_context_window: i32,

    // CPI-SI Family Member Tracking
    // "The household of faith" -- Galatians 6:10
    pub active_family_member: String,
    pub family_invocation_count: i32,
}

/// RuntimeTaskItem represents a single task from TodoWrite.
#[derive(Clone, Debug, Serialize, Deserialize, Default)]
pub struct RuntimeTaskItem {
    pub id: String,
    pub subject: String,
    /// pending, in_progress, completed
    pub status: String,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    pub owner: String,
}

/// RuntimeTaskList tracks TodoWrite state with actual task data.
/// "Whatsoever thy hand findeth to do, do it with thy might" -- Ecclesiastes 9:10
#[derive(Clone, Debug, Serialize, Deserialize, Default)]
pub struct RuntimeTaskList {
    pub total: i32,
    pub pending: i32,
    pub in_progress: i32,
    pub completed: i32,

    /// Active tasks (in_progress) -- for statusline display
    #[serde(default, skip_serializing_if = "Vec::is_empty")]
    pub active_tasks: Vec<RuntimeTaskItem>,
}

/// RuntimeTrajectoryMetrics tracks time spent in each trajectory section.
/// "There is a time for every purpose under heaven" -- Ecclesiastes 3:1
#[derive(Clone, Debug, Serialize, Deserialize, Default)]
pub struct RuntimeTrajectoryMetrics {
    #[serde(rename = "B.1_time_ms")]
    pub b1_time_ms: i32,
    #[serde(rename = "B.2_time_ms")]
    pub b2_time_ms: i32,
    #[serde(rename = "B.3_time_ms")]
    pub b3_time_ms: i32,
    #[serde(rename = "B.4_time_ms")]
    pub b4_time_ms: i32,
    pub pivot_count: i32,
    pub reset_count: i32,

    // Momentum tracking for trajectory advancement
    // "Precept upon precept, line upon line" -- Isaiah 28:10
    pub accumulated_work_ms: i32,
    pub momentum_score: i32,
}

/// RuntimeTransition records the most recent state transition.
#[derive(Clone, Debug, Serialize, Deserialize, Default)]
pub struct RuntimeTransition {
    pub from_section: String,
    pub to_section: String,
    pub trigger: String,
    pub timestamp: String,
}

// ────────────────────────────────────────────────────────────────
// B.2 Choice Types
// ────────────────────────────────────────────────────────────────

/// KeyValue represents the ternary key choice.
///
/// Maps to Go's `KeyValue` int type with constants:
///   -1 = Finality (toward foundation, completion, or retreat)
///    0 = Lateral  (maintain, hold position, awaiting)
///   +1 = Expansion (toward God, growth, building)
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq, Serialize, Deserialize)]
pub enum KeyValue {
    /// Toward foundation, completion, or retreat (-1)
    #[serde(rename = "-1")]
    Finality,
    /// Maintain, hold position, awaiting (0)
    #[serde(rename = "0")]
    #[default]
    Lateral,
    /// Toward God, growth, building (+1)
    #[serde(rename = "1")]
    Expansion,
}

impl KeyValue {
    /// Returns the symbol for display.
    pub fn symbol(&self) -> &str {
        match self {
            Self::Expansion => "+",
            Self::Lateral => "\u{25C6}", // diamond
            Self::Finality => "-",
        }
    }

    /// Returns valid operators for this key.
    pub fn operators(&self) -> Vec<&str> {
        match self {
            Self::Expansion => vec!["+", "->", ">"],
            Self::Lateral => vec!["@", "|", "="],
            Self::Finality => vec!["<-", "-", "<"],
        }
    }

    /// Integer value matching Go's KeyValue constants.
    pub fn as_int(&self) -> i32 {
        match self {
            Self::Finality => -1,
            Self::Lateral => 0,
            Self::Expansion => 1,
        }
    }
}

/// HaltType represents whether a HALT was reached and what kind.
#[derive(Clone, Debug, Default, PartialEq, Eq, Serialize, Deserialize)]
pub enum HaltType {
    /// No HALT reached, work continues
    #[serde(rename = "none")]
    #[default]
    None,
    /// Anchor reached, choice validated
    #[serde(rename = "completed")]
    Completed,
    /// Checkpoint, verdict pending
    #[serde(rename = "interrupted")]
    Interrupted,
}

/// Tendency represents the direction of moral alignment.
#[derive(Clone, Debug, Default, PartialEq, Eq, Serialize, Deserialize)]
pub enum Tendency {
    #[serde(rename = "God")]
    God,
    #[serde(rename = "self")]
    Self_,
    #[serde(rename = "neutral")]
    #[default]
    Neutral,
}

/// ChoiceContext captures the context BEFORE outcome is known.
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct ChoiceContext {
    pub intended_key: KeyValue,
    pub key_symbol: String,
    pub key_operators: Vec<String>,
    pub for_what: String,
    pub intent: String,
    pub prompt_type: String,
    pub position_at_choice: String,
    pub k_at_choice: i32,
    pub z_at_choice: f64,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    pub tool: String,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    pub tool_input: String,
    pub timestamp: String,
}

impl Default for ChoiceContext {
    fn default() -> Self {
        Self {
            intended_key: KeyValue::default(),
            key_symbol: String::new(),
            key_operators: Vec::new(),
            for_what: String::new(),
            intent: String::new(),
            prompt_type: String::new(),
            position_at_choice: String::new(),
            k_at_choice: 0,
            z_at_choice: 0.0,
            tool: String::new(),
            tool_input: String::new(),
            timestamp: String::new(),
        }
    }
}

/// ChoiceResult captures what happened after the choice was made.
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct ChoiceResult {
    pub led_to: String,
    pub tool_outcome: String,
    pub halt_reached: bool,
    pub halt_type: HaltType,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    pub halt_trigger: String,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    pub halt_result: String,
    pub timestamp: String,
}

impl Default for ChoiceResult {
    fn default() -> Self {
        Self {
            led_to: String::new(),
            tool_outcome: String::new(),
            halt_reached: false,
            halt_type: HaltType::None,
            halt_trigger: String::new(),
            halt_result: String::new(),
            timestamp: String::new(),
        }
    }
}

/// ChoicePattern captures the pattern aspect.
#[derive(Clone, Debug, Serialize, Deserialize, Default)]
pub struct ChoicePattern {
    pub trigger: String,
    pub related_to: String,
    pub reasoning: String,
}

/// ChoiceTendency captures the tendency aspect.
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct ChoiceTendency {
    pub leads_toward: Tendency,
    pub tendency_score: f64,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    pub scripture_alignment: String,
}

impl Default for ChoiceTendency {
    fn default() -> Self {
        Self {
            leads_toward: Tendency::Neutral,
            tendency_score: 0.0,
            scripture_alignment: String::new(),
        }
    }
}

/// ImpactScores captures the full impact pipeline.
#[derive(Clone, Debug, Serialize, Deserialize, Default)]
pub struct ImpactScores {
    pub true_score: f64,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    pub feedback_word: String,
    #[serde(default, skip_serializing_if = "String::is_empty")]
    pub feedback_tier: String,
    pub normalized: f64,
    pub scaled: i32,
    pub k_align_delta: f64,
    pub k_align_before: f64,
    pub k_align_after: f64,
}

/// ChoiceRecord is the complete record of a choice.
#[derive(Clone, Debug, Serialize, Deserialize, Default)]
pub struct ChoiceRecord {
    pub id: String,
    pub context: ChoiceContext,
    pub result: ChoiceResult,
    pub pattern: ChoicePattern,
    pub tendency: ChoiceTendency,
    pub impact: ImpactScores,
    pub session_id: String,
    pub sequence_num: i32,
}

/// ChoiceSummary provides aggregated choice statistics.
#[derive(Clone, Debug, Serialize, Deserialize, Default)]
pub struct ChoiceSummary {
    pub total_choices: i32,
    pub expansion_count: i32,
    pub lateral_count: i32,
    pub finality_count: i32,
    pub completed_halts: i32,
    pub interrupted_halts: i32,
    pub average_true_score: f64,
    pub final_k_align: f64,
    pub tendency_toward_god: i32,
    pub tendency_toward_self: i32,
}

/// ChoiceHistory tracks all choices in a session.
#[derive(Clone, Debug, Default, Serialize, Deserialize)]
pub struct ChoiceHistory {
    pub session_id: String,
    pub started_at: String,
    pub last_updated: String,
    pub choices: Vec<ChoiceRecord>,
    pub summary: ChoiceSummary,
}

// ────────────────────────────────────────────────────────────────
// B.3 Constructors
// ────────────────────────────────────────────────────────────────

/// Creates a new choice context with key, purpose, and intent.
/// Timestamp is set to the provided value (caller supplies ISO 8601 string).
pub fn new_choice_context(key: KeyValue, for_what: &str, intent: &str) -> ChoiceContext {
    ChoiceContext {
        intended_key: key,
        key_symbol: key.symbol().to_string(),
        key_operators: key.operators().iter().map(|s| s.to_string()).collect(),
        for_what: for_what.to_string(),
        intent: intent.to_string(),
        ..ChoiceContext::default()
    }
}

/// Creates a result for a completed choice.
pub fn new_choice_result(led_to: &str, halt_type: HaltType, tool_outcome: &str) -> ChoiceResult {
    ChoiceResult {
        led_to: led_to.to_string(),
        tool_outcome: tool_outcome.to_string(),
        halt_reached: halt_type != HaltType::None,
        halt_type,
        ..ChoiceResult::default()
    }
}

/// Creates a new choice record (before outcome).
pub fn new_choice_record(session_id: &str, seq_num: i32, ctx: &ChoiceContext) -> ChoiceRecord {
    ChoiceRecord {
        id: generate_choice_id(session_id, seq_num),
        context: ctx.clone(),
        session_id: session_id.to_string(),
        sequence_num: seq_num,
        ..ChoiceRecord::default()
    }
}

/// Creates a new history for a session.
pub fn new_choice_history(session_id: &str) -> ChoiceHistory {
    ChoiceHistory {
        session_id: session_id.to_string(),
        ..ChoiceHistory::default()
    }
}

/// Creates impact scores from a TrueScore.
///
/// The normalization math:
///   normalized = (true_score * 2) - 1       maps [0,1] -> [-1,+1]
///   scaled     = normalized * 100            maps to integer percentage
///   delta      = normalized * weight         where weight = 0.05 (lateral) or 0.1 (directed)
///   new_k_align = (current * 0.9) + delta    exponential moving average, clamped to [-1,+1]
pub fn new_impact_scores(
    true_score: f64,
    key_direction: i32,
    current_k_align: f64,
) -> ImpactScores {
    let normalized = (true_score * 2.0) - 1.0;
    let scaled = (normalized * 100.0) as i32;

    let delta = if key_direction == 0 {
        normalized * 0.05
    } else {
        normalized * 0.1
    };

    let new_k_align = ((current_k_align * 0.9) + delta).clamp(-1.0, 1.0);

    ImpactScores {
        true_score,
        normalized,
        scaled,
        k_align_delta: delta,
        k_align_before: current_k_align,
        k_align_after: new_k_align,
        ..ImpactScores::default()
    }
}

// ────────────────────────────────────────────────────────────────
// B.4 Methods
// ────────────────────────────────────────────────────────────────

impl ChoiceHistory {
    /// Adds a choice to the history.
    pub fn add_choice(&mut self, choice: ChoiceRecord) {
        self.choices.push(choice);
        self.update_summary();
    }

    /// Returns the most recent choice, or None if empty.
    pub fn get_last_choice(&self) -> Option<&ChoiceRecord> {
        self.choices.last()
    }

    /// Recalculates summary statistics from all choices.
    pub fn update_summary(&mut self) {
        self.summary = ChoiceSummary::default();
        let mut score_sum: f64 = 0.0;

        for c in &self.choices {
            self.summary.total_choices += 1;

            match c.context.intended_key {
                KeyValue::Expansion => self.summary.expansion_count += 1,
                KeyValue::Lateral => self.summary.lateral_count += 1,
                KeyValue::Finality => self.summary.finality_count += 1,
            }

            match c.result.halt_type {
                HaltType::Completed => self.summary.completed_halts += 1,
                HaltType::Interrupted => self.summary.interrupted_halts += 1,
                HaltType::None => {}
            }

            score_sum += c.impact.true_score;

            match c.tendency.leads_toward {
                Tendency::God => self.summary.tendency_toward_god += 1,
                Tendency::Self_ => self.summary.tendency_toward_self += 1,
                Tendency::Neutral => {}
            }

            self.summary.final_k_align = c.impact.k_align_after;
        }

        if self.summary.total_choices > 0 {
            self.summary.average_true_score = score_sum / f64::from(self.summary.total_choices);
        }
    }
}

impl ChoiceRecord {
    /// Fills in the result and calculates impact.
    ///
    /// When halt_type is Completed, runs the full impact pipeline with
    /// exponential moving average on k_align. Otherwise preserves current
    /// k_align with a neutral tendency.
    pub fn complete(&mut self, result: &ChoiceResult, true_score: f64, current_k_align: f64) {
        self.result = result.clone();

        if result.halt_type == HaltType::Completed {
            let key_dir = self.context.intended_key.as_int();
            self.impact = new_impact_scores(true_score, key_dir, current_k_align);

            if self.impact.k_align_after > 0.1 {
                self.tendency.leads_toward = Tendency::God;
            } else if self.impact.k_align_after < -0.1 {
                self.tendency.leads_toward = Tendency::Self_;
            } else {
                self.tendency.leads_toward = Tendency::Neutral;
            }
            self.tendency.tendency_score = self.impact.k_align_after;
        } else {
            let normalized = (true_score * 2.0) - 1.0;
            self.impact = ImpactScores {
                true_score,
                normalized,
                scaled: (normalized * 100.0) as i32,
                k_align_before: current_k_align,
                k_align_after: current_k_align,
                ..ImpactScores::default()
            };
            self.tendency.leads_toward = Tendency::Neutral;
        }
    }

    /// Adds pattern information to the record.
    pub fn set_pattern(&mut self, trigger: &str, related_to: &str, reasoning: &str) {
        self.pattern = ChoicePattern {
            trigger: trigger.to_string(),
            related_to: related_to.to_string(),
            reasoning: reasoning.to_string(),
        };
    }
}

// ────────────────────────────────────────────────────────────────
// B.5 Helpers
// ────────────────────────────────────────────────────────────────

/// Generates a choice ID from session ID and sequence number.
fn generate_choice_id(session_id: &str, seq_num: i32) -> String {
    format!("{session_id}-choice-{seq_num}")
}

// ============================================================================
// CLOSING
// ============================================================================
//
// This is the L3 CPI-SI type foundation — ported from Go to Rust.
// All runtime state, choice tracking, and impact scoring types live here.
//
// Architecture mirrors the Go layer hierarchy:
//   L0: foundation (workflow types)  <-- re-exported here
//   L3: types (this crate)           <-- CPI-SI specific
//
// "In the beginning God created the heaven and the earth." -- Genesis 1:1
// Types are the foundation upon which all else is built.

#[cfg(test)]
mod tests {
    use super::*;

    // ────────────────────────────────────────────────────────────
    // T.1 KeyValue symbol and operators
    // ────────────────────────────────────────────────────────────

    #[test]
    fn key_value_symbol() {
        assert_eq!(KeyValue::Expansion.symbol(), "+");
        assert_eq!(KeyValue::Lateral.symbol(), "\u{25C6}");
        assert_eq!(KeyValue::Finality.symbol(), "-");
    }

    #[test]
    fn key_value_operators() {
        let exp_ops = KeyValue::Expansion.operators();
        assert_eq!(exp_ops, vec!["+", "->", ">"]);

        let lat_ops = KeyValue::Lateral.operators();
        assert_eq!(lat_ops, vec!["@", "|", "="]);

        let fin_ops = KeyValue::Finality.operators();
        assert_eq!(fin_ops, vec!["<-", "-", "<"]);
    }

    #[test]
    fn key_value_as_int() {
        assert_eq!(KeyValue::Finality.as_int(), -1);
        assert_eq!(KeyValue::Lateral.as_int(), 0);
        assert_eq!(KeyValue::Expansion.as_int(), 1);
    }

    // ────────────────────────────────────────────────────────────
    // T.2 Constructor tests
    // ────────────────────────────────────────────────────────────

    #[test]
    fn new_choice_context_populates_key_fields() {
        let ctx = new_choice_context(KeyValue::Expansion, "build crate", "grow the system");
        assert_eq!(ctx.intended_key, KeyValue::Expansion);
        assert_eq!(ctx.key_symbol, "+");
        assert_eq!(ctx.key_operators, vec!["+", "->", ">"]);
        assert_eq!(ctx.for_what, "build crate");
        assert_eq!(ctx.intent, "grow the system");
    }

    #[test]
    fn new_choice_result_halt_flag() {
        let r1 = new_choice_result("success", HaltType::Completed, "file created");
        assert!(r1.halt_reached);
        assert_eq!(r1.halt_type, HaltType::Completed);

        let r2 = new_choice_result("continue", HaltType::None, "ongoing");
        assert!(!r2.halt_reached);
        assert_eq!(r2.halt_type, HaltType::None);
    }

    // ────────────────────────────────────────────────────────────
    // T.3 ImpactScores normalization math
    // ────────────────────────────────────────────────────────────

    #[test]
    fn impact_scores_normalization() {
        // true_score=0.8, key_direction=1 (expansion), current_k_align=0.0
        let impact = new_impact_scores(0.8, 1, 0.0);

        // normalized = (0.8 * 2) - 1 = 0.6
        let expected_norm = 0.6_f64;
        assert!((impact.normalized - expected_norm).abs() < 1e-10);

        // scaled = 0.6 * 100 = 60
        assert_eq!(impact.scaled, 60);

        // delta = 0.6 * 0.1 = 0.06  (key_direction != 0)
        let expected_delta = 0.06_f64;
        assert!((impact.k_align_delta - expected_delta).abs() < 1e-10);

        // new_k_align = (0.0 * 0.9) + 0.06 = 0.06
        assert!((impact.k_align_after - 0.06).abs() < 1e-10);
        assert!((impact.k_align_before - 0.0).abs() < 1e-10);
    }

    #[test]
    fn impact_scores_lateral_weight() {
        // Lateral direction uses 0.05 weight instead of 0.1
        let impact = new_impact_scores(1.0, 0, 0.5);

        // normalized = (1.0 * 2) - 1 = 1.0
        assert!((impact.normalized - 1.0).abs() < 1e-10);

        // delta = 1.0 * 0.05 = 0.05  (key_direction == 0)
        assert!((impact.k_align_delta - 0.05).abs() < 1e-10);

        // new_k_align = (0.5 * 0.9) + 0.05 = 0.45 + 0.05 = 0.50
        assert!((impact.k_align_after - 0.50).abs() < 1e-10);
    }

    #[test]
    fn impact_scores_clamps_to_bounds() {
        // Push k_align past +1.0
        let impact = new_impact_scores(1.0, 1, 0.99);
        // new_k_align = (0.99 * 0.9) + (1.0 * 0.1) = 0.891 + 0.1 = 0.991
        assert!(impact.k_align_after <= 1.0);
        assert!(impact.k_align_after >= -1.0);

        // Push k_align past -1.0
        let impact_neg = new_impact_scores(0.0, 1, -0.99);
        // normalized = -1.0, delta = -0.1
        // new_k_align = (-0.99 * 0.9) + (-0.1) = -0.891 + (-0.1) = -0.991
        assert!(impact_neg.k_align_after >= -1.0);
        assert!(impact_neg.k_align_after <= 1.0);
    }

    // ────────────────────────────────────────────────────────────
    // T.4 ChoiceHistory add and get
    // ────────────────────────────────────────────────────────────

    #[test]
    fn choice_history_add_and_get() {
        let mut history = new_choice_history("session-001");
        assert!(history.get_last_choice().is_none());

        let ctx = new_choice_context(KeyValue::Expansion, "test", "testing");
        let record = new_choice_record("session-001", 1, &ctx);
        history.add_choice(record);

        assert_eq!(history.choices.len(), 1);
        let last = history.get_last_choice().unwrap();
        assert_eq!(last.id, "session-001-choice-1");
        assert_eq!(last.sequence_num, 1);
        assert_eq!(history.summary.total_choices, 1);
        assert_eq!(history.summary.expansion_count, 1);
    }

    #[test]
    fn choice_history_summary_counts() {
        let mut history = new_choice_history("session-002");

        // Add two expansion, one lateral, one finality
        for (i, key) in [
            KeyValue::Expansion,
            KeyValue::Expansion,
            KeyValue::Lateral,
            KeyValue::Finality,
        ]
        .iter()
        .enumerate()
        {
            let ctx = new_choice_context(*key, "task", "intent");
            let record = new_choice_record("session-002", (i + 1) as i32, &ctx);
            history.add_choice(record);
        }

        assert_eq!(history.summary.total_choices, 4);
        assert_eq!(history.summary.expansion_count, 2);
        assert_eq!(history.summary.lateral_count, 1);
        assert_eq!(history.summary.finality_count, 1);
    }

    // ────────────────────────────────────────────────────────────
    // T.5 ChoiceRecord complete and set_pattern
    // ────────────────────────────────────────────────────────────

    #[test]
    fn choice_record_complete_with_halt() {
        let ctx = new_choice_context(KeyValue::Expansion, "build", "grow");
        let mut record = new_choice_record("s1", 1, &ctx);

        let result = new_choice_result("success", HaltType::Completed, "file written");
        record.complete(&result, 0.8, 0.0);

        assert!(record.result.halt_reached);
        assert_eq!(record.result.halt_type, HaltType::Completed);
        // k_align_after should be positive (0.06) -> tendency toward God
        assert!(record.impact.k_align_after > 0.0);
        assert_eq!(record.tendency.leads_toward, Tendency::Neutral);
        // 0.06 < 0.1 threshold so it stays Neutral
    }

    #[test]
    fn choice_record_complete_without_halt() {
        let ctx = new_choice_context(KeyValue::Lateral, "hold", "wait");
        let mut record = new_choice_record("s2", 1, &ctx);

        let result = new_choice_result("ongoing", HaltType::None, "continuing");
        record.complete(&result, 0.5, 0.3);

        // Without completed halt, k_align should stay at current value
        assert!((record.impact.k_align_after - 0.3).abs() < 1e-10);
        assert_eq!(record.tendency.leads_toward, Tendency::Neutral);
    }

    #[test]
    fn choice_record_set_pattern() {
        let ctx = new_choice_context(KeyValue::Expansion, "code", "write");
        let mut record = new_choice_record("s3", 1, &ctx);

        record.set_pattern("user request", "crate creation", "building types layer");

        assert_eq!(record.pattern.trigger, "user request");
        assert_eq!(record.pattern.related_to, "crate creation");
        assert_eq!(record.pattern.reasoning, "building types layer");
    }

    // ────────────────────────────────────────────────────────────
    // T.6 Serialization roundtrip
    // ────────────────────────────────────────────────────────────

    #[test]
    fn serialization_roundtrip_runtime_state() {
        let state = RuntimeState {
            version: "1.0.0".to_string(),
            anchor_key: "genesis".to_string(),
            ..RuntimeState::default()
        };

        let json = serde_json::to_string(&state).expect("serialize");
        let deserialized: RuntimeState = serde_json::from_str(&json).expect("deserialize");

        assert_eq!(deserialized.version, "1.0.0");
        assert_eq!(deserialized.anchor_key, "genesis");
    }

    #[test]
    fn serialization_roundtrip_choice_record() {
        let ctx = new_choice_context(KeyValue::Expansion, "build", "grow");
        let mut record = new_choice_record("sess-1", 5, &ctx);
        record.set_pattern("trigger", "related", "reason");

        let json = serde_json::to_string(&record).expect("serialize");
        let deserialized: ChoiceRecord = serde_json::from_str(&json).expect("deserialize");

        assert_eq!(deserialized.id, "sess-1-choice-5");
        assert_eq!(deserialized.context.intended_key, KeyValue::Expansion);
        assert_eq!(deserialized.pattern.trigger, "trigger");
    }

    #[test]
    fn serialization_halt_type_values() {
        let json_none = serde_json::to_string(&HaltType::None).unwrap();
        assert_eq!(json_none, "\"none\"");

        let json_completed = serde_json::to_string(&HaltType::Completed).unwrap();
        assert_eq!(json_completed, "\"completed\"");

        let json_interrupted = serde_json::to_string(&HaltType::Interrupted).unwrap();
        assert_eq!(json_interrupted, "\"interrupted\"");
    }

    #[test]
    fn serialization_tendency_values() {
        let json_god = serde_json::to_string(&Tendency::God).unwrap();
        assert_eq!(json_god, "\"God\"");

        let json_self = serde_json::to_string(&Tendency::Self_).unwrap();
        assert_eq!(json_self, "\"self\"");

        let json_neutral = serde_json::to_string(&Tendency::Neutral).unwrap();
        assert_eq!(json_neutral, "\"neutral\"");
    }

    #[test]
    fn serialization_trajectory_metrics_field_names() {
        let metrics = RuntimeTrajectoryMetrics {
            b1_time_ms: 100,
            b2_time_ms: 200,
            ..RuntimeTrajectoryMetrics::default()
        };

        let json = serde_json::to_string(&metrics).expect("serialize");
        // Go JSON tags use "B.1_time_ms" etc.
        assert!(json.contains("\"B.1_time_ms\""));
        assert!(json.contains("\"B.2_time_ms\""));
    }
}
