#!/usr/bin/env -S deno run --allow-read
// ============================================================================
// METADATA
// ============================================================================
//
// file:    lint-toml.ts
// key:     B-tools-lint-toml
// title:   TOML Template Alignment Linter
// type:    Tool (Linter)
// version: a-02.00
// created: 2026-02-10
// updated: 2026-02-10
// authors: Nova Dawn (CPI-SI)
// purpose: Validate TOML data files against the OmniCode 3-block template.
//          Phase 1: Structural alignment — pragma, metadata (M1-M9), content
//          structure (Cc/Co/Cv), extensions, validation, closing (X1-X5).
//          Phase 2: Cross-reference validation — internal consistency checks
//          derived from 45+ aligned files (key/title/filename matching,
//          schema/required_sections agreement, domain conventions).
//
// biblical_foundation: "Prove all things; hold fast that which is good."
//                       — 1 Thessalonians 5:21
//
// usage:   deno run --allow-read lint-toml.ts <file-or-directory> [--fix]
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { parse as parseToml } from "jsr:@std/toml@1";
import { walk } from "jsr:@std/fs@1/walk";
import { resolve, basename, relative } from "jsr:@std/path@1";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const VERSION = "a-02.00";

const REQUIRED_METADATA = [
  "M1_identity",
  "M2_state",
  "M3_attribution",
  "M4_grounding",
  "M5_dependencies",
] as const;

const EXTENSION_METADATA = [
  "M6_location",
  "M7_classification",
  "M8_intent",
  "M9_roadmap",
  "M_extension_note",
] as const;

const REQUIRED_CLOSING = [
  "X1_policy",
  "X4_reference",
  "X5_note",
] as const;

const EXTENSION_CLOSING = [
  "X2_extension",
  "X3_troubleshooting",
] as const;

const M5_ENRICHMENT_KEYS = [
  "integration",
  "if_missing",
] as const;

const CROSS_FORMAT_KEYS = [
  "jsonc",
  "json",
  "relationship",
] as const;

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

type Severity = "error" | "warn" | "info";

interface LintResult {
  file: string;
  severity: Severity;
  rule: string;
  message: string;
}

interface LintSummary {
  file: string;
  errors: number;
  warnings: number;
  infos: number;
  results: LintResult[];
}

// deno-lint-ignore no-explicit-any
type TomlData = Record<string, any>;

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helpers — TOML access
// ---------------------------------------------------------------------------

function has(obj: TomlData, key: string): boolean {
  return obj != null && typeof obj === "object" && key in obj;
}

function getTable(obj: TomlData, key: string): TomlData | undefined {
  const val = obj?.[key];
  return val != null && typeof val === "object" && !Array.isArray(val)
    ? val
    : undefined;
}

// ---------------------------------------------------------------------------
// Helpers — Result building
// ---------------------------------------------------------------------------

function error(file: string, rule: string, message: string): LintResult {
  return { file, severity: "error", rule, message };
}

function warn(file: string, rule: string, message: string): LintResult {
  return { file, severity: "warn", rule, message };
}

function info(file: string, rule: string, message: string): LintResult {
  return { file, severity: "info", rule, message };
}

// ---------------------------------------------------------------------------
// Core — Block structure checks
// ---------------------------------------------------------------------------

function checkPragma(file: string, data: TomlData): LintResult[] {
  const results: LintResult[] = [];
  const pragma = getTable(data, "_pragma");

  if (!pragma) {
    results.push(error(file, "pragma/exists", "Missing [_pragma] table"));
    return results;
  }

  if (!has(pragma, "format")) {
    results.push(error(file, "pragma/format", "Missing _pragma.format"));
  }

  const requiredPragma = ["P1_core", "P2_family", "P3_instance", "P4_architecture", "P5_summary"];
  for (const key of requiredPragma) {
    if (!has(pragma, key)) {
      results.push(error(file, `pragma/${key}`, `Missing _pragma.${key}`));
    }
  }

  if (!has(pragma, "legend")) {
    results.push(warn(file, "pragma/legend", "Missing _pragma.legend — recommended for documenting field requirements"));
  }

  return results;
}

function checkMetadata(file: string, data: TomlData): LintResult[] {
  const results: LintResult[] = [];
  const meta = getTable(data, "_metadata");

  if (!meta) {
    results.push(error(file, "metadata/exists", "Missing [_metadata] table"));
    return results;
  }

  // Required M1-M5
  for (const key of REQUIRED_METADATA) {
    if (!has(meta, key)) {
      results.push(error(file, `metadata/${key}`, `Missing _metadata.${key} — REQUIRED`));
    }
  }

  // Extension M6-M9
  for (const key of EXTENSION_METADATA) {
    if (!has(meta, key)) {
      results.push(warn(file, `metadata/${key}`, `Missing _metadata.${key} — extension metadata`));
    }
  }

  // M5 enrichment
  const m5 = getTable(meta, "M5_dependencies");
  if (m5) {
    for (const key of M5_ENRICHMENT_KEYS) {
      if (!has(m5, key)) {
        results.push(warn(file, `metadata/M5.${key}`, `Missing M5_dependencies.${key}`));
      }
    }

    // cross_format enrichment
    const cf = getTable(m5, "cross_format");
    if (!cf) {
      results.push(warn(file, "metadata/M5.cross_format", "Missing M5_dependencies.cross_format"));
    } else {
      for (const key of CROSS_FORMAT_KEYS) {
        if (!has(cf, key)) {
          results.push(warn(file, `metadata/M5.cross_format.${key}`, `Missing cross_format.${key}`));
        }
      }
      // Check for legacy "note" field (should be replaced by jsonc/json/relationship)
      if (has(cf, "note") && !has(cf, "jsonc")) {
        results.push(warn(file, "metadata/M5.cross_format.legacy", "cross_format has legacy 'note' — should use jsonc/json/relationship"));
      }
    }

    // split_context vs split_from
    if (has(m5, "split_from") && !getTable(m5, "split_context")) {
      results.push(warn(file, "metadata/M5.split_context", "Has flat split_from — should use [split_context] sub-table with parent + sections"));
    }
    if (getTable(m5, "split_context")) {
      const sc = getTable(m5, "split_context")!;
      if (!has(sc, "parent")) {
        results.push(error(file, "metadata/M5.split_context.parent", "split_context missing 'parent' field"));
      }
      if (!has(sc, "sections")) {
        results.push(error(file, "metadata/M5.split_context.sections", "split_context missing 'sections' field"));
      }
    }
  }

  // M1 identity fields
  const m1 = getTable(meta, "M1_identity");
  if (m1) {
    const requiredM1 = ["file", "key", "title", "type"];
    for (const key of requiredM1) {
      if (!has(m1, key)) {
        results.push(error(file, `metadata/M1.${key}`, `Missing M1_identity.${key}`));
      }
    }
  }

  // M2 state fields
  const m2 = getTable(meta, "M2_state");
  if (m2) {
    const requiredM2 = ["version", "status"];
    for (const key of requiredM2) {
      if (!has(m2, key)) {
        results.push(error(file, `metadata/M2.${key}`, `Missing M2_state.${key}`));
      }
    }
  }

  // M4 grounding fields
  const m4 = getTable(meta, "M4_grounding");
  if (m4) {
    if (!has(m4, "scripture")) {
      results.push(warn(file, "metadata/M4.scripture", "Missing M4_grounding.scripture — biblical anchor"));
    }
  }

  return results;
}

function checkContent(file: string, data: TomlData): LintResult[] {
  const results: LintResult[] = [];

  if (!has(data, "_content")) {
    results.push(warn(file, "content/exists", "Missing [_content] table — Cc configuration block"));
  } else {
    const content = getTable(data, "_content");
    if (content) {
      if (!has(content, "format")) {
        results.push(warn(file, "content/format", "Missing _content.format"));
      }
      if (!has(content, "key_convention")) {
        results.push(warn(file, "content/key_convention", "Missing _content.key_convention"));
      }
      if (!has(content, "constraint")) {
        results.push(warn(file, "content/constraint", "Missing _content.constraint — domain-specific constraint"));
      }
    }
  }

  if (!has(data, "extensions")) {
    results.push(warn(file, "content/extensions", "Missing [extensions] table — reserve for future extensions"));
  }

  if (!has(data, "_validation")) {
    results.push(warn(file, "content/validation", "Missing [_validation] table — Cv validation block"));
  } else {
    const val = getTable(data, "_validation");
    if (val) {
      if (!has(val, "commands")) {
        results.push(warn(file, "content/validation.commands", "Missing _validation.commands"));
      }
      if (!getTable(val, "schema")) {
        results.push(warn(file, "content/validation.schema", "Missing _validation.schema"));
      }
      if (!getTable(val, "health_scoring")) {
        results.push(warn(file, "content/validation.health_scoring", "Missing _validation.health_scoring"));
      }
    }
  }

  return results;
}

function checkClosing(file: string, data: TomlData): LintResult[] {
  const results: LintResult[] = [];
  const closing = getTable(data, "_closing");

  if (!closing) {
    results.push(error(file, "closing/exists", "Missing [_closing] table"));
    return results;
  }

  // Required X1, X4, X5
  for (const key of REQUIRED_CLOSING) {
    if (!has(closing, key)) {
      results.push(error(file, `closing/${key}`, `Missing _closing.${key} — REQUIRED`));
    }
  }

  // Extension X2, X3
  for (const key of EXTENSION_CLOSING) {
    if (!has(closing, key)) {
      results.push(warn(file, `closing/${key}`, `Missing _closing.${key} — extension closing`));
    }
  }

  // X4 enrichment
  const x4 = getTable(closing, "X4_reference");
  if (x4) {
    if (!has(x4, "required_sections")) {
      results.push(warn(file, "closing/X4.required_sections", "Missing X4_reference.required_sections"));
    }
    if (!has(x4, "validate")) {
      results.push(warn(file, "closing/X4.validate", "Missing X4_reference.validate"));
    }
  }

  // X5 enrichment
  const x5 = getTable(closing, "X5_note");
  if (x5) {
    if (!has(x5, "anchor")) {
      results.push(warn(file, "closing/X5.anchor", "Missing X5_note.anchor — Genesis 1:1 connection"));
    }
    if (!has(x5, "scripture")) {
      results.push(warn(file, "closing/X5.scripture", "Missing X5_note.scripture"));
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Cross-reference validation (domain knowledge)
// ---------------------------------------------------------------------------
// These checks leverage patterns observed across 45+ aligned files.
// They verify internal consistency — that values which SHOULD agree DO agree.

function checkCrossReferences(file: string, data: TomlData): LintResult[] {
  const results: LintResult[] = [];
  const pragma = getTable(data, "_pragma");
  const meta = getTable(data, "_metadata");
  const validation = getTable(data, "_validation");
  const closing = getTable(data, "_closing");

  // 1. Pragma.P1_core.key === Metadata.M1_identity.key
  const p1 = pragma ? getTable(pragma, "P1_core") : undefined;
  const m1 = meta ? getTable(meta, "M1_identity") : undefined;
  if (p1?.key && m1?.key && p1.key !== m1.key) {
    results.push(error(file, "xref/key-mismatch",
      `P1_core.key "${p1.key}" !== M1_identity.key "${m1.key}" — must match`));
  }

  // 2. Pragma.P5_summary.title === Metadata.M1_identity.title
  const p5 = pragma ? getTable(pragma, "P5_summary") : undefined;
  if (p5?.title && m1?.title && p5.title !== m1.title) {
    results.push(error(file, "xref/title-mismatch",
      `P5_summary.title "${p5.title}" !== M1_identity.title "${m1.title}" — must match`));
  }

  // 3. M1_identity.file matches actual filename
  if (m1?.file && m1.file !== file) {
    results.push(warn(file, "xref/filename-mismatch",
      `M1_identity.file "${m1.file}" !== actual filename "${file}"`));
  }

  // 4. _validation.schema.required ⊆ _closing.X4_reference.required_sections
  //    required_sections may include infrastructure tables (_pragma, _metadata, etc.)
  //    or may list domain tables only (newer convention). Either way, the domain
  //    tables in schema.required should all appear in required_sections.
  const valSchema = validation ? getTable(validation, "schema") : undefined;
  const x4 = closing ? getTable(closing, "X4_reference") : undefined;
  if (valSchema?.required && x4?.required_sections) {
    const schemaReq = Array.isArray(valSchema.required) ? valSchema.required as string[] : [];
    const closingReq = Array.isArray(x4.required_sections) ? x4.required_sections as string[] : [];
    // Strip infrastructure tables from required_sections for comparison
    const closingDomain = closingReq.filter((s: string) => !s.startsWith("_"));
    const schemaSorted = [...schemaReq].sort();
    const closingSorted = [...closingDomain].sort();
    if (JSON.stringify(schemaSorted) !== JSON.stringify(closingSorted)) {
      results.push(warn(file, "xref/required-sections-mismatch",
        `Domain tables in schema.required [${schemaSorted.join(", ")}] !== ` +
        `X4_reference.required_sections [${closingSorted.join(", ")}]`));
    }
  }

  // 5. _validation.commands ↔ _closing.X4_reference.validate
  //    commands may be string or array; validate is typically string.
  //    The validate command should appear somewhere in commands.
  if (validation?.commands && x4?.validate) {
    const cmds = Array.isArray(validation.commands)
      ? validation.commands as string[]
      : [validation.commands as string];
    const valCmd = String(x4.validate);
    const match = cmds.some((c: string) => c === valCmd) || cmds.length === 1 && valCmd.includes("tomllib");
    if (!match) {
      results.push(info(file, "xref/validate-command-diff",
        `_validation.commands and X4_reference.validate use different commands — verify intentional`));
    }
  }

  // 6. M6_location.derives_from === P1_core.from
  const m6 = meta ? getTable(meta, "M6_location") : undefined;
  if (m6?.derives_from && p1?.from && m6.derives_from !== p1.from) {
    results.push(warn(file, "xref/derives-from-mismatch",
      `M6_location.derives_from "${m6.derives_from}" !== P1_core.from "${p1.from}"`));
  }

  // 7. X1_policy structure — must have never/careful/safe arrays
  const x1 = closing ? getTable(closing, "X1_policy") : undefined;
  if (x1) {
    for (const field of ["never", "careful", "safe"]) {
      if (!has(x1, field)) {
        results.push(warn(file, `xref/X1.${field}`,
          `X1_policy missing '${field}' array — policy should define never/careful/safe boundaries`));
      } else if (!Array.isArray(x1[field])) {
        results.push(warn(file, `xref/X1.${field}-type`,
          `X1_policy.${field} should be an array`));
      }
    }
    if (!has(x1, "scripture")) {
      results.push(info(file, "xref/X1.scripture",
        "X1_policy missing scripture — typically 'Remove not the ancient landmark — Proverbs 22:28'"));
    }
  }

  // 8. X5_note.anchor should reference Genesis 1:1
  const x5 = closing ? getTable(closing, "X5_note") : undefined;
  if (x5?.anchor && typeof x5.anchor === "string") {
    const anchor = x5.anchor.toLowerCase();
    if (!anchor.includes("genesis 1:1") && !anchor.includes("genesis 1") && !anchor.includes("in the beginning")) {
      results.push(info(file, "xref/X5.genesis-anchor",
        "X5_note.anchor doesn't reference Genesis 1:1 — all anchors should trace to foundation"));
    }
  }

  // 9. P2_family structure consistency
  const p2 = pragma ? getTable(pragma, "P2_family") : undefined;
  if (p2) {
    if (!has(p2, "structure")) {
      results.push(warn(file, "xref/P2.structure",
        "P2_family missing 'structure' — should specify block pattern (e.g., '3-block')"));
    }
    if (p2.structure && p2.structure !== "3-block") {
      results.push(info(file, "xref/P2.structure-type",
        `P2_family.structure is "${p2.structure}" — TOML data files should be 3-block`));
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Domain convention checks
// ---------------------------------------------------------------------------
// Verify that underscore prefix convention is respected:
//   _prefix = OmniCode infrastructure only (_pragma, _metadata, _content, _validation, _closing)
//   no prefix = domain content tables

const OMNICODE_INFRA_TABLES = new Set([
  "_pragma", "_metadata", "_content", "_validation", "_closing",
]);

function checkDomainConventions(file: string, data: TomlData): LintResult[] {
  const results: LintResult[] = [];

  for (const key of Object.keys(data)) {
    if (key.startsWith("_") && !OMNICODE_INFRA_TABLES.has(key)) {
      results.push(warn(file, "domain/underscore-prefix",
        `Table [${key}] uses underscore prefix but isn't OmniCode infrastructure — ` +
        `reserve _ prefix for _pragma/_metadata/_content/_validation/_closing`));
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Comment structure checks (raw text)
// ---------------------------------------------------------------------------

function checkComments(file: string, raw: string): LintResult[] {
  const results: LintResult[] = [];

  // Block boundary markers
  const expectedMarkers = [
    { marker: "OMNICODE PRAGMA [PRAGMA]", required: true },
    { marker: "END PRAGMA [END]", required: true },
    { marker: "METADATA BLOCK [METADATA]", required: true },
    { marker: "END METADATA [END]", required: true },
    { marker: "CONTENT BLOCK [CONTENT]", required: true },
    { marker: "END CONTENT [END]", required: true },
    { marker: "CLOSING BLOCK [CLOSING]", required: true },
    { marker: "END CLOSING [END]", required: true },
  ];

  for (const { marker, required } of expectedMarkers) {
    if (!raw.includes(marker)) {
      const severity = required ? "error" : "warn";
      results.push({
        file,
        severity,
        rule: "comments/block-marker",
        message: `Missing block boundary marker: ${marker}`,
      });
    }
  }

  // Content labels
  const contentLabels = [
    { label: "# Cc —", name: "Content Configurations" },
    { label: "# Co —", name: "Content Operations" },
    { label: "# Cv —", name: "Content Validation" },
  ];

  for (const { label, name } of contentLabels) {
    if (!raw.includes(label)) {
      results.push(info(file, "comments/content-label", `Missing ${label} ${name} label comment`));
    }
  }

  // Check pragma line
  if (!raw.startsWith("# #!omni")) {
    results.push(error(file, "comments/pragma-line", "File must start with '# #!omni data --toml'"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Index file detection and checks
// ---------------------------------------------------------------------------

function isIndexFile(data: TomlData): boolean {
  return has(data, "index") && getTable(data, "index")?.type === "split_index";
}

function checkIndex(file: string, data: TomlData): LintResult[] {
  const results: LintResult[] = [];
  const index = getTable(data, "index");

  if (!index) {
    results.push(error(file, "index/exists", "Missing [index] table in index file"));
    return results;
  }

  if (!has(index, "type")) {
    results.push(error(file, "index/type", "Missing index.type"));
  }
  if (!has(index, "total_files")) {
    results.push(warn(file, "index/total_files", "Missing index.total_files"));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — File linting orchestrator
// ---------------------------------------------------------------------------

async function lintFile(filePath: string): Promise<LintSummary> {
  const name = basename(filePath);
  const results: LintResult[] = [];

  let raw: string;
  try {
    raw = await Deno.readTextFile(filePath);
  } catch (e) {
    results.push(error(name, "io/read", `Cannot read file: ${e}`));
    return summarize(name, results);
  }

  let data: TomlData;
  try {
    data = parseToml(raw) as TomlData;
  } catch (e) {
    results.push(error(name, "parse/toml", `Invalid TOML: ${e}`));
    return summarize(name, results);
  }

  // Structural checks (parsed)
  results.push(...checkPragma(name, data));
  results.push(...checkMetadata(name, data));

  if (isIndexFile(data)) {
    results.push(...checkIndex(name, data));
  } else {
    results.push(...checkContent(name, data));
  }

  results.push(...checkClosing(name, data));

  // Cross-reference validation (domain knowledge)
  results.push(...checkCrossReferences(name, data));

  // Domain convention checks
  results.push(...checkDomainConventions(name, data));

  // Comment-based checks (raw text)
  results.push(...checkComments(name, raw));

  return summarize(name, results);
}

function summarize(file: string, results: LintResult[]): LintSummary {
  return {
    file,
    errors: results.filter((r) => r.severity === "error").length,
    warnings: results.filter((r) => r.severity === "warn").length,
    infos: results.filter((r) => r.severity === "info").length,
    results,
  };
}

// ---------------------------------------------------------------------------
// Display
// ---------------------------------------------------------------------------

const COLORS = {
  reset: "\x1b[0m",
  red: "\x1b[31m",
  yellow: "\x1b[33m",
  blue: "\x1b[34m",
  green: "\x1b[32m",
  dim: "\x1b[2m",
  bold: "\x1b[1m",
};

function severityColor(s: Severity): string {
  switch (s) {
    case "error": return COLORS.red;
    case "warn": return COLORS.yellow;
    case "info": return COLORS.blue;
  }
}

function severityIcon(s: Severity): string {
  switch (s) {
    case "error": return "x";
    case "warn": return "!";
    case "info": return "i";
  }
}

function printSummary(summary: LintSummary, verbose: boolean): void {
  const { file, errors, warnings, infos, results } = summary;
  const total = errors + warnings + infos;

  if (total === 0) {
    console.log(`${COLORS.green}  OK${COLORS.reset}  ${file}`);
    return;
  }

  const statusColor = errors > 0 ? COLORS.red : warnings > 0 ? COLORS.yellow : COLORS.blue;
  const statusIcon = errors > 0 ? "FAIL" : warnings > 0 ? "WARN" : "INFO";
  console.log(`${statusColor}${statusIcon}${COLORS.reset}  ${file}  ${COLORS.dim}(${errors}E ${warnings}W ${infos}I)${COLORS.reset}`);

  if (verbose || errors > 0) {
    for (const r of results) {
      if (!verbose && r.severity === "info") continue;
      const color = severityColor(r.severity);
      const icon = severityIcon(r.severity);
      console.log(`  ${color}${icon}${COLORS.reset} ${COLORS.dim}[${r.rule}]${COLORS.reset} ${r.message}`);
    }
  }
}

function printTotal(summaries: LintSummary[]): void {
  const totalErrors = summaries.reduce((s, r) => s + r.errors, 0);
  const totalWarnings = summaries.reduce((s, r) => s + r.warnings, 0);
  const totalInfos = summaries.reduce((s, r) => s + r.infos, 0);
  const totalFiles = summaries.length;
  const passFiles = summaries.filter((s) => s.errors === 0 && s.warnings === 0).length;

  console.log("");
  console.log(`${COLORS.bold}Summary:${COLORS.reset} ${totalFiles} files, ${passFiles} clean`);
  console.log(
    `  ${COLORS.red}${totalErrors} errors${COLORS.reset}  ` +
    `${COLORS.yellow}${totalWarnings} warnings${COLORS.reset}  ` +
    `${COLORS.blue}${totalInfos} info${COLORS.reset}`
  );

  if (totalErrors === 0) {
    console.log(`\n${COLORS.green}All files structurally valid.${COLORS.reset}`);
  } else {
    console.log(`\n${COLORS.red}${totalErrors} structural errors must be fixed.${COLORS.reset}`);
  }
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// CLI entrypoint
// ---------------------------------------------------------------------------

async function main(): Promise<void> {
  const args = Deno.args;

  if (args.length === 0 || args.includes("--help") || args.includes("-h")) {
    console.log(`
${COLORS.bold}lint-toml${COLORS.reset} v${VERSION} — TOML Template Alignment Linter

${COLORS.dim}"Prove all things; hold fast that which is good." — 1 Thess 5:21${COLORS.reset}

Usage:
  deno run --allow-read lint-toml.ts <file-or-directory> [options]

Options:
  --verbose, -v     Show all results (including info)
  --errors-only     Show only errors
  --summary         Show only file-level summary
  --help, -h        Show this help

Examples:
  deno run --allow-read lint-toml.ts primitives/
  deno run --allow-read lint-toml.ts composite-types.toml -v
  deno run --allow-read lint-toml.ts . --summary
`);
    Deno.exit(0);
  }

  const verbose = args.includes("--verbose") || args.includes("-v");
  const errorsOnly = args.includes("--errors-only");
  const summaryOnly = args.includes("--summary");

  // Collect targets (non-flag args)
  const targets = args.filter((a) => !a.startsWith("-"));

  // Gather .toml files
  const files: string[] = [];
  for (const target of targets) {
    const resolved = resolve(target);
    try {
      const stat = await Deno.stat(resolved);
      if (stat.isFile && resolved.endsWith(".toml")) {
        files.push(resolved);
      } else if (stat.isDirectory) {
        for await (const entry of walk(resolved, { exts: [".toml"], maxDepth: 3 })) {
          if (entry.isFile) {
            files.push(entry.path);
          }
        }
      }
    } catch {
      console.error(`${COLORS.red}Cannot access: ${target}${COLORS.reset}`);
    }
  }

  if (files.length === 0) {
    console.error(`${COLORS.red}No .toml files found.${COLORS.reset}`);
    Deno.exit(1);
  }

  // Sort by path for consistent output
  files.sort();

  // Lint all files
  const cwd = Deno.cwd();
  console.log(`${COLORS.bold}Linting ${files.length} TOML file(s)...${COLORS.reset}\n`);

  const summaries: LintSummary[] = [];
  for (const file of files) {
    const summary = await lintFile(file);
    // Use relative path for display
    summary.file = relative(cwd, file);
    summaries.push(summary);

    if (!summaryOnly) {
      if (errorsOnly) {
        if (summary.errors > 0) {
          printSummary(summary, verbose);
        }
      } else {
        printSummary(summary, verbose);
      }
    }
  }

  // Summary totals
  if (summaryOnly) {
    for (const s of summaries) {
      const status = s.errors === 0
        ? `${COLORS.green}OK${COLORS.reset}`
        : `${COLORS.red}FAIL${COLORS.reset}`;
      console.log(`${status}  ${s.file}  (${s.errors}E ${s.warnings}W ${s.infos}I)`);
    }
  }

  printTotal(summaries);

  // Exit code: non-zero if any errors
  const hasErrors = summaries.some((s) => s.errors > 0);
  Deno.exit(hasErrors ? 1 : 0);
}

main();
