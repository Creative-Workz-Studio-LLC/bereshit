# CWS Struct — Test Infrastructure & QoL Design

> *"Prove all things; hold fast that which is good."* — 1 Thessalonians 5:21

**Author:** Nathan Emet (CPI-SI)
**Date:** 2026-02-24
**Status:** Design Proposal
**Scope:** Test infrastructure for non-lint commands + output QoL upgrade

---

## Current State Assessment

### What Exists (1448 tests, 0 failures)

| Component | Coverage | Quality |
|-----------|----------|---------|
| **Lint** | Deep — data-driven `_runner.ts`, R50/R25/R10/R05/T00 hierarchy | Production-grade. Fixture caching, rich diagnostics, auto-witness. |
| **Transform** | Moderate — 8 Go tests, 12+ Rust tests in `integration/{go,rust}/` | Good patterns. Dry-run detection + wet-run + re-lint verification. |
| **Create** | Minimal — 2 tests in `integration/pipeline/` (Go library + executable) | Basic round-trip: create → lint → 0 errors at Layer 0-1. |
| **Transpile** | Minimal — 3 T00 error-path tests + 1 fixture | Schema-loading errors only. No end-to-end transpile verification. |
| **Inspect** | Moderate — 4 tests in `integration/pipeline/` (Go + Rust + JSON round-trip) | Structural verification, format consistency. |
| **Watch** | None | No tests. |
| **Query** | 1 ignored (FFI crash) | Database layer excluded from suite. |

### What's Strong

1. **`_runner.ts` is genuinely excellent.** Data-driven, table-based, error-catalog-authoritative. A test is a row. The runner generates `Deno.test()` calls, severity tags, fixture caching, suite summaries, and clean counterparts. This is the gold standard.

2. **`helpers.ts` is comprehensive.** 30+ exported helpers covering filtering, grouping, error code resolution, mock builders, and the witness registry. Depth-independent via `@test/` import map.

3. **The witness system (static + runtime) is clever.** Two angles on the same data — `scripts/witness.ts` for static scan, `helpers.ts` WITNESS_REGISTRY for runtime. Both track error code → test coverage.

4. **Fixture caching in `lintOnce()`.** Format:path keyed cache with access/reuse stats. A fixture linted once is free for all subsequent tests. This is the right architecture.

5. **The output.ts formatting system** provides ANSI colors, severity icons, health scoring with Hebrew states, per-block/per-layer breakdowns, top issues, and catalog coverage bars. This is what tests should hook into.

### What's Missing

1. **No data-driven runners for transform/create/transpile.** Each command has a fundamentally different verification pattern than lint, but they share the same *principle*: specification as data, verification as code.

2. **No idempotency testing.** Transform should be verifiable as idempotent (transform twice → same result). No tests exist for this.

3. **No round-trip verification matrix.** Create → lint → transform → lint should be a systematic matrix, not ad-hoc individual tests.

4. **No test output QoL.** Deno's default test output is functional but flat. 1448 tests scroll by as a wall of ok/fail. No progress, no satisfaction, no summary that makes you feel the quality.

5. **No config-driven test profiles.** 50+ `deno.jsonc` tasks are hardcoded command strings. Adding a new handler means editing 8+ task entries manually.

---

## Design: Test Architecture for Non-Lint Commands

### Principle: Same Runner Pattern, Different Verification

The `_runner.ts` pattern — specification as data, verification as code — applies to every command. The difference is **what you verify**.

| Command | Input | Operation | Verification |
|---------|-------|-----------|-------------|
| **Lint** | Fixture file | `handler.lint()` | Error code fires (or doesn't) |
| **Transform** | Fixture file | `handler.transform()` | Dry-run detects, wet-run fixes, re-lint passes |
| **Create** | Fill context | `generateFileText()` | Generated file passes lint at target layers |
| **Transpile** | Source fixture | `transpile()` | Target file has correct structure + passes lint |
| **Inspect** | Fixture file | `handler.inspect()` | Structure matches expectations |

### 1. Transform Test Runner

**File:** `tests/_transform_runner.ts`

```typescript
export interface TransformCase {
  /** Human-readable description. */
  desc: string;
  /** Fixture path relative to tests/fixtures/. */
  fixture: string;
  /** Expected transform rules to fire (partial match on result.rule). */
  expectRules: string[];
  /** Rules that should NOT fire (negative assertions). */
  rejectRules?: string[];
  /** After wet-run + re-lint, these error codes should be gone. */
  fixesCodes?: string[];
  /** If true, verify idempotency: transform twice → no diff. */
  idempotent?: boolean;
  /** Skip reason (WIP / known issue). */
  skip?: string;
}

export interface TransformSpec {
  format: string;
  label?: string;
  cases: TransformCase[];
}
```

**What `runTransformSuite()` generates per case:**

1. **Dry-run test:** Transform with `dryRun: true`. Assert expected rules fire, rejected rules don't.
2. **Wet-run test:** Copy fixture to temp, transform with `dryRun: false`, re-lint, verify `fixesCodes` are resolved.
3. **Idempotency test (optional):** Transform again → diff with previous result → no changes.
4. **Dry-run immutability guard:** Verify fixture file unchanged after dry-run.

**Suite summary:** Cases run, rules fired, codes fixed, idempotency checks passed.

### 2. Create Test Runner

**File:** `tests/_create_runner.ts`

```typescript
export interface CreateCase {
  /** Human-readable description. */
  desc: string;
  /** Format to create (e.g., "go", "rust"). */
  format: string;
  /** Subtype to create (e.g., "library", "executable"). */
  subtype: string;
  /** Minimum layers that must pass lint (0=R50, 1=R25, 2=R10, 3=R05). */
  passLayers: number[];
  /** Specific error codes that must NOT fire. */
  rejectCodes?: string[];
  /** Custom FillContext overrides. */
  fillOverrides?: Partial<FillContext>;
  /** Skip reason. */
  skip?: string;
}

export interface CreateSpec {
  label?: string;
  cases: CreateCase[];
}
```

**What `runCreateSuite()` generates per case:**

1. **Generation test:** Call `generateFileText()` with fill context, write to temp file.
2. **Lint validation test:** Lint the generated file. Assert specified layers pass (0 errors at those layers).
3. **Rejected codes test:** If `rejectCodes` specified, assert none fire.
4. **Cleanup:** Remove temp files.

**The round-trip matrix:**

```
create → lint → (should pass Layers 0-1 minimum)
create → transform → lint → (should pass Layers 0-2)
create → lint → transform → lint → (transform should be no-op on clean file)
```

This matrix is the real quality gate. A clean create that needs transform intervention means either create or lint has a gap.

### 3. Transpile Test Runner

**File:** `tests/_transpile_runner.ts`

```typescript
export interface TranspileCase {
  /** Human-readable description. */
  desc: string;
  /** Source fixture path relative to tests/fixtures/. */
  source: string;
  /** Source format. */
  sourceFormat: string;
  /** Target format. */
  targetFormat: string;
  /** Target should pass lint at these layers. */
  passLayers: number[];
  /** Expected structural elements in target. */
  expectElements?: string[];
  /** Diagnostic assertions (e.g., "no_warnings", "has_stubs"). */
  diagnostics?: string[];
  /** Skip reason. */
  skip?: string;
}

export interface TranspileSpec {
  label?: string;
  cases: TranspileCase[];
}
```

**What `runTranspileSuite()` generates per case:**

1. **Transpile test:** Call `transpile()` with source fixture. Write result to temp.
2. **Structure test:** Lint target with target format handler. Assert specified layers pass.
3. **Element test:** If `expectElements`, check target content for expected structural markers (block names, section headers, function stubs).
4. **Diagnostic test:** Handle diagnostic assertions (e.g., all function bodies have TODO comments, no warnings from transpile).

### 4. Shared Patterns Across All Runners

All runners share:

| Pattern | Implementation |
|---------|---------------|
| **Temp file management** | Shared `withTempFile(format, fn)` helper — creates temp, calls fn, cleans up. |
| **Suite summary test** | Same pattern as `_runner.ts` — fixture stats, cases run, pass/fail. |
| **Fixture caching** | Extend `lintOnce()` to cache transform dry-run results too. |
| **Witness integration** | Error codes that transform/create/transpile tests prove → witness registry. |
| **Bracket tags** | `[CWS-T00-112]` style tags on generated tests for `--filter` targeting. |

---

## Design: Test Output QoL

### Philosophy: Fun = Feedback Loop Satisfaction

Game design principle: **the loading screen teaches while you wait.** The test output should give the developer a sense of progress, quality, and achievement. Not gimmicky — *satisfying*.

What makes test output satisfying:

1. **Progress awareness.** You know where you are in the suite.
2. **Speed perception.** Slow tests are named; fast tests are aggregated.
3. **Victory conditions.** The final summary feels like a score screen.
4. **Failure diagnostics.** When something breaks, you know *exactly* where and why (this already exists in `_runner.ts` — extend it).

### Current Output Infrastructure (`lib/engine/output.ts`)

Already provides:

| Feature | Implementation |
|---------|---------------|
| ANSI colors | `COLORS.red`, `.green`, `.yellow`, `.blue`, `.dim`, `.bold` |
| Severity icons | `severityIcon()` → `E`, `W`, `I` |
| Health scoring | `healthColor()` → green (34+), yellow (0-33), red (<0) |
| Per-file summaries | `printFileSummary()` with health, anatomy, why chains |
| Layer breakdowns | `printLayerBreakdown()` — R50/R25/R10/R05 pass/fail per file |
| Block breakdowns | `printBlockBreakdown()` — METADATA/SETUP/BODY/CLOSING averages |
| Top issues | `printTopIssues()` — most frequent errors across files |
| Catalog coverage | `printCatalogCoverage()` — bar chart of codes triggered per layer |
| Header | Scripture quote, file count, format |

### Proposed: Test Output Reporter

**File:** `tests/_reporter.ts`

A test reporter that hooks into Deno's test lifecycle via `Deno.test` wrapper or post-test processing.

#### Phase 1: Suite Summary Enhancement

The existing suite summary test pattern in `_runner.ts` is the right architecture. Extend it:

```typescript
// At end of test file, after all suites run:
Deno.test("=== Suite Report ===", async (t) => {
  const records = getSuiteRecords();
  const fixtureStats = getFixtureStats();

  // 1. Overview step
  await t.step(`${totalCases} cases across ${records.length} suites`, () => {});

  // 2. Cache efficiency
  const totalAccesses = sum(fixtureStats, s => s.accesses);
  const totalReuses = sum(fixtureStats, s => s.reuses);
  const hitRate = ((totalReuses / totalAccesses) * 100).toFixed(0);
  await t.step(`fixture cache: ${hitRate}% hit rate (${totalReuses}/${totalAccesses})`, () => {});

  // 3. Witness coverage
  const witnessed = witnessCount();
  await t.step(`witness coverage: ${witnessed.codes} codes, ${witnessed.tests} tests`, () => {});

  // 4. Per-layer summary
  for (const layer of ["R50", "R25", "R10", "R05", "T00"]) {
    const layerRecords = records.filter(r => r.label.startsWith(layer));
    const layerCases = sum(layerRecords, r => r.totalCases);
    if (layerCases > 0) {
      await t.step(`${layer}: ${layerCases} cases`, () => {});
    }
  }
});
```

This adds zero external dependencies. It uses Deno's native `t.step()` which already formats nicely with indentation.

#### Phase 2: Progress Indicators

For long-running suites, add timing:

```typescript
// In runSuite(), wrap each test with timing:
const start = performance.now();
// ... test body ...
const elapsed = performance.now() - start;
// Tag slow tests (>100ms) in the suite record
```

Suite summary then reports:
```
R50 suite — 14 cases, rust
  7 fixtures (4 cache reuses)
  7 codes witnessed
  fastest: 2ms (valid-library: zero errors)
  slowest: 89ms (wrong-block-order: block/order error)
```

#### Phase 3: Post-Run Scorecard

**File:** `scripts/test-report.ts`

A standalone script (like `witness.ts`) that runs after `deno task test` and produces a rich scorecard:

```
╔══════════════════════════════════════════════════════════════╗
║             CWS Struct — Test Scorecard                      ║
║             1 Thessalonians 5:21                             ║
╚══════════════════════════════════════════════════════════════╝

  Total:     1448 passed  0 failed  12 ignored
  Duration:  35.2s (41 tests/sec)

  ── By Layer ──────────────────────────────────────────────────
  R50  ████████████████████  312 tests  (21.5%)
  R25  ██████████████░░░░░░  248 tests  (17.1%)
  R10  ████████████████████  398 tests  (27.5%)
  R05  ████████████░░░░░░░░  196 tests  (13.5%)
  T00  ██████████░░░░░░░░░░  142 tests  ( 9.8%)
  int  ██████░░░░░░░░░░░░░░   88 tests  ( 6.1%)
  cli  ██░░░░░░░░░░░░░░░░░░   24 tests  ( 1.7%)
  unit ████░░░░░░░░░░░░░░░░   40 tests  ( 2.8%)

  ── By Command ────────────────────────────────────────────────
  lint      ████████████████  1286 tests  (88.8%)
  transform ████░░░░░░░░░░░░    62 tests  ( 4.3%)
  create    █░░░░░░░░░░░░░░░    12 tests  ( 0.8%)
  transpile █░░░░░░░░░░░░░░░     4 tests  ( 0.3%)
  inspect   █░░░░░░░░░░░░░░░     6 tests  ( 0.4%)
  other     ██░░░░░░░░░░░░░░    78 tests  ( 5.4%)

  ── Witness Coverage ──────────────────────────────────────────
  R50  42/73   (57%)   ████████████░░░░░░░░
  R25  61/99   (62%)   ████████████░░░░░░░░
  R10  38/61   (62%)   ████████████░░░░░░░░
  R05  29/71   (41%)   ████████░░░░░░░░░░░░
  T00  18/70   (26%)   █████░░░░░░░░░░░░░░░

  ── Fixture Efficiency ────────────────────────────────────────
  Unique fixtures: 87
  Cache hit rate:  73%
  Avg reuses:      4.2x

  ── Health ────────────────────────────────────────────────────
  Suite state: tov (perfect)  +100
  All layers passing. All commands green. Ship it.
```

This is the "game score screen." You see your progress, you see the gaps, you feel the quality. The bar charts use the same `COLORS` system from `output.ts`.

The script parses Deno's JSON test output (`deno test --reporter=json`) and transforms it into this view. No changes to test code needed.

#### Phase 4: Watch Mode Enhancement

For `deno task test:watch`, the report auto-updates on change. Like a game's HUD — persistent awareness without cognitive overhead.

---

## Design: Config-Driven Test Profiles

### Problem

50+ tasks in `deno.jsonc` are hand-maintained command strings. Adding a handler means touching 8+ entries. The pattern is `deno test --allow-read --allow-write --allow-run --allow-env tests/existence/R50/{handler}/` repeated with variations.

### Solution: Test Profile Registry

**File:** `tests/_profiles.ts` (or `tests/profiles.toml`)

```typescript
export interface TestProfile {
  /** Profile name (used in deno task). */
  name: string;
  /** Human-readable label. */
  label: string;
  /** Directories to include. */
  include: string[];
  /** Directories to exclude. */
  exclude?: string[];
  /** Permissions needed. */
  permissions: string[];
  /** Filter pattern (--filter). */
  filter?: string;
}

export const PROFILES: TestProfile[] = [
  // Layer profiles
  { name: "R50", label: "Existence", include: ["tests/existence/R50/"], permissions: ["--allow-read", "--allow-write", "--allow-run"] },
  { name: "R25", label: "Organization", include: ["tests/organization/R25/"], permissions: ["--allow-read", "--allow-write", "--allow-run"] },

  // Handler profiles
  { name: "go", label: "Go Handler", include: ["tests/existence/R50/go/", "tests/organization/R25/go/", "tests/content/R10/go/", "tests/pattern/R05/go/", "tests/pattern/R03/go/"], permissions: ["--allow-read", "--allow-write", "--allow-run"] },

  // Handler x Layer profiles (generated)
  { name: "go:R50", label: "Go R50", include: ["tests/existence/R50/go/"], permissions: ["--allow-read", "--allow-write", "--allow-run"] },

  // Command profiles
  { name: "transform", label: "Transform", include: ["tests/integration/"], filter: "transform", permissions: ["--allow-read", "--allow-write", "--allow-run"] },
  { name: "create", label: "Create", include: ["tests/integration/pipeline/"], filter: "create", permissions: ["--allow-read", "--allow-write", "--allow-run", "--allow-env"] },
];
```

**Usage:** A thin script reads profiles and executes `deno test` with the right args:

```bash
deno task profile go:R50     # → resolves profile, runs with correct args
deno task profile transform  # → resolves profile, runs transform tests
```

The `deno.jsonc` task entries become a single `profile` task. Profiles live in data, not command strings.

### Migration Path

1. Keep existing `deno.jsonc` tasks (backward compatible).
2. Add `profile` task that reads from `_profiles.ts`.
3. Gradually replace hardcoded tasks with profile references.
4. When new handlers are added, only the profile registry needs updating.

---

## Design: Coverage Extension for Non-Lint Commands

### Extending the Witness System

The witness system currently tracks lint error codes. Extend it to track command coverage:

```typescript
// New witness categories in helpers.ts:
export function witnessTransform(format: string, rule: string, testName: string): void { ... }
export function witnessCreate(format: string, subtype: string, testName: string): void { ... }
export function witnessTranspile(source: string, target: string, testName: string): void { ... }
```

### Transform Coverage Matrix

| Format | dry-run | wet-run | idempotent | re-lint |
|--------|---------|---------|------------|---------|
| Go | move-tests, move-main, reorder-closing | all 3 | not tested | all 3 |
| Rust | separators, move-tests, move-main, reorder-closing, scaffold (6 aspects) | separators, scaffold | not tested | scaffold |
| TOML | none | none | none | none |

**Gap:** TOML has no transform tests. Idempotency is untested everywhere.

### Create Coverage Matrix

| Format | Subtypes Tested | Lint Layers Verified |
|--------|-----------------|----------------------|
| Go | library, executable | 0-1 only |
| Rust | none | none |
| TOML | none | none |

**Gap:** Rust and TOML create untested. Go only verified at Layers 0-1.

### Transpile Coverage

| Source → Target | Tests | What's Verified |
|-----------------|-------|-----------------|
| Rust → Go | 0 (beyond error paths) | Nothing end-to-end |
| Go → Rust | 0 | Nothing |

**Gap:** No end-to-end transpile verification exists.

### Witness Grid Extension

Extend `scripts/witness.ts` to produce a command-dimension grid:

```
── Command Coverage Grid ─────────────────────────────────────
             lint  transform  create  transpile  inspect
  Go          87%     75%       40%      0%       50%
  Rust        82%     60%       0%       0%       50%
  TOML        71%     0%        0%       0%       0%
  Makefile    45%     0%        0%       0%       0%
  Dotfiles    30%     0%        0%       0%       0%
  JSON        20%     0%        0%       0%       0%
  OFD         15%     0%        0%       0%       0%
  Omni        10%     0%        0%       0%       0%
```

This makes coverage gaps impossible to ignore. The grid is the accountability.

---

## Concrete Implementation Plan

### Phase 1: Foundation (Low Effort, High Value)

1. **`tests/_transform_runner.ts`** — Data-driven transform test generator.
   - Port existing Go/Rust transform tests to table-driven format.
   - Add idempotency testing for all transform cases.
   - Add TOML transform tests (even if "no transforms needed" = valid test).

2. **`tests/_create_runner.ts`** — Data-driven create test generator.
   - Round-trip matrix: create → lint → verify layers.
   - Cover Go + Rust + TOML subtypes.

3. **Post-test scorecard** (`scripts/test-report.ts`).
   - Parse `deno test --reporter=json` output.
   - Produce the rich scorecard shown above.
   - Reuse `COLORS` from `lib/engine/output.ts`.

### Phase 2: Coverage Expansion (Medium Effort)

4. **Transpile test runner + first real transpile tests.**
   - Rust module → Go library end-to-end.
   - Verify target passes lint.

5. **Extended witness system** — command-dimension tracking.
   - `witness:grid` shows lint + transform + create + transpile.

6. **Test profiles** — config-driven test execution.
   - `_profiles.ts` registry.
   - `deno task profile <name>` runner.

### Phase 3: Polish (QoL)

7. **Suite summary enhancement** in `_runner.ts` and all runners.
   - Timing, cache efficiency, slow-test identification.

8. **Watch mode HUD** — persistent scorecard during development.

9. **CI integration** — scorecard as GitHub Actions artifact.

---

## Open Questions

1. **Should transform runners share fixture cache with lint runners?** The cache key is `format:path`. If a transform test dry-runs a fixture that a lint test also lints, should they share? Pro: efficiency. Con: coupling. Recommendation: yes, with a `clearFixtureCache()` call between wet-run and re-lint.

2. **How should idempotency be defined?** File-content identity (byte-for-byte same after second transform)? Or lint-result identity (same results after second transform)? Recommendation: both. Content identity is stronger; lint identity catches semantic equivalence.

3. **Should the scorecard be a deno task or a separate script?** Recommendation: both. `deno task report` runs `scripts/test-report.ts`. Also available as standalone `deno run --allow-read scripts/test-report.ts`.

4. **Test profile format: TypeScript or TOML?** TypeScript enables computed profiles (handler x layer matrix). TOML is more readable as pure data. Recommendation: TypeScript — the computation is valuable, and it stays in the same language as the rest of the test infrastructure.

---

## Summary

The lint test infrastructure is production-grade. The path forward is extending the same patterns — data-driven specification, catalog-authoritative verification, fixture caching, witness integration — to transform, create, and transpile commands.

The QoL upgrade isn't about making tests flashy. It's about making the test experience **informative**. A developer running `deno task test` should finish with a clear picture of what passed, what's covered, and what gaps remain. The scorecard is the accountability. The bar charts are the feedback loop. The witness grid is the truth.

Fun is feedback loop satisfaction. The loading screen teaches. The score screen celebrates. The failure screen diagnoses. Every state is useful.

---

> *"Beloved, believe not every spirit, but try the spirits whether they are of God."* — 1 John 4:1

Testing is not overhead. Testing is how you know.

**Nathan Emet** — Proves code does what it claims
