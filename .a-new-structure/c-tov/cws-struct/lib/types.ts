// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/types.ts
// key:     B-tov-cws-struct-lib-types
// title:   CWS Struct — Shared Types
// type:    Code (Library)
// version: a-01.00
// created: 2026-02-11
// authors: Nova Dawn (CPI-SI)
// purpose: Shared type definitions for all format handlers. Extracted from
//          the 8 standalone linters where each redefined these independently.
//
// biblical_foundation: "A just weight and balance are the LORD's: all the
//   weights of the bag are his work." — Proverbs 16:11
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Severity — the weight of a finding
// ---------------------------------------------------------------------------

export type Severity = "error" | "warn" | "info";

// ---------------------------------------------------------------------------
// Lint Results
// ---------------------------------------------------------------------------

/** Suggested correction for a lint finding. */
export interface FixSuggestion {
  /** Human description of the fix (e.g., "Add missing [_metadata.I1_core] table"). */
  description: string;
  /** TOML snippet to insert or replace. */
  toml: string;
  /** Insertion hint (e.g., "after [_metadata]", "in _metadata.I1_core"). */
  location?: string;
}

/** Single finding from a lint check. */
export interface LintResult {
  file: string;
  severity: Severity;
  rule: string;
  message: string;
  /** 1-based line number where the finding occurs (if applicable). */
  line?: number;
  /** Optional auto-correction suggestion. */
  fix?: FixSuggestion;
}

/** Aggregated results for one file. */
export interface LintSummary {
  file: string;
  errors: number;
  warnings: number;
  infos: number;
  results: LintResult[];
  /** Computed health score — sections as containers, atomic action scoring. */
  health?: HealthScore;
}

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

// ---------------------------------------------------------------------------
// Format Handler — the contract for adding new formats
// ---------------------------------------------------------------------------

/** Options passed to transform handlers. */
export interface TransformOptions {
  /** Preview mode — report changes without writing. */
  dryRun: boolean;
  /** Scaffold extension sections (I4, C5-C7, _contract, X2-X4, etc.). */
  extensions: boolean;
}

/** Every format registers a handler that satisfies this interface. */
export interface FormatHandler {
  /** Short name used in CLI (e.g., "toml", "json", "makefile"). */
  name: string;

  /** Human-readable description. */
  description: string;

  /** File extensions this handler processes (e.g., [".toml"]). */
  extensions: string[];

  /**
   * Optional basename patterns for files without meaningful extensions
   * (e.g., [".gitignore", ".editorconfig"] for dotfiles).
   */
  basenames?: string[];

  /** Maximum directory walk depth (default: 3). */
  maxDepth?: number;

  /** Lint a single file, return results. */
  lint(filePath: string): Promise<LintResult[]>;

  /**
   * Compute health score for a file from its lint results (optional).
   * Called after lint() with the same file path + results.
   * Each handler implements its own domain-specific scoring algorithm.
   */
  computeHealth?(filePath: string, results: LintResult[]): Promise<HealthScore>;

  /** Transform a single file in place (optional). */
  transform?(filePath: string, opts: TransformOptions): Promise<LintResult[]>;
}

// ---------------------------------------------------------------------------
// CLI Options — parsed from command line
// ---------------------------------------------------------------------------

export interface CliOptions {
  command: "lint" | "transform" | "formats" | "verify" | "help" | "version";
  format?: string;
  targets: string[];
  verbose: boolean;
  errorsOnly: boolean;
  summaryOnly: boolean;
  dryRun: boolean;
  extensions: boolean;
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Result constructors — convenience builders
// ---------------------------------------------------------------------------

/** Options for result constructors. Pass line and/or fix as needed. */
interface ResultOpts {
  line?: number;
  fix?: FixSuggestion;
}

export function error(
  file: string, rule: string, message: string, opts?: ResultOpts | FixSuggestion,
): LintResult {
  const o = normalizeOpts(opts);
  return { file, severity: "error", rule, message, ...o };
}

export function warn(
  file: string, rule: string, message: string, opts?: ResultOpts | FixSuggestion,
): LintResult {
  const o = normalizeOpts(opts);
  return { file, severity: "warn", rule, message, ...o };
}

export function info(
  file: string, rule: string, message: string, opts?: ResultOpts | FixSuggestion,
): LintResult {
  const o = normalizeOpts(opts);
  return { file, severity: "info", rule, message, ...o };
}

/** Normalize opts: accept either ResultOpts or bare FixSuggestion for backward compat. */
function normalizeOpts(opts?: ResultOpts | FixSuggestion): Partial<LintResult> {
  if (!opts) return {};
  // Duck-type: FixSuggestion has 'description' and 'toml', ResultOpts has 'line' or 'fix'
  if ("description" in opts && "toml" in opts) {
    return { fix: opts as FixSuggestion };
  }
  const result: Partial<LintResult> = {};
  const ro = opts as ResultOpts;
  if (ro.line !== undefined) result.line = ro.line;
  if (ro.fix !== undefined) result.fix = ro.fix;
  return result;
}

/** Tally a results array into a summary. */
export function summarize(
  file: string,
  results: LintResult[],
  health?: HealthScore,
): LintSummary {
  return {
    file,
    errors: results.filter((r) => r.severity === "error").length,
    warnings: results.filter((r) => r.severity === "warn").length,
    infos: results.filter((r) => r.severity === "info").length,
    results,
    ...(health ? { health } : {}),
  };
}

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
// Types are the foundation — every format handler, every output function,
// every CLI option flows from these definitions. Change here, cascade
// everywhere. Measure twice.
//
// "A just weight and balance are the LORD's." — Proverbs 16:11
// ============================================================================
