// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R50/go/structure_test.ts
// purpose: R50 (file-level) detection tests for the Go format handler.
//          Tests block presence, order, and format detection — the coarsest
//          grain in the R[50]→R[5] detection hierarchy.
// source:  Split from tests/handlers/go_test.ts
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  fixture, lazyFormat, errors, warnings, infos,
  hasRule, hasMessage, hasErrorCode, errorCodeReport,
} from "../../helpers.ts";

// ============================================================================
// BODY
// ============================================================================

// Lazy handler — initializes on first property access, no setup test needed.
const go = lazyFormat("go");

// ---------------------------------------------------------------------------
// structure/ — Overall block structure
// ---------------------------------------------------------------------------

Deno.test("R50/structure/valid-library: zero errors", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("R50/structure/valid-library: zero warnings (all separators correct)", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const warns = warnings(results);
  assertEquals(warns.length, 0, `Expected 0 warnings, got ${warns.length}: ${JSON.stringify(warns, null, 2)}`);
});

Deno.test("R50/structure/valid-executable: zero errors", async () => {
  const results = await go.lint(fixture("go/structure/valid-executable.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("[CWS-R50-022] R50/structure/missing-block: produces block error", async () => {
  const results = await go.lint(fixture("go/structure/missing-block.go"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Expected at least one error for missing METADATA block");
  assert(
    hasRule(results, "block") || hasMessage(results, "METADATA") || hasMessage(results, "block"),
    "Expected error to reference missing block or METADATA",
  );
  // Error code: CWS-R50-022 (block/{{tag}} — missing block tag)
  assert(
    hasErrorCode(results, "CWS-R50-022"),
    `Expected CWS-R50-022 (missing block), got: ${errorCodeReport(errs)}`,
  );
});

Deno.test("[CWS-R50-020] R50/structure/wrong-block-order: produces order error", async () => {
  const results = await go.lint(fixture("go/structure/wrong-block-order.go"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Expected error for wrong block order (SETUP before METADATA)");
  assert(
    hasRule(results, "block/order"),
    `Expected block/order error, got rules: ${errs.map((e) => e.rule).join(", ")}`,
  );
  // Error code: CWS-R50-020 (block order violation — Layer 0)
  assert(
    hasErrorCode(results, "CWS-R50-020"),
    `Expected CWS-R50-020 (block order), got: ${errorCodeReport(errs)}`,
  );
});

// ---------------------------------------------------------------------------
// format/ — Format detection
// ---------------------------------------------------------------------------

Deno.test("[CWS-R50-072] R50/format/no-omni: info only, no errors", async () => {
  const results = await go.lint(fixture("go/format/no-omni.go"));
  const errs = errors(results);
  const warns = warnings(results);

  assertEquals(errs.length, 0, "Plain .go file should produce no errors");
  assertEquals(warns.length, 0, "Plain .go file should produce no warnings");

  // Should have at least one info saying it's not a 4-block file
  const infoResults = infos(results);
  assertGreater(infoResults.length, 0, "Should produce info about not being a 4-block file");
  // Error code: CWS-R50-072 (structure/skip — Layer 0)
  assert(
    hasErrorCode(results, "CWS-R50-072"),
    `Expected CWS-R50-072 (structure skip), got: ${errorCodeReport(results)}`,
  );
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R50 Go structure tests — 6 tests covering file-level detection:
// block presence, block order, format detection.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
