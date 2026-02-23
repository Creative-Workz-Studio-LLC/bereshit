// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/config.ts
// key:     B-tov-cws-struct-lib-foundation-config
// title:   CWS Struct — Tool Configuration
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// updated: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Config-driven values for cws-struct. Loads from deno.jsonc
//          _cwsStruct section with hardcoded defaults as tripwire fallback.
//          "Where no counsel is, the people fall: but in the multitude of
//          counsellors there is safety." — Proverbs 11:14
//
// biblical_foundation: "Where no counsel is, the people fall: but in the
//   multitude of counsellors there is safety." — Proverbs 11:14
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

/** Tool configuration — loaded from deno.jsonc or defaults. */
export interface ToolConfig {
  /** Tool version (from deno.jsonc "version" field). */
  version: string;
  /** Parallel file processing concurrency. */
  concurrency: number;
  /** Maximum files discovered in a single walk. */
  maxFiles: number;
  /** Maximum file size in bytes before skipping. */
  maxFileSize: number;
  /** Directory basenames to exclude from walking. */
  excludedDirs: string[];
  /** Impact weights for health scoring. */
  impactWeights: Record<string, number>;
}

// ---------------------------------------------------------------------------
// Constants — defaults (tripwire fallback)
// ---------------------------------------------------------------------------

/** Default config used when deno.jsonc has no _cwsStruct section. */
const DEFAULTS: ToolConfig = {
  version: "0.1.0",
  concurrency: 8,
  maxFiles: 10_000,
  maxFileSize: 1_048_576,
  excludedDirs: [".git", "node_modules", ".cache", "__pycache__", "target", "bin"],
  impactWeights: { error: 2.0, warn: 1.0, info: 0.25 },
};

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Config loader
// ---------------------------------------------------------------------------

/**
 * Load tool configuration from deno.jsonc.
 *
 * Reads the "version" field and "_cwsStruct" section from deno.jsonc.
 * If deno.jsonc is missing or _cwsStruct isn't present, returns defaults.
 * Each field individually falls back to its default — partial configs work.
 *
 * @param configPath - Path to deno.jsonc (default: ./deno.jsonc).
 * @returns Resolved ToolConfig with all fields populated.
 */
export async function loadConfig(configPath?: string): Promise<Readonly<ToolConfig>> {
  const path = configPath ?? new URL("../../deno.jsonc", import.meta.url).pathname;

  let raw: Record<string, unknown>;
  try {
    const text = await Deno.readTextFile(path);
    // Strip JSONC comments (single-line only — sufficient for deno.jsonc)
    const jsonText = text.replace(/^\s*\/\/.*$/gm, "");
    raw = JSON.parse(jsonText);
  } catch {
    // No config file → all defaults. Tripwire holds.
    return { ...DEFAULTS };
  }

  // Top-level version
  const version = typeof raw.version === "string" ? raw.version : DEFAULTS.version;

  // _cwsStruct section
  const section = (raw._cwsStruct ?? {}) as Record<string, unknown>;

  const config: ToolConfig = {
    version,
    concurrency: typeof section.concurrency === "number"
      ? section.concurrency
      : DEFAULTS.concurrency,
    maxFiles: typeof section.maxFiles === "number"
      ? section.maxFiles
      : DEFAULTS.maxFiles,
    maxFileSize: typeof section.maxFileSize === "number"
      ? section.maxFileSize
      : DEFAULTS.maxFileSize,
    excludedDirs: Array.isArray(section.excludedDirs)
      ? section.excludedDirs as string[]
      : DEFAULTS.excludedDirs,
    impactWeights: isImpactWeights(section.impactWeights)
      ? section.impactWeights as Record<string, number>
      : DEFAULTS.impactWeights,
  };

  // Freeze config — immutable after load. No subsystem should mutate it.
  return Object.freeze(config);
}

/**
 * Type guard for impact weights — must be Record<string, number>.
 */
function isImpactWeights(val: unknown): val is Record<string, number> {
  if (!val || typeof val !== "object") return false;
  return Object.values(val as Record<string, unknown>).every(
    (v) => typeof v === "number",
  );
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Config-driven values: version, concurrency, limits, weights.
// Loaded from deno.jsonc _cwsStruct section. Defaults as tripwire.
// Each field individually falls back — partial config is safe.
//
// "Where no counsel is, the people fall." — Proverbs 11:14
// ============================================================================
