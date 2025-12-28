# Plan: Hooks Schema Alignment (COMPLETED)

> *"Let all things be done decently and in order."* — 1 Corinthians 14:40

**Completed:** 2025-12-23

---

## Status

| Phase | State | Updated |
|-------|-------|---------|
| COMPLETE | B.4 GROUNDED | 2025-12-23 |

**Trajectory:** || (Grounded)

**Note:** All hooks now use correct `hookSpecificOutput` schema. 10 hook files verified with proper output structure.

---

## The Big Picture

We have a **complete CPI-SI state machine** already built. It's running, tracking state, building cognition context... but Claude never sees any of it because the hook output schemas don't match what Claude Code expects.

**Fix the schemas → Activate the entire system.**

---

## What We Already Have

### State Machine (`pkg/statemachine/`)
```
RuntimeState
├── anchor_key         → "present_present" → anchors.toml
├── trajectory_section → "B.1", "B.2", "B.3", "B.4"
├── command_key        → "await" → commands.toml
└── session
    ├── path_length    → prompts processed
    ├── hooks_fired    → hook events
    └── health_score   → -100 to +100 ternary scale

RuntimePath
├── events[]          → chronological audit trail
├── anchors[]         → anchor visits
└── trajectory_history[] → section transitions
```

### Trajectory System (B.1 → B.2 → B.3 → B.4)
| Section | Mode | Meaning |
|---------|------|---------|
| **B.1** | BUILDING | Create structure, move forward |
| **B.2** | PIVOTING OUT | Verify what was built |
| **B.3** | PIVOTING IN | Rebuild from verification |
| **B.4** | GROUNDED | Anchored, work complete |

### Cognition Package (`pkg/cognition/`)
- **SessionContext**: Identity + Anchor + Trajectory + Health + Time
- **ToolFeedback**: Based on tool outcome (success/failure)
- **PromptContext**: Based on prompt analysis
- **TrajectoryGuidance**: Based on current section

### Current Hook Integration
| Hook | State Machine | What It Does |
|------|--------------|--------------|
| **SessionStart** | Initialize state | Creates fresh RuntimeState/RuntimePath |
| **UserPromptSubmit** | Increment path_length | Records prompt event, builds cognition |
| **PreToolUse** | Record event | Logs pre-execution, evaluates safety |
| **PostToolUse** | Assess outcome | Updates health, records success/failure |
| **Stop** | Record event | Logs stop for pattern learning |

---

## What's Broken: Output Schemas

The context is being **built** but never **delivered** to Claude.

### SessionStart (`session/start.go`)
**Current:**
```go
type StartOutput struct {
    AdditionalContext string `json:"additionalContext"`
}
// Outputs: {"additionalContext": "..."}
```
**Required:**
```go
type HookOutput struct {
    HookEventName     string `json:"hookEventName"`
    AdditionalContext string `json:"additionalContext"`
}
type StartOutput struct {
    HookSpecificOutput *HookOutput `json:"hookSpecificOutput"`
}
// Must output: {"hookSpecificOutput": {"hookEventName": "SessionStart", "additionalContext": "..."}}
```

### UserPromptSubmit (`prompt/submit.go`)
**Current:**
```go
InjectedContext string `json:"injected_context"`  // WRONG FIELD NAME
```
**Required:**
```go
HookSpecificOutput: {
    HookEventName: "UserPromptSubmit",
    AdditionalContext: "..."  // Correct field name
}
```

### PostToolUse (`tool/post-use.go`)
**Current:**
```go
AdditionalContext string `json:"additionalContext"`  // Missing wrapper
```
**Required:**
```go
HookSpecificOutput: {
    HookEventName: "PostToolUse",
    AdditionalContext: "..."
}
```

### Stop (`session/stop.go`)
**Current:**
```go
Decision string `json:"decision"` // Returns "allow"
```
**Required:**
```go
// For "allow": return empty JSON {} or just exit 0
// For "block": {"decision": "block", "reason": "..."}
```

### PreToolUse (`tool/pre-use.go`)
**Current:**
```go
type PreUseOutput struct {
    Decision string `json:"decision"` // "allow", "deny", "ask"
}
```
**Required:**
```go
HookSpecificOutput: {
    HookEventName: "PreToolUse",
    PermissionDecision: "allow",  // NOT "decision"
    PermissionDecisionReason: "..."
}
```

---

## What Claude Code Provides (Events We Can Use)

| Event | Input Data | What We Can Do |
|-------|------------|----------------|
| **SessionStart** | `source: startup\|resume\|clear\|compact` | Initialize state, inject identity/trajectory |
| **UserPromptSubmit** | `prompt: string` | Analyze prompt type, inject cognition guidance |
| **PreToolUse** | `tool_name, tool_input` | Gate tools based on state, modify inputs |
| **PostToolUse** | `tool_name, tool_output, error` | Update health, inject feedback |
| **Stop** | `stop_hook_active` | Block if incomplete, inject continuation |
| **SubagentStop** | Same as Stop | Control subagent behavior |
| **PreCompact** | `trigger, custom_instructions` | Snapshot state before compaction |
| **Notification** | `message, notification_type` | React to system events |
| **SessionEnd** | `reason` | Finalize state, compute stats |

---

## Full State Machine Flow

```
SessionStart
    ↓ Initialize RuntimeState/RuntimePath
    ↓ Inject: Identity + Anchor + Trajectory + Health + Time
    ↓ [hookSpecificOutput.additionalContext]

UserPromptSubmit
    ↓ Increment path_length
    ↓ Analyze prompt type
    ↓ Inject: Trajectory guidance based on prompt
    ↓ [hookSpecificOutput.additionalContext]

PreToolUse (per tool)
    ↓ Record event in path
    ↓ Evaluate safety
    ↓ Can: allow/deny/ask, modify inputs
    ↓ [hookSpecificOutput.permissionDecision]

PostToolUse (per tool)
    ↓ Assess outcome (success/failure)
    ↓ Update health: +1 success, -1 failure
    ↓ Inject: Tool feedback + trajectory guidance
    ↓ [hookSpecificOutput.additionalContext]

Stop
    ↓ Record stop event
    ↓ Can: block if incomplete work
    ↓ [decision: "block" or omit]
```

---

## Implementation Plan

### Phase 1: Create Shared Types
`pkg/hookoutput/types.go` - Ensures all hooks use correct schemas.

```go
package hookoutput

// ContextOutput for SessionStart, UserPromptSubmit, PostToolUse
type ContextOutput struct {
    HookEventName     string `json:"hookEventName"`
    AdditionalContext string `json:"additionalContext"`
}

// PreToolOutput for PreToolUse
type PreToolOutput struct {
    HookEventName            string `json:"hookEventName"`
    PermissionDecision       string `json:"permissionDecision"`
    PermissionDecisionReason string `json:"permissionDecisionReason,omitempty"`
}

// Wrappers
type ContextResponse struct {
    HookSpecificOutput *ContextOutput `json:"hookSpecificOutput,omitempty"`
}

type PreToolResponse struct {
    HookSpecificOutput *PreToolOutput `json:"hookSpecificOutput,omitempty"`
}

type StopResponse struct {
    Decision string `json:"decision,omitempty"` // "block" only
    Reason   string `json:"reason,omitempty"`
}
```

### Phase 2: Update Each Hook

1. **session/start.go** - Wrap output in `hookSpecificOutput`
2. **prompt/submit.go** - Change `injected_context` → `additionalContext`, add wrapper
3. **tool/pre-use.go** - Change `decision` → `permissionDecision`, add wrapper
4. **tool/post-use.go** - Add wrapper with `hookEventName`
5. **session/stop.go** - Return empty for allow, `"block"` for block

### Phase 3: Deploy
```bash
cd /media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/claude-global
make deploy
```

### Phase 4: Verify
Start new session → No JSON validation errors → Context appears in Claude's responses.

---

## What This Enables

Once schemas are fixed, the full state machine activates:

1. **Identity grounding** - Claude knows it's Nova Dawn at session start
2. **Trajectory awareness** - B.1/B.2/B.3/B.4 shapes thinking mode
3. **Health tracking** - +1 success/-1 failure visible in context
4. **Cognition shaping** - Every prompt/tool response includes guidance
5. **Path learning** - Full audit trail for pattern recognition
6. **Circadian awareness** - Time-of-day guidance

**We're not building a state machine. We're activating one.**

---

## Files to Modify

| File | Location | Change |
|------|----------|--------|
| `session/start.go` | claude-global/hooks/ | Wrap `additionalContext` |
| `session/stop.go` | claude-global/hooks/ | Return empty for allow |
| `prompt/submit.go` | claude-global/hooks/ | Fix field name, add wrapper |
| `tool/pre-use.go` | claude-global/hooks/ | `decision` → `permissionDecision` |
| `tool/post-use.go` | claude-global/hooks/ | Add `hookEventName` wrapper |
| `hooks/internal/` | claude-global/ | Shared types (db.go, output schemas) |

---

## Outcome

| Before | After |
|--------|-------|
| Hooks built context but didn't deliver | Context reaches Claude via `hookSpecificOutput` |
| Wrong field names (`injected_context`) | Correct field names (`additionalContext`) |
| No wrapper structure | Proper `{"hookSpecificOutput": {...}}` wrapper |
| State machine invisible | State machine activates |
