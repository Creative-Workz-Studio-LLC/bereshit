// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/integration/makefile/lint_table_test.ts
// key:     B-tov-cws-struct-tests-integration-makefile-lint-table
// title:   Makefile Integration Tests — Cross-Concern Lint
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Integration-level Makefile lint tests. Verifies the Makefile handler
//          detects structural, metadata, ordering, and closing issues.
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

const makefile = lazyFormat("makefile");

function fixture(name: string): string {
  return new URL(`../../fixtures/${name}`, import.meta.url).pathname;
}

// ---------------------------------------------------------------------------
// 1. Structure: non-OmniCode Makefile
// ---------------------------------------------------------------------------

Deno.test("integration/makefile: not-omni detected", async () => {
  const path = fixture("makefile/structure/not-omni.mk");
  const results = await makefile.lint(path);
  // Handler reports info, not error — non-OmniCode files are noted, not rejected
  assert(results.length > 0, "Non-OmniCode Makefile should produce results");
});

// ---------------------------------------------------------------------------
// 2. Metadata: missing required fields
// ---------------------------------------------------------------------------

Deno.test("integration/makefile: missing-required has errors", async () => {
  const path = fixture("makefile/metadata/missing-required.mk");
  const results = await makefile.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Missing required metadata should have issues");
});

Deno.test("integration/makefile: missing-extension produces results", async () => {
  const path = fixture("makefile/metadata/missing-extension.mk");
  const results = await makefile.lint(path);
  // May produce info/warn/error depending on handler policy
  assert(results.length > 0, "Missing extension should produce results");
});

// ---------------------------------------------------------------------------
// 3. Ordering: cross-reference mismatch
// ---------------------------------------------------------------------------

Deno.test("integration/makefile: xref-mismatch has errors", async () => {
  const path = fixture("makefile/ordering/xref-mismatch.mk");
  const results = await makefile.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Cross-reference mismatch should have issues");
});

// ---------------------------------------------------------------------------
// 4. Closing: missing required tags
// ---------------------------------------------------------------------------

Deno.test("integration/makefile: missing-required-tags has errors", async () => {
  const path = fixture("makefile/closing/missing-required-tags.mk");
  const results = await makefile.lint(path);
  const issues = [...errors(results), ...warnings(results)];
  assert(issues.length > 0, "Missing required closing tags should have issues");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Makefile integration tests — structure, metadata, ordering, and closing
// validation. Each fixture exercises the full Makefile lint pipeline.
//
// "Except the LORD build the house, they labour in vain." — Psalm 127:1
// ============================================================================
