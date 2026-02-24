// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/_runner.ts
// key:     B-tov-cws-struct-tests-runner
// title:   Shared Data-Driven Test Runner
// type:    Code (Test Infrastructure)
// version: a-02.00
// created: 2026-02-24
// updated: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Universal table-driven test generation for ALL layers.
//          Tests are specified as data — fixture + expected error code + severity.
//          The error catalog is the source of truth for severity and rule patterns.
//          Add a row to the table → test exists. No more manual boilerplate.
//
//          QoL features (v2):
//            - Suite summary test with fixture stats and code coverage
//            - Severity indicators in test names (E/W/I)
//            - Auto-witness integration (runtime + bracket tags)
//            - Rich failure diagnostics: catalog truth + what fired + reproduce
//            - Fixture access tracking with cache reuse metrics
//            - Structured result breakdowns on assertion failure
//
//          Used by: R50 (existence), R25 (organization), R10 (content), R05 (pattern)
//          The layer is embedded in the error code (CWS-R50-*, CWS-R25-*, etc.)
//          The runner doesn't need to know about layers — it just verifies codes.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import type { LintResult } from "@lib/foundation/mod.ts";
import {
  fixture, lazyFormat, errors, warnings, infos,
  byRule, hasRule, hasMessage, hasErrorCode, errorCodeReport,
  byErrorCode, errorCodeSummary,
  witnessFor, diagnosticCommand,
} from "@test/helpers.ts";
import {
  getByCode, dimensionalCode, categoryToDomain,
} from "@lib/data/errors.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Types — The test specification
// ---------------------------------------------------------------------------

/**
 * A single test case. The minimum is: code + fixture + desc.
 * Everything else is either derived from the error catalog or optional.
 *
 * This IS the debugger in disguise — each row maps an error code to a
 * fixture that triggers it. Navigate the table to navigate the system.
 */
export interface TestCase {
  /** CWS error code this test witnesses. */
  code: string;

  /** Fixture path relative to tests/fixtures/. */
  fixture: string;

  /** Human-readable test description. */
  desc: string;

  /** Expected rule fragment (partial match on result.rule). */
  rule?: string;

  /**
   * Override severity check. If omitted, severity is derived from the
   * error catalog entry for this code. That's the point — the catalog
   * is authoritative, not the test.
   */
  severity?: "error" | "warn" | "info";

  /** Fragments that MUST appear in at least one matching result message. */
  messageContains?: string[];

  /** Minimum count of results matching this code. Default: 1. */
  minCount?: number;

  /** "Clean" fixture that should NOT trigger this code. */
  cleanFixture?: string;

  /** Clean fixture description (defaults to "clean — no {code}"). */
  cleanDesc?: string;

  /** If true, at least one matching result must have a `.fix` property. */
  hasFix?: boolean;

  /** If hasFix, validate fix action types. */
  fixActions?: string[];

  /** Extended validation for complex cases. Called with ALL lint results. */
  validate?: (results: LintResult[]) => void;

  /**
   * Skip this test case (with reason). Useful for WIP or known issues
   * without removing the specification.
   */
  skip?: string;
}

/**
 * A format's test specification for a given layer/concern.
 * Format name + optional label + all cases.
 */
export interface FormatSpec {
  /** Format handler name (e.g., "go", "rust", "toml"). */
  format: string;

  /**
   * Optional label for test names (e.g., "R50", "R25/setup").
   * Defaults to layer extracted from error code.
   */
  label?: string;

  /** Test cases — the table that defines what gets tested. */
  cases: TestCase[];
}

// Backward-compatible aliases for R10 imports
export type R10TestCase = TestCase;
export type R10FormatSpec = FormatSpec;

// ---------------------------------------------------------------------------
// Suite tracking — statistics collected as suites register
// ---------------------------------------------------------------------------

/**
 * Record of what a single runSuite() call registered.
 * Collected synchronously during test registration (import-time),
 * consumed by the suite summary test (run-time).
 */
interface SuiteRecord {
  format: string;
  label: string;
  totalCases: number;
  skippedCases: number;
  uniqueFixtures: Set<string>;
  codesWitnessed: string[];
  cleanTests: number;
}

/** All suites registered this run. Accessible via getSuiteRecords(). */
const _suiteRecords: SuiteRecord[] = [];

// ---------------------------------------------------------------------------
// Fixture cache and stats
// ---------------------------------------------------------------------------

/**
 * Fixture result cache — lint each file ONCE, share results across all tests.
 *
 * The bottleneck was: 5 tests lint `valid-library.rs` = 5 full pipeline runs.
 * Now: first test lints it, subsequent tests get cached results instantly.
 * Key = "format:fixturePath" to prevent cross-format collisions.
 */
const _fixtureCache = new Map<string, Promise<LintResult[]>>();

/**
 * Per-fixture access tracking — accesses + cache reuses.
 * Consumed by suite summaries and getFixtureStats().
 */
const _fixtureStats = new Map<string, { accesses: number; reuses: number }>();

/**
 * Lint a fixture file, returning cached results if available.
 * Accepts either a relative fixture path (resolved via fixture()) or an
 * absolute path (used as-is).
 *
 * Tracks access stats: every call increments accesses;
 * cache returns increment reuses. The first lint is the real work;
 * all subsequent calls for the same format:path are free.
 */
export function lintOnce(
  handler: import("@lib/foundation/mod.ts").FormatHandler,
  format: string,
  fixturePath: string,
): Promise<LintResult[]> {
  const resolvedPath = fixturePath.startsWith("/") ? fixturePath : fixture(fixturePath);
  const key = `${format}:${resolvedPath}`;

  // Track access stats
  const stats = _fixtureStats.get(key) ?? { accesses: 0, reuses: 0 };
  stats.accesses++;

  const existing = _fixtureCache.get(key);
  if (existing) {
    stats.reuses++;
    _fixtureStats.set(key, stats);
    return existing;
  }

  _fixtureStats.set(key, stats);
  const promise = handler.lint(resolvedPath);
  _fixtureCache.set(key, promise);
  return promise;
}

// ---------------------------------------------------------------------------
// Diagnostics — rich failure context
// ---------------------------------------------------------------------------

/** Severity indicator for test names. */
function severityTag(severity: string): string {
  switch (severity) {
    case "error": return "(E)";
    case "warn": return "(W)";
    case "info": return "(I)";
    default: return "";
  }
}

/**
 * Build a rich diagnostic message for when an expected error code doesn't fire.
 *
 * Shows:
 *   1. What the catalog says about the expected code
 *   2. What codes DID fire from the same fixture (grouped, counted)
 *   3. Result breakdown by severity
 *   4. The exact `deno test --filter` command to reproduce
 *
 * This is the difference between "Expected CWS-R50-020, got: <wall of text>"
 * and actually understanding WHY the test failed.
 */
function formatDiagnostic(tc: TestCase, results: LintResult[]): string {
  const entry = getByCode(tc.code);
  const lines: string[] = [
    `${tc.code} did not fire.`,
    "",
  ];

  // What the catalog says this code IS
  if (entry) {
    const dim = dimensionalCode(entry);
    const domain = categoryToDomain(entry.category);
    lines.push(
      "  Expected:",
      `    Code:     ${entry.code} [${dim}]`,
      `    Layer:    ${entry.layerName}`,
      `    Domain:   ${entry.category} (${domain})`,
      `    Severity: ${entry.severity}`,
      `    Template: "${entry.messageTemplate}"`,
      `    Check:    ${entry.checkFunction}`,
      "",
    );
  } else {
    lines.push(
      "  Expected:",
      `    Code: ${tc.code} — NOT FOUND IN CATALOG`,
      "",
    );
  }

  // What DID fire from this fixture
  if (results.length > 0) {
    const summary = errorCodeSummary(results);
    const errs = results.filter(r => r.severity === "error").length;
    const warns = results.filter(r => r.severity === "warn").length;
    const infs = results.filter(r => r.severity === "info").length;

    lines.push(
      `  Actual (${results.length} results: ${errs}E ${warns}W ${infs}I):`,
    );

    for (const [code, count] of summary) {
      if (code === "UNCATALOGED") {
        lines.push(`    ${count}x  UNCATALOGED`);
      } else {
        const e = getByCode(code);
        const sev = e ? severityTag(e.severity) : "";
        const cat = e ? `[${e.category}]` : "";
        lines.push(`    ${count}x  ${code} ${cat} ${sev}`);
      }
    }
    lines.push("");
  } else {
    lines.push("  Actual: 0 results (fixture produced nothing)");
    lines.push("");
  }

  lines.push(`  Reproduce: ${diagnosticCommand(tc.code)}`);

  return lines.join("\n");
}

/**
 * Build a structured result breakdown for assertion failures.
 * Groups results by severity with rule and first line of message.
 */
function resultBreakdown(results: LintResult[], filterFn?: (r: LintResult) => boolean): string {
  const filtered = filterFn ? results.filter(filterFn) : results;
  if (filtered.length === 0) return "(none)";

  const lines: string[] = [];
  for (const r of filtered) {
    const tag = severityTag(r.severity);
    const msg = r.message.length > 80
      ? r.message.substring(0, 77) + "..."
      : r.message;
    lines.push(`  ${tag} ${r.rule}: ${msg}`);
  }

  return lines.join("\n");
}

// ---------------------------------------------------------------------------
// Runner — Generate tests from specifications
// ---------------------------------------------------------------------------

/**
 * Generate Deno.test() calls from a format specification.
 *
 * For each case:
 *   1. Lints the fixture (cached — free after first call for same file)
 *   2. Registers witness coverage (auto-witness integration)
 *   3. Verifies error code exists in catalog
 *   4. Verifies error code fires (with rich diagnostic on failure)
 *   5. Verifies severity matches error catalog (source of truth)
 *   6. Verifies minimum count
 *   7. Verifies rule fragment if specified
 *   8. Verifies message content if messageContains is specified
 *   9. Verifies fix actions if hasFix is specified
 *  10. Runs extended validation if validate is specified
 *  11. Creates a clean counterpart test if cleanFixture is specified
 *
 * After all cases: generates a suite summary test with steps showing
 * fixture stats, codes witnessed, and cache efficiency.
 *
 * Returns the SuiteRecord for external inspection.
 */
export function runSuite(spec: FormatSpec): SuiteRecord {
  const handler = lazyFormat(spec.format);
  const label = spec.label ?? "";

  // Build suite record (synchronous — populated during registration)
  const record: SuiteRecord = {
    format: spec.format,
    label: label || spec.format,
    totalCases: spec.cases.length,
    skippedCases: 0,
    uniqueFixtures: new Set(),
    codesWitnessed: [],
    cleanTests: 0,
  };

  for (const tc of spec.cases) {
    // Derive layer label from error code if no spec label: CWS-R50-022 → "R50"
    const testLabel = label || tc.code.replace(/^CWS-(R\d+)-.*/, "$1");

    // Derive severity for name tag from catalog (or override)
    const catalogEntry = getByCode(tc.code);
    const sev = tc.severity ?? catalogEntry?.severity;
    const tag = sev ? ` ${severityTag(sev)}` : "";

    const testPrefix = `[${tc.code}] ${testLabel}/${tc.fixture}`;
    const testName = `${testPrefix}: ${tc.desc}${tag}`;

    // Track fixtures
    record.uniqueFixtures.add(tc.fixture);

    // Skip if marked
    if (tc.skip) {
      record.skippedCases++;
      Deno.test({
        name: testName,
        ignore: true,
        fn: () => {},
      });
      continue;
    }

    // Track witnessed codes
    record.codesWitnessed.push(tc.code);

    // --- Primary test: error code fires ---
    Deno.test(testName, async () => {
      // Auto-witness: register this test with the witness system
      witnessFor(tc.code, testName);

      const results = await lintOnce(handler, spec.format, tc.fixture);

      // 1. Verify the error code exists in the catalog
      assert(
        catalogEntry,
        `${tc.code} should exist in error catalog — is it registered in lib/data/errors.ts?`,
      );

      // 2. Verify the error code fires — with rich diagnostic on failure
      assert(
        hasErrorCode(results, tc.code),
        formatDiagnostic(tc, results),
      );

      // 3. Verify severity matches catalog (or override)
      const matching = byErrorCode(results, tc.code);
      const expectedSeverity = tc.severity ?? catalogEntry!.severity;
      assertEquals(
        matching[0]!.severity,
        expectedSeverity,
        `${tc.code} severity: expected "${expectedSeverity}" (${tc.severity ? "override" : "catalog"}), got "${matching[0]!.severity}"`,
      );

      // 4. Verify minimum count
      const minCount = tc.minCount ?? 1;
      assertGreater(
        matching.length,
        minCount - 1,
        `${tc.code}: expected at least ${minCount} result(s), got ${matching.length}`,
      );

      // 5. Verify rule fragment
      if (tc.rule) {
        assert(
          matching.some((r: LintResult) => r.rule.includes(tc.rule!)),
          `${tc.code}: expected rule containing "${tc.rule}", got: ${matching.map((r: LintResult) => r.rule).join(", ")}`,
        );
      }

      // 6. Verify message content
      for (const frag of tc.messageContains ?? []) {
        assert(
          hasMessage(matching, frag),
          `${tc.code}: expected "${frag}" in messages, got: ${matching.map((r: LintResult) => r.message).join(" | ")}`,
        );
      }

      // 7. Verify fix actions
      if (tc.hasFix) {
        // deno-lint-ignore no-explicit-any
        const withFix = matching.filter((r: any) => r.fix);
        assertGreater(
          withFix.length, 0,
          `${tc.code}: expected at least one result with a fix suggestion`,
        );
        if (tc.fixActions) {
          // deno-lint-ignore no-explicit-any
          const fix = (withFix[0] as any).fix;
          assertEquals(
            fix.actions.length,
            tc.fixActions.length,
            `${tc.code}: expected ${tc.fixActions.length} fix actions, got ${fix.actions.length}`,
          );
          for (let i = 0; i < tc.fixActions.length; i++) {
            assertEquals(
              fix.actions[i].type,
              tc.fixActions[i],
              `${tc.code}: fix action ${i} should be "${tc.fixActions[i]}"`,
            );
          }
        }
      }

      // 8. Extended validation
      if (tc.validate) {
        tc.validate(results);
      }
    });

    // --- Clean counterpart: code should NOT fire ---
    if (tc.cleanFixture) {
      record.cleanTests++;
      record.uniqueFixtures.add(tc.cleanFixture);
      const cleanLabel = label || tc.code.replace(/^CWS-(R\d+)-.*/, "$1");
      const cleanDesc = tc.cleanDesc ?? `clean — no ${tc.code}`;
      Deno.test(`${cleanLabel}/${tc.cleanFixture}: ${cleanDesc}`, async () => {
        const results = await lintOnce(handler, spec.format, tc.cleanFixture!);
        const matching = byErrorCode(results, tc.code);
        assertEquals(
          matching.length, 0,
          `Clean file should not trigger ${tc.code}, got: ${errorCodeReport(matching)}`,
        );
      });
    }
  }

  // --- Suite summary ---
  _suiteRecords.push(record);
  const activeCases = record.totalCases - record.skippedCases;

  Deno.test(`${record.label} suite — ${activeCases} cases, ${spec.format}`, async (t) => {
    // Fixture efficiency
    const totalTestFns = activeCases + record.cleanTests;
    const uniqueCount = record.uniqueFixtures.size;
    const reuses = totalTestFns > uniqueCount ? totalTestFns - uniqueCount : 0;

    await t.step(
      `${uniqueCount} fixture${uniqueCount !== 1 ? "s" : ""}${reuses > 0 ? ` (${reuses} cache reuse${reuses !== 1 ? "s" : ""})` : ""}`,
      () => {},
    );

    await t.step(
      `${record.codesWitnessed.length} code${record.codesWitnessed.length !== 1 ? "s" : ""} witnessed`,
      () => {},
    );

    if (record.cleanTests > 0) {
      await t.step(
        `${record.cleanTests} clean counterpart${record.cleanTests !== 1 ? "s" : ""}`,
        () => {},
      );
    }

    if (record.skippedCases > 0) {
      await t.step(
        `${record.skippedCases} skipped`,
        () => {},
      );
    }
  });

  return record;
}

// Backward-compatible alias
export const runR10Suite = runSuite;

// ---------------------------------------------------------------------------
// Stats — external access to tracking data
// ---------------------------------------------------------------------------

/** Clear the fixture cache. Useful if running tests that modify fixture files. */
export function clearFixtureCache(): void {
  _fixtureCache.clear();
  _fixtureStats.clear();
}

/**
 * Get fixture access stats — path → { accesses, reuses }.
 * Useful for diagnosing cache efficiency or detecting unexpected re-lints.
 */
export function getFixtureStats(): ReadonlyMap<string, { accesses: number; reuses: number }> {
  return _fixtureStats;
}

/**
 * Get all suite records registered this run.
 * Each record captures what runSuite() registered: cases, fixtures, codes.
 */
export function getSuiteRecords(): readonly SuiteRecord[] {
  return _suiteRecords;
}

// ---------------------------------------------------------------------------
// Helpers — Reusable validation functions for complex cases
// ---------------------------------------------------------------------------

/**
 * Validate that a fixture produces zero errors (only warnings/info allowed).
 * Useful as a `validate` function for tests where the code is a warning.
 */
export function noErrors(results: LintResult[]): void {
  const errs = errors(results);
  assertEquals(errs.length, 0,
    `Expected 0 errors:\n${resultBreakdown(errs)}`);
}

/** Validate zero warnings. */
export function noWarnings(results: LintResult[]): void {
  const warns = warnings(results);
  assertEquals(warns.length, 0,
    `Expected 0 warnings:\n${resultBreakdown(warns)}`);
}

/**
 * Validate that a fixture produces zero placement warnings.
 * Useful for "correct placement" clean tests.
 */
export function noPlacementWarnings(results: LintResult[]): void {
  const placement = byRule(results, "content/");
  assertEquals(
    placement.length, 0,
    `Expected 0 content warnings:\n${resultBreakdown(placement)}`,
  );
}

/**
 * Validate the identity registration info pattern:
 * has identity/register info, no errors.
 */
export function hasIdentityRegisterInfo(results: LintResult[]): void {
  const errs = errors(results);
  assertEquals(errs.length, 0,
    `Identity registration should not produce errors:\n${resultBreakdown(errs)}`);
  assert(hasRule(results, "identity/register"), "Should produce identity/register info");
  const regInfos = infos(results).filter((r: LintResult) => r.rule.includes("identity/register"));
  assertGreater(regInfos.length, 0, "Should have at least one identity registration info");
}

/**
 * Validate no content/PRAGMA or content/METADATA false positives.
 */
export function noContentPragmaWarnings(results: LintResult[]): void {
  const contentPragma = byRule(results, "content/PRAGMA/");
  const contentMeta = byRule(results, "content/METADATA/");
  assertEquals(contentPragma.length, 0,
    `Expected 0 content/PRAGMA:\n${resultBreakdown(contentPragma)}`);
  assertEquals(contentMeta.length, 0,
    `Expected 0 content/METADATA:\n${resultBreakdown(contentMeta)}`);
}

/**
 * Validate no doc warnings.
 */
export function noDocWarnings(results: LintResult[]): void {
  const docRules = byRule(results, "doc/");
  assertEquals(docRules.length, 0,
    `Expected 0 doc warnings:\n${resultBreakdown(docRules)}`);
}

/**
 * Validate no closing zone warnings.
 */
export function noClosingWarnings(results: LintResult[]): void {
  const zoneWarns = byRule(results, "closing/");
  assertEquals(zoneWarns.length, 0,
    `Expected 0 closing warnings:\n${resultBreakdown(zoneWarns)}`);
}

/**
 * Validate no body subsection order warnings.
 */
export function noBodyOrderWarnings(results: LintResult[]): void {
  const orderWarns = byRule(results, "body/subsection-order");
  assertEquals(orderWarns.length, 0,
    `Expected 0 body order warnings:\n${resultBreakdown(orderWarns)}`);
}

/**
 * Validate no value/PRAGMA or value/METADATA warnings.
 */
export function noValueWarnings(results: LintResult[]): void {
  const contentWarns = warnings(results).filter((r: LintResult) =>
    r.rule.startsWith("value/PRAGMA/") || r.rule.startsWith("value/METADATA/"));
  assertEquals(contentWarns.length, 0,
    `Expected 0 value warnings:\n${resultBreakdown(contentWarns)}`);
}

/**
 * Validate no depth warnings.
 */
export function noDepthWarnings(results: LintResult[]): void {
  const depth = results.filter((r: LintResult) => r.rule.includes("-depth"));
  assertEquals(depth.length, 0,
    `Expected 0 depth warnings:\n${resultBreakdown(depth)}`);
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Shared data-driven test runner — universal table-driven test generation.
// The error catalog is the source of truth. Tests derive expectations from it.
// Add a row → test exists. Change the catalog → tests follow.
//
// v2 QoL: severity tags, auto-witness, rich diagnostics, suite summaries,
// fixture stats, structured failure output. The tests don't just pass —
// they TELL you what happened. Failures explain themselves.
//
// Used by ALL layers: R50, R25, R10, R05.
// The layer is just an error code prefix. The runner is layer-agnostic.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
