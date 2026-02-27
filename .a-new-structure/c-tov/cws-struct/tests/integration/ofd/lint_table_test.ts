// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/ofd/lint_table_test.ts
// key:     B-tov-cws-struct-tests-integration-ofd-lint-table
// title:   OFD Integration Tests — Cross-Concern Lint
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Integration-level OFD lint tests. Verifies the OFD handler
//          detects structure, identity, children, dependencies, grounding,
//          and system-level issues across .ofd fixtures.
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

const ofd = lazyFormat("ofd");

function fixture(name: string): string {
  return new URL(`../../fixtures/${name}`, import.meta.url).pathname;
}

// ---------------------------------------------------------------------------
// 1. Structure: section-level issues
// ---------------------------------------------------------------------------

Deno.test("integration/ofd: bad-sections has errors", async () => {
  const path = fixture("ofd/structure/bad-sections.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Bad sections should have errors");
});

Deno.test("integration/ofd: no-children has errors", async () => {
  const path = fixture("ofd/structure/no-children.ofd");
  const results = await ofd.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "No children should have issues");
});

Deno.test("integration/ofd: missing-dependencies has errors", async () => {
  const path = fixture("ofd/structure/missing-dependencies.ofd");
  const results = await ofd.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Missing dependencies should have issues");
});

Deno.test("integration/ofd: missing-identity-section has errors", async () => {
  const path = fixture("ofd/structure/missing-identity-section.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Missing identity section should have errors");
});

// ---------------------------------------------------------------------------
// 2. Identity: field-level issues
// ---------------------------------------------------------------------------

Deno.test("integration/ofd: missing-fields has errors", async () => {
  const path = fixture("ofd/identity/missing-fields.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Missing identity fields should have errors");
});

Deno.test("integration/ofd: wrong-types has errors", async () => {
  const path = fixture("ofd/identity/wrong-types.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Wrong identity types should have errors");
});

Deno.test("integration/ofd: bad-source-ext has errors", async () => {
  const path = fixture("ofd/identity/bad-source-ext.ofd");
  const results = await ofd.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Bad source extension should have issues");
});

Deno.test("integration/ofd: missing-required-fields has errors", async () => {
  const path = fixture("ofd/identity/missing-required-fields.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Missing required identity fields should have errors");
});

Deno.test("integration/ofd: wrong-type-fields has errors", async () => {
  const path = fixture("ofd/identity/wrong-type-fields.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Wrong type fields should have errors");
});

Deno.test("integration/ofd: missing-state-required has errors", async () => {
  const path = fixture("ofd/identity/missing-state-required.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Missing state required fields should have errors");
});

Deno.test("integration/ofd: missing-attribution-required has errors", async () => {
  const path = fixture("ofd/identity/missing-attribution-required.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Missing attribution required should have errors");
});

// ---------------------------------------------------------------------------
// 3. Children: bad children
// ---------------------------------------------------------------------------

Deno.test("integration/ofd: bad-children has errors", async () => {
  const path = fixture("ofd/children/bad-children.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Bad children should have errors");
});

// ---------------------------------------------------------------------------
// 4. Dependencies: wrong types
// ---------------------------------------------------------------------------

Deno.test("integration/ofd: wrong dependency types has errors", async () => {
  const path = fixture("ofd/dependencies/wrong-types.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Wrong dependency types should have errors");
});

// ---------------------------------------------------------------------------
// 5. Grounding: bad grounding
// ---------------------------------------------------------------------------

Deno.test("integration/ofd: bad-grounding has errors", async () => {
  const path = fixture("ofd/grounding/bad-grounding.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Bad grounding should have errors");
});

Deno.test("integration/ofd: scriptures-wrong-type has errors", async () => {
  const path = fixture("ofd/grounding/scriptures-wrong-type.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Wrong scripture type should have errors");
});

// ---------------------------------------------------------------------------
// 6. System: empty/bad files
// ---------------------------------------------------------------------------

Deno.test("integration/ofd: empty file has errors", async () => {
  const path = fixture("ofd/system/empty.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Empty file should have errors");
});

Deno.test("integration/ofd: bad-json has errors", async () => {
  const path = fixture("ofd/system/bad-json.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Bad JSON should have errors");
});

Deno.test("integration/ofd: array-root has errors", async () => {
  const path = fixture("ofd/system/array-root.ofd");
  const results = await ofd.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Array root should have errors");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// OFD integration tests — 18 fixtures across structure, identity, children,
// dependencies, grounding, and system concerns. The most fixture-rich format
// handler, fully exercised.
//
// "The LORD trieth the righteous." — Psalm 11:5
// ============================================================================
