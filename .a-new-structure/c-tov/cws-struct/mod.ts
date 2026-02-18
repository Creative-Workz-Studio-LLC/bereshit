#!/usr/bin/env -S deno run --allow-read
// ============================================================================
// METADATA
// ============================================================================
//
// file:    mod.ts
// key:     B-tov-cws-struct-mod
// title:   CWS Struct — CLI Entrypoint
// type:    Code (Executable)
// version: a-01.00
// created: 2026-02-11
// authors: Nova Dawn (CPI-SI)
// purpose: Unified CLI for structural alignment — lint and transform files
//          across all OmniCode formats. Dispatches to registered format
//          handlers. Replaces 8 standalone Deno scripts with one expandable tool.
//
// biblical_foundation: "Except the LORD build the house, they labour in
//   vain that build it." — Psalm 127:1
//
// usage:
//   cws-struct <operation> [format] <targets...> [options]
//   cws-struct lint [format] <targets...>       # validate (format auto-detected)
//   cws-struct transform [format] <targets...>  # fix (format auto-detected)
//   cws-struct verify env [--verbose]           # check dev environment
//   cws-struct formats                          # list registered formats
//   cws-struct help
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { relative } from "@std/path";
import { pooledMap } from "@std/async/pool";
import type { CliOptions, LintSummary } from "./lib/foundation/mod.ts";
import { summarize } from "./lib/foundation/mod.ts";
import { discoverFiles, discoverAllFiles } from "./lib/engine/mod.ts";
import {
  COLORS,
  printFileSummary,
  printTotals,
  printHeader,
} from "./lib/engine/mod.ts";
import {
  getFormat,
  listFormatDetails,
} from "./lib/engine/mod.ts";
import { verifyEnvironment } from "./lib/verify/env.ts";

// Register all format handlers (side-effect imports)
import "./lib/handlers/toml.ts";
import "./lib/handlers/omni.ts";
import "./lib/handlers/ofd.ts";
import "./lib/handlers/json.ts";
import "./lib/handlers/go.ts";
import "./lib/handlers/makefile.ts";
import "./lib/handlers/dotfiles.ts";
import "./lib/handlers/rust.ts";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const VERSION = "0.1.0";
const TOOL_NAME = "cws-struct";

/** Bounded concurrency for parallel file linting/transform. */
const LINT_CONCURRENCY = 8;

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// CLI parsing
// ---------------------------------------------------------------------------

function parseArgs(args: string[]): CliOptions {
  if (args.length === 0 || args.includes("--help") || args.includes("-h")) {
    return {
      command: "help",
      targets: [],
      verbose: false,
      errorsOnly: false,
      summaryOnly: false,
      dryRun: false,
      extensions: false,
      json: false,
      failFast: false,
    };
  }

  if (args.includes("--version")) {
    return {
      command: "version",
      targets: [],
      verbose: false,
      errorsOnly: false,
      summaryOnly: false,
      dryRun: false,
      extensions: false,
      json: false,
      failFast: false,
    };
  }

  const command = args[0] as CliOptions["command"];
  const rest = args.slice(1);

  // Check if second arg is a format name
  let format: string | undefined;
  const nonFlags = rest.filter((a) => !a.startsWith("-"));
  if (nonFlags.length > 0) {
    // Try to match first non-flag as format name
    const candidate = nonFlags[0]!;
    if (getFormat(candidate)) {
      format = candidate;
    } else if (!candidate.includes("/") && !candidate.includes("\\") && !candidate.includes(".")) {
      // Looks like a format name (no path separators or extensions) but isn't registered.
      // Treat as unknown format so runLint/runTransform can report the error.
      format = candidate;
    }
  }

  const targets = nonFlags.filter((a) => a !== format);

  return {
    command: (command === "lint" || command === "transform" || command === "formats" || command === "verify") ? command : "help",
    format,
    targets,
    verbose: rest.includes("--verbose") || rest.includes("-v"),
    errorsOnly: rest.includes("--errors-only"),
    summaryOnly: rest.includes("--summary"),
    dryRun: rest.includes("--dry-run"),
    extensions: rest.includes("--extensions"),
    json: rest.includes("--json"),
    failFast: rest.includes("--fail-fast"),
  };
}

// ---------------------------------------------------------------------------
// Commands
// ---------------------------------------------------------------------------

function showHelp(): void {
  const formats = listFormatDetails();

  console.log(`
${COLORS.bold}${TOOL_NAME}${COLORS.reset} v${VERSION} — Structural Alignment Tool

${COLORS.dim}"Except the LORD build the house, they labour in vain that build it."${COLORS.reset}
${COLORS.dim}— Psalm 127:1${COLORS.reset}

${COLORS.bold}Usage:${COLORS.reset}
  ${TOOL_NAME} <operation> [format] <targets...> [options]

${COLORS.bold}Operations:${COLORS.reset}
  lint        Validate structural alignment
  transform   Transform files to aligned structure
  verify env  Check development environment tools and versions
  formats     List registered format handlers
  help        Show this help

  ${COLORS.dim}Format is optional — omit it to auto-detect from file extensions.${COLORS.reset}
  ${COLORS.dim}When specified, only files matching that format are processed.${COLORS.reset}

${COLORS.bold}Options:${COLORS.reset}
  --verbose, -v     Show all results (including info)
  --errors-only     Show only errors
  --summary         Show only file-level summary
  --json            Machine-readable JSON output (suppresses human output)
  --fail-fast       Stop on first file with errors
  --dry-run         Preview transforms without writing
  --extensions      Also scaffold extension sections (I4, C5-C7, X2-X4, etc.)
  --help, -h        Show this help
  --version         Show version

${COLORS.bold}Registered formats:${COLORS.reset}`);

  for (const f of formats) {
    console.log(`  ${COLORS.bold}${f.name}${COLORS.reset}  ${f.extensions.join(", ")}  — ${f.description}`);
  }

  console.log(`
${COLORS.bold}Examples:${COLORS.reset}
  ${TOOL_NAME} lint .                        ${COLORS.dim}# auto-detect all formats${COLORS.reset}
  ${TOOL_NAME} lint rust src/                ${COLORS.dim}# lint only Rust files${COLORS.reset}
  ${TOOL_NAME} lint toml word/core/types/    ${COLORS.dim}# lint only TOML files${COLORS.reset}
  ${TOOL_NAME} lint toml . --summary         ${COLORS.dim}# summary view${COLORS.reset}
  ${TOOL_NAME} lint . --json                 ${COLORS.dim}# machine-readable JSON output${COLORS.reset}
  ${TOOL_NAME} lint . --fail-fast            ${COLORS.dim}# stop on first error${COLORS.reset}
  ${TOOL_NAME} transform .                   ${COLORS.dim}# auto-fix all formats${COLORS.reset}
  ${TOOL_NAME} transform rust src/ --dry-run ${COLORS.dim}# preview Rust fixes${COLORS.reset}
  ${TOOL_NAME} verify env                    ${COLORS.dim}# check dev tools${COLORS.reset}
  ${TOOL_NAME} verify env --verbose          ${COLORS.dim}# include optional tools${COLORS.reset}
`);
}

function showFormats(): void {
  const formats = listFormatDetails();
  console.log(`${COLORS.bold}Registered formats:${COLORS.reset}\n`);
  for (const f of formats) {
    console.log(`  ${COLORS.bold}${f.name}${COLORS.reset}`);
    console.log(`    Extensions: ${f.extensions.join(", ")}`);
    console.log(`    ${f.description}\n`);
  }
}

/**
 * Lint files for a single format handler.
 * Extracted so both explicit-format and auto-detect paths share the same logic.
 *
 * Uses bounded concurrency (LINT_CONCURRENCY) to lint files in parallel while
 * capping open file handles. Results stream in order via pooledMap.
 *
 * Supports:
 * - --json: Suppress human output (caller handles JSON serialization)
 * - --fail-fast: Stop after first file with errors (current batch completes)
 * - Progress counter: [N/total] when stdout is a TTY (not in json/piped mode)
 */
async function lintWithHandler(
  opts: CliOptions,
  handler: import("./lib/foundation/mod.ts").FormatHandler,
  files: string[],
): Promise<LintSummary[]> {
  const cwd = Deno.cwd();
  const summaries: LintSummary[] = [];
  const showHuman = !opts.json;
  const isTTY = showHuman && Deno.stdout.isTerminal();
  const encoder = new TextEncoder();
  let hitFailFast = false;

  const pool = pooledMap(LINT_CONCURRENCY, files, async (file) => {
    const results = await handler.lint(file);
    const health = handler.computeHealth
      ? await handler.computeHealth(file, results)
      : undefined;
    return summarize(relative(cwd, file), results, health);
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
        if (summary.errors > 0) printFileSummary(summary, opts.verbose);
      } else {
        printFileSummary(summary, opts.verbose);
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

/**
 * Emit lint results as structured JSON to stdout.
 * Machine-readable output for CI/CD pipelines and tooling integration.
 */
function emitJson(summaries: LintSummary[]): void {
  let totalErrors = 0, totalWarnings = 0, totalInfos = 0;
  for (const s of summaries) {
    totalErrors += s.errors;
    totalWarnings += s.warnings;
    totalInfos += s.infos;
  }

  const output = {
    tool: TOOL_NAME,
    version: VERSION,
    files: summaries.map((s) => ({
      file: s.file,
      errors: s.errors,
      warnings: s.warnings,
      infos: s.infos,
      health: s.health ?? null,
      results: s.results.map((r) => ({
        severity: r.severity,
        rule: r.rule,
        message: r.message,
        line: r.line ?? null,
        fix: r.fix ?? null,
      })),
    })),
    totals: {
      files: summaries.length,
      errors: totalErrors,
      warnings: totalWarnings,
      infos: totalInfos,
    },
  };

  console.log(JSON.stringify(output, null, 2));
}

async function runLint(opts: CliOptions): Promise<boolean> {
  if (opts.targets.length === 0) {
    console.error(
      `${COLORS.red}No targets specified. Provide files or directories to lint.${COLORS.reset}`,
    );
    return false;
  }

  const showHuman = !opts.json;

  // ── Explicit format: lint with that handler ────────────────────
  if (opts.format) {
    const handler = getFormat(opts.format);
    if (!handler) {
      console.error(`${COLORS.red}Unknown format: ${opts.format}${COLORS.reset}`);
      console.error(`Run '${TOOL_NAME} formats' to see available formats.`);
      return false;
    }

    const files = await discoverFiles(opts.targets, handler);
    if (files.length === 0) {
      console.error(
        `${COLORS.red}No ${handler.extensions.join("/")} files found in specified targets.${COLORS.reset}`,
      );
      return false;
    }

    if (showHuman) printHeader(TOOL_NAME, VERSION, files.length, handler.description);
    const summaries = await lintWithHandler(opts, handler, files);
    if (showHuman) printTotals(summaries);
    if (opts.json) emitJson(summaries);
    return summaries.every((s) => s.errors === 0);
  }

  // ── No format: auto-detect — single walk, dispatch by extension ──
  const filesByFormat = await discoverAllFiles(opts.targets);
  const allSummaries: LintSummary[] = [];
  let totalFiles = 0;

  for (const [formatName, files] of filesByFormat) {
    const handler = getFormat(formatName)!;
    totalFiles += files.length;

    if (showHuman) {
      console.log(
        `\n${COLORS.bold}── ${handler.name}${COLORS.reset} (${files.length} file${files.length > 1 ? "s" : ""}) — ${handler.description}`,
      );
    }

    const summaries = await lintWithHandler(opts, handler, files);
    allSummaries.push(...summaries);
  }

  if (totalFiles === 0) {
    console.error(
      `${COLORS.red}No files matched any registered format in specified targets.${COLORS.reset}`,
    );
    console.error(`Run '${TOOL_NAME} formats' to see available formats.`);
    return false;
  }

  if (showHuman) printTotals(allSummaries);
  if (opts.json) emitJson(allSummaries);
  return allSummaries.every((s) => s.errors === 0);
}

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
  handler: import("./lib/foundation/mod.ts").FormatHandler,
  files: string[],
): Promise<LintSummary[]> {
  if (!handler.transform) return [];

  const cwd = Deno.cwd();
  const summaries: LintSummary[] = [];
  const transformFn = handler.transform.bind(handler);

  const pool = pooledMap(LINT_CONCURRENCY, files, async (file) => {
    const results = await transformFn(file, {
      dryRun: opts.dryRun,
      extensions: opts.extensions,
    });
    return summarize(relative(cwd, file), results);
  });

  for await (const summary of pool) {
    summaries.push(summary);
    printFileSummary(summary, opts.verbose);
  }

  return summaries;
}

async function runTransform(opts: CliOptions): Promise<boolean> {
  if (opts.targets.length === 0) {
    console.error(
      `${COLORS.red}No targets specified. Provide files or directories to transform.${COLORS.reset}`,
    );
    return false;
  }

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
    const summaries = await transformWithHandler(opts, handler, files);
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

    const summaries = await transformWithHandler(opts, handler, files);
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

// ---------------------------------------------------------------------------
// Exit codes — consistent, documented
// ---------------------------------------------------------------------------

/** Exit code 0: Clean run, no structural errors found. */
const EXIT_OK = 0;

/** Exit code 1: Structural errors found in target files. */
const EXIT_LINT_ERRORS = 1;

/** Exit code 2: Tool error — CLI misuse, I/O failure, or unhandled exception. */
const EXIT_TOOL_ERROR = 2;

// ---------------------------------------------------------------------------
// Input validation
// ---------------------------------------------------------------------------

/** Known CLI flags — anything else starting with "--" or "-" is suspicious. */
const KNOWN_FLAGS = new Set([
  "--verbose", "-v",
  "--errors-only",
  "--summary",
  "--dry-run",
  "--extensions",
  "--json",
  "--fail-fast",
  "--help", "-h",
  "--version",
]);

/**
 * Validate CLI arguments. Returns an error message if invalid, undefined if clean.
 * Guards against obvious misuse before dispatching to commands.
 */
function validateArgs(args: string[]): string | undefined {
  for (const arg of args) {
    // Reject excessively long arguments (path or otherwise) — 4096 is generous
    if (arg.length > 4096) {
      return `Argument too long (${arg.length} chars, max 4096)`;
    }

    // Warn on unknown flags (but don't block — could be future flags)
    // The parseArgs function already ignores unknown flags, this is belt-and-suspenders
    if ((arg.startsWith("--") || (arg.startsWith("-") && arg.length === 2)) && !KNOWN_FLAGS.has(arg)) {
      // Check if it looks like a known flag with a typo
      const command = args[0];
      if (command !== "help" && command !== "version" && command !== "formats") {
        console.error(
          `${COLORS.yellow}Warning: Unknown flag "${arg}" — ignoring.${COLORS.reset}`,
        );
      }
    }

    // Reject null bytes in arguments — prevents path injection via null terminators
    if (arg.includes("\0")) {
      return `Invalid argument: contains null byte`;
    }
  }
  return undefined;
}

// ---------------------------------------------------------------------------
// Main — dispatch with error boundary
// ---------------------------------------------------------------------------

async function main(): Promise<void> {
  // Input validation
  const validationError = validateArgs(Deno.args);
  if (validationError) {
    console.error(`${COLORS.red}${TOOL_NAME}: ${validationError}${COLORS.reset}`);
    Deno.exit(EXIT_TOOL_ERROR);
  }

  const opts = parseArgs(Deno.args);

  switch (opts.command) {
    case "help":
      showHelp();
      Deno.exit(EXIT_OK);
      break;

    case "version":
      console.log(`${TOOL_NAME} v${VERSION}`);
      Deno.exit(EXIT_OK);
      break;

    case "lint": {
      const ok = await runLint(opts);
      Deno.exit(ok ? EXIT_OK : EXIT_LINT_ERRORS);
      break;
    }

    case "transform": {
      const ok = await runTransform(opts);
      Deno.exit(ok ? EXIT_OK : EXIT_LINT_ERRORS);
      break;
    }

    case "verify": {
      const ok = await verifyEnvironment(opts.verbose);
      Deno.exit(ok ? EXIT_OK : EXIT_LINT_ERRORS);
      break;
    }

    case "formats":
      showFormats();
      Deno.exit(EXIT_OK);
      break;
  }
}

// Top-level error boundary — no unhandled exceptions crash the CLI
main().catch((err: unknown) => {
  console.error(
    `${COLORS.red}${TOOL_NAME}: Unexpected error: ${err instanceof Error ? err.message : String(err)}${COLORS.reset}`,
  );
  if (err instanceof Error && err.stack) {
    console.error(`${COLORS.dim}${err.stack}${COLORS.reset}`);
  }
  Deno.exit(EXIT_TOOL_ERROR);
});
