# Implementation Plan: Schema-Driven METADATA + Content-Aware Linting + Fillable Creation + Native GUI

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost, whether he have sufficient to finish it?"* -- Luke 14:28

**Planned by:** Caleb Navon (Plan Builder)
**Date:** 2026-02-19
**Baseline:** 264 passing tests, 0 failures

---

## Current State

Based on thorough research of the entire codebase, here is what exists:

### Architecture Overview

```
cws-struct/
  lib/
    foundation/
      types.ts          -- LintResult, FormatHandler, CliOptions
      health.ts         -- Health scoring (container/block/file)
      schema.ts         -- TOML 3-block schema loader (loadRules)
      code-schema.ts    -- Go/Rust 4-block schema loader (loadCodeRules) -- SETUP + BODY ONLY
      mod.ts            -- Barrel re-exports
    engine/
      mod.ts            -- Registry, discovery, output
    handlers/
      shared/
        types.ts        -- Shared types + HARDCODED I/C constants
        code-4block.ts  -- Shared functions (findBlocks, validateICFieldContent, etc.)
        mod.ts          -- Barrel
      go.ts             -- Go handler (~900 lines)
      rust.ts           -- Rust handler (~900 lines)
      toml.ts           -- TOML handler (~600 lines, fully schema-driven)
    studio/
      serve.ts          -- Web server (REST API for lint/create/formats)
    verify/
      env.ts            -- Environment verification
  public/               -- Static web frontend (HTML/CSS/JS)
  tests/                -- 264 tests
  mod.ts                -- CLI entrypoint

cwe-creator-tools/
  scaffold-3block.ts    -- Data file scaffolder (TOML, JSON, dotfiles)
  scaffold-4block.ts    -- Code file scaffolder (Go, Rust)
  create.ts             -- Interactive CLI wizard
  lint-seed.ts          -- Seed template linter
```

### The Gap: What Is and What Should Be

| Area | Current State | Target State |
|------|---------------|--------------|
| **METADATA schema** | Go/Rust schemas define METADATA block richly (I/C sections, doc_comments, subtypes, content rules) -- BUT `code-schema.ts` IGNORES the METADATA section, only reads SETUP + BODY | `code-schema.ts` also loads METADATA definitions; handlers read I/C requirements FROM schema |
| **I/C field requirements** | HARDCODED in `shared/types.ts` lines 181-200 (`PRAGMA_FIELD_REQUIREMENTS`, `METADATA_FIELD_REQUIREMENTS`) | Loaded from schema at runtime -- single source of truth |
| **Content validation rules** | HARDCODED in `shared/types.ts` lines 260-296 (`PRAGMA_CONTENT_RULES`, `METADATA_CONTENT_RULES`) | Loaded from schema -- patterns, enums, severity all schema-defined |
| **Content awareness** | Checks "is field present?" and "does value match pattern?" -- no context awareness | Template vs derived distinction, subtype-aware validation, cross-field consistency |
| **File creation** | `scaffold-4block.ts` does regex replacement on comment-block metadata ONLY; does NOT update Pragma/Metadata vars | Full fillable system -- replaces I/C var fields, supports all metadata from args |
| **GUI** | Web-only server (`lib/studio/serve.ts` + `public/`) | Tauri v2 native app wrapping the same frontend, adapter pattern from CWS Dashboard |

### Critical File Counts

| Category | Files | Affected by This Plan |
|----------|-------|-----------------------|
| **Schemas (JSONC)** | 11 schema files | 3 (go-4block, rust-4block, toml-3block) |
| **Foundation (TS)** | 5 files | 3 (code-schema.ts, schema.ts, mod.ts) |
| **Handlers (TS)** | 5 files | 4 (shared/types.ts, shared/code-4block.ts, go.ts, rust.ts) |
| **Creator tools (TS)** | 4 files | 2 (scaffold-4block.ts, create.ts) |
| **Studio (TS + HTML)** | 2 files | 1 (serve.ts) |
| **Tests** | 264 tests | ~30 need updating, ~40 new tests |
| **Templates** | 3 (go, rust, toml) | 0 (read-only -- schema describes them) |
| **New files** | 0 | ~4 (Tauri scaffolding is design-only) |

---

## Phases

### Phase 1: Schema Enrichment -- METADATA Block Loading (Foundation)

**Goal:** Extend `code-schema.ts` to ALSO load the METADATA block definition from Go/Rust schemas, producing handler-ready rules for I/C field requirements, content rules, doc comment expectations, and subtype definitions.

**Why first:** Everything downstream depends on this. The linter evolution, content awareness, and fillable creation all need schema-driven I/C definitions. Without this, all other phases hardcode.

**Files (in order):**

| # | File | Change |
|:-:|------|--------|
| 1 | `lib/foundation/code-schema.ts` | Add `extractMetadataBlock()` function that reads `METADATA.identity_statics.pragma_sections` and `metadata_sections` from the schema. Add `extractDocComments()`, `extractSubtypes()`, `extractContentRules()`. Extend `Code4BlockRules` interface with new fields: `pragmaFieldRequirements`, `metadataFieldRequirements`, `contentRules`, `docCommentExpectations`, `subtypeDefinitions`. The `loadCodeRules()` function calls these new extractors and populates the expanded return type. |
| 2 | `lib/foundation/mod.ts` | Re-export new types if any new type interfaces are added. |

**Depends on:** Nothing -- this is foundation.

**Verification:**
- `deno task test` -- all 264 tests still pass (backward compatible, old constants still exist)
- New unit test: `tests/foundation/code_schema_test.ts` that loads Go schema and asserts `pragmaFieldRequirements.I1.required` equals `["key", "format", "from", "at"]`
- Same test for Rust schema
- Both schemas produce identical I/C field requirements (they should -- the I/C standard is format-agnostic)

**Complexity:** Medium. The schema already has the data (Go schema lines 295-309, Rust schema lines 275-289). This is extraction + compilation, not invention.

**Estimated new tests:** 8-10

---

### Phase 2: Linter Migration -- Schema-Driven I/C Validation (Build Up)

**Goal:** Replace hardcoded `PRAGMA_FIELD_REQUIREMENTS`, `METADATA_FIELD_REQUIREMENTS`, `PRAGMA_CONTENT_RULES`, and `METADATA_CONTENT_RULES` in `shared/types.ts` with values loaded from Phase 1's schema extraction. Handlers read from schema, not constants.

**Why second:** This is the payoff of Phase 1. The hardcoded constants become deprecated, and the schema becomes the single source of truth for the METADATA block -- completing the triangle (schema defines, linter validates, template demonstrates).

**Files (in order):**

| # | File | Change |
|:-:|------|--------|
| 1 | `lib/handlers/shared/types.ts` | Mark `PRAGMA_FIELD_REQUIREMENTS`, `METADATA_FIELD_REQUIREMENTS`, `PRAGMA_CONTENT_RULES`, `METADATA_CONTENT_RULES` as deprecated (keep for backward compat during transition). Add comment: "Superseded by schema-driven loading via code-schema.ts". Remove the hardcoded values ONLY after handlers are migrated. |
| 2 | `lib/handlers/go.ts` | In the `_goRules` top-level await, the handler already loads `await loadCodeRules("go")`. Change `validateICFields()` to read field requirements from `_goRules.pragmaFieldRequirements` and `_goRules.metadataFieldRequirements` instead of importing from `shared/types.ts`. Same for content rules in `validateICFieldContent()` calls. |
| 3 | `lib/handlers/rust.ts` | Same migration as Go -- read I/C requirements from `_rustRules` instead of shared constants. |
| 4 | `lib/handlers/shared/types.ts` | After both handlers are migrated, remove the deprecated constants. Keep the TYPE definitions (`FieldContentRule`, `IdentityField`) -- those are structural, not data. |
| 5 | `lib/handlers/shared/code-4block.ts` | `validateICFieldContent()` function signature stays the same (it takes `rules` as parameter). No change needed -- it's already parameterized. |

**Depends on:** Phase 1.

**Verification:**
- `deno task test` -- all 264 tests pass (behavior identical, source changed)
- Grep for `PRAGMA_FIELD_REQUIREMENTS` -- should appear ONLY in deprecated exports for tests, not in handler logic
- New test: modify a schema field requirement and verify the linter reflects the change (proves schema-driven-ness)

**Complexity:** Medium. The function signatures already accept rules as parameters. This is wiring, not redesign. The risk is in the test fixtures that import these constants directly -- those need updating.

**Estimated test changes:** ~15 existing tests need import path updates; 5 new tests.

**Risk: Tests importing deprecated constants**
- Tests like `go_test.ts` and `rust_test.ts` import `PRAGMA_FIELD_REQUIREMENTS` and `METADATA_FIELD_REQUIREMENTS` directly
- Mitigation: Keep re-exports from handlers (`export { PRAGMA_FIELD_REQUIREMENTS }` on go.ts line 74) but source them from schema instead of hardcoded constants
- Alternative: Tests import from schema loader directly

---

### Phase 3: Content-Aware Validation (Build Up)

**Goal:** Evolve the linter from structural to content-aware. Distinguish template values from derived values. Validate subtype consistency. Cross-field checks. Doc comment quality.

**Why third:** Phases 1-2 make the linter schema-driven. Phase 3 uses that foundation to add intelligence -- the schema already defines what's valid, now the linter checks deeper.

**Files (in order):**

| # | File | Change |
|:-:|------|--------|
| 1 | `lib/handlers/shared/code-4block.ts` | Add `validateTemplateVsDerived()` -- when `isTemplate` is true, certain field values are expected (e.g., I1.at = "template", C1.status = "Template"). When false, template placeholder values should NOT appear. Add `validateSubtypeConsistency()` -- I2.subtype value should match the detected subtype from the file's pragma directive. Add `validateCrossFieldConsistency()` -- I1.format should match the file's actual format (Go file should have I1.format = "go"). |
| 2 | `lib/handlers/go.ts` | Call the new shared validation functions. Add Go-specific doc comment quality check using the schema's `doc_comments` section (package_doc, pragma_doc, metadata_doc). |
| 3 | `lib/handlers/rust.ts` | Same as Go. Add Rust-specific `//!` doc comment check. |
| 4 | Schema enrichment (if needed) | The Go schema already defines `doc_comments.package_doc`, `pragma_doc`, `metadata_doc` with severity levels. If `code-schema.ts` doesn't extract these in Phase 1, extend it here. |

**Depends on:** Phase 2.

**Verification:**
- `deno task test` -- existing tests pass
- New fixture files: `tests/fixtures/go/metadata/bad-content-values.go` (already exists!), add `template-with-derived-values.go`, `derived-with-template-values.go`, `wrong-subtype.go`
- New fixture: `tests/fixtures/rust/metadata/bad-content-values.rs` (already exists!)
- Each fixture tests a specific content-awareness scenario

**Complexity:** Medium-High. Template vs derived detection exists (`isTemplate` on BaseFileContext). Subtype detection exists (`subtype` on BaseFileContext). Cross-field is new logic but straightforward (compare parsed values).

**Estimated new tests:** 15-20

**Risk: False positives on existing files**
- Existing Go/Rust files in the project may have minor inconsistencies that new checks catch
- Mitigation: New checks at `info` severity initially, promoted to `warn` after validation across the codebase
- Run `cws-struct lint go` and `cws-struct lint rust` against actual project files to measure false positive rate before promoting severity

---

### Phase 4: Fillable File Creation (Build Up)

**Goal:** Extend `scaffold-4block.ts` to replace METADATA values in Pragma/Metadata vars (I/C fields), not just comment-block metadata. Add `--scripture`, `--organization`, `--component` args. Support fillable field specification from schema.

**Why fourth:** Phases 1-3 make the linter smart about content. Phase 4 uses the same schema definitions to drive creation -- the schema says what fields exist, creation fills them, the linter verifies them. Triangle complete.

**Files (in order):**

| # | File | Change |
|:-:|------|--------|
| 1 | `cwe-creator-tools/scaffold-4block.ts` | Extend `ScaffoldOptions` with optional fields: `component`, `scripture`, `organization`, `path`, `subtype`. Extend `scaffoldFromTemplate()` to find and replace values in `var Pragma = [][2]string{...}` (Go) and `pub static PRAGMA: &[(&str, &str)] = &[...]` (Rust). Each I/C field in the template gets updated when the user provides a corresponding arg. Fields the user doesn't provide keep the template's self-describing value. Add `--no-verify` support already exists. Add `--component`, `--scripture`, `--organization`, `--path`, `--subtype` CLI flags. |
| 2 | `cwe-creator-tools/scaffold-3block.ts` | Same pattern for TOML -- replace `[_metadata.I1_core]` table values from CLI args. |
| 3 | `cwe-creator-tools/create.ts` | Add metadata step fields for the new options. The interactive wizard already collects key, title, purpose, version. Add optional: component, scripture, organization. These go into `WizardState` and feed through to scaffolder args. |
| 4 | `lib/studio/serve.ts` | Update `/api/create` endpoint to accept the new fields. |

**Depends on:** Phase 2 (needs schema-driven field definitions to know which fields are fillable). Can start after Phase 2 is verified -- does not require Phase 3.

**Verification:**
- Scaffold a Go file with all args: `scaffold-4block.ts go test.go --key B-test --title "Test" --component "Testing" --scripture "Psalm 23:1"`
- Verify the scaffolded file's Pragma var has `{"I3.component", "Testing"}` and Metadata var has `{"C3.scripture", "Psalm 23:1"}`
- Run `cws-struct lint go test.go` -- should pass with 0 errors
- Same for Rust
- Interactive wizard smoke test

**Complexity:** Medium. The hardest part is reliable regex replacement within Go slice literals and Rust static arrays. The patterns are consistent (each entry is `{"key", "value"}` for Go and `("key", "value")` for Rust), but multi-line replacement needs care.

**Estimated new tests:** 8-12

**Risk: Template format drift**
- If templates change their I/C field format, the scaffolder's regex breaks
- Mitigation: Schema defines the expected format (slice_pattern in schema). Scaffolder reads this. If template format changes, schema changes, scaffolder adapts.

---

### Phase 5: Native GUI Design Document (Design Only)

**Goal:** Produce a design document for a Tauri v2 native application that wraps the CWS Studio frontend. NOT implementation -- architectural decisions, component design, API mapping, and a phased build plan.

**Why last:** The GUI is a consumer of everything built in Phases 1-4. The design needs to understand what the final API surface looks like before specifying the GUI that calls it.

**Deliverable:** A design document (not code) covering:

| Section | Content |
|---------|---------|
| **Architecture** | Tauri v2 wrapping `public/` frontend. Adapter pattern from CWS Dashboard (TauriAdapter for IPC, WebAdapter for dev mode). Deno backend runs as sidecar or subprocess. |
| **Platform** | Desktop app (Linux primary, macOS/Windows via Tauri cross-compile). ExFAT considerations for project drive. |
| **Frontend** | Extend existing `public/` (vanilla HTML/CSS/JS). Three views: Create (wizard), Validate (lint results), Formats (documentation). Add: File browser, multi-file lint, health dashboard. |
| **Backend API** | Existing REST endpoints (`/api/lint`, `/api/create`, `/api/formats`, `/api/seeds`, `/api/health`). Tauri IPC commands mirror these. |
| **IPC Design** | Tauri commands: `lint_file`, `lint_directory`, `create_file`, `get_formats`, `get_seeds`, `get_health`. Each maps to existing Deno functions. |
| **Data Flow** | User action -> Tauri command -> Deno sidecar -> cws-struct engine -> Response -> Frontend render. |
| **Sidecar** | Deno compiled binary (`deno compile`) runs as Tauri sidecar. Communicates via stdio JSON-RPC or HTTP localhost. |
| **Phase 0** | Tauri v2 scaffold, frontend bundle, sidecar setup, one working command (formats). |
| **Phase 1** | Lint integration -- single file, then directory. Health score display. |
| **Phase 2** | Create integration -- wizard maps to Tauri commands. |
| **Phase 3** | Multi-file operations, file browser, drag-and-drop. |
| **ExFAT** | Compiled sidecar binary needs execute permissions. Solution: Build on native FS (same as CWS Dashboard's Cargo target-dir workaround). Tauri build target dir on native FS. |
| **Prior Art** | CWS Dashboard already solves Tauri + exfat. Reuse: `src-tauri/` structure, `.cargo/config.toml` target-dir, npm `--no-bin-links`, `ESBUILD_BINARY_PATH`. |

**Depends on:** Phases 1-4 (API surface must be stable).

**Verification:** Document reviewed by Seanje. No code to test.

**Complexity:** Low (design), High (future implementation).

**Files:**

| # | File | Change |
|:-:|------|--------|
| 1 | New: `word/claude/plans/cws-studio-native-design.md` | Design document |

---

## Trade-offs

### Option A: Schema-First (Recommended)
- **Approach:** Enrich schema first (Phase 1), then migrate linter (Phase 2), then add intelligence (Phase 3), then creation (Phase 4)
- **Cost:** 4 sequential phases before GUI design
- **Benefit:** Single source of truth established early. Each phase validates the previous. No rework.
- **Why recommended:** The triangle (schema = template = linter) is the core architectural insight. Building from the schema outward means every consumer (linter, creator, GUI) reads the same truth.

### Option B: Linter-First
- **Approach:** Add content-aware checks first (hardcoded), then extract to schema later
- **Cost:** Two rounds of work -- implement, then refactor to schema
- **Benefit:** Visible results faster
- **Why not:** Creates more hardcoded constants (the problem we're solving). Technical debt accumulates.

### Option C: Parallel Streams
- **Approach:** Schema enrichment + linter evolution + creation in parallel
- **Cost:** Coordination overhead. Merge conflicts. Possible rework if schema shape changes.
- **Benefit:** Faster wall-clock time
- **Why partially viable:** Phases 3 and 4 could run in parallel (they're independent) but both need Phase 2 done first.

**Recommendation:** Option A with Phases 3 and 4 parallelizable after Phase 2 completes.

---

## Risks & Mitigation

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Schema shape doesn't match linter expectations | Low | High | Both schemas already define METADATA block identically to what's hardcoded. Read, don't assume. |
| Existing tests break during migration | Medium | Medium | Phase 2 keeps deprecated exports. Tests updated incrementally, not all at once. Run `deno task test` after every file change. |
| Content-aware checks create false positives | Medium | Low | New checks start at `info` severity. Promoted to `warn` only after full codebase scan. |
| Scaffold regex breaks on edge cases | Medium | Medium | Test with all 3 template subtypes per language (library, executable, demo-test). Fixtures for each. |
| Tauri sidecar adds complexity | Low | Medium | Design document only in Phase 5. Implementation is a separate initiative. CWS Dashboard already solved the hard problems. |
| ExFAT blocks native compilation | Low | Low | Known workaround exists (CWS Dashboard uses target-dir on native FS). Document in design. |

---

## Execution Summary

| Phase | Name | Depends On | Complexity | New Tests | Key Files |
|:-----:|------|-----------|:----------:|:---------:|-----------|
| 1 | Schema Enrichment | -- | Medium | 8-10 | `code-schema.ts`, `mod.ts` |
| 2 | Linter Migration | Phase 1 | Medium | 5 + ~15 updates | `shared/types.ts`, `go.ts`, `rust.ts` |
| 3 | Content-Aware Validation | Phase 2 | Medium-High | 15-20 | `code-4block.ts`, `go.ts`, `rust.ts` |
| 4 | Fillable Creation | Phase 2 | Medium | 8-12 | `scaffold-4block.ts`, `scaffold-3block.ts`, `create.ts` |
| 5 | GUI Design Document | Phases 1-4 | Low | 0 | New design document |

**Total estimated new/changed tests:** ~50-60
**Target test count after completion:** ~315-325
**Critical constraint:** 264 existing tests stay green through every phase.

---

## Verification Commands (Per Phase)

```bash
# Phase 1 -- schema loads METADATA
deno task test:foundation

# Phase 2 -- linter uses schema
deno task test:go
deno task test:rust
deno task test

# Phase 3 -- content-aware checks
deno task test:metadata
deno task test:go
deno task test:rust

# Phase 4 -- fillable creation
deno run --allow-read --allow-write cwe-creator-tools/scaffold-4block.ts go /tmp/test.go --key B-test --title "Test"
deno run --allow-read mod.ts lint go /tmp/test.go

# Phase 5 -- design review
# No code to verify -- document review

# Full regression at any point
deno task test
```

---

> *"Give me this mountain."* -- Joshua 14:12

The mountain is clear. The route is mapped. Foundation first, then build up, then build out. Each phase stands on the previous. The schema is the anchor. Everything else derives.
