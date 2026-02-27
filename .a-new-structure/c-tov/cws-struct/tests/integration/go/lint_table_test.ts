// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/go/lint_table_test.ts
// key:     B-tov-cws-struct-tests-integration-go-lint-table
// title:   Go Integration Tests — Cross-Concern Lint
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Integration-level Go lint tests. Full pipeline, health scoring,
//          transform, create-verify, cross-layer interactions.
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

const go = lazyFormat("go");

function fixture(name: string): string {
  return new URL(`../../fixtures/${name}`, import.meta.url).pathname;
}

// ---------------------------------------------------------------------------
// 1. Structure: valid file health
// ---------------------------------------------------------------------------

Deno.test("integration/go: valid-library lints clean", async () => {
  const path = fixture("go/structure/valid-library.go");
  const results = await go.lint(path);
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${errs.map(e => e.rule).join(", ")}`);
});

Deno.test("integration/go: valid-library has positive health", async () => {
  const path = fixture("go/structure/valid-library.go");
  const results = await go.lint(path);
  const health = await go.computeHealth!(path, results);

  assertExists(health, "Health should be computed");
  assert(health.total > 0, `Health should be positive, got ${health.total}`);
  assert(
    ["sound", "whole", "perfect"].includes(health.level),
    `Level should be sound+, got ${health.level}`,
  );
});

Deno.test("integration/go: valid-executable lints clean", async () => {
  const path = fixture("go/structure/valid-executable.go");
  const results = await go.lint(path);
  const errs = errors(results);
  assertEquals(errs.length, 0, `Executable should be clean: ${errs.map(e => e.rule).join(", ")}`);
});

Deno.test("integration/go: error codes resolve", async () => {
  const path = fixture("go/structure/valid-library.go");
  const results = await go.lint(path);
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
// 2. Structure: broken files have expected health degradation
// ---------------------------------------------------------------------------

Deno.test("integration/go: missing-block has structural errors", async () => {
  const path = fixture("go/structure/missing-block.go");
  const results = await go.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Missing block should have errors");
});

Deno.test("integration/go: missing-block scores lower than valid", async () => {
  const validPath = fixture("go/structure/valid-library.go");
  const validResults = await go.lint(validPath);
  const validHealth = await go.computeHealth!(validPath, validResults);

  const brokenPath = fixture("go/structure/missing-block.go");
  const brokenResults = await go.lint(brokenPath);
  const brokenHealth = await go.computeHealth!(brokenPath, brokenResults);

  assert(
    brokenHealth.total < validHealth.total,
    `Missing-block (${brokenHealth.total}) should score lower than valid (${validHealth.total})`,
  );
});

Deno.test("integration/go: wrong-block-order produces errors", async () => {
  const path = fixture("go/structure/wrong-block-order.go");
  const results = await go.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Wrong block order should have issues");
});

// ---------------------------------------------------------------------------
// 3. Transform integration
// ---------------------------------------------------------------------------

Deno.test("integration/go: transform dry-run on valid file", async () => {
  const path = fixture("go/structure/valid-library.go");
  const results = await go.transform!(path, {
    dryRun: true,
    extensions: false,
    force: false,
    steps: false,
  });

  assert(Array.isArray(results), "Transform should return results array");
});

Deno.test("integration/go: transform wet-run preserves validity", async () => {
  const srcPath = fixture("go/structure/valid-library.go");
  const tmpDir = await Deno.makeTempDir();
  const tmpFile = `${tmpDir}/test-transform.go`;

  try {
    await Deno.copyFile(srcPath, tmpFile);

    await go.transform!(tmpFile, {
      dryRun: false,
      extensions: false,
      force: false,
      steps: false,
    });

    const afterResults = await go.lint(tmpFile);
    const afterErrs = errors(afterResults);
    assertEquals(afterErrs.length, 0, "Transform should preserve validity");
  } finally {
    await Deno.remove(tmpDir, { recursive: true });
  }
});

// ---------------------------------------------------------------------------
// 4. Content placement
// ---------------------------------------------------------------------------

Deno.test("integration/go: correct placement is clean", async () => {
  const path = fixture("go/structure/content-placement-correct.go");
  const results = await go.lint(path);
  const errs = errors(results);
  assertEquals(errs.length, 0, `Correct placement should be clean: ${errs.map(e => e.rule).join(", ")}`);
});

Deno.test("integration/go: wrong placement has errors", async () => {
  const path = fixture("go/structure/content-placement-wrong.go");
  const results = await go.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Wrong placement should have issues");
});

// ---------------------------------------------------------------------------
// 5. Summarize integration
// ---------------------------------------------------------------------------

Deno.test("integration/go: summarize counts match results", async () => {
  const path = fixture("go/structure/valid-library.go");
  const results = await go.lint(path);
  const health = await go.computeHealth!(path, results);
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
// Go integration tests — cross-concern verification. Structure, health,
// transform, content placement, and summarize. Full pipeline per test.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
