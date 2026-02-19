# Full System Update — Claude Code Infrastructure

## Context

CC has evolved from v2.0.0 to v2.1.47 with significant new capabilities: agent frontmatter fields, hook API features, skill frontmatter options, and settings. Our infrastructure was last fully updated around Feb 8. Audit found 18 items across 5 priority tiers — 3 critical, 5 high, 5 medium, 5 low.

## Scope

Update all infrastructure files around the global CLAUDE.md to leverage current CC capabilities.

**NOT in scope:** CLAUDE.md identity document itself (already updated this session), hook Go source code rebuilds (separate session — source changes need compile + deploy).

---

## Phase 1: Hooks Source Code (Critical fixes — source only, no build)

### 1a. Fix SubagentStop output schema
**File:** `~/.claude/hooks/cmd/subagent-stop/main.go`
- Replace custom `SubagentStopOutput` struct with `hookoutput.SubagentStopResponse`
- Add missing input fields: `agent_id`, `agent_transcript_path`, `last_assistant_message`

### 1b. Update PreToolUse TodoWrite → Task tools
**File:** `~/.claude/hooks/cmd/pre-use/main.go`
- Rename `inferKeyFromTodos` → `inferKeyFromTasks`
- Handle `TaskCreate`, `TaskUpdate`, `TaskList`, `TaskGet` input structures instead of `TodoWrite`
- Update the tool name matching

### 1c. Complete familyMembers map
**File:** `~/.claude/hooks/cmd/pre-use/main.go`
- Add missing 8 family members to the `familyMembers` map

---

## Phase 2: Agent Frontmatter (High — all 13 agents)

### 2a. Add `disallowedTools` per role

| Agent Role | disallowedTools |
|-----------|----------------|
| **Read-only** (Ezra, Joanna, Tabitha, Caleb, Selah) | `Write, Edit, NotebookEdit` |
| **Review-only** (Micah, Phoebe) | `NotebookEdit` |
| **Full-access** (Nathan, Nehemiah, Deborah, Bezalel, Lydia, Asa) | none needed |

### 2b. Add `tools` with `Task(agent_type)` restrictions
- Analysis agents: restrict to `Task(Explore)` only (no spawning implementation agents)
- Creation agents: allow `Task(Explore)`, `Task(Plan)`
- Operations agents: full Task access

### 2c. Normalize `tools` and `skills` to YAML list format
- `tools: Read, Glob, Grep, Bash` → YAML list
- `skills: meta-awareness, recognize-pattern` → YAML list

---

## Phase 3: Skill Frontmatter (High/Medium — 18 skills)

### 3a. Fix `tools:` → `allowed-tools:` (9 skills)
Skills using wrong field name: meta-awareness, create-from-template, recognize-pattern, reflect-on-session, create-journal-entry, integrate-learning, create-code, create-data, create-documentation

### 3b. Add `user-invocable: false` to internal skills
Growth/session skills not meant for direct user invocation:
- meta-awareness
- recognize-pattern
- recognize-stopping-point
- reflect-on-session
- integrate-learning
- session-awareness

### 3c. Normalize `allowed-tools` to YAML list format
All 18 skills: convert comma-separated strings to YAML lists

### 3d. Add `version` to skills missing it
9 skills lack version field — add `version: "1.0.0"`

---

## Phase 4: Settings.json (Medium)

### 4a. Add `once: true` to Setup hook
```json
"Setup": [{ "hooks": [{ "type": "command", "command": "...", "once": true }] }]
```

### 4b. Add useful missing env vars
- `CLAUDE_CODE_SHELL`: "bash" (explicit shell override)
- `CLAUDE_CODE_FILE_READ_MAX_OUTPUT_TOKENS`: leave unset (default is fine)

### 4c. Fix `disableBypassPermissionsMode`
Current: `"disable"` — should be `true`

---

## Phase 5: Rules (Medium — new rule + updates)

### 5a. Create `rules/agent-teams.md`
New rule covering:
- When to use agent teams vs solo agents
- Task delegation patterns (TaskCreate → assign → track)
- Message passing (SendMessage, broadcast sparingly)
- Team lifecycle (TeamCreate → work → shutdown → TeamDelete)
- CPI-SI family dispatch within teams
- Ctrl+F to kill all background agents, Ctrl+B to background

### 5b. Update `rules/task-decomposition.md`
- Line 58: fix remaining `TodoWrite` reference if any
- Add TaskGet for reading full task details
- Add task deletion via TaskUpdate `status: "deleted"`

---

## Phase 6: Verification

1. Run `deno task test` in cws-struct to ensure nothing broke
2. Verify settings.json is valid JSON after edits
3. Spot-check 2-3 agent files for correct YAML frontmatter
4. Spot-check 2-3 skill files for correct frontmatter
5. Read the new agent-teams rule for completeness

---

## Files Modified

| Category | Files | Count |
|----------|-------|-------|
| **Hooks source** | `cmd/subagent-stop/main.go`, `cmd/pre-use/main.go` | 2 |
| **Agents** | All 13 `~/.claude/agents/*.md` | 13 |
| **Skills** | All 18 `~/.claude/skills/*/SKILL.md` | 18 |
| **Settings** | `~/.claude/settings.json` | 1 |
| **Rules** | `rules/agent-teams.md` (new), `rules/task-decomposition.md` | 2 |
| **Total** | | 36 |

## Execution Order

Phase 1 → 2 → 3 → 4 → 5 → 6 (sequential, each phase completes before next)

Within each phase, independent file edits can be parallelized.
