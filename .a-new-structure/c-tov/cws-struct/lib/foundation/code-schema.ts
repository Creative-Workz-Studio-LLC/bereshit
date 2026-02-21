// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/code-schema.ts
// key:     B-tov-cws-struct-lib-foundation-code-schema
// title:   CWS Struct — Code 4-Block Schema Loader
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-19
// authors: Nova Dawn (CPI-SI)
// purpose: Load Go/Rust 4-block schemas at runtime, derive handler-ready rules.
//          The schemas are the single source of truth for SETUP subsections,
//          BODY subsections, placement rules, and subtype emphasis.
//          Handlers read from here — when the schema grows, the linter grows.
//
// biblical_foundation: "The entrance of thy words giveth light; it giveth
//   understanding unto the simple." — Psalm 119:130
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Imports
// ---------------------------------------------------------------------------

import { parse as parseJsonc } from "@std/jsonc";
import { getDefaultPipeline } from "./schema-pipeline.ts";
import type { SubsectionDef } from "../handlers/shared/types.ts";

// ---------------------------------------------------------------------------
// 2. Types — raw schema data
// ---------------------------------------------------------------------------

/** Raw subsection definition extracted from the schema. */
export interface SchemaSubsectionDef {
  position: number;
  tag: string;
  /** Canonical display name (e.g., "Imports", "Core Types"). */
  pattern: string;
  /** Alternative names that match the same subsection. */
  altPatterns: string[];
  purpose: string;
  productionNote: string;
  requiredIn: string[];
}

/** Raw body subtype definition from the schema. */
export interface SchemaBodySubtype {
  canonicalCount: number;
  orderingPrinciple: string;
  note: string;
  subsections: SchemaSubsectionDef[];
}

/** I/C field requirement for a single section (e.g., I1, C3). */
export interface SchemaFieldRequirement {
  required: string[];
  defined: string[];
  purpose: string;
}

/** Doc comment expectation from the schema. */
export interface SchemaDocComment {
  purpose: string;
  severity: string;
  format?: string;
  contains?: string;
  location?: string;
}

/** Subtype definition from the schema. */
export interface SchemaSubtypeDef {
  subtypeValue: string;
  filePattern: string;
  pragmaDirective: string;
  templateDirective: string;
  hasPublicApi: boolean;
  hasTestsBlock?: boolean;
  packagePattern?: string;
  purpose: string;
}

// ---------------------------------------------------------------------------
// 2b. Types — CLOSING block schema data
// ---------------------------------------------------------------------------

/** Schema-derived closing zone definition (replaces hardcoded CLOSING_ZONES). */
export interface SchemaClosingZone {
  tag: string;
  kind: "code" | "doc";
  pattern: RegExp;
}

/** Schema-derived closing doc requirement (replaces hardcoded CLOSING_DOC_REQUIREMENTS). */
export interface SchemaClosingDocReq {
  tag: string;
  required: boolean;
  templateOnly?: boolean;
  fields?: {
    required: string[];
    defined: string[];
  };
}

/**
 * Complete CLOSING block data loaded from schema.
 *
 * Replaces the hardcoded constants in handlers/shared/types.ts:
 * - zones replaces CLOSING_ZONES + ClosingZoneDef
 * - docRequirements replaces CLOSING_DOC_REQUIREMENTS + ClosingDocRequirement
 * - fieldPatterns replaces X1_FIELD_PATTERNS + X5_FIELD_PATTERNS
 */
export interface SchemaClosingData {
  /** All zones (code + doc) in canonical order, with compiled regex patterns. */
  zones: SchemaClosingZone[];
  /** Documentation zone requirements — which zones required, what fields. */
  docRequirements: SchemaClosingDocReq[];
  /** Field detection patterns per doc zone: "X1" → { "never": /.../, ... }. */
  fieldPatterns: Record<string, Record<string, RegExp>>;
}

// ---------------------------------------------------------------------------
// 2c. Types — Fill content (schema-driven file generation)
// ---------------------------------------------------------------------------

/** Syntax template for identity variable declarations (Go [][2]string vs Rust &[(&str, &str)]). */
export interface SchemaIdentitySyntax {
  declaration: string;
  entry: string;
  close: string;
}

/** Default values for placeholder substitution. Core fields are typed; additional schema-defined defaults are captured by the index signature. */
export interface SchemaFillDefaults {
  version: string;
  status: string;
  organization: string;
  scripture: string;
  scripture_text: string;
  type: string;
  structure: string;
  /** Additional defaults (architect, role, layer, etc.) defined in the schema. */
  [key: string]: string;
}

/** Default field values for CLOSING documentation zones. */
export interface SchemaClosingDefaults {
  [tag: string]: Record<string, string>;
}

/** A single identity group (e.g., I1 → Core, C3 → Grounding). */
export interface IdentityGroup {
  /** Range prefix matching field keys (e.g., "I1", "C3"). */
  range: string;
  /** Display label for inline group comment (e.g., "Core", "Grounding"). */
  label: string;
  /** Optional docstring emitted before the static declaration. */
  docstring?: string;
}

/** Complete identity grouping — pragma groups, metadata groups, and section headers. */
export interface IdentityGrouping {
  /** Pragma groups (I1-I4) with range/label/docstring. */
  pragma: IdentityGroup[];
  /** Metadata groups (C1-C7) with range/label/docstring. */
  metadata: IdentityGroup[];
  /** Section header comments emitted before each static block. */
  sectionHeaders: { pragma: string; metadata: string };
}

/** A single transformer mode — defines which formatting features are enabled. */
export interface TransformerMode {
  /** Target linter result (e.g., "0E 0W 0I", "0E 0W", "0E"). */
  linterTarget: string;
  /** Emit identity statics with formatting (headers, groups, alignment). */
  identityFormatting: boolean;
  /** Emit section headers (e.g., "// Identity (I1-I4)"). */
  sectionHeaders: boolean;
  /** Emit inline group comments (e.g., "// I2: Family"). */
  groupComments: boolean;
  /** Emit docstrings before static declarations. */
  docstrings: boolean;
  /** Column-align field entries within statics. */
  columnAlignment: boolean;
  /** Emit block overviews (purpose + section order). */
  blockOverviews: boolean;
  /** Emit Reserved Omission with Available/Reserved groups. */
  roGrouped: boolean;
  /** Emit all CLOSING zones with complete field content. */
  closingComplete: boolean;
}

/** Named transformer modes from schema — strict/balance/growth leniency scale. */
export interface TransformerModes {
  strict: TransformerMode;
  balance: TransformerMode;
  growth: TransformerMode;
}

/**
 * Fill content loaded from schema — everything needed to GENERATE a file.
 *
 * Combined with existing schema data (subsections, zones, field requirements),
 * this enables schema-driven file creation: `cws-struct create go library`.
 */
export interface SchemaFillContent {
  /** Default values for placeholders. */
  defaults: SchemaFillDefaults;
  /** OmniCode directive templates (e.g., "//omni:key {{key}}"). */
  directives: string[];
  /** Package/module declaration template (Go: "package {{package}}", Rust: absent). */
  packageDeclaration?: string;
  /** Module documentation line (Rust: "//! {{title}}", Go: absent). */
  moduleDoc?: string;
  /** Identity variable syntax — how to declare Pragma/Metadata arrays. */
  identitySyntax: {
    pragma: SchemaIdentitySyntax;
    metadata: SchemaIdentitySyntax;
  };
  /** Ordered Pragma field entries: [field_key, template_value]. */
  pragmaEntries: [string, string][];
  /** Ordered Metadata field entries: [field_key, template_value]. */
  metadataEntries: [string, string][];
  /** METADATA block comment lines (e.g., "// Key: {{key}}"). */
  metadataComment: string[];
  /** Default field values for CLOSING doc zones (X1, X5). */
  closingDefaults: SchemaClosingDefaults;
  /** Identity group definitions for structured METADATA formatting. */
  identityGroups?: IdentityGrouping;
  /** Named transformer leniency modes (strict/balance/growth). */
  transformerModes?: TransformerModes;
}

/**
 * Content kind → block/subsection placement maps, loaded from schema.
 *
 * These replace hardcoded maps in each handler. The schema's
 * content_kind_mapping section is the single source of truth.
 */
export interface PlacementMaps {
  /** Content kind → block name ("SETUP" or "BODY"). Missing = any block. */
  blockPlacement: Record<string, string>;
  /** Content kind → subsection tag within SETUP. */
  subsectionPlacement: Record<string, string>;
  /** Content kinds that are never valid in the METADATA block. */
  metadataForbidden: string[];
}

// ---------------------------------------------------------------------------
// 2f. Types — Form constraint data (from form schemas)
// ---------------------------------------------------------------------------

/** A section that CAN be in a container for a specific form. */
export interface FormSectionConstraint {
  position: number;
  tag: string;
  status: "REQUIRED" | "AVAILABLE";
}

/** A section that CANNOT be in a container for a specific form. */
export interface FormReservedSection {
  tag: string;
  whyReserved: string;
  activeIn: string;
  /** Canonical position in the section ordering (e.g., 2 for S2_Modules).
   *  Extracted from the schema key prefix. Used by the transform to show
   *  reserved sections inline at their correct position. */
  position?: number;
}

/** Per-container CAN/CANNOT constraint set. */
export interface FormContainerConstraints {
  can: FormSectionConstraint[];
  cannot: FormReservedSection[];
}

/** Complete form constraints for a single form (e.g., module, library). */
export interface FormConstraints {
  /** Form name (e.g., "module", "library", "bare-bone"). */
  name: string;
  /** True if this is the bare-bone foundation form. */
  isFoundation: boolean;
  /** Which form this inherits from (usually "bare-bone"). */
  inherits?: string;
  /** BODY/CONTENT ordering principle (e.g., "type_lifecycle", "definition_flow"). */
  ordering: string;
  /** Per-container constraints for 4-block (code). */
  SETUP: FormContainerConstraints;
  BODY: FormContainerConstraints;
  CLOSING: FormContainerConstraints;
  /** Per-container constraint for 3-block (data). CONTENT = SETUP + BODY equivalent. */
  CONTENT?: FormContainerConstraints;
}

/** Derived rules for a 4-block code handler — compiled and ready for use. */
export interface Code4BlockRules {
  /** SETUP subsections with compiled regexes (ready for linter pattern matching). */
  setupSubsections: SubsectionDef[];

  /** BODY canonical subsection names per subtype (e.g., library → ["Org Chart", "Helpers", ...]). */
  bodySubsections: Record<string, readonly string[]>;

  /** Placement rules from schema — what content belongs where. */
  placementRules: {
    mustBeInSetup: string[];
    mustNotBeInSetup: string[];
  };

  /** Raw SETUP data for future content-aware checks. */
  setupData: SchemaSubsectionDef[];

  /** Raw BODY data per subtype for future content-aware checks. */
  bodyData: Record<string, SchemaBodySubtype>;

  /** Subtype emphasis — which subsections are heavy/light per subtype, per block. */
  subtypeEmphasis: {
    setup: Record<string, { heavy: string[]; light: string[] }>;
    body: Record<string, { heavy: string[]; light: string[] }>;
  };

  // ── Content placement maps (schema-driven, replaces hardcoded) ─────

  /** Content kind → block/subsection placement maps from schema. */
  placementMaps: PlacementMaps;

  // ── METADATA block rules (Phase 1: schema-driven I/C) ──────────────

  /** Pragma I/C field requirements per section (I1, I2, I3, I4). */
  pragmaFieldRequirements: Record<string, SchemaFieldRequirement>;

  /** Metadata I/C field requirements per section (C1, C2, ..., C7). */
  metadataFieldRequirements: Record<string, SchemaFieldRequirement>;

  /** Doc comment expectations — format-specific (Go: package_doc/pragma_doc, Rust: //! crate). */
  docCommentExpectations: Record<string, SchemaDocComment>;

  /** Subtype definitions — what I2.subtype values are valid and what they mean. */
  subtypeDefinitions: Record<string, SchemaSubtypeDef>;

  // ── CLOSING block rules (schema-driven, replaces hardcoded) ──────

  /** CLOSING block zones, requirements, and field patterns from schema. */
  closingData: SchemaClosingData;

  // ── Fill content (schema-driven file generation) ──────────────────

  /** Fill content for schema-driven file generation. Undefined if schema lacks fill_content. */
  fillContent?: SchemaFillContent;
}

// ---------------------------------------------------------------------------
// 3. Constants
// ---------------------------------------------------------------------------

/** Schema filenames per format (used to build schema IDs for the pipeline). */
const SCHEMA_FILES: Record<string, string> = {
  go:   "go-4block-schema.jsonc",
  rust: "rust-4block-schema.jsonc",
};

/** Supported format identifiers. */
export type CodeFormat = "go" | "rust";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Regex compilation helpers
// ---------------------------------------------------------------------------

/**
 * Convert a human-readable subsection name to a regex fragment.
 *
 * Handles the common patterns:
 * - "Core Types" → `Core\s*Types`     (spaces become optional whitespace)
 * - "Re-exports" → `Re-?exports`      (hyphens become optional)
 * - "Interface Definitions" → `Interface\s*Definitions`
 *
 * Word boundaries (\b) are added at the call site, not here.
 */
function nameToFragment(name: string): string {
  return name
    .split(/\s+/)
    .map(word => word.replace(/-/g, "-?"))
    .join("\\s*");
}

/**
 * Build a compiled regex for matching a SETUP subsection header.
 *
 * Matches patterns like:
 *   // Imports
 *   // 3. Constants
 *   // S.1 Imports
 *
 * All names (canonical + aliases) are combined with `|`.
 */
function buildSubsectionPattern(
  canonicalName: string,
  altNames: string[],
): RegExp {
  const fragments = [canonicalName, ...altNames].map(nameToFragment);
  const nameGroup = fragments.length === 1
    ? fragments[0]
    : `(?:${fragments.join("|")})`;

  // Match: // optional-S. optional-N. Name
  // - (?:S\.)? handles Rust's "S.1 Imports" prefix
  // - (?:\d+\.?\s+)? handles "3. Constants" numbering
  // - \\s{1,2} limits whitespace after // to 1-2 spaces, so block overview
  //   TOC lines (//   1. Imports  — desc) with 3+ spaces DON'T match.
  //   This distinguishes subsection headers from overview index lines.
  return new RegExp(
    `^\\/\\/\\s{1,2}(?:S\\.)?(?:\\d+\\.?\\s+)?${nameGroup}\\b`,
    "i",
  );
}

/**
 * Build an alias-only regex (excludes canonical name).
 * Returns undefined if no aliases exist.
 */
function buildAliasPattern(altNames: string[]): RegExp | undefined {
  if (altNames.length === 0) return undefined;

  const fragments = altNames.map(nameToFragment);
  const nameGroup = fragments.length === 1
    ? fragments[0]
    : `(?:${fragments.join("|")})`;

  return new RegExp(
    `^\\/\\/\\s{1,2}(?:S\\.)?(?:\\d+\\.?\\s+)?${nameGroup}\\b`,
    "i",
  );
}

// ---------------------------------------------------------------------------
// 2. Schema extraction — SETUP subsections
// ---------------------------------------------------------------------------

/**
 * Extract SETUP subsection definitions from a schema's SETUP block.
 *
 * Handles two schema formats:
 * - Rust-style: `subsection_order.S1_Imports` with position/tag/pattern/alt_patterns
 * - Go-style:   `subsection_order.S1_Imports` (same after upgrade)
 *
 * Both schemas now use the same S{N}_{Tag} key format.
 */
function extractSetupSubsections(
  // deno-lint-ignore no-explicit-any
  setupBlock: Record<string, any>,
): SchemaSubsectionDef[] {
  const subOrder = setupBlock["subsection_order"] as
    // deno-lint-ignore no-explicit-any
    | Record<string, any>
    | undefined;
  if (!subOrder) return [];

  const results: SchemaSubsectionDef[] = [];

  for (const [key, value] of Object.entries(subOrder)) {
    // Match S1_Imports, S2_Constants, ..., S10_BuildTags
    const keyMatch = key.match(/^S(\d+)_/);
    if (!keyMatch || typeof value !== "object" || !value) continue;

    // deno-lint-ignore no-explicit-any
    const v = value as Record<string, any>;
    results.push({
      position: typeof v["position"] === "number"
        ? v["position"]
        : parseInt(keyMatch[1]!, 10),
      tag: v["tag"] ?? "",
      pattern: v["pattern"] ?? v["tag"] ?? "",
      altPatterns: Array.isArray(v["alt_patterns"]) ? v["alt_patterns"] : [],
      purpose: v["purpose"] ?? "",
      productionNote: v["production_note"] ?? "",
      requiredIn: Array.isArray(v["required_in"]) ? v["required_in"] : [],
    });
  }

  // Sort by position (dependency chain)
  results.sort((a, b) => a.position - b.position);
  return results;
}

// ---------------------------------------------------------------------------
// 3. Schema extraction — BODY subtypes
// ---------------------------------------------------------------------------

/**
 * Extract BODY subtype definitions from a schema's BODY block.
 *
 * Handles structured format: each subtype has B{N}_{Tag} entries
 * with position/tag/pattern/alt_patterns/purpose/production_note/required_in.
 */
function extractBodySubtypes(
  // deno-lint-ignore no-explicit-any
  bodyBlock: Record<string, any>,
): Record<string, SchemaBodySubtype> {
  const subOrder = bodyBlock["subsection_order"] as
    // deno-lint-ignore no-explicit-any
    | Record<string, any>
    | undefined;
  if (!subOrder) return {};

  const result: Record<string, SchemaBodySubtype> = {};

  // Dynamic: extract subtype keys from schema (not hardcoded)
  const subtypeNames = Object.keys(subOrder).filter(
    (k) => typeof subOrder[k] === "object" && subOrder[k] !== null &&
           k !== "note" && k !== "ordering_varies_by_subtype"
  );

  for (const subtypeName of subtypeNames) {
    // deno-lint-ignore no-explicit-any
    const subtypeData = subOrder[subtypeName] as Record<string, any>;
    if (!subtypeData || typeof subtypeData !== "object") continue;

    const canonicalCount = typeof subtypeData["canonical_count"] === "number"
      ? subtypeData["canonical_count"]
      : 0;
    const orderingPrinciple =
      typeof subtypeData["ordering_principle"] === "string"
        ? subtypeData["ordering_principle"]
        : "";
    const note = typeof subtypeData["note"] === "string"
      ? subtypeData["note"]
      : "";

    const subsections: SchemaSubsectionDef[] = [];

    // Extract B1_Xxx through B9_Xxx entries
    for (const [key, value] of Object.entries(subtypeData)) {
      const keyMatch = key.match(/^B(\d+)_/);
      if (!keyMatch || typeof value !== "object" || !value) continue;

      // deno-lint-ignore no-explicit-any
      const v = value as Record<string, any>;
      subsections.push({
        position: typeof v["position"] === "number"
          ? v["position"]
          : parseInt(keyMatch[1]!, 10),
        tag: v["tag"] ?? "",
        pattern: v["pattern"] ?? v["tag"] ?? "",
        altPatterns: Array.isArray(v["alt_patterns"]) ? v["alt_patterns"] : [],
        purpose: v["purpose"] ?? "",
        productionNote: v["production_note"] ?? "",
        requiredIn: Array.isArray(v["required_in"]) ? v["required_in"] : [],
      });
    }

    subsections.sort((a, b) => a.position - b.position);

    // Normalize key: "demo_test" → "demo-test" (schema uses underscore, code uses hyphen)
    const normalizedName = subtypeName.replace(/_/g, "-");
    result[normalizedName] = {
      canonicalCount,
      orderingPrinciple,
      note,
      subsections,
    };
  }

  return result;
}

// ---------------------------------------------------------------------------
// 4. Schema extraction — placement rules + subtype emphasis
// ---------------------------------------------------------------------------

function extractPlacementRules(
  // deno-lint-ignore no-explicit-any
  setupBlock: Record<string, any>,
): { mustBeInSetup: string[]; mustNotBeInSetup: string[] } {
  // deno-lint-ignore no-explicit-any
  const rules = setupBlock["placement_rules"] as Record<string, any>;
  if (!rules) return { mustBeInSetup: [], mustNotBeInSetup: [] };

  return {
    mustBeInSetup: Array.isArray(rules["must_be_in_setup"])
      ? rules["must_be_in_setup"]
      : [],
    mustNotBeInSetup: Array.isArray(rules["must_not_be_in_setup"])
      ? rules["must_not_be_in_setup"]
      : [],
  };
}

function extractSubtypeEmphasis(
  // deno-lint-ignore no-explicit-any
  block: Record<string, any>,
): Record<string, { heavy: string[]; light: string[] }> {
  // deno-lint-ignore no-explicit-any
  const emphasis = block["subtype_emphasis"] as Record<string, any>;
  if (!emphasis) return {};

  const result: Record<string, { heavy: string[]; light: string[] }> = {};

  // Dynamic: extract subtype keys from schema (not hardcoded)
  const subtypeNames = Object.keys(emphasis).filter(
    (k) => typeof emphasis[k] === "object" && emphasis[k] !== null && k !== "note"
  );

  for (const subtypeName of subtypeNames) {
    // deno-lint-ignore no-explicit-any
    const data = emphasis[subtypeName] as Record<string, any>;
    if (!data || typeof data !== "object") continue;

    const normalizedName = subtypeName.replace(/_/g, "-");
    result[normalizedName] = {
      heavy: Array.isArray(data["heavy"]) ? data["heavy"] : [],
      light: Array.isArray(data["light"]) ? data["light"] : [],
    };
  }

  return result;
}

// ---------------------------------------------------------------------------
// 4b. Schema extraction — content kind placement maps
// ---------------------------------------------------------------------------

/**
 * Extract content_kind_mapping from the SETUP block.
 *
 * Returns block_placement (kind → "SETUP"|"BODY"), subsection_placement
 * (kind → subsection tag), and metadata_forbidden (list of forbidden kinds).
 *
 * Falls back to empty structures if the schema section is missing — this
 * keeps handlers working with older schemas that lack content_kind_mapping.
 */
function extractContentKindMapping(
  // deno-lint-ignore no-explicit-any
  setupBlock: Record<string, any>,
): PlacementMaps {
  // deno-lint-ignore no-explicit-any
  const mapping = setupBlock?.["content_kind_mapping"] as Record<string, any> | undefined;

  if (!mapping || typeof mapping !== "object") {
    return { blockPlacement: {}, subsectionPlacement: {}, metadataForbidden: [] };
  }

  const blockPlacement: Record<string, string> = {};
  const bp = mapping["block_placement"];
  if (bp && typeof bp === "object") {
    for (const [kind, block] of Object.entries(bp)) {
      if (typeof block === "string") blockPlacement[kind] = block;
    }
  }

  const subsectionPlacement: Record<string, string> = {};
  const sp = mapping["subsection_placement"];
  if (sp && typeof sp === "object") {
    for (const [kind, tag] of Object.entries(sp)) {
      if (typeof tag === "string") subsectionPlacement[kind] = tag;
    }
  }

  const metadataForbidden: string[] = [];
  const mf = mapping["metadata_forbidden"];
  if (Array.isArray(mf)) {
    for (const item of mf) {
      if (typeof item === "string") metadataForbidden.push(item);
    }
  }

  return { blockPlacement, subsectionPlacement, metadataForbidden };
}

// ---------------------------------------------------------------------------
// 5. Schema extraction — METADATA block (I/C, doc comments, subtypes)
// ---------------------------------------------------------------------------

/**
 * Extract I/C field requirements from the METADATA block's identity_statics.
 *
 * Reads pragma_sections (I1-I4) and metadata_sections (C1-C7).
 * Both Go and Rust schemas define these identically — the I/C standard
 * is format-agnostic, only the carrier syntax differs.
 *
 * Returns two maps: pragma (I-sections) and metadata (C-sections).
 * Each entry has required[], defined[], and purpose.
 */
function extractFieldRequirements(
  // deno-lint-ignore no-explicit-any
  metadataBlock: Record<string, any>,
): {
  pragma: Record<string, SchemaFieldRequirement>;
  metadata: Record<string, SchemaFieldRequirement>;
} {
  // deno-lint-ignore no-explicit-any
  const statics = metadataBlock["identity_statics"] as Record<string, any>;
  if (!statics) return { pragma: {}, metadata: {} };

  const pragma: Record<string, SchemaFieldRequirement> = {};
  const metadata: Record<string, SchemaFieldRequirement> = {};

  // Extract pragma_sections → I1, I2, I3, I4
  // deno-lint-ignore no-explicit-any
  const pragmaSections = statics["pragma_sections"] as Record<string, any>;
  if (pragmaSections) {
    for (const [key, value] of Object.entries(pragmaSections)) {
      if (typeof value !== "object" || !value) continue;
      // deno-lint-ignore no-explicit-any
      const v = value as Record<string, any>;

      // Key is like "I1_core" — normalize to "I1"
      const sectionMatch = key.match(/^(I\d+)_/);
      if (!sectionMatch) continue;

      pragma[sectionMatch[1]!] = {
        required: Array.isArray(v["required"]) ? v["required"] : [],
        defined: Array.isArray(v["defined"]) ? v["defined"] : [],
        purpose: typeof v["purpose"] === "string" ? v["purpose"] : "",
      };
    }
  }

  // Extract metadata_sections → C1, C2, ..., C7
  // deno-lint-ignore no-explicit-any
  const metaSections = statics["metadata_sections"] as Record<string, any>;
  if (metaSections) {
    for (const [key, value] of Object.entries(metaSections)) {
      if (typeof value !== "object" || !value) continue;
      // deno-lint-ignore no-explicit-any
      const v = value as Record<string, any>;

      // Key is like "C1_state" — normalize to "C1"
      const sectionMatch = key.match(/^(C\d+)_/);
      if (!sectionMatch) continue;

      metadata[sectionMatch[1]!] = {
        required: Array.isArray(v["required"]) ? v["required"] : [],
        defined: Array.isArray(v["defined"]) ? v["defined"] : [],
        purpose: typeof v["purpose"] === "string" ? v["purpose"] : "",
      };
    }
  }

  return { pragma, metadata };
}

/**
 * Extract doc comment expectations from the METADATA block.
 *
 * Go has: package_doc, pragma_doc, metadata_doc (each with purpose, severity, format).
 * Rust has: purpose, location, crate_root_severity, module_severity.
 *
 * Returns a map of comment type → expectation.
 */
function extractDocComments(
  // deno-lint-ignore no-explicit-any
  metadataBlock: Record<string, any>,
): Record<string, SchemaDocComment> {
  // deno-lint-ignore no-explicit-any
  const docComments = metadataBlock["doc_comments"] as Record<string, any>;
  if (!docComments) return {};

  const result: Record<string, SchemaDocComment> = {};

  for (const [key, value] of Object.entries(docComments)) {
    // Skip non-object entries (like "note" string)
    if (typeof value !== "object" || !value) continue;
    // Skip known non-doc entries
    if (key === "three_tier_discipline") continue;

    // deno-lint-ignore no-explicit-any
    const v = value as Record<string, any>;
    result[key] = {
      purpose: typeof v["purpose"] === "string" ? v["purpose"] : "",
      severity: typeof v["severity"] === "string"
        ? v["severity"]
        : typeof v["crate_root_severity"] === "string"
          ? v["crate_root_severity"]
          : "info",
      format: typeof v["format"] === "string" ? v["format"] : undefined,
      contains: typeof v["contains"] === "string" ? v["contains"] : undefined,
      location: typeof v["location"] === "string" ? v["location"] : undefined,
    };
  }

  // Rust flat pattern: doc_comments has crate_root_severity/module_severity
  // directly (not nested objects). Synthesize named entries.
  if (Object.keys(result).length === 0) {
    const purpose = typeof docComments["purpose"] === "string"
      ? docComments["purpose"] : "";
    const location = typeof docComments["location"] === "string"
      ? docComments["location"] : undefined;

    if (typeof docComments["crate_root_severity"] === "string") {
      result["crate_root_docs"] = {
        purpose,
        severity: docComments["crate_root_severity"],
        format: "//!",
        location,
      };
    }
    if (typeof docComments["module_severity"] === "string") {
      result["module_docs"] = {
        purpose,
        severity: docComments["module_severity"],
        format: "//!",
        location,
      };
    }
  }

  return result;
}

/**
 * Extract subtype definitions from the METADATA block.
 *
 * Both Go and Rust define library, executable, demo_test subtypes
 * with I2_subtype, file_pattern, directives, and capabilities.
 */
function extractSubtypeDefs(
  // deno-lint-ignore no-explicit-any
  metadataBlock: Record<string, any>,
): Record<string, SchemaSubtypeDef> {
  // deno-lint-ignore no-explicit-any
  const subtypes = metadataBlock["subtypes"] as Record<string, any>;
  if (!subtypes) return {};

  const result: Record<string, SchemaSubtypeDef> = {};

  // Dynamic: extract subtype keys from schema (not hardcoded)
  const subtypeNames = Object.keys(subtypes).filter(
    (k) => typeof subtypes[k] === "object" && subtypes[k] !== null && k !== "note"
  );

  for (const subtypeName of subtypeNames) {
    // deno-lint-ignore no-explicit-any
    const data = subtypes[subtypeName] as Record<string, any>;
    if (!data || typeof data !== "object") continue;

    // Normalize: "demo_test" → "demo-test", "bare_bone" → "bare-bone"
    const normalizedName = subtypeName.replace(/_/g, "-");
    result[normalizedName] = {
      subtypeValue: typeof data["I2_subtype"] === "string" ? data["I2_subtype"] : normalizedName,
      filePattern: typeof data["file_pattern"] === "string" ? data["file_pattern"] : "",
      pragmaDirective: typeof data["pragma_directive"] === "string" ? data["pragma_directive"] : "",
      templateDirective: typeof data["template_directive"] === "string" ? data["template_directive"] : "",
      hasPublicApi: data["has_public_api"] === true,
      hasTestsBlock: data["has_tests_block"] === true ? true : undefined,
      packagePattern: typeof data["package"] === "string" ? data["package"] : undefined,
      purpose: typeof data["purpose"] === "string" ? data["purpose"] : "",
    };
  }

  return result;
}

// ---------------------------------------------------------------------------
// 6. Schema extraction — CLOSING block (zones, requirements, field patterns)
// ---------------------------------------------------------------------------

/**
 * Build a zone detection regex from a tag and kind.
 *
 * Code zones match: `// Cv — ...`, `// Ce — ...`, `// Cc — ...`
 *   Pattern: `// TAG DASH` where DASH is em-dash, en-dash, or hyphen.
 *
 * Doc zones match: `// X1: ...` or `// X1 ...` (colon or space after tag)
 *   Pattern: `// TAG COLON-OR-SPACE`
 *
 * These match the patterns that were hardcoded in types.ts CLOSING_ZONES.
 */
function buildZonePattern(tag: string, kind: "code" | "doc"): RegExp {
  if (kind === "code") {
    // Accept both descriptive format (// Cv — Validation) and bare subsection
    // header format (// Cv). The subsection header is what the transform emits;
    // the descriptive format is what humans may add. Both are valid zone markers.
    return new RegExp(`^\\/\\/\\s+${tag}(\\s+[—–-]|\\s*$)`);
  }
  // Accept: // X1: description, // X1 description, or bare // X1 (subsection header)
  return new RegExp(`^\\/\\/\\s+${tag}([:\\s]|$)`);
}

/**
 * Build a field detection regex from a field name.
 *
 * Matches: `// Never:`, `// NEVER:`, `// never:` etc.
 * Pattern: `// FIELDNAME :` with case-insensitive flag.
 *
 * These match the patterns that were hardcoded in types.ts
 * X1_FIELD_PATTERNS and X5_FIELD_PATTERNS.
 */
function buildFieldPattern(fieldName: string): RegExp {
  return new RegExp(`^\\/\\/\\s*${fieldName}\\s*:`, "i");
}

/**
 * Extract CLOSING block data from a schema's CLOSING section.
 *
 * Reads zone_ordering, code_zones, documentation_sections.
 * Builds compiled regexes for zone detection and field matching.
 *
 * Falls back to empty structures if the schema lacks CLOSING data —
 * this keeps the linter working with older schemas that haven't been
 * enriched yet (they'll just skip CLOSING checks).
 */
function extractClosingData(
  // deno-lint-ignore no-explicit-any
  closingBlock: Record<string, any> | undefined,
): SchemaClosingData {
  const zones: SchemaClosingZone[] = [];
  const docRequirements: SchemaClosingDocReq[] = [];
  const fieldPatterns: Record<string, Record<string, RegExp>> = {};

  if (!closingBlock) return { zones, docRequirements, fieldPatterns };

  // ── Extract zone ordering ───────────────────────────────────────
  // deno-lint-ignore no-explicit-any
  const zoneOrdering = closingBlock["zone_ordering"] as Record<string, any>;
  const codeZoneTags: string[] = zoneOrdering
    ? (Array.isArray(zoneOrdering["code_zones"]) ? zoneOrdering["code_zones"] : [])
    : [];
  const docZoneTags: string[] = zoneOrdering
    ? (Array.isArray(zoneOrdering["documentation_sections"]) ? zoneOrdering["documentation_sections"] : [])
    : [];

  // ── Build code zones from code_zones section ────────────────────
  // deno-lint-ignore no-explicit-any
  const codeZonesData = closingBlock["code_zones"] as Record<string, any>;
  if (codeZonesData) {
    // Walk the code_zones section — look for entries with "tag" field
    for (const tag of codeZoneTags) {
      // Find the entry with matching tag (e.g., Cv_Validation has tag: "Cv")
      let found = false;
      for (const [, value] of Object.entries(codeZonesData)) {
        if (typeof value !== "object" || !value) continue;
        // deno-lint-ignore no-explicit-any
        const v = value as Record<string, any>;
        if (v["tag"] === tag) {
          zones.push({
            tag,
            kind: "code",
            pattern: buildZonePattern(tag, "code"),
          });
          found = true;
          break;
        }
      }
      // If no detailed entry found, still add from ordering
      if (!found) {
        zones.push({
          tag,
          kind: "code",
          pattern: buildZonePattern(tag, "code"),
        });
      }
    }
  } else {
    // No detailed code_zones — build from ordering alone
    for (const tag of codeZoneTags) {
      zones.push({ tag, kind: "code", pattern: buildZonePattern(tag, "code") });
    }
  }

  // ── Build doc zones from documentation_sections ─────────────────
  // deno-lint-ignore no-explicit-any
  const docSectionsData = closingBlock["documentation_sections"] as Record<string, any>;
  if (docSectionsData) {
    for (const tag of docZoneTags) {
      // Find the entry with matching tag
      let entryFound = false;
      for (const [, value] of Object.entries(docSectionsData)) {
        if (typeof value !== "object" || !value) continue;
        // deno-lint-ignore no-explicit-any
        const v = value as Record<string, any>;
        if (v["tag"] === tag) {
          zones.push({
            tag,
            kind: "doc",
            pattern: buildZonePattern(tag, "doc"),
          });

          // Build doc requirement
          const req: SchemaClosingDocReq = {
            tag,
            required: v["required"] === true,
            templateOnly: v["template_only"] === true ? true : undefined,
          };
          if (v["fields"] && typeof v["fields"] === "object") {
            // deno-lint-ignore no-explicit-any
            const fields = v["fields"] as Record<string, any>;
            req.fields = {
              required: Array.isArray(fields["required"]) ? fields["required"] : [],
              defined: Array.isArray(fields["defined"]) ? fields["defined"] : [],
            };
            // Build field detection patterns
            const patterns: Record<string, RegExp> = {};
            for (const f of req.fields.required) {
              patterns[f] = buildFieldPattern(f);
            }
            for (const f of req.fields.defined) {
              if (!patterns[f]) patterns[f] = buildFieldPattern(f);
            }
            if (Object.keys(patterns).length > 0) {
              fieldPatterns[tag] = patterns;
            }
          }
          docRequirements.push(req);
          entryFound = true;
          break;
        }
      }
      // If no detailed entry, add minimal zone + non-required doc req
      if (!entryFound) {
        zones.push({ tag, kind: "doc", pattern: buildZonePattern(tag, "doc") });
        docRequirements.push({ tag, required: false });
      }
    }
  } else {
    // No detailed doc sections — build from ordering alone
    for (const tag of docZoneTags) {
      zones.push({ tag, kind: "doc", pattern: buildZonePattern(tag, "doc") });
      docRequirements.push({ tag, required: false });
    }
  }

  return { zones, docRequirements, fieldPatterns };
}

// ---------------------------------------------------------------------------
// 6b. Fill content extraction
// ---------------------------------------------------------------------------

/**
 * Extract fill content from a schema's top-level `fill_content` section.
 *
 * Returns undefined if the schema doesn't have fill_content — this is valid
 * (fill_content is enrichment, not required for linting).
 */
function extractFillContent(
  // deno-lint-ignore no-explicit-any
  structure: Record<string, any> | undefined,
): SchemaFillContent | undefined {
  if (!structure) return undefined;
  // deno-lint-ignore no-explicit-any
  const fc = structure["fill_content"] as Record<string, any> | undefined;
  if (!fc) return undefined;

  // Defaults — capture core fields + any additional schema-defined defaults
  // deno-lint-ignore no-explicit-any
  const rawDefaults = (fc["defaults"] ?? {}) as Record<string, any>;
  const defaults: SchemaFillDefaults = {
    version: String(rawDefaults["version"] ?? "a-01.00"),
    status: String(rawDefaults["status"] ?? "Active"),
    organization: String(rawDefaults["organization"] ?? "CreativeWorkzStudio LLC"),
    scripture: String(rawDefaults["scripture"] ?? "Genesis 1:1"),
    scripture_text: String(rawDefaults["scripture_text"] ?? "In the beginning God created the heaven and the earth."),
    type: String(rawDefaults["type"] ?? "code"),
    structure: String(rawDefaults["structure"] ?? "4-block"),
  };
  // Capture extra defaults (architect, role, layer, domain, paradigm, etc.)
  for (const [key, value] of Object.entries(rawDefaults)) {
    if (!(key in defaults) && typeof value === "string") {
      defaults[key] = value;
    }
  }

  // Directives
  const directives: string[] = Array.isArray(fc["directives"])
    ? fc["directives"].map(String)
    : [];

  // Package/module declarations (language-specific)
  const packageDeclaration = fc["package_declaration"]
    ? String(fc["package_declaration"])
    : undefined;
  const moduleDoc = fc["module_doc"]
    ? String(fc["module_doc"])
    : undefined;

  // Identity syntax
  // deno-lint-ignore no-explicit-any
  const rawSyntax = (fc["identity_syntax"] ?? {}) as Record<string, any>;
  const identitySyntax = {
    pragma: extractSyntaxEntry(rawSyntax["pragma"]),
    metadata: extractSyntaxEntry(rawSyntax["metadata"]),
  };

  // Pragma/Metadata entries
  const pragmaEntries = extractEntryPairs(fc["pragma_entries"]);
  const metadataEntries = extractEntryPairs(fc["metadata_entries"]);

  // Metadata comment lines
  const metadataComment: string[] = Array.isArray(fc["metadata_comment"])
    ? fc["metadata_comment"].map(String)
    : [];

  // Closing defaults
  const closingDefaults: SchemaClosingDefaults = {};
  // deno-lint-ignore no-explicit-any
  const rawClosing = (fc["closing_defaults"] ?? {}) as Record<string, any>;
  for (const [tag, fields] of Object.entries(rawClosing)) {
    if (fields && typeof fields === "object" && !Array.isArray(fields)) {
      const entry: Record<string, string> = {};
      for (const [k, v] of Object.entries(fields as Record<string, unknown>)) {
        entry[k] = String(v);
      }
      closingDefaults[tag] = entry;
    }
  }

  // Identity groups — structured METADATA formatting data
  const identityGroups = extractIdentityGroups(fc);

  // Transformer modes — strict/balance/growth leniency scale
  const transformerModes = extractTransformerModes(fc);

  return {
    defaults,
    directives,
    packageDeclaration,
    moduleDoc,
    identitySyntax,
    pragmaEntries,
    metadataEntries,
    metadataComment,
    closingDefaults,
    identityGroups,
    transformerModes,
  };
}

/** Extract a syntax entry (declaration/entry/close) from a raw object. */
function extractSyntaxEntry(
  // deno-lint-ignore no-explicit-any
  raw: Record<string, any> | undefined,
): SchemaIdentitySyntax {
  if (!raw) return { declaration: "", entry: "", close: "" };
  return {
    declaration: String(raw["declaration"] ?? ""),
    entry: String(raw["entry"] ?? ""),
    close: String(raw["close"] ?? ""),
  };
}

/** Extract ordered [field, value] pairs from a raw array. */
function extractEntryPairs(
  // deno-lint-ignore no-explicit-any
  raw: any[] | undefined,
): [string, string][] {
  if (!Array.isArray(raw)) return [];
  return raw
    .filter((item) => Array.isArray(item) && item.length >= 2)
    .map((item) => [String(item[0]), String(item[1])]);
}

/**
 * Extract identity groups from fill_content.identity_groups.
 *
 * Returns undefined if not present (forward-compatible with older schemas).
 */
function extractIdentityGroups(
  // deno-lint-ignore no-explicit-any
  fc: Record<string, any>,
): IdentityGrouping | undefined {
  // deno-lint-ignore no-explicit-any
  const raw = fc["identity_groups"] as Record<string, any> | undefined;
  if (!raw || typeof raw !== "object") return undefined;

  function parseGroups(arr: unknown): IdentityGroup[] {
    if (!Array.isArray(arr)) return [];
    return arr
      .filter((item) => item && typeof item === "object" && "range" in item)
      .map((item) => ({
        range: String(item.range),
        label: String(item.label ?? ""),
        ...(typeof item.docstring === "string" ? { docstring: item.docstring } : {}),
      }));
  }

  const pragma = parseGroups(raw["pragma"]);
  const metadata = parseGroups(raw["metadata"]);

  // deno-lint-ignore no-explicit-any
  const headers = (raw["section_headers"] ?? {}) as Record<string, any>;
  const sectionHeaders = {
    pragma: typeof headers["pragma"] === "string" ? headers["pragma"] : "",
    metadata: typeof headers["metadata"] === "string" ? headers["metadata"] : "",
  };

  return { pragma, metadata, sectionHeaders };
}

/**
 * Extract transformer modes from fill_content.transformer_modes.
 *
 * Returns undefined if not present (forward-compatible with older schemas).
 */
function extractTransformerModes(
  // deno-lint-ignore no-explicit-any
  fc: Record<string, any>,
): TransformerModes | undefined {
  // deno-lint-ignore no-explicit-any
  const raw = fc["transformer_modes"] as Record<string, any> | undefined;
  if (!raw || typeof raw !== "object") return undefined;

  function parseMode(
    // deno-lint-ignore no-explicit-any
    obj: Record<string, any> | undefined,
  ): TransformerMode {
    if (!obj) {
      return {
        linterTarget: "0E", identityFormatting: false, sectionHeaders: false,
        groupComments: false, docstrings: false, columnAlignment: false,
        blockOverviews: false, roGrouped: false, closingComplete: false,
      };
    }
    return {
      linterTarget: typeof obj["linter_target"] === "string" ? obj["linter_target"] : "0E",
      identityFormatting: obj["identity_formatting"] === true,
      sectionHeaders: obj["section_headers"] === true,
      groupComments: obj["group_comments"] === true,
      docstrings: obj["docstrings"] === true,
      columnAlignment: obj["column_alignment"] === true,
      blockOverviews: obj["block_overviews"] === true,
      roGrouped: obj["ro_grouped"] === true,
      closingComplete: obj["closing_complete"] === true,
    };
  }

  // deno-lint-ignore no-explicit-any
  const strict = parseMode(raw["strict"] as Record<string, any> | undefined);
  // deno-lint-ignore no-explicit-any
  const balance = parseMode(raw["balance"] as Record<string, any> | undefined);
  // deno-lint-ignore no-explicit-any
  const growth = parseMode(raw["growth"] as Record<string, any> | undefined);

  return { strict, balance, growth };
}

// ---------------------------------------------------------------------------
// 7. Schema resolution + caching
// ---------------------------------------------------------------------------

/** Per-format caches. Loaded once per format, used for all subsequent calls. */
const caches = new Map<string, Code4BlockRules>();

/**
 * Build the schema ID for a code format.
 * Schema IDs are relative paths within the schema tree.
 */
function schemaIdFor(format: CodeFormat): string {
  const file = SCHEMA_FILES[format];
  if (!file) throw new Error(`Unknown code format: ${format}. Known: ${Object.keys(SCHEMA_FILES).join(", ")}`);
  return `code/format/${file}`;
}

// ---------------------------------------------------------------------------
// 6. Pure parse — schema text to compiled rules (no I/O)
// ---------------------------------------------------------------------------

/**
 * Parse a 4-block code schema from JSONC text. **Pure function — no I/O.**
 *
 * Takes raw JSONC text and a format identifier, returns compiled handler-ready
 * rules. All extract*() functions are called here. The format parameter is used
 * for structure key lookup (go_structure vs rust_structure) and error messages.
 *
 * This is the core of the schema pipeline — the thing that turns text into
 * rules. Where the text comes from (file, embedded, override) is the caller's
 * concern.
 *
 * @param format - Format identifier ("go", "rust", or future formats)
 * @param jsonText - Raw JSONC text content of the schema file
 * @returns Compiled handler-ready rules
 * @throws Error if text is invalid JSONC or missing required structure
 */
export function parseCodeSchema(format: string, jsonText: string): Code4BlockRules {
  // ── Parse JSONC ───────────────────────────────────────────────

  // deno-lint-ignore no-explicit-any
  let parsed: Record<string, any>;
  try {
    // deno-lint-ignore no-explicit-any
    parsed = parseJsonc(jsonText) as Record<string, any>;
  } catch (e) {
    throw new Error(
      `Schema parse error for ${format}: ${e instanceof Error ? e.message : String(e)}\n` +
      `The schema must be valid JSONC. Check for syntax errors.`,
    );
  }

  if (!parsed || typeof parsed !== "object") {
    throw new Error(
      `Schema for ${format} parsed to non-object — expected JSONC object`,
    );
  }

  // ── Navigate to structural blocks ─────────────────────────────

  // Each schema nests block definitions under a format-specific key:
  //   Go:   go_structure.SETUP, go_structure.BODY
  //   Rust: rust_structure.SETUP, rust_structure.BODY
  // Fall back to "blocks" for future schemas that use a generic key.
  // deno-lint-ignore no-explicit-any
  const blocks = (parsed[`${format}_structure`] ?? parsed["blocks"]) as Record<string, any>;
  if (!blocks) {
    throw new Error(
      `Schema for ${format} missing structure key (expected "${format}_structure" or "blocks") — cannot derive rules`,
    );
  }

  // deno-lint-ignore no-explicit-any
  const setupBlock = blocks["SETUP"] as Record<string, any>;
  // deno-lint-ignore no-explicit-any
  const bodyBlock = blocks["BODY"] as Record<string, any>;
  // deno-lint-ignore no-explicit-any
  const metadataBlock = blocks["METADATA"] as Record<string, any>;

  const missing: string[] = [];
  if (!setupBlock) missing.push("SETUP");
  if (!bodyBlock) missing.push("BODY");
  // METADATA is expected but not fatal — handlers can fall back to empty
  if (!metadataBlock) missing.push("METADATA");

  if (missing.length > 0) {
    // Only throw if SETUP or BODY is missing — those are structural requirements
    const critical = missing.filter((b) => b !== "METADATA");
    if (critical.length > 0) {
      throw new Error(
        `Schema for ${format} missing blocks: ${critical.join(", ")}`,
      );
    }
  }

  // ── Extract SETUP + BODY raw data ─────────────────────────────

  const setupData = extractSetupSubsections(setupBlock);
  const bodyData = extractBodySubtypes(bodyBlock);
  const placementRules = extractPlacementRules(setupBlock);

  // Subtype emphasis is block-specific — SETUP and BODY have different heavy/light distributions
  const subtypeEmphasis = {
    setup: extractSubtypeEmphasis(setupBlock),
    body: extractSubtypeEmphasis(bodyBlock),
  };

  // Content kind → block/subsection placement maps
  const placementMaps = extractContentKindMapping(setupBlock);

  // ── Extract METADATA block data ───────────────────────────────

  const fieldReqs = metadataBlock
    ? extractFieldRequirements(metadataBlock)
    : { pragma: {}, metadata: {} };
  const docCommentExpectations = metadataBlock
    ? extractDocComments(metadataBlock)
    : {};
  const subtypeDefinitions = metadataBlock
    ? extractSubtypeDefs(metadataBlock)
    : {};

  // ── Extract CLOSING block data ──────────────────────────────

  // deno-lint-ignore no-explicit-any
  const closingBlock = blocks["CLOSING"] as Record<string, any> | undefined;
  const closingData = extractClosingData(closingBlock);

  // ── Extract fill content (for schema-driven file generation) ──

  // deno-lint-ignore no-explicit-any
  const fillContent = extractFillContent(parsed[`${format}_structure`] ?? parsed["blocks"] as Record<string, any> | undefined);

  // ── Compile SETUP subsection regexes ──────────────────────────

  const setupSubsections: SubsectionDef[] = setupData.map((s) => ({
    tag: s.tag,
    pattern: buildSubsectionPattern(s.pattern, s.altPatterns),
    aliases: buildAliasPattern(s.altPatterns),
  }));

  // ── Build BODY canonical name arrays ──────────────────────────

  const bodySubsections: Record<string, readonly string[]> = {};
  for (const [subtype, data] of Object.entries(bodyData)) {
    bodySubsections[subtype] = data.subsections.map((s) => s.pattern);
  }

  // ── Assemble ──────────────────────────────────────────────────

  return {
    setupSubsections,
    bodySubsections,
    placementRules,
    setupData,
    bodyData,
    subtypeEmphasis,
    placementMaps,
    pragmaFieldRequirements: fieldReqs.pragma,
    metadataFieldRequirements: fieldReqs.metadata,
    docCommentExpectations,
    subtypeDefinitions,
    closingData,
    fillContent,
  };
}

// ---------------------------------------------------------------------------
// 7. I/O loader — thin wrapper that reads file + delegates to parseCodeSchema
// ---------------------------------------------------------------------------

/**
 * Load and process a 4-block code schema.
 *
 * Resolves the schema through the pipeline (env override → project-local →
 * default), then delegates to parseCodeSchema() for parsing and compilation.
 * Result is cached — subsequent calls return cache.
 *
 * This is the I/O boundary. parseCodeSchema() is the pure computation.
 * The pipeline handles discovery, caching raw text, and error diagnostics.
 *
 * @param format - "go" or "rust"
 * @returns Derived rules compiled for handler use
 */
export async function loadCodeRules(format: CodeFormat): Promise<Code4BlockRules> {
  const cached = caches.get(format);
  if (cached) return cached;

  const pipeline = getDefaultPipeline();
  const text = await pipeline.getText(schemaIdFor(format));

  const rules = parseCodeSchema(format, text);
  caches.set(format, rules);
  return rules;
}

/**
 * Clear cached rules. Useful for testing or after schema edits.
 *
 * @param format - Clear only this format's cache. If omitted, clears all.
 */
export function clearCodeCache(format?: string): void {
  if (format) {
    caches.delete(format);
  } else {
    caches.clear();
  }
}

// ---------------------------------------------------------------------------
// 7. Form Schema Loader — load per-subtype form constraints
// ---------------------------------------------------------------------------

/** Cache for loaded form constraints: "rust:module" → FormConstraints. */
const formCache = new Map<string, FormConstraints | null>();

/**
 * Parse a form schema's JSONC text into FormConstraints.
 *
 * Pure function — no I/O.
 */
export function parseFormSchema(jsonText: string): FormConstraints | null {
  // deno-lint-ignore no-explicit-any
  let data: Record<string, any>;
  try {
    data = parseJsonc(jsonText) as Record<string, any>;
  } catch {
    return null;
  }

  // deno-lint-ignore no-explicit-any
  const identity = data["form_identity"] as Record<string, any> | undefined;
  if (!identity) return null;

  // deno-lint-ignore no-explicit-any
  const containers = data["containers"] as Record<string, any> | undefined;
  if (!containers) return null;

  const name = typeof identity["name"] === "string" ? identity["name"] : "";
  const isFoundation = identity["is_foundation"] === true;
  const inherits = typeof identity["inherits"] === "string" ? identity["inherits"] : undefined;
  const ordering = typeof identity["ordering"] === "string" ? identity["ordering"] : "";

  function extractContainer(
    // deno-lint-ignore no-explicit-any
    block: Record<string, any> | undefined,
  ): FormContainerConstraints {
    if (!block) return { can: [], cannot: [] };

    const canSections: FormSectionConstraint[] = [];
    const cannotSections: FormReservedSection[] = [];

    // Extract "can" entries — objects with position + tag + status
    // deno-lint-ignore no-explicit-any
    const canBlock = block["can"] as Record<string, any> | undefined;
    if (canBlock && typeof canBlock === "object") {
      for (const [_key, value] of Object.entries(canBlock)) {
        if (!value || typeof value !== "object" || !("tag" in value)) continue;
        const tag = typeof value["tag"] === "string" ? value["tag"] : "";
        const position = typeof value["position"] === "number" ? value["position"] : 0;
        const statusRaw = typeof value["status"] === "string" ? value["status"] : "";
        // Normalize: "REQUIRED", "AVAILABLE (...)" → "REQUIRED" or "AVAILABLE"
        const status: "REQUIRED" | "AVAILABLE" = statusRaw.startsWith("REQUIRED")
          ? "REQUIRED"
          : "AVAILABLE";
        canSections.push({ position, tag, status });
      }
    }

    // Extract "cannot" entries — two patterns:
    // 1. Individual: { tag?, why_reserved, active_in } — key infers tag (S2_Modules → Modules)
    // 2. Grouped: { sections: ["Tag1", "Tag2"], why_reserved } — expands to multiple entries
    // deno-lint-ignore no-explicit-any
    const cannotBlock = block["cannot"] as Record<string, any> | undefined;
    if (cannotBlock && typeof cannotBlock === "object") {
      for (const [_key, value] of Object.entries(cannotBlock)) {
        if (!value || typeof value !== "object") continue;
        // Skip non-reserved entries (description strings, etc.)
        if (!("why_reserved" in value)) continue;

        const whyReserved = typeof value["why_reserved"] === "string" ? value["why_reserved"] : "";
        const activeIn = typeof value["active_in"] === "string" ? value["active_in"] : "";

        // Extract canonical position from key prefix: "S2_Modules" → 2, "B6_Output" → 6
        const posMatch = _key.match(/^[A-Z](\d+)_/);
        const position = posMatch ? parseInt(posMatch[1]!, 10) : undefined;

        // Pattern 2: grouped entry with sections array — expand each section
        if (Array.isArray(value["sections"])) {
          for (const section of value["sections"]) {
            if (typeof section === "string") {
              cannotSections.push({ tag: section, whyReserved, activeIn });
            }
          }
          continue;
        }

        // Pattern 1: individual entry — infer tag from key or explicit tag field
        const tag = typeof value["tag"] === "string"
          ? value["tag"]
          // Infer tag from key: "S2_Modules" → "Modules", "B6_Output" → "Output"
          : _key.replace(/^[A-Z]\d+_/, "");
        cannotSections.push({ tag, whyReserved, activeIn, position });
      }
    }

    // Sort CAN by position
    canSections.sort((a, b) => a.position - b.position);

    return { can: canSections, cannot: cannotSections };
  }

  // Build result with required 4-block containers
  const result: FormConstraints = {
    name,
    isFoundation,
    inherits,
    ordering,
    SETUP: extractContainer(containers["SETUP"]),
    BODY: extractContainer(containers["BODY"]),
    CLOSING: extractContainer(containers["CLOSING"]),
  };

  // 3-block support: CONTENT is the equivalent of SETUP + BODY for data files
  if (containers["CONTENT"]) {
    result.CONTENT = extractContainer(containers["CONTENT"]);
  }

  return result;
}

/**
 * Load form constraints for a specific format + subtype.
 *
 * Returns null gracefully if the form schema doesn't exist (forward-compatible).
 * Caches per format:subtype:category key.
 *
 * Schema ID mapping (category="code", the default):
 *   "bare-bone" → "code/forms/bare-bone/rust-bare-bone.jsonc"
 *   all others  → "code/forms/declared/rust-{subtype}.jsonc"
 *
 * Schema ID mapping (category="data"):
 *   "bare-bone" → "data/forms/bare-bone/toml-bare-bone.jsonc"
 *   all others  → "data/forms/declared/toml-{subtype}.jsonc"
 */
export async function loadFormConstraints(
  format: string,
  subtype: string,
  category: "code" | "data" = "code",
): Promise<FormConstraints | null> {
  const cacheKey = `${category}:${format}:${subtype}`;
  if (formCache.has(cacheKey)) return formCache.get(cacheKey)!;

  // Build schema ID based on subtype and category
  const schemaId = subtype === "bare-bone"
    ? `${category}/forms/bare-bone/${format}-bare-bone.jsonc`
    : `${category}/forms/declared/${format}-${subtype}.jsonc`;

  try {
    const pipeline = getDefaultPipeline();
    const text = await pipeline.getText(schemaId);
    if (!text) {
      formCache.set(cacheKey, null);
      return null;
    }
    const constraints = parseFormSchema(text);
    formCache.set(cacheKey, constraints);
    return constraints;
  } catch {
    formCache.set(cacheKey, null);
    return null;
  }
}

/**
 * Clear form constraint cache (for testing or after schema edits).
 */
export function clearFormCache(): void {
  formCache.clear();
}

// ============================================================================
// CLOSING
// ============================================================================
//
// The schema is the single source of truth for code structure.
// This loader reads it once, compiles it into handler-ready form,
// and caches for the session. When the schema grows, the linter grows.
//
// Schema says WHAT. Handler says HOW. Tests say PROVE IT.
//
// "The entrance of thy words giveth light." — Psalm 119:130
// ============================================================================
