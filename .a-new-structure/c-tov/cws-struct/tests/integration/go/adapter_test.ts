// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/go/adapter_test.ts
// purpose: GoAdapter interface tests — verifies the LanguageAdapter contract.
//          Tests format identity, extension matching, line classification,
//          identity field parsing, directive finding, zone detection,
//          subsection enrichment, and context extras.
// source:  Split from tests/handlers/go_test.ts
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import { goAdapter } from "@lib/handlers/go.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// GoAdapter implements LanguageAdapter
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-110] adapter/go: format is 'go'", () => {
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
// GoAdapter interface tests — 11 tests verifying the LanguageAdapter contract.
// Tests adapter identity, delegation, zone detection, and context building.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
