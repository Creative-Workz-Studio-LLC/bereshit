# Go SDK Restructuring — cws-struct Alignment

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost?"* — Luke 14:28

**Author:** Caleb Navon (plan-builder), reviewed by Nova Dawn
**Date:** 2026-02-24

---

## Context

cws-struct proved what organized code looks like at scale (1,208 tests, 8 formats, schema-driven pipeline). The Go SDK hookoutput package — the substrate-agnostic layer we just built for Gemini CLI support — scored **53/100 on our own linter**. Functions in SETUP. Types in BODY. No pragma. No end markers. A 370-line monolith buried in `util/pure/`.

Seanje's directive: "slowly update the SDK kit code and ordering itself to reflect cws-struct and have the same level of folder organization, level a bar."

This plan restructures hookoutput as the EXEMPLAR — setting the standard for all future Go SDK work.

---

## Current State

**File:** `pkg/util/pure/hookoutput/types.go` — 369 lines, single file, 6 concerns mixed together

| Problem | cws-struct Score |
|---------|:---:|
| Missing `//omni:key` pragma | CWS-R50-011 (Error) |
| Missing END block markers | CWS-R50-021 (Warning x4) |
| Functions in SETUP block (lines 59-78) | CWS-R10-002 (Warning x3) |
| Type declarations in BODY block (lines 128-340) | CWS-R10-002 (Warning x6) |
| No package doc comment | CWS-R05-001 (Warning) |
| Missing Pragma/Metadata identity vars | CWS-R25-080 (Warning x2) |

**8 consumer files** in `hooks/` import it. Zero consumers in `pkg/` itself.

---

## 5-Phase Plan

### Phase 1: Fix 4-Block Ordering In-Place (~20 min)

**File:** `pkg/util/pure/hookoutput/types.go` (edit in place)

Reorder the monolith to be 4-block compliant BEFORE splitting:

**SETUP order (correct):** Imports → Constants (Substrate iota, Event constants, Permission constants) → Variables (cached detection) → Types (all 6 struct types)

**BODY order (correct):** Helpers (MustJSON) → Core Operations (DetectSubstrate, IsGemini, IsClaude) → Public APIs (all New*Response constructors)

Add `//omni:key`, `//omni:code`, `//omni:version` pragmas. Add END block markers.

**Verify:** `go build ./pkg/...` + `go build ./hooks/...` + `cws-struct lint go` score improves from 53 → 80+

---

### Phase 2: Split Into 7 Focused Files (~40 min)

**Same directory** (`pkg/util/pure/hookoutput/`). No import changes.

| File | Concern | ~Lines | Dependencies |
|------|---------|:------:|-------------|
| `substrate.go` | Substrate detection (type, DetectSubstrate, IsGemini, IsClaude) | 50 | `os`, `sync` |
| `events.go` | Event name constants (Claude + Gemini) + permission/decision constants | 45 | None |
| `context.go` | ContextHookOutput, ContextResponse, 7 New*Response constructors | 75 | substrate, events |
| `pretool.go` | PreToolHookOutput, PreToolResponse, 5 NewPreTool* constructors | 80 | substrate, events |
| `stop.go` | StopResponse, NewStopAllow, NewStopBlock | 40 | substrate, events |
| `input.go` | HookInput struct (universal input parsing) | 40 | None |
| `json.go` | MustJSON helper | 25 | `encoding/json` |

Each file: own `//omni:key`, proper 4-block, END markers.

**Dependency graph:**
```
events.go (pure constants)    input.go (pure struct)    json.go (pure helper)
    ^           ^
    |           |
substrate.go (os, sync)
    ^       ^       ^
    |       |       |
context.go  pretool.go  stop.go
```

**Verify:** `go build ./pkg/...` + `go build ./hooks/...` (same import path, no consumer changes)

---

### Phase 3: Relocate to `pkg/sdk/hookoutput/` (~10 min)

```bash
mkdir -p pkg/sdk/hookoutput/
git mv pkg/util/pure/hookoutput/*.go pkg/sdk/hookoutput/
```

**Why `sdk/` not `foundation/`:** hookoutput defines contracts for EXTERNAL consumers (hooks module). `foundation/` is for internal pkg foundations. `sdk/` says "this is the API surface for external modules" — analogous to cws-struct's `lib/foundation/`.

**New pkg/ layer map:**
```
pkg/
  foundation/     — L0: internal foundations (types, result, schema, database)
  core/           — L1: domain logic (statemachine, coordinates, health)
  orchestration/  — L2: orchestration (cognition, config, loader, logging)
  sdk/            — SDK: contracts for external consumers
    hookoutput/   — Hook output schema, types, constructors
  util/           — Utilities: pure helpers, fs, term, vcs
```

**Verify:** `go build ./pkg/...` passes. `go build ./hooks/...` expected to FAIL (import path changed).

---

### Phase 4: Update 8 Import Paths in hooks/ (~15 min)

**Old:** `"cws.studio/pkg/util/pure/hookoutput"`
**New:** `"cws.studio/pkg/sdk/hookoutput"`

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

Package name stays `hookoutput`. All `hookoutput.NewXxx()` calls unchanged.

**Verify:** `go build ./hooks/...` + `go vet ./hooks/...` pass clean.

---

### Phase 5: Full Verification + Documentation (~15 min)

1. `go build ./...` (full workspace)
2. `go vet ./...` (full workspace)
3. `make all` in hooks/ (rebuild all 14 binaries)
4. Smoke test: `echo '{"session_id":"test","source":"startup"}' | ./bin/start`
5. `cws-struct lint go pkg/sdk/hookoutput/*.go` — each file scores 80+
6. Update `pkg/README.md` with `sdk/` layer description

---

## Broader SDK Vision (Future)

```
pkg/sdk/
  hookoutput/      — Hook output schema + constructors (THIS PLAN)
  hookinput/       — Hook input parsing (FUTURE: extract from hookoutput/input.go)
  hookregistry/    — Hook registration, event discovery (FUTURE)
  agentoutput/     — Agent dispatch contracts (FUTURE)
```

**SDK layer rule:** Contracts consumed by EXTERNAL modules. May import stdlib + foundation/. Must NOT import core/ or orchestration/. Grows by addition.

---

## Journal Entry

Write to `divisions/tech/cpi-si/knowledge-base/journals/universal/2026-02-24_sdk-restructuring-cws-struct-alignment.md` — preparing the family for the transition. Content covers: why we're restructuring, the 7 cws-struct principles mapped to Go, the 5 phases, and what each family member needs to know based on where they work.

---

## Risks

| Risk | Mitigation |
|------|-----------|
| Import path breakage across modules | Phase 3+4 run back-to-back. go.work resolves cross-module deps. |
| Split introduces ordering issue | Phase 1 fixes ordering FIRST. Phase 2 only splits. |
| No test safety net | hookoutput has no tests. Compilation IS the test (Go's type system). Smoke test in Phase 5. |
| `sdk/` feels premature with 1 package | Justified by principle + clear future residents (hookinput, agentoutput). |

---

## Estimated Effort

| Phase | Time |
|:-----:|:----:|
| 1. Fix ordering | 20 min |
| 2. Split files | 40 min |
| 3. Relocate | 10 min |
| 4. Update imports | 15 min |
| 5. Verify + docs | 15 min |
| **Total** | **~100 min** |

---

> *"Give me this mountain."* — Joshua 14:12

The mountain isn't 369 lines. It's establishing the organizational standard that all future SDK work inherits.
