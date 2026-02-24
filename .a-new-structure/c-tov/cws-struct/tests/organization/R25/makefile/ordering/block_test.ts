// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/organization/R25/makefile/ordering/block_test.ts
// purpose: Makefile R[25] block-level detection tests — DATA-DRIVEN.
//          Closing section tags, cross-reference consistency, metadata
//          required/recommended/extension section checks.
//          Error catalog is the source of truth.
// layer:   R[25] — Block-level organization
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertGreater } from "jsr:@std/assert";
import { errors, warnings, infos } from "@test/helpers.ts";
import type { TestCase } from "@test/_runner.ts";
import { runSuite } from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Makefile R25 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const MAKEFILE_R25_CASES: TestCase[] = [

  // --- closing: required section tags ---

  {
    code: "CWS-R25-052",
    fixture: "makefile/closing/missing-required-tags.mk",
    desc: "detects missing POLICY/REFERENCE/NOTE closing tags",
    rule: "closing/section/",
  },

  // --- xref: cross-reference consistency ---

  {
    code: "CWS-R25-026",
    fixture: "makefile/ordering/xref-mismatch.mk",
    desc: "pragma key != metadata key mismatch",
    rule: "xref/P1-M1-key",
    messageContains: ["pragma key", "mismatch"],
  },

  {
    code: "CWS-R25-027",
    fixture: "makefile/ordering/xref-mismatch.mk",
    desc: "pragma title != metadata title mismatch",
    rule: "xref/P5-M1-title",
    messageContains: ["pragma title", "Title mismatch"],
  },

  // --- metadata: required/recommended/extension ---

  {
    code: "CWS-R25-046",
    fixture: "makefile/metadata/missing-required.mk",
    desc: "error for missing M1 required metadata section",
    validate: (results) => {
      const metaErrs = errors(results).filter((r) =>
        r.rule.startsWith("metadata/") && r.rule.endsWith("/required")
      );
      assertGreater(metaErrs.length, 0,
        "Missing M1 section should trigger metadata/M1/required error");
    },
  },

  {
    code: "CWS-R25-047",
    fixture: "makefile/metadata/missing-required.mk",
    desc: "warns for missing M4/M5 recommended sections",
    validate: (results) => {
      const metaWarns = warnings(results).filter((r) =>
        r.rule.startsWith("metadata/") && r.rule.endsWith("/recommended")
      );
      assertGreater(metaWarns.length, 0,
        "Missing M4/M5 sections should trigger metadata/M{n}/recommended warnings");
    },
  },

  {
    code: "CWS-R25-048",
    fixture: "makefile/metadata/missing-required.mk",
    desc: "info for missing M6-M10 extension sections",
    validate: (results) => {
      const extInfos = infos(results).filter((r) =>
        r.rule.startsWith("metadata/") && r.rule.endsWith("/extension")
      );
      assertGreater(extInfos.length, 0,
        "Missing M6-M10 sections should trigger metadata/M{n}/extension infos");
    },
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "makefile", label: "R25", cases: MAKEFILE_R25_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[25] Makefile block-level detection tests — DATA-DRIVEN.
// 6 table-driven cases. Fixture caching: xref-mismatch (2x->1x),
// missing-required (3x->1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
