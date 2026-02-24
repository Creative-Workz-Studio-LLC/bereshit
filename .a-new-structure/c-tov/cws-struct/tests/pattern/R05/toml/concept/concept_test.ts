// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/pattern/R05/toml/concept/concept_test.ts
// purpose: TOML R[5] concept/detail tests — DATA-DRIVEN.
//          Value format checking, field constraints, form enforcement,
//          body zones, version consistency, date ordering.
//          Error catalog is the source of truth.
// layer:   R[5] — Concept/detail detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  lazyFormat, errors, warnings, infos, byRule, hasRule,
} from "@test/helpers.ts";
import type { TestCase } from "@test/_runner.ts";
import {
  runSuite, lintOnce, noErrors,
} from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// TOML R05 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const TOML_R05_CASES: TestCase[] = [

  // --- content: version consistency and date ordering ---

  {
    code: "CWS-R05-082",
    fixture: "toml/content/value-version-mismatch.toml",
    desc: "detects at/version inconsistency",
    rule: "values/consistency/version",
  },

  {
    code: "CWS-R05-081",
    fixture: "toml/content/value-version-mismatch.toml",
    desc: "detects date order problem (created after updated)",
    rule: "values/C1_state/date-order",
  },

  // --- form: floor/ceiling enforcement ---

  {
    code: "CWS-R25-033",
    fixture: "toml/cargo/missing-omni-metadata.toml",
    desc: "required-section-missing for OmniMetadata",
    messageContains: ["OmniMetadata"],
  },

  {
    code: "CWS-R25-034",
    fixture: "toml/cargo/library-with-bin.toml",
    desc: "reserved-section-present for BinTarget",
    messageContains: ["BinTarget"],
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "toml", label: "R05", cases: TOML_R05_CASES });

// ---------------------------------------------------------------------------
// Additional tests — value checks, form validation, body zones
// ---------------------------------------------------------------------------

const toml = lazyFormat("toml");

// --- value-problems: 8 checks sharing one fixture ---

Deno.test("R05/content/value-problems: detects bad I1_core.at version format", async () => {
  const results = await lintOnce(toml, "toml", "toml/content/value-problems.toml");
  assert(hasRule(results, "values/I1_core/at"), "Should flag invalid I1_core.at version format");
  const atErrors = errors(results).filter((r) => r.rule.includes("values/I1_core/at"));
  assertGreater(atErrors.length, 0, "I1_core.at should produce an error for bad format");
});

Deno.test("R05/content/value-problems: detects bad C1_state.version format", async () => {
  const results = await lintOnce(toml, "toml", "toml/content/value-problems.toml");
  assert(hasRule(results, "values/C1_state/version"), "Should flag invalid C1_state.version format");
  const versionErrors = errors(results).filter((r) => r.rule.includes("values/C1_state/version"));
  assertGreater(versionErrors.length, 0, "C1_state.version should produce an error for bad format");
});

Deno.test("R05/content/value-problems: detects missing B- prefix", async () => {
  const results = await lintOnce(toml, "toml", "toml/content/value-problems.toml");
  assert(hasRule(results, "values/I1_core/key"), "Should flag missing B- prefix on I1_core.key");
});

Deno.test("R05/content/value-problems: detects empty required field", async () => {
  const results = await lintOnce(toml, "toml", "toml/content/value-problems.toml");
  const emptyWarns = warnings(results).filter((r) =>
    r.rule.includes("values/") && r.rule.includes("/empty"));
  assertGreater(emptyWarns.length, 0, "Should warn about empty required fields");
});

Deno.test("R05/content/value-problems: detects filename mismatch", async () => {
  const results = await lintOnce(toml, "toml", "toml/content/value-problems.toml");
  assert(hasRule(results, "values/I3_instance/file"), "Should flag filename mismatch");
});

Deno.test("R05/content/value-problems: detects bad date format", async () => {
  const results = await lintOnce(toml, "toml", "toml/content/value-problems.toml");
  assert(hasRule(results, "values/C1_state/created"), "Should flag invalid date format");
});

Deno.test("R05/content/value-problems: detects unknown type", async () => {
  const results = await lintOnce(toml, "toml", "toml/content/value-problems.toml");
  const typeInfos = infos(results).filter((r) => r.rule.includes("values/I2_family/type"));
  assertGreater(typeInfos.length, 0, "Should flag unknown I2_family.type");
});

Deno.test("R05/content/value-problems: detects empty required array", async () => {
  const results = await lintOnce(toml, "toml", "toml/content/value-problems.toml");
  const emptyArrayWarns = warnings(results).filter((r) => r.message.includes("empty array"));
  assertGreater(emptyArrayWarns.length, 0, "Should warn about empty required arrays");
});

// --- clean: valid-complete ---

Deno.test("R05/content/valid-complete: zero value errors and warnings", async () => {
  const results = await lintOnce(toml, "toml", "toml/structure/valid-complete.toml");
  const valueResults = byRule(results, "values/");
  const valueErrors = valueResults.filter((r) => r.severity === "error");
  const valueWarns = valueResults.filter((r) => r.severity === "warn");
  assertEquals(valueErrors.length, 0, `Expected 0 value errors: ${JSON.stringify(valueErrors, null, 2)}`);
  assertEquals(valueWarns.length, 0, `Expected 0 value warnings: ${JSON.stringify(valueWarns, null, 2)}`);
});

// --- body zones ---

Deno.test("R05/content/body-zones: zero errors", async () => {
  const results = await lintOnce(toml, "toml", "toml/content/content-body-zones.toml");
  noErrors(results);
});

Deno.test("R05/content/body-zones: zone labels (Cc, Co, Cv) present — no zone warnings", async () => {
  const results = await lintOnce(toml, "toml", "toml/content/content-body-zones.toml");
  const zoneWarns = byRule(results, "content/zone");
  assertEquals(zoneWarns.length, 0, `Expected 0 content/zone warnings: ${JSON.stringify(zoneWarns, null, 2)}`);
});

// --- form: clean validation ---

Deno.test("R05/form/valid-library: detects library form arg", async () => {
  const results = await lintOnce(toml, "toml", "toml/cargo/valid-library.toml");
  const formInfo = infos(results).filter((r) => r.rule === "pragma/forms");
  assertGreater(formInfo.length, 0, "Should detect library form in pragma");
  assert(formInfo[0]!.message.includes("library"), "Form info should mention library");
});

Deno.test("R05/form/valid-library: no required-section-missing", async () => {
  const results = await lintOnce(toml, "toml", "toml/cargo/valid-library.toml");
  const missing = results.filter((r) => r.rule === "form/required-section-missing");
  assertEquals(missing.length, 0, "Valid library should have all required sections");
});

Deno.test("R05/form/valid-library: no reserved-section-present", async () => {
  const results = await lintOnce(toml, "toml", "toml/cargo/valid-library.toml");
  const reserved = results.filter((r) => r.rule === "form/reserved-section-present");
  assertEquals(reserved.length, 0, "Valid library should have no reserved sections");
});

Deno.test("R05/form/library-with-bin: reserved message includes why_reserved", async () => {
  const results = await lintOnce(toml, "toml", "toml/cargo/library-with-bin.toml");
  const reserved = results.filter((r) => r.rule === "form/reserved-section-present");
  assertGreater(reserved.length, 0, "Should have reserved section info");
  assert(
    reserved.some((r) => r.message.includes("Libraries don't have binary targets")),
    "Reserved message should explain why [[bin]] is reserved for libraries",
  );
});

Deno.test("R05/form/library-with-bin: zero errors despite reserved section", async () => {
  const results = await lintOnce(toml, "toml", "toml/cargo/library-with-bin.toml");
  noErrors(results);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[5] TOML concept/detail tests — DATA-DRIVEN.
// 4 table-driven cases + 16 bespoke = 22+ test functions.
// Fixture caching: value-problems (8x→1x), valid-complete (1x), body-zones (2x→1x),
// valid-library (3x→1x), library-with-bin (2x→1x), value-version-mismatch (2x→1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
