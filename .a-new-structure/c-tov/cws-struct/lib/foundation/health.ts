// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/health.ts
// key:     B-tov-cws-struct-lib-foundation-health
// title:   CWS Struct — Health Scoring (Ternary)
// type:    Code (Library)
// version: a-07.00
// created: 2026-02-17
// updated: 2026-02-21
// authors: Nova Dawn (CPI-SI)
// purpose: Ternary health scoring system — atomic actions with direction.
//          Each observation carries direction (-1, 0, +1) and impact weight.
//          Direction = which way (shavar → yashar → tov).
//          Impact = how much it matters (error=2×, warn=1×, info=0.25×).
//          Score range: -100 (fully misaligned) to +100 (fully aligned).
//          0 = yashar (neutral, not assessed, baseline).
//
//          Three-level scoring architecture:
//            Container: asymmetric weighted scoring (narrow gate + / wide gate -)
//            Block: logarithmic positional weighting × detection weight
//            File: per-block averaging (each block contributes equally)
//
//          Asymmetric scoring (a-06.00):
//            Narrow Gate (+): aligned actions contribute linearly (+weight)
//            Wide Gate (-): misaligned actions cascade (-weight × cascadeN)
//            1st failure = -1×, 2nd = -2×, 3rd = -3×, ... nth = -n×
//            Easy to break things (wide gate). Hard to heal (narrow gate).
//            Theologically sound — destruction easier than creation.
//
//          Detection weight integration (a-06.00):
//            Each container has a detection_weight [0.00-1.00] from schema.
//            Containers with all granted/denied concepts = 1.00 (certain).
//            Containers with defers = lower (ambiguous signal).
//            Applied as multiplier in block-level scoring.
//
//          State resolution (a-05.00):
//            Score → HebrewState (7 levels) → emoji → direction
//            Implements TrueToLevel from bereshit-base-algorithms.adoc
//            and NormalizeBase50 from ALG-001.
//            7 states: broken/wanting/lacking/even/sound/whole/perfect
//            5 hardpoints: -100, -50, 0, +50, +100
//
//          The log curve lives at block level — foundation checks within
//          each block get full weight, refinement checks diminish.
//          Detection weights modulate block-level confidence.
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
 *
 * Aligned:    contribution = +impact_weight (linear — narrow gate)
 * Misaligned: contribution = -impact_weight × cascade_n (wide gate)
 *
 * The base weight is the same ruler both ways — "diverse weights" is
 * abomination. But CASCADING applies only to misalignment. Easy to
 * break things (wide gate). Hard to heal (narrow gate). This is
 * theologically sound — destruction is always easier than creation.
 */
export let IMPACT_WEIGHT: Record<string, number> = {
  error: 2.0,
  warn:  1.0,
  info:  0.25,
};

/**
 * Override impact weights from tool config. Call before dispatching to handlers.
 * Follows the same module-level setter pattern as setGlobalPolicy().
 */
export function setImpactWeights(weights: Record<string, number>): void {
  IMPACT_WEIGHT = weights;
}

// ---------------------------------------------------------------------------
// Constants — Hebrew state resolution (ALG-001)
// ---------------------------------------------------------------------------

/**
 * The 7 Hebrew states — from bereshit-base-algorithms.adoc.
 *
 * broken → wanting → lacking → even → sound → whole → perfect
 * Maps score ranges to discrete states. Each state carries direction.
 *
 * "There is a way which seemeth right unto a man, but the end thereof
 *  are the ways of death." — Proverbs 14:12
 * The states don't judge intent — they measure alignment.
 */
export type HebrewState =
  | "broken"   // [-100, -67] shavar — fully misaligned
  | "wanting"  // [-66, -34]  chaser — significantly lacking
  | "lacking"  // [-33, -1]   ratsah — slightly off
  | "even"     // [0, 0]      yashar — neutral baseline
  | "sound"    // [1, 33]     tamim  — slightly aligned
  | "whole"    // [34, 66]    shalem — significantly aligned
  | "perfect"; // [67, 100]   tov    — fully aligned

/** Direction derived from state: -1 (misaligned), 0 (neutral), +1 (aligned). */
export type HebrewDirection = -1 | 0 | 1;

/**
 * State boundaries — exact ranges from log.toml [ternary.levels].
 * Ordered from most negative to most positive for linear scan.
 */
const STATE_RANGES: ReadonlyArray<{ max: number; state: HebrewState }> = [
  { max: -67, state: "broken" },
  { max: -34, state: "wanting" },
  { max: -1,  state: "lacking" },
  { max: 0,   state: "even" },
  { max: 33,  state: "sound" },
  { max: 66,  state: "whole" },
  // everything above 66 → "perfect" (handled by default)
];

/** Emoji for each Hebrew state — visual health indicator. */
const STATE_EMOJI: Record<HebrewState, string> = {
  broken:  "\u{1F534}",  // 🔴
  wanting: "\u{1F7E0}",  // 🟠
  lacking: "\u{1F7E1}",  // 🟡
  even:    "\u{26AA}",   // ⚪
  sound:   "\u{1F7E2}",  // 🟢
  whole:   "\u{1F49A}",  // 💚
  perfect: "\u{2705}",   // ✅
};

/** Direction for each Hebrew state. */
const STATE_DIRECTION: Record<HebrewState, HebrewDirection> = {
  broken:  -1,
  wanting: -1,
  lacking: -1,
  even:     0,
  sound:   +1,
  whole:   +1,
  perfect: +1,
};

// ---------------------------------------------------------------------------
// Constants — Trite encoding (ALG-010, ternary-storage-algorithms.adoc)
// ---------------------------------------------------------------------------

/**
 * Maximum valid trite value. 3^5 - 1 = 242.
 * Values 0-242 are valid packed trite bytes.
 * Value 243+ is reserved (WEB variants in the full system).
 */
const TRITE_MAX = 242;

/**
 * Position labels for the 5-trit health encoding.
 *
 * For 4-block files (code): [metadata, setup, body, closing, file]
 * For 3-block files (data): [metadata, content, closing, (unused=0), file]
 *
 * The 5th trit is always the file-level direction — the emergent result.
 * Block trits carry the per-block direction. File trit carries the whole.
 */
export const TRITE_POSITIONS = [
  "metadata", "setup/content", "body/unused", "closing", "file",
] as const;

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
  /** Lint layer that produced this action: 0=whole, 1=structure, 2=container, 3=content. */
  layer?: 0 | 1 | 2 | 3;
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
  /** Resolved Hebrew state — 7 levels from broken to perfect. */
  level: HebrewState;
  /** Visual indicator for the state. */
  emoji: string;
  /** Direction derived from state: -1/0/+1. */
  direction: HebrewDirection;
  /** NormalizeBase50 value — R[50] hardpoint: -100/-50/0/+50/+100. */
  hardpoint: -100 | -50 | 0 | 50 | 100;
  /**
   * Health trite — 5 balanced trits packed into 1 byte (0-242).
   *
   * ALG-010 encoding: 5 trits in 1 byte, 243 states.
   * Each trit = direction of a block or the file as a whole.
   * Position 0: metadata, 1: setup/content, 2: body/(unused),
   * 3: closing, 4: file-level direction.
   *
   * Trite of 121 = [0,0,0,0,0] = all even (yashar baseline).
   * Trite of 242 = [+1,+1,+1,+1,+1] = all perfect.
   * Trite of 0   = [-1,-1,-1,-1,-1] = all broken.
   */
  trite: number;
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
// Each atomic action contributes asymmetrically:
//
//   Aligned (+1):    contribution = +impact_weight        (narrow gate)
//   Misaligned (-1): contribution = -impact_weight × N    (wide gate, Nth failure)
//   Neutral (0):     no contribution (can't assess)
//
// Score = (sum of contributions / sum of max possible weights) × 100
//
// The narrow gate: earning +100 requires every check to pass. Linear.
// The wide gate: failures cascade. 1st = -1×, 2nd = -2×, 3rd = -3×.
// A single failure is forgivable. Many failures compound exponentially.
//
// Detection weights modulate block-level confidence per container.
// Neutral actions prevent cascade inflation (root cause → children neutral).
//
// "Enter ye in at the strait gate: for wide is the gate, and broad is
//  the way, that leadeth to destruction." — Matthew 7:13
// "Diverse weights, and diverse measures, both of them are alike
//  abomination to the LORD." — Proverbs 20:10

// ---------------------------------------------------------------------------
// State resolution — TrueToLevel + NormalizeBase50 (ALG-001)
// ---------------------------------------------------------------------------
//
// Score → State → Emoji → Direction.
// The score tells you WHERE. The state tells you WHAT IT MEANS.
// The emoji tells you AT A GLANCE. The direction tells you WHICH WAY.
//
// "The LORD seeth not as man seeth; for man looketh on the outward
//  appearance, but the LORD looketh on the heart." — 1 Samuel 16:7

/**
 * TrueToLevel — resolve a numeric score to its Hebrew state.
 *
 * Implements ALG-001 from bereshit-base-algorithms.adoc.
 * Linear scan through STATE_RANGES. O(7) — constant time.
 * Score is clamped to [-100, +100] before resolution.
 *
 * Ranges (from log.toml [ternary.levels]):
 *   broken [-100, -67], wanting [-66, -34], lacking [-33, -1],
 *   even [0, 0], sound [1, 33], whole [34, 66], perfect [67, 100]
 */
export function trueToLevel(score: number): HebrewState {
  const clamped = Math.max(-100, Math.min(100, Math.round(score)));
  for (const { max, state } of STATE_RANGES) {
    if (clamped <= max) return state;
  }
  return "perfect";
}

/**
 * LevelToEmoji — visual indicator for a Hebrew state.
 *
 * Each state maps to a single emoji for terminal output.
 * broken 🔴  wanting 🟠  lacking 🟡  even ⚪  sound 🟢  whole 💚  perfect ✅
 */
export function levelToEmoji(level: HebrewState): string {
  return STATE_EMOJI[level];
}

/**
 * LevelToDirection — direction derived from Hebrew state.
 *
 * broken/wanting/lacking → -1 (misaligned)
 * even → 0 (neutral)
 * sound/whole/perfect → +1 (aligned)
 */
export function levelToDirection(level: HebrewState): HebrewDirection {
  return STATE_DIRECTION[level];
}

/**
 * NormalizeBase50 — quantize score to R[50] resolution.
 *
 * 5 hardpoints on the scale: -100, -50, 0, +50, +100.
 * This is the coarsest detection granularity — R[50].
 * For quick assessment: bad, slightly bad, neutral, slightly good, or good?
 *
 * Implements NormalizeBase50 from ALG-001.
 *   ≤ -75 → -100
 *   ≤ -25 → -50
 *   ≤  25 →  0
 *   ≤  75 → +50
 *   else  → +100
 */
export function normalizeBase50(score: number): -100 | -50 | 0 | 50 | 100 {
  const clamped = Math.max(-100, Math.min(100, Math.round(score)));
  if (clamped <= -75) return -100;
  if (clamped <= -25) return -50;
  if (clamped <= 25) return 0;
  if (clamped <= 75) return 50;
  return 100;
}

// ---------------------------------------------------------------------------
// Trite encoding — ALG-010 (ternary-storage-algorithms.adoc)
// ---------------------------------------------------------------------------
//
// 5 balanced trits → 1 byte (0-242). 3^5 = 243 states.
//
// Packing: Horner's method (MST first)
//   1. Convert each trit from {-1, 0, +1} to unsigned {0, 1, 2}
//   2. result = u4
//   3. result = result * 3 + u3
//   4. result = result * 3 + u2
//   5. result = result * 3 + u1
//   6. result = result * 3 + u0
//
// Unpacking: repeated division (LST first)
//   1. For i = 0..4: trits[i] = value % 3; value = floor(value / 3)
//   2. Convert each trit from {0, 1, 2} to balanced {-1, 0, +1}
//
// "And God saw every thing that he had made, and, behold, it was very good."
// — Genesis 1:31

/**
 * Pack 5 balanced trits into a single byte (0-242).
 *
 * Implements the pack_balanced_trits algorithm from ALG-010.
 * Each trit must be -1, 0, or +1.
 *
 * For health: [metadata, setup/content, body/unused, closing, file].
 *
 * @returns Packed byte 0-242. Returns 121 (all even) if inputs invalid.
 */
export function encodeHealthTrite(
  t0: -1 | 0 | 1,
  t1: -1 | 0 | 1,
  t2: -1 | 0 | 1,
  t3: -1 | 0 | 1,
  t4: -1 | 0 | 1,
): number {
  // Convert balanced {-1, 0, +1} → unsigned {0, 1, 2}
  const u0 = t0 + 1;
  const u1 = t1 + 1;
  const u2 = t2 + 1;
  const u3 = t3 + 1;
  const u4 = t4 + 1;

  // Horner's method (MST first → LST last)
  let result = u4;
  result = result * 3 + u3;
  result = result * 3 + u2;
  result = result * 3 + u1;
  result = result * 3 + u0;

  // Sanity clamp — should always be [0, 242]
  return Math.min(TRITE_MAX, Math.max(0, result));
}

/**
 * Unpack a byte (0-242) into 5 balanced trits.
 *
 * Implements unpack_to_balanced_trits from ALG-010.
 *
 * @returns Array of 5 trits: [t0, t1, t2, t3, t4], each -1/0/+1.
 *          Position order: [metadata, setup/content, body/unused, closing, file].
 *          Returns [0,0,0,0,0] for out-of-range input.
 */
export function decodeHealthTrite(
  byte: number,
): [(-1 | 0 | 1), (-1 | 0 | 1), (-1 | 0 | 1), (-1 | 0 | 1), (-1 | 0 | 1)] {
  if (byte < 0 || byte > TRITE_MAX) {
    return [0, 0, 0, 0, 0];
  }

  let value = Math.floor(byte);
  const trits: (-1 | 0 | 1)[] = [];

  // Extract via repeated division (LST first)
  for (let i = 0; i < 5; i++) {
    const unsigned = value % 3;
    trits.push((unsigned - 1) as -1 | 0 | 1);
    value = Math.floor(value / 3);
  }

  return trits as [(-1 | 0 | 1), (-1 | 0 | 1), (-1 | 0 | 1), (-1 | 0 | 1), (-1 | 0 | 1)];
}

// ---------------------------------------------------------------------------
// Health Score computation — the ternary algorithm
// ---------------------------------------------------------------------------

/**
 * Compute a container score from its atomic actions.
 *
 * Asymmetric ternary scoring:
 *
 *   Aligned (+1):    contribution = +impact_weight        (linear, narrow gate)
 *   Misaligned (-1): contribution = -impact_weight × N    (cascade, wide gate)
 *   Neutral (0):     tracked but doesn't affect score
 *
 * Where N is the cascade index — the Nth misalignment in this container.
 * 1st failure = 1×, 2nd = 2×, 3rd = 3×. The more things wrong, the
 * worse each additional failure becomes.
 *
 * Normalization: maxWeight = sum of all base weights (best-case: all +1).
 * Score = (weightedSum / maxWeight) × 100, clamped to [-100, +100].
 *
 * This means: all aligned = +100. A few misalignments = slightly negative.
 * Many misalignments = deeply negative (cascade amplifies).
 * Easy to break things (wide gate). Hard to heal (narrow gate).
 *
 * "Enter ye in at the strait gate: for wide is the gate, and broad is
 *  the way, that leadeth to destruction." — Matthew 7:13
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
  let cascadeIndex = 0;

  for (const a of actions) {
    const weight = IMPACT_WEIGHT[a.impact ?? "warn"] ?? 1.0;

    if (a.direction > 0) {
      aligned++;
      // Narrow gate: linear contribution
      weightedSum += weight;
      maxWeight += weight;
    } else if (a.direction < 0) {
      misaligned++;
      cascadeIndex++;
      // Wide gate: cascading negative contribution
      // The Nth misalignment costs N× the base weight.
      // 1st: -1×w, 2nd: -2×w, 3rd: -3×w, ...
      weightedSum -= weight * cascadeIndex;
      maxWeight += weight;
    } else {
      neutral++;
      // direction 0: tracked but doesn't affect score.
      // Neutral doesn't push toward either pole.
    }
  }

  const score = maxWeight > 0
    ? Math.max(-100, Math.min(100, Math.round((weightedSum / maxWeight) * 100)))
    : 0;

  return {
    section, block, total, aligned, neutral, misaligned, score, actions,
  };
}

/**
 * Compute a block score from its containers — logarithmic positional
 * weighting × detection weight.
 *
 * Collects all non-neutral actions from containers, sorts by severity
 * (errors first, fails before passes), and applies logarithmic positional
 * weighting: multiplier(i) = ln(n-i+1) / ln(n+1).
 *
 * Detection weight integration: each action is weighted by the detection
 * confidence of its container. Containers with all granted/denied concepts
 * (detectionWeight ≈ 1.0) contribute full signal. Containers with many
 * defers (detectionWeight ≈ 0.5) contribute half signal. This means
 * ambiguous containers don't distort the score.
 *
 * Foundation checks within the block get full weight.
 * Refinement checks get diminishing weight.
 * Container scores remain linear for drill-down reporting.
 *
 * @param block Block name
 * @param containers Container scores
 * @param detectionWeights Optional map: container section name → detection weight [0-1]
 *
 * "Except the LORD build the house, they labour in vain that build it."
 * — Psalm 127:1
 */
export function computeBlockScore(
  block: string,
  containers: ContainerScore[],
  detectionWeights?: ReadonlyMap<string, number>,
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

  // Logarithmic positional weighting × detection weight
  // positional: multiplier(i) = ln(n - i + 1) / ln(n + 1)
  // detection:  container's confidence from schema [0.0 - 1.0]
  const n = active.length;
  const logDenom = Math.log(n + 1);

  let weightedSum = 0;
  let maxWeight = 0;

  for (let i = 0; i < n; i++) {
    const a = active[i]!;
    const baseWeight = IMPACT_WEIGHT[a.impact ?? "warn"] ?? 1.0;
    const positionalMultiplier = Math.log(n - i + 1) / logDenom;
    // Detection weight: how confident are we about this container's signal?
    // Default 1.0 (full confidence) when not provided.
    const detWeight = detectionWeights?.get(a.container) ?? 1.0;
    const effectiveWeight = baseWeight * positionalMultiplier * detWeight;

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
    const level = trueToLevel(0);
    // All even (yashar) → trite = 121 (midpoint)
    const trite = encodeHealthTrite(0, 0, 0, 0, 0);
    return {
      total: 0, level, emoji: levelToEmoji(level),
      direction: levelToDirection(level), hardpoint: normalizeBase50(0),
      trite, blocks, totalActions: 0,
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
    const level = trueToLevel(0);
    const trite = encodeHealthTrite(0, 0, 0, 0, 0);
    return {
      total: 0, level, emoji: levelToEmoji(level),
      direction: levelToDirection(level), hardpoint: normalizeBase50(0),
      trite, blocks, totalActions,
      alignedCount, neutralCount, misalignedCount,
    };
  }

  const total = Math.round(
    activeBlocks.reduce((sum, b) => sum + b.score, 0) / activeBlocks.length,
  );

  // ── State resolution (ALG-001) ─────────────────────────────────
  const level = trueToLevel(total);
  const direction = levelToDirection(level);

  // ── Trite encoding (ALG-010) ───────────────────────────────────
  // Map block names → trit positions. Each block's direction is the
  // direction of the Hebrew state resolved from its score.
  //
  // Position mapping:
  //   t0 = metadata block direction
  //   t1 = setup (code) or content (data) block direction
  //   t2 = body block direction (0 for non-code files)
  //   t3 = closing block direction
  //   t4 = file-level direction (the emergent result)
  //
  // Blocks not found default to 0 (even/yashar — unassessed).
  const blockDir = (name: string): -1 | 0 | 1 => {
    const b = blocks.find((bl) => bl.block === name);
    if (!b) return 0;
    const bLevel = trueToLevel(b.score);
    return levelToDirection(bLevel);
  };

  const t0 = blockDir("metadata");
  const t1 = blockDir("setup") || blockDir("content"); // code vs data
  const t2 = blockDir("body"); // 0 for data files (no body block)
  const t3 = blockDir("closing");
  const t4 = direction; // file-level

  const trite = encodeHealthTrite(t0, t1, t2, t3, t4);

  return {
    total, level, emoji: levelToEmoji(level),
    direction, hardpoint: normalizeBase50(total),
    trite, blocks, totalActions,
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
// a-05.00: State resolution — score becomes meaningful.
//   Score → HebrewState (7 levels) → emoji → direction.
//   TrueToLevel: [-100,+100] → broken/wanting/lacking/even/sound/whole/perfect
//   NormalizeBase50: 5 hardpoints (-100/-50/0/+50/+100) for R[50] detection.
//   HealthScore now carries level, emoji, direction, hardpoint.
//   AtomicAction gains optional layer tag (0-3) for lint chain integration.
//   Implements ALG-001 from bereshit-base-algorithms.adoc.
//
// a-06.00: Asymmetric weighting — narrow gate / wide gate.
//   Container scoring: aligned = +weight (linear), misaligned = -weight × N (cascade).
//   The Nth misalignment costs N× base weight. Destruction easier than creation.
//   Block scoring: detection_weight from schema modulates container confidence.
//   computeBlockScore() accepts optional detectionWeights map.
//   Container score now clamped to [-100, +100].
//   Implements asymmetric normalization from ALG-001.
//
// a-07.00: Health trite encoding — ALG-010 (ternary-storage-algorithms.adoc).
//   5 balanced trits packed into 1 byte (0-242). 3^5 = 243 states.
//   encodeHealthTrite(): pack 5 trits → byte. decodeHealthTrite(): byte → 5 trits.
//   Horner's method for packing, repeated division for unpacking.
//   HealthScore gains `trite` field, computed from per-block directions.
//   Position mapping: [metadata, setup/content, body/unused, closing, file].
//   121 = all even (yashar). 242 = all perfect. 0 = all broken.
//   Implements pack_balanced_trits / unpack_to_balanced_trits from ALG-010.
//
// "Enter ye in at the strait gate: for wide is the gate, and broad is
//  the way, that leadeth to destruction." — Matthew 7:13
// "Diverse weights, and diverse measures, both of them are alike
//  abomination to the LORD." — Proverbs 20:10
// "Except the LORD build the house, they labour in vain that build it."
// — Psalm 127:1
// "The LORD seeth not as man seeth; for man looketh on the outward
//  appearance, but the LORD looketh on the heart." — 1 Samuel 16:7
// ============================================================================
