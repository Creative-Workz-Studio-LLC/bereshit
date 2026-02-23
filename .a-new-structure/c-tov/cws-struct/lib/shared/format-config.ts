// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/shared/format-config.ts
// key:     B-tov-cws-struct-lib-shared-format-config
// title:   CWS Struct — Schema-Driven Format Configuration
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Bridge between schema-driven format rules and the hardcoded constants
//          in shared/types.ts. Provides FormatConfig — a typed object loaded
//          from _format.jsonc schemas that replaces hardcoded widths, directive
//          lists, and boundary patterns.
//
//          Consumers call loadFormatConfig(lang) once at handler init.
//          Check functions receive FormatConfig through context instead of
//          importing constants. Hardcoded values remain as defaults for
//          graceful fallback.
//
// biblical_foundation: "Look that thou make them after their pattern,
//   which was shewed thee in the mount." — Exodus 25:40
//   The schema IS the pattern shown on the mount.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import {
  loadUniversalFormatRules,
  loadLanguageFormatRules,
  clearFormatCache,
} from "../data/mod.ts";
import type { CommentSyntax } from "../data/mod.ts";
import {
  BLOCK_SEPARATOR_WIDTH,
  SUBSECTION_SEPARATOR_WIDTH,
  REQUIRED_DIRECTIVES,
  RECOMMENDED_DIRECTIVES,
} from "./types.ts";

import { registerCache } from "../foundation/cache-registry.ts";
// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// FormatConfig — the schema-driven replacement for hardcoded constants
// ---------------------------------------------------------------------------

/**
 * Schema-driven format configuration.
 *
 * Each field mirrors a hardcoded constant in shared/types.ts, but the value
 * comes from _format.jsonc schemas. When the schema differs from the hardcoded
 * default, gaps are tracked for reconciliation.
 *
 * Consumers receive this through their file context — no more importing
 * BLOCK_SEPARATOR_WIDTH from types.ts.
 */
export interface FormatConfig {
  /** Block separator width (// ====...====). Schema: universal.separators.block.standard_width */
  blockSeparatorWidth: number;

  /** Subsection separator width (// ────...────). Schema: universal.separators.subsection.standard_width */
  subsectionSeparatorWidth: number;

  /** Block separator character. Schema: universal.separators.block.character */
  blockSeparatorChar: string;

  /** Subsection separator character. Schema: universal.separators.subsection.character */
  subsectionSeparatorChar: string;

  /** Required directives as //omni:{name} strings. Schema: universal.pragma.directives[required=true] */
  requiredDirectives: readonly string[];

  /** Recommended directives as //omni:{name} strings. Schema: universal.pragma.directives[recommended=true] */
  recommendedDirectives: readonly string[];

  /** Closing zone order (e.g., "code zones first, then documentation sections"). */
  closingZoneOrder: string;

  /** Code zones in CLOSING (e.g., ["Cv", "Ce", "Cc"]). */
  closingCodeZones: readonly string[];

  /** Documentation sections in CLOSING (e.g., ["X1", ..., "X6"]). */
  closingDocSections: readonly string[];

  /** Language-specific comment syntax (null for universal-only loads). */
  commentSyntax: CommentSyntax | null;

  /** Pragma pattern (shebang format). */
  pragmaPattern: string;

  /** Legacy pragma pattern (//omni: directive format). */
  pragmaLegacyPattern: string;

  /** Gaps detected between schema and handler defaults. */
  gaps: FormatConfigGap[];
}

/** A gap between schema value and hardcoded handler constant. */
export interface FormatConfigGap {
  field: string;
  schemaValue: unknown;
  handlerValue: unknown;
  note: string;
}

// ---------------------------------------------------------------------------
// Default config — the hardcoded values from types.ts as FormatConfig
// ---------------------------------------------------------------------------

/**
 * Default format config using hardcoded values.
 * Used when schema loading fails or isn't available.
 */
export function defaultFormatConfig(): FormatConfig {
  return {
    blockSeparatorWidth: BLOCK_SEPARATOR_WIDTH,
    subsectionSeparatorWidth: SUBSECTION_SEPARATOR_WIDTH,
    blockSeparatorChar: "=",
    subsectionSeparatorChar: "─",
    requiredDirectives: [...REQUIRED_DIRECTIVES],
    recommendedDirectives: [...RECOMMENDED_DIRECTIVES],
    closingZoneOrder: "code zones first, then documentation sections",
    closingCodeZones: ["Cv", "Ce", "Cc"],
    closingDocSections: ["X1", "X2", "X3", "X4", "X5", "X6"],
    commentSyntax: null,
    pragmaPattern: "",
    pragmaLegacyPattern: "",
    gaps: [],
  };
}

// ---------------------------------------------------------------------------
// Loader — schema-driven config with gap detection
// ---------------------------------------------------------------------------

/** Cached configs by language (or "universal" for universal-only). */
const configCache = new Map<string, FormatConfig>();

/**
 * Load format config from schema, detecting gaps against handler defaults.
 *
 * @param lang - Language identifier (e.g., "go", "rust"). If omitted, loads universal only.
 * @returns FormatConfig with schema values and documented gaps.
 */
export async function loadFormatConfig(lang?: string): Promise<FormatConfig> {
  const cacheKey = lang ?? "universal";
  const cached = configCache.get(cacheKey);
  if (cached) return cached;

  try {
    const universal = await loadUniversalFormatRules();
    const gaps: FormatConfigGap[] = [];

    // --- Separator widths ---
    const schemaBlockWidth = universal.separators.block.standard_width;
    const schemaSubWidth = universal.separators.subsection.standard_width;

    if (schemaBlockWidth !== BLOCK_SEPARATOR_WIDTH) {
      gaps.push({
        field: "blockSeparatorWidth",
        schemaValue: schemaBlockWidth,
        handlerValue: BLOCK_SEPARATOR_WIDTH,
        note: `Schema says ${schemaBlockWidth}, handler uses ${BLOCK_SEPARATOR_WIDTH}`,
      });
    }
    if (schemaSubWidth !== SUBSECTION_SEPARATOR_WIDTH) {
      gaps.push({
        field: "subsectionSeparatorWidth",
        schemaValue: schemaSubWidth,
        handlerValue: SUBSECTION_SEPARATOR_WIDTH,
        note: `Schema says ${schemaSubWidth}, handler uses ${SUBSECTION_SEPARATOR_WIDTH}`,
      });
    }

    // --- Directives ---
    const schemaRequired = universal.pragma.directives
      .filter((d) => d.required)
      .map((d) => `//omni:${d.name}`);
    const schemaRecommended = universal.pragma.directives
      .filter((d) => d.recommended)
      .map((d) => `//omni:${d.name}`);

    // Check directive gaps
    const handlerRequired: string[] = [...REQUIRED_DIRECTIVES];
    const handlerRecommended: string[] = [...RECOMMENDED_DIRECTIVES];

    const missingRequired = handlerRequired.filter((d) => !schemaRequired.includes(d));
    const extraRequired = schemaRequired.filter((d) => !handlerRequired.includes(d));

    if (missingRequired.length > 0 || extraRequired.length > 0) {
      gaps.push({
        field: "requiredDirectives",
        schemaValue: schemaRequired,
        handlerValue: handlerRequired,
        note: `Schema: [${schemaRequired.join(", ")}], handler: [${handlerRequired.join(", ")}]`,
      });
    }

    if (JSON.stringify(schemaRecommended.sort()) !== JSON.stringify(handlerRecommended.sort())) {
      gaps.push({
        field: "recommendedDirectives",
        schemaValue: schemaRecommended,
        handlerValue: handlerRecommended,
        note: `Schema recommends [${schemaRecommended.join(", ")}], handler recommends [${handlerRecommended.join(", ")}]`,
      });
    }

    // --- Build config using SCHEMA values (not handler defaults) ---
    const config: FormatConfig = {
      blockSeparatorWidth: schemaBlockWidth,
      subsectionSeparatorWidth: schemaSubWidth,
      blockSeparatorChar: universal.separators.block.character,
      subsectionSeparatorChar: universal.separators.subsection.character,
      requiredDirectives: schemaRequired,
      recommendedDirectives: schemaRecommended,
      closingZoneOrder: universal.closing_zones.order,
      closingCodeZones: universal.closing_zones.code_zones,
      closingDocSections: universal.closing_zones.doc_sections,
      commentSyntax: null,
      pragmaPattern: universal.pragma.pattern,
      pragmaLegacyPattern: universal.pragma.legacy_pattern,
      gaps,
    };

    // --- Add language-specific data ---
    if (lang) {
      try {
        const langRules = await loadLanguageFormatRules(lang);
        config.commentSyntax = langRules.comment_syntax;
      } catch {
        // Language rules not available — use universal only
      }
    }

    configCache.set(cacheKey, config);
    return config;
  } catch {
    // Schema loading failed — return hardcoded defaults
    const fallback = defaultFormatConfig();
    fallback.gaps.push({
      field: "ALL",
      schemaValue: "unavailable",
      handlerValue: "defaults",
      note: "Schema loading failed — using hardcoded defaults",
    });
    return fallback;
  }
}

/**
 * Check if the loaded config has any schema-handler gaps.
 * Useful for verbose output or debug modes.
 */
export function hasFormatGaps(config: FormatConfig): boolean {
  return config.gaps.length > 0;
}

/**
 * Format gap report for display.
 */
export function formatGapReport(config: FormatConfig): string[] {
  return config.gaps.map((g) => `  ${g.field}: ${g.note}`);
}

/** Clear the format config cache (for testing). */
export function clearFormatConfigCache(): void {
  configCache.clear();
  clearFormatCache();
}
registerCache("shared/format-config", clearFormatConfigCache);

// ============================================================================
// CLOSING
// ============================================================================
//
// FormatConfig bridges schema → handler. The schema is the source of truth;
// the handler constants are the fallback. Gap detection ensures we know
// exactly where they diverge. When all gaps are reconciled, the hardcoded
// constants in types.ts can be removed entirely.
//
// "Look that thou make them after their pattern." — Exodus 25:40
// ============================================================================
