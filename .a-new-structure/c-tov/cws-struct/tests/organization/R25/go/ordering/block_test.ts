// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/organization/R25/go/ordering/block_test.ts
// purpose: Go R[25] block-level detection tests — DATA-DRIVEN.
//          Doc comments, subsection order, required subsections,
//          closing zones, identity statics, language checks.
//          Error catalog is the source of truth.
// layer:   R[25] — Block-level detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  lazyFormat, errors, byRule, hasMessage,
} from "@test/helpers.ts";
import type { TestCase } from "@test/_runner.ts";
import {
  runSuite, lintOnce, noErrors, noDocWarnings, noClosingWarnings,
} from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Go R25 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const GO_R25_CASES: TestCase[] = [

  // --- doc: doc comment quality checks (schema-driven) ---

  {
    code: "CWS-R05-001",
    fixture: "go/setup/no-header-doc.go",
    desc: "warns about missing package doc comment",
    rule: "doc/package",
  },

  {
    code: "CWS-R05-002",
    fixture: "go/setup/no-header-doc.go",
    desc: "warns about missing Pragma doc comment",
    rule: "doc/pragma-var",
  },

  {
    code: "CWS-R05-003",
    fixture: "go/setup/no-header-doc.go",
    desc: "warns about missing Metadata doc comment",
    rule: "doc/metadata-var",
  },

  // --- setup: subsection order ---

  {
    code: "CWS-R25-023",
    fixture: "go/setup/subsection-order-wrong.go",
    desc: "detects Core Types before Constants (wrong dependency order)",
    rule: "setup/subsection-order",
    cleanFixture: "go/setup/subsection-order-correct.go",
    cleanDesc: "all subsections in correct order — zero order warnings",
  },

  {
    code: "CWS-R10-043",
    fixture: "go/setup/aliased-subsection-names.go",
    desc: "detects alias names and suggests canonical alternatives",
    rule: "setup/alias-name",
    validate: (results) => {
      const orderWarns = byRule(results, "setup/subsection-order");
      assertEquals(orderWarns.length, 0,
        "Aliased names in correct order should produce 0 order warnings");
    },
  },

  // --- setup/required: required SETUP subsections ---

  {
    code: "CWS-R25-030",
    fixture: "go/setup/missing-required-subsections.go",
    desc: "library missing ErrorTypes and CoreTypes",
    rule: "setup/required-subsection",
    minCount: 2,
    validate: (results) => {
      const required = byRule(results, "setup/required-subsection");
      assertEquals(required.length, 2, "Should report exactly 2 missing required subsections");
      assert(hasMessage(required, "Error Types") || hasMessage(required, "Core Types"),
        "Should mention at least one missing required subsection");
    },
  },

  // --- body: subsection order and required ---

  {
    code: "CWS-R25-024",
    fixture: "go/body/wrong-body-order.go",
    desc: "detects BODY subsections out of numeric order",
    rule: "body/subsection-order",
    messageContains: ["§2"],
    validate: noErrors,
    cleanFixture: "go/structure/valid-library.go",
    cleanDesc: "valid-library has zero body order warnings",
  },

  {
    code: "CWS-R25-031",
    fixture: "go/body/missing-required-subsections.go",
    desc: "library missing all 7 required body subsections",
    rule: "body/required-subsection",
    minCount: 7,
  },

  // --- closing: zone order and required zones ---

  {
    code: "CWS-R25-020",
    fixture: "go/closing/wrong-closing-order.go",
    desc: "detects code zone after documentation section",
    rule: "closing/zone-order",
    validate: noErrors,
    cleanFixture: "go/structure/valid-library.go",
    cleanDesc: "valid-library has zero closing zone warnings",
  },

  {
    code: "CWS-R25-032",
    fixture: "go/closing/missing-required-zones.go",
    desc: "missing X1 and X5 documentation zones triggers info",
    validate: (results) => {
      assertGreater(byRule(results, "closing/required-X1").length, 0, "Should detect missing X1 zone");
      assertGreater(byRule(results, "closing/required-X5").length, 0, "Should detect missing X5 zone");
    },
  },

  // --- language: Go-specific checks ---

  {
    code: "CWS-R25-002",
    fixture: "go/metadata/placeholder-values.go",
    desc: "detects missing import statement",
    rule: "go/import",
  },

  // --- identity: PRAGMA and METADATA structure ---

  {
    code: "CWS-R25-010",
    fixture: "go/identity/empty-pragma-metadata.go",
    desc: "warns when Pragma var is declared but empty",
    rule: "identity/Pragma/empty",
  },

  {
    code: "CWS-R25-011",
    fixture: "go/identity/empty-pragma-metadata.go",
    desc: "warns when Metadata var is declared but empty",
    rule: "identity/Metadata/empty",
  },

  {
    code: "CWS-R25-012",
    fixture: "go/identity/legacy-identity.go",
    desc: "detects old map-style identity and recommends upgrade",
    rule: "identity/upgrade",
  },

  // --- language: wiring verification ---

  {
    code: "CWS-R25-001",
    fixture: "go/structure/valid-library.go",
    desc: "go/package rule resolves to R25-001",
    skip: "wiring-only — tested via error catalog import below",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "go", label: "R25", cases: GO_R25_CASES });

// ---------------------------------------------------------------------------
// Additional tests — schema wiring, doc severity, clean validation
// ---------------------------------------------------------------------------

Deno.test("[CWS-R25-001] R25/language/go-package-wiring: go/package resolves to R25-001", async () => {
  const { matchRule, getByCode } = await import("@lib/data/errors.ts");
  const entry = matchRule("go/package");
  assert(entry !== undefined, "go/package should match a catalog entry");
  assert(entry!.code === "CWS-R25-001", `Expected CWS-R25-001, got ${entry!.code}`);
  const direct = getByCode("CWS-R25-001");
  assert(direct !== undefined, "CWS-R25-001 should exist in error catalog");
});

const go = lazyFormat("go");

Deno.test("R25/doc/schema-driven-severity: Go doc comment expectations loaded from schema", async () => {
  const { loadCodeRules } = await import("@lib/foundation/mod.ts");
  const rules = await loadCodeRules("go");
  const pkgDoc = rules.docCommentExpectations["package_doc"];
  const pragmaDoc = rules.docCommentExpectations["pragma_doc"];
  const metaDoc = rules.docCommentExpectations["metadata_doc"];
  assertEquals(pkgDoc?.severity, "warn", "package_doc severity from schema");
  assertEquals(pragmaDoc?.severity, "warn", "pragma_doc severity from schema");
  assertEquals(metaDoc?.severity, "warn", "metadata_doc severity from schema");
});

Deno.test("R25/doc/valid-library: no doc warnings", async () => {
  const results = await lintOnce(go, "go", "go/structure/valid-library.go");
  noDocWarnings(results);
});

Deno.test("R25/closing/valid-library: no errors, no closing warnings", async () => {
  const results = await lintOnce(go, "go", "go/structure/valid-library.go");
  noErrors(results);
  noClosingWarnings(results);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[25] Go block-level detection tests — DATA-DRIVEN.
// 15 table-driven cases (1 skipped) + 4 bespoke = 21+ test functions.
// Fixture caching: valid-library (4x→1x), empty-pragma-metadata (2x→1x),
// missing-required-subsections (2x→1x), no-header-doc (3x→1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
