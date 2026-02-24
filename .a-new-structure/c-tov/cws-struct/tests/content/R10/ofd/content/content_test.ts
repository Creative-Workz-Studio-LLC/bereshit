// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/content/R10/ofd/content/content_test.ts
// purpose: R10 (content-level) detection tests for OFD — DATA-DRIVEN.
//          Dependency field type validation.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { R10TestCase } from "../../_r10.ts";
import { runR10Suite } from "../../_r10.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// OFD R10 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const OFD_R10_CASES: R10TestCase[] = [
  {
    code: "CWS-R10-060",
    fixture: "ofd/dependencies/wrong-types.ofd",
    desc: "errors when provides is not an array",
    rule: "dependencies/provides_type",
  },

  {
    code: "CWS-R10-061",
    fixture: "ofd/dependencies/wrong-types.ofd",
    desc: "errors when derives is not an array",
    rule: "dependencies/derives_type",
  },
];

runR10Suite({ format: "ofd", cases: OFD_R10_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R10 OFD content tests — DATA-DRIVEN. 2 table rows → 2 tests.
// Dependency field type validation (provides and derives must be arrays).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
