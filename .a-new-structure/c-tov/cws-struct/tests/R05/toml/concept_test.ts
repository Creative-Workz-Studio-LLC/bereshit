// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R05/toml/concept_test.ts
// key:     B-tov-cws-struct-tests-R05-toml-concept
// title:   CWS Struct — TOML R05 Concept/Detail Detection Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// source:  Extracted from tests/handlers/toml_test.ts
// purpose: R05 (finest) detection — concept-level and detail validation for TOML.
//          Value format checking, field constraints, form enforcement,
//          body zones, version consistency.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  fixture, lazyFormat, errors, warnings, infos, byRule, hasRule, hasMessage,
  hasErrorCode, errorCodeReport,
} from "../../helpers.ts";

// ============================================================================
// BODY
// ============================================================================

const toml = lazyFormat("toml");

// ---------------------------------------------------------------------------
// content/ — Value validation, zones, consistency
// ---------------------------------------------------------------------------

Deno.test("R05/content/valid-complete: zero value errors", async () => {
  const results = await toml.lint(fixture("toml/structure/valid-complete.toml"));
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

Deno.test("R05/content/value-problems: detects bad version format", async () => {
  const results = await toml.lint(fixture("toml/content/value-problems.toml"));
  assert(
    hasRule(results, "values/I1_core/at"),
    "Should flag invalid I1_core.at version format",
  );
  const atErrors = errors(results).filter((r) => r.rule.includes("values/I1_core/at"));
  assertGreater(atErrors.length, 0, "I1_core.at should produce an error for bad format");
});

Deno.test("R05/content/value-problems: detects bad C1 version format", async () => {
  const results = await toml.lint(fixture("toml/content/value-problems.toml"));
  assert(
    hasRule(results, "values/C1_state/version"),
    "Should flag invalid C1_state.version format",
  );
  const versionErrors = errors(results).filter((r) => r.rule.includes("values/C1_state/version"));
  assertGreater(versionErrors.length, 0, "C1_state.version should produce an error for bad format");
});

Deno.test("R05/content/value-problems: detects missing B- prefix", async () => {
  const results = await toml.lint(fixture("toml/content/value-problems.toml"));
  assert(
    hasRule(results, "values/I1_core/key"),
    "Should flag missing B- prefix on I1_core.key",
  );
});

Deno.test("R05/content/value-problems: detects empty required field", async () => {
  const results = await toml.lint(fixture("toml/content/value-problems.toml"));
  const emptyWarns = warnings(results).filter((r) =>
    r.rule.includes("values/") && r.rule.includes("/empty")
  );
  assertGreater(emptyWarns.length, 0, "Should warn about empty required fields");
});

Deno.test("R05/content/value-problems: detects filename mismatch", async () => {
  const results = await toml.lint(fixture("toml/content/value-problems.toml"));
  assert(
    hasRule(results, "values/I3_instance/file"),
    "Should flag filename mismatch",
  );
});

Deno.test("R05/content/value-problems: detects bad date format", async () => {
  const results = await toml.lint(fixture("toml/content/value-problems.toml"));
  assert(
    hasRule(results, "values/C1_state/created"),
    "Should flag invalid date format",
  );
});

Deno.test("R05/content/value-problems: detects unknown type", async () => {
  const results = await toml.lint(fixture("toml/content/value-problems.toml"));
  const typeInfos = infos(results).filter((r) => r.rule.includes("values/I2_family/type"));
  assertGreater(typeInfos.length, 0, "Should flag unknown I2_family.type");
});

Deno.test("R05/content/value-problems: detects empty required array", async () => {
  const results = await toml.lint(fixture("toml/content/value-problems.toml"));
  const emptyArrayWarns = warnings(results).filter((r) =>
    r.message.includes("empty array")
  );
  assertGreater(emptyArrayWarns.length, 0, "Should warn about empty required arrays");
});

Deno.test("[CWS-R05-082] R05/content/value-version-mismatch: detects at/version inconsistency", async () => {
  const results = await toml.lint(fixture("toml/content/value-version-mismatch.toml"));
  assert(
    hasRule(results, "values/consistency/version"),
    "Should flag version inconsistency between I1_core.at and C1_state.version",
  );
  // Error code: CWS-R05-082 (values/consistency/version — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-082"),
    `Expected CWS-R05-082 (version consistency), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R05-081] R05/content/value-version-mismatch: detects date order problem", async () => {
  const results = await toml.lint(fixture("toml/content/value-version-mismatch.toml"));
  assert(
    hasRule(results, "values/C1_state/date-order"),
    "Should flag created date after updated date",
  );
  // Error code: CWS-R05-081 (values/C1_state/date-order — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-081"),
    `Expected CWS-R05-081 (TOML value validation), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("R05/content/body-zones: zero errors", async () => {
  const results = await toml.lint(fixture("toml/content/content-body-zones.toml"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("R05/content/body-zones: zone labels (Cc, Co, Cv) present", async () => {
  const results = await toml.lint(fixture("toml/content/content-body-zones.toml"));
  const zoneWarns = byRule(results, "content/zone");
  assertEquals(zoneWarns.length, 0, `Expected 0 content/zone warnings: ${JSON.stringify(zoneWarns, null, 2)}`);
});

// ---------------------------------------------------------------------------
// form/ — Form-aware validation (floor/ceiling enforcement)
// ---------------------------------------------------------------------------

Deno.test("R05/form/valid-library: detects library form arg", async () => {
  const results = await toml.lint(fixture("toml/cargo/valid-library.toml"));
  const formInfo = infos(results).filter((r) => r.rule === "pragma/forms");
  assertGreater(formInfo.length, 0, "Should detect library form in pragma");
  assert(formInfo[0]!.message.includes("library"), "Form info should mention library");
});

Deno.test("R05/form/valid-library: no required-section-missing", async () => {
  const results = await toml.lint(fixture("toml/cargo/valid-library.toml"));
  const missing = results.filter((r) => r.rule === "form/required-section-missing");
  assertEquals(missing.length, 0, "Valid library should have all required sections");
});

Deno.test("R05/form/valid-library: no reserved-section-present", async () => {
  const results = await toml.lint(fixture("toml/cargo/valid-library.toml"));
  const reserved = results.filter((r) => r.rule === "form/reserved-section-present");
  assertEquals(reserved.length, 0, "Valid library should have no reserved sections");
});

Deno.test("[CWS-R25-033] R05/form/missing-omni-metadata: required-section-missing for OmniMetadata", async () => {
  const results = await toml.lint(fixture("toml/cargo/missing-omni-metadata.toml"));
  const missing = results.filter((r) => r.rule === "form/required-section-missing");
  assertGreater(missing.length, 0, "Should warn about missing OmniMetadata");
  assert(
    missing.some((r) => r.message.includes("OmniMetadata")),
    "Should specifically mention OmniMetadata",
  );
  // Error code: CWS-R25-033 (form/required-section-missing — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-033"),
    `Expected CWS-R25-033 (required section missing), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R25-034] R05/form/library-with-bin: reserved-section-present for BinTarget", async () => {
  const results = await toml.lint(fixture("toml/cargo/library-with-bin.toml"));
  const reserved = results.filter((r) => r.rule === "form/reserved-section-present");
  assertGreater(reserved.length, 0, "Should detect reserved [[bin]] section");
  assert(
    reserved.some((r) => r.message.includes("BinTarget")),
    "Should specifically mention BinTarget",
  );
  // Error code: CWS-R25-034 (form/reserved-section-present — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-034"),
    `Expected CWS-R25-034 (reserved section present), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("R05/form/library-with-bin: reserved message includes why_reserved", async () => {
  const results = await toml.lint(fixture("toml/cargo/library-with-bin.toml"));
  const reserved = results.filter((r) => r.rule === "form/reserved-section-present");
  assertGreater(reserved.length, 0, "Should have reserved section info");
  assert(
    reserved.some((r) => r.message.includes("Libraries don't have binary targets")),
    "Reserved message should explain why [[bin]] is reserved for libraries",
  );
});

Deno.test("R05/form/library-with-bin: zero errors despite reserved section", async () => {
  const results = await toml.lint(fixture("toml/cargo/library-with-bin.toml"));
  const errs = errors(results);
  assertEquals(errs.length, 0, "Reserved section is info-level, not error-level");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R05 TOML tests — finest detection layer.
// Value format checking, field constraints, form floor/ceiling enforcement,
// body zone validation, version consistency, date ordering.
//
// 20 tests. The most granular level of TOML validation.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
