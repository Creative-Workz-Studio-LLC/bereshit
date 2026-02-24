// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/unit/rust/functions_test.ts
// purpose: Pure unit tests for exported Rust handler functions — no fixtures,
//          no lint pipeline, just direct function invocation.
// source:  Split from tests/handlers/rust_test.ts
// layer:   Unit — pure function tests
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import { fixture, lazyFormat } from "@test/helpers.ts";
import {
  parseStaticFields, validateICFields,
  PRAGMA_FIELD_REQUIREMENTS, METADATA_FIELD_REQUIREMENTS,
  classifyLine, getSubsectionRanges, getTopLevelDeclarations,
} from "@lib/handlers/rust.ts";
import type { RustContentKind } from "@lib/handlers/rust.ts";

// ============================================================================
// BODY
// ============================================================================

// Lazy handler — needed only for getSubsectionRanges test (loads SETUP_SUBSECTIONS)
const rust = lazyFormat("rust");

// ---------------------------------------------------------------------------
// classifyLine — Line classification
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-108] unit/classifyLine: identifies all major Rust constructs", () => {
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

// ---------------------------------------------------------------------------
// getTopLevelDeclarations — Brace-depth tracking
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// getSubsectionRanges — Subsection boundary detection
// ---------------------------------------------------------------------------

Deno.test("unit/getSubsectionRanges: finds subsection boundaries", async () => {
  // Trigger lazy loading of Rust rules (SETUP_SUBSECTIONS) before testing
  await rust.lint(fixture("rust/structure/valid-library.rs"));

  const lines = [
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 1. Imports",
    "// ──────────────────────────────────────────────────────────────────────────",
    "use std::io;",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 3. Constants",
    "// ──────────────────────────────────────────────────────────────────────────",
    "const MAX: u32 = 5;",
    "// ──────────────────────────────────────────────────────────────────────────",
    "// 7. Core Types",
    "// ──────────────────────────────────────────────────────────────────────────",
    "struct Foo {}",
  ];
  const ranges = getSubsectionRanges(lines);
  assertEquals(ranges.length, 3);
  assertEquals(ranges[0]!.tag, "Imports");
  assertEquals(ranges[1]!.tag, "Constants");
  assertEquals(ranges[2]!.tag, "Core Types");
  // Imports ends where Constants starts
  assertEquals(ranges[0]!.endIdx, ranges[1]!.startIdx);
});

// ---------------------------------------------------------------------------
// parseStaticFields — PRAGMA/METADATA static field extraction
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// validateICFields — Identity/Context field validation
// ---------------------------------------------------------------------------

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

// ============================================================================
// CLOSING
// ============================================================================
//
// Unit tests for exported Rust handler functions — pure function testing.
// 15 tests covering: classifyLine (1), getTopLevelDeclarations (2),
// getSubsectionRanges (1), parseStaticFields (7), validateICFields (4).
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
