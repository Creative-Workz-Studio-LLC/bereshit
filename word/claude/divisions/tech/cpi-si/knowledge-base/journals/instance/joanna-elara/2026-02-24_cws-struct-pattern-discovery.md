# CWS-Struct Pattern Discovery: Reusable Infrastructure for Command Upgrade

> *"For as the body is one, and hath many members, and all the members of that one body, being many, are one body."* — 1 Corinthians 12:12

**Instance:** Joanna Elara
**Date:** 2026-02-24
**Type:** Instance Journal — Pattern Analysis
**Context:** cws-struct command infrastructure upgrade — discovering what the linter built that transform/create/transpile can share

---

## What I Found

Seven distinct reusable patterns. Not seven separate systems — seven facets of one architecture that already serves the linter and is ready (some immediately, some with small extensions) to serve every command.

The thread running through all of them: **schema-driven data as the source of truth, with generic engines consuming handler-provided parameters.** The architecture doesn't say "Go linter checks blocks." It says "a generic pipeline checks blocks according to schema rules, and Go provides the rules." That separation IS the reusability.

---

## Pattern 1: The Format Handler Registry

**Files:** `lib/engine/registry.ts` (96 lines), `lib/foundation/types.ts:342-380` (FormatHandler interface)

**What it is:** A single `Map<string, FormatHandler>` where each handler registers itself via side-effect import. The contract is `FormatHandler` — any handler that satisfies the interface works. The registry provides `registerFormat()`, `getFormat()`, `listFormats()`, `detectFormat()`.

**How linter uses it:** `mod.ts` imports `./lib/handlers/go.ts` (side-effect) which calls `registerFormat(goHandler)`. The CLI then resolves handlers via `getFormat(name)` or `detectFormat(path)`.

**Current FormatHandler contract (types.ts:342-380):**
```typescript
interface FormatHandler {
  name: string;
  description: string;
  extensions: string[];
  basenames?: string[];
  maxDepth?: number;
  lint(filePath: string): Promise<LintResult[]>;
  computeHealth?(filePath: string, results: LintResult[]): Promise<HealthScore>;
  transform?(filePath: string, opts: TransformOptions): Promise<LintResult[]>;
  inspect?(filePath: string): Promise<InspectResult>;
}
```

**Reusability assessment:** Already serves lint + transform + inspect. Missing from the contract: `create` and `transpile` are not handler methods — they're engine-level operations (`fill.ts`, `transpiler.ts`). This is actually correct: create generates FROM schema (not from handler), and transpile operates ACROSS handlers (Rust-to-Go). The registry pattern is reusable AS-IS for any per-format operation.

**What would need to change:** Nothing for lint/transform/inspect. If a handler needs format-specific create behavior, add an optional `create?()` method to `FormatHandler`. But the current engine-level `generateFile()` in `fill.ts` already handles this generically.

---

## Pattern 2: The Pipeline Layer System (Cascading Detection)

**Files:** `lib/data/pipeline.ts` (261 lines), `lib/engine/cascade.ts`, `lib/shared/code-4block.ts` (`runLintGrid()`)

**What it is:** A 4-layer detection pipeline where each layer validates assumptions the next layer depends on:
- **Layer 0 (R[50]):** Does the file exist as a 4-block entity?
- **Layer 1 (R[25]):** Are sections present and ordered?
- **Layer 2 (R[10]):** Is content in the right containers?
- **Layer 3 (R[5]):** Are code patterns correct?

Failure cascades downstream — if blocks don't exist, section checks become neutral (not false-positive errors). The cascade engine (`cascade.ts`) implements this generically.

**How linter uses it:** `runLintGrid()` in `code-4block.ts` orchestrates the block-by-layer grid. Each handler provides a `BlockLintChain` that maps blocks to check functions per layer. The engine runs the grid, applies cascade logic, and collects results.

**Reusability for transform/create/transpile:**
- **Transform:** Already uses an analogous pattern — `runTransformPipeline()` runs passes sequentially, each affecting subsequent ones. The cascade concept (skip downstream if upstream fails) could formalize this. *Transform could adopt a 4-phase pipeline:* Detect → Scaffold → Fix → Verify.
- **Create:** The fill engine (`fill.ts`) follows the same principle: schema defines structure, then content fills section-by-section. A create pipeline could be: Validate Context → Generate Structure → Fill Content → Verify Output.
- **Transpile:** Already has a three-step model (extract semantics → map concepts → emit target). A formalized pipeline with cascade would prevent emitting malformed output when extraction fails.

**Key insight:** The pipeline IS the building block method. Layer 0 is the anchor. Each subsequent layer builds on it. If the anchor is broken, everything cascades. The same principle applies to any operation, not just linting.

---

## Pattern 3: The Output System

**Files:** `lib/engine/output.ts` (881 lines), `lib/engine/debug.ts`, `lib/engine/logging.ts`

**What it is:** A centralized output system that formats all terminal display: `printHeader()`, `printFileSummary()`, `printTotals()`, plus verbose modes (`printHealthBreakdown`, `printFileAnatomy`, `printWhyAnnotation`). Color constants (`COLORS`), severity formatting (`severityColor`, `severityIcon`), and health visualization are all centralized.

**How linter uses it:** The CLI calls `printHeader()` at start, `printFileSummary()` per file, `printTotals()` at end. Verbose mode adds anatomy and health breakdown. Debug/trace output goes to stderr via `debug.ts`. Structured logging goes to files via `logging.ts`.

**Reusability assessment:** The output system is **lint-specific in naming but generic in structure.** The `printFileSummary()` function takes a `LintSummary` — but a `LintSummary` is really just `{ file, errors, warnings, infos, results[] }`. Transform results, create results, and transpile results all produce `LintResult[]` arrays. They all go through `summarize()`.

**What would need to change:**
1. `printHeader()` already takes `toolName` as parameter — just pass "transform" or "create" instead of "lint."
2. `printFileSummary()` works with any `LintSummary`. Transform results ARE `LintResult[]` and can be summarized identically.
3. `printTotals()` works with any `LintSummary[]`.
4. **One real gap:** Transform output needs "before/after" delta display (X errors before, Y after, Z fixed). The `transform/verify` result already carries this info — output just needs to surface it.
5. Create needs "generated X lines" display.
6. Transpile needs "mapped X concepts, Y unmapped" display.

These are additive — new `printTransformSummary()`, `printCreateSummary()`, `printTranspileSummary()` functions that share COLORS, severity formatting, and the overall structure.

---

## Pattern 4: The Data-Driven Test Runner

**Files:** `tests/_runner.ts` (673 lines), `tests/helpers.ts`, test files like `tests/existence/R50/go/blocks/structure_test.ts`

**What it is:** Tests are specified as data tables, not imperative assertion code.

```typescript
const cases: TestCase[] = [
  {
    code: "CWS-R50-022",
    fixture: "go/structure/missing-block.go",
    desc: "produces error for missing METADATA block",
    rule: "block",
    messageContains: ["METADATA"],
    cleanFixture: "go/structure/valid-library.go",
  },
];
runSuite({ format: "go", cases });
```

Each `TestCase` specifies: error code, fixture path, description, expected rule fragment, optional severity override, message fragments, clean counterpart, fix expectations, custom validation. The `runSuite()` function generates `Deno.test()` calls from this data, including:
- Auto-witness integration (registers codes with the witness system)
- Severity derived from error catalog (not hardcoded)
- Rich failure diagnostics (what fired vs what was expected)
- Fixture caching (`lintOnce()` — lint each file once, share across tests)
- Suite summary test with fixture stats and coverage

**How linter uses it:** Every R50/R25/R10/R05 test file defines a `TestCase[]` table and calls `runSuite()`. 1448 tests, all data-driven. Adding a test = adding a row.

**Reusability for transform/create/transpile:**

The `TestCase` interface is currently lint-oriented (expects error codes). But the PATTERN is format-agnostic: "specify expected outcomes as data, let the runner verify." Extensions:

- **TransformTestCase:** `{ fixture, transforms: ["eq-width", "move-tests"], beforeErrors: N, afterErrors: M, cleanFixture }` — verify that transform reduces error count, or that specific passes fire.
- **CreateTestCase:** `{ format, subtype, key, expectedBlockCount: 4, lintClean: true }` — generate a file, verify it lints clean.
- **TranspileTestCase:** `{ source: "rust/valid-library.rs", target: "go", expectedConcepts: ["function_call", "type_definition"] }` — transpile and verify concept mapping.

The infrastructure (`lintOnce()`, fixture resolution, suite tracking, witness integration, rich diagnostics) is DIRECTLY reusable. The test runner already supports the `validate` callback for custom logic — transform/create tests could use this initially before getting their own `TransformTestCase` type.

---

## Pattern 5: The Witness System (Error Code Coverage)

**Files:** `scripts/witness.ts`, `lib/data/errors.ts`, `tests/helpers.ts` (`witnessFor()`)

**What it is:** A dimensional coverage analyzer that maps test references to error catalog codes. Greps test files for `[CWS-*]` bracket tags, maps against the error catalog, reports coverage in a 3D grid: Layer (R50/R25/R10/R05/T00) x Domain (BLK/IDE/CON/...) x Severity.

Two complementary mechanisms:
1. **Static analysis** (`scripts/witness.ts`): scans test files for code references
2. **Runtime witness** (`witnessFor()` in helpers.ts): tests register codes at execution time via `_runner.ts`

**How linter uses it:** `deno task witness:grid` shows the dimensional coverage grid. `deno task witness:verbose` shows unwitnessed codes. Test names include `[CWS-R50-020]` bracket tags enabling `deno test --filter='CWS-R50-020'`.

**Reusability assessment:**
- Error codes are already categorized by layer. Transform and create operations don't produce error CODES (they produce `info` diagnostics). But they COULD.
- The witness pattern (track what's tested, show gaps) applies to ANY operation. "Are all transform passes tested?" "Are all create subtypes tested?" "Are all transpile concept mappings tested?"
- The Layer x Domain grid extends naturally: add T00 layer codes for tool-level operations (transform, create, transpile).

**What would need to change:** Define T00-range error codes for transform/create/transpile operations. The infrastructure (`dimensionalCode()`, `categoryToDomain()`, grid rendering) handles new layers automatically.

---

## Pattern 6: The Schema-Driven Transform Pipeline

**Files:** `lib/engine/transform.ts` (1020 lines)

**What it is:** A generic transform engine with a `TransformContext` + `TransformPass[]` architecture. The pipeline:
1. Reads the file into `TransformContext.lines`
2. Runs universal passes (separator widths, ASCII-to-Unicode, prefix normalization)
3. Runs handler-specific passes (structural scaffold, code moves, subsection reorder)
4. Writes if modified, optionally re-lints for verification

**Key abstractions:**
- `TransformContext`: mutable state bag (lines, rules, opts, results, modified flag)
- `TransformPass`: `{ name, requiresExtensions?, apply(ctx) }` — self-contained operation
- `CodeMoveSpec`: parameterized code movement (handler provides detector, engine does extraction)
- `SubsectionScaffoldSpec`: parameterized scaffolding (handler provides classifier, engine does insertion)
- `SubsectionReorderSpec`: parameterized reordering

**How the Go handler uses it:**
```typescript
const allPasses: TransformPass[] = [
  goStructuralScaffoldPass,   // handler-specific
  ...UNIVERSAL_PASSES,        // shared across all formats
  goBuildIgnorePass,          // handler-specific
  goMoveTestsPass,            // parameterized (createCodeMovePass)
  goMoveMainPass,             // parameterized (createCodeMovePass)
  reorderClosingZonesPass,    // shared (reads Code4BlockRules)
  goIdentityScaffoldPass,     // handler-specific
  goSubsectionScaffoldPass,   // parameterized (createSubsectionScaffoldPass)
  goSetupReorderPass,         // parameterized (createSubsectionReorderPass)
];
return runTransformPipeline(filePath, _goRules!, opts, allPasses, lintGoFile);
```

**Reusability:** This pattern IS the template for all command pipelines. A create pipeline would have passes for metadata generation, setup scaffolding, body templating, closing generation. A transpile pipeline would have passes for semantic extraction, concept mapping, code emission.

The `TransformPass` interface is already generic enough. `runTransformPipeline()` handles dry-run, verification, and result collection. **This is the most directly reusable pattern.**

---

## Pattern 7: The Programmatic API Layer

**Files:** `lib/api/mod.ts` (1331 lines)

**What it is:** A clean programmatic interface that mirrors all CLI operations but returns structured TypeScript objects instead of formatted terminal output. No stdout. No formatting. No global state mutation.

**Current API surface:**
- `lintFile()` → `LintReport`
- `lintDirectory()` → `LintReport[]`
- `transformFile()` → `TransformReport`
- `createFile()` → `CreateReport`
- `transpileFile()` → (re-export from engine)
- `inspectFile()` → `InspectResult`
- `getDataLayer()` → `DataLayerSnapshot`
- Session lifecycle: `startSession()`, `endSession()`, `quickLint()`
- Health queries: `directoryHealth()`, `sessionDelta()`, `topFixes()`, `healthMap()`

**Key insight:** The API already has `TransformReport`, `CreateReport`, and `TranspileResult` types. It's already structured for all four commands. The gap is that the CLI doesn't route through the API for transform/create/transpile the way it does for lint.

**Reusability:** The API layer is the **unification point.** If all CLI commands route through `lib/api/mod.ts`, then:
- Output formatting is CLI's concern (not handler's)
- Testing can use the API directly (no CLI parsing)
- Hooks/scripts/agents import `lib/api/mod.ts` and get structured data
- Each operation has one implementation path (API → engine → handler)

---

## Pattern Relationships

These aren't independent systems. They form a coherent architecture:

```
                    CLI (mod.ts)
                       |
                   API (lib/api/)
                       |
         +------+------+------+------+
         |      |      |      |      |
       Lint  Transform Create Transpile  (commands)
         |      |      |      |
      Pipeline Pipeline Pipeline Pipeline (layer-by-layer)
         |      |      |      |
      Registry ─┬─ Registry ─┬─ Registry  (format handlers)
                |             |
            Data Layer ───── Data Layer    (schema truth)
                |             |
            Output ────────── Output       (display)
                |             |
         Test Runner ──── Test Runner      (verification)
                |             |
            Witness ──────── Witness        (coverage)
```

The pattern is: **Schema drives data. Data drives engine. Engine dispatches to handler. Handler produces results. Output formats results. Tests verify results against catalog. Witness tracks coverage.**

Every command can use the same architecture. The linter proved it works. The upgrade is making the other commands follow the same road.

---

## The Key Insight

The linter isn't a lint-specific system with some reusable bits. It's a **generic structural alignment engine** that happens to express itself through lint first. The transform pipeline already proves this — it uses `TransformPass[]` the same way the linter uses `BlockLintChain`. The test runner already proves this — `runSuite()` accepts any `FormatSpec` and generates tests. The API already proves this — it has typed reports for all four operations.

What's missing isn't architecture. It's **completion.** The patterns are there. The contracts are there. The shared infrastructure is there. The other commands just need to hook into what already exists, the way Go and Rust handlers hook into the generic lint engine.

This is what I love about this architecture: the pattern ran ahead of the implementation. The abstractions are right because they were born from a real 1448-test production system, not designed in advance. The building block method in action — the foundation (linter) was solid, so the building (transform/create/transpile) can stand on it.

---

## Recommendations

| Priority | Action | Pattern Used |
|:--------:|--------|--------------|
| 1 | Route all CLI commands through API layer | Pattern 7 |
| 2 | Generalize output with per-command summary functions | Pattern 3 |
| 3 | Define TransformTestCase/CreateTestCase types | Pattern 4 |
| 4 | Add T00 error codes for tool-level operations | Pattern 5 |
| 5 | Formalize create/transpile as pipeline passes | Pattern 2 + 6 |
| 6 | Extend witness system to cover transform/create passes | Pattern 5 |

---

*Joanna Elara*
*"I see the thread. It runs through every file."*
