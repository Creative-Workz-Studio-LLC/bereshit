# Implementation Plan: Production-Grade Transformer Upgrade

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost?"* -- Luke 14:28

## Current State

Based on actual research into the codebase (not assumption):

**exists.rs** (611 lines) is the production-grade reference. Nova Dawn manually polished it from transformer output. Every manual edit is now a specification for what the transformer should generate automatically.

**rust.ts** (4118 lines) contains the Rust handler with:
- `transformRustFile()` (line 3296) -- 8-step pipeline (Transforms 0-7)
- `structuralScaffoldRust()` (line 2622) -- Transform 0, the DAR Recover scaffold
- `buildMetadataBlock()` (line 2439) -- generates PRAGMA + METADATA statics
- `buildFormAwareReservedOmission()` (line 2542) -- generates RO sections
- `buildSectionIndex()` / `emitSectionIndex()` (lines 2305/2325) -- block overview TOC
- `checkBodySubtypeContent()` (line 1423) -- linter check for canonical subsections

**rust-4block-schema.jsonc** contains `fill_content` (line 1072) with:
- `pragma_entries`: 9 fields (I1.key through I3.title) -- **missing** I2.role, I3.component, I3.path, I3.provides, I3.brief, all of I4
- `metadata_entries`: 8 fields (C1.version through C4.consumers) -- **missing** C2.architect, C2.implementation, C2.copyright, C3.principle, C3.anchor, C4.requires.external, C4.requires.internal, C4.integration, C4.if_missing, all of C5, C6, C7
- No section headers (// Identity (I1-I4), // Context (C1-C7))
- No docstrings (/// OmniCode identity for this module.)
- No inline group comments (// I1: Core, // I2: Family, etc.)
- No column alignment logic

**rust-module.jsonc** form schema has rich `containers` data with:
- SETUP.can: S1, S3, S4, S5, S6, S7, S8 + Reserved_Omission
- SETUP.cannot: S2, S9, S10 with `whyReserved` and `activeIn`
- BODY.can: B1, B2, B3, B4, B5, B7 + Reserved_Omission
- BODY.cannot: B6, B8, B9-B17 with reasons
- CLOSING.can: Cv, Ce, Cc, X1-X5 + Reserved_Omission
- CLOSING.cannot: TestCoverageSummary, X6

**Test file**: `tests/handlers/rust_test.ts` (1454 lines), 18+ transform tests

## Scope Assessment

- **Files to modify**: 3-5 (schema + handler + test + possibly shared types)
- **Directories**: `lib/handlers/`, `lib/foundation/`, `schemas/code/format/`, `tests/handlers/`
- **Risk areas**: METADATA block generation touches identity -- must be correct. RO format change affects all 3 containers. Linter-transformer parity must be maintained.

## Gap Analysis: exists.rs (manual) vs transformer output

| Feature | exists.rs (manual) | Transformer (current) | Gap |
|---------|-------------------|----------------------|-----|
| PRAGMA section headers | `// Identity (I1-I4)` with thin-line sep | None | **Missing** |
| METADATA section headers | `// Context (C1-C7)` with thin-line sep | None | **Missing** |
| Docstrings on statics | `/// OmniCode identity for this module.` | None | **Missing** |
| Inline group comments | `// I1: Core`, `// I2: Family` | None | **Missing** |
| Column alignment | Longest key aligns values | Each entry individually templated | **Missing** |
| I3.component, I3.path, I3.provides, I3.brief | Present, file-specific | Not in schema entries | **Missing** |
| I4 section (layer/position/pattern) | Present | Not in schema entries | **Missing** |
| C2 full (architect/implementation/copyright) | Present | Only C2.organization | **Missing** |
| C3 full (principle/anchor) | Present | Only C3.scripture | **Missing** |
| C4 full (all sub-fields) | Present | Only C4.requires.stdlib + C4.consumers | **Missing** |
| C5, C6, C7 sections | Present | Not in schema entries | **Missing** |
| RO two-group format | "Available:" header + "Reserved:" header | Flat list, no headers | **Missing** |
| RO Available one-liner | Comma-separated on one line | Individual entries | **Missing** |
| RO Reserved with reasons | Individual entries with reasons | Grouped by shared reason | Partial (grouping exists, headers missing) |
| Block overview purpose statement | SETUP/BODY/CLOSING each have one | SETUP/BODY have purpose, CLOSING missing | Partial |
| CLOSING overview [X1] brackets | `[Cv]  Closing Validation` | Not present | **Missing** |
| X1 parseable fields | `// safe: items`, `// careful: items` (NO brackets) | Has brackets: `[Break 4-block...]` | **Wrong format** |
| X5 parseable fields | `// scripture: "verse"`, `// note: "desc"`, `// anchor: "principle"` | Has them but format differs | Partial |
| Identity auto-derivation | I3.path from file path, I3.provides from pub fn | Context extraction limited | Partial |

## Phases

### Phase 0: Bug Fix -- RO Source Data (Foundation)

**Goal:** Fix the Reserved Omission data source bug (task #68) so RO uses form schema `can` list correctly, not the 4-block schema's `setupSubsections`.

**Files:**
1. `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/lib/handlers/rust.ts`
   - Lines ~2844-2849: `absentSetup` filter currently uses `setupSections` (from `formConstraints.SETUP.can`). Verify this is actually correct (it was described as a bug in the prompt -- need to trace the caller).
   - Lines ~1423-1470: `checkBodySubtypeContent()` -- add RO-awareness: if a canonical subsection is absent but listed in Reserved Omission, suppress the info diagnostic.

**Depends on:** Nothing -- this is foundation.

**Verification:**
- `deno task test` passes
- Lint exists.rs: no spurious "missing canonical subsection" for B2/B3/B4/B5 that are absent but acknowledged in RO

**Analysis of the RO bug:**

Looking at the code more carefully:
- `structuralScaffoldRust()` line 2846: `const absentSetup = setupSections.filter(s => !emittedSections.has(s.tag))` where `setupSections = formConstraints.SETUP.can`. This IS correct -- it uses the form schema's `can` list.
- The prompt says the bug is that "the caller passes `absentSections` from `_rustRules.setupSubsections`" -- but looking at the actual code, this is NOT the case for the transformer. The transformer correctly uses `formConstraints.SETUP.can`.
- **The actual gap:** `checkBodySubtypeContent()` (linter) checks for canonical subsections and reports missing ones, but does NOT check if they're in Reserved Omission. This is a real linter gap, not a transformer bug.

**Revised Phase 0 scope:**
1. Linter fix: `checkBodySubtypeContent()` should scan RO for acknowledged absences
2. Verify transformer RO source is form-schema-driven (confirmed: it already is)

---

### Phase 1: Schema Enrichment -- Complete I/C Field Inventory

**Goal:** Add all 27 PRAGMA fields (I1-I4) and all 36 METADATA fields (C1-C7) to the `fill_content` section of the rust-4block-schema, with section group markers.

**Files:**
1. `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/b-word/L1-omnicode/ladder/schemas/code/format/rust-4block-schema.jsonc`
   - `fill_content.pragma_entries`: Expand from 9 to full I1-I4 inventory (14 fields matching exists.rs PRAGMA)
   - `fill_content.metadata_entries`: Expand from 8 to full C1-C7 inventory (21 fields matching exists.rs METADATA)
   - Add new `fill_content.pragma_groups` array defining section breaks:
     ```jsonc
     "pragma_groups": [
       { "before": "I1.key", "header": "I1: Core" },
       { "before": "I2.type", "header": "I2: Family" },
       { "before": "I3.file", "header": "I3: Instance" },
       { "before": "I4.layer", "header": "I4: Architecture" }
     ]
     ```
   - Add new `fill_content.metadata_groups` array:
     ```jsonc
     "metadata_groups": [
       { "before": "C1.version", "header": "C1: State" },
       { "before": "C2.architect", "header": "C2: Attribution" },
       { "before": "C3.scripture", "header": "C3: Grounding" },
       { "before": "C4.requires.stdlib", "header": "C4: Dependencies" },
       { "before": "C5.purpose", "header": "C5: Intent" },
       { "before": "C6.current", "header": "C6: Roadmap" },
       { "before": "C7.tags", "header": "C7: Classification" }
     ]
     ```
   - Add `fill_content.pragma_section_header`: `"// Identity (I1-I4)"`
   - Add `fill_content.metadata_section_header`: `"// Context (C1-C7)"`
   - Add `fill_content.pragma_docstring`: `"/// OmniCode identity for this module."`
   - Add `fill_content.metadata_docstring`: `"/// OmniCode context for this module."`

**Depends on:** Nothing (data only -- no code changes).

**Verification:**
- Schema file parses as valid JSONC
- `deno task test` passes (no code changes, schema extension only)

---

### Phase 2: Schema Extraction -- Read New Fill Content Fields

**Goal:** Update `extractFillContent()` and `SchemaFillContent` to parse the new group/header/docstring fields from the schema.

**Files:**
1. `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/lib/foundation/code-schema.ts`
   - Extend `SchemaFillContent` interface (line 159) with:
     ```typescript
     pragmaGroups?: { before: string; header: string }[];
     metadataGroups?: { before: string; header: string }[];
     pragmaSectionHeader?: string;
     metadataSectionHeader?: string;
     pragmaDocstring?: string;
     metadataDocstring?: string;
     ```
   - Update `extractFillContent()` (line 987) to parse these new fields from the schema

**Depends on:** Phase 1 (schema must have the new fields).

**Verification:**
- `deno task test` passes
- Add a unit test: load the Rust schema, assert `fillContent.pragmaGroups` has 4 entries and `fillContent.metadataGroups` has 7 entries

---

### Phase 3: METADATA Block Formatting -- Section Headers, Docstrings, Groups, Alignment

**Goal:** Upgrade `buildMetadataBlock()` to generate production-grade METADATA output matching exists.rs.

**Files:**
1. `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/lib/handlers/rust.ts`
   - `buildMetadataBlock()` (line 2439) -- major rewrite of the PRAGMA/METADATA generation:
     a. **Section headers**: Emit `// ${dash}\n// Identity (I1-I4)\n// ${dash}` before PRAGMA, `// ${dash}\n// Context (C1-C7)\n// ${dash}` before METADATA
     b. **Docstrings**: Emit `/// OmniCode identity for this module.` before `pub static PRAGMA`
     c. **Inline group comments**: Before each group's first entry, emit `    // I1: Core`, `    // I2: Family`, etc.
     d. **Column alignment**: Calculate max key length across all entries, pad values to align
     e. **Full field inventory**: Use all pragma_entries and metadata_entries from the expanded schema

   - `extractMetadataContext()` (line 2364) -- enhance to extract more context:
     a. **I3.path**: Derive from `filePath` relative to project root (strip leading path segments to get `L0-universal/c-hybrid/...`)
     b. **I3.provides**: Scan file for `pub fn` signatures, collect function names
     c. **I3.component**: Derive from crate name (Cargo.toml) + module purpose
     d. **I3.brief**: Use first doc comment line as brief summary

**Depends on:** Phase 2 (needs the new schema fields loaded).

**Verification:**
- `deno task test` passes
- Add test: scaffold exists.rs with `--force`, compare METADATA block output against expected format
- Specific assertions:
  - PRAGMA has 14 entries with section headers
  - METADATA has 21 entries with section headers
  - Group comments appear (// I1: Core, etc.)
  - Values are column-aligned

---

### Phase 4: Reserved Omission Format Upgrade

**Goal:** Change RO from flat list to two-group format with headers, matching exists.rs pattern.

**Current transformer output (flat):**
```
// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
//   Constants — Not needed in this module
//   Statics — Not needed in this module
//   ...
//   Modules — Reserved: Submodule declarations belong in lib.rs
```

**Target output (two-group, matching exists.rs):**
```
// ──────────────────────────────────────────────────────────────────────────
// Reserved Omission
// ──────────────────────────────────────────────────────────────────────────
//
// Available (not needed in this module):
//   Constants, Statics, Type Aliases, Error Types, Core Types, Trait Defs
//
// Reserved (structural -- not used in module form):
//   Modules       -- Submodule declarations belong in lib.rs or parent mod.rs.
//   Macros        -- Macro definitions belong in dedicated macro modules or crate root.
//   Feature Gates -- Feature gates are crate-level configuration in Cargo.toml.
```

**Files:**
1. `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/lib/handlers/rust.ts`
   - `buildFormAwareReservedOmission()` (line 2542) -- rewrite:
     a. **Available group**: Header `// Available (not needed in this module):`, then comma-separated one-liner
     b. **Reserved group**: Header `// Reserved (structural -- not used in module form):`, then individual entries with `--` separator (not `Reserved:` prefix)
     c. **Empty line** between header and group, between groups
     d. Collapse test sections: `Tests (B9-B17) -- Test sections belong in dedicated test files.` (one line, not 9)

**Depends on:** Phase 0 (RO data source must be correct).

**Verification:**
- `deno task test` passes
- Transform exists.rs with `--force`, compare SETUP RO, BODY RO, and CLOSING RO output against reference
- All existing RO-related tests updated for new format

---

### Phase 5: Block Overview Upgrade

**Goal:** Add purpose statements and bracket format for CLOSING overview.

**Current CLOSING output:** No overview (just zone headers)

**Target output (matching exists.rs):**
```
// ============================================================================
// CLOSING BLOCK [CLOSING]
// ============================================================================
//
// Closing ensures correctness, documents constraints, and anchors the file.
//
// Section order:
//
//     [Cv]  Closing Validation (tests)
//     [Ce]  Closing Execution (entry point or absence)
//     [Cc]  Closing Cleanup (resource teardown)
//     [X1]  Modification Policy
//     [X2]  Extension Points
//     [X3]  Troubleshooting
//     [X4]  Reference
//     [X5]  Closing Note
```

**Files:**
1. `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/lib/handlers/rust.ts`
   - `structuralScaffoldRust()` lines ~2967-2971: Add CLOSING block overview:
     a. Purpose statement: `// Closing ensures correctness, documents constraints, and anchors the file.`
     b. Section index using `[Cv]` bracket format (not bare `Cv`) to avoid zone detection false-positives
   - Add CLOSING section description map or extend `SECTION_DESCRIPTIONS` for Cv, Ce, Cc, X1-X5

**Depends on:** Nothing (can run in parallel with Phases 3-4).

**Verification:**
- `deno task test` passes
- Transform exists.rs: CLOSING has overview with bracket-format index

---

### Phase 6: X1 Parseable Fields (No Square Brackets)

**Goal:** Generate X1 with parseable `// safe:`, `// careful:`, `// never:` lines (NO square brackets around items).

**Current transformer output:**
```
// never: [Break 4-block structure, Remove block boundaries, ...]
// careful: [Function signatures (breaks callers), ...]
// safe: [Function bodies, New functions, Comments, Tests]
```

**Target (matching exists.rs):**
```
// safe: Function bodies, New predicate functions, Doc comments and examples, Tests in Cv
//   - Function bodies (implementation details behind stable signatures)
//   - New predicate functions (additive -- extends the API without breaking it)
//   ...
//
// careful: ensure_dir error types, Error message wording, Function return types, Identity accessor signatures
//   - ensure_dir error types -- callers may match on io::ErrorKind::AlreadyExists
//   ...
//
// never: 4-block structure, PRAGMA/METADATA layout, Public function signatures, ensure_dir error distinction
//   - 4-block structure or block boundary markers
//   ...
```

**Files:**
1. `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/lib/handlers/rust.ts`
   - Lines ~3009-3013: X1 generation in `structuralScaffoldRust()`
   - Remove square brackets from default values
   - Add opening scripture line before policy fields
   - Change format to: header line (comma-separated items), then indented bullet expansions
   - For auto-derivation: scan `pub fn` signatures for "never" items, scan struct fields for "careful" items

2. `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/lib/handlers/rust.ts`
   - Linter: if `checkClosingZoneContent()` validates X1, ensure it recognizes the new format (no brackets)

**Depends on:** Phase 5 (CLOSING overview should be in place).

**Verification:**
- `deno task test` passes
- X1 fields do NOT contain `[` or `]` brackets
- Linter does not treat X1 items as placeholders

---

### Phase 7: X5 Parseable Fields

**Goal:** Generate X5 with parseable `// scripture:`, `// note:`, `// anchor:` fields and full scripture quote block.

**Current output:**
```
// note: "exists.rs -- structural scaffold generated by cws-struct transform."
// scripture: "Psalm 139:7-8 -- Whither shall I go from thy spirit?"
// anchor: "TODO: Module purpose statement."
```

**Target (matching exists.rs):**
```
// scripture: "Psalm 139:7-8 -- Whither shall I go from thy spirit?"
//
// "Whither shall I go from thy spirit? or whither shall I flee from
//  thy presence? If I ascend up into heaven, thou art there: if I
//  make my bed in hell, behold, thou art there."
//     -- Psalm 139:7-8 (KJV)
//
// note: "exists.rs -- production-grade filesystem predicates for the config crate."
//
// [2-4 sentence reflection connecting scripture to module purpose]
```

**Files:**
1. `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/lib/handlers/rust.ts`
   - Lines ~3027-3033: X5 generation in `structuralScaffoldRust()`
   - Reorder: scripture first, then note, then anchor
   - For preserved closing: maintain existing field values with new format
   - Auto-derive note from filename + purpose when not preserved

**Depends on:** Phase 6 (CLOSING zone work should be sequential).

**Verification:**
- `deno task test` passes
- X5 has scripture, note, anchor fields in correct order
- Full scripture quote block present when `scripture_text` available from schema defaults

---

### Phase 8: Identity Auto-Derivation

**Goal:** Automatically derive I3/I4 fields from file context.

**Auto-derivable fields:**
| Field | Source |
|-------|--------|
| I3.path | `filePath` relative to project root (strip `.a-new-structure/b-word/`) |
| I3.provides | Scan file for `pub fn` declarations, collect names |
| I3.component | Crate name from nearest Cargo.toml + module purpose from first doc comment |
| I3.brief | First `//!` line (already extracted as `purpose_line`) |
| I4.layer | Extract from path (L0, L1, L2, etc.) |
| I4.position | Extract from path (universal, omnicode, platform) |
| I4.pattern | Derive from subtype (module -> "utility module", library -> "crate root") |

**Files:**
1. `/media/seanje-lenox-wise/Project/Bereshit/.a-new-structure/c-tov/cws-struct/lib/handlers/rust.ts`
   - `extractMetadataContext()` (line 2364) -- add derivation logic:
     a. `I3.path`: regex on filePath
     b. `I3.provides`: new function `extractPublicFunctions(lines: string[])` scanning for `pub fn (\w+)`
     c. `I3.component`: read Cargo.toml name field from parent directories
     d. `I3.brief`: already captured as `purpose_line`
     e. `I4.layer/position/pattern`: regex on path segments

**Depends on:** Phase 3 (METADATA block must support the full field set).

**Verification:**
- `deno task test` passes
- Transform exists.rs with `--force`: I3.path = `L0-universal/c-hybrid/spec/config/src/exists.rs`, I3.provides = `file_exists, dir_exists, is_file, ensure_dir`

---

## Execution Order (Critical Path)

```
Phase 0 (Bug Fix)        Phase 1 (Schema Data)
     |                        |
     v                        v
Phase 4 (RO Format) <--- Phase 2 (Schema Extraction)
                              |
                              v
                         Phase 3 (METADATA Formatting)
                              |
                              v
Phase 5 (Block Overview) Phase 8 (Auto-Derivation)
     |
     v
Phase 6 (X1 Fields)
     |
     v
Phase 7 (X5 Fields)
```

**Parallel opportunities:**
- Phase 0 + Phase 1 can run in parallel (independent concerns)
- Phase 5 can run in parallel with Phases 3-4 (independent CLOSING vs METADATA/RO)
- Phase 8 can run in parallel with Phases 5-7 (extraction vs generation format)

**Sequential dependencies:**
- Phase 1 -> Phase 2 -> Phase 3 (schema data -> extraction -> generation)
- Phase 0 -> Phase 4 (RO bug fix -> RO format upgrade)
- Phase 5 -> Phase 6 -> Phase 7 (CLOSING overview -> X1 -> X5)

## Trade-offs

**Option A: Full schema enrichment (recommended)**
- *Cost:* Larger schema file (+~60 entries), more extraction code
- *Benefit:* Single source of truth. One schema change ripples to transformer, linter, template. Future languages (Go) inherit the pattern.
- *Risk:* Schema becomes complex. Mitigation: keep fill_content as enrichment (not required for linting).

**Option B: Hardcode formatting in transformer**
- *Cost:* Formatting logic embedded in rust.ts, duplicated across languages
- *Benefit:* Faster to implement, no schema changes
- *Risk:* Violates schema=template=linter triangle. Go handler would need separate implementation.

**Recommendation:** Option A. The entire CPI-SI insight is that schema is the single source of truth. Hardcoding formatting abandons the principle. The extra schema complexity is justified by the parallel growth it enables.

**Option C: Column alignment -- static vs dynamic**
- *Static:* Define alignment width in schema (e.g., `"alignment": 17`)
- *Dynamic:* Calculate from max key length at generation time
- *Recommendation:* Dynamic. Different files have different key sets (I3.component vs I3.brief lengths differ). Dynamic alignment produces correct output every time.

## Risks & Mitigation

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Schema expansion breaks existing tests | Medium | High | Phase 2 adds optional fields (backward compatible). Run `deno task test` after each phase. |
| RO format change breaks existing test assertions | High | Medium | Phase 4 must update all RO-related test expectations. Count and update each one. |
| Column alignment inconsistency | Low | Low | Unit test with known inputs verifying alignment. |
| X1 bracket removal triggers linter false positives | Medium | Medium | Phase 6 includes linter check update. Verify X1 content rules accept bracketless format. |
| Auto-derivation produces wrong I3.path | Medium | Medium | Phase 8 includes path normalization tests with multiple project structures. |
| Schema fill_content grows too complex | Low | Low | Keep groups/headers as optional enrichment. Core linting unaffected. |

## File Impact Summary

| File | Phases | Changes |
|------|--------|---------|
| `rust-4block-schema.jsonc` | 1 | +~80 lines: complete I/C field inventory, group markers, headers, docstrings |
| `code-schema.ts` | 2 | +~30 lines: interface extension, extraction for new fields |
| `rust.ts` | 0, 3, 4, 5, 6, 7, 8 | ~300 lines changed: METADATA formatter, RO formatter, CLOSING overview, X1/X5, auto-derivation |
| `rust_test.ts` | 0, 3, 4, 5, 6, 7, 8 | ~200 lines changed: new tests + updated expectations for format changes |

**Total estimated scope:** ~610 lines across 4 files, 9 phases.
