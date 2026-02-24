// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/existence/R50/go/directives/directive_test.ts
// purpose: Go R[50] directive detection tests — DATA-DRIVEN.
//          Unrecognized code/template formats, missing required/recommended
//          directives, meta.key wiring.
//          Error catalog is the source of truth.
// layer:   R[50] — File-level detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assert } from "jsr:@std/assert";
import type { TestCase } from "@test/_runner.ts";
import { runSuite } from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Go R50 Directive Specification — THE TABLE
// ---------------------------------------------------------------------------

const GO_R50_DIRECTIVE_CASES: TestCase[] = [

  // --- directive: format recognition ---

  {
    code: "CWS-R50-013",
    fixture: "go/directives/unknown-code-format.go",
    desc: "info for unrecognized //omni:code value",
  },

  {
    code: "CWS-R50-014",
    fixture: "go/directives/unknown-template-format.go",
    desc: "info for unrecognized template format",
  },

  // --- directive: required and recommended ---

  {
    code: "CWS-R50-011",
    fixture: "go/directives/missing-directives.go",
    desc: "error for missing //omni:key",
  },

  {
    code: "CWS-R50-012",
    fixture: "go/directives/missing-directives.go",
    desc: "warn for missing recommended directives",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "go", label: "R50", cases: GO_R50_DIRECTIVE_CASES });

// ---------------------------------------------------------------------------
// Additional tests — wiring verification
// ---------------------------------------------------------------------------

Deno.test("[CWS-R50-010] R50/directive/meta-key-wiring: directive/meta.key resolves to R50-010", async () => {
  const { matchRule, getByCode } = await import("@lib/data/errors.ts");
  const entry = matchRule("directive/meta.key");
  assert(entry !== undefined, "directive/meta.key should match a catalog entry");
  assert(entry!.code === "CWS-R50-010", `Expected CWS-R50-010, got ${entry!.code}`);
  const direct = getByCode("CWS-R50-010");
  assert(direct !== undefined, "CWS-R50-010 should exist in error catalog");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[50] Go directive tests — DATA-DRIVEN.
// 4 table-driven cases + 1 bespoke = 7+ test functions.
// Fixture caching: missing-directives (2x→1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
