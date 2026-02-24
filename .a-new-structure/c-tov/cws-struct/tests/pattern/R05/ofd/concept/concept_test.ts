// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/pattern/R05/ofd/concept/concept_test.ts
// purpose: OFD R[05] concept-level detection tests — DATA-DRIVEN.
//          Children entry validation, grounding/scripture structure.
//          Error catalog is the source of truth.
// layer:   R[05] — Concept/detail detection
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
// OFD R05 Concept Specification — THE TABLE
// ---------------------------------------------------------------------------

const OFD_R05_CASES: TestCase[] = [

  // --- children: entry validation ---

  {
    code: "CWS-R05-060",
    fixture: "ofd/children/bad-children.ofd",
    desc: "error when child entry is not an object",
  },

  {
    code: "CWS-R05-061",
    fixture: "ofd/children/bad-children.ofd",
    desc: "error when child missing path field",
  },

  {
    code: "CWS-R05-062",
    fixture: "ofd/children/bad-children.ofd",
    desc: "error when child path is not a string",
  },

  {
    code: "CWS-R05-063",
    fixture: "ofd/children/bad-children.ofd",
    desc: "info when child missing type field",
  },

  {
    code: "CWS-R05-064",
    fixture: "ofd/children/bad-children.ofd",
    desc: "info when child missing description",
  },

  // --- grounding: section validation ---

  {
    code: "CWS-R05-065",
    fixture: "ofd/grounding/bad-grounding.ofd",
    desc: "warn for missing recommended grounding field",
  },

  {
    code: "CWS-R05-066",
    fixture: "ofd/grounding/scriptures-wrong-type.ofd",
    desc: "error when scriptures is not array",
  },

  {
    code: "CWS-R05-067",
    fixture: "ofd/grounding/bad-grounding.ofd",
    desc: "error when scripture entry is not object",
  },

  {
    code: "CWS-R05-068",
    fixture: "ofd/grounding/bad-grounding.ofd",
    desc: "warn when scripture missing text",
  },

  {
    code: "CWS-R05-069",
    fixture: "ofd/grounding/bad-grounding.ofd",
    desc: "warn when scripture missing reference",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "ofd", label: "R05", cases: OFD_R05_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[05] OFD concept-level detection tests — DATA-DRIVEN.
// 10 table-driven cases. Fixture caching: bad-children (5x->1x),
// bad-grounding (4x->1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
