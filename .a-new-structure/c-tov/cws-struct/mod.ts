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
import type { CliOptions, LintPolicy, LintSummary, ToolConfig, FormatHandler, InspectResult, LintLayer, BlockName, LintGridOptions } from "./lib/foundation/mod.ts";
import { summarize, setGlobalPolicy, setLintFocus, setImpactWeights, loadConfig } from "./lib/foundation/mod.ts";
import { discoverFiles, discoverAllFiles, setDiscoveryLimits } from "./lib/engine/mod.ts";
import {
  COLORS,
  printFileSummary,
  printTotals,
  printHeader,
  resolveErrorCodes,
} from "./lib/engine/mod.ts";
import {
  getFormat,
  listFormatDetails,
} from "./lib/engine/mod.ts";
import { generateFileText } from "./lib/engine/mod.ts";
import type { FillContext } from "./lib/engine/mod.ts";
import { transpileFile } from "./lib/engine/mod.ts";
import { computeQuickFixes, printQuickFixes, printSuggestSummary } from "./lib/engine/mod.ts";
import { matchRule } from "./lib/data/errors.ts";
import type { CodeFormat } from "./lib/foundation/mod.ts";
import { writeLogEntries, writeLogHeader, writeTransformLogEntries } from "./lib/engine/logging.ts";
import { formatInspectText, formatInspectJson } from "./lib/engine/inspect.ts";
import {
  initDebug,
  debugConfig,
  debugPolicy,
  debugDiscovery,
  debugFileStart,
  debugFileEnd,
  debugHealth,
} from "./lib/engine/debug.ts";
import { verifyEnvironment } from "./lib/verify/env.ts";
import { startStudio } from "./lib/studio/serve.ts";
import { basename, dirname } from "@std/path";
// Data layer: lazy-loaded to avoid triggering SQLite FFI when not needed.
// The import() is called only when --db flag or query command is used.
// This prevents `cws-struct help` from requiring --allow-ffi --allow-env.
let _dbModule: typeof import("./lib/data/database.ts") | null = null;
async function getDbModule() {
  if (!_dbModule) {
    _dbModule = await import("./lib/data/database.ts");
  }
  return _dbModule;
}

import { trueToLevel, levelToEmoji } from "./lib/foundation/mod.ts";

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

let VERSION = "0.1.0";
const TOOL_NAME = "cws-struct";

/** Bounded concurrency for parallel file linting/transform. Config-driven. */
let LINT_CONCURRENCY = 8;

// ---------------------------------------------------------------------------
// Explicit file detection
// ---------------------------------------------------------------------------

/**
 * Identify which CLI targets are explicit files (not directories).
 *
 * When a user runs `cws-struct lint go path/to/file.go`, they explicitly
 * chose that file. If it has no structural markers, that's a warning — not
 * just a silent skip. Discovery (directory walks) can silently skip; explicit
 * targets should not.
 *
 * Returns resolved absolute paths so they match what discovery returns.
 */
async function identifyExplicitFiles(targets: string[]): Promise<Set<string>> {
  const explicit = new Set<string>();
  for (const target of targets) {
    try {
      const resolved = await Deno.realPath(target);
      const stat = await Deno.stat(resolved);
      if (stat.isFile) {
        explicit.add(resolved);
      }
    } catch {
      // If we can't stat/resolve, discovery will report the error
    }
  }
  return explicit;
}

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
      force: false,
      steps: false,
      policy: "balanced",
      debug: false,
      trace: false,
      inspect: false,
      deep: false,
      why: false,
      changed: false,
      staged: false,
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
      force: false,
      steps: false,
      policy: "balanced",
      debug: false,
      trace: false,
      inspect: false,
      deep: false,
      why: false,
      changed: false,
      staged: false,
    };
  }

  const command = args[0] as CliOptions["command"];
  const rest = args.slice(1);

  // Check if second arg is a format name
  let format: string | undefined;

  // Identify indices consumed as values of value-taking flags (--key X, --policy X, etc.)
  // so they don't leak into nonFlags → targets.
  const valueTakingFlags = new Set(["--key", "--title", "--purpose", "--policy", "--port", "--log", "--check", "--db", "--limit", "--since", "--suggest"]);
  const consumedIndices = new Set<number>();
  for (let i = 0; i < rest.length; i++) {
    if (valueTakingFlags.has(rest[i]!) && i + 1 < rest.length) {
      consumedIndices.add(i + 1);
    }
  }
  const nonFlags = rest.filter((a, i) => !a.startsWith("-") && !consumedIndices.has(i));
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

  // Extract --key, --title, --purpose, --policy values
  const keyIdx = rest.indexOf("--key");
  const titleIdx = rest.indexOf("--title");
  const purposeIdx = rest.indexOf("--purpose");
  const policyIdx = rest.indexOf("--policy");
  const logIdx = rest.indexOf("--log");
  const checkIdx = rest.indexOf("--check");
  const dbIdx = rest.indexOf("--db");
  const sinceIdx = rest.indexOf("--since");

  const keyVal = keyIdx >= 0 ? rest[keyIdx + 1] : undefined;
  const titleVal = titleIdx >= 0 ? rest[titleIdx + 1] : undefined;
  const purposeVal = purposeIdx >= 0 ? rest[purposeIdx + 1] : undefined;
  const policyRaw = policyIdx >= 0 ? rest[policyIdx + 1] : undefined;
  const logPath = logIdx >= 0 ? rest[logIdx + 1] : undefined;
  const check = checkIdx >= 0 ? rest[checkIdx + 1] : undefined;
  const dbPath = dbIdx >= 0 ? rest[dbIdx + 1] : undefined;
  const sinceVal = sinceIdx >= 0 ? rest[sinceIdx + 1] : undefined;
  const suggestIdx = rest.indexOf("--suggest");
  const suggestRaw = suggestIdx >= 0 ? rest[suggestIdx + 1] : undefined;
  // --suggest with no value or non-numeric value = default (5), numeric = that count
  const suggest = suggestIdx >= 0
    ? (suggestRaw && /^\d+$/.test(suggestRaw) ? parseInt(suggestRaw, 10) : 5)
    : undefined;
  const policy: LintPolicy = policyRaw === "strict" || policyRaw === "growth"
    ? policyRaw
    : "balanced";

  // For create command: args are `create <format> <subtype> <dest>`
  // format is already extracted above; subtype is the next non-flag after format
  let subtype: string | undefined;
  if (command === "create" && format) {
    const afterFormat = nonFlags.filter((a) => a !== format);
    // First non-path arg is subtype, rest are targets
    if (afterFormat.length >= 2) {
      const candidateSubtype = afterFormat[0]!;
      if (!candidateSubtype.includes("/") && !candidateSubtype.includes("\\") && !candidateSubtype.includes(".")) {
        subtype = candidateSubtype;
      }
    }
  }

  // Filter subtype from targets for create command
  const finalTargets = command === "create" && subtype
    ? targets.filter((t) => t !== subtype)
    : targets;

  // For query command: first non-flag after "query" is the subcommand
  // e.g., `cws-struct query health path/to/file.go`
  //        command="query", queryCommand="health", targets=["path/to/file.go"]
  let queryCommand: string | undefined;
  let queryTargets = finalTargets;
  if (command === "query" && finalTargets.length > 0) {
    const sub = finalTargets[0]!;
    const validSubs = new Set(["health", "worst", "best", "patterns", "trend", "instance", "session", "stats"]);
    if (validSubs.has(sub)) {
      queryCommand = sub;
      queryTargets = finalTargets.slice(1);
    }
  }

  const isDebug = rest.includes("--debug");
  const isTrace = rest.includes("--trace");
  const isInspect = rest.includes("--inspect");
  const isWhy = rest.includes("--why");

  return {
    command: (command === "lint" || command === "transform" || command === "create" || command === "transpile" || command === "query" || command === "watch" || command === "formats" || command === "verify" || command === "studio" || command === "help") ? command : "help",
    helpTopic: command === "help" ? nonFlags[0] : undefined,
    format,
    targets: command === "query" ? queryTargets : finalTargets,
    verbose: rest.includes("--verbose") || rest.includes("-v") || rest.includes("-vv") || rest.includes("--deep"),
    deep: rest.includes("--deep") || rest.includes("-vv"),
    errorsOnly: rest.includes("--errors-only"),
    summaryOnly: rest.includes("--summary"),
    dryRun: rest.includes("--dry-run"),
    extensions: rest.includes("--extensions"),
    json: rest.includes("--json"),
    failFast: rest.includes("--fail-fast"),
    force: rest.includes("--force"),
    steps: rest.includes("--steps"),
    policy,
    subtype,
    key: keyVal,
    title: titleVal,
    purpose: purposeVal,
    logPath,
    check,
    dbPath,
    queryCommand,
    debug: isDebug || isTrace, // --trace implies --debug
    trace: isTrace,
    inspect: isInspect,
    why: isWhy,
    changed: rest.includes("--changed"),
    since: sinceVal,
    staged: rest.includes("--staged"),
    suggest,
  };
}

// ---------------------------------------------------------------------------
// Commands
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Help system — paginated by topic
// ---------------------------------------------------------------------------

/** Help topics — each is a self-contained page. */
const HELP_TOPICS: Record<string, () => void> = {
  lint: showHelpLint,
  transform: showHelpTransform,
  create: showHelpCreate,
  transpile: showHelpTranspile,
  query: showHelpQuery,
  options: showHelpOptions,
  layers: showHelpLayers,
  testing: showHelpTesting,
  witness: showHelpWitness,
  examples: showHelpExamples,
};

function showHelp(topic?: string): void {
  // Topic-specific help: `cws-struct help lint`, `cws-struct help testing`, etc.
  if (topic && HELP_TOPICS[topic]) {
    HELP_TOPICS[topic]!();
    return;
  }

  if (topic) {
    console.error(`${COLORS.red}Unknown help topic: ${topic}${COLORS.reset}`);
    console.error(`Available: ${Object.keys(HELP_TOPICS).join(", ")}\n`);
  }

  // Default: compact overview that fits one screen (~40 lines)
  const formats = listFormatDetails();

  console.log(`
${COLORS.bold}${TOOL_NAME}${COLORS.reset} v${VERSION} — Structural Alignment Tool
${COLORS.dim}${formats.length} formats · 252 error codes · 4 detection layers · 93 test files${COLORS.reset}

${COLORS.dim}"Except the LORD build the house, they labour in vain that build it." — Psalm 127:1${COLORS.reset}

${COLORS.bold}Usage:${COLORS.reset}
  ${TOOL_NAME} <command> [format] <targets...> [options]
  ${COLORS.dim}Format is optional — omit to auto-detect from file extensions.${COLORS.reset}

${COLORS.bold}Commands:${COLORS.reset}
  lint             Validate structural alignment (4-layer analysis)
  transform        Transform files to aligned structure (auto-fix)
  create           Generate new file from schema (schema-driven fill)
  transpile        Transpile between formats (e.g., Rust → Go)
  watch            Monitor directories, re-lint on change
  query            Query health database (requires --db)
  verify env       Check development environment
  formats          List registered format handlers
  studio           Launch CWS Studio web interface

${COLORS.bold}Quick start:${COLORS.reset}
  ${TOOL_NAME} lint .                     ${COLORS.dim}# lint everything (auto-detect)${COLORS.reset}
  ${TOOL_NAME} lint rust src/ --verbose   ${COLORS.dim}# lint Rust files with detail${COLORS.reset}
  ${TOOL_NAME} lint . --suggest           ${COLORS.dim}# lint + quick-fix suggestions${COLORS.reset}
  ${TOOL_NAME} transform . --dry-run     ${COLORS.dim}# preview auto-fixes${COLORS.reset}
  ${TOOL_NAME} create go library out.go   ${COLORS.dim}# generate from schema${COLORS.reset}
  deno task test                         ${COLORS.dim}# run full test suite${COLORS.reset}
  deno task witness:grid                 ${COLORS.dim}# error code coverage grid${COLORS.reset}

${COLORS.bold}Registered formats:${COLORS.reset}`);

  for (const f of formats) {
    console.log(`  ${COLORS.bold}${f.name.padEnd(10)}${COLORS.reset} ${f.extensions.join(", ").padEnd(16)} ${f.description}`);
  }

  console.log(`
${COLORS.bold}Help topics:${COLORS.reset}  ${COLORS.dim}(${TOOL_NAME} help <topic>)${COLORS.reset}
  lint         Lint command options and display modes
  transform    Transform and auto-fix options
  create       Schema-driven file generation
  transpile    Cross-format transpilation (Rust → Go)
  query        Health database queries (8 subcommands)
  options      All CLI flags grouped by category
  layers       Detection layer architecture (R[50] → T[00])
  testing      Test navigation (deno task test:*)
  witness      Error code coverage analysis
  examples     Full examples for every command
`);
}

function showHelpLint(): void {
  console.log(`
${COLORS.bold}${TOOL_NAME} lint${COLORS.reset} — Validate Structural Alignment

${COLORS.bold}Usage:${COLORS.reset}
  ${TOOL_NAME} lint [format] <targets...> [options]

${COLORS.bold}Display modes:${COLORS.reset}
  ${COLORS.dim}(default)${COLORS.reset}         Errors + warnings, one-line health score
  --verbose, -v     + info results, + layer breakdown
  --deep, -vv       + file anatomy (directives, blocks, identity, per-action checks)
  --errors-only     Errors only (suppress warnings/info)
  --summary         One-line-per-file (status + counts)
  --json            Machine-readable JSON (suppresses human output)
  --inspect         Parsed structure only, no lint checks

${COLORS.bold}Behavior:${COLORS.reset}
  --fail-fast       Stop after first file with errors
  --policy <P>      Lint policy: strict (-1), balanced (0, default), growth (+1)
  --check <focus>   Focus on a health grid slice:
                      ${COLORS.dim}"metadata"        vertical slice (one block)${COLORS.reset}
                      ${COLORS.dim}"R25"             horizontal slice (one layer)${COLORS.reset}
                      ${COLORS.dim}"metadata:R25"    single cell${COLORS.reset}
                      ${COLORS.dim}"setup:Imports"    specific container${COLORS.reset}
  --suggest [N]     Top N quick-fix suggestions ranked by impact (default: 5)
  --why             Trace each result to its data-layer source (reasoning chain)
  --log <path>      Write atomic health observations to log file

${COLORS.bold}Diff-aware:${COLORS.reset}
  --changed         Only lint files changed since last commit
  --since <ref>     Git ref for diff baseline (implies --changed)
  --staged          Only lint staged files (git diff --cached)

${COLORS.bold}Database:${COLORS.reset}
  --db <path>       Record results to SQLite health database

${COLORS.bold}Examples:${COLORS.reset}
  ${TOOL_NAME} lint .                              ${COLORS.dim}# auto-detect all formats${COLORS.reset}
  ${TOOL_NAME} lint rust src/                      ${COLORS.dim}# lint only Rust files${COLORS.reset}
  ${TOOL_NAME} lint go . --verbose                 ${COLORS.dim}# all results + health breakdown${COLORS.reset}
  ${TOOL_NAME} lint rust . --deep                  ${COLORS.dim}# file anatomy + per-action detail${COLORS.reset}
  ${TOOL_NAME} lint . --check metadata:R25         ${COLORS.dim}# focus on metadata organization${COLORS.reset}
  ${TOOL_NAME} lint . --json --fail-fast           ${COLORS.dim}# CI mode: JSON + stop on first error${COLORS.reset}
  ${TOOL_NAME} lint --changed .                    ${COLORS.dim}# only files changed since HEAD${COLORS.reset}
  ${TOOL_NAME} lint --staged .                     ${COLORS.dim}# only staged files${COLORS.reset}
  ${TOOL_NAME} lint . --suggest 10                 ${COLORS.dim}# top 10 quick-fix suggestions${COLORS.reset}
  ${TOOL_NAME} lint . --why                        ${COLORS.dim}# reasoning chain per result${COLORS.reset}
`);
}

function showHelpTransform(): void {
  console.log(`
${COLORS.bold}${TOOL_NAME} transform${COLORS.reset} — Auto-Fix Structural Alignment

${COLORS.bold}Usage:${COLORS.reset}
  ${TOOL_NAME} transform [format] <targets...> [options]

${COLORS.bold}Options:${COLORS.reset}
  --dry-run         Preview changes without writing files
  --extensions      Also scaffold extension sections (I4, C5-C7, X2-X4, etc.)
  --force           Re-scaffold even if file already has block structure
  --steps           Write each scaffold phase to .steps/ directory
  --verbose, -v     Show all transform results including info
  --log <path>      Write transform operations to structured log

${COLORS.bold}What transform does:${COLORS.reset}
  1. Scaffold missing block separators (METADATA, SETUP, BODY, CLOSING)
  2. Scaffold missing subsection headers within blocks
  3. Reorder out-of-order subsections to canonical position
  4. Move code found in wrong blocks to correct locations

${COLORS.bold}Examples:${COLORS.reset}
  ${TOOL_NAME} transform .                         ${COLORS.dim}# auto-fix all formats${COLORS.reset}
  ${TOOL_NAME} transform rust src/ --dry-run       ${COLORS.dim}# preview Rust fixes${COLORS.reset}
  ${TOOL_NAME} transform go . --extensions         ${COLORS.dim}# include extension sections${COLORS.reset}
  ${TOOL_NAME} transform . --steps                 ${COLORS.dim}# step-by-step scaffold output${COLORS.reset}
  ${TOOL_NAME} transform go file.go --force        ${COLORS.dim}# re-scaffold existing structure${COLORS.reset}
`);
}

function showHelpCreate(): void {
  console.log(`
${COLORS.bold}${TOOL_NAME} create${COLORS.reset} — Schema-Driven File Generation

${COLORS.bold}Usage:${COLORS.reset}
  ${TOOL_NAME} create <format> <subtype> <dest> [options]

${COLORS.bold}Formats:${COLORS.reset}  go, rust
${COLORS.bold}Subtypes:${COLORS.reset} library, executable, module, bare-bone, demo-test

${COLORS.bold}Options:${COLORS.reset}
  --key <K>         OmniCode key (default: derived from filename)
  --title <T>       File title (default: derived from filename)
  --purpose <P>     File purpose (default: placeholder)
  --dry-run         Preview output without writing
  --extensions      Include extension sections (I4, C5-C7, etc.)

${COLORS.bold}How it works:${COLORS.reset}
  The schema defines the complete specification for each format × subtype.
  Generated files lint with 0 errors — the schema IS the specification.
  Different subtypes produce different section layouts (form-aware fill).

${COLORS.bold}Examples:${COLORS.reset}
  ${TOOL_NAME} create go library pkg/auth/auth.go                    ${COLORS.dim}# Go library${COLORS.reset}
  ${TOOL_NAME} create go executable cmd/main.go                      ${COLORS.dim}# Go executable (package main)${COLORS.reset}
  ${TOOL_NAME} create rust module src/config.rs                      ${COLORS.dim}# Rust module${COLORS.reset}
  ${TOOL_NAME} create rust executable src/main.rs --key B-cli        ${COLORS.dim}# with custom key${COLORS.reset}
  ${TOOL_NAME} create go library out.go --dry-run                    ${COLORS.dim}# preview without writing${COLORS.reset}
  ${TOOL_NAME} create go bare-bone scratch.go                        ${COLORS.dim}# minimal skeleton${COLORS.reset}
`);
}

function showHelpTranspile(): void {
  console.log(`
${COLORS.bold}${TOOL_NAME} transpile${COLORS.reset} — Cross-Format Transpilation

${COLORS.bold}Usage:${COLORS.reset}
  ${TOOL_NAME} transpile <source-format> <target-format> <file> [options]

${COLORS.bold}Supported:${COLORS.reset}  rust → go

${COLORS.bold}Options:${COLORS.reset}
  --dry-run         Preview output without writing

${COLORS.bold}What transpile produces:${COLORS.reset}
  A structural skeleton in the target format:
  - Correct 4-block layout with separators
  - Section headers in canonical order
  - Identity fields transpiled (key, title, types)
  - Function signatures with Go conventions (PascalCase, camelCase)
  - Method bodies translated where possible (format!, return, etc.)
  - TODO stubs for complex bodies

${COLORS.bold}Examples:${COLORS.reset}
  ${TOOL_NAME} transpile rust go src/lib.rs            ${COLORS.dim}# Rust → Go library${COLORS.reset}
  ${TOOL_NAME} transpile rust go src/lib.rs --dry-run  ${COLORS.dim}# preview transpilation${COLORS.reset}
`);
}

function showHelpQuery(): void {
  console.log(`
${COLORS.bold}${TOOL_NAME} query${COLORS.reset} — Health Database Queries

${COLORS.bold}Usage:${COLORS.reset}
  ${TOOL_NAME} query <subcommand> [args] --db <path>

${COLORS.bold}Subcommands:${COLORS.reset}
  health <path>         File health score, Hebrew state, format, and history
  worst [--limit N]     Worst-scoring files (sorted ascending)
  best [--limit N]      Best-scoring files (sorted descending)
  patterns [--limit N]  Top recurring issues with trend arrows (↑ increasing, ↓ decreasing)
  trend <path>          Health score over time for one file (reverse chronological)
  instance <name>       CPI-SI instance activity (runs, sessions, last active)
  session <id>          Session summary (files linted, health delta, patterns)
  stats                 Aggregate statistics (files, runs, avg health)

${COLORS.bold}Database setup:${COLORS.reset}
  ${COLORS.dim}Add --db <path> to any lint command to start recording.${COLORS.reset}
  ${COLORS.dim}The database is created automatically on first use.${COLORS.reset}

${COLORS.bold}Options:${COLORS.reset}
  --db <path>       SQLite database path (required for all queries)
  --limit <N>       Limit results (default: 10)

${COLORS.bold}Examples:${COLORS.reset}
  ${TOOL_NAME} lint . --db .cws-struct.db                      ${COLORS.dim}# lint + record${COLORS.reset}
  ${TOOL_NAME} query health path/to/file.go --db .cws-struct.db  ${COLORS.dim}# file health${COLORS.reset}
  ${TOOL_NAME} query worst --db .cws-struct.db                 ${COLORS.dim}# worst files${COLORS.reset}
  ${TOOL_NAME} query best --limit 20 --db .cws-struct.db       ${COLORS.dim}# top 20 files${COLORS.reset}
  ${TOOL_NAME} query patterns --db .cws-struct.db              ${COLORS.dim}# recurring issues${COLORS.reset}
  ${TOOL_NAME} query trend path/to/file.go --db .cws-struct.db ${COLORS.dim}# score over time${COLORS.reset}
  ${TOOL_NAME} query stats --db .cws-struct.db                 ${COLORS.dim}# aggregate stats${COLORS.reset}
`);
}

function showHelpOptions(): void {
  console.log(`
${COLORS.bold}${TOOL_NAME}${COLORS.reset} — All CLI Options

${COLORS.bold}Display:${COLORS.reset}
  --verbose, -v     Show all results including info-level messages
  --deep, -vv       Verbose + file anatomy (directives, blocks, identity, per-action)
  --errors-only     Show only errors (suppress warnings/info)
  --summary         One-line-per-file view (status + counts only)
  --json            Machine-readable JSON output (suppresses human output)
  --why             Trace each result to its data-layer source (reasoning chain)
  --inspect         Show parsed structure without running lint checks

${COLORS.bold}Behavior:${COLORS.reset}
  --fail-fast       Stop after first file with errors
  --policy <P>      Ternary lint policy: strict (-1), balanced (0, default), growth (+1)
  --check <focus>   Focus lint on a health grid slice
  --suggest [N]     Top N quick-fix suggestions ranked by impact (default: 5)
  --log <path>      Write atomic health observations to structured log file

${COLORS.bold}Diff-aware:${COLORS.reset}
  --changed         Only lint files changed since last commit (git diff HEAD)
  --since <ref>     Git ref for diff baseline (implies --changed)
  --staged          Only lint staged files (git diff --cached)

${COLORS.bold}Create/Transform:${COLORS.reset}
  --dry-run         Preview output without writing files
  --extensions      Scaffold extension sections (I4, C5-C7, X2-X4, etc.)
  --force           Re-scaffold even if file already has block structure
  --steps           Write each scaffold phase to .steps/ directory
  --key <K>         OmniCode key for create (default: derived from path)
  --title <T>       File title for create (default: derived from filename)
  --purpose <P>     File purpose for create (default: placeholder)

${COLORS.bold}Database:${COLORS.reset}
  --db <path>       Enable SQLite health database (record + query results)
  --limit <N>       Limit results for query commands (default: 10)

${COLORS.bold}Diagnostic:${COLORS.reset}
  --debug           Diagnostic output to stderr (parser, data layer, timing)
  --trace           Step-by-step execution log to stderr (implies --debug)

${COLORS.bold}Other:${COLORS.reset}
  --port <N>        Studio port (default: 4200)
  --help, -h        Show this help
  --version         Show version
`);
}

function showHelpLayers(): void {
  console.log(`
${COLORS.bold}${TOOL_NAME}${COLORS.reset} — Detection Layer Architecture

${COLORS.bold}Layers:${COLORS.reset}  ${COLORS.dim}(each layer builds on the one below — higher = stricter)${COLORS.reset}

  ${COLORS.bold}R[50]${COLORS.reset}  Existence      ${COLORS.dim}Does the file have structure?${COLORS.reset}
         Block separators (METADATA, SETUP, BODY, CLOSING)
         Separator characters, width, consistency

  ${COLORS.bold}R[25]${COLORS.reset}  Organization   ${COLORS.dim}Is the structure in the right order?${COLORS.reset}
         Block ordering (M → S → B → C)
         Subsection ordering within blocks
         Section presence and placement

  ${COLORS.bold}R[10]${COLORS.reset}  Content        ${COLORS.dim}Do the contents follow the schema?${COLORS.reset}
         Key naming patterns and types
         Directive values and formats
         Identity field validation

  ${COLORS.bold}R[05]${COLORS.reset}  Pattern        ${COLORS.dim}Does the file show depth?${COLORS.reset}
         Concept detection (function calls, error propagation, etc.)
         Metadata completeness and grounding
         Children/scripture structure (OFD)

  ${COLORS.bold}T[00]${COLORS.reset}  Tool           ${COLORS.dim}Is the infrastructure sound?${COLORS.reset}
         Data layer integrity (error catalog, format rules, types)
         Engine integrity (registry, pipeline, cascade, output)
         Foundation integrity (schema, composition, health)
         ${COLORS.dim}(Internal tests only — not user-facing lint)${COLORS.reset}

${COLORS.bold}Error code anatomy:${COLORS.reset}
  CWS-R50-020 → 3D coordinates: R50.BLK.020
    Layer:  R50 (Existence)
    Domain: BLK (block structure)
    ID:     020 (specific check)

${COLORS.bold}Cascade behavior:${COLORS.reset}
  If R[50] fails (no blocks found), R[25]–R[05] health is neutralized.
  If a specific block fails at R[50], only that block cascades.
  Cascade prevents misleading scores — can't organize what doesn't exist.

${COLORS.bold}Usage:${COLORS.reset}
  ${TOOL_NAME} lint . --check R25              ${COLORS.dim}# focus on organization layer${COLORS.reset}
  ${TOOL_NAME} lint . --check metadata:R10     ${COLORS.dim}# metadata content only${COLORS.reset}
  deno task test:R50                           ${COLORS.dim}# run existence tests${COLORS.reset}
  deno task witness --layer R25                ${COLORS.dim}# coverage for one layer${COLORS.reset}
`);
}

function showHelpTesting(): void {
  console.log(`
${COLORS.bold}${TOOL_NAME}${COLORS.reset} — Test Navigation

${COLORS.bold}Structure:${COLORS.reset}  ${COLORS.dim}tests/<semantic-layer>/<R-level>/<format-or-subsystem>/<concern>/${COLORS.reset}

  tests/
  ├── existence/R50/       ${COLORS.dim}# Does it exist? (blocks, separators, types)${COLORS.reset}
  ├── organization/R25/    ${COLORS.dim}# Is it ordered? (block order, sections)${COLORS.reset}
  ├── content/R10/         ${COLORS.dim}# Is it correct? (keys, values, schema)${COLORS.reset}
  ├── pattern/R05/         ${COLORS.dim}# Is it deep? (concepts, metadata, grounding)${COLORS.reset}
  ├── pattern/R03/         ${COLORS.dim}# Recursion and self-access patterns${COLORS.reset}
  ├── tool/T00/            ${COLORS.dim}# Infrastructure integrity${COLORS.reset}
  ├── integration/         ${COLORS.dim}# End-to-end handler tests${COLORS.reset}
  └── cli/                 ${COLORS.dim}# CLI invocation tests${COLORS.reset}

${COLORS.bold}Run by scope:${COLORS.reset}
  deno task test                     ${COLORS.dim}# everything (93 files, 1200+ tests)${COLORS.reset}
  deno task test:existence           ${COLORS.dim}# all R50 tests${COLORS.reset}
  deno task test:organization        ${COLORS.dim}# all R25 tests${COLORS.reset}
  deno task test:content             ${COLORS.dim}# all R10 tests${COLORS.reset}
  deno task test:pattern             ${COLORS.dim}# all R05 + R03 tests${COLORS.reset}
  deno task test:tool                ${COLORS.dim}# all T00 tests${COLORS.reset}

${COLORS.bold}Run by R-level:${COLORS.reset}
  deno task test:R50                 ${COLORS.dim}# existence layer${COLORS.reset}
  deno task test:R25                 ${COLORS.dim}# organization layer${COLORS.reset}
  deno task test:R10                 ${COLORS.dim}# content layer${COLORS.reset}
  deno task test:R05                 ${COLORS.dim}# pattern layer${COLORS.reset}
  deno task test:T00                 ${COLORS.dim}# tool layer${COLORS.reset}

${COLORS.bold}Run by format handler:${COLORS.reset}
  deno task test:go                  ${COLORS.dim}# Go handler (R50→R03)${COLORS.reset}
  deno task test:rust                ${COLORS.dim}# Rust handler (R50→R03)${COLORS.reset}
  deno task test:toml                ${COLORS.dim}# TOML handler (R50→R05)${COLORS.reset}

${COLORS.bold}Run by subsystem:${COLORS.reset}
  deno task test:engine              ${COLORS.dim}# engine across layers${COLORS.reset}
  deno task test:data                ${COLORS.dim}# data layer across layers${COLORS.reset}
  deno task test:foundation          ${COLORS.dim}# foundation across layers${COLORS.reset}
  deno task test:shared              ${COLORS.dim}# shared utilities across layers${COLORS.reset}

${COLORS.bold}Run by handler × level:${COLORS.reset}
  deno task test:rust:R50            ${COLORS.dim}# Rust existence tests${COLORS.reset}
  deno task test:go:R25              ${COLORS.dim}# Go organization tests${COLORS.reset}
  deno task test:toml:R10            ${COLORS.dim}# TOML content tests${COLORS.reset}

${COLORS.bold}Run by concern:${COLORS.reset}
  deno task test:blocks              ${COLORS.dim}# block detection across formats${COLORS.reset}
  deno task test:ordering            ${COLORS.dim}# block ordering across formats${COLORS.reset}
  deno task test:concept             ${COLORS.dim}# concept detection across formats${COLORS.reset}

${COLORS.bold}Run one error code's test:${COLORS.reset}
  deno test --filter='CWS-R50-020'   ${COLORS.dim}# test for specific error code${COLORS.reset}
  deno test --filter='CWS-T00-087'   ${COLORS.dim}# test for transpiler integrity${COLORS.reset}

${COLORS.bold}Cross-cutting:${COLORS.reset}
  deno task test:cli                 ${COLORS.dim}# CLI invocation tests${COLORS.reset}
  deno task test:integration         ${COLORS.dim}# end-to-end handler tests${COLORS.reset}
  deno task test:errors              ${COLORS.dim}# error catalog integrity${COLORS.reset}
`);
}

function showHelpWitness(): void {
  console.log(`
${COLORS.bold}${TOOL_NAME}${COLORS.reset} — Witness Coverage Analysis

${COLORS.bold}What it does:${COLORS.reset}
  Scans test files for [CWS-*] bracket tags, maps them against the
  252-code error catalog, reports coverage in 3D: Layer × Domain × Severity.

${COLORS.bold}Usage:${COLORS.reset}  ${COLORS.dim}(via deno task)${COLORS.reset}
  deno task witness                  ${COLORS.dim}# summary: X/252 codes witnessed${COLORS.reset}
  deno task witness:verbose          ${COLORS.dim}# + unwitnessed codes by layer${COLORS.reset}
  deno task witness:grid             ${COLORS.dim}# layer × domain coverage grid${COLORS.reset}
  deno task witness:full             ${COLORS.dim}# grid + verbose + file locations${COLORS.reset}

${COLORS.bold}Filters:${COLORS.reset}
  deno task witness -- --layer R25          ${COLORS.dim}# one layer only${COLORS.reset}
  deno task witness -- --domain BLK         ${COLORS.dim}# one domain only${COLORS.reset}
  deno task witness -- --layer R25 --files  ${COLORS.dim}# which files witness each code${COLORS.reset}

${COLORS.bold}How witness tags work:${COLORS.reset}
  Each Deno.test() name starts with [CWS-R50-020] (or similar).
  The witness script greps for these tags and maps them to the error catalog.
  One tag per test file's first test — identifies which error code that file covers.

${COLORS.bold}3D coordinates:${COLORS.reset}
  CWS-R50-020 → R50.BLK.020
    Layer:  R50 (from error code prefix)
    Domain: BLK (from category → domain mapping)
    ID:     020 (catalog offset)

${COLORS.bold}Domain codes:${COLORS.reset}
  BLK  Block structure       SEP  Separators          DIR  Directives
  IDE  Identity              CON  Content              ORD  Ordering
  FLD  Field values          SCH  Schema               CHP  Children/Path
  GRD  Grounding             PIP  Pipeline             CMP  Composition
  TRN  Transpiler            REG  Registry             FIL  Fill
  DAT  Data                  DBA  Database             INT  Integrity
`);
}

function showHelpExamples(): void {
  console.log(`
${COLORS.bold}${TOOL_NAME}${COLORS.reset} — Examples

${COLORS.bold}── Lint ──${COLORS.reset}
  ${TOOL_NAME} lint .                                  ${COLORS.dim}# auto-detect all formats${COLORS.reset}
  ${TOOL_NAME} lint rust src/                          ${COLORS.dim}# lint only Rust files${COLORS.reset}
  ${TOOL_NAME} lint go . --verbose                     ${COLORS.dim}# all results + health breakdown${COLORS.reset}
  ${TOOL_NAME} lint rust . --deep                      ${COLORS.dim}# file anatomy + per-action detail${COLORS.reset}
  ${TOOL_NAME} lint . --check metadata:R25             ${COLORS.dim}# focus on metadata organization${COLORS.reset}
  ${TOOL_NAME} lint . --json --fail-fast               ${COLORS.dim}# CI mode: JSON + stop on first error${COLORS.reset}
  ${TOOL_NAME} lint --changed .                        ${COLORS.dim}# only files changed since HEAD${COLORS.reset}
  ${TOOL_NAME} lint --staged .                         ${COLORS.dim}# only staged files${COLORS.reset}
  ${TOOL_NAME} lint --since main .                     ${COLORS.dim}# changed since main branch${COLORS.reset}
  ${TOOL_NAME} lint . --suggest 10                     ${COLORS.dim}# top 10 quick-fix suggestions${COLORS.reset}
  ${TOOL_NAME} lint . --why                            ${COLORS.dim}# reasoning chain per result${COLORS.reset}
  ${TOOL_NAME} lint . --inspect                        ${COLORS.dim}# parsed structure, no checks${COLORS.reset}
  ${TOOL_NAME} lint toml . --summary                   ${COLORS.dim}# one-line per TOML file${COLORS.reset}
  ${TOOL_NAME} lint . --policy strict                  ${COLORS.dim}# strict policy (no grace)${COLORS.reset}

${COLORS.bold}── Transform ──${COLORS.reset}
  ${TOOL_NAME} transform .                             ${COLORS.dim}# auto-fix all formats${COLORS.reset}
  ${TOOL_NAME} transform rust src/ --dry-run           ${COLORS.dim}# preview Rust fixes${COLORS.reset}
  ${TOOL_NAME} transform go . --extensions             ${COLORS.dim}# include extension sections${COLORS.reset}
  ${TOOL_NAME} transform . --steps                     ${COLORS.dim}# step-by-step scaffold output${COLORS.reset}
  ${TOOL_NAME} transform go file.go --force            ${COLORS.dim}# re-scaffold existing structure${COLORS.reset}

${COLORS.bold}── Create ──${COLORS.reset}
  ${TOOL_NAME} create go library pkg/auth/auth.go      ${COLORS.dim}# Go library${COLORS.reset}
  ${TOOL_NAME} create go executable cmd/main.go        ${COLORS.dim}# Go executable (package main)${COLORS.reset}
  ${TOOL_NAME} create rust module src/config.rs        ${COLORS.dim}# Rust module${COLORS.reset}
  ${TOOL_NAME} create rust executable src/main.rs      ${COLORS.dim}# Rust executable${COLORS.reset}
  ${TOOL_NAME} create go bare-bone scratch.go          ${COLORS.dim}# minimal skeleton${COLORS.reset}
  ${TOOL_NAME} create go library out.go --dry-run      ${COLORS.dim}# preview without writing${COLORS.reset}
  ${TOOL_NAME} create go library out.go --key B-auth --title "Auth" --purpose "Authentication"

${COLORS.bold}── Transpile ──${COLORS.reset}
  ${TOOL_NAME} transpile rust go src/lib.rs            ${COLORS.dim}# Rust → Go library${COLORS.reset}
  ${TOOL_NAME} transpile rust go src/lib.rs --dry-run  ${COLORS.dim}# preview transpilation${COLORS.reset}

${COLORS.bold}── Database ──${COLORS.reset}
  ${TOOL_NAME} lint . --db .cws-struct.db              ${COLORS.dim}# lint + record to database${COLORS.reset}
  ${TOOL_NAME} query health file.go --db .cws-struct.db   ${COLORS.dim}# file health + history${COLORS.reset}
  ${TOOL_NAME} query worst --db .cws-struct.db         ${COLORS.dim}# worst-scoring files${COLORS.reset}
  ${TOOL_NAME} query best --limit 20 --db .cws-struct.db  ${COLORS.dim}# top 20 files${COLORS.reset}
  ${TOOL_NAME} query patterns --db .cws-struct.db      ${COLORS.dim}# recurring issues + trends${COLORS.reset}
  ${TOOL_NAME} query trend file.go --db .cws-struct.db ${COLORS.dim}# score over time${COLORS.reset}
  ${TOOL_NAME} query instance "Nova Dawn" --db .cws-struct.db  ${COLORS.dim}# instance activity${COLORS.reset}
  ${TOOL_NAME} query stats --db .cws-struct.db         ${COLORS.dim}# aggregate stats${COLORS.reset}

${COLORS.bold}── Watch & Tools ──${COLORS.reset}
  ${TOOL_NAME} watch .                                 ${COLORS.dim}# watch all, re-lint on change${COLORS.reset}
  ${TOOL_NAME} watch go src/                           ${COLORS.dim}# watch Go files only${COLORS.reset}
  ${TOOL_NAME} verify env                              ${COLORS.dim}# check dev tools${COLORS.reset}
  ${TOOL_NAME} verify env --verbose                    ${COLORS.dim}# include optional tools${COLORS.reset}
  ${TOOL_NAME} studio                                  ${COLORS.dim}# launch web UI on :4200${COLORS.reset}
  ${TOOL_NAME} studio --port 3000                      ${COLORS.dim}# custom port${COLORS.reset}
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
  explicitFiles?: Set<string>,
): Promise<LintSummary[]> {
  const cwd = Deno.cwd();
  const summaries: LintSummary[] = [];
  const showHuman = !opts.json;
  const isTTY = showHuman && Deno.stdout.isTerminal();
  const encoder = new TextEncoder();
  let hitFailFast = false;

  const pool = pooledMap(LINT_CONCURRENCY, files, async (file) => {
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

/**
 * Emit atomic health observations to a structured log file.
 * Format: TIMESTAMP|ACTION|DELTA|SOURCE|DETAIL (from log.toml spec).
 * "The books are opened." — Revelation 20:12
 */
async function emitHealthLog(
  logPath: string,
  summaries: LintSummary[],
  formatName: string,
): Promise<void> {
  for (const s of summaries) {
    if (!s.health) continue;
    // Collect all atomic actions from health blocks → containers → actions
    const actions = s.health.blocks.flatMap((b) =>
      b.containers.flatMap((c) => c.actions)
    );
    if (actions.length > 0) {
      try {
        await writeLogEntries(logPath, actions, formatName, s.file);
      } catch (e) {
        if (e instanceof Deno.errors.NotCapable || e instanceof Deno.errors.PermissionDenied) {
          console.error(
            `${COLORS.red}--log requires write permission. Run with: deno run --allow-read --allow-write mod.ts ...${COLORS.reset}`,
          );
          return; // Stop trying — permission won't change mid-run
        }
        throw e;
      }
    }
  }
}

/**
 * Emit lint results as structured JSON to stdout.
 * Machine-readable output for CI/CD pipelines and tooling integration.
 */
function emitJson(summaries: LintSummary[], why?: boolean): void {
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
      results: s.results.map((r) => {
        const base = {
          severity: r.severity,
          rule: r.rule,
          message: r.message,
          line: r.line ?? null,
          errorCode: r.errorCode ?? null,
          layerName: r.layerName ?? null,
          fix: r.fix ?? null,
        };
        if (!why) return base;
        // Enrich with reasoning chain
        const entry = matchRule(r.rule);
        return {
          ...base,
          why: entry ? {
            layer: entry.layer,
            layerName: entry.layerName,
            category: entry.category,
            suggestion: entry.suggestionTemplate,
            checkFunction: entry.checkFunction,
          } : null,
        };
      }),
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

/**
 * Run inspect mode — show parsed structure without lint checks.
 * Calls handler.inspect() for each file and formats the output.
 */
async function runInspect(handler: FormatHandler, files: string[], json: boolean): Promise<boolean> {
  if (!handler.inspect) {
    console.error(
      `${COLORS.yellow}--inspect not supported for format '${handler.name}' (no inspect method).${COLORS.reset}`,
    );
    return false;
  }

  const results: InspectResult[] = [];
  for (const file of files) {
    try {
      const result = await handler.inspect(file);
      results.push(result);
      if (!json) {
        console.log(formatInspectText(result));
      }
    } catch (e: unknown) {
      const msg = e instanceof Error ? e.message : String(e);
      console.error(`${COLORS.red}Error inspecting ${file}: ${msg}${COLORS.reset}`);
    }
  }

  if (json) {
    console.log(results.length === 1
      ? formatInspectJson(results[0]!)
      : JSON.stringify(results, null, 2));
  }

  return true;
}

async function runLint(opts: CliOptions, db?: DbHandle | null): Promise<boolean> {
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
        emitJson([], opts.why);
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
    if (opts.json) emitJson(summaries, opts.why);
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
  if (opts.json) emitJson(allSummaries, opts.why);
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
  explicitFiles?: Set<string>,
): Promise<LintSummary[]> {
  if (!handler.transform) return [];

  const cwd = Deno.cwd();
  const summaries: LintSummary[] = [];
  const transformFn = handler.transform.bind(handler);

  const pool = pooledMap(LINT_CONCURRENCY, files, async (file) => {
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

async function runTransform(opts: CliOptions): Promise<boolean> {
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

// ---------------------------------------------------------------------------
// Create command — schema-driven file generation
// ---------------------------------------------------------------------------

/**
 * Generate a new 4-block code file from schema.
 *
 * Usage: cws-struct create <format> <subtype> <dest> [--key K] [--title T] [--purpose P]
 *
 * The proof: if the generated file lints with 0 errors,
 * the schema IS the complete specification.
 */
async function runCreate(opts: CliOptions): Promise<boolean> {
  if (!opts.format) {
    console.error(`${COLORS.red}Format required. Usage: ${TOOL_NAME} create <format> <subtype> <dest>${COLORS.reset}`);
    return false;
  }

  const handler = getFormat(opts.format);
  if (!handler) {
    console.error(`${COLORS.red}Unknown format: ${opts.format}${COLORS.reset}`);
    return false;
  }

  const subtype = opts.subtype ?? "library";
  const dest = opts.targets[0];
  if (!dest) {
    console.error(`${COLORS.red}Destination path required. Usage: ${TOOL_NAME} create ${opts.format} ${subtype} <dest>${COLORS.reset}`);
    return false;
  }

  // Derive defaults from destination path
  const filename = basename(dest);
  const dirName = basename(dirname(dest));
  const ext = filename.split(".").pop() ?? "";
  const nameWithoutExt = filename.replace(`.${ext}`, "");

  // Build FillContext
  const ctx: FillContext = {
    format: opts.format as "go" | "rust",
    subtype,
    key: opts.key ?? `B-${nameWithoutExt}`,
    title: opts.title ?? nameWithoutExt.split(/[-_]/).map(
      (w) => w.charAt(0).toUpperCase() + w.slice(1),
    ).join(" "),
    purpose: opts.purpose ?? "[purpose]",
    filename,
    packageOrCrate: subtype === "executable" && opts.format === "go"
      ? "main"
      : dirName !== "." ? dirName : nameWithoutExt,
    from: dest,
  };

  const text = await generateFileText(ctx);

  if (opts.dryRun) {
    console.log(`${COLORS.bold}${TOOL_NAME}${COLORS.reset} — Preview (dry run)\n`);
    console.log(`${COLORS.dim}Destination: ${dest}${COLORS.reset}`);
    console.log(`${COLORS.dim}Format: ${opts.format}, Subtype: ${subtype}${COLORS.reset}`);
    console.log(`${COLORS.dim}Key: ${ctx.key}, Title: ${ctx.title}${COLORS.reset}\n`);
    console.log(text);
    return true;
  }

  // Write the file
  await Deno.writeTextFile(dest, text);
  console.log(`${COLORS.green}Created${COLORS.reset} ${dest}`);
  console.log(`${COLORS.dim}Format: ${opts.format}, Subtype: ${subtype}, Key: ${ctx.key}${COLORS.reset}`);

  // Verify: lint the generated file
  const results = await handler.lint(dest);
  resolveErrorCodes(results);
  const errors = results.filter((r) => r.severity === "error");
  if (errors.length > 0) {
    console.error(`\n${COLORS.red}Generated file has ${errors.length} lint error(s):${COLORS.reset}`);
    for (const e of errors) {
      const codePrefix = e.errorCode ? `${e.errorCode} ` : "";
      console.error(`  ${codePrefix}${e.rule}: ${e.message}`);
    }
    return false;
  }

  console.log(`${COLORS.green}Verified${COLORS.reset} — 0 lint errors (schema = specification)`);
  return true;
}

// ---------------------------------------------------------------------------
// Transpile command — three-cord format conversion
// ---------------------------------------------------------------------------

/**
 * Transpile a source file from one format to another.
 *
 * Usage: cws-struct transpile <source-format> <target-format> <file> [--dry-run]
 *
 * The transpiler produces a structural skeleton in the target format:
 * correct 4-block layout, section headers, identity, function stubs.
 * Method bodies are TODO stubs — this proves architecture, not parsing.
 */
async function runTranspile(opts: CliOptions): Promise<boolean> {
  // Parse: transpile <source-format> <target-format> <file>
  // opts.format has the first non-flag (source format)
  // opts.targets has the remaining non-flags
  const sourceFormat = opts.format as CodeFormat | undefined;
  if (!sourceFormat) {
    console.error(`${COLORS.red}Source format required. Usage: ${TOOL_NAME} transpile <source> <target> <file>${COLORS.reset}`);
    return false;
  }

  // Target format is first target, source file is second
  if (opts.targets.length < 2) {
    console.error(`${COLORS.red}Target format and source file required. Usage: ${TOOL_NAME} transpile ${sourceFormat} <target> <file>${COLORS.reset}`);
    return false;
  }

  const targetFormat = opts.targets[0] as CodeFormat;
  const sourceFile = opts.targets[1]!;

  // Verify the source file exists
  try {
    await Deno.stat(sourceFile);
  } catch {
    console.error(`${COLORS.red}Source file not found: ${sourceFile}${COLORS.reset}`);
    return false;
  }

  console.log(`${COLORS.bold}${TOOL_NAME}${COLORS.reset} — Transpile ${sourceFormat} → ${targetFormat}`);
  console.log(`${COLORS.dim}Source: ${sourceFile}${COLORS.reset}`);
  if (opts.dryRun) {
    console.log(`${COLORS.dim}Mode: dry run (preview only)${COLORS.reset}`);
  }
  console.log();

  const result = await transpileFile(sourceFile, sourceFormat, targetFormat, {
    dryRun: opts.dryRun,
  });

  // Print diagnostics
  for (const d of result.diagnostics) {
    const color = d.severity === "error" ? COLORS.red
      : d.severity === "warn" ? COLORS.yellow
      : COLORS.dim;
    const icon = d.severity === "error" ? "X"
      : d.severity === "warn" ? "!"
      : "i";
    const lineRef = d.sourceLine ? `:${d.sourceLine}` : "";
    console.log(`  ${color}${icon}${COLORS.reset} ${d.message}${lineRef}`);
  }

  if (!result.success) {
    console.error(`\n${COLORS.red}Transpilation failed.${COLORS.reset}`);
    return false;
  }

  // Print stats
  const s = result.stats;
  console.log(`\n${COLORS.bold}Stats:${COLORS.reset}`);
  console.log(`  Chunks extracted:  ${s.chunksExtracted}`);
  console.log(`  Items extracted:   ${s.itemsExtracted}`);
  console.log(`  Sections mapped:   ${s.sectionsMapped}`);
  console.log(`  Stubs generated:   ${s.stubsGenerated}`);
  console.log(`  Direct transfers:  ${s.directTransfers}`);
  console.log(`  Output lines:      ${result.lineCount}`);

  if (opts.dryRun) {
    console.log(`\n${COLORS.bold}Preview:${COLORS.reset}\n`);
    console.log(result.output);
  } else if (result.targetFile) {
    console.log(`\n${COLORS.green}Written${COLORS.reset} ${result.targetFile}`);
  }

  return true;
}

// ---------------------------------------------------------------------------
// Query command — read from health database
// ---------------------------------------------------------------------------

/**
 * Parse --check value into a LintGridOptions for block × layer slicing.
 *
 * Accepts:
 *   "metadata"       → vertical slice (focusBlock only)
 *   "R25"            → horizontal slice (focusLayer only)
 *   "metadata:R25"   → single cell (both)
 *   undefined        → full grid (no focus)
 */
function parseLintFocus(check?: string): LintGridOptions | undefined {
  if (!check) return undefined;

  const LAYER_MAP: Record<string, LintLayer> = { r50: 0, r25: 1, r10: 2, r5: 3 };
  const BLOCK_NAMES = new Set<string>(["metadata", "setup", "body", "closing"]);

  const parts = check.split(":");
  const opts: LintGridOptions = {};

  for (const part of parts) {
    const lower = part.toLowerCase();
    if (BLOCK_NAMES.has(lower)) opts.focusBlock = lower as BlockName;
    else if (LAYER_MAP[lower] !== undefined) opts.focusLayer = LAYER_MAP[lower];
    else if (part) opts.focusContainer = part; // Preserve original case
  }

  return (opts.focusBlock || opts.focusLayer !== undefined || opts.focusContainer) ? opts : undefined;
}

/**
 * Parse --limit value from CLI args. Returns default if not present or invalid.
 */
function parseLimit(args: string[], defaultLimit = 10): number {
  const idx = args.indexOf("--limit");
  if (idx < 0 || idx + 1 >= args.length) return defaultLimit;
  const n = parseInt(args[idx + 1]!, 10);
  return Number.isFinite(n) && n > 0 ? n : defaultLimit;
}

/**
 * Health color for terminal display — matches output.ts Hebrew state thresholds.
 * perfect/whole (34+) = green, sound/even (0-33) = yellow, lacking/wanting/broken (<0) = red.
 */
function qColor(score: number | null): string {
  if (score === null) return COLORS.dim;
  if (score >= 34) return COLORS.green;
  if (score >= 0) return COLORS.yellow;
  return COLORS.red;
}

/** Format a file health line: score + state + emoji. */
function formatHealthLine(score: number | null, _state: string | null): string {
  if (score === null) return `${COLORS.dim}(no score)${COLORS.reset}`;
  const level = trueToLevel(score);
  const emoji = levelToEmoji(level);
  return `${qColor(score)}${score}/\u00b1100 ${level} ${emoji}${COLORS.reset}`;
}

/**
 * Query health database.
 *
 * Usage: cws-struct query <subcommand> [args] --db <path>
 *
 * "The books were opened." — Revelation 20:12
 */
async function runQuery(opts: CliOptions): Promise<boolean> {
  if (!opts.dbPath) {
    console.error(`${COLORS.red}--db <path> required for query command.${COLORS.reset}`);
    console.error(`Example: ${TOOL_NAME} query stats --db .cws-struct.db`);
    return false;
  }

  if (!opts.queryCommand) {
    console.error(`${COLORS.red}Query subcommand required.${COLORS.reset}`);
    console.error(`Available: health, worst, best, patterns, trend, instance, session, stats`);
    console.error(`Example: ${TOOL_NAME} query stats --db ${opts.dbPath}`);
    return false;
  }

  // Verify database file exists
  try {
    await Deno.stat(opts.dbPath);
  } catch {
    console.error(`${COLORS.red}Database not found: ${opts.dbPath}${COLORS.reset}`);
    console.error(`Run a lint with --db to create it: ${TOOL_NAME} lint . --db ${opts.dbPath}`);
    return false;
  }

  const mod = await getDbModule();
  const db = new mod.CwsDatabase(undefined, opts.dbPath);
  const limit = parseLimit(Deno.args);

  try {
    switch (opts.queryCommand) {
      case "health":
        return queryHealth(db, opts);

      case "worst":
        return queryWorst(db, limit);

      case "best":
        return queryBest(db, limit);

      case "patterns":
        return queryPatterns(db, limit);

      case "trend":
        return queryTrend(db, opts);

      case "instance":
        return queryInstance(db, opts);

      case "session":
        return querySession(db, opts);

      case "stats":
        return queryStats(db);

      default:
        console.error(`${COLORS.red}Unknown query subcommand: ${opts.queryCommand}${COLORS.reset}`);
        console.error(`Available: health, worst, best, patterns, trend, instance, session, stats`);
        return false;
    }
  } finally {
    db.close();
  }
}

// ── Query subcommand implementations ──────────────────────────────────

function queryHealth(db: DbHandle, opts: CliOptions): boolean {
  const filePath = opts.targets[0];
  if (!filePath) {
    console.error(`${COLORS.red}File path required. Usage: ${TOOL_NAME} query health <path> --db <db>${COLORS.reset}`);
    return false;
  }

  const report = db.getFileHealth(filePath);
  if (!report) {
    console.error(`${COLORS.yellow}No data for: ${filePath}${COLORS.reset}`);
    console.error(`${COLORS.dim}Run a lint with --db first to track this file.${COLORS.reset}`);
    return false;
  }

  console.log(`${COLORS.bold}File health:${COLORS.reset} ${report.path}`);
  console.log(`  Score: ${formatHealthLine(report.score, report.state)}`);
  if (report.trite) {
    console.log(`  ${COLORS.dim}Trite: ${report.trite}${COLORS.reset}`);
  }
  console.log(`  Format: ${report.format}${report.subtype ? ` (${report.subtype})` : ""}`);

  if (report.lastRun) {
    console.log(`  Last run: ${COLORS.dim}${report.lastRun.timestamp} by ${report.lastRun.instanceName ?? "unknown"}${COLORS.reset}`);
    console.log(`    ${report.lastRun.errorCount}E ${report.lastRun.warnCount}W ${report.lastRun.infoCount}I`);
  }

  if (report.history.length > 1) {
    console.log(`\n${COLORS.bold}History:${COLORS.reset} (${report.history.length} entries)`);
    for (const h of report.history.slice(0, 10)) {
      const delta = h.delta !== null && h.delta !== 0
        ? ` ${h.delta > 0 ? COLORS.green + "+" : COLORS.red}${h.delta}${COLORS.reset}`
        : "";
      console.log(
        `  ${COLORS.dim}${h.timestamp}${COLORS.reset}  ${formatHealthLine(h.score, h.hebrewState)}${delta}`,
      );
    }
  }

  return true;
}

function queryWorst(db: DbHandle, limit: number): boolean {
  const files = db.getUnhealthyFiles(100); // everything below +100 (i.e., everything)
  if (files.length === 0) {
    console.log(`${COLORS.green}No unhealthy files tracked.${COLORS.reset}`);
    return true;
  }

  // Sort by score ascending (worst first) — getUnhealthyFiles already does this
  const show = files.slice(0, limit);
  console.log(`${COLORS.bold}Worst health files:${COLORS.reset} (${show.length}/${files.length})\n`);

  for (let i = 0; i < show.length; i++) {
    const f = show[i]!;
    console.log(
      `  ${COLORS.dim}${String(i + 1).padStart(2)}.${COLORS.reset} ${formatHealthLine(f.currentScore, f.currentState)}  ${f.path}`,
    );
  }

  return true;
}

function queryBest(db: DbHandle, limit: number): boolean {
  const files = db.getBestFiles(limit);
  if (files.length === 0) {
    console.log(`${COLORS.dim}No scored files tracked.${COLORS.reset}`);
    return true;
  }

  console.log(`${COLORS.bold}Best health files:${COLORS.reset} (${files.length})\n`);

  for (let i = 0; i < files.length; i++) {
    const f = files[i]!;
    console.log(
      `  ${COLORS.dim}${String(i + 1).padStart(2)}.${COLORS.reset} ${formatHealthLine(f.currentScore, f.currentState)}  ${f.path}`,
    );
  }

  return true;
}

function queryPatterns(db: DbHandle, limit: number): boolean {
  const patterns = db.getTopPatterns(limit);
  if (patterns.length === 0) {
    console.log(`${COLORS.dim}No patterns tracked yet.${COLORS.reset}`);
    return true;
  }

  // Also get trending patterns
  const trending = db.getTrendingPatterns(limit);
  const trendingCodes = new Set(trending.map((p) => `${p.errorCode}:${p.rule}`));

  console.log(`${COLORS.bold}Top recurring issues:${COLORS.reset} (${patterns.length})\n`);

  for (let i = 0; i < patterns.length; i++) {
    const p = patterns[i]!;
    const trendIcon = trendingCodes.has(`${p.errorCode}:${p.rule}`)
      ? `${COLORS.red}\u2191${COLORS.reset}` // ↑
      : p.trending === "decreasing"
        ? `${COLORS.green}\u2193${COLORS.reset}` // ↓
        : `${COLORS.dim}\u2192${COLORS.reset}`; // →

    console.log(
      `  ${COLORS.dim}${String(i + 1).padStart(2)}.${COLORS.reset} ` +
      `${COLORS.bold}${p.errorCode}${COLORS.reset} (${p.rule}) ` +
      `\u2014 ${p.affectedFiles} file${p.affectedFiles !== 1 ? "s" : ""}, ` +
      `${p.frequency}x ${trendIcon}`,
    );
  }

  return true;
}

function queryTrend(db: DbHandle, opts: CliOptions): boolean {
  const filePath = opts.targets[0];
  if (!filePath) {
    console.error(`${COLORS.red}File path required. Usage: ${TOOL_NAME} query trend <path> --db <db>${COLORS.reset}`);
    return false;
  }

  const history = db.getHealthHistory(filePath, 20);
  if (history.length === 0) {
    console.error(`${COLORS.yellow}No health history for: ${filePath}${COLORS.reset}`);
    return false;
  }

  console.log(`${COLORS.bold}Health trend:${COLORS.reset} ${filePath}\n`);

  // Show newest first (reverse chronological is more scannable)
  const reversed = [...history].reverse();
  for (const h of reversed) {
    const delta = h.delta !== null && h.delta !== 0
      ? ` (${h.delta > 0 ? "+" : ""}${h.delta})`
      : "";
    const dateStr = h.timestamp.split("T")[0] ?? h.timestamp;
    console.log(
      `  ${COLORS.dim}${dateStr}${COLORS.reset}  ${formatHealthLine(h.score, h.hebrewState)}${delta}`,
    );
  }

  // Overall trend summary
  if (history.length >= 2) {
    const first = history[history.length - 1]!; // oldest
    const last = history[0]!; // newest
    const totalDelta = last.score - first.score;
    const deltaColor = totalDelta > 0 ? COLORS.green : totalDelta < 0 ? COLORS.red : COLORS.dim;
    console.log(
      `\n  ${COLORS.bold}Net:${COLORS.reset} ${deltaColor}${totalDelta > 0 ? "+" : ""}${totalDelta}${COLORS.reset} over ${history.length} entries`,
    );
  }

  return true;
}

function queryInstance(db: DbHandle, opts: CliOptions): boolean {
  const name = opts.targets[0];
  if (!name) {
    console.error(`${COLORS.red}Instance name required. Usage: ${TOOL_NAME} query instance <name> --db <db>${COLORS.reset}`);
    return false;
  }

  const activity = db.getInstanceActivity(name);
  if (!activity) {
    console.error(`${COLORS.yellow}No data for instance: ${name}${COLORS.reset}`);
    return false;
  }

  const inst = activity.instance;
  console.log(`${COLORS.bold}Instance:${COLORS.reset} ${inst.name} (${inst.type})`);
  console.log(`  First seen: ${COLORS.dim}${inst.firstSeen}${COLORS.reset}`);
  console.log(`  Runs: ${inst.runsCount}  Sessions: ${activity.sessionsCount}`);
  if (inst.lastActive) {
    console.log(`  Last active: ${COLORS.dim}${inst.lastActive}${COLORS.reset}`);
  }

  if (activity.recentRuns.length > 0) {
    console.log(`\n${COLORS.bold}Recent runs:${COLORS.reset}`);
    for (const run of activity.recentRuns.slice(0, 5)) {
      const scoreStr = run.totalScore !== null
        ? formatHealthLine(run.totalScore, run.hebrewState)
        : `${COLORS.dim}(no score)${COLORS.reset}`;
      const file = db.getFileById(run.fileId);
      const filePath = file?.path ?? `file#${run.fileId}`;
      console.log(
        `  ${COLORS.dim}${run.timestamp}${COLORS.reset}  ${run.errorCount}E ${run.warnCount}W  ${scoreStr}  ${COLORS.dim}${filePath}${COLORS.reset}`,
      );
    }
  }

  return true;
}

function querySession(db: DbHandle, opts: CliOptions): boolean {
  const sessionId = opts.targets[0];
  if (!sessionId) {
    console.error(`${COLORS.red}Session ID required. Usage: ${TOOL_NAME} query session <id> --db <db>${COLORS.reset}`);
    return false;
  }

  const summary = db.getSessionSummary(sessionId);
  if (!summary) {
    console.error(`${COLORS.yellow}No data for session: ${sessionId}${COLORS.reset}`);
    return false;
  }

  const s = summary.session;
  console.log(`${COLORS.bold}Session:${COLORS.reset} ${s.id}`);
  if (s.instanceName) {
    console.log(`  Instance: ${s.instanceName}`);
  }
  console.log(`  Started: ${COLORS.dim}${s.startTime}${COLORS.reset}`);
  if (s.endTime) {
    console.log(`  Ended: ${COLORS.dim}${s.endTime}${COLORS.reset}`);
  }
  console.log(`  Files linted: ${s.filesLinted}  Health delta: ${s.healthDelta > 0 ? "+" : ""}${s.healthDelta}`);

  if (summary.runs.length > 0) {
    console.log(`\n${COLORS.bold}Runs:${COLORS.reset} ${summary.runs.length}`);
    for (const run of summary.runs.slice(0, 10)) {
      const file = db.getFileById(run.fileId);
      const filePath = file?.path ?? `file#${run.fileId}`;
      console.log(
        `  ${COLORS.dim}${run.timestamp}${COLORS.reset}  ${run.errorCount}E ${run.warnCount}W  ${COLORS.dim}${filePath}${COLORS.reset}`,
      );
    }
  }

  if (summary.patternsFound.length > 0) {
    console.log(`\n${COLORS.bold}Patterns found:${COLORS.reset} ${summary.patternsFound.join(", ")}`);
  }

  return true;
}

function queryStats(db: DbHandle): boolean {
  const stats = db.getStats();

  console.log(`${COLORS.bold}Database statistics:${COLORS.reset}\n`);
  console.log(`  Files tracked: ${stats.totalFiles}`);
  console.log(`  Total runs: ${stats.totalRuns}`);

  if (stats.avgScore !== null) {
    console.log(`  Avg health: ${formatHealthLine(stats.avgScore, null)}`);
    console.log(`  Healthy (>=34): ${COLORS.green}${stats.healthyFiles}${COLORS.reset}`);
    console.log(`  Unhealthy (<0): ${COLORS.red}${stats.unhealthyFiles}${COLORS.reset}`);
  } else {
    console.log(`  ${COLORS.dim}No health scores recorded yet.${COLORS.reset}`);
  }

  return true;
}

// ---------------------------------------------------------------------------
// Database recording — wire into lint/transform pipelines
// ---------------------------------------------------------------------------

/** Database handle type — lazy-loaded, so we use the Awaited module type. */
type DbModule = Awaited<ReturnType<typeof getDbModule>>;
type DbHandle = InstanceType<DbModule["CwsDatabase"]>;

/**
 * Open a database connection and return it. Returns null if --db not specified.
 * "The books were opened." — Revelation 20:12
 */
async function openDatabase(opts: CliOptions): Promise<DbHandle | null> {
  if (!opts.dbPath) return null;

  try {
    const mod = await getDbModule();
    return new mod.CwsDatabase(undefined, opts.dbPath);
  } catch (e) {
    console.error(
      `${COLORS.yellow}Warning: Could not open database ${opts.dbPath}: ${e instanceof Error ? e.message : String(e)}${COLORS.reset}`,
    );
    return null;
  }
}

/**
 * Record lint summaries to the database.
 * Handles the full lifecycle: update file record, record lint run, record actions,
 * record health history. Called after each lint batch.
 */
function recordToDatabase(
  db: DbHandle,
  summaries: LintSummary[],
  formatName: string,
  instanceName: string,
  sessionId?: string,
): void {
  try {
    db.recordBatch(
      summaries.map((s) => ({
        filePath: s.file,
        format: formatName,
        summary: s,
        handler: formatName,
        instanceName,
        instanceType: "cpisi",
        sessionId,
      })),
    );
  } catch (e) {
    console.error(
      `${COLORS.yellow}Warning: Database recording failed: ${e instanceof Error ? e.message : String(e)}${COLORS.reset}`,
    );
  }
}

// ---------------------------------------------------------------------------
// Diff-aware linting — only lint files that changed
// ---------------------------------------------------------------------------

/**
 * Get files changed according to git.
 *
 * Modes:
 * - staged: `git diff --name-only --cached`
 * - since <ref>: `git diff --name-only <ref>`
 * - default: `git diff --name-only HEAD` (uncommitted changes)
 *
 * Returns absolute paths. Filters to existing files only.
 */
async function getChangedFiles(opts: {
  staged?: boolean;
  since?: string;
  targets: string[];
}): Promise<string[]> {
  const args = ["diff", "--name-only"];

  if (opts.staged) {
    args.push("--cached");
  } else if (opts.since) {
    args.push(opts.since);
  } else {
    args.push("HEAD");
  }

  // Add target directories as pathspec (after --)
  if (opts.targets.length > 0) {
    args.push("--", ...opts.targets);
  }

  try {
    const cmd = new Deno.Command("git", { args, stdout: "piped", stderr: "piped" });
    const { code, stdout, stderr } = await cmd.output();

    if (code !== 0) {
      const errText = new TextDecoder().decode(stderr).trim();
      console.error(
        `${COLORS.yellow}Warning: git diff failed (${errText}). Falling back to full lint.${COLORS.reset}`,
      );
      return [];
    }

    const output = new TextDecoder().decode(stdout).trim();
    if (!output) return [];

    const cwd = Deno.cwd();
    const paths: string[] = [];

    for (const line of output.split("\n")) {
      const trimmed = line.trim();
      if (!trimmed) continue;

      // git outputs paths relative to repo root — resolve to absolute
      const abs = trimmed.startsWith("/") ? trimmed : `${cwd}/${trimmed}`;
      try {
        await Deno.stat(abs);
        paths.push(abs);
      } catch {
        // File was deleted — skip
      }
    }

    return paths;
  } catch {
    console.error(
      `${COLORS.yellow}Warning: Could not run git. Falling back to full lint.${COLORS.reset}`,
    );
    return [];
  }
}

// ---------------------------------------------------------------------------
// Watch mode — re-lint on file changes
// ---------------------------------------------------------------------------

/**
 * Watch directories for file changes and re-lint automatically.
 * Debounces rapid changes (300ms) and shows compact delta output.
 *
 * "Watch ye, stand fast in the faith." — 1 Corinthians 16:13
 */
async function runWatch(opts: CliOptions): Promise<void> {
  if (opts.targets.length === 0) {
    console.error(
      `${COLORS.red}No targets specified. Provide directories to watch.${COLORS.reset}`,
    );
    Deno.exit(EXIT_TOOL_ERROR);
  }

  const db = await openDatabase(opts);
  const instanceName = db ? (await getDbModule()).detectInstance().name : "";

  // Build a set of valid extensions from the format filter or all registered formats
  const validExtensions = new Set<string>();
  if (opts.format) {
    const handler = getFormat(opts.format);
    if (!handler) {
      console.error(`${COLORS.red}Unknown format: ${opts.format}${COLORS.reset}`);
      Deno.exit(EXIT_TOOL_ERROR);
    }
    for (const ext of handler.extensions) validExtensions.add(ext);
  } else {
    for (const detail of listFormatDetails()) {
      for (const ext of detail.extensions) validExtensions.add(ext);
    }
  }

  // Track previous health scores for delta display
  const prevScores = new Map<string, number | undefined>();

  console.log(
    `${COLORS.bold}${TOOL_NAME}${COLORS.reset} watching ${opts.targets.join(", ")}` +
    (opts.format ? ` (${opts.format} only)` : "") +
    `\n${COLORS.dim}Press Ctrl+C to stop.${COLORS.reset}\n`,
  );

  // Debounce: collect changed paths, lint after 300ms quiet
  let debounceTimer: number | undefined;
  const pending = new Set<string>();

  const watcher = Deno.watchFs(opts.targets, { recursive: true });

  for await (const event of watcher) {
    if (event.kind !== "modify" && event.kind !== "create") continue;

    for (const path of event.paths) {
      // Filter by extension
      const ext = "." + path.split(".").pop();
      if (!validExtensions.has(ext)) continue;
      pending.add(path);
    }

    // Debounce — wait 300ms after last change
    if (debounceTimer !== undefined) clearTimeout(debounceTimer);
    debounceTimer = setTimeout(async () => {
      const files = [...pending];
      pending.clear();

      for (const file of files) {
        const relPath = relative(Deno.cwd(), file);
        const now = new Date();
        const timeStr = `${String(now.getHours()).padStart(2, "0")}:${String(now.getMinutes()).padStart(2, "0")}:${String(now.getSeconds()).padStart(2, "0")}`;

        try {
          // Detect format from extension
          const ext = "." + file.split(".").pop();
          let handler: FormatHandler | undefined;
          if (opts.format) {
            handler = getFormat(opts.format);
          } else {
            // Find handler by extension
            for (const detail of listFormatDetails()) {
              if (detail.extensions.includes(ext)) {
                handler = getFormat(detail.name);
                break;
              }
            }
          }

          if (!handler) continue;

          // Lint the single file
          const results = await handler.lint(file);
          resolveErrorCodes(results);
          const health = handler.computeHealth
            ? await handler.computeHealth(file, results)
            : undefined;
          const summary = summarize(relPath, results, health);

          // Build compact output line
          const prev = prevScores.get(file);
          const score = health?.total;
          const level = health ? trueToLevel(health.total) : undefined;
          const emoji = level ? levelToEmoji(level) : "";

          let delta = "";
          if (score !== undefined && prev !== undefined) {
            const diff = score - prev;
            if (diff > 0) delta = ` ${COLORS.green}(+${diff})${COLORS.reset}`;
            else if (diff < 0) delta = ` ${COLORS.red}(${diff})${COLORS.reset}`;
            else delta = ` (no change)`;
          }

          const scoreStr = score !== undefined
            ? `${score} ${level} ${emoji}`
            : `${summary.errors}E ${summary.warnings}W`;

          const statusColor = summary.errors > 0 ? COLORS.red : COLORS.green;
          console.log(
            `${COLORS.dim}[${timeStr}]${COLORS.reset} ${statusColor}${relPath}${COLORS.reset} → ${scoreStr}${delta}`,
          );

          prevScores.set(file, score);

          // Record to database if enabled
          if (db) {
            recordToDatabase(db, [summary], handler.name, instanceName);
          }
        } catch (e) {
          console.error(
            `${COLORS.dim}[${timeStr}]${COLORS.reset} ${COLORS.red}${relPath}: ${e instanceof Error ? e.message : String(e)}${COLORS.reset}`,
          );
        }
      }
    }, 300) as unknown as number;
  }
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
  "--force",
  "--steps",
  "--policy",
  "--port",
  "--key",
  "--title",
  "--purpose",
  "--log",
  "--check",
  "--db",
  "--limit",
  "--debug",
  "--trace",
  "--inspect",
  "--why",
  "--changed",
  "--since",
  "--staged",
  "--suggest",
  "--deep", "-vv",
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
  // Load tool configuration from deno.jsonc. Tripwire: missing → defaults.
  const config = await loadConfig();
  VERSION = config.version;
  LINT_CONCURRENCY = config.concurrency;
  setImpactWeights(config.impactWeights);
  setDiscoveryLimits({
    maxFiles: config.maxFiles,
    maxFileSize: config.maxFileSize,
    excludedDirs: config.excludedDirs,
  });

  // Input validation
  const validationError = validateArgs(Deno.args);
  if (validationError) {
    console.error(`${COLORS.red}${TOOL_NAME}: ${validationError}${COLORS.reset}`);
    Deno.exit(EXIT_TOOL_ERROR);
  }

  const opts = parseArgs(Deno.args);

  // Initialize debug/trace output (must be before any debug calls).
  initDebug(opts);
  debugConfig(config);
  debugPolicy(opts.policy);

  // Set session-wide ternary policy before dispatching to handlers.
  setGlobalPolicy(opts.policy);

  // Set session-wide lint grid focus before dispatching to handlers.
  // Handlers call getLintFocus() inside runLintGrid() to read the focus.
  setLintFocus(parseLintFocus(opts.check));

  switch (opts.command) {
    case "help":
      showHelp(opts.helpTopic);
      Deno.exit(EXIT_OK);
      break;

    case "version":
      console.log(`${TOOL_NAME} v${VERSION}`);
      Deno.exit(EXIT_OK);
      break;

    case "lint": {
      const db = await openDatabase(opts);
      const ok = await runLint(opts, db);
      if (db) {
        // Update pattern trends after the full lint run
        try {
          db.recalculateAffectedFiles();
          db.updateTrends();
        } catch { /* best-effort pattern analysis */ }
        console.log(`${COLORS.dim}Database: ${opts.dbPath} (${db.getStats().totalRuns} runs recorded)${COLORS.reset}`);
        db.close();
      }
      Deno.exit(ok ? EXIT_OK : EXIT_LINT_ERRORS);
      break;
    }

    case "transform": {
      const ok = await runTransform(opts);
      Deno.exit(ok ? EXIT_OK : EXIT_LINT_ERRORS);
      break;
    }

    case "query": {
      const ok = await runQuery(opts);
      Deno.exit(ok ? EXIT_OK : EXIT_LINT_ERRORS);
      break;
    }

    case "create": {
      const ok = await runCreate(opts);
      Deno.exit(ok ? EXIT_OK : EXIT_LINT_ERRORS);
      break;
    }

    case "transpile": {
      const ok = await runTranspile(opts);
      Deno.exit(ok ? EXIT_OK : EXIT_LINT_ERRORS);
      break;
    }

    case "watch": {
      await runWatch(opts);
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

    case "studio": {
      const portIdx = Deno.args.indexOf("--port");
      const port = portIdx >= 0 && Deno.args[portIdx + 1]
        ? parseInt(Deno.args[portIdx + 1]!, 10)
        : 4200;
      startStudio(port);
      // Studio runs indefinitely — Deno.serve keeps the process alive
      break;
    }
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
