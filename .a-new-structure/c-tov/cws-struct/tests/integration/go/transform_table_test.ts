// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/go/transform_table_test.ts
// key:     B-tov-cws-struct-tests-integration-go-transform-table
// title:   Go Transform Tests — Table-Driven
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Table-driven Go transform tests using _transform_runner.ts.
//          Each row generates up to 3 tests: dry-run, actual, idempotent.
//          Ports the manual transform_test.ts to data-driven format.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { runTransformSuite } from "@test/_transform_runner.ts";
import type { TransformFormatSpec } from "@test/_transform_runner.ts";

// ============================================================================
// BODY
// ============================================================================

const goTransformSpec: TransformFormatSpec = {
  format: "go",
  label: "go/transform",
  cases: [
    // ── Valid files: no transforms needed ──────────────────────────
    {
      fixture: "go/structure/valid-library.go",
      desc: "valid library needs no structural transforms",
      expectedRules: [],
      witnessCode: "CWS-T00-112",
    },
    {
      fixture: "go/structure/valid-executable.go",
      desc: "valid executable needs no structural transforms",
      expectedRules: [],
    },

    // ── move-tests: Test func in BODY → CLOSING Cv ────────────────
    {
      fixture: "go/closing/tests-in-body.go",
      desc: "move Test func from BODY to CLOSING Cv",
      expectedRules: ["move-tests"],
      resolvedRules: ["test-placement"],
      idempotent: true,
    },

    // ── move-main: func main() in BODY → CLOSING Ce ──────────────
    {
      fixture: "go/closing/main-in-body.go",
      desc: "move func main() from BODY to CLOSING Ce",
      expectedRules: ["move-main"],
      resolvedRules: ["main-placement"],
      idempotent: true,
    },

    // ── reorder-closing: fix CLOSING zone order ───────────────────
    {
      fixture: "go/closing/wrong-closing-order.go",
      desc: "reorder CLOSING zones to canonical order",
      expectedRules: ["reorder-closing"],
      resolvedRules: ["closing/zone-order"],
      idempotent: true,
    },
  ],
};

runTransformSuite(goTransformSpec);

// ============================================================================
// CLOSING
// ============================================================================
//
// Table-driven Go transform tests — 5 cases generating up to 13 tests.
// Each transform operation verified: detected (dry-run), applied (actual),
// and converged (idempotent).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
