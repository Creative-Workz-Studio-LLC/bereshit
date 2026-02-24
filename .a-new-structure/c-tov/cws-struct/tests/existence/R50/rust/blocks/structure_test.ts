// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/existence/R50/rust/blocks/structure_test.ts
// purpose: Rust R[50] file-level detection tests — DATA-DRIVEN.
//          Structure, block presence, block order, format detection,
//          directives, derived/template classification.
//          Each test case is a row in a table. Error catalog is authority.
// layer:   R[50] — File-level detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import {
  fixture, lazyFormat, errors, warnings, byRule,
} from "@test/helpers.ts";
import type { TestCase } from "@test/_runner.ts";
import {
  runSuite, lintOnce, noErrors,
} from "@test/_runner.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Rust R50 Test Specification — THE TABLE
// ---------------------------------------------------------------------------
//
// Each row maps: error code → fixture → expected behavior.
// Severity comes from the error catalog unless overridden.
// ---------------------------------------------------------------------------

const RUST_R50_CASES: TestCase[] = [

  // --- structure: block presence and order ---

  {
    code: "CWS-R50-022",
    fixture: "rust/structure/missing-block.rs",
    desc: "produces error for missing METADATA block",
    rule: "block",
    messageContains: ["METADATA"],
    cleanFixture: "rust/structure/valid-library.rs",
    cleanDesc: "valid-library has zero errors",
  },

  {
    code: "CWS-R50-020",
    fixture: "rust/structure/wrong-block-order.rs",
    desc: "produces error for wrong block order (SETUP before METADATA)",
    rule: "block/order",
    cleanFixture: "rust/structure/valid-library.rs",
    cleanDesc: "valid-library has correct block order",
  },

  // --- format: format detection ---

  {
    code: "CWS-R50-073",
    fixture: "rust/format/no-omni.rs",
    desc: "plain .rs with no omni pragma — info only, no errors",
    validate: (results) => {
      assertEquals(errors(results).length, 0, "Should produce no errors");
      assertEquals(warnings(results).length, 0, "Should produce no warnings");
    },
  },

  // --- directive: OmniCode directive checks ---

  {
    code: "CWS-R50-011",
    fixture: "rust/directives/missing-key-directive.rs",
    desc: "derived file missing //omni:key",
    severity: "warn",  // Non-crate-root: Rust handler downgrades to warn
  },

  {
    code: "CWS-R50-012",
    fixture: "rust/structure/missing-block.rs",
    desc: "warns for missing recommended directives",
  },

  // --- block: end marker checks ---

  {
    code: "CWS-R50-021",
    fixture: "rust/identity/unparseable-statics.rs",
    desc: "detects missing END separators for blocks",
    rule: "block/end-",
  },

  // --- derived: template vs derived classification ---

  {
    code: "CWS-R50-042",
    fixture: "rust/directives/leftover-template/lib.rs",
    desc: "warns when derived file has template directive",
  },
];

// ---------------------------------------------------------------------------
// Run the suite — generates all Deno.test() calls from the table
// ---------------------------------------------------------------------------

runSuite({ format: "rust", label: "R50", cases: RUST_R50_CASES });

// ---------------------------------------------------------------------------
// Additional tests — clean validation requiring bespoke assertions
// ---------------------------------------------------------------------------

const rust = lazyFormat("rust");

Deno.test("R50/rust/valid-library: zero errors", async () => {
  const results = await lintOnce(rust, "rust", "rust/structure/valid-library.rs");
  noErrors(results);
});

Deno.test("R50/rust/valid-library: only separator warnings allowed", async () => {
  const results = await lintOnce(rust, "rust", "rust/structure/valid-library.rs");
  for (const w of warnings(results)) {
    assert(
      w.rule.includes("separator") || w.rule.includes("width") || w.rule.includes("style"),
      `Unexpected warning rule: ${w.rule} — ${w.message}`,
    );
  }
});

Deno.test("R50/rust/valid-module: zero errors", async () => {
  const results = await lintOnce(rust, "rust", "rust/structure/valid-module.rs");
  noErrors(results);
});

Deno.test("R50/rust/valid-module: subtype detected as module", async () => {
  const results = await lintOnce(rust, "rust", "rust/structure/valid-module.rs");
  const subtypeWarns = results.filter((r) => r.rule.includes("I2.subtype-value"));
  assertEquals(subtypeWarns.length, 0, `Module subtype should be recognized: ${JSON.stringify(subtypeWarns)}`);
});

Deno.test("R50/rust/valid-executable: zero errors", async () => {
  const results = await lintOnce(rust, "rust", "rust/structure/valid-executable.rs");
  noErrors(results);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[50] Rust structure tests — DATA-DRIVEN.
// 7 table-driven cases + 5 bespoke = 14+ test functions.
// Fixture caching: valid-library, missing-block shared across tests — lint once.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
