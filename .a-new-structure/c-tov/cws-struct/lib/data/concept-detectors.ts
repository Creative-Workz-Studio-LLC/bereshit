// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/data/concept-detectors.ts
// key:     B-tov-cws-struct-lib-data-concept-detectors
// title:   CWS Struct — Concept Detector Loader (Schema-Driven)
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// purpose: Schema-driven loader for R[5] concept detection patterns.
//          Reads detect regex from R5_patterns/{concept}/{language}.jsonc,
//          compiles into RegExp objects, caches per language.
//
//          The linter doesn't own this data. Bereshit does.
//          The schemas serve three tools:
//            - detect → linter (pattern recognition)
//            - emit   → transpiler (code generation)
//            - both   → bidirectional transpilation
//
//          This loader reads "detect" only. The transpiler will read "emit".
//          Same schema, different consumers.
//
// biblical_foundation: "For as the body is one, and hath many members, and
//   all the members of that one body, being many, are one body."
//   — 1 Corinthians 12:12
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { parse as parseJsonc } from "@std/jsonc";
// Direct imports — barrel would create cycle: foundation/mod → code-schema → data/mod → concept-detectors
import { getDefaultPipeline } from "../foundation/schema-pipeline.ts";
import { registerCache } from "../foundation/cache-registry.ts";
import { ToolError } from "../foundation/tool-error.ts";
import { CONCEPT_REGISTRY, CONCEPT_ORDER } from "./concepts.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// 1. Types — concept detection patterns
// ---------------------------------------------------------------------------

/** A single compiled concept detection pattern. */
export interface ConceptPattern {
  /** Pattern identifier within the concept (e.g., "qualified_call", "mutable"). */
  id: string;

  /** Human-readable syntax template (e.g., "{module}::{function}({args})"). */
  syntax: string;

  /** Compiled detection regex — built from schema "detect" string. */
  detect: RegExp;

  /** Named capture group mappings (e.g., { "module": 1, "function": 2 }). */
  captures: Record<string, number>;

  /** Lower = more specific. Used for ordering when multiple patterns match. */
  priority: number;

  /** Optional notes from the schema author. */
  notes?: string;
}

/** All detection patterns for one concept in one language. */
export interface ConceptDetector {
  /** Concept identifier (e.g., "function_call", "error_propagation"). */
  conceptId: string;

  /** Language identifier (e.g., "rust", "go"). */
  language: string;

  /** Detection patterns ordered by priority (lower = more specific). */
  patterns: ConceptPattern[];
}

// ---------------------------------------------------------------------------
// 2. Detection — test a line against concept patterns
// ---------------------------------------------------------------------------

/**
 * Test whether a line matches any detection pattern for a concept.
 *
 * Returns true if ANY pattern in the detector matches the line.
 * Patterns are tested in priority order — first match wins, but we
 * only care about presence, not which pattern matched.
 *
 * @param detector - The concept detector with compiled patterns
 * @param line     - A single line of source code to test
 * @returns true if any pattern matches
 */
export function detectConcept(detector: ConceptDetector, line: string): boolean {
  for (const pattern of detector.patterns) {
    if (pattern.detect.test(line)) return true;
  }
  return false;
}

/**
 * Test a line and return the first matching pattern (or undefined).
 *
 * Useful for diagnostics — tells you WHICH pattern matched, not just that one did.
 *
 * @param detector - The concept detector with compiled patterns
 * @param line     - A single line of source code to test
 * @returns The first matching pattern, or undefined if no match
 */
export function matchConceptPattern(
  detector: ConceptDetector,
  line: string,
): ConceptPattern | undefined {
  for (const pattern of detector.patterns) {
    if (pattern.detect.test(line)) return pattern;
  }
  return undefined;
}

// ---------------------------------------------------------------------------
// 3. Schema shape — raw JSON before compilation
// ---------------------------------------------------------------------------

/** Raw pattern object as it appears in the schema JSONC. */
interface RawSchemaPattern {
  id: string;
  syntax?: string;
  detect?: string;
  emit?: string;
  captures?: Record<string, number>;
  priority: number;
  notes?: string;
  when?: string;
  mutable?: boolean;
}

/** Top-level schema shape for a concept language file. */
interface RawConceptSchema {
  concept: string;
  language: string;
  patterns: RawSchemaPattern[];
}

// ---------------------------------------------------------------------------
// 4. Compilation — schema JSON → typed ConceptDetector
// ---------------------------------------------------------------------------

/**
 * Compile a raw schema pattern into a ConceptPattern with a real RegExp.
 *
 * @param raw       - Raw pattern from the schema
 * @param conceptId - Concept identifier (for error reporting)
 * @returns Compiled ConceptPattern
 * @throws ToolError(T00-064) if the detect regex is invalid
 */
function compilePattern(raw: RawSchemaPattern, conceptId: string): ConceptPattern {
  let regex: RegExp;
  try {
    regex = new RegExp(raw.detect!);
  } catch (e) {
    throw new ToolError("CWS-T00-064", {
      concept: conceptId,
      pattern: raw.id,
      error: e instanceof Error ? e.message : String(e),
    });
  }
  return {
    id: raw.id,
    syntax: raw.syntax ?? raw.emit ?? raw.id,
    detect: regex,
    captures: raw.captures ?? {},
    priority: raw.priority,
    notes: raw.notes,
  };
}

/**
 * Compile all detect-bearing patterns from a raw schema into a ConceptDetector.
 *
 * Patterns without a "detect" field (emit-only patterns) are silently skipped.
 * Invalid regex throws — schema authors must provide valid regex.
 *
 * @param schema   - Parsed schema data
 * @param language - Language identifier for the detector
 * @returns ConceptDetector with compiled patterns (may be empty if no detect patterns)
 */
function compileDetector(schema: RawConceptSchema, language: string): ConceptDetector {
  const patterns: ConceptPattern[] = [];

  for (const raw of schema.patterns) {
    // Skip patterns without detect field (emit-only for transpiler)
    if (!raw.detect) continue;

    patterns.push(compilePattern(raw, schema.concept));
  }

  // Sort by priority ascending (1 = most specific, higher = more general)
  patterns.sort((a, b) => a.priority - b.priority);

  return {
    conceptId: schema.concept,
    language,
    patterns,
  };
}

// ---------------------------------------------------------------------------
// 5. Loader — schema-driven concept detectors
// ---------------------------------------------------------------------------

const detectorCache = new Map<string, ConceptDetector[]>();

/** Concept IDs that had no detect patterns (missing schema or emit-only). Keyed by language. */
const detectorGaps = new Map<string, string[]>();

/**
 * Get concepts missing detect patterns for a language.
 * Populated after loadConceptDetectors() runs.
 * Returns empty array if not yet loaded or all concepts have patterns.
 */
export function getDetectorGaps(language: string): string[] {
  return detectorGaps.get(language) ?? [];
}

/**
 * Load all concept detectors for a language from R5_patterns schemas.
 *
 * Reads each concept directory (11 dirs, names from CONCEPT_ORDER),
 * parses the {language}.jsonc file, compiles detect regex into ConceptDetector[].
 *
 * Graceful degradation:
 *   - Missing schema file → skip concept (returns detector with empty patterns)
 *   - No detect field → skip concept (returns detector with empty patterns)
 *   - Invalid regex → throws (schema author error, must be fixed)
 *
 * Results are cached per language. Second call returns cached instance.
 *
 * @param language - Language identifier (e.g., "rust", "go")
 * @returns Array of 11 ConceptDetector entries (one per concept, order matches CONCEPT_ORDER)
 */
export async function loadConceptDetectors(language: string): Promise<ConceptDetector[]> {
  // Cache hit
  const cached = detectorCache.get(language);
  if (cached) return cached;

  const pipeline = getDefaultPipeline();
  const detectors: ConceptDetector[] = [];
  const gaps: string[] = [];

  for (const conceptId of CONCEPT_ORDER) {
    const entry = CONCEPT_REGISTRY[conceptId];
    if (!entry) {
      // Concept in CONCEPT_ORDER but not in registry — defensive skip
      detectors.push({ conceptId, language, patterns: [] });
      continue;
    }

    const schemaId = `code/R5_patterns/${entry.dirName}/${language}.jsonc`;

    try {
      const text = await pipeline.getText(schemaId);
      const data = parseJsonc(text) as unknown as RawConceptSchema;

      // Validate the schema identifies itself correctly
      if (data.concept && data.concept !== conceptId) {
        // Schema concept field doesn't match expected — use it anyway but log opportunity
        // for schema correction (not a blocker for detection)
      }

      const detector = compileDetector(data, language);
      // Ensure conceptId matches our iteration (schema may use different casing)
      detectors.push({
        ...detector,
        conceptId,
      });

      // T00-065: Schema loaded but has zero detect patterns (emit-only)
      if (detector.patterns.length === 0) {
        gaps.push(conceptId);
      }
    } catch (e) {
      // T00-064 (invalid regex) should propagate — it's a schema author bug
      if (e instanceof ToolError) throw e;

      // Schema not found or unreadable — graceful degradation.
      // This is expected when a language hasn't added detect patterns yet.
      detectors.push({ conceptId, language, patterns: [] });
      gaps.push(conceptId);
    }
  }

  // Store diagnostics: T00-065 gaps and T00-063 summary
  if (gaps.length > 0) {
    detectorGaps.set(language, gaps);
  }

  detectorCache.set(language, detectors);
  return detectors;
}

/**
 * Get a specific concept detector from the loaded set.
 *
 * Convenience wrapper — loads all detectors for the language (cached),
 * then returns the one matching the concept ID.
 *
 * @param language  - Language identifier
 * @param conceptId - Concept to find (e.g., "function_call")
 * @returns The matching detector, or undefined if concept not found
 */
export async function getConceptDetector(
  language: string,
  conceptId: string,
): Promise<ConceptDetector | undefined> {
  const detectors = await loadConceptDetectors(language);
  return detectors.find((d) => d.conceptId === conceptId);
}

/**
 * Check how many concepts have at least one detect pattern for a language.
 *
 * Useful for diagnostics: "Rust: 11/11 concepts have detect patterns"
 * vs "Go: 0/11 concepts have detect patterns (emit-only)".
 *
 * @param language - Language identifier
 * @returns Object with total concepts, concepts with detect, and concepts without
 */
export async function detectCoverage(language: string): Promise<{
  total: number;
  withDetect: number;
  withoutDetect: number;
  coverage: string[];
  gaps: string[];
}> {
  const detectors = await loadConceptDetectors(language);
  const coverage: string[] = [];
  const gaps: string[] = [];

  for (const d of detectors) {
    if (d.patterns.length > 0) {
      coverage.push(d.conceptId);
    } else {
      gaps.push(d.conceptId);
    }
  }

  return {
    total: detectors.length,
    withDetect: coverage.length,
    withoutDetect: gaps.length,
    coverage,
    gaps,
  };
}

/** Clear concept detector cache and diagnostics. */
export function clearConceptDetectorCache(): void {
  detectorCache.clear();
  detectorGaps.clear();
}
registerCache("data/concept-detectors", clearConceptDetectorCache);

// ============================================================================
// CLOSING
// ============================================================================
//
// Concept detector loader — reads Bereshit's R[5] schema truth, compiles
// detect regex into typed objects for the linter to consume.
//
// The linter is ONE consumer. The transpiler reads "emit" from the same
// schemas. Same 11 concepts, same schema files, different consumers.
// Change the schema, both tools change behavior.
//
// Graceful degradation means the loader handles the bidirectional gap:
// Rust has detect (linter works), Go has emit only (loader returns empty).
// When Go gets detect patterns (Phase 0), the loader picks them up
// automatically — no code changes needed. Data-driven.
//
// "For as the body is one, and hath many members, and all the members of
//  that one body, being many, are one body." — 1 Corinthians 12:12
// ============================================================================
