// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R03/rust/recursion/scope_test.ts
// key:     B-tov-cws-struct-tests-R03-rust-recursion
// title:   CWS Struct — R[3] Rust Recursion Detection Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// purpose: Format-level tests proving R[3] scope analysis detects recursion
//          in Rust files that R[5] regex cannot detect line-by-line.
//
//          Gate | Verifies
//          -----|----------------------------------------
//          G1   | Recursive Rust function detected (concept not "missing")
//          G2   | Non-recursive fixture has no false R[3] detections
//          G3   | graph.rs has_cycle detected (real-world validation)
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import { fixture, lazyFormat, byRule } from "@test/helpers.ts";

// ============================================================================
// BODY
// ============================================================================

const rust = lazyFormat("rust");

// ---------------------------------------------------------------------------
// G1: Recursive function fixture — recursion detected via R[3]
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-106] R03/rust/recursion/G1: recursive function — recursion concept detected", async () => {
  const results = await rust.lint(fixture("rust/scope/recursive-function.rs"));

  // The fixture has recursive functions (walk_tree calls walk_tree, factorial calls factorial).
  // R[5] regex can't detect this line-by-line. R[3] scope analysis should catch it.
  // If recursion IS detected, there should be NO "concept/free-functions/recursion/missing" info.
  const recursionMissing = byRule(results, "concept/free-functions/recursion/missing");
  assertEquals(recursionMissing.length, 0,
    "Recursive function should be detected by R[3] — no 'recursion/missing' expected");
});

// ---------------------------------------------------------------------------
// G2: Non-recursive fixture — no false R[3] detections
// ---------------------------------------------------------------------------

Deno.test("R03/rust/recursion/G2: non-recursive file — no false recursion detection", async () => {
  // valid-library.rs has no recursive functions
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));

  // concept/ results for recursion: if granted and not found, shows as "missing".
  // If denied and found, shows as denied. Neither should show false recursion detection.
  const recursionDenied = byRule(results, "concept/").filter(
    (r) => r.rule.includes("recursion") && r.severity === "warn",
  );
  assertEquals(recursionDenied.length, 0,
    "Non-recursive file should not have false recursion detection");
});

// ---------------------------------------------------------------------------
// G3: graph.rs real-world — has_cycle detected
// ---------------------------------------------------------------------------

Deno.test("R03/rust/recursion/G3: graph.rs — has_cycle recursion detected by R[3]", async () => {
  // This is the real-world file that exposed the R[3] need.
  // has_cycle calls itself recursively at line 223.
  const graphPath = new URL(
    "../../../../../../../b-word/L0-universal/c-hybrid/spec/config/src/graph.rs",
    import.meta.url,
  ).pathname;

  const results = await rust.lint(graphPath);

  // After R[3] fix: recursion should be detected, so no "missing" info
  const recursionMissing = byRule(results, "concept/free-functions/recursion/missing");
  assertEquals(recursionMissing.length, 0,
    "graph.rs has_cycle should be detected by R[3] scope analysis");

  // Verify overall health — should be clean (0E 0W 0I was confirmed in Phase 3)
  const errors = results.filter((r) => r.severity === "error");
  assertEquals(errors.length, 0, "graph.rs should have 0 errors");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[3] Rust recursion format-level tests — 3 gates proving scope analysis
// correctly detects recursive functions in real Rust files.
//
// The tripwire: multiline regex + scope analysis both run. If either catches
// the recursion, the concept is detected. Redundancy that doesn't harm.
//
// "Day unto day uttereth speech." — Psalm 19:2
// ============================================================================
