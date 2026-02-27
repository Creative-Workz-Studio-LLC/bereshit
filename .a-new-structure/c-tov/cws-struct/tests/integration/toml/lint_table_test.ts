// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/toml/lint_table_test.ts
// key:     B-tov-cws-struct-tests-integration-toml-lint-table
// title:   TOML Integration Tests — Cross-Concern Lint
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Integration-level TOML lint tests. Unlike R[n] layer tests
//          (which test one error code against one fixture), these test
//          broader concerns: full pipeline, transform, health scoring,
//          cross-layer interactions.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertExists } from "jsr:@std/assert";
import { lazyFormat, errors, warnings, infos } from "@test/helpers.ts";
import { summarize, trueToLevel } from "@lib/foundation/mod.ts";
import { resolveErrorCodes } from "@lib/engine/mod.ts";

// ============================================================================
// BODY
// ============================================================================

const toml = lazyFormat("toml");

function fixture(name: string): string {
  return new URL(`../../fixtures/${name}`, import.meta.url).pathname;
}

// ---------------------------------------------------------------------------
// 1. Structure: valid file health
// ---------------------------------------------------------------------------

Deno.test("integration/toml: valid-complete lints clean", async () => {
  const path = fixture("toml/structure/valid-complete.toml");
  const results = await toml.lint(path);
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${errs.map(e => e.rule).join(", ")}`);
});

Deno.test("integration/toml: valid-complete has positive health", async () => {
  const path = fixture("toml/structure/valid-complete.toml");
  const results = await toml.lint(path);
  const health = await toml.computeHealth!(path, results);

  assertExists(health, "Health should be computed");
  assert(health.total > 0, `Health should be positive, got ${health.total}`);
  assert(
    ["sound", "whole", "perfect"].includes(health.level),
    `Level should be sound+, got ${health.level}`,
  );
});

Deno.test("integration/toml: valid-complete error codes resolve", async () => {
  const path = fixture("toml/structure/valid-complete.toml");
  const results = await toml.lint(path);
  resolveErrorCodes(results);

  // All results should have codes resolved
  for (const r of results) {
    if (r.errorCode) {
      assert(
        r.errorCode.startsWith("CWS-"),
        `Error code should start with CWS-, got ${r.errorCode}`,
      );
    }
  }
});

// ---------------------------------------------------------------------------
// 2. Structure: broken files have expected health degradation
// ---------------------------------------------------------------------------

Deno.test("integration/toml: missing-metadata scores lower than valid", async () => {
  const validPath = fixture("toml/structure/valid-complete.toml");
  const validResults = await toml.lint(validPath);
  const validHealth = await toml.computeHealth!(validPath, validResults);

  const brokenPath = fixture("toml/structure/missing-metadata.toml");
  const brokenResults = await toml.lint(brokenPath);
  const brokenHealth = await toml.computeHealth!(brokenPath, brokenResults);

  assert(
    brokenHealth.total < validHealth.total,
    `Missing-metadata (${brokenHealth.total}) should score lower than valid (${validHealth.total})`,
  );
});

Deno.test("integration/toml: empty file produces structural errors", async () => {
  const path = fixture("toml/structure/empty.toml");
  const results = await toml.lint(path);
  const errs = errors(results);

  assert(errs.length > 0, "Empty file should have errors");
  assert(
    errs.some(e => e.rule.includes("block") || e.rule.includes("structure") || e.rule.includes("metadata")),
    "Should have structural errors",
  );
});

// ---------------------------------------------------------------------------
// 3. Transform integration
// ---------------------------------------------------------------------------

Deno.test("integration/toml: transform dry-run on valid file", async () => {
  const path = fixture("toml/structure/valid-complete.toml");
  const results = await toml.transform!(path, {
    dryRun: true,
    extensions: false,
    force: false,
    steps: false,
  });

  assert(Array.isArray(results), "Transform should return results array");
});

Deno.test("integration/toml: transform wet-run preserves validity", async () => {
  const srcPath = fixture("toml/structure/valid-complete.toml");
  const tmpDir = await Deno.makeTempDir();
  const tmpFile = `${tmpDir}/test-transform.toml`;

  try {
    await Deno.copyFile(srcPath, tmpFile);

    // Transform
    await toml.transform!(tmpFile, {
      dryRun: false,
      extensions: false,
      force: false,
      steps: false,
    });

    // Re-lint
    const afterResults = await toml.lint(tmpFile);
    const afterErrs = errors(afterResults);
    assertEquals(afterErrs.length, 0, "Transform should preserve validity");
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

// ---------------------------------------------------------------------------
// 4. Cargo integration
// ---------------------------------------------------------------------------

Deno.test("integration/toml: valid Cargo.toml library lints clean", async () => {
  const path = fixture("toml/cargo/valid-library.toml");
  const results = await toml.lint(path);
  const errs = errors(results);
  assertEquals(errs.length, 0, `Cargo library should lint clean: ${errs.map(e => e.rule).join(", ")}`);
});

Deno.test("integration/toml: Cargo library has positive health", async () => {
  const path = fixture("toml/cargo/valid-library.toml");
  const results = await toml.lint(path);
  const health = await toml.computeHealth!(path, results);

  assertExists(health);
  assert(health.total > 0, `Cargo health should be positive, got ${health.total}`);
});

// ---------------------------------------------------------------------------
// 5. Metadata validation
// ---------------------------------------------------------------------------

Deno.test("integration/toml: missing identity has identity errors", async () => {
  const path = fixture("toml/metadata/missing-required-identity.toml");
  const results = await toml.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Missing identity should have errors");
});

Deno.test("integration/toml: bad keys detected", async () => {
  const path = fixture("toml/metadata/bad-keys.toml");
  const results = await toml.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Bad keys should have errors");
});

// ---------------------------------------------------------------------------
// 6. Content validation
// ---------------------------------------------------------------------------

Deno.test("integration/toml: content value problems detected", async () => {
  const path = fixture("toml/content/value-problems.toml");
  const results = await toml.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Value problems should have errors");
});

// ---------------------------------------------------------------------------
// 7. Closing validation
// ---------------------------------------------------------------------------

Deno.test("integration/toml: wrong X-zone order detected", async () => {
  const path = fixture("toml/closing/wrong-x-order.toml");
  const results = await toml.lint(path);
  // Should have warnings or errors about order
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Wrong order should have issues");
});

Deno.test("integration/toml: missing required closing sections", async () => {
  const path = fixture("toml/closing/missing-required-sections.toml");
  const results = await toml.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Missing required sections should have issues");
});

// ---------------------------------------------------------------------------
// 8. Summarize integration
// ---------------------------------------------------------------------------

Deno.test("integration/toml: summarize counts match results", async () => {
  const path = fixture("toml/structure/valid-complete.toml");
  const results = await toml.lint(path);
  const health = await toml.computeHealth!(path, results);
  const summary = summarize(path, results, health);

  let manualErrors = 0, manualWarns = 0, manualInfos = 0;
  for (const r of results) {
    if (r.severity === "error") manualErrors++;
    else if (r.severity === "warn") manualWarns++;
    else if (r.severity === "info") manualInfos++;
  }

  assertEquals(summary.errors, manualErrors);
  assertEquals(summary.warnings, manualWarns);
  assertEquals(summary.infos, manualInfos);
  assertExists(summary.health);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// TOML integration tests — cross-concern verification. Structure, health,
// transform, Cargo, metadata, content, closing, and summarize. Each test
// exercises the full pipeline, not a single error code.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
