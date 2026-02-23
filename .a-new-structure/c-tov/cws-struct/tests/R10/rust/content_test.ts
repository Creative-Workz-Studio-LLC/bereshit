// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R10/rust/content_test.ts
// purpose: Rust R[10] content/placement tests — content in wrong blocks,
//          metadata leaks, subsection placement, emphasis, scaling, test/main
//          placement, X6 template-only checks.
// source:  Split from tests/handlers/rust_test.ts
// layer:   R[10] — Content/placement detection
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  fixture, lazyFormat, errors, warnings, byRule, hasRule, hasMessage,
  hasErrorCode, errorCodeReport,
} from "../../helpers.ts";

// ============================================================================
// BODY
// ============================================================================

const rust = lazyFormat("rust");

// ---------------------------------------------------------------------------
// structure/ — Content placement within blocks
// ---------------------------------------------------------------------------

Deno.test("R10/structure/content-placement-correct: zero placement warnings", async () => {
  const results = await rust.lint(fixture("rust/structure/content-placement-correct.rs"));
  const errs = errors(results);
  const placementWarns = byRule(results, "content/block-placement");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(placementWarns.length, 0, `Expected 0 content placement warnings: ${JSON.stringify(placementWarns, null, 2)}`);
});

Deno.test("[CWS-R10-002] R10/structure/content-placement-wrong: detects fn in SETUP and use/struct in BODY", async () => {
  const results = await rust.lint(fixture("rust/structure/content-placement-wrong.rs"));
  const placementWarns = byRule(results, "content/block-placement");
  // fn in SETUP, use in BODY, struct in BODY = 3 block-placement warnings
  assertGreater(placementWarns.length, 0, "Should detect at least one block-placement violation");
  // Verify specific violations are named
  assert(
    hasMessage(results, "fn_decl") && hasMessage(results, "SETUP"),
    "Should flag fn_decl in SETUP block",
  );
  assert(
    hasMessage(results, "use_decl") && hasMessage(results, "BODY"),
    "Should flag use_decl in BODY block",
  );
  // Error code: CWS-R10-002 (content/block-placement — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-002"),
    `Expected CWS-R10-002 (block placement), got: ${errorCodeReport(placementWarns)}`,
  );
});

Deno.test("R10/structure/scaling: small fixture does not trigger scaling signals", async () => {
  const f = fixture("rust/setup/subsection-order-correct.rs");
  const results = await rust.lint(f);
  const scaling = byRule(results, "structure/scaling");
  assertEquals(scaling.length, 0, "Small fixture should not trigger scaling signals");
});

// ---------------------------------------------------------------------------
// metadata/ — METADATA block content checks
// ---------------------------------------------------------------------------

Deno.test("[CWS-R10-011] R10/metadata/missing-ic-fields: warns about missing required fields", async () => {
  const results = await rust.lint(fixture("rust/metadata/missing-ic-crate/lib.rs"));
  const warns = warnings(results);

  // Missing from PRAGMA: I1.key, I1.from, I2.type
  // Missing from METADATA: C1.version, C2.organization, C3.scripture, C4.consumers
  // That's 7 required field warnings minimum
  assertGreater(warns.length, 6, "Expected 7+ warnings for missing required I/C fields");

  // Check specific missing fields
  assert(hasRule(results, "I1.key"), "Should warn about missing I1.key");
  assert(hasRule(results, "I1.from"), "Should warn about missing I1.from");
  assert(hasRule(results, "I2.type"), "Should warn about missing I2.type");
  assert(hasRule(results, "C1.version"), "Should warn about missing C1.version");
  assert(hasRule(results, "C2.organization"), "Should warn about missing C2.organization");
  assert(hasRule(results, "C3.scripture"), "Should warn about missing C3.scripture");
  assert(hasRule(results, "C4.consumers"), "Should warn about missing C4.consumers");
  // Error code: CWS-R10-011 (identity/{{varName}}/{{group}}.{{field}} — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-011"),
    `Expected CWS-R10-011 (identity field), got: ${errorCodeReport(warns)}`,
  );
});

Deno.test("R10/metadata/missing-ic-fields: zero errors (missing fields are warnings, not errors)", async () => {
  const results = await rust.lint(fixture("rust/metadata/missing-ic-crate/lib.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("[CWS-R05-012] R10/metadata/placeholder-values: warns about template placeholders", async () => {
  const results = await rust.lint(fixture("rust/metadata/placeholder-values.rs"));
  // Should detect [bracketed] placeholder values
  assert(
    hasRule(results, "placeholder") || hasRule(results, "template") || hasMessage(results, "placeholder") || hasMessage(results, "["),
    "Should detect placeholder values in I/C fields",
  );
  // Error code: CWS-R05-012 (template/placeholders — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-012"),
    `Expected CWS-R05-012 (template placeholders), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R10-001] R10/metadata/metadata-leak: detects code declarations in METADATA block", async () => {
  const results = await rust.lint(fixture("rust/metadata/metadata-leak.rs"));
  const leakWarns = byRule(results, "content/metadata-leak");
  assertGreater(leakWarns.length, 0, "Should detect code declarations in METADATA block");
  assert(
    hasMessage(results, "use_decl") || hasMessage(results, "struct_decl"),
    "Should identify the type of leaked construct",
  );
  // Error code: CWS-R10-001 (content/metadata-leak — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-001"),
    `Expected CWS-R10-001 (metadata leak), got: ${errorCodeReport(leakWarns)}`,
  );
});

// ---------------------------------------------------------------------------
// content/ — Subsection placement within SETUP
// ---------------------------------------------------------------------------

Deno.test("[CWS-R10-003] R10/content/subsection-placement: detects misplaced declarations within SETUP subsections", async () => {
  const results = await rust.lint(fixture("rust/setup/misplaced-content.rs"));
  const placementInfos = byRule(results, "content/subsection-placement");
  // Fixture has: struct_decl in Constants (should be CoreTypes),
  // const_decl in CoreTypes (should be Constants) = 2 misplacements
  assertGreater(placementInfos.length, 0, `Expected subsection-placement infos, got: ${JSON.stringify(placementInfos, null, 2)}`);
  // Error code: CWS-R10-003 (content/subsection-placement — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-003"),
    `Expected CWS-R10-003 (subsection placement), got: ${errorCodeReport(placementInfos)}`,
  );
  // Verify it reports both directions of misplacement
  const messages = placementInfos.map((r: { message: string }) => r.message).join(" | ");
  assert(messages.includes("CoreTypes"), `Should suggest CoreTypes for struct: ${messages}`);
  assert(messages.includes("Constants"), `Should suggest Constants for const: ${messages}`);

  // Phase 9: Auto-move suggestions — fix should contain remove+insert actions
  const withFix = placementInfos.filter((r: { fix?: unknown }) => r.fix);
  assertGreater(withFix.length, 0, "At least one placement info should include a fix suggestion");
  // deno-lint-ignore no-explicit-any
  const fix = (withFix[0] as any).fix;
  assertEquals(fix.actions.length, 2, "Fix should have 2 actions (remove + insert)");
  assertEquals(fix.actions[0].type, "remove", "First action should be remove");
  assertEquals(fix.actions[1].type, "insert", "Second action should be insert");
  assert(fix.actions[1].content.length > 0, "Insert action should have content lines");
});

// ---------------------------------------------------------------------------
// setup/ — Emphasis checks
// ---------------------------------------------------------------------------

Deno.test("[CWS-R10-040] R10/setup/emphasis-inverted: library with empty heavy subs and full light subs", async () => {
  const f = fixture("rust/setup/emphasis-inverted.rs");
  const results = await rust.lint(f);

  const emphasis = byRule(results, "emphasis/setup-inverted");
  assertGreater(emphasis.length, 0, "Should detect inverted emphasis");
  assertEquals(emphasis[0]!.severity, "info", "Emphasis check should be info-level");
  // Error code: CWS-R10-040 (emphasis/setup-inverted — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-040"),
    `Expected CWS-R10-040 (emphasis inverted), got: ${errorCodeReport(emphasis)}`,
  );
  assert(hasMessage(emphasis, "library"), "Should mention the subtype");
});

// ---------------------------------------------------------------------------
// closing/ — Test/main placement and X6 template-only
// ---------------------------------------------------------------------------

Deno.test("[CWS-R10-004] R10/closing/tests-in-body: detects #[cfg(test)] in BODY — should be CLOSING Cv", async () => {
  const results = await rust.lint(fixture("rust/closing/tests-in-body.rs"));
  const errs = errors(results);
  const testWarns = byRule(results, "closing/test-placement");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(testWarns.length, 0, "Should detect test module in BODY block");
  assert(
    hasMessage(results, "CLOSING Cv") || hasMessage(results, "Cv zone"),
    "Should guide toward CLOSING Cv zone",
  );

  // Error code: CWS-R10-004 (test placement — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-004"),
    `Expected CWS-R10-004 (test placement), got: ${errorCodeReport(testWarns)}`,
  );
});

Deno.test("[CWS-R10-005] R10/closing/main-in-body: detects fn main() in BODY — should be CLOSING Ce", async () => {
  const results = await rust.lint(fixture("rust/closing/main-in-body.rs"));
  const errs = errors(results);
  const mainWarns = byRule(results, "closing/main-placement");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(mainWarns.length, 0, "Should detect fn main() in BODY block");
  assert(
    hasMessage(results, "CLOSING Ce") || hasMessage(results, "Ce zone"),
    "Should guide toward CLOSING Ce zone",
  );

  // Error code: CWS-R10-005 (main placement — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-005"),
    `Expected CWS-R10-005 (main placement), got: ${errorCodeReport(mainWarns)}`,
  );
});

Deno.test("[CWS-R10-006] R10/closing/X6-template-only: detects X6 section in non-template file", async () => {
  const results = await rust.lint(fixture("rust/closing/x6-in-derived.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  assert(hasRule(results, "closing/X6-template-only"), "Should detect X6 in non-template file");

  const x6 = byRule(results, "closing/X6-template-only");
  assertEquals(x6[0]!.severity, "warn", "X6-template-only should be warn-level");
  assert(hasMessage(x6, "Template Guide"), "Should mention Template Guide");

  // Error code: CWS-R10-006 (X6 template-only — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-006"),
    `Expected CWS-R10-006 (X6 template-only), got: ${errorCodeReport(x6)}`,
  );
});

Deno.test("R10/closing/X6-template-only: valid-library has no X6 warning", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const x6 = byRule(results, "closing/X6-template-only");
  assertEquals(x6.length, 0, "Valid library without X6 should produce no X6-template-only warning");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[10] Rust content/placement tests — content in wrong blocks, metadata leaks,
// subsection placement, emphasis, scaling, test/main placement, X6 checks.
// 13 tests covering: structure (3), metadata (4), content (1), setup (1),
// closing (4).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
