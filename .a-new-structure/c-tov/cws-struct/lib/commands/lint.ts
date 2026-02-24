// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/commands/lint.ts
// key:     B-tov-cws-struct-lib-commands-lint
// title:   CWS Struct — Lint Command Orchestration
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Lint command — thin orchestration layer. Discovers files, dispatches
//          to format handlers, collects summaries, delegates to engine for
//          output, logging, and inspection. No engine logic lives here.
//
// biblical_foundation: "Prove all things; hold fast that which is good."
//   — 1 Thessalonians 5:21
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
  printHeader,
  resolveErrorCodes,
  discoverFiles,
  discoverAllFiles,
  getFormat,
  computeQuickFixes,
  printQuickFixes,
  printSuggestSummary,
  getChangedFiles,
  emitHealthLog,
  writeLogHeader,
  runInspect,
  emitJson,
} from "../engine/mod.ts";
import {
  debugDiscovery,
  debugFileStart,
  debugFileEnd,
  debugHealth,
} from "../engine/debug.ts";
import {
  TOOL_NAME, VERSION, CONCURRENCY,
  getDbModule, recordToDatabase,
} from "./config.ts";
import type { DbHandle } from "./config.ts";
import { identifyExplicitFiles } from "./parse.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Per-handler lint — bounded concurrency, streaming results
// ---------------------------------------------------------------------------

/**
 * Lint files for a single format handler.
 * Extracted so both explicit-format and auto-detect paths share the same logic.
 *
 * Uses bounded concurrency (CONCURRENCY) to lint files in parallel while
 * capping open file handles. Results stream in order via pooledMap.
 *
 * Supports:
 * - --json: Suppress human output (caller handles JSON serialization)
 * - --fail-fast: Stop after first file with errors (current batch completes)
 * - Progress counter: [N/total] when stdout is a TTY (not in json/piped mode)
 */
async function lintWithHandler(
  opts: CliOptions,
  handler: FormatHandler,
  files: string[],
  explicitFiles?: Set<string>,
): Promise<LintSummary[]> {
  const cwd = Deno.cwd();
  const summaries: LintSummary[] = [];
  const showHuman = !opts.json;
  const isTTY = showHuman && Deno.stdout.isTerminal();
  const encoder = new TextEncoder();
  let hitFailFast = false;

  const pool = pooledMap(CONCURRENCY, files, async (file) => {
    debugFileStart(file);
    const results = await handler.lint(file);

    // ── Explicit-target upgrade ──────────────────────────────────
    // When a user explicitly passes a file (not discovered via directory
    // walk), a silent info-level skip is misleading — the user pointed at
    // this file and expects feedback. Upgrade to warn.
    //
    // Applies to all handlers that emit these skip rules:
    //   structure/skip   — Go, Rust (no omni directives or block markers)
    //   structure/module — Rust (module file, identity in lib.rs)
    if (explicitFiles?.has(file)) {
      for (const r of results) {
        if ((r.rule === "structure/skip" || r.rule === "structure/module") && r.severity === "info") {
          r.severity = "warn";
          r.message += " — file was explicitly targeted";
        }
      }
    }

    // ── Error code resolution ────────────────────────────────────
    // Enrich each LintResult with its CWS error code and layer name.
    // Handlers emit rule strings; the engine resolves them to catalog codes.
    resolveErrorCodes(results);

    const health = handler.computeHealth
      ? await handler.computeHealth(file, results)
      : undefined;
    if (health) debugHealth(health);
    const summary = summarize(relative(cwd, file), results, health);
    debugFileEnd(file, results.length);

    // Extract pragma from line 1 for display
    try {
      const text = await Deno.readTextFile(file);
      const firstLine = text.split("\n")[0]?.trim();
      if (firstLine?.startsWith("//omni:") || firstLine?.startsWith("#!omni") || firstLine?.startsWith("// #!omni")) {
        summary.pragma = firstLine;
      }
    } catch { /* file already reported errors if unreadable */ }

    // Verbose: populate anatomy from inspect (block → container → content)
    if (opts.verbose && handler.inspect) {
      try {
        const ir = await handler.inspect(file);
        summary.anatomy = {
          directives: ir.directives,
          blocks: ir.blocks.map((b) => ({
            name: b.name,
            startLine: b.startLine,
            endLine: b.endLine,
            lines: b.endLine - b.startLine + 1,
          })),
          identity: ir.identity ?? {},
          sections: {},
          content: {},
        };
        // Group sections by block
        for (const s of ir.sections) {
          const block = s.block;
          if (!summary.anatomy.sections[block]) summary.anatomy.sections[block] = [];
          summary.anatomy.sections[block].push({
            name: s.name, block, line: s.line,
          });
        }
        // Group content by block
        for (const c of ir.content) {
          for (const block of c.blocks) {
            if (!summary.anatomy.content![block]) summary.anatomy.content![block] = [];
            summary.anatomy.content![block].push(`${c.category} (${c.count})`);
          }
        }
      } catch { /* inspect failure is non-fatal — verbose degrades gracefully */ }
    }

    return summary;
  });

  for await (const summary of pool) {
    summaries.push(summary);

    // Progress counter — TTY only, cleared before each result
    if (isTTY) {
      Deno.stdout.writeSync(encoder.encode(
        `\r\x1b[K[${summaries.length}/${files.length}] `,
      ));
    }

    if (showHuman && !opts.summaryOnly) {
      if (opts.errorsOnly) {
        if (summary.errors > 0) printFileSummary(summary, opts.verbose, opts.check, opts.why, opts.deep);
      } else {
        printFileSummary(summary, opts.verbose, opts.check, opts.why, opts.deep);
      }
    }

    // Fail-fast: stop accepting new results after first error file
    if (opts.failFast && summary.errors > 0) {
      hitFailFast = true;
      break;
    }
  }

  // Clear progress line if we wrote one
  if (isTTY) {
    Deno.stdout.writeSync(encoder.encode("\r\x1b[K"));
  }

  if (showHuman && opts.summaryOnly) {
    for (const s of summaries) {
      const status =
        s.errors === 0
          ? `${COLORS.green}OK${COLORS.reset}`
          : `${COLORS.red}FAIL${COLORS.reset}`;
      console.log(`${status}  ${s.file}  (${s.errors}E ${s.warnings}W ${s.infos}I)`);
    }
  }

  if (showHuman && hitFailFast) {
    console.log(
      `\n${COLORS.yellow}Stopped early (--fail-fast): ${files.length - summaries.length} file(s) skipped.${COLORS.reset}`,
    );
  }

  return summaries;
}

// ---------------------------------------------------------------------------
// runLint — full lint orchestration
// ---------------------------------------------------------------------------

/**
 * Run the lint command — discover files, dispatch to handlers, collect and
 * report results. Handles both explicit-format and auto-detect modes.
 *
 * "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
 */
export async function runLint(opts: CliOptions, db?: DbHandle | null): Promise<boolean> {
  if (opts.targets.length === 0) {
    console.error(
      `${COLORS.red}No targets specified. Provide files or directories to lint.${COLORS.reset}`,
    );
    return false;
  }

  const showHuman = !opts.json;
  const instanceName = db ? (await getDbModule()).detectInstance().name : "";

  // ── Diff-aware: get changed files from git ──────────────────────
  // --since implies --changed behavior
  const isDiffAware = opts.changed || opts.staged || opts.since !== undefined;
  let changedSet: Set<string> | undefined;

  if (isDiffAware) {
    const changedFiles = await getChangedFiles({
      staged: opts.staged,
      since: opts.since,
      targets: opts.targets,
    });

    if (changedFiles.length === 0) {
      if (showHuman) {
        console.log(`${COLORS.green}No changed files to lint.${COLORS.reset}`);
      }
      if (opts.json) {
        emitJson([], TOOL_NAME, VERSION, opts.why);
      }
      return true;
    }

    changedSet = new Set(changedFiles);
    if (showHuman) {
      console.log(
        `${COLORS.dim}${changedFiles.length} file${changedFiles.length === 1 ? "" : "s"} changed${opts.staged ? " (staged)" : opts.since ? ` (since ${opts.since})` : " (since HEAD)"}${COLORS.reset}`,
      );
    }
  }

  // Identify explicit file targets (not directories) for skip-upgrade logic.
  // When a user points at a specific file, structure/skip becomes a warning.
  const explicitFiles = await identifyExplicitFiles(opts.targets);

  // ── Explicit format: lint with that handler ────────────────────
  if (opts.format) {
    const handler = getFormat(opts.format);
    if (!handler) {
      console.error(`${COLORS.red}Unknown format: ${opts.format}${COLORS.reset}`);
      console.error(`Run '${TOOL_NAME} formats' to see available formats.`);
      return false;
    }

    let files = await discoverFiles(opts.targets, handler);

    // Filter to changed files only when diff-aware
    if (changedSet) {
      const totalDiscovered = files.length;
      files = files.filter((f) => changedSet!.has(f));
      if (showHuman && files.length < totalDiscovered) {
        console.log(
          `${COLORS.dim}Linting ${files.length} of ${totalDiscovered} ${handler.name} files${COLORS.reset}`,
        );
      }
    }

    if (files.length === 0) {
      if (changedSet) {
        if (showHuman) console.log(`${COLORS.green}No changed ${handler.name} files to lint.${COLORS.reset}`);
        return true;
      }
      console.error(
        `${COLORS.red}No ${handler.extensions.join("/")} files found in specified targets.${COLORS.reset}`,
      );
      return false;
    }

    debugDiscovery(handler.name, files.length);

    // ── Inspect mode: show parsed structure, no lint checks ──
    if (opts.inspect) {
      return runInspect(handler, files, opts.json);
    }

    if (showHuman) printHeader(TOOL_NAME, VERSION, files.length, handler.description);
    if (opts.logPath) await writeLogHeader(opts.logPath, handler.name, files.length, VERSION);
    const summaries = await lintWithHandler(opts, handler, files, explicitFiles);
    if (opts.logPath) await emitHealthLog(opts.logPath, summaries, handler.name);
    if (db) recordToDatabase(db, summaries, handler.name, instanceName);
    if (showHuman) {
      printTotals(summaries);
      // Quick-fix suggestions — explicit (--suggest) or compact summary (always-on when errors exist)
      const suggestions = computeQuickFixes(summaries, opts.suggest ?? 5);
      if (opts.suggest !== undefined) {
        printQuickFixes(suggestions);
      } else if (summaries.some((s) => s.errors > 0) && summaries.length > 1) {
        printSuggestSummary(suggestions);
      }
    }
    if (opts.json) emitJson(summaries, TOOL_NAME, VERSION, opts.why);
    return summaries.every((s) => s.errors === 0);
  }

  // ── No format: auto-detect — single walk, dispatch by extension ──
  const filesByFormat = await discoverAllFiles(opts.targets);
  const allSummaries: LintSummary[] = [];
  let totalFiles = 0;

  for (const [formatName, formatFiles] of filesByFormat) {
    const handler = getFormat(formatName)!;

    // Filter to changed files when diff-aware
    const files = changedSet
      ? formatFiles.filter((f) => changedSet!.has(f))
      : formatFiles;

    if (files.length === 0) continue; // Skip formats with no changed files

    totalFiles += files.length;

    // ── Inspect mode: show parsed structure per format, no lint checks ──
    if (opts.inspect) {
      if (handler.inspect) {
        if (showHuman) {
          console.log(
            `\n${COLORS.bold}── ${handler.name}${COLORS.reset} (${files.length} file${files.length > 1 ? "s" : ""})`,
          );
        }
        await runInspect(handler, files, opts.json);
      }
      continue;
    }

    if (showHuman) {
      console.log(
        `\n${COLORS.bold}── ${handler.name}${COLORS.reset} (${files.length} file${files.length > 1 ? "s" : ""}) — ${handler.description}`,
      );
    }

    if (opts.logPath) await writeLogHeader(opts.logPath, handler.name, files.length, VERSION);
    const summaries = await lintWithHandler(opts, handler, files, explicitFiles);
    if (opts.logPath) await emitHealthLog(opts.logPath, summaries, handler.name);
    if (db) recordToDatabase(db, summaries, handler.name, instanceName);
    allSummaries.push(...summaries);
  }

  if (totalFiles === 0) {
    console.error(
      `${COLORS.red}No files matched any registered format in specified targets.${COLORS.reset}`,
    );
    console.error(`Run '${TOOL_NAME} formats' to see available formats.`);
    return false;
  }

  if (showHuman) {
    printTotals(allSummaries);
    const suggestions = computeQuickFixes(allSummaries, opts.suggest ?? 5);
    if (opts.suggest !== undefined) {
      printQuickFixes(suggestions);
    } else if (allSummaries.some((s) => s.errors > 0) && allSummaries.length > 1) {
      printSuggestSummary(suggestions);
    }
  }
  if (opts.json) emitJson(allSummaries, TOOL_NAME, VERSION, opts.why);
  return allSummaries.every((s) => s.errors === 0);
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Lint orchestration — discover, dispatch, collect, report.
// Engine does the work. This file connects the pieces.
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
