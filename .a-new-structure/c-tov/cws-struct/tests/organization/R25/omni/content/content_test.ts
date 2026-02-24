// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/organization/R25/omni/content/content_test.ts
// purpose: OmniCode R[25] content-level detection tests — DATA-DRIVEN.
//          Containment entry presence.
//          Error catalog is the source of truth.
// layer:   R[25] — Block-level organization
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
// Omni R25 Content Specification — THE TABLE
// ---------------------------------------------------------------------------

const OMNI_R25_CONTENT_CASES: TestCase[] = [

  // --- containment entries ---

  {
    code: "CWS-R25-061",
    fixture: "omni/content/no-containment.omni",
    desc: "warn for no >> containment entries",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "omni", label: "R25", cases: OMNI_R25_CONTENT_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[25] OmniCode content-level detection tests — DATA-DRIVEN.
// 1 table-driven case.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
