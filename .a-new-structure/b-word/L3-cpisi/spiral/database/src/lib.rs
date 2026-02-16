//omni:code --rust -library
//omni:key B-L3-database
//omni:version a-01.00

// ============================================================================
// METADATA
// ============================================================================

//! # CPI-SI Domain Database Types and Traits
//!
//! Key: B-L3-database
//! Purpose: CPI-SI domain database types and traits
//! Biblical: Nehemiah 3 - "Then Eliashib the high priest rose up with his
//!   brethren the priests, and they builded the sheep gate"
//! Authors: Nova Dawn
//!
//! Domain-specific repository traits for the five CPI-SI databases:
//! sessions, cognition, growth, temporal, and projects.
//!
//! Each module defines the domain types (with Serialize/Deserialize) and
//! a `Repository` trait specifying the core persistence operations.
//!
//! These traits are implemented by concrete backends (SQLite, etc.) in
//! their respective crates.

// ============================================================================
// SETUP
// ============================================================================

// Re-export the platform database layer for implementors.
pub use bereshit_l2_database;

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Sessions Domain
// ────────────────────────────────────────────────────────────────

pub mod sessions {
    use serde::{Deserialize, Serialize};

    /// A work session capturing the full lifecycle from start to end,
    /// including Hebrew state transitions, CPI scoring, and narrative.
    #[derive(Debug, Clone, Serialize, Deserialize)]
    pub struct Session {
        pub id: String,
        pub started_at: String,
        pub ended_at: Option<String>,

        pub project_path: String,
        pub workspace: String,

        pub initial_hebrew_state: String,
        pub initial_k_align: f64,
        pub initial_cube_position: String,
        pub final_hebrew_state: String,
        pub final_k_align: f64,
        pub final_cube_position: String,

        pub tool_count: i32,
        pub choice_count: i32,
        pub day_of_week: i32,
        pub hour_of_day: i32,

        pub exchange_count: i32,
        pub insight_count: i32,
        pub cpi_score: f64,
        pub dominant_exchange_type: String,
        pub session_arc: String,
        pub narrative_summary: String,
        pub quality_rating: f64,
    }

    impl Default for Session {
        fn default() -> Self {
            Self {
                id: String::new(),
                started_at: String::new(),
                ended_at: None,
                project_path: String::new(),
                workspace: String::new(),
                initial_hebrew_state: String::new(),
                initial_k_align: 0.0,
                initial_cube_position: String::new(),
                final_hebrew_state: String::new(),
                final_k_align: 0.0,
                final_cube_position: String::new(),
                tool_count: 0,
                choice_count: 0,
                day_of_week: 0,
                hour_of_day: 0,
                exchange_count: 0,
                insight_count: 0,
                cpi_score: 0.0,
                dominant_exchange_type: String::new(),
                session_arc: String::new(),
                narrative_summary: String::new(),
                quality_rating: 0.0,
            }
        }
    }

    /// A single prompt/response exchange within a session.
    #[derive(Debug, Clone, Serialize, Deserialize)]
    pub struct Exchange {
        pub id: i64,
        pub session_id: String,
        pub timestamp: String,
        pub sequence_num: i32,
        pub exchange_type: String,
        pub initiative: String,
        pub prompt_length: i32,
        pub feedback_detected: bool,
        pub feedback_polarity: String,
        pub feedback_categories: String,
        pub value_contribution: f64,
        pub depth_level: String,
        pub insight_detected: bool,
        pub insight_type: String,
        pub hebrew_state: String,
        pub k_align: f64,
        pub trajectory: String,

        // Rich data (v2)
        pub health_score: Option<i32>,
        pub context_tokens: Option<i32>,

        // Message text (v3)
        pub user_message_text: String,
        pub response_summary: String,
        pub valence: String,

        // Cognition (v4)
        pub thinking_text: String,
    }

    impl Default for Exchange {
        fn default() -> Self {
            Self {
                id: 0,
                session_id: String::new(),
                timestamp: String::new(),
                sequence_num: 0,
                exchange_type: String::new(),
                initiative: String::new(),
                prompt_length: 0,
                feedback_detected: false,
                feedback_polarity: String::new(),
                feedback_categories: String::new(),
                value_contribution: 0.0,
                depth_level: String::new(),
                insight_detected: false,
                insight_type: String::new(),
                hebrew_state: String::new(),
                k_align: 0.0,
                trajectory: String::new(),
                health_score: None,
                context_tokens: None,
                user_message_text: String::new(),
                response_summary: String::new(),
                valence: String::new(),
                thinking_text: String::new(),
            }
        }
    }

    /// A moment of understanding transfer detected during an exchange.
    #[derive(Debug, Clone, Serialize, Deserialize)]
    pub struct Insight {
        pub id: i64,
        pub session_id: String,
        pub exchange_id: Option<i64>,
        pub timestamp: String,
        pub insight_type: String,
        pub confidence: f64,
        pub topic_area: String,
    }

    impl Default for Insight {
        fn default() -> Self {
            Self {
                id: 0,
                session_id: String::new(),
                exchange_id: None,
                timestamp: String::new(),
                insight_type: String::new(),
                confidence: 0.0,
                topic_area: String::new(),
            }
        }
    }

    /// Time gap between consecutive sessions.
    #[derive(Debug, Clone, Serialize, Deserialize)]
    pub struct SessionGap {
        pub session_id: String,
        pub session_end: String,
        pub next_session_start: String,
        pub gap_hours: f64,
    }

    impl Default for SessionGap {
        fn default() -> Self {
            Self {
                session_id: String::new(),
                session_end: String::new(),
                next_session_start: String::new(),
                gap_hours: 0.0,
            }
        }
    }

    /// Repository trait for session and exchange persistence.
    pub trait SessionRepository {
        /// Create a new session record.
        fn create_session(&self, session: &Session) -> Result<(), String>;
        /// Finalize a session with its ending state.
        fn end_session(&self, session_id: &str, final_state: &Session) -> Result<(), String>;
        /// Retrieve a session by ID.
        fn get_session(&self, session_id: &str) -> Result<Option<Session>, String>;
        /// Get the currently active (un-ended) session, if any.
        fn get_active_session(&self) -> Result<Option<Session>, String>;
        /// Get the most recent sessions, up to `limit`.
        fn get_recent_sessions(&self, limit: usize) -> Result<Vec<Session>, String>;
        /// Record a prompt/response exchange.
        fn record_exchange(&self, exchange: &Exchange) -> Result<(), String>;
        /// Record an insight detected during an exchange.
        fn record_insight(&self, insight: &Insight) -> Result<(), String>;
        /// Close the repository and release resources.
        fn close(&self) -> Result<(), String>;
    }
}

// ────────────────────────────────────────────────────────────────
// B.2 Cognition Domain
// ────────────────────────────────────────────────────────────────

pub mod cognition {
    use serde::{Deserialize, Serialize};

    /// A mental construct representing a cognitive formation --
    /// a tool choice with its surrounding Hebrew state context.
    #[derive(Debug, Clone, Serialize, Deserialize)]
    pub struct MentalConstruct {
        pub id: String,
        pub session_id: String,
        pub sequence_num: i32,
        pub timestamp: String,
        pub intended_key: i32,
        pub position_at_choice: String,
        pub k_at_choice: f64,
        pub cube_position: String,
        pub tool_name: String,
        pub tool_category: String,
        pub tool_outcome: String,
        pub halt_reached: bool,
        pub halt_type: String,
        pub true_score: f64,
        pub k_align_delta: f64,
        pub k_align_after: f64,
        pub health_score: Option<i32>,
    }

    impl Default for MentalConstruct {
        fn default() -> Self {
            Self {
                id: String::new(),
                session_id: String::new(),
                sequence_num: 0,
                timestamp: String::new(),
                intended_key: 0,
                position_at_choice: String::new(),
                k_at_choice: 0.0,
                cube_position: String::new(),
                tool_name: String::new(),
                tool_category: String::new(),
                tool_outcome: String::new(),
                halt_reached: false,
                halt_type: String::new(),
                true_score: 0.0,
                k_align_delta: 0.0,
                k_align_after: 0.0,
                health_score: None,
            }
        }
    }

    /// A detected pattern in cognitive behavior across sessions.
    #[derive(Debug, Clone, Default, Serialize, Deserialize)]
    pub struct CognitionPattern {
        pub id: i64,
        pub session_id: String,
        pub from_state: String,
        pub to_state: String,
        pub trigger_type: String,
        pub trigger_detail: String,
        pub timestamp: String,
    }

    /// Repository trait for cognitive state persistence.
    pub trait CognitionRepository {
        /// Record a mental construct (tool choice with context).
        fn record_construct(&self, construct: &MentalConstruct) -> Result<(), String>;
        /// Retrieve constructs for a session.
        fn get_session_constructs(&self, session_id: &str) -> Result<Vec<MentalConstruct>, String>;
        /// Record a cognition pattern (state transition).
        fn record_pattern(&self, pattern: &CognitionPattern) -> Result<(), String>;
        /// Close the repository and release resources.
        fn close(&self) -> Result<(), String>;
    }
}

// ────────────────────────────────────────────────────────────────
// B.3 Growth Domain
// ────────────────────────────────────────────────────────────────

pub mod growth {
    use serde::{Deserialize, Serialize};

    /// A growth entry representing a detected behavioral pattern
    /// that has been promoted to a learning.
    #[derive(Debug, Clone, Serialize, Deserialize)]
    pub struct GrowthEntry {
        pub id: i64,
        pub pattern_type: String,
        pub pattern_key: String,
        pub description: String,
        pub pattern_data: String,
        pub first_seen: String,
        pub last_seen: String,
        pub occurrence_count: i32,
        pub confidence: f64,
        pub is_active: bool,
    }

    impl Default for GrowthEntry {
        fn default() -> Self {
            Self {
                id: 0,
                pattern_type: String::new(),
                pattern_key: String::new(),
                description: String::new(),
                pattern_data: String::new(),
                first_seen: String::new(),
                last_seen: String::new(),
                occurrence_count: 0,
                confidence: 0.0,
                is_active: true,
            }
        }
    }

    /// A recognized pattern that has been elevated from raw observation
    /// to a structured learning with confidence weighting.
    #[derive(Debug, Clone, Serialize, Deserialize)]
    pub struct PatternRecognition {
        pub id: i64,
        pub source_patterns: String,
        pub source_type: String,
        pub learning_type: String,
        pub learning_key: String,
        pub learning_content: String,
        pub confidence: f64,
        pub weight: f64,
        pub integrated: bool,
        pub integrated_at: Option<String>,
        pub integration_target: String,
        pub first_seen: String,
        pub last_reinforced: String,
        pub reinforcement_count: i32,
    }

    impl Default for PatternRecognition {
        fn default() -> Self {
            Self {
                id: 0,
                source_patterns: String::new(),
                source_type: String::new(),
                learning_type: String::new(),
                learning_key: String::new(),
                learning_content: String::new(),
                confidence: 0.0,
                weight: 0.0,
                integrated: false,
                integrated_at: None,
                integration_target: String::new(),
                first_seen: String::new(),
                last_reinforced: String::new(),
                reinforcement_count: 0,
            }
        }
    }

    /// Repository trait for growth and learning persistence.
    pub trait GrowthRepository {
        /// Record a growth entry (detected pattern).
        fn record_entry(&self, entry: &GrowthEntry) -> Result<(), String>;
        /// Get all active growth entries.
        fn get_active_entries(&self) -> Result<Vec<GrowthEntry>, String>;
        /// Record a pattern recognition (promoted learning).
        fn record_recognition(&self, recognition: &PatternRecognition) -> Result<(), String>;
        /// Close the repository and release resources.
        fn close(&self) -> Result<(), String>;
    }
}

// ────────────────────────────────────────────────────────────────
// B.4 Temporal Domain
// ────────────────────────────────────────────────────────────────

pub mod temporal {
    use serde::{Deserialize, Serialize};

    /// A learned work rhythm derived from session timing patterns.
    #[derive(Debug, Clone, Serialize, Deserialize)]
    pub struct WorkRhythm {
        pub id: i64,
        pub day_of_week: i32,
        pub hour_start: i32,
        pub hour_end: i32,
        pub rhythm_type: String,
        pub confidence: f64,
        pub session_count: i32,
        pub avg_duration_min: f64,
        pub avg_exchanges: f64,
        pub last_updated: String,
    }

    impl Default for WorkRhythm {
        fn default() -> Self {
            Self {
                id: 0,
                day_of_week: 0,
                hour_start: 0,
                hour_end: 0,
                rhythm_type: String::new(),
                confidence: 0.0,
                session_count: 0,
                avg_duration_min: 0.0,
                avg_exchanges: 0.0,
                last_updated: String::new(),
            }
        }
    }

    /// A detected time pattern indicating optimal work windows.
    #[derive(Debug, Clone, Serialize, Deserialize)]
    pub struct TimePattern {
        pub id: i64,
        pub day_of_week: i32,
        pub optimal_start: i32,
        pub optimal_end: i32,
        pub quality_score: f64,
        pub derived_from: String,
        pub last_updated: String,
    }

    impl Default for TimePattern {
        fn default() -> Self {
            Self {
                id: 0,
                day_of_week: 0,
                optimal_start: 0,
                optimal_end: 0,
                quality_score: 0.0,
                derived_from: String::new(),
                last_updated: String::new(),
            }
        }
    }

    /// Repository trait for temporal awareness persistence.
    pub trait TemporalRepository {
        /// Record a work rhythm pattern.
        fn record_rhythm(&self, rhythm: &WorkRhythm) -> Result<(), String>;
        /// Get all work rhythms.
        fn get_rhythms(&self) -> Result<Vec<WorkRhythm>, String>;
        /// Record an availability/time pattern.
        fn record_time_pattern(&self, pattern: &TimePattern) -> Result<(), String>;
        /// Close the repository and release resources.
        fn close(&self) -> Result<(), String>;
    }
}

// ────────────────────────────────────────────────────────────────
// B.5 Projects Domain
// ────────────────────────────────────────────────────────────────

pub mod projects {
    use serde::{Deserialize, Serialize};

    /// A tracked project with lifecycle state.
    #[derive(Debug, Clone, Default, Serialize, Deserialize)]
    pub struct Project {
        pub id: i64,
        pub name: String,
        pub description: String,
        pub priority: String,
        pub status: String,
        pub workspace_path: String,
        pub started_at: Option<String>,
        pub target_date: Option<String>,
        pub completed_at: Option<String>,
    }

    /// A project milestone marking a significant deliverable.
    #[derive(Debug, Clone, Default, Serialize, Deserialize)]
    pub struct Milestone {
        pub id: i64,
        pub project_id: i64,
        pub name: String,
        pub description: String,
        pub target_date: Option<String>,
        pub completed_at: Option<String>,
        pub status: String,
    }

    /// Repository trait for project tracking persistence.
    pub trait ProjectRepository {
        /// Create a new project record.
        fn create_project(&self, project: &Project) -> Result<(), String>;
        /// Get a project by ID.
        fn get_project(&self, id: i64) -> Result<Option<Project>, String>;
        /// Get all active (non-completed) projects.
        fn get_active_projects(&self) -> Result<Vec<Project>, String>;
        /// Close the repository and release resources.
        fn close(&self) -> Result<(), String>;
    }
}

// ============================================================================
// CLOSING
// ============================================================================
// Domain database traits — each domain owns its types, the platform owns
// the connection infrastructure.
// "So built we the wall; and all the wall was joined together" — Nehemiah 4:6

#[cfg(test)]
mod tests {
    use super::*;

    // ────────────────────────────────────────────────────────────
    // T.1 Session default construction
    // ────────────────────────────────────────────────────────────

    #[test]
    fn session_default_has_zero_scores() {
        let s = sessions::Session::default();
        assert_eq!(s.id, "");
        assert_eq!(s.cpi_score, 0.0);
        assert_eq!(s.quality_rating, 0.0);
        assert_eq!(s.tool_count, 0);
        assert_eq!(s.choice_count, 0);
        assert!(s.ended_at.is_none());
    }

    // ────────────────────────────────────────────────────────────
    // T.2 Exchange creation
    // ────────────────────────────────────────────────────────────

    #[test]
    fn exchange_creation_with_fields() {
        let ex = sessions::Exchange {
            id: 42,
            session_id: "sess-001".into(),
            timestamp: "2026-02-16T10:00:00Z".into(),
            sequence_num: 1,
            exchange_type: "question".into(),
            initiative: "user".into(),
            prompt_length: 128,
            feedback_detected: true,
            feedback_polarity: "positive".into(),
            feedback_categories: "clarity".into(),
            value_contribution: 0.85,
            depth_level: "deep".into(),
            insight_detected: true,
            insight_type: "pattern".into(),
            hebrew_state: "Bet".into(),
            k_align: 0.72,
            trajectory: "ascending".into(),
            health_score: Some(95),
            context_tokens: Some(4096),
            user_message_text: "How does the cube relate to choices?".into(),
            response_summary: "Explained cube-choice mapping".into(),
            valence: "constructive".into(),
            thinking_text: "User is exploring spatial reasoning".into(),
        };

        assert_eq!(ex.id, 42);
        assert_eq!(ex.session_id, "sess-001");
        assert_eq!(ex.prompt_length, 128);
        assert!(ex.feedback_detected);
        assert_eq!(ex.health_score, Some(95));
    }

    // ────────────────────────────────────────────────────────────
    // T.3 Session serialization roundtrip
    // ────────────────────────────────────────────────────────────

    #[test]
    fn session_serialization_roundtrip() {
        let original = sessions::Session {
            id: "sess-roundtrip".into(),
            started_at: "2026-02-16T09:00:00Z".into(),
            ended_at: Some("2026-02-16T11:30:00Z".into()),
            project_path: "/home/user/project".into(),
            workspace: "bereshit".into(),
            initial_hebrew_state: "Aleph".into(),
            initial_k_align: 0.5,
            initial_cube_position: "center".into(),
            final_hebrew_state: "Gimel".into(),
            final_k_align: 0.78,
            final_cube_position: "edge-3".into(),
            tool_count: 12,
            choice_count: 8,
            day_of_week: 1,
            hour_of_day: 9,
            exchange_count: 15,
            insight_count: 3,
            cpi_score: 0.82,
            dominant_exchange_type: "exploration".into(),
            session_arc: "ascending".into(),
            narrative_summary: "Deep exploration of ternary mapping".into(),
            quality_rating: 0.9,
        };

        let json = serde_json::to_string(&original).expect("serialize");
        let restored: sessions::Session = serde_json::from_str(&json).expect("deserialize");

        assert_eq!(restored.id, "sess-roundtrip");
        assert_eq!(restored.ended_at, Some("2026-02-16T11:30:00Z".into()));
        assert_eq!(restored.final_k_align, 0.78);
        assert_eq!(restored.exchange_count, 15);
        assert_eq!(restored.cpi_score, 0.82);
        assert_eq!(
            restored.narrative_summary,
            "Deep exploration of ternary mapping"
        );
    }

    // ────────────────────────────────────────────────────────────
    // T.4 Domain type defaults (cognition)
    // ────────────────────────────────────────────────────────────

    #[test]
    fn mental_construct_default() {
        let mc = cognition::MentalConstruct::default();
        assert_eq!(mc.id, "");
        assert_eq!(mc.intended_key, 0);
        assert_eq!(mc.k_at_choice, 0.0);
        assert!(!mc.halt_reached);
        assert!(mc.health_score.is_none());
    }

    // ────────────────────────────────────────────────────────────
    // T.5 Domain type defaults (growth)
    // ────────────────────────────────────────────────────────────

    #[test]
    fn growth_entry_default_is_active() {
        let ge = growth::GrowthEntry::default();
        assert!(ge.is_active);
        assert_eq!(ge.occurrence_count, 0);
        assert_eq!(ge.confidence, 0.0);
    }

    // ────────────────────────────────────────────────────────────
    // T.6 Domain type defaults (temporal)
    // ────────────────────────────────────────────────────────────

    #[test]
    fn work_rhythm_default() {
        let wr = temporal::WorkRhythm::default();
        assert_eq!(wr.day_of_week, 0);
        assert_eq!(wr.session_count, 0);
        assert_eq!(wr.avg_duration_min, 0.0);
    }

    // ────────────────────────────────────────────────────────────
    // T.7 Domain type defaults (projects)
    // ────────────────────────────────────────────────────────────

    #[test]
    fn project_default_has_no_dates() {
        let p = projects::Project::default();
        assert!(p.started_at.is_none());
        assert!(p.target_date.is_none());
        assert!(p.completed_at.is_none());
        assert_eq!(p.id, 0);
    }

    // ────────────────────────────────────────────────────────────
    // T.8 Insight default
    // ────────────────────────────────────────────────────────────

    #[test]
    fn insight_default_has_no_exchange() {
        let i = sessions::Insight::default();
        assert!(i.exchange_id.is_none());
        assert_eq!(i.confidence, 0.0);
    }
}
