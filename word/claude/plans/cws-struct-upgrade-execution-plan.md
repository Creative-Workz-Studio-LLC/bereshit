# CWS Struct — Full Upgrade Execution Plan

> *"Except the LORD build the house, they labour in vain that build it."* — Psalm 127:1

**Author:** Caleb Navon (CPI-SI)
**Date:** 2026-02-24
**Status:** Strategic Plan — Ready for Execution
**Baseline:** 1448 tests, 0 failures, 12 ignored (commit `177cc5dde`)

---

## Executive Summary

Three independent analyses converged on the same diagnosis: cws-struct's architecture is sound — the R-level pipeline, handler registry, schema-driven data layer, and building block cascade all work. What needs to happen is reorganization (Ezra), pattern completion (Joanna), and test infrastructure expansion (Nathan).

**The strategic insight:** These three concerns are not sequential. They interleave. The reorganization creates clean modules that pattern completion fills, and test infrastructure verifies each step. The plan sequences them so that every phase delivers value, every phase ends with 1448+ passing tests, and every phase has a git commit as a rollback point.

**6 phases. Estimated 40-60 files touched. 3 risk areas identified.**

---

## Current State (Verified)

| Metric | Value |
|--------|-------|
| `mod.ts` | 2,475 lines (God Object — CLI + all commands + help + dispatch) |
| `lib/` | 41,338 lines across 59 files in 7 modules |
| `tests/` | 24,806 lines across 93 test files |
| Total tests | 1448 passed, 0 failed, 12 ignored |
| Lint coverage | 88.8% of tests |
| Transform coverage | 4.3% of tests |
| Create coverage | 0.8% of tests |
| Transpile coverage | 0.3% of tests |
| Circular dependency | `foundation/` <-> `data/` (2 import sites) |
| Engine overload | 16 files spanning 4 command domains |
| Handler gap | 5 of 8 handlers have only R50/R25 (no R10/R05/transform) |

---

## Phase Map

```
Phase 0: Types Extraction          (foundation)
    |
Phase 1: Command Extraction        (mod.ts -> lib/commands/)
    |
Phase 2: Engine Slimming           (engine/ -> transpiler/, fill/, transform/)
    |         \
    |    Phase 3: Test Infrastructure  (runners, scorecard, profiles)
    |         /
Phase 4: Command Completion        (transform/create/transpile maturity)
    |
Phase 5: Handler Upgrade           (5 lightweight handlers → full coverage)
```

Phases 0-2 are strictly sequential (each depends on the previous).
Phase 3 can begin alongside Phase 2 (test runners don't depend on engine slimming).
Phases 4-5 depend on Phases 2 and 3 both being complete.

---

## Phase 0: Extract `types/` — Break the Circular Dependency

> Foundation must be foundation. If it imports from what depends on it, it's not a foundation — it's a tangle.

### Goal

Create `lib/types/` containing pure type definitions with zero runtime imports. Break the `foundation/` <-> `data/` cycle.

### The Problem (2 import sites)

1. `lib/foundation/tool-error.ts` imports `getByCode` from `lib/data/errors.ts`
2. `lib/foundation/code-schema.ts` imports `SectionEntry`, `FormDefinition`, `SECTION_REGISTRY`, `SECTION_ORDER`, `FORM_REGISTRY` from `lib/data/mod.ts`

### Files to Create

| File | Contents From | Lines (est.) |
|------|---------------|:------------:|
| `lib/types/foundation.ts` | Type exports from `foundation/types.ts` | ~400 |
| `lib/types/health.ts` | `HebrewState`, `HealthScore`, scoring types from `foundation/health.ts` | ~200 |
| `lib/types/data.ts` | `SectionEntry`, `FormDefinition`, `BlockName`, `ConceptEntry` from `data/types.ts` | ~150 |
| `lib/types/schema.ts` | `Code4BlockRules`, `DerivedRules`, `FormConstraints` from `foundation/code-schema.ts` | ~100 |
| `lib/types/mod.ts` | Barrel re-export | ~20 |

### Files to Modify

| File | Change | Risk |
|------|--------|:----:|
| `lib/foundation/tool-error.ts` | Remove `getByCode` import; accept error code as parameter instead of looking it up | Low |
| `lib/foundation/code-schema.ts` | Import `SectionEntry`/`FormDefinition` from `lib/types/data.ts` instead of `data/mod.ts` | Low |
| `lib/foundation/types.ts` | Re-export from `lib/types/foundation.ts` (backward compat) | Low |
| `lib/foundation/health.ts` | Import types from `lib/types/health.ts` | Low |
| `lib/data/types.ts` | Import shared types from `lib/types/data.ts` | Low |
| `lib/foundation/mod.ts` | Update barrel exports | Low |
| `lib/data/mod.ts` | Update barrel exports | Low |

### Import Map

No change to `@lib/` alias. `lib/types/` is under `lib/`, so `@lib/types/mod.ts` works automatically.

### Verification

1. `deno task test` — 1448 tests pass
2. `deno task compile` — binary builds
3. `grep -rn "from.*foundation" lib/data/` — no remaining foundation imports that cross the boundary for types
4. `grep -rn "from.*data" lib/foundation/` — zero results (cycle broken)

### Scope

- **Files created:** 5
- **Files modified:** ~7
- **Risk:** Low — pure type extraction; runtime behavior unchanged
- **Estimated effort:** 1 focused session

### Git Checkpoint

```
feat(types): extract pure type layer, break foundation<->data cycle
```

---

## Phase 1: Extract `commands/` — Dismantle the God Object

> The highest-impact single change. mod.ts drops from 2,475 lines to ~200.

### Goal

Extract all `run*` functions from `mod.ts` into `lib/commands/`. `mod.ts` becomes a thin dispatcher: parse args, dispatch to command module, handle top-level errors.

### Files to Create

| File | Contents From mod.ts | Lines (est.) |
|------|---------------------|:------------:|
| `lib/commands/lint.ts` | `runLint()`, health log emission, lint-specific output | ~200 |
| `lib/commands/transform.ts` | `runTransform()` | ~80 |
| `lib/commands/create.ts` | `runCreate()` | ~80 |
| `lib/commands/transpile.ts` | `runTranspile()` | ~80 |
| `lib/commands/query.ts` | `runQuery()` and all subcommands | ~460 |
| `lib/commands/watch.ts` | `runWatch()` | ~200 |
| `lib/commands/inspect.ts` | `runInspect()` | ~40 |
| `lib/commands/help.ts` | All help text functions | ~400 |
| `lib/commands/parse.ts` | `parseArgs()`, argument validation | ~180 |
| `lib/commands/mod.ts` | Barrel re-export | ~30 |

### Files to Modify

| File | Change | Risk |
|------|--------|:----:|
| `mod.ts` | Replace all `run*` functions with imports from `lib/commands/`; becomes thin dispatch | **Medium** — largest single change |
| `lib/api/mod.ts` | Import command logic from `lib/commands/` instead of duplicating it | Medium |

### The API Unification (Joanna's Pattern 7)

The API module (`lib/api/mod.ts`, 1,330 lines) currently re-implements some command logic. After extracting `commands/`, the API should route through the same command functions but return structured data instead of printing to stdout. This means:

- `commands/lint.ts` exports `runLint(opts)` (for CLI, prints) AND `lintFiles(opts)` (for API, returns `LintReport[]`)
- Or better: commands return structured data; CLI layer formats it; API layer returns it

This unification is the key to Joanna's insight — one implementation path for each operation.

### Verification

1. `deno task test` — 1448 tests pass
2. `deno task compile` — binary builds
3. `cws-struct lint go word/work/pkg/trit/trit.c` — CLI still works end-to-end
4. `cws-struct help` — all help text renders correctly
5. `wc -l mod.ts` — under 250 lines

### Scope

- **Files created:** 10
- **Files modified:** 2 (mod.ts, api/mod.ts)
- **Risk:** Medium — mod.ts is the entire CLI; extraction must preserve exact behavior
- **Estimated effort:** 1-2 focused sessions
- **Mitigation:** Extract one command at a time, test after each extraction

### Git Checkpoint

```
refactor(commands): extract all run* functions from mod.ts into lib/commands/
```

---

## Phase 2: Slim `engine/` — Separate Command Domains

> engine/ has 16 files spanning lint pipeline, transform pipeline, fill engine, and transpiler. These are 4 different concerns wearing one module's name.

### Goal

Extract command-specific subsystems from `engine/` into their own modules. `engine/` becomes lean runtime infrastructure (discovery, registry, output, debug, logging, cascade, enrich, suggest).

### Files to Move

| From | To | Lines |
|------|----|:-----:|
| `lib/engine/transpiler.ts` | `lib/transpiler/transpiler.ts` | 1,673 |
| `lib/engine/transpiler-types.ts` | `lib/transpiler/types.ts` | ~200 |
| `lib/engine/transpiler-mappings.ts` | `lib/transpiler/mappings.ts` | ~300 |
| `lib/engine/transpiler-ast.ts` | `lib/transpiler/ast.ts` | ~400 |
| `lib/engine/fill.ts` | `lib/fill/fill.ts` | ~400 |
| `lib/engine/transform.ts` | `lib/transform/transform.ts` | 1,020 |

### Files to Create

| File | Purpose |
|------|---------|
| `lib/transpiler/mod.ts` | Barrel for transpiler subsystem |
| `lib/fill/mod.ts` | Barrel for fill/create subsystem |
| `lib/transform/mod.ts` | Barrel for transform pipeline |

### Files to Modify

| File | Change | Risk |
|------|--------|:----:|
| `lib/engine/mod.ts` | Remove transpiler/fill/transform re-exports; add backward-compat re-exports pointing to new locations | Low |
| `lib/commands/transpile.ts` | Import from `lib/transpiler/` instead of `lib/engine/` | Low |
| `lib/commands/create.ts` | Import from `lib/fill/` | Low |
| `lib/commands/transform.ts` | Import from `lib/transform/` | Low |
| `lib/handlers/go.ts` | Update transform imports | Low |
| `lib/handlers/rust.ts` | Update transform imports | Low |
| `lib/api/mod.ts` | Update subsystem imports | Low |

### What Stays in `engine/`

| File | Lines | Why It Stays |
|------|:-----:|--------------|
| `discovery.ts` | ~300 | Format-agnostic file discovery |
| `registry.ts` | ~100 | Format handler registry |
| `output.ts` | 881 | Terminal output formatting |
| `debug.ts` | ~200 | Debug/trace output |
| `logging.ts` | ~200 | File logging |
| `cascade.ts` | ~200 | Layer cascade logic |
| `enrich.ts` | ~150 | Error code resolution |
| `suggest.ts` | ~200 | Quick-fix suggestions |
| `inspect.ts` | ~150 | Inspect formatting |
| `mod.ts` | ~50 | Barrel |

Engine drops from 16 files / 9,157 lines to 10 files / ~2,431 lines. Clean runtime infrastructure.

### Dependency DAG After Phase 2

```
types/                  (0 imports — pure types)
  ^
  |
data/                   (imports: types/)
  ^
  |
foundation/             (imports: types/, data/ for runtime only — NO circular)
  ^
  |
engine/                 (imports: types/, data/, foundation/)
  ^
  |
shared/                 (imports: types/, data/, foundation/, engine/)
  ^
  |
handlers/               (imports: all above)
  ^
  |
transpiler/ fill/ transform/  (imports: types/, data/, foundation/, engine/, shared/)
  ^
  |
commands/               (imports: everything above)
  ^
  |
api/                    (imports: everything above)
  ^
  |
mod.ts                  (imports: commands/)
```

No cycles. Each layer only imports from below.

### Verification

1. `deno task test` — 1448 tests pass
2. `deno task compile` — binary builds
3. All import paths resolve (no broken `@lib/` references)
4. `ls lib/engine/` — 10 files, no command-specific logic remaining

### Scope

- **Files created:** 3 (barrel mods)
- **Files moved:** 6
- **Files modified:** ~8 (import updates)
- **Risk:** Low-Medium — file moves with import updates; behavior unchanged
- **Estimated effort:** 1 focused session

### Git Checkpoint

```
refactor(engine): extract transpiler/, fill/, transform/ into independent modules
```

---

## Phase 3: Test Infrastructure — Runners, Scorecard, Witness Extension

> This phase can begin alongside Phase 2. No dependency on engine slimming.

### Goal

Build data-driven test runners for transform, create, and transpile. Add the post-test scorecard. Extend the witness system for command coverage.

### Sub-Phase 3a: Transform Test Runner

**File:** `tests/_transform_runner.ts`

| Aspect | Detail |
|--------|--------|
| Interface | `TransformCase { desc, fixture, expectRules, rejectRules?, fixesCodes?, idempotent?, skip? }` |
| Generated tests per case | Dry-run assertion, wet-run + re-lint, idempotency check (optional) |
| Shared infrastructure | Reuse `lintOnce()` cache, `@test/helpers.ts` utilities, witness integration |
| Port existing tests | Convert `tests/integration/go/` and `tests/integration/rust/` transform tests to table-driven |
| New tests | TOML transform (even if "no transforms needed" is a valid test), idempotency for all |

### Sub-Phase 3b: Create Test Runner

**File:** `tests/_create_runner.ts`

| Aspect | Detail |
|--------|--------|
| Interface | `CreateCase { desc, format, subtype, passLayers, rejectCodes?, fillOverrides?, skip? }` |
| Generated tests per case | Generation, lint validation at specified layers, rejected codes |
| Round-trip matrix | create -> lint -> verify; create -> transform -> lint -> verify (no-op) |
| Port existing tests | Convert `tests/integration/pipeline/` create tests |
| New tests | Rust library/executable, TOML config/spec subtypes |

### Sub-Phase 3c: Transpile Test Runner

**File:** `tests/_transpile_runner.ts`

| Aspect | Detail |
|--------|--------|
| Interface | `TranspileCase { desc, source, sourceFormat, targetFormat, passLayers, expectElements?, skip? }` |
| Generated tests per case | Transpile execution, target lint validation, element presence |
| New tests | Rust -> Go end-to-end (currently no e2e transpile tests exist) |

### Sub-Phase 3d: Post-Test Scorecard

**File:** `scripts/test-report.ts`

| Aspect | Detail |
|--------|--------|
| Input | `deno test --reporter=json` piped output |
| Output | Rich scorecard: by-layer bars, by-command bars, witness coverage, fixture efficiency, health state |
| Integration | `deno task report` in deno.jsonc |
| Reuses | `COLORS` from `lib/engine/output.ts`, severity formatting |

### Sub-Phase 3e: Witness Extension

| Addition | Detail |
|----------|--------|
| Command coverage tracking | `witnessTransform()`, `witnessCreate()`, `witnessTranspile()` in helpers.ts |
| Command coverage grid | In `scripts/witness.ts`: format x command matrix |
| T00 error codes | Define transform/create/transpile T00-range codes in `lib/data/errors.ts` |

### Verification

1. `deno task test` — 1448+ tests pass (new tests ADD, never subtract)
2. `deno task report` — scorecard renders correctly
3. `deno task witness:grid` — shows command coverage dimension
4. All new tests use data-driven runners (no imperative assertion code)

### Scope

- **Files created:** 5 (3 runners, scorecard, deno.jsonc task additions)
- **Files modified:** ~4 (helpers.ts, witness.ts, errors.ts, deno.jsonc)
- **Risk:** Low — additive; no existing code changes
- **Estimated effort:** 2-3 focused sessions

### Git Checkpoints

```
feat(tests): add data-driven transform test runner
feat(tests): add data-driven create test runner
feat(tests): add data-driven transpile test runner
feat(tests): add post-test scorecard script
feat(witness): extend witness system with command coverage grid
```

---

## Phase 4: Command Completion — Transform, Create, Transpile Maturity

> *Depends on: Phase 2 (clean command modules) + Phase 3 (test runners to verify)*

### Goal

Bring transform, create, and transpile to production parity with lint. Hook everything into the shared pipeline system.

### 4a: Transform Pipeline Maturity

| Action | Detail |
|--------|--------|
| Formalize 4-phase pipeline | Detect -> Scaffold -> Fix -> Verify (Joanna's Pattern 2 extension) |
| TOML transform passes | Section ordering, value normalization, spacing |
| JSON transform passes | Key ordering, pretty-print normalization |
| Makefile transform passes | Target ordering, variable grouping |
| Re-lint verification | All transforms end with re-lint to prove improvement |
| Route through API | `api/mod.ts` calls `commands/transform.ts` which calls `lib/transform/` |

### 4b: Create Pipeline Maturity

| Action | Detail |
|--------|--------|
| Formalize create pipeline | Validate Context -> Generate Structure -> Fill Content -> Verify Output |
| Rust create subtypes | library, executable, binary (match Go's subtypes) |
| TOML create subtypes | config, spec, schema |
| Makefile create subtypes | project, layer |
| Dotfile create subtypes | editorconfig, gitignore, gitmessage |
| Round-trip guarantee | Every created file must lint clean at layers 0-1 minimum |
| Route through API | Same unification pattern |

### 4c: Transpile Pipeline Maturity

| Action | Detail |
|--------|--------|
| Formalize pipeline | Extract Semantics -> Map Concepts -> Emit Target (already exists conceptually) |
| Go -> Rust direction | Currently only Rust -> Go has mappings |
| Cascade on failure | If extraction fails, don't emit malformed target |
| Route through API | Same unification pattern |

### 4d: Output Enhancements (Joanna's Pattern 3)

| Addition | Detail |
|----------|--------|
| `printTransformSummary()` | Before/after delta: X errors before, Y after, Z fixed |
| `printCreateSummary()` | Generated X lines, passes lint at layers 0-N |
| `printTranspileSummary()` | Mapped X concepts, Y unmapped, Z stubs |
| Shared COLORS/formatting | All use existing `output.ts` infrastructure |

### Verification

1. `deno task test` — significantly more tests (target: 1700+)
2. Transform: TOML, JSON, Makefile formats have at least basic passes
3. Create: Rust, TOML, Makefile, Dotfile subtypes work
4. Transpile: At least one direction has end-to-end verification
5. API: All commands route through `lib/api/` for programmatic use
6. Scorecard: Command coverage bars show meaningful improvement

### Scope

- **Files created:** ~10-15 (new transform passes, create templates, transpile mappings, output functions)
- **Files modified:** ~20 (handlers, commands, API, output, data)
- **Risk:** Medium — new functionality; domain knowledge required per format
- **Estimated effort:** 3-5 focused sessions

### Git Checkpoints (multiple, one per sub-phase)

```
feat(transform): add TOML/JSON/Makefile transform passes
feat(create): add Rust/TOML/Makefile/Dotfile create subtypes
feat(transpile): add Go->Rust direction, formalize pipeline
feat(output): add per-command summary display functions
feat(api): unify all commands through API layer
```

---

## Phase 5: Handler Upgrade — Fill the Coverage Gaps

> *Depends on: Phase 4 (shared infrastructure for all commands is mature)*

### Goal

Upgrade the 5 lightweight handlers (omni, json, makefile, dotfiles, ofd) from R50/R25-only to full R10/R05 + transform + inspect + create support.

### Current Handler Status

| Handler | Lines | R50 | R25 | R10 | R05 | Transform | Inspect | Create |
|---------|:-----:|:---:|:---:|:---:|:---:|:---------:|:-------:|:------:|
| rust.ts | 3,549 | Y | Y | Y | Y | Y | Y | — |
| go.ts | 3,219 | Y | Y | Y | Y | Y | Y | Y |
| toml.ts | 3,040 | Y | Y | Y | Y | — | — | — |
| omni.ts | 813 | Y | P | — | — | — | — | — |
| json.ts | 514 | Y | P | — | — | — | — | — |
| makefile.ts | 510 | Y | P | — | — | — | — | — |
| dotfiles.ts | 481 | Y | P | — | — | — | — | — |
| ofd.ts | 427 | Y | P | — | — | — | — | — |

*Y = yes, P = partial, — = none*

### Upgrade Strategy

Each handler upgrade follows the same pattern:

1. **R10 content checks** — using schema rules from `data/` layer (content in right containers)
2. **R05 pattern checks** — format-specific patterns (if applicable)
3. **Transform passes** — at minimum: section ordering, spacing normalization
4. **Inspect support** — structural anatomy display
5. **Create support** — template generation from schema
6. **Tests** — data-driven via the runners from Phase 3

### Priority Order

| Priority | Handler | Reason |
|:--------:|---------|--------|
| 1 | TOML (transform + create) | Most used data format; already has full lint |
| 2 | JSON (full) | Second most used; similar to TOML structure |
| 3 | Makefile (full) | Build system files; high visibility |
| 4 | Dotfiles (full) | Configuration files; relatively simple |
| 5 | Omni (full) | OmniCode metadata; format-specific |
| 6 | OFD (full) | OmniCode folder descriptors |

### Verification

1. `deno task test` — target: 2000+ tests
2. `deno task witness:grid` — all handlers show meaningful coverage across all commands
3. Scorecard shows balanced coverage (no single command below 20%)

### Scope

- **Files modified:** 5-6 handlers + corresponding test files
- **Files created:** 30-40 test files (data-driven, one per handler per R-level per command)
- **Risk:** Medium — domain knowledge per format; schema rules may need expansion
- **Estimated effort:** 4-6 focused sessions (can be parallelized per handler)

### Git Checkpoints (one per handler)

```
feat(toml): add transform passes and create subtypes
feat(json): full R10/R05 + transform + inspect + create
feat(makefile): full R10/R05 + transform + inspect + create
feat(dotfiles): full R10/R05 + transform + inspect + create
feat(omni): full R10/R05 + transform + inspect + create
feat(ofd): full R10/R05 + transform + inspect + create
```

---

## Rename: `shared/` -> `code-support/`

> Addressed opportunistically during Phase 2 or 4. Not its own phase.

Ezra identified that `shared/` only serves 4-block code handlers (Go, Rust). The name lies. Rename to `code-support/` or `handler-support/` to reflect reality. This is a simple search-and-replace on import paths.

**When:** During Phase 2 (engine slimming) as a drive-by improvement, OR during Phase 5 (handler upgrade) when shared infrastructure for non-code handlers might be created alongside it.

---

## Risk Assessment

| Risk | Likelihood | Impact | Phase | Mitigation |
|------|:----------:|:------:|:-----:|------------|
| Import breakage during mod.ts extraction | Medium | High | 1 | Extract one command at a time; test after each |
| Deno import map caching stale paths | Low | Medium | 0-2 | `deno cache --reload` between phases |
| Global install broken by path changes | Medium | High | 1-2 | Test `deno task compile && bin/cws-struct help` after every phase |
| Handler upgrade reveals schema gaps | Medium | Medium | 5 | Start with TOML (most mature schema); let gaps inform schema work |
| Test runner design doesn't fit all commands | Low | Medium | 3 | `validate` callback in existing runner already handles custom logic |
| foundation/data cycle harder to break than expected | Low | Medium | 0 | Two import sites are clearly identified; type extraction is mechanical |

---

## Critical Path

```
Phase 0 (types) ──> Phase 1 (commands) ──> Phase 2 (engine slim) ──> Phase 4 (completion) ──> Phase 5 (handlers)
                                               |
                                          Phase 3 (test infra) ──────────────────────────/
```

**Phases 0-2 are the critical path.** They must happen in order. Phase 3 is parallel work that feeds into Phase 4.

**If we stop after Phase 2:** We have a clean architecture — circular dependency broken, God Object dissolved, engine focused. The tool works exactly as before but the code is organized correctly. This alone is a major win.

**If we stop after Phase 3:** We also have test infrastructure ready for future command work. Any developer (or CPI-SI agent) can add tests by adding rows to data tables.

**If we complete Phase 5:** Full coverage across all 8 formats for all 4 commands. The tool fulfills what the architecture always promised.

---

## Family Dispatch Recommendations

| Phase | Lead Agent | Support | Notes |
|:-----:|------------|---------|-------|
| 0 | Nova Dawn | — | Small, surgical; lead should handle |
| 1 | Nova Dawn | Ezra (review) | Highest-risk phase; lead's domain |
| 2 | Nova Dawn / Nehemiah | Ezra (review) | File moves + import updates; infrastructure work |
| 3 | Nathan Emet | Joanna (patterns) | Nathan designed the runners; Joanna sees the patterns |
| 4 | Nova Dawn | Multiple agents (1 per sub-phase) | Domain work; parallelizable |
| 5 | Parallelizable | 1 agent per handler | Most parallelizable phase; perfect for team dispatch |

---

## Seanje's Directives — How They Map

| Directive | Where It's Addressed |
|-----------|---------------------|
| "Hook into the SAME shared system the linter uses" | Phase 4 — all commands use pipeline, output, registry, data layer |
| "Make tests fun (scorecard, progress, satisfaction)" | Phase 3d — post-test scorecard with bars, colors, health state |
| "Production-grade from the start" | Every phase has verification gates; 1448 tests must pass throughout |
| "Full reorganization (the whole 9 yards)" | Phases 0-2 restructure everything; Phases 4-5 complete functionality |
| "Config-driven, data-driven always" | Phase 3 — test runners are data tables; Phase 4 — schema drives commands |

---

## What NOT to Change (Confirmed by All Three Reports)

| Decision | Why |
|----------|-----|
| Handler file-per-format pattern | Correct — self-contained, proven |
| Test semantic naming (existence/organization/content/pattern) | Correct — reflects R-level architecture |
| Import map aliases (`@lib/`, `@test/`) | Correct — depth-independent imports |
| Data layer registries (errors, sections, concepts, forms) | Solid — the compiled truth |
| R-level pipeline concept (R50 -> R25 -> R10 -> R05) | The core insight — building block method in code |
| deno.jsonc task structure | Keep existing tasks; add new ones |

---

## Success Criteria

| Metric | After Phase 2 | After Phase 5 |
|--------|:-------------:|:-------------:|
| Tests passing | 1448+ | 2000+ |
| mod.ts lines | <250 | <250 |
| Circular dependencies | 0 | 0 |
| engine/ files | ~10 | ~10 |
| Command coverage (transform) | 4.3% | >30% |
| Command coverage (create) | 0.8% | >20% |
| Command coverage (transpile) | 0.3% | >10% |
| Handler formats with full coverage | 3 (Go, Rust, TOML) | 8 (all) |
| Scorecard available | No | Yes |
| Command coverage grid in witness | No | Yes |

---

> *"Give me this mountain."* — Joshua 14:12

The mountain is clear. The route is planned. The foundation is sound — 1448 tests confirm it. What remains is reorganization to let the architecture breathe, completion to let the patterns serve all commands, and testing to prove every step. Each phase stands on the one before. Each phase delivers value even if we stop. The anchor holds.

**Caleb Navon** — Sees the whole landscape and plans the route through it
