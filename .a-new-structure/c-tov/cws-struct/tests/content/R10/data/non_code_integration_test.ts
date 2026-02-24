// R-level: R10 (data layer — non-code schema pattern integration)
// Migrated from: tests/data/non_code_integration_test.ts
// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R10/data/non_code_integration_test.ts
// key:     B-tov-cws-struct-tests-data-non-code-integration
// title:   CWS Struct — Non-Code Schema Pattern Integration Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Prove that non-code schema patterns match REAL code. Loads schemas
//          via the schema pipeline, compiles regex patterns from the schema,
//          and tests them against representative lines from actual Go/Rust
//          fixture files. Tests classify lines using the schema patterns
//          to verify the schemas accurately describe what the chunker sees.
//
//          Gate | Verifies
//          -----|-----------------------------------------------------
//          P1   | Pragma patterns match real OmniCode pragma lines
//          P2   | Separator patterns match both ASCII and Unicode
//          P3   | Go build_tag patterns match real build constraints
//          P4   | Go generate_directive matches //go:generate lines
//          P5   | Go compiler_directive matches //go:noinline etc.
//          P6   | Rust inner_doc patterns match //! lines
//          P7   | Rust doc_comment patterns match /// lines
//          P8   | Rust attribute patterns match #[...] lines
//          P9   | Rust inner_attribute patterns match #![...] lines
//          P10  | Comment patterns EXCLUDE pragmas, separators, docs
//          P11  | Blank patterns match empty/whitespace lines
//          P12  | Classification priority resolves ambiguous cases
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";

import {
  loadNonCode,
  loadLanguageNonCode,
  clearNonCodeCache,
} from "@lib/data/non-code.ts";

import type {
  ChunkCategory,
  ComposedNonCode,
} from "@lib/data/non-code.ts";

// ---------------------------------------------------------------------------
// Test helpers
// ---------------------------------------------------------------------------

/**
 * Compile a ChunkCategory into RegExp matchers.
 * Handles: pattern (single), patterns (array), start_pattern.
 * Returns array of compiled patterns.
 */
function compilePatterns(cat: ChunkCategory): RegExp[] {
  const regexes: RegExp[] = [];
  if (cat.pattern && cat.pattern !== "<language_specific>") {
    regexes.push(new RegExp(cat.pattern));
  }
  if (cat.start_pattern) {
    regexes.push(new RegExp(cat.start_pattern));
  }
  if (cat.patterns) {
    for (const p of cat.patterns) {
      regexes.push(new RegExp(p));
    }
  }
  return regexes;
}

/** Test that at least one pattern matches the line. */
function matchesAny(patterns: RegExp[], line: string): boolean {
  return patterns.some((p) => p.test(line));
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// P1: Pragma patterns match real OmniCode pragma lines
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-077] non-code-int/P1: Go pragma start_pattern matches #!omni line", async () => {
  clearNonCodeCache();
  const composed = await loadNonCode("go");
  const pragma = composed.allCategories["pragma"];
  assert(pragma !== undefined);
  const patterns = compilePatterns(pragma!);

  // Real pragma lines from Go fixtures
  assert(matchesAny(patterns, "// #!omni code --go -library"), "Should match pragma line");
  assert(matchesAny(patterns, "// #!omni template --go"), "Should match template pragma");
});

Deno.test("non-code-int/P1: Go pragma continuation matches //omni: directives", async () => {
  const composed = await loadNonCode("go");
  const pragma = composed.allCategories["pragma"];
  assert(pragma !== undefined);
  assert(pragma!.continuation !== undefined);
  const continuation = new RegExp(pragma!.continuation!);

  assert(continuation.test("//omni:key B-test-fixture"), "Should match //omni:key");
  assert(continuation.test("//omni:version a-01.00"), "Should match //omni:version");
  assert(continuation.test("//omni:code --go -library"), "Should match //omni:code");
  assert(!continuation.test("// regular comment"), "Should NOT match regular comment");
});

Deno.test("non-code-int/P1: Rust pragma patterns match same format", async () => {
  clearNonCodeCache();
  const composed = await loadNonCode("rust");
  const pragma = composed.allCategories["pragma"];
  assert(pragma !== undefined);
  const patterns = compilePatterns(pragma!);

  assert(matchesAny(patterns, "// #!omni code --rust -library"), "Should match Rust pragma");
});

// ---------------------------------------------------------------------------
// P2: Separator patterns match both ASCII and Unicode
// ---------------------------------------------------------------------------

Deno.test("non-code-int/P2: Go separator matches ASCII = banners", async () => {
  const composed = await loadNonCode("go");
  const sep = composed.allCategories["separator"];
  const patterns = compilePatterns(sep!);

  assert(matchesAny(patterns, "// ============================================================================"), "ASCII = banner");
});

Deno.test("non-code-int/P2: Go separator matches ASCII - banners", async () => {
  const composed = await loadNonCode("go");
  const sep = composed.allCategories["separator"];
  const patterns = compilePatterns(sep!);

  assert(matchesAny(patterns, "// ---------------------------------------------------------------------------"), "ASCII - banner");
});

Deno.test("non-code-int/P2: Go separator matches Unicode ═ banners", async () => {
  const composed = await loadNonCode("go");
  const sep = composed.allCategories["separator"];
  const patterns = compilePatterns(sep!);

  assert(matchesAny(patterns, "// ════════════════════════════════════════════════════════════════════════"), "Unicode ═ banner");
});

Deno.test("non-code-int/P2: Go separator matches Unicode ─ banners", async () => {
  const composed = await loadNonCode("go");
  const sep = composed.allCategories["separator"];
  const patterns = compilePatterns(sep!);

  assert(matchesAny(patterns, "// ──────────────────────────────────────────────────────────────────────────"), "Unicode ─ banner");
});

Deno.test("non-code-int/P2: separator does NOT match short lines", async () => {
  const composed = await loadNonCode("go");
  const sep = composed.allCategories["separator"];
  const patterns = compilePatterns(sep!);

  assert(!matchesAny(patterns, "// ---"), "Short --- is not a separator (< 10 chars)");
  assert(!matchesAny(patterns, "// ===="), "Short === is not a separator (< 10 chars)");
});

// ---------------------------------------------------------------------------
// P3: Go build_tag patterns match real build constraints
// ---------------------------------------------------------------------------

Deno.test("non-code-int/P3: Go build_tag matches //go:build", async () => {
  const lang = await loadLanguageNonCode("go");
  const bt = lang.chunk_categories["build_tag"];
  assert(bt !== undefined);
  const patterns = compilePatterns(bt!);

  assert(matchesAny(patterns, "//go:build linux"), "Should match //go:build linux");
  assert(matchesAny(patterns, "//go:build !windows && amd64"), "Should match complex build tag");
  assert(matchesAny(patterns, "//go:build ignore"), "Should match //go:build ignore");
});

Deno.test("non-code-int/P3: Go build_tag matches legacy +build", async () => {
  const lang = await loadLanguageNonCode("go");
  const bt = lang.chunk_categories["build_tag"];
  const patterns = compilePatterns(bt!);

  assert(matchesAny(patterns, "// +build linux"), "Should match legacy // +build linux");
});

// ---------------------------------------------------------------------------
// P4: Go generate_directive matches //go:generate lines
// ---------------------------------------------------------------------------

Deno.test("non-code-int/P4: Go generate_directive matches real directives", async () => {
  const lang = await loadLanguageNonCode("go");
  const gd = lang.chunk_categories["generate_directive"];
  assert(gd !== undefined);
  const patterns = compilePatterns(gd!);

  assert(matchesAny(patterns, "//go:generate stringer -type=Pill"), "Should match go:generate");
  assert(matchesAny(patterns, "//go:generate mockgen -source=service.go"), "Should match mockgen");
  assert(!matchesAny(patterns, "//go:build linux"), "Should NOT match go:build");
});

// ---------------------------------------------------------------------------
// P5: Go compiler_directive matches //go:noinline etc.
// ---------------------------------------------------------------------------

Deno.test("non-code-int/P5: Go compiler_directive matches //go: pragmas", async () => {
  const lang = await loadLanguageNonCode("go");
  const cd = lang.chunk_categories["compiler_directive"];
  assert(cd !== undefined);
  const patterns = compilePatterns(cd!);

  assert(matchesAny(patterns, "//go:noinline"), "Should match noinline");
  assert(matchesAny(patterns, "//go:nosplit"), "Should match nosplit");
  assert(matchesAny(patterns, "//go:linkname localFunc pkg.RemoteFunc"), "Should match linkname");
  assert(matchesAny(patterns, "//go:noescape"), "Should match noescape");
  assert(!matchesAny(patterns, "//go:generate stringer"), "Should NOT match generate");
  assert(!matchesAny(patterns, "//go:build linux"), "Should NOT match build");
});

// ---------------------------------------------------------------------------
// P6: Rust inner_doc patterns match //! lines
// ---------------------------------------------------------------------------

Deno.test("non-code-int/P6: Rust inner_doc matches //! crate docs", async () => {
  const lang = await loadLanguageNonCode("rust");
  const innerDoc = lang.chunk_categories["inner_doc"];
  assert(innerDoc !== undefined);
  const patterns = compilePatterns(innerDoc!);

  assert(matchesAny(patterns, "//! This crate provides utilities"), "Should match //! doc");
  assert(matchesAny(patterns, "//!"), "Should match bare //!");
  assert(!matchesAny(patterns, "/// Outer doc"), "Should NOT match /// doc");
  assert(!matchesAny(patterns, "// Regular comment"), "Should NOT match // comment");
});

// ---------------------------------------------------------------------------
// P7: Rust doc_comment patterns match /// lines
// ---------------------------------------------------------------------------

Deno.test("non-code-int/P7: Rust doc_comment matches /// lines", async () => {
  const lang = await loadLanguageNonCode("rust");
  const docComment = lang.chunk_categories["doc_comment"];
  assert(docComment !== undefined);
  const patterns = compilePatterns(docComment!);

  assert(matchesAny(patterns, "/// Documentation for the function"), "Should match /// doc");
  assert(matchesAny(patterns, "///"), "Should match bare ///");
  assert(!matchesAny(patterns, "//! Inner doc"), "Should NOT match //! doc");
});

// ---------------------------------------------------------------------------
// P8: Rust attribute patterns match #[...] lines
// ---------------------------------------------------------------------------

Deno.test("non-code-int/P8: Rust attribute matches #[...] lines", async () => {
  const lang = await loadLanguageNonCode("rust");
  const attr = lang.chunk_categories["attribute"];
  assert(attr !== undefined);
  const patterns = compilePatterns(attr!);

  assert(matchesAny(patterns, "#[derive(Debug, Clone)]"), "Should match #[derive]");
  assert(matchesAny(patterns, "#[cfg(test)]"), "Should match #[cfg(test)]");
  assert(matchesAny(patterns, "#[allow(dead_code)]"), "Should match #[allow]");
  assert(!matchesAny(patterns, "#![feature(async_fn)]"), "Should NOT match inner attribute");
});

// ---------------------------------------------------------------------------
// P9: Rust inner_attribute patterns match #![...] lines
// ---------------------------------------------------------------------------

Deno.test("non-code-int/P9: Rust inner_attribute matches #![...] lines", async () => {
  const lang = await loadLanguageNonCode("rust");
  const innerAttr = lang.chunk_categories["inner_attribute"];
  assert(innerAttr !== undefined);
  const patterns = compilePatterns(innerAttr!);

  assert(matchesAny(patterns, "#![allow(dead_code)]"), "Should match #![allow]");
  assert(matchesAny(patterns, "#![feature(async_fn)]"), "Should match #![feature]");
  assert(!matchesAny(patterns, "#[derive(Debug)]"), "Should NOT match outer attribute");
});

// ---------------------------------------------------------------------------
// P10: Comment patterns EXCLUDE pragmas, separators, docs
// ---------------------------------------------------------------------------

Deno.test("non-code-int/P10: Rust comment excludes doc patterns", async () => {
  const lang = await loadLanguageNonCode("rust");
  const comment = lang.chunk_categories["comment"];
  assert(comment !== undefined);
  const patterns = compilePatterns(comment!);

  // Rust comment pattern: ^//[^/!]|^//$
  assert(matchesAny(patterns, "// Regular comment"), "Should match regular comment");
  assert(matchesAny(patterns, "//"), "Should match bare //");
  assert(!matchesAny(patterns, "/// Doc comment"), "Should NOT match /// (doc)");
  assert(!matchesAny(patterns, "//! Inner doc"), "Should NOT match //! (inner doc)");
});

// ---------------------------------------------------------------------------
// P11: Blank patterns match empty/whitespace lines
// ---------------------------------------------------------------------------

Deno.test("non-code-int/P11: blank pattern matches empty lines", async () => {
  const composed = await loadNonCode("go");
  const blank = composed.allCategories["blank"];
  assert(blank !== undefined);
  const patterns = compilePatterns(blank!);

  assert(matchesAny(patterns, ""), "Should match empty string");
  assert(matchesAny(patterns, "   "), "Should match whitespace-only");
  assert(matchesAny(patterns, "\t"), "Should match tab-only");
  assert(!matchesAny(patterns, "// comment"), "Should NOT match comment");
  assert(!matchesAny(patterns, "func main() {"), "Should NOT match code");
});

// ---------------------------------------------------------------------------
// P12: Classification priority resolves ambiguous cases
// ---------------------------------------------------------------------------

Deno.test("non-code-int/P12: classification priority exists and has correct order", async () => {
  const composed = await loadNonCode("go");
  const priority = composed.allRules["classification_priority"];
  assert(priority !== undefined, "classification_priority rule should exist");
  assert(priority!.order !== undefined, "should have order array");

  const order = priority!.order!;
  // Pragma should be highest priority
  assertEquals(order[0], "pragma");
  // Separator before comment
  const sepIdx = order.indexOf("separator");
  const commentIdx = order.indexOf("comment");
  assert(sepIdx < commentIdx, "separator should be higher priority than comment");
});

Deno.test("non-code-int/P12: pragma takes priority over separator (same line)", async () => {
  // A line starting with // #!omni matches both pragma.start_pattern
  // and could theoretically match separator patterns. Pragma wins.
  const composed = await loadNonCode("go");
  const pragma = composed.allCategories["pragma"];
  const sep = composed.allCategories["separator"];
  const pragmaPatterns = compilePatterns(pragma!);
  const sepPatterns = compilePatterns(sep!);

  const line = "// #!omni code --go -library";
  assert(matchesAny(pragmaPatterns, line), "Pragma should match");
  assert(!matchesAny(sepPatterns, line), "Separator should NOT match (no repeated =/-)");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Non-code integration tests — 12 pattern gates proving schemas accurately
// describe what the chunker sees in real code. Each test loads schema patterns,
// compiles them to regex, and tests against representative lines from actual
// Go and Rust files.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
