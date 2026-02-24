// R-level: R10 (engine layer — output formatting)
// Migrated from: tests/engine/output_test.ts
// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R10/engine/output_test.ts
// key:     B-tov-cws-struct-tests-engine-output
// title:   CWS Struct — Output Formatting Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Verify enhanced summary output — per-block breakdown, per-layer
//          breakdown, and top issues aggregation. Tests capture console.log
//          output and verify structure and content.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import {
  assertEquals,
  assertStringIncludes,
} from "jsr:@std/assert";

import {
  printTotals,
} from "@lib/engine/output.ts";

import type {
  LintSummary,
  HealthScore,
} from "@lib/foundation/mod.ts";

// ---------------------------------------------------------------------------
// Test helpers
// ---------------------------------------------------------------------------

/** Capture console.log output during a function call. */
function captureLog(fn: () => void): string[] {
  const lines: string[] = [];
  const original = console.log;
  console.log = (...args: unknown[]) => {
    lines.push(args.map(String).join(" "));
  };
  try {
    fn();
  } finally {
    console.log = original;
  }
  return lines;
}

/** Build a minimal HealthScore for testing. */
function makeHealth(
  total: number,
  blocks: Array<{ block: string; score: number; section: string; aligned: number; misaligned: number; neutral: number }>,
): HealthScore {
  return {
    total,
    level: total >= 67 ? "perfect" : total >= 34 ? "whole" : total >= 1 ? "sound" : total === 0 ? "even" : total >= -33 ? "lacking" : total >= -66 ? "wanting" : "broken",
    emoji: total >= 67 ? "\uD83D\uDC51" : total >= 34 ? "\uD83D\uDC8E" : total >= 1 ? "\uD83D\uDC9A" : "\uD83D\uDFE1",
    direction: total > 0 ? 1 : total < 0 ? -1 : 0,
    hardpoint: 0,
    trite: 0,
    blocks: blocks.map((b) => ({
      block: b.block,
      score: b.score,
      containers: [{
        block: b.block,
        section: b.section,
        score: b.score,
        total: b.aligned + b.misaligned + b.neutral,
        aligned: b.aligned,
        misaligned: b.misaligned,
        neutral: b.neutral,
        actions: [],
      }],
    })),
    totalActions: blocks.reduce((n, b) => n + b.aligned + b.misaligned + b.neutral, 0),
    alignedCount: blocks.reduce((n, b) => n + b.aligned, 0),
    neutralCount: blocks.reduce((n, b) => n + b.neutral, 0),
    misalignedCount: blocks.reduce((n, b) => n + b.misaligned, 0),
  };
}

/** Build a minimal LintSummary. */
function makeSummary(
  file: string,
  errors: number,
  warnings: number,
  infos: number,
  health?: HealthScore,
  results?: Array<{ severity: "error" | "warn" | "info"; rule: string; message: string; errorCode?: string }>,
): LintSummary {
  return {
    file,
    errors,
    warnings,
    infos,
    health,
    results: (results ?? []).map((r) => ({ file, severity: r.severity, rule: r.rule, message: r.message, errorCode: r.errorCode })),
  };
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Basic summary output
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-085] output/printTotals: shows file count and errors", () => {
  const summaries = [
    makeSummary("a.go", 0, 1, 2),
    makeSummary("b.go", 3, 0, 1),
  ];

  const lines = captureLog(() => printTotals(summaries));
  const joined = lines.join("\n");

  assertStringIncludes(joined, "2 files");
  assertStringIncludes(joined, "1 clean");
  assertStringIncludes(joined, "3 errors");
  assertStringIncludes(joined, "1 warnings");
  assertStringIncludes(joined, "3 info");
});

Deno.test("output/printTotals: shows avg health when scored", () => {
  const summaries = [
    makeSummary("a.go", 0, 0, 0, makeHealth(80, [
      { block: "METADATA", score: 90, section: "directives", aligned: 9, misaligned: 1, neutral: 0 },
    ])),
    makeSummary("b.go", 0, 0, 0, makeHealth(60, [
      { block: "METADATA", score: 70, section: "directives", aligned: 7, misaligned: 3, neutral: 0 },
    ])),
  ];

  const lines = captureLog(() => printTotals(summaries));
  const joined = lines.join("\n");

  assertStringIncludes(joined, "avg health");
  assertStringIncludes(joined, "70"); // (80 + 60) / 2 = 70
  assertStringIncludes(joined, "2 scored");
});

// ---------------------------------------------------------------------------
// 2. Per-block breakdown
// ---------------------------------------------------------------------------

Deno.test("output/printTotals: shows per-block breakdown with multi-file health", () => {
  const summaries = [
    makeSummary("a.go", 0, 0, 0, makeHealth(80, [
      { block: "METADATA", score: 90, section: "directives", aligned: 9, misaligned: 1, neutral: 0 },
      { block: "SETUP", score: 70, section: "ordering", aligned: 7, misaligned: 3, neutral: 0 },
      { block: "BODY", score: 80, section: "content-expectations", aligned: 8, misaligned: 2, neutral: 0 },
      { block: "CLOSING", score: 60, section: "placement", aligned: 6, misaligned: 4, neutral: 0 },
    ])),
    makeSummary("b.go", 0, 0, 0, makeHealth(60, [
      { block: "METADATA", score: 80, section: "directives", aligned: 8, misaligned: 2, neutral: 0 },
      { block: "SETUP", score: 50, section: "ordering", aligned: 5, misaligned: 5, neutral: 0 },
      { block: "BODY", score: 60, section: "content-expectations", aligned: 6, misaligned: 4, neutral: 0 },
      { block: "CLOSING", score: 40, section: "placement", aligned: 4, misaligned: 6, neutral: 0 },
    ])),
  ];

  const lines = captureLog(() => printTotals(summaries));
  const joined = lines.join("\n");

  assertStringIncludes(joined, "Per-block:");
  assertStringIncludes(joined, "METADATA");
  assertStringIncludes(joined, "SETUP");
  assertStringIncludes(joined, "BODY");
  assertStringIncludes(joined, "CLOSING");
  // METADATA avg: (90+80)/2 = 85
  assertStringIncludes(joined, "85");
});

Deno.test("output/printTotals: omits per-block for single file", () => {
  const summaries = [
    makeSummary("a.go", 0, 0, 0, makeHealth(80, [
      { block: "METADATA", score: 90, section: "directives", aligned: 9, misaligned: 1, neutral: 0 },
    ])),
  ];

  const lines = captureLog(() => printTotals(summaries));
  const joined = lines.join("\n");

  // Single file should NOT show per-block breakdown (redundant with individual output)
  assertEquals(joined.includes("Per-block:"), false);
});

// ---------------------------------------------------------------------------
// 3. Per-layer breakdown
// ---------------------------------------------------------------------------

Deno.test("output/printTotals: shows per-layer breakdown", () => {
  const summaries = [
    makeSummary("a.go", 0, 0, 0, makeHealth(80, [
      { block: "METADATA", score: 100, section: "blocks", aligned: 5, misaligned: 0, neutral: 0 },
      { block: "METADATA", score: 80, section: "identity", aligned: 8, misaligned: 2, neutral: 0 },
      { block: "SETUP", score: 60, section: "placement", aligned: 6, misaligned: 4, neutral: 0 },
      { block: "BODY", score: 50, section: "content-expectations", aligned: 5, misaligned: 5, neutral: 0 },
    ])),
    makeSummary("b.go", 0, 0, 0, makeHealth(60, [
      { block: "METADATA", score: 100, section: "blocks", aligned: 5, misaligned: 0, neutral: 0 },
      { block: "METADATA", score: 70, section: "identity", aligned: 7, misaligned: 3, neutral: 0 },
      { block: "SETUP", score: 40, section: "placement", aligned: 4, misaligned: 6, neutral: 0 },
      { block: "BODY", score: 30, section: "content-expectations", aligned: 3, misaligned: 7, neutral: 0 },
    ])),
  ];

  const lines = captureLog(() => printTotals(summaries));
  const joined = lines.join("\n");

  assertStringIncludes(joined, "Per-layer:");
  assertStringIncludes(joined, "R[50]");
  assertStringIncludes(joined, "R[25]");
  assertStringIncludes(joined, "R[10]");
  assertStringIncludes(joined, "R[5]");
});

Deno.test("output/printTotals: shows cascade count in layer breakdown", () => {
  const summaries = [
    makeSummary("a.go", 2, 0, 0, makeHealth(-50, [
      { block: "METADATA", score: -100, section: "blocks", aligned: 0, misaligned: 5, neutral: 0 },
      // Downstream is all neutral (cascade from blocks failure)
      { block: "SETUP", score: 0, section: "ordering", aligned: 0, misaligned: 0, neutral: 5 },
    ])),
    makeSummary("b.go", 0, 0, 0, makeHealth(80, [
      { block: "METADATA", score: 100, section: "blocks", aligned: 5, misaligned: 0, neutral: 0 },
      { block: "SETUP", score: 80, section: "ordering", aligned: 8, misaligned: 2, neutral: 0 },
    ])),
  ];

  const lines = captureLog(() => printTotals(summaries));
  const joined = lines.join("\n");

  // File a's SETUP layer should be counted as cascaded
  assertStringIncludes(joined, "cascaded");
});

// ---------------------------------------------------------------------------
// 4. Top issues
// ---------------------------------------------------------------------------

Deno.test("[CWS-R50-020] output/printTotals: shows top issues when errors across multiple files", () => {
  const summaries = [
    makeSummary("a.go", 2, 0, 0, undefined, [
      { severity: "error", rule: "block/order", message: "Wrong block order", errorCode: "CWS-R50-020" },
      { severity: "error", rule: "identity/pragma-missing", message: "No pragma", errorCode: "CWS-R25-001" },
    ]),
    makeSummary("b.go", 1, 0, 0, undefined, [
      { severity: "error", rule: "block/order", message: "Wrong block order", errorCode: "CWS-R50-020" },
    ]),
    makeSummary("c.go", 1, 0, 0, undefined, [
      { severity: "error", rule: "block/order", message: "Wrong block order", errorCode: "CWS-R50-020" },
    ]),
  ];

  const lines = captureLog(() => printTotals(summaries));
  const joined = lines.join("\n");

  assertStringIncludes(joined, "Top issues:");
  assertStringIncludes(joined, "block/order");
  assertStringIncludes(joined, "3 files"); // block/order appears in all 3 files
  assertStringIncludes(joined, "CWS-R50-020");
});

Deno.test("output/printTotals: omits top issues for single file", () => {
  const summaries = [
    makeSummary("a.go", 2, 0, 0, undefined, [
      { severity: "error", rule: "block/order", message: "Wrong block order" },
    ]),
  ];

  const lines = captureLog(() => printTotals(summaries));
  const joined = lines.join("\n");

  assertEquals(joined.includes("Top issues:"), false);
});

Deno.test("output/printTotals: omits top issues when no errors", () => {
  const summaries = [
    makeSummary("a.go", 0, 2, 0),
    makeSummary("b.go", 0, 1, 0),
  ];

  const lines = captureLog(() => printTotals(summaries));
  const joined = lines.join("\n");

  assertEquals(joined.includes("Top issues:"), false);
});

// ---------------------------------------------------------------------------
// 5. All-clean message
// ---------------------------------------------------------------------------

Deno.test("output/printTotals: shows all-clean message when zero errors", () => {
  const summaries = [
    makeSummary("a.go", 0, 0, 0),
    makeSummary("b.go", 0, 1, 2),
  ];

  const lines = captureLog(() => printTotals(summaries));
  const joined = lines.join("\n");

  assertStringIncludes(joined, "All files structurally valid");
});

Deno.test("output/printTotals: shows must-be-fixed message when errors exist", () => {
  const summaries = [
    makeSummary("a.go", 3, 0, 0),
  ];

  const lines = captureLog(() => printTotals(summaries));
  const joined = lines.join("\n");

  assertStringIncludes(joined, "must be fixed");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Output formatting tests — the face of the tool.
// Summary that's informative and scannable, not noisy.
//
// "Write the vision, and make it plain upon tables."
// — Habakkuk 2:2
// ============================================================================
