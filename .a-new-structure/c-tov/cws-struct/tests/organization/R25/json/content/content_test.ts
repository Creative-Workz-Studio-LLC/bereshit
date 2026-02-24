// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/organization/R25/json/content/content_test.ts
// purpose: JSON R[25] content-level detection tests — DATA-DRIVEN.
//          Pragma legend presence.
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
// JSON R25 Content Specification — THE TABLE
// ---------------------------------------------------------------------------

const JSON_R25_CONTENT_CASES: TestCase[] = [

  // --- pragma: legend presence ---

  {
    code: "CWS-R25-069",
    fixture: "json/metadata/missing-m1-m2-fields.jsonc",
    desc: "info for missing _pragma_legend",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "json", label: "R25", cases: JSON_R25_CONTENT_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[25] JSON content-level detection tests — DATA-DRIVEN.
// 1 table-driven case.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
