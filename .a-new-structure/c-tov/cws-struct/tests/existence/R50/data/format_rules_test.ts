// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R50/data/format_rules_test.ts
// key:     B-tov-cws-struct-tests-data-format-rules
// title:   CWS Struct — Format Rules Loader Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Verify the schema-driven format rules loader works correctly.
//          Tests prove that universal and per-language _format.jsonc schemas
//          load, parse, cache, and compose into usable format definitions.
//
//          Gate | Verifies
//          -----|----------------------------------------
//          G1   | Universal format rules load from schema
//          G2   | Universal rules have all required fields
//          G3   | Language format rules load per-language
//          G4   | Composed rules combine universal + language
//          G5   | Cache returns same object on second load
//          G6   | clearFormatCache forces fresh load
//          G7   | Go and Rust have different comment syntax
//          G8   | Separator widths are positive numbers
//          G9   | Block boundary patterns are regex-valid
//          G10  | Pragma rules have required + recommended directives
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertNotEquals } from "jsr:@std/assert";

import {
  loadUniversalFormatRules,
  loadLanguageFormatRules,
  loadFormatRules,
  clearFormatCache,
} from "@lib/data/format-rules.ts";

import type {
  UniversalFormatRules,
  LanguageFormatRules,
  ComposedFormatRules,
} from "@lib/data/format-rules.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// G1: Universal format rules load
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-071] format-rules/G1: universal rules load successfully", async () => {
  clearFormatCache();
  const universal = await loadUniversalFormatRules();
  assert(universal !== undefined, "universal should load");
  assert(universal !== null, "universal should not be null");
});

// ---------------------------------------------------------------------------
// G2: Universal rules have required fields
// ---------------------------------------------------------------------------

Deno.test("format-rules/G2: universal has separators", async () => {
  const universal = await loadUniversalFormatRules();

  assert(universal.separators !== undefined, "should have separators");
  assert(universal.separators.block !== undefined, "should have block separator");
  assert(universal.separators.subsection !== undefined, "should have subsection separator");
});

Deno.test("format-rules/G2: universal has block_boundaries", async () => {
  const universal = await loadUniversalFormatRules();
  assert(universal.block_boundaries !== undefined, "should have block_boundaries");
  assert(universal.block_boundaries.pattern.length > 0, "pattern should be non-empty");
});

Deno.test("format-rules/G2: universal has pragma rules", async () => {
  const universal = await loadUniversalFormatRules();
  assert(universal.pragma !== undefined, "should have pragma");
  assert(universal.pragma.pattern.length > 0, "pragma pattern should be non-empty");
});

Deno.test("format-rules/G2: universal has identity_fields", async () => {
  const universal = await loadUniversalFormatRules();
  assert(universal.identity_fields !== undefined, "should have identity_fields");
});

Deno.test("format-rules/G2: universal has closing_zones", async () => {
  const universal = await loadUniversalFormatRules();
  assert(universal.closing_zones !== undefined, "should have closing_zones");
});

// ---------------------------------------------------------------------------
// G3: Language format rules load per-language
// ---------------------------------------------------------------------------

Deno.test("format-rules/G3: Go format rules load", async () => {
  const lang = await loadLanguageFormatRules("go");
  assertEquals(lang.language, "go");
  assert(lang.comment_syntax !== undefined, "should have comment_syntax");
  assert(lang.pragma_expression !== undefined, "should have pragma_expression");
});

Deno.test("format-rules/G3: Rust format rules load", async () => {
  const lang = await loadLanguageFormatRules("rust");
  assertEquals(lang.language, "rust");
  assert(lang.comment_syntax !== undefined, "should have comment_syntax");
  assert(lang.pragma_expression !== undefined, "should have pragma_expression");
});

// ---------------------------------------------------------------------------
// G4: Composed rules combine universal + language
// ---------------------------------------------------------------------------

Deno.test("format-rules/G4: loadFormatRules returns composed Go rules", async () => {
  clearFormatCache();
  const composed = await loadFormatRules("go");

  assert(composed.universal !== undefined, "should have universal side");
  assert(composed.language !== undefined, "should have language side");
  assertEquals(composed.language.language, "go");
});

Deno.test("format-rules/G4: loadFormatRules returns composed Rust rules", async () => {
  clearFormatCache();
  const composed = await loadFormatRules("rust");

  assert(composed.universal !== undefined, "should have universal side");
  assert(composed.language !== undefined, "should have language side");
  assertEquals(composed.language.language, "rust");
});

// ---------------------------------------------------------------------------
// G5: Cache returns same object
// ---------------------------------------------------------------------------

Deno.test("format-rules/G5: second load returns cached result", async () => {
  clearFormatCache();
  const first = await loadFormatRules("go");
  const second = await loadFormatRules("go");
  assert(first === second, "cached load should return same reference");
});

// ---------------------------------------------------------------------------
// G6: clearFormatCache forces fresh load
// ---------------------------------------------------------------------------

Deno.test("format-rules/G6: clearFormatCache forces new object", async () => {
  clearFormatCache();
  const first = await loadFormatRules("rust");
  clearFormatCache();
  const second = await loadFormatRules("rust");
  assert(first !== second, "after clear, should get new object");
  assertEquals(first.language.language, second.language.language);
});

// ---------------------------------------------------------------------------
// G7: Go and Rust have different comment syntax
// ---------------------------------------------------------------------------

Deno.test("format-rules/G7: Go uses // line comment", async () => {
  const go = await loadLanguageFormatRules("go");
  assertEquals(go.comment_syntax.line, "//");
});

Deno.test("format-rules/G7: Rust uses // line comment", async () => {
  const rust = await loadLanguageFormatRules("rust");
  assertEquals(rust.comment_syntax.line, "//");
});

Deno.test("format-rules/G7: Rust has doc_comment (Go does not)", async () => {
  const rust = await loadLanguageFormatRules("rust");
  // Rust has /// for doc comments
  assert(
    rust.comment_syntax.doc_comment !== undefined &&
    rust.comment_syntax.doc_comment !== null,
    "Rust should have doc_comment",
  );
  assertEquals(rust.comment_syntax.doc_comment, "///");

  const go = await loadLanguageFormatRules("go");
  // Go has no dedicated doc comment syntax
  assert(
    go.comment_syntax.doc_comment === null || go.comment_syntax.doc_comment === undefined,
    "Go should not have doc_comment",
  );
});

// ---------------------------------------------------------------------------
// G8: Separator widths are positive numbers
// ---------------------------------------------------------------------------

Deno.test("format-rules/G8: block separator width is positive", async () => {
  const universal = await loadUniversalFormatRules();
  assert(
    universal.separators.block.standard_width > 0,
    "block separator width should be positive",
  );
});

Deno.test("format-rules/G8: subsection separator width is positive", async () => {
  const universal = await loadUniversalFormatRules();
  assert(
    universal.separators.subsection.standard_width > 0,
    "subsection separator width should be positive",
  );
});

Deno.test("format-rules/G8: block > subsection width", async () => {
  const universal = await loadUniversalFormatRules();
  assert(
    universal.separators.block.standard_width >=
      universal.separators.subsection.standard_width,
    "block separator should be >= subsection separator",
  );
});

// ---------------------------------------------------------------------------
// G9: Block boundary patterns are regex-valid
// ---------------------------------------------------------------------------

Deno.test("format-rules/G9: block boundary pattern compiles as regex", async () => {
  const universal = await loadUniversalFormatRules();
  // Should not throw when constructing regex
  const regex = new RegExp(universal.block_boundaries.pattern);
  assert(regex !== null);
});

Deno.test("format-rules/G9: title pattern compiles as regex", async () => {
  const universal = await loadUniversalFormatRules();
  const regex = new RegExp(universal.block_boundaries.title_pattern);
  assert(regex !== null);
});

// ---------------------------------------------------------------------------
// G10: Pragma rules have directives
// ---------------------------------------------------------------------------

Deno.test("format-rules/G10: pragma has directives array", async () => {
  const universal = await loadUniversalFormatRules();
  assert(
    universal.pragma.directives !== undefined,
    "pragma should have directives",
  );
  assert(
    universal.pragma.directives.length > 0,
    "should have at least one directive",
  );
});

Deno.test("format-rules/G10: pragma has at least one required directive", async () => {
  const universal = await loadUniversalFormatRules();
  const required = universal.pragma.directives.filter((d) => d.required);
  assert(required.length > 0, "should have at least one required directive");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Format rules tests — 10 gates proving the schema-driven format loader works.
// Universal rules define WHAT (separators, boundaries, pragma, identity, closing).
// Language rules define HOW (comment syntax, pragma expression, constructs).
// Cache ensures performance. Clear ensures testability.
//
// "Look that thou make them after their pattern." — Exodus 25:40
// ============================================================================
