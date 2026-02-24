// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/content/R10/rust/content/content_test.ts
// purpose: R10 (content/placement) detection tests for Rust — DATA-DRIVEN.
//          Each test case is a row in a table. The error catalog is the
//          source of truth for severity and rule patterns.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals } from "jsr:@std/assert";
import { fixture, lazyFormat, byRule } from "@test/helpers.ts";
import { runR10Suite, noErrors } from "../../_r10.ts";
import type { R10TestCase } from "../../_r10.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Rust R10 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const RUST_R10_CASES: R10TestCase[] = [

  // --- placement: content in wrong blocks ---

  {
    code: "CWS-R10-002",
    fixture: "rust/structure/content-placement-wrong.rs",
    desc: "detects fn in SETUP and use/struct in BODY",
    rule: "content/block-placement",
    messageContains: ["fn_decl", "SETUP"],
    cleanFixture: "rust/structure/content-placement-correct.rs",
    cleanDesc: "zero placement warnings",
  },

  {
    code: "CWS-R10-001",
    fixture: "rust/metadata/metadata-leak.rs",
    desc: "detects code declarations in METADATA block",
    rule: "content/metadata-leak",
  },

  // --- scaling: block size signals ---

  {
    code: "CWS-R10-041",
    fixture: "rust/structure/scaling-large.rs",
    desc: "detects SETUP block exceeding size threshold",
    rule: "scaling/setup-size",
    messageContains: ["211 content lines"],
  },

  {
    code: "CWS-R10-042",
    fixture: "rust/structure/scaling-large.rs",
    desc: "detects BODY block exceeding size threshold",
    rule: "scaling/body-size",
    messageContains: ["511 content lines"],
  },

  // --- identity: field presence ---

  {
    code: "CWS-R10-011",
    fixture: "rust/metadata/missing-ic-crate/lib.rs",
    desc: "warns about missing required I/C fields",
    minCount: 7,
    validate: (results) => {
      noErrors(results);
      for (const field of ["I1.key", "I1.from", "I2.type", "C1.version", "C2.organization", "C3.scripture", "C4.consumers"]) {
        const has = results.some((r) => r.rule.includes(field));
        assertEquals(has, true, `Should warn about missing ${field}`);
      }
    },
  },

  {
    code: "CWS-R10-013",
    fixture: "rust/metadata/unknown-subtype.rs",
    desc: "warns when I2.subtype is not a known value",
    rule: "I2.subtype-value",
  },

  {
    code: "CWS-R05-012",
    fixture: "rust/metadata/placeholder-values.rs",
    desc: "warns about template placeholders",
  },

  // --- value: template/derived status checks ---

  {
    code: "CWS-R10-031",
    fixture: "rust/metadata/template-with-derived-values.rs",
    desc: "template file with non-Template C1.status",
    rule: "template-status",
  },

  {
    code: "CWS-R10-033",
    fixture: "rust/metadata/value-mismatches.rs",
    desc: "derived file with C1.status='Template'",
    rule: "derived-status",
  },

  {
    code: "CWS-R10-035",
    fixture: "rust/metadata/value-mismatches.rs",
    desc: "I1.format='go' in Rust file",
    rule: "format-mismatch",
    messageContains: ["go", "rust"],
  },

  // --- subsection: content placement within SETUP ---

  {
    code: "CWS-R10-003",
    fixture: "rust/setup/misplaced-content.rs",
    desc: "detects misplaced declarations within SETUP subsections",
    rule: "content/subsection-placement",
    messageContains: ["CoreTypes", "Constants"],
    hasFix: true,
    fixActions: ["remove", "insert"],
  },

  // --- emphasis: subtype emphasis balance ---

  {
    code: "CWS-R10-040",
    fixture: "rust/setup/emphasis-inverted.rs",
    desc: "library with empty heavy subs and full light subs",
    rule: "emphasis/setup-inverted",
    messageContains: ["library"],
  },

  // --- closing: test/main placement ---

  {
    code: "CWS-R10-004",
    fixture: "rust/closing/tests-in-body.rs",
    desc: "detects #[cfg(test)] in BODY — should be CLOSING Cv",
    rule: "closing/test-placement",
    validate: noErrors,
  },

  {
    code: "CWS-R10-005",
    fixture: "rust/closing/main-in-body.rs",
    desc: "detects fn main() in BODY — should be CLOSING Ce",
    rule: "closing/main-placement",
    validate: noErrors,
  },

  // --- closing: X6 template-only ---

  {
    code: "CWS-R10-006",
    fixture: "rust/closing/x6-in-derived.rs",
    desc: "detects X6 section in non-template file",
    rule: "closing/X6-template-only",
    messageContains: ["Template Guide"],
    cleanFixture: "rust/structure/valid-library.rs",
    cleanDesc: "valid-library has no X6 warning",
    validate: noErrors,
  },

  // --- closing: zone content violations ---

  {
    code: "CWS-R10-050",
    fixture: "rust/closing/zone-content-violations.rs",
    desc: "detects code in Ce zone that should be empty (module)",
    rule: "content/closing-zone/Ce",
    messageContains: ["empty", "module"],
  },

  {
    code: "CWS-R10-051",
    fixture: "rust/closing/zone-content-violations.rs",
    desc: "detects empty Cv zone when schema expects content (module)",
    rule: "content/closing-zone/Cv",
    messageContains: ["empty", "cfg(test)"],
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runR10Suite({ format: "rust", cases: RUST_R10_CASES });

// ---------------------------------------------------------------------------
// Additional bespoke tests
// ---------------------------------------------------------------------------

const rust = lazyFormat("rust");

Deno.test("R10/structure/scaling: small fixture does not trigger scaling signals", async () => {
  const results = await rust.lint(fixture("rust/setup/subsection-order-correct.rs"));
  const scaling = byRule(results, "structure/scaling");
  assertEquals(scaling.length, 0, "Small fixture should not trigger scaling signals");
});

Deno.test("R10/metadata/missing-ic-fields: zero errors (missing fields are warnings, not errors)", async () => {
  const results = await rust.lint(fixture("rust/metadata/missing-ic-crate/lib.rs"));
  noErrors(results);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R10 Rust content/placement tests — DATA-DRIVEN.
// 17 table-driven cases + 2 bespoke = 21+ test functions.
// The table IS the specification. The error catalog IS the authority.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
