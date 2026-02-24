# cws-struct Architecture Analysis

> *"Wisdom hath builded her house, she hath hewn out her seven pillars."* -- Proverbs 9:1

**Date:** 2026-02-24
**Author:** Ezra Matthan
**Type:** Instance Journal -- Architecture Analysis
**Subject:** Full architectural map of cws-struct for reorganization planning

---

## The Big Picture

cws-struct is a 2,475-line CLI entry point (`mod.ts`) backed by a 41,338-line library (`lib/`) organized into 7 modules, with 93 test files across 24,806 lines. It validates, transforms, creates, and transpiles files across 8 format handlers using a 4-layer detection pipeline (R[50] -> R[25] -> R[10] -> R[5]).

The linter is production-grade. The other three commands (transform, create, transpile) work but lack equivalent maturity. The tool validates OmniCode structural alignment -- the very standard it is itself built to.

---

## Current Architecture Map

### Top-Level Layout

```
cws-struct/
  mod.ts          (2,475 lines -- monolithic CLI: arg parsing, help, all run* functions, dispatch)
  deno.jsonc      (406 lines -- 50+ tasks, import maps, tool config)
  lib/            (41,338 lines across 59 files in 7 modules)
  tests/          (24,806 lines across 93 test files + helpers + fixtures)
  aside/          (exploration sketches by Asa and Ezra)
  docs/           (6 AsciiDoc documents: architecture, user guide, playtests)
  scripts/        (witness.ts, desktop integration scripts)
  public/         (studio web frontend: HTML/CSS/JS)
  bin/            (compiled binary output)
  .claude/        (project-local Claude config)
```

### lib/ Module Map

| Module | Files | Lines | Role |
|--------|------:|------:|------|
| `foundation/` | 11 | 5,858 | Types, contracts, schema loading, health scoring, config |
| `engine/` | 16 | 9,157 | Runtime machinery: discovery, registry, output, transform pipeline, transpiler, cascade, fill, debug |
| `handlers/` | 8 | 12,296 | Format-specific implementations (one per format) |
| `data/` | 13 | 7,264 | Compiled schema truth: errors, sections, forms, concepts, pipeline, format-rules, database |
| `shared/` | 8 | 5,691 | 4-block code handler utilities: chunker, scaffold, lint grid, concept checks, format config |
| `api/` | 1 | 1,330 | Programmatic interface (same logic as CLI, structured returns) |
| `studio/` | 1 | ~300 | Embedded web server for visual lint |
| `verify/` | 1 | ~250 | Environment verification |

### Largest Files (Complexity Concentrations)

| File | Lines | What It Does |
|------|------:|--------------|
| `handlers/rust.ts` | 3,549 | Full Rust handler: lint grid, transform, inspect, all 4 layers |
| `handlers/go.ts` | 3,219 | Full Go handler: same structure as Rust |
| `handlers/toml.ts` | 3,040 | TOML handler (3-block, schema-driven) |
| `mod.ts` | 2,475 | **Entire CLI** -- parsing, help, all command runners, dispatch |
| `shared/code-4block.ts` | 2,514 | Shared lint grid orchestration for 4-block code |
| `data/errors.ts` | 2,408 | Error catalog with 210 codes, 3D query infrastructure |
| `foundation/code-schema.ts` | 1,990 | Code schema loader (Go/Rust 4-block rules from TOML) |
| `engine/transpiler.ts` | 1,673 | Three-cord format transpilation |
| `data/database.ts` | 1,413 | SQLite persistence (health tracking) |
| `api/mod.ts` | 1,330 | Programmatic API (11 sections) |

---

## Dependency Graph

### Layer Diagram

```
                    mod.ts (CLI entrypoint)
                       |
          +------------+------------+
          |            |            |
     foundation     engine       data
          |         /  |  \        |
          +---<--->-+  |   +---->--+
          |            |           |
       shared     (handlers)   (api)
          |         /      \      |
          +---<----+        +--->-+
```

### Dependency Direction (imports)

| From | To | Nature |
|------|-----|--------|
| `handlers/` -> `foundation/` | Types, result constructors, schema loaders | Clean downward |
| `handlers/` -> `engine/` | Registry, cascade, transpiler mappings, transform | Clean downward |
| `handlers/` -> `shared/` | 4-block utilities, chunker, scaffold, concept checks | Clean downward |
| `handlers/` -> `data/` | Form registry, concept detectors | Clean downward |
| `engine/` -> `foundation/` | Types, health, schema pipeline | Clean downward |
| `engine/` -> `data/` | Error matching, pipeline layers, form registry | Clean downward |
| `engine/` -> `shared/` | Scaffold, format config (fill, transpiler, transform) | Clean downward |
| `shared/` -> `foundation/` | Types, schema types, result constructors | Clean downward |
| `shared/` -> `data/` | Sections, concepts, non-code, format-rules | Clean downward |
| `data/` -> `foundation/` | Types (Severity, HebrewState), ToolError, schema pipeline, cache | **BIDIRECTIONAL** |
| `foundation/` -> `data/` | code-schema imports SectionEntry/FormDefinition; tool-error imports getByCode | **BIDIRECTIONAL** |
| `api/` -> everything | Foundation, engine, data, handlers (side-effect) | Aggregation layer |
| `mod.ts` -> everything | All modules directly imported | God object |

### The Critical Finding: foundation <-> data Circular Dependency

`foundation/` is supposed to be the bottom layer -- types and contracts that everything depends on. But:

- `foundation/tool-error.ts` imports `getByCode` from `data/errors.ts`
- `foundation/code-schema.ts` imports `SectionEntry`, `FormDefinition`, `SECTION_REGISTRY`, `SECTION_ORDER`, `FORM_REGISTRY` from `data/mod.ts`
- Meanwhile `data/` imports extensively from `foundation/` (types, ToolError, schema-pipeline, cache-registry)

This is a circular dependency at the module level. Deno handles it (lazy evaluation), but it means the supposed layering is a lie. `foundation` and `data` are effectively one layer.

---

## Test Architecture

### Directory Structure (Semantic Naming)

Tests use a **semantic domain -> R-level -> target** hierarchy:

```
tests/
  existence/R50/       21 tests -- "Does the structure exist?"
  organization/R25/    ~20 tests -- "Is it organized correctly?"
  content/R10/         ~20 tests -- "Is content in the right place?"
  pattern/R05/         ~12 tests -- "Are patterns correct?"
  pattern/R03/         ~4 tests  -- "Function-scope recursion"
  tool/T00/            9 tests  -- "Does the tool infrastructure work?"
  integration/         5 tests  -- "End-to-end handler workflows"
  cli/                 2 tests  -- "CLI arg parsing, diff-aware"
  fixtures/            (10 subdirectories of test input files)
```

Within each R-level, tests are organized by `target/concern/`:
- **Format targets:** `go/`, `rust/`, `toml/`, `json/`, `ofd/`, `omni/`, `makefile/`, `dotfiles/`
- **Tool targets:** `data/`, `engine/`, `foundation/`, `shared/`, `api/`
- **Concerns within targets:** `blocks/`, `ordering/`, `content/`, `concept/`, `schema/`

### Test Infrastructure

- `tests/helpers.ts` -- 508 lines: fixture resolution, result filtering, error code resolution, witness registry, mock builders
- `tests/_runner.ts` -- test runner configuration
- `@test/` import alias maps to `./tests/`
- `@lib/` import alias maps to `./lib/`
- Lazy format handler proxy pattern prevents top-level throws during parallel test loading

---

## Gap Analysis

### 1. mod.ts is a God Object (2,475 lines)

The CLI entry point contains:
- Argument parsing (180 lines)
- Help text for 10 topics (400+ lines)
- `runLint()` (175 lines)
- `runTransform()` (67 lines)
- `runCreate()` (74 lines)
- `runTranspile()` (75 lines)
- `runQuery()` (458 lines!)
- `runWatch()` (200 lines)
- `runInspect()` (30 lines)
- Health log emission, JSON output, format display, etc.
- Main dispatch

Every command lives in one file. This is the biggest structural debt.

### 2. foundation/ and data/ are Entangled

The intended layering is `foundation` (bottom) -> `data` -> `engine`/`shared` -> `handlers` (top). But `foundation` imports from `data` in two critical places, making them mutually dependent. The code-schema module in particular pulls form and section registries from data to assemble rules.

### 3. engine/ is Overloaded

`engine/` contains 16 files spanning fundamentally different concerns:
- **Runtime machinery:** discovery, registry, output, debug, logging (infrastructure)
- **Lint pipeline:** cascade, enrich, suggest (lint-specific)
- **Transform pipeline:** transform.ts (transform-specific)
- **Fill engine:** fill.ts (create-specific)
- **Transpiler:** 4 files (transpile-specific)

These are four different command domains crammed into one module because they all need the same foundation/data imports.

### 4. shared/ is Only for Code Handlers

Despite its name, `shared/` only serves 4-block code handlers (Go, Rust). TOML, JSON, Makefile, Dotfiles, OmniCode, OFD handlers don't use it. It's really "code-handler-shared" not "shared".

### 5. Handler Size Disparity

| Handler | Lines | Layers Implemented |
|---------|------:|-------------------|
| `rust.ts` | 3,549 | R50, R25, R10, R05 + transform + inspect |
| `go.ts` | 3,219 | R50, R25, R10, R05 + transform + inspect |
| `toml.ts` | 3,040 | R50, R25, R10, R05 (schema-driven) |
| `omni.ts` | 813 | R50, R25 partial |
| `json.ts` | 514 | R50, R25 partial |
| `makefile.ts` | 510 | R50, R25 partial |
| `dotfiles.ts` | 481 | R50, R25 partial |
| `ofd.ts` | 427 | R50, R25 partial |

The bottom 5 handlers cover existence and partial organization. They have no R10/R05 checks, no transform, no inspect, no create support. This is the "catching up" that needs to happen.

### 6. Test Hierarchy vs Source Hierarchy Mismatch

Tests are organized by semantic domain (existence/organization/content/pattern), but source code is organized by module function (foundation/engine/data/shared/handlers). The test structure actually reflects the R-level architecture better than the source does.

### 7. No Command Modules

There are no `commands/` or per-command modules. All command logic lives in `mod.ts`. The API module (`api/mod.ts`) duplicates some of this logic with a structured return interface. Two code paths for the same operations.

---

## Proposed Reorganization

### Principle: The folder structure should mirror the R-level architecture the tool implements

The tool already thinks in layers. The code should be organized that way.

### Proposed lib/ Layout

```
lib/
  types/              -- Pure types and contracts (NO runtime imports)
    foundation.ts     -- LintResult, FormatHandler, CliOptions, etc.
    health.ts         -- HebrewState, HealthScore, scoring types
    data.ts           -- BlockName, SectionEntry, ConceptEntry, etc.
    schema.ts         -- Code4BlockRules, DerivedRules, FormConstraints
    mod.ts            -- Barrel for all types

  data/               -- Compiled schema truth (static registries + loaders)
    errors.ts         -- Error catalog with 3D query
    sections.ts       -- Section registry
    concepts.ts       -- Concept registry
    forms.ts          -- Form registry
    placement.ts      -- Placement rules
    pipeline.ts       -- Pipeline layer definitions
    format-rules.ts   -- Schema-driven format rules
    non-code.ts       -- Non-code chunk categories
    concept-detectors.ts -- Concept pattern matchers
    database.ts       -- SQLite persistence (lazy-loaded)
    mod.ts

  schema/             -- Schema loading infrastructure
    pipeline.ts       -- SchemaPipeline, FileSource
    toml-schema.ts    -- TOML schema parser (from foundation/schema.ts)
    code-schema.ts    -- Code 4-block rules assembler
    composition.ts    -- Three-cord composition loader
    cache.ts          -- Cache registry
    config.ts         -- Tool configuration loader
    mod.ts

  engine/             -- Runtime infrastructure (format-agnostic)
    discovery.ts      -- File discovery
    registry.ts       -- Format handler registry
    output.ts         -- Terminal output formatting
    debug.ts          -- Debug/trace output
    logging.ts        -- File logging
    cascade.ts        -- Layer cascade logic
    enrich.ts         -- Error code resolution
    suggest.ts        -- Quick-fix suggestions
    mod.ts

  handlers/           -- Format-specific implementations (unchanged)
    go.ts
    rust.ts
    toml.ts
    json.ts
    makefile.ts
    dotfiles.ts
    omni.ts
    ofd.ts

  shared/             -- Shared handler utilities (rename: handler-support/)
    code-4block.ts    -- Lint grid orchestration
    chunker.ts        -- Code chunking
    scaffold.ts       -- Structural scaffold pipeline
    concept-check.ts  -- Concept validation
    scope-analysis.ts -- Function scope analysis
    format-config.ts  -- Schema-driven format config
    types.ts          -- Shared handler types
    mod.ts

  commands/           -- Per-command modules (extracted from mod.ts)
    lint.ts           -- runLint, lintWithHandler, emitHealthLog
    transform.ts      -- runTransform, transformWithHandler
    create.ts         -- runCreate
    transpile.ts      -- runTranspile
    query.ts          -- runQuery (all subcommands)
    watch.ts          -- runWatch
    inspect.ts        -- runInspect
    verify.ts         -- verifyEnvironment
    help.ts           -- All help text functions
    parse.ts          -- parseArgs, validateArgs
    mod.ts

  transpiler/         -- Transpiler subsystem (extracted from engine/)
    transpiler.ts
    transpiler-types.ts
    transpiler-mappings.ts
    transpiler-ast.ts
    mod.ts

  fill/               -- Fill/create subsystem (extracted from engine/)
    fill.ts
    mod.ts

  transform/          -- Transform pipeline (extracted from engine/)
    transform.ts
    mod.ts

  api/                -- Programmatic interface (unchanged, but imports from commands/)
    mod.ts

  studio/             -- Web server (unchanged)
    serve.ts

  tool-error.ts       -- Standalone (no circular deps -- uses error code type, not getByCode)
```

### Key Changes

1. **Extract `types/`** from `foundation/` -- pure types with zero runtime imports break the circular dependency. `foundation/` dissolves: types go to `types/`, schema loading goes to `schema/`, health scoring goes to `types/health.ts`, config goes to `schema/config.ts`, cache goes to `schema/cache.ts`.

2. **Extract `commands/`** from `mod.ts` -- each command becomes a module. `mod.ts` becomes a thin dispatcher (~200 lines): parse args, dispatch to command module.

3. **Extract `transpiler/`, `fill/`, `transform/`** from `engine/` -- each command's domain logic gets its own module. `engine/` becomes lean runtime infrastructure.

4. **Extract `schema/`** -- schema loading is its own concern, not "foundation" and not "data". It reads TOML schemas and produces typed rules. Clean boundary.

5. **Resolve the circular dependency** -- `types/` has zero runtime imports. `data/` imports from `types/`. `schema/` imports from `types/` and `data/`. `engine/` imports from all three. `handlers/` imports from everything below. `commands/` imports from everything. Clean DAG.

### Proposed Dependency DAG

```
types/                  (0 imports -- pure types)
  ^
  |
data/                   (imports: types/)
  ^
  |
schema/                 (imports: types/, data/)
  ^
  |
engine/                 (imports: types/, data/, schema/)
  ^
  |
shared/                 (imports: types/, data/, schema/, engine/)
  ^
  |
handlers/               (imports: types/, data/, schema/, engine/, shared/)
  ^
  |
transpiler/ fill/ transform/  (imports: types/, data/, schema/, engine/, shared/)
  ^
  |
commands/               (imports: everything above)
  ^
  |
api/                    (imports: everything above)
  ^
  |
mod.ts                  (imports: commands/, api/)
```

No cycles. Each layer only imports from below.

---

## What Surprised Me

1. **mod.ts at 2,475 lines is the single biggest architectural debt.** It's a God object containing all command logic, all help text, argument parsing, and dispatch. The API module duplicates some of this, creating two code paths. Extracting commands will be the highest-impact change.

2. **The foundation/data circular dependency is subtle but real.** `code-schema.ts` needs `SectionEntry` and `FormDefinition` from `data/` to assemble rules, but `data/` needs `Severity` and `HebrewState` from `foundation/`. The fix is clean: extract pure types into their own module.

3. **The test structure already knows the right architecture.** Tests are organized by R-level (existence/R50, organization/R25, content/R10, pattern/R05) which is the actual conceptual architecture. The source code should follow this same organizing principle -- not by module function, but by what layer of truth each file serves.

4. **The handler size disparity tells the story.** Rust and Go handlers are 3,200-3,500 lines with full R50-R05 coverage. The other 6 handlers are 400-800 lines with only R50/R25. This IS the "catching up" gap. The shared infrastructure exists for code handlers; it needs equivalents for data and documentation handlers.

5. **The API module is already the right shape.** `api/mod.ts` at 1,330 lines provides a clean programmatic interface with proper types. It's the model for how `commands/` should work -- structured returns, no stdout coupling, pure logic.

---

## Recommendations

### Priority Order

1. **Extract `commands/` from `mod.ts`** -- highest impact, most structural debt. Each `run*` function becomes a module. `mod.ts` drops from 2,475 to ~200 lines.

2. **Extract `types/`** -- break the circular dependency. Clean foundation for everything else.

3. **Extract `schema/`** -- separate schema loading from types. Clean boundary.

4. **Slim `engine/`** -- move transpiler, fill, transform to their own modules. Engine becomes runtime infrastructure only.

5. **Upgrade remaining handlers** -- with clean architecture, adding R10/R05/transform/inspect to the 6 lighter handlers becomes tractable.

### What NOT to Change

- **Handler file-per-format pattern** -- this is correct. Each format is self-contained.
- **Test semantic naming** -- existence/organization/content/pattern is the right structure.
- **Import map aliases** -- `@lib/` and `@test/` serve well.
- **Data layer design** -- registries, error catalog, pipeline definitions are solid.

---

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost?"* -- Luke 14:28

The architecture is sound in concept. The R-level pipeline, the handler registry pattern, the schema-driven approach, the data layer -- all correct. What needs work is the physical organization: the God object in mod.ts, the entangled foundation/data layer, and the overloaded engine module. The structure knows what it wants to be. It just needs the reorganization to let it breathe.

-- Ezra Matthan
