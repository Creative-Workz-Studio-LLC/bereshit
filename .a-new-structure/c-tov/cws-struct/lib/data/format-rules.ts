// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/data/format-rules.ts
// key:     B-tov-cws-struct-lib-data-format-rules
// title:   CWS Struct — Format Rules Registry (Schema-Driven)
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Types and loader for formatting rules from R50_codefile schemas.
//          Universal rules come from R50_codefile/universal/_format.jsonc.
//          Per-language overrides come from R50_codefile/languages/{lang}/_format.jsonc.
//
//          These define HOW code files look: separator characters, block boundary
//          patterns, pragma format, identity field sections, closing zones.
//          The linter reads these instead of hardcoding them.
//
// biblical_foundation: "Look that thou make them after their pattern."
//   — Exodus 25:40
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { parse as parseJsonc } from "@std/jsonc";
// Direct import — barrel would create cycle: foundation/mod → code-schema → data/mod → format-rules
import { getDefaultPipeline } from "../foundation/schema-pipeline.ts";
import { registerCache } from "../foundation/cache-registry.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Types — universal formatting
// ---------------------------------------------------------------------------

/** Separator rule (= for blocks, - for subsections). */
export interface SeparatorRule {
  character: string;
  standard_width: number;
  description: string;
}

/** Block boundary pattern definitions. */
export interface BlockBoundaryRules {
  pattern: string;
  title_pattern: string;
  end_pattern: string;
  description: string;
}

/** Pragma directive format. */
export interface PragmaDirectiveRule {
  name: string;
  pattern: string;
  required?: boolean;
  recommended?: boolean;
}

/** Pragma format rules. */
export interface PragmaRules {
  pattern: string;
  legacy_pattern: string;
  directives: PragmaDirectiveRule[];
}

/** Identity field section definitions. */
export interface IdentityFieldRules {
  pragma_sections: string[];
  metadata_sections: string[];
  description: string;
}

/** Closing zone definitions. */
export interface ClosingZoneRules {
  code_zones: string[];
  doc_sections: string[];
  order: string;
  description: string;
}

/** Reserved omission rules. */
export interface ReservedOmissionRules {
  required_in: string[];
  not_required_in: string[];
  marker_pattern: string;
  description: string;
}

/** Universal formatting — loaded from R50_codefile/universal/_format.jsonc. */
export interface UniversalFormatRules {
  resolution: string;
  scope: string;
  separators: {
    block: SeparatorRule;
    subsection: SeparatorRule;
    note: string;
  };
  block_boundaries: BlockBoundaryRules;
  pragma: PragmaRules;
  identity_fields: IdentityFieldRules;
  closing_zones: ClosingZoneRules;
  reserved_omission: ReservedOmissionRules;
}

// ---------------------------------------------------------------------------
// 2. Types — language-specific formatting
// ---------------------------------------------------------------------------

/** Comment syntax for a language. */
export interface CommentSyntax {
  line: string;
  block_open?: string;
  block_close?: string;
  doc_comment?: string | null;
  inner_doc?: string | null;
  note?: string;
}

/** How the pragma is expressed in a specific language. */
export interface PragmaExpression {
  format: string;
  comment_style: string;
  example: string;
}

/** How identity constructs look in a specific language. */
export interface IdentityConstruct {
  type: string;
  example: string;
  note?: string;
}

/** Language-specific format rules — loaded from R50_codefile/languages/{lang}/_format.jsonc. */
export interface LanguageFormatRules {
  language: string;
  inherits: string;
  comment_syntax: CommentSyntax;
  pragma_expression: PragmaExpression;
  identity_constructs: Record<string, IdentityConstruct>;
  /** Language-specific constructs (build_tag, attribute, etc.) */
  language_specific?: Record<string, unknown>;
}

/** Composed format rules — universal + language overlay. */
export interface ComposedFormatRules {
  universal: UniversalFormatRules;
  language: LanguageFormatRules;
}

// ---------------------------------------------------------------------------
// 3. Loader — schema-driven format rules
// ---------------------------------------------------------------------------

const formatCache = new Map<string, ComposedFormatRules>();
let universalCache: UniversalFormatRules | null = null;

/**
 * Load universal format rules from R50_codefile/universal/_format.jsonc.
 *
 * @returns Universal formatting rules
 */
export async function loadUniversalFormatRules(): Promise<UniversalFormatRules> {
  if (universalCache) return universalCache;

  const pipeline = getDefaultPipeline();
  const text = await pipeline.getText("code/R50_codefile/universal/_format.jsonc");
  const data = parseJsonc(text) as Record<string, unknown>;
  const formatting = data["formatting"] as UniversalFormatRules;

  universalCache = formatting;
  return formatting;
}

/**
 * Load language-specific format rules from R50_codefile/languages/{lang}/_format.jsonc.
 *
 * @param lang - Language identifier (e.g., "go", "rust")
 * @returns Language formatting rules
 */
export async function loadLanguageFormatRules(lang: string): Promise<LanguageFormatRules> {
  const pipeline = getDefaultPipeline();
  const text = await pipeline.getText(`code/R50_codefile/languages/${lang}/_format.jsonc`);
  const data = parseJsonc(text) as Record<string, unknown>;

  return {
    language: (data["language"] as string) ?? lang,
    inherits: (data["inherits"] as string) ?? "universal",
    comment_syntax: data["comment_syntax"] as CommentSyntax,
    pragma_expression: data["pragma_expression"] as PragmaExpression,
    identity_constructs: (data["identity_constructs"] as Record<string, IdentityConstruct>) ?? {},
    language_specific: data["language_specific"] as Record<string, unknown> | undefined,
  };
}

/**
 * Load composed format rules (universal + language overlay).
 *
 * This is the main entry point for format rule consumers.
 * Results are cached by language.
 *
 * @param lang - Language identifier (e.g., "go", "rust")
 * @returns Composed format rules with both universal and language-specific data
 */
export async function loadFormatRules(lang: string): Promise<ComposedFormatRules> {
  const cached = formatCache.get(lang);
  if (cached) return cached;

  const [universal, language] = await Promise.all([
    loadUniversalFormatRules(),
    loadLanguageFormatRules(lang),
  ]);

  const composed: ComposedFormatRules = { universal, language };
  formatCache.set(lang, composed);
  return composed;
}

/** Clear format rules cache. */
export function clearFormatCache(): void {
  formatCache.clear();
  universalCache = null;
}
registerCache("data/format-rules", clearFormatCache);

// ============================================================================
// CLOSING
// ============================================================================
//
// Format rules are the structural grammar of OmniCode files.
// Universal rules define WHAT the patterns are (separators, boundaries, fields).
// Language rules define HOW those patterns look in a specific language
// (comment prefix, pragma expression, identity constructs).
//
// The linter reads these at runtime instead of hardcoding separator widths,
// block patterns, and directive lists. Change the schema, behavior changes.
//
// "Look that thou make them after their pattern." — Exodus 25:40
// ============================================================================
