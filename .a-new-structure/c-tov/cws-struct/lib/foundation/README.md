<!--
#!omni document --md
===============================================================================
METADATA
===============================================================================
-->
<!--
:key: B-tov-cws-struct-lib-foundation-README
:title: CWS Struct — Foundation Layer API Reference
:type: Documentation (API Reference)
:status: Active
:version: a-01.00
:created: 2026-02-22
:authors: Deborah Saphar (CPI-SI)
:purpose: Complete API reference for the foundation layer — the contracts that
          every handler, engine module, and CLI command depends on. Seven modules
          totaling ~4,200 lines. Zero documentation until now.
:derives_from: lib/foundation/mod.ts (barrel), lib/foundation/*.ts (modules)
-->

<!--
===============================================================================
HEADER
===============================================================================
-->

# Foundation Layer API Reference

> *"A just weight and balance are the LORD's: all the weights of the bag are his work."* — Proverbs 16:11

The foundation layer defines the contracts that the entire cws-struct tool depends on. Handlers import from here. The engine imports from here. The CLI imports from here. When you change something in `lib/foundation/`, the cascade touches everything.

**Entry point:** `import { ... } from "./lib/foundation/mod.ts"`

---

<!--
===============================================================================
CONTEXT
===============================================================================
-->

## Orientation

### What This Layer Is

The foundation is **everything below the handlers and above the data**. Seven modules providing:

| Module | Lines | Role |
|--------|------:|------|
| [types.ts](#typests) | 313 | Core contracts: LintResult, FormatHandler, CliOptions |
| [health.ts](#healthts) | 549 | Ternary health scoring with Hebrew state resolution |
| [schema-pipeline.ts](#schema-pipelinets) | 253 | Layered schema discovery and caching |
| [schema.ts](#schemats) | 574 | TOML 3-block schema loader (DerivedRules) |
| [code-schema.ts](#code-schemats) | 1,826 | Go/Rust 4-block schema loader + form constraints |
| [config.ts](#configts) | 138 | Tool configuration from deno.jsonc |
| [composition-loader.ts](#composition-loaderts) | 394 | Three-cord composition resolution |

### How It Fits

```
CLI (mod.ts)
  │
  ├─ engine/        ← orchestrates lint/transform/create
  │    │
  │    ├─ foundation/   ← YOU ARE HERE — contracts, scoring, schemas
  │    │
  │    └─ handlers/     ← format-specific (go, rust, toml, json, ...)
  │
  └─ data/           ← compiled schema registries (being built)
```

Every handler implements `FormatHandler`. Every lint check produces `LintResult[]`. Every health computation flows through `computeHealthScore()`. These are the non-negotiable contracts.

### Import Pattern

All foundation exports flow through the barrel (`mod.ts`):

```typescript
// Good — single import path
import { error, warn, info, summarize } from "../foundation/mod.ts";
import type { FormatHandler, LintResult } from "../foundation/mod.ts";

// Avoid — bypasses barrel
import { error } from "../foundation/types.ts";
```

### Prerequisites

| Concept | Where Defined |
|---------|---------------|
| DAR (Detect, Assess, Recover) | Engine layer — drives lint/transform/create |
| 4-block structure | METADATA → SETUP → BODY → CLOSING |
| 3-block structure | METADATA → CONTENT → CLOSING |
| Hebrew states | health.ts — 7 levels from shavar to tov |
| Three-cord architecture | composition-loader.ts — structure + content + index |
| Ternary values | granted / denied / defer |

---

<!--
===============================================================================
CONTENT
===============================================================================
-->

## types.ts

> Core contracts. Change here, cascade everywhere.

### Types

#### `Severity`

```typescript
type Severity = "error" | "warn" | "info";
```

The weight of a finding. Used in every `LintResult` and feeds into health scoring impact weights (error: 2x, warn: 1x, info: 0.25x).

#### `LintPolicy`

```typescript
type LintPolicy = "strict" | "balanced" | "growth";
```

Ternary threshold mapping to CPI-SI's 3 keys:

| Policy | Key | Effect |
|--------|:---:|--------|
| strict | -1 | info→warn, warn→error, error→error |
| balanced | 0 | no change (identity) |
| growth | +1 | error→warn, warn→info, info→info |

Structural checks bypass policy. Only form-aware checks use it.

#### `FixAction`

```typescript
interface FixAction {
  type: "remove" | "insert";
  startLine?: number;   // remove: first line (1-based)
  endLine?: number;     // remove: last line (inclusive)
  afterLine?: number;   // insert: line to insert after
  content?: string[];   // insert: lines to add
}
```

Atomic action within a fix suggestion. The smallest unit of code modification.

#### `FixSuggestion`

```typescript
interface FixSuggestion {
  description: string;
  toml?: string;       // TOML handler: snippet to insert/replace
  location?: string;   // hint: "after [_metadata]", "in _metadata.I1_core"
  actions?: FixAction[]; // Code handlers: ordered atomic actions
}
```

The DAR engine's recovery unit. Self-contained, traceable, composable.

#### `LintResult`

```typescript
interface LintResult {
  file: string;
  severity: Severity;
  rule: string;
  message: string;
  line?: number;        // 1-based
  fix?: FixSuggestion;
}
```

Single finding from a lint check. The universal output type. Every handler produces these. Every output formatter consumes these.

#### `LintSummary`

```typescript
interface LintSummary {
  file: string;
  errors: number;
  warnings: number;
  infos: number;
  results: LintResult[];
  health?: HealthScore;
  pragma?: string;       // "//omni:code --go -library"
}
```

Aggregated results for one file. Built by `summarize()`.

#### `FormatHandler`

```typescript
interface FormatHandler {
  name: string;           // CLI name: "toml", "go", "rust"
  description: string;
  extensions: string[];   // [".toml"], [".go"], [".rs"]
  basenames?: string[];   // [".gitignore", ".editorconfig"]
  maxDepth?: number;      // directory walk depth (default: 3)

  lint(filePath: string): Promise<LintResult[]>;
  computeHealth?(filePath: string, results: LintResult[]): Promise<HealthScore>;
  transform?(filePath: string, opts: TransformOptions): Promise<LintResult[]>;
}
```

**The contract for adding new formats.** Implement `lint()` at minimum. `computeHealth()` and `transform()` are optional. Register in the handler registry and the format is live.

#### `TransformOptions`

```typescript
interface TransformOptions {
  dryRun: boolean;      // preview without writing
  extensions: boolean;  // scaffold extension sections
  force: boolean;       // re-scaffold even if structure exists
  steps: boolean;       // write each phase to .steps/ directory
}
```

#### `CliOptions`

```typescript
interface CliOptions {
  command: "lint" | "transform" | "create" | "transpile" | "formats" | "verify" | "studio" | "help" | "version";
  format?: string;
  targets: string[];
  verbose: boolean;
  errorsOnly: boolean;
  summaryOnly: boolean;
  dryRun: boolean;
  extensions: boolean;
  json: boolean;
  failFast: boolean;
  force: boolean;
  steps: boolean;
  policy: LintPolicy;
  subtype?: string;
  key?: string;
  title?: string;
  purpose?: string;
  logPath?: string;
  check?: string;        // targeted block: "metadata", "setup", "body", "closing"
}
```

### Functions

#### `error()` / `warn()` / `info()`

```typescript
function error(file: string, rule: string, message: string, opts?: ResultOpts | FixSuggestion): LintResult
function warn(file: string, rule: string, message: string, opts?: ResultOpts | FixSuggestion): LintResult
function info(file: string, rule: string, message: string, opts?: ResultOpts | FixSuggestion): LintResult
```

Result constructors. The standard way to create `LintResult` values.

```typescript
// Usage in a handler
results.push(error(file, "STRUCT/metadata-missing", "No METADATA block found"));
results.push(warn(file, "SETUP/order", "Imports should come before constants", { line: 42 }));
results.push(info(file, "STYLE/spacing", "Consider blank line before section", {
  description: "Add blank line",
  actions: [{ type: "insert", afterLine: 15, content: [""] }]
}));
```

#### `policySeverity()`

```typescript
function policySeverity(base: Severity, policy: LintPolicy): Severity
```

Map a base severity through the ternary policy lens. Structural checks should NOT call this — only form-aware checks.

#### `setGlobalPolicy()` / `getGlobalPolicy()`

```typescript
function setGlobalPolicy(p: LintPolicy): void
function getGlobalPolicy(): LintPolicy
```

Module-level state. CLI sets it before dispatching. Handlers read it in form-aware checks. One policy per session.

#### `summarize()`

```typescript
function summarize(file: string, results: LintResult[], health?: HealthScore): LintSummary
```

Single-pass tally of results into a summary. Called after `lint()` and optional `computeHealth()`.

---

## health.ts

> Ternary health scoring. The score is measured, not assigned.

### Core Concept

Every observation is an `AtomicAction` with direction (-1, 0, +1) and impact weight. Direction tells you WHICH WAY (misaligned → neutral → aligned). Impact tells you HOW MUCH it matters.

Three-level architecture:

| Level | Scoring | Purpose |
|-------|---------|---------|
| **Container** | Linear weighted | Drill-down: exactly what's wrong in each section |
| **Block** | Logarithmic positional | Foundation checks get full weight, refinement diminishes |
| **File** | Per-block averaging | Each block speaks with equal voice |

Score range: -100 (fully misaligned) to +100 (fully aligned). 0 = yashar (neutral baseline).

### Types

#### `HebrewState`

```typescript
type HebrewState =
  | "broken"   // [-100, -67] shavar
  | "wanting"  // [-66, -34]  chaser
  | "lacking"  // [-33, -1]   ratsah
  | "even"     // [0, 0]      yashar
  | "sound"    // [1, 33]     tamim
  | "whole"    // [34, 66]    shalem
  | "perfect"; // [67, 100]   tov
```

Seven states from `bereshit-base-algorithms.adoc`. Each maps to a direction and emoji.

#### `HebrewDirection`

```typescript
type HebrewDirection = -1 | 0 | 1;
```

Derived from state: broken/wanting/lacking = -1, even = 0, sound/whole/perfect = +1.

#### `AtomicAction`

```typescript
interface AtomicAction {
  check: string;         // "I1_core.key", "SETUP/order"
  container: string;     // "I1_core", "SETUP"
  block: string;         // "metadata", "setup", "body", "closing"
  direction: -1 | 0 | 1;
  impact?: Severity;     // default: "warn"
  reason?: string;       // why (for non-neutral)
  layer?: 0 | 1 | 2 | 3; // 0=whole, 1=structure, 2=container, 3=content
}
```

The indivisible unit. Every health score traces back to these. The algorithm normalizes; it doesn't invent.

#### `ContainerScore`

```typescript
interface ContainerScore {
  section: string;    // "I1_core", "X1_policy"
  block: string;
  total: number;
  aligned: number;
  neutral: number;
  misaligned: number;
  score: number;       // -100 to +100
  actions: AtomicAction[];
}
```

Per-section score. Linear weighted. Used for drill-down reporting.

#### `BlockScore`

```typescript
interface BlockScore {
  block: string;                // "metadata", "setup", "body", "closing"
  containers: ContainerScore[];
  score: number;               // -100 to +100, log-weighted
}
```

Per-block score. Logarithmic positional weighting within the block — foundation checks get full weight.

#### `HealthScore`

```typescript
interface HealthScore {
  total: number;            // -100 to +100
  level: HebrewState;       // resolved state
  emoji: string;            // visual indicator
  direction: HebrewDirection; // -1/0/+1
  hardpoint: -100 | -50 | 0 | 50 | 100;  // NormalizeBase50
  blocks: BlockScore[];
  totalActions: number;
  alignedCount: number;
  neutralCount: number;
  misalignedCount: number;
}
```

File-level health. The true score. Carries everything: numeric score, resolved Hebrew state, visual emoji, direction, R[50] hardpoint, and full block breakdown.

### Functions

#### `IMPACT_WEIGHT` / `setImpactWeights()`

```typescript
let IMPACT_WEIGHT: Record<string, number> = { error: 2.0, warn: 1.0, info: 0.25 };
function setImpactWeights(weights: Record<string, number>): void
```

Impact weights are symmetric — passing a critical check earns the same weight as failing it costs. Override from tool config before dispatching to handlers.

#### `trueToLevel()`

```typescript
function trueToLevel(score: number): HebrewState
```

Score → Hebrew state. Implements ALG-001 TrueToLevel. Clamps to [-100, +100]. Linear scan, constant time.

#### `levelToEmoji()` / `levelToDirection()`

```typescript
function levelToEmoji(level: HebrewState): string     // "🔴" through "✅"
function levelToDirection(level: HebrewState): HebrewDirection
```

State → visual indicator. State → ternary direction.

#### `normalizeBase50()`

```typescript
function normalizeBase50(score: number): -100 | -50 | 0 | 50 | 100
```

Quantize to 5 hardpoints. R[50] resolution — the coarsest detection granularity.

#### `computeContainerScore()`

```typescript
function computeContainerScore(section: string, block: string, actions: AtomicAction[]): ContainerScore
```

Ternary scoring: `direction x impact_weight`. Neutral actions tracked but don't affect score — prevents cascade inflation. Empty container = 0 (yashar).

#### `computeBlockScore()`

```typescript
function computeBlockScore(block: string, containers: ContainerScore[]): BlockScore
```

Collects non-neutral actions, sorts by severity (errors first, fails before passes), applies logarithmic positional weighting: `multiplier(i) = ln(n-i+1) / ln(n+1)`. Foundation checks within the block get full weight.

#### `computeHealthScore()`

```typescript
function computeHealthScore(blocks: BlockScore[]): HealthScore
```

Per-block averaging. Each block contributes equally to file score. Blocks with only neutral actions don't participate (can't be assessed). Resolves final score to HebrewState + emoji + direction + hardpoint.

---

## schema-pipeline.ts

> Layered schema discovery. Schemas are game data, the linter is the engine.

### Core Concept

The pipeline resolves schema files through layered sources in priority order. First source that provides the schema wins. Errors mean "try next source," not "abort."

Resolution order for the default pipeline:
1. `CWS_STRUCT_SCHEMA_DIR` env override
2. `.cws-struct/schemas/` in CWD (project-local)
3. Default relative to source/binary

### Types

#### `SchemaSource`

```typescript
interface SchemaSource {
  readonly name: string;
  getText(schemaId: string): Promise<string>;
}
```

Interface for raw schema text providers. Pipeline tries sources in priority order.

### Classes

#### `FileSource`

```typescript
class FileSource implements SchemaSource {
  constructor(baseDir: string, name?: string);
  getText(schemaId: string): Promise<string>;
}
```

Filesystem-backed source. Schema IDs resolve as relative paths under `baseDir`. Works identically in dev mode and compiled binaries (Deno's virtual FS).

#### `SchemaPipeline`

```typescript
class SchemaPipeline {
  addSource(source: SchemaSource): void;
  getText(schemaId: string): Promise<string>;   // cached
  listSources(): readonly string[];
  clear(): void;
}
```

Unified discovery with caching. Raw text cached by schema ID. Parsed-rule caching is the caller's concern.

### Functions

#### `getDefaultPipeline()`

```typescript
function getDefaultPipeline(): SchemaPipeline
```

Singleton, lazy-initialized. Creates the 3-source default pipeline. All loaders (`loadRules`, `loadCodeRules`, `loadComposition`) call this.

#### `clearPipeline()`

```typescript
function clearPipeline(): void
```

Reset singleton. Use in tests when changing `CWS_STRUCT_SCHEMA_DIR` or when schema files have changed.

---

## schema.ts

> TOML 3-block schema loader. The schema is the single source of truth.

### Core Concept

Loads `toml-3block-schema.jsonc`, parses the `validation_contract`, compiles it into `DerivedRules` that the TOML handler consumes directly. Separation: `parseTomlSchema()` is pure computation, `loadRules()` handles I/O and caching.

### Types (Selection)

#### `DerivedRules`

The primary output. Contains everything the TOML handler needs:

```typescript
interface DerivedRules {
  contract: ValidationContract;         // raw contract for reference

  metadataKeyPattern: RegExp;           // compiled patterns
  closingKeyPattern: RegExp;

  allKnownSections: Set<string>;        // membership testing
  allKnownClosing: Set<string>;

  requiredIdentity: string[];           // stripped arrays (no table prefixes)
  extensionIdentity: string[];
  requiredContext: string[];
  extensionContext: string[];
  requiredClosing: string[];
  definedClosing: string[];
  templateClosing: string[];
  requiredContent: string[];
  definedCc: string[];
  definedCo: string[];
  requiredValidation: string[];
  extensionZones: string[];

  metadataFields: Record<string, FieldRequirement>;  // per-section field requirements
  contentFields: Record<string, FieldRequirement>;
  closingFields: Record<string, FieldRequirement>;

  pragmaTaxonomy: PragmaTaxonomy;       // types, derivations, forms, layouts
}
```

#### `PragmaTaxonomy`

Schema-declared pragma options — what types, derivations, and forms are valid.

```typescript
interface PragmaTaxonomy {
  validTypes: string[];          // ["template", "data", "code"]
  baseType: string;              // "data"
  format: string;                // "toml"
  knownDerivations: Set<string>; // {"compiler", "cargo"}
  knownForms: Set<string>;       // {"library", "executable", "test"}
  allKnownArgs: Set<string>;     // union of derivations + forms
  derivationLayouts: Record<string, DerivationLayout>;
}
```

### Functions

#### `parseTomlSchema()`

```typescript
function parseTomlSchema(jsonText: string): DerivedRules
```

Pure computation. Takes raw JSONC text, returns compiled rules. No I/O, no caching. Use this for testing or when you have schema text from a non-file source.

#### `loadRules()`

```typescript
function loadRules(): Promise<DerivedRules>
```

I/O wrapper. Resolves schema through the pipeline, delegates to `parseTomlSchema()`, caches result. Subsequent calls return cache.

#### `clearCache()`

```typescript
function clearCache(): void
```

Clear the cached rules. Use in tests or after schema edits.

---

## code-schema.ts

> Go/Rust 4-block schema loader. 1,826 lines. The largest foundation module.

### Core Concept

Loads language-specific 4-block schemas (e.g., `go-4block-schema.jsonc`) and compiles them into `Code4BlockRules` that Go and Rust handlers consume. Also loads form constraint schemas and composition bridge rules.

Two schema dimensions:
- **Format schema** (per-language): SETUP subsections, BODY subtypes, placement rules, closing zones
- **Form schema** (per-form): section requirements, ordering, reserved sections

### Key Types

#### `Code4BlockRules`

```typescript
interface Code4BlockRules {
  setupSubsections: SchemaSubsectionDef[];
  bodySubtypes: Record<string, SchemaBodySubtype>;
  closingZones: SchemaClosingZone[];
  closingDocRequirements: SchemaClosingDocReq[];
  closingData: SchemaClosingData;
  identity: SchemaIdentitySyntax;
  placementMaps: PlacementMaps;
  fillDefaults: SchemaFillDefaults;
  closingDefaults: SchemaClosingDefaults;
  fillContent: SchemaFillContent | null;
  subtypes: SchemaSubtypeDef[];
  docComments: SchemaDocComment[];
  format: CodeFormat;
  fieldRequirements: Record<string, SchemaFieldRequirement>;
}
```

Everything a code handler needs. SETUP order, BODY organization, CLOSING zones, identity syntax, fill defaults, and field requirements.

#### `FormConstraints`

```typescript
interface FormConstraints {
  formName: string;
  blockConstraints: Record<string, FormContainerConstraints>;
  ordering?: Record<string, string>;
}
```

Per-form section requirements — which sections are required, available, or reserved for a given form (bare-bone, module, library, executable, demo-test).

### Functions

#### `parseCodeSchema()` / `loadCodeRules()`

```typescript
function parseCodeSchema(jsonText: string): Code4BlockRules   // pure
function loadCodeRules(format: string): Promise<Code4BlockRules>  // I/O + cache
```

Same pattern as TOML schema: pure parse + I/O wrapper. Format is the language identifier (e.g., `"go"`, `"rust"`).

#### `clearCodeCache()`

```typescript
function clearCodeCache(): void
```

Clear all cached code rules.

#### `parseFormSchema()` / `loadFormConstraints()`

```typescript
function parseFormSchema(jsonText: string): FormConstraints   // pure
function loadFormConstraints(format: string, form: string): Promise<FormConstraints> // I/O + cache
```

Load form-specific constraints. Used by form-aware lint checks.

#### `clearFormCache()`

```typescript
function clearFormCache(): void
```

Clear all cached form constraints.

#### `compositionToFormConstraints()` / `loadCompositionFormConstraints()`

```typescript
function compositionToFormConstraints(target: CompositionTarget): FormConstraints  // pure
function loadCompositionFormConstraints(format: string, form: string): Promise<FormConstraints> // I/O
```

Bridge between the composition system and form constraints. Derives `FormConstraints` from `CompositionTarget.active_sections`. This is the path forward — composition targets replace standalone form schemas.

---

## config.ts

> Tool configuration. Tripwire defaults with config-driven overrides.

### Types

#### `ToolConfig`

```typescript
interface ToolConfig {
  version: string;                    // from deno.jsonc "version"
  concurrency: number;                // parallel file processing (default: 8)
  maxFiles: number;                   // max discovered files (default: 10,000)
  maxFileSize: number;                // max file size in bytes (default: 1MB)
  excludedDirs: string[];             // [".git", "node_modules", ...]
  impactWeights: Record<string, number>; // { error: 2.0, warn: 1.0, info: 0.25 }
}
```

### Functions

#### `loadConfig()`

```typescript
function loadConfig(configPath?: string): Promise<ToolConfig>
```

Loads from `deno.jsonc` `_cwsStruct` section. Each field individually falls back to defaults — partial configs are safe. If `deno.jsonc` is missing, all defaults apply.

---

## composition-loader.ts

> The linker of the three-cord architecture.

### Core Concept

Given a format+form (e.g., "rust-module"), resolves all schemas needed:
- **Structure schemas**: format-agnostic block shapes (whole + molded + parts)
- **Content schemas**: language-specific fill (whole + molded + parts)
- **Index targets**: the recipe that maps structure to content

Structure stays constant across languages. Content changes per language. The composition target maps the two.

### Types

#### `CompositionTarget`

```typescript
interface CompositionTarget {
  pragma: string;              // "//omni:code --rust -module"
  format: string;              // "rust"
  form: string;                // "module"
  block_pattern: string;       // "4-block"
  composition: {
    structure: CompositionSide;
    content: CompositionSide;
  };
  active_sections: CompositionActiveSections;
  resolution_order: string[];
}
```

#### `CompositionSide`

```typescript
interface CompositionSide {
  whole: string;               // "structure/whole/4-block.jsonc"
  molded: string;              // "structure/molded/module.jsonc"
  parts: CompositionParts;     // per-block schema paths
}
```

#### `ResolvedComposition`

```typescript
interface ResolvedComposition {
  target: CompositionTarget;
  structure: { whole: LoadedSchema; molded: LoadedSchema; parts: Map<string, LoadedSchema> };
  content: { whole: LoadedSchema; molded: LoadedSchema; parts: Map<string, LoadedSchema> };
}
```

Fully resolved — all schemas loaded and ready for the transpiler.

### Functions

#### `parseCompositionTarget()`

```typescript
function parseCompositionTarget(jsonText: string): CompositionTarget
```

Pure parse. Validates required fields.

#### `loadComposition()`

```typescript
function loadComposition(format: string, form: string): Promise<ResolvedComposition>
```

Main entry point. Loads target recipe, then loads all referenced structure and content schemas in parallel.

```typescript
const composed = await loadComposition("rust", "module");
// composed.target.pragma === "//omni:code --rust -module"
// composed.structure.whole contains 4-block definition
// composed.content.molded contains Rust module expectations
```

#### `loadCompositionCached()`

```typescript
function loadCompositionCached(format: string, form: string): Promise<ResolvedComposition>
```

Same as `loadComposition()` but caches by `format:form` key.

#### `listCompositionTargets()`

```typescript
function listCompositionTargets(): Promise<Array<{ format: string; form: string }>>
```

Scan `index/manifest.jsonc` for available targets.

#### `clearCompositionCache()`

```typescript
function clearCompositionCache(): void
```

Clear the composition cache.

---

<!--
===============================================================================
FOOTER
===============================================================================
-->

## Design Patterns

### Pure Parse + I/O Wrapper

Every schema loader follows the same two-function pattern:

| Pure Function | I/O Wrapper | Module |
|---------------|-------------|--------|
| `parseTomlSchema()` | `loadRules()` | schema.ts |
| `parseCodeSchema()` | `loadCodeRules()` | code-schema.ts |
| `parseFormSchema()` | `loadFormConstraints()` | code-schema.ts |
| `parseCompositionTarget()` | `loadComposition()` | composition-loader.ts |

Pure functions take text, return typed objects. No side effects, no caching. Testable in isolation.

I/O wrappers resolve through the schema pipeline, delegate to the pure function, cache results.

### Module-Level Singletons

Three module-level singletons exist:

| Singleton | Setter | Getter | Clearer |
|-----------|--------|--------|---------|
| Global policy | `setGlobalPolicy()` | `getGlobalPolicy()` | (set to "balanced") |
| Impact weights | `setImpactWeights()` | `IMPACT_WEIGHT` | (set to defaults) |
| Schema pipeline | (lazy) | `getDefaultPipeline()` | `clearPipeline()` |

Set before dispatching to handlers. Clear in tests.

### Cache Hierarchy

```
SchemaPipeline         ← raw JSONC text cache (by schema ID)
  └── loadRules()      ← parsed DerivedRules cache (module-level)
  └── loadCodeRules()  ← parsed Code4BlockRules cache (by format)
  └── loadFormConstraints() ← parsed FormConstraints cache (by format:form)
  └── loadCompositionCached() ← ResolvedComposition cache (by format:form)
```

`clearPipeline()` clears raw text only. Each loader has its own `clear*()` for parsed rules. For full reset: clear all caches + clear pipeline.

## Related Documents

| Document | Relationship |
|----------|-------------|
| [lib/data/README.md](../data/README.md) | Data layer — compiled schema registries |
| [lib/shared/types.ts](../shared/types.ts) | Shared 4-block type extensions |
| [b-word/L1-omnicode/ladder/schemas/](../../../../b-word/L1-omnicode/ladder/schemas/) | Schema source files |

---

> *"Except the LORD build the house, they labour in vain that build it."* — Psalm 127:1

The foundation layer IS the house's foundation. Seven modules. ~4,200 lines. Every handler stands on these contracts. Every score traces through these algorithms. Every schema resolves through this pipeline.

The contracts are the boundaries. The algorithms are the fill. Boundaries before content.
