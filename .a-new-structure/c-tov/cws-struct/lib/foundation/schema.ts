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
import { fromFileUrl, dirname, join } from "@std/path";

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
}

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

/** Relative path from this file to the schema. */
const SCHEMA_REL_PATH = "../../../../b-word/L1-omnicode/ladder/schemas/data/format/toml-3block-schema.jsonc";

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

/**
 * Resolve the absolute path to the schema file.
 * Uses import.meta.url to find this file's location, then navigates
 * relative to it.
 */
function resolveSchemaPath(): string {
  const thisDir = dirname(fromFileUrl(import.meta.url));
  return join(thisDir, SCHEMA_REL_PATH);
}

// ---------------------------------------------------------------------------
// Loader — lazy, cached
// ---------------------------------------------------------------------------

/** Module-level cache. Loaded once, used for all subsequent lint calls. */
let cached: DerivedRules | null = null;

/**
 * Load and process the TOML 3-block schema.
 * First call reads from disk and caches. Subsequent calls return cache.
 *
 * Error handling:
 * - Schema file missing → clear error with path and resolution hint
 * - Schema file corrupt → clear error with parse details
 * - Schema missing required sections → clear error identifying what's absent
 * - All errors include the schema path for debugging
 */
export async function loadRules(): Promise<DerivedRules> {
  if (cached) return cached;

  const schemaPath = resolveSchemaPath();

  // Read schema file — provide actionable error if missing
  let text: string;
  try {
    text = await Deno.readTextFile(schemaPath);
  } catch (e) {
    if (e instanceof Deno.errors.NotFound) {
      throw new Error(
        `Schema file not found: ${schemaPath}\n` +
        `Expected at: b-word/L1-omnicode/ladder/schemas/data/format/toml-3block-schema.jsonc\n` +
        `This file is required for TOML linting. Verify the schema exists and the path is correct.`,
      );
    }
    if (e instanceof Deno.errors.PermissionDenied) {
      throw new Error(
        `Permission denied reading schema: ${schemaPath}\n` +
        `Run with --allow-read or check file permissions.`,
      );
    }
    throw new Error(`Cannot read schema at ${schemaPath}: ${e instanceof Error ? e.message : String(e)}`);
  }

  // Parse JSONC — provide clear error on corrupt schema
  // deno-lint-ignore no-explicit-any
  let parsed: Record<string, any>;
  try {
    // deno-lint-ignore no-explicit-any
    parsed = parseJsonc(text) as Record<string, any>;
  } catch (e) {
    throw new Error(
      `Schema parse error at ${schemaPath}: ${e instanceof Error ? e.message : String(e)}\n` +
      `The schema must be valid JSONC. Check for syntax errors.`,
    );
  }

  if (!parsed || typeof parsed !== "object") {
    throw new Error(`Schema at ${schemaPath} parsed to non-object — expected JSONC object`);
  }

  const vc = parsed["validation_contract"] as Record<string, unknown>;

  if (!vc) {
    throw new Error(
      `Schema at ${schemaPath} missing "validation_contract" — cannot derive rules`,
    );
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
    throw new Error(
      `Schema validation_contract missing: ${missing.join(", ")}\n` +
      `Schema path: ${schemaPath}`,
    );
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

  // ── Assemble and cache ─────────────────────────────────────────

  cached = {
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
  };

  return cached;
}

/**
 * Clear the cached rules. Useful for testing or after schema edits.
 */
export function clearCache(): void {
  cached = null;
}

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
