#!/usr/bin/env -S deno run --allow-read
// ============================================================================
// METADATA
// ============================================================================
//
// file:    lint-json.ts
// key:     B-tools-lint-json
// title:   JSON/JSONC Template Alignment Linter
// type:    Tool (Linter)
// version: a-01.00
// created: 2026-02-10
// authors: Nova Dawn (CPI-SI)
// derives: lint-toml.ts (structural base — 4-block skeleton, display, CLI)
// purpose: Validate JSON and JSONC data files against the OmniCode 3-block
//          template. Checks structural alignment: pragma (P1-P5), metadata
//          (M1-M9), content labels (Cc/Co/Cv), closing (X1-X5), block
//          boundary markers. Handles both .json (strict parse) and .jsonc
//          (comment-aware parse with comment structure checks).
//
// biblical_foundation: "Prove all things; hold fast that which is good."
//                       — 1 Thessalonians 5:21
//
// usage:   deno run --allow-read lint-json.ts <file-or-directory> [options]
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { parse as parseJsonc } from "jsr:@std/jsonc@1";
import { walk } from "jsr:@std/fs@1/walk";
import { resolve, basename, relative } from "jsr:@std/path@1";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

const VERSION = "a-01.00";

// Pragma — P1 through P5 section headers and their required sub-keys
const PRAGMA_SECTIONS: Record<string, string[]> = {
  _P1_CORE: ["_P1_tag", "_P1_key", "_P1_from", "_P1_at"],
  _P2_FAMILY: ["_P2_tag", "_P2_type", "_P2_subtype", "_P2_role", "_P2_structure"],
  _P3_INSTANCE: ["_P3_tag", "_P3_component", "_P3_layer", "_P3_includes", "_P3_provides"],
  _P4_ARCH: ["_P4_tag", "_P4_layers", "_P4_anchor"],
  _P5_SUMMARY: ["_P5_tag", "_P5_title", "_P5_brief"],
};

// Metadata — required M1-M5 keys (top-level flat keys)
const REQUIRED_METADATA = [
  "_M1_identity",
  "_M2_state",
  "_M3_attribution",
  "_M4_grounding",
  "_M5_dependencies",
] as const;

// Metadata — extension M6-M9 keys (present in derived files)
const EXTENSION_METADATA = [
  "_M6_location",
  "_M7_classification",
  "_M8_intent",
  "_M9_roadmap",
] as const;

// M1 required sub-keys
const M1_REQUIRED = ["file", "key", "title", "type"] as const;

// M2 required sub-keys
const M2_REQUIRED = ["version", "status"] as const;

// Closing — X1-X5 section headers
const REQUIRED_CLOSING_SECTIONS = ["_X1", "_X5"] as const;
const EXTENSION_CLOSING_SECTIONS = ["_X2", "_X3"] as const;
// X4 always expected (REFERENCE)
const X4_KEY = "_X4" as const;

// Content labels
const CONTENT_LABELS = ["_Cc", "_Co", "_Cv"] as const;

// Block boundary tag keys (JSON/JSONC express these as _tag keys, not comments)
const BLOCK_BOUNDARY_TAGS = [
  { key: "_pragma_tag", value: "OMNICODE PRAGMA [PRAGMA]", required: true },
  { key: "_pragma_end_tag", value: "END PRAGMA [END]", required: true },
  { key: "_metadata_tag", value: "METADATA BLOCK [METADATA]", required: true },
  { key: "_metadata_end_tag", value: "END METADATA [END]", required: true },
  { key: "_content_tag", value: "CONTENT BLOCK [CONTENT]", required: true },
  { key: "_content_end_tag", value: "END CONTENT [END]", required: true },
  { key: "_closing_tag", value: "CLOSING BLOCK [CLOSING]", required: true },
  { key: "_closing_end_tag", value: "END CLOSING [END]", required: true },
] as const;

// Block boundary separator keys
const BLOCK_BOUNDARY_SEPARATORS = [
  "_pragma",
  "_pragma_end",
  "_metadata_start",
  "_metadata_end",
  "_content_start",
  "_content_end",
  "_closing_start",
  "_closing_end",
] as const;

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

type Severity = "error" | "warn" | "info";
type Format = "json" | "jsonc";

interface LintResult {
  file: string;
  severity: Severity;
  rule: string;
  message: string;
}

interface LintSummary {
  file: string;
  format: Format;
  errors: number;
  warnings: number;
  infos: number;
  results: LintResult[];
}

// deno-lint-ignore no-explicit-any
type JsonData = Record<string, any>;

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helpers — Data access
// ---------------------------------------------------------------------------

function has(obj: JsonData, key: string): boolean {
  return obj != null && typeof obj === "object" && key in obj;
}

function isObj(val: unknown): val is JsonData {
  return val != null && typeof val === "object" && !Array.isArray(val);
}

function getObj(obj: JsonData, key: string): JsonData | undefined {
  const val = obj?.[key];
  return isObj(val) ? val : undefined;
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
// Core — Pragma checks (flat _P1..._P5 keys)
// ---------------------------------------------------------------------------

function checkPragma(file: string, data: JsonData): LintResult[] {
  const results: LintResult[] = [];

  // Top-level pragma separator and tag
  if (!has(data, "_pragma")) {
    results.push(error(file, "pragma/exists", 'Missing "_pragma" separator key'));
    return results;
  }

  if (!has(data, "_pragma_tag")) {
    results.push(error(file, "pragma/tag", 'Missing "_pragma_tag" key'));
  }

  if (!has(data, "_pragma_format")) {
    results.push(error(file, "pragma/format", 'Missing "_pragma_format" key'));
  }

  if (!has(data, "_pragma_legend")) {
    results.push(warn(file, "pragma/legend", 'Missing "_pragma_legend" — recommended for documenting field requirements'));
  }

  // P1 through P5 sections
  for (const [header, subkeys] of Object.entries(PRAGMA_SECTIONS)) {
    if (!has(data, header)) {
      results.push(error(file, `pragma/${header}`, `Missing "${header}" separator key`));
      continue;
    }
    for (const subkey of subkeys) {
      if (!has(data, subkey)) {
        results.push(error(file, `pragma/${subkey}`, `Missing "${subkey}"`));
      }
    }
  }

  // End markers
  if (!has(data, "_pragma_end")) {
    results.push(error(file, "pragma/end", 'Missing "_pragma_end" separator'));
  }
  if (!has(data, "_pragma_end_tag")) {
    results.push(error(file, "pragma/end_tag", 'Missing "_pragma_end_tag"'));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Metadata checks (flat _M1..._M5 keys)
// ---------------------------------------------------------------------------

function checkMetadata(file: string, data: JsonData): LintResult[] {
  const results: LintResult[] = [];

  // Block boundaries
  if (!has(data, "_metadata_start")) {
    results.push(error(file, "metadata/start", 'Missing "_metadata_start" separator'));
  }
  if (!has(data, "_metadata_tag")) {
    results.push(error(file, "metadata/tag", 'Missing "_metadata_tag"'));
  }
  if (!has(data, "_metadata_end")) {
    results.push(error(file, "metadata/end", 'Missing "_metadata_end" separator'));
  }
  if (!has(data, "_metadata_end_tag")) {
    results.push(error(file, "metadata/end_tag", 'Missing "_metadata_end_tag"'));
  }

  // Required M1-M5
  for (const key of REQUIRED_METADATA) {
    if (!has(data, key)) {
      results.push(error(file, `metadata/${key}`, `Missing "${key}" — REQUIRED`));
    }
  }

  // M1 sub-keys
  const m1 = getObj(data, "_M1_identity");
  if (m1) {
    for (const sub of M1_REQUIRED) {
      if (!has(m1, sub)) {
        results.push(error(file, `metadata/M1.${sub}`, `Missing _M1_identity.${sub}`));
      }
    }
  } else if (has(data, "_M1_identity")) {
    results.push(warn(file, "metadata/M1.type", "_M1_identity should be an object"));
  }

  // M2 sub-keys
  const m2 = getObj(data, "_M2_state");
  if (m2) {
    for (const sub of M2_REQUIRED) {
      if (!has(m2, sub)) {
        results.push(error(file, `metadata/M2.${sub}`, `Missing _M2_state.${sub}`));
      }
    }
  } else if (has(data, "_M2_state")) {
    results.push(warn(file, "metadata/M2.type", "_M2_state should be an object"));
  }

  // M3 sub-keys
  const m3 = getObj(data, "_M3_attribution");
  if (m3) {
    if (!has(m3, "architect")) {
      results.push(warn(file, "metadata/M3.architect", "Missing _M3_attribution.architect"));
    }
    if (!has(m3, "implementation")) {
      results.push(warn(file, "metadata/M3.implementation", "Missing _M3_attribution.implementation"));
    }
  }

  // M4 grounding — scripture + principle
  const m4 = getObj(data, "_M4_grounding");
  if (m4) {
    if (!has(m4, "scripture")) {
      results.push(warn(file, "metadata/M4.scripture", "Missing _M4_grounding.scripture — biblical anchor"));
    }
    if (!has(m4, "principle")) {
      results.push(warn(file, "metadata/M4.principle", "Missing _M4_grounding.principle"));
    }
  }

  // M5 dependencies — requires, consumers, cross_format
  const m5 = getObj(data, "_M5_dependencies");
  if (m5) {
    if (!has(m5, "requires")) {
      results.push(warn(file, "metadata/M5.requires", "Missing _M5_dependencies.requires"));
    }
    if (!has(m5, "consumers")) {
      results.push(warn(file, "metadata/M5.consumers", "Missing _M5_dependencies.consumers"));
    }
    // Relationship tracking — two valid conventions:
    //   cross_format: for files that exist across formats (TOML ↔ JSON ↔ JSONC)
    //   related_data: for format-native files tracking domain relationships
    // Both are valid. Neither = genuine gap.
    const cf = getObj(m5, "cross_format");
    const rd = getObj(m5, "related_data") ?? (has(m5, "related_data") ? m5["related_data"] : undefined);

    if (cf) {
      // cross_format path — validate its structure
      if (!has(cf, "relationship")) {
        results.push(warn(file, "metadata/M5.cross_format.relationship", "Missing cross_format.relationship"));
      }
      if (!has(cf, "role")) {
        results.push(info(file, "metadata/M5.cross_format.role", "Missing cross_format.role — recommended"));
      }
    } else if (rd !== undefined) {
      // related_data path — validate its structure
      if (isObj(rd)) {
        if (!has(rd, "references") && !has(rd, "referenced_by") && !has(rd, "derives_from")) {
          results.push(warn(file, "metadata/M5.related_data.structure",
            "related_data exists but has no references, referenced_by, or derives_from"));
        }
      }
      // related_data present and non-empty — relationship tracking satisfied
    } else {
      // Neither convention present — genuine gap
      results.push(warn(file, "metadata/M5.relationships",
        "Missing relationship tracking — expected cross_format (cross-format files) or related_data (domain-native files)"));
    }
  }

  // Extension note
  if (!has(data, "_M_extension_note")) {
    results.push(warn(file, "metadata/M_extension_note", 'Missing "_M_extension_note" — extension contract'));
  }

  // Extension metadata (M6-M9) — info only
  for (const key of EXTENSION_METADATA) {
    if (!has(data, key)) {
      results.push(info(file, `metadata/${key}`, `Missing "${key}" — extension metadata (optional)`));
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Content checks (flat _Cc, _Co, _Cv labels)
// ---------------------------------------------------------------------------

function checkContent(file: string, data: JsonData): LintResult[] {
  const results: LintResult[] = [];

  // Block boundaries
  if (!has(data, "_content_start")) {
    results.push(error(file, "content/start", 'Missing "_content_start" separator'));
  }
  if (!has(data, "_content_tag")) {
    results.push(error(file, "content/tag", 'Missing "_content_tag"'));
  }
  if (!has(data, "_content_end")) {
    results.push(error(file, "content/end", 'Missing "_content_end" separator'));
  }
  if (!has(data, "_content_end_tag")) {
    results.push(error(file, "content/end_tag", 'Missing "_content_end_tag"'));
  }

  // Content labels (Cc, Co, Cv)
  for (const label of CONTENT_LABELS) {
    if (!has(data, label)) {
      results.push(error(file, `content/${label}`, `Missing "${label}" separator — content label`));
    }
    const tagKey = `${label}_tag`;
    if (!has(data, tagKey)) {
      results.push(error(file, `content/${tagKey}`, `Missing "${tagKey}"`));
    }
  }

  // Cc format key — JSONC uses _Cc_format, JSON uses _Cc_format_note
  if (!has(data, "_Cc_format") && !has(data, "_Cc_format_note")) {
    results.push(warn(file, "content/Cc_format", 'Missing "_Cc_format" or "_Cc_format_note"'));
  }

  // Cc key convention
  if (!has(data, "_Cc_key_convention") && !has(data, "_Cc_encoding")) {
    results.push(info(file, "content/Cc_convention", 'No "_Cc_key_convention" or "_Cc_encoding" found'));
  }

  // Cv verification
  if (!has(data, "_Cv_verify") && !has(data, "_Cv_validation")) {
    results.push(warn(file, "content/Cv_verify", 'Missing "_Cv_verify" or "_Cv_validation" — validation commands'));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Closing checks (flat _X1..._X5 keys)
// ---------------------------------------------------------------------------

function checkClosing(file: string, data: JsonData): LintResult[] {
  const results: LintResult[] = [];

  // Block boundaries
  if (!has(data, "_closing_start")) {
    results.push(error(file, "closing/start", 'Missing "_closing_start" separator'));
  }
  if (!has(data, "_closing_tag")) {
    results.push(error(file, "closing/tag", 'Missing "_closing_tag"'));
  }
  if (!has(data, "_closing_end")) {
    results.push(error(file, "closing/end", 'Missing "_closing_end" separator'));
  }
  if (!has(data, "_closing_end_tag")) {
    results.push(error(file, "closing/end_tag", 'Missing "_closing_end_tag"'));
  }

  // Required X1, X5
  for (const key of REQUIRED_CLOSING_SECTIONS) {
    if (!has(data, key)) {
      results.push(error(file, `closing/${key}`, `Missing "${key}" — REQUIRED`));
    }
    const tagKey = `${key}_tag`;
    if (!has(data, tagKey)) {
      results.push(error(file, `closing/${tagKey}`, `Missing "${tagKey}"`));
    }
  }

  // X1 enrichment
  if (has(data, "_X1")) {
    if (!has(data, "_X1_scripture")) {
      results.push(warn(file, "closing/X1.scripture", 'Missing "_X1_scripture"'));
    }
    if (!has(data, "_X1_never")) {
      results.push(warn(file, "closing/X1.never", 'Missing "_X1_never" — modification policy'));
    }
    if (!has(data, "_X1_careful")) {
      results.push(warn(file, "closing/X1.careful", 'Missing "_X1_careful"'));
    }
    if (!has(data, "_X1_safe")) {
      results.push(warn(file, "closing/X1.safe", 'Missing "_X1_safe"'));
    }
  }

  // X4 (REFERENCE) — expected
  if (!has(data, X4_KEY)) {
    results.push(warn(file, "closing/X4", 'Missing "_X4" — REFERENCE section'));
  } else {
    if (!has(data, "_X4_tag")) {
      results.push(warn(file, "closing/X4_tag", 'Missing "_X4_tag"'));
    }
    if (!has(data, "_X4_related_files")) {
      results.push(warn(file, "closing/X4.related_files", 'Missing "_X4_related_files"'));
    }
    if (!has(data, "_X4_validate")) {
      results.push(warn(file, "closing/X4.validate", 'Missing "_X4_validate"'));
    }
  }

  // Extension X2, X3
  for (const key of EXTENSION_CLOSING_SECTIONS) {
    if (!has(data, key)) {
      results.push(info(file, `closing/${key}`, `Missing "${key}" — extension closing (optional)`));
    }
  }

  // X5 enrichment
  if (has(data, "_X5")) {
    if (!has(data, "_X5_note")) {
      results.push(warn(file, "closing/X5.note", 'Missing "_X5_note" — final guidance'));
    }
    if (!has(data, "_X5_scripture")) {
      results.push(warn(file, "closing/X5.scripture", 'Missing "_X5_scripture"'));
    }
    if (!has(data, "_X5_anchor")) {
      results.push(warn(file, "closing/X5.anchor", 'Missing "_X5_anchor" — Genesis 1:1 connection'));
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Block boundary tag verification (parsed data)
// ---------------------------------------------------------------------------

function checkBoundaryTags(file: string, data: JsonData): LintResult[] {
  const results: LintResult[] = [];

  for (const { key, value, required } of BLOCK_BOUNDARY_TAGS) {
    if (!has(data, key)) {
      // Already caught in block-specific checks — skip duplicate
      continue;
    }
    const actual = data[key];
    if (typeof actual === "string" && actual !== value) {
      results.push(warn(file, `boundary/${key}`, `"${key}" = "${actual}" — expected "${value}"`));
    }
  }

  // Separator keys should exist
  for (const key of BLOCK_BOUNDARY_SEPARATORS) {
    if (!has(data, key)) {
      results.push(error(file, `boundary/${key}`, `Missing block boundary: "${key}"`));
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — JSONC-specific checks (comment structure)
// ---------------------------------------------------------------------------

function checkJsoncComments(file: string, raw: string): LintResult[] {
  const results: LintResult[] = [];

  // Pragma line — should have // #!omni near the top
  const lines = raw.split("\n").slice(0, 5);
  const hasPragmaComment = lines.some((l) => l.trim().startsWith("// #!omni"));
  if (!hasPragmaComment) {
    results.push(error(file, "jsonc/pragma-comment", "JSONC file should have '// #!omni ...' pragma comment near the top"));
  }

  // Comment-based block markers (JSONC uses // comments alongside _tag keys)
  const commentMarkers = [
    "OMNICODE PRAGMA [PRAGMA]",
    "END PRAGMA [END]",
    "METADATA BLOCK [METADATA]",
    "END METADATA [END]",
    "CONTENT BLOCK [CONTENT]",
    "END CONTENT [END]",
    "CLOSING BLOCK [CLOSING]",
    "END CLOSING [END]",
  ];

  for (const marker of commentMarkers) {
    if (!raw.includes(marker)) {
      results.push(info(file, "jsonc/comment-marker", `Missing comment block marker: ${marker}`));
    }
  }

  // Content label comments
  const contentLabelComments = [
    { pattern: "Cc —", name: "Content Configurations" },
    { pattern: "Co —", name: "Content Operations" },
    { pattern: "Cv —", name: "Content Validation" },
  ];

  for (const { pattern, name } of contentLabelComments) {
    if (!raw.includes(pattern)) {
      results.push(info(file, "jsonc/content-label", `Missing "${pattern}" ${name} label in comments`));
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Format-specific validation
// ---------------------------------------------------------------------------

function checkFormatConsistency(file: string, data: JsonData, fmt: Format): LintResult[] {
  const results: LintResult[] = [];

  const pragmaFormat = data["_pragma_format"];
  if (typeof pragmaFormat === "string") {
    const expectedSuffix = fmt === "jsonc" ? "--jsonc" : "--json";
    if (!pragmaFormat.includes(expectedSuffix)) {
      results.push(warn(
        file,
        "format/pragma",
        `_pragma_format "${pragmaFormat}" doesn't match file extension .${fmt} (expected "${expectedSuffix}")`,
      ));
    }
  }

  // JSONC should have _Cc_format; JSON should have _Cc_format_note
  if (fmt === "jsonc") {
    if (has(data, "_Cc_format_note") && !has(data, "_Cc_format")) {
      results.push(info(file, "format/Cc", 'JSONC files typically use "_Cc_format" instead of "_Cc_format_note"'));
    }
  } else {
    if (has(data, "_Cc_format") && !has(data, "_Cc_format_note")) {
      results.push(info(file, "format/Cc", 'JSON files typically use "_Cc_format_note" instead of "_Cc_format"'));
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — File linting orchestrator
// ---------------------------------------------------------------------------

function detectFormat(filePath: string): Format {
  return filePath.endsWith(".jsonc") ? "jsonc" : "json";
}

async function lintFile(filePath: string): Promise<LintSummary> {
  const name = basename(filePath);
  const fmt = detectFormat(filePath);
  const results: LintResult[] = [];

  let raw: string;
  try {
    raw = await Deno.readTextFile(filePath);
  } catch (e) {
    results.push(error(name, "io/read", `Cannot read file: ${e}`));
    return summarize(name, fmt, results);
  }

  let data: JsonData;
  try {
    if (fmt === "jsonc") {
      data = parseJsonc(raw) as JsonData;
    } else {
      data = JSON.parse(raw) as JsonData;
    }
  } catch (e) {
    const parseType = fmt === "jsonc" ? "JSONC" : "JSON";
    results.push(error(name, `parse/${fmt}`, `Invalid ${parseType}: ${e}`));
    return summarize(name, fmt, results);
  }

  if (!isObj(data)) {
    results.push(error(name, "structure/root", "Root value must be an object"));
    return summarize(name, fmt, results);
  }

  // Structural checks (parsed data)
  results.push(...checkPragma(name, data));
  results.push(...checkMetadata(name, data));
  results.push(...checkContent(name, data));
  results.push(...checkClosing(name, data));
  results.push(...checkBoundaryTags(name, data));
  results.push(...checkFormatConsistency(name, data, fmt));

  // JSONC-specific comment checks (raw text)
  if (fmt === "jsonc") {
    results.push(...checkJsoncComments(name, raw));
  }

  return summarize(name, fmt, results);
}

function summarize(file: string, format: Format, results: LintResult[]): LintSummary {
  return {
    file,
    format,
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
  cyan: "\x1b[36m",
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

function formatBadge(fmt: Format): string {
  return fmt === "jsonc"
    ? `${COLORS.cyan}JSONC${COLORS.reset}`
    : `${COLORS.dim}JSON${COLORS.reset}`;
}

function printSummary(summary: LintSummary, verbose: boolean): void {
  const { file, format, errors, warnings, infos, results } = summary;
  const total = errors + warnings + infos;

  if (total === 0) {
    console.log(`${COLORS.green}  OK${COLORS.reset}  ${formatBadge(format)}  ${file}`);
    return;
  }

  const statusColor = errors > 0 ? COLORS.red : warnings > 0 ? COLORS.yellow : COLORS.blue;
  const statusIcon = errors > 0 ? "FAIL" : warnings > 0 ? "WARN" : "INFO";
  console.log(
    `${statusColor}${statusIcon}${COLORS.reset}  ${formatBadge(format)}  ${file}  ` +
    `${COLORS.dim}(${errors}E ${warnings}W ${infos}I)${COLORS.reset}`,
  );

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

  const jsonCount = summaries.filter((s) => s.format === "json").length;
  const jsoncCount = summaries.filter((s) => s.format === "jsonc").length;

  console.log("");
  console.log(`${COLORS.bold}Summary:${COLORS.reset} ${totalFiles} files (${jsoncCount} JSONC, ${jsonCount} JSON), ${passFiles} clean`);
  console.log(
    `  ${COLORS.red}${totalErrors} errors${COLORS.reset}  ` +
    `${COLORS.yellow}${totalWarnings} warnings${COLORS.reset}  ` +
    `${COLORS.blue}${totalInfos} info${COLORS.reset}`,
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
${COLORS.bold}lint-json${COLORS.reset} v${VERSION} — JSON/JSONC Template Alignment Linter

${COLORS.dim}"Prove all things; hold fast that which is good." — 1 Thess 5:21${COLORS.reset}

Usage:
  deno run --allow-read lint-json.ts <file-or-directory> [options]

Supports both ${COLORS.cyan}.jsonc${COLORS.reset} and ${COLORS.dim}.json${COLORS.reset} files.
Format-specific checks applied based on file extension.

Options:
  --verbose, -v     Show all results (including info)
  --errors-only     Show only errors
  --summary         Show only file-level summary
  --json-only       Lint only .json files
  --jsonc-only      Lint only .jsonc files
  --help, -h        Show this help

Examples:
  deno run --allow-read lint-json.ts cornerstone/
  deno run --allow-read lint-json.ts config.jsonc -v
  deno run --allow-read lint-json.ts . --jsonc-only --summary
`);
    Deno.exit(0);
  }

  const verbose = args.includes("--verbose") || args.includes("-v");
  const errorsOnly = args.includes("--errors-only");
  const summaryOnly = args.includes("--summary");
  const jsonOnly = args.includes("--json-only");
  const jsoncOnly = args.includes("--jsonc-only");

  // Collect targets (non-flag args)
  const targets = args.filter((a) => !a.startsWith("-"));

  // Determine which extensions to accept
  const exts: string[] = [];
  if (jsonOnly) {
    exts.push(".json");
  } else if (jsoncOnly) {
    exts.push(".jsonc");
  } else {
    exts.push(".json", ".jsonc");
  }

  // Gather files
  const files: string[] = [];
  for (const target of targets) {
    const resolved = resolve(target);
    try {
      const stat = await Deno.stat(resolved);
      if (stat.isFile && exts.some((ext) => resolved.endsWith(ext))) {
        files.push(resolved);
      } else if (stat.isDirectory) {
        for await (const entry of walk(resolved, {
          exts: exts.map((e) => e.replace(".", "")),
          maxDepth: 5,
        })) {
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
    const extLabel = exts.join("/");
    console.error(`${COLORS.red}No ${extLabel} files found.${COLORS.reset}`);
    Deno.exit(1);
  }

  // Sort by path for consistent output
  files.sort();

  // Lint all files
  const cwd = Deno.cwd();
  console.log(`${COLORS.bold}Linting ${files.length} JSON/JSONC file(s)...${COLORS.reset}\n`);

  const summaries: LintSummary[] = [];
  for (const file of files) {
    const summary = await lintFile(file);
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
      console.log(`${status}  ${formatBadge(s.format)}  ${s.file}  (${s.errors}E ${s.warnings}W ${s.infos}I)`);
    }
  }

  printTotal(summaries);

  // Exit code: non-zero if any errors
  const hasErrors = summaries.some((s) => s.errors > 0);
  Deno.exit(hasErrors ? 1 : 0);
}

main();
