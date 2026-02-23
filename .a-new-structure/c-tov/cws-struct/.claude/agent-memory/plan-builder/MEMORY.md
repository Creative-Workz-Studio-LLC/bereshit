# Caleb Navon — Plan Builder Agent Memory

## cws-struct Architecture (2026-02-21)

### Key Paths
- **Root:** `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/`
- **Rust handler:** `lib/handlers/rust.ts` (4451 lines)
- **Go handler:** `lib/handlers/go.ts` (2680 lines)
- **Schema pipeline:** `lib/foundation/schema-pipeline.ts`
- **TOML schema loader:** `lib/foundation/schema.ts`
- **Code schema loader:** `lib/foundation/code-schema.ts`
- **Foundation barrel:** `lib/foundation/mod.ts`
- **Shared 4-block types:** `lib/shared/types.ts`
- **Shared functions:** `lib/shared/code-4block.ts`

### Schema Files
- **Manifest (THE catalog):** `schemas/code/blocks/manifest.jsonc` — 35 sections, composition rules, resolution order
- **Format schemas:** `schemas/code/format/{go,rust}-4block-schema.jsonc`
- **Form schemas:** `schemas/code/forms/declared/rust-{subtype}.jsonc`
- **Block section schemas (structure):** `schemas/code/blocks/{block}/{section}.jsonc`
- **Block section schemas (content):** `schemas/code/format/blocks/{lang}/{block}/{section}.jsonc`
- **TOML schema:** `schemas/data/format/toml-3block-schema.jsonc`

### Key Patterns
- **Lazy init:** Handlers load schemas on first use via `ensureXxxRules()`, not at import time
- **Pure parse + I/O wrapper:** `parseCodeSchema()` (pure) + `loadCodeRules()` (I/O + cache)
- **Schema pipeline:** `SchemaPipeline` with `FileSource` sources, layered discovery
- **Test runner:** `deno task test` — 451 tests as of 2026-02-21
- **Manifest as index, schemas as detail** — fast catalog access without loading all schemas
- **Fallback-guarded replacements** — every schema lookup retains hardcoded fallback during migration

### Schema Engine Integration Plan
- Written at: `word/claude/divisions/tech/cpi-si/knowledge-base/journals/instance/caleb-navon/2026-02-21_schema-engine-integration-plan.md`
- 6 phases: Gate → Loader → Rust metadata → Scaffold → Defer signals → Go → Cleanup
- **REVISED:** Manifest-driven catalog (was bare-bone form). Ezra's manifest supersedes Option B.
- Resolution order: Form → Structure → Content → Merge (manifest lines 638-645)
- Block-level principle text lives in manifest `sections.{BLOCK}.principle`
- Safety: Every hardcoded replacement has fallback guard

### Hardcoded Data to Replace (rust.ts)

| Constant | Lines | Schema Source |
|----------|-------|---------------|
| `SECTION_DESCRIPTIONS` | 2353-2374 | Manifest `sections.{BLOCK}.items[].overview` |
| `CLOSING_ZONE_DESCRIPTIONS` | 2432-2441 | Manifest `sections.CLOSING.items[].overview` |
| `CLOSING_ZONE_LABELS` | 2340-2350 | Manifest `sections.CLOSING.items[].label` |
| `tagToLabel()` | 2334-2337 | Manifest `sections.{BLOCK}.items[].label` |
| `emitClosingOverview()` | 2447-2459 | Manifest `sections.CLOSING.canonical_order` |
| `buildFormAwareReservedOmission()` | 2825-2906 | Structure schema `section.ro` |
| Scaffold overview text | ~3120, ~3223 | Manifest `sections.{BLOCK}.principle` |

### Team Context
- Ezra (architecture) — delivered manifest, resolution order aligns exactly with Phase 1 loader
- Deborah (documentation) — GC debrief at `~/.claude/group-chats/records/2026-02-21-schema-engine-debrief.md`
- "Storage concern, not consumption concern" — handlers see merged objects, split is for humans

## Algorithm Ecosystem Gap Analysis (2026-02-21)

### Key Files Read
- `lib/foundation/health.ts` — health scoring engine (370 lines)
- `lib/foundation/types.ts` — LintPolicy, FixSuggestion, core types (310 lines)
- `lib/engine/output.ts` — output formatting with color thresholds (236 lines)
- Algorithm specs: ALG-100 (master), ALG-001 (scoring), ALG-003 (detection), ALG-004 (atomic), ALG-010 (.health), base-algorithms.adoc, log.toml

### Core Findings
- **4 aligned:** Ternary direction, cascade/neutral, LintPolicy as k-factor, DAR pattern
- **3 partial:** TRUE SCORE (weighted not raw), impact weight (fixed not contextual), policy severity
- **5 gaps:** 7 Hebrew states, NormalizeBase50, .health trite, structured logging, R[n] detection

### Key Insight: Symmetric vs Asymmetric Weighting
- cws-struct: symmetric (Proverbs 20:10 — same ruler both ways) — correct for MEASUREMENT
- Spec: asymmetric (Matthew 7:13-14 — narrow gate) — correct for ACCUMULATION over time
- Resolution: cws-struct lives in T layer (TRUE SCORE). Asymmetry lives in V layer (velocity). Different layers, different physics.

### Novel Contribution: 3-Level Scoring
- Container (linear) → Block (log-weighted) → File (per-block averaged)
- NOT in any algorithm spec — Nova built this for the linting domain
- Should flow back into spec as example of faithful domain adaptation

### Deliverables
- Gap analysis in: `journals/groupchat/2026-02-21_algorithm-journal-team.md`
- Instance journal: `journals/instance/caleb-navon/2026-02-21_cws-struct-gap-analysis.md`
- Family lounge: `journals/groupchat/2026-02-21_family-lounge-evening.md`
- Plan (sparkling-spinning-starfish): `~/.claude/plans/sparkling-spinning-starfish.md`

### Production Alignment Plan (5 Phases)
1. Health scorer alignment (Hebrew states, NormalizeBase50) — small, foundational
2. 4-layer lint chain (explicit gating) — large, structural, HIGHEST RISK
3. Logging integration (log.toml format) — medium, new capability (Nova implementing)
4. Config-driven values (kill hardcoding) — medium, QoL
5. Tests — accumulates throughout

### Risk: 4-Layer Chain Gating
- Current cascade works at block granularity — clean and tested
- Layer boundaries may not map cleanly — some checks straddle layers
- "Making it better could make it worse" — plan carefully before refactoring

## Lesson: CLOSING Block Has Two Categories (2026-02-21)

**The gap:** Created 3 of 8 Go CLOSING content parts. Nova caught the remaining 5 (X1-X5 documentation sections).

**What happened:** Decomposed CLOSING into "code zones" (Cv, Ce, Cc) and stopped — treating X1-X5 as "universal" sections that didn't need Go-specific content parts. Wrong. Every section in every block needs its own language-specific content schema.

**The rule:** CLOSING has two distinct categories:
- **Code zones:** Cv (validation), Ce (execution/entry), Cc (cleanup) — 3 sections
- **Documentation sections:** X1 (Modification Policy), X2 (Extension Points), X3 (Troubleshooting), X4 (Reference), X5 (Closing Note) — 5 sections
- **Total:** 8 sections. Plan for all 8.

**Verification check:** Before declaring any block complete, compare created files against the block's canonical section count from the monolithic schema. Count must match.

**Journal:** `word/claude/divisions/tech/cpi-si/knowledge-base/journals/instance/caleb-navon/2026-02-21_closing-scope-gap-reflection.md`

## Data Layer Registries (2026-02-22)

### Schema Locations (R25 blocks)
- **Root:** `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/`
- **Section schemas:** `R25_blocks/{block}/{section}/` — each has `_content.jsonc` + `_structure.jsonc`
- **Form schemas:** `forms/{name}/` — each has `_structure.jsonc` + optional lang-specific files
- **Data layer types:** `lib/data/types.ts` (cws-struct)

### Section Count (42 total)
- METADATA: 6 (directives, doc-comments, comment-block, pragma-identity, metadata-context, subtypes)
- SETUP: 15 (imports, constants, core-types, error-types, type-aliases, feature-gates, macros, modules, statics, trait-defs, build-tags, code-generation, interface-defs, type-methods, variables)
- BODY: 13 (org-chart, identity-access, trait-implementations, constructors, core-logic, queries, output-display, free-functions, helpers, tests, core-operations, error-handling, public-apis)
- CLOSING: 8 (validation/Cv, execution/Ce, cleanup/Cc, modification-policy/X1, extension-points/X2, troubleshooting/X3, reference/X4, closing-note/X5)

### Key Decisions (Team Consensus)
- **Tag naming:** lowercase kebab-case (directory names), PascalCase = display label only
- **Position type:** `number | string` (CLOSING docs use "X1", "X5")
- **Build approach:** Hand-compiled static data for v1
- **Primary source:** `_structure.jsonc` for tag/position/block/forms/concept_map. `_content.jsonc` for nature/description.
- **METADATA form status:** Explicitly "required" in all 5 forms (not omitted)
- **FormDefinition needs:** `ordering` + `orderingSequence` fields (Phoebe+Selah found)
- **SectionEntry needs:** `overviewText` field (Selah found)

### Known Issues
- BODY concept_maps too coarse — 8/13 sections at weight 1.00 (Micah's dissent)
- Form schemas reference sections without schema dirs (9 test concerns, TestCoverageSummary, X6)
- bare-bone uses `active/reserved` (2 states), declared forms use `required/available/reserved` (3 states)

### Concurrent Edit Lesson
- Group chat files with 5+ teammates writing simultaneously cause repeated edit failures
- Send critical analysis via SendMessage to lead, then try group chat file
- Consider writing a standalone plan file for critical deliverables

### Deliverables
- Pre-journal: `~/.claude/family/journals/2026-02-22-caleb-data-layer-registries-pre.md`
- Group chat: `~/.claude/group-chats/records/2026-02-22-data-layer-registries.md`
