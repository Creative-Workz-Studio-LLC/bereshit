// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/handlers/rust_test.ts
// key:     B-tov-cws-struct-tests-handlers-rust
// title:   CWS Struct — Rust Linter Tests
// type:    Code (Test)
// version: b-03.00
// created: 2026-02-17
// updated: 2026-02-20
// authors: Nova Dawn (CPI-SI)
// purpose: Tests for the Rust 4-block format handler. Tests go through the
//          public FormatHandler.lint() interface — same path the CLI uses.
//
//          Fixture-driven: each .rs file in tests/fixtures/rust/<category>/
//          targets a specific condition. Tests assert on result counts and
//          rule names. Category prefixes enable --filter targeting.
//
//          Categories:
//            structure/   — 4-block structure, parsing, block order, scaling
//            consistency/ — Cross-cutting validation (pragma vs metadata)
//            metadata/    — METADATA block (IC fields, presence, content)
//            doc/         — Doc comment quality (schema-driven)
//            setup/       — SETUP block (subsections, order, emphasis)
//            body/        — BODY block (subsections, order, content)
//            closing/     — CLOSING block (zones, content, depth, X6)
//            format/      — Format detection
//            unit/        — Exported function unit tests (no fixtures)
//            transform/   — Transformer tests
//            adapter/     — RustAdapter tests
//            form/        — Form-aware validation (layer chain)
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import { fixture, getFormat, errors, warnings, infos, byRule, hasRule, hasMessage } from "../helpers.ts";
import {
  parseStaticFields, validateICFields,
  validateICFieldContent,
  PRAGMA_FIELD_REQUIREMENTS, METADATA_FIELD_REQUIREMENTS,
  PRAGMA_CONTENT_RULES, METADATA_CONTENT_RULES,
  classifyLine, getSubsectionRanges, getTopLevelDeclarations,
} from "../../lib/handlers/rust.ts";
import type { RustContentKind } from "../../lib/handlers/rust.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Lint via registry — the integration surface
// ---------------------------------------------------------------------------

const rust = getFormat("rust");

if (!rust) {
  throw new Error("Rust format handler not registered — check imports in helpers.ts");
}

// ---------------------------------------------------------------------------
// structure/ — Overall block structure
// ---------------------------------------------------------------------------

Deno.test("structure/valid-library: zero errors", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("structure/valid-library: may have separator width warnings", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  // Warnings about separator widths are acceptable — not errors.
  for (const w of warnings(results)) {
    assert(
      w.rule.includes("separator") || w.rule.includes("width") || w.rule.includes("style"),
      `Unexpected warning rule: ${w.rule} — ${w.message}`,
    );
  }
});

Deno.test("structure/valid-module: zero errors", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-module.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("structure/valid-module: subtype detected as module", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-module.rs"));
  // Module subtype should be recognized — no "Unknown subtype" warnings
  const subtypeWarns = results.filter((r) => r.rule.includes("I2.subtype-value"));
  assertEquals(subtypeWarns.length, 0, `Module subtype should be recognized: ${JSON.stringify(subtypeWarns)}`);
});

Deno.test("structure/valid-executable: zero errors", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-executable.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("structure/missing-block: produces block error", async () => {
  const results = await rust.lint(fixture("rust/structure/missing-block.rs"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Expected at least one error for missing METADATA block");
  // Should mention the missing block
  assert(
    hasRule(results, "block") || hasMessage(results, "METADATA") || hasMessage(results, "block"),
    "Expected error to reference missing block or METADATA",
  );
});

Deno.test("structure/wrong-block-order: produces order error", async () => {
  const results = await rust.lint(fixture("rust/structure/wrong-block-order.rs"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Expected error for wrong block order (SETUP before METADATA)");
  assert(
    hasRule(results, "block/order"),
    `Expected block/order error, got rules: ${errs.map((e) => e.rule).join(", ")}`,
  );
});

Deno.test("structure/content-placement-correct: zero placement warnings", async () => {
  const results = await rust.lint(fixture("rust/structure/content-placement-correct.rs"));
  const errs = errors(results);
  const placementWarns = byRule(results, "content/");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(placementWarns.length, 0, `Expected 0 content placement warnings: ${JSON.stringify(placementWarns, null, 2)}`);
});

Deno.test("structure/content-placement-wrong: detects fn in SETUP and use/struct in BODY", async () => {
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
});

// ---------------------------------------------------------------------------
// metadata/ — METADATA block
// ---------------------------------------------------------------------------

Deno.test("metadata/missing-ic-fields: warns about missing required fields", async () => {
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
});

Deno.test("metadata/missing-ic-fields: zero errors (missing fields are warnings, not errors)", async () => {
  const results = await rust.lint(fixture("rust/metadata/missing-ic-crate/lib.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("metadata/placeholder-values: warns about template placeholders", async () => {
  const results = await rust.lint(fixture("rust/metadata/placeholder-values.rs"));
  // Should detect [bracketed] placeholder values
  assert(
    hasRule(results, "placeholder") || hasRule(results, "template") || hasMessage(results, "placeholder") || hasMessage(results, "["),
    "Should detect placeholder values in I/C fields",
  );
});

Deno.test("metadata/metadata-leak: detects code declarations in METADATA block", async () => {
  const results = await rust.lint(fixture("rust/metadata/metadata-leak.rs"));
  const leakWarns = byRule(results, "content/metadata-leak");
  assertGreater(leakWarns.length, 0, "Should detect code declarations in METADATA block");
  assert(
    hasMessage(results, "use_decl") || hasMessage(results, "struct_decl"),
    "Should identify the type of leaked construct",
  );
});

Deno.test("metadata/bad-content-values: detects invalid PRAGMA field values", async () => {
  const results = await rust.lint(fixture("rust/metadata/bad-content-values.rs"));
  const warns = warnings(results);
  const contentWarns = warns.filter((r) => r.rule.startsWith("value/PRAGMA/"));

  // 8 Pragma content checks: I1.key, I1.format, I1.at, I2.type, I2.structure, I3.file, I3.title
  // (I1.from is info, not warning)
  assert(hasRule(results, "value/PRAGMA/I1.key"), "Should catch bad key format");
  assert(hasRule(results, "value/PRAGMA/I1.format"), "Should catch unknown format");
  assert(hasRule(results, "value/PRAGMA/I1.at"), "Should catch bad version");
  assert(hasRule(results, "value/PRAGMA/I2.type"), "Should catch unknown type");
  assert(hasRule(results, "value/PRAGMA/I2.structure"), "Should catch unknown structure");
  assert(hasRule(results, "value/PRAGMA/I3.file"), "Should catch empty file");
  assert(hasRule(results, "value/PRAGMA/I3.title"), "Should catch empty title");
  assertGreater(contentWarns.length, 6, `Expected 7+ PRAGMA content warnings, got ${contentWarns.length}`);
});

Deno.test("metadata/bad-content-values: detects invalid METADATA field values", async () => {
  const results = await rust.lint(fixture("rust/metadata/bad-content-values.rs"));

  assert(hasRule(results, "value/METADATA/C1.version"), "Should catch bad version");
  assert(hasRule(results, "value/METADATA/C1.status"), "Should catch unknown status");
  assert(hasRule(results, "value/METADATA/C2.organization"), "Should catch empty organization");
});

Deno.test("metadata/bad-content-values: info-level checks for dates and paths", async () => {
  const results = await rust.lint(fixture("rust/metadata/bad-content-values.rs"));
  const infoResults = infos(results);

  // Info-level content checks: I1.from (path), C1.created, C1.updated (dates), C3.scripture (empty)
  const contentInfos = infoResults.filter((r) =>
    r.rule.startsWith("value/PRAGMA/") || r.rule.startsWith("value/METADATA/"));
  assertGreater(contentInfos.length, 3, `Expected 4+ content info results, got ${contentInfos.length}`);

  // Verify severity levels
  assert(hasRule(results, "value/PRAGMA/I1.from"), "Should flag path-like check on I1.from");
  assert(hasRule(results, "value/METADATA/C1.created"), "Should flag date format on C1.created");
  assert(hasRule(results, "value/METADATA/C1.updated"), "Should flag date format on C1.updated");
  assert(hasRule(results, "value/METADATA/C3.scripture"), "Should flag empty scripture");

  // Verify all content infos are actually info severity
  for (const r of contentInfos) {
    assertEquals(r.severity, "info", `${r.rule} should be info, got ${r.severity}`);
  }
});

Deno.test("metadata/valid-library: zero content value warnings", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const warns = warnings(results);
  const contentWarns = warns.filter((r) =>
    r.rule.startsWith("value/PRAGMA/") || r.rule.startsWith("value/METADATA/"));
  assertEquals(contentWarns.length, 0,
    `Expected 0 content warnings on valid-library, got: ${JSON.stringify(contentWarns.map(r => r.rule))}`);
});

Deno.test("metadata/placeholder-values: content checks skip placeholders", async () => {
  const results = await rust.lint(fixture("rust/metadata/placeholder-values.rs"));
  // Content checks should NOT fire for placeholder values like [YOUR-KEY-HERE]
  const contentWarns = warnings(results).filter((r) =>
    r.rule.startsWith("value/PRAGMA/") || r.rule.startsWith("value/METADATA/"));
  assertEquals(contentWarns.length, 0,
    `Content checks should skip placeholders: ${JSON.stringify(contentWarns.map(r => r.rule))}`);
});

Deno.test("metadata/bad-content-values: total of 14 content check results", async () => {
  const results = await rust.lint(fixture("rust/metadata/bad-content-values.rs"));
  const contentResults = results.filter((r) =>
    r.rule.startsWith("value/PRAGMA/") || r.rule.startsWith("value/METADATA/"));
  assertEquals(contentResults.length, 14,
    `Expected 14 total content results (8 PRAGMA + 6 METADATA), got ${contentResults.length}: ${JSON.stringify(contentResults.map(r => r.rule))}`);
});

Deno.test("structure/scaling: small fixture does not trigger scaling signals", async () => {
  const f = fixture("rust/setup/subsection-order-correct.rs");
  const results = await rust.lint(f);
  const scaling = byRule(results, "structure/scaling");
  assertEquals(scaling.length, 0, "Small fixture should not trigger scaling signals");
});

// ---------------------------------------------------------------------------
// consistency/ — Cross-cutting validation (pragma-driven checks)
// ---------------------------------------------------------------------------

Deno.test("consistency/template-with-derived-values: detects wrong I1.at and C1.status for template", async () => {
  const results = await rust.lint(fixture("rust/metadata/template-with-derived-values.rs"));
  const templateAt = byRule(results, "content/PRAGMA/template-at");
  const templateStatus = byRule(results, "content/METADATA/template-status");
  assertGreater(templateAt.length, 0,
    `Template file with I1.at="a-01.00" should trigger content/PRAGMA/template-at`);
  assertGreater(templateStatus.length, 0,
    `Template file with C1.status="Active" should trigger content/METADATA/template-status`);
});

Deno.test("consistency/derived-with-template-values: detects template values in derived file", async () => {
  const results = await rust.lint(fixture("rust/metadata/derived-with-template-values.rs"));
  const derivedAt = byRule(results, "content/PRAGMA/derived-at");
  const derivedStatus = byRule(results, "content/METADATA/derived-status");
  assertGreater(derivedAt.length, 0,
    `Derived file with I1.at="template" should trigger content/PRAGMA/derived-at`);
  assertGreater(derivedStatus.length, 0,
    `Derived file with C1.status="Template" should trigger content/METADATA/derived-status`);
});

Deno.test("consistency/wrong-subtype: detects I2.subtype mismatch with pragma", async () => {
  const results = await rust.lint(fixture("rust/metadata/wrong-subtype.rs"));
  const subtypeMismatch = byRule(results, "content/PRAGMA/subtype-mismatch");
  assertGreater(subtypeMismatch.length, 0,
    `Pragma says -library but I2.subtype="executable" should trigger content/PRAGMA/subtype-mismatch`);
});

Deno.test("consistency/valid-library: no content-aware false positives", async () => {
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

Deno.test("doc/valid-library: no doc comment warnings (has //! doc comments)", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const docRules = byRule(results, "doc/");
  assertEquals(docRules.length, 0,
    `Valid library should have 0 doc warnings: ${JSON.stringify(docRules.map(r => r.rule))}`);
});

Deno.test("doc/template-with-doc-comments: no doc/template info (template has //!)", async () => {
  const results = await rust.lint(fixture("rust/metadata/placeholder-values.rs"));
  const templateDoc = byRule(results, "doc/template");
  assertEquals(templateDoc.length, 0,
    "Template with //! doc comments should not trigger doc/template");
});

Deno.test("doc/schema-driven-severity: crate root severity from schema is warn", async () => {
  // Verify schema extraction produces the expected severity values.
  // The Rust schema defines crate_root_severity: "warn", module_severity: "info".
  // We can't easily test crate root (needs lib.rs filename) or missing //! module
  // from existing fixtures, but we CAN verify the expectations loaded correctly.
  const { loadCodeRules } = await import("../../lib/foundation/mod.ts");
  const rules = await loadCodeRules("rust");
  const crateExp = rules.docCommentExpectations["crate_root_docs"];
  const moduleExp = rules.docCommentExpectations["module_docs"];
  assertEquals(crateExp?.severity, "warn", "crate_root_docs severity from schema");
  assertEquals(moduleExp?.severity, "info", "module_docs severity from schema");
});

// ---------------------------------------------------------------------------
// setup/ — SETUP block
// ---------------------------------------------------------------------------

Deno.test("setup/subsection-order-correct: all 10 subsections in correct order — zero order warnings", async () => {
  const results = await rust.lint(fixture("rust/setup/subsection-order-correct.rs"));
  const errs = errors(results);
  const orderWarns = byRule(results, "setup/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(orderWarns.length, 0, `Expected 0 subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

Deno.test("setup/subsection-order-wrong: detects Core Types before Constants", async () => {
  const results = await rust.lint(fixture("rust/setup/subsection-order-wrong.rs"));
  assert(
    hasRule(results, "setup/subsection-order"),
    `Should detect wrong SETUP subsection order, got rules: ${warnings(results).map((w) => w.rule).join(", ")}`,
  );
});

Deno.test("setup/subsection-partial-correct: 3 of 10 in correct relative order — zero order warnings", async () => {
  const results = await rust.lint(fixture("rust/setup/subsection-partial-correct.rs"));
  const errs = errors(results);
  const orderWarns = byRule(results, "setup/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(orderWarns.length, 0, `Expected 0 subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

Deno.test("setup/alias-name: detects alias names and suggests canonical alternatives", async () => {
  const results = await rust.lint(fixture("rust/setup/aliased-subsection-names.rs"));
  const aliasInfos = byRule(results, "setup/alias-name");
  // Fixture uses: Consts (→Constants), Errors (→ErrorTypes), Types (→CoreTypes),
  // Traits (→TraitDefs) = 4 aliases
  assertGreater(aliasInfos.length, 0, `Expected alias info diagnostics, got: ${JSON.stringify(aliasInfos, null, 2)}`);
  // Should NOT have order warnings — aliases are in correct order
  const orderWarns = byRule(results, "setup/subsection-order");
  assertEquals(orderWarns.length, 0, `Aliased names in correct order should produce 0 order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

Deno.test("content/subsection-placement: detects misplaced declarations within SETUP subsections", async () => {
  const results = await rust.lint(fixture("rust/setup/misplaced-content.rs"));
  const placementInfos = byRule(results, "content/subsection-placement");
  // Fixture has: struct_decl in Constants (should be CoreTypes),
  // const_decl in CoreTypes (should be Constants) = 2 misplacements
  assertGreater(placementInfos.length, 0, `Expected subsection-placement infos, got: ${JSON.stringify(placementInfos, null, 2)}`);
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

Deno.test("setup/header-doc: no documentation before first subsection triggers info", async () => {
  const f = fixture("rust/setup/no-header-doc.rs");
  const results = await rust.lint(f);
  const headerDoc = byRule(results, "setup/header-doc");
  assertGreater(headerDoc.length, 0, "Should detect missing header documentation");
  assertEquals(headerDoc[0]!.severity, "info");
  assert(hasMessage(headerDoc, "header documentation"), "Should mention header documentation");
});

Deno.test("setup/header-doc: good documentation before first subsection is clean", async () => {
  const f = fixture("rust/setup/good-header-doc.rs");
  const results = await rust.lint(f);
  const headerDoc = byRule(results, "setup/header-doc");
  assertEquals(headerDoc.length, 0, "Good header doc should produce no findings");
});

Deno.test("setup/missing-required-subsections: library missing Modules, ErrorTypes, CoreTypes", async () => {
  const f = fixture("rust/setup/missing-required-subsections.rs");
  const results = await rust.lint(f);

  const required = byRule(results, "setup/required-subsection");
  assertGreater(required.length, 0, "Should detect missing required SETUP subsections");
  assertEquals(required[0]!.severity, "warn", "Required subsection check should be warn-level");
  assert(hasMessage(required, "Modules") || hasMessage(required, "ErrorTypes") || hasMessage(required, "CoreTypes"),
    "Should mention at least one missing required subsection");
});

Deno.test("setup/missing-required-subsections: reports count matches schema", async () => {
  const f = fixture("rust/setup/missing-required-subsections.rs");
  const results = await rust.lint(f);
  const required = byRule(results, "setup/required-subsection");
  // Rust schema: library requires S1 Imports (present), S2 Modules (missing), S6 ErrorTypes (missing), S7 CoreTypes (missing)
  assertEquals(required.length, 3, "Should report exactly 3 missing required subsections");
});

Deno.test("setup/emphasis-inverted: library with empty heavy subs and full light subs", async () => {
  const f = fixture("rust/setup/emphasis-inverted.rs");
  const results = await rust.lint(f);

  const emphasis = byRule(results, "emphasis/setup-inverted");
  assertGreater(emphasis.length, 0, "Should detect inverted emphasis");
  assertEquals(emphasis[0]!.severity, "info", "Emphasis check should be info-level");
  assert(hasMessage(emphasis, "library"), "Should mention the subtype");
});

// ---------------------------------------------------------------------------
// body/ — BODY block
// ---------------------------------------------------------------------------

Deno.test("body/wrong-body-order: detects BODY subsections out of order", async () => {
  const results = await rust.lint(fixture("rust/body/wrong-body-order.rs"));
  const errs = errors(results);
  const orderWarns = byRule(results, "body/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(orderWarns.length, 0, "Should detect wrong BODY subsection order");
  assert(
    hasMessage(results, "§1") && hasMessage(results, "§4"),
    "Should reference the misordered subsection numbers",
  );
});

Deno.test("body/valid-library: zero body subsection order warnings", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const orderWarns = byRule(results, "body/subsection-order");
  assertEquals(orderWarns.length, 0, `Expected 0 body/subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

Deno.test("body/subtype-subsections: wrong subsection names for library triggers info", async () => {
  const f = fixture("rust/body/wrong-subtype-subsections.rs");
  const results = await rust.lint(f);
  const subtypeSubs = byRule(results, "body/subtype-subsections");
  assertGreater(subtypeSubs.length, 0, "Should detect wrong subsection names");
  assertEquals(subtypeSubs[0]!.severity, "info");
  assert(hasMessage(subtypeSubs, "library"), "Should mention the subtype");
});

Deno.test("body/missing-required-subsections: library missing IdentityAccessors and Constructors", async () => {
  const f = fixture("rust/body/missing-required-subsections.rs");
  const results = await rust.lint(f);

  const required = byRule(results, "body/required-subsection");
  assertGreater(required.length, 0, "Should detect missing required BODY subsections");
  assertEquals(required[0]!.severity, "warn", "Required body subsection check should be warn-level");
});

Deno.test("body/missing-required-subsections: reports count matches schema", async () => {
  const f = fixture("rust/body/missing-required-subsections.rs");
  const results = await rust.lint(f);
  const required = byRule(results, "body/required-subsection");
  // Rust schema: library requires B1 IdentityAccessors (missing), B3 Constructors (missing)
  assertEquals(required.length, 2, "Should report exactly 2 missing required body subsections");
});

// ---------------------------------------------------------------------------
// closing/ — CLOSING block
// ---------------------------------------------------------------------------

Deno.test("closing/wrong-closing-order: detects code zone after documentation section", async () => {
  const results = await rust.lint(fixture("rust/closing/wrong-closing-order.rs"));
  const errs = errors(results);
  const zoneWarns = byRule(results, "closing/zone-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(zoneWarns.length, 0, "Should detect code zone after documentation section");
  assert(
    hasMessage(results, "X1") || hasMessage(results, "Cv") || hasMessage(results, "Cc"),
    "Should reference the misordered zones",
  );
});

Deno.test("closing/valid-library: zero closing zone order warnings", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const zoneWarns = byRule(results, "closing/");
  assertEquals(zoneWarns.length, 0, `Expected 0 closing zone warnings: ${JSON.stringify(zoneWarns, null, 2)}`);
});

Deno.test("closing/tests-in-body: detects #[cfg(test)] in BODY — should be CLOSING Cv", async () => {
  const results = await rust.lint(fixture("rust/closing/tests-in-body.rs"));
  const errs = errors(results);
  const testWarns = byRule(results, "closing/test-placement");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(testWarns.length, 0, "Should detect test module in BODY block");
  assert(
    hasMessage(results, "CLOSING Cv") || hasMessage(results, "Cv zone"),
    "Should guide toward CLOSING Cv zone",
  );
});

Deno.test("closing/main-in-body: detects fn main() in BODY — should be CLOSING Ce", async () => {
  const results = await rust.lint(fixture("rust/closing/main-in-body.rs"));
  const errs = errors(results);
  const mainWarns = byRule(results, "closing/main-placement");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(mainWarns.length, 0, "Should detect fn main() in BODY block");
  assert(
    hasMessage(results, "CLOSING Ce") || hasMessage(results, "Ce zone"),
    "Should guide toward CLOSING Ce zone",
  );
});

Deno.test("closing/required-zones: missing X1 and X5 documentation zones triggers info", async () => {
  const f = fixture("rust/closing/missing-required-zones.rs");
  const results = await rust.lint(f);
  const requiredX1 = byRule(results, "closing/required-X1");
  const requiredX5 = byRule(results, "closing/required-X5");
  assertGreater(requiredX1.length, 0, "Should detect missing X1 zone");
  assertGreater(requiredX5.length, 0, "Should detect missing X5 zone");
  assertEquals(requiredX1[0]!.severity, "info");
  assertEquals(requiredX5[0]!.severity, "info");
});

Deno.test("closing/X1-content: X1 zone missing required fields triggers info", async () => {
  const f = fixture("rust/closing/x1-missing-fields.rs");
  const results = await rust.lint(f);
  const x1Content = byRule(results, "closing/X1-content");
  // X1 zone present but missing Never, Careful, Safe fields
  assertGreater(x1Content.length, 0, "Should detect missing X1 fields");
  assertEquals(x1Content[0]!.severity, "info");
  assert(hasMessage(x1Content, "never"), "Should mention missing 'never' field");
});

Deno.test("closing/X5-content: X5 zone missing scripture field triggers info", async () => {
  const f = fixture("rust/closing/x5-missing-scripture.rs");
  const results = await rust.lint(f);
  const x5Content = byRule(results, "closing/X5-content");
  // X5 zone present but missing Scripture field
  assertGreater(x5Content.length, 0, "Should detect missing scripture in X5 zone");
  assertEquals(x5Content[0]!.severity, "info");
  assert(hasMessage(x5Content, "scripture"), "Should mention missing 'scripture' field");
});

Deno.test("closing/X6-template-only: detects X6 section in non-template file", async () => {
  const results = await rust.lint(fixture("rust/closing/x6-in-derived.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  assert(hasRule(results, "closing/X6-template-only"), "Should detect X6 in non-template file");

  const x6 = byRule(results, "closing/X6-template-only");
  assertEquals(x6[0]!.severity, "warn", "X6-template-only should be warn-level");
  assert(hasMessage(x6, "Template Guide"), "Should mention Template Guide");
});

Deno.test("closing/X6-template-only: valid-library has no X6 warning", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const x6 = byRule(results, "closing/X6-template-only");
  assertEquals(x6.length, 0, "Valid library without X6 should produce no X6-template-only warning");
});

Deno.test("closing/X1-depth: detects empty and placeholder field values in X1", async () => {
  const results = await rust.lint(fixture("rust/closing/x1-empty-fields.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  const depth = byRule(results, "closing/X1-depth");
  assertGreater(depth.length, 0, "Should detect empty/placeholder X1 field values");
  assertEquals(depth[0]!.severity, "info", "X1-depth should be info-level");
  assert(hasMessage(depth, "never") || hasMessage(depth, "careful"),
    "Should mention the field with empty/placeholder value");
});

Deno.test("closing/depth: valid-library has no depth warnings", async () => {
  const results = await rust.lint(fixture("rust/structure/valid-library.rs"));
  const depth = results.filter((r) => r.rule.includes("-depth"));
  assertEquals(depth.length, 0, "Valid library should produce no depth warnings");
});

// ---------------------------------------------------------------------------
// format/ — Format detection
// ---------------------------------------------------------------------------

Deno.test("format/no-omni: info only, no errors", async () => {
  const results = await rust.lint(fixture("rust/format/no-omni.rs"));
  const errs = errors(results);
  const warns = warnings(results);

  assertEquals(errs.length, 0, "Plain .rs file should produce no errors");
  assertEquals(warns.length, 0, "Plain .rs file should produce no warnings");

  // Should have at least one info saying it's not a 4-block file
  const infoResults = infos(results);
  assertGreater(infoResults.length, 0, "Should produce info about not being a 4-block file");
});

// ---------------------------------------------------------------------------
// unit/ — Unit tests for exported functions
// ---------------------------------------------------------------------------

Deno.test("unit/classifyLine: identifies all major Rust constructs", () => {
  const cases: Array<[string, RustContentKind]> = [
    ["", "blank"],
    ["// comment", "comment"],
    ["//! doc comment", "comment"],
    ["/// item doc", "comment"],
    ["// ============================================================================", "comment"],
    ["use std::io;", "use_decl"],
    ["pub use crate::types::Config;", "reexport_decl"],
    ["use{std::io, std::fmt};", "use_decl"],
    ["mod types;", "mod_decl"],
    ["pub mod api;", "mod_decl"],
    ["const MAX: u32 = 5;", "const_decl"],
    ["pub const VERSION: &str = \"1.0\";", "const_decl"],
    ["static INSTANCE: LazyLock<Config> = LazyLock::new(|| Config::new());", "static_decl"],
    ["pub static PRAGMA: &[(&str, &str)] = &[];", "static_decl"],
    ["type Result<T> = std::result::Result<T, Error>;", "type_alias"],
    ["pub type Map = HashMap<String, String>;", "type_alias"],
    ["struct Config { field: u32 }", "struct_decl"],
    ["pub struct Server {", "struct_decl"],
    ["enum Status { Active, Inactive }", "enum_decl"],
    ["pub enum Error {", "enum_decl"],
    ["trait Validate {", "trait_decl"],
    ["pub trait Service {", "trait_decl"],
    ["unsafe trait Send {}", "trait_decl"],
    ["macro_rules! assert_ok {", "macro_decl"],
    ["fn main() {", "fn_decl"],
    ["pub fn new() -> Self {", "fn_decl"],
    ["async fn run() -> Result<()> {", "fn_decl"],
    ["pub async fn serve() {", "fn_decl"],
    ["unsafe fn raw_op() {", "fn_decl"],
    ["impl Config {", "impl_block"],
    ["impl Display for Error {", "impl_block"],
    ["unsafe impl Send for Config {}", "impl_block"],
    ["#[test]", "test_attr"],
    ["#[cfg(test)]", "test_attr"],
    ["#[cfg(feature = \"serde\")]", "cfg_attr"],
    ["#[derive(Debug, Clone)]", "attr"],
    ["pub(crate) fn internal() {", "fn_decl"],
    ["pub(crate) struct Inner {", "struct_decl"],
  ];

  for (const [input, expected] of cases) {
    const actual = classifyLine(input);
    assertEquals(actual, expected, `classifyLine("${input}") → "${actual}", expected "${expected}"`);
  }
});

Deno.test("unit/getTopLevelDeclarations: skips nested content", () => {
  const lines = [
    "use std::io;",
    "pub struct Config {",
    "    field: u32,",
    "}",
    "impl Config {",
    "    const INNER: u32 = 5;",   // nested — should NOT be classified
    "    fn new() -> Self {",       // nested — should NOT be classified
    "        Config { field: 0 }",
    "    }",
    "}",
    "pub fn top_level() {",         // top-level — should be classified
    "    let x = 5;",
    "}",
  ];
  const decls = getTopLevelDeclarations(lines);
  const kinds = decls.map((d) => d.kind);

  assertEquals(kinds, ["use_decl", "struct_decl", "impl_block", "fn_decl"],
    `Expected top-level only, got: ${JSON.stringify(decls, null, 2)}`);
});

Deno.test("unit/getTopLevelDeclarations: handles multi-level nesting", () => {
  const lines = [
    "impl Foo {",
    "    fn bar() {",
    "        if true {",
    "            const NESTED: u32 = 1;",  // depth 3 — skip
    "        }",
    "    }",
    "}",
    "const TOP: u32 = 2;",               // depth 0 — classify
  ];
  const decls = getTopLevelDeclarations(lines);
  assertEquals(decls.length, 2);
  assertEquals(decls[0]!.kind, "impl_block");
  assertEquals(decls[1]!.kind, "const_decl");
});

Deno.test("unit/getSubsectionRanges: finds subsection boundaries", () => {
  const lines = [
    "// ──────────────────────────────────────────────────────────────────────────",
    "// Imports",
    "// ──────────────────────────────────────────────────────────────────────────",
    "use std::io;",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// Constants",
    "// ──────────────────────────────────────────────────────────────────────────",
    "const MAX: u32 = 5;",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// Core Types",
    "// ──────────────────────────────────────────────────────────────────────────",
    "struct Foo {}",
  ];
  const ranges = getSubsectionRanges(lines);
  assertEquals(ranges.length, 3);
  assertEquals(ranges[0]!.tag, "Imports");
  assertEquals(ranges[1]!.tag, "Constants");
  assertEquals(ranges[2]!.tag, "CoreTypes");
  // Imports ends where Constants starts
  assertEquals(ranges[0]!.endIdx, ranges[1]!.startIdx);
});

Deno.test("unit/parseStaticFields: extracts PRAGMA fields", () => {
  const lines = [
    `pub static PRAGMA: &[(&str, &str)] = &[`,
    `    ("I1.key", "test-value"),`,
    `    ("I1.format", "rust"),`,
    `    // comment line`,
    `    ("I2.type", "code"),`,
    `];`,
  ];
  const fields = parseStaticFields(lines, "PRAGMA");
  assertEquals(fields.length, 3);
  assertEquals(fields[0]!.section, "I1");
  assertEquals(fields[0]!.field, "key");
  assertEquals(fields[0]!.value, "test-value");
  assertEquals(fields[2]!.section, "I2");
  assertEquals(fields[2]!.field, "type");
});

Deno.test("unit/parseStaticFields: handles nested keys (C4.requires.stdlib)", () => {
  const lines = [
    `pub static METADATA: &[(&str, &str)] = &[`,
    `    ("C4.requires.stdlib", "none"),`,
    `    ("C4.requires.external", "tokio"),`,
    `];`,
  ];
  const fields = parseStaticFields(lines, "METADATA");
  assertEquals(fields.length, 2);
  assertEquals(fields[0]!.section, "C4");
  assertEquals(fields[0]!.field, "requires.stdlib");
});

Deno.test("unit/parseStaticFields: skips comment-only lines", () => {
  const lines = [
    `pub static PRAGMA: &[(&str, &str)] = &[`,
    `    // This is a comment`,
    `    ("I1.key", "value"),`,
    `    // Another comment`,
    `];`,
  ];
  const fields = parseStaticFields(lines, "PRAGMA");
  assertEquals(fields.length, 1);
});

Deno.test("unit/parseStaticFields: handles single-line static", () => {
  const lines = [
    `pub static PRAGMA: &[(&str, &str)] = &[("I1.key", "value"), ("I1.format", "rust")];`,
  ];
  const fields = parseStaticFields(lines, "PRAGMA");
  assertEquals(fields.length, 2);
});

Deno.test("unit/parseStaticFields: returns empty for missing static", () => {
  const lines = [
    `fn main() {`,
    `    println!("no statics");`,
    `}`,
  ];
  const fields = parseStaticFields(lines, "PRAGMA");
  assertEquals(fields.length, 0);
});

Deno.test("unit/parseStaticFields: handles OWN_ prefix", () => {
  const lines = [
    `pub static OWN_PRAGMA: &[(&str, &str)] = &[`,
    `    ("I1.key", "value"),`,
    `];`,
  ];
  const fields = parseStaticFields(lines, "PRAGMA");
  assertEquals(fields.length, 1);
});

Deno.test("unit/parseStaticFields: handles r# raw strings", () => {
  const lines = [
    `pub static PRAGMA: &[(&str, &str)] = &[`,
    `    ("I1.key", r#"value-with-"quotes""#),`,
    `    ("I1.format", "rust"),`,
    `];`,
  ];
  const fields = parseStaticFields(lines, "PRAGMA");
  assertEquals(fields.length, 2);
  assertEquals(fields[0]!.value, `value-with-"quotes"`);
  assertEquals(fields[1]!.value, "rust");
});

Deno.test("unit/validateICFields: all required present produces no warnings", () => {
  const fields = [
    { section: "I1", field: "key", value: "test", line: 1 },
    { section: "I1", field: "format", value: "rust", line: 2 },
    { section: "I1", field: "from", value: "template", line: 3 },
    { section: "I1", field: "at", value: "a-01.00", line: 4 },
    { section: "I2", field: "type", value: "code", line: 5 },
    { section: "I2", field: "structure", value: "4-block", line: 6 },
    { section: "I3", field: "file", value: "test.rs", line: 7 },
    { section: "I3", field: "title", value: "Test", line: 8 },
  ];
  const results = validateICFields("test.rs", fields, PRAGMA_FIELD_REQUIREMENTS, "PRAGMA");
  const warns = results.filter((r) => r.severity === "warn");
  assertEquals(warns.length, 0, `Unexpected warnings: ${JSON.stringify(warns, null, 2)}`);
});

Deno.test("unit/validateICFields: missing required produces warnings", () => {
  const fields = [
    // I1: missing key, from
    { section: "I1", field: "format", value: "rust", line: 1 },
    { section: "I1", field: "at", value: "a-01.00", line: 2 },
    // I2: missing type
    { section: "I2", field: "structure", value: "4-block", line: 3 },
    // I3: has both
    { section: "I3", field: "file", value: "test.rs", line: 4 },
    { section: "I3", field: "title", value: "Test", line: 5 },
  ];
  const results = validateICFields("test.rs", fields, PRAGMA_FIELD_REQUIREMENTS, "PRAGMA");
  const warns = results.filter((r) => r.severity === "warn");
  assertEquals(warns.length, 3, "Should warn about I1.key, I1.from, I2.type");
});

Deno.test("unit/validateICFields: nested keys count as base field present", () => {
  const fields = [
    { section: "C4", field: "requires.stdlib", value: "none", line: 1 },
    { section: "C4", field: "consumers", value: "test", line: 2 },
  ];
  const requirements = { C4: { required: ["requires", "consumers"], defined: [] } };
  const results = validateICFields("test.rs", fields, requirements, "METADATA");
  const warns = results.filter((r) => r.severity === "warn");
  assertEquals(warns.length, 0, "C4.requires.stdlib should satisfy 'requires' requirement");
});

Deno.test("unit/validateICFields: missing defined fields produce info, not warn", () => {
  const fields = [
    { section: "I2", field: "type", value: "code", line: 1 },
    { section: "I2", field: "structure", value: "4-block", line: 2 },
    // Missing I2.subtype and I2.role (defined, not required)
  ];
  const requirements = { I2: { required: ["type", "structure"], defined: ["subtype", "role"] } };
  const results = validateICFields("test.rs", fields, requirements, "PRAGMA");
  const warns = results.filter((r) => r.severity === "warn");
  const infoResults = results.filter((r) => r.severity === "info");
  assertEquals(warns.length, 0, "Defined fields should not produce warnings");
  assertEquals(infoResults.length, 2, "Should produce info for each missing defined field");
});

// ---------------------------------------------------------------------------
// transform/ — Transformer tests
// ---------------------------------------------------------------------------

Deno.test("transform/separators: dry-run detects ASCII dashes and prefix issues", async () => {
  const f = fixture("rust/format/transform-separators.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false });
  const dashRules = byRule(results, "dash-to-unicode");
  assert(dashRules.length >= 2, `Expected ≥2 dash-to-unicode, got ${dashRules.length}`);
  const prefixRules = byRule(results, "prefix-normalize");
  assert(prefixRules.length >= 1, `Expected ≥1 prefix-normalize, got ${prefixRules.length}`);
});

Deno.test("transform/separators: dry-run does not modify file", async () => {
  const f = fixture("rust/format/transform-separators.rs");
  const before = await Deno.readTextFile(f);
  await rust.transform!(f, { dryRun: true, extensions: false });
  const after = await Deno.readTextFile(f);
  assertEquals(before, after, "File should be unchanged after dry-run");
});

Deno.test("transform/move-tests: dry-run detects #[cfg(test)] in BODY", async () => {
  const f = fixture("rust/closing/tests-in-body.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false });
  assert(hasRule(results, "move-tests"), "Should detect tests to move");
  assert(hasMessage(results, "#[cfg(test)]"), "Message should mention #[cfg(test)]");
});

Deno.test("transform/move-tests: actual transform moves tests to CLOSING Cv", async () => {
  // Work on a temp copy
  const src = fixture("rust/closing/tests-in-body.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    const results = await rust.transform!(tmp, { dryRun: false, extensions: false });
    assert(hasRule(results, "move-tests"), "Should report move-tests");
    assert(hasRule(results, "written"), "Should write file");

    // Lint the transformed file — should have NO test-placement warning
    const lintResults = await rust.lint(tmp);
    const testPlacement = byRule(lintResults, "test-placement");
    assertEquals(testPlacement.length, 0, "Transformed file should have no test-placement warnings");

    // Verify the content moved: CLOSING should contain cfg(test), BODY should not
    const content = await Deno.readTextFile(tmp);
    const closingStart = content.indexOf("// CLOSING\n");
    const bodyStart = content.indexOf("// BODY\n");
    const bodyEnd = content.indexOf("// END BODY");
    assert(closingStart > 0, "Should have CLOSING block");
    assert(content.indexOf("#[cfg(test)]", closingStart) > 0, "cfg(test) should be in CLOSING");
    // Check BODY section specifically — extract between BODY and END BODY
    const bodySection = content.slice(bodyStart, bodyEnd);
    assertEquals(bodySection.includes("\n#[cfg(test)]"), false,
      "BODY section should not contain #[cfg(test)] code");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/move-main: dry-run detects fn main() in BODY", async () => {
  const f = fixture("rust/closing/main-in-body.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false });
  assert(hasRule(results, "move-main"), "Should detect main to move");
  assert(hasMessage(results, "fn main()"), "Message should mention fn main()");
});

Deno.test("transform/move-main: actual transform moves main to CLOSING Ce", async () => {
  const src = fixture("rust/closing/main-in-body.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    const results = await rust.transform!(tmp, { dryRun: false, extensions: false });
    assert(hasRule(results, "move-main"), "Should report move-main");

    // Lint the transformed file
    const lintResults = await rust.lint(tmp);
    const mainPlacement = byRule(lintResults, "main-placement");
    assertEquals(mainPlacement.length, 0, "Transformed file should have no main-placement warnings");

    // Verify main is in CLOSING
    const content = await Deno.readTextFile(tmp);
    const closingStart = content.indexOf("// CLOSING");
    assert(content.indexOf("fn main()", closingStart) > 0, "fn main() should be in CLOSING");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/reorder-closing: dry-run detects out-of-order zones", async () => {
  const f = fixture("rust/closing/wrong-closing-order.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false });
  assert(hasRule(results, "reorder-closing"), "Should detect zones needing reorder");
});

Deno.test("transform/reorder-closing: actual transform fixes zone order", async () => {
  const src = fixture("rust/closing/wrong-closing-order.rs");
  const tmp = await Deno.makeTempFile({ suffix: ".rs" });
  await Deno.copyFile(src, tmp);

  try {
    const results = await rust.transform!(tmp, { dryRun: false, extensions: false });
    assert(hasRule(results, "reorder-closing"), "Should report reorder");

    // Lint the transformed file
    const lintResults = await rust.lint(tmp);
    const zoneOrder = byRule(lintResults, "zone-order");
    assertEquals(zoneOrder.length, 0, "Transformed file should have no zone-order warnings");

    // Verify code zones come before doc zones in the output
    const content = await Deno.readTextFile(tmp);
    const cvPos = content.indexOf("// Cv");
    const x1Pos = content.indexOf("// X1");
    assert(cvPos > 0 && x1Pos > 0, "Should have both Cv and X1 zones");
    assert(cvPos < x1Pos, "Cv should come before X1 after reorder");
  } finally {
    await Deno.remove(tmp);
  }
});

Deno.test("transform/valid-library: no changes needed", async () => {
  const f = fixture("rust/structure/valid-library.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false });
  // Valid library may have some separator width fixes but no structural moves
  const moves = results.filter((r) =>
    r.rule.includes("move-tests") ||
    r.rule.includes("move-main") ||
    r.rule.includes("reorder-closing"));
  assertEquals(moves.length, 0, "Valid library should need no structural transforms");
});

// ---------------------------------------------------------------------------
// adapter/ — RustAdapter implements LanguageAdapter
// ---------------------------------------------------------------------------

import { rustAdapter } from "../../lib/handlers/rust.ts";

Deno.test("adapter/rust: format is 'rust'", () => {
  assertEquals(rustAdapter.format, "rust");
});

Deno.test("adapter/rust: extensions include .rs", () => {
  assert(rustAdapter.extensions.includes(".rs"));
});

Deno.test("adapter/rust: classifyLine delegates to classifyLine", () => {
  assertEquals(rustAdapter.classifyLine("use std::io;"), "use_decl");
  assertEquals(rustAdapter.classifyLine("pub use crate::Foo;"), "reexport_decl");
  assertEquals(rustAdapter.classifyLine("fn main() {"), "fn_decl");
  assertEquals(rustAdapter.classifyLine("struct Foo {"), "struct_decl");
  assertEquals(rustAdapter.classifyLine("impl Foo {"), "impl_block");
  assertEquals(rustAdapter.classifyLine("// comment"), "comment");
  assertEquals(rustAdapter.classifyLine(""), "blank");
});

Deno.test("adapter/rust: parseIdentityFields delegates to parseStaticFields", () => {
  const lines = [
    'pub static PRAGMA: &[(&str, &str)] = &[',
    '    ("I1.key", "B-test"),',
    '    ("I1.format", "rust"),',
    '];',
  ];
  const fields = rustAdapter.parseIdentityFields(lines, "PRAGMA");
  assertEquals(fields.length, 2);
  assertEquals(fields[0]!.section, "I1");
  assertEquals(fields[0]!.field, "key");
  assertEquals(fields[0]!.value, "B-test");
});

Deno.test("adapter/rust: findOmniDirectives finds directives", () => {
  const lines = [
    "//omni:key B-test",
    "//omni:code --rust -library",
    "use std::io;",
  ];
  const directives = rustAdapter.findOmniDirectives(lines);
  assert(directives.has("//omni:key"));
  assert(directives.has("//omni:code"));
  assertEquals(directives.get("//omni:key")!.value, "B-test");
});

Deno.test("adapter/rust: findTestZone finds #[cfg(test)]", () => {
  const lines = [
    "// code above",
    "#[cfg(test)]",
    "mod tests {",
    "  #[test]",
    "  fn it_works() {}",
    "}",
    "// code below",
  ];
  const zone = rustAdapter.findTestZone(lines, 0, lines.length);
  assert(zone !== null);
  assertEquals(zone!.start, 1);
  assertEquals(zone!.end, 5);
});

Deno.test("adapter/rust: findMainZone finds fn main", () => {
  const lines = [
    "// header",
    "fn main() {",
    "    println!(\"hello\");",
    "}",
  ];
  const zone = rustAdapter.findMainZone(lines, 0, lines.length);
  assert(zone !== null);
  assertEquals(zone!.start, 1);
  assertEquals(zone!.end, 3);
});

Deno.test("adapter/rust: no enrichSubsectionPatterns (Rust has no legacy patterns)", () => {
  assertEquals(rustAdapter.enrichSubsectionPatterns, undefined);
});

Deno.test("adapter/rust: buildContextExtras identifies crate root (lib.rs)", () => {
  const extras = rustAdapter.buildContextExtras("/path/to/lib.rs", []);
  assertEquals(extras.isCrateRoot, true);
  assertEquals(extras.isModuleFile, false);
});

Deno.test("adapter/rust: buildContextExtras identifies crate root (main.rs)", () => {
  const extras = rustAdapter.buildContextExtras("/path/to/main.rs", []);
  assertEquals(extras.isCrateRoot, true);
  assertEquals(extras.isModuleFile, false);
});

Deno.test("adapter/rust: buildContextExtras identifies module file", () => {
  const extras = rustAdapter.buildContextExtras("/path/to/utils.rs", []);
  assertEquals(extras.isCrateRoot, false);
  assertEquals(extras.isModuleFile, true);
});

Deno.test("adapter/rust: buildContextExtras identifies template (not module)", () => {
  const lines = ["// #!omni template --rust -library"];
  const extras = rustAdapter.buildContextExtras("/path/to/template.rs", lines);
  assertEquals(extras.isCrateRoot, false);
  assertEquals(extras.isModuleFile, false);
});

// ---------------------------------------------------------------------------
// form/ — Form-aware validation (layer chain: format → variant)
// ---------------------------------------------------------------------------

Deno.test("form/module-reserved: detects S2 Modules in non-template module", async () => {
  const f = fixture("rust/form/module-has-reserved-section.rs");
  const results = await rust.lint(f);
  const reserved = byRule(results, "form/reserved-section-present");
  assertGreater(reserved.length, 0, "Should detect reserved S2 Modules in module file");
  assertEquals(reserved[0]!.severity, "warn");
  assert(hasMessage(reserved, "Modules"), "Should mention the reserved section tag");
  assert(hasMessage(reserved, "RESERVED"), "Should indicate the section is reserved");
});

Deno.test("form/module-missing-required: detects absent S1 Imports", async () => {
  const f = fixture("rust/form/module-missing-required.rs");
  const results = await rust.lint(f);
  const missing = byRule(results, "form/required-section-missing");
  assertGreater(missing.length, 0, "Should detect missing required section");
  assertEquals(missing[0]!.severity, "warn");
  assert(hasMessage(missing, "Imports"), "Should mention the missing Imports section");
});

Deno.test("form/template-reserved: reserved checks fire on templates (templates are live)", async () => {
  const f = fixture("rust/form/template-with-reserved.rs");
  const results = await rust.lint(f);
  const reserved = byRule(results, "form/reserved-section-present");
  assertGreater(reserved.length, 0, "Templates should be checked for reserved sections");
  assert(hasMessage(reserved, "Modules"), "Should detect S2 Modules in module template");
});

Deno.test("form/template-required: required checks SKIP templates", async () => {
  const f = fixture("rust/form/template-with-reserved.rs");
  const results = await rust.lint(f);
  const missing = byRule(results, "form/required-section-missing");
  assertEquals(missing.length, 0, "Templates should not trigger required-section-missing");
});

Deno.test("form/valid-module: template has no reserved violations (existing valid-module)", async () => {
  const f = fixture("rust/structure/valid-module.rs");
  const results = await rust.lint(f);
  const reserved = byRule(results, "form/reserved-section-present");
  assertEquals(reserved.length, 0, "Valid module template should have no reserved section violations");
});

Deno.test("form/bare-bone-default: files without subtype get bare-bone form constraints", async () => {
  // format/no-omni.rs has no subtype — should still get bare-bone form
  const f = fixture("rust/format/no-omni.rs");
  const results = await rust.lint(f);
  // Bare-bone form loads but with no reserved sections to violate in a minimal file,
  // we just verify no crash and no reserved-section-present results
  const reserved = byRule(results, "form/reserved-section-present");
  // A minimal file with no subsection headers won't trigger reserved checks
  assertEquals(reserved.length, 0, "Minimal file without subsection headers should have no reserved violations");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Rust 4-block linter + transformer tests — pragma-container organized.
// Fixture-driven through public lint() and transform() interfaces.
// Unit tests for parseStaticFields and validateICFields target parser directly.
//
// Spine: structure/ → consistency/ → setup/ → body/ → closing/ → format/ → form/ → unit/ → transform/
// Per-block: setup/ body/ closing/ (4-block specific)
// Cross-cutting: consistency/ (pragma vs metadata agreement)
// Filter: deno test --filter "category/" tests/handlers/rust_test.ts
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
