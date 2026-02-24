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
import type { SubsectionDef } from "./types.ts";
import {
  SECTION_REGISTRY,
  SECTION_ORDER,
  FORM_REGISTRY,
} from "../data/mod.ts";
import type { SectionEntry, FormDefinition } from "../data/mod.ts";
import { registerCache } from "./cache-registry.ts";

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
// 2b. Types — Typing map (arrow refinement profiles)
// ---------------------------------------------------------------------------

/** Section profile for a typing arrow refinement (e.g., module->utility). */
export interface TypingBlockProfile {
  required: string[];
  available: string[];
  irrelevant: string[];
  /**
   * Per-section concept overrides for required sections.
   * Maps section tag (PascalCase from schema) → concept → ternary override.
   *
   * Used when a typing's required section needs narrower concept expectations
   * than the base section concept map (e.g., FreeFunctions in a utility module
   * doesn't expect variable_binding or error_propagation).
   *
   * Only CHANGES from the base concept map need to be listed.
   */
  conceptOverrides?: Record<string, Record<string, string>>;
}

/** Complete typing profile for a form subtype (e.g., "utility" under "module"). */
export interface TypingProfile {
  description: string;
  SETUP: TypingBlockProfile;
  BODY: TypingBlockProfile;
}

// ---------------------------------------------------------------------------
// 2c. Types — CLOSING block schema data
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
 * Replaces the hardcoded constants in shared/types.ts:
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

  /** BODY subsections with compiled regexes — ALL 13 BODY sections, universal.
   *  Same pattern as setupSubsections: schema → SubsectionDef[] → getSubsectionRanges().
   *  Form-level filtering (bare-bone → library → ...) is separate from detection.
   *  Detection is universal; expectations are form-specific. */
  bodySubsectionDefs: SubsectionDef[];

  /** CLOSING subsections with compiled regexes — ALL 8 CLOSING sections, universal.
   *  Zone markers (Cv, Ce, X1, etc.) compiled from CLOSING_DISPLAY_TAGS.
   *  Tags use Title Case matching section display names for normalizeTag() alignment. */
  closingSubsectionDefs: SubsectionDef[];

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

  // ── Typing maps (arrow refinement: -module->utility) ────────────

  /** Typing maps per form: form name → typing name → profile.
   *  Used when pragma declares -<form>-><typing> arrow syntax.
   *  Narrows section expectations from generic form to specific variant. */
  typingMaps: Record<string, Record<string, TypingProfile>>;

  // ── Fill content (schema-driven file generation) ──────────────────

  /** Fill content for schema-driven file generation. Undefined if schema lacks fill_content. */
  fillContent?: SchemaFillContent;
}

// ---------------------------------------------------------------------------
// 3. Constants
// ---------------------------------------------------------------------------

/** Supported format identifiers. */
export type CodeFormat = "go" | "rust";

// ---------------------------------------------------------------------------
// 3b. Tag → label conversion (data layer kebab-case → display PascalCase)
// ---------------------------------------------------------------------------

/**
 * Convert a kebab-case tag to a display label.
 *
 * "core-types" → "Core Types"
 * "trait-implementations" → "Trait Implementations"
 * "imports" → "Imports"
 * "org-chart" → "Org Chart"
 * "public-apis" → "Public APIs"
 */
const KNOWN_ACRONYMS: Record<string, string> = {
  "apis": "APIs",
  "api": "API",
  "io": "IO",
  "id": "ID",
};

function tagToLabel(tag: string): string {
  return tag
    .split("-")
    .map((word) => {
      const lower = word.toLowerCase();
      return KNOWN_ACRONYMS[lower] ?? (word.charAt(0).toUpperCase() + word.slice(1));
    })
    .join(" ");
}

/**
 * Known alternative names for subsection headers.
 *
 * When a handler matches a subsection header in source code, it needs
 * to recognize both the canonical name AND common alternatives.
 * These come from real-world usage patterns across Go and Rust files.
 *
 * Key: SECTION_REGISTRY tag (kebab-case)
 * Value: Alternative display names (not including canonical)
 */
const ALT_PATTERNS: Record<string, string[]> = {
  // SETUP alternatives
  "imports": ["Dependencies", "External Imports"],
  "modules": ["Module Declarations", "Submodules", "Re-exports"],
  "constants": ["Const", "Constant Definitions"],
  "statics": ["Static Variables", "Package Variables"],
  "type-aliases": ["Type Definitions", "Aliases"],
  "error-types": ["Errors", "Error Definitions", "Custom Errors"],
  "core-types": ["Types", "Structures", "Structs", "Data Types"],
  "trait-defs": ["Traits", "Trait Definitions"],
  "macros": ["Macro Definitions"],
  "feature-gates": ["Feature Flags", "Conditional Compilation"],
  "variables": ["Vars", "Variable Declarations", "Package Variables"],
  "interface-defs": ["Interfaces", "Interface Definitions"],
  "type-methods": ["Methods", "Type Methods"],
  "code-generation": ["Code Gen", "Generate"],
  "build-tags": ["Build Constraints"],

  // BODY alternatives
  "org-chart": ["Block Overview", "Organization", "Module Overview"],
  "identity-access": ["Identity", "Accessors", "Identity Accessors"],
  "trait-implementations": ["Implementations", "Impl Blocks", "Trait Impls"],
  "constructors": ["Constructor", "Builders", "New Functions"],
  "core-logic": ["Core", "Logic", "Core Operations"],
  "queries": ["Query Methods", "Getters", "Observers"],
  "output-display": ["Output", "Display", "Formatting"],
  "free-functions": ["Functions", "Free Functions", "Utilities"],
  "helpers": ["Helper Functions", "Support Functions"],
  "tests": ["Test Functions", "Unit Tests"],
  "core-operations": ["Operations", "Business Logic"],
  "error-handling": ["Error Recovery", "Error Processing"],
  "public-apis": ["Public API", "Exported Functions", "API Surface"],
};

/**
 * Closing zone display tags keyed by SECTION_REGISTRY tag.
 *
 * The closing zones in source files use short display tags (Cv, Ce, Cc, X1-X5)
 * for zone header matching. SECTION_REGISTRY uses kebab-case tags.
 * This bridges between them.
 */
const CLOSING_DISPLAY_TAGS: Record<string, string> = {
  "validation": "Cv",
  "execution": "Ce",
  "cleanup": "Cc",
  "modification-policy": "X1",
  "extension-points": "X2",
  "troubleshooting": "X3",
  "reference": "X4",
  "closing-note": "X5",
  // X6 is NOT in SECTION_REGISTRY (template-only) but must be detectable
  // for checkClosingX6TemplateOnly to flag it in derived files.
  "template-guide": "X6",
};

/**
 * Closing zone kind — code or doc — keyed by SECTION_REGISTRY tag.
 */
const CLOSING_ZONE_KIND: Record<string, "code" | "doc"> = {
  "validation": "code",
  "execution": "code",
  "cleanup": "code",
  "modification-policy": "doc",
  "extension-points": "doc",
  "troubleshooting": "doc",
  "reference": "doc",
  "closing-note": "doc",
  "template-guide": "doc",
};

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
// 2. Regex compilation — zone + field patterns
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

// ---------------------------------------------------------------------------
// 3. Fill content extraction (used by assembleFillContent bridge)
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

// ---------------------------------------------------------------------------
// 6. Multi-file assembler — data layer + split schemas → compiled rules
// ---------------------------------------------------------------------------

/**
 * Assemble Code4BlockRules from the data layer (SECTION_REGISTRY, FORM_REGISTRY)
 * and split schema files (IC fields, doc comments, subtypes, placement maps).
 *
 * **This replaces the old parseCodeSchema().** Instead of reading one monolithic
 * schema file and extracting everything, we:
 * - Read SETUP/BODY/CLOSING sections from SECTION_REGISTRY (compiled data)
 * - Read form structures from FORM_REGISTRY (compiled data)
 * - Read IC field requirements from R25_blocks/metadata/ split schemas
 * - Read doc comment expectations from R25_blocks/metadata/doc-comments/
 * - Read subtype definitions from R25_blocks/metadata/subtypes/
 * - Read placement maps from code/{format}.jsonc
 * - Read fill content from archive monolithic schema (bridge until split)
 *
 * The result is identical to what parseCodeSchema produced — same
 * Code4BlockRules interface. Handlers don't change.
 *
 * @param format - "go" or "rust"
 * @returns Compiled handler-ready rules
 */
export async function assembleCodeRules(format: CodeFormat): Promise<Code4BlockRules> {
  const pipeline = getDefaultPipeline();

  // ── SETUP data from SECTION_REGISTRY ───────────────────────────

  const setupTags = SECTION_ORDER.setup;
  const setupData: SchemaSubsectionDef[] = setupTags.map((tag) => {
    const entry = SECTION_REGISTRY[tag]!;
    const label = tagToLabel(tag);
    const altPatterns = ALT_PATTERNS[tag] ?? [];
    return {
      position: typeof entry.position === "number" ? entry.position : parseInt(String(entry.position), 10),
      tag: label,  // Handler expects display label as "tag" (historical)
      pattern: label,
      altPatterns,
      purpose: entry.description,
      productionNote: "",
      requiredIn: Object.entries(entry.formStatus)
        .filter(([, status]) => status === "required")
        .map(([form]) => form),
    };
  });

  // Compile SETUP subsection regexes
  const setupSubsections: SubsectionDef[] = setupData.map((s) => ({
    tag: s.tag,
    pattern: buildSubsectionPattern(s.pattern, s.altPatterns),
    aliases: buildAliasPattern(s.altPatterns),
  }));

  // ── BODY data from SECTION_REGISTRY + FORM_REGISTRY ──────────

  const bodyTags = SECTION_ORDER.body;
  const bodyData: Record<string, SchemaBodySubtype> = {};
  const bodySubsections: Record<string, readonly string[]> = {};

  // For each form, collect the BODY sections that are required or available
  for (const [formName, formDef] of Object.entries(FORM_REGISTRY)) {
    const subsections: SchemaSubsectionDef[] = [];

    for (const tag of bodyTags) {
      const entry = SECTION_REGISTRY[tag]!;
      const formStatus = entry.formStatus[formName];
      // Include required and available sections (not reserved)
      if (formStatus === "required" || formStatus === "available") {
        const label = tagToLabel(tag);
        const altPatterns = ALT_PATTERNS[tag] ?? [];
        subsections.push({
          position: typeof entry.position === "number"
            ? entry.position
            : parseInt(String(entry.position), 10),
          tag: label,
          pattern: label,
          altPatterns,
          purpose: entry.description,
          productionNote: "",
          requiredIn: Object.entries(entry.formStatus)
            .filter(([, status]) => status === "required")
            .map(([form]) => form),
        });
      }
    }

    subsections.sort((a, b) => a.position - b.position);

    bodyData[formName] = {
      canonicalCount: subsections.length,
      orderingPrinciple: formDef.ordering ?? "unpositioned",
      note: formDef.description,
      subsections,
    };
    bodySubsections[formName] = subsections.map((s) => s.pattern);
  }

  // ── Placement rules from SECTION_REGISTRY ─────────────────────

  const mustBeInSetup: string[] = [];
  const mustNotBeInSetup: string[] = [];
  for (const tag of bodyTags) {
    const label = tagToLabel(tag);
    mustNotBeInSetup.push(label);
  }
  for (const tag of setupTags) {
    const label = tagToLabel(tag);
    mustBeInSetup.push(label);
  }

  // ── Subtype emphasis from FORM_REGISTRY + form schemas ───────

  const subtypeEmphasis: {
    setup: Record<string, { heavy: string[]; light: string[] }>;
    body: Record<string, { heavy: string[]; light: string[] }>;
  } = { setup: {}, body: {} };

  for (const formName of Object.keys(FORM_REGISTRY)) {
    // Try loading the form structure schema for emphasis data
    try {
      const formSchemaId = `code/forms/${formName}/_structure.jsonc`;
      const formText = await pipeline.getText(formSchemaId);
      // deno-lint-ignore no-explicit-any
      const formData = parseJsonc(formText) as Record<string, any>;
      // deno-lint-ignore no-explicit-any
      const form = formData?.["form"] as Record<string, any> | undefined;

      if (form) {
        // CLOSING emphasis (the only place emphasis is defined in form schemas)
        // deno-lint-ignore no-explicit-any
        const closingBlock = form["CLOSING"] as Record<string, any> | undefined;
        // deno-lint-ignore no-explicit-any
        const emphasis = closingBlock?.["emphasis"] as Record<string, any> | undefined;
        if (emphasis) {
          // Body emphasis from BODY block
          // deno-lint-ignore no-explicit-any
          const bodyBlock = form["BODY"] as Record<string, any> | undefined;
          // deno-lint-ignore no-explicit-any
          const bodyEmphasis = bodyBlock?.["emphasis"] as Record<string, any> | undefined;
          if (bodyEmphasis) {
            const toLabel = (s: string) => s.replace(/([a-z])([A-Z])/g, "$1 $2");
            subtypeEmphasis.body[formName] = {
              heavy: Array.isArray(bodyEmphasis["heavy"]) ? bodyEmphasis["heavy"].map(toLabel) : [],
              light: Array.isArray(bodyEmphasis["light"]) ? bodyEmphasis["light"].map(toLabel) : [],
            };
          }

          // Setup emphasis from SETUP block
          // deno-lint-ignore no-explicit-any
          const setupBlock = form["SETUP"] as Record<string, any> | undefined;
          // deno-lint-ignore no-explicit-any
          const setupEmphasis = setupBlock?.["emphasis"] as Record<string, any> | undefined;
          if (setupEmphasis) {
            // Form schemas use PascalCase ("ErrorTypes"), SubsectionDef tags
            // use tagToLabel format ("Error Types"). Bridge the naming convention.
            const toLabel = (s: string) => s.replace(/([a-z])([A-Z])/g, "$1 $2");
            subtypeEmphasis.setup[formName] = {
              heavy: Array.isArray(setupEmphasis["heavy"]) ? setupEmphasis["heavy"].map(toLabel) : [],
              light: Array.isArray(setupEmphasis["light"]) ? setupEmphasis["light"].map(toLabel) : [],
            };
          }
        }
      }
    } catch {
      // Form schema not found — that's fine, emphasis defaults to empty
    }
  }

  // ── Typing maps from form schemas (arrow refinement) ─────────

  const typingMaps: Record<string, Record<string, TypingProfile>> = {};

  for (const formName of Object.keys(FORM_REGISTRY)) {
    try {
      const formSchemaId = `code/forms/${formName}/_structure.jsonc`;
      const formText = await pipeline.getText(formSchemaId);
      // deno-lint-ignore no-explicit-any
      const formData = parseJsonc(formText) as Record<string, any>;
      // deno-lint-ignore no-explicit-any
      const form = formData?.["form"] as Record<string, any> | undefined;
      // deno-lint-ignore no-explicit-any
      const rawMap = form?.["typing_map"] as Record<string, any> | undefined;

      if (rawMap) {
        const formTypings: Record<string, TypingProfile> = {};
        // PascalCase → display label (e.g., "CoreLogic" → "Core Logic")
        const toLabel = (s: string) => s.replace(/([a-z])([A-Z])/g, "$1 $2");

        for (const [typingName, typingData] of Object.entries(rawMap)) {
          // Skip metadata keys
          if (typingName.startsWith("_")) continue;
          // deno-lint-ignore no-explicit-any
          const td = typingData as Record<string, any>;
          // deno-lint-ignore no-explicit-any
          const setupBlock = td["SETUP"] as { required?: string[]; available?: string[]; irrelevant?: string[]; concept_overrides?: Record<string, Record<string, string>> } | undefined;
          // deno-lint-ignore no-explicit-any
          const bodyBlock = td["BODY"] as { required?: string[]; available?: string[]; irrelevant?: string[]; concept_overrides?: Record<string, Record<string, string>> } | undefined;

          if (setupBlock || bodyBlock) {
            formTypings[typingName] = {
              description: String(td["description"] ?? ""),
              SETUP: {
                required: (setupBlock?.required ?? []).map(toLabel),
                available: (setupBlock?.available ?? []).map(toLabel),
                irrelevant: (setupBlock?.irrelevant ?? []).map(toLabel),
                conceptOverrides: setupBlock?.concept_overrides,
              },
              BODY: {
                required: (bodyBlock?.required ?? []).map(toLabel),
                available: (bodyBlock?.available ?? []).map(toLabel),
                irrelevant: (bodyBlock?.irrelevant ?? []).map(toLabel),
                conceptOverrides: bodyBlock?.concept_overrides,
              },
            };
          }
        }

        if (Object.keys(formTypings).length > 0) {
          typingMaps[formName] = formTypings;
        }
      }
    } catch {
      // Form schema not found or has no typing_map — that's fine
    }
  }

  // ── Placement maps from code/{format}.jsonc ──────────────────

  const placementMaps = await assemblePlacementMaps(format, pipeline);

  // ── IC field requirements from R25_blocks/metadata/ ──────────

  const pragmaFieldRequirements = await assembleFieldRequirements(
    "pragma-identity", ["i1-core", "i2-family", "i3-instance", "i4-architecture"],
    ["I1", "I2", "I3", "I4"], pipeline,
  );

  const metadataFieldRequirements = await assembleFieldRequirements(
    "metadata-context",
    ["c1-state", "c2-attribution", "c3-grounding", "c4-dependencies", "c5-intent", "c6-roadmap", "c7-classification"],
    ["C1", "C2", "C3", "C4", "C5", "C6", "C7"], pipeline,
  );

  // ── Doc comment expectations from R25_blocks/metadata/doc-comments/{format}.jsonc ─

  const docCommentExpectations = await assembleDocComments(format, pipeline);

  // ── Subtype definitions from R25_blocks/metadata/subtypes/{subtype}/{format}.jsonc ─

  const subtypeDefinitions = await assembleSubtypeDefs(format, pipeline);

  // ── CLOSING data from SECTION_REGISTRY + zone type mapping ───

  const closingData = assembleClosingData();

  // ── Fill content from archive monolithic schema (bridge) ──────

  const fillContent = await assembleFillContent(format, pipeline);

  // ── BODY subsection defs — ALL 13 sections, universal detection ────
  // Same compilation pattern as SETUP: schema → SubsectionDef[] → getSubsectionRanges().
  // Detection is universal (all sections compiled); form filtering is separate.
  const bodySubsectionDefs: SubsectionDef[] = SECTION_ORDER.body.map((tag) => {
    const label = tagToLabel(tag);
    const altPatterns = ALT_PATTERNS[tag] ?? [];
    return {
      tag: label,
      pattern: buildSubsectionPattern(label, altPatterns),
      aliases: buildAliasPattern(altPatterns),
    };
  });

  // ── CLOSING subsection defs — ALL 8 zones, universal detection ─────
  // Closing zones use display tags (Cv, Ce, X1) in source code but section
  // registry uses kebab-case (validation, execution). SubsectionDef tags use
  // Title Case labels for normalizeTag() alignment with both conventions.
  const closingSubsectionDefs: SubsectionDef[] = Object.entries(CLOSING_DISPLAY_TAGS)
    .filter(([registryTag]) => CLOSING_ZONE_KIND[registryTag] !== undefined)
    .map(([registryTag, displayTag]) => ({
      tag: tagToLabel(registryTag),
      pattern: buildZonePattern(displayTag, CLOSING_ZONE_KIND[registryTag]!),
    }));

  // ── Assemble ──────────────────────────────────────────────────

  return {
    setupSubsections,
    bodySubsections,
    bodySubsectionDefs,
    closingSubsectionDefs,
    placementRules: { mustBeInSetup, mustNotBeInSetup },
    setupData,
    bodyData,
    subtypeEmphasis,
    placementMaps,
    pragmaFieldRequirements,
    metadataFieldRequirements,
    docCommentExpectations,
    subtypeDefinitions,
    closingData,
    typingMaps,
    fillContent,
  };
}

// ---------------------------------------------------------------------------
// 6a. Sub-assemblers — each populates one field of Code4BlockRules
// ---------------------------------------------------------------------------

/**
 * Assemble placement maps from code/{format}.jsonc.
 */
async function assemblePlacementMaps(
  format: CodeFormat,
  pipeline: { getText(id: string): Promise<string> },
): Promise<PlacementMaps> {
  try {
    const text = await pipeline.getText(`code/R50_codefile/languages/${format}/${format}.jsonc`);
    // deno-lint-ignore no-explicit-any
    const data = parseJsonc(text) as Record<string, any>;
    // deno-lint-ignore no-explicit-any
    const kinds = data?.["content_kinds"] as Record<string, any> | undefined;
    if (!kinds) return { blockPlacement: {}, subsectionPlacement: {}, metadataForbidden: [] };

    const blockPlacement: Record<string, string> = {};
    const subsectionPlacement: Record<string, string> = {};

    for (const [kind, def] of Object.entries(kinds)) {
      if (typeof def !== "object" || !def || kind === "note") continue;
      // deno-lint-ignore no-explicit-any
      const d = def as Record<string, any>;
      if (typeof d["block"] === "string") blockPlacement[kind] = d["block"];
      if (typeof d["subsection"] === "string") subsectionPlacement[kind] = d["subsection"];
    }

    // Metadata forbidden
    // deno-lint-ignore no-explicit-any
    const mfData = data?.["metadata_forbidden"] as Record<string, any> | undefined;
    const metadataForbidden: string[] = Array.isArray(mfData?.["kinds"])
      ? mfData!["kinds"].filter((k: unknown) => typeof k === "string")
      : [];

    return { blockPlacement, subsectionPlacement, metadataForbidden };
  } catch {
    return { blockPlacement: {}, subsectionPlacement: {}, metadataForbidden: [] };
  }
}

/**
 * Assemble IC field requirements from R25_blocks/metadata/{parent}/{dirs}/_structure.jsonc.
 *
 * Reads each sub-container's fields to determine required/defined lists.
 */
async function assembleFieldRequirements(
  parentDir: string,
  subDirs: string[],
  sectionKeys: string[],
  pipeline: { getText(id: string): Promise<string> },
): Promise<Record<string, SchemaFieldRequirement>> {
  const result: Record<string, SchemaFieldRequirement> = {};

  for (let i = 0; i < subDirs.length; i++) {
    const dir = subDirs[i]!;
    const sectionKey = sectionKeys[i]!;

    try {
      const schemaId = `code/R25_blocks/metadata/${parentDir}/${dir}/_structure.jsonc`;
      const text = await pipeline.getText(schemaId);
      // deno-lint-ignore no-explicit-any
      const data = parseJsonc(text) as Record<string, any>;
      // deno-lint-ignore no-explicit-any
      const subContainer = data?.["sub_container"] as Record<string, any> | undefined;
      if (!subContainer) continue;

      // deno-lint-ignore no-explicit-any
      const fields = subContainer["fields"] as Record<string, any> | undefined;
      if (!fields) continue;

      const required: string[] = [];
      const defined: string[] = [];

      for (const [fieldName, fieldDef] of Object.entries(fields)) {
        if (typeof fieldDef !== "object" || !fieldDef) continue;
        // deno-lint-ignore no-explicit-any
        const fd = fieldDef as Record<string, any>;
        if (fd["required"] === true) required.push(fieldName);
        if (fd["defined"] === true) defined.push(fieldName);
      }

      const purpose = typeof subContainer["purpose"] === "string"
        ? subContainer["purpose"] : "";

      result[sectionKey] = { required, defined, purpose };
    } catch {
      // Sub-container schema not found — skip gracefully
    }
  }

  return result;
}

/**
 * Assemble doc comment expectations from R25_blocks/metadata/doc-comments/{format}.jsonc.
 */
async function assembleDocComments(
  format: CodeFormat,
  pipeline: { getText(id: string): Promise<string> },
): Promise<Record<string, SchemaDocComment>> {
  try {
    const schemaId = `code/R25_blocks/metadata/doc-comments/${format}.jsonc`;
    const text = await pipeline.getText(schemaId);
    // deno-lint-ignore no-explicit-any
    const data = parseJsonc(text) as Record<string, any>;
    // deno-lint-ignore no-explicit-any
    const content = data?.["content"] as Record<string, any> | undefined;
    if (!content) return {};

    const result: Record<string, SchemaDocComment> = {};

    // Extract severity section — Go has package_doc, pragma_doc, metadata_doc
    // deno-lint-ignore no-explicit-any
    const severity = content["severity"] as Record<string, any> | undefined;
    if (severity) {
      for (const [key, value] of Object.entries(severity)) {
        if (key === "note" || typeof value !== "string") continue;
        result[key] = {
          purpose: `Doc comment: ${key}`,
          severity: value,
        };
      }
    }

    // Rust flat pattern: crate_root_severity/module_severity directly
    if (Object.keys(result).length === 0) {
      // deno-lint-ignore no-explicit-any
      const rustContent = data as Record<string, any>;
      if (typeof rustContent["crate_root_severity"] === "string") {
        result["crate_root_docs"] = {
          purpose: typeof rustContent["purpose"] === "string" ? rustContent["purpose"] : "",
          severity: rustContent["crate_root_severity"],
          format: "//!",
          location: typeof rustContent["location"] === "string" ? rustContent["location"] : undefined,
        };
      }
      if (typeof rustContent["module_severity"] === "string") {
        result["module_docs"] = {
          purpose: typeof rustContent["purpose"] === "string" ? rustContent["purpose"] : "",
          severity: rustContent["module_severity"],
          format: "//!",
          location: typeof rustContent["location"] === "string" ? rustContent["location"] : undefined,
        };
      }
    }

    return result;
  } catch {
    return {};
  }
}

/**
 * Assemble subtype definitions from R25_blocks/metadata/subtypes/{subtype}/{format}.jsonc.
 */
async function assembleSubtypeDefs(
  format: CodeFormat,
  pipeline: { getText(id: string): Promise<string> },
): Promise<Record<string, SchemaSubtypeDef>> {
  const result: Record<string, SchemaSubtypeDef> = {};

  // Known subtypes from FORM_REGISTRY (excluding bare-bone — it's the foundation, not a subtype)
  const subtypes = Object.keys(FORM_REGISTRY).filter((name) => name !== "bare-bone");

  for (const subtype of subtypes) {
    try {
      const schemaId = `code/R25_blocks/metadata/subtypes/${subtype}/${format}.jsonc`;
      const text = await pipeline.getText(schemaId);
      // deno-lint-ignore no-explicit-any
      const data = parseJsonc(text) as Record<string, any>;
      // Go schemas use "sub_container", Rust schemas use "content" — check both
      // deno-lint-ignore no-explicit-any
      const subContainer = (data?.["sub_container"] ?? data?.["content"]) as Record<string, any> | undefined;
      if (!subContainer) continue;

      result[subtype] = {
        subtypeValue: typeof subContainer["I2_subtype"] === "string"
          ? subContainer["I2_subtype"] : subtype,
        filePattern: typeof subContainer["file_pattern"] === "string"
          ? subContainer["file_pattern"] : "",
        pragmaDirective: typeof subContainer["pragma_directive"] === "string"
          ? subContainer["pragma_directive"] : "",
        templateDirective: typeof subContainer["template_directive"] === "string"
          ? subContainer["template_directive"] : "",
        hasPublicApi: subContainer["has_public_api"] === true,
        hasTestsBlock: subContainer["has_tests_block"] === true ? true : undefined,
        packagePattern: typeof subContainer["package"] === "string"
          ? subContainer["package"] : undefined,
        purpose: typeof subContainer["purpose"] === "string"
          ? subContainer["purpose"] : "",
      };
    } catch {
      // Subtype schema not found for this format — skip gracefully
    }
  }

  return result;
}

/**
 * Assemble closing data from SECTION_REGISTRY + display tag mapping.
 *
 * The SECTION_REGISTRY closing sections have the metadata. The display tag
 * mapping provides the short tags (Cv, Ce, X1) used in zone header regex.
 * The zone type (code vs doc) comes from CLOSING_ZONE_KIND.
 *
 * For doc zone field requirements, we read the _structure.jsonc for
 * each doc zone to get the "fields" block. Currently these are
 * hardcoded in the CLOSING_DISPLAY_TAGS and CLOSING_ZONE_KIND maps
 * which are derived from the schema _structure.jsonc files.
 */
function assembleClosingData(): SchemaClosingData {
  const zones: SchemaClosingZone[] = [];
  const docRequirements: SchemaClosingDocReq[] = [];
  const fieldPatterns: Record<string, Record<string, RegExp>> = {};

  // Include SECTION_REGISTRY closing tags + template-guide (X6).
  // X6 is template-only (not in SECTION_REGISTRY) but must be detectable
  // for X6-template-only check in derived files.
  const closingTags = [
    ...SECTION_ORDER.closing,
    "template-guide",
  ];

  for (const tag of closingTags) {
    const displayTag = CLOSING_DISPLAY_TAGS[tag];
    if (!displayTag) continue;

    const kind = CLOSING_ZONE_KIND[tag] ?? "doc";

    zones.push({
      tag: displayTag,
      kind: kind === "code" ? "code" : "doc",
      pattern: buildZonePattern(displayTag, kind === "code" ? "code" : "doc"),
    });

    // Doc zone requirements
    if (kind === "doc") {
      const entry = SECTION_REGISTRY[tag];
      const isRequired = entry
        ? Object.values(entry.formStatus).some((s) => s === "required")
        : false;

      // X6 is template-only (not in our 8 closing sections)
      const templateOnly = displayTag === "X6" ? true : undefined;

      // X1 has known fields: never, careful, safe
      // X5 has known fields: note, scripture
      // Other doc zones: no specific field patterns
      const req: SchemaClosingDocReq = {
        tag: displayTag,
        required: isRequired,
        templateOnly,
      };

      if (displayTag === "X1") {
        req.fields = {
          required: ["never", "careful", "safe"],
          defined: [],
        };
        const patterns: Record<string, RegExp> = {};
        for (const f of req.fields.required) {
          patterns[f] = buildFieldPattern(f);
        }
        fieldPatterns[displayTag] = patterns;
      } else if (displayTag === "X5") {
        req.fields = {
          required: ["note", "scripture"],
          defined: [],
        };
        const patterns: Record<string, RegExp> = {};
        for (const f of req.fields.required) {
          patterns[f] = buildFieldPattern(f);
        }
        fieldPatterns[displayTag] = patterns;
      }

      docRequirements.push(req);
    }
  }

  return { zones, docRequirements, fieldPatterns };
}

/**
 * Load fill content from archive monolithic schema as a bridge.
 *
 * Fill content (for CREATE/TRANSFORM) hasn't been split into the new
 * schema format yet. Read it from the archived monolithic schema.
 * When the split is complete, this function gets replaced.
 */
async function assembleFillContent(
  format: CodeFormat,
  pipeline: { getText(id: string): Promise<string> },
): Promise<SchemaFillContent | undefined> {
  try {
    const archiveId = `code/_archive/format/${format}-4block-schema.jsonc`;
    const text = await pipeline.getText(archiveId);
    // deno-lint-ignore no-explicit-any
    const parsed = parseJsonc(text) as Record<string, any>;
    // deno-lint-ignore no-explicit-any
    const blocks = (parsed[`${format}_structure`] ?? parsed["blocks"]) as Record<string, any> | undefined;
    return extractFillContent(blocks);
  } catch {
    // Archive not available — fill content is optional
    return undefined;
  }
}

// ---------------------------------------------------------------------------
// 7. I/O loader — assembles from data layer + split schemas
// ---------------------------------------------------------------------------

/**
 * Load and assemble Code4BlockRules for a code format.
 *
 * Uses the multi-file assembler: data layer (SECTION_REGISTRY, FORM_REGISTRY)
 * provides the structural backbone, split schema files provide format-specific
 * data (IC fields, doc comments, subtypes, placement maps).
 *
 * Result is cached — subsequent calls return cache.
 *
 * @param format - "go" or "rust"
 * @returns Derived rules compiled for handler use
 */
export async function loadCodeRules(format: CodeFormat): Promise<Code4BlockRules> {
  const cached = caches.get(format);
  if (cached) return cached;

  const rules = await assembleCodeRules(format);
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

registerCache("code-schema/rules", clearCodeCache);
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
 * Schema ID mapping (category="code"):
 *   "bare-bone" → tries "code/forms/bare-bone/{format}.jsonc" then "code/forms/bare-bone/{format}-bare-bone.jsonc"
 *   all others  → tries "code/forms/{subtype}/{format}.jsonc" then "code/forms/declared/{format}-{subtype}.jsonc"
 *
 * Schema ID mapping (category="data"):
 *   "bare-bone" → "data/forms/bare-bone/{format}-bare-bone.jsonc"
 *   all others  → "data/forms/declared/{format}-{subtype}.jsonc"
 */
export async function loadFormConstraints(
  format: string,
  subtype: string,
  category: "code" | "data" = "code",
): Promise<FormConstraints | null> {
  const cacheKey = `${category}:${format}:${subtype}`;
  if (formCache.has(cacheKey)) return formCache.get(cacheKey)!;

  // Build schema ID — try the canonical path first, then the legacy flat path
  // Canonical: code/forms/{subtype}/{format}.jsonc  (e.g. code/forms/module/rust.jsonc)
  // Legacy:    code/forms/declared/{format}-{subtype}.jsonc  (e.g. code/forms/declared/rust-module.jsonc)
  const canonicalId = subtype === "bare-bone"
    ? `${category}/forms/bare-bone/${format}.jsonc`
    : `${category}/forms/${subtype}/${format}.jsonc`;
  const legacyId = subtype === "bare-bone"
    ? `${category}/forms/bare-bone/${format}-bare-bone.jsonc`
    : `${category}/forms/declared/${format}-${subtype}.jsonc`;
  const schemaId = canonicalId;  // try canonical first

  try {
    const pipeline = getDefaultPipeline();
    let text: string | undefined;
    try {
      text = await pipeline.getText(schemaId);
    } catch {
      // Canonical path not found — try legacy path
      try {
        text = await pipeline.getText(legacyId);
      } catch {
        // Neither path exists
      }
    }
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

registerCache("code-schema/forms", clearFormCache);
// ---------------------------------------------------------------------------
// 7b. Registry-to-FormConstraints Fallback
// ---------------------------------------------------------------------------
//
// When form constraint schemas (parseFormSchema) and composition targets
// (compositionToFormConstraints) are both unavailable, the SECTION_REGISTRY
// can derive FormConstraints directly. The registry IS the single source
// of truth for section ordering and form status.
//
// This ensures the scaffold transformer always has form-aware output —
// numbered section headers, block overviews, Reserved Omission grouping —
// even when the format doesn't have specialized form constraint schemas.

/**
 * Build FormConstraints from SECTION_REGISTRY for any format/form.
 *
 * Uses SECTION_REGISTRY formStatus to classify sections as
 * REQUIRED, AVAILABLE, or reserved for the given form. Positions
 * and labels come directly from the registry.
 *
 * @param form - Form name (e.g., "module", "library", "bare-bone")
 * @returns FormConstraints ready for scaffold consumption
 */
export function buildFormConstraintsFromRegistry(
  form: string,
): FormConstraints {
  const formDef = FORM_REGISTRY[form];

  function buildBlockConstraints(block: "setup" | "body" | "closing"): FormContainerConstraints {
    const tags = SECTION_ORDER[block];
    const can: FormSectionConstraint[] = [];
    const cannot: FormReservedSection[] = [];

    for (const tag of tags) {
      const entry = SECTION_REGISTRY[tag];
      if (!entry) continue;

      const status = entry.formStatus[form];
      const label = tagToLabel(tag);
      const pos = typeof entry.position === "number"
        ? entry.position
        : parseInt(String(entry.position).replace("X", ""), 10) + 100;

      if (status === "required") {
        can.push({ position: pos, tag: label, status: "REQUIRED" });
      } else if (status === "available") {
        can.push({ position: pos, tag: label, status: "AVAILABLE" });
      } else if (status === "reserved") {
        // Figure out which forms this IS active in
        const activeIn = Object.entries(entry.formStatus)
          .filter(([, s]) => s === "required" || s === "available")
          .map(([f]) => f)
          .join(", ");
        cannot.push({
          tag: label,
          whyReserved: `Reserved for ${form}`,
          activeIn,
          position: pos,
        });
      }
    }

    can.sort((a, b) => a.position - b.position);
    return { can, cannot };
  }

  return {
    name: form,
    isFoundation: form === "bare-bone",
    inherits: form === "bare-bone" ? undefined : "bare-bone",
    ordering: formDef?.ordering ?? "type_lifecycle",
    SETUP: buildBlockConstraints("setup"),
    BODY: buildBlockConstraints("body"),
    CLOSING: buildBlockConstraints("closing"),
  };
}

// ---------------------------------------------------------------------------
// 8. Composition-to-FormConstraints Bridge
// ---------------------------------------------------------------------------
//
// The three-cord composition targets (index layer) have active_sections
// with required/available/reserved string arrays per block. The scaffold
// reads FormConstraints with position/tag/status objects. This bridge
// converts between the two — adapter pattern, no scaffold changes needed.
//
// Bridge pattern: composition data flows IN, FormConstraints flows OUT.
// The scaffold doesn't know or care where its constraints come from.
//

import type {
  CompositionActiveSections,
  CompositionBlockSections,
} from "./composition-loader.ts";

/**
 * Convert composition target active_sections to FormConstraints.
 *
 * Bridges the three-cord composition system (cord 3: index) to the
 * handler's existing FormConstraints interface. The scaffold already knows
 * how to read FormConstraints — this adapter lets it read from composition
 * targets without any scaffold changes.
 *
 * Positions are resolved from Code4BlockRules schema data (SETUP from
 * setupData, BODY from bodyData[form], CLOSING from closingData.zones).
 * When a tag doesn't appear in the schema, position falls back to array
 * order — degraded but functional.
 *
 * @param activeSections - The composition target's active_sections
 * @param rules - Loaded Code4BlockRules for position resolution
 * @param form - Form name (e.g., "module", "library") for body subtype lookup
 * @returns FormConstraints ready for scaffold consumption
 */
export function compositionToFormConstraints(
  activeSections: CompositionActiveSections,
  rules: Code4BlockRules,
  form: string,
): FormConstraints {
  return {
    name: form,
    isFoundation: false,
    ordering: activeSections.BODY.ordering ?? "type_lifecycle",
    SETUP: bridgeBlock(
      activeSections.SETUP,
      rules.setupData,
    ),
    BODY: bridgeBlock(
      activeSections.BODY,
      rules.bodyData[form]?.subsections ?? [],
    ),
    CLOSING: bridgeClosingBlock(
      activeSections.CLOSING,
      rules.closingData,
    ),
  };
}

/**
 * Bridge a single block's composition sections to FormContainerConstraints.
 *
 * Maps required[] → can[] with status "REQUIRED", available[] → can[] with
 * status "AVAILABLE", reserved[] → cannot[]. Positions come from schema
 * subsection data via tag lookup.
 */
function bridgeBlock(
  sections: CompositionBlockSections,
  schemaSections: SchemaSubsectionDef[],
): FormContainerConstraints {
  // Build tag → position lookup from schema.
  // Schema tags use display labels ("Core Types") but composition targets
  // use PascalCase ("CoreTypes"). Index both forms for robust matching.
  const tagPositions = new Map<string, number>();
  for (const s of schemaSections) {
    tagPositions.set(s.tag, s.position);
    // Also index the no-space PascalCase form for schema tag compatibility
    const collapsed = s.tag.replace(/\s+/g, "");
    if (collapsed !== s.tag) {
      tagPositions.set(collapsed, s.position);
    }
  }

  const can: FormSectionConstraint[] = [];

  // Required sections
  for (const tag of sections.required) {
    can.push({
      position: tagPositions.get(tag) ?? can.length + 1,
      tag,
      status: "REQUIRED",
    });
  }

  // Available sections
  for (const tag of sections.available ?? []) {
    can.push({
      position: tagPositions.get(tag) ?? can.length + 1,
      tag,
      status: "AVAILABLE",
    });
  }

  // Sort by position — dependency chain ordering
  can.sort((a, b) => a.position - b.position);

  // Reserved sections → cannot
  const cannot: FormReservedSection[] = sections.reserved.map((tag) => ({
    tag,
    whyReserved: "Reserved in composition target",
    activeIn: "",
    position: tagPositions.get(tag),
  }));

  return { can, cannot };
}

/**
 * Bridge CLOSING block sections to FormContainerConstraints.
 *
 * Closing zones have a different position source — the zone_ordering
 * array from schema closingData. Code zones (Cv, Ce, Cc) and doc zones
 * (X1-X6) are ordered by their index in closingData.zones.
 */
function bridgeClosingBlock(
  sections: CompositionBlockSections,
  closingData: SchemaClosingData,
): FormContainerConstraints {
  // Position from zone ordering index
  const zonePositions = new Map(
    closingData.zones.map((z, i) => [z.tag, i + 1]),
  );

  const can: FormSectionConstraint[] = [];

  for (const tag of sections.required) {
    can.push({
      position: zonePositions.get(tag) ?? can.length + 1,
      tag,
      status: "REQUIRED",
    });
  }

  for (const tag of sections.available ?? []) {
    can.push({
      position: zonePositions.get(tag) ?? can.length + 1,
      tag,
      status: "AVAILABLE",
    });
  }

  can.sort((a, b) => a.position - b.position);

  const cannot: FormReservedSection[] = sections.reserved.map((tag) => ({
    tag,
    whyReserved: "Reserved in composition target",
    activeIn: "",
    position: zonePositions.get(tag),
  }));

  return { can, cannot };
}

/**
 * Load FormConstraints from a composition target.
 *
 * This is the async convenience wrapper that:
 * 1. Loads the composition target from index/targets/
 * 2. Loads the code rules for position resolution
 * 3. Bridges the two into FormConstraints
 *
 * Returns null if no composition target exists for this format+form.
 * Falls back gracefully — callers can try composition first, then
 * fall back to loadFormConstraints().
 *
 * @param format - Language (e.g., "rust", "go")
 * @param form - Form (e.g., "module", "library")
 * @returns FormConstraints from composition target, or null
 */
export async function loadCompositionFormConstraints(
  format: string,
  form: string,
): Promise<FormConstraints | null> {
  try {
    const { loadCompositionCached } = await import("./composition-loader.ts");
    const composition = await loadCompositionCached(format, form);
    const rules = await loadCodeRules(format as CodeFormat);
    return compositionToFormConstraints(
      composition.target.active_sections,
      rules,
      form,
    );
  } catch {
    // No composition target for this format+form — that's fine
    return null;
  }
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
