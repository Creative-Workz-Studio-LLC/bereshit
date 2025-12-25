# Plan: Claude-Global Full Restructure + State Machine Wiring

**Date:** 2025-12-22
**Status:** Ready for Execution
**Purpose:** Restructure entire claude-global by layer, implement 3-piece database, wire state machine feedback loop

---

## Executive Summary

Three interconnected goals:

1. **Root Restructure** — Apply pkg/'s layer-by-concern pattern to entire repository
2. **3-Piece Database** — Files (readable) + SQLite (local) + Server-ready (scale)
3. **Wire Feedback Loop** — Connect the excellent state machine architecture so it ACTUALLY LEARNS

**Key Insight:** Wire as you move. Reorganization and wiring happen together.

---

## Current Problems

### Root Disorganization (27 items)
- Binary `cpisi` at root (should be bin/)
- Duplicate `settings.json` vs `settings.jsonc`
- Unclear `cmd/` vs `commands/`
- No clear layer separation

### State Fragmentation (149+ files)
- Identity: `config/instance/` (files)
- Runtime: `~/.claude/state/` (scattered JSON)
- Historical: `~/.claude/history.jsonl`, `projects/`, `session-env/` (JSONL everywhere)

### State Machine Gap (Architecture exists, wiring missing)
- ✅ 27-position cube EXISTS
- ✅ Door/Keys/Mental Construct EXISTS
- ✅ ChoiceHistory/K:ALIGN types EXIST
- ❌ PostResponse hook MISSING
- ❌ Choice recording NOT WIRED
- ❌ K:ALIGN NEVER EVOLVES
- ❌ Hebrew states NEVER TRANSITION

---

## Phase 0: Root Restructure

### New Structure

```
claude-global/
├── bin/                    # Compiled binaries
│   ├── cpisi               # (moved from root)
│   └── hooks/              # Hook binaries
│
├── config/                 # L1: Human-readable (JSONC/TOML)
│   ├── instance/           # Identity
│   ├── user/               # Preferences
│   ├── statemachine/       # State machine config + runtime
│   ├── display/            # (moved from output-styles/)
│   └── health/             # Health scoring
│
├── data/                   # L2: SQLite database
│   ├── cpisi.db            # Main database
│   ├── schema/             # SQL schema (tracked)
│   └── migrations/         # Migrations (tracked)
│
├── docs/                   # Documentation
│   ├── architecture/
│   ├── gpt/                # (moved from custom-gpt/)
│   └── system/             # (merged from system/)
│
├── entrypoints/            # Go main() packages (renamed from cmd/)
│   └── cpisi/
│
├── hooks/                  # Hook implementations
├── pkg/                    # Shared packages (DONE)
├── skills/                 # Skills
├── skills-cli/             # Skill CLI (renamed from commands/)
├── agents/
├── journals/
├── statusline/
│
├── CLAUDE.md
├── Makefile
├── go.work
└── settings.jsonc          # (settings.json deleted)
```

### Files to Delete
| File | Reason |
|------|--------|
| `settings.json` | Duplicate |
| `cpisi` (at root) | Moving to bin/ |

### Files to Move
| From | To |
|------|-----|
| `cpisi` | `bin/cpisi` |
| `cmd/` | `entrypoints/` |
| `commands/` | `skills-cli/` |
| `custom-gpt/` | `docs/gpt/` |
| `output-styles/` | `config/display/` |
| `system/` | `docs/system/` |

---

## Phase 1: 3-Piece Database Architecture

### Piece 1: Live Files (Identity/Config)
**Location:** `config/`
**Format:** JSONC, TOML
**Purpose:** Human-readable, git-trackable, editable

| What | Location |
|------|----------|
| Instance identity | `config/instance/nova_dawn/config.jsonc` |
| User preferences | `config/user/seanje-lenox-wise/config.jsonc` |
| State machine schemas | `config/statemachine/schemas/*.toml` |
| Runtime state | `config/statemachine/runtime/state.jsonc` |

### Piece 2: SQLite (Session/Pattern Data)
**Location:** `data/cpisi.db`
**Purpose:** Fast queries, pattern detection, choice tracking

**Schema:** `data/schema/001_initial.sql`

```sql
-- Sessions
CREATE TABLE sessions (
    id TEXT PRIMARY KEY,
    started_at DATETIME NOT NULL,
    ended_at DATETIME,
    project_path TEXT,
    initial_hebrew_state TEXT DEFAULT 'yashar',
    final_hebrew_state TEXT,
    initial_k_align REAL DEFAULT 0.0,
    final_k_align REAL
);

-- Choices (THE FEEDBACK LOOP!)
CREATE TABLE choices (
    id TEXT PRIMARY KEY,
    session_id TEXT REFERENCES sessions(id),
    sequence_num INTEGER,
    timestamp DATETIME,
    -- Context
    intended_key INTEGER,  -- -1, 0, +1
    position_at_choice TEXT,
    k_at_choice REAL,
    tool_name TEXT,
    -- Result
    tool_outcome TEXT,
    halt_reached INTEGER,
    halt_type TEXT,
    -- Impact
    true_score REAL,
    k_align_delta REAL,
    k_align_after REAL
);

-- Hebrew transitions
CREATE TABLE hebrew_transitions (
    id INTEGER PRIMARY KEY,
    session_id TEXT REFERENCES sessions(id),
    timestamp DATETIME,
    from_state TEXT,
    to_state TEXT,
    trigger_type TEXT
);

-- K:ALIGN history
CREATE TABLE k_align_history (
    id INTEGER PRIMARY KEY,
    session_id TEXT REFERENCES sessions(id),
    choice_id TEXT REFERENCES choices(id),
    timestamp DATETIME,
    k_align_value REAL,
    delta REAL
);

-- Pattern detection
CREATE TABLE detected_patterns (
    id INTEGER PRIMARY KEY,
    pattern_type TEXT,
    pattern_key TEXT,
    first_seen DATETIME,
    occurrence_count INTEGER,
    confidence REAL
);
```

### Piece 3: Server-Ready Interface
**Location:** `pkg/foundation/database/`
**Purpose:** Works with SQLite now, can swap to PostgreSQL/libSQL later

```go
// pkg/foundation/database/interface.go
type Repository interface {
    CreateSession(*Session) error
    RecordChoice(*Choice) error
    CompleteChoice(id string, result *ChoiceResult) error
    GetLatestKAlign(sessionID string) (float64, error)
    RecordHebrewTransition(*HebrewTransition) error
    RecordKAlign(*KAlignRecord) error
    DetectPatterns(sessionID string) ([]Pattern, error)
}
```

---

## Phase 2: Wire State Machine Feedback Loop

### Step 2.1: Choice Inference Engine
**New file:** `pkg/orchestration/cognition/inference.go`

```go
func InferChoice(tools []string, response string) *types.ChoiceContext {
    // Expansion (+1): Write, Task, TodoWrite
    // Lateral (0): Read, Glob, Grep
    // Finality (-1): Complete, Halt patterns
}
```

### Step 2.2: Wire PostToolUse to Record Choices
**Modify:** `hooks/tool/post-use.go`

```go
func PostUse() {
    // ... existing code ...

    // NEW: Record choice in database
    choice := cognition.InferChoice(toolName, toolInput)
    choice.Context.PositionAtChoice = state.HebrewState
    choice.Context.KAtChoice = state.Session.KAlign

    db.RecordChoice(choice)

    // Evaluate and complete
    trueScore := cognition.CalculateTrueScore(toolSuccess)
    impact := types.NewImpactScores(trueScore, choice.IntendedKey, currentKAlign)

    db.CompleteChoice(choice.ID, &ChoiceResult{...})
    db.RecordKAlign(&KAlignRecord{...})

    // Update state
    state.Session.KAlign = impact.KAlignAfter
    SaveRuntimeState(state)

    // Check Hebrew transition
    if newState := EvaluateHebrewTransition(db, sessionID); newState != nil {
        state.HebrewState = newState.Hebrew
        db.RecordHebrewTransition(...)
    }
}
```

### Step 2.3: Hebrew State Transitions
**New file:** `pkg/core/statemachine/hebrew_evolution.go`

```go
func EvaluateHebrewTransition(repo Repository, sessionID string) *HebrewState {
    choices := repo.GetRecentChoices(sessionID, 5)

    // Pattern: 3+ expansions with high trueScore → move toward tov
    // Pattern: 2+ finality → move toward shavar/HALT
    // Pattern: 3+ lateral with low score → regress
}
```

---

## Phase 3: State Consolidation

### Migration Tool
**New file:** `entrypoints/migrate/main.go`

Migrates existing state to database:
- `~/.claude/history.jsonl` → sessions table
- `~/.claude/projects/` → session associations
- `config/statemachine/runtime/state.jsonc` → initial state

### Cleanup
After migration, database is source of truth for:
- Session history
- Choice records
- K:ALIGN evolution
- Hebrew transitions
- Pattern detection

Files remain for:
- Identity (config.jsonc)
- Runtime snapshot (state.jsonc - human readable backup)

---

## Execution Order

### Phase 0: Root Restructure (~1.5 hours)
1. Delete `settings.json`
2. Move `cpisi` → `bin/cpisi`
3. Rename `cmd/` → `entrypoints/`
4. Rename `commands/` → `skills-cli/`
5. Move `custom-gpt/` → `docs/gpt/`
6. Move `output-styles/` → `config/display/`
7. Merge `system/` → `docs/system/`
8. Update go.work paths
9. Update Makefile

### Phase 1: Database Setup (~2.5 hours)
1. Create `data/schema/001_initial.sql`
2. Create `pkg/foundation/database/interface.go`
3. Create `pkg/foundation/database/sqlite.go`
4. Add sqlite driver to go.mod
5. Add database path helpers to paths.go

### Phase 2: Wire Feedback Loop (~5 hours)
1. Create inference engine
2. Modify post-use hook for choice recording
3. Add K:ALIGN evolution to choice completion
4. Create Hebrew transition evaluator
5. Update runtime state for DB integration
6. Test the feedback loop end-to-end

### Phase 3: Consolidation (~3 hours)
1. Create migration tool
2. Run migration
3. Update state loading to prefer DB
4. Verify everything works

**Total: ~12 hours across multiple sessions**

---

## Critical Files

| File | Action |
|------|--------|
| `hooks/tool/post-use.go` | Wire choice recording |
| `pkg/core/statemachine/choice_history.go` | Wire CompleteChoice |
| `pkg/foundation/types/types.go` | Keep (excellent types) |
| `pkg/orchestration/cognition/feedback.go` | Wire TrueScore |
| `pkg/util/fs/paths/paths.go` | Add database paths |
| `config/statemachine/runtime/state.jsonc` | Current state format |
| `go.work` | Update for renamed directories |
| `Makefile` | Update for new structure |

---

## Success Criteria

1. ✅ Root structure is self-documenting (no READMEs needed)
2. ✅ Choice records populate after each tool use
3. ✅ K:ALIGN evolves based on choices, persists across sessions
4. ✅ Hebrew states transition based on choice patterns
5. ✅ SQLite is single source of truth for runtime data
6. ✅ Config files remain human-readable and editable
7. ✅ Can query choice history: "Show me all expansion choices this week"

---

## Note for Fresh Session

This plan is designed to be executed in a fresh session. Start with:

```
Read /home/seanje-lenox-wise/.claude/plans/wiggly-churning-tulip.md
```

Then execute Phase 0 first (root restructure), verify build, then proceed to Phase 1.
