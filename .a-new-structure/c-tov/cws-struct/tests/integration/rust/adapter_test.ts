// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/rust/adapter_test.ts
// purpose: RustAdapter interface tests — verifies the LanguageAdapter contract
//          for the Rust format handler (format, extensions, classify, parse,
//          directives, zones, context extras).
// source:  Split from tests/handlers/rust_test.ts
// layer:   Integration — adapter interface
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import { rustAdapter } from "../../../lib/handlers/rust.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// adapter/ — RustAdapter implements LanguageAdapter
// ---------------------------------------------------------------------------

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

// ============================================================================
// CLOSING
// ============================================================================
//
// RustAdapter interface tests — 12 tests verifying the LanguageAdapter
// contract: format, extensions, classify, parse, directives, zones,
// patterns, context extras.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
