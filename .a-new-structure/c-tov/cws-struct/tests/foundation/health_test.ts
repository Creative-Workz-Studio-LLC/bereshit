// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/foundation/health_test.ts
// key:     B-tov-cws-struct-tests-foundation-health
// title:   CWS Struct — Health Scoring Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-17
// authors: Nathan Emet (CPI-SI)
// purpose: Tests for computeContainerScore, computeBlockScore,
//          computeHealthScore. Verifies asymmetric scoring: errors cost 2x,
//          warnings 1x, infos 0.25x.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import {
  computeContainerScore, computeBlockScore, computeHealthScore,
} from "../../lib/foundation/mod.ts";
import type { AtomicAction } from "../../lib/foundation/mod.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helper — build atomic actions
// ---------------------------------------------------------------------------

function pass(check: string, container = "test", block = "metadata"): AtomicAction {
  return { check, container, block, passed: true };
}

function fail(
  check: string,
  severity: "error" | "warn" | "info" = "error",
  container = "test",
  block = "metadata",
): AtomicAction {
  return { check, container, block, passed: false, severity, reason: `Failed: ${check}` };
}

// ---------------------------------------------------------------------------
// computeContainerScore
// ---------------------------------------------------------------------------

Deno.test("computeContainerScore: empty actions = 100", () => {
  const score = computeContainerScore("section", "block", []);
  assertEquals(score.score, 100);
  assertEquals(score.total, 0);
  assertEquals(score.passed, 0);
});

Deno.test("computeContainerScore: all passing = 100", () => {
  const actions = [pass("check1"), pass("check2"), pass("check3")];
  const score = computeContainerScore("I1_core", "metadata", actions);
  assertEquals(score.score, 100);
  assertEquals(score.total, 3);
  assertEquals(score.passed, 3);
  assertEquals(score.failedErrors, 0);
});

Deno.test("computeContainerScore: one error in 10 checks = asymmetric scoring", () => {
  // 9 pass + 1 error: earned = 9, penalty = 2, raw = (9-2)/10 = 0.7 → 70%
  const actions = [
    ...Array(9).fill(null).map((_, i) => pass(`check${i}`)),
    fail("bad_check", "error"),
  ];
  const score = computeContainerScore("section", "metadata", actions);
  assertEquals(score.score, 70, "1 error in 10 = 70%, not 90%");
  assertEquals(score.failedErrors, 1);
});

Deno.test("computeContainerScore: one warning in 10 checks", () => {
  // 9 pass + 1 warn: earned = 9, penalty = 1, raw = (9-1)/10 = 0.8 → 80%
  const actions = [
    ...Array(9).fill(null).map((_, i) => pass(`check${i}`)),
    fail("warn_check", "warn"),
  ];
  const score = computeContainerScore("section", "metadata", actions);
  assertEquals(score.score, 80);
  assertEquals(score.failedWarnings, 1);
});

Deno.test("computeContainerScore: one info in 10 checks", () => {
  // 9 pass + 1 info: earned = 9, penalty = 0.25, raw = (9-0.25)/10 = 0.875 → 88%
  const actions = [
    ...Array(9).fill(null).map((_, i) => pass(`check${i}`)),
    fail("info_check", "info"),
  ];
  const score = computeContainerScore("section", "metadata", actions);
  assertEquals(score.score, 88);
  assertEquals(score.failedInfos, 1);
});

Deno.test("computeContainerScore: all errors = 0 (clamped)", () => {
  const actions = [fail("e1", "error"), fail("e2", "error")];
  const score = computeContainerScore("broken", "metadata", actions);
  assertEquals(score.score, 0, "All errors should clamp to 0");
  assertEquals(score.passed, 0);
  assertEquals(score.failedErrors, 2);
});

Deno.test("computeContainerScore: mixed failures", () => {
  // 5 pass + 1 error + 1 warn + 1 info = 8 total
  // earned = 5, penalty = 2 + 1 + 0.25 = 3.25, raw = (5-3.25)/8 = 0.21875 → 22%
  const actions = [
    pass("p1"), pass("p2"), pass("p3"), pass("p4"), pass("p5"),
    fail("e1", "error"),
    fail("w1", "warn"),
    fail("i1", "info"),
  ];
  const score = computeContainerScore("mixed", "metadata", actions);
  assertEquals(score.score, 22);
  assertEquals(score.total, 8);
  assertEquals(score.passed, 5);
});

// ---------------------------------------------------------------------------
// computeBlockScore
// ---------------------------------------------------------------------------

Deno.test("computeBlockScore: empty containers = 0", () => {
  const score = computeBlockScore("metadata", []);
  assertEquals(score.score, 0);
  assertEquals(score.containers.length, 0);
});

Deno.test("computeBlockScore: averages container scores", () => {
  const c1 = computeContainerScore("I1", "metadata", [pass("a"), pass("b")]);
  const c2 = computeContainerScore("I2", "metadata", [pass("a"), fail("b", "error")]);
  // c1 = 100, c2 = 0 (1 pass - 2 penalty = -1/2 = 0 clamped) → average = 50
  const block = computeBlockScore("metadata", [c1, c2]);
  assertEquals(block.score, 50);
});

Deno.test("computeBlockScore: single perfect container = 100", () => {
  const c1 = computeContainerScore("I1", "metadata", [pass("a")]);
  const block = computeBlockScore("metadata", [c1]);
  assertEquals(block.score, 100);
});

// ---------------------------------------------------------------------------
// computeHealthScore
// ---------------------------------------------------------------------------

Deno.test("computeHealthScore: empty blocks = 0", () => {
  const health = computeHealthScore([]);
  assertEquals(health.total, 0);
  assertEquals(health.totalActions, 0);
  assertEquals(health.passCount, 0);
  assertEquals(health.failCount, 0);
});

Deno.test("computeHealthScore: aggregates from blocks", () => {
  const c1 = computeContainerScore("I1", "metadata", [pass("a"), pass("b")]);
  const c2 = computeContainerScore("X1", "closing", [pass("a"), fail("b", "warn")]);
  const b1 = computeBlockScore("metadata", [c1]);
  const b2 = computeBlockScore("closing", [c2]);
  const health = computeHealthScore([b1, b2]);

  // c2: 2 total, earned=1, penalty=1 warn=1.0, raw=(1-1)/2=0 → score=0
  // b1 = 100, b2 = 0 → average = 50
  assertEquals(health.total, 50);
  assertEquals(health.totalActions, 4);
  assertEquals(health.passCount, 3);
  assertEquals(health.failCount, 1);
});

Deno.test("computeHealthScore: perfect health = 100", () => {
  const c1 = computeContainerScore("I1", "metadata", [pass("a"), pass("b"), pass("c")]);
  const c2 = computeContainerScore("X1", "closing", [pass("d"), pass("e")]);
  const b1 = computeBlockScore("metadata", [c1]);
  const b2 = computeBlockScore("closing", [c2]);
  const health = computeHealthScore([b1, b2]);
  assertEquals(health.total, 100);
  assertEquals(health.passCount, 5);
  assertEquals(health.failCount, 0);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Health scoring tests — verifying asymmetric math works as designed.
// "Diverse weights, and diverse measures, both of them are alike abomination
//  to the LORD." — Proverbs 20:10
// ============================================================================
