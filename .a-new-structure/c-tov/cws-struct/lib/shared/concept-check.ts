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
import type { TypingBlockProfile } from "../foundation/code-schema.ts";
import type { TernaryValue, BlockName } from "../data/types.ts";
import type { ConceptDetector } from "../data/concept-detectors.ts";
import { detectConcept } from "../data/concept-detectors.ts";
import { SECTION_REGISTRY, SECTION_ORDER, CONCEPT_ORDER } from "../data/mod.ts";
import {
  detectConceptMultiline,
  detectConceptByScope,
  hasR3Detection,
} from "./scope-analysis.ts";
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
  const language = detectors[0]?.language ?? "";

  for (const detector of detectors) {
    const conceptId = detector.conceptId;
    const expectation = conceptMap[conceptId];

    // Skip if concept not in map (shouldn't happen, but defensive)
    if (expectation === undefined) continue;

    // Defer = context-dependent, linter can't decide → skip
    if (expectation === "defer") continue;

    // R[5]: Line-by-line regex detection
    let found = false;
    if (detector.patterns.length > 0) {
      found = codeLines.some((line) => detectConcept(detector, line));
    }

    // R[3] fallback: scope-aware detection when R[5] didn't find it.
    // Two strategies:
    //   1. Multiline: join lines and re-test regex (e.g., recursion backreference)
    //   2. Scope: parse function declarations and check cross-line patterns
    if (!found && hasR3Detection(conceptId, language)) {
      // Try multiline text matching first (works with existing regex patterns)
      if (detector.patterns.length > 0) {
        found = detectConceptMultiline(detector, codeLines);
      }
      // If still not found, try scope-level semantic analysis
      if (!found) {
        found = detectConceptByScope(conceptId, containerLines, language);
      }
    }

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
// Typing-aware concept map overlay
// ---------------------------------------------------------------------------

/**
 * Options for buildConceptContainers() — typing-aware concept narrowing.
 *
 * When a typing arrow is present (e.g., -module->utility), the concept map
 * is overlaid based on the section's status in the typing profile:
 *
 *   required   → Keep base map, then apply concept_overrides if present.
 *   available  → Soften: all granted → defer (present but not expected to be complete).
 *   irrelevant → Skip: all → defer (section isn't meaningful for this typing).
 *   not listed → For CLOSING sections not in SETUP/BODY typing: soften (tests are verification).
 *
 * This is bidirectional verification:
 *   granted-granted = HALT (verified present)
 *   denied-denied   = HALT (verified absent)
 *   defer-defer     = HALT (human review — linter can't decide)
 */
export interface ConceptContainerOptions {
  /** Typing profile for the current block (SETUP or BODY from TypingProfile). */
  typingBlock?: TypingBlockProfile;
}

/**
 * Apply typing-aware overlay to a concept map.
 *
 * Normalization bridge: typing maps use display labels ("Identity Access",
 * "Free Functions") from PascalCase schema keys. Section registry uses
 * kebab-case tags ("identity-access", "free-functions"). Both are normalized
 * to lowercase-no-separators for matching: "identityaccess", "freefunctions".
 *
 * @param baseMap       Base concept map from SECTION_REGISTRY
 * @param sectionTag    Kebab-case section tag (e.g., "identity-access")
 * @param typingBlock   Typing profile block (SETUP or BODY)
 * @param block         Block name for CLOSING heuristic
 * @returns Overlaid concept map (new object, base not mutated)
 */
function applyTypingOverlay(
  baseMap: Record<string, TernaryValue>,
  sectionTag: string,
  typingBlock: TypingBlockProfile | undefined,
  block: BlockName,
): Record<string, TernaryValue> {
  // No typing → no overlay
  if (!typingBlock) return baseMap;

  // Normalize to lowercase, strip spaces AND hyphens for cross-format matching.
  // "Identity Access" → "identityaccess", "identity-access" → "identityaccess"
  const norm = (s: string) => s.toLowerCase().replace(/[\s-]+/g, "");
  const normTag = norm(sectionTag);

  // Check section status: required / available / irrelevant
  const isRequired = typingBlock.required.some((s) => norm(s) === normTag);
  const isAvailable = typingBlock.available.some((s) => norm(s) === normTag);
  const isIrrelevant = typingBlock.irrelevant.some((s) => norm(s) === normTag);

  // Irrelevant sections: all concepts → defer (skip entirely)
  if (isIrrelevant) {
    const map: Record<string, TernaryValue> = {};
    for (const key of Object.keys(baseMap)) map[key] = "defer";
    return map;
  }

  // Available sections: soften — all granted → defer
  // (Section is present but not expected to exhibit all patterns)
  if (isAvailable) {
    const map: Record<string, TernaryValue> = {};
    for (const [key, val] of Object.entries(baseMap)) {
      map[key] = val === "granted" ? "defer" : val;
    }
    return map;
  }

  // Required sections: keep base map, apply concept_overrides
  if (isRequired) {
    // Check for per-section concept overrides
    // Schema uses PascalCase keys in concept_overrides (e.g., "FreeFunctions")
    // Find by normalizing both sides
    if (typingBlock.conceptOverrides) {
      for (const [overrideKey, overrides] of Object.entries(typingBlock.conceptOverrides)) {
        if (norm(overrideKey) === normTag) {
          const map = { ...baseMap };
          for (const [conceptId, newValue] of Object.entries(overrides)) {
            if (conceptId in map) {
              map[conceptId] = newValue as TernaryValue;
            }
          }
          return map;
        }
      }
    }
    return baseMap;
  }

  // Section not listed in typing (e.g., CLOSING sections when typing only covers BODY).
  // For CLOSING block: soften (tests/validation verify BODY, not primary code).
  // For other unlisted: keep base map (conservative).
  if (block === "closing") {
    const map: Record<string, TernaryValue> = {};
    for (const [key, val] of Object.entries(baseMap)) {
      map[key] = val === "granted" ? "defer" : val;
    }
    return map;
  }

  return baseMap;
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
 *   4. Apply typing-aware concept map overlay (if typing present)
 *   5. Create a CheckFn that runs concept validation
 *
 * The returned CheckFn[] are closures that capture the detection context.
 * The lint grid calls them during traversal — lazy evaluation.
 *
 * @param block        Block name ("metadata", "setup", "body", "closing")
 * @param file         File path for result attribution
 * @param blockLines   Lines within this block (from getBlockLines)
 * @param ranges       Subsection ranges (from getSubsectionRanges)
 * @param detectors    Loaded concept detectors for the language
 * @param opts         Typing context for concept map overlays
 * @returns ContainerCheckSet[] — one per section that has a concept map
 */
export function buildConceptContainers(
  block: BlockName,
  file: string,
  blockLines: string[],
  ranges: SubsectionRange[],
  detectors: ConceptDetector[],
  opts?: ConceptContainerOptions,
): ContainerCheckSet[] {
  const containers: ContainerCheckSet[] = [];
  const sectionTags = SECTION_ORDER[block];

  if (!sectionTags) return containers;

  for (const tag of sectionTags) {
    const section = SECTION_REGISTRY[tag];
    if (!section) continue;

    // Apply typing-aware concept map overlay.
    // Use TAG (kebab-case: "identity-access") — matches typing labels via norm().
    const conceptMap = applyTypingOverlay(
      section.conceptMap, tag, opts?.typingBlock, block,
    );

    // Collect defer-defer pairs: defer in BASE map AND still defer after overlay.
    // These are concepts the machine inherently cannot resolve — human review needed.
    // granted→defer (from typing) is NOT a defer-defer: the typing resolved it.
    // Only base-defer AND overlay-defer = true unresolvable pair.
    const deferPairs: string[] = [];
    for (const cid of CONCEPT_ORDER) {
      const baseVal = section.conceptMap[cid];
      const overlayVal = conceptMap[cid];
      if (baseVal === "defer" && overlayVal === "defer") {
        deferPairs.push(cid);
      }
    }

    // Skip sections where ALL concepts are deferred AND no review items
    const hasActionable = CONCEPT_ORDER.some((cid) => {
      const val = conceptMap[cid];
      return val === "granted" || val === "denied";
    });
    if (!hasActionable && deferPairs.length === 0) continue;

    // Find container lines via tag normalization
    const lines = getContainerLines(blockLines, ranges, tag);

    // Create closure check functions
    const checks: CheckFn[] = [];

    // Primary: concept validation (granted/denied checks)
    if (hasActionable) {
      checks.push(() => validateContainerConcepts(
        file, tag, lines, conceptMap, detectors,
      ));
    }

    // Review: defer-defer pairs surface for human judgment.
    // Only emit when container has actual content — empty containers have nothing to review.
    if (deferPairs.length > 0 && lines.length > 0) {
      checks.push(() => deferPairs.map((cid) =>
        info(file, `review/concept/${tag}/${cid}`,
          `Concept "${cid}" is deferred in "${tag}" — human review recommended`,
          { container: tag },
        )
      ));
    }

    // Only push containers that have actual work to do
    if (checks.length > 0) {
      containers.push({
        container: tag,
        checks,
      });
    }
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
