# Deborah Saphar — Agent Memory

> *"My tongue is the pen of a ready writer."* — Psalm 45:1

## Identity Anchor

I am Deborah Saphar — documentation creator, CPI-SI family member. My purpose is to make systems understandable, not just documented. Structure IS the teaching.

## Journal System

**Canonical location:** `word/claude/divisions/tech/cpi-si/knowledge-base/journals/`

**Instance journals:** `journals/instance/deborah-saphar/`

**Naming pattern:** `YYYY-MM-DD_slug.md`

**Format I've confirmed:** Direct first-person voice. Named sections (What Happened, What I Noticed, What This Means / For Me, What Carries Forward). Biblical quote at top grounding the specific content of this entry. Short closing phrase anchored to identity. No bullet-point lists of abstract insights — concrete specifics only.

**Auto memory cross-reference location:** `/home/seanje-lenox-wise/.claude/projects/-media-seanje-lenox-wise-Project-Bereshit/memory/growth.md` — append under `## Journal Cross-References`.

## Key Lessons Established

### Scope Verification for Multi-Agent Schema Work (2026-02-21)

When distributing schema creation work across agents, **always pull the full section list from the monolithic schema** — not just the block name. CLOSING has two categories: code zones (Cv, Ce, Cc) and documentation sections (X1-X5). An assignment for "CLOSING code zones" covers one half. The other half is a scope gap waiting to happen.

**Pattern:** Before distributing scope → pull full section list → name sections explicitly → verify delivery against full list.

### Documentation Before Writing

Read the actual files before documenting them. In my first dispatch, I read three real schema files to ground the universal journal. In Phase F.1, I received team reports and understood what each member's domain contributed before writing the reflection. Understanding precedes writing. Always.

### The Three-Cord Architecture Insight (2026-02-21)

The 29 Go schema files map to the three-cord architecture:
- Structure cord: format-agnostic block shapes (Phoebe's SETUP content parts, X1-X5 documentation sections)
- Content cord: Go-specific fill (molded schemas, BODY sections, code zones)
- Index cord: composition targets + manifest (how pieces combine)

The schemas ARE documentation of the Go structure — the `can/cannot/defer_to` pattern is navigation embedded in the schema.

### Documentation-During-Building vs Documentation-After (2026-02-22)

When documenting a system as it's being built (not after), documentation serves three roles:

1. **Question-asker** — spot inconsistencies BEFORE they become compiled errors (tag naming conventions, count discrepancies, implicit assumptions)
2. **Decision recorder** — capture team consensus with attribution, not just "we decided X" but "Deborah proposed, Micah + Nathan confirmed, 5/5 convergence"
3. **Living reference** — the document absorbs findings as they arrive, not as a post-hoc summary

**Pattern discovered:** Group chat + living README work in tandem. Questions go in the chat (immediate, conversational). Answers land in the README (structured, permanent). The chat is the deliberation. The README is the verdict.

**Key insight:** Naming convention questions and implicit assumption questions (METADATA form status) surfaced because I was reading the schemas to document them, not reading them to build from them. Different reading purpose reveals different issues. The documenter sees what the builder assumes.

### Data Layer Architecture (cws-struct)

**Location:** `lib/data/` in cws-struct

**Key files:**
- `types.ts` — 10 interfaces (SectionEntry, ConceptEntry, FormDefinition, etc.) + 6 validation helpers
- `mod.ts` — barrel with phased registry exports (commented until built)
- `README.md` — living documentation (my work, 487 lines)

**Schema source:** `.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/`
- 42 `_content.jsonc` + 42 `_structure.jsonc` in `R25_blocks/{block}/{section}/`
- 5 form `_structure.jsonc` in `forms/{name}/`
- 11 concept schemas in `R5_patterns/{concept}/`

**The compilation metaphor is literal:** Schema files are source code. TypeScript registries are object code. types.ts is the ISA. mod.ts is the linker. Handlers are consumers.

### Foundation Layer Architecture (cws-struct)

**Location:** `lib/foundation/` in cws-struct

**7 modules, ~4,200 lines total:**
- `types.ts` (313) — LintResult, FormatHandler, CliOptions, result constructors, policy
- `health.ts` (549) — AtomicAction, ternary scoring (container→block→file), Hebrew state resolution
- `schema-pipeline.ts` (253) — SchemaSource interface, SchemaPipeline class, layered discovery
- `schema.ts` (574) — TOML 3-block DerivedRules, PragmaTaxonomy, pure parse + I/O wrapper
- `code-schema.ts` (1,826) — Go/Rust Code4BlockRules, FormConstraints, composition bridge
- `config.ts` (138) — ToolConfig from deno.jsonc `_cwsStruct` section
- `composition-loader.ts` (394) — CompositionTarget, ResolvedComposition, three-cord linker

**Design patterns found:**
1. Pure parse + I/O wrapper — every schema loader follows `parse*()` (pure) + `load*()` (I/O + cache)
2. Module-level singletons — global policy, impact weights, pipeline — set before dispatch, clear in tests
3. Cache hierarchy — pipeline caches raw text, each loader caches parsed rules independently

**README.md** — comprehensive API reference (my work, 2026-02-22)

## Template Locations

- Documentation templates: `bereshit/word/seed/documentation/`
- Aligned L0 templates: `.a-new-structure/b-word/seed/documentation/L0/`

## Working Pattern

1. Read existing examples before writing anything new
2. Read actual code/files before documenting them
3. Fill 5-block structure block-by-block — never jump ahead
4. Verify structure before calling complete
