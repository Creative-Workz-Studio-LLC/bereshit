// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/health.ts
// key:     B-tov-cws-struct-lib-foundation-health
// title:   CWS Struct — Health Scoring (Ternary)
// type:    Code (Library)
// version: a-03.00
// created: 2026-02-17
// updated: 2026-02-18
// authors: Nova Dawn (CPI-SI)
// purpose: Ternary health scoring system — atomic actions with direction.
//          Each observation carries direction (-1, 0, +1) and impact weight.
//          Direction = which way (shavar → yashar → tov).
//          Impact = how much it matters (error=2×, warn=1×, info=0.25×).
//          Score range: -100 (fully misaligned) to +100 (fully aligned).
//          0 = yashar (neutral, not assessed, baseline).
//
//          File-level scoring uses LOGARITHMIC positional weighting:
//          actions sorted by severity, multiplier = ln(n-i+1) / ln(n+1).
//          Foundation checks get full weight; refinement checks diminish.
//          dH/dn = k/n — the calculus of diminishing marginal contribution.
//
//          Container/block scores remain linear for drill-down.
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

/** Per-block score — aggregated from containers. */
export interface BlockScore {
  /** Block name: "metadata", "content", "setup", "body", "closing" */
  block: string;
  /** Container scores within this block. */
  containers: ContainerScore[];
  /** Block-level score: -100 to +100, average of container scores. */
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
 * Compute file health from block scores — logarithmic positional weighting.
 *
 * Instead of averaging block scores, this collects ALL non-neutral actions,
 * sorts them by severity (errors → warns → infos) and direction (fails first),
 * then applies a logarithmic positional multiplier:
 *
 *   multiplier(i) = ln(n - i + 1) / ln(n + 1)
 *
 * Position 0 (first error)   → multiplier ≈ 1.0   (full weight)
 * Position n-1 (last info)   → multiplier ≈ 0.15   (minimal weight)
 *
 * The calculus: dH/dn = k/n — each additional check contributes less than
 * the previous. Foundational checks (structural errors) carry disproportionate
 * weight. Refinement checks (cosmetic infos) provide diminishing returns.
 *
 * This is Genesis 1:1 as a weight curve: the foundation matters most.
 * The ruler is still impartial (Proverbs 20:10) — same weight for pass and
 * fail at the same position. But POSITION reflects importance.
 *
 * "Except the LORD build the house, they labour in vain that build it."
 * — Psalm 127:1
 *
 * Container and block scores remain linear for drill-down reporting.
 * Only the file-level score uses logarithmic aggregation.
 */
export function computeHealthScore(blocks: BlockScore[]): HealthScore {
  if (blocks.length === 0) {
    return {
      total: 0, blocks, totalActions: 0,
      alignedCount: 0, neutralCount: 0, misalignedCount: 0,
    };
  }

  // ── Collect all actions and counts ─────────────────────────────
  const allActions: AtomicAction[] = [];
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
      allActions.push(...c.actions);
    }
  }

  // ── Filter to non-neutral actions ──────────────────────────────
  const active = allActions.filter((a) => a.direction !== 0);

  if (active.length === 0) {
    return {
      total: 0, blocks, totalActions,
      alignedCount, neutralCount, misalignedCount,
    };
  }

  // ── Sort: severity (errors first), then direction (fails first) ─
  // Most impactful observations get highest positional weight.
  const SEVERITY_ORDER: Record<string, number> = { error: 0, warn: 1, info: 2 };
  active.sort((a, b) => {
    const sa = SEVERITY_ORDER[a.impact ?? "warn"] ?? 1;
    const sb = SEVERITY_ORDER[b.impact ?? "warn"] ?? 1;
    if (sa !== sb) return sa - sb;
    return a.direction - b.direction; // -1 before +1
  });

  // ── Logarithmic positional weighting ───────────────────────────
  // multiplier(i) = ln(n - i + 1) / ln(n + 1)
  //
  // Position 0: ln(n+1)/ln(n+1) = 1.0        (foundation — full weight)
  // Position n-1: ln(2)/ln(n+1)  ≈ 0.15      (refinement — minimal weight)
  //
  // The effective weight = base_impact × positional_multiplier.
  // Score = Σ(direction × effective_weight) / Σ(effective_weight) × 100.
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

  const total = maxWeight > 0
    ? Math.round((weightedSum / maxWeight) * 100)
    : 0;

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
//   File-level score: sort by severity, weight by ln(n-i+1)/ln(n+1).
//   Container/block scores: unchanged (linear, for drill-down).
//   Foundation checks matter most. Refinement checks diminish.
//
// "Diverse weights, and diverse measures, both of them are alike
//  abomination to the LORD." — Proverbs 20:10
// "Except the LORD build the house, they labour in vain that build it."
// — Psalm 127:1
// ============================================================================
