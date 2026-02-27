// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/omni/lint_table_test.ts
// key:     B-tov-cws-struct-tests-integration-omni-lint-table
// title:   Omni Integration Tests — Cross-Concern Lint
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Integration-level Omni lint tests. Verifies the Omni handler
//          detects structural and content issues in .omni files.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assert } from "jsr:@std/assert";
import { lazyFormat, errors, warnings } from "@test/helpers.ts";

// ============================================================================
// BODY
// ============================================================================

const omni = lazyFormat("omni");

function fixture(name: string): string {
  return new URL(`../../fixtures/${name}`, import.meta.url).pathname;
}

// ---------------------------------------------------------------------------
// 1. Structure: shebang issues
// ---------------------------------------------------------------------------

Deno.test("integration/omni: no-shebang has errors", async () => {
  const path = fixture("omni/structure/no-shebang.omni");
  const results = await omni.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "No shebang should have errors");
});

Deno.test("integration/omni: bad-shebang has errors", async () => {
  const path = fixture("omni/structure/bad-shebang.omni");
  const results = await omni.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Bad shebang should have issues");
});

// ---------------------------------------------------------------------------
// 2. Structure: empty/blank files
// ---------------------------------------------------------------------------

Deno.test("integration/omni: empty file has errors", async () => {
  const path = fixture("omni/structure/empty.omni");
  const results = await omni.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Empty file should have errors");
});

Deno.test("integration/omni: blank file has errors", async () => {
  const path = fixture("omni/structure/blank.omni");
  const results = await omni.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Blank file should have errors");
});

// ---------------------------------------------------------------------------
// 3. Content: containment issues
// ---------------------------------------------------------------------------

Deno.test("integration/omni: no-containment has issues", async () => {
  const path = fixture("omni/content/no-containment.omni");
  const results = await omni.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "No containment should have issues");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Omni integration tests — shebang, empty/blank, and containment validation.
// Each fixture exercises the full Omni lint pipeline.
//
// "In the beginning was the Word." — John 1:1
// ============================================================================
