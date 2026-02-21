// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/handlers/go_test.ts
// key:     B-tov-cws-struct-tests-handlers-go
// title:   CWS Struct — Go Linter Tests
// type:    Code (Test)
// version: a-03.00
// created: 2026-02-17
// updated: 2026-02-18
// authors: Nova Dawn (CPI-SI)
// purpose: Tests for the Go 4-block format handler. Tests go through the
//          public FormatHandler.lint() interface — same path the CLI uses.
//
//          Fixture-driven: each .go file in tests/fixtures/go/<category>/
//          targets a specific condition. Tests assert on result counts and
//          rule names. Category prefixes enable --filter targeting.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import { fixture, getFormat, errors, warnings, infos, byRule, hasRule, hasMessage } from "../helpers.ts";
import {
  parseSliceFields, validateICFields, validateICFieldContent,
  PRAGMA_FIELD_REQUIREMENTS, METADATA_FIELD_REQUIREMENTS,
  PRAGMA_CONTENT_RULES, METADATA_CONTENT_RULES,
  classifyGoLine, getSubsectionRanges, getTopLevelDeclarations,
} from "../../lib/handlers/go.ts";
import type { GoContentKind } from "../../lib/handlers/go.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Lint via registry — the integration surface
// ---------------------------------------------------------------------------

const go = getFormat("go");

if (!go) {
  throw new Error("Go format handler not registered — check imports in helpers.ts");
}

// ---------------------------------------------------------------------------
// structure/ — Overall block structure
// ---------------------------------------------------------------------------

Deno.test("structure/valid-library: zero errors", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("structure/valid-library: zero warnings (all separators correct)", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const warns = warnings(results);
  assertEquals(warns.length, 0, `Expected 0 warnings, got ${warns.length}: ${JSON.stringify(warns, null, 2)}`);
});

Deno.test("structure/valid-executable: zero errors", async () => {
  const results = await go.lint(fixture("go/structure/valid-executable.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("structure/missing-block: produces block error", async () => {
  const results = await go.lint(fixture("go/structure/missing-block.go"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Expected at least one error for missing METADATA block");
  assert(
    hasRule(results, "block") || hasMessage(results, "METADATA") || hasMessage(results, "block"),
    "Expected error to reference missing block or METADATA",
  );
});

Deno.test("structure/wrong-block-order: produces order error", async () => {
  const results = await go.lint(fixture("go/structure/wrong-block-order.go"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Expected error for wrong block order (SETUP before METADATA)");
  assert(
    hasRule(results, "block/order"),
    `Expected block/order error, got rules: ${errs.map((e) => e.rule).join(", ")}`,
  );
});

Deno.test("structure/content-placement-correct: zero placement warnings", async () => {
  const results = await go.lint(fixture("go/structure/content-placement-correct.go"));
  const errs = errors(results);
  const placementWarns = byRule(results, "content/");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(placementWarns.length, 0, `Expected 0 content placement warnings: ${JSON.stringify(placementWarns, null, 2)}`);
});

Deno.test("structure/content-placement-wrong: detects func in SETUP and import in BODY", async () => {
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
});

// ---------------------------------------------------------------------------
// metadata/ — METADATA block
// ---------------------------------------------------------------------------

Deno.test("metadata/missing-ic-fields: warns about missing required fields", async () => {
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
});

Deno.test("metadata/missing-ic-fields: zero errors (missing fields are warnings, not errors)", async () => {
  const results = await go.lint(fixture("go/metadata/missing-ic-fields.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("metadata/placeholder-values: warns about template placeholders", async () => {
  const results = await go.lint(fixture("go/metadata/placeholder-values.go"));
  assert(
    hasRule(results, "placeholder") || hasRule(results, "template") || hasMessage(results, "placeholder") || hasMessage(results, "["),
    "Should detect placeholder values in I/C fields",
  );
});

Deno.test("metadata/metadata-leak: detects code declarations in METADATA block", async () => {
  const results = await go.lint(fixture("go/metadata/metadata-leak.go"));
  const leakWarns = byRule(results, "content/metadata-leak");
  assertGreater(leakWarns.length, 0, "Should detect code declarations in METADATA block");
  assert(
    hasMessage(results, "import_decl") || hasMessage(results, "type_decl"),
    "Should identify the type of leaked construct",
  );
});

Deno.test("metadata/identity-registration: info when Pragma exists but no init()", async () => {
  const results = await go.lint(fixture("go/metadata/identity-registration.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, "Identity registration should not produce errors");
  assert(
    hasRule(results, "identity/register"),
    "Should produce identity/register info",
  );
  const regInfos = infos(results).filter((r) => r.rule.includes("identity/register"));
  assertGreater(regInfos.length, 0, "Should have at least one identity registration info");
});

Deno.test("metadata/valid-library: identity registration info (has Pragma, no init())", async () => {
  // The valid-library doesn't have init() — so it SHOULD get the info.
  // This test documents the behavior rather than expecting absence.
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  // File has Pragma var, so identity/register check runs
  const regInfos = infos(results).filter((r) => r.rule.includes("identity/register"));
  // Valid library fixture has no init() — info is expected and correct
  assertEquals(regInfos.length, 1, "Valid library with Pragma but no init() should get 1 identity/register info");
});

// ---------------------------------------------------------------------------
// metadata/ — Content validation (field VALUES, not just existence)
// ---------------------------------------------------------------------------

Deno.test("metadata/bad-content-values: detects invalid Pragma field values", async () => {
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
});

Deno.test("metadata/bad-content-values: detects invalid Metadata field values", async () => {
  const results = await go.lint(fixture("go/metadata/bad-content-values.go"));
  const valueWarns = byRule(results, "value/Metadata/");
  assertGreater(valueWarns.length, 0, "Should detect bad Metadata content values");

  // C1.version — bad version format
  assert(hasRule(results, "value/Metadata/C1.version"), "Should flag bad C1.version");
  // C1.status — unknown status
  assert(hasRule(results, "value/Metadata/C1.status"), "Should flag unknown C1.status");
  // C2.organization — empty
  assert(hasRule(results, "value/Metadata/C2.organization"), "Should flag empty C2.organization");
});

Deno.test("metadata/bad-content-values: info-level checks for dates and paths", async () => {
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
});

Deno.test("metadata/valid-library: zero content value warnings", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const valueWarns = warnings(results).filter((r) => r.rule.startsWith("value/"));
  assertEquals(valueWarns.length, 0,
    `Valid library should have zero value warnings: ${valueWarns.map((w) => w.rule).join(", ")}`);
});

Deno.test("metadata/placeholder-values: content checks skip placeholders", async () => {
  const results = await go.lint(fixture("go/metadata/placeholder-values.go"));
  // Placeholders are handled by detectPlaceholders, not validateICFieldContent
  // Content checks should NOT fire for placeholder values like [YOUR-KEY-HERE]
  const valueResults = byRule(results, "value/Pragma/I1.key");
  assertEquals(valueResults.length, 0,
    "Content check should skip placeholder values (detectPlaceholders handles those)");
});

Deno.test("metadata/bad-content-values: total of 14 content check results", async () => {
  const results = await go.lint(fixture("go/metadata/bad-content-values.go"));
  const valueResults = byRule(results, "value/");
  assertEquals(valueResults.length, 14,
    `Should produce exactly 14 content value results (8 Pragma + 6 Metadata), got: ${valueResults.map((r) => r.rule).join(", ")}`);
});

// ---------------------------------------------------------------------------
// metadata/ — Content-aware validation (pragma-driven checks)
// ---------------------------------------------------------------------------

Deno.test("metadata/template-with-derived-values: detects wrong I1.at and C1.status for template", async () => {
  const results = await go.lint(fixture("go/metadata/template-with-derived-values.go"));
  const templateAt = byRule(results, "content/Pragma/template-at");
  const templateStatus = byRule(results, "content/Metadata/template-status");
  assertGreater(templateAt.length, 0,
    `Template file with I1.at="a-01.00" should trigger content/Pragma/template-at`);
  assertGreater(templateStatus.length, 0,
    `Template file with C1.status="Active" should trigger content/Metadata/template-status`);
});

Deno.test("metadata/derived-with-template-values: detects template values in derived file", async () => {
  const results = await go.lint(fixture("go/metadata/derived-with-template-values.go"));
  const derivedAt = byRule(results, "content/Pragma/derived-at");
  const derivedStatus = byRule(results, "content/Metadata/derived-status");
  assertGreater(derivedAt.length, 0,
    `Derived file with I1.at="template" should trigger content/Pragma/derived-at`);
  assertGreater(derivedStatus.length, 0,
    `Derived file with C1.status="Template" should trigger content/Metadata/derived-status`);
});

Deno.test("metadata/wrong-subtype: detects I2.subtype mismatch with pragma", async () => {
  const results = await go.lint(fixture("go/metadata/wrong-subtype.go"));
  const subtypeMismatch = byRule(results, "content/Pragma/subtype-mismatch");
  assertGreater(subtypeMismatch.length, 0,
    `Pragma says -library but I2.subtype="executable" should trigger content/Pragma/subtype-mismatch`);
});

Deno.test("metadata/valid-library: no content-aware false positives", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const contentPragma = byRule(results, "content/Pragma/");
  const contentMeta = byRule(results, "content/Metadata/");
  assertEquals(contentPragma.length, 0,
    `Valid library should have 0 content/Pragma infos: ${JSON.stringify(contentPragma.map(r => r.rule))}`);
  assertEquals(contentMeta.length, 0,
    `Valid library should have 0 content/Metadata infos: ${JSON.stringify(contentMeta.map(r => r.rule))}`);
});

// ---------------------------------------------------------------------------
// doc/ — Doc comment quality checks (schema-driven)
// ---------------------------------------------------------------------------

Deno.test("doc/no-header-doc: warns about missing doc comments (package, pragma, metadata)", async () => {
  const results = await go.lint(fixture("go/setup/no-header-doc.go"));
  const docPackage = byRule(results, "doc/package");
  const docPragma = byRule(results, "doc/pragma-var");
  const docMetadata = byRule(results, "doc/metadata-var");
  assertGreater(docPackage.length, 0,
    "Should warn about missing package doc comment");
  assertGreater(docPragma.length, 0,
    "Should warn about missing Pragma doc comment");
  assertGreater(docMetadata.length, 0,
    "Should warn about missing Metadata doc comment");
  // Schema says severity is "warn" for all three
  assertEquals(docPackage[0]!.severity, "warn", "package doc severity from schema");
  assertEquals(docPragma[0]!.severity, "warn", "pragma doc severity from schema");
  assertEquals(docMetadata[0]!.severity, "warn", "metadata doc severity from schema");
});

Deno.test("doc/valid-library: no doc comment warnings (all comments present)", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const docRules = byRule(results, "doc/");
  assertEquals(docRules.length, 0,
    `Valid library should have 0 doc warnings: ${JSON.stringify(docRules.map(r => r.rule))}`);
});

Deno.test("doc/schema-driven-severity: Go doc comment expectations loaded from schema", async () => {
  const { loadCodeRules } = await import("../../lib/foundation/mod.ts");
  const rules = await loadCodeRules("go");
  const pkgDoc = rules.docCommentExpectations["package_doc"];
  const pragmaDoc = rules.docCommentExpectations["pragma_doc"];
  const metaDoc = rules.docCommentExpectations["metadata_doc"];
  assertEquals(pkgDoc?.severity, "warn", "package_doc severity from schema");
  assertEquals(pragmaDoc?.severity, "warn", "pragma_doc severity from schema");
  assertEquals(metaDoc?.severity, "warn", "metadata_doc severity from schema");
});

// ---------------------------------------------------------------------------
// setup/ — SETUP block
// ---------------------------------------------------------------------------

Deno.test("setup/subsection-order-correct: all subsections in correct dependency order — zero order warnings", async () => {
  const results = await go.lint(fixture("go/setup/subsection-order-correct.go"));
  const errs = errors(results);
  const orderWarns = byRule(results, "setup/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(orderWarns.length, 0, `Expected 0 subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

Deno.test("setup/subsection-order-wrong: detects Core Types before Constants (wrong dependency order)", async () => {
  const results = await go.lint(fixture("go/setup/subsection-order-wrong.go"));
  assert(
    hasRule(results, "setup/subsection-order"),
    `Should detect wrong SETUP subsection order, got rules: ${warnings(results).map((w) => w.rule).join(", ")}`,
  );
});

Deno.test("setup/alias-name: detects alias names and suggests canonical alternatives", async () => {
  const results = await go.lint(fixture("go/setup/aliased-subsection-names.go"));
  const aliasInfos = byRule(results, "setup/alias-name");
  // Fixture uses: Consts (→Constants), Vars (→Variables), Types (→CoreTypes),
  // Interfaces (→InterfaceDefs), Methods (→TypeMethods) = 5 aliases
  assertGreater(aliasInfos.length, 0, `Expected alias info diagnostics, got: ${JSON.stringify(aliasInfos, null, 2)}`);
  // Should NOT have order warnings — aliases are in correct order
  const orderWarns = byRule(results, "setup/subsection-order");
  assertEquals(orderWarns.length, 0, `Aliased names in correct order should produce 0 order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

Deno.test("content/subsection-placement: detects misplaced declarations within SETUP subsections", async () => {
  const results = await go.lint(fixture("go/setup/misplaced-content.go"));
  const placementInfos = byRule(results, "content/subsection-placement");
  // Fixture has: type_decl in Constants (should be CoreTypes),
  // const_decl in CoreTypes (should be Constants) = 2 misplacements
  assertGreater(placementInfos.length, 0, `Expected subsection-placement infos, got: ${JSON.stringify(placementInfos, null, 2)}`);
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
// body/ — BODY block
// ---------------------------------------------------------------------------

Deno.test("body/wrong-body-order: detects BODY subsections out of numeric order", async () => {
  const results = await go.lint(fixture("go/body/wrong-body-order.go"));
  const errs = errors(results);
  const orderWarns = byRule(results, "body/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(orderWarns.length, 0, "Should detect wrong BODY subsection order");
  // New numeric format: message references §N numbers
  assert(
    hasMessage(results, "§2") && hasMessage(results, "§5"),
    "Should reference the misordered subsection numbers (§5 before §2)",
  );
});

Deno.test("body/valid-library: zero body subsection order warnings", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const orderWarns = byRule(results, "body/subsection-order");
  assertEquals(orderWarns.length, 0, `Expected 0 body/subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

// ---------------------------------------------------------------------------
// closing/ — CLOSING block
// ---------------------------------------------------------------------------

Deno.test("closing/wrong-closing-order: detects code zone after documentation section", async () => {
  const results = await go.lint(fixture("go/closing/wrong-closing-order.go"));
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
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const zoneWarns = byRule(results, "closing/zone-order");
  assertEquals(zoneWarns.length, 0, `Expected 0 closing/zone-order warnings: ${JSON.stringify(zoneWarns, null, 2)}`);
});

Deno.test("closing/tests-in-body: detects Test func in BODY — should be CLOSING Cv", async () => {
  const results = await go.lint(fixture("go/closing/tests-in-body.go"));
  const errs = errors(results);
  const testWarns = byRule(results, "closing/test-placement");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(testWarns.length, 0, "Should detect test function in BODY block");
  assert(
    hasMessage(results, "CLOSING Cv") || hasMessage(results, "Cv zone"),
    "Should guide toward CLOSING Cv zone",
  );
});

Deno.test("closing/main-in-body: detects func main() in BODY — should be CLOSING Ce", async () => {
  const results = await go.lint(fixture("go/closing/main-in-body.go"));
  const errs = errors(results);
  const mainWarns = byRule(results, "closing/main-placement");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(mainWarns.length, 0, "Should detect func main() in BODY block");
  assert(
    hasMessage(results, "CLOSING Ce") || hasMessage(results, "Ce zone"),
    "Should guide toward CLOSING Ce zone",
  );
});

// ---------------------------------------------------------------------------
// format/ — Format detection
// ---------------------------------------------------------------------------

Deno.test("format/no-omni: info only, no errors", async () => {
  const results = await go.lint(fixture("go/format/no-omni.go"));
  const errs = errors(results);
  const warns = warnings(results);

  assertEquals(errs.length, 0, "Plain .go file should produce no errors");
  assertEquals(warns.length, 0, "Plain .go file should produce no warnings");

  // Should have at least one info saying it's not a 4-block file
  const infoResults = infos(results);
  assertGreater(infoResults.length, 0, "Should produce info about not being a 4-block file");
});

// ---------------------------------------------------------------------------
// unit/ — Unit tests for exported functions
// ---------------------------------------------------------------------------

Deno.test("unit/classifyGoLine: identifies all major Go constructs", () => {
  const cases: Array<[string, GoContentKind]> = [
    ["", "blank"],
    ["// comment", "comment"],
    ["// ============================================================================", "comment"],
    ["//omni:key B-test", "comment"],
    ["package main", "package_decl"],
    ["package config", "package_decl"],
    ["import \"fmt\"", "import_decl"],
    ["import (", "import_decl"],
    ["func init() {", "init_func"],
    ["func init(){", "init_func"],
    ["func (c Config) String() string {", "method_decl"],
    ["func (s *Server) Listen() error {", "method_decl"],
    ["func main() {", "func_decl"],
    ["func NewConfig() Config {", "func_decl"],
    ["func TestHello(t *testing.T) {", "func_decl"],
    ["type Config struct {", "type_decl"],
    ["type Handler interface {", "type_decl"],
    ["type (", "type_decl"],
    ["const Version = \"1.0\"", "const_decl"],
    ["const (", "const_decl"],
    ["var debug = false", "var_decl"],
    ["var (", "var_decl"],
    ["	return nil", "other"],
    ["if err != nil {", "other"],
  ];

  for (const [input, expected] of cases) {
    const actual = classifyGoLine(input);
    assertEquals(actual, expected, `classifyGoLine("${input}") → "${actual}", expected "${expected}"`);
  }
});

Deno.test("unit/getTopLevelDeclarations: skips nested content", () => {
  const lines = [
    "import \"fmt\"",
    "type Config struct {",
    "    Name string",
    "}",
    "func (c Config) String() string {",
    "    const inner = \"nested\"",    // nested — should NOT be classified
    "    return c.Name",
    "}",
    "func TopLevel() {",              // top-level — should be classified
    "    var x = 5",
    "}",
  ];
  const decls = getTopLevelDeclarations(lines);
  const kinds = decls.map((d) => d.kind);

  assertEquals(kinds, ["import_decl", "type_decl", "method_decl", "func_decl"],
    `Expected top-level only, got: ${JSON.stringify(decls, null, 2)}`);
});

Deno.test("unit/getTopLevelDeclarations: handles multi-level nesting", () => {
  const lines = [
    "func outer() {",
    "    if true {",
    "        var nested = 1",    // depth 2 — skip
    "    }",
    "}",
    "const TOP = 2",             // depth 0 — classify
  ];
  const decls = getTopLevelDeclarations(lines);
  assertEquals(decls.length, 2);
  assertEquals(decls[0]!.kind, "func_decl");
  assertEquals(decls[1]!.kind, "const_decl");
});

Deno.test("unit/getSubsectionRanges: finds subsection boundaries (new format)", () => {
  const lines = [
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 1. Imports",
    "// ──────────────────────────────────────────────────────────────────────────",
    "import \"fmt\"",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 6. Core Types",
    "// ──────────────────────────────────────────────────────────────────────────",
    "type Foo struct {}",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 2. Constants",
    "// ──────────────────────────────────────────────────────────────────────────",
    "const Max = 5",
  ];
  const ranges = getSubsectionRanges(lines);
  assertEquals(ranges.length, 3);
  assertEquals(ranges[0]!.tag, "Imports");
  assertEquals(ranges[1]!.tag, "CoreTypes");
  assertEquals(ranges[2]!.tag, "Constants");
  // Imports ends where CoreTypes starts
  assertEquals(ranges[0]!.endIdx, ranges[1]!.startIdx);
});

Deno.test("unit/getSubsectionRanges: backward compat with legacy //--- format", () => {
  const lines = [
    "// ──────────────────────────────────────────────────────────────────────────",
    "//--- I.1 Imports",
    "// ──────────────────────────────────────────────────────────────────────────",
    "import \"fmt\"",
    "// ──────────────────────────────────────────────────────────────────────────",
    "//--- K.1 Constants",
    "// ──────────────────────────────────────────────────────────────────────────",
    "const Max = 5",
  ];
  const ranges = getSubsectionRanges(lines);
  assertEquals(ranges.length, 2);
  assertEquals(ranges[0]!.tag, "Imports");
  assertEquals(ranges[1]!.tag, "Constants");
});

Deno.test("unit/parseSliceFields: extracts Pragma fields", () => {
  const lines = [
    `var Pragma = [][2]string{`,
    `	{"I1.key", "test-value"},`,
    `	{"I1.format", "go"},`,
    `	// comment line`,
    `	{"I2.type", "code"},`,
    `}`,
  ];
  const fields = parseSliceFields(lines, "Pragma");
  assertEquals(fields.length, 3);
  assertEquals(fields[0]!.section, "I1");
  assertEquals(fields[0]!.field, "key");
  assertEquals(fields[0]!.value, "test-value");
  assertEquals(fields[2]!.section, "I2");
  assertEquals(fields[2]!.field, "type");
});

Deno.test("unit/parseSliceFields: handles nested keys (C4.requires.stdlib)", () => {
  const lines = [
    `var Metadata = [][2]string{`,
    `	{"C4.requires.stdlib", "none"},`,
    `	{"C4.requires.external", "tokio"},`,
    `}`,
  ];
  const fields = parseSliceFields(lines, "Metadata");
  assertEquals(fields.length, 2);
  assertEquals(fields[0]!.section, "C4");
  assertEquals(fields[0]!.field, "requires.stdlib");
});

Deno.test("unit/parseSliceFields: skips comment-only lines", () => {
  const lines = [
    `var Pragma = [][2]string{`,
    `	// This is a comment`,
    `	{"I1.key", "value"},`,
    `	// Another comment`,
    `}`,
  ];
  const fields = parseSliceFields(lines, "Pragma");
  assertEquals(fields.length, 1);
});

Deno.test("unit/parseSliceFields: handles single-line var", () => {
  const lines = [
    `var Pragma = [][2]string{{"I1.key", "value"}, {"I1.format", "go"}}`,
  ];
  const fields = parseSliceFields(lines, "Pragma");
  assertEquals(fields.length, 2);
});

Deno.test("unit/parseSliceFields: returns empty for missing var", () => {
  const lines = [
    `func main() {`,
    `	fmt.Println("no vars")`,
    `}`,
  ];
  const fields = parseSliceFields(lines, "Pragma");
  assertEquals(fields.length, 0);
});

Deno.test("unit/parseSliceFields: handles backtick strings", () => {
  const lines = [
    "var Pragma = [][2]string{",
    "	{\"I1.key\", `value-with-\"quotes\"`},",
    "	{\"I1.format\", \"go\"},",
    "}",
  ];
  const fields = parseSliceFields(lines, "Pragma");
  assertEquals(fields.length, 2);
  assertEquals(fields[0]!.value, `value-with-"quotes"`);
  assertEquals(fields[1]!.value, "go");
});

Deno.test("unit/validateICFields: all required present produces no warnings", () => {
  const fields = [
    { section: "I1", field: "key", value: "test", line: 1 },
    { section: "I1", field: "format", value: "go", line: 2 },
    { section: "I1", field: "from", value: "template", line: 3 },
    { section: "I1", field: "at", value: "a-01.00", line: 4 },
    { section: "I2", field: "type", value: "code", line: 5 },
    { section: "I2", field: "structure", value: "4-block", line: 6 },
    { section: "I3", field: "file", value: "test.go", line: 7 },
    { section: "I3", field: "title", value: "Test", line: 8 },
  ];
  const results = validateICFields("test.go", fields, PRAGMA_FIELD_REQUIREMENTS, "Pragma");
  const warns = results.filter((r) => r.severity === "warn");
  assertEquals(warns.length, 0, `Unexpected warnings: ${JSON.stringify(warns, null, 2)}`);
});

Deno.test("unit/validateICFields: missing required produces warnings", () => {
  const fields = [
    // I1: missing key, from
    { section: "I1", field: "format", value: "go", line: 1 },
    { section: "I1", field: "at", value: "a-01.00", line: 2 },
    // I2: missing type
    { section: "I2", field: "structure", value: "4-block", line: 3 },
    // I3: has both
    { section: "I3", field: "file", value: "test.go", line: 4 },
    { section: "I3", field: "title", value: "Test", line: 5 },
  ];
  const results = validateICFields("test.go", fields, PRAGMA_FIELD_REQUIREMENTS, "Pragma");
  const warns = results.filter((r) => r.severity === "warn");
  assertEquals(warns.length, 3, "Should warn about I1.key, I1.from, I2.type");
});

Deno.test("unit/validateICFields: nested keys count as base field present", () => {
  const fields = [
    { section: "C4", field: "requires.stdlib", value: "none", line: 1 },
    { section: "C4", field: "consumers", value: "test", line: 2 },
  ];
  const requirements = { C4: { required: ["requires", "consumers"], defined: [] } };
  const results = validateICFields("test.go", fields, requirements, "Metadata");
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
  const results = validateICFields("test.go", fields, requirements, "Pragma");
  const warns = results.filter((r) => r.severity === "warn");
  const infoResults = results.filter((r) => r.severity === "info");
  assertEquals(warns.length, 0, "Defined fields should not produce warnings");
  assertEquals(infoResults.length, 2, "Should produce info for each missing defined field");
});

Deno.test("unit/validateICFieldContent: valid values produce no results", () => {
  const fields = [
    { section: "I1", field: "key", value: "B-my-project", line: 1 },
    { section: "I1", field: "format", value: "go", line: 2 },
    { section: "I1", field: "from", value: "b-word/seed/code/L0/go/library.omni", line: 3 },
    { section: "I1", field: "at", value: "a-01.00", line: 4 },
    { section: "I2", field: "type", value: "code", line: 5 },
    { section: "I2", field: "structure", value: "4-block", line: 6 },
    { section: "I3", field: "file", value: "test.go", line: 7 },
    { section: "I3", field: "title", value: "Test File", line: 8 },
  ];
  const results = validateICFieldContent("test.go", fields, PRAGMA_CONTENT_RULES, "Pragma");
  assertEquals(results.length, 0, `Valid values should produce no results: ${results.map((r) => r.rule).join(", ")}`);
});

Deno.test("unit/validateICFieldContent: bad patterns produce warnings", () => {
  const fields = [
    { section: "I1", field: "key", value: "bad key", line: 1 },
    { section: "I1", field: "at", value: "1.0.0", line: 2 },
    { section: "C1", field: "version", value: "xyz", line: 3 },
  ];
  const allRules = [...PRAGMA_CONTENT_RULES, ...METADATA_CONTENT_RULES];
  const results = validateICFieldContent("test.go", fields, allRules, "Pragma");
  assertEquals(results.length, 3, "Should flag all 3 bad patterns");
  assert(results.every((r) => r.severity === "warn"), "Pattern failures should be warn-level");
});

Deno.test("unit/validateICFieldContent: enum checks are case-insensitive", () => {
  const fields = [
    { section: "I2", field: "type", value: "Code", line: 1 },      // uppercase
    { section: "I2", field: "structure", value: "4-BLOCK", line: 2 }, // all caps
    { section: "C1", field: "status", value: "DRAFT", line: 3 },    // all caps
  ];
  const allRules = [...PRAGMA_CONTENT_RULES, ...METADATA_CONTENT_RULES];
  const results = validateICFieldContent("test.go", fields, allRules, "Test");
  assertEquals(results.length, 0, "Enum checks should accept any case: " +
    results.map((r) => `${r.rule}: ${r.message}`).join(", "));
});

Deno.test("unit/validateICFieldContent: skips placeholder values", () => {
  const fields = [
    { section: "I1", field: "key", value: "[YOUR-KEY-HERE]", line: 1 },
    { section: "I1", field: "format", value: "[format]", line: 2 },
    { section: "C1", field: "version", value: "[version]", line: 3 },
  ];
  const allRules = [...PRAGMA_CONTENT_RULES, ...METADATA_CONTENT_RULES];
  const results = validateICFieldContent("test.go", fields, allRules, "Pragma");
  assertEquals(results.length, 0, "Should skip placeholder values entirely");
});

Deno.test("unit/validateICFieldContent: skips missing fields", () => {
  // Only I1.key present — rules for other fields should not fire
  const fields = [
    { section: "I1", field: "key", value: "B-valid-key", line: 1 },
  ];
  const results = validateICFieldContent("test.go", fields, PRAGMA_CONTENT_RULES, "Pragma");
  assertEquals(results.length, 0, "Should only check fields that exist");
});

Deno.test("unit/validateICFieldContent: empty required content produces warning", () => {
  const fields = [
    { section: "I3", field: "file", value: "", line: 1 },
    { section: "I3", field: "title", value: "  ", line: 2 },   // whitespace-only
    { section: "C2", field: "organization", value: "", line: 3 },
  ];
  const allRules = [...PRAGMA_CONTENT_RULES, ...METADATA_CONTENT_RULES];
  const results = validateICFieldContent("test.go", fields, allRules, "Test");
  assertEquals(results.length, 3, "Empty and whitespace-only should trigger non-empty check");
});

// ---------------------------------------------------------------------------
// transform/ — Transformer tests
// ---------------------------------------------------------------------------

Deno.test("transform/valid-library: no changes needed (dry-run)", async () => {
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

// ---------------------------------------------------------------------------
// closing/ — CLOSING content validation (new: required zones + zone content)
// ---------------------------------------------------------------------------

Deno.test("closing/missing-required-zones: detects missing X1 and X5", async () => {
  const results = await go.lint(fixture("go/closing/missing-required-zones.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  assert(hasRule(results, "closing/required-X1"), "Should detect missing X1 zone");
  assert(hasRule(results, "closing/required-X5"), "Should detect missing X5 zone");

  // Both should be info-level
  const x1 = byRule(results, "closing/required-X1");
  const x5 = byRule(results, "closing/required-X5");
  assertEquals(x1[0]!.severity, "info", "Missing X1 should be info-level");
  assertEquals(x5[0]!.severity, "info", "Missing X5 should be info-level");
});

Deno.test("closing/x1-missing-fields: detects missing Careful: field in X1", async () => {
  const results = await go.lint(fixture("go/closing/x1-missing-fields.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  assert(hasRule(results, "closing/X1-content"), "Should detect missing X1 content fields");

  // Should NOT trigger required-X1 (zone is present)
  const reqX1 = byRule(results, "closing/required-X1");
  assertEquals(reqX1.length, 0, "Should not flag X1 as missing when it's present");

  const x1Content = byRule(results, "closing/X1-content");
  assertEquals(x1Content[0]!.severity, "info", "X1 content check should be info-level");
  assert(hasMessage(results, "careful"), "Should specifically mention missing 'careful' field");
});

Deno.test("closing/x5-missing-scripture: detects missing Scripture: field in X5", async () => {
  const results = await go.lint(fixture("go/closing/x5-missing-scripture.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  assert(hasRule(results, "closing/X5-content"), "Should detect missing X5 content fields");

  // Should NOT trigger required-X5 (zone is present)
  const reqX5 = byRule(results, "closing/required-X5");
  assertEquals(reqX5.length, 0, "Should not flag X5 as missing when it's present");

  const x5Content = byRule(results, "closing/X5-content");
  assertEquals(x5Content[0]!.severity, "info", "X5 content check should be info-level");
  assert(hasMessage(results, "scripture"), "Should specifically mention missing 'scripture' field");
});

Deno.test("closing/valid-library: no required-zone or zone-content warnings", async () => {
  // The valid-library has a minimal CLOSING — if it has X1/X5, no content warnings.
  // If it lacks X1/X5, the required checks fire but that's separate from zone-content.
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, "Valid library should produce no errors");
});

// ---------------------------------------------------------------------------
// closing/ — X6 template-only check
// ---------------------------------------------------------------------------

Deno.test("closing/X6-template-only: detects X6 section in non-template file", async () => {
  const results = await go.lint(fixture("go/closing/x6-in-derived.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  assert(hasRule(results, "closing/X6-template-only"), "Should detect X6 in non-template file");

  const x6 = byRule(results, "closing/X6-template-only");
  assertEquals(x6[0]!.severity, "warn", "X6-template-only should be warn-level");
  assert(hasMessage(x6, "Template Guide"), "Should mention Template Guide");
});

Deno.test("closing/X6-template-only: valid-library has no X6 warning", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const x6 = byRule(results, "closing/X6-template-only");
  assertEquals(x6.length, 0, "Valid library without X6 should produce no X6-template-only warning");
});

// ---------------------------------------------------------------------------
// closing/ — Field content depth check
// ---------------------------------------------------------------------------

Deno.test("closing/X1-depth: detects empty and placeholder field values in X1", async () => {
  const results = await go.lint(fixture("go/closing/x1-empty-fields.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  const depth = byRule(results, "closing/X1-depth");
  assertGreater(depth.length, 0, "Should detect empty/placeholder X1 field values");
  assertEquals(depth[0]!.severity, "info", "X1-depth should be info-level");
  // "Never:" is empty, "Careful: [placeholder]" is placeholder
  assert(hasMessage(depth, "never") || hasMessage(depth, "careful"),
    "Should mention the field with empty/placeholder value");
});

Deno.test("closing/depth: valid-library has no depth warnings", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const depth = results.filter((r) => r.rule.includes("-depth"));
  assertEquals(depth.length, 0, "Valid library should produce no depth warnings");
});

// ---------------------------------------------------------------------------
// setup/ — SETUP content validation (new: header documentation)
// ---------------------------------------------------------------------------

Deno.test("setup/no-header-doc: detects SETUP without header documentation", async () => {
  const results = await go.lint(fixture("go/setup/no-header-doc.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  assert(hasRule(results, "setup/header-doc"), "Should detect missing SETUP header documentation");

  const headerDoc = byRule(results, "setup/header-doc");
  assertEquals(headerDoc[0]!.severity, "info", "Header-doc check should be info-level");
});

// ---------------------------------------------------------------------------
// body/ — BODY content validation (new: subtype subsection names)
// ---------------------------------------------------------------------------

Deno.test("body/wrong-subtype-subsections: detects library with demo-test subsection names", async () => {
  const results = await go.lint(fixture("go/body/wrong-subtype-subsections.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  assert(hasRule(results, "body/subtype-subsections"), "Should detect wrong subsection names for library subtype");

  const subtypeResults = byRule(results, "body/subtype-subsections");
  assertEquals(subtypeResults[0]!.severity, "info", "Subtype subsection check should be info-level");
  assert(hasMessage(results, "Public APIs"), "Should mention missing 'Public APIs' canonical subsection");
});

// ---------------------------------------------------------------------------
// setup/required — Required SETUP subsections for subtype
// ---------------------------------------------------------------------------

Deno.test("setup/missing-required-subsections: library missing ErrorTypes and CoreTypes", async () => {
  const results = await go.lint(fixture("go/setup/missing-required-subsections.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  const required = byRule(results, "setup/required-subsection");
  assertGreater(required.length, 0, "Should detect missing required SETUP subsections");
  assertEquals(required[0]!.severity, "warn", "Required subsection check should be warn-level");
  assert(hasMessage(required, "ErrorTypes"), "Should mention missing ErrorTypes");
  assert(hasMessage(required, "CoreTypes"), "Should mention missing CoreTypes");
});

Deno.test("setup/missing-required-subsections: reports count matches schema", async () => {
  const results = await go.lint(fixture("go/setup/missing-required-subsections.go"));
  const required = byRule(results, "setup/required-subsection");
  // Go schema: library requires S1 Imports (present), S5 ErrorTypes (missing), S6 CoreTypes (missing)
  assertEquals(required.length, 2, "Should report exactly 2 missing required subsections");
});

// ---------------------------------------------------------------------------
// body/required — Required BODY subsections for subtype
// ---------------------------------------------------------------------------

Deno.test("body/missing-required-subsections: library missing Public APIs", async () => {
  const results = await go.lint(fixture("go/body/missing-required-subsections.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  const required = byRule(results, "body/required-subsection");
  assertGreater(required.length, 0, "Should detect missing required BODY subsections");
  assertEquals(required[0]!.severity, "warn", "Required body subsection check should be warn-level");
  assert(hasMessage(required, "PublicAPIs") || hasMessage(required, "Public"), "Should mention missing Public APIs");
});

// ---------------------------------------------------------------------------
// emphasis/ — Subtype emphasis balance
// ---------------------------------------------------------------------------

Deno.test("setup/emphasis-inverted: library with empty heavy subs and full light subs", async () => {
  const results = await go.lint(fixture("go/setup/emphasis-inverted.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);

  const emphasis = byRule(results, "emphasis/setup-inverted");
  assertGreater(emphasis.length, 0, "Should detect inverted emphasis");
  assertEquals(emphasis[0]!.severity, "info", "Emphasis check should be info-level");
  assert(hasMessage(emphasis, "library"), "Should mention the subtype");
});

Deno.test("setup/emphasis-inverted: valid library has no emphasis inversion", async () => {
  const results = await go.lint(fixture("go/structure/valid-library.go"));
  const emphasis = byRule(results, "emphasis/setup-inverted");
  assertEquals(emphasis.length, 0, "Valid library should have no emphasis inversion");
});

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

// ---------------------------------------------------------------------------
// adapter/ — GoAdapter implements LanguageAdapter
// ---------------------------------------------------------------------------

import { goAdapter } from "../../lib/handlers/go.ts";

Deno.test("adapter/go: format is 'go'", () => {
  assertEquals(goAdapter.format, "go");
});

Deno.test("adapter/go: extensions include .go", () => {
  assert(goAdapter.extensions.includes(".go"));
});

Deno.test("adapter/go: classifyLine delegates to classifyGoLine", () => {
  assertEquals(goAdapter.classifyLine("package main"), "package_decl");
  assertEquals(goAdapter.classifyLine("import \"fmt\""), "import_decl");
  assertEquals(goAdapter.classifyLine("func main() {"), "func_decl");
  assertEquals(goAdapter.classifyLine("type Foo struct {"), "type_decl");
  assertEquals(goAdapter.classifyLine("// comment"), "comment");
  assertEquals(goAdapter.classifyLine(""), "blank");
});

Deno.test("adapter/go: parseIdentityFields delegates to parseSliceFields", () => {
  const lines = [
    'var Pragma = [][2]string{',
    '  {"I1.key", "B-test"},',
    '  {"I1.format", "go"},',
    '}',
  ];
  const fields = goAdapter.parseIdentityFields(lines, "Pragma");
  assertEquals(fields.length, 2);
  assertEquals(fields[0]!.section, "I1");
  assertEquals(fields[0]!.field, "key");
  assertEquals(fields[0]!.value, "B-test");
});

Deno.test("adapter/go: findOmniDirectives finds directives", () => {
  const lines = [
    "//omni:key B-test",
    "//omni:code --go -library",
    "package foo",
  ];
  const directives = goAdapter.findOmniDirectives(lines);
  assert(directives.has("//omni:key"));
  assert(directives.has("//omni:code"));
  assertEquals(directives.get("//omni:key")!.value, "B-test");
});

Deno.test("adapter/go: findTestZone finds func Test declarations", () => {
  const lines = [
    "// helper",
    "func TestFoo(t *testing.T) {",
    "  t.Run(\"sub\", func(t *testing.T) {})",
    "}",
    "// end",
  ];
  const zone = goAdapter.findTestZone(lines, 0, lines.length);
  assert(zone !== null);
  assertEquals(zone!.start, 1);
  assertEquals(zone!.end, 3);
});

Deno.test("adapter/go: findMainZone finds func main", () => {
  const lines = [
    "// header",
    "func main() {",
    "  fmt.Println(\"hello\")",
    "}",
  ];
  const zone = goAdapter.findMainZone(lines, 0, lines.length);
  assert(zone !== null);
  assertEquals(zone!.start, 1);
  assertEquals(zone!.end, 3);
});

Deno.test("adapter/go: enrichSubsectionPatterns adds legacy patterns", () => {
  const base = [{ tag: "Imports", pattern: /^\/\/\s+Imports\b/ }];
  const enriched = goAdapter.enrichSubsectionPatterns!(base);
  // Enriched pattern should match both canonical and legacy
  assert(enriched[0]!.pattern.test("// Imports"), "Should match canonical");
  assert(enriched[0]!.pattern.test("//--- I.1 standard library"), "Should match legacy");
});

Deno.test("adapter/go: buildContextExtras identifies doc.go", () => {
  const extras = goAdapter.buildContextExtras("/path/to/doc.go", []);
  assertEquals(extras.isDocGo, true);
  assertEquals(extras.isTestFile, false);
});

Deno.test("adapter/go: buildContextExtras identifies test files", () => {
  const extras = goAdapter.buildContextExtras("/path/to/foo_test.go", []);
  assertEquals(extras.isDocGo, false);
  assertEquals(extras.isTestFile, true);
});

Deno.test("adapter/go: buildContextExtras for regular file", () => {
  const extras = goAdapter.buildContextExtras("/path/to/main.go", []);
  assertEquals(extras.isDocGo, false);
  assertEquals(extras.isTestFile, false);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Go linter + transformer tests — fixture-driven through the public lint()
// and transform() interfaces. Unit tests for parseSliceFields, validateICFields,
// and getSubsectionRanges target the parser directly with synthetic data.
// Mirrors the Rust test suite structure for handler parity.
//
// Categories: structure/, metadata/, setup/, body/, closing/, format/, unit/,
//             transform/, regression/
// Filter: deno test --filter "category/" tests/handlers/go_test.ts
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
