// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/output.ts
// key:     B-tov-cws-struct-lib-engine-output
// title:   CWS Struct — Output Formatting
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-11
// authors: Nova Dawn (CPI-SI)
// purpose: Consistent terminal output across all format handlers.
//          Colors, severity display, per-file summaries, totals.
//          Extracted from the duplicated display code in each standalone linter.
//
// biblical_foundation: "Write the vision, and make it plain upon tables,
//   that he may run that readeth it." — Habakkuk 2:2
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { LintResult, LintSummary, Severity, HealthScore } from "../foundation/mod.ts";

// ---------------------------------------------------------------------------
// Constants — ANSI colors
// ---------------------------------------------------------------------------

export const COLORS = {
  reset: "\x1b[0m",
  red: "\x1b[31m",
  yellow: "\x1b[33m",
  blue: "\x1b[34m",
  green: "\x1b[32m",
  dim: "\x1b[2m",
  bold: "\x1b[1m",
} as const;

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

export function severityColor(s: Severity): string {
  switch (s) {
    case "error":
      return COLORS.red;
    case "warn":
      return COLORS.yellow;
    case "info":
      return COLORS.blue;
  }
}

export function severityIcon(s: Severity): string {
  switch (s) {
    case "error":
      return "E";
    case "warn":
      return "W";
    case "info":
      return "I";
  }
}

// ---------------------------------------------------------------------------
// Per-file display
// ---------------------------------------------------------------------------

/**
 * Color a health score value based on threshold.
 * 80+ = green, 50-79 = yellow, <50 = red.
 */
function healthColor(score: number): string {
  if (score >= 80) return COLORS.green;
  if (score >= 50) return COLORS.yellow;
  return COLORS.red;
}

export function printFileSummary(
  summary: LintSummary,
  verbose: boolean,
): void {
  const status =
    summary.errors === 0
      ? `${COLORS.green}OK${COLORS.reset}`
      : `${COLORS.red}FAIL${COLORS.reset}`;

  // Health score on the status line (if computed)
  const healthStr = summary.health
    ? `  ${healthColor(summary.health.total)}health: ${summary.health.total}/100${COLORS.reset}`
    : "";

  console.log(
    `${status}  ${summary.file}  (${summary.errors}E ${summary.warnings}W ${summary.infos}I)${healthStr}`,
  );

  // Show individual results based on verbosity
  const show = verbose
    ? summary.results
    : summary.results.filter((r) => r.severity !== "info");

  for (const r of show) {
    const color = severityColor(r.severity);
    const icon = severityIcon(r.severity);
    const lineRef = r.line ? `${COLORS.dim}:${r.line}${COLORS.reset} ` : "";
    console.log(`  ${color}[${icon}]${COLORS.reset} ${lineRef}${r.rule}: ${r.message}`);
  }

  // Verbose: show per-block health breakdown
  if (verbose && summary.health) {
    printHealthBreakdown(summary.health);
  }

  if (show.length > 0 || (verbose && summary.health)) console.log("");
}

/**
 * Print per-block and per-container health breakdown.
 * Shows which sections are healthy and which need attention.
 */
function printHealthBreakdown(health: HealthScore): void {
  console.log(
    `  ${COLORS.dim}── health: ${health.passCount} pass, ${health.failCount} fail ` +
    `(${health.totalActions} actions) ──${COLORS.reset}`,
  );

  for (const block of health.blocks) {
    const color = healthColor(block.score);
    console.log(
      `  ${color}[${block.score}]${COLORS.reset} ${COLORS.bold}${block.block}${COLORS.reset}`,
    );

    for (const container of block.containers) {
      if (container.total === 0) continue;
      const cColor = healthColor(container.score);
      const fails = container.failedErrors + container.failedWarnings + container.failedInfos;
      const detail = fails > 0
        ? ` ${COLORS.dim}(${container.passed}/${container.total} pass, ` +
          `${container.failedErrors}E ${container.failedWarnings}W ${container.failedInfos}I)${COLORS.reset}`
        : ` ${COLORS.dim}(${container.total}/${container.total})${COLORS.reset}`;
      console.log(
        `    ${cColor}${container.score}${COLORS.reset} ${container.section}${detail}`,
      );
    }
  }
}

// ---------------------------------------------------------------------------
// Totals display
// ---------------------------------------------------------------------------

export function printTotals(summaries: LintSummary[]): void {
  const totalFiles = summaries.length;
  const passFiles = summaries.filter((s) => s.errors === 0).length;
  const totalErrors = summaries.reduce((n, s) => n + s.errors, 0);
  const totalWarnings = summaries.reduce((n, s) => n + s.warnings, 0);
  const totalInfos = summaries.reduce((n, s) => n + s.infos, 0);

  // Compute average health across files that have health scores
  const healthFiles = summaries.filter((s) => s.health !== undefined);
  const avgHealth = healthFiles.length > 0
    ? Math.round(healthFiles.reduce((sum, s) => sum + s.health!.total, 0) / healthFiles.length)
    : undefined;

  console.log("");
  console.log(
    `${COLORS.bold}Summary:${COLORS.reset} ${totalFiles} files, ${passFiles} clean`,
  );
  console.log(
    `  ${COLORS.red}${totalErrors} errors${COLORS.reset}  ` +
      `${COLORS.yellow}${totalWarnings} warnings${COLORS.reset}  ` +
      `${COLORS.blue}${totalInfos} info${COLORS.reset}`,
  );

  if (avgHealth !== undefined) {
    const hColor = healthColor(avgHealth);
    console.log(
      `  ${hColor}avg health: ${avgHealth}/100${COLORS.reset}` +
        ` ${COLORS.dim}(${healthFiles.length} scored)${COLORS.reset}`,
    );
  }

  if (totalErrors === 0) {
    console.log(
      `\n${COLORS.green}All files structurally valid.${COLORS.reset}`,
    );
  } else {
    console.log(
      `\n${COLORS.red}${totalErrors} structural errors must be fixed.${COLORS.reset}`,
    );
  }
}

// ---------------------------------------------------------------------------
// Headings
// ---------------------------------------------------------------------------

export function printHeader(
  toolName: string,
  version: string,
  fileCount: number,
  format: string,
): void {
  console.log(
    `${COLORS.bold}${toolName}${COLORS.reset} v${version} — ${format}`,
  );
  console.log(
    `${COLORS.dim}"Prove all things; hold fast that which is good." — 1 Thess 5:21${COLORS.reset}`,
  );
  console.log(
    `\n${COLORS.bold}Linting ${fileCount} file(s)...${COLORS.reset}\n`,
  );
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Output is the face of the tool. Consistent, readable, color-coded.
// "Make it plain upon tables, that he may run that readeth it."
// ============================================================================
