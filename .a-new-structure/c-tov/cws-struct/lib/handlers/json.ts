// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/handlers/json.ts
// key:     B-tov-cws-struct-lib-handlers-json
// title:   CWS Struct — JSON/JSONC Format Handler
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-13
// authors: Nova Dawn (CPI-SI)
// derives: lib/handlers/toml.ts (adapted for flat _ key convention)
// purpose: JSON/JSONC 3-block alignment linter as a format handler.
//          Validates pragma (_P1-_P5), metadata (_M1-_M5), content structure
//          (_Cc/_Co/_Cv), closing (_X1-_X5), block boundaries, and cross-refs.
//          Handles both .json (strict) and .jsonc (with comments).
//
// biblical_foundation: "Prove all things; hold fast that which is good."
//   — 1 Thessalonians 5:21
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { parse as parseJsonc } from "@std/jsonc";
import type { FormatHandler, LintResult, HealthScore } from "../foundation/mod.ts";
import { error, warn, info, computeHealthFromResults } from "../foundation/mod.ts";
import { registerFormat } from "../engine/mod.ts";

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

/** Required pragma keys — flat convention (_P1_key, _P2_type, etc.) */
const REQUIRED_PRAGMA_KEYS = [
  "_P1_key",
  "_P2_type",
  "_P2_structure",
  "_P5_title",
  "_P5_brief",
] as const;

/** Recommended pragma keys (warn if missing). */
const RECOMMENDED_PRAGMA_KEYS = [
  "_P1_from",
  "_P1_at",
  "_P2_subtype",
  "_P2_role",
  "_P3_component",
  "_P3_provides",
  "_P4_layers",
  "_P4_anchor",
] as const;

/** Required metadata objects. */
const REQUIRED_METADATA = [
  "_M1_identity",
  "_M2_state",
  "_M3_attribution",
  "_M4_grounding",
  "_M5_dependencies",
] as const;

/** Extension metadata (warn if missing). */
const EXTENSION_METADATA = [
  "_M_extension_note",
] as const;

/** Content sub-block separator keys. */
const CONTENT_SEPARATORS = ["_Cc", "_Co", "_Cv"] as const;

/** Required closing sub-blocks. */
const REQUIRED_CLOSING = ["_X1", "_X4", "_X5"] as const;

/** Optional closing sub-blocks (info if missing). */
const OPTIONAL_CLOSING = ["_X2", "_X3"] as const;

/** Block boundary markers. */
const BLOCK_BOUNDARIES = {
  pragma_start: "_pragma",
  pragma_end: "_pragma_end",
  metadata_start: "_metadata_start",
  metadata_end: "_metadata_end",
  content_start: "_content_start",
  content_end: "_content_end",
  closing_start: "_closing_start",
  closing_end: "_closing_end",
} as const;

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

// deno-lint-ignore no-explicit-any
type JsonData = Record<string, any>;

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helpers — JSON access
// ---------------------------------------------------------------------------

function has(obj: JsonData, key: string): boolean {
  return obj != null && typeof obj === "object" && key in obj;
}

function isObject(val: unknown): val is JsonData {
  return val != null && typeof val === "object" && !Array.isArray(val);
}

function getObject(obj: JsonData, key: string): JsonData | undefined {
  const val = obj?.[key];
  return isObject(val) ? val : undefined;
}

/**
 * Check if a key exists with any of a set of prefixes.
 * e.g., hasKeyWithPrefix(data, "_P3_") checks for _P3_component, _P3_layer, etc.
 */
function hasKeyWithPrefix(obj: JsonData, prefix: string): boolean {
  return Object.keys(obj).some((k) => k.startsWith(prefix));
}

// ---------------------------------------------------------------------------
// Parsing — handles both JSON and JSONC
// ---------------------------------------------------------------------------

/**
 * Parse a file as JSON or JSONC.
 * Uses @std/jsonc for .jsonc files (strips comments).
 * Falls back to JSON.parse for .json files.
 */
async function parseFile(
  filePath: string,
): Promise<{ data: JsonData | null; parseError: string | null }> {
  try {
    const text = await Deno.readTextFile(filePath);

    // Use @std/jsonc which handles both JSON and JSONC
    const parsed = parseJsonc(text);

    if (!isObject(parsed)) {
      return { data: null, parseError: "File does not contain a JSON object" };
    }

    return { data: parsed as JsonData, parseError: null };
  } catch (e) {
    const msg = e instanceof Error ? e.message : String(e);
    return { data: null, parseError: `Parse error: ${msg}` };
  }
}

// ---------------------------------------------------------------------------
// Core — Block structure checks
// ---------------------------------------------------------------------------

function checkPragma(file: string, data: JsonData): LintResult[] {
  const results: LintResult[] = [];

  // Check pragma block boundary
  if (!has(data, BLOCK_BOUNDARIES.pragma_start)) {
    results.push(
      error(file, "pragma/boundary", 'Missing "_pragma" block boundary marker'),
    );
  }

  // Check required pragma keys (flat convention)
  for (const key of REQUIRED_PRAGMA_KEYS) {
    if (!has(data, key)) {
      results.push(error(file, `pragma/${key}/required`, `Missing ${key} — REQUIRED`));
    }
  }

  // Check recommended pragma keys
  for (const key of RECOMMENDED_PRAGMA_KEYS) {
    if (!has(data, key)) {
      // Only warn if the P-group section exists at all
      const prefix = key.slice(0, 3); // _P1, _P2, _P3, _P4
      if (hasKeyWithPrefix(data, prefix + "_")) {
        results.push(
          warn(file, `pragma/${key}/recommended`, `Missing ${key} — recommended`),
        );
      }
    }
  }

  // Check pragma end boundary
  if (!has(data, BLOCK_BOUNDARIES.pragma_end)) {
    results.push(
      warn(
        file,
        "pragma/end_boundary",
        'Missing "_pragma_end" block boundary marker',
      ),
    );
  }

  // Check pragma_legend (recommended)
  if (!has(data, "_pragma_legend")) {
    results.push(
      info(
        file,
        "pragma/legend",
        "Missing _pragma_legend — recommended for documenting field requirements",
      ),
    );
  }

  return results;
}

function checkMetadata(file: string, data: JsonData): LintResult[] {
  const results: LintResult[] = [];

  // Check metadata block boundaries
  if (
    !has(data, BLOCK_BOUNDARIES.metadata_start) &&
    !has(data, "_metadata_tag")
  ) {
    results.push(
      warn(
        file,
        "metadata/boundary",
        'Missing "_metadata_start" or "_metadata_tag" block boundary',
      ),
    );
  }

  // Check required metadata objects
  for (const key of REQUIRED_METADATA) {
    if (!has(data, key)) {
      results.push(
        error(file, `metadata/${key}/required`, `Missing ${key} — REQUIRED`),
      );
    } else if (!isObject(data[key])) {
      results.push(
        error(
          file,
          `metadata/${key}/type`,
          `${key} should be an object, got ${typeof data[key]}`,
        ),
      );
    }
  }

  // Check M1_identity required fields
  const m1 = getObject(data, "_M1_identity");
  if (m1) {
    for (const field of ["file", "key", "title", "type"]) {
      if (!has(m1, field)) {
        results.push(
          warn(
            file,
            `metadata/M1/${field}`,
            `Missing _M1_identity.${field} — recommended`,
          ),
        );
      }
    }
  }

  // Check M2_state required fields
  const m2 = getObject(data, "_M2_state");
  if (m2) {
    for (const field of ["version", "status"]) {
      if (!has(m2, field)) {
        results.push(
          warn(
            file,
            `metadata/M2/${field}`,
            `Missing _M2_state.${field} — recommended`,
          ),
        );
      }
    }
  }

  // Check extension metadata (warning only)
  for (const key of EXTENSION_METADATA) {
    if (!has(data, key)) {
      results.push(
        warn(
          file,
          `metadata/${key}`,
          `Missing ${key} — extension metadata`,
        ),
      );
    }
  }

  // Check metadata end boundary
  if (!has(data, BLOCK_BOUNDARIES.metadata_end)) {
    results.push(
      warn(
        file,
        "metadata/end_boundary",
        'Missing "_metadata_end" block boundary marker',
      ),
    );
  }

  return results;
}

function checkContent(file: string, data: JsonData): LintResult[] {
  const results: LintResult[] = [];

  // Check content block boundaries
  if (
    !has(data, BLOCK_BOUNDARIES.content_start) &&
    !has(data, "_content_tag")
  ) {
    results.push(
      warn(
        file,
        "content/boundary",
        'Missing "_content_start" or "_content_tag" block boundary',
      ),
    );
  }

  // Check Cc/Co/Cv sub-block separators
  for (const sep of CONTENT_SEPARATORS) {
    if (!has(data, sep)) {
      results.push(
        warn(
          file,
          `content/${sep}`,
          `Missing ${sep} sub-block separator`,
        ),
      );
    }
  }

  // Check for Cc_tag, Co_tag, Cv_tag (recommended companions)
  for (const sep of CONTENT_SEPARATORS) {
    if (has(data, sep) && !has(data, `${sep}_tag`)) {
      results.push(
        info(
          file,
          `content/${sep}_tag`,
          `Missing ${sep}_tag — recommended companion to ${sep}`,
        ),
      );
    }
  }

  // Check content end boundary
  if (!has(data, BLOCK_BOUNDARIES.content_end)) {
    results.push(
      warn(
        file,
        "content/end_boundary",
        'Missing "_content_end" block boundary marker',
      ),
    );
  }

  return results;
}

function checkClosing(file: string, data: JsonData): LintResult[] {
  const results: LintResult[] = [];

  // Check closing block boundaries
  if (
    !has(data, BLOCK_BOUNDARIES.closing_start) &&
    !has(data, "_closing_tag")
  ) {
    results.push(
      warn(
        file,
        "closing/boundary",
        'Missing "_closing_start" or "_closing_tag" block boundary',
      ),
    );
  }

  // Check required closing sub-blocks
  for (const key of REQUIRED_CLOSING) {
    if (!has(data, key)) {
      results.push(
        error(file, `closing/${key}/required`, `Missing ${key} — REQUIRED`),
      );
    }
  }

  // Check optional closing sub-blocks (info only)
  for (const key of OPTIONAL_CLOSING) {
    if (!has(data, key)) {
      results.push(
        info(
          file,
          `closing/${key}/defined`,
          `Missing ${key} — optional closing section`,
        ),
      );
    }
  }

  // Check closing end boundary
  if (!has(data, BLOCK_BOUNDARIES.closing_end)) {
    results.push(
      warn(
        file,
        "closing/end_boundary",
        'Missing "_closing_end" block boundary marker',
      ),
    );
  }

  return results;
}

function checkCrossRefs(file: string, data: JsonData): LintResult[] {
  const results: LintResult[] = [];

  const m1 = getObject(data, "_M1_identity");
  if (!m1) return results;

  // _P1_key should match _M1_identity.key
  const pKey = data["_P1_key"];
  const mKey = m1["key"];
  if (pKey && mKey && pKey !== mKey) {
    results.push(
      error(
        file,
        "xref/P1-M1-key",
        `Key mismatch: _P1_key="${pKey}" vs _M1_identity.key="${mKey}"`,
      ),
    );
  }

  // _P5_title should match _M1_identity.title
  const pTitle = data["_P5_title"];
  const mTitle = m1["title"];
  if (pTitle && mTitle && pTitle !== mTitle) {
    results.push(
      warn(
        file,
        "xref/P5-M1-title",
        `Title mismatch: _P5_title="${pTitle}" vs _M1_identity.title="${mTitle}"`,
      ),
    );
  }

  return results;
}

// ---------------------------------------------------------------------------
// Lint orchestrator
// ---------------------------------------------------------------------------

async function lintJsonFile(filePath: string): Promise<LintResult[]> {
  const { data, parseError } = await parseFile(filePath);

  if (parseError || !data) {
    return [error(filePath, "parse", parseError ?? "Unknown parse error")];
  }

  // Quick check: is this a 3-block file at all?
  if (!has(data, "_pragma") && !has(data, "_P1_key")) {
    return [
      error(
        filePath,
        "structure/3block",
        "Not a 3-block file — missing _pragma and _P1_key",
      ),
    ];
  }

  const results: LintResult[] = [];

  results.push(...checkPragma(filePath, data));
  results.push(...checkMetadata(filePath, data));
  results.push(...checkContent(filePath, data));
  results.push(...checkClosing(filePath, data));
  results.push(...checkCrossRefs(filePath, data));

  return results;
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// Registration — plug into the registry
// ---------------------------------------------------------------------------

/** JSON health — bridge from LintResult[] to HealthScore. */
async function computeJsonHealth(
  filePath: string,
  results: LintResult[],
): Promise<HealthScore> {
  return computeHealthFromResults(results, 30, "content");
}

const jsonHandler: FormatHandler = {
  name: "json",
  description: "JSON/JSONC 3-block alignment (flat _ key convention)",
  extensions: [".json", ".jsonc"],
  maxDepth: 10,
  lint: lintJsonFile,
  computeHealth: computeJsonHealth,
};

registerFormat(jsonHandler);

export default jsonHandler;

//
// JSON and JSONC share the same 3-block structure, expressed in each format's
// native tongue. JSONC adds human explanation alongside machine structure.
// Both converge on the same standard — _ fields for documentation, named keys
// for data, block boundaries for navigation.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
