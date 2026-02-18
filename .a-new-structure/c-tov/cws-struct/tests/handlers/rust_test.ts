// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/handlers/rust_test.ts
// key:     B-tov-cws-struct-tests-handlers-rust
// title:   CWS Struct — Rust Linter Tests
// type:    Code (Test)
// version: b-01.50
// created: 2026-02-17
// authors: Nova Dawn (CPI-SI)
// purpose: Tests for the Rust 4-block format handler. Tests go through the
//          public FormatHandler.lint() interface — same path the CLI uses.
//
//          Fixture-driven: each .rs file in tests/fixtures/rust/ targets a
//          specific condition. Tests assert on result counts and rule names.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import { fixture, getFormat, errors, warnings, infos, byRule, hasRule, hasMessage } from "../helpers.ts";
import {
  parseStaticFields, validateICFields,
  PRAGMA_FIELD_REQUIREMENTS, METADATA_FIELD_REQUIREMENTS,
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
// Happy path: valid library
// ---------------------------------------------------------------------------

Deno.test("valid-library: zero errors", async () => {
  const results = await rust.lint(fixture("rust/valid-library.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

Deno.test("valid-library: may have separator width warnings", async () => {
  const results = await rust.lint(fixture("rust/valid-library.rs"));
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
  const results = await rust.lint(fixture("rust/valid-executable.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got ${errs.length}: ${JSON.stringify(errs, null, 2)}`);
});

// ---------------------------------------------------------------------------
// Missing block entirely
// ---------------------------------------------------------------------------

Deno.test("missing-block: produces block error", async () => {
  const results = await rust.lint(fixture("rust/missing-block.rs"));
  const errs = errors(results);
  assertGreater(errs.length, 0, "Expected at least one error for missing METADATA block");
  // Should mention the missing block
  assert(
    hasRule(results, "block") || hasMessage(results, "METADATA") || hasMessage(results, "block"),
    "Expected error to reference missing block or METADATA",
  );
});

// ---------------------------------------------------------------------------
// Missing I/C fields (fixture is lib.rs so isCrateRoot = true → I/C validation runs)
// ---------------------------------------------------------------------------

Deno.test("missing-ic-fields: warns about missing required fields", async () => {
  const results = await rust.lint(fixture("rust/missing-ic-crate/lib.rs"));
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

Deno.test("missing-ic-fields: zero errors (missing fields are warnings, not errors)", async () => {
  const results = await rust.lint(fixture("rust/missing-ic-crate/lib.rs"));
  const errs = errors(results);
  assertEquals(errs.length, 0, `Expected 0 errors, got: ${JSON.stringify(errs, null, 2)}`);
});

// ---------------------------------------------------------------------------
// Wrong block order
// ---------------------------------------------------------------------------

Deno.test("wrong-block-order: produces order error", async () => {
  const results = await rust.lint(fixture("rust/wrong-block-order.rs"));
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
  const results = await rust.lint(fixture("rust/no-omni.rs"));
  const errs = errors(results);
  const warns = warnings(results);

  assertEquals(errs.length, 0, "Plain .rs file should produce no errors");
  assertEquals(warns.length, 0, "Plain .rs file should produce no warnings");

  // Should have at least one info saying it's not a 4-block file
  const infoResults = infos(results);
  assertGreater(infoResults.length, 0, "Should produce info about not being a 4-block file");
});

// ---------------------------------------------------------------------------
// Placeholder values
// ---------------------------------------------------------------------------

Deno.test("placeholder-values: warns about template placeholders", async () => {
  const results = await rust.lint(fixture("rust/placeholder-values.rs"));
  // Should detect [bracketed] placeholder values
  assert(
    hasRule(results, "placeholder") || hasRule(results, "template") || hasMessage(results, "placeholder") || hasMessage(results, "["),
    "Should detect placeholder values in I/C fields",
  );
});

// ---------------------------------------------------------------------------
// 10-subsection SETUP order (dependency chain validation)
// ---------------------------------------------------------------------------

Deno.test("subsection-order-correct: all 10 subsections in correct order — zero order warnings", async () => {
  const results = await rust.lint(fixture("rust/subsection-order-correct.rs"));
  const errs = errors(results);
  const orderWarns = byRule(results, "setup/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(orderWarns.length, 0, `Expected 0 subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

Deno.test("subsection-order-wrong: detects Core Types before Constants", async () => {
  const results = await rust.lint(fixture("rust/subsection-order-wrong.rs"));
  assert(
    hasRule(results, "setup/subsection-order"),
    `Should detect wrong SETUP subsection order, got rules: ${warnings(results).map((w) => w.rule).join(", ")}`,
  );
});

Deno.test("subsection-partial-correct: 3 of 10 in correct relative order — zero order warnings", async () => {
  const results = await rust.lint(fixture("rust/subsection-partial-correct.rs"));
  const errs = errors(results);
  const orderWarns = byRule(results, "setup/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(orderWarns.length, 0, `Expected 0 subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

// ---------------------------------------------------------------------------
// Content placement — block-level and subsection-level validation
// ---------------------------------------------------------------------------

Deno.test("content-placement-correct: zero placement warnings", async () => {
  const results = await rust.lint(fixture("rust/content-placement-correct.rs"));
  const errs = errors(results);
  const placementWarns = byRule(results, "content/");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertEquals(placementWarns.length, 0, `Expected 0 content placement warnings: ${JSON.stringify(placementWarns, null, 2)}`);
});

Deno.test("content-placement-wrong: detects fn in SETUP and use/struct in BODY", async () => {
  const results = await rust.lint(fixture("rust/content-placement-wrong.rs"));
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

Deno.test("metadata-leak: detects code declarations in METADATA block", async () => {
  const results = await rust.lint(fixture("rust/metadata-leak.rs"));
  const leakWarns = byRule(results, "content/metadata-leak");
  assertGreater(leakWarns.length, 0, "Should detect code declarations in METADATA block");
  assert(
    hasMessage(results, "use_decl") || hasMessage(results, "struct_decl"),
    "Should identify the type of leaked construct",
  );
});

// ---------------------------------------------------------------------------
// BODY subsection ordering — numbered markers must be in ascending order
// ---------------------------------------------------------------------------

Deno.test("wrong-body-order: detects BODY subsections out of order", async () => {
  const results = await rust.lint(fixture("rust/wrong-body-order.rs"));
  const errs = errors(results);
  const orderWarns = byRule(results, "body/subsection-order");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(orderWarns.length, 0, "Should detect wrong BODY subsection order");
  assert(
    hasMessage(results, "§1") && hasMessage(results, "§4"),
    "Should reference the misordered subsection numbers",
  );
});

Deno.test("valid-library: zero body subsection order warnings", async () => {
  const results = await rust.lint(fixture("rust/valid-library.rs"));
  const orderWarns = byRule(results, "body/subsection-order");
  assertEquals(orderWarns.length, 0, `Expected 0 body/subsection-order warnings: ${JSON.stringify(orderWarns, null, 2)}`);
});

// ---------------------------------------------------------------------------
// CLOSING zone ordering — code zones before documentation, canonical order
// ---------------------------------------------------------------------------

Deno.test("wrong-closing-order: detects code zone after documentation section", async () => {
  const results = await rust.lint(fixture("rust/wrong-closing-order.rs"));
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
  const results = await rust.lint(fixture("rust/valid-library.rs"));
  const zoneWarns = byRule(results, "closing/");
  assertEquals(zoneWarns.length, 0, `Expected 0 closing zone warnings: ${JSON.stringify(zoneWarns, null, 2)}`);
});

// ---------------------------------------------------------------------------
// CLOSING content placement — tests in BODY → Cv, main in BODY → Ce
// ---------------------------------------------------------------------------

Deno.test("tests-in-body: detects #[cfg(test)] in BODY — should be CLOSING Cv", async () => {
  const results = await rust.lint(fixture("rust/tests-in-body.rs"));
  const errs = errors(results);
  const testWarns = byRule(results, "closing/test-placement");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(testWarns.length, 0, "Should detect test module in BODY block");
  assert(
    hasMessage(results, "CLOSING Cv") || hasMessage(results, "Cv zone"),
    "Should guide toward CLOSING Cv zone",
  );
});

Deno.test("main-in-body: detects fn main() in BODY — should be CLOSING Ce", async () => {
  const results = await rust.lint(fixture("rust/main-in-body.rs"));
  const errs = errors(results);
  const mainWarns = byRule(results, "closing/main-placement");
  assertEquals(errs.length, 0, `Expected 0 errors: ${JSON.stringify(errs, null, 2)}`);
  assertGreater(mainWarns.length, 0, "Should detect fn main() in BODY block");
  assert(
    hasMessage(results, "CLOSING Ce") || hasMessage(results, "Ce zone"),
    "Should guide toward CLOSING Ce zone",
  );
});

// ---------------------------------------------------------------------------
// Unit tests: classifyLine
// ---------------------------------------------------------------------------

Deno.test("classifyLine: identifies all major Rust constructs", () => {
  const cases: Array<[string, RustContentKind]> = [
    ["", "blank"],
    ["// comment", "comment"],
    ["//! doc comment", "comment"],
    ["/// item doc", "comment"],
    ["// ============================================================================", "comment"],
    ["use std::io;", "use_decl"],
    ["pub use crate::types::Config;", "use_decl"],
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
// Unit tests: getTopLevelDeclarations — brace depth tracking
// ---------------------------------------------------------------------------

Deno.test("getTopLevelDeclarations: skips nested content", () => {
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

Deno.test("getTopLevelDeclarations: handles multi-level nesting", () => {
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
// Unit tests: getSubsectionRanges
// ---------------------------------------------------------------------------

Deno.test("getSubsectionRanges: finds subsection boundaries", () => {
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

// ---------------------------------------------------------------------------
// Unit tests: parseStaticFields
// ---------------------------------------------------------------------------

Deno.test("parseStaticFields: extracts PRAGMA fields", () => {
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

Deno.test("parseStaticFields: handles nested keys (C4.requires.stdlib)", () => {
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

Deno.test("parseStaticFields: skips comment-only lines", () => {
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

Deno.test("parseStaticFields: handles single-line static", () => {
  const lines = [
    `pub static PRAGMA: &[(&str, &str)] = &[("I1.key", "value"), ("I1.format", "rust")];`,
  ];
  const fields = parseStaticFields(lines, "PRAGMA");
  assertEquals(fields.length, 2);
});

Deno.test("parseStaticFields: returns empty for missing static", () => {
  const lines = [
    `fn main() {`,
    `    println!("no statics");`,
    `}`,
  ];
  const fields = parseStaticFields(lines, "PRAGMA");
  assertEquals(fields.length, 0);
});

Deno.test("parseStaticFields: handles OWN_ prefix", () => {
  const lines = [
    `pub static OWN_PRAGMA: &[(&str, &str)] = &[`,
    `    ("I1.key", "value"),`,
    `];`,
  ];
  const fields = parseStaticFields(lines, "PRAGMA");
  assertEquals(fields.length, 1);
});

Deno.test("parseStaticFields: handles r# raw strings", () => {
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
// Unit tests: validateICFields
// ---------------------------------------------------------------------------

Deno.test("validateICFields: all required present produces no warnings", () => {
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

Deno.test("validateICFields: missing required produces warnings", () => {
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

Deno.test("validateICFields: nested keys count as base field present", () => {
  const fields = [
    { section: "C4", field: "requires.stdlib", value: "none", line: 1 },
    { section: "C4", field: "consumers", value: "test", line: 2 },
  ];
  const requirements = { C4: { required: ["requires", "consumers"], defined: [] } };
  const results = validateICFields("test.rs", fields, requirements, "METADATA");
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
  const results = validateICFields("test.rs", fields, requirements, "PRAGMA");
  const warns = results.filter((r) => r.severity === "warn");
  const infoResults = results.filter((r) => r.severity === "info");
  assertEquals(warns.length, 0, "Defined fields should not produce warnings");
  assertEquals(infoResults.length, 2, "Should produce info for each missing defined field");
});

// ---------------------------------------------------------------------------
// Transformer: cosmetic transforms (separator fixes)
// ---------------------------------------------------------------------------

Deno.test("transform-separators: dry-run detects ASCII dashes and prefix issues", async () => {
  const f = fixture("rust/transform-separators.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false });
  const dashRules = byRule(results, "dash-to-unicode");
  assert(dashRules.length >= 2, `Expected ≥2 dash-to-unicode, got ${dashRules.length}`);
  const prefixRules = byRule(results, "prefix-normalize");
  assert(prefixRules.length >= 1, `Expected ≥1 prefix-normalize, got ${prefixRules.length}`);
});

Deno.test("transform-separators: dry-run does not modify file", async () => {
  const f = fixture("rust/transform-separators.rs");
  const before = await Deno.readTextFile(f);
  await rust.transform!(f, { dryRun: true, extensions: false });
  const after = await Deno.readTextFile(f);
  assertEquals(before, after, "File should be unchanged after dry-run");
});

// ---------------------------------------------------------------------------
// Transformer: structural transforms (content moves)
// ---------------------------------------------------------------------------

Deno.test("transform move-tests: dry-run detects #[cfg(test)] in BODY", async () => {
  const f = fixture("rust/tests-in-body.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false });
  assert(hasRule(results, "move-tests"), "Should detect tests to move");
  assert(hasMessage(results, "#[cfg(test)]"), "Message should mention #[cfg(test)]");
});

Deno.test("transform move-tests: actual transform moves tests to CLOSING Cv", async () => {
  // Work on a temp copy
  const src = fixture("rust/tests-in-body.rs");
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

Deno.test("transform move-main: dry-run detects fn main() in BODY", async () => {
  const f = fixture("rust/main-in-body.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false });
  assert(hasRule(results, "move-main"), "Should detect main to move");
  assert(hasMessage(results, "fn main()"), "Message should mention fn main()");
});

Deno.test("transform move-main: actual transform moves main to CLOSING Ce", async () => {
  const src = fixture("rust/main-in-body.rs");
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

Deno.test("transform reorder-closing: dry-run detects out-of-order zones", async () => {
  const f = fixture("rust/wrong-closing-order.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false });
  assert(hasRule(results, "reorder-closing"), "Should detect zones needing reorder");
});

Deno.test("transform reorder-closing: actual transform fixes zone order", async () => {
  const src = fixture("rust/wrong-closing-order.rs");
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

Deno.test("transform valid-library: no changes needed", async () => {
  const f = fixture("rust/valid-library.rs");
  const results = await rust.transform!(f, { dryRun: true, extensions: false });
  // Valid library may have some separator width fixes but no structural moves
  const moves = results.filter((r) =>
    r.rule.includes("move-tests") ||
    r.rule.includes("move-main") ||
    r.rule.includes("reorder-closing"));
  assertEquals(moves.length, 0, "Valid library should need no structural transforms");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Rust linter + transformer tests — fixture-driven through the public
// lint() and transform() interfaces. Unit tests for parseStaticFields and
// validateICFields target the parser directly with synthetic data.
// Transformer tests use temp copies to verify non-destructive operation.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
