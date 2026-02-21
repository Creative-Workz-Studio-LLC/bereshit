// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/foundation/health_test.ts
// key:     B-tov-cws-struct-tests-foundation-health
// title:   CWS Struct — Health Scoring Tests (Ternary)
// type:    Code (Test)
// version: a-02.00
// created: 2026-02-17
// updated: 2026-02-18
// authors: Nathan Emet (CPI-SI)
// purpose: Tests for ternary health scoring: -100 (shavar) → 0 (yashar) → +100 (tov).
//          Direction × impact_weight. Errors weigh 2×, warnings 1×, infos 0.25×.
//          Neutral (0) tracked but doesn't affect score — prevents cascade inflation.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals } from "jsr:@std/assert";
import {
  computeContainerScore, computeBlockScore, computeHealthScore,
} from "../../lib/foundation/mod.ts";
import type { AtomicAction } from "../../lib/foundation/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helpers — build atomic actions (ternary)
// ---------------------------------------------------------------------------

/** Aligned action: direction +1, default impact "warn" (weight 1.0). */
function pass(check: string, container = "test", block = "metadata"): AtomicAction {
  return { check, container, block, direction: 1 };
}

/** Misaligned action: direction -1 with specified impact. */
function fail(
  check: string,
  impact: "error" | "warn" | "info" = "error",
  container = "test",
  block = "metadata",
): AtomicAction {
  return { check, container, block, direction: -1, impact, reason: `Failed: ${check}` };
}

/** Neutral action: direction 0 — tracked but doesn't affect score. */
function neutral(check: string, container = "test", block = "metadata"): AtomicAction {
  return { check, container, block, direction: 0, impact: "info", reason: "Cannot assess" };
}

// ---------------------------------------------------------------------------
// computeContainerScore
// ---------------------------------------------------------------------------

Deno.test("scoring/computeContainerScore: empty actions = 0 (yashar)", () => {
  const score = computeContainerScore("section", "block", []);
  assertEquals(score.score, 0, "Empty = yashar (not assessed), not perfect");
  assertEquals(score.total, 0);
  assertEquals(score.aligned, 0);
  assertEquals(score.neutral, 0);
  assertEquals(score.misaligned, 0);
});

Deno.test("scoring/computeContainerScore: all passing = 100 (tov)", () => {
  const actions = [pass("check1"), pass("check2"), pass("check3")];
  const score = computeContainerScore("I1_core", "metadata", actions);
  assertEquals(score.score, 100);
  assertEquals(score.total, 3);
  assertEquals(score.aligned, 3);
  assertEquals(score.misaligned, 0);
});

Deno.test("scoring/computeContainerScore: one error in 10 checks = 64", () => {
  // 9 pass (weight 1.0 each) + 1 error (weight 2.0)
  // weightedSum = 9 - 2 = 7, maxWeight = 9 + 2 = 11
  // score = 7/11 × 100 = 63.6 → 64
  const actions = [
    ...Array(9).fill(null).map((_, i) => pass(`check${i}`)),
    fail("bad_check", "error"),
  ];
  const score = computeContainerScore("section", "metadata", actions);
  assertEquals(score.score, 64, "1 error in 10: (9-2)/(9+2) = 7/11 → 64%");
  assertEquals(score.misaligned, 1);
});

Deno.test("scoring/computeContainerScore: one warning in 10 checks = 80", () => {
  // 9 pass (weight 1.0) + 1 warn (weight 1.0)
  // weightedSum = 9 - 1 = 8, maxWeight = 9 + 1 = 10
  // score = 8/10 × 100 = 80
  const actions = [
    ...Array(9).fill(null).map((_, i) => pass(`check${i}`)),
    fail("warn_check", "warn"),
  ];
  const score = computeContainerScore("section", "metadata", actions);
  assertEquals(score.score, 80);
  assertEquals(score.misaligned, 1);
});

Deno.test("scoring/computeContainerScore: one info in 10 checks = 95", () => {
  // 9 pass (weight 1.0) + 1 info (weight 0.25)
  // weightedSum = 9 - 0.25 = 8.75, maxWeight = 9 + 0.25 = 9.25
  // score = 8.75/9.25 × 100 = 94.6 → 95
  const actions = [
    ...Array(9).fill(null).map((_, i) => pass(`check${i}`)),
    fail("info_check", "info"),
  ];
  const score = computeContainerScore("section", "metadata", actions);
  assertEquals(score.score, 95, "Info has minimal impact: 0.25× weight");
  assertEquals(score.misaligned, 1);
});

Deno.test("scoring/computeContainerScore: all errors = -100 (shavar)", () => {
  // 2 errors (direction -1, weight 2.0 each)
  // weightedSum = -4, maxWeight = 4
  // score = -4/4 × 100 = -100
  const actions = [fail("e1", "error"), fail("e2", "error")];
  const score = computeContainerScore("broken", "metadata", actions);
  assertEquals(score.score, -100, "All errors = fully misaligned (shavar)");
  assertEquals(score.aligned, 0);
  assertEquals(score.misaligned, 2);
});

Deno.test("scoring/computeContainerScore: mixed failures = 21", () => {
  // 5 pass (weight 1.0) + 1 error (2.0) + 1 warn (1.0) + 1 info (0.25) = 8 total
  // weightedSum = 5 - 2 - 1 - 0.25 = 1.75
  // maxWeight = 5 + 2 + 1 + 0.25 = 8.25
  // score = 1.75/8.25 × 100 = 21.2 → 21
  const actions = [
    pass("p1"), pass("p2"), pass("p3"), pass("p4"), pass("p5"),
    fail("e1", "error"),
    fail("w1", "warn"),
    fail("i1", "info"),
  ];
  const score = computeContainerScore("mixed", "metadata", actions);
  assertEquals(score.score, 21);
  assertEquals(score.total, 8);
  assertEquals(score.aligned, 5);
  assertEquals(score.misaligned, 3);
});

Deno.test("scoring/computeContainerScore: neutral actions don't affect score", () => {
  // 2 pass (weight 1.0) + 3 neutral → neutral doesn't push either way
  // weightedSum = 2, maxWeight = 2, score = 100
  // But total = 5, neutral = 3
  const actions = [
    pass("p1"), pass("p2"),
    neutral("n1"), neutral("n2"), neutral("n3"),
  ];
  const score = computeContainerScore("cascaded", "metadata", actions);
  assertEquals(score.score, 100, "Neutral doesn't dilute — only non-neutral actions count");
  assertEquals(score.total, 5);
  assertEquals(score.aligned, 2);
  assertEquals(score.neutral, 3);
  assertEquals(score.misaligned, 0);
});

Deno.test("scoring/computeContainerScore: only neutral = 0 (yashar)", () => {
  // All neutral → maxWeight = 0, score = 0
  const actions = [neutral("n1"), neutral("n2")];
  const score = computeContainerScore("unknown", "metadata", actions);
  assertEquals(score.score, 0, "All neutral = yashar (can't assess)");
  assertEquals(score.total, 2);
  assertEquals(score.neutral, 2);
});

// ---------------------------------------------------------------------------
// computeBlockScore
// ---------------------------------------------------------------------------

Deno.test("scoring/computeBlockScore: empty containers = 0", () => {
  const score = computeBlockScore("metadata", []);
  assertEquals(score.score, 0);
  assertEquals(score.containers.length, 0);
});

Deno.test("scoring/computeBlockScore: log-weighted from container actions", () => {
  const c1 = computeContainerScore("I1", "metadata", [pass("a"), pass("b")]);
  const c2 = computeContainerScore("I2", "metadata", [pass("a"), fail("b", "error")]);
  // c1 linear = 100, c2 linear = -33 (for drill-down)
  //
  // Block log-weighted: collect all 4 non-neutral actions, sort by severity:
  //   [fail("b",error,-1), pass(warn,+1), pass(warn,+1), pass(warn,+1)]
  // n=4, logDenom=ln(5)=1.609
  //   i=0: base=2.0, mult=1.000, eff=2.000, contrib=-2.000
  //   i=1: base=1.0, mult=0.861, eff=0.861, contrib=+0.861
  //   i=2: base=1.0, mult=0.683, eff=0.683, contrib=+0.683
  //   i=3: base=1.0, mult=0.431, eff=0.431, contrib=+0.431
  // weightedSum=-0.025, maxWeight=3.975
  // score = round(-0.025/3.975 × 100) = round(-0.63) = -1
  //
  // The error at position 0 gets FULL weight (2.0 base × 1.0 positional).
  // Three passes with diminishing positional weight barely offset it.
  const block = computeBlockScore("metadata", [c1, c2]);
  assertEquals(block.score, -1);
});

Deno.test("scoring/computeBlockScore: single perfect container = 100", () => {
  const c1 = computeContainerScore("I1", "metadata", [pass("a")]);
  const block = computeBlockScore("metadata", [c1]);
  assertEquals(block.score, 100);
});

// ---------------------------------------------------------------------------
// computeHealthScore
// ---------------------------------------------------------------------------

Deno.test("scoring/computeHealthScore: empty blocks = 0", () => {
  const health = computeHealthScore([]);
  assertEquals(health.total, 0);
  assertEquals(health.totalActions, 0);
  assertEquals(health.alignedCount, 0);
  assertEquals(health.neutralCount, 0);
  assertEquals(health.misalignedCount, 0);
});

Deno.test("scoring/computeHealthScore: per-block averaging of log-weighted blocks", () => {
  const c1 = computeContainerScore("I1", "metadata", [pass("a"), pass("b")]);
  const c2 = computeContainerScore("X1", "closing", [pass("a"), fail("b", "warn")]);
  const b1 = computeBlockScore("metadata", [c1]);
  const b2 = computeBlockScore("closing", [c2]);
  const health = computeHealthScore([b1, b2]);

  // b1 (metadata): 2 passes → log-weighted = 100
  // b2 (closing): [fail(warn,-1), pass(warn,+1)]
  //   n=2, logDenom=ln(3)=1.099
  //   i=0: base=1.0, mult=1.000, eff=1.000, contrib=-1.000
  //   i=1: base=1.0, mult=0.631, eff=0.631, contrib=+0.631
  //   weightedSum=-0.369, maxWeight=1.631
  //   score = round(-0.369/1.631 × 100) = -23
  //
  // Per-block average: (100 + (-23)) / 2 = 38.5 → 39
  //
  // Each block speaks with equal voice. Metadata is perfect.
  // Closing has a foundation-level failure. The average reflects both.
  assertEquals(health.total, 39);
  assertEquals(health.totalActions, 4);
  assertEquals(health.alignedCount, 3);
  assertEquals(health.misalignedCount, 1);
  assertEquals(health.neutralCount, 0);
});

Deno.test("scoring/computeHealthScore: perfect health = 100", () => {
  const c1 = computeContainerScore("I1", "metadata", [pass("a"), pass("b"), pass("c")]);
  const c2 = computeContainerScore("X1", "closing", [pass("d"), pass("e")]);
  const b1 = computeBlockScore("metadata", [c1]);
  const b2 = computeBlockScore("closing", [c2]);
  const health = computeHealthScore([b1, b2]);
  assertEquals(health.total, 100);
  assertEquals(health.alignedCount, 5);
  assertEquals(health.misalignedCount, 0);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Ternary health scoring tests — direction × impact, nothing invented.
// -100 (shavar) → 0 (yashar) → +100 (tov).
//
// "Diverse weights, and diverse measures, both of them are alike abomination
//  to the LORD." — Proverbs 20:10
// ============================================================================
