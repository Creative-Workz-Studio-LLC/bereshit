// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/_transform_runner.ts
// key:     B-tov-cws-struct-tests-transform-runner
// title:   Shared Data-Driven Transform Test Runner
// type:    Code (Test Infrastructure)
// version: a-01.00
// created: 2026-02-24
// updated: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Table-driven test generation for transform operations.
//          Like _runner.ts but for transforms: dry-run detection,
//          actual file mutation, post-transform lint verification,
//          and idempotency checks.
//
//          Add a row to the table → three tests exist:
//            [dry-run]    → detects operations needed
//            [actual]     → transforms file + verifies lint improvement
//            [idempotent] → second transform produces no changes
//
//          Uses temp files for actual transforms — fixtures are never mutated.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import type { LintResult } from "@lib/foundation/mod.ts";
import { fixture, lazyFormat, byRule, hasRule, witnessFor } from "@test/helpers.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Types — The transform test specification
// ---------------------------------------------------------------------------

/** Default transform options passed to handler.transform(). */
const DEFAULT_TRANSFORM_OPTS = {
  dryRun: false,
  extensions: false,
  force: false,
  steps: false,
} as const;

/**
 * A single transform test case.
 *
 * Each case exercises one fixture through three phases:
 *   1. Dry-run: verify operations are detected
 *   2. Actual: transform + re-lint to verify improvement
 *   3. Idempotent: transform again → no structural changes
 */
export interface TransformTestCase {
  /** Fixture path relative to tests/fixtures/. */
  fixture: string;

  /** Human-readable description of what this transform tests. */
  desc: string;

  /**
   * Transform rules expected in dry-run results.
   * Partial match — "move-tests" matches "transform/move-tests".
   * If empty, this is a "valid file — no transforms needed" case.
   */
  expectedRules: string[];

  /**
   * Lint rules that should NOT appear after actual transform.
   * Verifies the transform actually fixed what it claimed.
   * Each entry is a partial match on result.rule.
   */
  resolvedRules?: string[];

  /**
   * Expected messages in transform results (partial match).
   * Useful for verifying the transform explains what it did.
   */
  messageContains?: string[];

  /**
   * Content checks after actual transform.
   * Each entry: { contains: string, in: "body" | "closing" | "setup" | "file" }
   * Verifies content moved to the right location.
   */
  contentChecks?: ContentCheck[];

  /**
   * If true (default), run a third test verifying the second transform
   * produces no structural changes. Set false for transforms that aren't
   * expected to be idempotent (rare — most should be).
   */
  idempotent?: boolean;

  /**
   * CWS witness code for bracket-tag test names.
   * If provided, tests will include [CWS-xxx] in their names.
   */
  witnessCode?: string;

  /** Skip this test case (with reason). */
  skip?: string;

  /** Extended validation on the actual transform results. */
  validate?: (results: LintResult[], postLintResults: LintResult[], content: string) => void;
}

/** Content location check after transform. */
interface ContentCheck {
  /** String that must appear in the specified block. */
  contains: string;
  /** Block to check: body, closing, setup, or entire file. */
  in: "body" | "closing" | "setup" | "file";
  /** If true, assert the string does NOT appear in this block. */
  absent?: boolean;
}

/**
 * A format's transform test specification.
 * Format name + all cases.
 */
export interface TransformFormatSpec {
  /** Format handler name (e.g., "go", "rust"). */
  format: string;

  /** Optional label for test names (e.g., "rust/transform"). */
  label?: string;

  /** Transform test cases — the table that drives everything. */
  cases: TransformTestCase[];
}

// ---------------------------------------------------------------------------
// Suite tracking
// ---------------------------------------------------------------------------

interface TransformSuiteRecord {
  format: string;
  label: string;
  totalCases: number;
  skippedCases: number;
  dryRunTests: number;
  actualTests: number;
  idempotentTests: number;
  uniqueFixtures: Set<string>;
}

const _suiteRecords: TransformSuiteRecord[] = [];

// ---------------------------------------------------------------------------
// Block extraction — locate content within transformed files
// ---------------------------------------------------------------------------

/** Extract content of a named block from a file's text. */
function extractBlock(content: string, blockName: string): string {
  const upper = blockName.toUpperCase();
  // Look for "// BLOCK_NAME" or "// ═══ ... BLOCK_NAME ... ═══"
  const patterns = [
    new RegExp(`^//\\s*${upper}\\b`, "m"),
    new RegExp(`^//\\s*═+\\s*\\n//\\s*${upper}\\b`, "m"),
  ];

  let startIdx = -1;
  for (const pat of patterns) {
    const match = content.match(pat);
    if (match && match.index !== undefined) {
      startIdx = match.index;
      break;
    }
  }

  if (startIdx === -1) return "";

  // Find end: next block marker or EOF
  const endPatterns = [
    /^\/\/\s*(?:SETUP|BODY|CLOSING|METADATA|END\s)/m,
    /^\/\/\s*═{3,}/m,
  ];

  const rest = content.slice(startIdx + upper.length);
  let endIdx = rest.length;

  for (const ep of endPatterns) {
    const match = rest.match(ep);
    if (match && match.index !== undefined && match.index < endIdx) {
      endIdx = match.index;
    }
  }

  return rest.slice(0, endIdx);
}

// ---------------------------------------------------------------------------
// Runner — Generate tests from transform specifications
// ---------------------------------------------------------------------------

/**
 * Generate Deno.test() calls from a transform format specification.
 *
 * For each case with expectedRules:
 *   [dry-run]    — verify rules fire without modifying fixture
 *   [actual]     — transform temp copy, verify rules fire + lint improves
 *   [idempotent] — transform again, verify no structural changes
 *
 * For each case with empty expectedRules:
 *   [no-op]      — verify no structural transforms needed
 */
export function runTransformSuite(spec: TransformFormatSpec): TransformSuiteRecord {
  const handler = lazyFormat(spec.format);
  const label = spec.label ?? `${spec.format}/transform`;

  const record: TransformSuiteRecord = {
    format: spec.format,
    label,
    totalCases: spec.cases.length,
    skippedCases: 0,
    dryRunTests: 0,
    actualTests: 0,
    idempotentTests: 0,
    uniqueFixtures: new Set(),
  };

  for (const tc of spec.cases) {
    const prefix = tc.witnessCode ? `[${tc.witnessCode}] ` : "";
    const testBase = `${prefix}${label}/${tc.fixture}`;

    record.uniqueFixtures.add(tc.fixture);

    if (tc.skip) {
      record.skippedCases++;
      Deno.test({ name: `${testBase}: ${tc.desc} (skipped)`, ignore: true, fn: () => {} });
      continue;
    }

    const isNoOp = tc.expectedRules.length === 0;

    if (isNoOp) {
      // ── No-op case: valid file needs no transforms ──
      record.dryRunTests++;
      Deno.test(`${testBase}: ${tc.desc} [no-op]`, async () => {
        if (tc.witnessCode) witnessFor(tc.witnessCode, `${testBase}: ${tc.desc} [no-op]`);
        const f = fixture(tc.fixture);
        const results = await handler.transform!(f, { ...DEFAULT_TRANSFORM_OPTS, dryRun: true });

        // Filter to structural moves only — separator fixes are cosmetic
        const moves = results.filter((r) =>
          r.rule.includes("move-tests") ||
          r.rule.includes("move-main") ||
          r.rule.includes("reorder-closing") ||
          r.rule.includes("scaffold"),
        );
        assertEquals(moves.length, 0, `Valid file should need no structural transforms, got: ${moves.map(r => r.rule).join(", ")}`);
      });
      continue;
    }

    // ── Dry-run test: verify detection ──
    record.dryRunTests++;
    Deno.test(`${testBase}: ${tc.desc} [dry-run]`, async () => {
      if (tc.witnessCode) witnessFor(tc.witnessCode, `${testBase}: ${tc.desc} [dry-run]`);
      const f = fixture(tc.fixture);

      // Verify fixture isn't modified
      const before = await Deno.readTextFile(f);
      const results = await handler.transform!(f, { ...DEFAULT_TRANSFORM_OPTS, dryRun: true });
      const after = await Deno.readTextFile(f);
      assertEquals(before, after, "Dry-run must not modify the fixture file");

      // Verify expected rules fire
      for (const rule of tc.expectedRules) {
        assert(
          hasRule(results, rule),
          `Expected rule "${rule}" in dry-run results. Got: ${results.map(r => r.rule).join(", ")}`,
        );
      }

      // Verify expected messages
      for (const frag of tc.messageContains ?? []) {
        assert(
          results.some(r => r.message.includes(frag)),
          `Expected "${frag}" in dry-run messages. Got: ${results.map(r => r.message).join(" | ")}`,
        );
      }
    });

    // ── Actual transform test: mutate temp copy + verify ──
    record.actualTests++;
    Deno.test(`${testBase}: ${tc.desc} [actual]`, async () => {
      if (tc.witnessCode) witnessFor(tc.witnessCode, `${testBase}: ${tc.desc} [actual]`);
      const src = fixture(tc.fixture);
      const ext = tc.fixture.split(".").pop() ?? "";
      const tmp = await Deno.makeTempFile({ suffix: `.${ext}` });
      await Deno.copyFile(src, tmp);

      try {
        const results = await handler.transform!(tmp, DEFAULT_TRANSFORM_OPTS);

        // Verify expected rules fire
        for (const rule of tc.expectedRules) {
          assert(
            hasRule(results, rule),
            `Expected rule "${rule}" in actual transform results. Got: ${results.map(r => r.rule).join(", ")}`,
          );
        }

        // Verify "written" rule fires (file was actually modified)
        assert(
          hasRule(results, "written"),
          `Expected "written" rule — file should have been modified. Got: ${results.map(r => r.rule).join(", ")}`,
        );

        // Re-lint: verify resolved rules are gone
        const lintResults = await handler.lint(tmp);
        for (const resolved of tc.resolvedRules ?? []) {
          const remaining = byRule(lintResults, resolved);
          assertEquals(
            remaining.length, 0,
            `Rule "${resolved}" should be resolved after transform, still found ${remaining.length}: ${remaining.map(r => `${r.rule}: ${r.message}`).join("; ")}`,
          );
        }

        // Content checks
        const content = await Deno.readTextFile(tmp);
        for (const check of tc.contentChecks ?? []) {
          const block = check.in === "file" ? content : extractBlock(content, check.in);
          if (check.absent) {
            assertEquals(
              block.includes(check.contains), false,
              `"${check.contains}" should NOT appear in ${check.in} block`,
            );
          } else {
            assert(
              block.includes(check.contains),
              `"${check.contains}" should appear in ${check.in} block`,
            );
          }
        }

        // Extended validation
        if (tc.validate) {
          tc.validate(results, lintResults, content);
        }
      } finally {
        await Deno.remove(tmp);
      }
    });

    // ── Idempotency test: transform again, no changes ──
    const runIdempotent = tc.idempotent !== false; // default: true
    if (runIdempotent) {
      record.idempotentTests++;
      Deno.test(`${testBase}: ${tc.desc} [idempotent]`, async () => {
        const src = fixture(tc.fixture);
        const ext = tc.fixture.split(".").pop() ?? "";
        const tmp = await Deno.makeTempFile({ suffix: `.${ext}` });
        await Deno.copyFile(src, tmp);

        try {
          // First transform
          await handler.transform!(tmp, DEFAULT_TRANSFORM_OPTS);
          const afterFirst = await Deno.readTextFile(tmp);

          // Second transform (dry-run to detect what would change)
          const secondResults = await handler.transform!(tmp, { ...DEFAULT_TRANSFORM_OPTS, dryRun: true });

          // Filter to structural moves — cosmetic changes (separators) may still trigger
          const structuralMoves = secondResults.filter((r) =>
            r.rule.includes("move-tests") ||
            r.rule.includes("move-main") ||
            r.rule.includes("reorder-closing"),
          );

          assertEquals(
            structuralMoves.length, 0,
            `Second transform should detect no structural changes. Got: ${structuralMoves.map(r => r.rule).join(", ")}`,
          );

          // Verify file content unchanged after second pass (actual, not dry-run)
          await handler.transform!(tmp, DEFAULT_TRANSFORM_OPTS);
          const afterSecond = await Deno.readTextFile(tmp);
          assertEquals(afterFirst, afterSecond, "File should be identical after second transform");
        } finally {
          await Deno.remove(tmp);
        }
      });
    }
  }

  // ── Suite summary ──
  _suiteRecords.push(record);
  const active = record.totalCases - record.skippedCases;
  const totalTests = record.dryRunTests + record.actualTests + record.idempotentTests;

  Deno.test(`${label} suite — ${active} cases → ${totalTests} tests`, async (t) => {
    await t.step(
      `${record.dryRunTests} dry-run, ${record.actualTests} actual, ${record.idempotentTests} idempotent`,
      () => {},
    );

    await t.step(
      `${record.uniqueFixtures.size} unique fixture${record.uniqueFixtures.size !== 1 ? "s" : ""}`,
      () => {},
    );

    if (record.skippedCases > 0) {
      await t.step(`${record.skippedCases} skipped`, () => {});
    }
  });

  return record;
}

/** Get all transform suite records. */
export function getTransformSuiteRecords(): readonly TransformSuiteRecord[] {
  return _suiteRecords;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Data-driven transform test runner — table-driven test generation for
// structural transforms. Each row generates up to 3 tests:
// dry-run, actual, and idempotent.
//
// The pattern: fixture → transform → lint → verify improvement.
// Idempotency proves the transform converges.
//
// "Be ye transformed by the renewing of your mind." — Romans 12:2
// ============================================================================
