// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/schema.ts
// key:     B-tov-cws-struct-lib-foundation-schema
// title:   CWS Struct — Schema Loader
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-16
// authors: Nova Dawn (CPI-SI)
// purpose: Load validation rules from toml-3block-schema.jsonc at runtime.
//          The schema is the single source of truth. The linter interprets it.
//
// biblical_foundation: "Thy word is a lamp unto my feet, and a light unto
//   my path." — Psalm 119:105
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { parse as parseJsonc } from "@std/jsonc";
import { getDefaultPipeline } from "./schema-pipeline.ts";
import { ToolError } from "./tool-error.ts";
import { registerCache } from "./cache-registry.ts";

// ---------------------------------------------------------------------------
// Types — mirrors validation_contract structure in the schema
// ---------------------------------------------------------------------------

/** Required/defined field lists for a single TOML section. */
export interface FieldRequirement {
  required: string[];
  defined: string[];
}

/** Metadata block contract — identity (I) + context (C) sections. */
export interface MetadataContract {
  required_identity: string[];
  extension_identity: string[];
  required_context: string[];
  extension_context: string[];
  key_pattern: string;
  field_requirements: Record<string, FieldRequirement>;
}

/** Content block contract — Cc/Co/Cv zones. */
export interface ContentContract {
  required_tables: string[];
  required_keys: Record<string, string[]>;
  required_sections: string[];
  defined_sections: string[];
  defined_cc: string[];
  defined_co: string[];
  required_validation: string[];
  extension_zones: string[];
  field_requirements: Record<string, FieldRequirement>;
}

/** Closing block contract — X sections. */
export interface ClosingContract {
  required_tables: string[];
  defined_tables: string[];
  template_tables: string[];
  key_pattern: string;
  field_requirements: Record<string, FieldRequirement>;
}

/** Full validation contract as defined in the schema. */
export interface ValidationContract {
  metadata: MetadataContract;
  content: ContentContract;
  closing: ClosingContract;
}

// ---------------------------------------------------------------------------
// Pragma taxonomy types — form option tagging
// ---------------------------------------------------------------------------

/** Normalization layout for a derivation — how it remaps standard 3-block tables. */
export interface DerivationNormalization {
  source: string;        // e.g., "package.metadata.omni"
  metadataFrom: string;  // e.g., "All keys except _closing"
  closingFrom: string;   // e.g., "package.metadata.omni._closing"
  contentSkip: boolean;  // true = skip standard _content check
}

/** Section requirement for a derivation. */
export interface DerivationSection {
  severity: string;
  purpose: string;
}

/** Layout definition for a single derivation (e.g., cargo, compiler). */
export interface DerivationLayout {
  purpose: string;
  normalization: DerivationNormalization | null;
  requiredSections: Record<string, DerivationSection>;
  definedSections: Record<string, DerivationSection>;
}

/** Schema-declared pragma taxonomy — types, formats, derivations, forms. */
export interface PragmaTaxonomy {
  validTypes: string[];
  baseType: string;
  format: string;
  knownDerivations: Set<string>;
  knownForms: Set<string>;
  allKnownArgs: Set<string>;
  derivationLayouts: Record<string, DerivationLayout>;
}

/** Derived rules — processed from the raw contract for direct use by the linter. */
export interface DerivedRules {
  /** Raw contract for reference. */
  contract: ValidationContract;

  // Compiled patterns
  metadataKeyPattern: RegExp;
  closingKeyPattern: RegExp;

  // Sets for membership testing
  allKnownSections: Set<string>;
  allKnownClosing: Set<string>;

  // Stripped arrays (no [_metadata.] / [_closing.] prefixes)
  requiredIdentity: string[];
  extensionIdentity: string[];
  requiredContext: string[];
  extensionContext: string[];
  requiredClosing: string[];
  definedClosing: string[];
  templateClosing: string[];
  requiredContent: string[];
  definedCc: string[];
  definedCo: string[];
  requiredValidation: string[];
  extensionZones: string[];

  // Field requirements keyed by section name (e.g., "I1_core", "X1_policy")
  metadataFields: Record<string, FieldRequirement>;
  contentFields: Record<string, FieldRequirement>;
  closingFields: Record<string, FieldRequirement>;

  // Pragma taxonomy — form option tagging (types, derivations, forms)
  pragmaTaxonomy: PragmaTaxonomy;
}

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

/** Schema ID for the TOML 3-block schema within the pipeline's schema tree. */
const TOML_SCHEMA_ID = "data/format/toml-3block-schema.jsonc";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/**
 * Strip TOML table path markers from schema arrays.
 * "[_metadata.I1_core]" → "I1_core"
 * "[_closing.X1_policy]" → "X1_policy"
 * "[identity]" → "identity"
 * "[_validation]" → "_validation"
 */
function stripTablePath(entry: string): string {
  // Remove surrounding brackets
  let s = entry;
  if (s.startsWith("[") && s.endsWith("]")) {
    s = s.slice(1, -1);
  }
  // Remove prefix up to last dot (e.g., "_metadata." or "_closing.")
  const lastDot = s.lastIndexOf(".");
  if (lastDot !== -1) {
    s = s.substring(lastDot + 1);
  }
  return s;
}

/**
 * Compile a regex string from the schema (e.g., "/^pattern$/") into a RegExp.
 * Falls back to a permissive pattern if parsing fails.
 */
function compilePattern(patternStr: string): RegExp {
  // Schema stores patterns as "/regex/" strings
  const match = patternStr.match(/^\/(.+)\/([gimsuy]*)$/);
  if (match) {
    try {
      return new RegExp(match[1]!, match[2] ?? "");
    } catch {
      // Fall through to default
    }
  }
  // Fallback — shouldn't happen with valid schema
  return /^(?:.*?)$/;
}

/**
 * Extract pragma taxonomy from the schema's pragma_taxonomy section.
 * Returns schema-declared types, derivations, forms, and derivation layouts.
 * Falls back to sensible defaults if the section is missing (backwards compat).
 */
function extractPragmaTaxonomy(
  // deno-lint-ignore no-explicit-any
  parsed: Record<string, any>,
): PragmaTaxonomy {
  // deno-lint-ignore no-explicit-any
  const pt = parsed["pragma_taxonomy"] as Record<string, any> | undefined;

  if (!pt) {
    // Backwards compatibility — return hardcoded defaults if schema lacks section
    return {
      validTypes: ["template", "data", "code"],
      baseType: "data",
      format: "toml",
      knownDerivations: new Set(["compiler", "cargo"]),
      knownForms: new Set(["library", "executable", "test"]),
      allKnownArgs: new Set(["compiler", "cargo", "library", "executable", "test"]),
      derivationLayouts: {},
    };
  }

  const validTypes = Array.isArray(pt["valid_types"])
    ? (pt["valid_types"] as string[])
    : ["template", "data", "code"];
  const baseType = (pt["base_type"] as string) ?? "data";
  const format = (pt["format"] as string) ?? "toml";

  // deno-lint-ignore no-explicit-any
  const argTax = pt["arg_taxonomy"] as Record<string, any> | undefined;
  // deno-lint-ignore no-explicit-any
  const derivationsObj = argTax?.["derivations"] as Record<string, any> | undefined;
  // deno-lint-ignore no-explicit-any
  const formsObj = argTax?.["forms"] as Record<string, any> | undefined;

  const knownDerivations = new Set(
    Array.isArray(derivationsObj?.["known"]) ? (derivationsObj["known"] as string[]) : [],
  );
  const knownForms = new Set(
    Array.isArray(formsObj?.["known"]) ? (formsObj["known"] as string[]) : [],
  );
  const allKnownArgs = new Set([...knownDerivations, ...knownForms]);

  // Extract derivation layouts
  // deno-lint-ignore no-explicit-any
  const layoutsRaw = pt["derivation_layouts"] as Record<string, any> | undefined;
  const derivationLayouts: Record<string, DerivationLayout> = {};

  if (layoutsRaw) {
    for (const [key, val] of Object.entries(layoutsRaw)) {
      if (key === "note" || typeof val !== "object" || val === null) continue;
      // deno-lint-ignore no-explicit-any
      const entry = val as Record<string, any>;
      // deno-lint-ignore no-explicit-any
      const normRaw = entry["normalization"] as Record<string, any> | null;

      derivationLayouts[key] = {
        purpose: (entry["purpose"] as string) ?? "",
        normalization: normRaw
          ? {
              source: (normRaw["source"] as string) ?? "",
              metadataFrom: (normRaw["metadata_from"] as string) ?? "",
              closingFrom: (normRaw["closing_from"] as string) ?? "",
              contentSkip: (normRaw["content_skip"] as boolean) ?? false,
            }
          : null,
        requiredSections: extractSections(entry["required_sections"]),
        definedSections: extractSections(entry["defined_sections"]),
      };
    }
  }

  return {
    validTypes,
    baseType,
    format,
    knownDerivations,
    knownForms,
    allKnownArgs,
    derivationLayouts,
  };
}

/**
 * Extract section requirements from a derivation layout.
 * Each entry is { severity, purpose }.
 */
function extractSections(
  // deno-lint-ignore no-explicit-any
  raw: Record<string, any> | undefined,
): Record<string, DerivationSection> {
  if (!raw || typeof raw !== "object") return {};
  const result: Record<string, DerivationSection> = {};
  for (const [key, val] of Object.entries(raw)) {
    if (typeof val === "object" && val !== null) {
      // deno-lint-ignore no-explicit-any
      const entry = val as Record<string, any>;
      result[key] = {
        severity: (entry["severity"] as string) ?? "info",
        purpose: (entry["purpose"] as string) ?? "",
      };
    }
  }
  return result;
}

/**
 * Normalize a field_requirements entry from the schema.
 * Ensures `defined` is always an array (schema may omit it).
 */
function normalizeFieldReq(
  raw: Record<string, unknown> | undefined,
): FieldRequirement {
  if (!raw) return { required: [], defined: [] };
  const required = Array.isArray(raw["required"])
    ? (raw["required"] as string[])
    : [];
  const defined = Array.isArray(raw["defined"])
    ? (raw["defined"] as string[])
    : [];
  return { required, defined };
}

// ---------------------------------------------------------------------------
// Schema resolution
// ---------------------------------------------------------------------------

// (Schema resolution moved to pipeline — getDefaultPipeline() handles discovery.)

// ---------------------------------------------------------------------------
// Loader — lazy, cached
// ---------------------------------------------------------------------------

/** Module-level cache. Loaded once, used for all subsequent lint calls. */
let cached: DerivedRules | null = null;

// ---------------------------------------------------------------------------
// 1. Pure parse — no I/O, no caching, no side effects
// ---------------------------------------------------------------------------

/**
 * Parse raw JSONC text of a TOML 3-block schema into linter-ready DerivedRules.
 *
 * This is the pure computation layer — takes text, returns rules. No disk
 * access, no caching, no side effects. The I/O wrapper (loadRules) reads
 * from disk and delegates here. The pipeline (Phase 2) will provide text
 * from any source — disk, embedded, override — and this function stays
 * unchanged.
 *
 * @param jsonText - Raw JSONC content of toml-3block-schema.jsonc
 * @returns Derived rules compiled for linter use
 */
export function parseTomlSchema(jsonText: string): DerivedRules {
  // Parse JSONC
  // deno-lint-ignore no-explicit-any
  let parsed: Record<string, any>;
  try {
    // deno-lint-ignore no-explicit-any
    parsed = parseJsonc(jsonText) as Record<string, any>;
  } catch (e) {
    throw new ToolError("CWS-T00-010", {
      error: e instanceof Error ? e.message : String(e),
    });
  }

  if (!parsed || typeof parsed !== "object") {
    throw new ToolError("CWS-T00-011");
  }

  const vc = parsed["validation_contract"] as Record<string, unknown>;

  if (!vc) {
    throw new ToolError("CWS-T00-012");
  }

  // deno-lint-ignore no-explicit-any
  const meta = vc["metadata"] as Record<string, any>;
  // deno-lint-ignore no-explicit-any
  const content = vc["content"] as Record<string, any>;
  // deno-lint-ignore no-explicit-any
  const closing = vc["closing"] as Record<string, any>;

  const missing: string[] = [];
  if (!meta) missing.push("metadata");
  if (!content) missing.push("content");
  if (!closing) missing.push("closing");

  if (missing.length > 0) {
    throw new ToolError("CWS-T00-013", { missing: missing.join(", ") });
  }

  // ── Build the raw contract ──────────────────────────────────────

  const contract: ValidationContract = {
    metadata: {
      required_identity: meta["required_identity"] ?? [],
      extension_identity: meta["extension_identity"] ?? [],
      required_context: meta["required_context"] ?? [],
      extension_context: meta["extension_context"] ?? [],
      key_pattern: meta["key_pattern"] ?? "",
      field_requirements: {},
    },
    content: {
      required_tables: content["required_tables"] ?? [],
      required_keys: content["required_keys"] ?? {},
      required_sections: content["required_sections"] ?? [],
      defined_sections: content["defined_sections"] ?? [],
      defined_cc: content["defined_cc"] ?? [],
      defined_co: content["defined_co"] ?? [],
      required_validation: content["required_validation"] ?? [],
      extension_zones: content["extension_zones"] ?? [],
      field_requirements: {},
    },
    closing: {
      required_tables: closing["required_tables"] ?? [],
      defined_tables: closing["defined_tables"] ?? [],
      template_tables: closing["template_tables"] ?? [],
      key_pattern: closing["key_pattern"] ?? "",
      field_requirements: {},
    },
  };

  // ── Process field_requirements ──────────────────────────────────

  const metaFieldReqs = (meta["field_requirements"] ?? {}) as Record<
    string,
    Record<string, unknown>
  >;
  for (const [key, val] of Object.entries(metaFieldReqs)) {
    contract.metadata.field_requirements[key] = normalizeFieldReq(val);
  }

  const contentFieldReqs = (content["field_requirements"] ?? {}) as Record<
    string,
    Record<string, unknown>
  >;
  for (const [key, val] of Object.entries(contentFieldReqs)) {
    contract.content.field_requirements[key] = normalizeFieldReq(val);
  }

  const closingFieldReqs = (closing["field_requirements"] ?? {}) as Record<
    string,
    Record<string, unknown>
  >;
  for (const [key, val] of Object.entries(closingFieldReqs)) {
    contract.closing.field_requirements[key] = normalizeFieldReq(val);
  }

  // ── Derive stripped arrays ─────────────────────────────────────

  const requiredIdentity = contract.metadata.required_identity.map(stripTablePath);
  const extensionIdentity = contract.metadata.extension_identity.map(stripTablePath);
  const requiredContext = contract.metadata.required_context.map(stripTablePath);
  const extensionContext = contract.metadata.extension_context.map(stripTablePath);
  const requiredClosing = contract.closing.required_tables.map(stripTablePath);
  const definedClosing = contract.closing.defined_tables.map(stripTablePath);
  const templateClosing = contract.closing.template_tables.map(stripTablePath);
  const requiredContent = contract.content.required_sections.map(stripTablePath);
  const definedCc = contract.content.defined_cc.map(stripTablePath);
  const definedCo = contract.content.defined_co.map(stripTablePath);
  const requiredValidation = contract.content.required_validation.map(stripTablePath);
  const extensionZones = [...contract.content.extension_zones];

  // ── Compile patterns ───────────────────────────────────────────

  const metadataKeyPattern = contract.metadata.key_pattern
    ? compilePattern(contract.metadata.key_pattern)
    : /^(?:I\d+_[a-z][a-z_]*|C\d+_[a-z][a-z_]*|_[a-z][a-z_]*)$/;

  const closingKeyPattern = contract.closing.key_pattern
    ? compilePattern(contract.closing.key_pattern)
    : /^(?:X\d+_[a-z][a-z_]*|_[a-z][a-z_]*)$/;

  // ── Build sets ─────────────────────────────────────────────────

  const allKnownSections = new Set([
    ...requiredIdentity,
    ...extensionIdentity,
    ...requiredContext,
    ...extensionContext,
  ]);

  const allKnownClosing = new Set([
    ...requiredClosing,
    ...definedClosing,
    ...templateClosing,
  ]);

  // ── Extract pragma taxonomy ──────────────────────────────────

  const pragmaTaxonomy = extractPragmaTaxonomy(parsed);

  // ── Assemble ───────────────────────────────────────────────────

  return {
    contract,
    metadataKeyPattern,
    closingKeyPattern,
    allKnownSections,
    allKnownClosing,
    requiredIdentity,
    extensionIdentity,
    requiredContext,
    extensionContext,
    requiredClosing,
    definedClosing,
    templateClosing,
    requiredContent,
    definedCc,
    definedCo,
    requiredValidation,
    extensionZones,
    metadataFields: contract.metadata.field_requirements,
    contentFields: contract.content.field_requirements,
    closingFields: contract.closing.field_requirements,
    pragmaTaxonomy,
  };
}

// ---------------------------------------------------------------------------
// 2. I/O loader — thin wrapper that reads file + delegates to parseTomlSchema
// ---------------------------------------------------------------------------

/**
 * Load and process the TOML 3-block schema.
 *
 * Resolves the schema through the pipeline (env override → project-local →
 * default), then delegates to parseTomlSchema() for parsing and compilation.
 * Result is cached — subsequent calls return cache.
 *
 * This is the I/O boundary. parseTomlSchema() is the pure computation.
 * The pipeline handles discovery, caching raw text, and error diagnostics.
 *
 * @returns Derived rules compiled for linter use
 */
export async function loadRules(): Promise<DerivedRules> {
  if (cached) return cached;

  const pipeline = getDefaultPipeline();
  const text = await pipeline.getText(TOML_SCHEMA_ID);

  cached = parseTomlSchema(text);
  return cached;
}

/**
 * Clear the cached rules. Useful for testing or after schema edits.
 */
export function clearCache(): void {
  cached = null;
}
registerCache("schema/derived-rules", clearCache);

// ============================================================================
// CLOSING
// ============================================================================
//
// The schema is the single source of truth. This loader reads it once,
// processes it into linter-ready form, and caches it for the session.
// When the schema changes, the linter automatically picks up new rules.
//
// "Thy word is a lamp unto my feet." — Psalm 119:105
// The schema lights the way. The linter walks the path.
// ============================================================================
