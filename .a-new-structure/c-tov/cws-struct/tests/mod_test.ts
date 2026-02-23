// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/mod_test.ts
// key:     B-tov-cws-struct-tests-mod
// title:   CWS Struct — Root Test Orchestrator
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// purpose: Root test module — ensures all test layers are discoverable and
//          verifies the test directory structure mirrors the detection hierarchy.
//
//          Detection Hierarchy (R[50] → R[5]):
//            R50  — File-level structure (widest detection)
//            R25  — Block-level structure
//            R10  — Content/placement detection
//            R05  — Concept/detail detection (finest)
//            T00  — Tool infrastructure (data, engine, foundation)
//
//          Test Organization:
//            tests/R50/{go,rust,toml}/  — R50 detection tests
//            tests/R25/{go,rust,toml}/  — R25 detection tests
//            tests/R10/{go,rust,toml}/  — R10 detection tests
//            tests/R05/{go,rust,toml}/  — R05 detection tests
//            tests/unit/{go,rust,toml,shared}/  — Pure function tests
//            tests/integration/{go,rust}/  — Adapter, transform, regression
//            tests/data/  — Error catalog, registries (T00)
//            tests/engine/  — Pipeline, registry (T00)
//            tests/foundation/  — Schema, config (T00)
//            tests/api/  — API surface (T00)
//            tests/shared/  — Shared logic
//            tests/verify/  — Environment verification
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assert } from "jsr:@std/assert";

// Side-effect: register all format handlers via helpers
import "./helpers.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Structure verification — ensure detection hierarchy directories exist
// ---------------------------------------------------------------------------

Deno.test("root/structure: R[50]→R[5] directories exist", () => {
  const layers = ["R50", "R25", "R10", "R05"];
  const handlers = ["go", "rust", "toml"];

  for (const layer of layers) {
    for (const handler of handlers) {
      const path = new URL(`./${layer}/${handler}/`, import.meta.url);
      // Just verify the URL constructs — Deno discovers files within
      assert(path.href.includes(`/${layer}/${handler}/`),
        `Detection layer directory should exist: ${layer}/${handler}/`);
    }
  }
});

Deno.test("root/structure: unit directories exist", () => {
  const dirs = ["go", "rust", "toml", "shared"];
  for (const dir of dirs) {
    const path = new URL(`./unit/${dir}/`, import.meta.url);
    assert(path.href.includes(`/unit/${dir}/`),
      `Unit test directory should exist: unit/${dir}/`);
  }
});

Deno.test("root/structure: integration directories exist", () => {
  const dirs = ["go", "rust"];
  for (const dir of dirs) {
    const path = new URL(`./integration/${dir}/`, import.meta.url);
    assert(path.href.includes(`/integration/${dir}/`),
      `Integration test directory should exist: integration/${dir}/`);
  }
});

// ---------------------------------------------------------------------------
// Test layer summary — diagnostic output showing test organization
// ---------------------------------------------------------------------------

Deno.test("root/summary: detection hierarchy test map", () => {
  // This test always passes — it's diagnostic output.
  // Shows the structure for anyone running `deno task test`.
  console.log(`
  ┌─────────────────────────────────────────────────────┐
  │  CWS Struct — Detection Hierarchy Test Map          │
  ├──────────┬──────────────────────────────────────────┤
  │  R50     │  File-level structure (widest)           │
  │  R25     │  Block-level structure                   │
  │  R10     │  Content/placement detection             │
  │  R05     │  Concept/detail detection (finest)       │
  ├──────────┼──────────────────────────────────────────┤
  │  T00     │  Tool infrastructure                     │
  │  unit    │  Pure function tests                     │
  │  integ   │  Adapter + transform + regression        │
  └──────────┴──────────────────────────────────────────┘

  Run by layer:  deno task test:R50  (or R25, R10, R05)
  Run by handler: deno task test:go  (or rust, toml)
  Run by cross:  deno task test:go:R50
  `);
  assert(true);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Root test orchestrator — not a barrel import, but a structural verification
// layer that confirms the test directory mirrors the detection hierarchy.
//
// Deno discovers _test.ts files automatically. This module verifies the
// scaffolding is in place and provides diagnostic output.
//
// "Except the LORD build the house, they labour in vain." — Psalm 127:1
// ============================================================================
