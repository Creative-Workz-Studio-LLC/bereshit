// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/existence/R50/json/blocks/structure_test.ts
// purpose: JSON R[50] file-level detection tests — DATA-DRIVEN.
//          3-block structure presence, format detection.
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
// JSON R50 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const JSON_R50_CASES: TestCase[] = [

  // --- structure: 3-block detection ---

  {
    code: "CWS-R50-070",
    fixture: "json/structure/not-3block.jsonc",
    desc: "error for file missing _pragma and _P1_key",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "json", label: "R50", cases: JSON_R50_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[50] JSON file-level detection tests — DATA-DRIVEN.
// 1 table-driven case.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
