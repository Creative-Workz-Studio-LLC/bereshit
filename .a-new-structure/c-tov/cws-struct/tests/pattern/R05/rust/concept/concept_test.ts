// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/pattern/R05/rust/concept/concept_test.ts
// purpose: Rust R[5] concept/detail tests — DATA-DRIVEN.
//          Field content values, header docs, subtype subsections, typing
//          arrows, closing zone content/depth, form-aware validation,
//          content expectations, concept detection, template checks,
//          and ternary policy severity thresholding.
//          Error catalog is the source of truth.
// layer:   R[5] — Concept/detail detection
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
  runSuite, lintOnce, noErrors, noValueWarnings, noDepthWarnings,
} from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Rust R05 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const RUST_R05_CASES: TestCase[] = [

  // --- metadata: field content value checks ---

  {
    code: "CWS-R05-080",
    fixture: "rust/metadata/bad-content-values.rs",
    desc: "detects invalid PRAGMA field values (I1.key, I1.format, I1.at, I2.type, I2.structure, I3.file, I3.title)",
    validate: (results) => {
      const rules = [
        "value/PRAGMA/I1.key", "value/PRAGMA/I1.format", "value/PRAGMA/I1.at",
        "value/PRAGMA/I2.type", "value/PRAGMA/I2.structure",
        "value/PRAGMA/I3.file", "value/PRAGMA/I3.title",
      ];
      for (const r of rules) {
        assert(hasRule(results, r), `Should catch ${r}`);
      }
      const pragmaWarns = warnings(results).filter((r) => r.rule.startsWith("value/PRAGMA/"));
      assertGreater(pragmaWarns.length, 6, `Expected 7+ PRAGMA content warnings, got ${pragmaWarns.length}`);
    },
  },

  {
    code: "CWS-R05-080",
    fixture: "rust/metadata/bad-content-values.rs",
    desc: "detects invalid METADATA field values (C1.version, C1.status, C2.organization)",
    validate: (results) => {
      assert(hasRule(results, "value/METADATA/C1.version"), "Should catch bad version");
      assert(hasRule(results, "value/METADATA/C1.status"), "Should catch unknown status");
      assert(hasRule(results, "value/METADATA/C2.organization"), "Should catch empty organization");
    },
  },

  {
    code: "CWS-R05-080",
    fixture: "rust/metadata/bad-content-values.rs",
    desc: "info-level checks for dates and paths",
    validate: (results) => {
      const contentInfos = infos(results).filter((r) =>
        r.rule.startsWith("value/PRAGMA/") || r.rule.startsWith("value/METADATA/"));
      assertGreater(contentInfos.length, 3, `Expected 4+ content info results, got ${contentInfos.length}`);
      assert(hasRule(results, "value/PRAGMA/I1.from"), "Should flag path check on I1.from");
      assert(hasRule(results, "value/METADATA/C1.created"), "Should flag date format on C1.created");
      assert(hasRule(results, "value/METADATA/C1.updated"), "Should flag date format on C1.updated");
      assert(hasRule(results, "value/METADATA/C3.scripture"), "Should flag empty scripture");
      for (const r of contentInfos) {
        assertEquals(r.severity, "info", `${r.rule} should be info, got ${r.severity}`);
      }
    },
  },

  {
    code: "CWS-R05-080",
    fixture: "rust/metadata/bad-content-values.rs",
    desc: "total of 14 content check results",
    validate: (results) => {
      const contentResults = results.filter((r) =>
        r.rule.startsWith("value/PRAGMA/") || r.rule.startsWith("value/METADATA/"));
      assertEquals(contentResults.length, 14,
        `Expected 14 total (8 PRAGMA + 6 METADATA), got ${contentResults.length}: ${JSON.stringify(contentResults.map(r => r.rule))}`);
    },
    cleanFixture: "rust/structure/valid-library.rs",
    cleanDesc: "valid-library has zero content value warnings",
  },

  // --- setup: header documentation ---

  {
    code: "CWS-R05-007",
    fixture: "rust/setup/no-header-doc.rs",
    desc: "no documentation before first subsection triggers info",
    rule: "setup/header-doc",
    messageContains: ["header documentation"],
    cleanFixture: "rust/setup/good-header-doc.rs",
    cleanDesc: "good documentation is clean",
  },

  // --- doc: template doc comment checks ---

  {
    code: "CWS-R05-004",
    fixture: "rust/metadata/template-no-doc-comment.rs",
    desc: "template without //! doc comment section",
    rule: "doc/template",
  },

  // --- comment-meta: biblical and version ---

  {
    code: "CWS-R05-005",
    fixture: "rust/metadata/unknown-subtype.rs",
    desc: "detects missing Scripture reference in METADATA comments",
    rule: "comment-meta/biblical",
  },

  {
    code: "CWS-R05-006",
    fixture: "rust/metadata/unknown-subtype.rs",
    desc: "detects missing Version in METADATA comments",
    rule: "comment-meta/version",
  },

  // --- body: subtype subsections ---

  {
    code: "CWS-R05-008",
    fixture: "rust/body/wrong-subtype-subsections.rs",
    desc: "wrong subsection names for library triggers info",
    rule: "body/subtype-subsections",
    messageContains: ["library"],
  },

  // --- closing: zone content and depth ---

  {
    code: "CWS-R05-030",
    fixture: "rust/closing/x1-missing-fields.rs",
    desc: "X1 zone missing required fields (Never, Careful, Safe) triggers info",
    rule: "closing/X1-content",
    messageContains: ["never"],
  },

  {
    code: "CWS-R05-030",
    fixture: "rust/closing/x5-missing-scripture.rs",
    desc: "X5 zone missing scripture field triggers info",
    rule: "closing/X5-content",
    messageContains: ["scripture"],
  },

  {
    code: "CWS-R05-031",
    fixture: "rust/closing/x1-empty-fields.rs",
    desc: "detects empty and placeholder field values in X1",
    rule: "closing/X1-depth",
    validate: noErrors,
    cleanFixture: "rust/structure/valid-library.rs",
    cleanDesc: "valid-library has no depth warnings",
  },

  {
    code: "CWS-R05-032",
    fixture: "rust/closing/reserved-zone-present.rs",
    desc: "detects reserved X6 zone in library CLOSING",
    rule: "closing-reserve",
    messageContains: ["X6"],
  },

  // --- template: usable schema checks ---

  {
    code: "CWS-R05-010",
    fixture: "rust/metadata/template-no-statics.rs",
    desc: "template missing PRAGMA static",
    rule: "template/pragma-static",
  },

  {
    code: "CWS-R05-011",
    fixture: "rust/metadata/template-no-statics.rs",
    desc: "template missing METADATA static",
    rule: "template/metadata-static",
  },

  // --- content/body: body content expectations ---

  {
    code: "CWS-R05-020",
    fixture: "rust/content/body-section-violations.rs",
    desc: "missing required BODY sections",
    rule: "content/body-required/",
    messageContains: ["IdentityAccess"],
  },

  {
    code: "CWS-R05-021",
    fixture: "rust/content/body-section-violations.rs",
    desc: "reserved BODY section with code",
    rule: "content/body-reserve/",
    messageContains: ["Helpers"],
  },

  // --- concept: detection ---

  {
    code: "CWS-R05-070",
    fixture: "rust/content/setup-section-violations.rs",
    desc: "denied concept detected in container",
    validate: (results) => {
      const denied = byRule(results, "concept/").filter((r) => r.severity === "warn");
      assertGreater(denied.length, 0, "Should detect denied concepts");
      assert(hasMessage(denied, "Denied concept") || hasMessage(denied, "denied"),
        "Should indicate concept is denied");
    },
  },

  {
    code: "CWS-R05-071",
    fixture: "rust/content/body-section-violations.rs",
    desc: "expected concept not found in container",
    validate: (results) => {
      const missing = byRule(results, "concept/").filter((r) =>
        r.severity === "info" && r.rule.includes("/missing"));
      assertGreater(missing.length, 0, "Should have missing expected concepts");
      assert(hasMessage(missing, "Expected concept") || hasMessage(missing, "not found"),
        "Should indicate expected concept is missing");
    },
  },

  // --- content/setup: setup content expectations ---

  {
    code: "CWS-R05-040",
    fixture: "rust/content/setup-section-violations.rs",
    desc: "missing import declarations",
    rule: "content/setup-section/Imports",
    messageContains: ["no import"],
  },

  {
    code: "CWS-R05-041",
    fixture: "rust/content/setup-section-violations.rs",
    desc: "reserved SETUP section with code",
    rule: "content/setup-reserve/",
    messageContains: ["Modules"],
  },

  // --- form: form-aware validation (ignored until Rust form schemas created) ---

  {
    code: "CWS-R05-070",
    fixture: "rust/form/module-has-reserved-section.rs",
    desc: "detects S2 Modules in non-template module",
    rule: "form/reserved-section-present",
    skip: "requires form constraint schemas in form_identity/containers format",
  },

  {
    code: "CWS-R05-071",
    fixture: "rust/form/module-missing-required.rs",
    desc: "detects absent S1 Imports",
    rule: "form/required-section-missing",
    skip: "requires form constraint schemas in form_identity/containers format",
  },

  {
    code: "CWS-R05-070",
    fixture: "rust/form/template-with-reserved.rs",
    desc: "reserved checks fire on templates (templates are live)",
    rule: "form/reserved-section-present",
    skip: "requires form constraint schemas in form_identity/containers format",
  },

  // --- policy: ternary policy severity thresholding (ignored) ---

  {
    code: "CWS-R05-071",
    fixture: "rust/form/module-missing-required.rs",
    desc: "growth policy: required-missing becomes info",
    skip: "requires form constraint schemas in form_identity/containers format",
  },

  {
    code: "CWS-R05-071",
    fixture: "rust/form/module-missing-required.rs",
    desc: "strict policy: required-missing becomes error",
    skip: "requires form constraint schemas in form_identity/containers format",
  },

  {
    code: "CWS-R05-071",
    fixture: "rust/form/module-missing-required.rs",
    desc: "balanced policy: required-missing stays warn",
    skip: "requires form constraint schemas in form_identity/containers format",
  },

  {
    code: "CWS-R05-070",
    fixture: "rust/form/module-has-reserved-section.rs",
    desc: "growth policy: reserved-present becomes info",
    skip: "requires form constraint schemas in form_identity/containers format",
  },

  {
    code: "CWS-R05-070",
    fixture: "rust/form/module-has-reserved-section.rs",
    desc: "strict policy: reserved-present becomes error",
    skip: "requires form constraint schemas in form_identity/containers format",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "rust", label: "R05", cases: RUST_R05_CASES });

// ---------------------------------------------------------------------------
// Additional tests — bespoke assertions beyond the table
// ---------------------------------------------------------------------------

const rust = lazyFormat("rust");

Deno.test("R05/metadata/placeholder-values: content checks skip placeholders", async () => {
  const results = await lintOnce(rust, "rust", "rust/metadata/placeholder-values.rs");
  noValueWarnings(results);
});

Deno.test("R05/body/subtype-subsections: RO-acknowledged absent subsections are not reported", async () => {
  const results = await lintOnce(rust, "rust", "rust/body/ro-acknowledged-subsections.rs");
  const subtypeSubs = byRule(results, "body/subtype-subsections");
  assertEquals(subtypeSubs.length, 0,
    "Subsections acknowledged in Reserved Omission should not be reported");
});

Deno.test("R05/body/typing-arrow: module->utility narrows canonical — irrelevant sections not flagged", async () => {
  const results = await lintOnce(rust, "rust", "rust/body/typing-arrow-utility.rs");
  const subtypeSubs = byRule(results, "body/subtype-subsections");
  const irrelevant = ["trait implementations", "constructors", "queries"];
  const flaggedIrrelevant = subtypeSubs.filter((r) =>
    irrelevant.some((s) => r.message.toLowerCase().includes(s)));
  assertEquals(flaggedIrrelevant.length, 0,
    "module->utility should NOT flag irrelevant sections");

  const requiredBody = byRule(results, "body/required-subsection");
  const irrelevantRequired = requiredBody.filter((r) =>
    irrelevant.some((s) => r.message.toLowerCase().includes(s)));
  assertEquals(irrelevantRequired.length, 0,
    "module->utility should NOT warn about irrelevant sections as required");

  const available = ["identity access", "core logic"];
  const availableRequired = requiredBody.filter((r) =>
    available.some((s) => r.message.toLowerCase().includes(s)));
  assertEquals(availableRequired.length, 0,
    "module->utility should NOT warn about available (optional) sections as required");
});

Deno.test("R05/body/typing-arrow: module->service with all 6 sections — clean", async () => {
  const results = await lintOnce(rust, "rust", "rust/body/typing-arrow-service.rs");
  assertEquals(byRule(results, "body/subtype-subsections").length, 0,
    "module->service with all 6 sections should be clean");
  assertEquals(byRule(results, "body/required-subsection").length, 0,
    "All required sections present should have no required warnings");
});

Deno.test("R05/form/template-required: required checks SKIP templates", async () => {
  const results = await lintOnce(rust, "rust", "rust/form/template-with-reserved.rs");
  const missing = byRule(results, "form/required-section-missing");
  assertEquals(missing.length, 0, "Templates should not trigger required-section-missing");
});

Deno.test("R05/form/valid-module: no reserved violations", async () => {
  const results = await lintOnce(rust, "rust", "rust/structure/valid-module.rs");
  assertEquals(byRule(results, "form/reserved-section-present").length, 0,
    "Valid module should have no reserved section violations");
});

Deno.test("R05/form/bare-bone-default: files without subtype get bare-bone form", async () => {
  const results = await lintOnce(rust, "rust", "rust/format/no-omni.rs");
  assertEquals(byRule(results, "form/reserved-section-present").length, 0,
    "Minimal file should have no reserved violations");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[5] Rust concept/detail tests — DATA-DRIVEN.
// 29 table-driven cases (8 skipped) + 7 bespoke = 36+ test functions.
// Fixture caching: bad-content-values (4x→1x), template-no-statics (2x→1x),
// unknown-subtype (2x→1x), body-section-violations (2x→1x),
// setup-section-violations (2x→1x), valid-library (2x→1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
