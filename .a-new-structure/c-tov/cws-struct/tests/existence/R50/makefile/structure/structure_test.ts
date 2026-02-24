// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/existence/R50/makefile/structure/structure_test.ts
// purpose: Makefile R[50] file-level detection tests — DATA-DRIVEN.
//          OmniCode detection, block boundary presence.
//          Error catalog is the source of truth.
// layer:   R[50] — File-level detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals } from "jsr:@std/assert";
import { errors, fixture, lazyFormat } from "@test/helpers.ts";
import type { TestCase } from "@test/_runner.ts";
import { runSuite } from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Makefile R50 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const MAKEFILE_R50_CASES: TestCase[] = [

  // --- structure: OmniCode detection ---

  {
    code: "CWS-R50-071",
    fixture: "makefile/structure/not-omni.mk",
    desc: "plain makefile without OmniCode markers",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "makefile", label: "R50", cases: MAKEFILE_R50_CASES });

// ---------------------------------------------------------------------------
// Negative test: file WITH SETUP block should NOT trigger blocks/setup error
// (Not in the runner table because it's a negative assertion — the code
//  should NOT fire, which inverts the runner's primary check.)
// ---------------------------------------------------------------------------

const makefile = lazyFormat("makefile");

Deno.test("[CWS-R50-023] R50/makefile/negative: SETUP block present — no blocks/setup error", async () => {
  const results = await makefile.lint(fixture("makefile/closing/missing-required-tags.mk"));
  const setupErrors = errors(results).filter((r) => r.rule === "blocks/setup");
  assertEquals(setupErrors.length, 0,
    "File with SETUP block should not trigger blocks/setup error");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[50] Makefile file-level detection tests — DATA-DRIVEN.
// 1 table-driven case + 1 standalone negative test.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
