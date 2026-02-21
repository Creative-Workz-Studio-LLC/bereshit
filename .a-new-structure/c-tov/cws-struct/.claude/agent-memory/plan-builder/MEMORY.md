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
- **Shared 4-block types:** `lib/handlers/shared/types.ts`
- **Shared functions:** `lib/handlers/shared/code-4block.ts`

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
