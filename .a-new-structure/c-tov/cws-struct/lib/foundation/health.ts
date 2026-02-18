// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/health.ts
// key:     B-tov-cws-struct-lib-foundation-health
// title:   CWS Struct — Health Scoring
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-17
// authors: Nova Dawn (CPI-SI)
// purpose: Health scoring system — atomic actions, containers, blocks, files.
//          Extracted from types.ts during lib/ restructuring.
//          No hardcoded weights. Everything computed from actual observations.
//
// biblical_foundation: "Diverse weights, and diverse measures, both of them
//   are alike abomination to the LORD." — Proverbs 20:10
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { Severity } from "./types.ts";

// ---------------------------------------------------------------------------
// Health Scoring — True scores from atomic actions
// ---------------------------------------------------------------------------
//
// Every lint check is an atomic action: it either passes or fails.
// Failure weighs heavier than success — more ways to fail than succeed.
//
// Container = section (I1_core, X1_policy, SETUP, etc.)
// Block = METADATA, CONTENT/SETUP, BODY, CLOSING
// File health = aggregate of block scores
//
// Scoring formula (asymmetric):
//   earned = passes × 1.0
//   penalty = errors × 2.0 + warnings × 1.0 + infos × 0.25
//   score = max(0, (earned - penalty) / total_checks) × 100
//
// This means one error in 10 checks → 70%, not 90%.
// Failure has consequences. Truth in measurement.

/** Single atomic action — one check, pass or fail. */
export interface AtomicAction {
  /** What was checked: "I1_core.key", "SETUP/order", "X1_policy/never" */
  check: string;
  /** Which container: "I1_core", "SETUP", "structural" */
  container: string;
  /** Which block: "metadata", "content", "setup", "body", "closing" */
  block: string;
  /** Did it pass? */
  passed: boolean;
  /** Severity if failed (null if passed). */
  severity?: Severity;
  /** Why it failed (null if passed). */
  reason?: string;
}

/** Per-section (container) score — drill-down to see exactly what's wrong. */
export interface ContainerScore {
  /** Section name: "I1_core", "X1_policy", "SETUP", etc. */
  section: string;
  /** Parent block: "metadata", "content", "setup", "body", "closing" */
  block: string;
  /** Total atomic actions in this container. */
  total: number;
  /** Actions that passed. */
  passed: number;
  /** Actions that failed (by severity). */
  failedErrors: number;
  failedWarnings: number;
  failedInfos: number;
  /** Computed score: 0-100, asymmetric (failures cost more). */
  score: number;
  /** Atomic action detail — the WHY for each pass/fail. */
  actions: AtomicAction[];
}

/** Per-block score — aggregated from containers. */
export interface BlockScore {
  /** Block name: "metadata", "content", "setup", "body", "closing" */
  block: string;
  /** Container scores within this block. */
  containers: ContainerScore[];
  /** Block-level score: 0-100, weighted from container scores. */
  score: number;
}

/** File-level health — the true score. */
export interface HealthScore {
  /** Overall health: 0-100. */
  total: number;
  /** Per-block breakdown. */
  blocks: BlockScore[];
  /** Quick summary counts. */
  totalActions: number;
  passCount: number;
  failCount: number;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Health Score computation — the algorithm
// ---------------------------------------------------------------------------
//
// No hardcoded weights. Everything computed from actual observations.
// The scoring formula is asymmetric: failure costs more than success gains.
// This grows — the basics are: track every atomic action, compute from reality.
//
// "Diverse weights, and diverse measures, both of them are alike
//  abomination to the LORD." — Proverbs 20:10
// The weights are not ours to set. They emerge from the structure itself.

/**
 * Compute a container score from its atomic actions.
 * Asymmetric: errors cost 2×, warnings cost 1×, infos cost 0.25×.
 * Success earns 1× per pass. Score = max(0, (earned - penalty) / total) × 100.
 */
export function computeContainerScore(
  section: string,
  block: string,
  actions: AtomicAction[],
): ContainerScore {
  const total = actions.length;
  if (total === 0) {
    return {
      section, block, total: 0, passed: 0,
      failedErrors: 0, failedWarnings: 0, failedInfos: 0,
      score: 100, actions,
    };
  }

  const passed = actions.filter((a) => a.passed).length;
  const failedErrors = actions.filter((a) => !a.passed && a.severity === "error").length;
  const failedWarnings = actions.filter((a) => !a.passed && a.severity === "warn").length;
  const failedInfos = actions.filter((a) => !a.passed && a.severity === "info").length;

  // Asymmetric scoring: failures cost more than successes gain
  const earned = passed * 1.0;
  const penalty = failedErrors * 2.0 + failedWarnings * 1.0 + failedInfos * 0.25;
  const raw = (earned - penalty) / total;
  const score = Math.round(Math.max(0, Math.min(1, raw)) * 100);

  return {
    section, block, total, passed,
    failedErrors, failedWarnings, failedInfos,
    score, actions,
  };
}

/**
 * Compute a block score from its containers.
 * Each container contributes equally — the algorithm doesn't privilege sections.
 */
export function computeBlockScore(
  block: string,
  containers: ContainerScore[],
): BlockScore {
  if (containers.length === 0) {
    return { block, containers, score: 0 };
  }
  const avg = containers.reduce((sum, c) => sum + c.score, 0) / containers.length;
  return { block, containers, score: Math.round(avg) };
}

/**
 * Compute file health from block scores.
 * All blocks contribute equally — no hardcoded weights.
 */
export function computeHealthScore(blocks: BlockScore[]): HealthScore {
  if (blocks.length === 0) {
    return { total: 0, blocks, totalActions: 0, passCount: 0, failCount: 0 };
  }

  const avg = blocks.reduce((sum, b) => sum + b.score, 0) / blocks.length;

  // Count all actions across all containers across all blocks
  let totalActions = 0;
  let passCount = 0;
  let failCount = 0;
  for (const b of blocks) {
    for (const c of b.containers) {
      totalActions += c.total;
      passCount += c.passed;
      failCount += c.total - c.passed;
    }
  }

  return {
    total: Math.round(avg),
    blocks,
    totalActions,
    passCount,
    failCount,
  };
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Health scoring is the measurement system — asymmetric because failure
// should cost more than success gains. Truth in measurement.
//
// "Diverse weights, and diverse measures, both of them are alike
//  abomination to the LORD." — Proverbs 20:10
// ============================================================================
