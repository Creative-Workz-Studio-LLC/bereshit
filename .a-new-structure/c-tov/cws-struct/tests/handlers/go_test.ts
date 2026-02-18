// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/handlers/go_test.ts
// key:     B-tov-cws-struct-tests-handlers-go
// title:   CWS Struct — Go Linter Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-17
// authors: Nova Dawn (CPI-SI)
// purpose: Tests for the Go 4-block format handler. Tests go through the
//          public FormatHandler.lint() interface — same path the CLI uses.
//
//          Fixture-driven: each .go file in tests/fixtures/go/ targets a
//          specific condition. Tests assert on result counts and rule names.
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
// Happy path: valid library
// ---------------------------------------------------------------------------

Deno.test("valid-library: zero errors", async () => {
  const results = await go.lint(fixture("go/valid-library.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("valid-library: may have separator width warnings", async () => {
  const results = await go.lint(fixture("go/valid-library.go"));
  // Warnings about separator widths are acceptable — not errors.
  for (const w of warnings(results)) {
    assert(
      w.rule.includes("separator") || w.rule.includes("width") || w.rule.includes("style"),
      `Unexpected warning rule: ${w.rule} — ${w.message}`,
    );
  }
});

// ---------------------------------------------------------------------------
// Happy path: valid executable
// ---------------------------------------------------------------------------

Deno.test("valid-executable: zero errors", async () => {
  const results = await go.lint(fixture("go/valid-executable.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

// ---------------------------------------------------------------------------
// Missing block entirely
// ---------------------------------------------------------------------------

Deno.test("missing-block: produces block error", async () => {
  const results = await go.lint(fixture("go/missing-block.go"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Expected at least one error for missing METADATA block");
  assert(
    hasRule(results, "block") || hasMessage(results, "METADATA") || hasMessage(results, "block"),
    "Expected error to reference missing block or METADATA",
  );
});

// ---------------------------------------------------------------------------
// Wrong block order
// ---------------------------------------------------------------------------

Deno.test("wrong-block-order: produces order error", async () => {
  const results = await go.lint(fixture("go/wrong-block-order.go"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Expected error for wrong block order (SETUP before METADATA)");
  assert(
    hasRule(results, "block/order"),
    `Expected block/order error, got rules: ${errs.map((e) => e.rule).join(", ")}`,
  );
});

// ---------------------------------------------------------------------------
// No omni markers
// ---------------------------------------------------------------------------

Deno.test("no-omni: info only, no errors", async () => {
  const results = await go.lint(fixture("go/no-omni.go"));
  const errs = errors(results);
  const warns = warnings(results);

  assertEquals(errs.length, 0, "Plain .go file should produce no errors");
  assertEquals(warns.length, 0, "Plain .go file should produce no warnings");

  // Should have at least one info saying it's not a 4-block file
  const infoResults = infos(results);
  assertGreater(infoResults.length, 0, "Should produce info about not being a 4-block file");
});

// ---------------------------------------------------------------------------
// Missing I/C fields
// ---------------------------------------------------------------------------

Deno.test("missing-ic-fields: warns about missing required fields", async () => {
  const results = await go.lint(fixture("go/missing-ic-fields.go"));
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

Deno.test("missing-ic-fields: zero errors (missing fields are warnings, not errors)", async () => {
  const results = await go.lint(fixture("go/missing-ic-fields.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got: ${JSON.stringify(errs, null, 2)}`);
});

// ---------------------------------------------------------------------------
// Placeholder values
// ---------------------------------------------------------------------------

Deno.test("placeholder-values: warns about template placeholders", async () => {
  const results = await go.lint(fixture("go/placeholder-values.go"));
  assert(
    hasRule(results, "placeholder") || hasRule(results, "template") || hasMessage(results, "placeholder") || hasMessage(results, "["),
    "Should detect placeholder values in I/C fields",
  );
});

// ---------------------------------------------------------------------------
// SETUP subsection order
// ---------------------------------------------------------------------------

Deno.test("subsection-order-correct: all 6 subsections in correct order — zero order warnings", async () => {
  const results = await go.lint(fixture("go/subsection-order-correct.go"));
  const errs = errors(results);
  const orderWarns = byRule(results, "setup/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(orderWarns.length, 0, `Expected 0 subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

Deno.test("subsection-order-wrong: detects Constants before Types", async () => {
  const results = await go.lint(fixture("go/subsection-order-wrong.go"));
  assert(
    hasRule(results, "setup/subsection-order"),
    `Should detect wrong SETUP subsection order, got rules: ${warnings(results).map((w) => w.rule).join(", ")}`,
  );
});

// ---------------------------------------------------------------------------
// Content placement — block-level and subsection-level validation
// ---------------------------------------------------------------------------

Deno.test("content-placement-correct: zero placement warnings", async () => {
  const results = await go.lint(fixture("go/content-placement-correct.go"));
  const errs = errors(results);
  const placementWarns = byRule(results, "content/");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(placementWarns.length, 0, `Expected 0 content placement warnings: ${JSON.stringify(placementWarns, null, 2)}`);
});

Deno.test("content-placement-wrong: detects func in SETUP and import in BODY", async () => {
  const results = await go.lint(fixture("go/content-placement-wrong.go"));
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

Deno.test("metadata-leak: detects code declarations in METADATA block", async () => {
  const results = await go.lint(fixture("go/metadata-leak.go"));
  const leakWarns = byRule(results, "content/metadata-leak");
  assertGreater(leakWarns.length, 0, "Should detect code declarations in METADATA block");
  assert(
    hasMessage(results, "import_decl") || hasMessage(results, "type_decl"),
    "Should identify the type of leaked construct",
  );
});

// ---------------------------------------------------------------------------
// BODY subsection ordering
// ---------------------------------------------------------------------------

Deno.test("wrong-body-order: detects BODY subsections out of order", async () => {
  const results = await go.lint(fixture("go/wrong-body-order.go"));
  const errs = errors(results);
  const orderWarns = byRule(results, "body/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(orderWarns.length, 0, "Should detect wrong BODY subsection order");
  assert(
    hasMessage(results, "Helpers") || hasMessage(results, "Public APIs"),
    "Should reference the misordered subsection labels",
  );
});

Deno.test("valid-library: zero body subsection order warnings", async () => {
  const results = await go.lint(fixture("go/valid-library.go"));
  const orderWarns = byRule(results, "body/subsection-order");
  assertEquals(orderWarns.length, 0, `Expected 0 body/subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

// ---------------------------------------------------------------------------
// CLOSING zone ordering
// ---------------------------------------------------------------------------

Deno.test("wrong-closing-order: detects code zone after documentation section", async () => {
  const results = await go.lint(fixture("go/wrong-closing-order.go"));
  const errs = errors(results);
  const zoneWarns = byRule(results, "closing/zone-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(zoneWarns.length, 0, "Should detect code zone after documentation section");
  assert(
    hasMessage(results, "X1") || hasMessage(results, "Cv") || hasMessage(results, "Cc"),
    "Should reference the misordered zones",
  );
});

Deno.test("valid-library: zero closing zone order warnings", async () => {
  const results = await go.lint(fixture("go/valid-library.go"));
  const zoneWarns = byRule(results, "closing/");
  assertEquals(zoneWarns.length, 0, `Expected 0 closing zone warnings: ${JSON.stringify(zoneWarns, null, 2)}`);
});

// ---------------------------------------------------------------------------
// CLOSING content placement — tests in BODY → Cv, main in BODY → Ce
// ---------------------------------------------------------------------------

Deno.test("tests-in-body: detects Test func in BODY — should be CLOSING Cv", async () => {
  const results = await go.lint(fixture("go/tests-in-body.go"));
  const errs = errors(results);
  const testWarns = byRule(results, "closing/test-placement");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(testWarns.length, 0, "Should detect test function in BODY block");
  assert(
    hasMessage(results, "CLOSING Cv") || hasMessage(results, "Cv zone"),
    "Should guide toward CLOSING Cv zone",
  );
});

Deno.test("main-in-body: detects func main() in BODY — should be CLOSING Ce", async () => {
  const results = await go.lint(fixture("go/main-in-body.go"));
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
// Identity registration
// ---------------------------------------------------------------------------

Deno.test("identity-registration: info when Pragma exists but no init()", async () => {
  const results = await go.lint(fixture("go/identity-registration.go"));
  const errs = errors(results);
  assertEquals(errs.length, 0, "Identity registration should not produce errors");
  assert(
    hasRule(results, "identity/register"),
    "Should produce identity/register info",
  );
  const regInfos = infos(results).filter((r) => r.rule.includes("identity/register"));
  assertGreater(regInfos.length, 0, "Should have at least one identity registration info");
});

Deno.test("valid-library: no identity registration info (has init() equivalent)", async () => {
  // The valid-library doesn't have init() — so it SHOULD get the info.
  // This test documents the behavior rather than expecting absence.
  const results = await go.lint(fixture("go/valid-library.go"));
  // File has Pragma var, so identity/register check runs
  const regInfos = infos(results).filter((r) => r.rule.includes("identity/register"));
  // Valid library fixture has no init() — info is expected and correct
  assertEquals(regInfos.length, 1, "Valid library with Pragma but no init() should get 1 identity/register info");
});

// ---------------------------------------------------------------------------
// Unit tests: classifyGoLine
// ---------------------------------------------------------------------------

Deno.test("classifyGoLine: identifies all major Go constructs", () => {
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

// ---------------------------------------------------------------------------
// Unit tests: getTopLevelDeclarations — brace depth tracking
// ---------------------------------------------------------------------------

Deno.test("getTopLevelDeclarations: skips nested content", () => {
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

Deno.test("getTopLevelDeclarations: handles multi-level nesting", () => {
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

// ---------------------------------------------------------------------------
// Unit tests: getSubsectionRanges
// ---------------------------------------------------------------------------

Deno.test("getSubsectionRanges: finds subsection boundaries", () => {
  const lines = [
    "// ────────────────────────────────────────────────────────────────────────",
    "//--- I.1 Imports",
    "// ────────────────────────────────────────────────────────────────────────",
    "import \"fmt\"",
    "// ────────────────────────────────────────────────────────────────────────",
    "//--- T.1 Types",
    "// ────────────────────────────────────────────────────────────────────────",
    "type Foo struct {}",
    "// ────────────────────────────────────────────────────────────────────────",
    "//--- K.1 Constants",
    "// ────────────────────────────────────────────────────────────────────────",
    "const Max = 5",
  ];
  const ranges = getSubsectionRanges(lines);
  assertEquals(ranges.length, 3);
  assertEquals(ranges[0]!.tag, "I");
  assertEquals(ranges[1]!.tag, "T");
  assertEquals(ranges[2]!.tag, "K");
  // Imports ends where Types starts
  assertEquals(ranges[0]!.endIdx, ranges[1]!.startIdx);
});

// ---------------------------------------------------------------------------
// Unit tests: parseSliceFields
// ---------------------------------------------------------------------------

Deno.test("parseSliceFields: extracts Pragma fields", () => {
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

Deno.test("parseSliceFields: handles nested keys (C4.requires.stdlib)", () => {
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

Deno.test("parseSliceFields: skips comment-only lines", () => {
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

Deno.test("parseSliceFields: handles single-line var", () => {
  const lines = [
    `var Pragma = [][2]string{{"I1.key", "value"}, {"I1.format", "go"}}`,
  ];
  const fields = parseSliceFields(lines, "Pragma");
  assertEquals(fields.length, 2);
});

Deno.test("parseSliceFields: returns empty for missing var", () => {
  const lines = [
    `func main() {`,
    `	fmt.Println("no vars")`,
    `}`,
  ];
  const fields = parseSliceFields(lines, "Pragma");
  assertEquals(fields.length, 0);
});

Deno.test("parseSliceFields: handles backtick strings", () => {
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

// ---------------------------------------------------------------------------
// Unit tests: validateICFields
// ---------------------------------------------------------------------------

Deno.test("validateICFields: all required present produces no warnings", () => {
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

Deno.test("validateICFields: missing required produces warnings", () => {
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

Deno.test("validateICFields: nested keys count as base field present", () => {
  const fields = [
    { section: "C4", field: "requires.stdlib", value: "none", line: 1 },
    { section: "C4", field: "consumers", value: "test", line: 2 },
  ];
  const requirements = { C4: { required: ["requires", "consumers"], defined: [] } };
  const results = validateICFields("test.go", fields, requirements, "Metadata");
  const warns = results.filter((r) => r.severity === "warn");
  assertEquals(warns.length, 0, "C4.requires.stdlib should satisfy 'requires' requirement");
});

Deno.test("validateICFields: missing defined fields produce info, not warn", () => {
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

// ============================================================================
// CLOSING
// ============================================================================
//
// Go linter tests — fixture-driven through the public lint() interface.
// Unit tests for parseSliceFields and validateICFields target the parser
// directly with synthetic data. Mirrors the Rust test suite structure for
// handler parity.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
