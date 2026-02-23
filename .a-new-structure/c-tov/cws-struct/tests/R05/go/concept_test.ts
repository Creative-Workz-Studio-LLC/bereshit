// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/R05/go/concept_test.ts
// purpose: R05 (concept/detail) detection tests for the Go format handler.
//          Tests field content validation, content-aware checks (pragma-driven),
//          closing zone content/depth, setup header docs, and body subtype
//          subsection names — the finest grain in the hierarchy.
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
// metadata/ — Content validation (field VALUES, not just existence)
// ---------------------------------------------------------------------------

Deno.test("[CWS-R05-080] R05/metadata/bad-content-values: detects invalid Pragma field values", async () => {
  const results = await go.lint(fixture("go/metadata/bad-content-values.go"));
  const valueWarns = byRule(results, "value/Pragma/");
  assertGreater(valueWarns.length, 0, "Should detect bad Pragma content values");

  // I1.key — bad pattern
  assert(hasRule(results, "value/Pragma/I1.key"), "Should flag bad I1.key pattern");
  // I1.format — unknown format
  assert(hasRule(results, "value/Pragma/I1.format"), "Should flag unknown I1.format");
  // I1.at — bad version
  assert(hasRule(results, "value/Pragma/I1.at"), "Should flag bad I1.at version");
  // I2.type — unknown type
  assert(hasRule(results, "value/Pragma/I2.type"), "Should flag unknown I2.type");
  // I2.structure — unknown structure
  assert(hasRule(results, "value/Pragma/I2.structure"), "Should flag unknown I2.structure");
  // I3.file — empty
  assert(hasRule(results, "value/Pragma/I3.file"), "Should flag empty I3.file");
  // I3.title — empty
  assert(hasRule(results, "value/Pragma/I3.title"), "Should flag empty I3.title");
  // Error code: CWS-R05-080 (value/{{varName}}/{{group}}.{{field}} — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-080"),
    `Expected CWS-R05-080 (value validation), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R05-080] R05/metadata/bad-content-values: detects invalid Metadata field values", async () => {
  const results = await go.lint(fixture("go/metadata/bad-content-values.go"));
  const valueWarns = byRule(results, "value/Metadata/");
  assertGreater(valueWarns.length, 0, "Should detect bad Metadata content values");

  // C1.version — bad version format
  assert(hasRule(results, "value/Metadata/C1.version"), "Should flag bad C1.version");
  // C1.status — unknown status
  assert(hasRule(results, "value/Metadata/C1.status"), "Should flag unknown C1.status");
  // C2.organization — empty
  assert(hasRule(results, "value/Metadata/C2.organization"), "Should flag empty C2.organization");
  // Error code: CWS-R05-080 (value/{{varName}}/{{group}}.{{field}} — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-080"),
    `Expected CWS-R05-080 (value validation), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R05-080] R05/metadata/bad-content-values: info-level checks for dates and paths", async () => {
  const results = await go.lint(fixture("go/metadata/bad-content-values.go"));

  // I1.from — not a path (info-level)
  assert(hasRule(results, "value/Pragma/I1.from"), "Should flag non-path I1.from");
  const fromResult = byRule(results, "value/Pragma/I1.from");
  assertEquals(fromResult[0]!.severity, "info", "I1.from check should be info-level");

  // C1.created — bad date format (info-level)
  assert(hasRule(results, "value/Metadata/C1.created"), "Should flag bad C1.created date");
  const createdResult = byRule(results, "value/Metadata/C1.created");
  assertEquals(createdResult[0]!.severity, "info", "C1.created check should be info-level");

  // C1.updated — bad date format (info-level)
  assert(hasRule(results, "value/Metadata/C1.updated"), "Should flag bad C1.updated date");

  // C3.scripture — empty (info-level)
  assert(hasRule(results, "value/Metadata/C3.scripture"), "Should flag empty C3.scripture");
  const scriptureResult = byRule(results, "value/Metadata/C3.scripture");
  assertEquals(scriptureResult[0]!.severity, "info", "C3.scripture check should be info-level");
  // Error code: CWS-R05-080 (value/{{varName}}/{{group}}.{{field}} — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-080"),
    `Expected CWS-R05-080 (value validation), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("R05/metadata/valid-library: zero content value warnings", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const valueWarns = warnings(results).filter((r) => r.rule.startsWith("value/"));
  assertEquals(valueWarns.length, 0,
    `Valid library should have zero value warnings: ${valueWarns.map((w) => w.rule).join(", ")}`);
});

Deno.test("R05/metadata/placeholder-values: content checks skip placeholders", async () => {
  const results = await go.lint(fixture("go/metadata/placeholder-values.go"));
  // Placeholders are handled by detectPlaceholders, not validateICFieldContent
  // Content checks should NOT fire for placeholder values like [YOUR-KEY-HERE]
  const valueResults = byRule(results, "value/Pragma/I1.key");
  assertEquals(valueResults.length, 0,
    "Content check should skip placeholder values (detectPlaceholders handles those)");
});

Deno.test("[CWS-R05-080] R05/metadata/bad-content-values: total of 14 content check results", async () => {
  const results = await go.lint(fixture("go/metadata/bad-content-values.go"));
  const valueResults = byRule(results, "value/");
  assertEquals(valueResults.length, 14,
    `Should produce exactly 14 content value results (8 Pragma + 6 Metadata), got: ${valueResults.map((r) => r.rule).join(", ")}`);
  // Error code: CWS-R05-080 (value/{{varName}}/{{group}}.{{field}} — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-080"),
    `Expected CWS-R05-080 (value validation), got: ${errorCodeReport(results)}`,
  );
});

// ---------------------------------------------------------------------------
// metadata/ — Content-aware validation (pragma-driven checks)
// ---------------------------------------------------------------------------

Deno.test("[CWS-R10-030] R05/metadata/template-with-derived-values: detects wrong I1.at for template", async () => {
  const results = await go.lint(fixture("go/metadata/template-with-derived-values.go"));
  const templateAt = byRule(results, "content/Pragma/template-at");
  const templateStatus = byRule(results, "content/Metadata/template-status");
  assertGreater(templateAt.length, 0,
    `Template file with I1.at="a-01.00" should trigger content/Pragma/template-at`);
  assertGreater(templateStatus.length, 0,
    `Template file with C1.status="Active" should trigger content/Metadata/template-status`);
  // Error code: CWS-R10-030 (content/Pragma/template-at — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-030"),
    `Expected CWS-R10-030 (template-at), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R10-032] R05/metadata/derived-with-template-values: detects template values in derived file", async () => {
  const results = await go.lint(fixture("go/metadata/derived-with-template-values.go"));
  const derivedAt = byRule(results, "content/Pragma/derived-at");
  const derivedStatus = byRule(results, "content/Metadata/derived-status");
  assertGreater(derivedAt.length, 0,
    `Derived file with I1.at="template" should trigger content/Pragma/derived-at`);
  assertGreater(derivedStatus.length, 0,
    `Derived file with C1.status="Template" should trigger content/Metadata/derived-status`);
  // Error code: CWS-R10-032 (content/Pragma/derived-at — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-032"),
    `Expected CWS-R10-032 (derived-at), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("[CWS-R10-034] R05/metadata/wrong-subtype: detects I2.subtype mismatch with pragma", async () => {
  const results = await go.lint(fixture("go/metadata/wrong-subtype.go"));
  const subtypeMismatch = byRule(results, "content/Pragma/subtype-mismatch");
  assertGreater(subtypeMismatch.length, 0,
    `Pragma says -library but I2.subtype="executable" should trigger content/Pragma/subtype-mismatch`);
  // Error code: CWS-R10-034 (content/Pragma/subtype-mismatch — Layer 2)
  assert(
    hasErrorCode(results, "CWS-R10-034"),
    `Expected CWS-R10-034 (subtype-mismatch), got: ${errorCodeReport(results)}`,
  );
});

Deno.test("R05/metadata/valid-library: no content-aware false positives", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const contentPragma = byRule(results, "content/Pragma/");
  const contentMeta = byRule(results, "content/Metadata/");
  assertEquals(contentPragma.length, 0,
    `Valid library should have 0 content/Pragma infos: ${JSON.stringify(contentPragma.map(r => r.rule))}`);
  assertEquals(contentMeta.length, 0,
    `Valid library should have 0 content/Metadata infos: ${JSON.stringify(contentMeta.map(r => r.rule))}`);
});

// ---------------------------------------------------------------------------
// closing/ — Zone content and depth checks
// ---------------------------------------------------------------------------

Deno.test("[CWS-R05-030] R05/closing/x1-missing-fields: detects missing Careful: field in X1", async () => {
  const results = await go.lint(fixture("go/closing/x1-missing-fields.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  assert(hasRule(results, "closing/X1-content"), "Should detect missing X1 content fields");
  // Error code: CWS-R05-030 (closing/{{tag}}-content — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-030"),
    `Expected CWS-R05-030 (closing content), got: ${errorCodeReport(results)}`,
  );

  // Should NOT trigger required-X1 (zone is present)
  const reqX1 = byRule(results, "closing/required-X1");
  assertEquals(reqX1.length, 0, "Should not flag X1 as missing when it's present");

  const x1Content = byRule(results, "closing/X1-content");
  assertEquals(x1Content[0]!.severity, "info", "X1 content check should be info-level");
  assert(hasMessage(results, "careful"), "Should specifically mention missing 'careful' field");
});

Deno.test("[CWS-R05-030] R05/closing/x5-missing-scripture: detects missing Scripture: field in X5", async () => {
  const results = await go.lint(fixture("go/closing/x5-missing-scripture.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  assert(hasRule(results, "closing/X5-content"), "Should detect missing X5 content fields");
  // Error code: CWS-R05-030 (closing/{{tag}}-content — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-030"),
    `Expected CWS-R05-030 (closing content), got: ${errorCodeReport(results)}`,
  );

  // Should NOT trigger required-X5 (zone is present)
  const reqX5 = byRule(results, "closing/required-X5");
  assertEquals(reqX5.length, 0, "Should not flag X5 as missing when it's present");

  const x5Content = byRule(results, "closing/X5-content");
  assertEquals(x5Content[0]!.severity, "info", "X5 content check should be info-level");
  assert(hasMessage(results, "scripture"), "Should specifically mention missing 'scripture' field");
});

Deno.test("[CWS-R05-031] R05/closing/X1-depth: detects empty and placeholder field values in X1", async () => {
  const results = await go.lint(fixture("go/closing/x1-empty-fields.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  const depth = byRule(results, "closing/X1-depth");
  assertGreater(depth.length, 0, "Should detect empty/placeholder X1 field values");
  assertEquals(depth[0]!.severity, "info", "X1-depth should be info-level");
  // Error code: CWS-R05-031 (closing/{{tag}}-depth — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-031"),
    `Expected CWS-R05-031 (closing depth), got: ${errorCodeReport(depth)}`,
  );
  // "Never:" is empty, "Careful: [placeholder]" is placeholder
  assert(hasMessage(depth, "never") || hasMessage(depth, "careful"),
    "Should mention the field with empty/placeholder value");
});

Deno.test("R05/closing/depth: valid-library has no depth warnings", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const depth = results.filter((r) => r.rule.includes("-depth"));
  assertEquals(depth.length, 0, "Valid library should produce no depth warnings");
});

// ---------------------------------------------------------------------------
// setup/ — SETUP header documentation
// ---------------------------------------------------------------------------

Deno.test("[CWS-R05-007] R05/setup/no-header-doc: detects SETUP without header documentation", async () => {
  const results = await go.lint(fixture("go/setup/no-header-doc.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  assert(hasRule(results, "setup/header-doc"), "Should detect missing SETUP header documentation");
  // Error code: CWS-R05-007 (setup/header-doc — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-007"),
    `Expected CWS-R05-007 (setup header doc), got: ${errorCodeReport(results)}`,
  );

  const headerDoc = byRule(results, "setup/header-doc");
  assertEquals(headerDoc[0]!.severity, "info", "Header-doc check should be info-level");
});

// ---------------------------------------------------------------------------
// body/ — BODY subtype subsection names
// ---------------------------------------------------------------------------

Deno.test("[CWS-R05-008] R05/body/wrong-subtype-subsections: detects library with demo-test subsection names", async () => {
  const results = await go.lint(fixture("go/body/wrong-subtype-subsections.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  assert(hasRule(results, "body/subtype-subsections"), "Should detect wrong subsection names for library subtype");
  // Error code: CWS-R05-008 (body/subtype-subsections — Layer 3)
  assert(
    hasErrorCode(results, "CWS-R05-008"),
    `Expected CWS-R05-008 (body subtype subsections), got: ${errorCodeReport(results)}`,
  );

  const subtypeResults = byRule(results, "body/subtype-subsections");
  assertEquals(subtypeResults[0]!.severity, "info", "Subtype subsection check should be info-level");
  assert(hasMessage(results, "Public APIs"), "Should mention missing 'Public APIs' canonical subsection");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// R05 Go concept/detail tests — 16 tests covering:
// field content validation (values, patterns, enums), content-aware checks
// (pragma-driven: template vs derived, subtype mismatch), closing zone
// content/depth, setup header docs, body subtype subsections.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
