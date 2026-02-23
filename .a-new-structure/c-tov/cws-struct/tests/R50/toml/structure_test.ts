// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R50/toml/structure_test.ts
// key:     B-tov-cws-struct-tests-R50-toml-structure
// title:   CWS Struct — TOML R50 File-Level Detection Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// source:  Extracted from tests/handlers/toml_test.ts
// purpose: R50 (widest) detection — file-level structure validation for TOML.
//          3-block presence, parsing, missing blocks, BOM handling.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  fixture, lazyFormat, errors, warnings, hasRule, hasMessage,
  hasErrorCode, errorCodeReport,
} from "../../helpers.ts";

// ============================================================================
// BODY
// ============================================================================

// Lazy handler — initializes on first property access, no setup test needed.
const toml = lazyFormat("toml");

// ---------------------------------------------------------------------------
// structure/ — 3-block structure, parsing, missing blocks
// ---------------------------------------------------------------------------

Deno.test("R50/structure/valid-complete: zero errors", async () => {
  const results = await toml.lint(fixture("toml/structure/valid-complete.toml"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("R50/structure/valid-complete: zero warnings", async () => {
  const results = await toml.lint(fixture("toml/structure/valid-complete.toml"));
  const warns = warnings(results);
  assertEquals(warns.length, 0, `Expected 0 warnings, got ${warns.length}: ${JSON.stringify(warns, null, 2)}`);
});

Deno.test("[CWS-R50-043] R50/structure/empty: produces errors for missing structure", async () => {
  const results = await toml.lint(fixture("toml/structure/empty.toml"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Empty file should produce at least one error");
  assert(
    hasMessage(results, "_metadata") || hasMessage(results, "metadata") || hasRule(results, "metadata"),
    "Should mention missing _metadata",
  );
  // Error code: CWS-R50-043 (metadata/exists — Layer 0)
  assert(
    hasErrorCode(results, "CWS-R50-043"),
    `Expected CWS-R50-043 (metadata exists), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R50-002] R50/structure/parse-error: reports parse error without crashing", async () => {
  const results = await toml.lint(fixture("toml/structure/parse-error.toml"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Invalid TOML should produce at least one error");
  assert(
    hasRule(results, "parse") || hasMessage(results, "parse") || hasMessage(results, "Parse") || hasMessage(results, "syntax"),
    "Error should mention parsing",
  );
  // Error code: CWS-R50-002 (parse/toml — Layer 0)
  assert(
    hasErrorCode(results, "CWS-R50-002"),
    `Expected CWS-R50-002 (TOML parse), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R50-043] R50/structure/missing-metadata: error with fix suggestion", async () => {
  const results = await toml.lint(fixture("toml/structure/missing-metadata.toml"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Missing _metadata should produce at least one error");
  assert(
    hasMessage(results, "_metadata") || hasMessage(results, "metadata"),
    "Error should mention _metadata",
  );
  const metadataErrors = errs.filter((r) =>
    r.message.includes("_metadata") || r.message.includes("metadata")
  );
  assert(metadataErrors.length > 0, "Should have metadata-related error");
  const hasFix = metadataErrors.some((r) => r.fix !== undefined);
  assert(hasFix, "Missing _metadata error should include a fix suggestion");
  // Error code: CWS-R50-043 (metadata/exists — Layer 0)
  assert(
    hasErrorCode(results, "CWS-R50-043"),
    `Expected CWS-R50-043 (metadata exists), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("R50/structure/bom-utf8: parses correctly with BOM stripping", async () => {
  const results = await toml.lint(fixture("toml/structure/bom-utf8.toml"));
  const parseErrors = errors(results).filter((r) => r.rule.includes("parse"));
  assertEquals(parseErrors.length, 0, "BOM file should parse without errors after BOM stripping");
});

Deno.test("[CWS-R50-044] R50/structure/missing-content: warns about missing _content table", async () => {
  const results = await toml.lint(fixture("toml/structure/missing-content.toml"));
  assert(
    hasMessage(results, "_content") || hasRule(results, "content"),
    "Should mention missing _content table",
  );
  // Error code: CWS-R50-044 (content/exists — Layer 0)
  assert(
    hasErrorCode(results, "CWS-R50-044"),
    `Expected CWS-R50-044 (content exists), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("R50/structure/missing-content: no parse errors", async () => {
  const results = await toml.lint(fixture("toml/structure/missing-content.toml"));
  const parseErrs = errors(results).filter((r) => r.rule.includes("parse"));
  assertEquals(parseErrs.length, 0, "File should parse without errors");
});

Deno.test("[CWS-R50-045] R50/structure/missing-closing: warns about missing _closing table", async () => {
  const results = await toml.lint(fixture("toml/structure/missing-closing.toml"));
  assert(
    hasMessage(results, "_closing") || hasRule(results, "closing"),
    "Should mention missing _closing table",
  );
  // Error code: CWS-R50-045 (closing/exists — Layer 0)
  assert(
    hasErrorCode(results, "CWS-R50-045"),
    `Expected CWS-R50-045 (closing exists), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("R50/structure/missing-closing: no parse errors", async () => {
  const results = await toml.lint(fixture("toml/structure/missing-closing.toml"));
  const parseErrs = errors(results).filter((r) => r.rule.includes("parse"));
  assertEquals(parseErrs.length, 0, "File should parse without errors");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R50 TOML tests — widest detection layer.
// File-level concerns: does the file parse? Are the 3 blocks present?
// Does BOM stripping work? Are missing blocks reported with fix suggestions?
//
// 10 tests. Same content as original, elevated to detection hierarchy.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
