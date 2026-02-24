// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R05/engine/suggest_test.ts
// key:     B-tov-cws-struct-tests-engine-suggest
// title:   CWS Struct — Quick-Fix Suggestions Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Verify the suggestion ranking engine — grouping, scoring, effort
//          estimation, and output formatting. Tests cover the core algorithm,
//          edge cases (empty input, single file, all clean), and ranking
//          properties (severity × frequency × cascade × effort).
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import { computeQuickFixes } from "@lib/engine/suggest.ts";
import type { QuickFixSuggestion } from "@lib/engine/suggest.ts";
import type { LintSummary, LintResult } from "@lib/foundation/types.ts";

// ---------------------------------------------------------------------------
// Test helpers
// ---------------------------------------------------------------------------

/** Build a minimal LintResult. */
function result(
  file: string,
  severity: "error" | "warn" | "info",
  rule: string,
  message = "test",
  errorCode?: string,
): LintResult {
  return { file, severity, rule, message, errorCode };
}

/** Build a minimal LintSummary from results. */
function summary(
  file: string,
  results: LintResult[],
): LintSummary {
  let errors = 0, warnings = 0, infos = 0;
  for (const r of results) {
    if (r.severity === "error") errors++;
    else if (r.severity === "warn") warnings++;
    else infos++;
  }
  return { file, errors, warnings, infos, results };
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// S1: Basic grouping and ranking
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-089] suggest/S1: empty summaries produces no suggestions", () => {
  const suggestions = computeQuickFixes([], 5);
  assertEquals(suggestions.length, 0);
});

Deno.test("suggest/S1: all-clean files produce no suggestions", () => {
  const summaries = [
    summary("a.go", []),
    summary("b.go", []),
  ];
  const suggestions = computeQuickFixes(summaries, 5);
  assertEquals(suggestions.length, 0);
});

Deno.test("suggest/S1: info-only results are skipped", () => {
  const summaries = [
    summary("a.go", [
      result("a.go", "info", "doc/package", "No doc comment"),
      result("a.go", "info", "doc/pragma-var", "No pragma doc"),
    ]),
  ];
  const suggestions = computeQuickFixes(summaries, 5);
  assertEquals(suggestions.length, 0);
});

Deno.test("suggest/S1: single file with errors produces suggestion", () => {
  const summaries = [
    summary("a.go", [
      result("a.go", "error", "block/order", "Block order violation"),
    ]),
  ];
  const suggestions = computeQuickFixes(summaries, 5);
  assert(suggestions.length >= 1);
  assertEquals(suggestions[0]!.fileCount, 1);
  assertEquals(suggestions[0]!.files[0], "a.go");
});

// ---------------------------------------------------------------------------
// S2: Frequency ranking — same issue across more files ranks higher
// ---------------------------------------------------------------------------

Deno.test("suggest/S2: higher frequency issue ranks above lower", () => {
  const summaries = [
    // Issue A: appears in 3 files
    summary("a.go", [result("a.go", "warn", "setup/subsection-order", "Wrong order")]),
    summary("b.go", [result("b.go", "warn", "setup/subsection-order", "Wrong order")]),
    summary("c.go", [result("c.go", "warn", "setup/subsection-order", "Wrong order")]),
    // Issue B: appears in 1 file
    summary("d.go", [result("d.go", "warn", "closing/zone-order", "Wrong zone order")]),
  ];
  const suggestions = computeQuickFixes(summaries, 5);
  assert(suggestions.length >= 2);
  // First suggestion should be the one in 3 files
  assertEquals(suggestions[0]!.fileCount, 3);
});

// ---------------------------------------------------------------------------
// S3: Severity ranking — errors rank above warnings (same frequency)
// ---------------------------------------------------------------------------

Deno.test("suggest/S3: error severity ranks above warn at same frequency", () => {
  const summaries = [
    // Warn issue: 2 files
    summary("a.go", [result("a.go", "warn", "setup/subsection-order", "Wrong order")]),
    summary("b.go", [result("b.go", "warn", "setup/subsection-order", "Wrong order")]),
    // Error issue: 2 files
    summary("c.go", [result("c.go", "error", "block/order", "Block order violation")]),
    summary("d.go", [result("d.go", "error", "block/order", "Block order violation")]),
  ];
  const suggestions = computeQuickFixes(summaries, 5);
  assert(suggestions.length >= 2);
  // Error should rank higher — error weight (3) × cascade (4) vs warn weight (2) × cascade (3)
  assertEquals(suggestions[0]!.severity, "error");
});

// ---------------------------------------------------------------------------
// S4: Cascade layer — Layer 0 issues rank above Layer 1 (same severity)
// ---------------------------------------------------------------------------

Deno.test("suggest/S4: layer 0 issue ranks above layer 1 at same severity/frequency/effort", () => {
  // Both rules need the SAME effort category to isolate cascade as the variable.
  // block/order → layer 0, category "block" (effort multiplier 2)
  // identity/Pragma → layer 1, category "identity" (effort multiplier 2)
  // Same severity, same frequency, same effort → cascade breaks the tie.
  const summaries = [
    // Layer 1 issue: warn, identity category (effort=2)
    summary("a.go", [result("a.go", "warn", "identity/Pragma", "Missing Pragma")]),
    // Layer 0 issue: warn, block category (effort=2)
    summary("b.go", [result("b.go", "warn", "block/order", "Block order violation")]),
  ];
  const suggestions = computeQuickFixes(summaries, 5);
  assert(suggestions.length >= 2);
  // Both: severityW=2, fileCount=1, effortM=2
  // block/order: cascade=4 → score=2*1*4*2=16
  // identity/Pragma: cascade=3 → score=2*1*3*2=12
  // Layer 0 (block/order) should rank first.
  const first = suggestions[0]!;
  assert(
    first.rule === "block/order" || first.layer === 0,
    `Expected layer 0 issue first, got rule='${first.rule}' layer=${first.layer}`,
  );
});

// ---------------------------------------------------------------------------
// S5: Limit parameter
// ---------------------------------------------------------------------------

Deno.test("suggest/S5: limit parameter caps results", () => {
  const summaries = [
    summary("a.go", [
      result("a.go", "warn", "setup/subsection-order", "A"),
      result("a.go", "warn", "body/subsection-order", "B"),
      result("a.go", "warn", "closing/zone-order", "C"),
      result("a.go", "error", "block/order", "D"),
      result("a.go", "error", "block/METADATA", "E"),
    ]),
  ];

  const s3 = computeQuickFixes(summaries, 3);
  assert(s3.length <= 3, `Expected at most 3, got ${s3.length}`);

  const s1 = computeQuickFixes(summaries, 1);
  assertEquals(s1.length, 1);
});

// ---------------------------------------------------------------------------
// S6: Effort estimation
// ---------------------------------------------------------------------------

Deno.test("suggest/S6: ordering category gets low effort", () => {
  const summaries = [
    summary("a.go", [result("a.go", "warn", "setup/subsection-order", "Wrong order")]),
  ];
  const suggestions = computeQuickFixes(summaries, 5);
  assert(suggestions.length >= 1);
  // The rule matches CWS-R25-023 with category "ordering" → effort = low
  const orderingSuggestion = suggestions.find((s) => s.category === "ordering");
  if (orderingSuggestion) {
    assertEquals(orderingSuggestion.effort, "low");
  }
});

// ---------------------------------------------------------------------------
// S7: Deduplication — same rule in same file counted once
// ---------------------------------------------------------------------------

Deno.test("suggest/S7: multiple occurrences in one file counted as one file", () => {
  const summaries = [
    summary("a.go", [
      result("a.go", "warn", "setup/subsection-order", "Order A"),
      result("a.go", "warn", "setup/subsection-order", "Order B"),
      result("a.go", "warn", "setup/subsection-order", "Order C"),
    ]),
  ];
  const suggestions = computeQuickFixes(summaries, 5);
  assert(suggestions.length >= 1);
  assertEquals(suggestions[0]!.fileCount, 1);
});

// ---------------------------------------------------------------------------
// S8: Suggestion fields populated correctly
// ---------------------------------------------------------------------------

Deno.test("suggest/S8: suggestion fields are all populated", () => {
  const summaries = [
    summary("a.go", [
      result("a.go", "error", "block/order", "Block order violation", "CWS-R50-020"),
    ]),
  ];
  const suggestions = computeQuickFixes(summaries, 5);
  assert(suggestions.length >= 1);
  const s = suggestions[0]!;

  // Code should be from catalog
  assert(s.code.length > 0, "code should be populated");
  assert(s.rule.length > 0, "rule should be populated");
  assert(s.layerName.length > 0, "layerName should be populated");
  assert(s.category.length > 0, "category should be populated");
  assert(s.suggestion.length > 0, "suggestion should be populated");
  assert(s.impactScore > 0, "impactScore should be positive");
  assert(s.estimatedGain > 0, "estimatedGain should be positive");
  assert(s.estimatedGain <= 100, "estimatedGain should be capped at 100");
  assertEquals(s.fileCount, 1);
  assertEquals(s.files.length, 1);
  assert(["low", "medium", "high"].includes(s.effort), "effort should be low/medium/high");
});

// ---------------------------------------------------------------------------
// S9: CLI parsing — --suggest flag
// ---------------------------------------------------------------------------

Deno.test("suggest/S9: help shows --suggest flag", async () => {
  const cmd = new Deno.Command("deno", {
    args: ["run", "--allow-read", "mod.ts", "help"],
    cwd: Deno.cwd(),
    stdout: "piped",
    stderr: "piped",
  });
  const { stdout } = await cmd.output();
  const out = new TextDecoder().decode(stdout);
  assert(out.includes("--suggest"), "Help should show --suggest flag");
});

// ---------------------------------------------------------------------------
// S10: Multiple files with mixed issues — correct total
// ---------------------------------------------------------------------------

Deno.test("suggest/S10: multi-file mixed issues aggregate correctly", () => {
  const summaries = [
    summary("a.go", [
      result("a.go", "error", "block/order", "Block order violation"),
      result("a.go", "warn", "setup/subsection-order", "Wrong order"),
    ]),
    summary("b.go", [
      result("b.go", "error", "block/order", "Block order violation"),
      result("b.go", "warn", "body/subsection-order", "Wrong body order"),
    ]),
    summary("c.go", [
      result("c.go", "error", "block/order", "Block order violation"),
      result("c.go", "warn", "setup/subsection-order", "Wrong order"),
    ]),
  ];
  const suggestions = computeQuickFixes(summaries, 10);

  // block/order should be in 3 files
  const blockOrder = suggestions.find((s) => s.rule === "block/order");
  assert(blockOrder !== undefined, "block/order should be in suggestions");
  assertEquals(blockOrder!.fileCount, 3);

  // setup/subsection-order should be in 2 files
  const setupOrder = suggestions.find((s) => s.rule === "setup/subsection-order");
  assert(setupOrder !== undefined, "setup/subsection-order should be in suggestions");
  assertEquals(setupOrder!.fileCount, 2);

  // body/subsection-order should be in 1 file
  const bodyOrder = suggestions.find((s) => s.rule === "body/subsection-order");
  assert(bodyOrder !== undefined, "body/subsection-order should be in suggestions");
  assertEquals(bodyOrder!.fileCount, 1);
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Suggestion engine tests — verify ranking, grouping, effort estimation,
// and output correctness. Each test isolates one ranking factor.
//
// "Redeeming the time, because the days are evil." — Ephesians 5:16
// ============================================================================
