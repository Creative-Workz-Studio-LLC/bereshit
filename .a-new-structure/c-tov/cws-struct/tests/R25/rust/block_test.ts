// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R25/rust/block_test.ts
// purpose: Rust R[25] block-level detection tests — subsection order, doc
//          comments, consistency checks, required subsections, closing zones.
// source:  Split from tests/handlers/rust_test.ts
// layer:   R[25] — Block-level detection
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

const rust = lazyFormat("rust");

// ---------------------------------------------------------------------------
// consistency/ — Cross-cutting validation (pragma-driven checks)
// ---------------------------------------------------------------------------

Deno.test("[CWS-R10-030] R25/consistency/template-with-derived-values: detects wrong I1.at and C1.status for template", async () => {
  const results = await rust.lint(fixture("rust/metadata/template-with-derived-values.rs"));
  const templateAt = byRule(results, "content/PRAGMA/template-at");
  const templateStatus = byRule(results, "content/METADATA/template-status");
  assertGreater(templateAt.length, 0,
    `Template file with I1.at="a-01.00" should trigger content/PRAGMA/template-at`);
  assertGreater(templateStatus.length, 0,
    `Template file with C1.status="Active" should trigger content/METADATA/template-status`);
  // Error code: CWS-R10-030 (content/PRAGMA/template-at — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-030"),
    `Expected CWS-R10-030 (template-at), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R10-032] R25/consistency/derived-with-template-values: detects template values in derived file", async () => {
  const results = await rust.lint(fixture("rust/metadata/derived-with-template-values.rs"));
  const derivedAt = byRule(results, "content/PRAGMA/derived-at");
  const derivedStatus = byRule(results, "content/METADATA/derived-status");
  assertGreater(derivedAt.length, 0,
    `Derived file with I1.at="template" should trigger content/PRAGMA/derived-at`);
  assertGreater(derivedStatus.length, 0,
    `Derived file with C1.status="Template" should trigger content/METADATA/derived-status`);
  // Error code: CWS-R10-032 (content/PRAGMA/derived-at — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-032"),
    `Expected CWS-R10-032 (derived-at), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R10-034] R25/consistency/wrong-subtype: detects I2.subtype mismatch with pragma", async () => {
  const results = await rust.lint(fixture("rust/metadata/wrong-subtype.rs"));
  const subtypeMismatch = byRule(results, "content/PRAGMA/subtype-mismatch");
  assertGreater(subtypeMismatch.length, 0,
    `Pragma says -library but I2.subtype="executable" should trigger content/PRAGMA/subtype-mismatch`);
  // Error code: CWS-R10-034 (content/PRAGMA/subtype-mismatch — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-034"),
    `Expected CWS-R10-034 (subtype mismatch), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("R25/consistency/valid-library: no content-aware false positives", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const contentPragma = byRule(results, "content/PRAGMA/");
  const contentMeta = byRule(results, "content/METADATA/");
  assertEquals(contentPragma.length, 0,
    `Valid library should have 0 content/PRAGMA infos: ${JSON.stringify(contentPragma.map(r => r.rule))}`);
  assertEquals(contentMeta.length, 0,
    `Valid library should have 0 content/METADATA infos: ${JSON.stringify(contentMeta.map(r => r.rule))}`);
});

// ---------------------------------------------------------------------------
// doc/ — Doc comment quality checks (schema-driven)
// ---------------------------------------------------------------------------

Deno.test("R25/doc/valid-library: no doc comment warnings (has //! doc comments)", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const docRules = byRule(results, "doc/");
  assertEquals(docRules.length, 0,
    `Valid library should have 0 doc warnings: ${JSON.stringify(docRules.map(r => r.rule))}`);
});

Deno.test("R25/doc/template-with-doc-comments: no doc/template info (template has //!)", async () => {
  const results = await rust.lint(fixture("rust/metadata/placeholder-values.rs"));
  const templateDoc = byRule(results, "doc/template");
  assertEquals(templateDoc.length, 0,
    "Template with //! doc comments should not trigger doc/template");
});

Deno.test("R25/doc/schema-driven-severity: crate root severity from schema is warn", async () => {
  // Verify schema extraction produces the expected severity values.
  const { loadCodeRules } = await import("../../../lib/foundation/mod.ts");
  const rules = await loadCodeRules("rust");
  const crateExp = rules.docCommentExpectations["crate_root"];
  const moduleExp = rules.docCommentExpectations["module"];
  assertEquals(crateExp?.severity, "warn", "crate_root severity from schema");
  assertEquals(moduleExp?.severity, "info", "module severity from schema");
});

// ---------------------------------------------------------------------------
// setup/ — SETUP block subsection order and required subsections
// ---------------------------------------------------------------------------

Deno.test("R25/setup/subsection-order-correct: all 10 subsections in correct order — zero order warnings", async () => {
  const results = await rust.lint(fixture("rust/setup/subsection-order-correct.rs"));
  const errs = errors(results);
  const orderWarns = byRule(results, "setup/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(orderWarns.length, 0, `Expected 0 subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

Deno.test("[CWS-R25-023] R25/setup/subsection-order-wrong: detects Core Types before Constants", async () => {
  const results = await rust.lint(fixture("rust/setup/subsection-order-wrong.rs"));
  assert(
    hasRule(results, "setup/subsection-order"),
    `Should detect wrong SETUP subsection order, got rules: ${warnings(results).map((w) => w.rule).join(", ")}`,
  );
  // Error code: CWS-R25-023 (setup/subsection-order — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-023"),
    `Expected CWS-R25-023 (setup order), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("R25/setup/subsection-partial-correct: 3 of 10 in correct relative order — zero order warnings", async () => {
  const results = await rust.lint(fixture("rust/setup/subsection-partial-correct.rs"));
  const errs = errors(results);
  const orderWarns = byRule(results, "setup/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(orderWarns.length, 0, `Expected 0 subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

Deno.test("[CWS-R10-043] R25/setup/alias-name: detects alias names and suggests canonical alternatives", async () => {
  const results = await rust.lint(fixture("rust/setup/aliased-subsection-names.rs"));
  const aliasInfos = byRule(results, "setup/alias-name");
  // Fixture uses: Consts (→Constants), Errors (→ErrorTypes), Types (→CoreTypes),
  // Traits (→TraitDefs) = 4 aliases
  assertGreater(aliasInfos.length, 0, `Expected alias info diagnostics, got: ${JSON.stringify(aliasInfos, null, 2)}`);
  // Should NOT have order warnings — aliases are in correct order
  const orderWarns = byRule(results, "setup/subsection-order");
  assertEquals(orderWarns.length, 0, `Aliased names in correct order should produce 0 order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
  // Error code: CWS-R10-043 (setup/alias-name — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-043"),
    `Expected CWS-R10-043 (alias name), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R25-030] R25/setup/missing-required-subsections: library missing Constants, ErrorTypes, CoreTypes", async () => {
  const f = fixture("rust/setup/missing-required-subsections.rs");
  const results = await rust.lint(f);

  const required = byRule(results, "setup/required-subsection");
  assertGreater(required.length, 0, "Should detect missing required SETUP subsections");
  assertEquals(required[0]!.severity, "warn", "Required subsection check should be warn-level");
  // Error code: CWS-R25-030 (setup/required-subsection — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-030"),
    `Expected CWS-R25-030 (required setup), got: ${errorCodeReport(required)}`,
  );
  // Library requires: Imports (present), Constants (missing), Error Types (missing), Core Types (missing)
  assert(hasMessage(required, "Constants") || hasMessage(required, "Error Types") || hasMessage(required, "Core Types"),
    "Should mention at least one missing required subsection");
});

Deno.test("R25/setup/missing-required-subsections: reports count matches schema", async () => {
  const f = fixture("rust/setup/missing-required-subsections.rs");
  const results = await rust.lint(f);
  const required = byRule(results, "setup/required-subsection");
  // Library requires: Imports (present), Constants (missing), Error Types (missing), Core Types (missing)
  assertEquals(required.length, 3, "Should report exactly 3 missing required subsections");
});

// ---------------------------------------------------------------------------
// body/ — BODY block subsection order and required subsections
// ---------------------------------------------------------------------------

Deno.test("[CWS-R25-024] R25/body/wrong-body-order: detects BODY subsections out of order", async () => {
  const results = await rust.lint(fixture("rust/body/wrong-body-order.rs"));
  const errs = errors(results);
  const orderWarns = byRule(results, "body/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(orderWarns.length, 0, "Should detect wrong BODY subsection order");
  // Error code: CWS-R25-024 (body/subsection-order — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-024"),
    `Expected CWS-R25-024 (body order), got: ${errorCodeReport(orderWarns)}`,
  );
  assert(
    hasMessage(results, "§1") && hasMessage(results, "§4"),
    "Should reference the misordered subsection numbers",
  );
});

Deno.test("R25/body/valid-library: zero body subsection order warnings", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const orderWarns = byRule(results, "body/subsection-order");
  assertEquals(orderWarns.length, 0, `Expected 0 body/subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

Deno.test("[CWS-R25-031] R25/body/missing-required-subsections: library missing all 7 required body subsections", async () => {
  const f = fixture("rust/body/missing-required-subsections.rs");
  const results = await rust.lint(f);

  const required = byRule(results, "body/required-subsection");
  assertGreater(required.length, 0, "Should detect missing required BODY subsections");
  assertEquals(required[0]!.severity, "warn", "Required body subsection check should be warn-level");

  // Error code: CWS-R25-031 (missing required body subsection — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-031"),
    `Expected CWS-R25-031 (required body sub), got: ${errorCodeReport(required)}`,
  );
});

Deno.test("R25/body/missing-required-subsections: reports count matches schema", async () => {
  const f = fixture("rust/body/missing-required-subsections.rs");
  const results = await rust.lint(f);
  const required = byRule(results, "body/required-subsection");
  // Library requires: Identity Access, Trait Implementations, Constructors,
  // Core Logic, Queries, Output Display, Free Functions = 7 required.
  // Fixture has only Helpers (reserved for library). All 7 missing.
  assertEquals(required.length, 7, "Should report exactly 7 missing required body subsections");
});

// ---------------------------------------------------------------------------
// closing/ — CLOSING block zone order and required zones
// ---------------------------------------------------------------------------

Deno.test("[CWS-R25-020] R25/closing/wrong-closing-order: detects code zone after documentation section", async () => {
  const results = await rust.lint(fixture("rust/closing/wrong-closing-order.rs"));
  const errs = errors(results);
  const zoneWarns = byRule(results, "closing/zone-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(zoneWarns.length, 0, "Should detect code zone after documentation section");
  assert(
    hasMessage(results, "X1") || hasMessage(results, "Cv") || hasMessage(results, "Cc"),
    "Should reference the misordered zones",
  );

  // Error code: CWS-R25-020 (closing zone order — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-020"),
    `Expected CWS-R25-020 (zone order), got: ${errorCodeReport(zoneWarns)}`,
  );
});

Deno.test("R25/closing/valid-library: zero closing zone order warnings", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const zoneWarns = byRule(results, "closing/");
  assertEquals(zoneWarns.length, 0, `Expected 0 closing zone warnings: ${JSON.stringify(zoneWarns, null, 2)}`);
});

Deno.test("[CWS-R25-032] R25/closing/required-zones: missing X1 and X5 documentation zones triggers info", async () => {
  const f = fixture("rust/closing/missing-required-zones.rs");
  const results = await rust.lint(f);
  const requiredX1 = byRule(results, "closing/required-X1");
  const requiredX5 = byRule(results, "closing/required-X5");
  assertGreater(requiredX1.length, 0, "Should detect missing X1 zone");
  assertGreater(requiredX5.length, 0, "Should detect missing X5 zone");
  assertEquals(requiredX1[0]!.severity, "info");
  assertEquals(requiredX5[0]!.severity, "info");

  // Error code: CWS-R25-032 (missing required closing zone — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-032"),
    `Expected CWS-R25-032 (required zone), got: ${errorCodeReport(results)}`,
  );
});

// ---------------------------------------------------------------------------
// identity/ — METADATA comment-meta block checks
// ---------------------------------------------------------------------------

Deno.test("[CWS-R25-017] R25/identity/missing-comment-meta-key: warns about missing Key: field", async () => {
  const results = await rust.lint(fixture("rust/metadata/missing-ic-crate/lib.rs"));
  const keyInfos = infos(results).filter((r) => r.rule === "comment-meta/key");
  assertGreater(keyInfos.length, 0, "Should report missing Key: in METADATA comment block");
  // Error code: CWS-R25-017 (comment-meta/key — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-017"),
    `Expected CWS-R25-017 (comment-meta key), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R25-018] R25/identity/missing-comment-meta-purpose: warns about missing Purpose: field", async () => {
  const results = await rust.lint(fixture("rust/metadata/missing-ic-crate/lib.rs"));
  const purposeInfos = infos(results).filter((r) => r.rule === "comment-meta/purpose");
  assertGreater(purposeInfos.length, 0, "Should report missing Purpose: in METADATA comment block");
  // Error code: CWS-R25-018 (comment-meta/purpose — Layer 1)
  assert(
    hasErrorCode(results, "CWS-R25-018"),
    `Expected CWS-R25-018 (comment-meta purpose), got: ${errorCodeReport(results)}`,
  );
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R[25] Rust block-level detection tests — subsection order, doc quality,
// consistency, required subsections, closing zones, identity comment-meta.
// 22 tests covering: consistency (4), doc (3), setup (6), body (4),
// closing (3), identity (2).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
