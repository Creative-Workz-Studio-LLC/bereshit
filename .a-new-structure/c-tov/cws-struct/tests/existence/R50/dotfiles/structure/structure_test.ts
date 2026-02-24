// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/existence/R50/dotfiles/structure/structure_test.ts
// purpose: Dotfiles R[50] file-level detection tests — DATA-DRIVEN.
//          Block boundary presence, order violations, end markers, pragma.
//          Error catalog is the source of truth.
// layer:   R[50] — File-level detection
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
// Dotfiles R50 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const DOTFILES_R50_CASES: TestCase[] = [

  // --- pragma: line and boundary ---

  {
    code: "CWS-R50-015",
    fixture: "dotfiles/structure/missing-pragma-line.editorconfig",
    desc: "warn for missing #!omni pragma line",
  },

  {
    code: "CWS-R50-016",
    fixture: "dotfiles/structure/missing-blocks.editorconfig",
    desc: "error for missing PRAGMA boundary",
  },

  {
    code: "CWS-R50-017",
    fixture: "dotfiles/structure/missing-end-pragma.editorconfig",
    desc: "warn for missing END PRAGMA boundary",
  },

  // --- blocks: boundary detection ---

  {
    code: "CWS-R50-024",
    fixture: "dotfiles/structure/missing-blocks.editorconfig",
    desc: "error for missing CLOSING boundary",
  },

  {
    code: "CWS-R50-025",
    fixture: "dotfiles/structure/missing-blocks.editorconfig",
    desc: "warn for missing END CLOSING boundary",
  },

  {
    code: "CWS-R50-026",
    fixture: "dotfiles/structure/wrong-order.editorconfig",
    desc: "error for CONTENT before METADATA (block order violation)",
  },

  {
    code: "CWS-R50-027",
    fixture: "dotfiles/structure/missing-blocks.editorconfig",
    desc: "error for missing METADATA boundary",
  },

  {
    code: "CWS-R50-028",
    fixture: "dotfiles/structure/missing-end-metadata.editorconfig",
    desc: "warn for missing END METADATA boundary",
  },

  {
    code: "CWS-R50-029",
    fixture: "dotfiles/structure/missing-blocks.editorconfig",
    desc: "error for missing CONTENT boundary",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "dotfiles", label: "R50", cases: DOTFILES_R50_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[50] Dotfiles file-level detection tests — DATA-DRIVEN.
// 9 table-driven cases. Fixture caching: missing-blocks (4x->1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
