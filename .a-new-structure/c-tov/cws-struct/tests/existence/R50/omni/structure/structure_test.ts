// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/existence/R50/omni/structure/structure_test.ts
// purpose: OmniCode R[50] file-level detection tests — DATA-DRIVEN.
//          Shebang presence, format validation, block tag detection.
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
// Omni R50 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const OMNI_R50_CASES: TestCase[] = [

  // --- shebang: presence and format ---

  {
    code: "CWS-R50-050",
    fixture: "omni/structure/blank.omni",
    desc: "error for blank first line (no shebang)",
  },

  {
    code: "CWS-R50-051",
    fixture: "omni/structure/no-shebang.omni",
    desc: "error for missing #!omni shebang",
  },

  {
    code: "CWS-R50-051",
    fixture: "omni/structure/bad-shebang.omni",
    desc: "error for wrong shebang (#!/bin/bash)",
  },

  // --- blocks: tag presence ---

  {
    code: "CWS-R50-022",
    fixture: "omni/structure/no-shebang.omni",
    desc: "error for missing block tags",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "omni", label: "R50", cases: OMNI_R50_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[50] OmniCode file-level detection tests — DATA-DRIVEN.
// 4 table-driven cases. Fixture caching: no-shebang (2x->1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
