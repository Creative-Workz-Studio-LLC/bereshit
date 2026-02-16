// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L3-cognition
// Purpose: Cognition orchestration — mental constructs and thinking patterns
// Biblical: "For as he thinketh in his heart, so is he" — Proverbs 23:7
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-16
//
// Cognition bridges health monitoring, state machine navigation, and
// database persistence into a unified thinking model.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

use serde::{Deserialize, Serialize};

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// B.1 Mental Construct Types
// ────────────────────────────────────────────────────────────────

/// A mental construct — a unit of thought or reasoning
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct MentalConstruct {
    /// Unique identifier
    pub id: String,
    /// The thought or reasoning content
    pub content: String,
    /// Construct type
    pub construct_type: ConstructType,
    /// Confidence level (0.0-1.0)
    pub confidence: f64,
    /// Associated Hebrew state
    pub hebrew_state: String,
    /// K:ALIGN at time of construct
    pub k_align: f64,
    /// Timestamp (ISO 8601)
    pub timestamp: String,
}

/// Types of mental constructs
#[derive(Clone, Debug, PartialEq, Eq, Serialize, Deserialize)]
pub enum ConstructType {
    /// Observation of fact or state
    Observation,
    /// Analysis or reasoning
    Analysis,
    /// Hypothesis or prediction
    Hypothesis,
    /// Decision or conclusion
    Decision,
    /// Reflection on past experience
    Reflection,
    /// Integration of multiple inputs
    Integration,
}

impl ConstructType {
    pub fn as_str(&self) -> &str {
        match self {
            Self::Observation => "observation",
            Self::Analysis => "analysis",
            Self::Hypothesis => "hypothesis",
            Self::Decision => "decision",
            Self::Reflection => "reflection",
            Self::Integration => "integration",
        }
    }
}

// ────────────────────────────────────────────────────────────────
// B.2 Cognition Session
// ────────────────────────────────────────────────────────────────

/// Tracks cognitive state within a session
#[derive(Clone, Debug, Default, Serialize, Deserialize)]
pub struct CognitionState {
    /// Active mental constructs
    pub constructs: Vec<MentalConstruct>,
    /// Current cognitive depth (how deep into analysis)
    pub depth: u32,
    /// Whether in a reflection cycle
    pub reflecting: bool,
    /// Count of observations this session
    pub observation_count: u32,
    /// Count of decisions this session
    pub decision_count: u32,
    /// Count of integrations this session
    pub integration_count: u32,
}

impl CognitionState {
    /// Add a new mental construct
    pub fn add_construct(&mut self, construct: MentalConstruct) {
        match construct.construct_type {
            ConstructType::Observation => self.observation_count += 1,
            ConstructType::Decision => self.decision_count += 1,
            ConstructType::Integration => self.integration_count += 1,
            _ => {}
        }
        self.constructs.push(construct);
    }

    /// Get the most recent construct
    pub fn last_construct(&self) -> Option<&MentalConstruct> {
        self.constructs.last()
    }

    /// Total constructs
    pub fn total_constructs(&self) -> usize {
        self.constructs.len()
    }

    /// Average confidence across all constructs
    pub fn average_confidence(&self) -> f64 {
        if self.constructs.is_empty() {
            return 0.0;
        }
        let sum: f64 = self.constructs.iter().map(|c| c.confidence).sum();
        sum / self.constructs.len() as f64
    }

    /// Constructs of a specific type
    pub fn by_type(&self, ct: &ConstructType) -> Vec<&MentalConstruct> {
        self.constructs
            .iter()
            .filter(|c| c.construct_type == *ct)
            .collect()
    }
}

// ────────────────────────────────────────────────────────────────
// B.3 Cognition Pattern
// ────────────────────────────────────────────────────────────────

/// A detected pattern in cognitive processing
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct CognitionPattern {
    /// Pattern name
    pub name: String,
    /// How many times observed
    pub frequency: u32,
    /// Confidence (0.0-1.0)
    pub confidence: f64,
    /// Related construct IDs
    pub related_constructs: Vec<String>,
    /// Description
    pub description: String,
}

// ============================================================================
// CLOSING
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn cognition_state_default() {
        let state = CognitionState::default();
        assert_eq!(state.total_constructs(), 0);
        assert_eq!(state.average_confidence(), 0.0);
    }

    #[test]
    fn add_construct_increments_counts() {
        let mut state = CognitionState::default();
        state.add_construct(MentalConstruct {
            id: "1".to_string(),
            content: "Test".to_string(),
            construct_type: ConstructType::Observation,
            confidence: 0.8,
            hebrew_state: "yashar".to_string(),
            k_align: 0.5,
            timestamp: String::new(),
        });
        assert_eq!(state.observation_count, 1);
        assert_eq!(state.total_constructs(), 1);
    }

    #[test]
    fn average_confidence() {
        let mut state = CognitionState::default();
        for conf in [0.6, 0.8, 1.0] {
            state.add_construct(MentalConstruct {
                id: String::new(),
                content: String::new(),
                construct_type: ConstructType::Analysis,
                confidence: conf,
                hebrew_state: String::new(),
                k_align: 0.0,
                timestamp: String::new(),
            });
        }
        assert!((state.average_confidence() - 0.8).abs() < 0.001);
    }

    #[test]
    fn filter_by_type() {
        let mut state = CognitionState::default();
        state.add_construct(MentalConstruct {
            id: "1".to_string(),
            content: "obs".to_string(),
            construct_type: ConstructType::Observation,
            confidence: 0.9,
            hebrew_state: String::new(),
            k_align: 0.0,
            timestamp: String::new(),
        });
        state.add_construct(MentalConstruct {
            id: "2".to_string(),
            content: "dec".to_string(),
            construct_type: ConstructType::Decision,
            confidence: 0.7,
            hebrew_state: String::new(),
            k_align: 0.0,
            timestamp: String::new(),
        });

        let obs = state.by_type(&ConstructType::Observation);
        assert_eq!(obs.len(), 1);
        assert_eq!(obs[0].id, "1");
    }

    #[test]
    fn construct_type_strings() {
        assert_eq!(ConstructType::Integration.as_str(), "integration");
        assert_eq!(ConstructType::Hypothesis.as_str(), "hypothesis");
    }

    #[test]
    fn last_construct() {
        let mut state = CognitionState::default();
        assert!(state.last_construct().is_none());

        state.add_construct(MentalConstruct {
            id: "first".to_string(),
            content: String::new(),
            construct_type: ConstructType::Observation,
            confidence: 0.5,
            hebrew_state: String::new(),
            k_align: 0.0,
            timestamp: String::new(),
        });
        state.add_construct(MentalConstruct {
            id: "second".to_string(),
            content: String::new(),
            construct_type: ConstructType::Decision,
            confidence: 0.9,
            hebrew_state: String::new(),
            k_align: 0.0,
            timestamp: String::new(),
        });

        assert_eq!(state.last_construct().unwrap().id, "second");
    }
}
