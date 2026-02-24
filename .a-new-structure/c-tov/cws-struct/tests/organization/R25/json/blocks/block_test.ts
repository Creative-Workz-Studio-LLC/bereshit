// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/organization/R25/json/blocks/block_test.ts
// purpose: JSON R[25] block-level detection tests — DATA-DRIVEN.
//          M1/M2 field checks, closing section, pragma required/recommended,
//          metadata required sections.
//          Error catalog is the source of truth.
// layer:   R[25] — Block-level organization
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertGreater } from "jsr:@std/assert";
import { errors, warnings, byRule } from "@test/helpers.ts";
import type { TestCase } from "@test/_runner.ts";
import { runSuite } from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// JSON R25 Test Specification — THE TABLE
// ---------------------------------------------------------------------------

const JSON_R25_CASES: TestCase[] = [

  // --- metadata: M1/M2 field checks ---

  {
    code: "CWS-R25-074",
    fixture: "json/metadata/missing-m1-m2-fields.jsonc",
    desc: "warns about missing _M1_identity fields",
    rule: "metadata/M1/",
    validate: (results) => {
      const m1Warns = byRule(results, "metadata/M1/");
      assertGreater(m1Warns.length, 0,
        "_M1_identity with missing fields should trigger metadata/M1/{{field}} warning");
    },
  },

  {
    code: "CWS-R25-075",
    fixture: "json/metadata/missing-m1-m2-fields.jsonc",
    desc: "warns about missing _M2_state fields",
    rule: "metadata/M2/",
    validate: (results) => {
      const m2Warns = byRule(results, "metadata/M2/");
      assertGreater(m2Warns.length, 0,
        "_M2_state with missing fields should trigger metadata/M2/{{field}} warning");
    },
  },

  // --- closing: missing closing sub-blocks ---

  {
    code: "CWS-R25-050",
    fixture: "json/metadata/missing-m1-m2-fields.jsonc",
    desc: "errors for missing _X1 and _X4",
    rule: "closing/",
    validate: (results) => {
      const closingErrs = errors(results).filter((r) => r.rule.startsWith("closing/"));
      assertGreater(closingErrs.length, 0,
        "Missing _X1 and _X4 should trigger closing/{{key}}/required errors");
    },
  },

  // --- pragma: required and recommended ---

  {
    code: "CWS-R25-044",
    fixture: "json/pragma/missing-required-pragma.jsonc",
    desc: "errors for missing required pragma fields",
    validate: (results) => {
      const pragmaErrs = errors(results).filter((r) =>
        r.rule.startsWith("pragma/") && r.rule.endsWith("/required")
      );
      assertGreater(pragmaErrs.length, 0,
        "Missing required pragma fields should trigger pragma/key/required errors");
    },
  },

  {
    code: "CWS-R25-045",
    fixture: "json/pragma/missing-required-pragma.jsonc",
    desc: "warns for missing recommended pragma fields",
    validate: (results) => {
      const pragmaWarns = warnings(results).filter((r) =>
        r.rule.startsWith("pragma/") && r.rule.endsWith("/recommended")
      );
      assertGreater(pragmaWarns.length, 0,
        "Missing recommended pragma fields should trigger pragma/key/recommended warnings");
    },
  },

  // --- metadata: required sections ---

  {
    code: "CWS-R25-040",
    fixture: "json/pragma/missing-required-pragma.jsonc",
    desc: "errors for missing required metadata sections",
    validate: (results) => {
      const metaErrs = errors(results).filter((r) =>
        r.rule.startsWith("metadata/") && r.rule.endsWith("/required")
      );
      assertGreater(metaErrs.length, 0,
        "Missing required metadata sections should trigger metadata/key/required errors");
    },
  },
];

// ---------------------------------------------------------------------------
// Run the suite
// ---------------------------------------------------------------------------

runSuite({ format: "json", label: "R25", cases: JSON_R25_CASES });

// ============================================================================
// CLOSING
// ============================================================================
//
// R[25] JSON block-level detection tests — DATA-DRIVEN.
// 6 table-driven cases. Fixture caching: missing-m1-m2-fields (3x->1x),
// missing-required-pragma (3x->1x).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
