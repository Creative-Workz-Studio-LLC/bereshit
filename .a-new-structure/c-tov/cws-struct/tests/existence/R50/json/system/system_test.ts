// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/existence/R50/json/system/system_test.ts
// purpose: JSON R[50] system-level tests — DATA-DRIVEN.
//          Parse failure detection for malformed JSONC input.
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
// JSON R50 System Specification — THE TABLE
// ---------------------------------------------------------------------------

const JSON_R50_SYSTEM_CASES: TestCase[] = [

  // --- system: parse failure ---

  {
    code: "CWS-R50-007",
    fixture: "json/system/bad-parse.jsonc",
    desc: "error for unparseable JSONC",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "json", label: "R50", cases: JSON_R50_SYSTEM_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[50] JSON system-level tests — DATA-DRIVEN.
// 1 table-driven case.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
