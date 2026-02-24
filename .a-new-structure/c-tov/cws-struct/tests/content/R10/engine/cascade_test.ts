// R-level: R10 (engine layer — cascade logic)
// Migrated from: tests/engine/cascade_test.ts
// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R10/engine/cascade_test.ts
// key:     B-tov-cws-struct-tests-engine-cascade
// title:   CWS Struct — Cascade Logic Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Tests for pipeline cascade logic — structural failure detection,
//          action cascading, per-block selective cascade, layer tagging.
//
//          The cascade is the pipeline's immune system. When a structural
//          check fails, downstream checks become neutral. These tests verify
//          that root cause surfaces first and noise is eliminated.
//
// biblical_foundation: "For precept must be upon precept, precept upon precept;
//   line upon line, line upon line." — Isaiah 28:10
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";

import type { AtomicAction } from "@lib/foundation/health.ts";
import type { LintResult } from "@lib/foundation/types.ts";

import {
  cascadeActions,
  cascadeActionGroups,
  cascadeBlock,
  hasStructuralFailure,
  detectFailedBlocks,
  tagLayer,
} from "@lib/engine/cascade.ts";

// ---------------------------------------------------------------------------
// Test fixtures
// ---------------------------------------------------------------------------

/** Make a minimal AtomicAction for testing. */
function action(
  direction: -1 | 0 | 1,
  block: string,
  check = "test-check",
  impact: "error" | "warn" | "info" = "warn",
): AtomicAction {
  return {
    check,
    container: "test",
    block,
    direction,
    impact,
  };
}

/** Make a minimal LintResult for testing. */
function lint(
  rule: string,
  severity: "error" | "warn" | "info" = "error",
): LintResult {
  return {
    file: "test.go",
    severity,
    rule,
    message: `Test finding: ${rule}`,
  };
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// C1: cascadeActions — flip aligned to neutral
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-082] cascade/cascadeActions: flips aligned (+1) actions to neutral (0)", () => {
  const actions: AtomicAction[] = [
    action(1, "setup"),
    action(1, "body"),
    action(1, "closing"),
  ];

  cascadeActions(actions, "Layer 0 failure", 0);

  for (const a of actions) {
    assertEquals(a.direction, 0, "Aligned actions should become neutral");
    assertEquals(a.impact, "info", "Cascaded actions should be info severity");
    assertEquals(a.reason, "Layer 0 failure", "Reason should be set");
    assertEquals(a.layer, 0, "Source layer should be tagged");
  }
});

Deno.test("cascade/cascadeActions: preserves misaligned (-1) actions", () => {
  const actions: AtomicAction[] = [
    action(-1, "setup", "check-a", "error"),
    action(-1, "body", "check-b", "warn"),
  ];

  cascadeActions(actions, "Layer 0 failure", 0);

  for (const a of actions) {
    assertEquals(a.direction, -1, "Misaligned should stay misaligned");
  }
  // Original impact preserved
  assertEquals(actions[0]!.impact, "error");
  assertEquals(actions[1]!.impact, "warn");
});

Deno.test("cascade/cascadeActions: preserves already-neutral (0) actions", () => {
  const actions: AtomicAction[] = [
    action(0, "setup"),
    action(0, "body"),
  ];

  cascadeActions(actions, "Layer 0 failure", 0);

  for (const a of actions) {
    assertEquals(a.direction, 0, "Already neutral stays neutral");
  }
  // Reason should NOT be overwritten for already-neutral actions
  assertEquals(actions[0]!.reason, undefined, "Already-neutral actions keep original reason");
});

Deno.test("cascade/cascadeActions: mixed directions — only aligned flipped", () => {
  const actions: AtomicAction[] = [
    action(1, "setup"),   // Should become 0
    action(-1, "setup"),  // Should stay -1
    action(0, "body"),    // Should stay 0
    action(1, "body"),    // Should become 0
  ];

  cascadeActions(actions, "cascade test", 1);

  assertEquals(actions[0]!.direction, 0);
  assertEquals(actions[1]!.direction, -1);
  assertEquals(actions[2]!.direction, 0);
  assertEquals(actions[3]!.direction, 0);
});

Deno.test("cascade/cascadeActions: empty array is no-op", () => {
  const actions: AtomicAction[] = [];
  cascadeActions(actions, "empty test", 0);
  assertEquals(actions.length, 0);
});

// ---------------------------------------------------------------------------
// C2: cascadeActionGroups — batch cascade
// ---------------------------------------------------------------------------

Deno.test("cascade/cascadeActionGroups: cascades multiple groups", () => {
  const group1 = [action(1, "setup"), action(-1, "setup")];
  const group2 = [action(1, "body"), action(1, "closing")];

  cascadeActionGroups([group1, group2], "Layer 0 failure", 0);

  // Group 1: aligned flipped, misaligned preserved
  assertEquals(group1[0]!.direction, 0);
  assertEquals(group1[1]!.direction, -1);

  // Group 2: all aligned flipped
  assertEquals(group2[0]!.direction, 0);
  assertEquals(group2[1]!.direction, 0);
});

Deno.test("cascade/cascadeActionGroups: handles empty groups array", () => {
  cascadeActionGroups([], "empty test", 0);
  // No-op — just verify no crash
});

// ---------------------------------------------------------------------------
// C3: cascadeBlock — selective per-block cascade
// ---------------------------------------------------------------------------

Deno.test("cascade/cascadeBlock: cascades only matching block", () => {
  const actions = [
    action(1, "setup"),
    action(1, "body"),
    action(1, "setup"),
    action(1, "closing"),
  ];

  cascadeBlock(actions, "setup", "SETUP sections disordered", 1);

  // Only "setup" block actions should be cascaded
  assertEquals(actions[0]!.direction, 0, "setup should cascade");
  assertEquals(actions[1]!.direction, 1, "body should NOT cascade");
  assertEquals(actions[2]!.direction, 0, "setup should cascade");
  assertEquals(actions[3]!.direction, 1, "closing should NOT cascade");
});

Deno.test("cascade/cascadeBlock: preserves misaligned in matching block", () => {
  const actions = [
    action(-1, "setup"),
    action(1, "setup"),
  ];

  cascadeBlock(actions, "setup", "SETUP missing", 1);

  assertEquals(actions[0]!.direction, -1, "Misaligned preserved even in matching block");
  assertEquals(actions[1]!.direction, 0, "Aligned in matching block cascaded");
});

Deno.test("cascade/cascadeBlock: no-op when no actions match block", () => {
  const actions = [
    action(1, "body"),
    action(1, "closing"),
  ];

  cascadeBlock(actions, "setup", "SETUP missing", 1);

  assertEquals(actions[0]!.direction, 1, "No match — unchanged");
  assertEquals(actions[1]!.direction, 1, "No match — unchanged");
});

// ---------------------------------------------------------------------------
// C4: hasStructuralFailure — detect layer-level failure
// ---------------------------------------------------------------------------

Deno.test("cascade/hasStructuralFailure: detects matching error pattern", () => {
  const results = [
    lint("block/METADATA", "error"),
    lint("block/SETUP", "error"),
    lint("section/order", "warn"),
  ];

  assert(
    hasStructuralFailure(results, ["block/METADATA", "block/SETUP"]),
    "Should detect block errors",
  );
});

Deno.test("cascade/hasStructuralFailure: ignores warnings", () => {
  const results = [
    lint("block/METADATA", "warn"),
    lint("block/SETUP", "info"),
  ];

  assertEquals(
    hasStructuralFailure(results, ["block/METADATA", "block/SETUP"]),
    false,
    "Warnings don't trigger structural failure",
  );
});

Deno.test("cascade/hasStructuralFailure: false when no matching patterns", () => {
  const results = [
    lint("section/order", "error"),
    lint("content/placement", "error"),
  ];

  assertEquals(
    hasStructuralFailure(results, ["block/METADATA", "block/SETUP"]),
    false,
    "Unrelated errors don't trigger structural failure",
  );
});

Deno.test("cascade/hasStructuralFailure: matches prefix patterns", () => {
  const results = [
    lint("block/METADATA/missing", "error"),
  ];

  assert(
    hasStructuralFailure(results, ["block/METADATA"]),
    "Should match prefix patterns (startsWith)",
  );
});

Deno.test("cascade/hasStructuralFailure: empty results = no failure", () => {
  assertEquals(
    hasStructuralFailure([], ["block/METADATA"]),
    false,
    "Empty results = no structural failure",
  );
});

Deno.test("cascade/hasStructuralFailure: empty patterns = no failure", () => {
  const results = [lint("block/METADATA", "error")];
  assertEquals(
    hasStructuralFailure(results, []),
    false,
    "Empty patterns = nothing to match against",
  );
});

// ---------------------------------------------------------------------------
// C5: detectFailedBlocks — identify which blocks failed
// ---------------------------------------------------------------------------

Deno.test("cascade/detectFailedBlocks: extracts failed block names", () => {
  const results = [
    lint("block/METADATA", "error"),
    lint("block/SETUP", "error"),
    lint("section/order", "error"),     // not a block rule
    lint("block/BODY", "warn"),          // warning, not error
  ];

  const failed = detectFailedBlocks(results, "block/");

  assertEquals(failed.length, 2);
  assert(failed.includes("METADATA"));
  assert(failed.includes("SETUP"));
});

Deno.test("cascade/detectFailedBlocks: deduplicates block names", () => {
  const results = [
    lint("block/METADATA", "error"),
    lint("block/METADATA", "error"),  // duplicate
  ];

  const failed = detectFailedBlocks(results, "block/");
  assertEquals(failed.length, 1);
  assertEquals(failed[0], "METADATA");
});

Deno.test("cascade/detectFailedBlocks: ignores nested rules", () => {
  const results = [
    lint("block/METADATA/field", "error"),  // has slash after block name
  ];

  const failed = detectFailedBlocks(results, "block/");
  assertEquals(failed.length, 0, "Nested rules (with slashes) should be ignored");
});

Deno.test("cascade/detectFailedBlocks: empty results = empty array", () => {
  assertEquals(detectFailedBlocks([], "block/").length, 0);
});

// ---------------------------------------------------------------------------
// C6: tagLayer — stamp actions with pipeline layer
// ---------------------------------------------------------------------------

Deno.test("cascade/tagLayer: tags all actions with layer number", () => {
  const actions = [action(1, "setup"), action(-1, "body"), action(0, "closing")];

  tagLayer(actions, 2);

  for (const a of actions) {
    assertEquals(a.layer, 2, "All actions should be tagged with layer 2");
  }
});

Deno.test("cascade/tagLayer: overwrites existing layer tags", () => {
  const a = action(1, "setup");
  a.layer = 0;

  tagLayer([a], 3);

  assertEquals(a.layer, 3, "Layer tag should be overwritten");
});

Deno.test("cascade/tagLayer: all valid layer numbers", () => {
  for (const layer of [0, 1, 2, 3] as const) {
    const a = action(1, "test");
    tagLayer([a], layer);
    assertEquals(a.layer, layer);
  }
});

// ---------------------------------------------------------------------------
// C7: Integration — cascade chain simulation
// ---------------------------------------------------------------------------

Deno.test("cascade/integration: Layer 0 failure cascades all downstream", () => {
  // Simulate a file with missing blocks (Layer 0 failure)
  const layer0Results = [
    lint("block/SETUP", "error"),
    lint("block/BODY", "error"),
  ];

  // Downstream actions that WOULD have been positive
  const layer1Actions = [action(1, "setup"), action(1, "body")];
  const layer2Actions = [action(1, "setup"), action(1, "body")];
  const layer3Actions = [action(1, "setup"), action(1, "body")];

  // Tag actions with their layers
  tagLayer(layer1Actions, 1);
  tagLayer(layer2Actions, 2);
  tagLayer(layer3Actions, 3);

  // Detect structural failure
  assert(
    hasStructuralFailure(layer0Results, ["block/METADATA", "block/SETUP", "block/BODY", "block/CLOSING"]),
    "Layer 0 should detect structural failure",
  );

  // Cascade all downstream
  cascadeActionGroups(
    [layer1Actions, layer2Actions, layer3Actions],
    "Layer 0: missing blocks → all downstream neutral",
    0,
  );

  // All downstream should be neutral
  for (const group of [layer1Actions, layer2Actions, layer3Actions]) {
    for (const a of group) {
      assertEquals(a.direction, 0, "All downstream should be cascaded to neutral");
      assertEquals(a.impact, "info");
      assertEquals(a.layer, 0, "Source layer should be 0 (where failure originated)");
    }
  }
});

Deno.test("cascade/integration: Layer 1 per-block cascade", () => {
  // SETUP sections disordered, but BODY is fine
  const _layer1Results = [
    lint("section/SETUP/order", "error"),
  ];

  // Layer 2 actions for both blocks
  const layer2Actions = [
    action(1, "setup"),   // Should cascade (SETUP failed)
    action(1, "body"),    // Should NOT cascade (BODY fine)
    action(1, "setup"),   // Should cascade
    action(1, "closing"), // Should NOT cascade
  ];

  tagLayer(layer2Actions, 2);

  // Detect failed blocks (using section prefix for this example)
  // In real usage, this would detect which blocks had section-level failures
  const failedBlocks = ["setup"]; // We know SETUP failed

  // Selective per-block cascade
  for (const block of failedBlocks) {
    cascadeBlock(layer2Actions, block, `${block} sections disordered`, 1);
  }

  assertEquals(layer2Actions[0]!.direction, 0, "setup cascaded");
  assertEquals(layer2Actions[1]!.direction, 1, "body preserved");
  assertEquals(layer2Actions[2]!.direction, 0, "setup cascaded");
  assertEquals(layer2Actions[3]!.direction, 1, "closing preserved");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Cascade logic tests — verifying the pipeline's immune system.
//
// Root cause surfaces first. Noise is eliminated. Each layer is a precept.
//
// "For precept must be upon precept, precept upon precept;
//  line upon line, line upon line." — Isaiah 28:10
// ============================================================================
