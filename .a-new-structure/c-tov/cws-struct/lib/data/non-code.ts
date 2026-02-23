// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/data/non-code.ts
// key:     B-tov-cws-struct-lib-data-non-code
// title:   CWS Struct — Non-Code Categories Registry (Schema-Driven)
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Types and loader for non-code element categories from R50_codefile schemas.
//          Universal categories come from R50_codefile/universal/_non-code.jsonc.
//          Per-language categories come from R50_codefile/languages/{lang}/_non-code.jsonc.
//
//          Non-code = everything the chunking engine must see that ISN'T executable
//          code. Pragmas, separators, doc comments, regular comments, blanks,
//          build tags, attributes. The chunker reads these to know WHAT it's
//          looking at before the handler classifies language-specific constructs.
//
// biblical_foundation: "The eye cannot say unto the hand, I have no need of
//   thee." — 1 Corinthians 12:21
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { parse as parseJsonc } from "@std/jsonc";
// Direct import — barrel would create cycle: foundation/mod → code-schema → data/mod → non-code
import { getDefaultPipeline } from "../foundation/schema-pipeline.ts";
import { registerCache } from "../foundation/cache-registry.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Types — chunk categories
// ---------------------------------------------------------------------------

/** A non-code chunk category definition from schema. */
export interface ChunkCategory {
  /** Human-readable description. */
  description: string;
  /** Primary start pattern (regex string). */
  start_pattern?: string;
  /** General pattern (regex string). */
  pattern?: string;
  /** Continuation pattern for multi-line categories. */
  continuation?: string;
  /** Multiple patterns (for categories with several forms). */
  patterns?: string[];
  /** Whether this category spans multiple lines. */
  multi_line?: boolean;
  /** Which block this category belongs to (if fixed). */
  block_target?: string;
  /** What this category attaches to (e.g., "next_declaration"). */
  attaches_to?: string;
  /** Whether this category is standalone (not attached). */
  standalone?: boolean;
  /** Categories this one excludes (priority resolution). */
  excludes?: string[];
  /** May include a title line (for separators). */
  may_include_title?: boolean;
  /** Internal structure pattern. */
  structure?: string;
  /** Role in the file. */
  role?: string;
  /** Classification priority. */
  priority?: string;
  /** Additional note. */
  note?: string;
  /** Special cases within this category (language-specific). */
  special_cases?: Record<string, unknown>;
  /** Detection method — "pattern" (default) or "contextual" (Go doc_comment). */
  detection_method?: string;
  /** Placement hint. */
  placement?: string;
  /** Example values. */
  examples?: string[];
}

/** A chunking rule from schema. */
export interface ChunkingRule {
  /** Human-readable description. */
  description: string;
  /** The rule text. */
  rule?: string;
  /** Exception to the rule. */
  exception?: string;
  /** Which languages this applies to. */
  applies_to?: string;
  /** Interaction notes with other rules. */
  interaction_with_docs?: string;
  /** Category this rule produces. */
  category?: string;
  /** Priority order for classification. */
  order?: string[];
  /** Single blank exception behavior. */
  single_blank_exception?: string;
  /** Additional note. */
  note?: string;
}

/** Language extension contract — how language schemas extend universal. */
export interface LanguageExtensionContract {
  description: string;
  rules: string[];
}

// ---------------------------------------------------------------------------
// 2. Types — composed non-code definitions
// ---------------------------------------------------------------------------

/** Universal non-code definitions — loaded from R50_codefile/universal/_non-code.jsonc. */
export interface UniversalNonCode {
  scope: string;
  purpose: string;
  chunk_categories: Record<string, ChunkCategory>;
  chunking_rules: Record<string, ChunkingRule>;
  language_extension_contract?: LanguageExtensionContract;
}

/** Language-specific non-code definitions — loaded from R50_codefile/languages/{lang}/_non-code.jsonc. */
export interface LanguageNonCode {
  language: string;
  purpose: string;
  chunk_categories: Record<string, ChunkCategory>;
  chunking_rules: Record<string, ChunkingRule>;
}

/** Composed non-code definitions — universal + language overlay. */
export interface ComposedNonCode {
  universal: UniversalNonCode;
  language: LanguageNonCode;
  /** Merged categories — language categories overlaid on universal. */
  allCategories: Record<string, ChunkCategory>;
  /** Merged rules — language rules overlaid on universal. */
  allRules: Record<string, ChunkingRule>;
}

// ---------------------------------------------------------------------------
// 3. Loader — schema-driven non-code definitions
// ---------------------------------------------------------------------------

const nonCodeCache = new Map<string, ComposedNonCode>();
let universalNonCodeCache: UniversalNonCode | null = null;

/**
 * Load universal non-code definitions from R50_codefile/universal/_non-code.jsonc.
 *
 * @returns Universal non-code categories and chunking rules
 */
export async function loadUniversalNonCode(): Promise<UniversalNonCode> {
  if (universalNonCodeCache) return universalNonCodeCache;

  const pipeline = getDefaultPipeline();
  const text = await pipeline.getText("code/R50_codefile/universal/_non-code.jsonc");
  const data = parseJsonc(text) as Record<string, unknown>;

  const result: UniversalNonCode = {
    scope: (data["scope"] as string) ?? "all code formats",
    purpose: (data["purpose"] as string) ?? "",
    chunk_categories: stripNotes(data["chunk_categories"] as Record<string, unknown>),
    chunking_rules: stripNotes(data["chunking_rules"] as Record<string, unknown>),
    language_extension_contract: data["language_extension_contract"] as LanguageExtensionContract | undefined,
  };

  universalNonCodeCache = result;
  return result;
}

/**
 * Load language-specific non-code definitions from R50_codefile/languages/{lang}/_non-code.jsonc.
 *
 * @param lang - Language identifier (e.g., "go", "rust")
 * @returns Language-specific non-code categories and chunking rules
 */
export async function loadLanguageNonCode(lang: string): Promise<LanguageNonCode> {
  const pipeline = getDefaultPipeline();
  const text = await pipeline.getText(`code/R50_codefile/languages/${lang}/_non-code.jsonc`);
  const data = parseJsonc(text) as Record<string, unknown>;

  return {
    language: (data["language"] as string) ?? lang,
    purpose: (data["purpose"] as string) ?? "",
    chunk_categories: stripNotes(data["chunk_categories"] as Record<string, unknown>),
    chunking_rules: stripNotes(data["chunking_rules"] as Record<string, unknown>),
  };
}

/**
 * Load composed non-code definitions (universal + language overlay).
 *
 * This is the main entry point for non-code consumers.
 * Results are cached by language. The merged `allCategories` and `allRules`
 * maps combine universal + language, with language values overriding universal
 * when both define the same key.
 *
 * @param lang - Language identifier (e.g., "go", "rust")
 * @returns Composed non-code definitions with merged categories and rules
 */
export async function loadNonCode(lang: string): Promise<ComposedNonCode> {
  const cached = nonCodeCache.get(lang);
  if (cached) return cached;

  const [universal, language] = await Promise.all([
    loadUniversalNonCode(),
    loadLanguageNonCode(lang),
  ]);

  // Merge: universal categories first, then language categories overlay
  const allCategories: Record<string, ChunkCategory> = {
    ...universal.chunk_categories,
    ...language.chunk_categories,
  };

  const allRules: Record<string, ChunkingRule> = {
    ...universal.chunking_rules,
    ...language.chunking_rules,
  };

  const composed: ComposedNonCode = { universal, language, allCategories, allRules };
  nonCodeCache.set(lang, composed);
  return composed;
}

/** Clear non-code cache. */
export function clearNonCodeCache(): void {
  nonCodeCache.clear();
  universalNonCodeCache = null;
}
registerCache("data/non-code", clearNonCodeCache);

// ---------------------------------------------------------------------------
// 4. Helpers
// ---------------------------------------------------------------------------

/**
 * Strip "note" keys from a record (JSONC schema convention — notes are for humans).
 * Returns typed entries only.
 */
function stripNotes<T>(raw: Record<string, unknown>): Record<string, T> {
  if (!raw) return {};
  const result: Record<string, T> = {};
  for (const [key, value] of Object.entries(raw)) {
    if (key === "note") continue;
    if (typeof value === "object" && value !== null) {
      result[key] = value as T;
    }
  }
  return result;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Non-code categories are the connective tissue of every source file.
// The universal categories (pragma, separator, doc_comment, comment, blank)
// exist in ALL languages. Language schemas ADD categories (Go: build_tag,
// generate_directive; Rust: inner_doc, attribute, inner_attribute) and
// OVERRIDE patterns with concrete regex.
//
// The composed result gives consumers everything they need: all categories
// (universal + language), all rules (universal + language), ready for the
// chunking engine to consume.
//
// "The eye cannot say unto the hand, I have no need of thee."
// — 1 Corinthians 12:21
// ============================================================================
