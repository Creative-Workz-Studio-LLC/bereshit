# Plan: Witness Report System — Expanded Error Codes + Structured Output + CPI-SI Feedback Loop

## Context

The CWS Manual Builder has 57 editorial lint checks, theme validation, structural validation, Bible verification, and TOC pipeline — but all output is terminal-only (colored chalk text). Only 2 commands support JSON (config, toc). The error code taxonomy stops at E43.

Seanje wants:
1. **Expanded error codes** — complete taxonomy covering all subsystems
2. **Structured output reports** — JSON for all validation domains
3. **`--witness`/`--report` flag** — machine-readable output
4. **CPI-SI feedback loop foundation** — the system bears witness to its own state

The "witness" concept is theological: the system observes, records, and reports its own health. This becomes the input for CPI-SI intelligence to act on — the feedback loop.

---

## Architecture: The Witness Pattern

```
cws-build witness              →  WitnessCollector  →  WitnessReport (JSON)
cws-build lint --report         →  WitnessCollector  →  WitnessReport (JSON, editorial only)
cws-build validate --report     →  WitnessCollector  →  WitnessReport (JSON, structural + theme)
```

**Key insight:** Every JSON output uses the same `WitnessReport` schema — `witness` fills all domains, `--report` fills one. Same envelope, variable content. CPI-SI always parses one schema.

**Health maps to Hebrew states:**
- `healthy` = tov (+1)
- `degraded` = yashar (0)
- `failing` = shavar (-1)

---

## Files to Create (7 new)

| File | Purpose |
|------|---------|
| `src/witness/types.ts` | WitnessReport schema — all interfaces |
| `src/witness/collector.ts` | WitnessCollector class — gathers domain results |
| `src/witness/runner.ts` | runWitness() — orchestrates all 5 validation domains |
| `src/witness/recommendations.ts` | generateRecommendations() — prioritized action items |
| `src/witness/serializer.ts` | JSON serialization (handles Map→Object) |
| `src/witness/index.ts` | Barrel re-exports |
| `src/display/witness.ts` | displayWitnessReport() — terminal summary dashboard |
| `src/cli/commands/witness.ts` | registerWitnessCommand() — CLI command |

## Files to Modify (6 existing)

| File | Change |
|------|--------|
| `src/errors.ts` | Add E5x (theme), E6x (scripture), E7x (report) error codes |
| `src/cli/index.ts` | Wire registerWitnessCommand() |
| `src/cli/commands/lint.ts` | Add `--report` flag |
| `src/cli/commands/validate.ts` | Add `--report` flag |
| `src/display/index.ts` | Re-export displayWitnessReport |
| `src/index.ts` | Re-export witness module |

---

## Step 1: Expand Error Codes (`src/errors.ts`)

Add three new ranges to the ErrorCode union and ERROR_CODES registry:

```
E5x — Theme validation (exit 5)
  E50: Theme validation failed (typography spec)
  E51: Page layout mismatch
  E52: Theme parse error
  E53: Theme element mismatch

E6x — Scripture/Bible validation (exit 6)
  E60: Verse not found in scripture data
  E61: Verse text mismatch

E7x — Config/Report (exit 7)
  E70: Report generation failed
  E71: Partial validation failure (some domains failed)
  E72: Config version mismatch
```

Add `'theme' | 'scripture' | 'report'` to the `ErrorCategory` type union.

**Complete taxonomy after expansion:** E0 (success), E1x (usage), E2x (dependency), E3x (build), E4x (validation), E5x (theme), E6x (scripture), E7x (report).

---

## Step 2: Witness Types (`src/witness/types.ts`)

The WitnessReport is an **envelope** that carries each domain's native findings:

```typescript
export const WITNESS_SCHEMA_VERSION = '1.0.0';

export type SystemHealth = 'healthy' | 'degraded' | 'failing';
export type DomainHealth = 'pass' | 'warn' | 'fail' | 'skipped';

export interface WitnessMetadata {
  schemaVersion: string;        // For forward compatibility
  timestamp: string;            // ISO 8601
  configVersion: string;        // From build.config.yaml
  builderVersion: string;       // From package.json
  duration: number;             // Total ms
  domainsRequested: string[];
  domainsExecuted: string[];
  trigger: string;              // 'witness', 'lint', 'validate'
  options: Record<string, unknown>;
}

export interface DomainSummary {
  domain: string;
  label: string;
  health: DomainHealth;
  counts: Record<string, number>;
  duration: number;
  error?: string;
}

export interface Recommendation {
  priority: number;             // 1 (critical) to 5 (nice-to-have)
  domain: string;
  message: string;
  file?: string;
  action?: string;              // Suggested command/fix
}

export interface WitnessReport {
  metadata: WitnessMetadata;
  health: SystemHealth;
  summary: DomainSummary[];     // Per-domain, worst-first
  domains: DomainResult[];      // Full results per domain
  totals: { errors: number; warnings: number; info: number; passes: number };
  recommendations: Recommendation[];
}
```

Five `DomainResult` variants (StructuralDomainResult, EditorialDomainResult, ThemeDomainResult, BibleDomainResult, TocDomainResult) — each wraps the existing result type for that domain.

---

## Step 3: WitnessCollector (`src/witness/collector.ts`)

```typescript
export class WitnessCollector {
  constructor(trigger: string, options: Record<string, unknown>)

  addStructural(result: ValidationResult, duration: number): void
  addEditorial(result: EditorialResult, duration: number): void
  addTheme(file: string, findings: ValidationFinding[], duration: number): void
  addBible(result: BibleValidationResult, duration: number): void
  addToc(result: PageParseResult, duration: number): void

  skipDomain(domain: string, reason: string): void
  failDomain(domain: string, error: Error): void

  finalize(configVersion: string, builderVersion: string): WitnessReport
}
```

`finalize()` computes:
- Per-domain health from native severity counts
- System health: any `fail` → `failing`, any `warn` → `degraded`, else `healthy`
- Sorts summary worst-first
- Calls `generateRecommendations()` for action items

---

## Step 4: Recommendations (`src/witness/recommendations.ts`)

```typescript
export function generateRecommendations(domains: DomainResult[]): Recommendation[]
```

Generates prioritized action items by analyzing domain results:
- Priority 1: Structural failures (book can't build)
- Priority 2: Theme validation failures (PDF won't render correctly)
- Priority 3: Editorial warnings by volume (most impactful checks)
- Priority 4: Bible mismatches (text accuracy)
- Priority 5: TOC staleness (page numbers need update)

Each recommendation includes a suggested `action` string (command to run or fix to make).

---

## Step 5: Serializer (`src/witness/serializer.ts`)

```typescript
export function serializeWitnessReport(report: WitnessReport): string
```

Handles JSON.stringify with:
- Map → Object conversion (for TOC pageMap if present)
- 2-space indentation for readability
- Deterministic output (sorted keys where order matters)

---

## Step 6: Witness Runner (`src/witness/runner.ts`)

```typescript
export interface WitnessOptions {
  bookType?: 'manual' | 'manual-l';
  bonus?: 'operational';
  domains?: string[];           // Filter to specific domains
  category?: string;            // Editorial lint category filter
}

export async function runWitness(
  config: BuildConfig,
  paths: RuntimePaths,
  options?: WitnessOptions,
): Promise<WitnessReport>
```

Orchestration order (each domain independently try/caught):

1. **Structural** — `runValidation(paths, opts, typography, pageLayout)`
2. **Theme** — iterate all 5 theme YAMLs, call theme validators per file
3. **Editorial** — `runEditorialLint(paths, opts, configs...)`
4. **Bible** — `runBibleValidation(paths, opts)`
5. **TOC** — `runPageParse(paths, opts)`

If a domain throws, collector marks it `skipped` with error message. Other domains continue.

**Theme as separate domain:** Currently theme validation runs inside `runValidation()` at Layer 1b. For the witness report, the runner calls theme validators directly (reusing exported functions from `validate/theme.ts`) to get per-theme-file granularity. The existing `validate` command behavior is unchanged.

---

## Step 7: Display (`src/display/witness.ts`)

```typescript
export function displayWitnessReport(report: WitnessReport): void
```

Terminal summary dashboard:

```
Witness Report
==============
  Config: 5.2.0  |  Builder: 5.2.0  |  Health: DEGRADED

--- Domain Summary ---

  PASS  Structural   12 pass, 0 fail, 2 warn       (350ms)
  WARN  Editorial    23 warn, 8 info across 99 files (1.2s)
  PASS  Theme        5 themes validated              (120ms)
  SKIP  Bible        Scripture data not available
  PASS  TOC          All page markers valid           (80ms)

--- Recommendations (3) ---

  [1] Editorial: 12 'shall' warnings in B-content — review obligation language
  [2] Editorial: 8 'register-mixing' findings — check document register consistency
  [3] TOC: 3 pages need update — run: cws-build toc --fill

--- Totals ---

  0 errors  |  25 warnings  |  8 info  |  12 passes
  Duration: 1.8s
```

---

## Step 8: CLI Command (`src/cli/commands/witness.ts`)

```typescript
program
  .command('witness')
  .description('Comprehensive system health check — all validation domains')
  .option('-t, --type <type>', 'Book type for structural validation (manual, manual-l)')
  .option('-b, --bonus <scope>', 'Bonus validation scope (operational)')
  .option('--json', 'Output JSON report to stdout')
  .option('--out <file>', 'Write JSON report to file')
  .option('--domains <list>', 'Comma-separated domains (structural,editorial,theme,bible,toc)')
  .action(async (opts) => { ... })
```

Default: runs all 5 domains, displays terminal summary.
With `--json`: outputs full WitnessReport JSON to stdout.
With `--out report.json`: writes JSON to file, terminal shows summary.

---

## Step 9: `--report` Flag on Existing Commands

### `lint.ts`
Add `.option('--report', 'Output JSON witness report instead of terminal display')`.
When `--report` is set: wrap EditorialResult in WitnessCollector, output JSON, skip terminal display.

### `validate.ts`
Same pattern. Wraps ValidationResult (structural + theme) in collector.

Both use the same WitnessReport schema — CPI-SI always parses one format.

---

## Step 10: Wire Into CLI (`src/cli/index.ts`)

Import `registerWitnessCommand` and call it alongside existing command registrations.

---

## Step 11: Re-exports

- `src/display/index.ts` — add `displayWitnessReport`
- `src/witness/index.ts` — export all witness types and functions
- `src/index.ts` — re-export witness module for library API

---

## Step 12: Build & Verify

1. `npm run build` — clean compilation
2. `cws-build witness` — terminal summary (all 5 domains)
3. `cws-build witness --json` — full JSON report to stdout
4. `cws-build witness --json --out /tmp/witness-report.json` — to file
5. `cws-build witness --domains editorial,theme` — selective domains
6. `cws-build lint --report` — editorial-only JSON report
7. `cws-build validate --report` — structural+theme JSON report
8. Verify JSON parses cleanly: `cat /tmp/witness-report.json | python3 -m json.tool`
9. Verify exit codes: failing domain → non-zero exit

---

## Implementation Order

| Phase | Steps | Files | What |
|:-----:|:-----:|:-----:|------|
| 1 | 1-2 | errors.ts, witness/types.ts, witness/index.ts | Foundation: error codes + schema |
| 2 | 3-5 | witness/collector.ts, recommendations.ts, serializer.ts | Core: collection + output |
| 3 | 6 | witness/runner.ts | Orchestration |
| 4 | 7-8 | display/witness.ts, cli/commands/witness.ts | UI: terminal + CLI |
| 5 | 9-11 | lint.ts, validate.ts, cli/index.ts, display/index.ts, index.ts | Integration |
| 6 | 12 | — | Build + verify |

---

## CPI-SI Feedback Loop (Future)

The witness report is the **observation** half of the CPI-SI feedback loop:

```
System State → Witness Report (observation)
                    ↓
              CPI-SI Agent (analysis)
                    ↓
              Action Plan (decision)
                    ↓
              Execute Fixes (action)
                    ↓
              Re-witness (verification)
                    ↓
              System State → ...
```

The `recommendations` array is the bridge — it's where observation becomes actionable. A future CPI-SI agent reads the witness report, processes recommendations through its Hebrew state machine, and produces an action plan. The `action` field on each recommendation gives the suggested command.

This plan implements the observation layer. The analysis/decision/action layers come later.

> *"Surely the Lord GOD will do nothing, but he revealeth his secret unto his servants the prophets."* — Amos 3:7
