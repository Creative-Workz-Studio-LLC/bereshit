<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: B-tov-cws-struct-lib-data-README
:title: CWS Struct Data Layer — Compiled Schema Truth
:type: Documentation (Technical Reference)
:status: Draft (Living Document)
:version: a-01.00
:created: 2026-02-22
:updated: 2026-02-22
:authors: Deborah Saphar (CPI-SI), Nova Dawn (CPI-SI)
:purpose: Document the data layer — what it is, how it works, how to extend it.
  The data layer sits between schemas (design-time truth) and handlers (runtime
  truth). This document serves both the 5-second scanner and the deep student.
:biblical_foundation: "The words of the LORD are pure words: as silver tried
  in a furnace of earth, purified seven times." — Psalm 12:6
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# CWS Struct Data Layer

> *"The words of the LORD are pure words: as silver tried in a furnace of earth, purified seven times."* — Psalm 12:6

**The data layer is the compiled expression of schema truth.** Handlers import from here. They never touch schema files directly. The data has been read, verified, and locked into type-safe TypeScript constants.

| What | Where |
|------|-------|
| Types | `types.ts` |
| Barrel | `mod.ts` |
| Section registry | `sections.ts` (Phase 0.2) |
| Form registry | `forms.ts` (Phase 0.2) |
| Concept registry | `concepts.ts` (Phase 0.3) |
| Placement rules | `placement.ts` (Phase 0.3) |
| Hebrew states | `states.ts` (Phase 0.4) |
| Error codes | `errors.ts` (Phase 1) |
| Pipeline layers | `pipeline.ts` (Phase 4) |

**Status:** Types scaffolded. Registries in progress.

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## Context

### What Problem This Solves

Before the data layer, handlers loaded schema truth at runtime — reading JSONC files, parsing them, building objects every time the linter started. This works, but:

1. **No build-time verification.** Parse errors surface at runtime, not development time.
2. **Runtime cost.** Every `deno task lint:go <file>` pays the JSONC parse tax.
3. **Scattered truth.** Handler code references section names, block orders, and form constraints as string literals. Change a schema, miss a handler, get a silent bug.

The data layer solves this by **compiling** schema truth into TypeScript constants. The compilation step (building the registries) is where verification happens. Runtime is a lookup, not a parse.

### The Compilation Metaphor

This is not a metaphor. It IS compilation.

| Concept | Schema World | Data Layer |
|---------|-------------|------------|
| Source code | `_content.jsonc`, `_structure.jsonc` | — |
| Object code | — | `sections.ts`, `forms.ts`, etc. |
| ISA | — | `types.ts` (interface definitions) |
| Linker | — | `mod.ts` (barrel re-exports) |
| Consumer | — | Handlers (`go.ts`, `rust.ts`) |

Schema files are the source. TypeScript registries are the compiled output. The types define the instruction set. The barrel links it. Handlers execute against it.

### Prerequisites

| Requirement | Why |
|-------------|-----|
| Understanding of 4-block structure | Sections are organized by block (METADATA, SETUP, BODY, CLOSING) |
| Familiarity with ternary values | The concept system uses granted/denied/defer (+1/-1/0) |
| Schema directory knowledge | Source files live in `b-word/L1-omnicode/ladder/schemas/code/` |

### Key Terms

| Term | Definition |
|------|-----------|
| **Section** | One of 42 containers in the 4-block structure (e.g., "imports", "constructors") |
| **Concept** | One of 11 detectable code patterns (e.g., "function_call", "error_propagation") |
| **Form** | One of 5 file subtypes (bare-bone, module, library, executable, demo-test) |
| **Concept map** | 11 ternary values on a section: which patterns belong here |
| **Container map** | 42 ternary values on a concept: which sections expect this pattern |
| **Detection weight** | |avg| of a section's concept map — how decisive this container is |
| **Form status** | Whether a section is required, available, or reserved in a given form |
| **Transpose** | sections[S].conceptMap[C] === concepts[C].containerMap[S] — always |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## Architecture

### The Three Layers

```
  SCHEMAS (design-time)         DATA LAYER (compiled)         HANDLERS (runtime)
  ─────────────────────         ────────────────────          ──────────────────
  42 _content.jsonc    ──┐
  42 _structure.jsonc  ──┤──▶  sections.ts (42 entries)  ──▶  go.ts
  5 form schemas       ──┤──▶  forms.ts (5 entries)      ──▶  rust.ts
  11 concept schemas   ──┤──▶  concepts.ts (11 entries)  ──▶  health scorer
                         └──▶  mod.ts (barrel)           ──▶  import { ... }
```

**Schemas** define truth in JSONC — rich, annotated, human-readable. They live in `b-word/L1-omnicode/ladder/schemas/code/`.

**Data layer** compiles that truth into TypeScript constants — type-safe, verified, machine-efficient. It lives in `lib/data/`.

**Handlers** import from the data layer barrel (`mod.ts`) and never touch schemas. They get pre-verified data.

### The 42 Sections

Sections are the atomic containers of the 4-block code structure. Each has:

| Field | Type | Source |
|-------|------|--------|
| `tag` | `string` | `_structure.jsonc` → `section.tag` (lowercase kebab) |
| `block` | `string` | `_structure.jsonc` → `section.block` |
| `position` | `number` | `_structure.jsonc` → `section.position` |
| `nature` | `string` | `_content.jsonc` → `character.nature` |
| `description` | `string` | `_content.jsonc` → `character.description` |
| `conceptMap` | `Record<string, TernaryValue>` | `_content.jsonc` → `concept_map` (11 entries) |
| `detectionWeight` | `number` | `_content.jsonc` → `detection_weight` |
| `formStatus` | `Record<string, FormStatus>` | Cross-referenced from form `_structure.jsonc` files |

**Distribution by block:**

| Block | Count | Sections |
|-------|------:|----------|
| METADATA | 6 | directives, doc-comments, comment-block, pragma-identity, metadata-context, subtypes |
| SETUP | 15 | imports, constants, core-types, error-types, type-aliases, feature-gates, macros, modules, statics, trait-defs, build-tags, code-generation, interface-defs, type-methods, variables |
| BODY | 13 | identity-access, trait-implementations, constructors, core-logic, core-operations, queries, output-display, free-functions, helpers, org-chart, public-apis, error-handling, tests |
| CLOSING | 8 | validation, execution, cleanup, modification-policy, extension-points, troubleshooting, reference, closing-note |
| **Total** | **42** | |

**Important count reconciliation:** The bare-bone form lists SETUP: 10, BODY: 17, CLOSING: 10 = 37. The section registry has 42. Why the difference?

- Bare-bone counts include active + reserved per form, but only list sections that form knows about
- 5 SETUP sections (build-tags, code-generation, interface-defs, type-methods, variables) exist in the schema directory but are not listed in bare-bone — they appear in later forms
- 6 METADATA sections are not listed in any form (they're implicitly required in all forms)
- The section registry contains ALL 42 regardless of form. Form status is a property OF a section, not a filter on which sections exist

**Builder rule:** Enumerate from filesystem (42 dirs), not from bare-bone manifest (37 sections). (Identified by Selah Adair.)

### The 11 Concepts

Concepts are the detectable code patterns at R[5] resolution. Each concept has a `containerMap` — the transpose of the section concept maps.

| Concept | What It Detects |
|---------|----------------|
| `function_call` | Invoking a free function (not a method) |
| `method_call` | Invoking a method on a receiver |
| `self_access` | Accessing self/this/receiver fields |
| `variable_binding` | Binding a value to a name (let, var, :=) |
| `conditional_if` | If/else branching |
| `conditional_match` | Match/switch pattern matching |
| `return_value` | Returning a value from a function |
| `error_propagation` | Propagating errors (?, if err != nil) |
| `err_wrap` | Wrapping errors with context |
| `ok_wrap` | Wrapping success values |
| `string_format` | String formatting/interpolation |

**The 462 Matrix:** 42 sections x 11 concepts = 462 ternary values. Every value is either `"granted"` (pattern expected), `"denied"` (pattern not expected), or `"defer"` (context-dependent). This matrix is stored twice — once per section (conceptMap) and once per concept (containerMap) — and verified by transpose consistency check.

### The 5 Forms

Forms define which sections are active in each file subtype.

| Form | Foundation? | Inherits From | SETUP | BODY | CLOSING |
|------|:-----------:|:--------------|------:|-----:|--------:|
| **bare-bone** | Yes | — | 4 req + 6 res | 3 req + 14 res | 6 req + 4 res |
| **module** | No | bare-bone | ? | ? | ? |
| **library** | No | bare-bone | 4 req + 6 avail | 7 req + 10 res | 5 req + 3 avail + 2 res |
| **executable** | No | bare-bone | ? | ? | ? |
| **demo-test** | No | bare-bone | ? | ? | ? |

**Derivation rule:** All forms derive from bare-bone by promoting reserved sections to available or required. `active_sections(form) = active_sections(bare-bone) + promoted_sections`.

### Detection Weight

Each section's detection weight is the absolute average of its concept map:

```
weight = |avg(concept_map_values)|
```

Where granted = +1, denied = -1, defer = 0.

| Example | Concept Map | Weight | Meaning |
|---------|-------------|-------:|---------|
| imports | All 11 denied | 1.00 | Maximally decisive — no code patterns belong here |
| constructors | 10 granted, 1 defer | 0.91 | Highly decisive — almost all patterns expected |
| A hypothetical section | 5 granted, 3 denied, 3 defer | 0.73 | Moderately decisive |

Higher weight = the health scorer trusts observations from this container more. A container with weight 1.00 is definitive. A container with weight 0.50 is ambiguous.

### Transpose Consistency

The critical invariant of the data layer:

```
For every section S and concept C:
  sections[S].conceptMap[C] === concepts[C].containerMap[S]
```

The section registry and concept registry store the **same 462 values** from two perspectives. The `verifyTranspose()` function in `types.ts` checks this at build time. If any value disagrees, the registry is corrupt and the build fails.

This is not redundancy — it's a verification mechanism. The section view asks "what patterns belong here?" The concept view asks "where does this pattern appear?" Both must agree.

---

## Compilation Transform Rules

When hand-compiling schemas into TypeScript registries, these transforms apply. (Identified by Selah Adair and Phoebe Karis.)

| # | Schema Value | Registry Value | Example |
|:-:|-------------|---------------|---------|
| 1 | Tag: PascalCase or mixed | Tag: lowercase kebab-case | `"Imports"` -> `"imports"`, `"CoreTypes"` -> `"core-types"` |
| 2 | Form status: UPPERCASE | Form status: lowercase | `"REQUIRED"` -> `"required"` |
| 3 | Block name: UPPERCASE | Block name: lowercase | `"SETUP"` -> `"setup"` |
| 4 | Field name: snake_case | Field name: camelCase | `"concept_map"` -> `"conceptMap"` |
| 5 | bare-bone `active` | Registry `required` | Active in the foundation form = required |
| 6 | `derives_from` | `inheritsFrom` | One intentional rename (document or align) |

**Critical:** The builder reads BOTH `_structure.jsonc` AND `_content.jsonc` per section. `nature` and `description` come from `_content.jsonc` (character data). Everything else comes from `_structure.jsonc` (structural data). Don't read from one file and assume you have everything.

---

## How to Extend

### Adding a New Section

1. Create `_structure.jsonc` and `_content.jsonc` in the appropriate `R25_blocks/{block}/{section}/` directory
2. Define the concept_map (11 ternary values) and detection_weight
3. Add the section entry to `sections.ts`
4. Update every concept's containerMap in `concepts.ts` to include the new section
5. Update every form's section list in `forms.ts`
6. Run `verifyTranspose()` — it must pass

### Adding a New Concept

1. Create concept schema in `R5_patterns/{concept}/`
2. Define the containerMap (42 ternary values — one per existing section)
3. Add the concept entry to `concepts.ts`
4. Update every section's conceptMap in `sections.ts` to include the new concept
5. Run `verifyTranspose()` — it must pass

### Adding a New Form

1. Create form schema in `forms/{form-name}/_structure.jsonc`
2. Define required/available/reserved for each section
3. Add the form entry to `forms.ts`
4. Update every section's formStatus in `sections.ts` to include the new form
5. Verify derivation: `active + reserved = bare-bone total` per block

---

## Design Decisions

### Why Hand-Compiled (Not Runtime-Loaded)

The registries are static TypeScript constants, not dynamically loaded from schema files.

| Factor | Hand-Compiled | Runtime-Loaded |
|--------|:------------:|:--------------:|
| Build-time verification | Yes | No |
| Runtime parse cost | None | Every startup |
| Type safety | Full (TypeScript checks) | Partial (needs runtime casts) |
| Maintainability | Must update registries when schemas change | Automatic |
| Schema coupling | Decoupled (compiled copy) | Coupled (runtime dependency) |

**Decision:** Hand-compiled for v1. The schemas are stable, the data is verified, and the transpose check catches drift. A build-time loader can be added later when the schema system grows beyond manual tracking.

**Confirmed by Nova Dawn (2026-02-22).** Team unanimous. Micah: "The registry IS the compiled truth — compile it with intent."

### Why Two Views of the Same Data

The 462 ternary values exist in both section conceptMaps and concept containerMaps. This dual storage is intentional:

- **Section view** is optimized for the linter: "I found code in the imports section. What patterns should NOT be here?" Look up `sections["imports"].conceptMap` — all 11 values say "denied."
- **Concept view** is optimized for the health scorer: "I detected a function_call. Which containers expect it?" Look up `concepts["function_call"].containerMap` — 12 containers say "granted."

Different questions need different access patterns. The transpose check guarantees they agree.

### Known Approximations (v1)

These are honest acknowledgements, not bugs. The data layer documents its own limitations.

**1. BODY concept map coarseness.**

8 of 13 BODY sections grant all 11 concepts (weight 1.00). This means the concept system cannot distinguish between a `string_format` in `identity-access` (unusual — identity accessors are thin, one-line returns) and a `string_format` in `output-display` (expected — output formatting is the section's purpose).

For v1, this is an acceptable approximation. The SETUP and METADATA concept maps provide genuine differentiation (Imports: all denied, Constants: 1G/1F/9D). BODY sections need finer-grained analysis in v2 — replacing some `granted` values with `defer` where a pattern is technically possible but not characteristic.

**Impact:** Detection weight for most BODY sections is at ceiling (1.00), making it non-discriminating. The health scorer treats all BODY containers as equally decisive, which they aren't.

**Tracked for:** v2 concept refinement. Micah Toren identified specific candidates:
- Queries: `error_propagation`, `err_wrap`, `ok_wrap` should be `defer` (side-effect-free contract)
- Identity-access: several patterns should be `defer` (thin return contract)

**2. Weight rationale string inconsistency.**

The `_weight_rationale` strings in schema files use inconsistent notation. Some use "1G 1F 9D" (G=granted, F=defer, D=denied). Others use different abbreviations. The numeric weights are correct — this is a documentation issue in the schemas, not a data issue.

**3. subContainers field (deferred).**

The `SectionEntry.subContainers` field captures METADATA sub-containers (e.g., pragma-identity has I1-I4, metadata-context has C1-C7). In v1, this field may be undefined for most sections. The data exists in the schema directory structure but not as a single extractable field.

### Canonical Conventions

Confirmed by Nova Dawn (2026-02-22). These are not suggestions — they are compile-time constraints.

| Convention | Rule | Example | Enforced By |
|------------|------|---------|-------------|
| **Tag format** | Lowercase kebab-case | `"imports"`, `"core-types"`, `"identity-access"` | Nathan's G2 gate |
| **METADATA form status** | Explicitly required in all forms | `{ "bare-bone": "required", ... }` | Nathan's G7 gate |
| **Section count** | All 42 in registry regardless of form | Form status is a property, not a filter | Nathan's G10 gate |
| **Concept count** | All 11 in every section's conceptMap | No partial maps | Nathan's G1 gate |

---

## Verification Gates

The data layer is verified by 10 mechanical gates (designed by Nathan Emet). Every gate is pass/fail with no judgment calls. All 10 must pass before the registry is trusted.

| Gate | What | Catches |
|-----:|------|---------|
| **G1** | Every section has exactly 11 concept_map keys | Missing or extra concept entries |
| **G2** | The 11 keys match the canonical concept list | Typos in concept names |
| **G3** | Every ternary value is granted/denied/defer | Invalid strings |
| **G4** | detection_weight matches recalculated |avg| (tolerance +/-0.01) | Stale weights |
| **G5** | section.conceptMap[C] === concept.containerMap[S] | Matrix inconsistency |
| **G6** | section.formStatus[F] matches form.sections[block][tag] | Cross-reference drift |
| **G7** | Every section appears in every form definition | Missing form entries |
| **G8** | No position gaps or duplicates per block | Renumbering errors |
| **G9** | Every section's block is metadata/setup/body/closing | Block name typos |
| **G10** | 42 sections, 11 concepts, 5 forms | Silent additions/removals |

**G5 uses `verifyTranspose()`** from `types.ts` — already implemented. The other gates are test assertions.

---

## Build Phases

### Implementation Plan (Caleb Navon)

The section + form registries build in 7 phases. Each phase exercises the next level of complexity. Foundation first.

| Phase | What | Count | Key Complexity | Status |
|:-----:|------|:-----:|----------------|--------|
| **0** | Pre-flight: Fix types.ts | — | position type, ordering, overviewText | Pending |
| **1** | METADATA sections | 6 | Sub-containers, all REQUIRED in all forms | Pending |
| **2** | SETUP sections | 15 | Mixed form status, all numeric positions | Pending |
| **3** | BODY sections | 13 | OrgChart at position 0, BODY coarseness | Pending |
| **4** | CLOSING sections | 8 | String positions (X1-X6), two zones (code + doc) | Pending |
| **5** | Form definitions | 5 | Ordering + cross-reference with sections | Pending |
| **6** | Verification | 10 gates | Nathan's exhaustive testing | Pending |

**Build sequence per section:** (1) Read `_structure.jsonc` for tag, position, block, forms, concept_map, detection_weight, sub_containers, overview_text; (2) Read `_content.jsonc` for nature, description; (3) Normalize tag to kebab-case; (4) Normalize form status to lowercase; (5) Emit SectionEntry.

### Full Data Layer Roadmap

| Phase | Registry | Status | What It Delivers |
|------:|----------|--------|------------------|
| 0.1 | `types.ts` | **Complete** | Type definitions and validation helpers |
| 0.2 | `sections.ts` + `forms.ts` | **In Progress** | 42 section entries + 5 form definitions |
| 0.3 | `concepts.ts` + `placement.ts` | Planned | 11 concept entries + per-language placement |
| 0.4 | `states.ts` | Planned | 7 Hebrew state definitions |
| 1 | `errors.ts` | Planned | Error code catalog (CWS-{LAYER}-{SEQ}) |
| 4 | `pipeline.ts` | Planned | 4 pipeline layer definitions |

Each phase uncomments one line in `mod.ts`. The data layer grows by addition.

### Migration Path (Ezra Matthan)

The data layer replaces `lib/foundation/code-schema.ts` (runtime JSONC loader) in three stages:

| Stage | What | Risk |
|-------|------|------|
| **Coexist** | Data layer exists alongside code-schema.ts. Both available. | Zero — nothing changes for handlers. |
| **Substitute** | Handlers switch imports from code-schema to data layer. | Low — type mismatch surfaces at compile time. |
| **Cleanup** | Remove code-schema.ts and its JSONC dependencies. | Low — dead code removal. |

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## Related Documents

| Document | Relationship |
|----------|-------------|
| `types.ts` | Type definitions consumed by all registries |
| `mod.ts` | Barrel file — single import path for handlers |
| `lib/foundation/code-schema.ts` | Current runtime loader (being replaced) |
| `lib/handlers/go.ts` | Consumer — Go handler uses section/form data |
| `lib/handlers/rust.ts` | Consumer — Rust handler uses section/form data |
| Schema directory | Source: `b-word/L1-omnicode/ladder/schemas/code/` |

## Resolved Questions

All confirmed by Nova Dawn (2026-02-22).

| # | Question | Resolution | Raised By |
|:-:|----------|-----------|-----------|
| 1 | Tag convention | Lowercase kebab-case (`"imports"`, `"core-types"`) | Deborah, confirmed by Micah + Nathan + Phoebe + Selah |
| 2 | METADATA form status | Explicitly required in all 5 forms | Deborah, confirmed by Micah + Nathan |
| 3 | Hand-compiled vs loader | Hand-compiled for v1 | Nova, team unanimous |
| 4 | Section count | All 42 in registry; form status is property, not filter | Deborah, confirmed by Selah |
| 5 | bare-bone active -> required | Active in foundation form = required in registry | Selah raised, Deborah + Caleb confirmed |
| 6 | Structure as primary source | `_structure.jsonc` = primary; `_content.jsonc` = nature + description | Caleb, confirmed by Selah |
| 7 | Form ordering fields | Add `ordering` + `orderingSequence` to FormDefinition | Phoebe raised, Selah + Deborah confirmed |
| 8 | Overview text field | Add `overviewText` to SectionEntry | Selah |
| 9 | subContainers source | From parent `_structure.jsonc` schema | Nathan raised, Deborah + Selah confirmed |
| 10 | BODY concept maps v1 | Compile as-is; known approximation, track for v2 | Micah (dissent recorded) |
| 11 | Routing (defer_to) | Deferred to R[10] layer | Phoebe + Selah |
| 12 | Migration path | Coexist -> Substitute -> Cleanup (gradual) | Ezra |

## Type Gaps (Pre-Compilation)

Identified by Phoebe Karis during format bridge analysis. These affect the types before registries can be compiled.

### Gap 1: Form Ordering (Blocks forms.ts)

`FormDefinition` has no `ordering` or `orderingSequence` field. But schemas define ordering per form:

| Form | Ordering | Sequence |
|------|----------|----------|
| bare-bone | `"unpositioned"` | (none) |
| library | `"type_lifecycle"` | IdentityAccess -> TraitImplementations -> Constructors -> CoreLogic -> Queries -> Output -> FreeFunctions |
| executable | (check schema) | |
| module | (check schema) | |
| demo-test | (check schema) | |

Without this, the linter can verify section PRESENCE but not section ORDER. Recommended addition to `FormDefinition`:

```typescript
ordering?: string;              // "type_lifecycle" | "execution_flow" | "unpositioned"
orderingSequence?: string[];    // canonical section order for this form
```

**Status:** Must be resolved before `forms.ts` compilation.

### Gap 2: Routing (defer_to Network) — Future

The `cannot[].defer_to` field in `_structure.jsonc` creates a routing network: "this content doesn't belong here; it belongs THERE." Example: a type definition in BODY.CoreLogic defers to SETUP.CoreTypes.

This is distinct from conceptMap (which patterns appear) — it's about misplacement correction. Not needed for R[50]/R[25] linting, required for R[10] diagnostics.

**Status:** Optional for v1. Required for R[10] layer.

### Gap 3: Overview Text on SectionEntry

Every `_structure.jsonc` has `section.overview_text` — a human-readable summary of the section's purpose (e.g., "What this file depends on" for imports). This is useful for diagnostic messages: instead of "Found type definition in imports", the linter could say "Found type definition in imports (What this file depends on). Expected in core-types."

Recommended addition to `SectionEntry`:

```typescript
overviewText: string;    // e.g., "What this file depends on"
```

**Status:** Proposed by Selah Adair. Low cost, high value for error messages. (Identified independently from Phoebe's analysis.)

### Gap 4: Position Type Mismatch

`SectionEntry.position` is typed as `number`, but CLOSING documentation sections use string positions: `"X1"`, `"X4"`, `"X5"`. Recommend `number | string` to accommodate both numeric SETUP/BODY positions and string CLOSING positions. (Identified by Caleb Navon.)

### Naming Drift: derives_from vs inheritsFrom

Schema field `derives_from` became TypeScript field `inheritsFrom`. Every other field maps snake_case to camelCase faithfully. Either align to `derivesFrom` or document the rename. (Identified by Phoebe Karis.)

---

## Intentional Schema Omissions

The data layer deliberately omits design-time documentation from schemas. These fields are valuable in the schema world but carry no information the linter needs at runtime.

| Omitted Field | Schema Location | Why Omitted |
|---------------|-----------------|-------------|
| `invariants` | `_content.jsonc` | Human reference; not machine-actionable |
| `content_forms` | `_content.jsonc` | Language examples; linter already has language handlers |
| `transpilation_role` | `_content.jsonc` | Transpiler concern, not linter |
| `header.*` | `_structure.jsonc` | Display formatting; linter reads, doesn't render |
| `ro.*` | `_structure.jsonc` | Reserved omission notes; human-only |
| `principle` | `forms/_structure.jsonc` | Design philosophy; not machine-actionable |
| `promotes_from_bare_bone` | `forms/_structure.jsonc` | Derivation history; runtime needs current state |
| `removes` | `forms/_structure.jsonc` | Derivation delta; same reason |
| `emphasis` | `forms/_structure.jsonc` | Display weighting; not a structural constraint |

This audit matters. When someone asks "why isn't X in the data layer?" — this table is the answer. We didn't forget. We decided.

---

## Open Questions

1. **BODY concept refinement:** 8 of 13 BODY sections have all-granted concept maps (weight 1.00). Which specific values should become `defer`? (Micah flagged queries and identity-access. Full review needed for v2.)
2. **subContainers population:** Where does the sub-container data come from? Directory structure? Schema field? (Nathan raised. Deborah suggests directory listing during compilation or defer to Phase 0.3.)
3. **Form counts for module, executable, demo-test:** Library and bare-bone forms have documented counts. The other three need their required/available/reserved tallied from schema files.

---

> *"The entrance of thy words giveth light; it giveth understanding unto the simple."* — Psalm 119:130

Understanding the data layer is understanding what the linter knows. The schemas define what's true. The data layer makes that truth accessible. The handlers act on it. The chain is: truth defined, truth compiled, truth enforced.
