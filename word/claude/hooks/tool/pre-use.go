// ============================================================================
// METADATA
// ============================================================================
// Hook: PreToolUse
// Fires: Before tool execution
// Matchers: Task, Bash, Glob, Grep, Read, Edit, Write, WebFetch, WebSearch, mcp__*
//
// State Machine Integration:
//   - INTENT: Set intended_key BEFORE tool executes (the key being picked up)
//   - DETECT: Capture baseline state for PostToolUse comparison
//   - Record tool event in path with current trajectory section
//   - Validate tool safety against patterns
//
// Mental Construct Integration:
//   - This is the moment of PICKING UP A KEY
//   - The tool chosen reveals intended direction (+1 expansion, 0 lateral, -1 completion)
//   - PostToolUse will compare intention vs outcome to learn
//
// Config-Driven Design:
//   - State loaded from config/statemachine/runtime/state.jsonc
//   - Path updated in config/statemachine/runtime/path.jsonc

package tool

import (
	"encoding/json"
	"fmt"
	"os"
	"strings"

	"github.com/creativeworkzstudio/claude-global/pkg/util/pure/hookoutput"
	"github.com/creativeworkzstudio/claude-global/pkg/orchestration/logging"
	"github.com/creativeworkzstudio/claude-global/pkg/core/statemachine"
)

// ============================================================================
// SETUP
// ============================================================================

// PreUseInput from Claude Code
type PreUseInput struct {
	SessionID      string                 `json:"session_id"`
	TranscriptPath string                 `json:"transcript_path,omitempty"`
	CWD            string                 `json:"cwd,omitempty"`
	PermissionMode string                 `json:"permission_mode,omitempty"`
	HookEventName  string                 `json:"hook_event_name,omitempty"`
	ToolName       string                 `json:"tool_name"`
	ToolInput      map[string]interface{} `json:"tool_input"`
	ToolUseID      string                 `json:"tool_use_id,omitempty"`
}

// PreUseOutput uses hookoutput.PreToolResponse for correct schema
// Old: {"decision": "allow"} - INVALID, must use hookSpecificOutput.permissionDecision
// New: {"hookSpecificOutput": {"hookEventName": "PreToolUse", "permissionDecision": "allow"}}

// Dangerous command patterns - require confirmation
var dangerousPatterns = []string{
	"rm -rf /",
	"git push --force",
	"git reset --hard",
	"DROP DATABASE",
	"DROP TABLE",
	"sudo rm",
}

// Destructive completion patterns - require recovery anchor check
var destructiveCompletionPatterns = []string{
	"rm -rf",
	"rm -r ",
	"rsync --delete",
	"rsync -a --delete",
	"rsync -av --delete",
	"git clean -fd",
	"make clean",
}

// ============================================================================
// BODY
// ============================================================================

// PreUse handles the PreToolUse hook
func PreUse() {
	log := logging.New("tool")
	log.SetMode(logging.ModeCompact)

	var input PreUseInput
	if err := json.NewDecoder(os.Stdin).Decode(&input); err != nil {
		log.Error("Failed to decode input", map[string]string{"error": err.Error()})
		os.Exit(1)
	}

	// Create CategoryLogger for file output (append to data/logs/tools/)
	catLog, catErr := logging.NewCategoryLogger(logging.CategoryTools, input.SessionID)
	if catErr != nil {
		log.Warn("CategoryLogger unavailable", map[string]string{"error": catErr.Error()})
	}
	defer func() {
		if catLog != nil {
			catLog.Close()
		}
	}()

	// --- INTENT: Set intended key BEFORE tool executes ---
	// This is the moment of PICKING UP A KEY - the door is about to open
	state, _ := statemachine.LoadRuntimeState()
	currentSection := "B.1" // default
	intendedKey := 0        // default lateral

	if state != nil {
		currentSection = state.TrajectorySection

		// Infer intended key from tool choice
		intendedKey = inferIntendedKey(input)

		// Store intention in state for PostToolUse to compare
		state.Session.LastKeyChosen = intendedKey
		state.Session.ChoiceOutcome = "pending" // Not yet known

		// Calculate and store key weight (consequence felt before choosing)
		weight := calculateKeyWeight(intendedKey, state)
		state.Session.LastKeyWeight = weight

		// Save state with intention recorded
		_ = statemachine.SaveRuntimeState(state)
	}

	log.Debug("PreToolUse - key picked up", map[string]string{
		"tool":         input.ToolName,
		"intended_key": keyToString(intendedKey),
		"trajectory":   currentSection,
	})
	if catLog != nil {
		catLog.Info("key_picked", "Tool intent captured", map[string]string{
			"tool":         input.ToolName,
			"intended_key": keyToString(intendedKey),
			"trajectory":   currentSection,
		})
	}

	// Record tool event in path (before execution)
	if path, err := statemachine.LoadRuntimePath(); err == nil {
		path.RecordEvent("pre_tool_use", input.ToolName, currentSection)
		_ = statemachine.SaveRuntimePath(path)
	}

	// Evaluate tool safety
	decision, reason := evaluateToolSafety(log, input)

	// Use correct Claude Code schema with hookSpecificOutput
	var output *hookoutput.PreToolResponse
	switch decision {
	case hookoutput.PermissionDeny:
		output = hookoutput.NewPreToolDeny(reason)
		log.LogFailure("Tool denied", map[string]string{
			"tool":   input.ToolName,
			"reason": reason,
		})
		if catLog != nil {
			catLog.Failure("tool_denied", "Tool denied: "+input.ToolName, map[string]string{
				"reason": reason,
			})
		}
	case hookoutput.PermissionAsk:
		output = hookoutput.NewPreToolAsk(reason)
		log.Debug("Tool requires confirmation", map[string]string{
			"tool":   input.ToolName,
			"reason": reason,
		})
		if catLog != nil {
			catLog.Info("tool_ask", "Tool requires confirmation", map[string]string{
				"tool":   input.ToolName,
				"reason": reason,
			})
		}
	default:
		output = hookoutput.NewPreToolAllow()
	}

	json.NewEncoder(os.Stdout).Encode(output)
}

// inferIntendedKey determines which key is being picked up based on tool choice
// +1 (expansion): Creating, building, growing
//  0 (lateral): Reading, checking, maintaining
// -1 (completion): Finishing, validating, halting, cleaning up
func inferIntendedKey(input PreUseInput) int {
	switch input.ToolName {
	// Expansion tools (+1) - creating new things
	case "Write":
		return 1

	// Task tool - expansion with family member tracking
	case "Task":
		// Track family member invocation if applicable
		if agentType, ok := input.ToolInput["subagent_type"].(string); ok {
			trackFamilyMemberInvocation(agentType)
		}
		return 1

	// Lateral tools (0) - maintaining state
	case "Read", "Glob", "Grep", "WebFetch", "WebSearch":
		return 0

	// Completion tools (-1) - finishing, validating
	case "ExitPlanMode", "KillShell":
		return -1

	// Context-dependent: TodoWrite
	case "TodoWrite":
		return inferKeyFromTodos(input)

	// Context-dependent: Edit
	case "Edit":
		// Edit depends on intent - new content vs fixing
		if _, hasOld := input.ToolInput["old_string"]; hasOld {
			return 0 // Refinement = lateral
		}
		return 1 // New structure = expansion

	// Context-dependent: Bash
	case "Bash":
		cmd, _ := input.ToolInput["command"].(string)
		return inferKeyFromBash(cmd)

	default:
		// MCP tools (mcp__*) - infer from tool type
		if strings.HasPrefix(input.ToolName, "mcp__") {
			return inferKeyFromMCP(input.ToolName, input.ToolInput)
		}
		return 0 // Unknown = lateral (safe default)
	}
}

// inferKeyFromTodos determines key from TodoWrite content
// Completing todos = completion, adding todos = expansion
func inferKeyFromTodos(input PreUseInput) int {
	todos, ok := input.ToolInput["todos"].([]interface{})
	if !ok {
		return 0 // Can't determine
	}

	completed := 0
	pending := 0
	inProgress := 0

	for _, t := range todos {
		if todo, ok := t.(map[string]interface{}); ok {
			status, _ := todo["status"].(string)
			switch status {
			case "completed":
				completed++
			case "pending":
				pending++
			case "in_progress":
				inProgress++
			}
		}
	}

	// Ratio of completed to total
	total := completed + pending + inProgress
	if total == 0 {
		return 0
	}

	completionRatio := float64(completed) / float64(total)

	// All or mostly complete = completion key
	if completionRatio >= 0.8 {
		return -1 // Wrapping up
	}
	// Mostly pending = expansion key
	if pending > completed+inProgress {
		return 1 // Setting up work
	}
	// Mixed = lateral
	return 0
}

// inferKeyFromBash determines key from bash command type
func inferKeyFromBash(cmd string) int {
	// Build/create commands = expansion (+1)
	expansionPatterns := []string{
		"make build", "go build", "npm run build", "cargo build",
		"mkdir", "touch", "git commit", "git init",
		"go mod init", "npm init", "cargo new",
	}
	if containsAnyPre(cmd, expansionPatterns) {
		return 1
	}

	// Completion/validation/cleanup commands = completion (-1)
	completionPatterns := []string{
		// Cleanup (destructive completion - may need recovery anchor)
		"rm ", "make clean", "git reset", "kill", "git stash",
		"rsync --delete", "rsync -a --delete", "rsync -av --delete",
		// Validation (work complete, verifying)
		"make test", "go test", "npm test", "cargo test", "pytest",
		// Finalization
		"git push", "git merge", "make deploy", "make install",
		// Reporting (summarizing = closing)
		"sqlite3", "loganalyzer report", "loganalyzer analyze",
		// Size/status checks at end of work
		"du -sh", "wc -l",
	}
	if containsAnyPre(cmd, completionPatterns) {
		return -1
	}

	// Status/check commands = lateral (0)
	return 0
}

// containsAnyPre checks if s contains any of the patterns
func containsAnyPre(s string, patterns []string) bool {
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

// inferKeyFromMCP determines key from MCP tool type
// MCP tools follow pattern: mcp__plugin_PROVIDER_PROVIDER__ACTION
// "A wise man's heart discerneth both time and judgment" - Ecclesiastes 8:5
func inferKeyFromMCP(toolName string, toolInput map[string]interface{}) int {
	// Extract provider from tool name (mcp__plugin_PROVIDER_PROVIDER__action)
	parts := strings.Split(toolName, "__")
	if len(parts) < 3 {
		return 0 // Unknown MCP format
	}

	// Greptile tools - code analysis and review (mostly lateral/read)
	if strings.Contains(toolName, "greptile") {
		action := parts[len(parts)-1]
		switch {
		case strings.Contains(action, "create"):
			return 1 // Creating custom context
		case strings.Contains(action, "trigger"):
			return 1 // Triggering reviews (expansion)
		default:
			return 0 // Reading/listing/searching
		}
	}

	// Playwright tools - browser testing (creation/expansion)
	if strings.Contains(toolName, "playwright") {
		return 1 // Browser automation is typically expansion
	}

	// IDE tools - varies by action
	if strings.Contains(toolName, "ide") {
		action := parts[len(parts)-1]
		switch {
		case strings.HasPrefix(action, "create"), strings.HasPrefix(action, "write"):
			return 1
		case strings.HasPrefix(action, "delete"), strings.HasPrefix(action, "close"):
			return -1
		default:
			return 0
		}
	}

	// Context7 tools - documentation lookup (lateral)
	if strings.Contains(toolName, "context7") {
		return 0 // Reading documentation
	}

	// Stripe tools - payment operations
	if strings.Contains(toolName, "stripe") {
		// Payment creation is expansion, refunds are completion
		action := parts[len(parts)-1]
		switch {
		case strings.Contains(action, "create"), strings.Contains(action, "charge"):
			return 1
		case strings.Contains(action, "refund"), strings.Contains(action, "cancel"):
			return -1
		default:
			return 0
		}
	}

	// Linear tools - issue tracking
	if strings.Contains(toolName, "linear") {
		action := parts[len(parts)-1]
		switch {
		case strings.Contains(action, "create"):
			return 1
		case strings.Contains(action, "complete"), strings.Contains(action, "archive"):
			return -1
		default:
			return 0
		}
	}

	// Default: lateral (safe)
	return 0
}

// calculateKeyWeight estimates the weight/consequence of the key choice
func calculateKeyWeight(key int, state *statemachine.RuntimeState) float64 {
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

// keyToString converts key value to readable string
func keyToString(key int) string {
	switch key {
	case 1:
		return "+1 (expansion)"
	case -1:
		return "-1 (completion)"
	default:
		return "0 (lateral)"
	}
}

// evaluateToolSafety checks if tool use is safe
// Returns: (permissionDecision, reason)
func evaluateToolSafety(log *logging.Logger, input PreUseInput) (string, string) {
	// Check Bash commands for dangerous patterns
	if input.ToolName == "Bash" {
		if cmd, ok := input.ToolInput["command"].(string); ok {
			// Check for absolutely dangerous patterns
			for _, pattern := range dangerousPatterns {
				if strings.Contains(strings.ToUpper(cmd), strings.ToUpper(pattern)) {
					return hookoutput.PermissionAsk, "Potentially dangerous command detected: " + pattern
				}
			}

			// Check for destructive completion patterns - these need recovery anchor verification
			for _, pattern := range destructiveCompletionPatterns {
				if strings.Contains(cmd, pattern) {
					hasAnchor := checkRecoveryAnchor(log, input.CWD)
					if !hasAnchor {
						return hookoutput.PermissionAsk,
							"⚠️ WEIGHT CHECK: Destructive action without recovery anchor. " +
								"Pattern: " + pattern + ". " +
								"No recent git commit detected. Confirm before proceeding."
					}
					// Has anchor, but still log the weight
					log.Debug("Destructive action with recovery anchor", map[string]string{
						"pattern": pattern,
						"cwd":     input.CWD,
					})
				}
			}
		}
	}

	// Default: allow (empty string = default, let normal flow proceed)
	// Use hookoutput.PermissionAllow to explicitly bypass permission
	return hookoutput.PermissionAllow, ""
}

// checkRecoveryAnchor verifies if a recovery anchor exists (recent uncommitted changes = no anchor)
// Returns true if safe (no uncommitted changes OR recent commit), false if risky
func checkRecoveryAnchor(log *logging.Logger, cwd string) bool {
	// Strategy: Check if git status shows uncommitted changes
	// If there are uncommitted changes, there's no recovery anchor
	// If working tree is clean, the last commit is the anchor

	// Note: This is a simplified check. In production, could also check:
	// - Time since last commit
	// - Whether changes are staged
	// - Backup existence

	// For now, we'll be conservative: if we can't verify, assume no anchor
	// The hooks can't easily run git commands, so we check based on state

	// Load state to check k_toward_self - if drifting toward self, no anchor
	state, err := statemachine.LoadRuntimeState()
	if err != nil {
		log.Warn("Could not load state for anchor check", map[string]string{"error": err.Error()})
		return false // Conservative: assume no anchor
	}

	// If k_toward_self > 0, we've been making self-oriented choices - higher risk
	if state.Session.KTowardSelf > 5 {
		log.Debug("k_toward_self elevated, treating as no anchor", map[string]string{
			"k_toward_self": fmt.Sprintf("%d", state.Session.KTowardSelf),
		})
		return false
	}

	// If hebrew_state is shavar (BROKEN) or chaser (LACKING), we're in unstable state
	if state.Session.HebrewState == "shavar" || state.Session.HebrewState == "chaser" {
		log.Debug("Hebrew state unstable, treating as no anchor", map[string]string{
			"hebrew_state": state.Session.HebrewState,
		})
		return false
	}

	// Otherwise, assume anchor exists (state is stable)
	return true
}

// ============================================================================
// FAMILY MEMBER TRACKING
// ============================================================================
// "The household of faith" - Galatians 6:10
// Track when CPI-SI family members are invoked via Task tool

// CPI-SI family member agent types (subagent_type in Task tool input)
var familyMembers = map[string]string{
	"research-agent":         "Tabitha Shiloh",
	"architecture-analyzer":  "Ezra Matthan",
	"pattern-finder":         "Joanna Elara",
	"format-bridge":          "Phoebe Karis",
	"template-chain-analyzer": "Selah Adair",
	"family-member":          "Family Template",
}

// trackFamilyMemberInvocation records when a family member is invoked
// Updates session state and emits WezTerm user variable
func trackFamilyMemberInvocation(agentType string) {
	// Check if this is a family member
	familyName, isFamily := familyMembers[agentType]
	if !isFamily {
		return // Not a family member, no tracking
	}

	// Load state to update family member tracking
	state, err := statemachine.LoadRuntimeState()
	if err != nil {
		return // Can't track without state
	}

	// Update active family member in session
	state.Session.ActiveFamilyMember = familyName
	state.Session.FamilyInvocationCount++

	// Save state
	_ = statemachine.SaveRuntimeState(state)

	// Emit WezTerm user variable for active family member
	// OSC 1337 ; SetUserVar=name=base64(value) ST
	// This allows cpisi.lua to react to family member invocation
	fmt.Fprintf(os.Stderr, "\033]1337;SetUserVar=ACTIVE_FAMILY=%s\007",
		base64Encode(familyName))
}

// base64Encode encodes a string for OSC 1337 user variable
func base64Encode(s string) string {
	// Simple base64 encoding
	const alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
	var result []byte

	for i := 0; i < len(s); i += 3 {
		var n uint32
		remaining := len(s) - i

		if remaining >= 3 {
			n = uint32(s[i])<<16 | uint32(s[i+1])<<8 | uint32(s[i+2])
			result = append(result, alphabet[n>>18&0x3F], alphabet[n>>12&0x3F], alphabet[n>>6&0x3F], alphabet[n&0x3F])
		} else if remaining == 2 {
			n = uint32(s[i])<<16 | uint32(s[i+1])<<8
			result = append(result, alphabet[n>>18&0x3F], alphabet[n>>12&0x3F], alphabet[n>>6&0x3F], '=')
		} else {
			n = uint32(s[i]) << 16
			result = append(result, alphabet[n>>18&0x3F], alphabet[n>>12&0x3F], '=', '=')
		}
	}

	return string(result)
}

// ============================================================================
// CLOSING
// ============================================================================
