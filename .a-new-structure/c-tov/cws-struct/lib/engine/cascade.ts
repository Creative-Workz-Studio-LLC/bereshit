// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/engine/cascade.ts
// key:     B-tov-cws-struct-lib-engine-cascade
// title:   CWS Struct — Pipeline Cascade Logic
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-22
// updated: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Reusable cascade logic for the 4-layer lint pipeline.
//          When Layer N has a structural failure, downstream layers
//          produce neutral results — root cause surfaces first.
//
//          This replaces the inline cascade blocks in go.ts and rust.ts
//          health scorers with a shared, data-driven implementation.
//
//          Genesis 1 principle: boundaries before content. Can't check
//          content if the boundaries are wrong.
//
// biblical_foundation: "For precept must be upon precept, precept upon precept;
//   line upon line, line upon line." — Isaiah 28:10
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { AtomicAction } from "../foundation/mod.ts";
import type { LintResult } from "../foundation/mod.ts";
import type { PipelineLayer } from "../data/mod.ts";
import { isDebug, debugCascade } from "./debug.ts";

// ---------------------------------------------------------------------------
// Cascade result types
// ---------------------------------------------------------------------------

/** Result of running one pipeline layer. */
export interface LayerResult {
  /** Which layer this result is for. */
  layer: PipelineLayer;

  /** Lint results from checks in this layer. */
  results: LintResult[];

  /** Atomic actions from checks in this layer (for health scoring). */
  actions: AtomicAction[];

  /**
   * Whether this layer passed structurally.
   *
   * A layer "fails structurally" when its findings indicate that
   * downstream layers cannot produce meaningful results:
   * - Layer 0: missing blocks → can't check sections
   * - Layer 1: missing required sections → can't check content
   * - Layer 2: content in wrong blocks → can't check patterns
   *
   * Determined by the failure predicate passed to runPipeline().
   */
  passed: boolean;

  /** Block names that failed (for selective per-block cascade). */
  failedBlocks: string[];
}

/** Full pipeline execution result. */
export interface PipelineResult {
  /** Results for each layer, in execution order. */
  layers: LayerResult[];

  /** All lint results across all layers (flattened). */
  allResults: LintResult[];

  /** All atomic actions across all layers (flattened). */
  allActions: AtomicAction[];

  /** Whether any layer triggered a cascade. */
  cascaded: boolean;

  /** Which layer first triggered the cascade (if any). */
  cascadeSource?: number;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Cascade action — flip aligned actions to neutral
// ---------------------------------------------------------------------------

/**
 * Cascade a group of atomic actions to neutral.
 *
 * When a parent layer fails, downstream actions that were aligned (+1)
 * become neutral (0) — they can't be assessed because the prerequisite
 * structural check failed.
 *
 * Actions that were already misaligned (-1) or neutral (0) are preserved —
 * they represent actual findings or already-cascaded state.
 *
 * @param actions - Array of actions to cascade
 * @param reason - Human explanation of why these were cascaded
 * @param sourceLayer - Which layer triggered the cascade
 */
export function cascadeActions(
  actions: AtomicAction[],
  reason: string,
  sourceLayer: number,
): void {
  let neutralized = 0;
  for (const a of actions) {
    if (a.direction > 0) {
      (a as { direction: -1 | 0 | 1 }).direction = 0;
      a.impact = "info";
      a.reason = reason;
      a.layer = sourceLayer as 0 | 1 | 2 | 3;
      neutralized++;
    }
  }
  if (isDebug() && neutralized > 0) {
    debugCascade(reason, neutralized);
  }
}

/**
 * Cascade multiple groups of actions at once.
 *
 * Convenience wrapper for cascading several action arrays with the same reason.
 */
export function cascadeActionGroups(
  groups: AtomicAction[][],
  reason: string,
  sourceLayer: number,
): void {
  for (const group of groups) {
    cascadeActions(group, reason, sourceLayer);
  }
}

// ---------------------------------------------------------------------------
// Selective cascade — per-block cascade for Layer 1
// ---------------------------------------------------------------------------

/**
 * Cascade actions for a specific block only.
 *
 * Layer 1 failures can be per-block. If SETUP sections are disordered
 * but BODY sections are fine, only cascade SETUP-related downstream checks.
 *
 * @param actions - All actions to filter and cascade
 * @param block - Block name to cascade (e.g., "setup", "metadata")
 * @param reason - Human explanation
 * @param sourceLayer - Which layer triggered
 */
export function cascadeBlock(
  actions: AtomicAction[],
  block: string,
  reason: string,
  sourceLayer: number,
): void {
  let neutralized = 0;
  for (const a of actions) {
    if (a.block === block && a.direction > 0) {
      (a as { direction: -1 | 0 | 1 }).direction = 0;
      a.impact = "info";
      a.reason = reason;
      a.layer = sourceLayer as 0 | 1 | 2 | 3;
      neutralized++;
    }
  }
  if (isDebug() && neutralized > 0) {
    debugCascade(`${reason} [block: ${block}]`, neutralized);
  }
}

// ---------------------------------------------------------------------------
// Structural failure detection
// ---------------------------------------------------------------------------

/**
 * Check if a set of lint results indicates structural failure.
 *
 * A layer has "structural failure" when errors indicate downstream layers
 * cannot produce meaningful results.
 *
 * @param results - Lint results from a layer
 * @param errorPatterns - Rule patterns that indicate structural failure
 *                        (e.g., ["block/METADATA", "block/SETUP"])
 * @returns true if any of the error patterns matched
 */
export function hasStructuralFailure(
  results: LintResult[],
  errorPatterns: string[],
): boolean {
  return results.some(
    (r) =>
      r.severity === "error" &&
      errorPatterns.some((p) => r.rule.startsWith(p) || r.rule === p),
  );
}

/**
 * Detect which blocks failed from lint results.
 *
 * Returns the list of block names that have structural errors.
 * Used for selective per-block cascade in Layer 1.
 *
 * @param results - Lint results
 * @param blockRulePrefix - Rule prefix for block presence checks (e.g., "block/")
 * @returns Array of failed block names (uppercase: "METADATA", "SETUP", etc.)
 */
export function detectFailedBlocks(
  results: LintResult[],
  blockRulePrefix: string,
): string[] {
  const failed: string[] = [];
  for (const r of results) {
    if (
      r.severity === "error" &&
      r.rule.startsWith(blockRulePrefix)
    ) {
      // Extract block name from rule (e.g., "block/METADATA" → "METADATA")
      const blockName = r.rule.slice(blockRulePrefix.length);
      if (blockName && !blockName.includes("/") && !failed.includes(blockName)) {
        failed.push(blockName);
      }
    }
  }
  return failed;
}

// ---------------------------------------------------------------------------
// Tag actions with layer info
// ---------------------------------------------------------------------------

/**
 * Tag all actions in a list with their pipeline layer number.
 *
 * Call this after collecting actions from a layer's checks.
 */
export function tagLayer(
  actions: AtomicAction[],
  layer: 0 | 1 | 2 | 3,
): void {
  for (const a of actions) {
    a.layer = layer;
  }
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Cascade logic — the pipeline's immune system.
//
// When a structural check fails, downstream checks that depend on it
// become neutral rather than producing misleading results. Root cause
// surfaces first, noise is eliminated.
//
// "For precept must be upon precept, precept upon precept;
//  line upon line, line upon line." — Isaiah 28:10
//
// Each layer is a precept. Each cascade is a boundary.
// You can't build on a precept that hasn't been established.
// ============================================================================
