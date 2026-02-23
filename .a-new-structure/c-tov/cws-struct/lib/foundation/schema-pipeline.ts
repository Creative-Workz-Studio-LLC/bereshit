// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/schema-pipeline.ts
// key:     B-tov-cws-struct-lib-foundation-schema-pipeline
// title:   CWS Struct — Schema Content Pipeline
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-19
// authors: Nova Dawn (CPI-SI)
// purpose: Unified schema discovery, loading, and caching pipeline.
//          Replaces hardcoded relative paths with layered resolution:
//          env override → project-local → default. Each source is tried
//          in order; first one that provides the schema wins.
//
//          Game dev insight: the engine discovers and loads game data.
//          Content creators add levels by adding data files, not editing
//          engine code. For cws-struct: schemas are game data, the linter
//          is the engine. Add a format by adding a schema.
//
// biblical_foundation: "Except the LORD build the house, they labour
//   in vain that build it." — Psalm 127:1
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { join } from "@std/path";
import { ToolError } from "./tool-error.ts";
import { registerCache } from "./cache-registry.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// SchemaSource — where schema text comes from
// ---------------------------------------------------------------------------

/**
 * A source of raw schema text. Implementations provide text from different
 * locations — filesystem, embedded data, network, etc.
 *
 * The pipeline tries sources in priority order. First one that returns
 * text wins. Errors mean "try next source," not "abort."
 */
export interface SchemaSource {
  /** Human-readable name for error messages and diagnostics. */
  readonly name: string;

  /**
   * Retrieve raw text for a schema ID.
   * @param schemaId - Relative path within schema tree (e.g., "code/format/go-4block-schema.jsonc")
   * @returns Raw JSONC text
   * @throws Any error — pipeline will catch and try next source
   */
  getText(schemaId: string): Promise<string>;
}

// ---------------------------------------------------------------------------
// FileSource — reads schemas from a directory on disk
// ---------------------------------------------------------------------------

/**
 * Schema source backed by a filesystem directory.
 *
 * Schema IDs are resolved as relative paths under the base directory:
 *   baseDir + "/" + schemaId → absolute file path
 *
 * In a compiled binary (deno compile --include), Deno.readTextFile()
 * reads from the virtual filesystem transparently — same code path.
 */
export class FileSource implements SchemaSource {
  readonly name: string;

  constructor(
    private readonly baseDir: string,
    name?: string,
  ) {
    this.name = name ?? `file(${baseDir})`;
  }

  async getText(schemaId: string): Promise<string> {
    const path = join(this.baseDir, schemaId);
    return await Deno.readTextFile(path);
  }
}

// ---------------------------------------------------------------------------
// SchemaPipeline — layered discovery with caching
// ---------------------------------------------------------------------------

/**
 * Unified schema content pipeline.
 *
 * Sources are tried in priority order (first added = highest priority).
 * Raw text is cached by schema ID — parsed-rule caching is the caller's
 * concern (loadCodeRules, loadRules handle their own caches).
 *
 * Resolution order for the default pipeline:
 * 1. CWS_STRUCT_SCHEMA_DIR env var → FileSource (override for testing/custom layouts)
 * 2. .cws-struct/schemas/ in CWD → FileSource (project-local customization)
 * 3. Default relative to binary/source → FileSource (shipped schemas)
 */
export class SchemaPipeline {
  private readonly sources: SchemaSource[] = [];
  private readonly textCache = new Map<string, string>();

  /** Add a schema source. First added = highest priority. */
  addSource(source: SchemaSource): void {
    this.sources.push(source);
  }

  /**
   * Retrieve raw text for a schema ID.
   *
   * Checks cache first. Then tries each source in order — first one
   * that returns text wins. Result is cached for future calls.
   *
   * @param schemaId - Relative path (e.g., "code/format/go-4block-schema.jsonc")
   * @returns Raw JSONC text
   * @throws Error with detailed diagnostics if no source provides the schema
   */
  async getText(schemaId: string): Promise<string> {
    // Cache hit — no I/O
    const cached = this.textCache.get(schemaId);
    if (cached !== undefined) return cached;

    // Try each source in priority order
    const tried: string[] = [];

    for (const source of this.sources) {
      try {
        const text = await source.getText(schemaId);
        this.textCache.set(schemaId, text);
        return text;
      } catch {
        tried.push(source.name);
      }
    }

    // No source found — structured error with diagnostics
    throw new ToolError("CWS-T00-001", {
      schemaId,
      sources: tried.join(", "),
    });
  }

  /** List registered sources (for diagnostics and testing). */
  listSources(): readonly string[] {
    return this.sources.map((s) => s.name);
  }

  /** Clear the raw text cache. Does NOT clear parsed-rule caches in loaders. */
  clear(): void {
    this.textCache.clear();
  }
}

// ---------------------------------------------------------------------------
// Default pipeline — singleton, lazy-initialized
// ---------------------------------------------------------------------------

let _pipeline: SchemaPipeline | null = null;

/**
 * Default schema base directory, relative to this module.
 *
 * import.meta.dirname works in both:
 * - Dev mode: resolves to the actual filesystem path
 * - Compiled binary: resolves within the virtual FS (via --include)
 */
const DEFAULT_SCHEMA_DIR = join(
  import.meta.dirname!,
  "../../../../b-word/L1-omnicode/ladder/schemas",
);

/**
 * Get the default schema pipeline (singleton, lazy-initialized).
 *
 * Sources in priority order:
 * 1. CWS_STRUCT_SCHEMA_DIR env override (if set)
 * 2. .cws-struct/schemas/ in CWD (project-local)
 * 3. Default schemas relative to source/binary
 *
 * Call clearPipeline() to force re-initialization (e.g., after changing
 * CWS_STRUCT_SCHEMA_DIR in tests).
 */
export function getDefaultPipeline(): SchemaPipeline {
  if (_pipeline) return _pipeline;

  _pipeline = new SchemaPipeline();

  // Source 1 (highest priority): Environment override
  let envDir: string | undefined;
  try {
    envDir = Deno.env.get("CWS_STRUCT_SCHEMA_DIR");
  } catch {
    // --allow-env not granted — skip env source
  }
  if (envDir) {
    _pipeline.addSource(
      new FileSource(envDir, `env(CWS_STRUCT_SCHEMA_DIR=${envDir})`),
    );
  }

  // Source 2: Project-local .cws-struct/schemas/
  try {
    const projectLocal = join(Deno.cwd(), ".cws-struct", "schemas");
    _pipeline.addSource(
      new FileSource(projectLocal, "project-local(.cws-struct/schemas/)"),
    );
  } catch {
    // Deno.cwd() can fail in unusual environments — skip
  }

  // Source 3 (lowest priority): Default relative to source/binary
  _pipeline.addSource(new FileSource(DEFAULT_SCHEMA_DIR, "default"));

  return _pipeline;
}

/**
 * Reset the pipeline singleton. Clears text cache and source list.
 *
 * Use in tests when changing CWS_STRUCT_SCHEMA_DIR, or when the
 * schema directory structure has changed and sources need re-evaluation.
 */
export function clearPipeline(): void {
  if (_pipeline) {
    _pipeline.clear();
    _pipeline = null;
  }
}
registerCache("schema-pipeline", clearPipeline);

// ============================================================================
// CLOSING
// ============================================================================
//
// The pipeline is the foundation the schema house sits on. Layered discovery
// replaces hardcoded paths. The schemas are game data — the engine discovers
// them, loads them, caches them. Adding a format means adding a schema file,
// not editing engine code.
//
// "Except the LORD build the house, they labour in vain that build it."
// — Psalm 127:1
// ============================================================================
