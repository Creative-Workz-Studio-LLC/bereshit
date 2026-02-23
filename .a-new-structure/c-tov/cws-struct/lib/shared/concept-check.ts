// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/shared/concept-check.ts
// key:     B-tov-cws-struct-lib-shared-concept-check
// title:   CWS Struct — Concept Validation + Container Check Generator
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-23
// authors: Nova Dawn (CPI-SI)
// purpose: Validate R[5] concept patterns within containers against the
//          462-value ternary matrix, and generate ContainerCheckSet[] for
//          the lint grid.
//
//          Phase 3: validateContainerConcepts() — core validation function
//          Phase 4: buildConceptContainers() — check generator for handlers
//
//          The ternary matrix (42 sections x 11 concepts) defines what's
//          expected in each container. This module enforces those expectations:
//            granted (+1) — pattern expected. If missing, report info.
//            denied  (-1) — pattern forbidden. If found, report warn.
//            defer    (0) — context-dependent, skip.
//
// biblical_foundation: "Try the spirits whether they are of God."
//   — 1 John 4:1
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { LintResult } from "../foundation/mod.ts";
import { info, warn } from "../foundation/mod.ts";
import type { TernaryValue, BlockName } from "../data/types.ts";
import type { ConceptDetector } from "../data/concept-detectors.ts";
import { detectConcept } from "../data/concept-detectors.ts";
import { SECTION_REGISTRY, SECTION_ORDER, CONCEPT_ORDER } from "../data/mod.ts";
import type { SubsectionRange } from "./types.ts";
import type { ContainerCheckSet, CheckFn } from "./code-4block.ts";
import { getContainerLines } from "./code-4block.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Line filtering — strip non-code lines before detection
// ---------------------------------------------------------------------------

/**
 * Filter lines to only those containing potential code patterns.
 *
 * Removes:
 *   - Empty/whitespace-only lines
 *   - Full-line comments (// or #)
 *   - Separator lines (// ====, // ----)
 *   - Block comment delimiters (/* ... *​/)
 *
 * Returns the code-bearing lines for concept detection.
 */
function filterCodeLines(lines: string[]): string[] {
  let inBlockComment = false;
  const result: string[] = [];

  for (const line of lines) {
    const trimmed = line.trim();

    // Track block comments
    if (trimmed.startsWith("/*")) {
      inBlockComment = true;
      if (trimmed.endsWith("*/")) inBlockComment = false;
      continue;
    }
    if (inBlockComment) {
      if (trimmed.endsWith("*/")) inBlockComment = false;
      continue;
    }

    // Skip empty/whitespace
    if (trimmed.length === 0) continue;

    // Skip line comments
    if (trimmed.startsWith("//") || trimmed.startsWith("#")) continue;

    result.push(line);
  }

  return result;
}

// ---------------------------------------------------------------------------
// Core validation — validateContainerConcepts
// ---------------------------------------------------------------------------

/** Options for concept validation. */
export interface ConceptCheckOptions {
  /** Base line number (file line of container start) for absolute positioning. */
  baseLine?: number;
}

/**
 * Validate concept patterns within a container against its concept map.
 *
 * For each of 11 concepts:
 *   granted → expected. If no pattern detected, report info.
 *   denied  → forbidden. If pattern detected, report warn.
 *   defer   → skip (context-dependent, not actionable by linter).
 *
 * Rule namespace: concept/{containerTag}/{conceptId}
 *   e.g., "concept/imports/function_call"
 *
 * @param file          File path for result attribution
 * @param containerTag  Section tag (kebab-case, e.g., "imports")
 * @param containerLines Source lines within this container
 * @param conceptMap    11 ternary values from the section registry
 * @param detectors     Loaded concept detectors for the file's language
 * @param opts          Optional line offset for absolute positioning
 * @returns Lint results for concept violations/advisories
 */
export function validateContainerConcepts(
  file: string,
  containerTag: string,
  containerLines: string[],
  conceptMap: Record<string, TernaryValue>,
  detectors: ConceptDetector[],
  _opts?: ConceptCheckOptions,
): LintResult[] {
  const results: LintResult[] = [];
  const codeLines = filterCodeLines(containerLines);

  for (const detector of detectors) {
    const conceptId = detector.conceptId;
    const expectation = conceptMap[conceptId];

    // Skip if concept not in map (shouldn't happen, but defensive)
    if (expectation === undefined) continue;

    // Defer = context-dependent, linter can't decide → skip
    if (expectation === "defer") continue;

    // Skip detectors with no patterns (language doesn't have detect yet)
    if (detector.patterns.length === 0) continue;

    // Test: does any code line match this concept's patterns?
    const found = codeLines.some((line) => detectConcept(detector, line));
    const rule = `concept/${containerTag}/${conceptId}`;

    if (expectation === "denied" && found) {
      // Violation: forbidden pattern detected in this container
      results.push(warn(file, rule,
        `Denied concept "${conceptId}" detected in "${containerTag}"`,
        { container: containerTag },
      ));
    } else if (expectation === "granted" && !found && codeLines.length > 0) {
      // Advisory: expected pattern not found (only when container has code)
      results.push(info(file, `${rule}/missing`,
        `Expected concept "${conceptId}" not found in "${containerTag}"`,
        { container: containerTag },
      ));
    }
  }

  return results;
}

// ---------------------------------------------------------------------------
// Container check generator — buildConceptContainers
// ---------------------------------------------------------------------------

/**
 * Build ContainerCheckSet[] for one block from the section registry.
 *
 * For each section in the block:
 *   1. Look up section in SECTION_REGISTRY
 *   2. Skip if section not found (reserved or not yet registered)
 *   3. Find container lines using subsection ranges
 *   4. Create a CheckFn that runs concept validation
 *
 * The returned CheckFn[] are closures that capture the detection context.
 * The lint grid calls them during traversal — lazy evaluation.
 *
 * @param block        Block name ("metadata", "setup", "body", "closing")
 * @param file         File path for result attribution
 * @param blockLines   Lines within this block (from getBlockLines)
 * @param ranges       Subsection ranges (from getSubsectionRanges)
 * @param detectors    Loaded concept detectors for the language
 * @returns ContainerCheckSet[] — one per section that has a concept map
 */
export function buildConceptContainers(
  block: BlockName,
  file: string,
  blockLines: string[],
  ranges: SubsectionRange[],
  detectors: ConceptDetector[],
): ContainerCheckSet[] {
  const containers: ContainerCheckSet[] = [];
  const sectionTags = SECTION_ORDER[block];

  if (!sectionTags) return containers;

  for (const tag of sectionTags) {
    const section = SECTION_REGISTRY[tag];
    if (!section) continue;

    // Skip sections where ALL concepts are deferred (nothing to check)
    const hasActionable = CONCEPT_ORDER.some((cid) => {
      const val = section.conceptMap[cid];
      return val === "granted" || val === "denied";
    });
    if (!hasActionable) continue;

    // Find container lines via tag normalization
    const lines = getContainerLines(blockLines, ranges, tag);

    // Create closure check function
    const checkFn: CheckFn = () => {
      return validateContainerConcepts(
        file,
        tag,
        lines,
        section.conceptMap,
        detectors,
      );
    };

    containers.push({
      container: tag,
      checks: [checkFn],
    });
  }

  return containers;
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Concept validation — the bridge between Bereshit's ternary matrix truth
// and the lint grid's container-level detection.
//
// validateContainerConcepts() is the core:
//   42 sections × 11 concepts = 462 ternary values → actionable lint results.
//   granted + missing → info (advisory).
//   denied  + found   → warn (violation).
//   defer             → skip (linter can't decide).
//
// buildConceptContainers() is the generator:
//   Takes block context + section registry → ContainerCheckSet[].
//   Handlers call this once per block. Grid processes them during traversal.
//
// "Try the spirits whether they are of God." — 1 John 4:1
// ============================================================================
