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
  parseSliceFields, validateICFields,
  PRAGMA_FIELD_REQUIREMENTS, METADATA_FIELD_REQUIREMENTS,
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
  const zoneWarns = byRule(results, "closing/");
  assertEquals(zoneWarns.length, 0, `Expected 0 closing zone warnings: ${JSON.stringify(zoneWarns, null, 2)}`);
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

// ---------------------------------------------------------------------------
// transform/ — Transformer tests
// ---------------------------------------------------------------------------

Deno.test("transform/valid-library: no changes needed (dry-run)", async () => {
  const f = fixture("go/structure/valid-library.go");
  const results = await go.transform!(f, { dryRun: true, extensions: false });
  const moves = byRule(results, "move-tests").concat(
    byRule(results, "move-main"),
    byRule(results, "reorder-closing"),
  );
  assertEquals(moves.length, 0, "Valid library should need no structural transforms");
});

Deno.test("transform/valid-executable: no changes needed (dry-run)", async () => {
  const f = fixture("go/structure/valid-executable.go");
  const results = await go.transform!(f, { dryRun: true, extensions: false });
  const moves = byRule(results, "move-tests").concat(
    byRule(results, "move-main"),
    byRule(results, "reorder-closing"),
  );
  assertEquals(moves.length, 0, "Valid executable should need no structural transforms");
});

Deno.test("transform/move-tests: dry-run detects Test func in BODY", async () => {
  const f = fixture("go/closing/tests-in-body.go");
  const results = await go.transform!(f, { dryRun: true, extensions: false });
  assert(hasRule(results, "move-tests"), "Should report move-tests for test func in BODY");
});

Deno.test("transform/move-tests: actual transform moves test to CLOSING Cv", async () => {
  const src = fixture("go/closing/tests-in-body.go");
  const tmp = await Deno.makeTempFile({ suffix: ".go" });
  await Deno.copyFile(src, tmp);

  try {
    const results = await go.transform!(tmp, { dryRun: false, extensions: false });
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
  const results = await go.transform!(f, { dryRun: true, extensions: false });
  assert(hasRule(results, "move-main"), "Should report move-main for func main() in BODY");
});

Deno.test("transform/move-main: actual transform moves main to CLOSING Ce", async () => {
  const src = fixture("go/closing/main-in-body.go");
  const tmp = await Deno.makeTempFile({ suffix: ".go" });
  await Deno.copyFile(src, tmp);

  try {
    const results = await go.transform!(tmp, { dryRun: false, extensions: false });
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
  const results = await go.transform!(f, { dryRun: true, extensions: false });
  assert(hasRule(results, "reorder-closing"), "Should detect out-of-order CLOSING zones");
});

Deno.test("transform/reorder-closing: actual transform fixes zone order", async () => {
  const src = fixture("go/closing/wrong-closing-order.go");
  const tmp = await Deno.makeTempFile({ suffix: ".go" });
  await Deno.copyFile(src, tmp);

  try {
    const results = await go.transform!(tmp, { dryRun: false, extensions: false });
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
// Go linter + transformer tests — fixture-driven through the public lint()
// and transform() interfaces. Unit tests for parseSliceFields, validateICFields,
// and getSubsectionRanges target the parser directly with synthetic data.
// Mirrors the Rust test suite structure for handler parity.
//
// Categories: structure/, metadata/, setup/, body/, closing/, format/, unit/, transform/
// Filter: deno test --filter "category/" tests/handlers/go_test.ts
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
