// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/root/transpile_table_test.ts
// key:     B-tov-cws-struct-tests-integration-root-transpile-table
// title:   Transpile Tests — Table-Driven
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Table-driven transpile tests using _transpile_runner.ts.
//          Tests the three-cord transpiler: Rust → Go conversions.
//          Verifies structural skeleton output, identity transfer,
//          and section mapping across format boundaries.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { runTranspileSuite } from "@test/_transpile_runner.ts";
import type { TranspileSpec } from "@test/_transpile_runner.ts";

// ============================================================================
// BODY
// ============================================================================

const rustToGoSpec: TranspileSpec = {
  label: "transpile/rust→go",
  cases: [
    // ── Rust module → Go: structural skeleton ───────────────────────
    {
      sourceFormat: "rust",
      targetFormat: "go",
      fixture: "transpiler/rust-module-with-content.rs",
      desc: "transpile Rust module to Go — produces structural skeleton",
      minOutputLines: 20,
      outputContains: [
        "package",     // Go package declaration
        "METADATA",    // Block structure preserved
        "SETUP",
        "BODY",
        "CLOSING",
      ],
      outputAbsent: [
        "fn ",         // Rust function syntax should not appear
        "pub ",        // Rust visibility should not carry over
      ],
    },

    // ── Same fixture: verify stats ──────────────────────────────────
    {
      sourceFormat: "rust",
      targetFormat: "go",
      fixture: "transpiler/rust-module-with-content.rs",
      desc: "transpile produces reasonable extraction stats",
      minChunks: 1,
      minItems: 1,
    },
  ],
};

runTranspileSuite(rustToGoSpec);

// ============================================================================
// CLOSING
// ============================================================================
//
// Table-driven transpile tests — Rust → Go conversion verification.
// The three-cord transpiler produces structural skeletons: correct layout,
// section headers, identity transfer, function stubs.
//
// "A threefold cord is not quickly broken." — Ecclesiastes 4:12
// ============================================================================
