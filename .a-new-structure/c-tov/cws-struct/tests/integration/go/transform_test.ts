// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/go/transform_test.ts
// purpose: Transform pipeline tests for the Go format handler.
//          Tests dry-run detection and actual file transforms for
//          move-tests, move-main, and reorder-closing operations.
// source:  Split from tests/handlers/go_test.ts
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import {
  fixture, lazyFormat, byRule, hasRule,
} from "@test/helpers.ts";

// ============================================================================
// BODY
// ============================================================================

const go = lazyFormat("go");

// ---------------------------------------------------------------------------
// transform/ — Valid files need no transforms
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-112] transform/valid-library: no changes needed (dry-run)", async () => {
  const f = fixture("go/structure/valid-library.go");
  const results = await go.transform!(f, { dryRun: true, extensions: false, force: false, steps: false });
  const moves = byRule(results, "move-tests").concat(
    byRule(results, "move-main"),
    byRule(results, "reorder-closing"),
  );
  assertEquals(moves.length, 0, "Valid library should need no structural transforms");
});

Deno.test("transform/valid-executable: no changes needed (dry-run)", async () => {
  const f = fixture("go/structure/valid-executable.go");
  const results = await go.transform!(f, { dryRun: true, extensions: false, force: false, steps: false });
  const moves = byRule(results, "move-tests").concat(
    byRule(results, "move-main"),
    byRule(results, "reorder-closing"),
  );
  assertEquals(moves.length, 0, "Valid executable should need no structural transforms");
});

// ---------------------------------------------------------------------------
// transform/ — move-tests
// ---------------------------------------------------------------------------

Deno.test("transform/move-tests: dry-run detects Test func in BODY", async () => {
  const f = fixture("go/closing/tests-in-body.go");
  const results = await go.transform!(f, { dryRun: true, extensions: false, force: false, steps: false });
  assert(hasRule(results, "move-tests"), "Should report move-tests for test func in BODY");
});

Deno.test("transform/move-tests: actual transform moves test to CLOSING Cv", async () => {
  const src = fixture("go/closing/tests-in-body.go");
  const tmp = await Deno.makeTempFile({ suffix: ".go" });
  await Deno.copyFile(src, tmp);

  try {
    const results = await go.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    assert(hasRule(results, "move-tests"), "Should report move-tests");
    assert(hasRule(results, "written"), "Should write file");

    // Lint the transformed file — should have NO test-placement warning
    const lintResults = await go.lint(tmp);
    const testPlacement = byRule(lintResults, "test-placement");
    assertEquals(testPlacement.length, 0, "Transformed file should have no test-placement warnings");
  } finally {
    await Deno.remove(tmp);
  }
});

// ---------------------------------------------------------------------------
// transform/ — move-main
// ---------------------------------------------------------------------------

Deno.test("transform/move-main: dry-run detects func main() in BODY", async () => {
  const f = fixture("go/closing/main-in-body.go");
  const results = await go.transform!(f, { dryRun: true, extensions: false, force: false, steps: false });
  assert(hasRule(results, "move-main"), "Should report move-main for func main() in BODY");
});

Deno.test("transform/move-main: actual transform moves main to CLOSING Ce", async () => {
  const src = fixture("go/closing/main-in-body.go");
  const tmp = await Deno.makeTempFile({ suffix: ".go" });
  await Deno.copyFile(src, tmp);

  try {
    const results = await go.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    assert(hasRule(results, "move-main"), "Should report move-main");
    assert(hasRule(results, "written"), "Should write file");

    // Lint the transformed file — should have NO main-placement warning
    const lintResults = await go.lint(tmp);
    const mainPlacement = byRule(lintResults, "main-placement");
    assertEquals(mainPlacement.length, 0, "Transformed file should have no main-placement warnings");
  } finally {
    await Deno.remove(tmp);
  }
});

// ---------------------------------------------------------------------------
// transform/ — reorder-closing
// ---------------------------------------------------------------------------

Deno.test("transform/reorder-closing: dry-run detects out-of-order zones", async () => {
  const f = fixture("go/closing/wrong-closing-order.go");
  const results = await go.transform!(f, { dryRun: true, extensions: false, force: false, steps: false });
  assert(hasRule(results, "reorder-closing"), "Should detect out-of-order CLOSING zones");
});

Deno.test("transform/reorder-closing: actual transform fixes zone order", async () => {
  const src = fixture("go/closing/wrong-closing-order.go");
  const tmp = await Deno.makeTempFile({ suffix: ".go" });
  await Deno.copyFile(src, tmp);

  try {
    const results = await go.transform!(tmp, { dryRun: false, extensions: false, force: false, steps: false });
    assert(hasRule(results, "reorder-closing"), "Should report reorder-closing");
    assert(hasRule(results, "written"), "Should write file");

    // Lint the transformed file — should have NO zone-order warning
    const lintResults = await go.lint(tmp);
    const zoneOrder = byRule(lintResults, "closing/zone-order");
    assertEquals(zoneOrder.length, 0, "Transformed file should have no zone-order warnings");
  } finally {
    await Deno.remove(tmp);
  }
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Go transform pipeline tests — 8 tests covering:
// dry-run detection, actual file transforms for move-tests,
// move-main, and reorder-closing. Each actual transform is
// verified by re-linting the output.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
