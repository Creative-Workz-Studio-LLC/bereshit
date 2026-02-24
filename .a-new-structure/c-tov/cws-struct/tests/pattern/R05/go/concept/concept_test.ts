// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/pattern/R05/go/concept/concept_test.ts
// purpose: Go R[5] concept/detail tests — DATA-DRIVEN.
//          Field content values, content-aware pragma checks,
//          closing zone content/depth, setup header docs, body subtype
//          subsection names.
//          Error catalog is the source of truth.
// layer:   R[5] — Concept/detail detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  lazyFormat, errors, warnings, infos, byRule, hasRule, hasMessage,
} from "@test/helpers.ts";
import type { TestCase } from "@test/_runner.ts";
import {
  runSuite, lintOnce, noErrors, noValueWarnings,
} from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Go R05 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const GO_R05_CASES: TestCase[] = [

  // --- metadata: field content value checks ---

  {
    code: "CWS-R05-080",
    fixture: "go/metadata/bad-content-values.go",
    desc: "detects invalid Pragma field values (I1.key, I1.format, I1.at, I2.type, I2.structure, I3.file, I3.title)",
    validate: (results) => {
      const rules = [
        "value/Pragma/I1.key", "value/Pragma/I1.format", "value/Pragma/I1.at",
        "value/Pragma/I2.type", "value/Pragma/I2.structure",
        "value/Pragma/I3.file", "value/Pragma/I3.title",
      ];
      for (const r of rules) {
        assert(hasRule(results, r), `Should catch ${r}`);
      }
      const pragmaWarns = warnings(results).filter((r) => r.rule.startsWith("value/Pragma/"));
      assertGreater(pragmaWarns.length, 6, `Expected 7+ Pragma content warnings, got ${pragmaWarns.length}`);
    },
  },

  {
    code: "CWS-R05-080",
    fixture: "go/metadata/bad-content-values.go",
    desc: "detects invalid Metadata field values (C1.version, C1.status, C2.organization)",
    validate: (results) => {
      assert(hasRule(results, "value/Metadata/C1.version"), "Should catch bad version");
      assert(hasRule(results, "value/Metadata/C1.status"), "Should catch unknown status");
      assert(hasRule(results, "value/Metadata/C2.organization"), "Should catch empty organization");
    },
  },

  {
    code: "CWS-R05-080",
    fixture: "go/metadata/bad-content-values.go",
    desc: "info-level checks for dates and paths",
    validate: (results) => {
      const contentInfos = infos(results).filter((r) =>
        r.rule.startsWith("value/Pragma/") || r.rule.startsWith("value/Metadata/"));
      assertGreater(contentInfos.length, 3, `Expected 4+ content info results, got ${contentInfos.length}`);
      assert(hasRule(results, "value/Pragma/I1.from"), "Should flag path check on I1.from");
      assert(hasRule(results, "value/Metadata/C1.created"), "Should flag date format on C1.created");
      assert(hasRule(results, "value/Metadata/C1.updated"), "Should flag date format on C1.updated");
      assert(hasRule(results, "value/Metadata/C3.scripture"), "Should flag empty scripture");
      for (const r of contentInfos) {
        assertEquals(r.severity, "info", `${r.rule} should be info, got ${r.severity}`);
      }
    },
  },

  {
    code: "CWS-R05-080",
    fixture: "go/metadata/bad-content-values.go",
    desc: "total of 14 content check results",
    validate: (results) => {
      const contentResults = results.filter((r) =>
        r.rule.startsWith("value/Pragma/") || r.rule.startsWith("value/Metadata/"));
      assertEquals(contentResults.length, 14,
        `Expected 14 total (8 Pragma + 6 Metadata), got ${contentResults.length}: ${JSON.stringify(contentResults.map(r => r.rule))}`);
    },
    cleanFixture: "go/structure/valid-library.go",
    cleanDesc: "valid-library has zero content value warnings",
  },

  // --- metadata: content-aware validation (pragma-driven) ---

  {
    code: "CWS-R10-030",
    fixture: "go/metadata/template-with-derived-values.go",
    desc: "detects wrong I1.at and C1.status for template",
    rule: "template-at",
    validate: (results) => {
      assertGreater(byRule(results, "content/Pragma/template-at").length, 0,
        "Should trigger content/Pragma/template-at");
      assertGreater(byRule(results, "content/Metadata/template-status").length, 0,
        "Should trigger content/Metadata/template-status");
    },
  },

  {
    code: "CWS-R10-032",
    fixture: "go/metadata/derived-with-template-values.go",
    desc: "detects template values in derived file",
    rule: "derived-at",
    validate: (results) => {
      assertGreater(byRule(results, "content/Pragma/derived-at").length, 0,
        "Should trigger content/Pragma/derived-at");
      assertGreater(byRule(results, "content/Metadata/derived-status").length, 0,
        "Should trigger content/Metadata/derived-status");
    },
  },

  {
    code: "CWS-R10-034",
    fixture: "go/metadata/wrong-subtype.go",
    desc: "detects I2.subtype mismatch with pragma",
    rule: "content/Pragma/subtype-mismatch",
    cleanFixture: "go/structure/valid-library.go",
    cleanDesc: "valid-library has no content-aware false positives",
  },

  // --- closing: zone content and depth ---

  {
    code: "CWS-R05-030",
    fixture: "go/closing/x1-missing-fields.go",
    desc: "X1 zone missing required fields (Careful) triggers info",
    rule: "closing/X1-content",
    messageContains: ["careful"],
  },

  {
    code: "CWS-R05-030",
    fixture: "go/closing/x5-missing-scripture.go",
    desc: "X5 zone missing scripture field triggers info",
    rule: "closing/X5-content",
    messageContains: ["scripture"],
  },

  {
    code: "CWS-R05-031",
    fixture: "go/closing/x1-empty-fields.go",
    desc: "detects empty and placeholder field values in X1",
    rule: "closing/X1-depth",
    validate: noErrors,
    cleanFixture: "go/structure/valid-library.go",
    cleanDesc: "valid-library has no depth warnings",
  },

  // --- setup: header documentation ---

  {
    code: "CWS-R05-007",
    fixture: "go/setup/no-header-doc.go",
    desc: "no documentation before first subsection triggers info",
    rule: "setup/header-doc",
  },

  // --- body: subtype subsection names ---

  {
    code: "CWS-R05-008",
    fixture: "go/body/wrong-subtype-subsections.go",
    desc: "library with demo-test subsection names triggers info",
    rule: "body/subtype-subsections",
    messageContains: ["Public APIs"],
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "go", label: "R05", cases: GO_R05_CASES });

// ---------------------------------------------------------------------------
// Additional tests — bespoke assertions beyond the table
// ---------------------------------------------------------------------------

const go = lazyFormat("go");

Deno.test("R05/metadata/placeholder-values: content checks skip placeholders", async () => {
  const results = await lintOnce(go, "go", "go/metadata/placeholder-values.go");
  noValueWarnings(results);
});

Deno.test("R05/metadata/valid-library: no content-aware false positives", async () => {
  const results = await lintOnce(go, "go", "go/structure/valid-library.go");
  const contentPragma = byRule(results, "content/Pragma/");
  const contentMeta = byRule(results, "content/Metadata/");
  assertEquals(contentPragma.length, 0,
    `Valid library should have 0 content/Pragma infos: ${JSON.stringify(contentPragma.map(r => r.rule))}`);
  assertEquals(contentMeta.length, 0,
    `Valid library should have 0 content/Metadata infos: ${JSON.stringify(contentMeta.map(r => r.rule))}`);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[5] Go concept/detail tests — DATA-DRIVEN.
// 12 table-driven cases + 2 bespoke = 16+ test functions.
// Fixture caching: bad-content-values (4x→1x), valid-library (3x→1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
