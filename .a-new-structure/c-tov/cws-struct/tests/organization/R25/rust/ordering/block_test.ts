// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/organization/R25/rust/ordering/block_test.ts
// purpose: Rust R[25] block-level detection tests — DATA-DRIVEN.
//          Subsection order, doc comments, consistency, required subsections,
//          closing zones, identity statics, comment-meta, cargo schema.
//          Error catalog is the source of truth.
// layer:   R[25] — Block-level detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  fixture, lazyFormat, errors, warnings, infos, byRule, hasRule, hasMessage,
} from "@test/helpers.ts";
import type { TestCase } from "@test/_runner.ts";
import {
  runSuite, lintOnce, noErrors,
  noDocWarnings, noClosingWarnings, noBodyOrderWarnings,
} from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Rust R25 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const RUST_R25_CASES: TestCase[] = [

  // --- consistency: pragma-driven cross-cutting checks ---

  {
    code: "CWS-R10-030",
    fixture: "rust/metadata/template-with-derived-values.rs",
    desc: "detects wrong I1.at and C1.status for template",
    rule: "template-at",
    validate: (results) => {
      assertGreater(byRule(results, "content/PRAGMA/template-at").length, 0,
        "Should trigger content/PRAGMA/template-at");
      assertGreater(byRule(results, "content/METADATA/template-status").length, 0,
        "Should trigger content/METADATA/template-status");
    },
  },

  {
    code: "CWS-R10-032",
    fixture: "rust/metadata/derived-with-template-values.rs",
    desc: "detects template values in derived file",
    rule: "derived-at",
    validate: (results) => {
      assertGreater(byRule(results, "content/PRAGMA/derived-at").length, 0,
        "Should trigger content/PRAGMA/derived-at");
      assertGreater(byRule(results, "content/METADATA/derived-status").length, 0,
        "Should trigger content/METADATA/derived-status");
    },
  },

  {
    code: "CWS-R10-034",
    fixture: "rust/metadata/wrong-subtype.rs",
    desc: "detects I2.subtype mismatch with pragma",
    rule: "content/PRAGMA/subtype-mismatch",
    cleanFixture: "rust/structure/valid-library.rs",
    cleanDesc: "valid-library has no content-aware false positives",
  },

  // --- doc: doc comment quality checks ---

  {
    code: "CWS-R25-003",
    fixture: "rust/structure/valid-library.rs",
    desc: "cargo/schema rule resolves to R25-003",
    skip: "wiring-only — tested via error catalog import below",
  },

  // --- setup: subsection order and required ---

  {
    code: "CWS-R25-023",
    fixture: "rust/setup/subsection-order-wrong.rs",
    desc: "detects Core Types before Constants",
    rule: "setup/subsection-order",
    cleanFixture: "rust/setup/subsection-order-correct.rs",
    cleanDesc: "all 10 subsections in correct order — zero order warnings",
  },

  {
    code: "CWS-R10-043",
    fixture: "rust/setup/aliased-subsection-names.rs",
    desc: "detects alias names and suggests canonical alternatives",
    rule: "setup/alias-name",
    validate: (results) => {
      const orderWarns = byRule(results, "setup/subsection-order");
      assertEquals(orderWarns.length, 0,
        "Aliased names in correct order should produce 0 order warnings");
    },
  },

  {
    code: "CWS-R25-030",
    fixture: "rust/setup/missing-required-subsections.rs",
    desc: "library missing Constants, ErrorTypes, CoreTypes",
    rule: "setup/required-subsection",
    minCount: 3,
    validate: (results) => {
      const required = byRule(results, "setup/required-subsection");
      assertEquals(required.length, 3, "Should report exactly 3 missing required subsections");
      assert(hasMessage(required, "Constants") || hasMessage(required, "Error Types") || hasMessage(required, "Core Types"),
        "Should mention at least one missing required subsection");
    },
  },

  // --- body: subsection order and required ---

  {
    code: "CWS-R25-024",
    fixture: "rust/body/wrong-body-order.rs",
    desc: "detects BODY subsections out of order",
    rule: "body/subsection-order",
    messageContains: ["§1"],
    validate: noErrors,
    cleanFixture: "rust/structure/valid-library.rs",
    cleanDesc: "valid-library has zero body order warnings",
  },

  {
    code: "CWS-R25-031",
    fixture: "rust/body/missing-required-subsections.rs",
    desc: "library missing all 7 required body subsections",
    rule: "body/required-subsection",
    minCount: 7,
  },

  // --- closing: zone order and required zones ---

  {
    code: "CWS-R25-020",
    fixture: "rust/closing/wrong-closing-order.rs",
    desc: "detects code zone after documentation section",
    rule: "closing/zone-order",
    validate: noErrors,
    cleanFixture: "rust/structure/valid-library.rs",
    cleanDesc: "valid-library has zero closing zone warnings",
  },

  {
    code: "CWS-R25-021",
    fixture: "rust/closing/wrong-inner-order.rs",
    desc: "detects Ce after Cc (wrong code zone order)",
    rule: "closing/code-zone-order",
    messageContains: ["Ce", "Cc"],
  },

  {
    code: "CWS-R25-022",
    fixture: "rust/closing/wrong-inner-order.rs",
    desc: "detects X1 after X5 (wrong doc section order)",
    rule: "closing/doc-section-order",
    messageContains: ["X1", "X5"],
  },

  {
    code: "CWS-R25-032",
    fixture: "rust/closing/missing-required-zones.rs",
    desc: "missing X1 and X5 documentation zones triggers info",
    validate: (results) => {
      assertGreater(byRule(results, "closing/required-X1").length, 0, "Should detect missing X1 zone");
      assertGreater(byRule(results, "closing/required-X5").length, 0, "Should detect missing X5 zone");
    },
  },

  // --- identity: comment-meta block checks ---

  {
    code: "CWS-R25-017",
    fixture: "rust/metadata/missing-ic-crate/lib.rs",
    desc: "warns about missing Key: in METADATA comment block",
    validate: (results) => {
      const keyInfos = infos(results).filter((r) => r.rule === "comment-meta/key");
      assertGreater(keyInfos.length, 0, "Should report missing Key: in METADATA comment block");
    },
  },

  {
    code: "CWS-R25-018",
    fixture: "rust/metadata/missing-ic-crate/lib.rs",
    desc: "warns about missing Purpose: in METADATA comment block",
    validate: (results) => {
      const purposeInfos = infos(results).filter((r) => r.rule === "comment-meta/purpose");
      assertGreater(purposeInfos.length, 0, "Should report missing Purpose: in METADATA comment block");
    },
  },

  // --- identity: statics presence checks ---

  {
    code: "CWS-R25-013",
    fixture: "rust/metadata/template-no-statics.rs",
    desc: "warns about missing PRAGMA static",
    rule: "identity/pragma-static",
  },

  {
    code: "CWS-R25-014",
    fixture: "rust/metadata/template-no-statics.rs",
    desc: "warns about missing METADATA static",
    rule: "identity/metadata-static",
  },

  {
    code: "CWS-R25-015",
    fixture: "rust/identity/unparseable-statics.rs",
    desc: "info when PRAGMA static exists but fields can't parse",
    rule: "identity/pragma-parse",
  },

  {
    code: "CWS-R25-016",
    fixture: "rust/identity/unparseable-statics.rs",
    desc: "info when METADATA static exists but fields can't parse",
    rule: "identity/metadata-parse",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "rust", label: "R25", cases: RUST_R25_CASES });

// ---------------------------------------------------------------------------
// Additional tests — schema wiring, doc severity, partial order
// ---------------------------------------------------------------------------

Deno.test("[CWS-R25-003] R25/language/cargo-schema-wiring: cargo/schema resolves to R25-003", async () => {
  const { matchRule, getByCode } = await import("@lib/data/errors.ts");
  const entry = matchRule("cargo/schema");
  assert(entry !== undefined, "cargo/schema should match a catalog entry");
  assert(entry!.code === "CWS-R25-003", `Expected CWS-R25-003, got ${entry!.code}`);
  const direct = getByCode("CWS-R25-003");
  assert(direct !== undefined, "CWS-R25-003 should exist in error catalog");
});

const rust = lazyFormat("rust");

Deno.test("R25/doc/schema-driven-severity: crate root=warn, module=info", async () => {
  const { loadCodeRules } = await import("@lib/foundation/mod.ts");
  const rules = await loadCodeRules("rust");
  const crateExp = rules.docCommentExpectations["crate_root"];
  const moduleExp = rules.docCommentExpectations["module"];
  assertEquals(crateExp?.severity, "warn", "crate_root severity from schema");
  assertEquals(moduleExp?.severity, "info", "module severity from schema");
});

Deno.test("R25/doc/valid-library: no doc warnings", async () => {
  const results = await lintOnce(rust, "rust", "rust/structure/valid-library.rs");
  const docRules = byRule(results, "doc/");
  assertEquals(docRules.length, 0, `Expected 0 doc warnings: ${JSON.stringify(docRules.map(r => r.rule))}`);
});

Deno.test("R25/doc/template-with-doc-comments: no doc/template info", async () => {
  const results = await lintOnce(rust, "rust", "rust/metadata/placeholder-values.rs");
  const templateDoc = byRule(results, "doc/template");
  assertEquals(templateDoc.length, 0, "Template with //! doc comments should not trigger doc/template");
});

Deno.test("R25/setup/subsection-partial-correct: 3 of 10 in correct relative order — zero order warnings", async () => {
  const results = await lintOnce(rust, "rust", "rust/setup/subsection-partial-correct.rs");
  noErrors(results);
  const orderWarns = byRule(results, "setup/subsection-order");
  assertEquals(orderWarns.length, 0, "Partial correct order should produce 0 order warnings");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[25] Rust block-level detection tests — DATA-DRIVEN.
// 19 table-driven cases + 5 bespoke = 26+ test functions.
// Fixture caching: valid-library (4x→1x), template-no-statics (2x→1x),
// wrong-inner-order (2x→1x), unparseable-statics (2x→1x), missing-ic (2x→1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
