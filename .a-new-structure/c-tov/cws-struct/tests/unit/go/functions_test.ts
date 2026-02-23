// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/unit/go/functions_test.ts
// purpose: Unit tests for Go handler exported functions. Tests pure functions
//          directly with synthetic data — no fixture files, no lint pipeline.
//          classifyGoLine, getTopLevelDeclarations, getSubsectionRanges,
//          parseSliceFields, validateICFields, validateICFieldContent.
// source:  Split from tests/handlers/go_test.ts
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import { fixture, lazyFormat } from "../../helpers.ts";
import {
  parseSliceFields, validateICFields, validateICFieldContent,
  PRAGMA_FIELD_REQUIREMENTS, METADATA_FIELD_REQUIREMENTS,
  PRAGMA_CONTENT_RULES, METADATA_CONTENT_RULES,
  classifyGoLine, getSubsectionRanges, getTopLevelDeclarations,
} from "../../../lib/handlers/go.ts";
import type { GoContentKind } from "../../../lib/handlers/go.ts";

// ============================================================================
// BODY
// ============================================================================

// Lazy handler — needed for getSubsectionRanges test that triggers rule loading.
const go = lazyFormat("go");

// ---------------------------------------------------------------------------
// classifyGoLine
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

// ---------------------------------------------------------------------------
// getTopLevelDeclarations
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// getSubsectionRanges
// ---------------------------------------------------------------------------

Deno.test("unit/getSubsectionRanges: finds subsection boundaries (new format)", async () => {
  // Trigger lazy loading of Go rules (SETUP_SUBSECTIONS) before testing
  await go.lint(fixture("go/structure/valid-library.go"));

  const lines = [
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 1. Imports",
    "// ──────────────────────────────────────────────────────────────────────────",
    "import \"fmt\"",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 7. Core Types",
    "// ──────────────────────────────────────────────────────────────────────────",
    "type Foo struct {}",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 3. Constants",
    "// ──────────────────────────────────────────────────────────────────────────",
    "const Max = 5",
  ];
  const ranges = getSubsectionRanges(lines);
  assertEquals(ranges.length, 3);
  assertEquals(ranges[0]!.tag, "Imports");
  assertEquals(ranges[1]!.tag, "Core Types");
  assertEquals(ranges[2]!.tag, "Constants");
  // Imports ends where Core Types starts
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

// ---------------------------------------------------------------------------
// parseSliceFields
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// validateICFields
// ---------------------------------------------------------------------------

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
// validateICFieldContent
// ---------------------------------------------------------------------------

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

// ============================================================================
// CLOSING
// ============================================================================
//
// Unit tests for Go handler functions — 21 tests covering:
// classifyGoLine, getTopLevelDeclarations, getSubsectionRanges,
// parseSliceFields, validateICFields, validateICFieldContent.
// Pure function tests with synthetic data — no fixture pipeline dependency.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
