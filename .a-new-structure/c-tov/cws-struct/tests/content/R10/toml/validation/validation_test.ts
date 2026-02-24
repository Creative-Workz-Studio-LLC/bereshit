// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/content/R10/toml/validation/validation_test.ts
// purpose: TOML R[10] key validation tests — DATA-DRIVEN.
//          Invalid keys, wrong types, extension keys in metadata and closing.
//          Error catalog is the source of truth.
// layer:   R[10] — Content-level detection
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
// TOML R10 Validation Specification — THE TABLE
// ---------------------------------------------------------------------------

const TOML_R10_VALIDATION_CASES: TestCase[] = [

  // --- metadata: key validation ---

  {
    code: "CWS-R10-020",
    fixture: "toml/metadata/bad-keys.toml",
    desc: "error for key violating naming pattern",
  },

  {
    code: "CWS-R10-022",
    fixture: "toml/metadata/bad-keys.toml",
    desc: "info for valid-pattern extension key",
  },

  {
    code: "CWS-R10-021",
    fixture: "toml/metadata/wrong-type-keys.toml",
    desc: "error when metadata section is not table",
  },

  // --- closing: key validation ---

  {
    code: "CWS-R10-023",
    fixture: "toml/metadata/bad-keys.toml",
    desc: "error for closing key violating naming",
  },

  {
    code: "CWS-R10-025",
    fixture: "toml/metadata/bad-keys.toml",
    desc: "info for valid-pattern closing extension key",
  },

  {
    code: "CWS-R10-024",
    fixture: "toml/metadata/wrong-type-keys.toml",
    desc: "error when closing section is not table",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "toml", label: "R10", cases: TOML_R10_VALIDATION_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[10] TOML key validation tests — DATA-DRIVEN.
// 6 table-driven cases. Fixture caching: bad-keys (4x->1x),
// wrong-type-keys (2x->1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
