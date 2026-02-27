// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/_create_runner.ts
// key:     B-tov-cws-struct-tests-create-runner
// title:   Shared Data-Driven Create Test Runner
// type:    Code (Test Infrastructure)
// version: a-01.00
// created: 2026-02-24
// updated: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Table-driven test generation for file creation operations.
//          Each test case generates a file from schema, then verifies:
//            1. File is created without error
//            2. Generated file lints clean (0 errors)
//            3. Required blocks are present
//            4. Content checks pass (expected strings in output)
//
//          The proof: if the generated file lints with 0 errors,
//          the schema IS the complete specification.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import type { LintResult } from "@lib/foundation/mod.ts";
import { lazyFormat, witnessFor } from "@test/helpers.ts";
import { generateFileText, resolveErrorCodes } from "@lib/engine/mod.ts";
import type { FillContext } from "@lib/engine/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Types — The create test specification
// ---------------------------------------------------------------------------

/**
 * A single create test case.
 *
 * Each case generates a file from schema and verifies the output.
 * The key assertion: generated file lints with 0 errors.
 */
export interface CreateTestCase {
  /** Subtype to create (e.g., "library", "executable", "module"). */
  subtype: string;

  /** Human-readable description. */
  desc: string;

  /** Key override. If omitted, derived from subtype. */
  key?: string;

  /** Title override. If omitted, derived from subtype. */
  title?: string;

  /** Purpose override. */
  purpose?: string;

  /** Package/crate name override. */
  packageOrCrate?: string;

  /**
   * Expected lint errors after creation. Default: 0.
   * Use > 0 only when testing known-incomplete schemas.
   */
  expectedErrors?: number;

  /**
   * Strings that MUST appear in the generated file content.
   * Verifies structural elements are present.
   */
  contentContains?: string[];

  /**
   * Strings that must NOT appear in the generated file.
   * Catches template placeholders that should have been filled.
   */
  contentAbsent?: string[];

  /**
   * Block names that must be present (e.g., "METADATA", "SETUP", "BODY", "CLOSING").
   * Verified by checking for block separator comments.
   */
  expectedBlocks?: string[];

  /**
   * CWS witness code for bracket-tag test names.
   */
  witnessCode?: string;

  /** Skip this test case (with reason). */
  skip?: string;

  /** Extended validation on the generated text + lint results. */
  validate?: (text: string, lintResults: LintResult[]) => void;
}

/**
 * A format's create test specification.
 */
export interface CreateFormatSpec {
  /** Format handler name (e.g., "go", "rust"). */
  format: string;

  /** Optional label for test names. */
  label?: string;

  /** Create test cases. */
  cases: CreateTestCase[];
}

// ---------------------------------------------------------------------------
// Suite tracking
// ---------------------------------------------------------------------------

interface CreateSuiteRecord {
  format: string;
  label: string;
  totalCases: number;
  skippedCases: number;
  cleanCases: number;
}

const _suiteRecords: CreateSuiteRecord[] = [];

// ---------------------------------------------------------------------------
// Runner — Generate tests from create specifications
// ---------------------------------------------------------------------------

/**
 * Generate Deno.test() calls from a create format specification.
 *
 * For each case:
 *   1. Generate file text from schema via generateFileText()
 *   2. Write to temp file
 *   3. Lint the generated file
 *   4. Verify 0 errors (or expectedErrors)
 *   5. Verify content checks
 *   6. Verify block presence
 *   7. Clean up temp file
 */
export function runCreateSuite(spec: CreateFormatSpec): CreateSuiteRecord {
  const handler = lazyFormat(spec.format);
  const label = spec.label ?? `${spec.format}/create`;

  // Determine file extension from format
  const extMap: Record<string, string> = {
    go: ".go",
    rust: ".rs",
    toml: ".toml",
    json: ".json",
    makefile: "",
    dotfiles: "",
  };
  const ext = extMap[spec.format] ?? "";

  const record: CreateSuiteRecord = {
    format: spec.format,
    label,
    totalCases: spec.cases.length,
    skippedCases: 0,
    cleanCases: 0,
  };

  for (const tc of spec.cases) {
    const prefix = tc.witnessCode ? `[${tc.witnessCode}] ` : "";
    const testName = `${prefix}${label}/${tc.subtype}: ${tc.desc}`;

    if (tc.skip) {
      record.skippedCases++;
      Deno.test({ name: testName, ignore: true, fn: () => {} });
      continue;
    }

    const expectedErrors = tc.expectedErrors ?? 0;
    if (expectedErrors === 0) record.cleanCases++;

    Deno.test(testName, async () => {
      if (tc.witnessCode) witnessFor(tc.witnessCode, testName);

      // Build FillContext
      const filename = `test-create-${tc.subtype}${ext}`;
      const ctx: FillContext = {
        format: spec.format as "go" | "rust",
        subtype: tc.subtype,
        key: tc.key ?? `B-test-create-${tc.subtype}`,
        title: tc.title ?? `Test Create ${tc.subtype}`,
        purpose: tc.purpose ?? `Test-generated ${tc.subtype} file`,
        filename,
        packageOrCrate: tc.packageOrCrate ?? (tc.subtype === "executable" && spec.format === "go" ? "main" : "testpkg"),
        from: `test/${filename}`,
      };

      // Generate file text
      const text = await generateFileText(ctx);
      assert(text.length > 0, "Generated file should not be empty");

      // Write to temp file and lint
      const tmp = await Deno.makeTempFile({ suffix: ext || ".txt" });
      try {
        await Deno.writeTextFile(tmp, text);

        // Lint the generated file
        const results = await handler.lint(tmp);
        resolveErrorCodes(results);
        const errors = results.filter((r) => r.severity === "error");

        assertEquals(
          errors.length,
          expectedErrors,
          expectedErrors === 0
            ? `Generated ${tc.subtype} should lint clean. Errors:\n${errors.map(e => `  ${e.rule}: ${e.message}`).join("\n")}`
            : `Expected ${expectedErrors} errors, got ${errors.length}`,
        );

        // Content checks — must appear
        for (const frag of tc.contentContains ?? []) {
          assert(
            text.includes(frag),
            `Generated file should contain "${frag}"`,
          );
        }

        // Content checks — must NOT appear
        for (const frag of tc.contentAbsent ?? []) {
          assertEquals(
            text.includes(frag), false,
            `Generated file should NOT contain "${frag}" (unfilled placeholder?)`,
          );
        }

        // Block presence checks
        for (const block of tc.expectedBlocks ?? []) {
          const upper = block.toUpperCase();
          assert(
            text.includes(upper),
            `Generated file should contain ${upper} block`,
          );
        }

        // Extended validation
        if (tc.validate) {
          tc.validate(text, results);
        }
      } finally {
        await Deno.remove(tmp);
      }
    });
  }

  // ── Suite summary ──
  _suiteRecords.push(record);
  const active = record.totalCases - record.skippedCases;

  Deno.test(`${label} suite — ${active} cases`, async (t) => {
    await t.step(
      `${record.cleanCases} lint-clean, ${active - record.cleanCases} with expected errors`,
      () => {},
    );

    if (record.skippedCases > 0) {
      await t.step(`${record.skippedCases} skipped`, () => {});
    }
  });

  return record;
}

/** Get all create suite records. */
export function getCreateSuiteRecords(): readonly CreateSuiteRecord[] {
  return _suiteRecords;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Data-driven create test runner — table-driven generation verification.
// The key proof: schema-generated files lint clean.
// If the generated file has errors, the schema is incomplete.
//
// "Write the vision, and make it plain upon tables." — Habakkuk 2:2
// ============================================================================
