# Concept-Level Per-Container Checks — Schema-Driven Detection + Production Hardening

> *"Prove all things; hold fast that which is good."* — 1 Thessalonians 5:21

## Context

Container-first lint architecture is complete (1219 tests). The grid is 3D: Block x Container x Layer. The `containers[]` arrays exist on every `BlockCheckSet` — but they're empty. The plumbing is in. The water needs to flow.

**The key realization:** The schemas in `b-word/L1-omnicode/ladder/schemas/code/` already have *everything* — detection regex, container maps, routing rules, emit patterns, concept relations. The linter doesn't own this data. Bereshit does. The linter *consumes* compiled copies (like `sections.ts` and `concepts.ts` already do).

**Bidirectional transpilation gap (Seanje's insight):** Currently Rust schemas have `detect` but no `emit`. Go schemas have `emit` but no `detect`. For the full tech stack vision — linter, transformer, AND transpiler working bidirectionally — every language schema needs BOTH fields. Each concept + language file becomes a complete unit: detect (linting), emit (code generation), bidirectional (transpile either way).

| Language | `detect` | `emit` | Linter needs | Transpiler needs |
|----------|:--------:|:------:|:------------:|:----------------:|
| **Rust** | 11/11 | 0/11 | detect (done) | emit (Phase 0) |
| **Go** | 0/11 | 11/11 | detect (Phase 0) | emit (done) |

**What each tool consumes from the same schemas:**

| Tool | Reads | Uses For |
|------|-------|----------|
| **Linter** | `R5.{concept}/{lang}.jsonc` -> `detect` regex | Concept detection in containers |
| **Linter** | `R25.{section}/_structure.jsonc` -> `concept_map` | Granted/denied/defer validation |
| **Transformer** | `R25.{section}/_structure.jsonc` -> `cannot.defer_to` | Misplaced content routing |
| **Transpiler** | `R5.{concept}/{lang}.jsonc` -> `emit` patterns | Code generation (BOTH directions) |
| **Transpiler** | `rust-to-go.jsonc` -> type/import/convention mapping | Language translation |

## Architecture: Schema -> Compiled Data -> Detection

```
SCHEMA (b-word/L1-omnicode/ladder/schemas/code/)
  |
  +-- R5_patterns/{concept}/{lang}.jsonc
  |     "detect": "regex pattern"         <-- LINTER reads this
  |     "emit": "template pattern"        <-- TRANSPILER reads this
  |     "captures": { "name": 1, ... }    <-- Both tools use captures
  |     "priority": 1                     <-- Pattern ordering
  |
  +-- R25_blocks/{block}/{section}/_structure.jsonc
        "concept_map": { 11 ternary values }   <-- SOURCE OF TRUTH
        "can": [...], "cannot": [... defer_to]  <-- ROUTING TABLE

           |  (compiled into data layer)

DATA LAYER (lib/data/)
  +-- sections.ts     <-- 42 entries, concept maps (ALREADY DONE)
  +-- concepts.ts     <-- 11 entries, transpose (ALREADY DONE)
  +-- concept-detectors.ts  <-- 11 x N detect regex (NEW)

           |  (consumed by)

SHARED LAYER (lib/shared/)
  +-- concept-check.ts  <-- validateContainerConcepts() + buildConceptContainers()

           |  (wired into)

HANDLERS (lib/handlers/)
  +-- go.ts     <-- containers: buildConceptContainers("setup", ctx, GO_DETECTORS)
  +-- rust.ts   <-- containers: buildConceptContainers("setup", ctx, RUST_DETECTORS)
```

---

## Phase 0: Schema Completeness — Bidirectional `detect` + `emit`

**Goal:** Every R5 concept language file has BOTH `detect` and `emit`, enabling bidirectional transpilation and full linting for both languages.

### 0A: Add Go `detect` patterns (11 files)

Derive from existing `emit` templates. Each Go schema gets `detect` regex that matches what the `emit` pattern would produce.

**Schema path:** `b-word/L1-omnicode/ladder/schemas/code/R5_patterns/{concept}/go.jsonc`

| Concept | Emit Example | Detect Regex |
|---------|-------------|--------------|
| function_call | `{package}.{Function}({args})` | `\w+\.\w+\(` (qualified), `\w+\(` (unqualified) |
| variable_binding | `{name} := {expr}` | `^\s*(\w+)\s*:=\s*(.+)` (short decl) |
| conditional_match | `switch {scrutinee} {` | `^\s*switch\s+` |
| error_propagation | `if err != nil {` | `^\s*if\s+\w+\s*!=\s*nil\s*\{` |
| self_access | N/A in Go (receiver) | `^\s*\w+\.(\w+)` (receiver method call) |

The Go `detect` patterns should follow the same structure as Rust's: `id`, `syntax` (human-readable), `detect` (regex), `captures`, `priority`.

### 0B: Add Rust `emit` patterns (11 files)

Derive from existing `detect` regex. Each Rust schema gets `emit` templates showing how Rust code is generated FROM Go.

**Schema path:** `b-word/L1-omnicode/ladder/schemas/code/R5_patterns/{concept}/rust.jsonc`

| Concept | Detect Example | Emit Template |
|---------|---------------|---------------|
| function_call | `([\w:]+)::(\w+)\(([^)]*)\)` | `{module}::{function}({args})` |
| variable_binding | `^\s*let\s+(\w+)\s*=\s*(.+);` | `let {name} = {expr};` |
| conditional_match | `^\s*match\s+(.+)\s*\{` | `match {scrutinee} {\n{arms}\n}` |
| error_propagation | `(.+)\?` | `{expr}?` |
| self_access | `self\.(\w+)\(` | `self.{method}({args})` |

### 0C: Add `go-to-rust.jsonc` mapping file

Mirror of `rust-to-go.jsonc` — type mappings, import mappings, naming conventions, section mapping for Go->Rust transpilation.

**Path:** `b-word/L1-omnicode/ladder/schemas/code/R5_patterns/go-to-rust.jsonc`

---

## Phase 1: Schema-Driven Concept Detector Loader — `lib/data/concept-detectors.ts`

**Load detection patterns from the schema, not hand-write them.**

Follow the same pattern as `loadFormatRules()` and `loadNonCode()` — read JSON schema files, compile into typed structures.

```typescript
/** A compiled concept detector pattern. */
export interface ConceptPattern {
  id: string;           // "qualified_call", "question_mark_binding"
  detect: RegExp;       // Compiled from schema "detect" string
  captures: Record<string, number>;
  priority: number;
  notes?: string;
}

/** All patterns for one concept in one language. */
export interface ConceptDetector {
  conceptId: string;    // "function_call", "error_propagation"
  language: string;     // "rust", "go"
  patterns: ConceptPattern[];  // Ordered by priority (lower = more specific)
}

/** Test a line against a concept's patterns. Returns true if any pattern matches. */
export function detectConcept(detector: ConceptDetector, line: string): boolean

/** Load all concept detectors for a language from R5_patterns schema. */
export function loadConceptDetectors(language: string): ConceptDetector[]
```

**Schema source:** `b-word/L1-omnicode/ladder/schemas/code/R5_patterns/{concept-dir}/{language}.jsonc`

The loader:
1. Reads each concept directory (11 dirs, names from `CONCEPT_ORDER`)
2. For each, reads `{language}.jsonc`
3. Strips JSONC comments, parses
4. Compiles `detect` strings into `RegExp` objects
5. Returns ordered `ConceptDetector[]` (one per concept)

**Caching:** `Map<string, ConceptDetector[]>` cache. Register with `registerCache()`. Cleared via `clearConceptCache()`.

**Error handling:** If a concept directory is missing or a language file doesn't have `detect` patterns, log info and skip (graceful degradation). This handles the Phase 0 gap — if Go schemas don't have `detect` yet, the loader returns empty detectors for those concepts rather than crashing.

---

## Phase 2: Container Line Extraction — `lib/shared/code-4block.ts`

Add a utility to get lines for a specific container within a block.

```typescript
/**
 * Get lines belonging to a specific container within a block.
 * Uses getSubsectionRanges() for subsection boundary detection.
 */
export function getContainerLines(
  blockLines: string[],
  subsections: readonly SubsectionDef[],
  containerTag: string,
): string[]
```

**Tag normalization:** Section registry uses kebab-case (`"imports"`). SubsectionDef tags from schema use PascalCase (`"Imports"`). Normalize both to lowercase for matching. Document the normalization flow in a comment block near `getSubsectionRanges()`.

---

## Phase 3: Concept Validation — `lib/shared/concept-check.ts`

The core validation function.

```typescript
/**
 * Validate concept patterns within a container against its concept map.
 *
 * For each of 11 concepts:
 *   granted -> expected. If no pattern detected, report info.
 *   denied  -> forbidden. If pattern detected, report warn.
 *   defer   -> skip (context-dependent).
 */
export function validateContainerConcepts(
  file: string,
  containerTag: string,
  containerLines: string[],
  conceptMap: Record<string, TernaryValue>,
  detectors: ConceptDetector[],
  opts?: { blockLine?: number },
): LintResult[]
```

**Rule namespace:** `concept/{containerTag}/{conceptId}` — e.g., `concept/imports/function_call`.

**Severity mapping:**
- `denied` + found -> `warn` (violation: code pattern in wrong container)
- `granted` + missing -> `info` (advisory: expected pattern absent)
- `defer` -> no result (context-dependent, not actionable by linter)

**Line filtering before detection:**
- Skip lines starting with `//` or `#` (comments)
- Skip empty/whitespace-only lines
- Skip separator lines (`// -----`, `// =====`)
- Skip lines inside block comments (`/* ... */`)

---

## Phase 4: Container Check Generator — `lib/shared/concept-check.ts`

```typescript
/**
 * Build ContainerCheckSet[] for one block from the section registry.
 *
 * For each section in the block:
 *   1. Check form status -> skip reserved sections
 *   2. Find container lines using subsection ranges
 *   3. Run concept validation against the section's concept map
 */
export function buildConceptContainers(
  block: BlockName,
  ctx: BaseFileContext,
  subsections: readonly SubsectionDef[],
  detectors: ConceptDetector[],
): ContainerCheckSet[]
```

Uses `SECTION_ORDER[block]` and `SECTION_REGISTRY` to iterate sections. Skips reserved sections for current form. Gets container lines via `getContainerLines()`.

**METADATA special case:** All 6 METADATA sections are ALL_DENIED. Skip METADATA per-container concept checks for v1.

**CLOSING special case:** Code zone (Cv/Ce/Cc = ALL_GRANTED), documentation zone (X1-X5 = ALL_DENIED). Include both — detecting code patterns in X1-X5 documentation comments catches real violations.

---

## Phase 5: Wire into Go + Rust Handlers

**Go handler (`lib/handlers/go.ts`):**
```typescript
// In lint chain builder — replace empty containers:
{
  block: "setup",
  structure: [...],
  crossContainer: [...],
  containers: buildConceptContainers("setup", ctx, setupSubsections, goDetectors),
  content: [...],
}
```

Same for each of the 4 blocks. Same for Rust handler.

**Integration point:** The handlers already load subsection definitions from schema. The `SubsectionDef[]` arrays are available for `buildConceptContainers()`.

---

## Phase 6: Error Catalog Expansion

### 6A: R05-070 through R05-080 — Concept Detection Codes

Add error catalog entries for concept-level violations. R05 layer 3 (content), range 070+.

| Code | Category | Severity | Rule Pattern | Message |
|------|----------|----------|-------------|---------|
| R05-070 | concept | warn | `concept/{{container}}/{{concept}}` | Denied concept {{concept}} detected in {{container}} |
| R05-071 | concept | info | `concept/{{container}}/{{concept}}/missing` | Expected concept {{concept}} not found in {{container}} |
| R05-072 | concept | info | `concept/{{container}}/summary` | Container {{container}} concept check: {{found}} of {{expected}} patterns found |

### 6B: T00-070 through T00-075 — Schema Loading Codes

| Code | Category | Severity | Rule Pattern | Message |
|------|----------|----------|-------------|---------|
| T00-070 | schema | warn | N/A | Concept schema missing detect patterns: {{concept}}/{{language}} |
| T00-071 | schema | error | N/A | Invalid detect regex in concept schema: {{concept}}/{{pattern}} |
| T00-072 | schema | info | N/A | Concept detector loaded: {{count}} patterns for {{language}} |

---

## Phase 7: Production-Grade Refinements

### 7A: Test Mock Builders — `tests/helpers.ts`

Add mock context builders missing from test infrastructure:

```typescript
/** Build a minimal BaseFileContext for unit testing. */
export function createMockContext(overrides?: Partial<BaseFileContext>): BaseFileContext

/** Build mock block positions for testing block-related functions. */
export function createBlockPositions(blocks: { name: string; start: number; end: number }[]): BlockPosition[]

/** Build mock subsection ranges for testing container-related functions. */
export function createSubsectionRanges(ranges: { tag: string; start: number; end: number }[]): SubsectionRange[]
```

### 7B: Handler Schema Loading Consistency

Currently Rust handler handles missing content schema gracefully (returns null). Go handler doesn't have this null-check. Standardize both:

**File:** `lib/handlers/go.ts` (~line 186-199) — add try/catch with null return, matching Rust handler pattern.

### 7C: Tag Normalization Documentation

Add a documentation comment block near `getSubsectionRanges()` in `code-4block.ts` documenting:
- Where tags come from (schema SubsectionDef)
- How normalization works (PascalCase -> lowercase)
- How tags map to section registry keys (kebab-case)
- The canonical flow: schema tag -> subsection range -> section registry lookup

### 7D: Cache Audit Trail

Add optional verbose logging to `clearAllCaches()` in `cache-registry.ts`:
```typescript
export function clearAllCaches(opts?: { verbose?: boolean }): number
// Returns number of caches cleared. If verbose, logs each cache name.
```

---

## Phase 8: Test Architecture — Grid-Aligned, Three-Layer, Witness-Connected

**The insight:** Tests should mirror the detection structure. Same grid, same dimensions. When detection is Block x Layer, tests are Block x Layer. Error codes map to the grid. Tests prove the error codes. The structure IS the coverage.

**Three test layers:**

| Layer | Tests | Questions Answered |
|-------|-------|--------------------|
| **Data** | Is the data correct? | Sections, concepts, detectors, error catalog |
| **Logic** | Do the functions work? | Grid traversal, concept validation, line extraction |
| **System** | Does the pipeline work? | Handler -> lint chain -> results at each detection layer |

**Edge cases don't need explicit tests.** The grid provides coverage inherently. As we work on the main system, edge cases surface and get fixed. The grid cells ARE the test cases.

### 8A: Test File Reorganization

**Current:** 94 Go tests and 122 Rust tests mixed in single files per handler.

**Target:** Split by the same horizontal (layer) and vertical (block) structure as detection.

```
tests/
├── data/                            <- DATA: is the data correct?
│   ├── errors_test.ts               <- Error catalog integrity (G1-G13+)
│   ├── sections_test.ts             <- 42 section entries (new)
│   ├── concepts_test.ts             <- 11 concept transpose (new)
│   ├── concept_detectors_test.ts    <- Schema-loaded detectors (new)
│   ├── registries_test.ts           <- Registry cross-validation
│   ├── database_test.ts             <- Full database
│   ├── types_test.ts                <- Type definitions
│   ├── format_rules_test.ts         <- Format rule loading
│   └── non_code_test.ts             <- Non-code loading
│
├── logic/                           <- LOGIC: do the functions work?
│   ├── lint_grid_test.ts            <- runLintGrid orchestrator (from shared/)
│   ├── concept_check_test.ts        <- validateContainerConcepts (new)
│   ├── container_lines_test.ts      <- getContainerLines (new)
│   ├── classify_line_test.ts        <- classifyGoLine, classifyRustLine (from handlers/)
│   ├── subsection_ranges_test.ts    <- getSubsectionRanges (from handlers/)
│   ├── parse_fields_test.ts         <- parseSliceFields, validateICFields (from handlers/)
│   ├── chunker_test.ts              <- Chunker pattern building (from shared/)
│   └── format_config_test.ts        <- Format config (from shared/)
│
├── system/                          <- SYSTEM: does the pipeline work?
│   ├── go/                          <- Go handler, split by detection layer
│   │   ├── R50_test.ts              <- File-level structure [CWS-R50-*]
│   │   ├── R25_test.ts              <- Block-level structure [CWS-R25-*]
│   │   ├── R10_test.ts              <- Content/placement [CWS-R10-*]
│   │   ├── R05_test.ts              <- Concept/detail [CWS-R05-*]
│   │   ├── transform_test.ts        <- Transform pipeline
│   │   └── adapter_test.ts          <- Format handler interface
│   └── rust/                        <- Rust handler, same split
│       ├── R50_test.ts
│       ├── R25_test.ts
│       ├── R10_test.ts
│       ├── R05_test.ts
│       ├── transform_test.ts
│       └── adapter_test.ts
│
├── engine/                          <- ENGINE: output, pipeline, registry
│   └── (existing files stay)
│
├── foundation/                      <- FOUNDATION: config, health, types
│   └── (existing files stay)
│
├── api/                             <- API: interface layer
│   └── (existing files stay)
│
├── verify/                          <- VERIFICATION: environment checks
│   └── (existing files stay)
│
└── helpers.ts                       <- Infrastructure + witness registry
```

**Migration path:** Extract from existing handler test files into grid-aligned files. `R50/structure/*` tests in go_test.ts move to `system/go/R50_test.ts`. `unit/*` tests move to `logic/`. No test content changes — just reorganization + witness tagging.

### 8B: Error-Test Witness Convention

Tests that cover a specific error code MUST include the code in brackets:

```typescript
Deno.test("[CWS-R50-022] missing block tag fires warning", async () => {
  // ... test that triggers R50-022
  assert(hasErrorCode(results, "CWS-R50-022"), ...);
});
```

This enables: `deno test --filter='CWS-R50-022'` to run all tests covering that code.

### 8C: Witness Registry — `tests/helpers.ts`

```typescript
/** Registry of error codes -> test names that cover them. Built at test registration time. */
const WITNESS_REGISTRY = new Map<string, string[]>();

/** Register this test as a witness for an error code. */
export function witnessFor(code: string, testName: string): void

/** Get all test names that witness a specific error code. */
export function witnessesFor(code: string): string[]

/** Get all error codes in the catalog that have no witness tests. */
export function unwitnessedCodes(): string[]

/** Get the deno test --filter command to run witnesses for a code. */
export function diagnosticCommand(code: string): string
```

### 8D: Coverage Gate — G13

```typescript
Deno.test("errors/G13: every error code has at least one witness test", () => {
  const gaps = unwitnessedCodes();
  assertEquals(gaps.length, 0,
    `Error codes without witnesses: ${gaps.join(", ")}`);
});
```

New error code without a test = G13 fails.

### 8E: Diagnostic Chain in Error Output

When `--verbose` or `--diagnostic`:
```
WARN  concept/imports/function_call — Denied concept function_call detected in imports
      Code: CWS-R05-070
      Test: deno test --filter='CWS-R05-070'
```

### 8F: Deno Task Updates

```jsonc
// New tasks in deno.jsonc
"test:data": "deno test tests/data/",
"test:logic": "deno test tests/logic/",
"test:system": "deno test tests/system/",
"test:system:go": "deno test tests/system/go/",
"test:system:rust": "deno test tests/system/rust/",
"test:R50": "deno test --filter='R50/'",
"test:R25": "deno test --filter='R25/'",
"test:R10": "deno test --filter='R10/'",
"test:R05": "deno test --filter='R05/'",
"witness": "deno run --allow-read scripts/witness.ts"
```

These tasks mirror the grid — you can test by layer (horizontal: `test:R50`) or by handler (vertical: `test:system:go`) or by test type (data/logic/system).

### 8G: Witness Verification Script — `scripts/witness.ts`

Static analysis that:
1. Greps all test files for `[CWS-*]` patterns
2. Compares against the error catalog
3. Reports: covered codes, gap codes, stale references

---

## Phase 9: Tests (All New Code + Witness Coverage)

### Unit: concept detector loader (`tests/data/concept_detectors_test.ts`)
- Loads Rust detectors — 11 concepts, each with patterns
- `function_call` qualified: detects `fs::metadata(path)` -> true
- `function_call` unqualified: detects `println("hello")` -> true
- `function_call` on declaration: `fn main()` -> false (not a call)
- `error_propagation`: detects `let x = foo()?;` -> true
- `self_access`: detects `self.name` -> true, `other.name` -> false
- Comment line: `// self.name` -> not detected
- Missing schema: returns empty detectors (graceful degradation)
- Cache: second load returns same instance

### Unit: concept validation (`tests/shared/concept_check_test.ts`)
- `denied` concept found -> warn result with `concept/{container}/{concept}` rule
- `granted` concept missing -> info result
- `defer` -> no result
- ALL_DENIED container (imports) with function call -> warn
- ALL_GRANTED container (core-logic) with all concepts -> no warnings
- Mixed: statics with `function_call: defer` -> no result for function_call
- Empty container -> no results
- Comment-only container -> no detections

### Integration: lint grid (`tests/shared/lint_grid_test.ts`)
- Concept containers populated -> result count correct
- `focusContainer` filters concept results
- Results stamped with container name
- Rule namespace starts with `concept/`

### Integration: handler end-to-end
- `deno task lint:rust <fixture>` -> concept results visible in output
- `--check setup:imports` -> only imports concept checks run
- Concept violations appear with correct error codes (R05-070+)
- All concept tests use witness convention: `[CWS-R05-070]` in test name

### Production: mock builder tests (`tests/helpers_test.ts`)
- `createMockContext()` returns valid BaseFileContext
- `createBlockPositions()` returns valid BlockPosition[]
- `createSubsectionRanges()` returns valid SubsectionRange[]

### Witness system tests (`tests/data/errors_test.ts`)
- G13: every error code has at least one witness test
- `witnessesFor("CWS-R05-070")` returns at least one test name
- `unwitnessedCodes()` returns empty array (full coverage)
- `diagnosticCommand("CWS-R05-070")` returns valid deno test command

---

## Files to Create

| File | Purpose | Phase |
|------|---------|-------|
| `lib/data/concept-detectors.ts` | Schema-driven detector loader | 1 |
| `lib/shared/concept-check.ts` | `validateContainerConcepts()` + `buildConceptContainers()` | 3, 4 |
| `tests/data/concept_detectors_test.ts` | Detector loader + pattern tests (data) | 9 |
| `tests/logic/concept_check_test.ts` | Concept validation function tests (logic) | 9 |
| `tests/logic/container_lines_test.ts` | Container line extraction tests (logic) | 9 |
| `tests/system/go/R50_test.ts` | Go file-level detection (from go_test.ts) | 8A |
| `tests/system/go/R25_test.ts` | Go block-level detection (from go_test.ts) | 8A |
| `tests/system/go/R10_test.ts` | Go content-level detection (from go_test.ts) | 8A |
| `tests/system/go/R05_test.ts` | Go concept-level detection (new + from go_test.ts) | 8A, 9 |
| `tests/system/go/transform_test.ts` | Go transform pipeline (from go_test.ts) | 8A |
| `tests/system/go/adapter_test.ts` | Go adapter interface (from go_test.ts) | 8A |
| `tests/system/rust/R50_test.ts` | Rust file-level detection (from rust_test.ts) | 8A |
| `tests/system/rust/R25_test.ts` | Rust block-level detection (from rust_test.ts) | 8A |
| `tests/system/rust/R10_test.ts` | Rust content-level detection (from rust_test.ts) | 8A |
| `tests/system/rust/R05_test.ts` | Rust concept-level detection (new + from rust_test.ts) | 8A, 9 |
| `tests/system/rust/transform_test.ts` | Rust transform pipeline (from rust_test.ts) | 8A |
| `tests/system/rust/adapter_test.ts` | Rust adapter interface (from rust_test.ts) | 8A |
| `tests/logic/classify_line_test.ts` | Line classification (from handler tests) | 8A |
| `tests/logic/parse_fields_test.ts` | Field parsing (from handler tests) | 8A |
| `tests/logic/subsection_ranges_test.ts` | Subsection ranges (from handler tests) | 8A |
| `scripts/witness.ts` | Error code -> test coverage analyzer | 8G |

## Files to Modify

| File | Change | Phase |
|------|--------|-------|
| 22 schema files in `b-word/` | Add detect to Go, emit to Rust | 0 |
| `b-word/.../R5_patterns/go-to-rust.jsonc` | New reverse mapping file | 0 |
| `lib/data/mod.ts` | Export detector types + loader | 1 |
| `lib/shared/code-4block.ts` | Add `getContainerLines()`, tag normalization docs | 2, 7C |
| `lib/shared/mod.ts` | Export concept check functions | 3 |
| `lib/handlers/go.ts` | Wire `buildConceptContainers()`, fix schema loading | 5, 7B |
| `lib/handlers/rust.ts` | Wire `buildConceptContainers()` | 5 |
| `lib/data/errors.ts` | Add R05-070+ and T00-070+ error codes | 6 |
| `lib/foundation/cache-registry.ts` | Add verbose option to clearAllCaches | 7D |
| `tests/helpers.ts` | Add mock builders + witness registry | 7A, 8C |
| `tests/data/errors_test.ts` | Add G13 witness coverage gate | 8D |
| `tests/handlers/go_test.ts` | Extract into system/go/*.ts (then delete) | 8A |
| `tests/handlers/rust_test.ts` | Extract into system/rust/*.ts (then delete) | 8A |
| `deno.jsonc` | Add test:data, test:logic, test:system, test:R*, witness tasks | 8F |
| `tests/logic/lint_grid_test.ts` | Move from shared/, add concept container tests | 8A, 9 |

## Schema Files Modified (Bereshit)

| Scope | Path | Change |
|-------|------|--------|
| 11 Go schemas | `R5_patterns/{concept}/go.jsonc` | Add `detect` regex to each pattern |
| 11 Rust schemas | `R5_patterns/{concept}/rust.jsonc` | Add `emit` template to each pattern |
| New mapping | `R5_patterns/go-to-rust.jsonc` | Reverse transpiler mapping |

## Existing Functions to Reuse

| Function | File | Role |
|----------|------|------|
| `getSubsectionRanges()` | `lib/shared/code-4block.ts:353` | Subsection boundary detection |
| `getBlockLines()` | `lib/shared/code-4block.ts:249` | Block line extraction |
| `blockLineToFile()` | `lib/shared/code-4block.ts:294` | Line number mapping |
| `info()`, `warn()` | `lib/foundation/mod.ts` | Result constructors |
| `SECTION_REGISTRY` | `lib/data/sections.ts:702` | 42 section concept maps |
| `SECTION_ORDER` | `lib/data/sections.ts:710` | Section ordering per block |
| `CONCEPT_ORDER` | `lib/data/concepts.ts:180` | 11 concept IDs in stable order |
| `registerCache()` | `lib/foundation/cache-registry.ts` | Cache lifecycle |

## Verification

**By test layer:**
1. `deno task test:data` — all data layer tests pass (sections, concepts, detectors, errors)
2. `deno task test:logic` — all logic tests pass (grid, concept check, container lines)
3. `deno task test:system` — all system tests pass (full pipeline at every detection layer)
4. `deno task test` — full suite, all tests pass (no regressions from 1219+)

**By detection layer (horizontal):**
5. `deno task test:R50` — all file-level detection tests
6. `deno task test:R25` — all block-level detection tests
7. `deno task test:R10` — all content-level detection tests
8. `deno task test:R05` — all concept-level detection tests

**By handler (vertical):**
9. `deno task test:system:go` — Go handler at all layers
10. `deno task test:system:rust` — Rust handler at all layers

**Concept-specific:**
11. `deno task lint:rust <fixture>` — concept results appear with `concept/` prefix
12. `--check setup:imports` -> imports container concept checks only
13. Type check: `deno check lib/data/concept-detectors.ts lib/shared/concept-check.ts`

**Schema:**
14. Each R5 concept/{lang}.jsonc has both `detect` and `emit`

**Witness system:**
15. `deno test --filter='CWS-R05-070'` — runs witness test for denied concept error
16. G13 gate passes — every error code has at least one witness test
17. `deno task witness` — zero gaps reported

## The Vision: Concept + Container = Portable Code Block

```
Schema (Bereshit's truth)
  |
  +-- R5: detect + emit + container_map + concept_relations
  |     |
  |   Linter: "function_call in imports? DENIED. Warn."
  |   Transformer: "function_call in imports. defer_to: BODY.FreeFunctions. Route."
  |   Transpiler: "function_call — Rust detect -> Go emit. Rust emit <- Go detect."
  |
  +-- R25: can/cannot/defer + concept_map
        |
      Container = address. Concept = content. Together = portable code block.
      Take "function_call in free-functions" -> validate, route, or emit.
      Same schema. Three tools. Bidirectional. Drag and drop.
```

The linter doesn't own this. Bereshit does. The linter is one consumer among three. The schemas are the physics engine. The tools are the game. And the game plays both ways.
