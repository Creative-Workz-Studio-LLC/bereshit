// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/commands/help.ts
// key:     B-tov-cws-struct-lib-commands-help
// title:   CWS Struct — Help System
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: All help output functions extracted from mod.ts God Object.
//          Pure display — no side effects, no shared state.
//          Accepts version/toolName via setToolInfo() for runtime config.
//
// biblical_foundation: "Give instruction to a wise man, and he will be yet
//   wiser: teach a just man, and he will increase in learning." — Proverbs 9:9
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import {
  COLORS,
  listFormatDetails,
} from "../engine/mod.ts";
import { TOOL_NAME, VERSION } from "./config.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Help topics — each is a self-contained page
// ---------------------------------------------------------------------------

/** Help topics — each is a self-contained page. */
export const HELP_TOPICS: Record<string, () => void> = {
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

export function showHelp(topic?: string): void {
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
    ID:     020 (catalog offset)

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

export function showFormats(): void {
  const formats = listFormatDetails();
  console.log(`${COLORS.bold}Registered formats:${COLORS.reset}\n`);
  for (const f of formats) {
    console.log(`  ${COLORS.bold}${f.name}${COLORS.reset}`);
    console.log(`    Extensions: ${f.extensions.join(", ")}`);
    console.log(`    ${f.description}\n`);
  }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Help system — pure display, no side effects.
// All text content lives here; mod.ts dispatches.
//
// "Give instruction to a wise man, and he will be yet wiser." — Proverbs 9:9
// ============================================================================
