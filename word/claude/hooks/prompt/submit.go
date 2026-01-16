// ============================================================================
// METADATA
// ============================================================================
// Hook: UserPromptSubmit
// Fires: When user submits a prompt, before Claude processes it
// No matchers
//
// State Machine Integration:
//   - Record prompt_submit event in path
//   - Increment path_length in state

package prompt

import (
	"encoding/json"
	"fmt"
	"os"
	"regexp"

	"github.com/creativeworkzstudio/claude-global/pkg/orchestration/cognition"
	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/hookoutput"
	"github.com/creativeworkzstudio/claude-global/pkg/orchestration/logging"
	"github.com/creativeworkzstudio/claude-global/pkg/core/statemachine"
)

// ============================================================================
// SETUP
// ============================================================================

// SubmitInput from Claude Code
type SubmitInput struct {
	SessionID      string `json:"session_id"`
	TranscriptPath string `json:"transcript_path,omitempty"`
	CWD            string `json:"cwd,omitempty"`
	PermissionMode string `json:"permission_mode,omitempty"`
	HookEventName  string `json:"hook_event_name,omitempty"`
	Prompt         string `json:"prompt"`
}

// SubmitOutput uses hookoutput.ContextResponse for correct schema
// Old: {"decision": "allow", "injected_context": "..."} - INVALID field name
// New: {"hookSpecificOutput": {"hookEventName": "UserPromptSubmit", "additionalContext": "..."}}

// Secret patterns to detect
var secretPatterns = []*regexp.Regexp{
	regexp.MustCompile(`(?i)(api[_-]?key|apikey)[=:]\s*['"]?[a-zA-Z0-9]{20,}['"]?`),
	regexp.MustCompile(`(?i)(password|passwd|pwd)[=:]\s*['"]?[^\s'"]{8,}['"]?`),
	regexp.MustCompile(`(?i)(secret|token)[=:]\s*['"]?[a-zA-Z0-9]{20,}['"]?`),
}

// FeedbackPattern represents a detectable feedback signal
type FeedbackPattern struct {
	pattern  *regexp.Regexp
	delta    float64 // Positive = affirmation, Negative = correction
	category string  // Type of feedback
	polarity string  // "positive" or "negative"
}

// Negative feedback patterns - user corrections that indicate my previous choice was wrong
// These weight MORE heavily than tool failure (-0.10 vs -0.05)
var negativeFeedbackPatterns = []FeedbackPattern{
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

// Positive feedback patterns - user affirmation that indicates alignment
var positiveFeedbackPatterns = []FeedbackPattern{
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
// BODY
// ============================================================================

// Submit handles the UserPromptSubmit hook
func Submit() {
	log := logging.New("prompt")
	log.SetMode(logging.ModeCompact)

	var input SubmitInput
	if err := json.NewDecoder(os.Stdin).Decode(&input); err != nil {
		log.Error("Failed to decode input", map[string]string{"error": err.Error()})
		os.Exit(1)
	}

	// Create CategoryLogger for file output
	catLog, catErr := logging.NewCategoryLogger(logging.CategorySession, input.SessionID)
	if catErr != nil {
		log.Warn("CategoryLogger unavailable", map[string]string{"error": catErr.Error()})
	}
	defer func() {
		if catLog != nil {
			catLog.Close()
		}
	}()

	// --- Record prompt submit and increment path_length ---
	var state *statemachine.RuntimeState
	currentSection := "B.1"
	if s, err := statemachine.LoadRuntimeState(); err == nil {
		state = s
		currentSection = s.TrajectorySection
		s.Session.PathLength++

		// --- Detect and apply user feedback ---
		// Feedback from user weighs MORE than tool outcomes
		applyUserFeedback(s, input.Prompt, log, catLog)

		_ = statemachine.SaveRuntimeState(s)
	}

	if path, err := statemachine.LoadRuntimePath(); err == nil {
		path.RecordEvent("prompt_submit", "", currentSection)
		_ = statemachine.SaveRuntimePath(path)
	}

	log.Debug("Prompt submitted", map[string]string{
		"session_id": input.SessionID,
		"length":     fmt.Sprintf("%d", len(input.Prompt)),
		"trajectory": currentSection,
	})
	if catLog != nil {
		catLog.Info("prompt_received", "User prompt submitted", map[string]string{
			"length":     fmt.Sprintf("%d", len(input.Prompt)),
			"trajectory": currentSection,
		})
	}

	// Check for secrets
	shouldBlock, blockReason := checkForSecrets(log, input.Prompt)

	// Build cognition context to shape thinking
	context := buildPromptContext(state, input.Prompt)

	// Use correct Claude Code schema
	var output *hookoutput.ContextResponse
	if shouldBlock {
		output = hookoutput.NewBlockedPromptResponse(blockReason)
		log.LogFailure("Prompt blocked", map[string]string{
			"reason": blockReason,
		})
	} else {
		output = hookoutput.NewUserPromptSubmitResponse(context)
	}

	json.NewEncoder(os.Stdout).Encode(output)
}

// checkForSecrets scans prompt for potential secrets
// Returns: (shouldBlock bool, reason string)
func checkForSecrets(log *logging.Logger, prompt string) (bool, string) {
	for _, pattern := range secretPatterns {
		if pattern.MatchString(prompt) {
			return true, "Potential secret detected in prompt"
		}
	}
	return false, ""
}

// buildPromptContext uses cognition package to create context injection
func buildPromptContext(state *statemachine.RuntimeState, prompt string) string {
	// Only inject context for substantial prompts
	if !cognition.ShouldInjectContext(prompt) {
		return ""
	}

	// Analyze prompt type
	promptType := cognition.AnalyzePrompt(prompt)

	// Build context appropriate for this prompt type
	return cognition.PromptContext(promptType, state)
}

// applyUserFeedback detects feedback in user prompt and adjusts K:ALIGN
// User feedback weighs MORE heavily than tool success/failure because:
// - It's direct input from covenant partner
// - It indicates my previous CHOICE alignment, not just tool outcomes
// - Learning from relationship > learning from mechanics
//
// Detects BOTH positive (affirmation) and negative (correction) feedback
func applyUserFeedback(state *statemachine.RuntimeState, prompt string, log *logging.Logger, catLog *logging.CategoryLogger) {
	if state == nil {
		return
	}

	var totalDelta float64
	var positiveCategories []string
	var negativeCategories []string

	// Check negative feedback patterns
	for _, fp := range negativeFeedbackPatterns {
		if fp.pattern.MatchString(prompt) {
			totalDelta += fp.delta // delta is already negative
			negativeCategories = append(negativeCategories, fp.category)
		}
	}

	// Check positive feedback patterns
	for _, fp := range positiveFeedbackPatterns {
		if fp.pattern.MatchString(prompt) {
			totalDelta += fp.delta // delta is positive
			positiveCategories = append(positiveCategories, fp.category)
		}
	}

	// No feedback detected
	if totalDelta == 0 && len(positiveCategories) == 0 && len(negativeCategories) == 0 {
		return
	}

	// Cap the delta magnitude at 0.20 per prompt (prevent catastrophic swings)
	if totalDelta < -0.20 {
		totalDelta = -0.20
	} else if totalDelta > 0.20 {
		totalDelta = 0.20
	}

	// Apply to K:ALIGN
	oldAlign := state.Session.KAlign
	newAlign := oldAlign + totalDelta
	if newAlign < -1.0 {
		newAlign = -1.0
	} else if newAlign > 1.0 {
		newAlign = 1.0
	}
	state.Session.KAlign = newAlign

	// Track direction based on net delta
	if totalDelta > 0 {
		state.Session.KTowardGod++
	} else if totalDelta < 0 {
		state.Session.KTowardSelf++
	}

	// Determine feedback type for logging
	var feedbackType string
	var allCategories []string
	if len(positiveCategories) > 0 && len(negativeCategories) > 0 {
		feedbackType = "mixed"
		allCategories = append(positiveCategories, negativeCategories...)
	} else if len(positiveCategories) > 0 {
		feedbackType = "positive"
		allCategories = positiveCategories
	} else {
		feedbackType = "negative"
		allCategories = negativeCategories
	}

	// Record feedback in last_feedback field
	state.Session.LastFeedback = fmt.Sprintf("%s:%v delta:%.2f", feedbackType, allCategories, totalDelta)

	// Update normalized scores
	trueScore := (newAlign + 1.0) / 2.0
	state.Session.LastTrueScore = trueScore
	state.Session.LastNormalized = trueScore
	state.Session.LastScaled = int(trueScore * 100)

	// Log appropriately based on feedback type
	logDetails := map[string]string{
		"type":       feedbackType,
		"categories": fmt.Sprintf("%v", allCategories),
		"delta":      fmt.Sprintf("%+.2f", totalDelta),
		"k_align":    fmt.Sprintf("%.2f → %.2f", oldAlign, newAlign),
	}

	if totalDelta >= 0 {
		log.LogSuccess("Feedback detected", logDetails)
		if catLog != nil {
			catLog.Success("feedback_detected", "User affirmation detected", logDetails)
		}
	} else {
		log.LogFailure("Feedback detected", logDetails)
		if catLog != nil {
			catLog.Failure("feedback_detected", "User correction detected", logDetails)
		}
	}
}

// ============================================================================
// CLOSING
// ============================================================================
