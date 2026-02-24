// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/organization/R25/ofd/content/content_test.ts
// purpose: OFD R[25] content-level detection tests — DATA-DRIVEN.
//          Section type validation, children field presence, children type.
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
// OFD R25 Content Specification — THE TABLE
// ---------------------------------------------------------------------------

const OFD_R25_CONTENT_CASES: TestCase[] = [

  // --- section type validation ---

  {
    code: "CWS-R25-063",
    fixture: "ofd/structure/bad-sections.ofd",
    desc: "error when identity/state are wrong type",
  },

  // --- children field ---

  {
    code: "CWS-R25-064",
    fixture: "ofd/structure/no-children.ofd",
    desc: "warn for missing children field",
  },

  {
    code: "CWS-R25-065",
    fixture: "ofd/structure/bad-sections.ofd",
    desc: "error when children is not an array",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "ofd", label: "R25", cases: OFD_R25_CONTENT_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[25] OFD content-level detection tests — DATA-DRIVEN.
// 3 table-driven cases. Fixture caching: bad-sections (2x->1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
