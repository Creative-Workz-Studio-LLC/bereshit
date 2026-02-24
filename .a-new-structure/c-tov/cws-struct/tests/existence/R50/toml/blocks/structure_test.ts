// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/existence/R50/toml/blocks/structure_test.ts
// purpose: TOML R[50] file-level detection tests — DATA-DRIVEN.
//          3-block presence, parsing, missing blocks, BOM handling,
//          fix suggestions.
//          Error catalog is the source of truth.
// layer:   R[50] — File-level detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals } from "jsr:@std/assert";
import {
  lazyFormat, errors,
} from "@test/helpers.ts";
import type { TestCase } from "@test/_runner.ts";
import {
  runSuite, lintOnce, noErrors, noWarnings,
} from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// TOML R50 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const TOML_R50_CASES: TestCase[] = [

  // --- structure: 3-block presence ---

  {
    code: "CWS-R50-043",
    fixture: "toml/structure/empty.toml",
    desc: "empty file produces errors for missing structure",
    messageContains: ["metadata"],
    cleanFixture: "toml/structure/valid-complete.toml",
    cleanDesc: "valid-complete has zero errors",
  },

  {
    code: "CWS-R50-043",
    fixture: "toml/structure/missing-metadata.toml",
    desc: "missing _metadata error with fix suggestion",
    messageContains: ["metadata"],
    hasFix: true,
  },

  {
    code: "CWS-R50-044",
    fixture: "toml/structure/missing-content.toml",
    desc: "warns about missing _content table",
    messageContains: ["content"],
    validate: (results) => {
      const parseErrs = errors(results).filter((r) => r.rule.includes("parse"));
      assertEquals(parseErrs.length, 0, "File should parse without errors");
    },
  },

  {
    code: "CWS-R50-045",
    fixture: "toml/structure/missing-closing.toml",
    desc: "warns about missing _closing table",
    messageContains: ["closing"],
    validate: (results) => {
      const parseErrs = errors(results).filter((r) => r.rule.includes("parse"));
      assertEquals(parseErrs.length, 0, "File should parse without errors");
    },
  },

  // --- parse: TOML parsing ---

  {
    code: "CWS-R50-002",
    fixture: "toml/structure/parse-error.toml",
    desc: "reports parse error without crashing",
    cleanFixture: "toml/structure/valid-complete.toml",
    cleanDesc: "valid-complete has no parse errors",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "toml", label: "R50", cases: TOML_R50_CASES });

// ---------------------------------------------------------------------------
// Additional tests — clean validation requiring bespoke assertions
// ---------------------------------------------------------------------------

const toml = lazyFormat("toml");

Deno.test("R50/toml/valid-complete: zero errors", async () => {
  const results = await lintOnce(toml, "toml", "toml/structure/valid-complete.toml");
  noErrors(results);
});

Deno.test("R50/toml/valid-complete: zero warnings", async () => {
  const results = await lintOnce(toml, "toml", "toml/structure/valid-complete.toml");
  noWarnings(results);
});

Deno.test("R50/toml/bom-utf8: parses correctly with BOM stripping", async () => {
  const results = await lintOnce(toml, "toml", "toml/structure/bom-utf8.toml");
  const parseErrors = errors(results).filter((r) => r.rule.includes("parse"));
  assertEquals(parseErrors.length, 0, "BOM file should parse without errors after BOM stripping");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[50] TOML structure tests — DATA-DRIVEN.
// 5 table-driven cases + 3 bespoke = 10+ test functions.
// Fixture caching: valid-complete (2x→1x), missing-content (inline),
// missing-closing (inline).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
