// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/rust/lint_table_test.ts
// key:     B-tov-cws-struct-tests-integration-rust-lint-table
// title:   Rust Integration Tests — Cross-Concern Lint
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Integration-level Rust lint tests. Full pipeline, health scoring,
//          transform, cross-layer interactions.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertExists } from "jsr:@std/assert";
import { lazyFormat, errors, warnings } from "@test/helpers.ts";
import { summarize } from "@lib/foundation/mod.ts";
import { resolveErrorCodes } from "@lib/engine/mod.ts";

// ============================================================================
// BODY
// ============================================================================

const rust = lazyFormat("rust");

function fixture(name: string): string {
  return new URL(`../../fixtures/${name}`, import.meta.url).pathname;
}

// ---------------------------------------------------------------------------
// 1. Structure: valid file health
// ---------------------------------------------------------------------------

Deno.test("integration/rust: valid-library lints clean", async () => {
  const path = fixture("rust/structure/valid-library.rs");
  const results = await rust.lint(path);
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${errs.map(e => e.rule).join(", ")}`);
});

Deno.test("integration/rust: valid-library has positive health", async () => {
  const path = fixture("rust/structure/valid-library.rs");
  const results = await rust.lint(path);
  const health = await rust.computeHealth!(path, results);

  assertExists(health, "Health should be computed");
  assert(health.total > 0, `Health should be positive, got ${health.total}`);
  assert(
    ["sound", "whole", "perfect"].includes(health.level),
    `Level should be sound+, got ${health.level}`,
  );
});

Deno.test("integration/rust: valid-module lints clean", async () => {
  const path = fixture("rust/structure/valid-module.rs");
  const results = await rust.lint(path);
  const errs = errors(results);
  assertEquals(errs.length, 0, `Module should be clean: ${errs.map(e => e.rule).join(", ")}`);
});

Deno.test("integration/rust: valid-executable lints clean", async () => {
  const path = fixture("rust/structure/valid-executable.rs");
  const results = await rust.lint(path);
  const errs = errors(results);
  assertEquals(errs.length, 0, `Executable should be clean: ${errs.map(e => e.rule).join(", ")}`);
});

Deno.test("integration/rust: error codes resolve", async () => {
  const path = fixture("rust/structure/valid-library.rs");
  const results = await rust.lint(path);
  resolveErrorCodes(results);

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
// 2. Structure: broken files
// ---------------------------------------------------------------------------

Deno.test("integration/rust: missing-block has structural errors", async () => {
  const path = fixture("rust/structure/missing-block.rs");
  const results = await rust.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Missing block should have errors");
});

Deno.test("integration/rust: missing-block scores lower than valid", async () => {
  const validPath = fixture("rust/structure/valid-library.rs");
  const validResults = await rust.lint(validPath);
  const validHealth = await rust.computeHealth!(validPath, validResults);

  const brokenPath = fixture("rust/structure/missing-block.rs");
  const brokenResults = await rust.lint(brokenPath);
  const brokenHealth = await rust.computeHealth!(brokenPath, brokenResults);

  assert(
    brokenHealth.total < validHealth.total,
    `Missing-block (${brokenHealth.total}) should score lower than valid (${validHealth.total})`,
  );
});

Deno.test("integration/rust: wrong-block-order produces errors", async () => {
  const path = fixture("rust/structure/wrong-block-order.rs");
  const results = await rust.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Wrong block order should have issues");
});

// ---------------------------------------------------------------------------
// 3. Transform integration
// ---------------------------------------------------------------------------

Deno.test("integration/rust: transform dry-run on valid file", async () => {
  const path = fixture("rust/structure/valid-library.rs");
  const results = await rust.transform!(path, {
    dryRun: true,
    extensions: false,
    force: false,
    steps: false,
  });

  assert(Array.isArray(results), "Transform should return results array");
});

Deno.test("integration/rust: transform wet-run preserves validity", async () => {
  const srcPath = fixture("rust/structure/valid-library.rs");
  const tmpDir = await Deno.makeTempDir();
  const tmpFile = `${tmpDir}/test-transform.rs`;

  try {
    await Deno.copyFile(srcPath, tmpFile);

    await rust.transform!(tmpFile, {
      dryRun: false,
      extensions: false,
      force: false,
      steps: false,
    });

    const afterResults = await rust.lint(tmpFile);
    const afterErrs = errors(afterResults);
    assertEquals(afterErrs.length, 0, "Transform should preserve validity");
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

// ---------------------------------------------------------------------------
// 4. Content placement
// ---------------------------------------------------------------------------

Deno.test("integration/rust: correct placement is clean", async () => {
  const path = fixture("rust/structure/content-placement-correct.rs");
  const results = await rust.lint(path);
  const errs = errors(results);
  assertEquals(errs.length, 0, `Correct placement should be clean: ${errs.map(e => e.rule).join(", ")}`);
});

Deno.test("integration/rust: wrong placement has errors", async () => {
  const path = fixture("rust/structure/content-placement-wrong.rs");
  const results = await rust.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Wrong placement should have issues");
});

// ---------------------------------------------------------------------------
// 5. Large file handling
// ---------------------------------------------------------------------------

Deno.test("integration/rust: large file processes without error", async () => {
  const path = fixture("rust/structure/scaling-large.rs");
  const results = await rust.lint(path);
  // Large file should produce some results but not crash
  assert(Array.isArray(results), "Large file should return results array");
});

// ---------------------------------------------------------------------------
// 6. Summarize integration
// ---------------------------------------------------------------------------

Deno.test("integration/rust: summarize counts match results", async () => {
  const path = fixture("rust/structure/valid-library.rs");
  const results = await rust.lint(path);
  const health = await rust.computeHealth!(path, results);
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
// Rust integration tests — cross-concern verification. Structure, health,
// transform, content placement, large file handling, and summarize.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
