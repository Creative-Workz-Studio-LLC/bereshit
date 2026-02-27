// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/json/lint_table_test.ts
// key:     B-tov-cws-struct-tests-integration-json-lint-table
// title:   JSON Integration Tests — Cross-Concern Lint
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Integration-level JSON lint tests. Verifies the JSON handler
//          detects structural, metadata, content, and pragma issues.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import { lazyFormat, errors, warnings } from "@test/helpers.ts";

// ============================================================================
// BODY
// ============================================================================

const json = lazyFormat("json");

function fixture(name: string): string {
  return new URL(`../../fixtures/${name}`, import.meta.url).pathname;
}

// ---------------------------------------------------------------------------
// 1. Structure: broken files detected
// ---------------------------------------------------------------------------

Deno.test("integration/json: not-3block has structural errors", async () => {
  const path = fixture("json/structure/not-3block.jsonc");
  const results = await json.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Not-3block should have errors");
});

Deno.test("integration/json: missing-boundaries has errors", async () => {
  const path = fixture("json/structure/missing-boundaries.jsonc");
  const results = await json.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Missing boundaries should have errors");
});

// ---------------------------------------------------------------------------
// 2. Metadata: missing fields detected
// ---------------------------------------------------------------------------

Deno.test("integration/json: missing-m1-m2-fields detected", async () => {
  const path = fixture("json/metadata/missing-m1-m2-fields.jsonc");
  const results = await json.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Missing M1/M2 fields should have issues");
});

// ---------------------------------------------------------------------------
// 3. Content: missing separators detected
// ---------------------------------------------------------------------------

Deno.test("integration/json: missing-sep-tags detected", async () => {
  const path = fixture("json/content/missing-sep-tags.jsonc");
  const results = await json.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Missing separator tags should have issues");
});

// ---------------------------------------------------------------------------
// 4. Pragma: missing required pragma
// ---------------------------------------------------------------------------

Deno.test("integration/json: missing-required-pragma detected", async () => {
  const path = fixture("json/pragma/missing-required-pragma.jsonc");
  const results = await json.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Missing required pragma should have errors");
});

// ---------------------------------------------------------------------------
// 5. System: bad parse
// ---------------------------------------------------------------------------

Deno.test("integration/json: bad-parse produces parse error", async () => {
  const path = fixture("json/system/bad-parse.jsonc");
  const results = await json.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Bad parse should produce errors");
  assert(
    errs.some(e => e.rule.includes("parse")),
    "Should have parse-related error",
  );
});

// ============================================================================
// CLOSING
// ============================================================================
//
// JSON integration tests — structural, metadata, content, pragma, and parse
// error detection. Each fixture exercises the full JSON lint pipeline.
//
// "Try the spirits whether they are of God." — 1 John 4:1
// ============================================================================
