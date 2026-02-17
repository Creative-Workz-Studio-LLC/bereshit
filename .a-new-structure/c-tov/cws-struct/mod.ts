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
//   cws-struct lint [format] <targets...> [options]
//   cws-struct transform [format] <targets...> [--dry-run]
//   cws-struct formats                          # list registered formats
//   cws-struct help
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { relative } from "@std/path";
import type { CliOptions, LintSummary } from "./lib/types.ts";
import { summarize } from "./lib/types.ts";
import { discoverFiles } from "./lib/discovery.ts";
import {
  COLORS,
  printFileSummary,
  printTotals,
  printHeader,
} from "./lib/output.ts";
import {
  getFormat,
  listFormatDetails,
  detectFormat,
} from "./lib/formats/registry.ts";
import { verifyEnvironment } from "./lib/verify/env.ts";

// Register all format handlers (side-effect imports)
import "./lib/formats/toml.ts";
import "./lib/formats/omni.ts";
import "./lib/formats/ofd.ts";
import "./lib/formats/json.ts";
import "./lib/formats/go.ts";
import "./lib/formats/makefile.ts";
import "./lib/formats/dotfiles.ts";
import "./lib/formats/rust.ts";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const VERSION = "0.1.0";
const TOOL_NAME = "cws-struct";

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
  ${TOOL_NAME} lint [format] <targets...> [options]
  ${TOOL_NAME} transform [format] <targets...> [--dry-run]
  ${TOOL_NAME} verify env [--verbose]
  ${TOOL_NAME} formats
  ${TOOL_NAME} help

${COLORS.bold}Commands:${COLORS.reset}
  lint        Validate structural alignment
  transform   Transform files to aligned structure
  verify env  Check development environment tools and versions
  formats     List registered format handlers
  help        Show this help

${COLORS.bold}Options:${COLORS.reset}
  --verbose, -v     Show all results (including info)
  --errors-only     Show only errors
  --summary         Show only file-level summary
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
  ${TOOL_NAME} lint toml word/core/types/
  ${TOOL_NAME} lint .                        # auto-detect formats
  ${TOOL_NAME} lint toml . --summary
  ${TOOL_NAME} transform toml file.toml --dry-run
  ${TOOL_NAME} verify env                    # check all dev tools
  ${TOOL_NAME} verify env --verbose          # include optional tools
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

async function runLint(opts: CliOptions): Promise<boolean> {
  const handler = opts.format ? getFormat(opts.format) : undefined;

  if (opts.format && !handler) {
    console.error(`${COLORS.red}Unknown format: ${opts.format}${COLORS.reset}`);
    console.error(`Run '${TOOL_NAME} formats' to see available formats.`);
    return false;
  }

  if (!handler) {
    console.error(
      `${COLORS.red}No format specified. Use '${TOOL_NAME} lint <format> <target>'${COLORS.reset}`,
    );
    console.error(`Run '${TOOL_NAME} formats' to see available formats.`);
    return false;
  }

  if (opts.targets.length === 0) {
    console.error(
      `${COLORS.red}No targets specified. Provide files or directories to lint.${COLORS.reset}`,
    );
    return false;
  }

  // Discover files
  const files = await discoverFiles(opts.targets, handler);

  if (files.length === 0) {
    console.error(
      `${COLORS.red}No ${handler.extensions.join("/")} files found in specified targets.${COLORS.reset}`,
    );
    return false;
  }

  // Print header
  printHeader(TOOL_NAME, VERSION, files.length, handler.description);

  // Lint each file
  const cwd = Deno.cwd();
  const summaries: LintSummary[] = [];

  for (const file of files) {
    const results = await handler.lint(file);
    const summary = summarize(relative(cwd, file), results);
    summaries.push(summary);

    if (!opts.summaryOnly) {
      if (opts.errorsOnly) {
        if (summary.errors > 0) printFileSummary(summary, opts.verbose);
      } else {
        printFileSummary(summary, opts.verbose);
      }
    }
  }

  // Summary-only mode: compact listing
  if (opts.summaryOnly) {
    for (const s of summaries) {
      const status =
        s.errors === 0
          ? `${COLORS.green}OK${COLORS.reset}`
          : `${COLORS.red}FAIL${COLORS.reset}`;
      console.log(`${status}  ${s.file}  (${s.errors}E ${s.warnings}W ${s.infos}I)`);
    }
  }

  printTotals(summaries);

  return summaries.every((s) => s.errors === 0);
}

async function runTransform(opts: CliOptions): Promise<boolean> {
  const handler = opts.format ? getFormat(opts.format) : undefined;

  if (!handler?.transform) {
    console.error(
      `${COLORS.red}Transform not available for format: ${opts.format ?? "none"}${COLORS.reset}`,
    );
    return false;
  }

  if (opts.targets.length === 0) {
    console.error(
      `${COLORS.red}No targets specified.${COLORS.reset}`,
    );
    return false;
  }

  const files = await discoverFiles(opts.targets, handler);

  if (files.length === 0) {
    console.error(
      `${COLORS.red}No matching files found.${COLORS.reset}`,
    );
    return false;
  }

  console.log(
    `${COLORS.bold}Transforming ${files.length} file(s)${opts.dryRun ? " (dry run)" : ""}...${COLORS.reset}\n`,
  );

  const cwd = Deno.cwd();
  let errors = 0;

  for (const file of files) {
    const results = await handler.transform(file, {
      dryRun: opts.dryRun,
      extensions: opts.extensions,
    });
    const summary = summarize(relative(cwd, file), results);
    printFileSummary(summary, opts.verbose);
    errors += summary.errors;
  }

  return errors === 0;
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// Main — dispatch
// ---------------------------------------------------------------------------

async function main(): Promise<void> {
  const opts = parseArgs(Deno.args);

  switch (opts.command) {
    case "help":
      showHelp();
      Deno.exit(0);
      break;

    case "version":
      console.log(`${TOOL_NAME} v${VERSION}`);
      Deno.exit(0);
      break;

    case "lint": {
      const ok = await runLint(opts);
      Deno.exit(ok ? 0 : 1);
      break;
    }

    case "transform": {
      const ok = await runTransform(opts);
      Deno.exit(ok ? 0 : 1);
      break;
    }

    case "verify": {
      const ok = await verifyEnvironment(opts.verbose);
      Deno.exit(ok ? 0 : 1);
      break;
    }

    case "formats":
      showFormats();
      Deno.exit(0);
      break;
  }
}

main();
