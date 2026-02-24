# Implementation Plan: Pipeline Alignment + Test Fun

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost, whether he have sufficient to finish it?"* — Luke 14:28

**Goal:** Bring transform, create, and transpile pipelines to linter maturity for Playtest 3, plus make test runs engaging.

---

## Current State

### What Exists Now (based on actual codebase research)

**Linter (THE benchmark):**
- 1448 tests, 252 error codes, 8 format handlers
- 4-layer cascade (R50/R25/R10/R05), form-aware detection
- Health scoring, schema-driven rules, data-driven architecture
- Error code catalog with dimensional coverage (witness system)
- Verified production-grade across Go, Rust, TOML, JSON, Makefile, Dotfiles, OmniCode, OFD

**Create (`lib/engine/fill.ts`, 439 lines):**
- Generates complete 4-block files from schema `fill_content`
- Form-aware: SETUP sections filtered by form status, BODY sections form-filtered by `bodyData`
- CLOSING: code zones (Cv/Ce/Cc) + required doc zones (X1/X5) with defaults from fill_content
- Works for Go + Rust, all 5 forms (bare-bone, library, module, executable, declared)
- Placeholder substitution engine with context overrides
- **Tests:** 453 lines (`fill_test.ts`) + 56 lines (T00 error path) = ~509 lines total, ~20 tests
- **GAP: No post-generation lint verification test** (the "proof" that schema=template=linter=generator is asserted but never mechanically verified in the test suite)

**Transform (`lib/engine/transform.ts`, 1019 lines):**
- Shared pipeline architecture: universal passes + handler-provided passes
- Universal: fix separator widths (= and --), convert ASCII to Unicode, normalize prefix
- Parameterized: code movement (tests->Cv, main->Ce), subsection scaffolding, subsection reorder
- Schema-driven: CLOSING zone reorder using `closingData.zones`
- Verification loop: re-lints after transform, reports error delta
- Dry-run mode, --extensions gating
- Handler integration: `transformGoFile()` (go.ts:1989), `transformRustFile()` (rust.ts:2726)
- **Tests:** 481 lines (R10 engine) + integration tests (Go: 60+ lines, Rust: 60+ lines) = ~600 lines, ~30 tests
- **GAP: No form-aware transform tests** (scaffold filtering by form is implemented but untested in isolation)
- **GAP: No end-to-end transform->lint verification** (the re-lint loop exists in code but isn't tested)
- **GAP: No TOML/JSON transform support** (only code formats)

**Transpile (`lib/engine/transpiler.ts` + 3 companions, ~3200 lines):**
- Three-cord architecture: Source -> Semantic IR -> Target
- Rust extraction: identity, SETUP chunks, BODY chunks, CLOSING zones
- Go generation: METADATA, SETUP (imports/constants/types), BODY (section-mapped), CLOSING
- Universal Concept AST: 11 concepts (variable_binding, return_value, conditional_if, conditional_match, error_propagation, function_call, method_call, ok_wrap, err_wrap, string_format, self_access)
- Schema-driven mappings: type conversion, import mapping, naming conventions, receiver conventions
- Section mapping: Rust module -> Go library (1 registered pair)
- Body translation: captures function bodies, translates via concept AST
- **Tests:** 694 lines (R10 engine) + 73 lines (T00 error paths) = ~767 lines, ~60 tests
- **GAP: Only Rust->Go direction** (no Go->Rust, no other pairs)
- **GAP: Extracts 0 semantic items from most fixtures** (only structural skeleton, reported in task context)
- **GAP: No round-trip verification** (transpile -> lint to verify output is valid)
- **GAP: Only 1 fixture** (`tests/fixtures/transpiler/rust-module-with-content.rs`)

---

## Scope Assessment

| Area | Files Affected | Risk |
|------|---------------|------|
| **Create alignment** | `lib/engine/fill.ts`, `tests/content/R10/engine/fill_test.ts`, new integration test | Low |
| **Transform alignment** | `lib/engine/transform.ts`, handler transforms in `go.ts`/`rust.ts`, `tests/content/R10/engine/transform_test.ts`, integration tests | Medium |
| **Transpile alignment** | `lib/engine/transpiler.ts`, `transpiler-ast.ts`, `tests/content/R10/engine/transpiler_test.ts`, new fixtures | Medium-High |
| **Test Fun** | New `scripts/test-fun.ts`, `deno.jsonc` task entry | Low |

**Total estimated files:** 10-15 modified, 3-5 new
**Blast radius:** Engine layer only. Handler lint paths untouched. Shared types untouched.

---

## Phases

### Phase 1: Create Pipeline — Lint Proof (Foundation)

**Goal:** Every file `create` generates passes `lint` with 0 errors. Prove it mechanically.

**Existing coverage (already in `tests/integration/pipeline/pipeline_integration_test.ts`):**
- Test T4: Go `library` -> create -> lint Layers 0-1 (PASSES)
- Test T4: Go `executable` -> create -> lint Layer 0 (PASSES)
- Test T3: Go `valid-library` -> transform dry-run -> re-lint (PASSES)
- Test T3: Go `valid-library` -> transform wet-run -> re-lint (PASSES)

**What's missing:** The existing T4 tests only cover Go library + executable at Layers 0-1. We need ALL format x form combinations verified across ALL layers (0-3), plus Rust.

**Files:**
1. `tests/integration/pipeline/pipeline_integration_test.ts` — Extend T4 section with remaining forms
2. `tests/content/R10/engine/fill_test.ts` — Add section-count verification tests

**What to do:**
- Extend T4 in the existing integration test with new form tests:
  - Go: `bare-bone`, `declared`, `template` (library + executable already done)
  - Rust: `module`, `library`, `executable`, `bare-bone`, `declared`, `template`
- Upgrade existing T4 tests from Layer 0-1 only to full Layer 0-3 assertion (all layers should pass for created files)
- Add section-count verification: generated files should have the correct form-specific section counts (e.g., bare-bone SETUP has fewer sections than library SETUP)
- This is THE proof test: if create produces 0-error files across ALL layers, the schema IS the complete spec.

**Depends on:** Nothing — this is foundation.

**Verification:**
- `deno task test:integration` passes
- Existing T4 tests still pass (no regression)
- New tests create 10 additional files (3 Go + 6 Rust + 1 Go upgrade), all lint clean across all 4 layers
- Count of generated sections matches expected per-form section counts

**Estimated new tests:** 12-15 (reduced from 15-20 because 2 Go forms already covered)

---

### Phase 2: Transform Pipeline — Schema Parity (Build from Create)

**Goal:** Transform uses the same schema knowledge as the linter for all decisions.

**Files:**
1. `tests/content/R10/engine/transform_test.ts` — Extend with form-aware tests
2. `tests/integration/pipeline/transform_lint_test.ts` — New: transform -> lint -> verify improvement
3. `tests/integration/go/transform_test.ts` — Extend
4. `tests/integration/rust/transform_test.ts` — Extend

**What to do:**

**2A. Form-aware transform tests (gap):**
- Test that `createSubsectionScaffoldPass` with `formStatus` set to "reserved" for certain sections actually skips those sections
- Test that scaffold respects form ordering (library vs bare-bone produce different scaffold output)
- Test form-aware identity scaffolding in handler transforms

**2B. End-to-end transform verification (gap):**
- Take a fixture with known lint errors (e.g., `rust/format/transform-separators.rs`)
- Lint it -> record error count
- Transform it (actual, not dry-run — on a temp copy)
- Lint again -> assert error count decreased
- This tests the verification loop that already exists in `runTransformPipeline()`

**2C. Transform coverage for all moveable patterns:**
- Test CLOSING zone reorder with more zone configurations
- Test subsection reorder for BODY (currently only SETUP tested)
- Test code movement edge cases: what happens when target block is empty, when source has multiple test functions

**Depends on:** Phase 1 (understanding of form-aware filtering proven by create tests).

**Verification:**
- All existing transform tests still pass
- New form-aware tests confirm reserved sections are skipped
- End-to-end tests confirm error count decreases after transform
- `deno task test:integration` passes

**Estimated new tests:** 20-25

---

### Phase 3: Transpile Pipeline — Extraction Depth (Build from Transform)

**Goal:** Transpiler extracts meaningful semantic items from real Rust fixtures, not just structural skeleton.

**Files:**
1. `tests/fixtures/transpiler/` — Add 3-4 new Rust fixtures with real code content
2. `tests/content/R10/engine/transpiler_test.ts` — Extend extraction tests
3. `tests/integration/pipeline/transpile_lint_test.ts` — New: transpile -> lint -> verify valid output
4. `lib/engine/transpiler.ts` — Fix extraction gaps (if identified by new tests)

**What to do:**

**3A. New fixtures with graduated complexity:**
- `rust-module-simple.rs` — 3 functions, 2 types, clear sections. Baseline extraction.
- `rust-module-with-error-types.rs` — Custom error enum + Result returns. Tests error pattern extraction.
- `rust-module-with-traits.rs` — Trait definition + impl blocks. Tests trait->interface mapping.
- Each fixture must be a valid 4-block Rust file that passes `cws-struct lint rust`

**3B. Extraction depth tests:**
- For each new fixture, test that `extractRustSemantics()` returns:
  - Correct chunk count (matches section count in fixture)
  - Correct item count per chunk (every `pub fn`, `struct`, `use`, `const` captured)
  - Correct item kinds (function, struct, import, constant, etc.)
  - Correct visibility (pub -> public, no pub -> private)
- This directly addresses the "extracts 0 semantic items" gap

**3C. Round-trip verification:**
- Transpile each fixture: `transpileFile(fixture, "rust", "go")`
- Lint the output: assert it passes `cws-struct lint go` with 0 errors (or enumerate acceptable warnings)
- This proves: Rust file -> Go file -> valid Go structure

**3D. Body translation verification:**
- For the simple fixture, ensure functions with bodies produce `translatedBody` (not empty TODO stubs)
- Test that concept AST finds at least one concept in each function body
- Test that emitted Go has correct package imports from body analysis

**Depends on:** Phase 2 (transform parity ensures the verification pipeline is trustworthy).

**Verification:**
- Extraction tests pass with correct counts
- Round-trip tests produce Go files that lint clean
- Body translation produces at least partial translations (not all TODO stubs)
- `deno task test:engine` + `deno task test:integration` pass

**Estimated new tests:** 25-30

---

### Phase 4: Test Fun — The Playtest Experience (Independent)

**Goal:** Make watching 1448+ tests enjoyable. A wrapper that adds personality to the test run.

**Files:**
1. `scripts/test-fun.ts` — New: test runner wrapper with personality
2. `deno.jsonc` — Add `test:fun` task

**What to build:**

A Deno script that wraps `deno test` and enhances the output with:

**Visual progress:**
- Live counter: `[742/1448]` with percentage
- Progress bar using Unicode block characters (`█░`)
- Time elapsed

**Milestone messages (at percentage thresholds):**
- 10%: `"Foundation holds. Blocks are solid."`
- 25%: `"Quarter mark. Structure stands."`
- 50%: `"Halfway. The wall rises."`
- 75%: `"Three quarters. Almost home."`
- 90%: `"Final stretch. Count the cost, build the tower."`
- 100% (all pass): `"And God saw every thing that he had made, and, behold, it was very good. -- Genesis 1:31"`
- 100% (some fail): `"A just man falleth seven times, and riseth up again. -- Proverbs 24:16"`

**Stats at completion:**
- Total tests, passed, failed, time
- Tests per second
- Layer breakdown (R50/R25/R10/R05/T00 counts from test names)
- "New since last run" delta if possible (compare to cached count)

**Implementation approach:**
- Spawn `deno test` as subprocess with `--reporter=dot` or parse stdout
- Parse Deno test output line by line (Deno outputs `ok` / `FAILED` per test)
- Count pass/fail, display progress bar, inject milestone messages
- Use ANSI colors (already have `COLORS` in output.ts)

**Task registration:**
```jsonc
"test:fun": "deno run --allow-read --allow-write --allow-run --allow-env scripts/test-fun.ts"
```

The script internally invokes the same deno test command as `deno task test`.

**Depends on:** Nothing — independent of pipeline work. Can be done in parallel.

**Verification:**
- `deno task test:fun` runs all tests and displays progress
- All 1448+ tests still pass (wrapper doesn't break anything)
- Milestone messages appear at correct thresholds
- Completion summary shows accurate stats

**Estimated effort:** 1 file, ~200-300 lines

---

### Phase 5: Cross-Pipeline Integration Tests (Capstone)

**Goal:** Prove all three pipelines work together and are "as updated as the linter."

**Existing coverage (already in `pipeline_integration_test.ts`):**
- T3: Transform -> re-lint on valid Go (dry + wet) -- DONE
- T4: Create -> lint for Go library + executable -- DONE (expanded in Phase 1)
- T5: Health actions -> log entries -- DONE
- T6: Inspect + lint see same structure -- DONE
- T7-T10: CLI, cross-format, summarize -- DONE

**What's missing:** Create -> Transform chain, Transform on files WITH errors, Transpile -> Transform -> Lint chain, witness coverage for new tests.

**Files:**
1. `tests/integration/pipeline/pipeline_integration_test.ts` — Add T11/T12 sections
2. `tests/integration/pipeline/full_cycle_test.ts` — New: transpile -> transform -> lint cycle

**What to do:**

**5A. Create -> Transform -> Lint cycle (T11):**
- For each form: Create -> Transform (should be no-op since create output is already clean) -> Lint
- This verifies create and transform agree on what "correct" looks like
- Add to existing `pipeline_integration_test.ts` as Test Group 11

**5B. Transform -> Lint improvement cycle (T12):**
- Start with a deliberately messy fixture (e.g., wrong separator widths, ASCII dashes)
- Lint -> record baseline error count
- Transform it (actual, not dry-run -- on a temp copy)
- Lint again -> assert error count decreased
- Add to existing `pipeline_integration_test.ts` as Test Group 12

**5C. Transpile -> Transform -> Lint full chain:**
- Transpile Rust fixture to Go
- Transform the Go output (fix any separator/formatting issues)
- Lint the final Go file
- Assert valid output
- New file `full_cycle_test.ts` — keeps transpile dependency separate

**5D. Witness coverage update:**
- Ensure new error codes (T00 tool-layer) are witnessed by new tests
- Run `deno task witness:grid` and verify coverage didn't regress

**Depends on:** Phases 1-3 complete.

**Verification:**
- Full cycle tests pass
- No witness coverage regression
- `deno task test` passes (all 1448+ existing + ~65 new tests)

**Estimated new tests:** 8-12 (reduced from 10-15 because base integration already strong)

---

## Trade-offs

| Option | Cost | Benefit | Recommendation |
|--------|------|---------|----------------|
| **A: Fix transpiler extraction first** | High risk — deep parser changes | More semantic items extracted | Defer to after playtest 3. Current skeleton output is valid. |
| **B: Add new format pairs (Go->Rust)** | Medium — new extraction + generation | Bidirectional transpilation | Defer. Rust->Go is the flagship. |
| **C: Focus on verification tests** | Low risk — read-only tests against existing code | Proves what works, finds what doesn't | **Recommended.** This is the path. |

**Recommendation:** Option C — **verification-first approach.** The code works. The tests proving it works are what's missing. Every phase adds tests that verify existing functionality before extending it. This is the lowest-risk path to "as updated as the linter" because the linter's maturity came from thorough testing, not from more features.

---

## Risks & Mitigation

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Create output has subtle lint errors in edge-case forms | Medium | Low | Phase 1 tests will catch them immediately; fix in fill.ts |
| Transform verification loop has edge cases | Low | Medium | Phase 2 tests on temp files, not fixtures (prevent fixture corruption) |
| Transpiler extraction yields 0 items on new fixtures too | Medium | High | Phase 3A fixtures designed with known extractable patterns; if extraction fails, it's a targeted fix in `classifyRustItem()` |
| Test fun wrapper breaks on Deno test output format changes | Low | Low | Parse loosely; fall through to raw output if parsing fails |
| Phase 5 integration tests are slow | Medium | Low | Use `--filter` patterns; keep integration tests under 10 seconds total |

---

## Summary

| Phase | Tests Added | Risk | Core Deliverable |
|-------|------------|------|------------------|
| 1. Create Lint Proof | 12-15 | Low | Every `create` output lints clean |
| 2. Transform Parity | 20-25 | Medium | Form-aware + end-to-end verification |
| 3. Transpile Depth | 25-30 | Medium-High | Real extraction + round-trip validation |
| 4. Test Fun | 0 (1 script) | Low | Engaging test runner experience |
| 5. Integration Capstone | 8-12 | Low | Full pipeline cross-verification |

**Total new tests:** ~65-82
**Total new files:** 5-7 (test files + fixtures + 1 script)
**Total modified files:** 5-6 (existing test files + deno.jsonc)

**Verified baselines (2026-02-24):**
- Current test count: 1448 passed, 0 failed, 12 ignored (confirmed by `deno task test`)
- 1236 explicit `Deno.test` calls + data-driven runner generates ~212 additional test cases
- Existing `pipeline_integration_test.ts` covers: create Go library/executable, transform dry/wet on valid Go, logging, inspect, CLI, cross-format, summarize (Tests T1-T10, ~30 tests)
- Transpiler fixtures: 1 (`rust-module-with-content.rs`)

The pipeline alignment is primarily a **testing gap**, not a **code gap.** The code works — the proof that it works to linter standards is what's missing. This plan fills that proof.
