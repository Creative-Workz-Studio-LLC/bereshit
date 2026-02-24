// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/organization/R25/omni/blocks/block_test.ts
// purpose: OmniCode R[25] block-level detection tests — DATA-DRIVEN.
//          Directive presence, type binding, state, attribution, grounding,
//          navigation, dependencies, metadata, declarations, containment.
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
// Omni R25 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const OMNI_R25_CASES: TestCase[] = [

  // --- directives: identity, state, attribution, grounding ---

  {
    code: "CWS-R25-080",
    fixture: "omni/structure/no-shebang.omni",
    desc: "error for missing @struct directive",
  },

  {
    code: "CWS-R25-081",
    fixture: "omni/structure/no-shebang.omni",
    desc: "error for missing @version directive",
  },

  {
    code: "CWS-R25-082",
    fixture: "omni/structure/no-shebang.omni",
    desc: "error for missing @architect directive",
  },

  {
    code: "CWS-R25-083",
    fixture: "omni/structure/no-shebang.omni",
    desc: "error for missing @grounding directive",
  },

  // --- sections: navigation, dependencies, metadata, declarations ---

  {
    code: "CWS-R25-084",
    fixture: "omni/structure/no-shebang.omni",
    desc: "warn for missing navigation section",
  },

  {
    code: "CWS-R25-085",
    fixture: "omni/structure/no-shebang.omni",
    desc: "warn for missing dependencies section",
  },

  {
    code: "CWS-R25-086",
    fixture: "omni/structure/no-shebang.omni",
    desc: "warn for missing metadata section",
  },

  {
    code: "CWS-R25-087",
    fixture: "omni/structure/no-shebang.omni",
    desc: "info for missing declarations section",
  },

  // --- containment ---

  {
    code: "CWS-R25-062",
    fixture: "omni/structure/no-shebang.omni",
    desc: "warn for missing >> containment block",
  },

  // --- bad-shebang variant: same codes fire ---

  {
    code: "CWS-R25-080",
    fixture: "omni/structure/bad-shebang.omni",
    desc: "bad shebang also triggers identity errors (consistency check)",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "omni", label: "R25", cases: OMNI_R25_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[25] OmniCode block-level detection tests — DATA-DRIVEN.
// 10 table-driven cases. Fixture caching: no-shebang (9x->1x).
// Massive cache efficiency — 9 tests share 1 fixture parse.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
