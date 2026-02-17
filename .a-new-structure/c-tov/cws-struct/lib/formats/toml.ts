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
//
// biblical_foundation: "Prove all things; hold fast that which is good."
//   — 1 Thessalonians 5:21
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { parse as parseToml } from "@std/toml";
import type { FormatHandler, LintResult, FixSuggestion, TransformOptions } from "../types.ts";
import { error, warn, info } from "../types.ts";
import { registerFormat } from "./registry.ts";
import { loadRules, type DerivedRules } from "../schema-loader.ts";

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

// deno-lint-ignore no-explicit-any
type TomlData = Record<string, any>;

// ---------------------------------------------------------------------------
// Schema cache — loaded once, used for all lint calls
// ---------------------------------------------------------------------------

let rules: DerivedRules | null = null;

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
): void {
  for (const f of required) {
    if (!has(table, f)) {
      results.push(
        error(file, `${path}/${f}`, `Missing ${path}.${f}`, fieldFix(path, f)),
      );
    }
  }
  for (const f of defined) {
    if (!has(table, f)) {
      results.push(
        info(
          file,
          `${path}/${f}`,
          `Missing ${path}.${f} — defined field`,
          fieldFix(path, f),
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

  // I — Identity sections (required)
  for (const key of r.requiredIdentity) {
    if (!has(meta, key)) {
      const after = findPrecedingMetaSection(meta, key, r);
      results.push(
        error(
          file,
          `metadata/${key}`,
          `Missing _metadata.${key} — REQUIRED`,
          sectionFix(`_metadata.${key}`, key, r.metadataFields, after),
        ),
      );
    }
  }

  // I — Identity sections (extension)
  for (const key of r.extensionIdentity) {
    if (!has(meta, key)) {
      const after = findPrecedingMetaSection(meta, key, r);
      results.push(
        warn(
          file,
          `metadata/${key}`,
          `Missing _metadata.${key} — extension identity`,
          sectionFix(`_metadata.${key}`, key, r.metadataFields, after),
        ),
      );
    }
  }

  // C — Context sections (required)
  for (const key of r.requiredContext) {
    if (!has(meta, key)) {
      const after = findPrecedingMetaSection(meta, key, r);
      results.push(
        error(
          file,
          `metadata/${key}`,
          `Missing _metadata.${key} — REQUIRED`,
          sectionFix(`_metadata.${key}`, key, r.metadataFields, after),
        ),
      );
    }
  }

  // C — Context sections (extension)
  for (const key of r.extensionContext) {
    if (!has(meta, key)) {
      const after = findPrecedingMetaSection(meta, key, r);
      results.push(
        warn(
          file,
          `metadata/${key}`,
          `Missing _metadata.${key} — extension context`,
          sectionFix(`_metadata.${key}`, key, r.metadataFields, after),
        ),
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
      );
    }
  }

  // C7 tags should be an array, not a string (value-level check, not in schema)
  const c7 = getTable(meta, "C7_classification");
  if (c7 && has(c7, "tags")) {
    if (typeof c7["tags"] === "string") {
      results.push(
        warn(
          file,
          "metadata/C7_classification/tags",
          "C7_classification.tags should be an array, not a comma-separated string",
        ),
      );
    }
  }

  // ── Structural lock: validate ALL keys under _metadata ──────────────
  for (const key of Object.keys(meta)) {
    // 1. Pattern validation
    if (!r.metadataKeyPattern.test(key)) {
      results.push(
        error(
          file,
          `metadata/invalid-key/${key}`,
          `Invalid key _metadata.${key} — must match I{N}_{name}, C{N}_{name}, or _{name}`,
        ),
      );
      continue;
    }

    // 2. Type validation — all metadata sections must be tables
    const val = meta[key];
    if (val !== null && val !== undefined && typeof val !== "object") {
      results.push(
        error(
          file,
          `metadata/${key}/type`,
          `_metadata.${key} must be a table, got ${typeof val}`,
        ),
      );
    }

    // 3. Unknown but valid-pattern extensions — info only
    if (!r.allKnownSections.has(key)) {
      results.push(
        info(
          file,
          `metadata/${key}/extension`,
          `_metadata.${key} is a valid extension (not in defined set)`,
        ),
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
  for (const label of ["Cc", "Co", "Cv"]) {
    if (!has(content, label)) {
      results.push(
        warn(
          file,
          `content/${label}`,
          `Missing _content.${label} zone label`,
          fieldFix("_content", label),
        ),
      );
    }
  }

  // ── Cc zone: required content sections ──────────────────────────
  for (const key of r.requiredContent) {
    if (!getTable(data, key)) {
      results.push(
        error(
          file,
          `content/cc/${key}`,
          `Missing [${key}] — REQUIRED content section`,
          sectionFix(key, key, r.contentFields),
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
        ),
      );
    }
  }

  // ── Cv zone: required validation ────────────────────────────────
  for (const key of r.requiredValidation) {
    if (!getTable(data, key)) {
      results.push(
        error(
          file,
          `content/cv/${key}`,
          `Missing [${key}] — REQUIRED validation section`,
          sectionFix(key, key, r.contentFields),
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
      );
    }
  }

  // ── Schema cross-validation: declared contract vs reality ──────

  const validation = getTable(data, "_validation");
  const schema = validation ? getTable(validation, "schema") : undefined;
  if (schema) {
    // Check schema.required sections actually exist
    const schemaReq = schema["required"];
    if (Array.isArray(schemaReq)) {
      for (const section of schemaReq) {
        if (typeof section === "string" && !getTable(data, section)) {
          results.push(
            error(
              file,
              `content/schema/${section}`,
              `Schema declares [${section}] as required, but section is missing`,
            ),
          );
        }
      }
    }

    // Check schema.defined sections — info if declared but missing
    const schemaDef = schema["defined"];
    if (Array.isArray(schemaDef)) {
      for (const section of schemaDef) {
        if (typeof section === "string" && !getTable(data, section)) {
          results.push(
            info(
              file,
              `content/schema/${section}`,
              `Schema declares [${section}] as defined, but section is missing`,
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
        const actualSection = getTable(data, sectionName);
        if (!fieldSpec || !actualSection) continue;

        const reqFields = fieldSpec["required"];
        if (Array.isArray(reqFields)) {
          for (const f of reqFields) {
            if (typeof f === "string" && !has(actualSection, f)) {
              results.push(
                error(
                  file,
                  `content/schema/fields/${sectionName}/${f}`,
                  `Schema declares ${sectionName}.${f} as required, but field is missing`,
                  fieldFix(sectionName, f),
                ),
              );
            }
          }
        }

        const defFields = fieldSpec["defined"];
        if (Array.isArray(defFields)) {
          for (const f of defFields) {
            if (typeof f === "string" && !has(actualSection, f)) {
              results.push(
                info(
                  file,
                  `content/schema/fields/${sectionName}/${f}`,
                  `Schema declares ${sectionName}.${f} as defined, but field is missing`,
                  fieldFix(sectionName, f),
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

  // X — Required closing sections
  for (const key of r.requiredClosing) {
    if (!has(closing, key)) {
      const after = findPrecedingCloseSection(closing, key, r);
      results.push(
        error(
          file,
          `closing/${key}`,
          `Missing _closing.${key} — REQUIRED`,
          sectionFix(`_closing.${key}`, key, r.closingFields, after),
        ),
      );
    }
  }

  // X — Defined closing sections (expected, not mandated)
  for (const key of r.definedClosing) {
    if (!has(closing, key)) {
      const after = findPrecedingCloseSection(closing, key, r);
      results.push(
        info(
          file,
          `closing/${key}`,
          `Missing _closing.${key} — defined closing section`,
          sectionFix(`_closing.${key}`, key, r.closingFields, after),
        ),
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
      );
    }
  }

  // ── Structural lock: validate ALL keys under _closing ──────────────
  for (const key of Object.keys(closing)) {
    // 1. Pattern validation
    if (!r.closingKeyPattern.test(key)) {
      results.push(
        error(
          file,
          `closing/invalid-key/${key}`,
          `Invalid key _closing.${key} — must match X{N}_{name} or _{name}`,
        ),
      );
      continue;
    }

    // 2. Type validation — all closing sections must be tables
    const val = closing[key];
    if (val !== null && val !== undefined && typeof val !== "object") {
      results.push(
        error(
          file,
          `closing/${key}/type`,
          `_closing.${key} must be a table, got ${typeof val}`,
        ),
      );
    }

    // 3. Unknown but valid-pattern extensions — info only
    if (!r.allKnownClosing.has(key)) {
      results.push(
        info(
          file,
          `closing/${key}/extension`,
          `_closing.${key} is a valid extension (not in defined set)`,
        ),
      );
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
): LintResult[] {
  const results: LintResult[] = [];
  const meta = getTable(data, "_metadata");

  if (!meta) return results;

  // I1_core.format should match pragma line pattern
  const i1 = getTable(meta, "I1_core");
  if (i1 && has(i1, "format")) {
    const format = i1["format"] as string;
    if (!format.startsWith("@omni")) {
      results.push(
        warn(
          file,
          "consistency/format",
          `I1_core.format="${format}" — expected @omni prefix`,
        ),
      );
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
        ),
      );
    }

    // valid_patterns should include X{N}_{name} for closing
    const patterns = contract["valid_patterns"];
    if (Array.isArray(patterns)) {
      if (!patterns.some((p: string) => p.includes("X"))) {
        results.push(
          warn(
            file,
            "consistency/contract/closing_patterns",
            "_contract.valid_patterns missing closing pattern (X{N}_{name})",
          ),
        );
      }
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

  // Load schema
  if (!rules) {
    try {
      rules = await loadRules();
    } catch (e) {
      return [
        error(filePath, "schema/load", `Failed to load validation schema: ${e}`),
      ];
    }
  }

  // Read
  let text: string;
  try {
    text = await Deno.readTextFile(filePath);
  } catch (e) {
    return [error(filePath, "io/read", `Cannot read file: ${e}`)];
  }

  // Parse
  let data: TomlData;
  try {
    data = parseToml(text) as TomlData;
  } catch (e) {
    return [error(filePath, "parse/toml", `TOML parse error: ${e}`)];
  }

  // ── Phase 1: Lint to discover fixable issues ──────────────────────

  const lintResults = [
    ...checkMetadata(filePath, data, rules),
    ...checkContent(filePath, data, rules),
    ...checkClosing(filePath, data, rules),
    ...checkConsistency(filePath, data, rules),
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
  // Lazy load schema on first call
  if (!rules) {
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
  }

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
    ...checkMetadata(filePath, data, rules),
    ...checkContent(filePath, data, rules),
    ...checkClosing(filePath, data, rules),
    ...checkConsistency(filePath, data, rules),
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
  description:
    "TOML 3-block alignment — schema-driven (metadata I1-I4/C1-C7, content Cc/Co/Cv, closing X1-X6)",
  extensions: [".toml"],
  maxDepth: 10,
  lint: lintTomlFile,
  transform: transformTomlFile,
};

registerFormat(tomlHandler);

export default tomlHandler;
