// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/organization/R25/toml/blocks/block_test.ts
// purpose: TOML R[25] block-level detection tests — DATA-DRIVEN.
//          Pragma consistency, closing order, metadata sections,
//          content zones, Cargo sections, form enforcement.
//          Error catalog is the source of truth.
// layer:   R[25] — Block-level detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  errors, warnings, infos, byRule, hasMessage,
} from "@test/helpers.ts";
import type { TestCase } from "@test/_runner.ts";
import {
  runSuite, noErrors,
} from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// TOML R25 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const TOML_R25_CASES: TestCase[] = [

  // --- consistency: pragma-driven cross-cutting checks ---

  {
    code: "CWS-R25-071",
    fixture: "toml/structure/pragma-mismatch.toml",
    desc: "detects pragma vs format field mismatch",
  },

  // --- closing: X section ordering ---

  {
    code: "CWS-R25-025",
    fixture: "toml/closing/wrong-x-order.toml",
    desc: "detects X sections out of order",
    rule: "closing/x-order",
    messageContains: ["X1"],
    validate: noErrors,
    cleanFixture: "toml/structure/valid-complete.toml",
    cleanDesc: "valid-complete has zero x-order warnings",
  },

  // --- metadata: extension sections ---

  {
    code: "CWS-R25-041",
    fixture: "toml/metadata/bad-contract.toml",
    desc: "warns about missing I4 extension identity section",
    validate: (results) => {
      const metaWarns = warnings(results).filter((r) =>
        r.rule.startsWith("metadata/") && (
          r.message.includes("I4_architecture") ||
          r.message.includes("C5_intent") ||
          r.message.includes("C6_roadmap") ||
          r.message.includes("C7_classification")
        )
      );
      assertGreater(metaWarns.length, 0, "Should warn about missing extension metadata sections");
    },
  },

  // --- content: zone labels ---

  {
    code: "CWS-R25-060",
    fixture: "toml/metadata/bad-contract.toml",
    desc: "warns about missing content zone labels Co and Cv",
    validate: (results) => {
      const zoneWarns = warnings(results).filter((r) =>
        r.rule.startsWith("content/") && (
          r.message.includes("Co") || r.message.includes("Cv")
        )
      );
      assertGreater(zoneWarns.length, 0, "Should warn about missing content zone labels");
    },
  },

  // --- cargo: section validation ---

  {
    code: "CWS-R25-004",
    fixture: "toml/cargo/missing-omni-metadata.toml",
    desc: "detects missing Cargo.toml sections",
    rule: "cargo/",
    messageContains: ["features"],
  },

  // --- closing: required and defined sections ---

  {
    code: "CWS-R25-050",
    fixture: "toml/closing/missing-required-sections.toml",
    desc: "errors for missing X1/X5 required closing sections",
    validate: (results) => {
      const requiredErrors = errors(results).filter((r) =>
        r.rule.includes("/required") && r.rule.startsWith("closing/")
      );
      assertGreater(requiredErrors.length, 0,
        "Missing required closing sections should trigger closing/key/required errors");
      assert(hasMessage(requiredErrors, "X1_policy") || hasMessage(requiredErrors, "X5_note"),
        "Should mention the missing required section name");
    },
  },

  {
    code: "CWS-R25-051",
    fixture: "toml/metadata/bad-contract.toml",
    desc: "missing defined closing sections X2/X3/X4",
    validate: (results) => {
      const closingInfos = infos(results).filter((r) =>
        r.rule.startsWith("closing/") && r.rule.endsWith("/defined") && (
          r.message.includes("X2_extension") ||
          r.message.includes("X3_troubleshooting") ||
          r.message.includes("X4_reference")
        )
      );
      assertGreater(closingInfos.length, 0, "Should report missing defined closing sections");
    },
  },

  // --- format: C7 classification ---

  {
    code: "CWS-R25-070",
    fixture: "toml/metadata/c7-tags-string.toml",
    desc: "C7_classification.tags as string instead of array",
    rule: "metadata/C7_classification/tags",
    messageContains: ["array"],
  },

  // --- metadata: required identity and context ---

  {
    code: "CWS-R25-040",
    fixture: "toml/metadata/missing-required-identity.toml",
    desc: "errors for missing required identity sections I1/I2/I3",
  },

  {
    code: "CWS-R25-042",
    fixture: "toml/metadata/missing-required-identity.toml",
    desc: "errors for missing required context sections C1-C4",
  },

  {
    code: "CWS-R25-043",
    fixture: "toml/metadata/missing-required-identity.toml",
    desc: "warns about missing extension context C5/C6/C7",
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "toml", label: "R25", cases: TOML_R25_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[25] TOML block-level detection tests — DATA-DRIVEN.
// 11 table-driven cases + 0 bespoke = 13+ test functions.
// Fixture caching: bad-contract (3x→1x), missing-required-identity (3x→1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
