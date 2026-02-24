// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/content/R10/go/content/content_test.ts
// purpose: R10 (content/placement) detection tests for Go — DATA-DRIVEN.
//          Each test case is a row in a table. The error catalog is the
//          source of truth for severity and rule patterns.
//          Add a row → test exists. No more boilerplate.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals } from "jsr:@std/assert";
import { fixture, lazyFormat, infos } from "@test/helpers.ts";
import type { R10TestCase } from "../../_r10.ts";
import {
  runR10Suite,
  noErrors, noPlacementWarnings, hasIdentityRegisterInfo,
} from "../../_r10.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Go R10 Test Specification — THE TABLE
// ---------------------------------------------------------------------------
//
// Each row maps: error code → fixture → expected behavior.
// Severity comes from the error catalog unless overridden.
// This IS the debugger in disguise — navigate the table to navigate the system.
// ---------------------------------------------------------------------------

const GO_R10_CASES: R10TestCase[] = [

  // --- placement: content in wrong blocks ---

  {
    code: "CWS-R10-002",
    fixture: "go/structure/content-placement-wrong.go",
    desc: "detects func in SETUP and import in BODY",
    rule: "content/block-placement",
    messageContains: ["func_decl", "import_decl"],
    cleanFixture: "go/structure/content-placement-correct.go",
    cleanDesc: "zero placement warnings",
    validate: noErrors,
  },

  {
    code: "CWS-R10-001",
    fixture: "go/metadata/metadata-leak.go",
    desc: "detects code declarations in METADATA block",
    rule: "content/metadata-leak",
  },

  // --- identity: field presence and registration ---

  {
    code: "CWS-R10-011",
    fixture: "go/metadata/missing-ic-fields.go",
    desc: "warns about missing required I/C fields",
    minCount: 7,
    validate: (results) => {
      // Verify zero errors (missing fields are warnings, not errors)
      noErrors(results);
      // Verify specific fields detected — these come from the schema
      for (const field of ["I1.key", "I1.from", "I2.type", "C1.version", "C2.organization", "C3.scripture", "C4.consumers"]) {
        const has = results.some((r) => r.rule.includes(field));
        assertEquals(has, true, `Should warn about missing ${field}`);
      }
    },
  },

  {
    code: "CWS-R10-012",
    fixture: "go/metadata/placeholder-values.go",
    desc: "warns about template placeholders",
  },

  {
    code: "CWS-R10-010",
    fixture: "go/metadata/identity-registration.go",
    desc: "info when Pragma exists but no init()",
    rule: "identity/register",
    validate: hasIdentityRegisterInfo,
  },

  // --- subsection: content within SETUP subsections ---

  {
    code: "CWS-R10-003",
    fixture: "go/setup/misplaced-content.go",
    desc: "detects misplaced declarations within SETUP subsections",
    rule: "content/subsection-placement",
    messageContains: ["CoreTypes", "Constants"],
    hasFix: true,
    fixActions: ["remove", "insert"],
  },

  // --- closing: test/main placement ---

  {
    code: "CWS-R10-004",
    fixture: "go/closing/tests-in-body.go",
    desc: "detects Test func in BODY — should be CLOSING Cv",
    rule: "closing/test-placement",
    validate: noErrors,
  },

  {
    code: "CWS-R10-005",
    fixture: "go/closing/main-in-body.go",
    desc: "detects func main() in BODY — should be CLOSING Ce",
    rule: "closing/main-placement",
    validate: noErrors,
  },

  // --- closing: X6 template-only ---

  {
    code: "CWS-R10-006",
    fixture: "go/closing/x6-in-derived.go",
    desc: "detects X6 section in non-template file",
    rule: "closing/X6-template-only",
    messageContains: ["Template Guide"],
    cleanFixture: "go/structure/valid-library.go",
    cleanDesc: "valid-library has no X6 warning",
    validate: noErrors,
  },

  // --- emphasis: subtype emphasis balance ---

  {
    code: "CWS-R10-040",
    fixture: "go/setup/emphasis-inverted.go",
    desc: "library with empty heavy subs and full light subs",
    rule: "emphasis/setup-inverted",
    messageContains: ["library"],
    cleanFixture: "go/structure/valid-library.go",
    cleanDesc: "valid library has no emphasis inversion",
    validate: noErrors,
  },
];

// ---------------------------------------------------------------------------
// Run the suite — generates all Deno.test() calls from the table
// ---------------------------------------------------------------------------

runR10Suite({ format: "go", cases: GO_R10_CASES });

// ---------------------------------------------------------------------------
// Additional tests — cases that need bespoke assertions beyond the table
// ---------------------------------------------------------------------------

const go = lazyFormat("go");

Deno.test("R10/structure/content-placement-correct: zero errors and zero placement warnings", async () => {
  const results = await go.lint(fixture("go/structure/content-placement-correct.go"));
  noErrors(results);
  noPlacementWarnings(results);
});

Deno.test("R10/metadata/valid-library: identity registration info (has Pragma, no init())", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const regInfos = infos(results).filter((r) => r.rule.includes("identity/register"));
  assertEquals(regInfos.length, 1, "Valid library with Pragma but no init() should get 1 identity/register info");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R10 Go content/placement tests — DATA-DRIVEN.
// 10 table-driven cases + 2 bespoke tests = 22 test functions
// (each case generates primary + optional clean counterpart).
//
// The table IS the specification. The error catalog IS the authority.
// Add a code → add a row → test exists → debugger grows.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
