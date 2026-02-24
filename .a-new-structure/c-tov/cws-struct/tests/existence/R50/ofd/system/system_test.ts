// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/existence/R50/ofd/system/system_test.ts
// purpose: OFD R[50] system-level tests — DATA-DRIVEN.
//          Parse failure detection: empty file, invalid JSON, wrong root type.
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
// OFD R50 System Specification — THE TABLE
// ---------------------------------------------------------------------------

const OFD_R50_SYSTEM_CASES: TestCase[] = [

  // --- system: parse failures ---

  {
    code: "CWS-R50-003",
    fixture: "ofd/system/empty.ofd",
    desc: "error for empty OFD file",
  },

  {
    code: "CWS-R50-004",
    fixture: "ofd/system/bad-json.ofd",
    desc: "error for unparseable JSON in OFD",
  },

  {
    code: "CWS-R50-005",
    fixture: "ofd/system/array-root.ofd",
    desc: "error when OFD root is an array",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "ofd", label: "R50", cases: OFD_R50_SYSTEM_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[50] OFD system-level tests — DATA-DRIVEN.
// 3 table-driven cases. Each fixture unique (no caching opportunity).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
