// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/_transpile_runner.ts
// key:     B-tov-cws-struct-tests-transpile-runner
// title:   Shared Data-Driven Transpile Test Runner
// type:    Code (Test Infrastructure)
// version: a-01.00
// created: 2026-02-24
// updated: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Table-driven test generation for transpile operations.
//          Each case transpiles a fixture between formats and verifies:
//            1. Transpilation succeeds (or fails as expected)
//            2. Output has expected structure (4-block, sections, stubs)
//            3. Stats meet minimums (chunks, items, lines)
//            4. Expected content appears in output
//            5. Diagnostics match expectations
//
//          The transpiler produces structural skeletons — correct layout,
//          section headers, identity, function stubs. Method bodies are TODOs.
//          These tests prove architecture, not parsing.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import { fixture, witnessFor } from "@test/helpers.ts";
import { transpileFile } from "@lib/engine/mod.ts";
import type { TranspileResult } from "@lib/transpiler/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Types — The transpile test specification
// ---------------------------------------------------------------------------

/**
 * A single transpile test case.
 */
export interface TranspileTestCase {
  /** Source format (e.g., "rust"). */
  sourceFormat: string;

  /** Target format (e.g., "go"). */
  targetFormat: string;

  /** Fixture path relative to tests/fixtures/. */
  fixture: string;

  /** Human-readable description. */
  desc: string;

  /**
   * Expected: transpilation succeeds. Default: true.
   * Set false for error-path tests.
   */
  expectSuccess?: boolean;

  /** Minimum output line count. */
  minOutputLines?: number;

  /** Minimum chunks extracted. */
  minChunks?: number;

  /** Minimum items extracted. */
  minItems?: number;

  /**
   * Strings that MUST appear in the transpiled output.
   * Verifies structural elements transferred correctly.
   */
  outputContains?: string[];

  /**
   * Strings that must NOT appear in the transpiled output.
   * Catches source-language artifacts that shouldn't carry over.
   */
  outputAbsent?: string[];

  /**
   * Expected diagnostics. Each entry checks severity and optional message fragment.
   */
  expectedDiagnostics?: DiagnosticCheck[];

  /**
   * CWS witness code for bracket-tag test names.
   */
  witnessCode?: string;

  /** Skip this test case (with reason). */
  skip?: string;

  /** Extended validation on the transpile result. */
  validate?: (result: TranspileResult) => void;
}

/** Diagnostic expectation. */
interface DiagnosticCheck {
  severity: "error" | "warn" | "info";
  messageContains?: string;
}

/**
 * A transpile test specification — source→target direction + all cases.
 */
export interface TranspileSpec {
  /** Label for test names (e.g., "rust→go"). */
  label: string;

  /** Transpile test cases. */
  cases: TranspileTestCase[];
}

// ---------------------------------------------------------------------------
// Suite tracking
// ---------------------------------------------------------------------------

interface TranspileSuiteRecord {
  label: string;
  totalCases: number;
  skippedCases: number;
  successCases: number;
  errorCases: number;
  uniqueFixtures: Set<string>;
}

const _suiteRecords: TranspileSuiteRecord[] = [];

// ---------------------------------------------------------------------------
// Runner — Generate tests from transpile specifications
// ---------------------------------------------------------------------------

/**
 * Generate Deno.test() calls from a transpile specification.
 *
 * For each case:
 *   1. Transpile fixture (dry-run — no file output)
 *   2. Verify success/failure matches expectation
 *   3. Verify output content
 *   4. Verify stats minimums
 *   5. Verify diagnostics
 */
export function runTranspileSuite(spec: TranspileSpec): TranspileSuiteRecord {
  const label = spec.label;

  const record: TranspileSuiteRecord = {
    label,
    totalCases: spec.cases.length,
    skippedCases: 0,
    successCases: 0,
    errorCases: 0,
    uniqueFixtures: new Set(),
  };

  for (const tc of spec.cases) {
    const prefix = tc.witnessCode ? `[${tc.witnessCode}] ` : "";
    const testName = `${prefix}${label}/${tc.fixture}: ${tc.desc}`;

    record.uniqueFixtures.add(tc.fixture);

    if (tc.skip) {
      record.skippedCases++;
      Deno.test({ name: testName, ignore: true, fn: () => {} });
      continue;
    }

    const expectSuccess = tc.expectSuccess !== false; // default: true
    if (expectSuccess) record.successCases++;
    else record.errorCases++;

    Deno.test(testName, async () => {
      if (tc.witnessCode) witnessFor(tc.witnessCode, testName);
      const f = fixture(tc.fixture);

      const result = await transpileFile(
        f,
        tc.sourceFormat as "rust" | "go",
        tc.targetFormat as "rust" | "go",
        { dryRun: true },
      );

      // 1. Verify success/failure
      assertEquals(
        result.success,
        expectSuccess,
        expectSuccess
          ? `Transpilation should succeed. Diagnostics: ${result.diagnostics.map(d => `${d.severity}: ${d.message}`).join("; ")}`
          : `Transpilation should fail.`,
      );

      if (!expectSuccess) {
        // For error cases, just verify diagnostics and return
        for (const check of tc.expectedDiagnostics ?? []) {
          assert(
            result.diagnostics.some((d) =>
              d.severity === check.severity &&
              (!check.messageContains || d.message.includes(check.messageContains)),
            ),
            `Expected diagnostic: ${check.severity}${check.messageContains ? ` containing "${check.messageContains}"` : ""}`,
          );
        }
        if (tc.validate) tc.validate(result);
        return;
      }

      // 2. Verify output exists
      assert(result.output.length > 0, "Transpiled output should not be empty");

      // 3. Verify stats minimums
      if (tc.minOutputLines) {
        assertGreater(
          result.lineCount,
          tc.minOutputLines - 1,
          `Expected at least ${tc.minOutputLines} output lines, got ${result.lineCount}`,
        );
      }
      if (tc.minChunks) {
        assertGreater(
          result.stats.chunksExtracted,
          tc.minChunks - 1,
          `Expected at least ${tc.minChunks} chunks, got ${result.stats.chunksExtracted}`,
        );
      }
      if (tc.minItems) {
        assertGreater(
          result.stats.itemsExtracted,
          tc.minItems - 1,
          `Expected at least ${tc.minItems} items, got ${result.stats.itemsExtracted}`,
        );
      }

      // 4. Verify output content
      for (const frag of tc.outputContains ?? []) {
        assert(
          result.output.includes(frag),
          `Transpiled output should contain "${frag}"`,
        );
      }
      for (const frag of tc.outputAbsent ?? []) {
        assertEquals(
          result.output.includes(frag), false,
          `Transpiled output should NOT contain "${frag}"`,
        );
      }

      // 5. Verify diagnostics
      for (const check of tc.expectedDiagnostics ?? []) {
        assert(
          result.diagnostics.some((d) =>
            d.severity === check.severity &&
            (!check.messageContains || d.message.includes(check.messageContains)),
          ),
          `Expected diagnostic: ${check.severity}${check.messageContains ? ` containing "${check.messageContains}"` : ""}`,
        );
      }

      // 6. Extended validation
      if (tc.validate) tc.validate(result);
    });
  }

  // ── Suite summary ──
  _suiteRecords.push(record);
  const active = record.totalCases - record.skippedCases;

  Deno.test(`${label} suite — ${active} cases`, async (t) => {
    await t.step(
      `${record.successCases} success, ${record.errorCases} error-path`,
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

/** Get all transpile suite records. */
export function getTranspileSuiteRecords(): readonly TranspileSuiteRecord[] {
  return _suiteRecords;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Data-driven transpile test runner — table-driven verification of format
// conversion. The transpiler produces structural skeletons: correct 4-block
// layout, section headers, identity, function stubs. These tests verify
// the architecture carries across format boundaries.
//
// "A threefold cord is not quickly broken." — Ecclesiastes 4:12
// ============================================================================
