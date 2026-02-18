// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/foundation/schema_test.ts
// key:     B-tov-cws-struct-tests-foundation-schema
// title:   CWS Struct — Schema Loader Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-17
// authors: Nathan Emet (CPI-SI)
// purpose: Tests for loadRules, clearCache, and DerivedRules structure.
//          Verifies the schema is correctly loaded and processed.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import { loadRules, clearCache } from "../../lib/foundation/mod.ts";
import type { DerivedRules } from "../../lib/foundation/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// loadRules — basic loading
// ---------------------------------------------------------------------------

Deno.test("schema/loadRules: returns DerivedRules object", async () => {
  clearCache();
  const rules = await loadRules();
  assert(rules !== null && rules !== undefined, "Should return rules");
  assert(rules.contract !== undefined, "Should have contract");
});

Deno.test("schema/loadRules: contract has metadata/content/closing", async () => {
  const rules = await loadRules();
  assert(rules.contract.metadata !== undefined, "Should have metadata contract");
  assert(rules.contract.content !== undefined, "Should have content contract");
  assert(rules.contract.closing !== undefined, "Should have closing contract");
});

// ---------------------------------------------------------------------------
// DerivedRules structure — compiled patterns
// ---------------------------------------------------------------------------

Deno.test("schema/loadRules: metadataKeyPattern is a RegExp", async () => {
  const rules = await loadRules();
  assert(rules.metadataKeyPattern instanceof RegExp, "Should be a RegExp");
});

Deno.test("schema/loadRules: closingKeyPattern is a RegExp", async () => {
  const rules = await loadRules();
  assert(rules.closingKeyPattern instanceof RegExp, "Should be a RegExp");
});

Deno.test("schema/loadRules: metadataKeyPattern matches valid section names", async () => {
  const rules = await loadRules();
  assert(rules.metadataKeyPattern.test("I1_core"), "Should match I1_core");
  assert(rules.metadataKeyPattern.test("C1_state"), "Should match C1_state");
});

// ---------------------------------------------------------------------------
// DerivedRules structure — stripped arrays
// ---------------------------------------------------------------------------

Deno.test("schema/loadRules: requiredIdentity contains stripped section names", async () => {
  const rules = await loadRules();
  assertGreater(rules.requiredIdentity.length, 0, "Should have required identity sections");
  // I1_core should be required
  assert(rules.requiredIdentity.includes("I1_core"), "I1_core should be required");
  // No brackets — stripped
  for (const s of rules.requiredIdentity) {
    assert(!s.startsWith("["), `"${s}" should not start with [`);
  }
});

Deno.test("schema/loadRules: requiredContext contains stripped section names", async () => {
  const rules = await loadRules();
  assertGreater(rules.requiredContext.length, 0, "Should have required context sections");
  assert(rules.requiredContext.includes("C1_state"), "C1_state should be required");
});

Deno.test("schema/loadRules: requiredClosing contains stripped section names", async () => {
  const rules = await loadRules();
  assertGreater(rules.requiredClosing.length, 0, "Should have required closing sections");
  assert(rules.requiredClosing.includes("X1_policy"), "X1_policy should be required");
});

// ---------------------------------------------------------------------------
// DerivedRules structure — sets
// ---------------------------------------------------------------------------

Deno.test("schema/loadRules: allKnownSections is a Set", async () => {
  const rules = await loadRules();
  assert(rules.allKnownSections instanceof Set, "Should be a Set");
  assert(rules.allKnownSections.has("I1_core"), "Should contain I1_core");
  assert(rules.allKnownSections.has("C1_state"), "Should contain C1_state");
});

Deno.test("schema/loadRules: allKnownClosing is a Set", async () => {
  const rules = await loadRules();
  assert(rules.allKnownClosing instanceof Set, "Should be a Set");
  assert(rules.allKnownClosing.has("X1_policy"), "Should contain X1_policy");
});

// ---------------------------------------------------------------------------
// DerivedRules structure — field requirements
// ---------------------------------------------------------------------------

Deno.test("schema/loadRules: metadataFields has I1_core requirements", async () => {
  const rules = await loadRules();
  const i1 = rules.metadataFields["I1_core"];
  assert(i1 !== undefined, "Should have I1_core field requirements");
  assertGreater(i1.required.length, 0, "I1_core should have required fields");
  assert(i1.required.includes("key"), "I1_core should require 'key'");
});

Deno.test("schema/loadRules: closingFields has X1_policy requirements", async () => {
  const rules = await loadRules();
  const x1 = rules.closingFields["X1_policy"];
  assert(x1 !== undefined, "Should have X1_policy field requirements");
  assertGreater(x1.required.length, 0, "X1_policy should have required fields");
  assert(x1.required.includes("never"), "X1_policy should require 'never'");
});

// ---------------------------------------------------------------------------
// clearCache
// ---------------------------------------------------------------------------

Deno.test("schema/clearCache: forces re-load on next call", async () => {
  // Load once (caches)
  const first = await loadRules();
  clearCache();
  // Load again (should re-read from disk)
  const second = await loadRules();
  // Both should have the same structure
  assertEquals(first.requiredIdentity.length, second.requiredIdentity.length);
  assertEquals(first.requiredClosing.length, second.requiredClosing.length);
});

Deno.test("schema/clearCache: idempotent (calling twice doesn't break)", () => {
  clearCache();
  clearCache(); // Should not throw
});

// ---------------------------------------------------------------------------
// Caching behavior
// ---------------------------------------------------------------------------

Deno.test("schema/loadRules: returns same reference on second call (caching)", async () => {
  clearCache();
  const first = await loadRules();
  const second = await loadRules();
  // Same object reference = cached
  assert(first === second, "Second call should return cached reference");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Schema loader tests — verifying the single source of truth loads correctly.
// "Thy word is a lamp unto my feet." — Psalm 119:105
// ============================================================================
