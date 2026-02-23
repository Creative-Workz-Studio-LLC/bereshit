// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/composition-loader.ts
// key:     B-tov-cws-struct-lib-foundation-composition-loader
// title:   CWS Struct — Three-Cord Composition Loader
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-21
// authors: Nova Dawn (CPI-SI)
// purpose: Load and resolve composition targets from the three-cord architecture.
//          Target recipes in index/targets/ map format+form to the structure
//          and content schemas that compose a complete file specification.
//          This is the linker: given a pragma, resolve all schemas needed.
//
//          Structure stays constant across languages. Content changes per
//          language. The composition target maps the two sides for a given
//          language+form combination. The transpiler uses this to know WHAT
//          to preserve (structure) and WHAT to translate (content).
//
// biblical_foundation: "A threefold cord is not quickly broken."
//   — Ecclesiastes 4:12
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
import { ToolError } from "./tool-error.ts";
import { registerCache } from "./cache-registry.ts";

// ---------------------------------------------------------------------------
// 2. Types — Composition target (matches index/targets/*.jsonc shape)
// ---------------------------------------------------------------------------

/** Schema paths per block in a composition recipe. */
export interface CompositionParts {
  setup: string[];
  body: string[];
  closing: string[];
}

/** One side (structure or content) of the composition. */
export interface CompositionSide {
  /** Path to whole schema (e.g., "R50_codefile/universal/_structure.jsonc"). */
  whole: string;
  /** Path to form-level schema (e.g., "forms/module/_structure.jsonc"). */
  molded: string;
  /** Paths to R25 block/section schemas per block. */
  parts: CompositionParts;
}

/** Section status per block in the composition target. */
export interface CompositionBlockSections {
  required: string[];
  available?: string[];
  reserved: string[];
  ordering?: string;
}

/** Active sections per block — what the composition resolves. */
export interface CompositionActiveSections {
  SETUP: CompositionBlockSections;
  BODY: CompositionBlockSections;
  CLOSING: CompositionBlockSections;
}

/**
 * A composition target — the recipe for building a specific format+form.
 *
 * Loaded from index/targets/{format}-{form}.jsonc. Contains paths to all
 * structure and content schemas needed, plus the active section list and
 * resolution order.
 *
 * This is the linker's instruction set: given "rust-module", compose
 * _structure + forms/module/_structure + _content +
 * forms/module/rust + all R25_blocks/ schemas.
 */
export interface CompositionTarget {
  /** The OmniCode pragma this recipe produces (e.g., "//omni:code --rust -module"). */
  pragma: string;
  /** Language identifier (e.g., "rust", "go"). */
  format: string;
  /** Form identifier (e.g., "module", "library", "executable"). */
  form: string;
  /** Block pattern (e.g., "4-block"). */
  block_pattern: string;
  /** Schema paths for both sides of the composition. */
  composition: {
    structure: CompositionSide;
    content: CompositionSide;
  };
  /** Per-block active/available/reserved section lists. */
  active_sections: CompositionActiveSections;
  /** Human-readable resolution steps. */
  resolution_order: string[];
}

// ---------------------------------------------------------------------------
// 3. Types — Loaded schema data (parsed JSONC → typed objects)
// ---------------------------------------------------------------------------

/**
 * A loaded whole schema (structure or content).
 *
 * These are loosely typed because the schema shapes vary:
 * - _structure.jsonc has blocks, sections, placement
 * - _content.jsonc has invariants, content_patterns
 *
 * The transpiler will narrow these based on the target's block_pattern.
 */
export type LoadedSchema = Record<string, unknown>;

/**
 * A fully resolved composition — all schemas loaded and ready.
 *
 * The transpiler receives this after resolution. It has everything needed
 * to validate a source file against its schema, extract semantic content,
 * and generate a target file using a different content schema.
 */
export interface ResolvedComposition {
  /** The original target recipe. */
  target: CompositionTarget;

  /** Loaded structure schemas. */
  structure: {
    whole: LoadedSchema;
    molded: LoadedSchema;
    parts: Map<string, LoadedSchema>;
  };

  /** Loaded content schemas. */
  content: {
    whole: LoadedSchema;
    molded: LoadedSchema;
    parts: Map<string, LoadedSchema>;
  };
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Schema ID construction
// ---------------------------------------------------------------------------

/**
 * Build the schema ID for a composition target file.
 *
 * @param format - Language identifier (e.g., "rust", "go")
 * @param form - Form identifier (e.g., "module", "library")
 * @returns Schema ID (e.g., "code/index/targets/rust/module.jsonc")
 */
function targetSchemaId(format: string, form: string): string {
  return `code/index/targets/${format}/${form}.jsonc`;
}

// ---------------------------------------------------------------------------
// 2. Parsing
// ---------------------------------------------------------------------------

/**
 * Parse a composition target from raw JSONC text.
 *
 * Extracts the `target` field from the schema file (ignoring pragma
 * and closing fields). Pure function — no I/O.
 *
 * @param jsonText - Raw JSONC text from the target file
 * @returns Parsed CompositionTarget
 * @throws If the JSON is invalid or missing required fields
 */
export function parseCompositionTarget(jsonText: string): CompositionTarget {
  const raw = parseJsonc(jsonText) as Record<string, unknown>;
  const target = raw.target as CompositionTarget | undefined;

  if (!target) {
    throw new ToolError("CWS-T00-020");
  }

  // Validate required fields exist
  if (!target.pragma || !target.format || !target.form) {
    throw new ToolError("CWS-T00-021", {
      pragma: String(target.pragma ?? ""),
      format: String(target.format ?? ""),
      form: String(target.form ?? ""),
    });
  }

  if (!target.composition?.structure || !target.composition?.content) {
    throw new ToolError("CWS-T00-022");
  }

  return target;
}

// ---------------------------------------------------------------------------
// 3. Loading — schema resolution
// ---------------------------------------------------------------------------

/**
 * Load a single schema file via the pipeline and parse it as JSONC.
 *
 * Composition targets reference paths relative to their domain (e.g.,
 * "R50_codefile/universal/_structure.jsonc", "forms/module/_structure.jsonc"). The pipeline
 * resolves schema IDs from the schemas root, so we prepend "code/".
 *
 * @param domainRelativePath - Path relative to the code/ domain
 * @returns Parsed schema as a record
 */
async function loadSchema(domainRelativePath: string): Promise<LoadedSchema> {
  const pipeline = getDefaultPipeline();
  const schemaId = `code/${domainRelativePath}`;
  const text = await pipeline.getText(schemaId);
  return parseJsonc(text) as LoadedSchema;
}

/**
 * Load all parts schemas from a list of paths.
 *
 * @param paths - Array of schema IDs
 * @returns Map of schema ID → loaded schema
 */
async function loadParts(
  paths: CompositionParts,
): Promise<Map<string, LoadedSchema>> {
  const allPaths = [...paths.setup, ...paths.body, ...paths.closing];
  const results = new Map<string, LoadedSchema>();

  // Load all parts in parallel
  const entries = await Promise.all(
    allPaths.map(async (path) => {
      const schema = await loadSchema(path);
      return [path, schema] as const;
    }),
  );

  for (const [path, schema] of entries) {
    results.set(path, schema);
  }

  return results;
}

/**
 * Load and fully resolve a composition target.
 *
 * This is the main entry point for the composition loader. Given a
 * format+form, it:
 * 1. Loads the target recipe from index/targets/
 * 2. Loads all referenced structure schemas (whole + molded + parts)
 * 3. Loads all referenced content schemas (whole + molded + parts)
 * 4. Returns the fully resolved composition
 *
 * @param format - Language (e.g., "rust", "go")
 * @param form - Form (e.g., "module", "library", "executable")
 * @returns Fully resolved composition with all schemas loaded
 *
 * @example
 * ```typescript
 * const composed = await loadComposition("rust", "module");
 * // composed.target.pragma === "//omni:code --rust -module"
 * // composed.structure.whole contains 4-block definition
 * // composed.content.molded contains Rust module expectations
 * ```
 */
export async function loadComposition(
  format: string,
  form: string,
): Promise<ResolvedComposition> {
  // 1. Load the target recipe
  const schemaId = targetSchemaId(format, form);
  const pipeline = getDefaultPipeline();
  const targetText = await pipeline.getText(schemaId);
  const target = parseCompositionTarget(targetText);

  // 2. Load structure schemas (whole, molded, parts — in parallel)
  const [structWhole, structMolded, structParts] = await Promise.all([
    loadSchema(target.composition.structure.whole),
    loadSchema(target.composition.structure.molded),
    loadParts(target.composition.structure.parts),
  ]);

  // 3. Load content schemas (whole, molded, parts — in parallel)
  const [contentWhole, contentMolded, contentParts] = await Promise.all([
    loadSchema(target.composition.content.whole),
    loadSchema(target.composition.content.molded),
    loadParts(target.composition.content.parts),
  ]);

  // 4. Compose and return
  return {
    target,
    structure: {
      whole: structWhole,
      molded: structMolded,
      parts: structParts,
    },
    content: {
      whole: contentWhole,
      molded: contentMolded,
      parts: contentParts,
    },
  };
}

// ---------------------------------------------------------------------------
// 4. Cache management
// ---------------------------------------------------------------------------

const compositionCache = new Map<string, ResolvedComposition>();

/**
 * Load a composition target with caching.
 *
 * Same as loadComposition but caches the result by format+form key.
 * Use clearCompositionCache() to reset.
 *
 * @param format - Language (e.g., "rust", "go")
 * @param form - Form (e.g., "module", "library", "executable")
 * @returns Cached or freshly loaded resolved composition
 */
export async function loadCompositionCached(
  format: string,
  form: string,
): Promise<ResolvedComposition> {
  const key = `${format}:${form}`;
  const cached = compositionCache.get(key);
  if (cached) return cached;

  const composition = await loadComposition(format, form);
  compositionCache.set(key, composition);
  return composition;
}

/**
 * List available composition targets by scanning the index/targets/ directory.
 *
 * @returns Array of { format, form } pairs for available targets
 */
export async function listCompositionTargets(): Promise<
  Array<{ format: string; form: string }>
> {
  const pipeline = getDefaultPipeline();
  const manifestText = await pipeline.getText("code/index/manifest.jsonc");
  const manifest = parseJsonc(manifestText) as Record<string, unknown>;

  // Extract target filenames from the layout.index_tree.files
  const layout = manifest.layout as Record<string, unknown> | undefined;
  const indexTree = layout?.index_tree as Record<string, unknown> | undefined;
  const files = indexTree?.files as Record<string, string> | undefined;

  if (!files) return [];

  const targets: Array<{ format: string; form: string }> = [];
  for (const path of Object.keys(files)) {
    const match = path.match(/^index\/targets\/(\w+)\/(\w+)\.jsonc$/);
    if (match && match[1] && match[2]) {
      targets.push({ format: match[1], form: match[2] });
    }
  }

  return targets;
}

/** Clear the composition cache. */
export function clearCompositionCache(): void {
  compositionCache.clear();
}
registerCache("composition-loader", clearCompositionCache);

// ============================================================================
// CLOSING
// ============================================================================
//
// The composition loader is the linker of the three-cord architecture.
// Structure (cord 1) defines shapes — format-agnostic, universal.
// Content (cord 2) defines fills — language-specific, variable.
// Index (cord 3) maps the composition — which schemas combine to
// produce a specific language+form file.
//
// Given a target like "rust-module", the loader resolves all schemas
// and returns a fully composed specification. The transpiler uses this
// to validate source, extract semantics, and generate target code.
//
// "A threefold cord is not quickly broken." — Ecclesiastes 4:12
// ============================================================================
