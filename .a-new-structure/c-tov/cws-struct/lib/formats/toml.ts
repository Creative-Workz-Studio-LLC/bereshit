// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/formats/toml.ts
// key:     B-tov-cws-struct-lib-formats-toml
// title:   CWS Struct — TOML Format Handler
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-11
// authors: Nova Dawn (CPI-SI)
// derives: c-tov/cwe-creator-tools/lint-toml.ts (v a-02.00, 823 lines)
// purpose: TOML 3-block alignment linter as a format handler.
//          Validates pragma (P1-P5), metadata (M1-M9), content structure
//          (Cc/Co/Cv), closing (X1-X5), and cross-references.
//
// biblical_foundation: "Prove all things; hold fast that which is good."
//   — 1 Thessalonians 5:21
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { parse as parseToml } from "@std/toml";
import type { FormatHandler, LintResult } from "../types.ts";
import { error, warn, info } from "../types.ts";
import { registerFormat } from "./registry.ts";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

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

  const requiredPragma = [
    "P1_core",
    "P2_family",
    "P3_instance",
    "P4_architecture",
    "P5_summary",
  ];
  for (const key of requiredPragma) {
    if (!has(pragma, key)) {
      results.push(error(file, `pragma/${key}`, `Missing _pragma.${key}`));
    }
  }

  if (!has(pragma, "legend")) {
    results.push(
      warn(
        file,
        "pragma/legend",
        "Missing _pragma.legend — recommended for documenting field requirements",
      ),
    );
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

  for (const key of REQUIRED_METADATA) {
    if (!has(meta, key)) {
      results.push(
        error(file, `metadata/${key}`, `Missing _metadata.${key} — REQUIRED`),
      );
    }
  }

  for (const key of EXTENSION_METADATA) {
    if (!has(meta, key)) {
      results.push(
        warn(
          file,
          `metadata/${key}`,
          `Missing _metadata.${key} — extension metadata`,
        ),
      );
    }
  }

  return results;
}

function checkContent(file: string, data: TomlData): LintResult[] {
  const results: LintResult[] = [];
  const content = getTable(data, "_content");

  if (!content) {
    results.push(error(file, "content/exists", "Missing [_content] table"));
    return results;
  }

  // Check Cc/Co/Cv sub-block labels
  for (const label of ["Cc", "Co", "Cv"]) {
    if (!has(content, label)) {
      results.push(
        warn(file, `content/${label}`, `Missing _content.${label} sub-block label`),
      );
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

  for (const key of REQUIRED_CLOSING) {
    if (!has(closing, key)) {
      results.push(
        error(file, `closing/${key}`, `Missing _closing.${key} — REQUIRED`),
      );
    }
  }

  return results;
}

function checkCrossRefs(file: string, data: TomlData): LintResult[] {
  const results: LintResult[] = [];
  const pragma = getTable(data, "_pragma");
  const meta = getTable(data, "_metadata");

  if (!pragma || !meta) return results;

  // P1.key should match M1.key
  const p1 = getTable(pragma, "P1_core");
  const m1 = getTable(meta, "M1_identity");

  if (p1 && m1) {
    const pKey = p1["key"];
    const mKey = m1["key"];
    if (pKey && mKey && pKey !== mKey) {
      results.push(
        error(
          file,
          "xref/P1-M1-key",
          `Key mismatch: _pragma.P1_core.key="${pKey}" vs _metadata.M1_identity.key="${mKey}"`,
        ),
      );
    }
  }

  // P5.title should match M1.title
  const p5 = getTable(pragma, "P5_summary");
  if (p5 && m1) {
    const pTitle = p5["title"];
    const mTitle = m1["title"];
    if (pTitle && mTitle && pTitle !== mTitle) {
      results.push(
        warn(
          file,
          "xref/P5-M1-title",
          `Title mismatch: P5="${pTitle}" vs M1="${mTitle}"`,
        ),
      );
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Transform — auto-fix missing Cc/Co/Cv labels
// ---------------------------------------------------------------------------

/** Escape a string for use inside TOML double quotes. */
function escapeTomlValue(s: string): string {
  return s.replace(/\\/g, "\\\\").replace(/"/g, '\\"');
}

/**
 * Generate Cc/Co/Cv label values from file metadata.
 *
 * - Cc derives from P5_summary.brief (what the file configures)
 * - Co derives from P3_instance.provides (what capabilities it gives)
 * - Cv is standard validation description
 */
function generateCcCoCv(
  data: TomlData,
): { cc: string; co: string; cv: string } {
  const pragma = getTable(data, "_pragma");
  const content = getTable(data, "_content");

  // Cc — from P5_summary.brief, falling back to constraint
  const p5 = pragma ? getTable(pragma, "P5_summary") : undefined;
  const brief = (p5?.["brief"] as string) ?? "";
  const constraint = (content?.["constraint"] as string) ?? "";
  const ccSource = brief || constraint || "configuration data";
  const cc = `Content Configurations — ${escapeTomlValue(ccSource)}`;

  // Co — from P3_instance.provides, falling back to M1_identity.role
  const p3 = pragma ? getTable(pragma, "P3_instance") : undefined;
  const provides = p3?.["provides"];
  const meta = getTable(data, "_metadata");
  const m1 = meta ? getTable(meta, "M1_identity") : undefined;
  const role = (m1?.["role"] as string) ?? "";

  let co: string;
  if (Array.isArray(provides) && provides.length > 0) {
    const items = provides.map((p: string) =>
      p.toLowerCase().replace(/_/g, " ")
    );
    const display = items.length <= 4
      ? items.join(", ")
      : items.slice(0, 3).join(", ") + `, +${items.length - 3} more`;
    co = `Content Operations — ${escapeTomlValue(display)}`;
  } else if (role) {
    co = `Content Operations — ${escapeTomlValue(role)}`;
  } else {
    co = "Content Operations — type definitions and integration";
  }

  // Cv — standard validation description
  const cv = "Content Validation — schema requirements, type constraints";

  return { cc, co, cv };
}

/**
 * Find the line index of the last scalar key = value line within [_content],
 * before the next section separator (comment or table header).
 * Returns -1 if [_content] not found.
 */
function findContentInsertionPoint(lines: string[]): number {
  let inContent = false;
  let lastKeyLine = -1;

  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i].trim();

    if (trimmed === "[_content]") {
      inContent = true;
      continue;
    }

    if (inContent) {
      // Another TOML table header → stop
      if (trimmed.startsWith("[") && !trimmed.startsWith("[_content")) {
        break;
      }
      // Comment separator → stop (marks beginning of sub-blocks)
      if (trimmed.startsWith("# ─") || trimmed.startsWith("# ═")) {
        break;
      }
      // Key = value line (not a comment, not blank)
      if (trimmed && !trimmed.startsWith("#") && trimmed.includes(" = ")) {
        lastKeyLine = i;
      }
    }
  }

  return lastKeyLine;
}

/**
 * Transform a TOML file by adding missing Cc/Co/Cv labels to [_content].
 * Labels are generated from the file's own pragma and metadata.
 */
async function transformTomlFile(
  filePath: string,
  dryRun: boolean,
): Promise<LintResult[]> {
  const results: LintResult[] = [];

  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch (e) {
    return [error(filePath, "io/read", `Cannot read file: ${e}`)];
  }

  let data: TomlData;
  try {
    data = parseToml(text) as TomlData;
  } catch (e) {
    return [error(filePath, "parse/toml", `TOML parse error: ${e}`)];
  }

  const content = getTable(data, "_content");
  if (!content) {
    results.push(
      info(filePath, "transform/skip", "No [_content] — nothing to transform"),
    );
    return results;
  }

  const needsCc = !has(content, "Cc");
  const needsCo = !has(content, "Co");
  const needsCv = !has(content, "Cv");

  if (!needsCc && !needsCo && !needsCv) {
    results.push(
      info(
        filePath,
        "transform/ok",
        "Cc/Co/Cv already present — no changes needed",
      ),
    );
    return results;
  }

  // Generate labels from file metadata
  const labels = generateCcCoCv(data);

  // Find insertion point in raw text
  const lines = text.split("\n");
  const insertIdx = findContentInsertionPoint(lines);

  if (insertIdx === -1) {
    results.push(
      error(
        filePath,
        "transform/position",
        "Could not find insertion point in [_content]",
      ),
    );
    return results;
  }

  // Build insertion lines
  const insertLines: string[] = [];
  if (needsCc) insertLines.push(`Cc = "${labels.cc}"`);
  if (needsCo) insertLines.push(`Co = "${labels.co}"`);
  if (needsCv) insertLines.push(`Cv = "${labels.cv}"`);

  if (dryRun) {
    for (const line of insertLines) {
      results.push(info(filePath, "transform/would-add", line));
    }
    return results;
  }

  // Insert after the last key line in [_content]
  lines.splice(insertIdx + 1, 0, ...insertLines);
  await Deno.writeTextFile(filePath, lines.join("\n"));

  for (const line of insertLines) {
    results.push(info(filePath, "transform/added", line));
  }

  return results;
}

// ---------------------------------------------------------------------------
// Lint orchestrator
// ---------------------------------------------------------------------------

async function lintTomlFile(filePath: string): Promise<LintResult[]> {
  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch (e) {
    return [error(filePath, "io/read", `Cannot read file: ${e}`)];
  }

  let data: TomlData;
  try {
    data = parseToml(text) as TomlData;
  } catch (e) {
    return [error(filePath, "parse/toml", `TOML parse error: ${e}`)];
  }

  return [
    ...checkPragma(filePath, data),
    ...checkMetadata(filePath, data),
    ...checkContent(filePath, data),
    ...checkClosing(filePath, data),
    ...checkCrossRefs(filePath, data),
  ];
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// Registration — plug into the registry
// ---------------------------------------------------------------------------

const tomlHandler: FormatHandler = {
  name: "toml",
  description: "TOML 3-block alignment (pragma, metadata M1-M9, content Cc/Co/Cv, closing X1-X5)",
  extensions: [".toml"],
  maxDepth: 10,
  lint: lintTomlFile,
  transform: transformTomlFile,
};

registerFormat(tomlHandler);

export default tomlHandler;
