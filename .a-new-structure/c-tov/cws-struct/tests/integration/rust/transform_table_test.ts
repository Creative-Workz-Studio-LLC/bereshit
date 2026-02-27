// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/rust/transform_table_test.ts
// key:     B-tov-cws-struct-tests-integration-rust-transform-table
// title:   Rust Transform Tests — Table-Driven
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Table-driven Rust transform tests using _transform_runner.ts.
//          Covers: separator normalization, move-tests, move-main,
//          reorder-closing, valid-library no-op, and scaffold.
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

const rustTransformSpec: TransformFormatSpec = {
  format: "rust",
  label: "rust/transform",
  cases: [
    // ── Valid file: no structural transforms needed ────────────────
    {
      fixture: "rust/structure/valid-library.rs",
      desc: "valid library needs no structural transforms",
      expectedRules: [],
      witnessCode: "CWS-T00-112",
    },

    // ── Separator normalization ───────────────────────────────────
    {
      fixture: "rust/format/transform-separators.rs",
      desc: "normalize ASCII dashes and prefixes to unicode",
      expectedRules: ["dash-to-unicode"],
      idempotent: true,
    },

    // ── move-tests: #[cfg(test)] in BODY → CLOSING Cv ────────────
    {
      fixture: "rust/closing/tests-in-body.rs",
      desc: "move #[cfg(test)] from BODY to CLOSING Cv",
      expectedRules: ["move-tests"],
      resolvedRules: ["test-placement"],
      messageContains: ["#[cfg(test)]"],
      idempotent: true,
    },

    // ── move-main: fn main() in BODY → CLOSING Ce ────────────────
    {
      fixture: "rust/closing/main-in-body.rs",
      desc: "move fn main() from BODY to CLOSING Ce",
      expectedRules: ["move-main"],
      resolvedRules: ["main-placement"],
      messageContains: ["fn main()"],
      idempotent: true,
    },

    // ── reorder-closing: fix CLOSING zone order ───────────────────
    {
      fixture: "rust/closing/wrong-closing-order.rs",
      desc: "reorder CLOSING zones to canonical order",
      expectedRules: ["reorder-closing"],
      resolvedRules: ["zone-order"],
      idempotent: true,
    },

    // ── scaffold: raw module gets 4-block structure ───────────────
    {
      fixture: "rust/format/scaffold-raw-module.rs",
      desc: "scaffold raw module into 4-block structure",
      expectedRules: ["scaffold"],
      contentChecks: [
        { contains: "METADATA BLOCK [METADATA]", in: "file" },
        { contains: "SETUP BLOCK [SETUP]", in: "file" },
        { contains: "BODY BLOCK [BODY]", in: "file" },
        { contains: "CLOSING BLOCK [CLOSING]", in: "file" },
        { contains: "END METADATA [END]", in: "file" },
        { contains: "END SETUP [END]", in: "file" },
        { contains: "END BODY [END]", in: "file" },
        { contains: "END CLOSING [END]", in: "file" },
      ],
      // Scaffold is idempotent — second scaffold on already-scaffolded file = no-op
      idempotent: true,
    },
  ],
};

runTransformSuite(rustTransformSpec);

// ============================================================================
// CLOSING
// ============================================================================
//
// Table-driven Rust transform tests — 6 cases generating up to 16 tests.
// Covers the full Rust transform pipeline: separators, moves, reorder,
// scaffold. Each operation verified through three phases.
//
// "Be ye transformed by the renewing of your mind." — Romans 12:2
// ============================================================================
