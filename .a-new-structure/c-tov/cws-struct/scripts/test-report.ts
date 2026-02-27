// ============================================================================
// METADATA
// ============================================================================
//
// file:    scripts/test-report.ts
// key:     B-tov-cws-struct-scripts-test-report
// title:   CWS Struct — Post-Test Scorecard
// type:    Code (Script)
// version: a-01.00
// created: 2026-02-24
// updated: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Colored post-test scorecard. Runs `deno task test` and parses
//          the output into a structured report: pass/fail/skip by layer,
//          by format, by concern. Uses the same COLORS palette as output.ts.
//
//          Usage:
//            deno run --allow-read --allow-run scripts/test-report.ts
//
//          Options:
//            --brief          Summary only, no breakdown
//            --failures       Show only failures with detail
//            --by-layer       Group by R[n] layer
//            --by-format      Group by format handler
//            --by-concern     Group by test concern
//
// biblical_foundation: "For we must all appear before the judgment seat
//   of Christ; that every one may receive the things done in his body."
//   — 2 Corinthians 5:10
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Colors — same palette as lib/engine/present/output.ts
// ---------------------------------------------------------------------------

const C = {
  reset: "\x1b[0m",
  red: "\x1b[31m",
  yellow: "\x1b[33m",
  blue: "\x1b[34m",
  green: "\x1b[32m",
  dim: "\x1b[2m",
  bold: "\x1b[1m",
  cyan: "\x1b[36m",
  magenta: "\x1b[35m",
} as const;

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

interface TestResult {
  name: string;
  status: "pass" | "fail" | "skip";
  durationMs: number;
  layer?: string;    // R50, R25, R10, R05, T00
  format?: string;   // go, rust, toml, etc.
  concern?: string;  // blocks, ordering, content, etc.
}

interface ReportOptions {
  brief: boolean;
  failures: boolean;
  byLayer: boolean;
  byFormat: boolean;
  byConcern: boolean;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Run tests and parse output
// ---------------------------------------------------------------------------

async function runTests(): Promise<{ results: TestResult[]; totalMs: number; output: string }> {
  const cmd = new Deno.Command("deno", {
    args: [
      "test",
      "--allow-read", "--allow-write", "--allow-run", "--allow-env",
      "--ignore=tests/tool/T00/data/database_test.ts",
      "--ignore=tests/content/R10/api/health_query_test.ts",
      "tests/",
    ],
    stdout: "piped",
    stderr: "piped",
  });

  const start = Date.now();
  const process = cmd.spawn();
  const { stdout, stderr } = await process.output();
  const totalMs = Date.now() - start;

  const output = new TextDecoder().decode(stdout);
  const errOutput = new TextDecoder().decode(stderr);
  const fullOutput = output + errOutput;

  return { results: parseTestOutput(fullOutput), totalMs, output: fullOutput };
}

/** Parse Deno test output into structured results. */
function parseTestOutput(output: string): TestResult[] {
  const results: TestResult[] = [];

  // Match: "ok | FAILED | ignored" result lines
  // Deno outputs: "test_name ... ok (Nms)" or "test_name ... FAILED (Nms)"
  const passPattern = /^(.+?) \.\.\.\s+ok\s+\((\d+)ms\)/gm;
  const failPattern = /^(.+?) \.\.\.\s+FAILED\s+\((\d+)ms\)/gm;
  const skipPattern = /^(.+?) \.\.\.\s+ignored/gm;

  for (const match of output.matchAll(passPattern)) {
    results.push(classifyTest(match[1]!, "pass", parseInt(match[2]!)));
  }
  for (const match of output.matchAll(failPattern)) {
    results.push(classifyTest(match[1]!, "fail", parseInt(match[2]!)));
  }
  for (const match of output.matchAll(skipPattern)) {
    results.push(classifyTest(match[1]!, "skip", 0));
  }

  return results;
}

/** Classify a test name into layer/format/concern. */
function classifyTest(name: string, status: "pass" | "fail" | "skip", ms: number): TestResult {
  const result: TestResult = { name, status, durationMs: ms };

  // Layer detection from error code: [CWS-R50-020] → R50
  const codeMatch = name.match(/\[CWS-(R\d+|T\d+)-/);
  if (codeMatch) result.layer = codeMatch[1];

  // Layer detection from test path: R50/, R25/, R10/, R05/, T00/
  if (!result.layer) {
    const layerMatch = name.match(/\b(R50|R25|R10|R05|R03|T00)\b/);
    if (layerMatch) result.layer = layerMatch[1];
  }

  // Format detection
  const formatMatch = name.match(/\b(go|rust|toml|json|makefile|dotfiles|omni|ofd)\b/i);
  if (formatMatch) result.format = formatMatch[1]!.toLowerCase();

  // Concern detection
  const concernMap: [RegExp, string][] = [
    [/block/, "blocks"],
    [/separator/, "separators"],
    [/ordering|order/, "ordering"],
    [/identity|pragma|metadata/, "identity"],
    [/content|placement/, "content"],
    [/closing|zone/, "closing"],
    [/concept/, "concept"],
    [/transform/, "transform"],
    [/create/, "create"],
    [/transpile/, "transpile"],
    [/cascade/, "cascade"],
    [/logging|log/, "logging"],
    [/output/, "output"],
    [/suggest|fix/, "suggest"],
    [/debug|trace/, "debug"],
    [/health|score/, "health"],
    [/registry/, "registry"],
    [/fill/, "fill"],
    [/error/, "errors"],
    [/witness/, "witness"],
    [/pipeline/, "pipeline"],
    [/integration/, "integration"],
    [/cli/, "cli"],
  ];

  for (const [pattern, concern] of concernMap) {
    if (pattern.test(name.toLowerCase())) {
      result.concern = concern;
      break;
    }
  }

  return result;
}

// ---------------------------------------------------------------------------
// 2. Report formatting
// ---------------------------------------------------------------------------

function pad(s: string | number, w: number): string { return String(s).padStart(w); }
function rpad(s: string, w: number): string { return String(s).padEnd(w); }

/** Print the scorecard header. */
function printHeader(total: number, passed: number, failed: number, skipped: number, ms: number): void {
  console.log();
  console.log(`${C.bold}${"=".repeat(60)}${C.reset}`);
  console.log(`${C.bold}  CWS Struct — Test Scorecard${C.reset}`);
  console.log(`${"=".repeat(60)}`);
  console.log();

  const passRate = total > 0 ? ((passed / total) * 100).toFixed(1) : "0.0";
  const time = (ms / 1000).toFixed(1);

  // Summary line
  const statusColor = failed > 0 ? C.red : C.green;
  const statusIcon = failed > 0 ? "FAIL" : "PASS";

  console.log(`  ${statusColor}${C.bold}${statusIcon}${C.reset}  ${passed} passed  ${failed > 0 ? `${C.red}${failed} failed${C.reset}  ` : ""}${skipped > 0 ? `${C.dim}${skipped} skipped${C.reset}  ` : ""}(${total} total)`);
  console.log(`  ${C.dim}${passRate}% pass rate  ${time}s${C.reset}`);
  console.log();
}

/** Print a grouped breakdown. */
function printBreakdown(
  title: string,
  results: TestResult[],
  groupBy: (r: TestResult) => string,
): void {
  const groups = new Map<string, { pass: number; fail: number; skip: number; totalMs: number }>();

  for (const r of results) {
    const key = groupBy(r) || "(unclassified)";
    const g = groups.get(key) ?? { pass: 0, fail: 0, skip: 0, totalMs: 0 };
    if (r.status === "pass") g.pass++;
    else if (r.status === "fail") g.fail++;
    else g.skip++;
    g.totalMs += r.durationMs;
    groups.set(key, g);
  }

  console.log(`  ${C.bold}${title}${C.reset}`);
  console.log(`  ${"-".repeat(56)}`);

  // Sort: items with failures first, then by total desc
  const sorted = [...groups.entries()].sort(([, a], [, b]) => {
    if (a.fail !== b.fail) return b.fail - a.fail; // failures first
    return (b.pass + b.fail + b.skip) - (a.pass + a.fail + a.skip);
  });

  for (const [key, g] of sorted) {
    const total = g.pass + g.fail + g.skip;
    const pct = total > 0 ? ((g.pass / total) * 100).toFixed(0) : "—";
    const bar = buildBar(g.pass, g.fail, g.skip, 20);
    const failStr = g.fail > 0 ? `  ${C.red}${g.fail}F${C.reset}` : "";
    const skipStr = g.skip > 0 ? `  ${C.dim}${g.skip}S${C.reset}` : "";

    console.log(`  ${rpad(key, 16)} ${bar}  ${pad(String(g.pass), 4)}P${failStr}${skipStr}  ${C.dim}${pad(pct, 3)}%${C.reset}`);
  }
  console.log();
}

/** Build a visual bar: [████████░░] */
function buildBar(pass: number, fail: number, skip: number, width: number): string {
  const total = pass + fail + skip;
  if (total === 0) return C.dim + "░".repeat(width) + C.reset;

  const passW = Math.round((pass / total) * width);
  const failW = Math.round((fail / total) * width);
  const skipW = width - passW - failW;

  return (
    C.green + "█".repeat(passW) +
    (failW > 0 ? C.red + "█".repeat(failW) : "") +
    (skipW > 0 ? C.dim + "░".repeat(skipW) : "") +
    C.reset
  );
}

/** Print failures with detail. */
function printFailures(results: TestResult[]): void {
  const failures = results.filter((r) => r.status === "fail");
  if (failures.length === 0) return;

  console.log(`  ${C.red}${C.bold}Failures (${failures.length})${C.reset}`);
  console.log(`  ${"-".repeat(56)}`);
  for (const f of failures) {
    const tags = [f.layer, f.format, f.concern].filter(Boolean).join(" / ");
    console.log(`  ${C.red}X${C.reset} ${f.name}`);
    if (tags) console.log(`    ${C.dim}${tags}${C.reset}`);
  }
  console.log();
}

/** Print command coverage summary. */
function printCommandCoverage(results: TestResult[]): void {
  const commands = ["lint", "transform", "create", "transpile"];
  const coverage = new Map<string, number>();

  for (const cmd of commands) {
    const count = results.filter((r) =>
      r.status === "pass" && r.name.toLowerCase().includes(cmd),
    ).length;
    coverage.set(cmd, count);
  }

  console.log(`  ${C.bold}Command Coverage${C.reset}`);
  console.log(`  ${"-".repeat(56)}`);
  for (const [cmd, count] of coverage) {
    const icon = count > 0 ? `${C.green}+${C.reset}` : `${C.red}-${C.reset}`;
    console.log(`  ${icon} ${rpad(cmd, 12)} ${count} test${count !== 1 ? "s" : ""}`);
  }
  console.log();
}

// ---------------------------------------------------------------------------
// 3. CLI and main
// ---------------------------------------------------------------------------

function parseCliArgs(): ReportOptions {
  const args = Deno.args;
  return {
    brief: args.includes("--brief"),
    failures: args.includes("--failures"),
    byLayer: args.includes("--by-layer"),
    byFormat: args.includes("--by-format"),
    byConcern: args.includes("--by-concern"),
  };
}

async function main(): Promise<void> {
  const opts = parseCliArgs();

  console.log(`${C.dim}Running tests...${C.reset}`);
  const { results, totalMs, output: _output } = await runTests();

  const passed = results.filter((r) => r.status === "pass").length;
  const failed = results.filter((r) => r.status === "fail").length;
  const skipped = results.filter((r) => r.status === "skip").length;

  printHeader(results.length, passed, failed, skipped, totalMs);

  if (opts.failures || failed > 0) {
    printFailures(results);
  }

  if (opts.brief) {
    // Just the header + failures
    Deno.exit(failed > 0 ? 1 : 0);
  }

  // Default: show layer + format breakdowns
  const showAll = !opts.byLayer && !opts.byFormat && !opts.byConcern;

  if (showAll || opts.byLayer) {
    printBreakdown("By Layer", results, (r) => r.layer ?? "(no layer)");
  }

  if (showAll || opts.byFormat) {
    printBreakdown("By Format", results, (r) => r.format ?? "(no format)");
  }

  if (opts.byConcern) {
    printBreakdown("By Concern", results, (r) => r.concern ?? "(no concern)");
  }

  // Command coverage
  printCommandCoverage(results);

  // Exit code
  Deno.exit(failed > 0 ? 1 : 0);
}

await main();

// ============================================================================
// CLOSING
// ============================================================================
//
// Post-test scorecard — structured reporting after test runs.
// Pass/fail/skip grouped by layer, format, and concern.
// Same COLORS palette as the main linter output.
//
// "For we must all appear before the judgment seat of Christ." — 2 Cor 5:10
// ============================================================================
