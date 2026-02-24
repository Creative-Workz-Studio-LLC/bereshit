// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/commands/transform.ts
// key:     B-tov-cws-struct-lib-commands-transform
// title:   CWS Struct — Transform Command
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Structural transformation — fix/align files across all OmniCode
//          formats. Extracted from mod.ts during Phase 1 God Object dismantling.
//
// biblical_foundation: "Be ye transformed by the renewing of your mind."
//   — Romans 12:2
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { relative } from "@std/path";
import { pooledMap } from "@std/async/pool";
import type { CliOptions, LintSummary, FormatHandler } from "../foundation/mod.ts";
import { summarize } from "../foundation/mod.ts";
import {
  COLORS,
  printFileSummary,
  printTotals,
  resolveErrorCodes,
  discoverFiles,
  discoverAllFiles,
  getFormat,
} from "../engine/mod.ts";
import { writeTransformLogEntries } from "../engine/logging.ts";
import { identifyExplicitFiles } from "./parse.ts";
import { TOOL_NAME, CONCURRENCY } from "./config.ts";

// ============================================================================
// BODY
// ============================================================================

/**
 * Transform files for a single format handler.
 * Extracted so both explicit-format and auto-detect paths share the same logic.
 *
 * Uses bounded concurrency — same pattern as lintWithHandler.
 * Note: transforms write files, so concurrency is safe per-file (each file independent)
 * but shouldn't be too high on slow I/O.
 */
async function transformWithHandler(
  opts: CliOptions,
  handler: FormatHandler,
  files: string[],
  explicitFiles?: Set<string>,
): Promise<LintSummary[]> {
  if (!handler.transform) return [];

  const cwd = Deno.cwd();
  const summaries: LintSummary[] = [];
  const transformFn = handler.transform.bind(handler);

  const pool = pooledMap(CONCURRENCY, files, async (file) => {
    const results = await transformFn(file, {
      dryRun: opts.dryRun,
      extensions: opts.extensions,
      force: opts.force,
      steps: opts.steps,
    });

    // Same explicit-target upgrade as lint — if transform skips an
    // explicitly targeted file, user should know via warning.
    if (explicitFiles?.has(file)) {
      for (const r of results) {
        if ((r.rule === "structure/skip" || r.rule === "structure/module") && r.severity === "info") {
          r.severity = "warn";
          r.message += " — file was explicitly targeted";
        }
      }
    }

    resolveErrorCodes(results);
    const summary = summarize(relative(cwd, file), results);

    // Extract pragma from line 1 for display
    try {
      const text = await Deno.readTextFile(file);
      const firstLine = text.split("\n")[0]?.trim();
      if (firstLine?.startsWith("//omni:") || firstLine?.startsWith("#!omni") || firstLine?.startsWith("// #!omni")) {
        summary.pragma = firstLine;
      }
    } catch { /* file already reported errors if unreadable */ }

    // Transform logging — write each file's transform results to log
    if (opts.logPath) {
      try {
        await writeTransformLogEntries(opts.logPath, results, handler.name, file);
      } catch { /* best-effort logging */ }
    }

    return summary;
  });

  for await (const summary of pool) {
    summaries.push(summary);
    printFileSummary(summary, opts.verbose, opts.check, opts.why, opts.deep);
  }

  return summaries;
}

export async function runTransform(opts: CliOptions): Promise<boolean> {
  if (opts.targets.length === 0) {
    console.error(
      `${COLORS.red}No targets specified. Provide files or directories to transform.${COLORS.reset}`,
    );
    return false;
  }

  // Same explicit-file detection as lint
  const explicitFiles = await identifyExplicitFiles(opts.targets);

  // ── Explicit format: transform with that handler ────────────────
  if (opts.format) {
    const handler = getFormat(opts.format);
    if (!handler?.transform) {
      console.error(
        `${COLORS.red}Transform not available for format: ${opts.format}${COLORS.reset}`,
      );
      return false;
    }

    const files = await discoverFiles(opts.targets, handler);
    if (files.length === 0) {
      console.error(
        `${COLORS.red}No ${handler.extensions.join("/")} files found in specified targets.${COLORS.reset}`,
      );
      return false;
    }

    console.log(
      `${COLORS.bold}Transforming ${files.length} file(s)${opts.dryRun ? " (dry run)" : ""}...${COLORS.reset}\n`,
    );
    const summaries = await transformWithHandler(opts, handler, files, explicitFiles);
    printTotals(summaries);
    return summaries.every((s) => s.errors === 0);
  }

  // ── No format: auto-detect — single walk, dispatch by extension ──
  const filesByFormat = await discoverAllFiles(opts.targets);
  const allSummaries: LintSummary[] = [];
  let totalFiles = 0;

  for (const [formatName, files] of filesByFormat) {
    const handler = getFormat(formatName)!;
    if (!handler.transform) continue;

    totalFiles += files.length;
    console.log(
      `\n${COLORS.bold}── ${handler.name}${COLORS.reset} (${files.length} file${files.length > 1 ? "s" : ""})${opts.dryRun ? " (dry run)" : ""}`,
    );

    const summaries = await transformWithHandler(opts, handler, files, explicitFiles);
    allSummaries.push(...summaries);
  }

  if (totalFiles === 0) {
    console.error(
      `${COLORS.red}No files matched any format with transform support in specified targets.${COLORS.reset}`,
    );
    console.error(`Run '${TOOL_NAME} formats' to see available formats.`);
    return false;
  }

  printTotals(allSummaries);
  return allSummaries.every((s) => s.errors === 0);
}

// ============================================================================
// CLOSING
// ============================================================================
//
// "Be ye transformed by the renewing of your mind." — Romans 12:2
// ============================================================================
