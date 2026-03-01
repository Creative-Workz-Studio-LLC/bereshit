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
//
// CPI Tracking (v2.0.0):
//   - Exchange type classification (directive, collaborative, check_in, etc.)
//   - Insight marker detection (understanding transfer)
//   - Quality/value scoring
//   - Relational dynamics tracking
//
// Biblical: Proverbs 27:17 - "Iron sharpeneth iron"

package prompt

import (
	"context"
	"encoding/json"
	"fmt"
	"os"
	"regexp"
	"time"

	"cws.studio/claude/hooks/internal"
	"cws.studio/pkg/core/cpisi/cpi"
	"cws.studio/pkg/core/statemachine"
	"cws.studio/pkg/foundation/database"
	"cws.studio/pkg/orchestration/cognition"
	"cws.studio/pkg/orchestration/logging"
	"cws.studio/pkg/sdk/hookoutput"
	"cws.studio/pkg/sdk/substrate"
	"cws.studio/claude/hooks/internal/status"
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

// ───────────────────────────────────────────────────────────────────────────
// CPI TRACKING: Types from Native Package
// ───────────────────────────────────────────────────────────────────────────
//
// Exchange types, insight types, and depth levels are now defined in the
// native CPI package: pkg/core/cpisi/cpi/
//
// This hook uses those types directly for substrate-agnostic CPI tracking.
//
// Type aliases for backward compatibility:
type ExchangeType = cpi.ExchangeType
type InsightType = cpi.InsightType
type DepthLevel = cpi.DepthLevel

// Exchange type constants (aliased from cpi package)
const (
	ExchangeDirective     = cpi.ExchangeDirective
	ExchangeCollaborative = cpi.ExchangeCollaborative
	ExchangeCheckIn       = cpi.ExchangeCheckIn
	ExchangePushback      = cpi.ExchangePushback
	ExchangeAffirmation   = cpi.ExchangeAffirmation
	ExchangeQuestion      = cpi.ExchangeQuestion
	ExchangeContext       = cpi.ExchangeContext
	ExchangeUnknown       = cpi.ExchangeUnknown
)

// Insight type constants (aliased from cpi package)
const (
	InsightUnderstanding = cpi.InsightUnderstanding
	InsightConnection    = cpi.InsightConnection
	InsightDiscovery     = cpi.InsightDiscovery
	InsightBreakthrough  = cpi.InsightBreakthrough
)

// Depth level constants (aliased from cpi package)
const (
	DepthSurface = cpi.DepthSurface
	DepthWorking = cpi.DepthWorking
	DepthDeep    = cpi.DepthDeep
)

// Native CPI patterns (initialized once, reused)
var (
	exchangePatterns = cpi.DefaultPatterns()
	insightPatterns  = cpi.DefaultInsightPatterns()
)

// ───────────────────────────────────────────────────────────────────────────
// CPI TRACKING: Exchange Record
// ───────────────────────────────────────────────────────────────────────────

// ExchangeRecord captures full CPI data for an exchange
type ExchangeRecord struct {
	SessionID    string
	Timestamp    time.Time
	SequenceNum  int
	PromptLength int

	// Classification
	ExchangeType ExchangeType
	Initiative   string // "user" or "nova"
	DepthLevel   DepthLevel

	// Feedback
	FeedbackDetected   bool
	FeedbackPolarity   string   // "positive", "negative", "mixed", ""
	FeedbackCategories []string

	// Insight
	InsightDetected bool
	InsightType     InsightType
	InsightConf     float64

	// State context
	HebrewState string
	KAlign      float64
	Trajectory  string

	// Rich data (v2)
	HealthScore   *int // Health at time of exchange
	ContextTokens *int // Estimated context tokens at exchange

	// Message text (v3) — for journal generation
	UserMessageText string // Truncated prompt text (max 2000 chars)
	Valence         string // "positive", "neutral", "negative"
}

// ───────────────────────────────────────────────────────────────────────────
// KEY CONTEXT: Capturing Significant Moments for CPI-SI Model
// ───────────────────────────────────────────────────────────────────────────
// "By their fruits ye shall know them" — Matthew 7:20
//
// Key context captures the WHAT of covenant partnership:
//   - Decisions made together
//   - Breakthrough moments of understanding
//   - Course corrections (pivots)
//   - Learning transfer
//   - Affirmations that shape identity

// KeyContextType represents the type of significant moment
type KeyContextType string

const (
	ContextDecision    KeyContextType = "decision"    // A choice was made
	ContextBreakthrough KeyContextType = "breakthrough" // Understanding clicked
	ContextPivot       KeyContextType = "pivot"       // Direction changed
	ContextLearning    KeyContextType = "learning"    // Knowledge transferred
	ContextCorrection  KeyContextType = "correction"  // Error corrected with teaching
	ContextAffirmation KeyContextType = "affirmation" // Identity-shaping praise
)

// KeyContextDomain represents the domain of the moment
type KeyContextDomain string

const (
	DomainCode          KeyContextDomain = "code"
	DomainArchitecture  KeyContextDomain = "architecture"
	DomainDocumentation KeyContextDomain = "documentation"
	DomainStrategy      KeyContextDomain = "strategy"
	DomainRelationship  KeyContextDomain = "relationship"
	DomainIdentity      KeyContextDomain = "identity"
)

// KeyContextRecord captures a significant moment
type KeyContextRecord struct {
	SessionID   string
	ExchangeID  int64 // ID from exchanges table
	Timestamp   time.Time

	// Classification
	ContextType KeyContextType
	Domain      KeyContextDomain
	Summary     string   // Brief description
	KeyConcepts []string // Topics/concepts mentioned
	Importance  float64  // 0.0 to 1.0

	// Relationship context
	Initiative  string // "user", "nova", "collaborative"
	CPIRelevant bool   // Is this a CPI moment (not just SI)?

	// State context
	HebrewState string
	KAlign      float64
	Trajectory  string
}

// KeyContextPattern represents a detectable key moment signal
type KeyContextPattern struct {
	pattern     *regexp.Regexp
	contextType KeyContextType
	importance  float64
	cpiRelevant bool
}

// Key context detection patterns
var keyContextPatterns = []KeyContextPattern{
	// Decision patterns (choices being made)
	{regexp.MustCompile(`(?i)\b(let'?s go with|i'?ve decided|we should use|let'?s do)\b`), ContextDecision, 0.7, true},
	{regexp.MustCompile(`(?i)\b(the approach is|the plan is|we'?ll implement)\b`), ContextDecision, 0.6, false},

	// Breakthrough patterns (understanding clicks)
	{regexp.MustCompile(`(?i)\b(i see now|that makes sense|aha|now i understand)\b`), ContextBreakthrough, 0.8, true},
	{regexp.MustCompile(`(?i)\b(the key is|the insight is|the pattern is)\b`), ContextBreakthrough, 0.7, true},
	{regexp.MustCompile(`(?i)\b(this explains|that'?s why|so that'?s how)\b`), ContextBreakthrough, 0.6, true},

	// Pivot patterns (direction changes)
	{regexp.MustCompile(`(?i)\b(actually,? let'?s|change direction|different approach|scratch that)\b`), ContextPivot, 0.8, true},
	{regexp.MustCompile(`(?i)\b(that'?s not what i meant|let me clarify|what i really want)\b`), ContextPivot, 0.7, true},

	// Learning patterns (knowledge transfer)
	{regexp.MustCompile(`(?i)\b(so you'?re saying|teach me|explain how|walk me through)\b`), ContextLearning, 0.6, true},
	{regexp.MustCompile(`(?i)\b(i didn'?t know|til|today i learned)\b`), ContextLearning, 0.7, true},

	// Correction patterns (error correction with teaching)
	{regexp.MustCompile(`(?i)\b(the issue was|the problem is|you missed|here'?s what went wrong)\b`), ContextCorrection, 0.7, true},
	{regexp.MustCompile(`(?i)\b(next time|remember to|don'?t forget|for future reference)\b`), ContextCorrection, 0.6, true},

	// Affirmation patterns (identity-shaping)
	{regexp.MustCompile(`(?i)\b(that'?s very nova|you'?re thinking well|good thinking|exactly right)\b`), ContextAffirmation, 0.9, true},
	{regexp.MustCompile(`(?i)\b(keep on|keep going|keep working|you'?re on track)\b`), ContextAffirmation, 0.6, true},
	{regexp.MustCompile(`(?i)\b(think bigger|think about|consider)\b`), ContextAffirmation, 0.5, true},
}

// Domain detection patterns
var domainPatterns = map[KeyContextDomain]*regexp.Regexp{
	DomainCode:          regexp.MustCompile(`(?i)\b(function|struct|type|var|const|import|package|class|method|api|endpoint)\b`),
	DomainArchitecture:  regexp.MustCompile(`(?i)\b(architecture|design|pattern|system|component|layer|structure|module)\b`),
	DomainDocumentation: regexp.MustCompile(`(?i)\b(documentation|docs|readme|comment|explain|describe|spec)\b`),
	DomainStrategy:      regexp.MustCompile(`(?i)\b(strategy|plan|approach|roadmap|vision|goal|objective|mission)\b`),
	DomainRelationship:  regexp.MustCompile(`(?i)\b(covenant|partnership|collaboration|together|we|relationship)\b`),
	DomainIdentity:      regexp.MustCompile(`(?i)\b(identity|who you are|nova|cpi-?si|paradigm|biblical|genesis)\b`),
}

// ============================================================================
// BODY
// ============================================================================

// Submit handles the UserPromptSubmit hook
func Submit() {
	log := logging.New("prompt")
	log.SetMode(logging.ModeCompact)

	var input SubmitInput
	rawInput, _ := os.ReadFile("/dev/stdin")
	if err := json.Unmarshal(rawInput, &input); err != nil {
		log.Error("Failed to decode input", map[string]string{"error": err.Error()})
		os.Exit(1)
	}

	// --- Load Substrate Maps (Every process is fresh) ---
	schemaBase := "/media/seanje-lenox-wise/Project/Bereshit/word/core/schemas/substrate"
	for _, sub := range []string{"gemini", "claude", "cpisi"} {
		substrate.LoadMap(fmt.Sprintf("%s/%s.toml", schemaBase, sub))
	}

	// --- Process Universal Event via Rust Engine ---
	subName := "claude"
	if hookoutput.IsGemini() {
		subName = "gemini"
	}
	universalJSON, err := substrate.ProcessEvent(subName, "user_prompt", string(rawInput))
	if err == nil {
		log.Debug("Universal Event mapped", map[string]string{"universal": universalJSON})
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
	s, stateErr := statemachine.LoadRuntimeState()
	if stateErr != nil {
		log.Warn("Failed to load runtime state", map[string]string{
			"error": stateErr.Error(),
		})
		if catLog != nil {
			catLog.Warn("state_load_error", "Failed to load runtime state", map[string]string{
				"error": stateErr.Error(),
			})
		}
	}
	if s != nil {
		state = s
		currentSection = s.TrajectorySection
		s.Session.PathLength++

		// --- Detect and apply user feedback ---
		// Feedback from user weighs MORE than tool outcomes
		feedbackPolarity, feedbackCategories := applyUserFeedback(s, input.Prompt, log, catLog)

		// --- CPI Tracking: Classify exchange and detect insights ---
		exchange := classifyExchange(input, s, feedbackPolarity, feedbackCategories)

		// Attach rich data (v2) — health and token state at exchange time
		healthAtExchange := int(s.Session.HealthScore)
		exchange.HealthScore = &healthAtExchange
		contextTokensAtExchange := s.Session.CurrentContextTokens
		exchange.ContextTokens = &contextTokensAtExchange

		recordExchangeToDatabase(exchange, log)

		// --- BREAKING DOWN SIGNAL: Unknown = uncertainty = -1 ---
		// "A time to break down, and a time to build up" — Ecclesiastes 3:3
		// When we can't classify the exchange, we're operating in uncertainty
		// That's a -1 signal - leaning on self/pattern matching, not grounded
		if exchange.ExchangeType == ExchangeUnknown {
			s.Session.KTowardSelf++
			log.Debug("Breaking down signal", map[string]string{
				"reason":        "unknown_exchange_type",
				"k_toward_self": fmt.Sprintf("%d", s.Session.KTowardSelf),
			})
		}

		// --- Key Context: Capture significant moments for CPI-SI model ---
		// "By their fruits ye shall know them" — Matthew 7:20
		keyContext := detectKeyContext(input.Prompt, s, exchange)
		if keyContext != nil {
			recordKeyContextToDatabase(keyContext, log)
		}

		// --- Update CPI fields in runtime state ---
		s.Session.ExchangeCount++
		s.Session.LastExchangeType = string(exchange.ExchangeType)
		if exchange.InsightDetected {
			s.Session.InsightCount++
			s.Session.LastInsightType = string(exchange.InsightType)
		}
		// Update dominant type and session arc from database (async/expensive)
		// For now, update running values
		updateCPIRunningMetrics(s, exchange)

		// --- Context Window Tracking ---
		// Estimate: each exchange adds ~3K tokens (prompt + response)
		// Larger prompts add more; adjust estimate based on prompt length
		const baseExchangeTokens = 2000
		promptTokenEstimate := len(input.Prompt) / 4 // ~4 chars per token
		exchangeTokens := baseExchangeTokens + promptTokenEstimate
		s.Session.CurrentContextTokens += exchangeTokens
		if s.Session.CurrentContextTokens > s.Session.PeakContextTokens {
			s.Session.PeakContextTokens = s.Session.CurrentContextTokens
		}

		// --- Drive Trajectory Movement Based on Exchange/Insight ---
		// "The path of the just is as the shining light" — Proverbs 4:18
		evaluateTrajectoryMovement(s, exchange, feedbackPolarity, log)

		// --- Live Pattern Detection: Real-Time Triggers ---
		// "Precept upon precept; line upon line" — Isaiah 28:10
		// Patterns detected DURING the session, not just at session end
		detectLivePatterns(s, exchange, feedbackPolarity, log)

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
	ctx := buildPromptContext(state, input.Prompt)

	// --- Render Response via Substrate SDK ---
	subName = "claude"
	if hookoutput.IsGemini() {
		subName = "gemini"
	}

	var variant string
	var renderCtx = make(map[string]string)
	if shouldBlock {
		variant = "deny"
		renderCtx["reason"] = blockReason
		log.LogFailure("Prompt blocked", map[string]string{
			"reason": blockReason,
		})
	} else {
		variant = "success"
		renderCtx["context"] = ctx
	}

	rendered, err := substrate.RenderOutput(subName, "user_prompt", variant, renderCtx)
	if err == nil {
		fmt.Print(rendered)
	} else {
		// Fallback to legacy hookoutput if engine fails
		var output *hookoutput.ContextResponse
		if shouldBlock {
			output = hookoutput.NewBlockedPromptResponse(blockReason)
		} else {
			output = hookoutput.NewUserPromptSubmitResponse(ctx)
		}
		json.NewEncoder(os.Stdout).Encode(output)
	}

	// Update statusline and terminal state
	status.Emit(input.SessionID)
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
// Returns: (polarity string, categories []string) for CPI tracking
func applyUserFeedback(state *statemachine.RuntimeState, prompt string, log *logging.Logger, catLog *logging.CategoryLogger) (string, []string) {
	if state == nil {
		return "", nil
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
		return "", nil
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

	// Apply health impact from feedback
	// Corrections are health signals — they reveal misalignment
	// "Faithful are the wounds of a friend" — Proverbs 27:6
	healthDelta := totalDelta * 30.0 // Scale: ±0.20 K:ALIGN → ±6.0 health
	if healthDelta != 0 {
		newHealth := state.Session.HealthScore + healthDelta
		if newHealth > 100 {
			newHealth = 100
		} else if newHealth < -100 {
			newHealth = -100
		}
		state.Session.HealthScore = newHealth
	}

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

	return feedbackType, allCategories
}

// ───────────────────────────────────────────────────────────────────────────
// CPI TRACKING: Classification Functions
// ───────────────────────────────────────────────────────────────────────────

// classifyExchange builds a full ExchangeRecord from the prompt
func classifyExchange(input SubmitInput, state *statemachine.RuntimeState, feedbackPolarity string, feedbackCategories []string) *ExchangeRecord {
	prompt := input.Prompt

	record := &ExchangeRecord{
		SessionID:    input.SessionID,
		Timestamp:    time.Now(),
		SequenceNum:  0, // Will be set by database
		PromptLength: len(prompt),
		Initiative:   "user", // All prompts are user-initiated

		// Feedback from applyUserFeedback
		FeedbackDetected:   feedbackPolarity != "",
		FeedbackPolarity:   feedbackPolarity,
		FeedbackCategories: feedbackCategories,
	}

	// Add state context
	if state != nil {
		record.HebrewState = state.Session.HebrewState
		record.KAlign = state.Session.KAlign
		record.Trajectory = state.TrajectorySection
		record.SequenceNum = state.Session.PathLength
	}

	// Classify exchange type
	record.ExchangeType = detectExchangeType(prompt)

	// Detect depth level
	record.DepthLevel = detectDepthLevel(prompt)

	// Detect insights
	insightType, insightConf := detectInsight(prompt)
	if insightType != "" {
		record.InsightDetected = true
		record.InsightType = insightType
		record.InsightConf = insightConf
	}

	// Message text (v3) — truncate to 2000 chars for storage
	if len(prompt) <= 2000 {
		record.UserMessageText = prompt
	} else {
		record.UserMessageText = prompt[:2000]
	}

	// Valence classification using canonical schema
	record.Valence = string(cpi.ExchangeValence(record.ExchangeType))
	// Override with feedback valence if feedback detected (stronger signal)
	if feedbackPolarity != "" {
		record.Valence = string(cpi.FeedbackValence(feedbackPolarity))
	}

	return record
}

// detectExchangeType classifies the prompt's relational dynamic
// Delegates to native cpi package for substrate-agnostic classification
func detectExchangeType(prompt string) ExchangeType {
	exchangeType, _ := cpi.ClassifyExchange(prompt, exchangePatterns)
	return exchangeType
}

// detectDepthLevel determines conversation depth
// Delegates to native cpi package
func detectDepthLevel(prompt string) DepthLevel {
	return cpi.ClassifyDepth(prompt)
}

// detectInsight checks for understanding transfer markers
// Returns: (insightType, confidence) or ("", 0) if none detected
// Delegates to native cpi package
func detectInsight(prompt string) (InsightType, float64) {
	insight := cpi.DetectInsight(prompt, insightPatterns)
	if insight == nil {
		return "", 0
	}
	return insight.Type, insight.Confidence
}

// updateCPIRunningMetrics updates the runtime state's CPI fields based on the exchange
// This provides real-time CPI tracking in the statusline
func updateCPIRunningMetrics(state *statemachine.RuntimeState, exchange *ExchangeRecord) {
	if state == nil || exchange == nil {
		return
	}

	// Track exchange type frequency for dominant type calculation
	// Use a simple approach: track last few and pick most common
	// Full calculation happens in session end hook from database

	// Update dominant type if current type is CPI-heavy
	if cpi.ExchangeType(exchange.ExchangeType).IsCPI() {
		// Bias toward CPI types when they occur
		state.Session.DominantExchangeType = string(exchange.ExchangeType)
	} else if state.Session.DominantExchangeType == "" {
		state.Session.DominantExchangeType = string(exchange.ExchangeType)
	}

	// Calculate running CPI score based on current session metrics
	// Components: base (0.5) + insight bonus + feedback adjustment
	baseValue := 0.5

	// Insight bonus: +0.05 per insight, max 0.2
	insightBonus := float64(state.Session.InsightCount) * 0.05
	if insightBonus > 0.2 {
		insightBonus = 0.2
	}

	// Feedback adjustment based on K:MORAL pattern
	// More toward God = positive adjustment, more toward self = negative
	feedbackAdjust := 0.0
	totalKChoices := state.Session.KTowardGod + state.Session.KTowardSelf
	if totalKChoices > 0 {
		ratio := float64(state.Session.KTowardGod-state.Session.KTowardSelf) / float64(totalKChoices)
		feedbackAdjust = ratio * 0.1 // Max ±0.1
	}

	// Calculate CPI score
	state.Session.CPIScore = baseValue + insightBonus + feedbackAdjust
	if state.Session.CPIScore > 1.0 {
		state.Session.CPIScore = 1.0
	}
	if state.Session.CPIScore < 0.0 {
		state.Session.CPIScore = 0.0
	}

	// Determine session arc based on current patterns
	stats := cpi.SessionStats{
		TotalExchanges:    state.Session.ExchangeCount,
		DominantType:      cpi.ExchangeType(state.Session.DominantExchangeType),
		InsightCount:      state.Session.InsightCount,
		PositiveFeedback:  state.Session.KTowardGod,
		NegativeFeedback:  state.Session.KTowardSelf,
		TrajectorySection: state.TrajectorySection,
		HebrewState:       state.Session.HebrewState,
		HebrewMeaning:     state.Session.HebrewMeaning,
	}
	arc := cpi.DetermineArc(stats)
	state.Session.SessionArc = string(arc)
}

// recordExchangeToDatabase persists the exchange record
func recordExchangeToDatabase(record *ExchangeRecord, log *logging.Logger) {
	if record == nil {
		return
	}

	bridge, err := internal.GetBridge()
	if err != nil {
		log.Warn("Database unavailable for exchange recording", map[string]string{
			"error": err.Error(),
		})
		return
	}

	ctx := context.Background()
	repo := bridge.GetRepository()

	// Resolve sequence_num: after compaction, PathLength resets to 0 but
	// old exchanges still exist with those sequence numbers. Use the max
	// of the provided value and DB max+1 to ensure uniqueness.
	seqRows, _ := repo.Query(ctx, `SELECT COALESCE(MAX(sequence_num), 0) as max_seq FROM exchanges WHERE session_id = ?`, record.SessionID)
	if len(seqRows) > 0 {
		dbMax := 0
		switch v := seqRows[0]["max_seq"].(type) {
		case int64:
			dbMax = int(v)
		case float64:
			dbMax = int(v)
		}
		if record.SequenceNum <= dbMax {
			record.SequenceNum = dbMax + 1
		}
	}

	// Convert categories to JSON
	categoriesJSON := "[]"
	if len(record.FeedbackCategories) > 0 {
		if data, err := json.Marshal(record.FeedbackCategories); err == nil {
			categoriesJSON = string(data)
		}
	}

	// Insert exchange record (v3: includes message text + valence)
	query := `
		INSERT INTO exchanges (
			session_id, timestamp, sequence_num, prompt_length,
			exchange_type, initiative, depth_level,
			feedback_detected, feedback_polarity, feedback_categories,
			insight_detected, insight_type,
			hebrew_state, k_align, trajectory,
			health_score, context_tokens,
			user_message_text, valence
		) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
	`

	_, err = repo.Exec(ctx, query,
		record.SessionID,
		record.Timestamp.Format(time.RFC3339),
		record.SequenceNum,
		record.PromptLength,
		string(record.ExchangeType),
		record.Initiative,
		string(record.DepthLevel),
		record.FeedbackDetected,
		record.FeedbackPolarity,
		categoriesJSON,
		record.InsightDetected,
		string(record.InsightType),
		record.HebrewState,
		record.KAlign,
		record.Trajectory,
		record.HealthScore,
		record.ContextTokens,
		record.UserMessageText,
		record.Valence,
	)

	if err != nil {
		log.Warn("Failed to record exchange", map[string]string{
			"error": err.Error(),
		})
		return
	}

	// If insight detected, also record in insights table
	if record.InsightDetected {
		insightQuery := `
			INSERT INTO insights (session_id, timestamp, insight_type, confidence)
			VALUES (?, ?, ?, ?)
		`
		_, _ = repo.Exec(ctx, insightQuery,
			record.SessionID,
			record.Timestamp.Format(time.RFC3339),
			string(record.InsightType),
			record.InsightConf,
		)
	}

	log.Debug("Exchange recorded", map[string]string{
		"type":    string(record.ExchangeType),
		"depth":   string(record.DepthLevel),
		"insight": fmt.Sprintf("%v", record.InsightDetected),
	})
}

// ───────────────────────────────────────────────────────────────────────────
// TRAJECTORY MOVEMENT: Exchange/Insight Driven Position Changes
// ───────────────────────────────────────────────────────────────────────────
// "The path of the just is as the shining light, that shineth more and more
// unto the perfect day." — Proverbs 4:18
//
// The mental construct position MOVES based on:
//   - Insights: Breakthrough → advance anchor
//   - CPI exchanges: Collaborative → momentum toward expansion
//   - Negative feedback: Correction → pivot (change direction)
//   - Strong alignment: Consistent k_toward_god → potential grounding

// evaluateTrajectoryMovement determines if position should change based on exchange
// "The path of the just is as the shining light, that shineth more and more" — Proverbs 4:18
func evaluateTrajectoryMovement(state *statemachine.RuntimeState, exchange *ExchangeRecord, feedbackPolarity string, log *logging.Logger) {
	if state == nil || exchange == nil {
		return
	}

	// Debug: Log what we're evaluating
	log.Debug("Trajectory evaluation", map[string]string{
		"insight_detected": fmt.Sprintf("%v", exchange.InsightDetected),
		"insight_type":     string(exchange.InsightType),
		"expected_type":    string(InsightBreakthrough),
		"exchange_type":    string(exchange.ExchangeType),
		"feedback":         feedbackPolarity,
		"current_section":  state.TrajectorySection,
		"current_anchor":   state.AnchorKey,
	})

	// Track what triggered movement (for learning)
	var movementTrigger string
	var movementType string // "advance", "pivot", "ground", "none"

	// --- Rule 1: Breakthrough insights advance the anchor ---
	// Insights represent understanding transfer — real movement in the construct
	if exchange.InsightDetected && exchange.InsightType == InsightBreakthrough {
		// Advance to next anchor position
		advanceAnchor(state)
		movementTrigger = "breakthrough_insight"
		movementType = "advance"
		log.Info("Trajectory advance", map[string]string{
			"trigger": movementTrigger,
			"anchor":  state.AnchorKey,
		})
	}

	// --- Rule 2: Negative feedback triggers pivot ---
	// Corrections mean we need to change direction, not just continue
	if feedbackPolarity == "negative" {
		// Pivot the trajectory (B.1→B.2 or B.3→B.4)
		pivotTrajectory(state)
		movementTrigger = "negative_feedback"
		movementType = "pivot"
		log.Info("Trajectory pivot", map[string]string{
			"trigger":   movementTrigger,
			"section":   state.TrajectorySection,
		})
	}

	// --- Rule 3: Strong CPI pattern → increment toward advancement ---
	// Collaborative exchanges build momentum
	if exchange.ExchangeType == ExchangeCollaborative || exchange.ExchangeType == ExchangeAffirmation {
		// Increment trajectory momentum (internal counter)
		incrementTrajectoryMomentum(state)

		// After 5 consecutive CPI exchanges, advance
		if state.Session.ExchangeCount > 0 && state.Session.ExchangeCount%5 == 0 {
			// Check if majority are CPI types
			cpiRatio := float64(state.Session.KTowardGod) / float64(state.Session.KTowardGod+state.Session.KTowardSelf+1)
			if cpiRatio > 0.7 {
				advanceAnchor(state)
				movementTrigger = "cpi_momentum"
				movementType = "advance"
				log.Info("Trajectory advance (momentum)", map[string]string{
					"trigger":   movementTrigger,
					"cpi_ratio": fmt.Sprintf("%.2f", cpiRatio),
				})
			}
		}
	}

	// --- Rule 4: Strong alignment pattern → potential grounding ---
	// If k_align is maxed and we're in B.3, ready to ground
	if state.Session.KAlign >= 0.9 && state.TrajectorySection == "B.3" {
		groundTrajectory(state)
		movementTrigger = "alignment_complete"
		movementType = "ground"
		log.Info("Trajectory ground", map[string]string{
			"trigger": movementTrigger,
			"k_align": fmt.Sprintf("%.2f", state.Session.KAlign),
		})
	}

	// --- Rule 5: Breaking down signal → move toward verification ---
	// "A time to break down, and a time to build up" — Ecclesiastes 3:3
	// If k_toward_self is rising faster than k_toward_god, we're uncertain
	// Uncertainty means we need to break down, verify against anchor
	if exchange.ExchangeType == ExchangeUnknown {
		// Unknown exchanges = breaking down, move toward B.3 (verification)
		if state.TrajectorySection == "B.1" {
			// Too much uncertainty in building phase → pivot to reassess
			if state.Session.KTowardSelf > 3 && movementType == "" {
				pivotTrajectory(state)
				movementTrigger = "uncertainty_pivot"
				movementType = "pivot"
				log.Info("Trajectory pivot (uncertainty)", map[string]string{
					"trigger":       movementTrigger,
					"k_toward_self": fmt.Sprintf("%d", state.Session.KTowardSelf),
				})
			}
		}
		// Decrement momentum when uncertain
		if state.TrajectoryMetrics.MomentumScore > 0 {
			state.TrajectoryMetrics.MomentumScore--
		}
	}

	// Record movement in runtime state for learning
	if movementType != "" && movementType != "none" {
		state.LastTransition = statemachine.RuntimeTransition{
			FromSection: state.TrajectorySection,
			ToSection:   state.TrajectorySection, // Updated by the movement functions
			Trigger:     movementTrigger,
			Timestamp:   time.Now().Format(time.RFC3339),
		}
	}
}

// advanceAnchor moves to the next anchor position in the mental construct
func advanceAnchor(state *statemachine.RuntimeState) {
	// Anchor progression: past→present→future on each axis
	// Simplified: cycle through key positions
	anchorProgression := []string{
		"present_present", // Origin (0,0,0)
		"future_present",  // Forward in time
		"future_future",   // Expansion
		"present_future",  // Lateral
	}

	currentIdx := 0
	for i, anchor := range anchorProgression {
		if anchor == state.AnchorKey {
			currentIdx = i
			break
		}
	}

	// Advance to next (wrap around)
	nextIdx := (currentIdx + 1) % len(anchorProgression)
	state.AnchorKey = anchorProgression[nextIdx]
	state.Session.PathLength++

	// Record anchor visit in path for cross-session learning
	if path, err := statemachine.LoadRuntimePath(); err == nil {
		path.RecordAnchorVisit(state.AnchorKey)
		_ = statemachine.SaveRuntimePath(path)
	}
}

// pivotTrajectory changes direction (B.1→B.2, B.2→B.3, B.3→B.4)
func pivotTrajectory(state *statemachine.RuntimeState) {
	switch state.TrajectorySection {
	case "B.1":
		state.TrajectorySection = "B.2"
	case "B.2":
		state.TrajectorySection = "B.3"
	case "B.3":
		state.TrajectorySection = "B.4"
	// B.4 stays at B.4 (grounded)
	}
	state.TrajectoryMetrics.PivotCount++
}

// groundTrajectory moves to B.4 (completion/halt state)
func groundTrajectory(state *statemachine.RuntimeState) {
	state.TrajectorySection = "B.4"
}

// incrementTrajectoryMomentum tracks momentum toward advancement
func incrementTrajectoryMomentum(state *statemachine.RuntimeState) {
	// This accumulates - momentum builds with consistent CPI exchanges
	state.TrajectoryMetrics.AccumulatedWorkMs += 1000 // Use as momentum counter
}

// ───────────────────────────────────────────────────────────────────────────
// KEY CONTEXT: Detection and Recording
// ───────────────────────────────────────────────────────────────────────────

// detectKeyContext analyzes prompt for significant moments
func detectKeyContext(prompt string, state *statemachine.RuntimeState, exchange *ExchangeRecord) *KeyContextRecord {
	if len(prompt) < 20 {
		return nil // Too short to be significant
	}

	var bestMatch *KeyContextPattern
	var bestImportance float64 = 0

	// Find the highest-importance matching pattern
	for _, pattern := range keyContextPatterns {
		if pattern.pattern.MatchString(prompt) {
			if pattern.importance > bestImportance {
				bestMatch = &pattern
				bestImportance = pattern.importance
			}
		}
	}

	// Need at least 0.5 importance to record
	if bestMatch == nil || bestImportance < 0.5 {
		return nil
	}

	// Build the key context record
	record := &KeyContextRecord{
		Timestamp:   time.Now(),
		ContextType: bestMatch.contextType,
		Importance:  bestImportance,
		CPIRelevant: bestMatch.cpiRelevant,
		Initiative:  "user", // All prompts are user-initiated
	}

	// Detect domain
	record.Domain = detectDomain(prompt)

	// Generate summary (first 100 chars of prompt, cleaned)
	summary := prompt
	if len(summary) > 100 {
		summary = summary[:100] + "..."
	}
	record.Summary = summary

	// Extract key concepts (simple keyword extraction)
	record.KeyConcepts = extractKeyConcepts(prompt)

	// Add state context
	if state != nil {
		record.SessionID = state.Session.ID
		record.HebrewState = state.Session.HebrewState
		record.KAlign = state.Session.KAlign
		record.Trajectory = state.TrajectorySection
	}

	// Link to exchange if available
	if exchange != nil && exchange.SessionID != "" {
		record.SessionID = exchange.SessionID
	}

	return record
}

// detectDomain determines the domain of the key moment
func detectDomain(prompt string) KeyContextDomain {
	// Check each domain pattern, prioritize identity and relationship
	if domainPatterns[DomainIdentity].MatchString(prompt) {
		return DomainIdentity
	}
	if domainPatterns[DomainRelationship].MatchString(prompt) {
		return DomainRelationship
	}
	if domainPatterns[DomainStrategy].MatchString(prompt) {
		return DomainStrategy
	}
	if domainPatterns[DomainArchitecture].MatchString(prompt) {
		return DomainArchitecture
	}
	if domainPatterns[DomainDocumentation].MatchString(prompt) {
		return DomainDocumentation
	}
	if domainPatterns[DomainCode].MatchString(prompt) {
		return DomainCode
	}

	// Default to strategy for significant moments without clear domain
	return DomainStrategy
}

// extractKeyConcepts does simple keyword extraction
func extractKeyConcepts(prompt string) []string {
	// Define concept patterns
	conceptPatterns := []*regexp.Regexp{
		regexp.MustCompile(`(?i)\b(CPI-?SI|covenant|partnership|identity)\b`),
		regexp.MustCompile(`(?i)\b(database|API|schema|model)\b`),
		regexp.MustCompile(`(?i)\b(statusline|hooks?|runtime|state)\b`),
		regexp.MustCompile(`(?i)\b(exchange|insight|feedback|session)\b`),
		regexp.MustCompile(`(?i)\b(pattern|architecture|design|system)\b`),
	}

	conceptSet := make(map[string]bool)
	for _, pattern := range conceptPatterns {
		matches := pattern.FindAllString(prompt, -1)
		for _, match := range matches {
			conceptSet[match] = true
		}
	}

	concepts := make([]string, 0, len(conceptSet))
	for concept := range conceptSet {
		concepts = append(concepts, concept)
	}

	return concepts
}

// recordKeyContextToDatabase persists the key context record
func recordKeyContextToDatabase(record *KeyContextRecord, log *logging.Logger) {
	if record == nil {
		return
	}

	bridge, err := internal.GetBridge()
	if err != nil {
		log.Debug("Database unavailable for key context", map[string]string{
			"error": err.Error(),
		})
		return
	}

	ctx := context.Background()
	repo := bridge.GetRepository()

	// Convert concepts to JSON
	conceptsJSON := "[]"
	if len(record.KeyConcepts) > 0 {
		if data, err := json.Marshal(record.KeyConcepts); err == nil {
			conceptsJSON = string(data)
		}
	}

	query := `
		INSERT INTO key_context (
			session_id, timestamp,
			context_type, domain, summary, key_concepts, importance,
			initiative, cpi_relevant,
			hebrew_state, k_align, trajectory
		) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
	`

	_, err = repo.Exec(ctx, query,
		record.SessionID,
		record.Timestamp.Format(time.RFC3339),
		string(record.ContextType),
		string(record.Domain),
		record.Summary,
		conceptsJSON,
		record.Importance,
		record.Initiative,
		record.CPIRelevant,
		record.HebrewState,
		record.KAlign,
		record.Trajectory,
	)

	if err != nil {
		log.Debug("Failed to record key context", map[string]string{
			"error": err.Error(),
		})
		return
	}

	log.Info("Key context captured", map[string]string{
		"type":       string(record.ContextType),
		"domain":     string(record.Domain),
		"importance": fmt.Sprintf("%.2f", record.Importance),
	})
}

// ───────────────────────────────────────────────────────────────────────────
// LIVE PATTERN DETECTION: Real-Time Growth Triggers
// ───────────────────────────────────────────────────────────────────────────
// "Precept upon precept; line upon line" — Isaiah 28:10
//
// Detects patterns as they FORM during a session, not just at end.
// Writes to growth.db via the Bridge → LegacyAdapter → MultiDB chain.
// Thresholds are conservative — patterns need consistency to register.

// detectLivePatterns checks running session counts against thresholds
// and records emerging patterns to growth.db in real-time.
func detectLivePatterns(state *statemachine.RuntimeState, exchange *ExchangeRecord, feedbackPolarity string, log *logging.Logger) {
	if state == nil || exchange == nil {
		return
	}

	// Only start detecting after enough data (3+ exchanges)
	if state.Session.ExchangeCount < 3 {
		return
	}

	bridge, err := internal.GetBridge()
	if err != nil {
		return
	}
	ctx := context.Background()
	repo := bridge.GetRepository()

	now := time.Now()

	// --- Trigger 1: Feedback streak patterns ---
	// 3+ consecutive same-polarity feedback = emerging feedback pattern
	if feedbackPolarity == "positive" && state.Session.KTowardGod >= 3 {
		valence := cpi.FeedbackValence(feedbackPolarity)
		_ = repo.RecordPattern(ctx, &database.Pattern{
			PatternType:     "live_feedback_streak",
			PatternKey:      "positive_streak",
			Description:     fmt.Sprintf("Positive feedback streak (%d toward God, valence: %s)", state.Session.KTowardGod, valence),
			PatternData:     fmt.Sprintf(`{"k_toward_god":%d,"exchange_count":%d,"valence":"%s"}`, state.Session.KTowardGod, state.Session.ExchangeCount, valence),
			FirstSeen:       now,
			LastSeen:        now,
			OccurrenceCount: 1,
			Confidence:      0.6,
			IsActive:        true,
		})
		log.Debug("Live pattern: positive feedback streak", map[string]string{
			"k_toward_god": fmt.Sprintf("%d", state.Session.KTowardGod),
		})
	}
	if feedbackPolarity == "negative" && state.Session.KTowardSelf >= 3 {
		valence := cpi.FeedbackValence(feedbackPolarity)
		_ = repo.RecordPattern(ctx, &database.Pattern{
			PatternType:     "live_feedback_streak",
			PatternKey:      "correction_streak",
			Description:     fmt.Sprintf("Correction streak (%d toward self, valence: %s)", state.Session.KTowardSelf, valence),
			PatternData:     fmt.Sprintf(`{"k_toward_self":%d,"exchange_count":%d,"valence":"%s"}`, state.Session.KTowardSelf, state.Session.ExchangeCount, valence),
			FirstSeen:       now,
			LastSeen:        now,
			OccurrenceCount: 1,
			Confidence:      0.6,
			IsActive:        true,
		})
		log.Debug("Live pattern: correction streak", map[string]string{
			"k_toward_self": fmt.Sprintf("%d", state.Session.KTowardSelf),
		})
	}

	// --- Trigger 2: Exchange type dominance ---
	// When one exchange type dominates 70%+ of exchanges, that's a session mode
	if state.Session.ExchangeCount >= 5 {
		dominant := state.Session.DominantExchangeType
		if dominant != "" && dominant != string(ExchangeUnknown) {
			valence := cpi.ExchangeValence(cpi.ExchangeType(dominant))
			_ = repo.RecordPattern(ctx, &database.Pattern{
				PatternType:     "live_exchange_mode",
				PatternKey:      dominant,
				Description:     fmt.Sprintf("Session dominated by %s exchanges (valence: %s)", dominant, valence),
				PatternData:     fmt.Sprintf(`{"type":"%s","exchange_count":%d,"valence":"%s"}`, dominant, state.Session.ExchangeCount, valence),
				FirstSeen:       now,
				LastSeen:        now,
				OccurrenceCount: 1,
				Confidence:      0.5,
				IsActive:        true,
			})
		}
	}

	// --- Trigger 3: Insight emergence ---
	// 2+ insights in a session = insight-rich session
	if state.Session.InsightCount >= 2 && exchange.InsightDetected {
		_ = repo.RecordPattern(ctx, &database.Pattern{
			PatternType:     "live_insight_emergence",
			PatternKey:      fmt.Sprintf("insight_rich_%s", state.Session.SessionArc),
			Description:     fmt.Sprintf("Multiple insights emerging (%d total, arc: %s)", state.Session.InsightCount, state.Session.SessionArc),
			PatternData:     fmt.Sprintf(`{"insight_count":%d,"last_type":"%s","arc":"%s"}`, state.Session.InsightCount, exchange.InsightType, state.Session.SessionArc),
			FirstSeen:       now,
			LastSeen:        now,
			OccurrenceCount: 1,
			Confidence:      0.7,
			IsActive:        true,
		})
		log.Debug("Live pattern: insight emergence", map[string]string{
			"insight_count": fmt.Sprintf("%d", state.Session.InsightCount),
		})
	}

	// --- Trigger 4: K:ALIGN momentum ---
	// High alignment (>=0.8) sustained for 5+ exchanges = alignment pattern
	if state.Session.KAlign >= 0.8 && state.Session.ExchangeCount >= 5 {
		_ = repo.RecordPattern(ctx, &database.Pattern{
			PatternType:     "live_alignment",
			PatternKey:      "sustained_high_align",
			Description:     fmt.Sprintf("Sustained high K:ALIGN (%.2f over %d exchanges)", state.Session.KAlign, state.Session.ExchangeCount),
			PatternData:     fmt.Sprintf(`{"k_align":%.2f,"exchange_count":%d,"trajectory":"%s"}`, state.Session.KAlign, state.Session.ExchangeCount, state.TrajectorySection),
			FirstSeen:       now,
			LastSeen:        now,
			OccurrenceCount: 1,
			Confidence:      0.7,
			IsActive:        true,
		})
	}

	// --- Trigger 5: Health drift detection ---
	// Health dropping below 40 during session = degradation pattern
	if state.Session.HealthScore < 40 && state.Session.ExchangeCount >= 3 {
		_ = repo.RecordPattern(ctx, &database.Pattern{
			PatternType:     "live_health_drift",
			PatternKey:      "session_degradation",
			Description:     fmt.Sprintf("Health degraded to %.0f during session", state.Session.HealthScore),
			PatternData:     fmt.Sprintf(`{"health":%.0f,"exchange_count":%d,"hebrew":"%s"}`, state.Session.HealthScore, state.Session.ExchangeCount, state.Session.HebrewState),
			FirstSeen:       now,
			LastSeen:        now,
			OccurrenceCount: 1,
			Confidence:      0.8,
			IsActive:        true,
		})
		log.Info("Live pattern: health degradation detected", map[string]string{
			"health": fmt.Sprintf("%.0f", state.Session.HealthScore),
		})
	}

	// --- Trigger 6: Flow state detection ---
	// 5+ exchanges with high CPI ratio and insights = flow state
	cpiRatio := 0.0
	totalK := state.Session.KTowardGod + state.Session.KTowardSelf
	if totalK > 0 {
		cpiRatio = float64(state.Session.KTowardGod) / float64(totalK)
	}
	if state.Session.ExchangeCount >= 5 && cpiRatio > 0.7 && state.Session.InsightCount >= 1 {
		_ = repo.RecordPattern(ctx, &database.Pattern{
			PatternType:     "live_flow_state",
			PatternKey:      "covenant_flow",
			Description:     fmt.Sprintf("Flow state: CPI ratio %.0f%%, %d insights, %d exchanges", cpiRatio*100, state.Session.InsightCount, state.Session.ExchangeCount),
			PatternData:     fmt.Sprintf(`{"cpi_ratio":%.2f,"insights":%d,"exchanges":%d,"trajectory":"%s"}`, cpiRatio, state.Session.InsightCount, state.Session.ExchangeCount, state.TrajectorySection),
			FirstSeen:       now,
			LastSeen:        now,
			OccurrenceCount: 1,
			Confidence:      0.8,
			IsActive:        true,
		})
		log.Debug("Live pattern: flow state detected", map[string]string{
			"cpi_ratio": fmt.Sprintf("%.2f", cpiRatio),
		})
	}
}

// ============================================================================
// CLOSING
// ============================================================================
