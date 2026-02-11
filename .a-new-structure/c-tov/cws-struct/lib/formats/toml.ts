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
  maxDepth: 3,
  lint: lintTomlFile,
};

registerFormat(tomlHandler);

export default tomlHandler;
