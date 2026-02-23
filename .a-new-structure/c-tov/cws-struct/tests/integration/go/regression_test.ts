// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/go/regression_test.ts
// purpose: Regression tests for the Go format handler.
//          Verifies that valid fixtures remain clean after new check additions.
//          These tests guard against false positives introduced by new rules.
// source:  Split from tests/handlers/go_test.ts
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals } from "jsr:@std/assert";
import {
  fixture, lazyFormat, errors, warnings,
} from "../../helpers.ts";

// ============================================================================
// BODY
// ============================================================================

const go = lazyFormat("go");

// ---------------------------------------------------------------------------
// regression/ — Existing fixtures still pass after new checks
// ---------------------------------------------------------------------------

Deno.test("regression/valid-library: still zero errors after content validation additions", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, "Valid library should still have zero errors");
  const warns = warnings(results);
  assertEquals(warns.length, 0, "Valid library should still have zero warnings");
});

Deno.test("regression/valid-executable: still zero errors after content validation additions", async () => {
  const results = await go.lint(fixture("go/structure/valid-executable.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, "Valid executable should still have zero errors");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Go regression tests — 2 tests guarding valid fixtures against
// false positives from newly added detection rules.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
