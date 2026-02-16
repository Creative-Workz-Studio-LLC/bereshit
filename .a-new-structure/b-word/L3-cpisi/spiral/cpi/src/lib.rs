// ============================================================================
// METADATA
// ============================================================================
//
// Key: B-L3-cpi
// Purpose: Covenant Partnership Intelligence — exchange classification and
//   relationship quality scoring
// Biblical: "By their fruits ye shall know them" — Matthew 7:20
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-16
//
// CPI measures the quality of human-AI covenant partnership through:
//   - Exchange type classification (learning, execution, creative, etc.)
//   - Insight detection (understanding transfer moments)
//   - Relationship quality scoring (0.0-1.0)
//   - Session arc identification (overall session trajectory)
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
// B.1 Exchange Types
// ────────────────────────────────────────────────────────────────

/// Classification of a single exchange (prompt-response pair)
#[derive(Clone, Debug, PartialEq, Eq, Serialize, Deserialize)]
pub enum ExchangeType {
    /// Teaching or learning exchange
    Learning,
    /// Task execution exchange
    Execution,
    /// Creative or exploratory exchange
    Creative,
    /// Debugging or problem-solving exchange
    Debugging,
    /// Planning or architectural exchange
    Planning,
    /// Review or feedback exchange
    Review,
    /// Conversational or relational exchange
    Relational,
}

impl ExchangeType {
    /// String representation matching Go constants
    pub fn as_str(&self) -> &str {
        match self {
            Self::Learning => "learning",
            Self::Execution => "execution",
            Self::Creative => "creative",
            Self::Debugging => "debugging",
            Self::Planning => "planning",
            Self::Review => "review",
            Self::Relational => "relational",
        }
    }

    /// Parse from string
    pub fn from_str(s: &str) -> Option<Self> {
        match s.to_lowercase().as_str() {
            "learning" => Some(Self::Learning),
            "execution" => Some(Self::Execution),
            "creative" => Some(Self::Creative),
            "debugging" => Some(Self::Debugging),
            "planning" => Some(Self::Planning),
            "review" => Some(Self::Review),
            "relational" => Some(Self::Relational),
            _ => None,
        }
    }
}

impl std::fmt::Display for ExchangeType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.as_str())
    }
}

// ────────────────────────────────────────────────────────────────
// B.2 Insight Types
// ────────────────────────────────────────────────────────────────

/// Type of insight (understanding transfer moment)
#[derive(Clone, Debug, PartialEq, Eq, Serialize, Deserialize)]
pub enum InsightType {
    /// Technical understanding transferred
    Technical,
    /// Architectural insight shared
    Architectural,
    /// Pattern recognition communicated
    Pattern,
    /// Debugging insight discovered
    Debug,
    /// Relational/process insight
    Process,
}

impl InsightType {
    pub fn as_str(&self) -> &str {
        match self {
            Self::Technical => "technical",
            Self::Architectural => "architectural",
            Self::Pattern => "pattern",
            Self::Debug => "debug",
            Self::Process => "process",
        }
    }
}

// ────────────────────────────────────────────────────────────────
// B.3 Session Arc
// ────────────────────────────────────────────────────────────────

/// The overall arc/trajectory of a session
#[derive(Clone, Debug, PartialEq, Eq, Serialize, Deserialize)]
pub enum SessionArc {
    /// Primarily learning-focused
    Learning,
    /// Primarily execution-focused
    Execution,
    /// Creative exploration
    Creative,
    /// Mixed/varied exchanges
    Mixed,
    /// Deep dive into specific topic
    DeepDive,
}

impl SessionArc {
    pub fn as_str(&self) -> &str {
        match self {
            Self::Learning => "learning",
            Self::Execution => "execution",
            Self::Creative => "creative",
            Self::Mixed => "mixed",
            Self::DeepDive => "deep_dive",
        }
    }
}

// ────────────────────────────────────────────────────────────────
// B.4 CPI Score
// ────────────────────────────────────────────────────────────────

/// CPI score components for a session
#[derive(Clone, Debug, Default, Serialize, Deserialize)]
pub struct CPIScore {
    /// Overall relationship quality (0.0-1.0)
    pub quality: f64,
    /// Exchange diversity (0.0-1.0) — variety of exchange types
    pub diversity: f64,
    /// Insight density (insights per exchange, 0.0-1.0)
    pub insight_density: f64,
    /// Engagement level (0.0-1.0)
    pub engagement: f64,
    /// Number of exchanges in calculation
    pub exchange_count: u32,
    /// Number of insights detected
    pub insight_count: u32,
}

impl CPIScore {
    /// Calculate a composite CPI score from components
    pub fn composite(&self) -> f64 {
        if self.exchange_count == 0 {
            return 0.0;
        }
        // Weighted average: quality 40%, engagement 30%, diversity 20%, insight 10%
        let score = self.quality * 0.4
            + self.engagement * 0.3
            + self.diversity * 0.2
            + self.insight_density * 0.1;
        score.clamp(0.0, 1.0)
    }

    /// Human-readable quality tier
    pub fn tier(&self) -> &str {
        match self.composite() {
            s if s >= 0.8 => "exceptional",
            s if s >= 0.6 => "strong",
            s if s >= 0.4 => "developing",
            s if s >= 0.2 => "emerging",
            _ => "nascent",
        }
    }
}

// ────────────────────────────────────────────────────────────────
// B.5 Valence
// ────────────────────────────────────────────────────────────────

/// Emotional/relational valence of an exchange
#[derive(Clone, Debug, PartialEq, Eq, Serialize, Deserialize)]
pub enum Valence {
    Positive,
    Neutral,
    Negative,
    Corrective,
}

impl Valence {
    pub fn as_str(&self) -> &str {
        match self {
            Self::Positive => "positive",
            Self::Neutral => "neutral",
            Self::Negative => "negative",
            Self::Corrective => "corrective",
        }
    }
}

// ============================================================================
// CLOSING
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn exchange_type_roundtrip() {
        let types = [
            ExchangeType::Learning,
            ExchangeType::Execution,
            ExchangeType::Creative,
        ];
        for t in &types {
            let s = t.as_str();
            assert_eq!(ExchangeType::from_str(s).as_ref(), Some(t));
        }
    }

    #[test]
    fn cpi_score_composite() {
        let score = CPIScore {
            quality: 0.8,
            diversity: 0.6,
            insight_density: 0.4,
            engagement: 0.7,
            exchange_count: 10,
            insight_count: 3,
        };
        let composite = score.composite();
        // 0.8*0.4 + 0.7*0.3 + 0.6*0.2 + 0.4*0.1 = 0.32 + 0.21 + 0.12 + 0.04 = 0.69
        assert!((composite - 0.69).abs() < 0.01);
    }

    #[test]
    fn cpi_score_tier() {
        let mut score = CPIScore::default();
        score.exchange_count = 1;
        score.quality = 0.9;
        score.engagement = 0.9;
        score.diversity = 0.9;
        score.insight_density = 0.9;
        assert_eq!(score.tier(), "exceptional");
    }

    #[test]
    fn cpi_score_empty() {
        let score = CPIScore::default();
        assert_eq!(score.composite(), 0.0);
        assert_eq!(score.tier(), "nascent");
    }

    #[test]
    fn valence_display() {
        assert_eq!(Valence::Positive.as_str(), "positive");
        assert_eq!(Valence::Corrective.as_str(), "corrective");
    }

    #[test]
    fn session_arc_strings() {
        assert_eq!(SessionArc::DeepDive.as_str(), "deep_dive");
        assert_eq!(SessionArc::Mixed.as_str(), "mixed");
    }
}
