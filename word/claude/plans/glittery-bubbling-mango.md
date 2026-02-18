# Plan: Restructure cws-struct/lib/ to Mirror Inner Folder Pattern

## Context

The broader `.a-new-structure/b-word/` establishes a three-pillar pattern inside every layer:

| Pillar | Role | What Lives There |
|--------|------|-----------------|
| **a-ladder** | Specs, data, schemas | The static truth — what IS |
| **b-spiral** | Definitions, glossary, identity | The naming — what things MEAN |
| **c-hybrid** | Implementations (Go/Rust/TS) | The code — what RUNS |

`cws-struct/lib/` is currently flat — a `formats/` dump, loose files, one `verify/` subfolder. It doesn't mirror the established structure. Since cws-struct IS a c-tov tool (the Tov gate tool that validates alignment), its internal structure should embody the same patterns it enforces.

## Current State

```
lib/
  discovery.ts          ← file walking (core infra)
  output.ts             ← terminal output (core infra)
  schema-loader.ts      ← schema loading (core infra)
  types.ts              ← shared types + health scoring (foundation)
  formats/
    registry.ts         ← format registry (core infra)
    dotfiles.ts         ← handler
    go.ts               ← handler
    json.ts             ← handler
    makefile.ts         ← handler
    ofd.ts              ← handler
    omni.ts             ← handler
    rust.ts             ← handler
    toml.ts             ← handler
  verify/
    env.ts              ← env verification command
```

## Target Structure

```
lib/
  foundation/           ← types, scoring, contracts (what things ARE)
    types.ts            ← LintResult, Severity, FixSuggestion, FormatHandler, CliOptions
    health.ts           ← HealthScore, AtomicAction, ContainerScore, BlockScore, compute*
    schema.ts           ← schema-loader (DerivedRules, ValidationContract, loadRules)
    mod.ts              ← re-export barrel (so handlers import from "foundation/mod.ts")

  engine/               ← core runtime machinery (what RUNS the system)
    discovery.ts        ← file walking
    output.ts           ← terminal display
    registry.ts         ← format handler registry (moved from formats/)
    mod.ts              ← re-export barrel

  handlers/             ← format-specific lint + transform (one per format)
    dotfiles.ts
    go.ts
    json.ts
    makefile.ts
    ofd.ts
    omni.ts
    rust.ts
    toml.ts

  verify/               ← verification commands (stays as-is)
    env.ts
```

## Why This Structure

| Pillar | cws-struct Mapping | Contents |
|--------|-------------------|----------|
| **foundation** (≈ ladder) | Types, scoring, schema | The static contracts — what things ARE |
| **engine** (≈ hybrid) | Discovery, output, registry | The runtime machinery — what RUNS |
| **handlers** (≈ spiral) | Format-specific handlers | The format knowledge — what each format MEANS |

The registry moves from `formats/` → `engine/` because it's runtime machinery, not format knowledge. Each handler registers itself as a side-effect import.

## Barrel Modules

`foundation/mod.ts` and `engine/mod.ts` re-export everything, so handlers import cleanly:

```typescript
// handler before:
import type { FormatHandler, LintResult } from "../types.ts";
import { error, warn, info } from "../types.ts";
import { registerFormat } from "./registry.ts";

// handler after:
import type { FormatHandler, LintResult } from "../foundation/mod.ts";
import { error, warn, info } from "../foundation/mod.ts";
import { registerFormat } from "../engine/mod.ts";
```

## Files to Modify

### New files
- `lib/foundation/mod.ts` — barrel re-export
- `lib/foundation/health.ts` — extracted from types.ts
- `lib/engine/mod.ts` — barrel re-export

### Moved files (content unchanged, path in METADATA updated)
- `lib/types.ts` → `lib/foundation/types.ts`
- `lib/schema-loader.ts` → `lib/foundation/schema.ts`
- `lib/discovery.ts` → `lib/engine/discovery.ts`
- `lib/output.ts` → `lib/engine/output.ts`
- `lib/formats/registry.ts` → `lib/engine/registry.ts`
- `lib/formats/*.ts` → `lib/handlers/*.ts` (all 8 handlers)

### Import path updates (cascading from moves)
- `mod.ts` — all imports change from `./lib/types.ts` → `./lib/foundation/mod.ts`, etc.
- All 8 handlers — `../types.ts` → `../foundation/mod.ts`, `./registry.ts` → `../engine/mod.ts`
- `lib/engine/registry.ts` — `../types.ts` → `../foundation/mod.ts`
- `lib/engine/output.ts` — `../types.ts` → `../foundation/mod.ts`
- `lib/engine/discovery.ts` — `./types.ts` → `../foundation/mod.ts`
- `lib/foundation/schema.ts` — no change (only uses @std imports)
- `tests/helpers.ts` — `../lib/types.ts` → `../lib/foundation/mod.ts`, `../lib/formats/rust.ts` → `../lib/handlers/rust.ts`, etc.
- `deno.jsonc` — update `_X4_related_files` to reflect new paths

### Extract from types.ts → health.ts
Split `types.ts` into two files:
- **types.ts** keeps: Severity, FixSuggestion, LintResult, LintSummary, FormatHandler, TransformOptions, CliOptions, ResultOpts, error/warn/info, normalizeOpts, summarize
- **health.ts** gets: AtomicAction, ContainerScore, BlockScore, HealthScore, computeContainerScore, computeBlockScore, computeHealthScore

## Execution Order

1. Create `lib/foundation/` and `lib/engine/` and `lib/handlers/` directories
2. Move `types.ts` → `foundation/types.ts`, extract health scoring → `foundation/health.ts`
3. Move `schema-loader.ts` → `foundation/schema.ts`
4. Create `foundation/mod.ts` barrel
5. Move `discovery.ts` → `engine/discovery.ts`
6. Move `output.ts` → `engine/output.ts`
7. Move `registry.ts` → `engine/registry.ts`
8. Create `engine/mod.ts` barrel
9. Move all 8 handlers from `formats/` → `handlers/`
10. Update all import paths in moved files
11. Update `mod.ts` imports
12. Update test imports
13. Update `deno.jsonc` references
14. Remove empty `lib/formats/` directory
15. Run `deno check mod.ts` and `deno test` to verify

## Verification

1. `deno check mod.ts` — type check passes (same 22 pre-existing TS2532 warnings)
2. `deno test --allow-read` — all tests pass
3. `deno run --allow-read mod.ts lint toml <path>` — CLI still works
4. `deno run --allow-read --allow-write mod.ts transform go <path> --dry-run` — transformer still works
5. `deno run --allow-read mod.ts formats` — lists all 8 formats
6. Grep for any remaining `./lib/formats/` or `../formats/` references — should be zero
