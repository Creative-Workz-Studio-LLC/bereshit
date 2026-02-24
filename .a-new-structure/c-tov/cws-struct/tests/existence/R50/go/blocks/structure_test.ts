// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/existence/R50/go/blocks/structure_test.ts
// purpose: Go R[50] file-level detection tests — DATA-DRIVEN.
//          Block presence, order, format detection, template/derived,
//          separator consistency, end markers.
//          Error catalog is the source of truth.
// layer:   R[50] — File-level detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals } from "jsr:@std/assert";
import {
  lazyFormat, errors, warnings,
} from "@test/helpers.ts";
import type { TestCase } from "@test/_runner.ts";
import {
  runSuite, lintOnce, noErrors, noWarnings,
} from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Go R50 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const GO_R50_CASES: TestCase[] = [

  // --- structure: block presence and order ---

  {
    code: "CWS-R50-022",
    fixture: "go/structure/missing-block.go",
    desc: "produces error for missing METADATA block",
    rule: "block",
    messageContains: ["METADATA"],
    cleanFixture: "go/structure/valid-library.go",
    cleanDesc: "valid-library has zero errors",
  },

  {
    code: "CWS-R50-020",
    fixture: "go/structure/wrong-block-order.go",
    desc: "produces error for wrong block order (SETUP before METADATA)",
    rule: "block/order",
    cleanFixture: "go/structure/valid-library.go",
    cleanDesc: "valid-library has correct block order",
  },

  // --- format: format detection ---

  {
    code: "CWS-R50-072",
    fixture: "go/format/no-omni.go",
    desc: "plain .go with no omni pragma — info only, no errors",
    validate: (results) => {
      assertEquals(errors(results).length, 0, "Should produce no errors");
      assertEquals(warnings(results).length, 0, "Should produce no warnings");
    },
  },

  // --- template/derived: classification ---

  {
    code: "CWS-R50-040",
    fixture: "go/metadata/template-with-derived-values.go",
    desc: "Go template missing //go:build ignore",
    rule: "template/build-ignore",
  },

  {
    code: "CWS-R50-041",
    fixture: "go/classify/derived-with-build-ignore.go",
    desc: "derived file has //go:build ignore",
    rule: "derived/build-ignore",
  },

  // --- block: end marker checks ---

  {
    code: "CWS-R50-021",
    fixture: "go/separators/inconsistent-widths.go",
    desc: "detects missing END separators for blocks",
    rule: "block/end-",
  },

  // --- style: separator consistency and standard-width ---

  {
    code: "CWS-R50-030",
    fixture: "go/separators/inconsistent-widths.go",
    desc: "detects inconsistent = separator widths",
    rule: "style/eq-separator-width",
  },

  {
    code: "CWS-R50-031",
    fixture: "go/separators/nonstandard-widths.go",
    desc: "detects non-standard = separator width",
    rule: "style/eq-separator-standard",
  },

  {
    code: "CWS-R50-032",
    fixture: "go/separators/inconsistent-widths.go",
    desc: "detects inconsistent box separator widths",
    rule: "style/box-separator-width",
  },

  {
    code: "CWS-R50-033",
    fixture: "go/separators/nonstandard-widths.go",
    desc: "detects non-standard box separator width",
    rule: "style/box-separator-standard",
  },

  {
    code: "CWS-R50-034",
    fixture: "go/separators/dash-inconsistent.go",
    desc: "detects inconsistent ASCII dash widths",
    rule: "style/dash-separator-width",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "go", label: "R50", cases: GO_R50_CASES });

// ---------------------------------------------------------------------------
// Additional tests — clean validation requiring bespoke assertions
// ---------------------------------------------------------------------------

const go = lazyFormat("go");

Deno.test("R50/go/valid-library: zero errors", async () => {
  const results = await lintOnce(go, "go", "go/structure/valid-library.go");
  noErrors(results);
});

Deno.test("R50/go/valid-library: zero warnings (all separators correct)", async () => {
  const results = await lintOnce(go, "go", "go/structure/valid-library.go");
  noWarnings(results);
});

Deno.test("R50/go/valid-executable: zero errors", async () => {
  const results = await lintOnce(go, "go", "go/structure/valid-executable.go");
  noErrors(results);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[50] Go structure tests — DATA-DRIVEN.
// 11 table-driven cases + 3 bespoke = 16+ test functions.
// Fixture caching: valid-library (2x→1x), inconsistent-widths (3x→1x),
// nonstandard-widths (2x→1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
