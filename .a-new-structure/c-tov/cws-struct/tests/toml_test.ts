// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/toml_test.ts
// key:     B-tov-cws-struct-tests-toml
// title:   CWS Struct — TOML Linter Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-17
// authors: Nova Dawn (CPI-SI)
// purpose: Tests for the TOML 3-block format handler. Tests go through the
//          public FormatHandler.lint() interface — same path the CLI uses.
//
//          Fixture-driven: each .toml file in tests/fixtures/toml/ targets a
//          specific condition. Tests assert on result counts and rule names.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import { fixture, getFormat, errors, warnings, infos, byRule, hasRule, hasMessage } from "./helpers.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Lint via registry — the integration surface
// ---------------------------------------------------------------------------

const toml = getFormat("toml");

if (!toml) {
  throw new Error("TOML format handler not registered — check imports in helpers.ts");
}

// ---------------------------------------------------------------------------
// Happy path: valid complete TOML
// ---------------------------------------------------------------------------

Deno.test("valid-complete: zero errors", async () => {
  const results = await toml.lint(fixture("toml/valid-complete.toml"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("valid-complete: zero warnings", async () => {
  const results = await toml.lint(fixture("toml/valid-complete.toml"));
  const warns = warnings(results);
  assertEquals(warns.length, 0, `Expected 0 warnings, got ${warns.length}: ${JSON.stringify(warns, null, 2)}`);
});

// ---------------------------------------------------------------------------
// Empty file
// ---------------------------------------------------------------------------

Deno.test("empty: produces errors for missing structure", async () => {
  const results = await toml.lint(fixture("toml/empty.toml"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Empty file should produce at least one error");
  // Should mention missing _metadata
  assert(
    hasMessage(results, "_metadata") || hasMessage(results, "metadata") || hasRule(results, "metadata"),
    "Should mention missing _metadata",
  );
});

// ---------------------------------------------------------------------------
// Parse error
// ---------------------------------------------------------------------------

Deno.test("parse-error: reports parse error without crashing", async () => {
  const results = await toml.lint(fixture("toml/parse-error.toml"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Invalid TOML should produce at least one error");
  assert(
    hasRule(results, "parse") || hasMessage(results, "parse") || hasMessage(results, "Parse") || hasMessage(results, "syntax"),
    "Error should mention parsing",
  );
});

// ---------------------------------------------------------------------------
// Missing metadata
// ---------------------------------------------------------------------------

Deno.test("missing-metadata: error with fix suggestion", async () => {
  const results = await toml.lint(fixture("toml/missing-metadata.toml"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Missing _metadata should produce at least one error");
  assert(
    hasMessage(results, "_metadata") || hasMessage(results, "metadata"),
    "Error should mention _metadata",
  );
  // Should have a fix suggestion
  const metadataErrors = errs.filter((r) =>
    r.message.includes("_metadata") || r.message.includes("metadata")
  );
  assert(metadataErrors.length > 0, "Should have metadata-related error");
  // Fix suggestions are optional but expected for missing _metadata
  const hasFix = metadataErrors.some((r) => r.fix !== undefined);
  assert(hasFix, "Missing _metadata error should include a fix suggestion");
});

// ---------------------------------------------------------------------------
// BOM UTF-8
// ---------------------------------------------------------------------------

Deno.test("bom-utf8: parses correctly with BOM stripping", async () => {
  const results = await toml.lint(fixture("toml/bom-utf8.toml"));
  // BOM should be stripped before parsing — no parse errors
  const parseErrors = errors(results).filter((r) => r.rule.includes("parse"));
  assertEquals(parseErrors.length, 0, "BOM file should parse without errors after BOM stripping");
  // The file is intentionally minimal — it will have missing-section warnings
  // but should NOT have parse errors
});

// ---------------------------------------------------------------------------
// Bad contract
// ---------------------------------------------------------------------------

Deno.test("bad-contract: detects phantom section declaration", async () => {
  const results = await toml.lint(fixture("toml/bad-contract.toml"));
  // _contract.required_identity includes "I99_phantom" which doesn't exist as a section
  assert(
    hasMessage(results, "I99_phantom"),
    "Should warn about phantom section I99_phantom in required_identity",
  );
  // The warning should be about the phantom declaration
  const phantomWarns = warnings(results).filter((r) => r.message.includes("I99_phantom"));
  assertGreater(phantomWarns.length, 0, "Should have at least one phantom declaration warning");
});

// ---------------------------------------------------------------------------
// Schema loading: clearCache works
// ---------------------------------------------------------------------------

Deno.test("schema cache: clearCache allows re-loading", async () => {
  // Lint once (loads schema)
  const results1 = await toml.lint(fixture("toml/valid-complete.toml"));
  // Import clearCache and call it
  const { clearCache } = await import("../lib/schema-loader.ts");
  clearCache();
  // Lint again (should reload schema)
  const results2 = await toml.lint(fixture("toml/valid-complete.toml"));
  // Results should be equivalent
  assertEquals(errors(results1).length, errors(results2).length, "Results should be consistent after cache clear");
});

// ---------------------------------------------------------------------------
// Value-level validation: valid file has no value errors
// ---------------------------------------------------------------------------

Deno.test("valid-complete: zero value errors", async () => {
  const results = await toml.lint(fixture("toml/valid-complete.toml"));
  const valueResults = byRule(results, "values/");
  const valueErrors = valueResults.filter((r) => r.severity === "error");
  const valueWarns = valueResults.filter((r) => r.severity === "warn");
  assertEquals(
    valueErrors.length,
    0,
    `Expected 0 value errors, got ${valueErrors.length}: ${JSON.stringify(valueErrors, null, 2)}`,
  );
  assertEquals(
    valueWarns.length,
    0,
    `Expected 0 value warnings, got ${valueWarns.length}: ${JSON.stringify(valueWarns, null, 2)}`,
  );
});

// ---------------------------------------------------------------------------
// Value-level validation: multiple value problems
// ---------------------------------------------------------------------------

Deno.test("value-problems: detects bad version format", async () => {
  const results = await toml.lint(fixture("toml/value-problems.toml"));
  // I1_core.at = "version1.bad" → error
  assert(
    hasRule(results, "values/I1_core/at"),
    "Should flag invalid I1_core.at version format",
  );
  const atErrors = errors(results).filter((r) => r.rule.includes("values/I1_core/at"));
  assertGreater(atErrors.length, 0, "I1_core.at should produce an error for bad format");
});

Deno.test("value-problems: detects bad C1 version format", async () => {
  const results = await toml.lint(fixture("toml/value-problems.toml"));
  // C1_state.version = "x-99.99" → error
  assert(
    hasRule(results, "values/C1_state/version"),
    "Should flag invalid C1_state.version format",
  );
  const versionErrors = errors(results).filter((r) => r.rule.includes("values/C1_state/version"));
  assertGreater(versionErrors.length, 0, "C1_state.version should produce an error for bad format");
});

Deno.test("value-problems: detects missing B- prefix", async () => {
  const results = await toml.lint(fixture("toml/value-problems.toml"));
  // key = "no-prefix-key" → warn
  assert(
    hasRule(results, "values/I1_core/key"),
    "Should flag missing B- prefix on I1_core.key",
  );
});

Deno.test("value-problems: detects empty required field", async () => {
  const results = await toml.lint(fixture("toml/value-problems.toml"));
  // I3_instance.title = "" → warn
  const emptyWarns = warnings(results).filter((r) =>
    r.rule.includes("values/") && r.rule.includes("/empty")
  );
  assertGreater(emptyWarns.length, 0, "Should warn about empty required fields");
});

Deno.test("value-problems: detects filename mismatch", async () => {
  const results = await toml.lint(fixture("toml/value-problems.toml"));
  // I3_instance.file = "wrong-filename.toml" but file is "value-problems.toml"
  assert(
    hasRule(results, "values/I3_instance/file"),
    "Should flag filename mismatch",
  );
});

Deno.test("value-problems: detects bad date format", async () => {
  const results = await toml.lint(fixture("toml/value-problems.toml"));
  // C1_state.created = "not-a-date" → warn
  assert(
    hasRule(results, "values/C1_state/created"),
    "Should flag invalid date format",
  );
});

Deno.test("value-problems: detects unknown type", async () => {
  const results = await toml.lint(fixture("toml/value-problems.toml"));
  // I2_family.type = "spreadsheet" → info
  const typeInfos = infos(results).filter((r) => r.rule.includes("values/I2_family/type"));
  assertGreater(typeInfos.length, 0, "Should flag unknown I2_family.type");
});

Deno.test("value-problems: detects empty required array", async () => {
  const results = await toml.lint(fixture("toml/value-problems.toml"));
  // X1_policy.never = [] → warn
  const emptyArrayWarns = warnings(results).filter((r) =>
    r.message.includes("empty array")
  );
  assertGreater(emptyArrayWarns.length, 0, "Should warn about empty required arrays");
});

// ---------------------------------------------------------------------------
// Value-level validation: version inconsistency + date order
// ---------------------------------------------------------------------------

Deno.test("value-version-mismatch: detects at/version inconsistency", async () => {
  const results = await toml.lint(fixture("toml/value-version-mismatch.toml"));
  // I1_core.at = "a-01.00" vs C1_state.version = "b-02.50"
  assert(
    hasRule(results, "values/consistency/version"),
    "Should flag version inconsistency between I1_core.at and C1_state.version",
  );
});

Deno.test("value-version-mismatch: detects date order problem", async () => {
  const results = await toml.lint(fixture("toml/value-version-mismatch.toml"));
  // created = "2026-02-17" > updated = "2026-02-10"
  assert(
    hasRule(results, "values/C1_state/date-order"),
    "Should flag created date after updated date",
  );
});

// ---------------------------------------------------------------------------
// CLOSING X section ordering
// ---------------------------------------------------------------------------

Deno.test("wrong-x-order: detects X sections out of order in _closing", async () => {
  const results = await toml.lint(fixture("toml/wrong-x-order.toml"));
  const errs = errors(results);
  const orderWarns = byRule(results, "closing/x-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(orderWarns.length, 0, "Should detect wrong X section order in _closing");
  assert(
    hasMessage(results, "X1") && hasMessage(results, "X3"),
    "Should reference the misordered X sections",
  );
});

Deno.test("valid-complete: zero closing x-order warnings", async () => {
  const results = await toml.lint(fixture("toml/valid-complete.toml"));
  const orderWarns = byRule(results, "closing/x-order");
  assertEquals(orderWarns.length, 0, `Expected 0 closing/x-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// TOML linter tests — fixture-driven through the public lint() interface.
// Value-level checks complement structural checks: structure says "field exists",
// values say "field is correct." Together they form the quality gate.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
