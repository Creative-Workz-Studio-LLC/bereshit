// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/dotfiles/lint_table_test.ts
// key:     B-tov-cws-struct-tests-integration-dotfiles-lint-table
// title:   Dotfiles Integration Tests — Cross-Concern Lint
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Integration-level Dotfiles lint tests. Verifies the Dotfiles handler
//          detects structural issues across editorconfig fixtures.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assert } from "jsr:@std/assert";
import { lazyFormat, errors, warnings } from "@test/helpers.ts";

// ============================================================================
// BODY
// ============================================================================

const dotfiles = lazyFormat("dotfiles");

function fixture(name: string): string {
  return new URL(`../../fixtures/${name}`, import.meta.url).pathname;
}

// ---------------------------------------------------------------------------
// 1. Structure: various broken patterns
// ---------------------------------------------------------------------------

Deno.test("integration/dotfiles: no-blocks produces results", async () => {
  const path = fixture("dotfiles/structure/no-blocks.editorconfig");
  const results = await dotfiles.lint(path);
  // Handler reports info for files without blocks, not error
  assert(results.length > 0, "No blocks should produce results");
});

Deno.test("integration/dotfiles: missing-blocks has errors", async () => {
  const path = fixture("dotfiles/structure/missing-blocks.editorconfig");
  const results = await dotfiles.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Missing blocks should have errors");
});

Deno.test("integration/dotfiles: wrong-order has issues", async () => {
  const path = fixture("dotfiles/structure/wrong-order.editorconfig");
  const results = await dotfiles.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Wrong order should have issues");
});

Deno.test("integration/dotfiles: missing-end-metadata has errors", async () => {
  const path = fixture("dotfiles/structure/missing-end-metadata.editorconfig");
  const results = await dotfiles.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Missing end metadata should have errors");
});

Deno.test("integration/dotfiles: missing-pragma-line has errors", async () => {
  const path = fixture("dotfiles/structure/missing-pragma-line.editorconfig");
  const results = await dotfiles.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Missing pragma line should have errors");
});

Deno.test("integration/dotfiles: missing-end-pragma has errors", async () => {
  const path = fixture("dotfiles/structure/missing-end-pragma.editorconfig");
  const results = await dotfiles.lint(path);
  const errs = errors(results);
  assert(errs.length > 0, "Missing end pragma should have errors");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Dotfiles integration tests — 6 structural fixtures exercising the full
// Dotfiles lint pipeline. Each broken pattern verified.
//
// "Order my steps in thy word." — Psalm 119:133
// ============================================================================
