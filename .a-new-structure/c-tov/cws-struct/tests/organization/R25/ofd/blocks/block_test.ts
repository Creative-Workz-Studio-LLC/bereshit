// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/organization/R25/ofd/blocks/block_test.ts
// purpose: OFD R[25] block-level detection tests — DATA-DRIVEN.
//          Identity section field presence, types, state, attribution,
//          optional top-level fields, dependency fields.
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
// OFD R25 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const OFD_R25_CASES: TestCase[] = [

  // --- identity: sub-field checks ---

  {
    code: "CWS-R25-068",
    fixture: "ofd/identity/missing-fields.ofd",
    desc: "error for missing identity sub-fields",
  },

  // --- identity: recommended fields ---

  {
    code: "CWS-R25-092",
    fixture: "ofd/identity/wrong-types.ofd",
    desc: "warn for missing recommended identity fields",
  },

  // --- state: recommended fields ---

  {
    code: "CWS-R25-095",
    fixture: "ofd/identity/wrong-types.ofd",
    desc: "warn for missing recommended state fields",
  },

  // --- attribution: optional ---

  {
    code: "CWS-R25-098",
    fixture: "ofd/identity/wrong-types.ofd",
    desc: "info for missing optional attribution",
  },

  // --- identity: required fields (key/type/title) ---

  {
    code: "CWS-R25-090",
    fixture: "ofd/identity/missing-required-fields.ofd",
    desc: "error for missing required identity fields",
  },

  // --- identity: field type errors ---

  {
    code: "CWS-R25-091",
    fixture: "ofd/identity/wrong-type-fields.ofd",
    desc: "error for wrong identity field types",
  },

  // --- state: required fields (version/created) ---

  {
    code: "CWS-R25-093",
    fixture: "ofd/identity/missing-state-required.ofd",
    desc: "error for missing required state fields",
  },

  // --- state: field type errors ---

  {
    code: "CWS-R25-094",
    fixture: "ofd/identity/wrong-type-fields.ofd",
    desc: "error for wrong state field types",
  },

  // --- attribution: required fields (architect/implementation) ---

  {
    code: "CWS-R25-096",
    fixture: "ofd/identity/missing-attribution-required.ofd",
    desc: "error for missing required attribution fields",
  },

  // --- attribution: field type errors ---

  {
    code: "CWS-R25-097",
    fixture: "ofd/identity/wrong-type-fields.ofd",
    desc: "error for wrong attribution field types",
  },

  // --- optional top-level fields ---

  {
    code: "CWS-R25-067",
    fixture: "ofd/identity/missing-required-fields.ofd",
    desc: "info for missing optional top-level fields",
  },

  // --- dependency fields ---

  {
    code: "CWS-R25-099",
    fixture: "ofd/structure/missing-dependencies.ofd",
    desc: "warn for missing dependency sub-fields",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "ofd", label: "R25", cases: OFD_R25_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[25] OFD block-level detection tests — DATA-DRIVEN.
// 12 table-driven cases. Fixture caching: wrong-types (3x->1x),
// wrong-type-fields (3x->1x), missing-required-fields (2x->1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
