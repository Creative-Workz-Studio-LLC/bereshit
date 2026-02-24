// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/pattern/R05/go/metadata/metadata_test.ts
// purpose: Go R[5] metadata content tests — DATA-DRIVEN.
//          Directive content mismatch, missing Pragma/Metadata identity constructs.
//          Error catalog is the source of truth.
// layer:   R[5] — Concept/detail detection
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
// Go R05 Metadata Specification — THE TABLE
// ---------------------------------------------------------------------------

const GO_R05_METADATA_CASES: TestCase[] = [

  // --- content: directive content mismatch ---

  {
    code: "CWS-R05-050",
    fixture: "go/directives/unknown-code-format.go",
    desc: "warn for //omni:code not matching content schema",
  },

  // --- content: missing identity constructs ---

  {
    code: "CWS-R05-051",
    fixture: "go/identity/legacy-identity.go",
    desc: "info for missing Pragma identity construct in METADATA",
  },

  {
    code: "CWS-R05-052",
    fixture: "go/identity/legacy-identity.go",
    desc: "info for missing Metadata identity construct in METADATA",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "go", label: "R05", cases: GO_R05_METADATA_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[5] Go metadata content tests — DATA-DRIVEN.
// 3 table-driven cases + 0 bespoke = 5+ test functions.
// Fixture caching: legacy-identity (2x→1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
