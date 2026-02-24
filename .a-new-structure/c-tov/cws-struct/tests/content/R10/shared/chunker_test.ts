// R-level: R10 — Format-level tests (single-format, single-concern)
// Migrated from: tests/shared/chunker_test.ts
// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R10/shared/chunker_test.ts
// key:     B-tov-cws-struct-tests-shared-chunker
// title:   CWS Struct — Universal Chunking Engine Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Verify the universal chunking engine — config-driven parsing,
//          delimiter tracking, doc/attr attachment, import grouping,
//          and structural pattern detection. Tests use a minimal "test
//          language" config to isolate the engine from any real handler.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import {
  parseChunks,
  isOmniPragma,
  isSeparatorBanner,
  isRegularComment,
} from "@lib/shared/chunker.ts";
import type { ChunkerConfig } from "@lib/shared/chunker.ts";
import type { GenericChunk } from "@lib/shared/scaffold.ts";

// ---------------------------------------------------------------------------
// Test language config — minimal C-like language for testing
// ---------------------------------------------------------------------------

/**
 * Minimal "test language" that recognizes:
 *   - fn_decl: `fn ...` or `func ...` (balanced with {})
 *   - import_decl: `use ...` or `import ...` (import grouping)
 *   - type_decl: `type ...` or `struct ...` (balanced with {})
 *   - const_decl: `const ...`
 *   - var_decl: `var ...`
 *   - paren_block: `import (` or `const (` (balanced with ())
 */
function testClassify(trimmed: string): string {
  if (trimmed === "") return "blank";
  if (trimmed.startsWith("//")) return "comment";
  if (/^(fn|func)\s+\w/.test(trimmed)) return "fn_decl";
  if (/^(use|import)\s+/.test(trimmed) && !trimmed.endsWith("(")) return "import_decl";
  if (/^(import|const|var)\s*\($/.test(trimmed)) return "paren_block";
  if (/^(type|struct)\s+\w/.test(trimmed)) return "type_decl";
  if (/^const\s+\w/.test(trimmed)) return "const_decl";
  if (/^var\s+\w/.test(trimmed)) return "var_decl";
  if (trimmed.startsWith("#[")) return "attr";
  return "other";
}

const testConfig: ChunkerConfig = {
  classifyLine: testClassify,

  patterns: {
    pragma: isOmniPragma,
    fileDoc: (t) => t.startsWith("//!"),
    separator: isSeparatorBanner,
    comment: isRegularComment,
    itemDoc: (t) => t.startsWith("///"),
    attribute: (t) => t.startsWith("#["),
    testBlock: (t) => t === "#[cfg(test)]",
  },

  balancedKinds: new Set([
    "fn_decl", "type_decl", "paren_block",
  ]),

  importKinds: new Set(["import_decl"]),

  kindToCategory: (kind, trimmed) => {
    if (kind === "fn_decl") {
      return /^(fn|func)\s+main\s*\(/.test(trimmed) ? "main_fn" : "fn_decl";
    }
    if (kind === "paren_block") return "paren_block";
    if (kind === "type_decl") return "type_def";
    if (kind === "const_decl") return "const";
    if (kind === "var_decl") return "var";
    if (kind === "attr") return "attr";
    return "other";
  },
};

// ---------------------------------------------------------------------------
// Test helpers
// ---------------------------------------------------------------------------

/** Find chunks by category. */
function findChunks(chunks: GenericChunk[], cat: string): GenericChunk[] {
  return chunks.filter((c) => c.category === cat);
}

/** Total line count across all chunks. */
function totalLines(chunks: GenericChunk[]): number {
  return chunks.reduce((sum, c) => sum + c.lines.length, 0);
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// C1: Empty and trivial inputs
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-100] chunker/C1: empty input produces no chunks", () => {
  const chunks = parseChunks([], testConfig);
  assertEquals(chunks.length, 0);
});

Deno.test("chunker/C1: single blank line produces one blank chunk", () => {
  const chunks = parseChunks([""], testConfig);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "blank");
});

Deno.test("chunker/C1: single comment produces one comment chunk", () => {
  const chunks = parseChunks(["// hello"], testConfig);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "comment");
});

// ---------------------------------------------------------------------------
// C2: Pragma detection
// ---------------------------------------------------------------------------

Deno.test("chunker/C2: pragma lines grouped together", () => {
  const lines = [
    "// #!omni code --go -library",
    "//omni:key B-test-001",
    "//omni:version a-01.00",
    "",
    "// regular comment",
  ];
  const chunks = parseChunks(lines, testConfig);
  const pragmas = findChunks(chunks, "pragma");
  assertEquals(pragmas.length, 1);
  assertEquals(pragmas[0]!.lines.length, 3);
});

// ---------------------------------------------------------------------------
// C3: File-level doc comments
// ---------------------------------------------------------------------------

Deno.test("chunker/C3: file doc comments grouped (//! syntax)", () => {
  const lines = [
    "//! This is a crate doc",
    "//! Second line of crate doc",
    "",
    "use std::io;",
  ];
  const chunks = parseChunks(lines, testConfig);
  const docs = findChunks(chunks, "doc_comment");
  assertEquals(docs.length, 1);
  assertEquals(docs[0]!.lines.length, 2);
});

// ---------------------------------------------------------------------------
// C4: Blank line grouping
// ---------------------------------------------------------------------------

Deno.test("chunker/C4: contiguous blank lines grouped", () => {
  const lines = ["", "", "", "// after blanks"];
  const chunks = parseChunks(lines, testConfig);
  assertEquals(chunks[0]!.category, "blank");
  assertEquals(chunks[0]!.lines.length, 3);
});

// ---------------------------------------------------------------------------
// C5: Separator detection
// ---------------------------------------------------------------------------

Deno.test("chunker/C5: separator banner with title captured", () => {
  const lines = [
    "// ─────────────────────────────────────────────",
    "// Section Title",
    "// ─────────────────────────────────────────────",
    "fn something() {}",
  ];
  const chunks = parseChunks(lines, testConfig);
  const seps = findChunks(chunks, "separator");
  assertEquals(seps.length, 1);
  assertEquals(seps[0]!.lines.length, 3);
});

// ---------------------------------------------------------------------------
// C6: Comment grouping
// ---------------------------------------------------------------------------

Deno.test("chunker/C6: contiguous comments grouped", () => {
  const lines = [
    "// first comment",
    "// second comment",
    "// third comment",
    "fn foo() {}",
  ];
  const chunks = parseChunks(lines, testConfig);
  assertEquals(chunks[0]!.category, "comment");
  assertEquals(chunks[0]!.lines.length, 3);
});

// ---------------------------------------------------------------------------
// C7: Brace-balanced items
// ---------------------------------------------------------------------------

Deno.test("chunker/C7: single-line fn captured", () => {
  const chunks = parseChunks(["fn foo() {}"], testConfig);
  const fns = findChunks(chunks, "fn_decl");
  assertEquals(fns.length, 1);
  assertEquals(fns[0]!.lines.length, 1);
});

Deno.test("chunker/C7: multi-line fn captured until brace balance", () => {
  const lines = [
    "fn bar() {",
    "    let x = 1;",
    "    let y = 2;",
    "}",
  ];
  const chunks = parseChunks(lines, testConfig);
  const fns = findChunks(chunks, "fn_decl");
  assertEquals(fns.length, 1);
  assertEquals(fns[0]!.lines.length, 4);
});

Deno.test("chunker/C7: nested braces tracked correctly", () => {
  const lines = [
    "fn nested() {",
    "    if true {",
    "        println!(\"deep\");",
    "    }",
    "}",
  ];
  const chunks = parseChunks(lines, testConfig);
  const fns = findChunks(chunks, "fn_decl");
  assertEquals(fns.length, 1);
  assertEquals(fns[0]!.lines.length, 5);
});

Deno.test("chunker/C7: main fn gets main_fn category", () => {
  const lines = [
    "fn main() {",
    "    run();",
    "}",
  ];
  const chunks = parseChunks(lines, testConfig);
  const mains = findChunks(chunks, "main_fn");
  assertEquals(mains.length, 1);
});

// ---------------------------------------------------------------------------
// C8: Parenthesis-balanced items (Go import/const/var blocks)
// ---------------------------------------------------------------------------

Deno.test("chunker/C8: paren-balanced block captured", () => {
  const lines = [
    "import (",
    '    "fmt"',
    '    "os"',
    ")",
  ];
  const chunks = parseChunks(lines, testConfig);
  const parens = findChunks(chunks, "paren_block");
  assertEquals(parens.length, 1);
  assertEquals(parens[0]!.lines.length, 4);
});

Deno.test("chunker/C8: nested parens tracked", () => {
  const lines = [
    "const (",
    "    X = func(a, b int) int {",
    "        return a + b",
    "    }",
    ")",
  ];
  const chunks = parseChunks(lines, testConfig);
  const parens = findChunks(chunks, "paren_block");
  assertEquals(parens.length, 1);
  assertEquals(parens[0]!.lines.length, 5);
});

// ---------------------------------------------------------------------------
// C9: Import grouping
// ---------------------------------------------------------------------------

Deno.test("chunker/C9: contiguous imports grouped", () => {
  const lines = [
    "use std::io;",
    "use std::fs;",
    "",
    "use std::path;",
    "fn foo() {}",
  ];
  const chunks = parseChunks(lines, testConfig);
  const imports = findChunks(chunks, "import");
  assertEquals(imports.length, 1);
  // Should absorb both use lines AND the blank between them AND the third use
  assertEquals(imports[0]!.lines.length, 4);
});

// ---------------------------------------------------------------------------
// C10: Doc comment attachment to items
// ---------------------------------------------------------------------------

Deno.test("chunker/C10: /// doc comment attaches to next fn", () => {
  const lines = [
    "/// This function does things",
    "/// More docs",
    "fn documented() {}",
  ];
  const chunks = parseChunks(lines, testConfig);
  const fns = findChunks(chunks, "fn_decl");
  assertEquals(fns.length, 1);
  // Doc comments should be attached — fn chunk has 3 lines total
  assertEquals(fns[0]!.lines.length, 3);
  assert(fns[0]!.lines[0]!.includes("///"));
});

Deno.test("chunker/C10: blank line between /// and fn is skipped", () => {
  const lines = [
    "/// Doc for next item",
    "",
    "fn spaced() {}",
  ];
  const chunks = parseChunks(lines, testConfig);
  const fns = findChunks(chunks, "fn_decl");
  assertEquals(fns.length, 1);
  // Doc attached, blank dropped
  assertEquals(fns[0]!.lines.length, 2);
  assert(fns[0]!.lines[0]!.includes("///"));
});

// ---------------------------------------------------------------------------
// C11: Attribute attachment to items
// ---------------------------------------------------------------------------

Deno.test("chunker/C11: #[attr] attaches to next fn", () => {
  const lines = [
    "#[derive(Debug)]",
    "struct Foo {",
    "    x: i32,",
    "}",
  ];
  const chunks = parseChunks(lines, testConfig);
  const types = findChunks(chunks, "type_def");
  assertEquals(types.length, 1);
  // Attribute should be attached — type chunk has 4 lines
  assertEquals(types[0]!.lines.length, 4);
  assert(types[0]!.lines[0]!.includes("#[derive"));
});

// ---------------------------------------------------------------------------
// C12: Test block capture
// ---------------------------------------------------------------------------

Deno.test("chunker/C12: #[cfg(test)] captures entire test module", () => {
  const lines = [
    "#[cfg(test)]",
    "mod tests {",
    "    #[test]",
    "    fn it_works() {",
    "        assert!(true);",
    "    }",
    "}",
  ];
  const chunks = parseChunks(lines, testConfig);
  const tests = findChunks(chunks, "test_module");
  assertEquals(tests.length, 1);
  assertEquals(tests[0]!.lines.length, 7);
});

// ---------------------------------------------------------------------------
// C13: Single-line declarations
// ---------------------------------------------------------------------------

Deno.test("chunker/C13: const declaration produces single chunk", () => {
  const chunks = parseChunks(["const MAX = 100"], testConfig);
  const consts = findChunks(chunks, "const");
  assertEquals(consts.length, 1);
  assertEquals(consts[0]!.lines.length, 1);
});

Deno.test("chunker/C13: var declaration produces single chunk", () => {
  const chunks = parseChunks(["var count int"], testConfig);
  const vars = findChunks(chunks, "var");
  assertEquals(vars.length, 1);
  assertEquals(vars[0]!.lines.length, 1);
});

// ---------------------------------------------------------------------------
// C14: Line conservation — every line accounted for
// ---------------------------------------------------------------------------

Deno.test("chunker/C14: all input lines appear in output chunks", () => {
  const lines = [
    "// #!omni code --go -library",
    "//omni:key B-test",
    "",
    "// comment block",
    "// more comments",
    "",
    "use std::io;",
    "use std::fs;",
    "",
    "const MAX = 100;",
    "",
    "/// Doc for bar",
    "fn bar() {",
    "    x();",
    "}",
    "",
    "fn main() {",
    "    bar();",
    "}",
  ];
  const chunks = parseChunks(lines, testConfig);
  assertEquals(totalLines(chunks), lines.length);
});

// ---------------------------------------------------------------------------
// C15: Preset pattern helpers
// ---------------------------------------------------------------------------

Deno.test("chunker/C15: isOmniPragma detects pragma patterns", () => {
  assert(isOmniPragma("// #!omni code --go -library"));
  assert(isOmniPragma("//omni:key B-test"));
  assert(!isOmniPragma("// regular comment"));
  assert(!isOmniPragma("fn main() {}"));
});

Deno.test("chunker/C15: isSeparatorBanner detects separators", () => {
  assert(isSeparatorBanner("// ─────────────────────────────────────────"));
  assert(isSeparatorBanner("// ═══════════════════════════════════════════"));
  assert(isSeparatorBanner("// -----------------------------------------------"));
  assert(!isSeparatorBanner("// short ---"));
  assert(!isSeparatorBanner("fn main() {}"));
});

Deno.test("chunker/C15: isRegularComment detects regular comments", () => {
  assert(isRegularComment("// hello"));
  assert(!isRegularComment("//omni:key X"));
  assert(!isRegularComment("//! crate doc"));
  assert(!isRegularComment("fn main() {}"));
});

// ---------------------------------------------------------------------------
// C16: Mixed realistic file
// ---------------------------------------------------------------------------

Deno.test("chunker/C16: realistic file parses correctly", () => {
  const lines = [
    "// #!omni code --rust -library",
    "//omni:key B-test-lib",
    "",
    "//! My library crate",
    "//! Does amazing things",
    "",
    "use std::io;",
    "use std::fs;",
    "",
    "// ─────────────────────────────────────────────",
    "// Constants",
    "// ─────────────────────────────────────────────",
    "",
    "const MAX = 100;",
    "",
    "/// The Foo type",
    "#[derive(Debug)]",
    "struct Foo {",
    "    x: i32,",
    "}",
    "",
    "fn main() {",
    "    println!(\"hello\");",
    "}",
    "",
    "#[cfg(test)]",
    "mod tests {",
    "    #[test]",
    "    fn it_works() {",
    "        assert!(true);",
    "    }",
    "}",
  ];

  const chunks = parseChunks(lines, testConfig);

  // Verify all lines accounted for
  assertEquals(totalLines(chunks), lines.length);

  // Verify key chunk categories exist
  assert(findChunks(chunks, "pragma").length >= 1, "Should have pragma chunk");
  assert(findChunks(chunks, "doc_comment").length >= 1, "Should have doc comment");
  assert(findChunks(chunks, "import").length >= 1, "Should have import chunk");
  assert(findChunks(chunks, "separator").length >= 1, "Should have separator");
  assert(findChunks(chunks, "type_def").length >= 1, "Should have type chunk");
  assert(findChunks(chunks, "main_fn").length >= 1, "Should have main_fn chunk");
  assert(findChunks(chunks, "test_module").length >= 1, "Should have test_module");

  // The type_def chunk should include the doc comment + attribute + struct
  const typeDef = findChunks(chunks, "type_def")[0]!;
  assert(typeDef.lines.some((l) => l.includes("///")), "Type should have doc comment attached");
  assert(typeDef.lines.some((l) => l.includes("#[derive")), "Type should have attribute attached");
  assert(typeDef.lines.some((l) => l.includes("struct Foo")), "Type should have struct declaration");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Universal chunker tests — verify the engine works with a minimal test config,
// isolating engine behavior from any specific language handler.
// Each test group targets one parsing phase.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
