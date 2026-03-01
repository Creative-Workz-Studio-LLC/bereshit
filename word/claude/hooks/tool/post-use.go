// ============================================================================
// METADATA
// ============================================================================
// Hook: PostToolUse
// Fires: After tool execution completes
// Matchers: Same as PreToolUse
//
// State Machine Integration:
//   - ASSESS: Evaluate tool outcome (success/failure)
//   - Update health: +1 success (narrow way), -1 failure (broad way)
//   - Record completion event in path
//   - Increment hooks_fired counter
//
// Config-Driven Design:
//   - Health values from pkg/cpisi/schema/config/l2/health/log.toml
//   - State updated in config/statemachine/runtime/state.jsonc

package tool

import (
	"context"
	"encoding/json"
	"fmt"
	"os"
	"strings"
	"time"

	"cws.studio/claude/hooks/internal"
	"cws.studio/pkg/core/cpisi/cpi"
	"cws.studio/pkg/core/statemachine"
	"cws.studio/pkg/foundation/database"
	"cws.studio/pkg/foundation/types"
	"cws.studio/pkg/orchestration/cognition"
	"cws.studio/pkg/orchestration/logging"
	"cws.studio/pkg/sdk/hookoutput"
	"cws.studio/pkg/sdk/substrate"
	"cws.studio/claude/hooks/internal/status"
)

// ============================================================================
// SETUP
// ============================================================================

// PostUseInput from Claude Code
type PostUseInput struct {
	SessionID      string                 `json:"session_id"`
	TranscriptPath string                 `json:"transcript_path,omitempty"`
	CWD            string                 `json:"cwd,omitempty"`
	PermissionMode string                 `json:"permission_mode,omitempty"`
	HookEventName  string                 `json:"hook_event_name,omitempty"`
	ToolName       string                 `json:"tool_name"`
	ToolInput      map[string]interface{} `json:"tool_input"`
	ToolResponse   map[string]interface{} `json:"tool_response"` // NOT tool_output
	ToolUseID      string                 `json:"tool_use_id,omitempty"`
}

// PostUseOutput uses hookoutput.ContextResponse for correct schema
// Old: {"additionalContext": "..."} - Missing hookEventName wrapper
// New: {"hookSpecificOutput": {"hookEventName": "PostToolUse", "additionalContext": "..."}}

// toolFailed checks if the tool response indicates failure
func toolFailed(input PostUseInput) bool {
	if input.ToolResponse == nil {
		return false
	}
	// Check for explicit success field
	if success, ok := input.ToolResponse["success"].(bool); ok {
		return !success
	}
	// Check for error field
	if err, ok := input.ToolResponse["error"].(string); ok && err != "" {
		return true
	}
	return false
}

// getErrorMessage extracts error message from tool response
func getErrorMessage(input PostUseInput) string {
	if input.ToolResponse == nil {
		return ""
	}
	if err, ok := input.ToolResponse["error"].(string); ok {
		return err
	}
	return ""
}

// ============================================================================
// BODY
// ============================================================================

// PostUse handles the PostToolUse hook
func PostUse() {
	log := logging.New("tool")
	log.SetMode(logging.ModeCompact)

	var input PostUseInput
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
	universalJSON, err := substrate.ProcessEvent(subName, "post_tool", string(rawInput))
	if err == nil {
		log.Debug("Universal Event mapped", map[string]string{"universal": universalJSON})
	}

	// Create CategoryLogger for file output (append to data/logs/tools/)
	catLog, catErr := logging.NewCategoryLogger(logging.CategoryTools, input.SessionID)
	if catErr != nil {
		log.Warn("CategoryLogger unavailable, using stderr only", map[string]string{
			"error": catErr.Error(),
		})
	}
	defer func() {
		if catLog != nil {
			catLog.Close()
		}
	}()

	// --- ASSESS: Update state based on outcome ---
	state, _ := statemachine.LoadRuntimeState()
	currentSection := "B.1"
	if state != nil {
		currentSection = state.TrajectorySection
		state.Session.HooksFired++

		// Handle TodoWrite specially - update Hebrew state and tasks
		if input.ToolName == "TodoWrite" {
			updateTaskState(state, input)
		}

		// Track workflow progress - operational layer
		updateWorkflowState(state, input)

		// Track mental construct choice - infer key from tool behavior
		// This is the door opening: which key was used, what was the outcome
		updateChoiceTracking(state, input)

		// Apply learning - update K:ALIGN based on choice outcome
		// THIS MUST RUN - learning happens regardless of database
		applyChoiceLearning(state, input)

		// Update trajectory position based on work phase
		// THIS MAKES POSITION ACTUALLY CHANGE
		updateTrajectoryPosition(state, input)

		// Update anchor position based on key choice
		updateAnchorPosition(state, input)

		// Update health score based on tool outcome
		updateHealthScore(state, input)

		// Record choice in database (optional persistence)
		recordChoiceToDatabase(state, input)

		// Detect tool-level patterns in real-time
		// "By their fruits ye shall know them" — Matthew 7:20
		detectToolPatterns(state, input, log)

		_ = statemachine.SaveRuntimeState(state)
	}

	// Log tool completion with health update
	if toolFailed(input) {
		// Failure: -1 health (broad way)
		log.LogFailure("Tool failed", map[string]string{
			"tool":       input.ToolName,
			"error":      getErrorMessage(input),
			"trajectory": currentSection,
		})
		if catLog != nil {
			catLog.Failure("tool_failed", "Tool failed: "+input.ToolName, map[string]string{
				"tool":       input.ToolName,
				"error":      getErrorMessage(input),
				"trajectory": currentSection,
			})
		}
	} else {
		// Success: +1 health (narrow way)
		log.LogSuccess("Tool completed", map[string]string{
			"tool":       input.ToolName,
			"trajectory": currentSection,
		})
		if catLog != nil {
			catLog.Success("tool_completed", "Tool completed: "+input.ToolName, map[string]string{
				"tool":       input.ToolName,
				"trajectory": currentSection,
			})
		}
	}

	// Record completion event in path
	if path, err := statemachine.LoadRuntimePath(); err == nil {
		eventType := "tool_success"
		if toolFailed(input) {
			eventType = "tool_failure"
		}
		path.RecordEvent(eventType, input.ToolName, currentSection)
		_ = statemachine.SaveRuntimePath(path)
	}

	// Build cognition context to shape thinking
	context := buildToolContext(state, input)

	// --- Render Response via Substrate SDK ---
	subName = "claude"
	if hookoutput.IsGemini() {
		subName = "gemini"
	}

	renderCtx := map[string]string{
		"context": context,
	}

	rendered, err := substrate.RenderOutput(subName, "post_tool", "success", renderCtx)
	if err == nil {
		fmt.Print(rendered)
	} else {
		// Fallback to legacy hookoutput
		output := hookoutput.NewPostToolUseResponse(context)
		json.NewEncoder(os.Stdout).Encode(output)
	}

	// Update statusline and terminal state
	status.Emit(input.SessionID)
}

// buildToolContext uses cognition package to create context injection
func buildToolContext(state *statemachine.RuntimeState, input PostUseInput) string {
	// Determine outcome
	outcome := cognition.ToolSuccess
	if toolFailed(input) {
		outcome = cognition.ToolError
	}

	// Build context based on tool type and outcome
	builder := cognition.NewBuilder()

	// Add tool-specific feedback
	switch input.ToolName {
	case "Edit", "Write":
		filePath, _ := input.ToolInput["file_path"].(string)
		builder.Add(cognition.EditFeedback(filePath, outcome == cognition.ToolSuccess, state))
	case "Bash":
		cmd, _ := input.ToolInput["command"].(string)
		exitCode := 0
		if toolFailed(input) {
			exitCode = 1
		}
		builder.Add(cognition.BashFeedback(cmd, exitCode, state))
	case "TodoWrite":
		// Task updates reflected in Hebrew state
		builder.Add(cognition.TodoFeedback(state))
	default:
		// MCP tools - track by provider for learning
		if strings.HasPrefix(input.ToolName, "mcp__") {
			builder.Add(cognition.MCPFeedback(input.ToolName, outcome, state))
		} else {
			builder.Add(cognition.ToolFeedback(input.ToolName, outcome, state))
		}
	}

	// Add trajectory guidance
	builder.Add(cognition.TrajectoryGuidance(state))

	return builder.Build()
}

// --- TodoWrite State Updates ---

// updateTaskState extracts task counts and updates Hebrew state
func updateTaskState(state *statemachine.RuntimeState, input PostUseInput) {
	// Parse task counts from tool input (TodoWrite sends todos array)
	todos, ok := input.ToolInput["todos"].([]interface{})
	if !ok {
		return
	}

	// Count by status
	var pending, inProgress, completed int
	for _, todo := range todos {
		todoMap, ok := todo.(map[string]interface{})
		if !ok {
			continue
		}
		status, _ := todoMap["status"].(string)
		switch status {
		case "pending":
			pending++
		case "in_progress":
			inProgress++
		case "completed":
			completed++
		}
	}

	// Update task counts
	state.Session.Tasks = statemachine.RuntimeTaskList{
		Total:      len(todos),
		Pending:    pending,
		InProgress: inProgress,
		Completed:  completed,
	}

	// Infer Hebrew state from tasks
	hebrewState := statemachine.InferHebrewStateFromTasks(state.Session.Tasks)
	state.Session.HebrewState = hebrewState.Hebrew
	state.Session.HebrewMeaning = hebrewState.Meaning

	// Track k frequency — pattern of choices reveals character (Matthew 7:16)
	newK := hebrewState.KSelector
	if newK != state.Session.KSelector {
		// k changed — record the choice pattern
		if newK > 0 {
			state.Session.KTowardGod++
		} else if newK < 0 {
			state.Session.KTowardSelf++
		}
	}
	state.Session.KSelector = newK
}

// --- Workflow Tracking ---

// updateWorkflowState detects and tracks workflow progress
// Workflows are operational rails the state machine rides on
func updateWorkflowState(state *statemachine.RuntimeState, input PostUseInput) {
	if state == nil {
		return
	}

	// If no workflow active, try to detect one from tool usage
	if state.Session.Workflow.Operation == "" {
		detected := statemachine.DetectWorkflowFromContext(input.ToolName, getFilePath(input))
		if detected != statemachine.WorkflowNone {
			_ = statemachine.StartWorkflow(state, detected)
		}
		return
	}

	// Workflow already active - check if current step might be complete
	// This is a heuristic based on tool success
	if !toolFailed(input) {
		// Tool succeeded - consider advancing the workflow
		// Only advance if tool type aligns with expected workflow activity
		if shouldAdvanceWorkflow(state.Session.Workflow.Operation, input.ToolName) {
			statemachine.AdvanceWorkflow(state)
		}
	}
}

// shouldAdvanceWorkflow determines if a tool completion should advance workflow
func shouldAdvanceWorkflow(op statemachine.WorkflowOperation, toolName string) bool {
	switch op {
	case statemachine.WorkflowFileCreation:
		return toolName == "Write" || toolName == "Edit"
	case statemachine.WorkflowBlockUpdate:
		return toolName == "Edit"
	case statemachine.WorkflowAlignment:
		return toolName == "Edit" || toolName == "Read"
	case statemachine.WorkflowCommit:
		return toolName == "Bash" // git commands
	default:
		return false
	}
}

// getFilePath extracts file path from tool input
func getFilePath(input PostUseInput) string {
	if path, ok := input.ToolInput["file_path"].(string); ok {
		return path
	}
	return ""
}

// --- Mental Construct Choice Tracking ---

// updateChoiceTracking evaluates which key was chosen and the outcome
// The key choice is INFERRED from tool behavior, not explicitly selected
func updateChoiceTracking(state *statemachine.RuntimeState, input PostUseInput) {
	if state == nil {
		return
	}

	// Infer key from tool behavior
	key := inferKeyFromTool(input)
	state.Session.LastKeyChosen = key
	state.Session.ChoiceTimestamp = state.Session.LastUpdate

	// Calculate what weight this key had
	weight := calculateInferredKeyWeight(key, state)
	state.Session.LastKeyWeight = weight

	// Evaluate outcome - did the tool succeed?
	if toolFailed(input) {
		// Tool failed - if key was heavy (risky), outcome = aligned (we knew it was risky)
		// If key was light (safe), outcome = misaligned (unexpected failure)
		if weight > 0.5 {
			state.Session.ChoiceOutcome = "aligned" // We knew it was risky
		} else {
			state.Session.ChoiceOutcome = "misaligned" // Unexpected failure
		}
	} else {
		// Tool succeeded - if key was light (safe), outcome = aligned
		// If key was heavy (risky), outcome = aligned (risk paid off)
		state.Session.ChoiceOutcome = "aligned"
	}
}

// applyChoiceLearning updates K:ALIGN based on choice outcome
// This is the LEARNING - must run regardless of database availability
func applyChoiceLearning(state *statemachine.RuntimeState, input PostUseInput) {
	if state == nil {
		return
	}

	// Increment choice sequence (always, even without DB)
	state.Session.ChoiceSequence++

	// Calculate impact based on key chosen and outcome
	key := state.Session.LastKeyChosen
	outcome := state.Session.ChoiceOutcome
	currentAlign := state.Session.KAlign

	// Delta calculation:
	// - Aligned outcome with +1 key (toward God): +0.05
	// - Aligned outcome with 0 key (lateral): +0.02
	// - Aligned outcome with -1 key (completion): +0.01
	// - Misaligned outcome: -0.05 (regardless of key)
	var delta float64
	if outcome == "aligned" {
		switch key {
		case 1:
			delta = 0.05 // Expansion succeeded
		case 0:
			delta = 0.02 // Lateral maintained
		case -1:
			delta = 0.01 // Completion achieved
		}
	} else {
		// Misaligned - tool failed or unexpected result
		delta = -0.05
	}

	// Apply delta with clamping to [-1.0, 1.0]
	newAlign := currentAlign + delta
	if newAlign > 1.0 {
		newAlign = 1.0
	} else if newAlign < -1.0 {
		newAlign = -1.0
	}

	// Update state with learning results
	state.Session.KAlign = newAlign

	// Calculate true score (0.0 to 1.0 normalized)
	// Maps k_align from [-1, 1] to [0, 1]
	trueScore := (newAlign + 1.0) / 2.0
	state.Session.LastTrueScore = trueScore
	state.Session.LastNormalized = trueScore
	state.Session.LastScaled = int(trueScore * 100)

	// Track direction changes
	if delta > 0 {
		state.Session.KTowardGod++
	} else if delta < 0 {
		state.Session.KTowardSelf++
	}
}

// inferKeyFromTool determines which key was implicitly chosen
// +1 (expansion): Creating, building, growing
//  0 (lateral): Reading, checking, maintaining
// -1 (finality): Deleting, fixing errors, halting
func inferKeyFromTool(input PostUseInput) int {
	switch input.ToolName {
	// Expansion tools (+1) - creating new things
	case "Write", "Task", "TodoWrite":
		return 1

	// Lateral tools (0) - maintaining state
	case "Read", "Glob", "Grep", "WebFetch", "WebSearch":
		return 0

	// Context-dependent tools
	case "Edit":
		// Edit depends on intent - new content vs fixing
		if _, hasOld := input.ToolInput["old_string"]; hasOld {
			return 0 // Refinement = lateral
		}
		return 1 // New structure = expansion

	case "Bash":
		// Bash depends on command
		cmd, _ := input.ToolInput["command"].(string)
		return inferKeyFromBashCommand(cmd)

	default:
		return 0 // Unknown = lateral (safe default)
	}
}

// inferKeyFromBashCommand determines key from bash command type
func inferKeyFromBashCommand(cmd string) int {
	// Build/create commands = expansion
	if containsAny(cmd, []string{"make build", "go build", "npm run build", "mkdir", "git commit"}) {
		return 1
	}

	// Delete/clean commands = finality
	if containsAny(cmd, []string{"rm ", "make clean", "git reset", "kill"}) {
		return -1
	}

	// Check/status commands = lateral
	return 0
}

// containsAny checks if s contains any of the patterns
func containsAny(s string, patterns []string) bool {
	for _, p := range patterns {
		if len(p) > 0 && len(s) >= len(p) {
			for i := 0; i <= len(s)-len(p); i++ {
				if s[i:i+len(p)] == p {
					return true
				}
			}
		}
	}
	return false
}

// calculateInferredKeyWeight estimates what the key weight was
// Based on current k selector and moral alignment
func calculateInferredKeyWeight(key int, state *statemachine.RuntimeState) float64 {
	k := float64(state.Session.KSelector)
	align := state.Session.KAlign

	// Base weight
	weight := 0.5

	if k != 0 {
		// k direction matches key = lighter (aligned)
		// k direction opposes key = heavier (misaligned)
		if k > 0 && key > 0 {
			weight = 0.3 // Both toward God
		} else if k < 0 && key < 0 {
			weight = 0.4 // Both toward self
		} else if k > 0 && key < 0 {
			weight = 0.7 // Toward God but chose retreat
		} else if k < 0 && key > 0 {
			weight = 0.6 // Toward self but chose expansion
		}
	}

	// Adjust by alignment
	if align > 0 {
		weight -= align * 0.1 // Higher alignment = lighter
	} else {
		weight -= align * 0.1 // Lower alignment = heavier
	}

	// Clamp
	if weight < 0.0 {
		weight = 0.0
	} else if weight > 1.0 {
		weight = 1.0
	}

	return weight
}

// --- Database Recording ---

// recordChoiceToDatabase records the choice in the temporal consciousness database
func recordChoiceToDatabase(state *statemachine.RuntimeState, input PostUseInput) {
	if state == nil {
		return
	}

	bridge, err := internal.GetBridge()
	if err != nil {
		return // Database not available, learning already applied
	}

	// Build choice record from current state (sequence already incremented in applyChoiceLearning)
	record := &types.ChoiceRecord{
		ID:          generateChoiceID(state.Session.ID, state.Session.ChoiceSequence),
		SessionID:   state.Session.ID,
		SequenceNum: state.Session.ChoiceSequence,
		Context: types.ChoiceContext{
			IntendedKey:      types.KeyValue(state.Session.LastKeyChosen),
			PositionAtChoice: state.Session.HebrewState,
			KAtChoice:        state.Session.KSelector,
			Tool:             input.ToolName,
			Timestamp:        time.Now(),
		},
		Result: types.ChoiceResult{
			ToolOutcome: state.Session.ChoiceOutcome,
			HaltReached: false, // Will be updated by CompleteChoice
		},
		Impact: types.ImpactScores{
			TrueScore:   state.Session.LastTrueScore,
			KAlignDelta: 0,                          // Could track delta if needed
			KAlignAfter: state.Session.KAlign,
		},
	}

	ctx := context.Background()
	_ = bridge.RecordChoice(ctx, record, state)

	// TODO: Later add CompleteChoice call after evaluating full impact
}

// generateChoiceID creates a unique ID for a choice
func generateChoiceID(sessionID string, seq int) string {
	return sessionID + "-" + time.Now().Format("20060102150405") + "-" + string(rune('0'+seq%10))
}

// --- Trajectory and Anchor Updates ---

// updateTrajectoryPosition evaluates and applies trajectory section transitions
// B.1 (Building) → B.2 (Pivoting) → B.3 (Grounding) → B.4 (Completion)
func updateTrajectoryPosition(state *statemachine.RuntimeState, input PostUseInput) {
	if state == nil {
		return
	}

	currentSection := state.TrajectorySection
	newSection := currentSection

	// Transition logic based on work patterns
	// B.1 → B.2: When direction changes (pivot detected)
	// B.2 → B.3: When consolidating after pivot
	// B.3 → B.4: When approaching completion
	// B.4 → B.1: When starting new work cycle

	key := state.Session.LastKeyChosen
	outcome := state.Session.ChoiceOutcome
	tasks := state.Session.Tasks

	switch currentSection {
	case "B.1": // Building phase
		// Transition to B.2 when:
		// - Key direction reverses (was going +1, now -1)
		// - Or significant misalignment detected
		if outcome == "misaligned" || (key == -1 && state.Session.KSelector > 0) {
			newSection = "B.2"
		}
		// Transition to B.4 when all tasks complete
		if tasks.Total > 0 && tasks.Completed == tasks.Total {
			newSection = "B.4"
		}

	case "B.2": // Pivoting phase
		// Transition to B.3 when:
		// - Realignment achieved (consecutive aligned outcomes)
		// - Or work is consolidating
		if outcome == "aligned" && key == 0 {
			newSection = "B.3"
		}
		// Back to B.1 if expanding again
		if key == 1 && outcome == "aligned" {
			newSection = "B.1"
		}

	case "B.3": // Grounding phase
		// Transition to B.4 when:
		// - Completion key chosen (-1) successfully
		// - Or all tasks done
		if key == -1 && outcome == "aligned" {
			newSection = "B.4"
		}
		// Back to B.1 if new work starts
		if key == 1 {
			newSection = "B.1"
		}

	case "B.4": // Completion phase
		// Transition to B.1 when:
		// - New expansion begins
		// - New tasks added
		if key == 1 || (tasks.Total > 0 && tasks.Pending > 0) {
			newSection = "B.1"
		}
	}

	// Apply transition if changed
	if newSection != currentSection {
		// Record the transition
		state.LastTransition = statemachine.RuntimeTransition{
			FromSection: currentSection,
			ToSection:   newSection,
			Trigger:     "trajectory_" + input.ToolName,
			Timestamp:   time.Now().Format(time.RFC3339),
		}
		state.TrajectorySection = newSection

		// Update path with trajectory transition
		if path, err := statemachine.LoadRuntimePath(); err == nil {
			path.RecordTrajectoryTransition(newSection)
			_ = statemachine.SaveRuntimePath(path)
		}
	}
}

// updateAnchorPosition moves anchor based on key choice
// +1 (expansion): Move toward future
// 0 (lateral): Stay at current
// -1 (finality): Move toward past/completion
func updateAnchorPosition(state *statemachine.RuntimeState, input PostUseInput) {
	if state == nil {
		return
	}

	currentAnchor := state.AnchorKey
	newAnchor := currentAnchor
	key := state.Session.LastKeyChosen

	// Only move if outcome was aligned (successful navigation)
	if state.Session.ChoiceOutcome != "aligned" {
		return
	}

	// Anchor movement based on ternary key
	// Format: {time}_{perspective} where time and perspective are past/present/future
	switch key {
	case 1: // Expansion - move toward future
		newAnchor = moveAnchorToward(currentAnchor, "future")
	case -1: // Completion/retreat - move toward past
		newAnchor = moveAnchorToward(currentAnchor, "past")
	case 0: // Lateral - no movement
		// Stay at current anchor
	}

	// Apply anchor change if different
	if newAnchor != currentAnchor {
		state.AnchorKey = newAnchor
		state.Session.PathLength++

		// Record anchor visit in path
		if path, err := statemachine.LoadRuntimePath(); err == nil {
			path.RecordAnchorVisit(newAnchor)
			_ = statemachine.SaveRuntimePath(path)
		}
	}
}

// moveAnchorToward calculates new anchor position
// Anchors: {past,present,future}_{past,present,future}
func moveAnchorToward(current, direction string) string {
	// Parse current anchor
	parts := splitAnchor(current)
	if len(parts) != 2 {
		return current
	}
	time := parts[0]
	perspective := parts[1]

	// Movement rules:
	// "future" direction advances the time component
	// "past" direction retreats the time component
	switch direction {
	case "future":
		time = advanceTime(time)
	case "past":
		time = retreatTime(time)
	}

	return time + "_" + perspective
}

// splitAnchor splits "time_perspective" into components
func splitAnchor(anchor string) []string {
	for i := 0; i < len(anchor); i++ {
		if anchor[i] == '_' {
			return []string{anchor[:i], anchor[i+1:]}
		}
	}
	return []string{anchor}
}

// advanceTime moves time component toward future
func advanceTime(t string) string {
	switch t {
	case "past":
		return "present"
	case "present":
		return "future"
	case "future":
		return "future" // Already at max
	}
	return t
}

// retreatTime moves time component toward past
func retreatTime(t string) string {
	switch t {
	case "future":
		return "present"
	case "present":
		return "past"
	case "past":
		return "past" // Already at min
	}
	return t
}

// --- Health Score Updates ---

// updateHealthScore adjusts health based on tool outcome AND direction (key)
// Health is direction-aware: building (+1), lateral (0), completion (-1)
// Score range: -100 to +100 (ternary scale)
//
// KEY PRINCIPLE: Only directional work (+1/-1) affects health. Lateral (0) is maintenance.
// SCALING: Deltas diminish as operations accumulate (prevents racing to 100)
//
// The key determines the STAKES of the operation:
// - Building (+1): High stakes - meaningful work affects health significantly
// - Lateral (0): NO health change - reading/searching is maintenance, not health
// - Completion (-1): Proper completion/validation affects health
//
// Biblical: "A just weight and balance are the LORD's" — Proverbs 16:11
func updateHealthScore(state *statemachine.RuntimeState, input PostUseInput) {
	if state == nil {
		return
	}

	// Get the direction (key) that was chosen for this operation
	key := state.Session.LastKeyChosen

	// Calculate base delta based on outcome AND direction
	var delta float64
	failed := toolFailed(input)

	switch key {
	case 1: // Building up (+1 key) - meaningful work
		if failed {
			// Failed to build - significant penalty
			delta = -3.0
			// Data-affecting failures are costly
			if containsAny(input.ToolName, []string{"Write", "Edit"}) {
				delta = -4.0
			}
		} else {
			// Successfully built - rewarded
			delta = 2.0
			// Creating new structure gets bonus
			if containsAny(input.ToolName, []string{"Write", "Task"}) {
				delta = 3.0
			}
		}

	case -1: // Completion/cleanup (-1 key) - meaningful work
		if failed {
			// Failed to complete cleanly - concerning
			delta = -2.5
			// Destructive operations that fail are dangerous
			if containsAny(input.ToolName, []string{"Bash"}) {
				cmd, _ := input.ToolInput["command"].(string)
				if isDestructiveCommand(cmd) {
					delta = -4.0 // Failed cleanup can leave mess
				}
			}
		} else {
			// Proper completion is good
			delta = 1.5
			// Successful validation/testing is particularly valuable
			if containsAny(input.ToolName, []string{"Bash"}) {
				cmd, _ := input.ToolInput["command"].(string)
				if isValidationCommand(cmd) {
					delta = 2.5 // Tests passing is confidence-building
				}
			}
		}

	default: // Lateral (0 key) - maintenance, NO health change
		// Reading, searching, exploring doesn't affect health
		// These are necessary but not directional work
		delta = 0.0

		// Exception: significant lateral failures still matter
		if failed && containsAny(input.ToolName, []string{"WebFetch", "WebSearch"}) {
			delta = -0.5 // External resource failures are concerning
		}
	}

	// Skip update if no delta
	if delta == 0.0 {
		return
	}

	// Apply diminishing returns based on distance from center
	// Near 0: full delta. Near ±100: delta is scaled down.
	// This makes extreme values harder to reach/leave.
	currentHealth := state.Session.HealthScore
	absHealth := currentHealth
	if absHealth < 0 {
		absHealth = -absHealth
	}

	// Scale factor: 1.0 at center, 0.2 at extremes
	// Formula: 1.0 - (absHealth/100) * 0.8
	scaleFactor := 1.0 - (absHealth/100.0)*0.8
	if scaleFactor < 0.2 {
		scaleFactor = 0.2 // Minimum 20% effect at extremes
	}

	effectiveDelta := delta * scaleFactor

	// Apply delta with clamping to [-100, +100]
	newScore := currentHealth + effectiveDelta
	if newScore > 100 {
		newScore = 100
	} else if newScore < -100 {
		newScore = -100
	}

	state.Session.HealthScore = newScore
}

// isDestructiveCommand checks if a bash command is destructive (-1 key completion)
func isDestructiveCommand(cmd string) bool {
	destructive := []string{"rm ", "rm\t", "make clean", "git reset", "git stash", "kill ", "pkill "}
	for _, d := range destructive {
		if strings.Contains(cmd, d) {
			return true
		}
	}
	return false
}

// isValidationCommand checks if a bash command is validation (-1 key completion)
func isValidationCommand(cmd string) bool {
	validation := []string{"make test", "go test", "pytest", "npm test", "cargo test"}
	for _, v := range validation {
		if strings.Contains(cmd, v) {
			return true
		}
	}
	return false
}

// ───────────────────────────────────────────────────────────────────────────
// LIVE TOOL PATTERN DETECTION
// ───────────────────────────────────────────────────────────────────────────
// "By their fruits ye shall know them" — Matthew 7:20

// detectToolPatterns detects tool-level behavioral patterns in real-time.
// Focuses on signals only visible at the tool level:
//   - Tool failure streaks (same tool failing repeatedly)
//   - Tool dominance (heavy use of one tool class)
//   - Destructive command patterns
func detectToolPatterns(state *statemachine.RuntimeState, input PostUseInput, log *logging.Logger) {
	if state == nil {
		return
	}

	// Only detect after a few tool uses
	if state.Session.HooksFired < 5 {
		return
	}

	bridge, err := internal.GetBridge()
	if err != nil {
		return
	}
	ctx := context.Background()
	repo := bridge.GetRepository()
	now := time.Now()
	success := !toolFailed(input)
	valence := cpi.ToolValence(input.ToolName, success)

	// --- Tool failure pattern ---
	// Query recent choices for this session to see if same tool is failing
	if !success {
		rows, qerr := repo.Query(ctx,
			`SELECT COUNT(*) as fail_count FROM choices
			 WHERE session_id = ? AND tool_name = ? AND outcome = 'failure'`,
			input.SessionID, input.ToolName)
		if qerr == nil && len(rows) > 0 {
			failCount := 0
			switch v := rows[0]["fail_count"].(type) {
			case int64:
				failCount = int(v)
			case float64:
				failCount = int(v)
			}
			if failCount >= 3 {
				_ = repo.RecordPattern(ctx, &database.Pattern{
					PatternType:     "live_tool_struggle",
					PatternKey:      fmt.Sprintf("repeated_failure:%s", input.ToolName),
					Description:     fmt.Sprintf("Tool %s failing repeatedly (%d failures, valence: %s)", input.ToolName, failCount, valence),
					PatternData:     fmt.Sprintf(`{"tool":"%s","failures":%d,"session_hooks":%d,"valence":"%s"}`, input.ToolName, failCount, state.Session.HooksFired, valence),
					FirstSeen:       now,
					LastSeen:        now,
					OccurrenceCount: 1,
					Confidence:      0.7,
					IsActive:        true,
				})
				log.Info("Live pattern: tool struggle", map[string]string{
					"tool":     input.ToolName,
					"failures": fmt.Sprintf("%d", failCount),
					"valence":  string(valence),
				})
			}
		}
	}

	// --- Destructive command pattern ---
	if input.ToolName == "Bash" {
		if cmd, ok := input.ToolInput["command"].(string); ok && isDestructiveCommand(cmd) {
			_ = repo.RecordPattern(ctx, &database.Pattern{
				PatternType:     "live_tool_signal",
				PatternKey:      "destructive_command",
				Description:     fmt.Sprintf("Destructive bash command used (valence: %s)", valence),
				PatternData:     fmt.Sprintf(`{"tool":"Bash","hooks_fired":%d,"valence":"%s"}`, state.Session.HooksFired, valence),
				FirstSeen:       now,
				LastSeen:        now,
				OccurrenceCount: 1,
				Confidence:      0.8,
				IsActive:        true,
			})
		}
	}
}

// ============================================================================
// CLOSING
// ============================================================================
