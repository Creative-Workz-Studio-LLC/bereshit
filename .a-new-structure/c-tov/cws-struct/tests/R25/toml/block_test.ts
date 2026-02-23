// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R25/toml/block_test.ts
// key:     B-tov-cws-struct-tests-R25-toml-block
// title:   CWS Struct — TOML R25 Block-Level Detection Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// source:  Extracted from tests/handlers/toml_test.ts
// purpose: R25 detection — block-level structure validation for TOML.
//          Pragma consistency, block order, closing section ordering.
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
// consistency/ — Cross-cutting consistency (pragma vs format)
// ---------------------------------------------------------------------------

Deno.test("[CWS-R25-071] R25/consistency/pragma-mismatch: detects pragma vs format field mismatch", async () => {
  const results = await toml.lint(fixture("toml/structure/pragma-mismatch.toml"));
  assert(
    hasRule(results, "pragma") || hasMessage(results, "mismatch") || hasMessage(results, "pragma"),
    "Should detect pragma/format type mismatch",
  );
  // Error code: CWS-R25-071 (pragma/classification — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-071"),
    `Expected CWS-R25-071 (pragma classification), got: ${errorCodeReport(results)}`,
  );
});

// ---------------------------------------------------------------------------
// closing/ — X section ordering
// ---------------------------------------------------------------------------

Deno.test("[CWS-R25-025] R25/closing/wrong-x-order: detects X sections out of order", async () => {
  const results = await toml.lint(fixture("toml/closing/wrong-x-order.toml"));
  const errs = errors(results);
  const orderWarns = byRule(results, "closing/x-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(orderWarns.length, 0, "Should detect wrong X section order in _closing");
  assert(
    hasMessage(results, "X1") && hasMessage(results, "X3"),
    "Should reference the misordered X sections",
  );
  // Error code: CWS-R25-025 (closing/x-order — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-025"),
    `Expected CWS-R25-025 (closing x-order), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("R25/closing/valid-complete: zero x-order warnings", async () => {
  const results = await toml.lint(fixture("toml/structure/valid-complete.toml"));
  const orderWarns = byRule(results, "closing/x-order");
  assertEquals(orderWarns.length, 0, `Expected 0 closing/x-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

// ---------------------------------------------------------------------------
// metadata/ — Missing metadata sections (extension identity/context)
// ---------------------------------------------------------------------------

Deno.test("[CWS-R25-072] R25/metadata/missing-extension-sections: warns about missing I4/C5/C6/C7", async () => {
  const results = await toml.lint(fixture("toml/metadata/bad-contract.toml"));
  const metaWarns = warnings(results).filter((r) =>
    r.rule.startsWith("metadata/") && (
      r.message.includes("I4_architecture") ||
      r.message.includes("C5_intent") ||
      r.message.includes("C6_roadmap") ||
      r.message.includes("C7_classification")
    )
  );
  assertGreater(metaWarns.length, 0, "Should warn about missing extension metadata sections");
  // Error code: CWS-R25-072 (metadata/{{key}} — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-072"),
    `Expected CWS-R25-072 (missing metadata section), got: ${errorCodeReport(results)}`,
  );
});

// ---------------------------------------------------------------------------
// content/ — Missing content zone labels
// ---------------------------------------------------------------------------

Deno.test("[CWS-R25-060] R25/content/missing-zone-labels: warns about missing Co and Cv zones", async () => {
  const results = await toml.lint(fixture("toml/metadata/bad-contract.toml"));
  const zoneWarns = warnings(results).filter((r) =>
    r.rule.startsWith("content/") && (
      r.message.includes("Co") || r.message.includes("Cv")
    )
  );
  assertGreater(zoneWarns.length, 0, "Should warn about missing content zone labels");
  // Error code: CWS-R25-060 (content/{{label}} — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-060"),
    `Expected CWS-R25-060 (missing zone label), got: ${errorCodeReport(results)}`,
  );
});

// ---------------------------------------------------------------------------
// closing/ — Missing closing sections (defined but absent)
// ---------------------------------------------------------------------------

Deno.test("[CWS-R25-073] R25/closing/missing-defined-sections: reports missing X2/X3/X4", async () => {
  const results = await toml.lint(fixture("toml/metadata/bad-contract.toml"));
  const closingInfos = infos(results).filter((r) =>
    r.rule.startsWith("closing/") && (
      r.message.includes("X2_extension") ||
      r.message.includes("X3_troubleshooting") ||
      r.message.includes("X4_reference")
    )
  );
  assertGreater(closingInfos.length, 0, "Should report missing defined closing sections");
  // Error code: CWS-R25-073 (closing/{{key}} — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-073"),
    `Expected CWS-R25-073 (missing closing section), got: ${errorCodeReport(results)}`,
  );
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R25 TOML tests — block-level detection.
// Pragma consistency, closing section ordering, missing metadata sections,
// missing content zone labels, missing closing sections.
//
// 6 tests. Block-level structure concerns.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
