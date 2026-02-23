// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R10/go/content_test.ts
// purpose: R10 (content/placement) detection tests for the Go format handler.
//          Tests content placement between blocks, metadata field presence,
//          subsection placement, closing zone placement, emphasis balance,
//          and X6 template-only checks.
// source:  Split from tests/handlers/go_test.ts
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import {
  fixture, lazyFormat, errors, warnings, infos, byRule,
  hasRule, hasMessage, hasErrorCode, errorCodeReport,
} from "../../helpers.ts";

// ============================================================================
// BODY
// ============================================================================

const go = lazyFormat("go");

// ---------------------------------------------------------------------------
// structure/ — Content placement between blocks
// ---------------------------------------------------------------------------

Deno.test("R10/structure/content-placement-correct: zero placement warnings", async () => {
  const results = await go.lint(fixture("go/structure/content-placement-correct.go"));
  const errs = errors(results);
  const placementWarns = byRule(results, "content/");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(placementWarns.length, 0, `Expected 0 content placement warnings: ${JSON.stringify(placementWarns, null, 2)}`);
});

Deno.test("[CWS-R10-002] R10/structure/content-placement-wrong: detects func in SETUP and import in BODY", async () => {
  const results = await go.lint(fixture("go/structure/content-placement-wrong.go"));
  const placementWarns = byRule(results, "content/block-placement");
  assertGreater(placementWarns.length, 0, "Should detect at least one block-placement violation");
  // Verify specific violations
  assert(
    hasMessage(results, "func_decl") && hasMessage(results, "SETUP"),
    "Should flag func_decl in SETUP block",
  );
  assert(
    hasMessage(results, "import_decl") && hasMessage(results, "BODY"),
    "Should flag import_decl in BODY block",
  );
  // Error code: CWS-R10-002 (content/block-placement — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-002"),
    `Expected CWS-R10-002 (block placement), got: ${errorCodeReport(placementWarns)}`,
  );
});

// ---------------------------------------------------------------------------
// metadata/ — Field presence and identity registration
// ---------------------------------------------------------------------------

Deno.test("[CWS-R10-011] R10/metadata/missing-ic-fields: warns about missing required fields", async () => {
  const results = await go.lint(fixture("go/metadata/missing-ic-fields.go"));
  const warns = warnings(results);

  // Missing from Pragma: I1.key, I1.from, I2.type
  // Missing from Metadata: C1.version, C2.organization, C3.scripture, C4.consumers
  // That's 7 required field warnings minimum
  assertGreater(warns.length, 6, `Expected 7+ warnings for missing required I/C fields, got ${warns.length}: ${JSON.stringify(warns, null, 2)}`);

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
  const results = await go.lint(fixture("go/metadata/missing-ic-fields.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("[CWS-R10-012] R10/metadata/placeholder-values: warns about template placeholders", async () => {
  const results = await go.lint(fixture("go/metadata/placeholder-values.go"));
  assert(
    hasRule(results, "placeholder") || hasRule(results, "template") || hasMessage(results, "placeholder") || hasMessage(results, "["),
    "Should detect placeholder values in I/C fields",
  );
  // Error code: CWS-R10-012 (identity/{{varName}}/placeholder — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-012"),
    `Expected CWS-R10-012 (placeholder), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R10-001] R10/metadata/metadata-leak: detects code declarations in METADATA block", async () => {
  const results = await go.lint(fixture("go/metadata/metadata-leak.go"));
  const leakWarns = byRule(results, "content/metadata-leak");
  assertGreater(leakWarns.length, 0, "Should detect code declarations in METADATA block");
  assert(
    hasMessage(results, "import_decl") || hasMessage(results, "type_decl"),
    "Should identify the type of leaked construct",
  );
  // Error code: CWS-R10-001 (content/metadata-leak — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-001"),
    `Expected CWS-R10-001 (metadata leak), got: ${errorCodeReport(leakWarns)}`,
  );
});

Deno.test("[CWS-R10-010] R10/metadata/identity-registration: info when Pragma exists but no init()", async () => {
  const results = await go.lint(fixture("go/metadata/identity-registration.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, "Identity registration should not produce errors");
  assert(
    hasRule(results, "identity/register"),
    "Should produce identity/register info",
  );
  const regInfos = infos(results).filter((r) => r.rule.includes("identity/register"));
  assertGreater(regInfos.length, 0, "Should have at least one identity registration info");
  // Error code: CWS-R10-010 (identity/register — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-010"),
    `Expected CWS-R10-010 (identity register), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("R10/metadata/valid-library: identity registration info (has Pragma, no init())", async () => {
  // The valid-library doesn't have init() — so it SHOULD get the info.
  // This test documents the behavior rather than expecting absence.
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  // File has Pragma var, so identity/register check runs
  const regInfos = infos(results).filter((r) => r.rule.includes("identity/register"));
  // Valid library fixture has no init() — info is expected and correct
  assertEquals(regInfos.length, 1, "Valid library with Pragma but no init() should get 1 identity/register info");
});

// ---------------------------------------------------------------------------
// setup/ — Content within SETUP subsections
// ---------------------------------------------------------------------------

Deno.test("[CWS-R10-003] R10/content/subsection-placement: detects misplaced declarations within SETUP subsections", async () => {
  const results = await go.lint(fixture("go/setup/misplaced-content.go"));
  const placementInfos = byRule(results, "content/subsection-placement");
  // Fixture has: type_decl in Constants (should be CoreTypes),
  // const_decl in CoreTypes (should be Constants) = 2 misplacements
  assertGreater(placementInfos.length, 0, `Expected subsection-placement infos, got: ${JSON.stringify(placementInfos, null, 2)}`);
  // Error code: CWS-R10-003 (content/subsection-placement — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-003"),
    `Expected CWS-R10-003 (subsection placement), got: ${errorCodeReport(placementInfos)}`,
  );
  // Verify it reports both directions of misplacement
  const messages = placementInfos.map((r: { message: string }) => r.message).join(" | ");
  assert(messages.includes("CoreTypes"), `Should suggest CoreTypes for type_decl: ${messages}`);
  assert(messages.includes("Constants"), `Should suggest Constants for const_decl: ${messages}`);

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
// closing/ — Test and main placement
// ---------------------------------------------------------------------------

Deno.test("[CWS-R10-004] R10/closing/tests-in-body: detects Test func in BODY — should be CLOSING Cv", async () => {
  const results = await go.lint(fixture("go/closing/tests-in-body.go"));
  const errs = errors(results);
  const testWarns = byRule(results, "closing/test-placement");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(testWarns.length, 0, "Should detect test function in BODY block");
  // Error code: CWS-R10-004 (closing/test-placement — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-004"),
    `Expected CWS-R10-004 (test placement), got: ${errorCodeReport(testWarns)}`,
  );
  assert(
    hasMessage(results, "CLOSING Cv") || hasMessage(results, "Cv zone"),
    "Should guide toward CLOSING Cv zone",
  );
});

Deno.test("[CWS-R10-005] R10/closing/main-in-body: detects func main() in BODY — should be CLOSING Ce", async () => {
  const results = await go.lint(fixture("go/closing/main-in-body.go"));
  const errs = errors(results);
  const mainWarns = byRule(results, "closing/main-placement");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(mainWarns.length, 0, "Should detect func main() in BODY block");
  // Error code: CWS-R10-005 (closing/main-placement — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-005"),
    `Expected CWS-R10-005 (main placement), got: ${errorCodeReport(mainWarns)}`,
  );
  assert(
    hasMessage(results, "CLOSING Ce") || hasMessage(results, "Ce zone"),
    "Should guide toward CLOSING Ce zone",
  );
});

// ---------------------------------------------------------------------------
// closing/ — X6 template-only check
// ---------------------------------------------------------------------------

Deno.test("[CWS-R10-006] R10/closing/X6-template-only: detects X6 section in non-template file", async () => {
  const results = await go.lint(fixture("go/closing/x6-in-derived.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  assert(hasRule(results, "closing/X6-template-only"), "Should detect X6 in non-template file");
  // Error code: CWS-R10-006 (closing/X6-template-only — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-006"),
    `Expected CWS-R10-006 (X6 template only), got: ${errorCodeReport(results)}`,
  );

  const x6 = byRule(results, "closing/X6-template-only");
  assertEquals(x6[0]!.severity, "warn", "X6-template-only should be warn-level");
  assert(hasMessage(x6, "Template Guide"), "Should mention Template Guide");
});

Deno.test("R10/closing/X6-template-only: valid-library has no X6 warning", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const x6 = byRule(results, "closing/X6-template-only");
  assertEquals(x6.length, 0, "Valid library without X6 should produce no X6-template-only warning");
});

// ---------------------------------------------------------------------------
// emphasis/ — Subtype emphasis balance
// ---------------------------------------------------------------------------

Deno.test("[CWS-R10-040] R10/setup/emphasis-inverted: library with empty heavy subs and full light subs", async () => {
  const results = await go.lint(fixture("go/setup/emphasis-inverted.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

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

Deno.test("R10/setup/emphasis-inverted: valid library has no emphasis inversion", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const emphasis = byRule(results, "emphasis/setup-inverted");
  assertEquals(emphasis.length, 0, "Valid library should have no emphasis inversion");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R10 Go content/placement tests — 15 tests covering:
// content placement between blocks, metadata field presence,
// identity registration, subsection placement, closing zone placement,
// X6 template-only, emphasis balance.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
