// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/suggest.ts
// key:     B-tov-cws-struct-lib-engine-suggest
// title:   CWS Struct — Quick-Fix Suggestions Engine
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Rank lint findings by impact and produce actionable "fix this first"
//          suggestions. Groups issues by error code across files, scores by
//          severity × frequency × cascade multiplier ÷ effort, and returns
//          the top N most impactful fixes to make.
//
//          Ranking factors:
//            1. Severity weight  — error=3, warn=2, info=1
//            2. Frequency        — how many files have this issue
//            3. Cascade layer    — L0=4×, L1=3×, L2=2×, L3=1× (upstream fixes clear noise)
//            4. Effort estimate  — low=3×, medium=2×, high=1× (easy wins ranked higher)
//            5. Health gain      — estimated score improvement from fixing
//
//          The score isn't precise — it's directional. "Fix this first" not
//          "this is exactly 4.7 points better." Good enough to prioritize.
//
// biblical_foundation: "Redeeming the time, because the days are evil."
//   — Ephesians 5:16  (Fix what matters most. Time is a gift.)
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { LintSummary, LintResult, Severity } from "../foundation/mod.ts";
import type { ErrorCodeEntry } from "../data/mod.ts";
import { matchRule } from "../data/mod.ts";
import { COLORS } from "./output.ts";

// ---------------------------------------------------------------------------
// Constants — ranking weights
// ---------------------------------------------------------------------------

/** Severity impact weight — error matters most. */
const SEVERITY_WEIGHT: Record<Severity, number> = {
  error: 3,
  warn: 2,
  info: 1,
};

/**
 * Cascade multiplier by detection layer.
 *
 * Layer 0 (R[50]) fixes clear the most downstream noise.
 * Layer 3 (R[05]) fixes are leaf-level — no cascade benefit.
 */
const CASCADE_MULTIPLIER: Record<number, number> = {
  0: 4,  // whole file — fixes here ripple everywhere
  1: 3,  // structure  — fixes here clear container + content
  2: 2,  // container  — fixes here clear content
  3: 1,  // content    — leaf level
  4: 0,  // tool       — internal, not user-fixable
};

/**
 * Effort multiplier by category — easy wins rank higher.
 *
 * The multiplier is INVERTED: low effort = high multiplier.
 * A "low effort" fix that affects many files is the best ROI.
 */
const EFFORT_MULTIPLIER: Record<string, number> = {
  // Low effort — reorder, reclassify, add markers
  style: 3,
  ordering: 3,
  classify: 3,

  // Medium effort — add blocks, fields, identity
  block: 2,
  directive: 2,
  identity: 2,
  required: 2,
  metadata: 2,
  closing: 2,
  format: 2,
  validation: 2,
  language: 2,
  system: 1,  // IO errors — can't fix code, need permissions

  // High effort — move code, write content, fix placement
  placement: 1,
  content: 1,
  doc: 1,
  template: 1,
  emphasis: 1,
  scaling: 1,
  ofd: 1,
  omni: 2,
  schema: 1,
};

/** Default effort multiplier for unknown categories. */
const DEFAULT_EFFORT = 2;

// ---------------------------------------------------------------------------
// Types — suggestion output
// ---------------------------------------------------------------------------

/** One ranked quick-fix suggestion. */
export interface QuickFixSuggestion {
  /** Error code (e.g., "CWS-R50-020") — or rule string if no code. */
  code: string;
  /** Rule string from the lint results. */
  rule: string;
  /** Detection layer: 0-3. */
  layer: number;
  /** Layer display name: "R[50]", "R[25]", etc. */
  layerName: string;
  /** Category within layer. */
  category: string;
  /** Default severity for this issue. */
  severity: Severity;
  /** Number of files affected. */
  fileCount: number;
  /** Files affected (first 5 for display). */
  files: string[];
  /** What to do — from the error catalog suggestionTemplate. */
  suggestion: string;
  /** Effort level: "low" | "medium" | "high". */
  effort: "low" | "medium" | "high";
  /** Composite impact score — higher = fix first. */
  impactScore: number;
  /** Estimated health gain (approximate). */
  estimatedGain: number;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Core — compute ranked suggestions
// ---------------------------------------------------------------------------

/**
 * Compute ranked quick-fix suggestions from lint summaries.
 *
 * Groups issues by error code (or rule string), counts frequency across files,
 * ranks by composite impact score, returns top N.
 *
 * @param summaries Lint summaries from all files
 * @param limit Maximum number of suggestions (default 5)
 * @returns Ranked suggestions, highest impact first
 */
export function computeQuickFixes(
  summaries: LintSummary[],
  limit = 5,
): QuickFixSuggestion[] {
  // 1. Group results by error code across files
  const groups = groupByErrorCode(summaries);

  // 2. Build suggestion for each group
  const suggestions: QuickFixSuggestion[] = [];

  for (const [key, group] of groups) {
    const entry = group.entry;
    const fileCount = group.files.size;
    const files = [...group.files].slice(0, 5);

    // Determine layer and cascade
    const layer = entry?.layer ?? guessLayer(group.representative);
    const category = entry?.category ?? "unknown";
    const severity = group.representative.severity;

    // Effort estimation
    const effortMultiplier = EFFORT_MULTIPLIER[category] ?? DEFAULT_EFFORT;
    const effort = effortMultiplier >= 3 ? "low" as const
      : effortMultiplier >= 2 ? "medium" as const
      : "high" as const;

    // Composite impact score
    const severityW = SEVERITY_WEIGHT[severity];
    const cascadeM = CASCADE_MULTIPLIER[layer] ?? 1;
    const impactScore = severityW * fileCount * cascadeM * effortMultiplier;

    // Estimated health gain — rough: each fix removes one misaligned action per file.
    // Impact weight (error=2, warn=1, info=0.25) × files ÷ typical total actions (~30)
    const impactWeight = severity === "error" ? 2 : severity === "warn" ? 1 : 0.25;
    const estimatedGain = Math.round(impactWeight * fileCount * 100 / 30);

    // Suggestion text
    const suggestion = entry?.suggestionTemplate
      ?? group.representative.fix?.description
      ?? group.representative.message;

    suggestions.push({
      code: entry?.code ?? key,
      rule: group.representative.rule,
      layer,
      layerName: formatLayerName(layer),
      category,
      severity,
      fileCount,
      files,
      suggestion,
      effort,
      impactScore,
      estimatedGain: Math.min(estimatedGain, 100), // cap at ±100
    });
  }

  // 3. Sort by impact score descending, take top N
  suggestions.sort((a, b) => b.impactScore - a.impactScore);
  return suggestions.slice(0, limit);
}

// ---------------------------------------------------------------------------
// Helpers — grouping
// ---------------------------------------------------------------------------

interface ResultGroup {
  /** Catalog entry (if found). */
  entry: ErrorCodeEntry | undefined;
  /** Set of files affected. */
  files: Set<string>;
  /** Representative result (first encountered). */
  representative: LintResult;
  /** Total count across all files. */
  totalCount: number;
}

/**
 * Group lint results by error code across all files.
 * Returns Map keyed by error code (or rule string as fallback).
 */
function groupByErrorCode(summaries: LintSummary[]): Map<string, ResultGroup> {
  const groups = new Map<string, ResultGroup>();

  for (const summary of summaries) {
    // Track rules seen in THIS file (count files, not occurrences)
    const seenKeys = new Set<string>();

    for (const result of summary.results) {
      // Skip info results — they're not actionable fixes
      if (result.severity === "info") continue;

      // Lookup error code entry
      const entry = matchRule(result.rule);
      const key = entry?.code ?? result.errorCode ?? result.rule;

      const isNewFile = !seenKeys.has(key);
      seenKeys.add(key);

      const existing = groups.get(key);
      if (existing) {
        if (isNewFile) existing.files.add(summary.file);
        existing.totalCount++;
      } else {
        const files = new Set<string>();
        files.add(summary.file);
        groups.set(key, {
          entry,
          files,
          representative: result,
          totalCount: 1,
        });
      }
    }
  }

  return groups;
}

/**
 * Guess detection layer from a lint result (when no catalog entry found).
 * Heuristic based on rule prefix patterns.
 */
function guessLayer(result: LintResult): number {
  const r = result.rule.toLowerCase();
  if (r.startsWith("block/") || r.startsWith("directive/") || r.startsWith("parse/")) return 0;
  if (r.startsWith("setup/") || r.startsWith("body/") || r.startsWith("closing/") ||
      r.startsWith("identity/") || r.startsWith("form/")) return 1;
  if (r.startsWith("content/") || r.startsWith("placement/")) return 2;
  if (r.startsWith("doc/") || r.startsWith("template/")) return 3;
  return 1; // default to structure
}

/** Format layer number to display name. */
function formatLayerName(layer: number): string {
  switch (layer) {
    case 0: return "R[50]";
    case 1: return "R[25]";
    case 2: return "R[10]";
    case 3: return "R[5]";
    default: return `L${layer}`;
  }
}

// ---------------------------------------------------------------------------
// Display — formatted output
// ---------------------------------------------------------------------------

/**
 * Print quick-fix suggestions to stdout.
 *
 * Format:
 * ```
 * ═══ Quick Fixes (estimated +23 health) ═══
 * 1. Fix section ordering in 5 files (CWS-R25-003)
 *    ↳ Move constants before types in SETUP block
 *    ↳ Estimated gain: +8 health, effort: low
 *    ↳ Files: file1.go, file2.go, file3.go, file4.go, file5.go
 * ```
 */
export function printQuickFixes(suggestions: QuickFixSuggestion[]): void {
  if (suggestions.length === 0) return;

  // Total estimated gain
  const totalGain = suggestions.reduce((sum, s) => sum + s.estimatedGain, 0);

  console.log("");
  console.log(
    `${COLORS.bold}\u2550\u2550\u2550 Quick Fixes${COLORS.reset}` +
    ` ${COLORS.green}(estimated +${totalGain} health)${COLORS.reset}` +
    ` ${COLORS.bold}\u2550\u2550\u2550${COLORS.reset}`,
  );

  for (let i = 0; i < suggestions.length; i++) {
    const s = suggestions[i]!;
    const num = `${i + 1}.`;

    // Effort color
    const effortColor = s.effort === "low" ? COLORS.green
      : s.effort === "medium" ? COLORS.yellow
      : COLORS.red;

    // Severity color
    const sevColor = s.severity === "error" ? COLORS.red
      : s.severity === "warn" ? COLORS.yellow
      : COLORS.blue;

    // Code display
    const codeStr = s.code.startsWith("CWS-")
      ? `${COLORS.dim}(${s.code})${COLORS.reset}`
      : `${COLORS.dim}(${s.rule})${COLORS.reset}`;

    // Layer tag
    const layerTag = `${COLORS.dim}[${s.layerName}]${COLORS.reset}`;

    // Main line: "1. Fix section ordering in 5 files (CWS-R25-003)"
    console.log(
      `${COLORS.bold}${num}${COLORS.reset} ` +
      `${sevColor}${s.suggestion}${COLORS.reset} ` +
      `in ${s.fileCount} file${s.fileCount !== 1 ? "s" : ""} ` +
      `${codeStr} ${layerTag}`,
    );

    // Detail: estimated gain + effort
    console.log(
      `   \u21B3 Estimated gain: ${COLORS.green}+${s.estimatedGain} health${COLORS.reset}, ` +
      `effort: ${effortColor}${s.effort}${COLORS.reset}`,
    );

    // File list (truncated to 5)
    const fileNames = s.files.map((f) => {
      // Show just the filename, not full path
      const parts = f.split("/");
      return parts[parts.length - 1]!;
    });
    const fileStr = fileNames.join(", ");
    const moreStr = s.fileCount > 5
      ? ` ${COLORS.dim}(+${s.fileCount - 5} more)${COLORS.reset}`
      : "";
    console.log(
      `   \u21B3 ${COLORS.dim}Files: ${fileStr}${moreStr}${COLORS.reset}`,
    );
  }

  console.log(
    `${COLORS.bold}\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550` +
    `\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550` +
    `\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550` +
    `\u2550${COLORS.reset}`,
  );
}

/**
 * Print a compact 3-suggestion summary (for always-on summary mode).
 * Shorter than full printQuickFixes — just the top 3 one-liners.
 */
export function printSuggestSummary(suggestions: QuickFixSuggestion[]): void {
  if (suggestions.length === 0) return;

  const top3 = suggestions.slice(0, 3);
  console.log("");
  console.log(`  ${COLORS.bold}Fix first:${COLORS.reset}`);

  for (let i = 0; i < top3.length; i++) {
    const s = top3[i]!;
    const effortColor = s.effort === "low" ? COLORS.green
      : s.effort === "medium" ? COLORS.yellow
      : COLORS.red;
    const codeStr = s.code.startsWith("CWS-")
      ? `${COLORS.bold}${s.code}${COLORS.reset}`
      : "";

    console.log(
      `    ${COLORS.dim}${i + 1}.${COLORS.reset} ${codeStr} ${s.suggestion}` +
      ` ${COLORS.dim}(${s.fileCount} file${s.fileCount !== 1 ? "s" : ""},` +
      ` ${effortColor}${s.effort}${COLORS.reset}${COLORS.dim})${COLORS.reset}`,
    );
  }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Quick-fix suggestions — "Redeeming the time."
// Fix what matters most. Every suggestion traces to the error catalog,
// every score traces to severity × frequency × cascade × effort.
// Directional, not precise. "Fix this first," not "exactly 4.7 points."
//
// "Redeeming the time, because the days are evil." — Ephesians 5:16
// ============================================================================
