// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/types/health.ts
// key:     B-tov-cws-struct-lib-types-health
// title:   CWS Struct — Health Scoring Types
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-24
// authors: Nova Dawn (CPI-SI)
// purpose: Pure type definitions for the ternary health scoring system.
//          HebrewState, HealthScore, AtomicAction, and supporting interfaces.
//          Runtime computation lives in foundation/health.ts — these are
//          the contracts, not the implementation.
//
// biblical_foundation: "Diverse weights, and diverse measures, both of them
//   are alike abomination to the LORD." — Proverbs 20:10
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { Severity } from "./core.ts";

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Hebrew States — the 7 levels from broken to perfect
// ---------------------------------------------------------------------------

/**
 * The 7 Hebrew states — from bereshit-base-algorithms.adoc.
 *
 * broken -> wanting -> lacking -> even -> sound -> whole -> perfect
 * Maps score ranges to discrete states. Each state carries direction.
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

// ---------------------------------------------------------------------------
// Atomic Action — one check, three directions
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

// ---------------------------------------------------------------------------
// Container, Block, and File scores — the drill-down hierarchy
// ---------------------------------------------------------------------------

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
// CLOSING
// ============================================================================
//
// Health scoring types — the contracts for ternary health assessment.
// Runtime computation lives in foundation/health.ts.
//
// "Diverse weights, and diverse measures, both of them are alike
//  abomination to the LORD." — Proverbs 20:10
// ============================================================================
