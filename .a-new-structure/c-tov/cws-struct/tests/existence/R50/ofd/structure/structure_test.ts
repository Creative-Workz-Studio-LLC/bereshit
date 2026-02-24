// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/existence/R50/ofd/structure/structure_test.ts
// purpose: OFD R[50] file-level detection tests — DATA-DRIVEN.
//          Top-level field presence, type validation, source extension.
//          Error catalog is the source of truth.
// layer:   R[50] — File-level detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertGreater } from "jsr:@std/assert";
import { warnings } from "@test/helpers.ts";
import type { TestCase } from "@test/_runner.ts";
import { runSuite } from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// OFD R50 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const OFD_R50_CASES: TestCase[] = [

  // --- identity: required top-level fields ---

  {
    code: "CWS-R50-052",
    fixture: "ofd/identity/missing-fields.ofd",
    desc: "error for missing ofd version field",
  },

  {
    code: "CWS-R50-054",
    fixture: "ofd/identity/missing-fields.ofd",
    desc: "error for missing source field",
  },

  {
    code: "CWS-R50-057",
    fixture: "ofd/identity/missing-fields.ofd",
    desc: "error for missing compiled field",
  },

  // --- identity: type validation ---

  {
    code: "CWS-R50-053",
    fixture: "ofd/identity/wrong-types.ofd",
    desc: "error when ofd field is not a number",
  },

  {
    code: "CWS-R50-055",
    fixture: "ofd/identity/wrong-types.ofd",
    desc: "error when source field is not a string",
  },

  {
    code: "CWS-R50-058",
    fixture: "ofd/identity/wrong-types.ofd",
    desc: "error when compiled field is not a string",
  },

  // --- identity: source extension ---

  {
    code: "CWS-R50-056",
    fixture: "ofd/identity/bad-source-ext.ofd",
    desc: "error when source is not .omni",
  },

  // --- structure: required section ---

  {
    code: "CWS-R50-059",
    fixture: "ofd/structure/missing-identity-section.ofd",
    desc: "error for missing identity section",
  },

  // --- R25 code that fires at structure level (recommended fields) ---

  {
    code: "CWS-R25-066",
    fixture: "ofd/identity/missing-fields.ofd",
    desc: "warn for missing recommended top-level fields",
    validate: (results) => {
      const warns = warnings(results);
      assertGreater(warns.length, 0, "Missing recommended fields should produce warnings");
    },
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "ofd", label: "R50", cases: OFD_R50_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[50] OFD file-level detection tests — DATA-DRIVEN.
// 9 table-driven cases. Fixture caching: missing-fields (3x->1x), wrong-types (3x->1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
