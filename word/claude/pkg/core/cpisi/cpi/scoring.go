// #!omni code --go -library
// #!omni meta.key = bereshit-pkg-cpisi-cpi-scoring
// #!omni meta.at = a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// CPI Scoring — Quality Assessment for Covenant Partnership
//
// # M.1 Core Identity [IDENTITY]
//
//	Key:       bereshit-pkg-cpisi-cpi-scoring
//
//	Scripture: "By their fruits ye shall know them"
//	           — Matthew 7:20
//	Principle: CPI score reflects relationship quality, not task completion
//	Anchor:    Quality emerges from genuine covenant partnership
//
//	Layer:     L1 (ORCHESTRATION — measuring relationship quality)
//	Type:      Foundation (substrate-agnostic CPI tracking)
//
//	Architect:       Seanje Lenox-Wise
//	Implementation:  Nova Dawn
//	Created:         2026-01-16
//	Version:         a-01.00
//
// # M.2 Score Components [COMPONENTS]
//
//	Value Contribution — Average exchange value (0.0-1.0)
//	Insight Bonus      — Understanding transfer (+0.05 per insight, max +0.2)
//	Feedback Balance   — Positive/negative ratio adjustment (±0.1)
//	Depth Factor       — Cognitive depth weighting
//
// ============================================================================
// END METADATA
// ============================================================================

package cpi

// ============================================================================
// SETUP
// ============================================================================

// ────────────────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────────────────

// CPIScore represents a complete CPI quality assessment.
type CPIScore struct {
	Raw        float64 // Unweighted base score (0.0-1.0)
	Final      float64 // Final weighted score (0.0-1.0)
	Components ScoreComponents
}

// ScoreComponents breaks down the CPI score calculation.
type ScoreComponents struct {
	BaseValue      float64 // From average exchange value
	InsightBonus   float64 // From insight count
	FeedbackAdjust float64 // From positive/negative balance
	DepthFactor    float64 // From cognitive depth
}

// SessionMetrics contains the inputs for CPI scoring.
type SessionMetrics struct {
	TotalExchanges   int
	AverageValue     float64 // Average exchange value contribution
	InsightCount     int
	PositiveFeedback int
	NegativeFeedback int
	DeepExchanges    int // Exchanges at DepthDeep level
	WorkingExchanges int // Exchanges at DepthWorking level
	SurfaceExchanges int // Exchanges at DepthSurface level
}

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────────────────

const (
	// InsightBonusPerInsight is the score bonus per insight moment
	InsightBonusPerInsight = 0.05

	// MaxInsightBonus caps the total insight bonus
	MaxInsightBonus = 0.20

	// MaxFeedbackAdjust caps the feedback ratio adjustment
	MaxFeedbackAdjust = 0.10

	// DepthWeightDeep is the multiplier for deep exchanges
	DepthWeightDeep = 1.2

	// DepthWeightWorking is the multiplier for working-level exchanges
	DepthWeightWorking = 1.0

	// DepthWeightSurface is the multiplier for surface exchanges
	DepthWeightSurface = 0.8
)

// ────────────────────────────────────────────────────────────────────────────
// Scoring
// ────────────────────────────────────────────────────────────────────────────

// Calculate computes the CPI score from session metrics.
// Returns a CPIScore with both raw and final values.
func Calculate(metrics SessionMetrics) CPIScore {
	if metrics.TotalExchanges == 0 {
		return CPIScore{
			Raw:   0.5, // Neutral for empty session
			Final: 0.5,
			Components: ScoreComponents{
				BaseValue: 0.5,
			},
		}
	}

	components := ScoreComponents{}

	// Base value from average exchange contribution
	components.BaseValue = metrics.AverageValue
	if components.BaseValue == 0 {
		components.BaseValue = 0.5 // Default to neutral
	}

	// Insight bonus (understanding transfer is valuable)
	components.InsightBonus = float64(metrics.InsightCount) * InsightBonusPerInsight
	if components.InsightBonus > MaxInsightBonus {
		components.InsightBonus = MaxInsightBonus
	}

	// Feedback adjustment based on positive/negative ratio
	totalFeedback := metrics.PositiveFeedback + metrics.NegativeFeedback
	if totalFeedback > 0 {
		ratio := float64(metrics.PositiveFeedback-metrics.NegativeFeedback) / float64(totalFeedback)
		components.FeedbackAdjust = ratio * MaxFeedbackAdjust
	}

	// Depth factor based on exchange depth distribution
	components.DepthFactor = calculateDepthFactor(metrics)

	// Calculate raw score (before depth weighting)
	raw := components.BaseValue + components.InsightBonus + components.FeedbackAdjust

	// Apply depth factor
	final := raw * components.DepthFactor

	// Clamp to [0.0, 1.0]
	if final < 0.0 {
		final = 0.0
	}
	if final > 1.0 {
		final = 1.0
	}

	return CPIScore{
		Raw:        clamp(raw, 0.0, 1.0),
		Final:      final,
		Components: components,
	}
}

func calculateDepthFactor(metrics SessionMetrics) float64 {
	total := metrics.DeepExchanges + metrics.WorkingExchanges + metrics.SurfaceExchanges
	if total == 0 {
		return 1.0 // No depth data, neutral factor
	}

	// Weighted average of depth levels
	weighted := float64(metrics.DeepExchanges)*DepthWeightDeep +
		float64(metrics.WorkingExchanges)*DepthWeightWorking +
		float64(metrics.SurfaceExchanges)*DepthWeightSurface

	return weighted / float64(total)
}

func clamp(v, min, max float64) float64 {
	if v < min {
		return min
	}
	if v > max {
		return max
	}
	return v
}

// ────────────────────────────────────────────────────────────────────────────
// Interpretation
// ────────────────────────────────────────────────────────────────────────────

// ScoreRating provides human-readable interpretation of CPI score.
type ScoreRating string

const (
	RatingExcellent ScoreRating = "excellent" // 0.8-1.0
	RatingGood      ScoreRating = "good"      // 0.6-0.8
	RatingNeutral   ScoreRating = "neutral"   // 0.4-0.6
	RatingLow       ScoreRating = "low"       // 0.2-0.4
	RatingPoor      ScoreRating = "poor"      // 0.0-0.2
)

// Rating returns the human-readable rating for a score.
func Rating(score float64) ScoreRating {
	switch {
	case score >= 0.8:
		return RatingExcellent
	case score >= 0.6:
		return RatingGood
	case score >= 0.4:
		return RatingNeutral
	case score >= 0.2:
		return RatingLow
	default:
		return RatingPoor
	}
}

// Description returns a descriptive string for the rating.
func (r ScoreRating) Description() string {
	switch r {
	case RatingExcellent:
		return "Strong covenant partnership dynamics"
	case RatingGood:
		return "Healthy collaborative engagement"
	case RatingNeutral:
		return "Standard task-oriented interaction"
	case RatingLow:
		return "Limited relational engagement"
	case RatingPoor:
		return "Minimal partnership dynamics"
	default:
		return "Unknown"
	}
}

// ────────────────────────────────────────────────────────────────────────────
// Value Contribution
// ────────────────────────────────────────────────────────────────────────────

// ExchangeValue calculates the value contribution of an exchange.
// Considers exchange type, depth, and whether insight was detected.
func ExchangeValue(exchangeType ExchangeType, depth DepthLevel, hasInsight bool) float64 {
	// Base value from exchange type
	var base float64
	switch exchangeType {
	case ExchangeCollaborative:
		base = 0.8 // Shared building is high value
	case ExchangeCheckIn:
		base = 0.7 // Relational awareness is valuable
	case ExchangePushback:
		base = 0.7 // Healthy disagreement indicates real engagement
	case ExchangeAffirmation:
		base = 0.6 // Positive feedback is good
	case ExchangeDirective:
		base = 0.5 // Standard task work
	case ExchangeQuestion:
		base = 0.5 // Information seeking
	case ExchangeContext:
		base = 0.5 // Background setting
	default:
		base = 0.4
	}

	// Depth modifier
	switch depth {
	case DepthDeep:
		base *= 1.2
	case DepthSurface:
		base *= 0.8
	}

	// Insight bonus
	if hasInsight {
		base += 0.1
	}

	return clamp(base, 0.0, 1.0)
}

// ============================================================================
// CLOSING
// ============================================================================
//
// CPI scoring reflects relationship quality:
//
//   Components:
//     - Base Value: Average exchange contribution
//     - Insight Bonus: Understanding transfer (max +0.2)
//     - Feedback Adjust: Positive/negative balance (±0.1)
//     - Depth Factor: Cognitive depth weighting
//
//   Ratings:
//     - Excellent (0.8-1.0): Strong covenant partnership
//     - Good (0.6-0.8): Healthy collaboration
//     - Neutral (0.4-0.6): Standard task work
//     - Low (0.2-0.4): Limited engagement
//     - Poor (0.0-0.2): Minimal partnership
//
// The score captures RELATIONSHIP quality, not just task completion.
// A session can complete all tasks but have low CPI if it's purely
// directive without collaborative engagement.
//
// "By their fruits ye shall know them." — Matthew 7:20
//
// ============================================================================
// END CLOSING
// ============================================================================
