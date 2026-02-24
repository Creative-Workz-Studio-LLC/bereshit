// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/existence/R50/json/blocks/boundary_test.ts
// purpose: JSON R[50] boundary marker detection tests — DATA-DRIVEN.
//          Content end, closing start, closing end boundaries.
//          Error catalog is the source of truth.
// layer:   R[50] — File-level detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { TestCase } from "@test/_runner.ts";
import { runSuite } from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// JSON R50 Boundary Specification — THE TABLE
// ---------------------------------------------------------------------------

const JSON_R50_BOUNDARY_CASES: TestCase[] = [

  // --- structure: boundary markers ---

  {
    code: "CWS-R50-060",
    fixture: "json/structure/missing-boundaries.jsonc",
    desc: "warn for missing _content_end boundary",
  },

  {
    code: "CWS-R50-061",
    fixture: "json/structure/missing-boundaries.jsonc",
    desc: "warn for missing _closing_start boundary",
  },

  {
    code: "CWS-R50-062",
    fixture: "json/structure/missing-boundaries.jsonc",
    desc: "warn for missing _closing_end boundary",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "json", label: "R50", cases: JSON_R50_BOUNDARY_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[50] JSON boundary marker detection tests — DATA-DRIVEN.
// 3 table-driven cases. Fixture caching: missing-boundaries (3x->1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
