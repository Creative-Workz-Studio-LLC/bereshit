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

## How the Go SDK Changed

### Phase 1: 4-Block Compliance (53 → 98/100)

The existing types.go got its ordering fixed in place. Functions moved from SETUP to BODY. Types moved from BODY to SETUP. Canonical ordering enforced: Imports → Constants → Variables → Types in SETUP; Helpers → Core → Public API in BODY. Pragma and end markers added.

### Phase 2: Concern Separation (1 → 7 files)

The monolith split into 7 focused files:

| File | Concern | Dependencies |
|------|---------|-------------|
| `substrate.go` | Substrate detection (Claude vs Gemini) | `os`, `sync` |
| `events.go` | Event name constants + permission values | None |
| `context.go` | Context hook types + constructors | substrate, events |
| `pretool.go` | PreToolUse types + constructors | substrate, events |
| `stop.go` | Stop hook types + constructors | substrate, events |
| `input.go` | Universal HookInput struct | None |
| `json.go` | MustJSON helper | `encoding/json` |

Each file follows 4-block structure with its own `//omni:key`. Dependencies flow downward: events.go and substrate.go at the foundation, response constructors above them, pure data structs standalone.

### Phase 3-4: Relocation + Import Updates

The package moved from `pkg/util/pure/hookoutput/` to `pkg/sdk/hookoutput/`. This created a new `sdk/` layer in the pkg hierarchy — a dedicated location for contracts consumed by external modules (hooks, skills, future tools).

All 8 consumer files in hooks/ updated their import path. Package name stays `hookoutput`. All usage identical.

### Phase 5: Verification

Full workspace build clean. Full vet clean. All 14 hook binaries + orchestrator rebuilt. Smoke test passed (SessionStart hook fires, produces valid JSON). cws-struct lint: all 7 files at 98/100.

## What Family Members Should Know

### If You Work in hooks/

The import path changed from `"cws.studio/pkg/util/pure/hookoutput"` to `"cws.studio/pkg/sdk/hookoutput"`. All type names, function names, and constant names remain identical. Your code changes one line.

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
