# CWS-Struct Production Hardening Plan

> *"Whatsoever thy hand findeth to do, do it with thy might."* -- Ecclesiastes 9:10

## Context

**Why:** cws-struct is functional -- 229 tests pass, 8 format handlers registered, lint + transform + verify all working. But the engine has performance gaps and missing production features that need closing before this tool is truly production-ready. Seanje's ask: "make sure the code itself now has caching, performance enhancements and other quality of life logic updating and production passing."

**What prompted it:** After completing the test reorganization (block-based fixtures, category-prefixed test names, targeted test tasks), we assessed the codebase honestly. The tool works; now it needs to work *well*.

**Intended outcome:** A production-grade CLI tool with parallel linting, intelligent caching, machine-readable output, robust error handling, and QoL features that make it a pleasure to use.

---

## Phase 1: Foundation Performance (Quick Wins)

Low-risk changes to existing files. Each is independent -- can land and verify separately.

### 1.1 Single-pass `summarize()` in types.ts

**File:** `lib/foundation/types.ts:182-195`

**Problem:** Three separate `.filter()` passes over the same array to count errors, warnings, infos.

**Fix:** Single `for` loop with a switch:
```ts
export function summarize(file: string, results: LintResult[], health?: HealthScore): LintSummary {
  let errors = 0, warnings = 0, infos = 0;
  for (const r of results) {
    switch (r.severity) {
      case "error": errors++; break;
      case "warn": warnings++; break;
      case "info": infos++; break;
    }
  }
  return { file, errors, warnings, infos, results, ...(health ? { health } : {}) };
}
```

**Verify:** `deno task test:foundation` -- types tests all pass.

---

### 1.2 Single-pass `computeContainerScore()` in health.ts

**File:** `lib/foundation/health.ts:125-155`

**Problem:** Four separate `.filter()` passes (passed, failedErrors, failedWarnings, failedInfos).

**Fix:** Single `for` loop:
```ts
let passed = 0, failedErrors = 0, failedWarnings = 0, failedInfos = 0;
for (const a of actions) {
  if (a.passed) { passed++; }
  else if (a.severity === "error") { failedErrors++; }
  else if (a.severity === "warn") { failedWarnings++; }
  else { failedInfos++; }
}
```

**Verify:** `deno task test:foundation` -- health scoring tests all pass.

---

### 1.3 Memoize sibling file checks

**Files:**
- `lib/handlers/go.ts:666-692` -- `packageHasIdentityVars()`
- `lib/handlers/rust.ts:528-548` -- `crateHasIdentityStatics()`

**Problem:** These functions read ALL sibling `.go`/`.rs` files in the same directory to check if any sibling already declares identity variables. Called per-file, so linting 10 Go files in the same directory causes 10x redundant directory reads.

**Fix:** Module-level `Map<string, boolean>` cache keyed by directory path. Check cache first, populate on miss. Cache lives for the duration of the CLI run (no persistence needed).

```ts
const _siblingCache = new Map<string, boolean>();

async function packageHasIdentityVars(filePath: string): Promise<boolean> {
  const dir = dirname(filePath);
  const cached = _siblingCache.get(dir);
  if (cached !== undefined) return cached;
  // ... existing logic ...
  _siblingCache.set(dir, result);
  return result;
}
```

Also export a `clearSiblingCache()` for tests (so tests don't leak state).

**Verify:** `deno task test:go` and `deno task test:rust` -- all handler tests pass.

---

### 1.4 File size guard in discovery.ts

**File:** `lib/engine/discovery.ts:107-151`

**Problem:** No guard against oversized files. A 50MB generated file would be read entirely into memory by the linter.

**Fix:** Add `MAX_FILE_SIZE = 1_048_576` (1MB). In `discoverFiles()`, stat each file and skip with warning if it exceeds the limit. Files we lint are source code -- anything over 1MB is almost certainly generated.

```ts
const MAX_FILE_SIZE = 1_048_576; // 1MB -- source files should never be this large

// Inside the walk loop, after matchesHandler:
const fileStat = await Deno.stat(entry.path);
if (fileStat.size > MAX_FILE_SIZE) {
  console.error(`Skipping ${entry.path}: file too large (${(fileStat.size / 1024).toFixed(0)}KB > 1MB limit)`);
  continue;
}
```

Note: For single-file targets (explicit path), we still stat the file and guard. The user targeting a specific file that's too large should get the warning.

**Verify:** `deno task test:engine` -- discovery tests pass. Add one test with a large fixture (or mock).

---

### 1.5 Parallel env checks in env.ts

**File:** `lib/verify/env.ts:346-351`

**Problem:** Sequential `for` loop calls `checkTool()` one at a time. 26 tools, each spawning a subprocess. Sequential execution means wall-clock time = sum of all tool checks.

**Fix:** `Promise.all()` -- all 26 tools check concurrently. Each `checkTool()` is independent (spawns its own subprocess).

```ts
const results = await Promise.all(TOOLS.map((spec) => checkTool(spec)));
```

The display logic after this stays the same -- it already iterates `results`.

**Verify:** `deno task test:verify` -- env tests pass. Manual: `deno task verify` should run noticeably faster.

---

## Phase 2: Engine Performance (Core Flow)

These touch the main lint/transform pipeline in mod.ts and discovery.ts.

### 2.1 Parallel file linting with bounded concurrency

**File:** `mod.ts:198-234` -- `lintWithHandler()`

**Problem:** Files are linted sequentially: `for (const file of files) { await handler.lint(file); }`. On a directory with 50 TOML files, this means 50 sequential file reads + lint passes.

**Fix:** Bounded concurrency using `pooledMap` from `@std/async` (already available via Deno std):

```ts
import { pooledMap } from "@std/async";

const CONCURRENCY = 8; // bounded -- don't open 1000 files simultaneously

async function lintWithHandler(opts, handler, files): Promise<LintSummary[]> {
  const cwd = Deno.cwd();
  const summaries: LintSummary[] = [];

  const results = pooledMap(CONCURRENCY, files, async (file) => {
    const results = await handler.lint(file);
    const health = handler.computeHealth
      ? await handler.computeHealth(file, results)
      : undefined;
    return summarize(relative(cwd, file), results, health);
  });

  for await (const summary of results) {
    summaries.push(summary);
    // Print as each completes (streaming output)
    if (!opts.summaryOnly) {
      if (opts.errorsOnly) {
        if (summary.errors > 0) printFileSummary(summary, opts.verbose);
      } else {
        printFileSummary(summary, opts.verbose);
      }
    }
  }
  return summaries;
}
```

Add `"@std/async": "jsr:@std/async@1"` to `deno.jsonc` imports.

Same pattern for `transformWithHandler()`.

**Verify:** `deno task test:cli` -- CLI tests pass. `deno task test:integration` -- integration tests pass.

---

### 2.2 Single-walk auto-detect

**File:** `mod.ts:267-284` (runLint auto-detect path), `lib/engine/discovery.ts`

**Problem:** Auto-detect loops through ALL registered formats, calling `discoverFiles()` per format. 8 formats = 8 directory walks of the same tree. Each walk re-reads the same directory entries.

**Fix:** Add `discoverAllFiles()` to discovery.ts that does ONE walk and dispatches files to matching handlers using the registry's `detectFormat()`:

```ts
export async function discoverAllFiles(
  targets: string[],
  maxDepth?: number,
): Promise<Map<string, string[]>> {
  // Map<formatName, filePaths[]>
  const result = new Map<string, string[]>();
  // ... single walk, for each file call detectFormat(), bucket into result map ...
  return result;
}
```

Then in `runLint()` auto-detect path:
```ts
const filesByFormat = await discoverAllFiles(opts.targets);
for (const [formatName, files] of filesByFormat) {
  const handler = getFormat(formatName)!;
  // ... existing per-format logic ...
}
```

Same for `runTransform()`.

**Verify:** `deno task test:cli` and `deno task test:engine` -- existing tests pass. Manual: `cws-struct lint .` should be measurably faster.

---

### 2.3 Error context preservation

**Files:** Multiple handlers + discovery.ts

**Problem:** Several `catch {}` blocks silently swallow errors:
- `discovery.ts:142` -- `catch { console.error("Cannot access: ...") }` -- loses the actual error
- `go.ts` and `rust.ts` -- `buildContext()` returns null on error, losing the why
- `env.ts:287` -- `catch { result.found = false }` -- swallows subprocess errors

**Fix:** Preserve error info in a structured way:
- `discovery.ts`: Log the actual error message alongside the path
- `buildContext()`: Return error info that gets surfaced as a lint result (not silently null)
- `env.ts`: Log specific error (e.g., "permission denied" vs "not found")

Pattern:
```ts
} catch (e: unknown) {
  const msg = e instanceof Error ? e.message : String(e);
  console.error(`Cannot access ${target}: ${msg}`);
}
```

For `buildContext()`, convert the catch to a lint result:
```ts
} catch (e: unknown) {
  const msg = e instanceof Error ? e.message : String(e);
  return { results: [error(filePath, "read-error", `Cannot read file: ${msg}`)] };
}
```

**Verify:** All test suites pass. Manually test with a permission-denied file to see structured error output.

---

## Phase 3: CLI Quality of Life

New features that make the tool production-ready for CI/CD and daily use.

### 3.1 `--json` output mode

**Files:** `mod.ts`, `lib/engine/output.ts`, `lib/foundation/types.ts`

**Problem:** No machine-readable output. CI/CD pipelines need to parse lint results programmatically.

**Fix:** Add `--json` flag to CLI:
- `CliOptions.json: boolean`
- When `--json`: suppress all console output, collect summaries, print JSON to stdout at end
- JSON structure: `{ tool, version, files: LintSummary[], totals: { errors, warnings, infos, avgHealth } }`

```ts
// In mod.ts, after collecting all summaries:
if (opts.json) {
  const output = {
    tool: TOOL_NAME,
    version: VERSION,
    files: allSummaries,
    totals: { errors: totalErrors, warnings: totalWarnings, infos: totalInfos },
  };
  console.log(JSON.stringify(output, null, 2));
}
```

Add `"--json"` to KNOWN_FLAGS and parseArgs.

**Verify:** `cws-struct lint toml tests/fixtures/toml/ --json | python3 -m json.tool` -- valid JSON output.

---

### 3.2 `--fail-fast` flag

**File:** `mod.ts`

**Problem:** When linting 100 files, you might want to stop on the first file with errors (for quick feedback during development).

**Fix:** Add `--fail-fast` to CliOptions. In `lintWithHandler()`, after each summary, check `if (opts.failFast && summary.errors > 0) break;`.

With parallel linting (Phase 2.1), fail-fast triggers after the current batch completes -- it won't abort in-flight lints but won't start new ones.

**Verify:** `deno task test:cli` -- add test for --fail-fast behavior.

---

### 3.3 Progress indicator

**File:** `lib/engine/output.ts`, `mod.ts`

**Problem:** When linting many files, no progress feedback. Silent until all results print.

**Fix:** Lightweight counter in `lintWithHandler()`: print `[N/total]` before each file result. Only when stdout is a TTY (skip in piped/JSON mode).

```ts
const isTTY = Deno.stdout.isTerminal();
// In the loop:
if (isTTY && !opts.json) {
  Deno.stdout.writeSync(new TextEncoder().encode(`\r[${i+1}/${files.length}] Linting...`));
}
```

Clear the progress line before printing each file result.

**Verify:** Manual testing -- run `cws-struct lint .` and observe progress counter.

---

## Reuse (Existing Code)

| What | Where | Reuse How |
|------|-------|-----------|
| `pooledMap` | `@std/async` (Deno std) | Import for bounded concurrency |
| `detectFormat()` | `lib/engine/registry.ts:70-84` | Used in single-walk to bucket files |
| `matchesHandler()` | `lib/engine/discovery.ts:156-171` | Reused within `discoverAllFiles()` |
| `EXCLUDED_DIR_PATTERNS` | `lib/engine/discovery.ts:43-50` | Shared between both discover functions |
| `COLORS`, `severityIcon` | `lib/engine/output.ts` | Already shared, no changes |
| `error()`, `warn()`, `info()` | `lib/foundation/types.ts` | Used in error context preservation |

---

## Phase 4: Architecture (Shared 4-Block Extraction)

The centerpiece refactor. Go and Rust handlers share ~60-70% identical code. Extract the shared logic into a reusable module that both handlers compose from.

### 4.1 Extract shared types to `lib/handlers/shared/types.ts`

**New file:** `lib/handlers/shared/types.ts`

**What moves:** These interfaces are IDENTICAL in both go.ts and rust.ts:
- `BlockPosition` -- `{ block: string, startLine: number, endLine: number }`
- `DirectiveInfo` -- `{ value: string, line: number }`
- `SubsectionRange` -- `{ tag: string, startLine: number, endLine: number }`
- `IdentityField` -- `{ section: string, field: string, value: string, line: number }`
- `BlockRange` -- `{ startLine: number, endLine: number }`

Also extract a generic `FileContext` base type:
```ts
interface FileContext {
  filePath: string;
  lines: string[];
  blocks: BlockPosition[];
  directives: Map<string, DirectiveInfo>;
  isTemplate: boolean;
  isDerived: boolean;
}
```

Go handler extends: `GoFileContext extends FileContext { packageName, ... }`
Rust handler extends: `RustFileContext extends FileContext { crateType, ... }`

**Verify:** `deno task test` -- all tests pass with imported types.

---

### 4.2 Extract shared block-parsing functions to `lib/handlers/shared/code-4block.ts`

**New file:** `lib/handlers/shared/code-4block.ts`

**Functions that are IDENTICAL (copy-paste between Go and Rust):**
- `findBlocks(lines, blockPatterns, endPatterns)` -- go.ts:287, rust.ts:266
- `findOmniDirectives(lines)` -- go.ts:330, rust.ts:310
- `getBlockLines(lines, blocks, blockName)` -- go.ts:371, rust.ts:351
- `blockLineToFile(blocks, blockName, relIdx)` -- go.ts:399, rust.ts:379
- `getSubsectionRanges(blockLines, subsections)` -- go.ts:468, rust.ts:459
- `getTopLevelDeclarations(blockLines, classifier)` -- go.ts:501, rust.ts:494
- `validateICFields(file, fields, requirements, blockName, results)` -- go.ts:610, rust.ts:715
- `findBlockRange(lines, blockName)` -- go.ts:1679, rust.ts:1744

**Functions that are ~85-95% identical (parameterize the difference):**
- `checkDirectives(ctx, requiredDirectives, recommendedDirectives)` -- both handlers
- `checkBlockStructure(ctx, blocks)` -- both handlers
- `checkCommentMetadata(ctx)` -- both handlers
- `checkSeparatorConsistency(ctx, blockWidth, subsectionWidth)` -- both handlers
- `checkTemplateVsDerived(ctx)` -- both handlers
- `checkClosingZoneOrder(ctx)` -- both handlers
- `checkSetupSubsectionOrder(ctx, subsections)` -- both handlers
- `checkBodySubsectionOrder(ctx)` -- both handlers

**Pattern:** Each shared function takes configuration params for the language-specific parts:

```ts
interface Code4BlockConfig {
  /** Language name for error messages */
  language: string;
  /** Block boundary patterns */
  blockPatterns: Record<string, RegExp>;
  /** END marker patterns */
  endPatterns: Record<string, RegExp>;
  /** SETUP subsection markers in dependency order */
  setupSubsections: readonly { tag: string; pattern: RegExp }[];
  /** Block separator widths: [block, subsection] */
  separatorWidths: [number, number];
  /** Classify a line of code */
  classifyLine: (line: string) => string;
  /** Parse identity fields from source */
  parseIdentityFields: (lines: string[], varName: string) => IdentityField[];
  /** Check if sibling files have identity vars */
  siblingHasIdentity: (filePath: string) => Promise<boolean>;
}
```

**Estimated extraction:** ~800-1000 lines move to shared, ~200-400 lines become thin wrappers calling shared code with language-specific config.

**Verify:** After each function extraction, run `deno task test:go` and `deno task test:rust` to confirm zero regression.

---

### 4.3 Refactor Go handler to compose from shared module

**File:** `lib/handlers/go.ts` (currently ~2,591 lines)

**What stays:** `classifyGoLine()`, `parseSliceFields()`, `packageHasIdentityVars()`, Go-specific `SETUP_SUBSECTIONS` patterns, `KNOWN_CODE_DIRECTIVES`, `checkPackageAndImports()`, `findTestFunc`, `findMainFunc`.

**What moves out:** All functions listed in 4.2 above. The `lintGoFile()` orchestrator becomes:
```ts
async function lintGoFile(filePath: string): Promise<LintResult[]> {
  const ctx = await buildContext(filePath);
  if (!ctx) return [...];
  return runSharedLintChecks(ctx, GO_CONFIG).concat(checkGoSpecific(ctx));
}
```

**Target:** Go handler drops from ~2,591 to ~800-1000 lines (language-specific code only).

---

### 4.4 Refactor Rust handler to compose from shared module

**File:** `lib/handlers/rust.ts` (currently ~2,581 lines)

**Same pattern as 4.3.** Rust-specific code stays, shared code delegates to `code-4block.ts`.

**What stays:** `classifyLine()`, `parseStaticFields()`, `crateHasIdentityStatics()`, Rust-specific `SETUP_SUBSECTIONS` patterns, `KNOWN_CODE_DIRECTIVES`, `checkDocComments()`, `findCfgTest`, `findFnMain`.

**Target:** Rust handler drops from ~2,581 to ~800-1000 lines.

---

### 4.5 Create `lib/handlers/shared/mod.ts` barrel

**New file:** Re-exports shared types and functions. Handlers import from `../shared/mod.ts`.

---

## Implementation Order (Updated)

| Step | Phase | Item | Files Modified | Risk |
|:----:|:-----:|------|---------------|:----:|
| 1 | 1.1 | Single-pass summarize | types.ts | Low |
| 2 | 1.2 | Single-pass health scoring | health.ts | Low |
| 3 | 1.3 | Sibling check memoization | go.ts, rust.ts | Low |
| 4 | 1.4 | File size guard | discovery.ts | Low |
| 5 | 1.5 | Parallel env checks | env.ts | Low |
| 6 | 2.1 | Parallel file linting | mod.ts, deno.jsonc | Med |
| 7 | 2.2 | Single-walk auto-detect | mod.ts, discovery.ts | Med |
| 8 | 2.3 | Error context | discovery.ts, go.ts, rust.ts, env.ts | Low |
| 9 | 3.1 | --json output | mod.ts, output.ts, types.ts | Low |
| 10 | 3.2 | --fail-fast | mod.ts | Low |
| 11 | 3.3 | Progress indicator | output.ts, mod.ts | Low |
| 12 | 4.1 | Extract shared types | NEW: shared/types.ts | Low |
| 13 | 4.2 | Extract shared functions | NEW: shared/code-4block.ts | High |
| 14 | 4.3 | Refactor Go handler | go.ts | High |
| 15 | 4.4 | Refactor Rust handler | rust.ts | High |
| 16 | 4.5 | Shared barrel | NEW: shared/mod.ts | Low |

**Test after EVERY step.** `deno task test` must stay at 229+ passing. Steps 12-15 should each have their own wave commit.

---

## Files Modified (Complete)

| File | Changes |
|------|---------|
| `lib/foundation/types.ts` | Single-pass summarize, json + failFast CliOption |
| `lib/foundation/health.ts` | Single-pass container scoring |
| `lib/engine/discovery.ts` | File size guard, `discoverAllFiles()`, error context |
| `lib/engine/output.ts` | Progress indicator |
| `lib/verify/env.ts` | Parallel tool checks, error context |
| `lib/handlers/go.ts` | Sibling cache, error context, compose from shared |
| `lib/handlers/rust.ts` | Sibling cache, error context, compose from shared |
| `lib/handlers/shared/types.ts` | NEW -- shared interfaces |
| `lib/handlers/shared/code-4block.ts` | NEW -- shared lint/parse functions |
| `lib/handlers/shared/mod.ts` | NEW -- barrel exports |
| `mod.ts` | Parallel linting, single-walk, --json, --fail-fast, progress |
| `deno.jsonc` | Add `@std/async` import |

---

## NOT in Scope (Future Work)

| Item | Why Deferred |
|------|-------------|
| **Shared comment-block handler** (Makefile/Dotfiles) | Lower duplication -- apply the pattern from 4-block extraction later |
| **Schema-driven handler configs** | Design decision after extraction proves the pattern |
| **`--watch` mode** | Future feature, not production-critical |

---

## Verification Plan

### After Phases 1-3 (Performance + QoL):

1. **Full test suite:** `deno task test` -- 229+ tests pass (0 regressions)
2. **Targeted suites:** `deno task test:foundation`, `test:engine`, `test:cli`, `test:verify`
3. **Type check:** `deno task check` -- clean
4. **Manual smoke tests:**
   - `cws-struct lint .` -- auto-detect, observe progress counter
   - `cws-struct lint toml tests/fixtures/ --json` -- valid JSON
   - `cws-struct lint rust tests/fixtures/ --fail-fast` -- stops on first error
   - `cws-struct verify env` -- runs faster (parallel)
   - `cws-struct lint . --summary` -- summary mode still works
5. **Integration tests:** `deno task test:integration` -- seed templates still zero errors
6. **Performance (subjective):** `cws-struct lint .` should feel noticeably faster on auto-detect

### After Phase 4 (Extraction):

7. **Full test suite again:** 229+ tests pass -- extraction must not change behavior
8. **Handler-specific:** `deno task test:go`, `deno task test:rust` -- all existing checks still fire
9. **Line count check:** go.ts and rust.ts should each drop from ~2,500 to ~800-1,000 lines
10. **Shared module test:** New tests for shared functions (parameterized, handler-independent)
11. **Integration:** Seed templates still lint clean
12. **Manual:** Lint same files before and after extraction -- output should be byte-identical

---

> *"Prove all things; hold fast that which is good."* -- 1 Thessalonians 5:21
