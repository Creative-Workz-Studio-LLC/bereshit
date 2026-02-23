// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R50/rust/structure_test.ts
// purpose: Rust R[50] file-level detection tests — overall structure, block
//          presence, block order, and format detection.
// source:  Split from tests/handlers/rust_test.ts
// layer:   R[50] — File-level detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  fixture, lazyFormat, errors, warnings, infos, byRule, hasRule, hasMessage,
  hasErrorCode, errorCodeReport,
} from "../../helpers.ts";

// ============================================================================
// BODY
// ============================================================================

// Lazy handler — initializes on first property access, no setup test needed.
const rust = lazyFormat("rust");

// ---------------------------------------------------------------------------
// structure/ — Overall block structure
// ---------------------------------------------------------------------------

Deno.test("R50/structure/valid-library: zero errors", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("R50/structure/valid-library: may have separator width warnings", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  // Warnings about separator widths are acceptable — not errors.
  for (const w of warnings(results)) {
    assert(
      w.rule.includes("separator") || w.rule.includes("width") || w.rule.includes("style"),
      `Unexpected warning rule: ${w.rule} — ${w.message}`,
    );
  }
});

Deno.test("R50/structure/valid-module: zero errors", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-module.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("R50/structure/valid-module: subtype detected as module", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-module.rs"));
  // Module subtype should be recognized — no "Unknown subtype" warnings
  const subtypeWarns = results.filter((r) => r.rule.includes("I2.subtype-value"));
  assertEquals(subtypeWarns.length, 0, `Module subtype should be recognized: ${JSON.stringify(subtypeWarns)}`);
});

Deno.test("R50/structure/valid-executable: zero errors", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-executable.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("[CWS-R50-022] R50/structure/missing-block: produces block error", async () => {
  const results = await rust.lint(fixture("rust/structure/missing-block.rs"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Expected at least one error for missing METADATA block");
  // Should mention the missing block
  assert(
    hasRule(results, "block") || hasMessage(results, "METADATA") || hasMessage(results, "block"),
    "Expected error to reference missing block or METADATA",
  );
  // Error code: CWS-R50-022 (block/{{tag}} — missing block tag, Layer 0)
  assert(
    hasErrorCode(results, "CWS-R50-022"),
    `Expected CWS-R50-022 (missing block), got: ${errorCodeReport(errs)}`,
  );
});

Deno.test("[CWS-R50-020] R50/structure/wrong-block-order: produces order error", async () => {
  const results = await rust.lint(fixture("rust/structure/wrong-block-order.rs"));
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

Deno.test("[CWS-R50-073] R50/format/no-omni: info only, no errors", async () => {
  const results = await rust.lint(fixture("rust/format/no-omni.rs"));
  const errs = errors(results);
  const warns = warnings(results);

  assertEquals(errs.length, 0, "Plain .rs file should produce no errors");
  assertEquals(warns.length, 0, "Plain .rs file should produce no warnings");

  // Should have at least one info saying it's not a 4-block file
  const infoResults = infos(results);
  assertGreater(infoResults.length, 0, "Should produce info about not being a 4-block file");
  // Error code: CWS-R50-073 (structure/module — Layer 0)
  assert(
    hasErrorCode(results, "CWS-R50-073"),
    `Expected CWS-R50-073 (structure module), got: ${errorCodeReport(results)}`,
  );
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[50] Rust structure tests — file-level detection layer.
// 8 tests covering: valid files (3 subtypes), missing block, wrong order, format.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
