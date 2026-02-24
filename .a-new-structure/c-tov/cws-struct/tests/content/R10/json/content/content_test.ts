// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/content/R10/json/content/content_test.ts
// purpose: R10 (content-level) detection tests for JSON/JSONC — DATA-DRIVEN.
//          Tests content separator tag companions.
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
// JSON R10 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const JSON_R10_CASES: R10TestCase[] = [
  {
    code: "CWS-R10-007",
    fixture: "json/content/missing-sep-tags.jsonc",
    desc: "info for missing _Cc_tag, _Co_tag, _Cv_tag",
    rule: "content/_C",
  },
];

runR10Suite({ format: "json", cases: JSON_R10_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R10 JSON content tests — DATA-DRIVEN. 1 table row → 1 test.
// Add a code → add a row → test exists.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
