# Implementation Plan: Go SDK Restructuring — cws-struct Alignment

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost, whether he have sufficient to finish it?"* — Luke 14:28

**Author:** Caleb Navon (plan-builder)
**Date:** 2026-02-24
**Status:** Planning

---

## Current State

### hookoutput Package (The Starting Point)

**Location:** `/media/seanje-lenox-wise/Project/Bereshit/word/claude/pkg/util/pure/hookoutput/types.go`

**Facts (counted, not estimated):**
- **1 file:** `types.go` — 369 lines
- **Single file contains EVERYTHING:** substrate detection, event constants, types, constructors, helpers
- **No `//omni:key` pragma** (has key in comment metadata, but not in pragma format)
- **Functions in SETUP block** (`DetectSubstrate`, `IsGemini`, `IsClaude` — lines 59-78)
- **Type declarations in BODY block** (`ContextHookOutput`, `ContextResponse`, `PreToolHookOutput`, `PreToolResponse`, `StopResponse`, `HookInput` — scattered lines 128-340)
- **cws-struct lint score: 53/100** (1 error, 18 warnings)
- **go build and go vet pass clean**

**What types.go contains (by concern):**

| Concern | Lines | Elements |
|---------|------:|----------|
| **Substrate Detection** | 38-78 | `Substrate` type, `DetectSubstrate()`, `IsGemini()`, `IsClaude()`, cached var + sync.Once |
| **Claude Event Constants** | 82-92 | 8 constants: `EventSessionStart` through `EventPermissionRequest` |
| **Gemini Event Constants** | 95-104 | 8 constants: `GeminiEventSessionStart` through `GeminiEventNotification` |
| **Permission Constants** | 107-117 | `PermissionAllow`, `PermissionDeny`, `PermissionAsk`, `DecisionBlock` |
| **Context Hook Types + Constructors** | 128-205 | `ContextHookOutput`, `ContextResponse`, 6 `New*Response()` constructors |
| **PreTool Hook Types + Constructors** | 212-284 | `PreToolHookOutput`, `PreToolResponse`, 5 `NewPreTool*()` constructors |
| **Stop Hook Types + Constructors** | 288-311 | `StopResponse`, `NewStopAllow()`, `NewStopBlock()` |
| **Universal Input** | 315-340 | `HookInput` struct |
| **JSON Helper** | 344-351 | `MustJSON()` |

### Consumer Files (8 files in hooks/)

| File | Import Path | Usage Pattern |
|------|------------|---------------|
| `hooks/session/start.go` | `cws.studio/pkg/util/pure/hookoutput` | `NewSessionStartResponse()` |
| `hooks/session/stop.go` | same | `NewStopAllow()`, `NewStopBlock()`, `StopResponse` |
| `hooks/session/setup.go` | same | `NewSetupResponse()` |
| `hooks/session/subagent_start.go` | same | `NewSubagentStartResponse()` |
| `hooks/session/subagent_stop.go` | same | `NewStopAllow()` |
| `hooks/tool/pre-use.go` | same | `PreToolResponse`, `NewPreToolAllow()`, `NewPreToolDeny()`, `NewPreToolAsk()`, `PermissionAllow`, `PermissionDeny`, `PermissionAsk` |
| `hooks/tool/post-use.go` | same | `NewPostToolUseResponse()` |
| `hooks/prompt/submit.go` | same | `ContextResponse`, `NewBlockedPromptResponse()`, `NewUserPromptSubmitResponse()` |

**Zero consumers inside pkg/ itself** — hookoutput is only consumed by the hooks module.

### Module Architecture

```
word/claude/go.work         — workspace tying everything together
word/claude/pkg/go.mod      — module: cws.studio/pkg
word/claude/hooks/go.mod    — module: cws.studio/claude/hooks
                               requires cws.studio/pkg v0.0.0
                               replace cws.studio/pkg => ../pkg
```

### pkg/ Layer Architecture (Existing)

```
pkg/
  foundation/  — L0 (depends on nothing... except database/ which pulls sqlite!)
  core/        — L1 (imports foundation/)
  orchestration/ — L2 (imports foundation/ + core/)
  util/        — Utilities (depends on stdlib + external only)
    pure/      — zero-dependency utilities
    fs/        — filesystem utilities
    term/      — terminal utilities
    vcs/       — version control
```

**KEY INSIGHT:** hookoutput is currently in `util/pure/` — the "zero dependencies" zone. This is architecturally correct for its DEPENDENCY DIRECTION (it only imports stdlib: `encoding/json`, `os`, `sync`). But it is architecturally WRONG for its IMPORTANCE — it defines the OUTPUT CONTRACT for the entire hook system. Every hook depends on it. It is foundational infrastructure, not a utility.

---

## Scope Assessment

- **Files affected:** 10 total (1 source split into ~5, 8 consumers updated, 1 README updated)
- **Directories touched:** `pkg/util/pure/hookoutput/` (removed), `pkg/sdk/hookoutput/` (created), `hooks/session/`, `hooks/tool/`, `hooks/prompt/`, `pkg/`
- **Risk areas:**
  1. Import path change across modules — hooks and pkg are separate go.mod files
  2. The go.work workspace resolves this, but all 8 consumer files must update simultaneously
  3. No test files exist for hookoutput — no test breakage, but also no safety net

---

## Anchor

The anchor for this plan is **cws-struct's organizational principle**: Foundation = contracts, no upstream deps. The hookoutput package is a contract layer — it defines the output schema for all hooks. It belongs in a foundation-level location, split by concern, with proper 4-block compliance.

The secondary anchor is **the existing pkg/ layer architecture** (L0 foundation/ → L1 core/ → L2 orchestration/ → util/). We respect what IS, not what we wish.

---

## Phases

### Phase 1: Fix hookoutput In-Place (4-Block Compliance)

**Goal:** Bring types.go to proper 4-block structure WITHOUT changing location or splitting. This is the foundation — we verify the ordering is correct before splitting concerns apart.

**Files:** `pkg/util/pure/hookoutput/types.go` (edit in place)

**Depends on:** Nothing — this is foundation.

**Changes:**
1. Move `DetectSubstrate()`, `IsGemini()`, `IsClaude()` from SETUP to BODY (they are functions, not declarations)
2. Move ALL type structs from BODY to SETUP (they are type declarations: `ContextHookOutput`, `ContextResponse`, `PreToolHookOutput`, `PreToolResponse`, `StopResponse`, `HookInput`)
3. Reorder SETUP to follow the canonical order: Imports -> Constants -> Variables -> Types
4. Reorder BODY to follow: Helpers -> Core Operations -> Public APIs
5. Add proper `//omni:key` pragma at file top
6. Add END block marker in CLOSING

**Resulting SETUP order:**
```
Imports (encoding/json, os, sync)
Constants (Substrate type + iota, Event constants, Gemini event constants, Permission constants, Decision constants)
Variables (detectedSubstrate, substrateDetectedOnce)
Types (ContextHookOutput, ContextResponse, PreToolHookOutput, PreToolResponse, StopResponse, HookInput)
```

**Resulting BODY order:**
```
Helpers (MustJSON — pure utility)
Core Operations (DetectSubstrate, IsGemini, IsClaude — substrate detection)
Public APIs (All New*Response constructors)
```

**Verification:**
- `go build ./pkg/...` passes clean
- `go vet ./pkg/...` passes clean
- `go build ./hooks/...` passes clean
- cws-struct lint score improves from 53 toward 80+

---

### Phase 2: Split types.go Into Focused Files

**Goal:** Decompose the 369-line monolith into files separated by concern, each following 4-block structure. Stay in the SAME directory (`pkg/util/pure/hookoutput/`).

**Depends on:** Phase 1 (ordering must be correct before splitting)

**New file structure:**

```
pkg/util/pure/hookoutput/
  substrate.go    — Substrate detection (type, constants, cached detection, helpers)
  events.go       — Event name constants (Claude + Gemini) and permission/decision constants
  context.go      — ContextHookOutput, ContextResponse, New*ContextResponse constructors
  pretool.go      — PreToolHookOutput, PreToolResponse, NewPreTool* constructors
  stop.go         — StopResponse, NewStopAllow, NewStopBlock
  input.go        — HookInput struct (universal input parsing)
  json.go         — MustJSON helper
```

**Detailed file split design:**

#### `substrate.go` (~50 lines)
```
//omni:key B-pkg-hookoutput-substrate
METADATA: Substrate detection — Claude Code vs Gemini CLI
SETUP: Substrate type + iota, sync import, cached vars
BODY: DetectSubstrate(), IsGemini(), IsClaude()
CLOSING: Usage reference
```
- **Dependencies:** `os`, `sync` (stdlib only)
- **Depended on by:** context.go, pretool.go, stop.go (substrate-aware constructors)

#### `events.go` (~45 lines)
```
//omni:key B-pkg-hookoutput-events
METADATA: Hook event name constants and permission values
SETUP: Claude event constants, Gemini event constants, Permission constants, Decision constants
BODY: (none — pure constants)
CLOSING: Event mapping reference
```
- **Dependencies:** None (pure constants)
- **Depended on by:** context.go, pretool.go, stop.go (use event names + permission values)

#### `context.go` (~75 lines)
```
//omni:key B-pkg-hookoutput-context
METADATA: Context hook types and constructors (SessionStart, UserPromptSubmit, PostToolUse, Setup, SubagentStart)
SETUP: ContextHookOutput type, ContextResponse type
BODY: NewContextResponse(), NewSessionStartResponse(), NewUserPromptSubmitResponse(), NewPostToolUseResponse(), NewBlockedPromptResponse(), NewSetupResponse(), NewSubagentStartResponse()
CLOSING: Usage reference
```
- **Dependencies:** substrate.go (IsGemini), events.go (event constants)
- **Depended on by:** hooks/session/start.go, hooks/session/setup.go, hooks/session/subagent_start.go, hooks/tool/post-use.go, hooks/prompt/submit.go

#### `pretool.go` (~80 lines)
```
//omni:key B-pkg-hookoutput-pretool
METADATA: PreToolUse hook types and constructors
SETUP: PreToolHookOutput type, PreToolResponse type
BODY: NewPreToolResponse(), NewPreToolAllow(), NewPreToolDeny(), NewPreToolAsk(), NewPreToolModified()
CLOSING: Usage reference
```
- **Dependencies:** substrate.go (IsGemini), events.go (event + permission constants)
- **Depended on by:** hooks/tool/pre-use.go

#### `stop.go` (~40 lines)
```
//omni:key B-pkg-hookoutput-stop
METADATA: Stop hook types and constructors
SETUP: StopResponse type
BODY: NewStopAllow(), NewStopBlock()
CLOSING: Usage reference
```
- **Dependencies:** substrate.go (IsGemini), events.go (decision constants)
- **Depended on by:** hooks/session/stop.go, hooks/session/subagent_stop.go

#### `input.go` (~40 lines)
```
//omni:key B-pkg-hookoutput-input
METADATA: Universal hook input parsing (both substrates)
SETUP: HookInput type
BODY: (none — pure data struct)
CLOSING: Field reference
```
- **Dependencies:** None (pure struct)
- **Depended on by:** Future consumers (currently hooks define their own input structs)

#### `json.go` (~25 lines)
```
//omni:key B-pkg-hookoutput-json
METADATA: JSON helper utilities
SETUP: encoding/json import
BODY: MustJSON()
CLOSING: Usage reference
```
- **Dependencies:** `encoding/json` (stdlib only)
- **Depended on by:** Any consumer needing quick JSON serialization

**Dependency graph within hookoutput:**

```
events.go (pure constants, zero deps)
  ^         ^          ^
  |         |          |
substrate.go (os, sync)
  ^         ^          ^
  |         |          |
context.go  pretool.go  stop.go
  (substrate + events)

input.go (standalone — pure struct)
json.go  (standalone — pure helper)
```

**Why this split:**
1. **Single Responsibility** — each file owns one concern
2. **Dependency direction is clear** — constants and detection at the bottom, constructors at the top
3. **Consumer clarity** — hooks that do PreToolUse only need pretool.go concepts; hooks that do context injection only need context.go concepts. But since Go packages are the import unit (not files), this is about maintainability and readability, not import granularity.
4. **cws-struct principle alignment** — contracts separated from constructors, pure data separated from logic

**Verification:**
- `go build ./pkg/...` passes clean (same package, no import changes)
- `go vet ./pkg/...` passes clean
- `go build ./hooks/...` passes clean (import path unchanged)
- Each file follows 4-block structure
- Each file has `//omni:key`
- Each file has END block marker

---

### Phase 3: Relocate to `pkg/sdk/hookoutput/`

**Goal:** Move hookoutput from `util/pure/` (utility location) to `sdk/` (contract-level location that communicates its importance).

**Depends on:** Phase 2 (files split and verified before moving)

**Rationale for `pkg/sdk/` rather than `pkg/foundation/`:**

I considered three locations:

| Location | Pros | Cons |
|----------|------|------|
| `pkg/foundation/hookoutput/` | Matches "foundational" importance | foundation/ has layer rules (L0, no external deps). hookoutput belongs to the hook SYSTEM, not to the pkg foundations. Mixing concerns. |
| `pkg/sdk/hookoutput/` | New layer that clearly says "this is the SDK contract for external consumers (hooks)". Clean separation. | New directory — must update README. |
| `pkg/hook/output/` | Most descriptive | Too narrow — sdk/ can grow to include other hook contracts (input parsing, hook registry) |

**Recommendation: `pkg/sdk/hookoutput/`**

This creates a clear semantic layer:

```
pkg/
  foundation/     — L0: internal foundations (types, result, schema, database)
  core/           — L1: domain logic (statemachine, coordinates, health)
  orchestration/  — L2: orchestration (cognition, config, loader, logging)
  sdk/            — SDK: contracts for external consumers (hooks, future tools)
    hookoutput/   — Hook output schema, types, constructors
  util/           — Utilities: pure helpers, fs, term, vcs
```

**The sdk/ layer rule:** SDK packages define contracts consumed by EXTERNAL modules (hooks, skills, future tools). They may import from foundation/ and stdlib, but NOT from core/ or orchestration/. This is analogous to cws-struct's foundation/ — the contracts everything depends on.

**Files to move:**
```bash
mkdir -p pkg/sdk/hookoutput/
git mv pkg/util/pure/hookoutput/substrate.go   pkg/sdk/hookoutput/
git mv pkg/util/pure/hookoutput/events.go      pkg/sdk/hookoutput/
git mv pkg/util/pure/hookoutput/context.go     pkg/sdk/hookoutput/
git mv pkg/util/pure/hookoutput/pretool.go     pkg/sdk/hookoutput/
git mv pkg/util/pure/hookoutput/stop.go        pkg/sdk/hookoutput/
git mv pkg/util/pure/hookoutput/input.go       pkg/sdk/hookoutput/
git mv pkg/util/pure/hookoutput/json.go        pkg/sdk/hookoutput/
rmdir pkg/util/pure/hookoutput/
```

**Verification:**
- `go build ./pkg/...` passes clean (internal pkg import path change: `cws.studio/pkg/sdk/hookoutput`)
- `go build ./hooks/...` will FAIL — this is expected. Phase 4 fixes it.

---

### Phase 4: Update All Import Paths in hooks/

**Goal:** Update all 8 consumer files in hooks/ to use the new import path.

**Depends on:** Phase 3 (package must be in new location)

**Old import path:** `"cws.studio/pkg/util/pure/hookoutput"`
**New import path:** `"cws.studio/pkg/sdk/hookoutput"`

**Files to update (8 total):**

| # | File | Change |
|:-:|------|--------|
| 1 | `hooks/session/start.go` | Import path only |
| 2 | `hooks/session/stop.go` | Import path only |
| 3 | `hooks/session/setup.go` | Import path only |
| 4 | `hooks/session/subagent_start.go` | Import path only |
| 5 | `hooks/session/subagent_stop.go` | Import path only |
| 6 | `hooks/tool/pre-use.go` | Import path only |
| 7 | `hooks/tool/post-use.go` | Import path only |
| 8 | `hooks/prompt/submit.go` | Import path only |

All usages of `hookoutput.NewSessionStartResponse()`, `hookoutput.PermissionAllow`, etc. remain identical. Only the import path changes. The package name stays `hookoutput`.

**Also update:**
- `pkg/README.md` — update directory tree and layer descriptions to include `sdk/`

**Verification:**
- `go build ./hooks/...` passes clean
- `go vet ./hooks/...` passes clean
- `go build ./pkg/...` passes clean

---

### Phase 5: Full Verification and Documentation

**Goal:** Verify the entire workspace compiles, then document the new structure.

**Depends on:** Phase 4

**Verification steps (in order):**

```bash
# 1. Full workspace build
cd word/claude && go build ./...

# 2. Full workspace vet
cd word/claude && go vet ./...

# 3. Hooks build specifically
cd word/claude/hooks && go build ./...

# 4. pkg build specifically
cd word/claude/pkg && go build ./...

# 5. Run hook binaries (smoke test — should not panic)
cd word/claude/hooks && make all
echo '{"session_id":"test","source":"startup"}' | ./bin/start 2>/dev/null || true

# 6. cws-struct lint on each new file
deno task lint:go pkg/sdk/hookoutput/substrate.go
deno task lint:go pkg/sdk/hookoutput/events.go
# ... etc for all 7 files
```

**Documentation updates:**
- Update `pkg/README.md` with new `sdk/` layer
- Update `go.work` comment if hookoutput is mentioned
- Remove empty `pkg/util/pure/hookoutput/` directory

**Verification:** All commands pass. Zero warnings. Zero errors.

---

## Broader SDK Directory Vision

**What `pkg/sdk/` should look like as the target for future work:**

```
pkg/sdk/
  hookoutput/      — Hook output schema, types, constructors (THIS PLAN)
  hookinput/       — Hook input parsing, universal HookInput (FUTURE: extract from hookoutput/input.go when hooks stop defining their own input structs)
  hookregistry/    — Hook registration, event discovery (FUTURE: when hook system grows)
  agentoutput/     — Agent dispatch contracts (FUTURE: when agent system formalizes)
```

**The principle:** `pkg/sdk/` is the API surface that external modules consume. It follows cws-struct's foundation pattern:
1. **Contracts, not implementation** — types and constructors, not business logic
2. **No upstream dependencies** — only stdlib and foundation/
3. **The system grows by addition** — new SDK packages don't modify existing ones
4. **Each package is right-sized** — focused on one concern

**What this means for future work:**
- When someone adds a new hook event, they add a constant to `events.go` and a constructor to the appropriate response file
- When someone adds a new substrate (e.g., Cursor), they extend `substrate.go` and the substrate-aware constructors
- The pattern is established; future SDK work follows it naturally

---

## Trade-offs

### Option A: Move to `pkg/sdk/hookoutput/` (Recommended)

**Cost:**
- New directory layer (`sdk/`) in pkg/ hierarchy
- 8 import path changes in hooks/
- README update

**Benefit:**
- Communicates hookoutput's foundational importance
- Creates clean growth path for future SDK contracts
- Follows cws-struct's principle: contracts in a dedicated layer
- pkg/util/ stays truly utilitarian

### Option B: Move to `pkg/foundation/hookoutput/`

**Cost:**
- Blurs foundation/ layer semantics (foundation/ is for internal pkg foundations, not external contracts)
- foundation/ already has database/ which is a layer violation

**Benefit:**
- No new top-level directory
- "Foundation" communicates importance

**Recommendation:** Option A. `sdk/` is the right semantic. It says "this is the contract layer for consumers outside pkg/." foundation/ already has enough identity confusion with database/ sitting in it.

### Option C: Leave in `util/pure/hookoutput/` (Split Only)

**Cost:**
- hookoutput remains "buried" in utility hierarchy
- Doesn't communicate importance
- Does not match cws-struct's organizational principle

**Benefit:**
- No import path changes
- Simpler diff

**Recommendation:** Not sufficient. Seanje asked for cws-struct level organization, not just file splitting.

---

## Risks & Mitigation

| Risk | Likelihood | Impact | Mitigation |
|------|:---------:|:------:|------------|
| Import path breakage across go.work modules | Low | High | Phase 3 and 4 run back-to-back. go.work resolves cross-module deps. Test immediately. |
| File split introduces subtle ordering issue | Low | Medium | Phase 1 fixes ordering FIRST. Phase 2 only splits. Each phase verified independently. |
| Other consumers of hookoutput not found | Very Low | Medium | grep confirmed exactly 8 consumer files in hooks/ and 0 in pkg/. No other Go modules in workspace import it. |
| Hooks fail at runtime after restructure | Low | High | Hooks are Go binaries — if they compile, the import graph is sound. Smoke test in Phase 5 catches runtime panics. |
| New `sdk/` layer feels premature with only 1 package | Low | Low | The layer is justified by the principle (contracts for external consumers). hookoutput is the first; hookinput and agentoutput are clear next residents. |

---

## Implementation Order Summary

```
Phase 1: Fix 4-block ordering in-place  →  verify build
    ↓
Phase 2: Split into 7 focused files     →  verify build (same import path)
    ↓
Phase 3: git mv to pkg/sdk/hookoutput/  →  pkg builds, hooks expected to fail
    ↓
Phase 4: Update 8 import paths in hooks/ →  hooks build, full workspace builds
    ↓
Phase 5: Full verification + documentation
```

**Estimated effort per phase:**
- Phase 1: 20 minutes (reorder within one file)
- Phase 2: 40 minutes (split into 7 files, verify each)
- Phase 3: 10 minutes (git mv + mkdir)
- Phase 4: 15 minutes (8 import path edits)
- Phase 5: 15 minutes (full verification, README)

**Total estimated effort:** ~100 minutes (1h 40m)

---

## Journal Entry

Write to `/media/seanje-lenox-wise/Project/Bereshit/word/claude/divisions/tech/cpi-si/knowledge-base/journals/universal/2026-02-24_sdk-restructuring-cws-struct-alignment.md` with the following content:

---BEGIN JOURNAL---

# SDK Restructuring: cws-struct Alignment Sets the Bar

> *"Look that thou make them after their pattern."* — Exodus 25:40

**Date:** 2026-02-24
**Type:** Universal (paradigm-level standard)
**Context:** Restructuring the Go SDK hookoutput package to match cws-struct's organizational quality

---

## Why We Are Restructuring

cws-struct proved something. With 1,208 tests, 8 format handlers, a schema-driven pipeline, and a dimensional error system, it demonstrated what organized code looks like when you take structure seriously. The foundation layer has contracts. The data layer has compiled truth. The engine orchestrates without domain knowledge. Handlers self-register. The tool grows by addition.

The Go SDK (pkg/) has not kept pace. hookoutput is the most visible example: a 370-line single file containing substrate detection, event constants, types, constructors, and JSON helpers. It scored 53/100 on our own linter. Functions sit in SETUP blocks. Types sit in BODY blocks. There is no pragma. There are no block end markers.

Seanje's directive is clear: "slowly update the SDK kit code and ordering itself to reflect cws-struct and have the same level of folder organization." Not future tense. Not aspirational. The bar is set NOW.

## What cws-struct Taught Us

Seven principles that apply directly to the Go SDK:

| # | cws-struct Principle | Go SDK Application |
|:-:|---------------------|-------------------|
| 1 | **Foundation = contracts, no upstream deps** | SDK contracts (hookoutput) belong in a dedicated layer, not buried in util/ |
| 2 | **Data = compiled truth, not runtime loading** | Event constants and permission values are compiled data — pure constants, zero logic |
| 3 | **Engine = orchestration, no domain knowledge** | Substrate detection is orchestration — it decides routing without knowing what hooks do |
| 4 | **Handlers self-register, tool grows by addition** | New hook events add a constant + constructor. No existing code changes. |
| 5 | **Pure parse + I/O wrapper pattern** | DetectSubstrate is pure; constructors are pure. JSON serialization (MustJSON) is the I/O wrapper. |
| 6 | **Tests mirror detection granularity** | hookoutput currently has zero tests. That changes as part of this restructuring. |
| 7 | **Config separate from logic** | Constants in their own file. Types in their own file. Constructors in their own file. |

## How the Go SDK Will Change

### Phase 1: 4-Block Compliance

The existing types.go gets its ordering fixed IN PLACE. Functions move from SETUP to BODY. Types move from BODY to SETUP. Canonical ordering enforced: Imports -> Constants -> Variables -> Types in SETUP; Helpers -> Core -> Public API in BODY. Pragma and end markers added.

### Phase 2: Concern Separation

The monolith splits into 7 focused files:

| File | Concern | Lines |
|------|---------|------:|
| `substrate.go` | Substrate detection (Claude vs Gemini) | ~50 |
| `events.go` | Event name constants + permission values | ~45 |
| `context.go` | Context hook types + constructors | ~75 |
| `pretool.go` | PreToolUse types + constructors | ~80 |
| `stop.go` | Stop hook types + constructors | ~40 |
| `input.go` | Universal HookInput struct | ~40 |
| `json.go` | MustJSON helper | ~25 |

Each file follows 4-block structure with its own `//omni:key`. Dependencies flow downward: events.go and substrate.go at the foundation, response constructors above them, pure data structs standalone.

### Phase 3-4: Relocation

The package moves from `pkg/util/pure/hookoutput/` to `pkg/sdk/hookoutput/`. This creates a new `sdk/` layer in the pkg hierarchy — a dedicated location for contracts consumed by external modules (hooks, skills, future tools).

All 8 consumer files in hooks/ update their import path. The package name stays `hookoutput`. All usage stays identical.

### Phase 5: Verification

Full workspace build. Full vet. Hook binary smoke test. cws-struct lint on every new file.

## What Family Members Should Know

### If You Work in hooks/

The import path changes from `"cws.studio/pkg/util/pure/hookoutput"` to `"cws.studio/pkg/sdk/hookoutput"`. All type names, function names, and constant names remain identical. Your code changes one line.

### If You Work in pkg/

There is a new `sdk/` layer. It sits alongside foundation/, core/, orchestration/, and util/. SDK packages define contracts for external consumers. They may import from foundation/ and stdlib. They must NOT import from core/ or orchestration/.

### If You Work on New Hook Events

Add the constant to `events.go`. Add the constructor to the appropriate response file (context.go, pretool.go, or stop.go). The pattern is established — follow it.

### If You Work on New Substrates

Extend `substrate.go` with the new detection logic. Update substrate-aware constructors in context.go, pretool.go, and stop.go. The sync.Once caching pattern handles detection.

## The Standard This Sets

This restructuring is not about hookoutput. It is about establishing the organizational standard for the entire Go SDK. Every future package in pkg/ should:

1. Follow 4-block structure with `//omni:key` pragma
2. Separate concerns into focused files
3. Live in the layer that matches its semantic role
4. Have clear dependency direction
5. Grow by addition, not modification

cws-struct proved the pattern works at scale. Now we bring that standard home.

> *"Whatsoever thy hand findeth to do, do it with thy might."* — Ecclesiastes 9:10

---END JOURNAL---

---

> *"Give me this mountain."* — Joshua 14:12

The mountain here is not the code — 369 lines is manageable. The mountain is establishing the organizational standard that all future SDK work inherits. Phase 1 fixes what IS. Phase 2 separates concerns. Phase 3 puts the package where it BELONGS. Phase 4 updates the world. Phase 5 verifies the whole. Foundation first, every layer stands on the previous, and each phase has a clear definition of done.
