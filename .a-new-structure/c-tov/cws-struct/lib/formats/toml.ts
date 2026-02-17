// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/formats/toml.ts
// key:     B-tov-cws-struct-lib-formats-toml
// title:   CWS Struct — TOML Format Handler (Schema-Driven)
// type:    Code (Library)
// version: a-03.00
// created: 2026-02-11
// updated: 2026-02-16
// authors: Nova Dawn (CPI-SI)
// derives: c-tov/cwe-creator-tools/lint-toml.ts (v a-02.00, 823 lines)
// purpose: Schema-driven TOML 3-block alignment linter. Validation rules are
//          loaded from toml-3block-schema.jsonc at runtime — the schema is the
//          single source of truth. Code interprets, schema defines.
//          Includes value-level validation: empty fields, version format, dates,
//          key prefix, filename match, type values, cross-field consistency.
//
// biblical_foundation: "Prove all things; hold fast that which is good."
//   — 1 Thessalonians 5:21
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { parse as parseToml } from "@std/toml";
import type {
  FormatHandler, LintResult, FixSuggestion, TransformOptions,
  AtomicAction, HealthScore,
} from "../types.ts";
import {
  error, warn, info,
  computeContainerScore, computeBlockScore, computeHealthScore,
} from "../types.ts";
import { registerFormat } from "./registry.ts";
import { loadRules, type DerivedRules } from "../schema-loader.ts";

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

// deno-lint-ignore no-explicit-any
type TomlData = Record<string, any>;

// ---------------------------------------------------------------------------
// Constants — Value validation patterns
// ---------------------------------------------------------------------------

/** Living version format: (a|b|c)-NN.NN */
const VERSION_PATTERN = /^[abc]-\d{2}\.\d{2}$/;

/** ISO date: YYYY-MM-DD */
const DATE_PATTERN = /^\d{4}-\d{2}-\d{2}$/;

/** All Bereshit keys start with B- */
const KEY_PREFIX = "B-";

/** Valid I2_family.type values */
const VALID_FAMILY_TYPES = new Set(["data", "code", "documentation"]);

// ---------------------------------------------------------------------------
// Pragma taxonomy — type + format + args
// ---------------------------------------------------------------------------
//
// Pragma line:  # #!omni <type> --<format> [--<arg>...]
// Format field: @omni <type> --<format> [--<arg>...]
//
// Types:  template, data, code
// Args:   derivations (templates derived from parent: --compiler, --cargo)
//         forms (shape variants of same template: --library, --executable, --test)

/** Parsed pragma/format information */
interface PragmaInfo {
  type: string;      // template, data, code
  format: string;    // toml, rs, go, c, etc.
  args: string[];    // derivation or form args
}

/** Valid main types for TOML files */
const VALID_PRAGMA_TYPES = ["template", "data", "code"];

/** Known derivations — templates derived from a parent template */
const KNOWN_DERIVATIONS = new Set(["compiler", "cargo"]);

/** Known forms — shape variants of the same base template */
const KNOWN_FORMS = new Set(["library", "executable", "test"]);

/** All known args (derivations + forms) */
const KNOWN_ARGS = new Set([...KNOWN_DERIVATIONS, ...KNOWN_FORMS]);

/**
 * Parse a pragma line: # #!omni <type> --<format> [--<arg>...]
 * Returns null if line doesn't match pragma pattern.
 */
function parsePragmaLine(line: string): PragmaInfo | null {
  const match = line.match(
    /^#\s*#!omni\s+(\w+)\s+--(\w+)((?:\s+--\w+)*)\s*$/,
  );
  if (!match) return null;
  const args = match[3]!.trim()
    ? match[3]!.trim().split(/\s+/).map((a) => a.replace(/^--/, ""))
    : [];
  return { type: match[1]!, format: match[2]!, args };
}

/**
 * Parse a format field: @omni <type> --<format> [--<arg>...]
 * Returns null if format doesn't match expected pattern.
 */
function parseFormatField(format: string): PragmaInfo | null {
  const match = format.match(
    /^@omni\s+(\w+)\s+--(\w+)((?:\s+--\w+)*)\s*$/,
  );
  if (!match) return null;
  const args = match[3]!.trim()
    ? match[3]!.trim().split(/\s+/).map((a) => a.replace(/^--/, ""))
    : [];
  return { type: match[1]!, format: match[2]!, args };
}

// ---------------------------------------------------------------------------
// Derivation layouts — how different pragma args remap table paths
// ---------------------------------------------------------------------------
//
// Standard TOML: _metadata, _content, _closing (direct top-level tables)
// Cargo.toml:    package.metadata.omni → _metadata
//                package.metadata.omni._closing → _closing
//                No _content table — content is in standard Cargo sections

/**
 * Normalize Cargo.toml data for linting.
 *
 * Cargo's sanctioned extension point is [package.metadata]. OmniCode metadata
 * lives at [package.metadata.omni]. This function remaps that namespace to the
 * standard _metadata/_closing tables so existing check functions work unchanged.
 *
 * Content check is skipped entirely for Cargo files — content IS the standard
 * Cargo sections ([dependencies], [features], [lints], etc.).
 */
function normalizeCargoData(data: TomlData): TomlData {
  const pkg = getTable(data, "package");
  if (!pkg) return data;
  const metadata = getTable(pkg, "metadata");
  if (!metadata) return data;
  const omni = getTable(metadata, "omni");
  if (!omni) return data;

  const normalized = { ...data };

  // Extract _metadata: everything in omni except _closing
  const meta: TomlData = {};
  for (const [key, val] of Object.entries(omni)) {
    if (key !== "_closing") {
      meta[key] = val;
    }
  }
  normalized["_metadata"] = meta;

  // Extract _closing from omni._closing
  if (has(omni, "_closing")) {
    normalized["_closing"] = omni["_closing"];
  }

  return normalized;
}

/**
 * Validate Cargo-specific content sections.
 *
 * Cargo.toml content is in standard Cargo sections, not a [_content] table.
 * This checks that essential Cargo sections are present.
 */
function checkCargoContent(
  file: string,
  data: TomlData,
  lineMap?: Map<string, number>,
): LintResult[] {
  const results: LintResult[] = [];

  // [package] is required
  if (!getTable(data, "package")) {
    results.push(
      error(file, "cargo/package", "Missing [package] — required for Cargo.toml",
        { line: lineMap?.get("package") }),
    );
  }

  // [dependencies] is expected (most crates need it)
  if (!getTable(data, "dependencies")) {
    results.push(
      info(file, "cargo/dependencies", "Missing [dependencies] — most crates need dependencies",
        { line: lineMap?.get("package") }),
    );
  }

  // [features] is expected for templates
  if (!getTable(data, "features")) {
    results.push(
      info(file, "cargo/features", "Missing [features] — templates should include features scaffold",
        { line: lineMap?.get("package") }),
    );
  }

  results.push(
    info(file, "cargo/layout", "Cargo.toml uses package.metadata.omni for OmniCode metadata — standard _content check skipped",
      { line: lineMap?.get("package.metadata.omni") ?? lineMap?.get("package") }),
  );

  return results;
}

// ---------------------------------------------------------------------------
// Schema cache — loaded once, used for all lint calls
// ---------------------------------------------------------------------------

// Schema rules loaded fresh from schema-loader.ts each call.
// schema-loader.ts manages its own lazy singleton cache.
// Calling loadRules() directly ensures clearCache() works for tests.

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

/**
 * Resolve a dotted path like "operations.half_adder" by walking the tree.
 * Falls back to single-key lookup for non-dotted paths.
 */
function resolveTable(obj: TomlData, path: string): TomlData | undefined {
  if (!path.includes(".")) return getTable(obj, path);
  const parts = path.split(".");
  let current: TomlData | undefined = obj;
  for (const part of parts) {
    current = getTable(current!, part);
    if (!current) return undefined;
  }
  return current;
}

// ---------------------------------------------------------------------------
// Helpers — Line number mapping
// ---------------------------------------------------------------------------

/**
 * Build a map from TOML paths to 1-based line numbers.
 *
 * Scans raw TOML text for:
 *   - Table headers: [name] or [parent.child]
 *   - Key-value pairs: key = value
 *
 * Returns entries like:
 *   "_metadata" → 5          (line where [_metadata] appears)
 *   "_metadata.I1_core" → 7  (line where [_metadata.I1_core] appears)
 *   "_metadata.I1_core.key" → 8  (line where key = ... appears under that table)
 *
 * Handles both standard and dotted table names. Does NOT handle inline tables
 * or arrays of tables — those aren't used in our TOML structure.
 */
function buildLineMap(text: string): Map<string, number> {
  const lineMap = new Map<string, number>();
  const lines = text.split("\n");
  let currentTable = "";

  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();
    const lineNum = i + 1; // 1-based

    // Table header: [table_name] or [parent.child]
    const tableMatch = trimmed.match(/^\[([^\]]+)\]$/);
    if (tableMatch) {
      currentTable = tableMatch[1]!;
      lineMap.set(currentTable, lineNum);
      continue;
    }

    // Skip comments and blank lines
    if (trimmed === "" || trimmed.startsWith("#")) continue;

    // Key-value pair: key = value
    const kvMatch = trimmed.match(/^([A-Za-z0-9_.-]+)\s*=/);
    if (kvMatch && currentTable) {
      const fullPath = `${currentTable}.${kvMatch[1]}`;
      lineMap.set(fullPath, lineNum);
    }
  }

  return lineMap;
}

// ---------------------------------------------------------------------------
// Helpers — Fix suggestion generation
// ---------------------------------------------------------------------------

/**
 * Generate a TOML snippet for a missing section.
 * Uses field_requirements from the schema to populate required keys.
 *
 * @param afterSection  Explicit insertion target (e.g., "_metadata.I2_family").
 *                      If omitted, defaults to the parent table path.
 */
function sectionFix(
  tablePath: string,
  sectionName: string,
  fields: Record<string, { required: string[]; defined: string[] }>,
  afterSection?: string,
): FixSuggestion {
  const fieldReqs = fields[sectionName];
  const keys = fieldReqs
    ? fieldReqs.required.map((k) => `${k} = ""`).join("\n")
    : "";
  const toml = keys ? `[${tablePath}]\n${keys}` : `[${tablePath}]`;
  const defaultAfter = tablePath.split(".").slice(0, -1).join(".") || tablePath;
  return {
    description: `Add missing [${tablePath}] table`,
    toml,
    location: `after ${afterSection ?? defaultAfter}`,
  };
}

/**
 * Find the nearest preceding existing metadata section in schema order.
 * Used to place missing sections after the correct sibling, not at the end.
 *
 * Schema order: I1, I2, I3 (required identity) → I4+ (extension identity)
 *             → C1, C2, C3, C4 (required context) → C5-C7, _contract (extension context)
 */
function findPrecedingMetaSection(
  meta: TomlData,
  targetKey: string,
  r: DerivedRules,
): string {
  const order = [
    ...r.requiredIdentity,
    ...r.extensionIdentity,
    ...r.requiredContext,
    ...r.extensionContext,
  ];
  const idx = order.indexOf(targetKey);
  for (let i = (idx >= 0 ? idx : order.length) - 1; i >= 0; i--) {
    if (has(meta, order[i]!)) return `_metadata.${order[i]!}`;
  }
  return "_metadata";
}

/**
 * Find the nearest preceding existing closing section in schema order.
 */
function findPrecedingCloseSection(
  closing: TomlData,
  targetKey: string,
  r: DerivedRules,
): string {
  const order = [
    ...r.requiredClosing,
    ...r.definedClosing,
    ...r.templateClosing,
  ];
  const idx = order.indexOf(targetKey);
  for (let i = (idx >= 0 ? idx : order.length) - 1; i >= 0; i--) {
    if (has(closing, order[i]!)) return `_closing.${order[i]!}`;
  }
  return "_closing";
}

/**
 * Generate a fix for a missing field within an existing section.
 */
function fieldFix(path: string, fieldName: string): FixSuggestion {
  return {
    description: `Add missing ${fieldName} field`,
    toml: `${fieldName} = ""`,
    location: `in ${path}`,
  };
}

// ---------------------------------------------------------------------------
// Helpers — Field validation (required/defined per section)
// ---------------------------------------------------------------------------

/**
 * Check required and defined fields within a TOML table.
 * Required fields produce errors with fix suggestions.
 * Defined fields produce info with fix suggestions.
 * Only call this when the parent table is confirmed to exist.
 */
function checkFields(
  file: string,
  results: LintResult[],
  table: TomlData,
  path: string,
  required: readonly string[],
  defined: readonly string[],
  lineMap?: Map<string, number>,
): void {
  for (const f of required) {
    if (!has(table, f)) {
      const fix = fieldFix(path, f);
      // Line of the table header where this field should exist
      const line = lineMap?.get(path);
      results.push(
        error(file, `${path}/${f}`, `Missing ${path}.${f}`, line ? { line, fix } : fix),
      );
    }
  }
  for (const f of defined) {
    if (!has(table, f)) {
      const fix = fieldFix(path, f);
      const line = lineMap?.get(path);
      results.push(
        info(
          file,
          `${path}/${f}`,
          `Missing ${path}.${f} — defined field`,
          line ? { line, fix } : fix,
        ),
      );
    }
  }
}

// ---------------------------------------------------------------------------
// Core — Metadata validation (schema-driven)
// ---------------------------------------------------------------------------

function checkMetadata(
  file: string,
  data: TomlData,
  r: DerivedRules,
  lineMap?: Map<string, number>,
): LintResult[] {
  const results: LintResult[] = [];
  const meta = getTable(data, "_metadata");

  if (!meta) {
    results.push(
      error(file, "metadata/exists", "Missing [_metadata] table", {
        description: "Add [_metadata] block",
        toml: "[_metadata]",
        location: "after pragma line",
      }),
    );
    return results;
  }

  const metaLine = lineMap?.get("_metadata");

  // I — Identity sections (required)
  for (const key of r.requiredIdentity) {
    if (!has(meta, key)) {
      const after = findPrecedingMetaSection(meta, key, r);
      const fix = sectionFix(`_metadata.${key}`, key, r.metadataFields, after);
      const line = lineMap?.get(`_metadata.${key}`) ?? metaLine;
      results.push(
        error(file, `metadata/${key}`, `Missing _metadata.${key} — REQUIRED`,
          line ? { line, fix } : fix),
      );
    }
  }

  // I — Identity sections (extension)
  for (const key of r.extensionIdentity) {
    if (!has(meta, key)) {
      const after = findPrecedingMetaSection(meta, key, r);
      const fix = sectionFix(`_metadata.${key}`, key, r.metadataFields, after);
      const line = lineMap?.get(`_metadata.${key}`) ?? metaLine;
      results.push(
        warn(file, `metadata/${key}`, `Missing _metadata.${key} — extension identity`,
          line ? { line, fix } : fix),
      );
    }
  }

  // C — Context sections (required)
  for (const key of r.requiredContext) {
    if (!has(meta, key)) {
      const after = findPrecedingMetaSection(meta, key, r);
      const fix = sectionFix(`_metadata.${key}`, key, r.metadataFields, after);
      const line = lineMap?.get(`_metadata.${key}`) ?? metaLine;
      results.push(
        error(file, `metadata/${key}`, `Missing _metadata.${key} — REQUIRED`,
          line ? { line, fix } : fix),
      );
    }
  }

  // C — Context sections (extension)
  for (const key of r.extensionContext) {
    if (!has(meta, key)) {
      const after = findPrecedingMetaSection(meta, key, r);
      const fix = sectionFix(`_metadata.${key}`, key, r.metadataFields, after);
      const line = lineMap?.get(`_metadata.${key}`) ?? metaLine;
      results.push(
        warn(file, `metadata/${key}`, `Missing _metadata.${key} — extension context`,
          line ? { line, fix } : fix),
      );
    }
  }

  // ── Field validation: iterate all metadata field requirements from schema ──
  for (const [sectionName, fieldReqs] of Object.entries(r.metadataFields)) {
    const section = getTable(meta, sectionName);
    if (section) {
      checkFields(
        file,
        results,
        section,
        `_metadata.${sectionName}`,
        fieldReqs.required,
        fieldReqs.defined,
        lineMap,
      );
    }
  }

  // C7 tags should be an array, not a string (value-level check, not in schema)
  const c7 = getTable(meta, "C7_classification");
  if (c7 && has(c7, "tags")) {
    if (typeof c7["tags"] === "string") {
      const line = lineMap?.get("_metadata.C7_classification.tags");
      results.push(
        warn(file, "metadata/C7_classification/tags",
          "C7_classification.tags should be an array, not a comma-separated string",
          line ? { line } : undefined),
      );
    }
  }

  // ── Structural lock: validate ALL keys under _metadata ──────────────
  for (const key of Object.keys(meta)) {
    const keyLine = lineMap?.get(`_metadata.${key}`);

    // 1. Pattern validation
    if (!r.metadataKeyPattern.test(key)) {
      results.push(
        error(file, `metadata/invalid-key/${key}`,
          `Invalid key _metadata.${key} — must match I{N}_{name}, C{N}_{name}, or _{name}`,
          keyLine ? { line: keyLine } : undefined),
      );
      continue;
    }

    // 2. Type validation — all metadata sections must be tables
    const val = meta[key];
    if (val !== null && val !== undefined && typeof val !== "object") {
      results.push(
        error(file, `metadata/${key}/type`,
          `_metadata.${key} must be a table, got ${typeof val}`,
          keyLine ? { line: keyLine } : undefined),
      );
    }

    // 3. Unknown but valid-pattern extensions — info only
    if (!r.allKnownSections.has(key)) {
      results.push(
        info(file, `metadata/${key}/extension`,
          `_metadata.${key} is a valid extension (not in defined set)`,
          keyLine ? { line: keyLine } : undefined),
      );
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Content validation (schema-driven)
// ---------------------------------------------------------------------------

function checkContent(
  file: string,
  data: TomlData,
  r: DerivedRules,
  lineMap?: Map<string, number>,
): LintResult[] {
  const results: LintResult[] = [];
  const content = getTable(data, "_content");

  if (!content) {
    results.push(
      error(file, "content/exists", "Missing [_content] table", {
        description: "Add [_content] block header",
        toml: '[_content]\nformat = "TOML — UTF-8, LF line endings"\nCc = ""\nCo = ""\nCv = ""',
        location: "after metadata block",
      }),
    );
    return results;
  }

  // Check Cc/Co/Cv zone labels
  const contentLine = lineMap?.get("_content");
  for (const label of ["Cc", "Co", "Cv"]) {
    if (!has(content, label)) {
      const fix = fieldFix("_content", label);
      const line = lineMap?.get(`_content.${label}`) ?? contentLine;
      results.push(
        warn(file, `content/${label}`, `Missing _content.${label} zone label`,
          line ? { line, fix } : fix),
      );
    }
  }

  // ── Cc zone: required content sections ──────────────────────────
  for (const key of r.requiredContent) {
    if (!getTable(data, key)) {
      const fix = sectionFix(key, key, r.contentFields);
      const line = lineMap?.get(key) ?? contentLine;
      results.push(
        error(
          file,
          `content/cc/${key}`,
          `Missing [${key}] — REQUIRED content section`,
          line ? { line, fix } : fix,
        ),
      );
    }
  }

  // ── Cc zone: defined sections (expected, not mandated) ──────────
  for (const key of r.definedCc) {
    if (!getTable(data, key)) {
      results.push(
        info(
          file,
          `content/cc/${key}`,
          `Missing [${key}] — defined Cc section`,
          { line: lineMap?.get(key) ?? contentLine },
        ),
      );
    }
  }

  // ── Co zone: defined sections ───────────────────────────────────
  for (const key of r.definedCo) {
    if (!getTable(data, key)) {
      results.push(
        info(
          file,
          `content/co/${key}`,
          `Missing [${key}] — defined Co section`,
          { line: lineMap?.get(key) ?? contentLine },
        ),
      );
    }
  }

  // ── Cv zone: required validation ────────────────────────────────
  for (const key of r.requiredValidation) {
    if (!getTable(data, key)) {
      const fix = sectionFix(key, key, r.contentFields);
      const line = lineMap?.get(key) ?? contentLine;
      results.push(
        error(
          file,
          `content/cv/${key}`,
          `Missing [${key}] — REQUIRED validation section`,
          line ? { line, fix } : fix,
        ),
      );
    }
  }

  // ── Field validation: iterate all content field requirements from schema ──
  for (const [sectionName, fieldReqs] of Object.entries(r.contentFields)) {
    const section = getTable(data, sectionName);
    if (section) {
      checkFields(
        file,
        results,
        section,
        sectionName,
        fieldReqs.required,
        fieldReqs.defined,
        lineMap,
      );
    }
  }

  // ── Schema cross-validation: declared contract vs reality ──────

  const validation = getTable(data, "_validation");
  const schema = validation ? getTable(validation, "schema") : undefined;
  const validationLine = lineMap?.get("_validation");
  const schemaLine = lineMap?.get("_validation.schema") ?? validationLine;
  if (schema) {
    // Check schema.required sections actually exist
    const schemaReq = schema["required"];
    if (Array.isArray(schemaReq)) {
      for (const section of schemaReq) {
        if (typeof section === "string" && !resolveTable(data, section)) {
          results.push(
            error(
              file,
              `content/schema/${section}`,
              `Schema declares [${section}] as required, but section is missing`,
              { line: lineMap?.get("_validation.schema.required") ?? schemaLine },
            ),
          );
        }
      }
    }

    // Check schema.defined sections — info if declared but missing
    const schemaDef = schema["defined"];
    if (Array.isArray(schemaDef)) {
      for (const section of schemaDef) {
        if (typeof section === "string" && !resolveTable(data, section)) {
          results.push(
            info(
              file,
              `content/schema/${section}`,
              `Schema declares [${section}] as defined, but section is missing`,
              { line: lineMap?.get("_validation.schema.defined") ?? schemaLine },
            ),
          );
        }
      }
    }

    // Check schema.fields — validate declared field requirements vs actual content
    const schemaFields = getTable(schema, "fields");
    if (schemaFields) {
      for (const sectionName of Object.keys(schemaFields)) {
        const fieldSpec = getTable(schemaFields, sectionName);
        const actualSection = resolveTable(data, sectionName);
        if (!fieldSpec || !actualSection) continue;

        const reqFields = fieldSpec["required"];
        if (Array.isArray(reqFields)) {
          for (const f of reqFields) {
            if (typeof f === "string" && !has(actualSection, f)) {
              const fix = fieldFix(sectionName, f);
              const line = lineMap?.get(`${sectionName}.${f}`) ?? lineMap?.get(sectionName);
              results.push(
                error(
                  file,
                  `content/schema/fields/${sectionName}/${f}`,
                  `Schema declares ${sectionName}.${f} as required, but field is missing`,
                  line ? { line, fix } : fix,
                ),
              );
            }
          }
        }

        const defFields = fieldSpec["defined"];
        if (Array.isArray(defFields)) {
          for (const f of defFields) {
            if (typeof f === "string" && !has(actualSection, f)) {
              const fix = fieldFix(sectionName, f);
              const line = lineMap?.get(`${sectionName}.${f}`) ?? lineMap?.get(sectionName);
              results.push(
                info(
                  file,
                  `content/schema/fields/${sectionName}/${f}`,
                  `Schema declares ${sectionName}.${f} as defined, but field is missing`,
                  line ? { line, fix } : fix,
                ),
              );
            }
          }
        }
      }
    }
  }

  // ── Extension zones: must be tables if present ──────────────────
  const ext = getTable(data, "_extensions");
  if (ext) {
    for (const zone of r.extensionZones) {
      if (has(ext, zone) && typeof ext[zone] !== "object") {
        results.push(
          warn(
            file,
            `content/extensions/${zone}/type`,
            `_extensions.${zone} must be a table, got ${typeof ext[zone]}`,
            { line: lineMap?.get(`_extensions.${zone}`) },
          ),
        );
      }
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Closing validation (schema-driven)
// ---------------------------------------------------------------------------

function checkClosing(
  file: string,
  data: TomlData,
  r: DerivedRules,
  lineMap?: Map<string, number>,
): LintResult[] {
  const results: LintResult[] = [];
  const closing = getTable(data, "_closing");

  if (!closing) {
    results.push(
      error(file, "closing/exists", "Missing [_closing] table", {
        description: "Add [_closing] block",
        toml: "[_closing]",
        location: "after content block",
      }),
    );
    return results;
  }

  const closingLine = lineMap?.get("_closing");

  // X — Required closing sections
  for (const key of r.requiredClosing) {
    if (!has(closing, key)) {
      const after = findPrecedingCloseSection(closing, key, r);
      const fix = sectionFix(`_closing.${key}`, key, r.closingFields, after);
      const line = lineMap?.get(`_closing.${key}`) ?? closingLine;
      results.push(
        error(file, `closing/${key}`, `Missing _closing.${key} — REQUIRED`,
          line ? { line, fix } : fix),
      );
    }
  }

  // X — Defined closing sections (expected, not mandated)
  for (const key of r.definedClosing) {
    if (!has(closing, key)) {
      const after = findPrecedingCloseSection(closing, key, r);
      const fix = sectionFix(`_closing.${key}`, key, r.closingFields, after);
      const line = lineMap?.get(`_closing.${key}`) ?? closingLine;
      results.push(
        info(file, `closing/${key}`, `Missing _closing.${key} — defined closing section`,
          line ? { line, fix } : fix),
      );
    }
  }

  // ── Field validation: iterate all closing field requirements from schema ──
  for (const [sectionName, fieldReqs] of Object.entries(r.closingFields)) {
    const section = getTable(closing, sectionName);
    if (section) {
      checkFields(
        file,
        results,
        section,
        `_closing.${sectionName}`,
        fieldReqs.required,
        fieldReqs.defined,
        lineMap,
      );
    }
  }

  // ── Structural lock: validate ALL keys under _closing ──────────────
  for (const key of Object.keys(closing)) {
    const keyLine = lineMap?.get(`_closing.${key}`);

    // 1. Pattern validation
    if (!r.closingKeyPattern.test(key)) {
      results.push(
        error(file, `closing/invalid-key/${key}`,
          `Invalid key _closing.${key} — must match X{N}_{name} or _{name}`,
          keyLine ? { line: keyLine } : undefined),
      );
      continue;
    }

    // 2. Type validation — all closing sections must be tables
    const val = closing[key];
    if (val !== null && val !== undefined && typeof val !== "object") {
      results.push(
        error(file, `closing/${key}/type`,
          `_closing.${key} must be a table, got ${typeof val}`,
          keyLine ? { line: keyLine } : undefined),
      );
    }

    // 3. Unknown but valid-pattern extensions — info only
    if (!r.allKnownClosing.has(key)) {
      results.push(
        info(file, `closing/${key}/extension`,
          `_closing.${key} is a valid extension (not in defined set)`,
          keyLine ? { line: keyLine } : undefined),
      );
    }
  }

  // ── X section ordering — X1 before X2 before X3, etc. ──────────────
  const xSections: Array<{ key: string; num: number }> = [];
  for (const key of Object.keys(closing)) {
    const m = key.match(/^X(\d+)_/);
    if (m) {
      xSections.push({ key, num: parseInt(m[1]!, 10) });
    }
  }

  if (xSections.length >= 2) {
    let lastNum = -1;
    for (const xs of xSections) {
      if (xs.num < lastNum) {
        const foundOrder = xSections.map((x) => x.key).join(" → ");
        const xLine = lineMap?.get(`_closing.${xs.key}`);
        results.push(
          warn(file, "closing/x-order",
            `_closing.${xs.key} (X${xs.num}) appears after X${lastNum} — expected ascending order: ${foundOrder}`,
            xLine ? { line: xLine } : undefined),
        );
        break;
      }
      lastNum = xs.num;
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Consistency checks (schema-driven)
// ---------------------------------------------------------------------------

function checkConsistency(
  file: string,
  data: TomlData,
  r: DerivedRules,
  pragma: PragmaInfo | null = null,
  lineMap?: Map<string, number>,
): LintResult[] {
  const results: LintResult[] = [];
  const meta = getTable(data, "_metadata");

  if (!meta) return results;

  // lineMap helper — look up field line with fallback to section line
  const ln = (path: string): number | undefined => lineMap?.get(path);

  // ── Pragma line validation ───────────────────────────────────────
  if (pragma) {
    // Pragma type must be valid
    if (!VALID_PRAGMA_TYPES.includes(pragma.type)) {
      results.push(
        error(
          file,
          "consistency/pragma/type",
          `Pragma type "${pragma.type}" — expected one of: ${VALID_PRAGMA_TYPES.join(", ")}`,
          { line: 1 },
        ),
      );
    }

    // Pragma format must be "toml" for TOML files
    if (pragma.format !== "toml") {
      results.push(
        warn(
          file,
          "consistency/pragma/format",
          `Pragma format "--${pragma.format}" — expected --toml for .toml files`,
          { line: 1 },
        ),
      );
    }

    // Pragma args should be recognized
    for (const arg of pragma.args) {
      if (!KNOWN_ARGS.has(arg)) {
        results.push(
          info(
            file,
            "consistency/pragma/arg",
            `Pragma arg "--${arg}" is not a known derivation or form (known: ${[...KNOWN_ARGS].join(", ")})`,
            { line: 1 },
          ),
        );
      }
    }
  }

  // ── I1_core.format ↔ pragma consistency ──────────────────────────
  const i1 = getTable(meta, "I1_core");
  if (i1 && has(i1, "format")) {
    const format = i1["format"] as string;
    if (!format.startsWith("@omni")) {
      results.push(
        warn(
          file,
          "consistency/format",
          `I1_core.format="${format}" — expected @omni prefix`,
          { line: ln("_metadata.I1_core.format") },
        ),
      );
    }

    // Parse the format field and cross-check with pragma
    const formatInfo = parseFormatField(format);
    if (pragma && formatInfo) {
      // Type must match: pragma type ↔ format type
      if (pragma.type !== formatInfo.type) {
        results.push(
          warn(
            file,
            "consistency/pragma-vs-format/type",
            `Pragma type "${pragma.type}" ≠ format type "${formatInfo.type}" — these must match`,
            { line: ln("_metadata.I1_core.format") },
          ),
        );
      }

      // Args must match: pragma args ↔ format args
      const pragmaArgs = pragma.args.sort().join(",");
      const formatArgs = formatInfo.args.sort().join(",");
      if (pragmaArgs !== formatArgs) {
        results.push(
          warn(
            file,
            "consistency/pragma-vs-format/args",
            `Pragma args [${pragma.args.join(", ")}] ≠ format args [${formatInfo.args.join(", ")}] — these must match`,
            { line: ln("_metadata.I1_core.format") },
          ),
        );
      }
    } else if (pragma && !formatInfo) {
      results.push(
        info(
          file,
          "consistency/format/parse",
          `I1_core.format="${format}" — could not parse type/args (expected @omni <type> --<format> [--<args>])`,
          { line: ln("_metadata.I1_core.format") },
        ),
      );
    }

    // I1_core.at should match pragma type (when at is a keyword)
    if (pragma && i1 && has(i1, "at")) {
      const at = i1["at"] as string;
      if (VALID_PRAGMA_TYPES.includes(at) && at !== pragma.type) {
        results.push(
          warn(
            file,
            "consistency/pragma-vs-at",
            `I1_core.at="${at}" ≠ pragma type "${pragma.type}" — these must match`,
            { line: ln("_metadata.I1_core.at") },
          ),
        );
      }
    }
  }

  // I2_family.structure should be "3-block" for TOML
  const i2 = getTable(meta, "I2_family");
  if (i2 && has(i2, "structure")) {
    const structure = i2["structure"] as string;
    if (structure !== "3-block") {
      results.push(
        warn(
          file,
          "consistency/structure",
          `I2_family.structure="${structure}" — TOML files should be 3-block`,
          { line: ln("_metadata.I2_family.structure") },
        ),
      );
    }
  }

  // _contract — validate against schema-derived required lists
  const contract = getTable(meta, "_contract");
  if (contract) {
    // required_identity must list all required identity items
    const reqId = contract["required_identity"];
    if (Array.isArray(reqId)) {
      for (const key of r.requiredIdentity) {
        if (!reqId.includes(key)) {
          results.push(
            warn(
              file,
              "consistency/contract/identity",
              `_contract.required_identity missing "${key}"`,
              { line: ln("_metadata._contract.required_identity") },
            ),
          );
        }
      }
    } else {
      results.push(
        warn(
          file,
          "consistency/contract/identity",
          "_contract.required_identity should be an array",
          { line: ln("_metadata._contract.required_identity") },
        ),
      );
    }

    // required_context must list all required context items
    const reqCtx = contract["required_context"];
    if (Array.isArray(reqCtx)) {
      for (const key of r.requiredContext) {
        if (!reqCtx.includes(key)) {
          results.push(
            warn(
              file,
              "consistency/contract/context",
              `_contract.required_context missing "${key}"`,
              { line: ln("_metadata._contract.required_context") },
            ),
          );
        }
      }
    } else {
      results.push(
        warn(
          file,
          "consistency/contract/context",
          "_contract.required_context should be an array",
          { line: ln("_metadata._contract.required_context") },
        ),
      );
    }

    // extension lists should be arrays if present
    for (const extKey of ["extension_identity", "extension_context"]) {
      if (has(contract, extKey) && !Array.isArray(contract[extKey])) {
        results.push(
          warn(
            file,
            `consistency/contract/${extKey}`,
            `_contract.${extKey} should be an array`,
            { line: ln(`_metadata._contract.${extKey}`) },
          ),
        );
      }
    }

    // required_closing must list all required closing items
    const reqClose = contract["required_closing"];
    if (Array.isArray(reqClose)) {
      for (const key of r.requiredClosing) {
        if (!reqClose.includes(key)) {
          results.push(
            warn(
              file,
              "consistency/contract/closing",
              `_contract.required_closing missing "${key}"`,
              { line: ln("_metadata._contract.required_closing") },
            ),
          );
        }
      }
    } else if (has(contract, "required_closing")) {
      results.push(
        warn(
          file,
          "consistency/contract/closing",
          "_contract.required_closing should be an array",
          { line: ln("_metadata._contract.required_closing") },
        ),
      );
    }

    // defined_closing should be an array if present
    if (
      has(contract, "defined_closing") &&
      !Array.isArray(contract["defined_closing"])
    ) {
      results.push(
        warn(
          file,
          "consistency/contract/defined_closing",
          "_contract.defined_closing should be an array",
          { line: ln("_metadata._contract.defined_closing") },
        ),
      );
    }

    // valid_patterns should be an array if present
    if (
      has(contract, "valid_patterns") &&
      !Array.isArray(contract["valid_patterns"])
    ) {
      results.push(
        warn(
          file,
          "consistency/contract/patterns",
          "_contract.valid_patterns should be an array",
          { line: ln("_metadata._contract.valid_patterns") },
        ),
      );
    }

    // valid_patterns should include X{N}_{name} for closing
    const patterns = contract["valid_patterns"];
    if (Array.isArray(patterns)) {
      if (!patterns.some((p: string) => /^X(\d+_|\{[A-Z]+\}_)/.test(p))) {
        results.push(
          warn(
            file,
            "consistency/contract/closing_patterns",
            "_contract.valid_patterns missing closing pattern (X{N}_{name})",
            { line: ln("_metadata._contract.valid_patterns") },
          ),
        );
      }
    }

    // Reverse validation: contract declarations must match actual sections.
    // A contract declaring "I99_phantom" when no [_metadata.I99_phantom] exists
    // is a phantom declaration — likely a typo or stale reference.
    const metaKeys = Object.keys(meta).filter((k) => !k.startsWith("_"));
    for (const listKey of ["required_identity", "extension_identity"] as const) {
      const declared = contract[listKey];
      if (Array.isArray(declared)) {
        for (const item of declared as string[]) {
          if (!metaKeys.includes(item)) {
            results.push(
              warn(
                file,
                `consistency/contract/phantom_${listKey}`,
                `_contract.${listKey} declares "${item}" but [_metadata.${item}] does not exist`,
                { line: ln(`_metadata._contract.${listKey}`) },
              ),
            );
          }
        }
      }
    }
    for (const listKey of ["required_context", "extension_context"] as const) {
      const declared = contract[listKey];
      if (Array.isArray(declared)) {
        for (const item of declared as string[]) {
          if (!metaKeys.includes(item)) {
            results.push(
              warn(
                file,
                `consistency/contract/phantom_${listKey}`,
                `_contract.${listKey} declares "${item}" but [_metadata.${item}] does not exist`,
                { line: ln(`_metadata._contract.${listKey}`) },
              ),
            );
          }
        }
      }
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Field value validation (value-level, not structural)
// ---------------------------------------------------------------------------
//
// Structural checks verify that sections and fields EXIST.
// Value checks verify that existing field values are CORRECT:
//   Level 1: Empty required fields (placeholders that were never filled)
//   Level 2: Format-specific patterns (version, date, key prefix, type)
//   Level 3: Cross-field consistency (version match, filename match)

function checkFieldValues(
  file: string,
  data: TomlData,
  r: DerivedRules,
  pragma: PragmaInfo | null = null,
  lineMap?: Map<string, number>,
): LintResult[] {
  const results: LintResult[] = [];
  const meta = getTable(data, "_metadata");

  // lineMap helper — look up field line
  const ln = (path: string): number | undefined => lineMap?.get(path);

  // ── Level 1: Empty required fields (all blocks) ──────────────────

  // Metadata sections
  if (meta) {
    for (const [sectionName, fieldReqs] of Object.entries(r.metadataFields)) {
      const section = getTable(meta, sectionName);
      if (!section) continue;
      for (const f of fieldReqs.required) {
        const val = section[f];
        if (typeof val === "string" && val.trim() === "") {
          results.push(
            warn(
              file,
              `values/${sectionName}/${f}/empty`,
              `_metadata.${sectionName}.${f} is empty — required field needs a value`,
              { line: ln(`_metadata.${sectionName}.${f}`) },
            ),
          );
        }
        if (Array.isArray(val) && val.length === 0) {
          results.push(
            warn(
              file,
              `values/${sectionName}/${f}/empty`,
              `_metadata.${sectionName}.${f} is an empty array — required field needs content`,
              { line: ln(`_metadata.${sectionName}.${f}`) },
            ),
          );
        }
      }
    }
  }

  // Content sections
  for (const [sectionName, fieldReqs] of Object.entries(r.contentFields)) {
    const section = getTable(data, sectionName);
    if (!section) continue;
    for (const f of fieldReqs.required) {
      const val = section[f];
      if (typeof val === "string" && val.trim() === "") {
        results.push(
          warn(
            file,
            `values/${sectionName}/${f}/empty`,
            `${sectionName}.${f} is empty — required field needs a value`,
            { line: ln(`${sectionName}.${f}`) },
          ),
        );
      }
    }
  }

  // Closing sections
  const closing = getTable(data, "_closing");
  if (closing) {
    for (const [sectionName, fieldReqs] of Object.entries(r.closingFields)) {
      const section = getTable(closing, sectionName);
      if (!section) continue;
      for (const f of fieldReqs.required) {
        const val = section[f];
        if (typeof val === "string" && val.trim() === "") {
          results.push(
            warn(
              file,
              `values/${sectionName}/${f}/empty`,
              `_closing.${sectionName}.${f} is empty — required field needs a value`,
              { line: ln(`_closing.${sectionName}.${f}`) },
            ),
          );
        }
        if (Array.isArray(val) && val.length === 0) {
          results.push(
            warn(
              file,
              `values/${sectionName}/${f}/empty`,
              `_closing.${sectionName}.${f} is an empty array — required field needs content`,
              { line: ln(`_closing.${sectionName}.${f}`) },
            ),
          );
        }
      }
    }
  }

  // Without metadata, remaining checks are metadata-specific — bail early
  if (!meta) return results;

  // ── Level 2: Format-specific patterns ────────────────────────────

  const i1 = getTable(meta, "I1_core");
  // Keywords that skip version consistency (at vs C1_state.version)
  const atValue = i1 != null && typeof i1["at"] === "string" ? i1["at"] : "";
  const isKeywordAt = VALID_PRAGMA_TYPES.includes(atValue);
  // Determine effective type from pragma (preferred) or at field (fallback)
  const effectiveType = pragma?.type ?? (isKeywordAt ? atValue : null);

  if (i1) {
    // Key prefix: Bereshit keys start with B-
    const key = typeof i1["key"] === "string" ? i1["key"] : "";
    if (key && !key.startsWith(KEY_PREFIX)) {
      results.push(
        warn(
          file,
          "values/I1_core/key/prefix",
          `I1_core.key="${key}" — expected ${KEY_PREFIX} prefix`,
          { line: ln("_metadata.I1_core.key") },
        ),
      );
    }

    // Version format: (a|b|c)-NN.NN or lifecycle keyword
    // Keywords: "template" (seed), "data" (specification), "code" (implementation)
    const at = typeof i1["at"] === "string" ? i1["at"] : "";
    const AT_KEYWORDS = ["template", "data", "code"];
    if (at && !AT_KEYWORDS.includes(at) && !VERSION_PATTERN.test(at)) {
      results.push(
        error(
          file,
          "values/I1_core/at/format",
          `I1_core.at="${at}" — expected version pattern (a|b|c)-NN.NN or keyword (${AT_KEYWORDS.join(", ")})`,
          { line: ln("_metadata.I1_core.at") },
        ),
      );
    }
  }

  // I2 type validation
  const i2 = getTable(meta, "I2_family");
  if (i2) {
    const familyType = typeof i2["type"] === "string" ? i2["type"] : "";
    if (familyType && !VALID_FAMILY_TYPES.has(familyType)) {
      results.push(
        info(
          file,
          "values/I2_family/type/unknown",
          `I2_family.type="${familyType}" — expected data, code, or documentation`,
          { line: ln("_metadata.I2_family.type") },
        ),
      );
    }

    // Pragma type ↔ family type consistency
    // "template" pragma type is compatible with any family type (templates can be data or code)
    // "data" pragma type should have I2.type = "data"
    // "code" pragma type should have I2.type = "code"
    if (effectiveType && familyType && effectiveType !== "template") {
      if (effectiveType !== familyType) {
        results.push(
          warn(
            file,
            "consistency/pragma-vs-family-type",
            `Pragma type "${effectiveType}" ≠ I2_family.type "${familyType}" — data files should have type "data", code files should have type "code"`,
            { line: ln("_metadata.I2_family.type") },
          ),
        );
      }
    }
  }

  // I3 filename match
  const i3 = getTable(meta, "I3_instance");
  if (i3) {
    const declaredFile =
      typeof i3["file"] === "string" ? i3["file"] : "";
    const actualFilename = file.split("/").pop() ?? "";
    if (declaredFile && actualFilename && declaredFile !== actualFilename) {
      results.push(
        warn(
          file,
          "values/I3_instance/file/mismatch",
          `I3_instance.file="${declaredFile}" but actual filename is "${actualFilename}"`,
          { line: ln("_metadata.I3_instance.file") },
        ),
      );
    }
  }

  // C1 version and date validation
  const c1 = getTable(meta, "C1_state");
  if (c1) {
    // Version format
    const version = typeof c1["version"] === "string" ? c1["version"] : "";
    if (version && !VERSION_PATTERN.test(version)) {
      results.push(
        error(
          file,
          "values/C1_state/version/format",
          `C1_state.version="${version}" — expected version pattern (a|b|c)-NN.NN`,
          { line: ln("_metadata.C1_state.version") },
        ),
      );
    }

    // Date format
    for (const dateField of ["created", "updated"] as const) {
      const dateVal = c1[dateField];
      if (
        typeof dateVal === "string" &&
        dateVal &&
        !DATE_PATTERN.test(dateVal)
      ) {
        results.push(
          warn(
            file,
            `values/C1_state/${dateField}/format`,
            `C1_state.${dateField}="${dateVal}" — expected YYYY-MM-DD`,
            { line: ln(`_metadata.C1_state.${dateField}`) },
          ),
        );
      }
    }

    // Date consistency: created should not be after updated
    const created =
      typeof c1["created"] === "string" ? c1["created"] : "";
    const updated =
      typeof c1["updated"] === "string" ? c1["updated"] : "";
    if (
      created &&
      updated &&
      DATE_PATTERN.test(created) &&
      DATE_PATTERN.test(updated) &&
      created > updated
    ) {
      results.push(
        info(
          file,
          "values/C1_state/date-order",
          `C1_state.created="${created}" is after C1_state.updated="${updated}"`,
          { line: ln("_metadata.C1_state.created") },
        ),
      );
    }
  }

  // ── Level 3: Cross-field consistency ─────────────────────────────

  // Version consistency: I1_core.at should match C1_state.version
  // (skip when at is a keyword — template, data, code)
  if (i1 && c1 && !isKeywordAt) {
    const at = typeof i1["at"] === "string" ? i1["at"] : "";
    const version = typeof c1["version"] === "string" ? c1["version"] : "";
    if (
      at &&
      version &&
      VERSION_PATTERN.test(at) &&
      VERSION_PATTERN.test(version) &&
      at !== version
    ) {
      results.push(
        warn(
          file,
          "values/consistency/version",
          `I1_core.at="${at}" differs from C1_state.version="${version}"`,
          { line: ln("_metadata.I1_core.at") },
        ),
      );
    }
  }

  // ── Level 4: Pragma classification info ────────────────────────────

  if (pragma) {
    // Classify args as derivations or forms
    const derivations = pragma.args.filter((a) => KNOWN_DERIVATIONS.has(a));
    const forms = pragma.args.filter((a) => KNOWN_FORMS.has(a));

    if (derivations.length > 0) {
      results.push(
        info(
          file,
          "pragma/derivations",
          `Derived template: ${derivations.join(", ")} (from parent template)`,
          { line: 1 },
        ),
      );
    }

    if (forms.length > 0) {
      results.push(
        info(
          file,
          "pragma/forms",
          `Template form: ${forms.join(", ")} (shape variant)`,
          { line: 1 },
        ),
      );
    }

    results.push(
      info(
        file,
        "pragma/classification",
        `Pragma: type=${pragma.type}, format=${pragma.format}${pragma.args.length ? ", args=" + pragma.args.join(",") : ""}`,
        { line: 1 },
      ),
    );
  }

  return results;
}

// ---------------------------------------------------------------------------
// Core — Content body validation (zone ordering, density, coherence)
// ---------------------------------------------------------------------------
//
// Structural checks verify content sections EXIST.
// Value checks verify field values are CORRECT.
// Body checks verify content ORGANIZATION is sound:
//   1. Content density — the content block should have actual data tables
//   2. Zone label coherence — Cc/Co/Cv labels should be non-empty strings
//   3. Zone ordering — Cc sections before Co before Cv in file

/**
 * Classify a top-level TOML section into its content zone.
 *
 * Zone membership is determined by the schema's section lists:
 *   Cc → requiredContent + definedCc (identity, settings, resources)
 *   Co → definedCo (behavior, integrations)
 *   Cv → requiredValidation (_validation) + _validation.* subtables
 *
 * Returns "meta", "closing", "content-header", "extension" for structural
 * tables, or "Cc", "Co", "Cv", "unknown" for content tables.
 */
function classifyZone(
  sectionPath: string,
  r: DerivedRules,
): "meta" | "closing" | "content-header" | "extension" | "Cc" | "Co" | "Cv" | "unknown" {
  // Structural tables
  if (sectionPath.startsWith("_metadata")) return "meta";
  if (sectionPath.startsWith("_closing")) return "closing";
  if (sectionPath === "_content") return "content-header";
  if (sectionPath.startsWith("_extensions")) return "extension";

  // Content zones — classify by root key
  const rootKey = sectionPath.split(".")[0]!;

  // Cv zone: _validation and its subtables
  if (rootKey === "_validation") return "Cv";

  // Cc zone: required content + defined Cc sections
  if (r.requiredContent.includes(rootKey) || r.definedCc.includes(rootKey)) return "Cc";

  // Co zone: defined Co sections
  if (r.definedCo.includes(rootKey)) return "Co";

  return "unknown";
}

/**
 * Extract line positions of all TOML section headers from raw text.
 * Returns Map<sectionPath, lineNumber> (1-based).
 */
function findSectionPositions(text: string): Map<string, number> {
  const positions = new Map<string, number>();
  const lines = text.split("\n");

  for (let i = 0; i < lines.length; i++) {
    const trimmed = lines[i]!.trim();

    // Standard table: [section.path]
    const tableMatch = trimmed.match(/^\[([^\[\]]+)\]\s*$/);
    if (tableMatch) {
      positions.set(tableMatch[1]!, i + 1);
      continue;
    }

    // Array of tables: [[section.path]]
    const arrayMatch = trimmed.match(/^\[\[([^\[\]]+)\]\]\s*$/);
    if (arrayMatch) {
      // Only record first occurrence
      if (!positions.has(arrayMatch[1]!)) {
        positions.set(arrayMatch[1]!, i + 1);
      }
    }
  }

  return positions;
}

/**
 * Validate content body organization: density, label coherence, zone ordering.
 *
 * This check requires raw text (for line positions) in addition to parsed data.
 * It validates the ORGANIZATION of content, not just its existence or values.
 */
function checkContentBody(
  file: string,
  text: string,
  data: TomlData,
  r: DerivedRules,
): LintResult[] {
  const results: LintResult[] = [];
  const content = getTable(data, "_content");

  // No content block → other checks handle this
  if (!content) return results;

  // Build section position map once for all line-aware results
  const positions = findSectionPositions(text);

  // ── 1. Content density — actual data tables should exist ──────────
  //
  // A content block with only [_content] header and Cc/Co/Cv labels
  // but no data tables is structurally valid but practically hollow.

  const allKeys = Object.keys(data);
  const contentKeys = allKeys.filter((k) => {
    // Not structural
    if (k.startsWith("_metadata") || k === "_content" ||
        k.startsWith("_closing") || k.startsWith("_extensions")) return false;
    // _validation IS content (Cv zone)
    if (k === "_validation") return true;
    // Everything else that's top-level is content
    return true;
  });

  if (contentKeys.length === 0) {
    results.push(
      warn(
        file,
        "content/body/density",
        "Content block has no data tables — only [_content] header exists",
        { line: positions.get("_content") },
      ),
    );
  }

  // ── 2. Zone label coherence — Cc/Co/Cv values ────────────────────
  //
  // Zone labels should be non-empty strings that describe the zone's content.
  // Empty or wrong-type labels indicate unfilled template placeholders.

  const contentHeaderLine = positions.get("_content");
  for (const label of ["Cc", "Co", "Cv"] as const) {
    if (has(content, label)) {
      const val = content[label];
      if (typeof val !== "string") {
        results.push(
          warn(
            file,
            `content/body/label/${label}/type`,
            `_content.${label} should be a string, got ${typeof val}`,
            { line: contentHeaderLine },
          ),
        );
      } else if (val.trim() === "") {
        results.push(
          warn(
            file,
            `content/body/label/${label}/empty`,
            `_content.${label} is empty — should describe the ${label} zone content`,
            { line: contentHeaderLine },
          ),
        );
      }
    }
  }

  // ── 3. Zone ordering — Cc before Co before Cv in file ─────────────
  //
  // The schema convention: "Cc data comes first (identity, settings, resources),
  // Co data next (behavior, integrations), Cv data last ([_validation])."
  // This is not arbitrary — it mirrors the read flow:
  //   What IS this? (Cc) → What does it DO? (Co) → Is it VALID? (Cv)

  // Classify each section and record zone boundaries
  let lastCcLine = 0;
  let firstCoLine = Infinity;
  let lastCoLine = 0;
  let firstCvLine = Infinity;
  let firstCcLine = Infinity;

  const unclassified: Array<{ name: string; line: number }> = [];

  for (const [sectionPath, lineNum] of positions) {
    const zone = classifyZone(sectionPath, r);

    if (zone === "Cc") {
      lastCcLine = Math.max(lastCcLine, lineNum);
      firstCcLine = Math.min(firstCcLine, lineNum);
    } else if (zone === "Co") {
      firstCoLine = Math.min(firstCoLine, lineNum);
      lastCoLine = Math.max(lastCoLine, lineNum);
    } else if (zone === "Cv") {
      firstCvLine = Math.min(firstCvLine, lineNum);
    } else if (zone === "unknown") {
      unclassified.push({ name: sectionPath, line: lineNum });
    }
  }

  // Check: Cc should finish before Co starts
  if (lastCcLine > 0 && firstCoLine < Infinity && lastCcLine > firstCoLine) {
    results.push(
      warn(
        file,
        "content/body/zone-order/cc-co",
        `Cc zone extends past Co zone start — last Cc section at line ${lastCcLine}, first Co at line ${firstCoLine}. Convention: Cc sections before Co.`,
        { line: lastCcLine },
      ),
    );
  }

  // Check: Co should finish before Cv starts
  if (lastCoLine > 0 && firstCvLine < Infinity && lastCoLine > firstCvLine) {
    results.push(
      warn(
        file,
        "content/body/zone-order/co-cv",
        `Co zone extends past Cv zone start — last Co section at line ${lastCoLine}, first Cv at line ${firstCvLine}. Convention: Co sections before Cv.`,
        { line: lastCoLine },
      ),
    );
  }

  // Check: Cc should finish before Cv starts (handles case with no Co sections)
  if (lastCcLine > 0 && firstCvLine < Infinity && lastCcLine > firstCvLine) {
    results.push(
      warn(
        file,
        "content/body/zone-order/cc-cv",
        `Cc zone extends past Cv zone start — last Cc at line ${lastCcLine}, first Cv at line ${firstCvLine}. Convention: Cc before Cv.`,
        { line: lastCcLine },
      ),
    );
  }

  // ── 4. Unclassified content sections ──────────────────────────────
  //
  // Sections that exist in the content area but don't match any schema zone.
  // Not an error — user-defined sections are valid — but worth noting.

  // Filter unclassified to only those in the content area (between content header and closing)
  const contentHeaderPos = contentHeaderLine ?? 0;
  const closingLine = positions.get("_closing") ?? Infinity;

  for (const u of unclassified) {
    if (u.line > contentHeaderPos && u.line < closingLine) {
      results.push(
        info(
          file,
          `content/body/unclassified/${u.name}`,
          `[${u.name}] is not in a known zone (Cc/Co/Cv) — user-defined content section`,
          { line: u.line },
        ),
      );
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Transform — auto-correction engine (schema-driven)
// ---------------------------------------------------------------------------

/** Escape a string for use inside TOML double quotes. */
function escapeTomlValue(s: string): string {
  return s.replace(/\\/g, "\\\\").replace(/"/g, '\\"');
}

/**
 * Generate Cc/Co/Cv label values from file metadata.
 *
 * - Cc derives from I3_instance.brief (what the file configures)
 * - Co derives from I3_instance.provides (what capabilities it gives)
 * - Cv is standard validation description
 */
function generateCcCoCv(
  data: TomlData,
): { cc: string; co: string; cv: string } {
  const meta = getTable(data, "_metadata");
  const content = getTable(data, "_content");

  // Cc — from I3_instance.brief, falling back to constraint
  const i3 = meta ? getTable(meta, "I3_instance") : undefined;
  const brief = (i3?.["brief"] as string) ?? "";
  const constraint = (content?.["constraint"] as string) ?? "";
  const ccSource = brief || constraint || "configuration data";
  const cc = `Content Configurations — ${escapeTomlValue(ccSource)}`;

  // Co — from I3_instance.provides, falling back to C5_intent.purpose
  const provides = i3?.["provides"];
  const c5 = meta ? getTable(meta, "C5_intent") : undefined;
  const purpose = (c5?.["purpose"] as string) ?? "";

  let co: string;
  if (Array.isArray(provides) && provides.length > 0) {
    const items = provides.map((p: string) =>
      p.toLowerCase().replace(/_/g, " ")
    );
    const display = items.length <= 4
      ? items.join(", ")
      : items.slice(0, 3).join(", ") + `, +${items.length - 3} more`;
    co = `Content Operations — ${escapeTomlValue(display)}`;
  } else if (purpose) {
    co = `Content Operations — ${escapeTomlValue(purpose)}`;
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
    const trimmed = lines[i]!.trim();

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

// ---------------------------------------------------------------------------
// Transform — line resolution (locate where to insert corrections)
// ---------------------------------------------------------------------------

/** Find line index of a TOML section header like [_metadata.I1_core]. */
function findSectionLine(lines: string[], path: string): number {
  const target = `[${path}]`;
  for (let i = 0; i < lines.length; i++) {
    if (lines[i]!.trim() === target) return i;
  }
  return -1;
}

/**
 * From a section header line, find the last non-blank content line
 * before the next section header or block boundary.
 */
function findSectionContentEnd(lines: string[], startIdx: number): number {
  let last = startIdx;
  for (let i = startIdx + 1; i < lines.length; i++) {
    const t = lines[i]!.trim();
    if (t.startsWith("[")) break;
    if (t.startsWith("# ═")) break;
    if (t !== "" && !t.startsWith("#")) last = i;
  }
  return last;
}

/**
 * Find the end of the last [parent.*] child section under a parent path.
 * For "after _metadata", returns the end of the last [_metadata.*] section.
 */
function findLastChildEnd(lines: string[], parentPath: string): number {
  let lastEnd = -1;
  const parentHeader = `[${parentPath}]`;
  const childPrefix = `[${parentPath}.`;
  for (let i = 0; i < lines.length; i++) {
    const t = lines[i]!.trim();
    if (t === parentHeader || t.startsWith(childPrefix)) {
      const end = findSectionContentEnd(lines, i);
      if (end > lastEnd) lastEnd = end;
    }
  }
  return lastEnd;
}

/** Find a block boundary comment line containing the given marker. */
function findBlockBoundary(lines: string[], marker: string): number {
  for (let i = 0; i < lines.length; i++) {
    if (lines[i]!.includes(marker)) return i;
  }
  return -1;
}

// ---------------------------------------------------------------------------
// Transform — fix resolution
// ---------------------------------------------------------------------------

/** A fix with its resolved insertion point in the file. */
interface ResolvedFix {
  /** Line index to insert AFTER. */
  insertAfter: number;
  /** TOML lines to insert. */
  content: string[];
  /** Human description of what this fix does. */
  description: string;
  /** Rule that generated the fix. */
  rule: string;
}

/**
 * Resolve a fix suggestion's location hint to a concrete line index.
 *
 * Location formats:
 *   "after pragma line"      → insert after line 0
 *   "after metadata block"   → find END METADATA boundary
 *   "after content block"    → find END CONTENT boundary
 *   "in _metadata.I1_core"   → insert field at end of that section
 *   "after _metadata"        → insert new section after last _metadata.* child
 *   "after _closing"         → insert new section after last _closing.* child
 *
 * Returns the line index to insert AFTER, or -1 if unresolvable.
 */
function resolveFixLocation(lines: string[], location: string): number {
  if (!location) return -1;

  // Pragma — insert after the first line
  if (location === "after pragma line") {
    return Math.min(0, lines.length - 1);
  }

  // Block boundaries
  if (location === "after metadata block") {
    return findBlockBoundary(lines, "END METADATA");
  }
  if (location === "after content block") {
    return findBlockBoundary(lines, "END CONTENT");
  }

  // "in <section>" — field insertion inside an existing section
  if (location.startsWith("in ")) {
    const sectionPath = location.slice(3);
    const headerIdx = findSectionLine(lines, sectionPath);
    if (headerIdx >= 0) {
      return findSectionContentEnd(lines, headerIdx);
    }
    return -1;
  }

  // "after <parent>" — section insertion after parent's children
  if (location.startsWith("after ")) {
    const target = location.slice(6);
    // Try child sections first (e.g., last [_metadata.*] section)
    const childEnd = findLastChildEnd(lines, target);
    if (childEnd >= 0) return childEnd;
    // Fall back to the parent section header itself
    const headerIdx = findSectionLine(lines, target);
    if (headerIdx >= 0) return findSectionContentEnd(lines, headerIdx);
    return -1;
  }

  return -1;
}

// ---------------------------------------------------------------------------
// Transform — orchestrator
// ---------------------------------------------------------------------------

/**
 * Auto-correct a TOML file using fix suggestions from the linter.
 *
 * Pipeline: lint → collect fixes → resolve insertion points → apply bottom-up.
 *
 * Also runs content-aware Cc/Co/Cv label generation as a special case
 * (generates meaningful values from file metadata, not empty placeholders).
 */
async function transformTomlFile(
  filePath: string,
  opts: TransformOptions,
): Promise<LintResult[]> {
  const { dryRun, extensions } = opts;
  const results: LintResult[] = [];

  // Load schema (schema-loader manages its own lazy cache)
  let rules: DerivedRules;
  try {
    rules = await loadRules();
  } catch (e) {
    return [
      error(filePath, "schema/load", `Failed to load validation schema: ${e}`),
    ];
  }

  // Read
  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch (e) {
    return [error(filePath, "io/read", `Cannot read file: ${e}`)];
  }

  // Strip UTF-8 BOM if present
  if (text.charCodeAt(0) === 0xFEFF) text = text.slice(1);

  // Parse
  let data: TomlData;
  try {
    data = parseToml(text) as TomlData;
  } catch (e) {
    return [error(filePath, "parse/toml", `TOML parse error: ${e}`)];
  }

  // Extract pragma info from first line
  const firstLine = text.split("\n")[0] ?? "";
  const pragma = parsePragmaLine(firstLine);
  const isCargo = pragma?.args.includes("cargo") ?? false;

  // Normalize Cargo layout for linting
  const lintData = isCargo ? normalizeCargoData(data) : data;

  // Build line map once — shared across all check functions for traceback
  const lineMap = buildLineMap(text);

  // ── Phase 1: Lint to discover fixable issues ──────────────────────

  const lintResults = isCargo
    ? [
        ...checkMetadata(filePath, lintData, rules, lineMap),
        ...checkCargoContent(filePath, data, lineMap),
        ...checkClosing(filePath, lintData, rules, lineMap),
        ...checkConsistency(filePath, lintData, rules, pragma, lineMap),
        ...checkFieldValues(filePath, lintData, rules, pragma, lineMap),
      ]
    : [
        ...checkMetadata(filePath, lintData, rules, lineMap),
        ...checkContent(filePath, lintData, rules, lineMap),
        ...checkClosing(filePath, lintData, rules, lineMap),
        ...checkConsistency(filePath, lintData, rules, pragma, lineMap),
        ...checkFieldValues(filePath, lintData, rules, pragma, lineMap),
        ...checkContentBody(filePath, text, lintData, rules),
      ];

  // Filter: determine which fixes to apply.
  //
  // - Always skip Cc/Co/Cv field fixes — Phase 3 handles these with
  //   content-aware values generated from metadata (not empty placeholders).
  // - Error-level fixes always apply (required structure).
  // - Warn-level fixes (extension sections) only apply with --extensions.
  // - Info-level fixes (defined sections) only apply with --extensions.
  const fixable = lintResults.filter((r) => {
    if (!r.fix) return false;
    if (
      r.rule === "content/Cc" ||
      r.rule === "content/Co" ||
      r.rule === "content/Cv"
    ) {
      return false;
    }
    // Without --extensions, only fix errors (required structure)
    if (!extensions && r.severity !== "error") return false;
    return true;
  });

  // ── Phase 2: Resolve insertion points ─────────────────────────────
  //
  // Section additions get individual entries.
  // Field additions are grouped by target section so multiple fields
  // for the same section insert as a unit (avoids interleaving).

  const lines = text.split("\n");
  const resolved: ResolvedFix[] = [];

  // Separate section additions from field additions
  const sectionAdds = fixable.filter((r) => r.fix!.toml.startsWith("["));
  const fieldAdds = fixable.filter((r) => !r.fix!.toml.startsWith("["));

  // Resolve section additions individually
  for (const r of sectionAdds) {
    const loc = r.fix!.location ?? "";
    const insertAfter = resolveFixLocation(lines, loc);
    if (insertAfter >= 0) {
      resolved.push({
        insertAfter,
        content: r.fix!.toml.split("\n"),
        description: r.fix!.description,
        rule: r.rule,
      });
    } else {
      results.push(
        warn(
          filePath,
          "transform/unresolved",
          `Cannot resolve location for: ${r.fix!.description}`,
        ),
      );
    }
  }

  // Group field additions by target section
  const fieldGroups = new Map<
    string,
    { insertAfter: number; fields: string[]; descriptions: string[] }
  >();

  for (const r of fieldAdds) {
    const loc = r.fix!.location ?? "";
    const insertAfter = resolveFixLocation(lines, loc);
    if (insertAfter < 0) {
      results.push(
        warn(
          filePath,
          "transform/unresolved",
          `Cannot resolve location for: ${r.fix!.description}`,
        ),
      );
      continue;
    }

    if (!fieldGroups.has(loc)) {
      fieldGroups.set(loc, { insertAfter, fields: [], descriptions: [] });
    }
    fieldGroups.get(loc)!.fields.push(r.fix!.toml);
    fieldGroups.get(loc)!.descriptions.push(r.fix!.description);
  }

  // Convert grouped fields into resolved fixes
  for (const [, group] of fieldGroups) {
    resolved.push({
      insertAfter: group.insertAfter,
      content: group.fields,
      description: group.descriptions.join(", "),
      rule: "transform/fields",
    });
  }

  // ── Phase 3: Content-aware Cc/Co/Cv labels ────────────────────────
  //
  // Special case: Cc/Co/Cv labels are generated from file metadata,
  // producing meaningful values instead of empty placeholders.

  const contentTable = getTable(data, "_content");
  if (contentTable) {
    const needsCc = !has(contentTable, "Cc");
    const needsCo = !has(contentTable, "Co");
    const needsCv = !has(contentTable, "Cv");

    if (needsCc || needsCo || needsCv) {
      const labels = generateCcCoCv(data);
      const insertIdx = findContentInsertionPoint(lines);

      if (insertIdx >= 0) {
        const labelLines: string[] = [];
        if (needsCc) labelLines.push(`Cc = "${labels.cc}"`);
        if (needsCo) labelLines.push(`Co = "${labels.co}"`);
        if (needsCv) labelLines.push(`Cv = "${labels.cv}"`);

        resolved.push({
          insertAfter: insertIdx,
          content: labelLines,
          description: "Generate Cc/Co/Cv labels from file metadata",
          rule: "transform/labels",
        });
      }
    }
  }

  // ── Phase 4: Nothing to do? ───────────────────────────────────────

  if (resolved.length === 0) {
    results.push(
      info(
        filePath,
        "transform/clean",
        "No corrections needed — file is structurally complete",
      ),
    );
    return results;
  }

  // ── Phase 5: Sort bottom-up and apply ─────────────────────────────
  //
  // Sort by insertAfter descending so insertions at higher line numbers
  // are applied first (preserving indices for earlier insertions).
  // For same-point insertions, reverse original order so forward-ordered
  // fixes stack correctly in the file.

  const indexed = resolved.map((rf, i) => ({ rf, idx: i }));
  indexed.sort((a, b) => {
    if (b.rf.insertAfter !== a.rf.insertAfter) {
      return b.rf.insertAfter - a.rf.insertAfter;
    }
    return b.idx - a.idx;
  });

  for (const { rf } of indexed) {
    if (dryRun) {
      results.push(info(filePath, "transform/would-add", rf.description));
      for (const line of rf.content) {
        results.push(info(filePath, "transform/preview", `  ${line}`));
      }
    } else {
      // Smart separator: section headers always get a preceding blank line
      // (TOML convention — visual grouping). Field-only insertions only get
      // a separator if the next line isn't already blank.
      const isSectionHeader = rf.content[0]?.startsWith("[") ?? false;
      const nextLine = lines[rf.insertAfter + 1]?.trim() ?? "";
      const sep = (isSectionHeader || nextLine !== "") ? [""] : [];
      lines.splice(rf.insertAfter + 1, 0, ...sep, ...rf.content);
      results.push(info(filePath, "transform/applied", rf.description));
    }
  }

  // ── Phase 6: Write and report ─────────────────────────────────────

  if (!dryRun) {
    await Deno.writeTextFile(filePath, lines.join("\n"));
    results.push(
      info(
        filePath,
        "transform/done",
        `Applied ${resolved.length} correction(s)`,
      ),
    );
  } else {
    results.push(
      info(
        filePath,
        "transform/summary",
        `${resolved.length} correction(s) available — run without --dry-run to apply`,
      ),
    );
  }

  return results;
}

// ---------------------------------------------------------------------------
// Lint orchestrator
// ---------------------------------------------------------------------------

async function lintTomlFile(filePath: string): Promise<LintResult[]> {
  // Load schema (schema-loader manages its own lazy cache)
  let rules: DerivedRules;
  try {
    rules = await loadRules();
  } catch (e) {
    return [
      error(
        filePath,
        "schema/load",
        `Failed to load validation schema: ${e}`,
      ),
    ];
  }

  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch (e) {
    return [error(filePath, "io/read", `Cannot read file: ${e}`)];
  }

  // Fix 2: Strip UTF-8 BOM if present
  if (text.charCodeAt(0) === 0xFEFF) text = text.slice(1);

  let data: TomlData;
  try {
    data = parseToml(text) as TomlData;
  } catch (e) {
    return [error(filePath, "parse/toml", `TOML parse error: ${e}`)];
  }

  // Extract pragma info from first line
  const firstLine = text.split("\n")[0] ?? "";
  const pragma = parsePragmaLine(firstLine);
  const isCargo = pragma?.args.includes("cargo") ?? false;

  // Cargo.toml uses package.metadata.omni instead of _metadata/_closing.
  // Normalize to standard table names so all check functions work unchanged.
  const lintData = isCargo ? normalizeCargoData(data) : data;

  // Build line map once — shared across all check functions for traceback
  const lineMap = buildLineMap(text);

  if (isCargo) {
    // Cargo layout: metadata + closing via normalization, content via Cargo sections
    return [
      ...checkMetadata(filePath, lintData, rules, lineMap),
      ...checkCargoContent(filePath, data, lineMap),
      ...checkClosing(filePath, lintData, rules, lineMap),
      ...checkConsistency(filePath, lintData, rules, pragma, lineMap),
      ...checkFieldValues(filePath, lintData, rules, pragma, lineMap),
    ];
  }

  return [
    ...checkMetadata(filePath, lintData, rules, lineMap),
    ...checkContent(filePath, lintData, rules, lineMap),
    ...checkClosing(filePath, lintData, rules, lineMap),
    ...checkConsistency(filePath, lintData, rules, pragma, lineMap),
    ...checkFieldValues(filePath, lintData, rules, pragma, lineMap),
    ...checkContentBody(filePath, text, lintData, rules),
  ];
}

// ---------------------------------------------------------------------------
// Health Scoring — true scores from atomic actions
// ---------------------------------------------------------------------------
//
// Every check the linter performs is an atomic action. This function derives
// the full action list from the schema (what SHOULD be checked) + results
// (what FAILED). No hardcoded weights — the algorithm computes from reality.
//
// "Diverse weights, and diverse measures, both of them are alike
//  abomination to the LORD." — Proverbs 20:10

/**
 * Build a set of rule strings present in results for fast lookup.
 * Results with matching rules indicate failed checks.
 */
function buildFailureIndex(results: LintResult[]): Map<string, LintResult> {
  const index = new Map<string, LintResult>();
  for (const r of results) {
    index.set(r.rule, r);
  }
  return index;
}

/**
 * Create an atomic action from a check.
 * If a matching failure exists in the index, it's a FAIL action.
 * Otherwise, it's a PASS action.
 */
function action(
  check: string,
  container: string,
  block: string,
  failures: Map<string, LintResult>,
  ruleKey: string,
): AtomicAction {
  const failure = failures.get(ruleKey);
  if (failure) {
    return {
      check, container, block, passed: false,
      severity: failure.severity,
      reason: failure.message,
    };
  }
  return { check, container, block, passed: true };
}

/**
 * Compute TOML file health from schema + results.
 *
 * Walks the schema to define what checks exist (total possible actions),
 * then cross-references with results to determine pass/fail for each.
 * Scores computed with asymmetric weighting — no hardcoded weights.
 */
async function computeTomlHealth(
  filePath: string,
  results: LintResult[],
): Promise<HealthScore> {
  // Load schema rules (cached after first call)
  let rules: DerivedRules;
  try {
    rules = await loadRules();
  } catch {
    // If schema fails to load, return empty health
    return computeHealthScore([]);
  }

  const failures = buildFailureIndex(results);

  // ── METADATA block ─────────────────────────────────────────────────
  const metaActions: Map<string, AtomicAction[]> = new Map();

  // Structural: does _metadata table exist?
  const metaStructural: AtomicAction[] = [
    action("_metadata exists", "structural", "metadata", failures, "metadata/exists"),
  ];
  metaActions.set("structural", metaStructural);

  // Required identity sections (I1, I2, I3)
  for (const section of rules.requiredIdentity) {
    const acts: AtomicAction[] = [];
    // Section existence check
    acts.push(action(`${section} exists`, section, "metadata", failures, `metadata/${section}`));

    // Field checks from schema
    const fieldReqs = rules.metadataFields[section];
    if (fieldReqs) {
      for (const f of fieldReqs.required) {
        acts.push(action(
          `${section}.${f}`, section, "metadata", failures, `_metadata.${section}/${f}`,
        ));
      }
      for (const f of fieldReqs.defined) {
        acts.push(action(
          `${section}.${f}`, section, "metadata", failures, `_metadata.${section}/${f}`,
        ));
      }
    }
    metaActions.set(section, acts);
  }

  // Extension identity sections (I4)
  for (const section of rules.extensionIdentity) {
    const acts: AtomicAction[] = [
      action(`${section} exists`, section, "metadata", failures, `metadata/${section}`),
    ];
    const fieldReqs = rules.metadataFields[section];
    if (fieldReqs) {
      for (const f of fieldReqs.required) {
        acts.push(action(
          `${section}.${f}`, section, "metadata", failures, `_metadata.${section}/${f}`,
        ));
      }
      for (const f of fieldReqs.defined) {
        acts.push(action(
          `${section}.${f}`, section, "metadata", failures, `_metadata.${section}/${f}`,
        ));
      }
    }
    metaActions.set(section, acts);
  }

  // Required context sections (C1-C4)
  for (const section of rules.requiredContext) {
    const acts: AtomicAction[] = [];
    acts.push(action(`${section} exists`, section, "metadata", failures, `metadata/${section}`));
    const fieldReqs = rules.metadataFields[section];
    if (fieldReqs) {
      for (const f of fieldReqs.required) {
        acts.push(action(
          `${section}.${f}`, section, "metadata", failures, `_metadata.${section}/${f}`,
        ));
      }
      for (const f of fieldReqs.defined) {
        acts.push(action(
          `${section}.${f}`, section, "metadata", failures, `_metadata.${section}/${f}`,
        ));
      }
    }
    metaActions.set(section, acts);
  }

  // Extension context sections (C5-C7)
  for (const section of rules.extensionContext) {
    const acts: AtomicAction[] = [
      action(`${section} exists`, section, "metadata", failures, `metadata/${section}`),
    ];
    const fieldReqs = rules.metadataFields[section];
    if (fieldReqs) {
      for (const f of fieldReqs.required) {
        acts.push(action(
          `${section}.${f}`, section, "metadata", failures, `_metadata.${section}/${f}`,
        ));
      }
      for (const f of fieldReqs.defined) {
        acts.push(action(
          `${section}.${f}`, section, "metadata", failures, `_metadata.${section}/${f}`,
        ));
      }
    }
    metaActions.set(section, acts);
  }

  // Cascade: if _metadata table is missing, ALL metadata checks fail
  const metaExists = !failures.has("metadata/exists");
  if (!metaExists) {
    for (const [, acts] of metaActions) {
      for (const a of acts) {
        if (a.passed && a.check !== "_metadata exists") {
          a.passed = false;
          a.severity = "error";
          a.reason = "Parent [_metadata] table missing — all metadata checks fail";
        }
      }
    }
  }

  // Build metadata containers
  const metaContainers = [...metaActions.entries()].map(([section, acts]) =>
    computeContainerScore(section, "metadata", acts)
  );

  // ── CONTENT block ──────────────────────────────────────────────────
  const contentActions: Map<string, AtomicAction[]> = new Map();

  // Structural: _content table exists
  const contentStructural: AtomicAction[] = [
    action("_content exists", "structural", "content", failures, "content/exists"),
  ];
  contentActions.set("structural", contentStructural);

  // Required content sections (identity, _validation, etc.)
  for (const section of rules.requiredContent) {
    const acts: AtomicAction[] = [
      action(`${section} exists`, section, "content", failures, `content/${section}`),
    ];
    const fieldReqs = rules.contentFields[section];
    if (fieldReqs) {
      for (const f of fieldReqs.required) {
        acts.push(action(
          `${section}.${f}`, section, "content", failures, `_content.${section}/${f}`,
        ));
      }
      for (const f of fieldReqs.defined) {
        acts.push(action(
          `${section}.${f}`, section, "content", failures, `_content.${section}/${f}`,
        ));
      }
    }
    contentActions.set(section, acts);
  }

  // Cc zone sections
  for (const section of rules.definedCc) {
    if (contentActions.has(section)) continue; // avoid duplication
    const acts: AtomicAction[] = [
      action(`Cc:${section} exists`, section, "content", failures, `content/cc/${section}`),
    ];
    contentActions.set(section, acts);
  }

  // Co zone sections
  for (const section of rules.definedCo) {
    if (contentActions.has(section)) continue;
    const acts: AtomicAction[] = [
      action(`Co:${section} exists`, section, "content", failures, `content/co/${section}`),
    ];
    contentActions.set(section, acts);
  }

  // Cv zone (validation)
  for (const section of rules.requiredValidation) {
    if (contentActions.has(section)) continue;
    const acts: AtomicAction[] = [
      action(`Cv:${section} exists`, section, "content", failures, `content/cv/${section}`),
    ];
    contentActions.set(section, acts);
  }

  // Content body zone ordering
  const bodyOrderActs: AtomicAction[] = [
    action("zone ordering", "body-order", "content", failures, "content-body/zone-order"),
  ];
  contentActions.set("body-order", bodyOrderActs);

  // Cascade: if _content table is missing, ALL content checks fail
  const contentExists = !failures.has("content/exists");
  if (!contentExists) {
    for (const [, acts] of contentActions) {
      for (const a of acts) {
        if (a.passed && a.check !== "_content exists") {
          a.passed = false;
          a.severity = "error";
          a.reason = "Parent [_content] table missing — all content checks fail";
        }
      }
    }
  }

  const contentContainers = [...contentActions.entries()].map(([section, acts]) =>
    computeContainerScore(section, "content", acts)
  );

  // ── CLOSING block ──────────────────────────────────────────────────
  const closingActions: Map<string, AtomicAction[]> = new Map();

  // Structural: _closing table exists
  const closingStructural: AtomicAction[] = [
    action("_closing exists", "structural", "closing", failures, "closing/exists"),
  ];
  closingActions.set("structural", closingStructural);

  // Required closing sections (X1, X5)
  for (const section of rules.requiredClosing) {
    const acts: AtomicAction[] = [];
    acts.push(action(`${section} exists`, section, "closing", failures, `closing/${section}`));
    const fieldReqs = rules.closingFields[section];
    if (fieldReqs) {
      for (const f of fieldReqs.required) {
        acts.push(action(
          `${section}.${f}`, section, "closing", failures, `_closing.${section}/${f}`,
        ));
      }
      for (const f of fieldReqs.defined) {
        acts.push(action(
          `${section}.${f}`, section, "closing", failures, `_closing.${section}/${f}`,
        ));
      }
    }
    closingActions.set(section, acts);
  }

  // Defined closing sections (X2, X3, X4)
  for (const section of rules.definedClosing) {
    const acts: AtomicAction[] = [
      action(`${section} exists`, section, "closing", failures, `closing/${section}`),
    ];
    const fieldReqs = rules.closingFields[section];
    if (fieldReqs) {
      for (const f of fieldReqs.required) {
        acts.push(action(
          `${section}.${f}`, section, "closing", failures, `_closing.${section}/${f}`,
        ));
      }
      for (const f of fieldReqs.defined) {
        acts.push(action(
          `${section}.${f}`, section, "closing", failures, `_closing.${section}/${f}`,
        ));
      }
    }
    closingActions.set(section, acts);
  }

  // X section ordering
  const orderActs: AtomicAction[] = [
    action("X section order", "ordering", "closing", failures, "closing/order"),
  ];
  closingActions.set("ordering", orderActs);

  // Cascade: if _closing table is missing, ALL closing checks fail
  const closingExists = !failures.has("closing/exists");
  if (!closingExists) {
    for (const [, acts] of closingActions) {
      for (const a of acts) {
        if (a.passed && a.check !== "_closing exists") {
          a.passed = false;
          a.severity = "error";
          a.reason = "Parent [_closing] table missing — all closing checks fail";
        }
      }
    }
  }

  const closingContainers = [...closingActions.entries()].map(([section, acts]) =>
    computeContainerScore(section, "closing", acts)
  );

  // ── STRUCTURAL checks (cross-block) ───────────────────────────────
  const structActions: AtomicAction[] = [];

  // Pragma checks
  structActions.push(action("pragma exists", "pragma", "structural", failures, "consistency/pragma"));
  structActions.push(action("pragma-metadata key match", "pragma", "structural", failures, "consistency/pragma-metadata/key"));
  structActions.push(action("pragma-metadata version match", "pragma", "structural", failures, "consistency/pragma-metadata/version"));

  // Value checks (key format, version format, etc.)
  structActions.push(action("key format (B- prefix)", "values", "structural", failures, "value/key-format"));
  structActions.push(action("version format", "values", "structural", failures, "value/version-format"));

  const structContainers = [
    computeContainerScore("pragma", "structural", structActions.filter(a => a.container === "pragma")),
    computeContainerScore("values", "structural", structActions.filter(a => a.container === "values")),
  ];

  // ── Aggregate ──────────────────────────────────────────────────────
  const blocks = [
    computeBlockScore("metadata", metaContainers),
    computeBlockScore("content", contentContainers),
    computeBlockScore("closing", closingContainers),
    computeBlockScore("structural", structContainers),
  ];

  return computeHealthScore(blocks);
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// Registration — plug into the registry
// ---------------------------------------------------------------------------

const tomlHandler: FormatHandler = {
  name: "toml",
  description:
    "TOML 3-block alignment — schema-driven (metadata I1-I4/C1-C7, content Cc/Co/Cv, closing X1-X6)",
  extensions: [".toml"],
  maxDepth: 10,
  lint: lintTomlFile,
  computeHealth: computeTomlHealth,
  transform: transformTomlFile,
};

registerFormat(tomlHandler);

export default tomlHandler;
