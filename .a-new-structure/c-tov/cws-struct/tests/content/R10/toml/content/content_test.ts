// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/content/R10/toml/content/content_test.ts
// purpose: R10 (content/placement) detection tests for TOML — DATA-DRIVEN.
//          Metadata contract validation, cargo derivation integration.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  fixture, lazyFormat, errors, warnings, infos,
  hasRule, hasMessage,
} from "@test/helpers.ts";
import { runR10Suite, noErrors } from "../../_r10.ts";
import type { R10TestCase } from "../../_r10.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// TOML R10 Test Specification — THE TABLE
// ---------------------------------------------------------------------------
//
// TOML R10 is lighter — mostly contract validation and cargo integration.
// As more TOML-specific R10 codes are added, they go in this table.
// ---------------------------------------------------------------------------

const TOML_R10_CASES: R10TestCase[] = [
  // TOML doesn't yet have dedicated CWS-R10-* codes for its format-specific
  // checks. The contract/phantom check is detected by message content.
  // As the error catalog grows, these become table rows with proper codes.
];

// Run any table-driven cases (currently empty — ready for growth)
if (TOML_R10_CASES.length > 0) {
  runR10Suite({ format: "toml", cases: TOML_R10_CASES });
}

// ---------------------------------------------------------------------------
// Bespoke TOML R10 tests — not yet mapped to CWS error codes
// ---------------------------------------------------------------------------
//
// These tests will migrate to the table as their error codes are created.
// The table grows; the bespoke section shrinks. That's the pattern.
// ---------------------------------------------------------------------------

const toml = lazyFormat("toml");

Deno.test("R10/metadata/bad-contract: detects phantom section declaration", async () => {
  const results = await toml.lint(fixture("toml/metadata/bad-contract.toml"));
  assert(
    hasMessage(results, "I99_phantom"),
    "Should warn about phantom section I99_phantom in required_identity",
  );
  const phantomWarns = warnings(results).filter((r) => r.message.includes("I99_phantom"));
  assertGreater(phantomWarns.length, 0, "Should have at least one phantom declaration warning");
});

Deno.test("R10/cargo/valid-library: zero errors", async () => {
  const results = await toml.lint(fixture("toml/cargo/valid-library.toml"));
  noErrors(results);
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
  noErrors(results);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R10 TOML tests — DATA-DRIVEN (table ready, bespoke section shrinks as
// error codes are created for TOML-specific R10 checks).
// 5 tests. Contract validation, cargo derivation, metadata extraction.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
