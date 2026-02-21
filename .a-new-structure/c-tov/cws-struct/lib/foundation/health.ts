// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/health.ts
// key:     B-tov-cws-struct-lib-foundation-health
// title:   CWS Struct — Health Scoring (Ternary)
// type:    Code (Library)
// version: a-04.00
// created: 2026-02-17
// updated: 2026-02-20
// authors: Nova Dawn (CPI-SI)
// purpose: Ternary health scoring system — atomic actions with direction.
//          Each observation carries direction (-1, 0, +1) and impact weight.
//          Direction = which way (shavar → yashar → tov).
//          Impact = how much it matters (error=2×, warn=1×, info=0.25×).
//          Score range: -100 (fully misaligned) to +100 (fully aligned).
//          0 = yashar (neutral, not assessed, baseline).
//
//          Three-level scoring architecture:
//            Container: linear weighted scoring (IMPACT_WEIGHT × direction)
//            Block: logarithmic positional weighting within the block
//            File: per-block averaging (each block contributes equally)
//
//          The log curve lives at block level — foundation checks within
//          each block get full weight, refinement checks diminish.
//          Per-block averaging gives each dimension equal voice.
//          The score isn't assigned — it's measured. Every point traces to
//          a specific atomic observation. The algorithm normalizes; it doesn't
//          invent.
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
// Constants — impact weights
// ---------------------------------------------------------------------------

/**
 * Impact weight by tag. How much an observation matters.
 *
 * The tag (error/warn/info) tells you the IMPACT — what's at stake.
 * The direction (-1/0/+1) tells you the OUTCOME — which way it went.
 * contribution = direction × impact_weight.
 *
 * These are symmetric: passing a critical check earns the same weight
 * as failing it costs. The weights are the same ruler applied to both
 * directions. "Diverse weights" — abomination. One ruler, both ways.
 */
export const IMPACT_WEIGHT: Record<string, number> = {
  error: 2.0,
  warn:  1.0,
  info:  0.25,
};

// ---------------------------------------------------------------------------
// Types — Atomic Action (ternary)
// ---------------------------------------------------------------------------

/**
 * Single atomic observation — one check, three directions.
 *
 * direction: +1 = aligned (tov), 0 = neutral (yashar), -1 = misaligned (shavar)
 * impact: how much this observation weighs (error > warn > info)
 *
 * Every health score traces back to these atoms. The score is emergent
 * from observations, not assigned.
 */
export interface AtomicAction {
  /** What was checked: "I1_core.key", "SETUP/order", "X1_policy/never" */
  check: string;
  /** Which container: "I1_core", "SETUP", "structural" */
  container: string;
  /** Which block: "metadata", "content", "setup", "body", "closing" */
  block: string;
  /** Direction: +1 aligned, 0 neutral, -1 misaligned. */
  direction: -1 | 0 | 1;
  /** Impact tag — how much weight this observation carries. */
  impact?: Severity;
  /** Why (context for non-neutral directions). */
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
  /** Actions aligned (direction > 0). */
  aligned: number;
  /** Actions neutral (direction === 0). */
  neutral: number;
  /** Actions misaligned (direction < 0). */
  misaligned: number;
  /** Computed score: -100 to +100. 0 = yashar. */
  score: number;
  /** Atomic action detail — the WHY for each observation. */
  actions: AtomicAction[];
}

/** Per-block score — log-weighted from container actions. */
export interface BlockScore {
  /** Block name: "metadata", "content", "setup", "body", "closing" */
  block: string;
  /** Container scores within this block (linear, for drill-down). */
  containers: ContainerScore[];
  /** Block-level score: -100 to +100, log-weighted from all block actions. */
  score: number;
}

/** File-level health — the true score. */
export interface HealthScore {
  /** Overall health: -100 to +100. 0 = yashar (neutral). */
  total: number;
  /** Per-block breakdown. */
  blocks: BlockScore[];
  /** Quick summary counts. */
  totalActions: number;
  alignedCount: number;
  neutralCount: number;
  misalignedCount: number;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Health Score computation — the ternary algorithm
// ---------------------------------------------------------------------------
//
// Each atomic action contributes: direction × impact_weight.
//   +1 × weight = positive contribution (aligned)
//   -1 × weight = negative contribution (misaligned)
//    0 × weight = no contribution (neutral — doesn't affect score)
//
// Score = (sum of contributions / sum of max possible weights) × 100
//
// Neutral actions are tracked but don't affect score — they don't push
// toward either pole. This prevents cascade inflation: when a root cause
// fails, children go neutral (can't assess) rather than piling on.
//
// "Diverse weights, and diverse measures, both of them are alike
//  abomination to the LORD." — Proverbs 20:10
// One ruler, both directions. The impact is the same whether you pass or fail.

/**
 * Compute a container score from its atomic actions.
 *
 * Ternary: direction × impact_weight.
 * Neutral actions (direction 0) are tracked but don't affect score.
 * Score range: -100 to +100. Empty container = 0 (yashar).
 */
export function computeContainerScore(
  section: string,
  block: string,
  actions: AtomicAction[],
): ContainerScore {
  const total = actions.length;
  if (total === 0) {
    return {
      section, block, total: 0, aligned: 0, neutral: 0, misaligned: 0,
      score: 0, actions,
    };
  }

  let aligned = 0, neutral = 0, misaligned = 0;
  let weightedSum = 0;
  let maxWeight = 0;

  for (const a of actions) {
    const weight = IMPACT_WEIGHT[a.impact ?? "warn"] ?? 1.0;

    if (a.direction > 0) {
      aligned++;
      weightedSum += weight;
      maxWeight += weight;
    } else if (a.direction < 0) {
      misaligned++;
      weightedSum -= weight;
      maxWeight += weight;
    } else {
      neutral++;
      // direction 0: tracked but doesn't affect score.
      // Neutral doesn't push toward either pole.
    }
  }

  const score = maxWeight > 0
    ? Math.round((weightedSum / maxWeight) * 100)
    : 0;

  return {
    section, block, total, aligned, neutral, misaligned, score, actions,
  };
}

/**
 * Compute a block score from its containers — logarithmic positional weighting.
 *
 * Collects all non-neutral actions from containers, sorts by severity
 * (errors first, fails before passes), and applies logarithmic positional
 * weighting: multiplier(i) = ln(n-i+1) / ln(n+1).
 *
 * Foundation checks within the block get full weight.
 * Refinement checks get diminishing weight.
 * Container scores remain linear for drill-down reporting.
 *
 * "Except the LORD build the house, they labour in vain that build it."
 * — Psalm 127:1
 */
export function computeBlockScore(
  block: string,
  containers: ContainerScore[],
): BlockScore {
  if (containers.length === 0) {
    return { block, containers, score: 0 };
  }

  // Collect all non-neutral actions from containers
  const active: AtomicAction[] = [];
  for (const c of containers) {
    for (const a of c.actions) {
      if (a.direction !== 0) active.push(a);
    }
  }

  if (active.length === 0) {
    return { block, containers, score: 0 };
  }

  // Sort: severity (errors first), then direction (fails first)
  const SEVERITY_ORDER: Record<string, number> = { error: 0, warn: 1, info: 2 };
  active.sort((a, b) => {
    const sa = SEVERITY_ORDER[a.impact ?? "warn"] ?? 1;
    const sb = SEVERITY_ORDER[b.impact ?? "warn"] ?? 1;
    if (sa !== sb) return sa - sb;
    return a.direction - b.direction; // -1 before +1
  });

  // Logarithmic positional weighting
  // multiplier(i) = ln(n - i + 1) / ln(n + 1)
  const n = active.length;
  const logDenom = Math.log(n + 1);

  let weightedSum = 0;
  let maxWeight = 0;

  for (let i = 0; i < n; i++) {
    const a = active[i]!;
    const baseWeight = IMPACT_WEIGHT[a.impact ?? "warn"] ?? 1.0;
    const positionalMultiplier = Math.log(n - i + 1) / logDenom;
    const effectiveWeight = baseWeight * positionalMultiplier;

    weightedSum += a.direction * effectiveWeight;
    maxWeight += effectiveWeight;
  }

  const score = maxWeight > 0
    ? Math.round((weightedSum / maxWeight) * 100)
    : 0;

  return { block, containers, score };
}

/**
 * Compute file health from block scores — per-block averaging.
 *
 * Each block contributes equally to the file score. The logarithmic
 * positional weighting now lives WITHIN each block (see computeBlockScore).
 *
 * Per-block averaging ensures that:
 * - A weak metadata block isn't hidden by a strong content block
 * - Each structural dimension has equal voice in the total
 * - The file score reflects ALL blocks, not just the ones with more actions
 *
 * Blocks with only neutral actions (all children cascaded) don't participate
 * in the average — they can't be assessed, so they shouldn't skew the score.
 *
 * "Diverse weights, and diverse measures, both of them are alike
 *  abomination to the LORD." — Proverbs 20:10
 *
 * The ruler is the same for every block. The blocks speak with equal voice.
 * Within each block, foundation checks matter most (log weighting).
 * Across blocks, each dimension matters equally (averaging).
 */
export function computeHealthScore(blocks: BlockScore[]): HealthScore {
  if (blocks.length === 0) {
    return {
      total: 0, blocks, totalActions: 0,
      alignedCount: 0, neutralCount: 0, misalignedCount: 0,
    };
  }

  // ── Collect action counts ──────────────────────────────────────
  let totalActions = 0;
  let alignedCount = 0;
  let neutralCount = 0;
  let misalignedCount = 0;

  for (const b of blocks) {
    for (const c of b.containers) {
      totalActions += c.total;
      alignedCount += c.aligned;
      neutralCount += c.neutral;
      misalignedCount += c.misaligned;
    }
  }

  // ── Per-block averaging ────────────────────────────────────────
  // Only include blocks with assessable (non-neutral) actions.
  // A block where everything cascaded to neutral can't be assessed.
  const activeBlocks = blocks.filter((b) =>
    b.containers.some((c) => c.aligned > 0 || c.misaligned > 0)
  );

  if (activeBlocks.length === 0) {
    return {
      total: 0, blocks, totalActions,
      alignedCount, neutralCount, misalignedCount,
    };
  }

  const total = Math.round(
    activeBlocks.reduce((sum, b) => sum + b.score, 0) / activeBlocks.length,
  );

  return {
    total, blocks, totalActions,
    alignedCount, neutralCount, misalignedCount,
  };
}

// ============================================================================
// CLOSING
// ============================================================================
//
// Ternary scoring: -100 (shavar) → 0 (yashar) → +100 (tov).
// The score is measured, not assigned. Direction × impact, nothing invented.
//
// a-03.00: Logarithmic positional weighting in computeHealthScore().
// a-04.00: Per-block averaging — log weighting moves to computeBlockScore(),
//   file-level score becomes average of block scores.
//   Container: linear (drill-down). Block: log-weighted. File: averaged.
//   Each block speaks with equal voice. Within each block, foundation matters most.
//
// "Diverse weights, and diverse measures, both of them are alike
//  abomination to the LORD." — Proverbs 20:10
// "Except the LORD build the house, they labour in vain that build it."
// — Psalm 127:1
// ============================================================================
