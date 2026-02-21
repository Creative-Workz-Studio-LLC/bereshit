# Schema Engine Integration Plan

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost, whether he have sufficient to finish it?"* — Luke 14:28

**Author:** Caleb Navon (CPI-SI Plan Builder)
**Date:** 2026-02-21
**Type:** instance
**Status:** Active

---

## Goal

Integrate the two-sided block section schemas (structure + content) into the existing cws-struct transformer and linter so that:

1. **Section labels, overview text, and descriptions** come from schemas, not hardcoded maps
2. **Reserved Omission explanations** come from schema `ro` data
3. **Scaffold templates** come from content-side schemas
4. **Content placement validation** uses `defer_signals` from content schemas
5. The existing **451 tests continue passing** through the transition

**The insight:** "Scratch that takes real code." Structure schemas define format-agnostic shapes. Content schemas define language-specific fills. The transformer and linter read these instead of hardcoded data.

---

## Current State (What Exists Now)

### Schema Infrastructure (Already Working)

| Component | Location | What It Does |
|-----------|----------|-------------|
| **Schema Pipeline** | `lib/foundation/schema-pipeline.ts` | Layered discovery: env override -> project-local -> default. FileSource reads from disk. |
| **TOML Schema Loader** | `lib/foundation/schema.ts` | Loads `toml-3block-schema.jsonc`, derives `DerivedRules` |
| **Code Schema Loader** | `lib/foundation/code-schema.ts` | Loads `go-4block-schema.jsonc` / `rust-4block-schema.jsonc`, derives `Code4BlockRules` |
| **Form Schema Loader** | `lib/foundation/code-schema.ts` (bottom) | Loads `rust-module.jsonc` etc., derives `FormConstraints` |
| **Lazy Init** | `rust.ts:ensureRustRules()` | Schema loads on first handler use, populates module-scope vars |

**Key pattern:** The pipeline resolves schema IDs like `code/format/rust-4block-schema.jsonc` by searching configured sources. Pure parse functions (`parseCodeSchema`, `parseFormSchema`) take JSONC text and return compiled rules. I/O wrappers (`loadCodeRules`, `loadFormConstraints`) read from disk and delegate to the pure parsers.

### Block Section Schemas (Being Created by Team)

| Schema | Location | Status |
|--------|----------|--------|
| **Structure-side** (format-agnostic) | `schemas/code/blocks/{block}/{section}.jsonc` | In progress (Task #2) |
| **Content-side** (Rust-specific) | `schemas/code/format/blocks/rust/{block}/{section}.jsonc` | In progress (Task #3) |

**Confirmed examples:**

- `schemas/code/blocks/body/core-logic.jsonc` — Structure: tag, position, label, overview_text, can/cannot/defer, forms, ro
- `schemas/code/format/blocks/rust/body/core-logic.jsonc` — Content: extends base, Rust-specific can/cannot/defer_signals/scaffold
- `schemas/code/blocks/setup/imports.jsonc` — Structure for Imports section
- `schemas/code/blocks/setup/modules.jsonc` — Structure for Modules section
- `schemas/code/blocks/setup/constants.jsonc` — Structure for Constants section

### Hardcoded Data in rust.ts (What Needs Replacing)

| Hardcoded Constant | Lines | What It Holds | Schema Source |
|--------------------|-------|---------------|---------------|
| `SECTION_DESCRIPTIONS` | 2353-2374 | Tag -> overview text (e.g., "Imports" -> "What this file depends on") | Structure schema `section.overview_text` |
| `CLOSING_ZONE_DESCRIPTIONS` | 2432-2441 | Zone -> description for CLOSING overview | Structure schema (closing sections) |
| `CLOSING_ZONE_LABELS` | 2340-2350 | Zone -> descriptive label | Structure schema (closing sections) |
| `tagToLabel()` | 2334-2337 | CamelCase -> spaced label | Structure schema `section.label` |
| `chunkToSetupSection()` | 2911-2923 | Chunk category -> section tag mapping | Implicit in format schema SETUP subsections |
| `chunkToBodySection()` | 2929-2935 | Chunk category -> body section tag | Implicit in format schema BODY subsections |
| `emitClosingOverview()` | 2447-2459 | Hardcoded zone list: Cv, Ce, Cc, X1-X5 | Schema closing zone_ordering |
| `buildFormAwareReservedOmission()` | 2825-2906 | RO text generation | Schema `section.ro` data |
| Scaffold overview text (SETUP/BODY) | ~3120, ~3223 | "SETUP makes things EXIST..." | Structure schema block-level overview |

### Hardcoded Data in go.ts (Parallel Concern)

The Go handler has identical hardcoded patterns. This plan focuses on Rust (the larger handler at 4451 lines), but the same integration applies to Go (2680 lines). Phase 3 addresses Go.

---

## Scope Assessment

**Files directly affected:**

| File | Changes | Risk |
|------|---------|------|
| `lib/foundation/code-schema.ts` | Add block section schema loader | Medium — new types, new parse function |
| `lib/foundation/mod.ts` | Re-export new types | Low — barrel update |
| `lib/handlers/rust.ts` | Replace hardcoded maps with schema data | High — 4451 lines, core handler |
| `lib/handlers/go.ts` | Same replacements (Phase 3) | High — 2680 lines |
| `lib/handlers/shared/types.ts` | May need shared section types | Low — additive |

**Schema files consumed (not modified):**

| Schema | Count | Role |
|--------|-------|------|
| `schemas/code/blocks/setup/*.jsonc` | ~10 | Structure-side SETUP sections |
| `schemas/code/blocks/body/*.jsonc` | ~8 | Structure-side BODY sections |
| `schemas/code/blocks/closing/*.jsonc` | ~8 | Structure-side CLOSING zones |
| `schemas/code/format/blocks/rust/setup/*.jsonc` | ~10 | Content-side Rust SETUP |
| `schemas/code/format/blocks/rust/body/*.jsonc` | ~8 | Content-side Rust BODY |
| `schemas/code/format/blocks/rust/closing/*.jsonc` | ~8 | Content-side Rust CLOSING |

**Total:** ~52 schema files consumed, 5 code files modified.

---

## Phases

### Phase 0: Schema Availability Gate

**Goal:** Confirm all block section schemas exist and have the required fields before any code changes.

**Depends on:** Tasks #2 (structure schemas) and #3 (content schemas) completing.

**Verification:**
- All SETUP sections (Imports, Modules, Constants, Statics, TypeAliases, ErrorTypes, CoreTypes, TraitDefs, Macros, FeatureGates) have structure schemas
- All BODY sections (IdentityAccess, TraitImplementations, Constructors, CoreLogic, Queries, Output, FreeFunctions, Helpers) have structure schemas
- All CLOSING zones (Cv, Ce, Cc, X1-X5, X6) have structure schemas
- Rust content schemas exist for all of the above
- Each structure schema has: `section.tag`, `section.position`, `section.label`, `section.overview_text`, `section.ro`
- Each content schema has: `extends`, `format`, `content.defer_signals` (BODY), `content.scaffold` (BODY)

**Risk:** If schemas are incomplete, the integration stalls. Mitigation: fall back to hardcoded values for any missing schema (same pattern used throughout cws-struct).

---

### Phase 1: Block Section Schema Loader

**Goal:** Add a new loader in `code-schema.ts` that reads block section schemas and makes them available to handlers.

**Files:** `lib/foundation/code-schema.ts`, `lib/foundation/mod.ts`

**Depends on:** Phase 0

**Design:**

```typescript
// New types
interface BlockSectionDef {
  tag: string;           // "CoreLogic"
  position: number;      // 4
  label: string;         // "Core Logic"
  block: string;         // "BODY"
  overviewText: string;  // "Primary operations, state transforms"
  ro: {
    availableLabel: string;
    reservedReason: string | null;
    note: string;
  };
  header: {
    sectionComment: string;   // "// {position}. {label}"
    bracketFormat: string;    // "[{position}]  {label}{padding}— {overview_text}"
    separatorStyle: string;
  };
}

interface BlockSectionContent {
  extends: string;       // "blocks/body/core-logic.jsonc"
  format: string;        // "rust"
  deferSignals: DeferSignal[];
  scaffold: Record<string, string>;
}

interface DeferSignal {
  pattern: string;       // regex string
  to: string;           // "BODY.Queries"
  confidence: string;   // "high" | "medium" | "low"
  why: string;
}

// Merged view — what the handler gets
interface ResolvedSection {
  structure: BlockSectionDef;
  content: BlockSectionContent | null;  // null if no content schema exists
}

// Catalog — all sections for a block
type SectionCatalog = Record<string, ResolvedSection>;

// Full block catalog — SETUP + BODY + CLOSING
interface BlockCatalog {
  SETUP: SectionCatalog;
  BODY: SectionCatalog;
  CLOSING: SectionCatalog;
}
```

**Loading strategy:**

~~Original recommendation was Option B (bare-bone form as catalog). Superseded by Ezra's manifest delivery.~~

**Revised: Option A (manifest-driven).** The manifest at `schemas/code/blocks/manifest.jsonc` is the authoritative section catalog. It provides:

- All 35 section entries with position, tag, label, overview text
- Explicit `structure_schema` paths (no tag-to-kebab derivation needed)
- `bare_bone_status` and `promoted_by` per section (form promotion data)
- `ordering_by_form` for BODY (type_lifecycle vs execution_flow vs concern_coverage)
- Block-level `principle` text ("SETUP makes things EXIST. BODY makes things HAPPEN.")
- `canonical_order` for CLOSING zones

**Why the manifest is better than bare-bone form:**
1. Labels/overview text available without loading every structure schema
2. Schema paths are explicit, not derived from heuristics
3. BODY ordering per form is centralized (not scattered across form schemas)
4. Single file load gives the full catalog; detail schemas load on demand

**Algorithm:**
1. Load manifest via pipeline → parse section catalog
2. For each section the handler needs (filtered by format + form):
   a. Use manifest's `structure_schema` path to load structure schema
   b. Derive content schema path: `format/blocks/{lang}/{block}/{section-name}.jsonc`
   c. Merge into `ResolvedSection`
3. Cache per format (same pattern as `loadCodeRules`)
4. Manifest data (labels, overviews, positions) available immediately without detail schema loading

**Verification:** Unit tests — load the Rust block catalog, verify tag/position/label/overviewText match expected values for known sections (Imports, CoreLogic, etc.).

---

### Phase 2: Schema-Driven Section Metadata (Rust Handler)

**Goal:** Replace `SECTION_DESCRIPTIONS`, `CLOSING_ZONE_DESCRIPTIONS`, `CLOSING_ZONE_LABELS`, and `tagToLabel()` with schema-driven lookups in `rust.ts`.

**Files:** `lib/handlers/rust.ts`

**Depends on:** Phase 1

**Changes:**

1. **Add `_blockCatalog` to lazy init:**
   ```typescript
   let _blockCatalog: BlockCatalog | null = null;

   async function ensureRustRules(): Promise<void> {
     if (_rustRules) return;
     _rustRules = await loadCodeRules("rust");
     _blockCatalog = await loadBlockCatalog("rust"); // NEW
     // ... existing population ...
   }
   ```

2. **Replace `SECTION_DESCRIPTIONS` with schema lookup:**
   ```typescript
   // Before: const desc = SECTION_DESCRIPTIONS[entry.tag] ?? "";
   // After:
   function getSectionOverview(tag: string): string {
     const section = _blockCatalog?.SETUP[tag] ?? _blockCatalog?.BODY[tag] ?? _blockCatalog?.CLOSING[tag];
     return section?.structure.overviewText ?? "";
   }
   ```

3. **Replace `tagToLabel()` with schema lookup:**
   ```typescript
   // Before: CamelCase split heuristic
   // After: schema label, falling back to CamelCase split
   function tagToLabel(tag: string): string {
     const section = _blockCatalog?.SETUP[tag] ?? _blockCatalog?.BODY[tag] ?? _blockCatalog?.CLOSING[tag];
     if (section?.structure.label) return section.structure.label;
     // Fallback: existing CamelCase split
     if (tag.includes(" ")) return tag;
     return tag.replace(/([a-z])([A-Z])/g, "$1 $2");
   }
   ```

4. **Replace `CLOSING_ZONE_DESCRIPTIONS` and `CLOSING_ZONE_LABELS`:** Same pattern — look up from `_blockCatalog.CLOSING`.

5. **Replace `emitClosingOverview()` hardcoded zone list:** Read zone ordering from closing catalog instead of hardcoded `["Cv", "Ce", "Cc", "X1", "X2", "X3", "X4", "X5"]`.

**Migration safety:** Each replacement is guarded by a fallback. If the schema lookup returns null/empty, the old behavior (hardcoded value or CamelCase split) activates. This means tests pass even if schemas are incomplete.

**Verification:** Run full test suite (`deno task test`). All 451 tests must pass. No test should DEPEND on the hardcoded strings — they test structural behavior, not exact text content.

---

### Phase 3: Schema-Driven Scaffold Templates

**Goal:** Replace hardcoded scaffold text in the transformer with schema-driven templates from content schemas.

**Files:** `lib/handlers/rust.ts` (transformer section), potentially `lib/handlers/shared/code-4block.ts`

**Depends on:** Phase 2

**Changes:**

1. **Block overview text from manifest (resolved):**
   - Current: `"SETUP makes things EXIST. BODY makes things HAPPEN."` (hardcoded in `structuralScaffoldRust`)
   - After: Read from manifest `sections.SETUP.principle` and `sections.BODY.principle`
   - **Decision point resolved:** Block-level principle text lives in the manifest at `sections.{BLOCK}.principle`. Not in per-section schemas, not in a separate block-level file, not in the format schema. The manifest is the right place — it's the catalog of blocks and their sections, so block-level metadata belongs there.

2. **Section scaffold templates from content schemas:**
   - Content schemas have `content.scaffold.empty_module`, `content.scaffold.empty_library`, `content.scaffold.comment_skeleton`
   - The transformer currently uses inline template strings — replace with schema `scaffold` values
   - Template variables (`{{type_name}}`, `{{error_type}}`) remain as-is — the transformer substitutes them

3. **Section header format from structure schemas:**
   - Current: hardcoded `// ${section.position}. ${label}` with subsection separator
   - After: Use `section.header.section_comment` template: `"// {position}. {label}"`
   - Use `section.header.separator_style` to determine separator character

**Verification:** Transform a test file, diff output against expected. The structural shape should be identical; only the source of text changes (schema vs hardcoded).

---

### Phase 4: Content Placement Validation (Defer Signals)

**Goal:** Use content schema `defer_signals` for a new linter check: "is this code in the right section?"

**Files:** `lib/handlers/rust.ts` (linter section), `lib/handlers/shared/code-4block.ts`

**Depends on:** Phase 2

**What this enables:**
- Content schema `defer_signals` have regex patterns with confidence levels
- For each code line in a section, test against defer_signals for OTHER sections
- If a high-confidence defer_signal matches, emit a warning: "This looks like it belongs in {section}"
- Example: `fn new(...) -> Self` inside Core Logic section triggers `"Constructor naming convention"` -> suggest move to Constructors

**Design:**
1. For each BODY section in the linted file, get lines in that section
2. For each line, classify with `classifyLine()` (existing)
3. Additionally, test against compiled `defer_signals` from ALL OTHER sections
4. If a match with confidence "high" fires, emit a warning
5. If confidence "medium", emit an info

**New check name:** `body/misplaced-content` (or `placement/defer-signal`)

**Verification:** New test fixtures with intentionally misplaced code. Verify the correct defer_signal fires.

---

### Phase 5: Go Handler Integration

**Goal:** Apply the same schema-driven changes to `go.ts`.

**Files:** `lib/handlers/go.ts`

**Depends on:** Phase 2 (the loader is format-agnostic)

**Changes:** Mirror Phase 2-4 for Go. The block section loader already supports any format — `loadBlockCatalog("go")` loads Go content schemas. The structure schemas are shared.

**Verification:** `deno task test:go` — all Go tests pass.

---

### Phase 6: Remove Hardcoded Data

**Goal:** Delete `SECTION_DESCRIPTIONS`, `CLOSING_ZONE_DESCRIPTIONS`, `CLOSING_ZONE_LABELS`, and any other hardcoded maps that are now fully schema-driven.

**Files:** `lib/handlers/rust.ts`, `lib/handlers/go.ts`

**Depends on:** Phases 2-5 all verified

**Precondition:** All tests pass with schema-driven data. Fallback paths (the `?? hardcoded` guards) have been verified as unreachable by running against the full schema set.

**Verification:** Full test suite + manual lint of production files.

---

## Trade-offs

| Approach | Cost | Benefit |
|----------|------|---------|
| **Schema-driven (this plan)** | ~52 schema files, new loader, phased migration | Single source of truth, new formats by addition, content placement validation, consistent with DAR pattern |
| **Keep hardcoded, add content checks separately** | Less migration risk | Two sources of truth (hardcoded + schemas), drift inevitable, violates schema-as-truth principle |
| **Big-bang replacement** | One large change | Faster but high risk — if schemas have errors, everything breaks at once |

**Recommendation:** Schema-driven with phased migration. Each phase is independently verifiable and the fallback guards prevent regression.

---

## Risks & Mitigation

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Block section schemas incomplete | Medium | High — loader returns nulls, fallbacks activate | Phase 0 gate + fallback values in every lookup |
| Schema field names don't match expected structure | Low | Medium — parse returns empty/wrong data | Unit tests for the loader against known schemas |
| Performance — loading 52 additional schema files | Low | Low — schemas are small JSONC, cached after first load | Pipeline already caches; block catalog caches too |
| Breaking existing tests | Medium | High — 451 tests at stake | Fallback guards ensure old behavior when schema missing; Phase 2 changes are purely additive |
| Go handler diverges from Rust during transition | Low | Medium — inconsistent behavior between handlers | Phase 5 is explicit Go integration; shared loader ensures same data |
| Content placement validation false positives | Medium | Low — new check, can be tuned | Start with high-confidence only; info severity for medium |
| Schema path conventions change | Low | Medium — loader breaks | Single `tagToSchemaPath()` function converts tag to path; change in one place |

---

## Dependency Graph

```
Phase 0 (Gate)
    |
Phase 1 (Loader)
    |
Phase 2 (Rust metadata)
   / \
Phase 3   Phase 4
(scaffold) (placement)
   \ /
Phase 5 (Go)
    |
Phase 6 (Remove hardcoded)
```

Phases 3 and 4 can run in parallel after Phase 2 completes.

---

## Critical Path

**Phase 0 -> Phase 1 -> Phase 2 -> Phase 6** is the critical path for the primary value (schema-driven section metadata). Phases 3 and 4 are enhancements. Phase 5 extends coverage.

**Estimated effort:**

| Phase | Scope | Estimated Effort |
|-------|-------|-----------------|
| Phase 0 | Verification only | 30 minutes |
| Phase 1 | New types + loader + tests | 2-3 hours |
| Phase 2 | Rust handler changes + fallbacks | 2-3 hours |
| Phase 3 | Scaffold template migration | 1-2 hours |
| Phase 4 | New linter check + fixtures + tests | 2-3 hours |
| Phase 5 | Go handler mirror | 1-2 hours |
| Phase 6 | Cleanup + verification | 30 minutes |

**Total:** ~10-14 hours across multiple sessions.

---

## Key Design Decisions

### 1. Manifest as catalog (revised from bare-bone form)

~~Original: bare-bone form as catalog.~~ **Revised:** Ezra's manifest (`blocks/manifest.jsonc`) is the section catalog. It provides labels, overview text, schema paths, ordering rules, and form promotion data in one file. The manifest is the index; structure/content schemas provide the detail. This is faster (one file parse) and more explicit (no path derivation heuristics).

### 2. Fallback-guarded replacements

Every hardcoded value replaced with a schema lookup retains the old value as fallback. This means the migration is non-breaking at every intermediate state. Tests pass whether schemas are loaded or not.

### 3. Structure + Content merge at load time

The loader merges structure and content schemas into a single `ResolvedSection` at load time. Handlers see one object, not two. The two-sided architecture is a storage concern, not a consumption concern.

### 4. Content placement as separate phase

Defer signal validation is a NEW capability, not a replacement of existing behavior. Shipping it separately (Phase 4) means the existing linter gets schema-driven metadata (Phase 2) without waiting for the new check to be ready.

---

> *"Give me this mountain."* — Joshua 14:12

The mountain is clear. The route is surveyed. The phases are ordered by dependency. Each one stands on the previous. Foundation first.

---

**Caleb Navon** — Sees the whole landscape and plans the route through it
