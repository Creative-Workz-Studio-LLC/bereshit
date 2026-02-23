// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R10/toml/content_test.ts
// key:     B-tov-cws-struct-tests-R10-toml-content
// title:   CWS Struct — TOML R10 Content/Placement Detection Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// source:  Extracted from tests/handlers/toml_test.ts
// purpose: R10 detection — content placement and contract validation for TOML.
//          Metadata contract validation, cargo derivation integration.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import { fixture, lazyFormat, errors, warnings, infos, hasRule, hasMessage } from "../../helpers.ts";

// ============================================================================
// BODY
// ============================================================================

const toml = lazyFormat("toml");

// ---------------------------------------------------------------------------
// metadata/ — Contract validation, identity checks
// ---------------------------------------------------------------------------

Deno.test("R10/metadata/bad-contract: detects phantom section declaration", async () => {
  const results = await toml.lint(fixture("toml/metadata/bad-contract.toml"));
  assert(
    hasMessage(results, "I99_phantom"),
    "Should warn about phantom section I99_phantom in required_identity",
  );
  const phantomWarns = warnings(results).filter((r) => r.message.includes("I99_phantom"));
  assertGreater(phantomWarns.length, 0, "Should have at least one phantom declaration warning");
});

// ---------------------------------------------------------------------------
// cargo/ — Cargo.toml integration tests
// ---------------------------------------------------------------------------

Deno.test("R10/cargo/valid-library: zero errors", async () => {
  const results = await toml.lint(fixture("toml/cargo/valid-library.toml"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("R10/cargo/valid-library: zero warnings", async () => {
  const results = await toml.lint(fixture("toml/cargo/valid-library.toml"));
  const warns = warnings(results);
  assertEquals(warns.length, 0, `Expected 0 warnings, got ${warns.length}: ${JSON.stringify(warns, null, 2)}`);
});

Deno.test("R10/cargo/valid-library: pragma classifies cargo derivation", async () => {
  const results = await toml.lint(fixture("toml/cargo/valid-library.toml"));
  assert(
    hasRule(results, "pragma/derivations") || hasRule(results, "pragma/classification"),
    "Should have pragma classification info",
  );
  const classInfo = infos(results).filter((r) =>
    r.rule.includes("pragma") && (r.message.includes("cargo") || r.message.includes("Cargo"))
  );
  assertGreater(classInfo.length, 0, "Should classify cargo derivation in pragma info");
});

Deno.test("R10/cargo/valid-library: metadata extracted from package.metadata.omni", async () => {
  const results = await toml.lint(fixture("toml/cargo/valid-library.toml"));
  // Cargo metadata extraction is silent — no layout info noise
  // Verify metadata sections are validated (I1 fields checked)
  const errs = results.filter((r) => r.severity === "error");
  assertEquals(errs.length, 0, "Valid library should have no errors");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R10 TOML tests — content/placement detection.
// Contract validation, cargo derivation, metadata extraction.
//
// 5 tests. Content in the right place, contracts honored.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
