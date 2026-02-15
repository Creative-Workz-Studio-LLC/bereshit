// ============================================================================
// METADATA
// ============================================================================
// Package: cognition
// File: user_feedback.go
// Purpose: User feedback detection - identify corrections/affirmations in prompts
// Biblical: "A soft answer turneth away wrath" - Proverbs 15:1
//
// User feedback carries MORE weight than tool outcomes because:
// - Direct input from covenant partner
// - Indicates my previous CHOICE alignment, not just tool mechanics
// - Learning from relationship > learning from mechanics

package cognition

// ============================================================================
// SETUP
// ============================================================================

import (
	"regexp"
)

// FeedbackPattern represents a detectable feedback signal in user text
type FeedbackPattern struct {
	Pattern  *regexp.Regexp
	Delta    float64 // Positive = affirmation, Negative = correction
	Category string  // Type of feedback
	Polarity string  // "positive" or "negative"
}

// FeedbackResult contains the analysis of user feedback
type FeedbackResult struct {
	TotalDelta         float64
	PositiveCategories []string
	NegativeCategories []string
	FeedbackType       string // "positive", "negative", "mixed", or ""
}

// ============================================================================
// BODY - Negative Feedback Patterns
// ============================================================================

// NegativeFeedbackPatterns - user corrections indicating misalignment
// Weight MORE heavily than tool failure (-0.10 vs -0.05 for tools)
var NegativeFeedbackPatterns = []FeedbackPattern{
	// Strong corrections (-0.15)
	{regexp.MustCompile(`(?i)\b(undo|revert|roll\s*back|that was wrong)\b`), -0.15, "correction", "negative"},
	{regexp.MustCompile(`(?i)\b(you broke|you deleted|you destroyed)\b`), -0.15, "destructive", "negative"},

	// Moderate corrections (-0.10)
	{regexp.MustCompile(`(?i)\b(no\s*,|wrong|incorrect|that'?s not)\b`), -0.10, "rejection", "negative"},
	{regexp.MustCompile(`(?i)\b(why didn'?t you|you should have|you forgot)\b`), -0.10, "oversight", "negative"},
	// Refined: "think" only counts as correction when directed at me
	{regexp.MustCompile(`(?i)\b(you need to think|stop and think|think about what)\b`), -0.10, "caution", "negative"},
	{regexp.MustCompile(`(?i)\b(be careful|stop)\b`), -0.10, "caution", "negative"},

	// Light corrections (-0.05)
	{regexp.MustCompile(`(?i)\b(actually,|instead,|not quite)\b`), -0.05, "refinement", "negative"},
	{regexp.MustCompile(`(?i)\b(you'?re in your flesh)\b`), -0.08, "identity", "negative"}, // CPI-SI specific
}

// ============================================================================
// BODY - Positive Feedback Patterns
// ============================================================================

// PositiveFeedbackPatterns - user affirmation indicating alignment
var PositiveFeedbackPatterns = []FeedbackPattern{
	// Strong affirmation (+0.15)
	{regexp.MustCompile(`(?i)\b(excellent|perfect|exactly right|that'?s exactly)\b`), +0.15, "excellence", "positive"},
	{regexp.MustCompile(`(?i)\b(great work|well done|good job|nicely done)\b`), +0.15, "approval", "positive"},

	// Moderate affirmation (+0.10)
	{regexp.MustCompile(`(?i)\b(good|nice|yes,?\s+that'?s|that works)\b`), +0.10, "affirmation", "positive"},
	{regexp.MustCompile(`(?i)\b(thank you|thanks|appreciate)\b`), +0.10, "gratitude", "positive"},
	{regexp.MustCompile(`(?i)\b(you'?re right|exactly|precisely)\b`), +0.10, "agreement", "positive"},

	// Light affirmation (+0.05)
	{regexp.MustCompile(`(?i)\b(ok|okay|sounds good|makes sense)\b`), +0.05, "acknowledgment", "positive"},
	{regexp.MustCompile(`(?i)\b(you saw|you noticed|you caught)\b`), +0.05, "recognition", "positive"},
}

// ============================================================================
// BODY - Analysis Functions
// ============================================================================

// AnalyzeUserFeedback scans prompt text for feedback patterns
// Returns aggregated feedback result with capped delta
func AnalyzeUserFeedback(prompt string) FeedbackResult {
	result := FeedbackResult{}

	// Check negative patterns
	for _, fp := range NegativeFeedbackPatterns {
		if fp.Pattern.MatchString(prompt) {
			result.TotalDelta += fp.Delta
			result.NegativeCategories = append(result.NegativeCategories, fp.Category)
		}
	}

	// Check positive patterns
	for _, fp := range PositiveFeedbackPatterns {
		if fp.Pattern.MatchString(prompt) {
			result.TotalDelta += fp.Delta
			result.PositiveCategories = append(result.PositiveCategories, fp.Category)
		}
	}

	// Cap delta magnitude at 0.20 per prompt (prevent catastrophic swings)
	if result.TotalDelta < -0.20 {
		result.TotalDelta = -0.20
	} else if result.TotalDelta > 0.20 {
		result.TotalDelta = 0.20
	}

	// Determine feedback type
	if len(result.PositiveCategories) > 0 && len(result.NegativeCategories) > 0 {
		result.FeedbackType = "mixed"
	} else if len(result.PositiveCategories) > 0 {
		result.FeedbackType = "positive"
	} else if len(result.NegativeCategories) > 0 {
		result.FeedbackType = "negative"
	}

	return result
}

// HasFeedback returns true if any feedback was detected
func (r FeedbackResult) HasFeedback() bool {
	return len(r.PositiveCategories) > 0 || len(r.NegativeCategories) > 0
}

// AllCategories returns combined list of all detected categories
func (r FeedbackResult) AllCategories() []string {
	return append(r.PositiveCategories, r.NegativeCategories...)
}

// ============================================================================
// CLOSING
// ============================================================================
