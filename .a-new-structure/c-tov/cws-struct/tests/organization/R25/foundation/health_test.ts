// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R25/foundation/health_test.ts
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
  trueToLevel, levelToEmoji, levelToDirection, normalizeBase50,
  encodeHealthTrite, decodeHealthTrite,
} from "@lib/foundation/mod.ts";
import type { AtomicAction, HebrewState } from "@lib/foundation/mod.ts";

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

Deno.test("[CWS-T00-091] scoring/computeContainerScore: empty actions = 0 (yashar)", () => {
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

Deno.test("scoring/computeContainerScore: mixed failures (asymmetric cascade)", () => {
  // 5 pass (weight 1.0 each, linear) + 3 failures (cascade)
  // Aligned: +1.0 × 5 = +5.0
  // Misaligned (cascade): error(2.0×1) + warn(1.0×2) + info(0.25×3)
  //   = -2.0 + -2.0 + -0.75 = -4.75
  // weightedSum = 5.0 - 4.75 = 0.25
  // maxWeight = 5.0 + 2.0 + 1.0 + 0.25 = 8.25
  // score = 0.25/8.25 × 100 = 3.03 → 3
  const actions = [
    pass("p1"), pass("p2"), pass("p3"), pass("p4"), pass("p5"),
    fail("e1", "error"),
    fail("w1", "warn"),
    fail("i1", "info"),
  ];
  const score = computeContainerScore("mixed", "metadata", actions);
  assertEquals(score.score, 3, "Asymmetric cascade: failures cost more than symmetric");
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
  assertEquals(health.level, "even");
  assertEquals(health.direction, 0);
  assertEquals(health.hardpoint, 0);
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
  assertEquals(health.level, "whole");      // 34-66 = whole (shalem)
  assertEquals(health.direction, 1);        // whole → aligned
  assertEquals(health.hardpoint, 50);       // normalizeBase50(39) → 50
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
  assertEquals(health.level, "perfect");    // 67-100 = perfect (tov)
  assertEquals(health.direction, 1);        // perfect → aligned
  assertEquals(health.hardpoint, 100);      // normalizeBase50(100) → 100
  assertEquals(health.alignedCount, 5);
  assertEquals(health.misalignedCount, 0);
});

// ---------------------------------------------------------------------------
// Hebrew State Resolution (ALG-001)
// ---------------------------------------------------------------------------

// trueToLevel — all 7 state boundaries
Deno.test("state/trueToLevel: broken at -100", () => {
  assertEquals(trueToLevel(-100), "broken");
});
Deno.test("state/trueToLevel: broken at -67 (boundary)", () => {
  assertEquals(trueToLevel(-67), "broken");
});
Deno.test("state/trueToLevel: wanting at -66 (boundary)", () => {
  assertEquals(trueToLevel(-66), "wanting");
});
Deno.test("state/trueToLevel: wanting at -34 (boundary)", () => {
  assertEquals(trueToLevel(-34), "wanting");
});
Deno.test("state/trueToLevel: lacking at -33 (boundary)", () => {
  assertEquals(trueToLevel(-33), "lacking");
});
Deno.test("state/trueToLevel: lacking at -1 (boundary)", () => {
  assertEquals(trueToLevel(-1), "lacking");
});
Deno.test("state/trueToLevel: even at 0", () => {
  assertEquals(trueToLevel(0), "even");
});
Deno.test("state/trueToLevel: sound at 1 (boundary)", () => {
  assertEquals(trueToLevel(1), "sound");
});
Deno.test("state/trueToLevel: sound at 33 (boundary)", () => {
  assertEquals(trueToLevel(33), "sound");
});
Deno.test("state/trueToLevel: whole at 34 (boundary)", () => {
  assertEquals(trueToLevel(34), "whole");
});
Deno.test("state/trueToLevel: whole at 66 (boundary)", () => {
  assertEquals(trueToLevel(66), "whole");
});
Deno.test("state/trueToLevel: perfect at 67 (boundary)", () => {
  assertEquals(trueToLevel(67), "perfect");
});
Deno.test("state/trueToLevel: perfect at 100", () => {
  assertEquals(trueToLevel(100), "perfect");
});

// trueToLevel — clamp beyond ±100
Deno.test("state/trueToLevel: clamp below -100 = broken", () => {
  assertEquals(trueToLevel(-150), "broken");
});
Deno.test("state/trueToLevel: clamp above 100 = perfect", () => {
  assertEquals(trueToLevel(150), "perfect");
});

// levelToEmoji — all 7 states
Deno.test("state/levelToEmoji: all 7 states have distinct emoji", () => {
  const states: HebrewState[] = ["broken", "wanting", "lacking", "even", "sound", "whole", "perfect"];
  const emojis = states.map(levelToEmoji);
  // Each should be non-empty
  for (const e of emojis) {
    assertEquals(typeof e, "string");
    assertEquals(e.length > 0, true);
  }
  // At least 4 distinct (negative states may share, positive states may share, but they shouldn't all be the same)
  const unique = new Set(emojis);
  assertEquals(unique.size >= 4, true);
});

// levelToDirection — ternary direction from state
Deno.test("state/levelToDirection: negative states → -1", () => {
  assertEquals(levelToDirection("broken"), -1);
  assertEquals(levelToDirection("wanting"), -1);
  assertEquals(levelToDirection("lacking"), -1);
});
Deno.test("state/levelToDirection: even → 0", () => {
  assertEquals(levelToDirection("even"), 0);
});
Deno.test("state/levelToDirection: positive states → +1", () => {
  assertEquals(levelToDirection("sound"), 1);
  assertEquals(levelToDirection("whole"), 1);
  assertEquals(levelToDirection("perfect"), 1);
});

// normalizeBase50 — 5 hardpoints
Deno.test("state/normalizeBase50: ≤-75 → -100", () => {
  assertEquals(normalizeBase50(-100), -100);
  assertEquals(normalizeBase50(-76), -100);
  assertEquals(normalizeBase50(-75), -100);
});
Deno.test("state/normalizeBase50: -74 to -25 → -50", () => {
  assertEquals(normalizeBase50(-74), -50);
  assertEquals(normalizeBase50(-50), -50);
  assertEquals(normalizeBase50(-25), -50);
});
Deno.test("state/normalizeBase50: -24 to 25 → 0", () => {
  assertEquals(normalizeBase50(-24), 0);
  assertEquals(normalizeBase50(0), 0);
  assertEquals(normalizeBase50(25), 0);
});
Deno.test("state/normalizeBase50: 26 to 75 → 50", () => {
  assertEquals(normalizeBase50(26), 50);
  assertEquals(normalizeBase50(50), 50);
  assertEquals(normalizeBase50(75), 50);
});
Deno.test("state/normalizeBase50: >75 → 100", () => {
  assertEquals(normalizeBase50(76), 100);
  assertEquals(normalizeBase50(100), 100);
});

// ---------------------------------------------------------------------------
// Trite Encoding (ALG-010)
// ---------------------------------------------------------------------------

// encodeHealthTrite — pack/unpack roundtrip
Deno.test("trite/encode: all even [0,0,0,0,0] = 121", () => {
  assertEquals(encodeHealthTrite(0, 0, 0, 0, 0), 121);
});

Deno.test("trite/encode: all broken [-1,-1,-1,-1,-1] = 0", () => {
  assertEquals(encodeHealthTrite(-1, -1, -1, -1, -1), 0);
});

Deno.test("trite/encode: all perfect [+1,+1,+1,+1,+1] = 242", () => {
  assertEquals(encodeHealthTrite(1, 1, 1, 1, 1), 242);
});

Deno.test("trite/encode: mixed [1,-1,0,1,-1] matches spec example", () => {
  // From ternary-storage-algorithms.adoc example:
  // Input [1,-1,0,1,-1] → unsigned [2,0,1,2,0]
  // Horner: u4=0 → 0; *3+u3=2 → 2; *3+u2=1 → 7; *3+u1=0 → 21; *3+u0=2 → 65
  assertEquals(encodeHealthTrite(1, -1, 0, 1, -1), 65);
});

// decodeHealthTrite — unpack known values
Deno.test("trite/decode: 121 = [0,0,0,0,0]", () => {
  assertEquals(decodeHealthTrite(121), [0, 0, 0, 0, 0]);
});

Deno.test("trite/decode: 0 = [-1,-1,-1,-1,-1]", () => {
  assertEquals(decodeHealthTrite(0), [-1, -1, -1, -1, -1]);
});

Deno.test("trite/decode: 242 = [+1,+1,+1,+1,+1]", () => {
  assertEquals(decodeHealthTrite(242), [1, 1, 1, 1, 1]);
});

Deno.test("trite/decode: 65 = [1,-1,0,1,-1]", () => {
  assertEquals(decodeHealthTrite(65), [1, -1, 0, 1, -1]);
});

// Roundtrip — encode then decode should return original
Deno.test("trite/roundtrip: all 243 values round-trip correctly", () => {
  for (let byte = 0; byte <= 242; byte++) {
    const trits = decodeHealthTrite(byte);
    const repacked = encodeHealthTrite(trits[0], trits[1], trits[2], trits[3], trits[4]);
    assertEquals(repacked, byte, `Roundtrip failed for byte ${byte}`);
  }
});

// Edge cases
Deno.test("trite/decode: out-of-range returns [0,0,0,0,0]", () => {
  assertEquals(decodeHealthTrite(-1), [0, 0, 0, 0, 0]);
  assertEquals(decodeHealthTrite(243), [0, 0, 0, 0, 0]);
  assertEquals(decodeHealthTrite(999), [0, 0, 0, 0, 0]);
});

// computeHealthScore includes trite
Deno.test("trite/healthScore: empty blocks → trite 121 (all even)", () => {
  const health = computeHealthScore([]);
  assertEquals(health.trite, 121);
});

Deno.test("trite/healthScore: perfect blocks → trite includes file direction +1", () => {
  const actions = [pass("p1"), pass("p2"), pass("p3")];
  const c = computeContainerScore("test", "metadata", actions);
  const b = computeBlockScore("metadata", [c]);
  const health = computeHealthScore([b]);
  const trits = decodeHealthTrite(health.trite);
  // t0 = metadata direction (should be +1)
  assertEquals(trits[0], 1, "metadata trit should be +1");
  // t4 = file direction (should be +1)
  assertEquals(trits[4], 1, "file-level trit should be +1");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Ternary health scoring tests — direction × impact, nothing invented.
// -100 (shavar) → 0 (yashar) → +100 (tov).
// Hebrew state resolution: 7 states, 5 hardpoints, ternary direction.
// Trite encoding: 5 balanced trits → 1 byte (0-242), 243 states (ALG-010).
//
// "Diverse weights, and diverse measures, both of them are alike abomination
//  to the LORD." — Proverbs 20:10
// ============================================================================
