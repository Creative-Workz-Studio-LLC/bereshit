// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R50/data/non_code_test.ts
// key:     B-tov-cws-struct-tests-data-non-code
// title:   CWS Struct — Non-Code Categories Loader Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Verify the schema-driven non-code category loader works correctly.
//          Tests prove that universal and per-language _non-code.jsonc schemas
//          load, parse, cache, and compose into usable non-code definitions.
//
//          Gate | Verifies
//          -----|----------------------------------------
//          G1   | Universal non-code loads from schema
//          G2   | Universal has 5 base categories (pragma, separator, doc, comment, blank)
//          G3   | Universal has chunking rules
//          G4   | Language non-code loads per-language
//          G5   | Composed non-code merges universal + language
//          G6   | Language categories overlay universal (not replace)
//          G7   | Cache returns same object on second load
//          G8   | clearNonCodeCache forces fresh load
//          G9   | Rust adds language-specific categories (inner_doc, attribute)
//          G10  | Go adds language-specific categories (build_tag, generate_directive)
//          G11  | Category descriptions are non-empty
//          G12  | Language extension contract exists in universal
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";

import {
  loadUniversalNonCode,
  loadLanguageNonCode,
  loadNonCode,
  clearNonCodeCache,
} from "@lib/data/non-code.ts";

import type {
  UniversalNonCode,
  LanguageNonCode,
  ComposedNonCode,
} from "@lib/data/non-code.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// G1: Universal non-code loads
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-072] non-code/G1: universal non-code loads successfully", async () => {
  clearNonCodeCache();
  const universal = await loadUniversalNonCode();
  assert(universal !== undefined, "universal should load");
  assert(universal !== null, "universal should not be null");
});

Deno.test("non-code/G1: universal has scope", async () => {
  const universal = await loadUniversalNonCode();
  assert(universal.scope.length > 0, "scope should be non-empty");
});

// ---------------------------------------------------------------------------
// G2: Universal has 5 base categories
// ---------------------------------------------------------------------------

Deno.test("non-code/G2: universal has pragma category", async () => {
  const universal = await loadUniversalNonCode();
  assert("pragma" in universal.chunk_categories, "should have pragma category");
});

Deno.test("non-code/G2: universal has separator category", async () => {
  const universal = await loadUniversalNonCode();
  assert("separator" in universal.chunk_categories, "should have separator category");
});

Deno.test("non-code/G2: universal has doc_comment category", async () => {
  const universal = await loadUniversalNonCode();
  assert("doc_comment" in universal.chunk_categories, "should have doc_comment category");
});

Deno.test("non-code/G2: universal has comment category", async () => {
  const universal = await loadUniversalNonCode();
  assert("comment" in universal.chunk_categories, "should have comment category");
});

Deno.test("non-code/G2: universal has blank category", async () => {
  const universal = await loadUniversalNonCode();
  assert("blank" in universal.chunk_categories, "should have blank category");
});

Deno.test("non-code/G2: universal has at least 5 categories", async () => {
  const universal = await loadUniversalNonCode();
  const count = Object.keys(universal.chunk_categories).length;
  assert(count >= 5, `expected >= 5 categories, got ${count}`);
});

// ---------------------------------------------------------------------------
// G3: Universal has chunking rules
// ---------------------------------------------------------------------------

Deno.test("non-code/G3: universal has chunking_rules", async () => {
  const universal = await loadUniversalNonCode();
  assert(universal.chunking_rules !== undefined, "should have chunking_rules");
  const count = Object.keys(universal.chunking_rules).length;
  assert(count > 0, "should have at least one chunking rule");
});

Deno.test("non-code/G3: classification_priority rule exists", async () => {
  const universal = await loadUniversalNonCode();
  assert(
    "classification_priority" in universal.chunking_rules,
    "should have classification_priority rule",
  );
});

// ---------------------------------------------------------------------------
// G4: Language non-code loads per-language
// ---------------------------------------------------------------------------

Deno.test("non-code/G4: Go non-code loads", async () => {
  const lang = await loadLanguageNonCode("go");
  assertEquals(lang.language, "go");
  assert(lang.chunk_categories !== undefined, "should have chunk_categories");
});

Deno.test("non-code/G4: Rust non-code loads", async () => {
  const lang = await loadLanguageNonCode("rust");
  assertEquals(lang.language, "rust");
  assert(lang.chunk_categories !== undefined, "should have chunk_categories");
});

// ---------------------------------------------------------------------------
// G5: Composed non-code merges universal + language
// ---------------------------------------------------------------------------

Deno.test("non-code/G5: loadNonCode returns composed Go definitions", async () => {
  clearNonCodeCache();
  const composed = await loadNonCode("go");

  assert(composed.universal !== undefined, "should have universal side");
  assert(composed.language !== undefined, "should have language side");
  assertEquals(composed.language.language, "go");
  assert(composed.allCategories !== undefined, "should have allCategories");
  assert(composed.allRules !== undefined, "should have allRules");
});

Deno.test("non-code/G5: loadNonCode returns composed Rust definitions", async () => {
  clearNonCodeCache();
  const composed = await loadNonCode("rust");

  assert(composed.universal !== undefined, "should have universal side");
  assert(composed.language !== undefined, "should have language side");
  assertEquals(composed.language.language, "rust");
});

// ---------------------------------------------------------------------------
// G6: Language categories overlay (merge, not replace)
// ---------------------------------------------------------------------------

Deno.test("non-code/G6: composed allCategories includes universal categories", async () => {
  clearNonCodeCache();
  const composed = await loadNonCode("go");

  // Universal categories should be present in allCategories
  assert("pragma" in composed.allCategories, "allCategories should include pragma");
  assert("separator" in composed.allCategories, "allCategories should include separator");
  assert("comment" in composed.allCategories, "allCategories should include comment");
});

Deno.test("non-code/G6: allCategories has more entries than universal alone", async () => {
  clearNonCodeCache();
  const composed = await loadNonCode("go");

  const universalCount = Object.keys(composed.universal.chunk_categories).length;
  const allCount = Object.keys(composed.allCategories).length;

  // Language adds categories, so all >= universal
  assert(
    allCount >= universalCount,
    `allCategories (${allCount}) should be >= universal (${universalCount})`,
  );
});

// ---------------------------------------------------------------------------
// G7: Cache returns same object
// ---------------------------------------------------------------------------

Deno.test("non-code/G7: second load returns cached result", async () => {
  clearNonCodeCache();
  const first = await loadNonCode("go");
  const second = await loadNonCode("go");
  assert(first === second, "cached load should return same reference");
});

// ---------------------------------------------------------------------------
// G8: clearNonCodeCache forces fresh load
// ---------------------------------------------------------------------------

Deno.test("non-code/G8: clearNonCodeCache forces new object", async () => {
  clearNonCodeCache();
  const first = await loadNonCode("rust");
  clearNonCodeCache();
  const second = await loadNonCode("rust");
  assert(first !== second, "after clear, should get new object");
  assertEquals(first.language.language, second.language.language);
});

// ---------------------------------------------------------------------------
// G9: Rust adds language-specific categories
// ---------------------------------------------------------------------------

Deno.test("non-code/G9: Rust has inner_doc category", async () => {
  const lang = await loadLanguageNonCode("rust");
  assert(
    "inner_doc" in lang.chunk_categories,
    "Rust should have inner_doc category",
  );
});

Deno.test("non-code/G9: Rust has attribute category", async () => {
  const lang = await loadLanguageNonCode("rust");
  assert(
    "attribute" in lang.chunk_categories,
    "Rust should have attribute category",
  );
});

// ---------------------------------------------------------------------------
// G10: Go adds language-specific categories
// ---------------------------------------------------------------------------

Deno.test("non-code/G10: Go has build_tag category", async () => {
  const lang = await loadLanguageNonCode("go");
  assert(
    "build_tag" in lang.chunk_categories,
    "Go should have build_tag category",
  );
});

Deno.test("non-code/G10: Go has generate_directive category", async () => {
  const lang = await loadLanguageNonCode("go");
  assert(
    "generate_directive" in lang.chunk_categories,
    "Go should have generate_directive category",
  );
});

// ---------------------------------------------------------------------------
// G10b: Go adds compiler_directive category (new — enrichment)
// ---------------------------------------------------------------------------

Deno.test("non-code/G10b: Go has compiler_directive category", async () => {
  const lang = await loadLanguageNonCode("go");
  assert(
    "compiler_directive" in lang.chunk_categories,
    "Go should have compiler_directive category (//go:noinline, //go:nosplit, etc.)",
  );
});

Deno.test("non-code/G10b: Go compiler_directive attaches to next declaration", async () => {
  const lang = await loadLanguageNonCode("go");
  const cd = lang.chunk_categories["compiler_directive"];
  assert(cd !== undefined, "compiler_directive should exist");
  assertEquals(cd!.attaches_to, "next_declaration");
});

// ---------------------------------------------------------------------------
// G10c: Separator patterns include Unicode box-drawing characters
// ---------------------------------------------------------------------------

Deno.test("non-code/G10c: Go separator patterns include Unicode chars", async () => {
  const lang = await loadLanguageNonCode("go");
  const sep = lang.chunk_categories["separator"];
  assert(sep !== undefined, "separator should exist");
  assert(sep!.patterns !== undefined, "separator should have patterns");
  const patternsStr = JSON.stringify(sep!.patterns);
  assert(patternsStr.includes("\u2550") || patternsStr.includes("\\u2550"), "Should include \u2550 (U+2550)");
  assert(patternsStr.includes("\u2500") || patternsStr.includes("\\u2500"), "Should include \u2500 (U+2500)");
});

Deno.test("non-code/G10c: Rust separator patterns include Unicode chars", async () => {
  const lang = await loadLanguageNonCode("rust");
  const sep = lang.chunk_categories["separator"];
  assert(sep !== undefined, "separator should exist");
  assert(sep!.patterns !== undefined, "separator should have patterns");
  const patternsStr = JSON.stringify(sep!.patterns);
  assert(patternsStr.includes("\u2550") || patternsStr.includes("\\u2550"), "Should include \u2550 (U+2550)");
  assert(patternsStr.includes("\u2500") || patternsStr.includes("\\u2500"), "Should include \u2500 (U+2500)");
});

// ---------------------------------------------------------------------------
// G11: Category descriptions are non-empty
// ---------------------------------------------------------------------------

Deno.test("non-code/G11: all universal categories have descriptions", async () => {
  const universal = await loadUniversalNonCode();
  for (const [name, cat] of Object.entries(universal.chunk_categories)) {
    assert(
      cat.description.length > 0,
      `category "${name}" should have non-empty description`,
    );
  }
});

Deno.test("non-code/G11: all Go categories have descriptions", async () => {
  const lang = await loadLanguageNonCode("go");
  for (const [name, cat] of Object.entries(lang.chunk_categories)) {
    assert(
      cat.description.length > 0,
      `Go category "${name}" should have non-empty description`,
    );
  }
});

// ---------------------------------------------------------------------------
// G12: Language extension contract exists
// ---------------------------------------------------------------------------

Deno.test("non-code/G12: universal has language_extension_contract", async () => {
  const universal = await loadUniversalNonCode();
  assert(
    universal.language_extension_contract !== undefined,
    "should have language_extension_contract",
  );
  assert(
    universal.language_extension_contract!.rules.length > 0,
    "contract should have rules",
  );
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Non-code category tests — 12 gates proving the schema-driven loader works.
// Universal categories (pragma, separator, doc_comment, comment, blank) are
// the foundation. Language schemas ADD categories (Rust: inner_doc, attribute;
// Go: build_tag, generate_directive) without replacing universal ones.
//
// The composed result gives consumers everything: all categories merged,
// all rules merged, ready for the chunking engine.
//
// "The eye cannot say unto the hand, I have no need of thee."
// — 1 Corinthians 12:21
// ============================================================================
