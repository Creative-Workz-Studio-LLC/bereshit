// ============================================================================
// METADATA
// ============================================================================
//
// file:    lib/foundation/types.ts
// key:     B-tov-cws-struct-lib-foundation-types
// title:   CWS Struct — Shared Types
// type:    Code (Library)
// version: a-02.00
// created: 2026-02-11
// updated: 2026-02-17
// authors: Nova Dawn (CPI-SI)
// purpose: Shared type definitions for all format handlers. Core contracts:
//          LintResult, FixSuggestion, FormatHandler, CliOptions.
//          Health scoring extracted to health.ts during lib/ restructuring.
//
// biblical_foundation: "A just weight and balance are the LORD's: all the
//   weights of the bag are his work." — Proverbs 16:11
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import type { HealthScore } from "./health.ts";

// ---------------------------------------------------------------------------
// Severity — the weight of a finding
// ---------------------------------------------------------------------------

export type Severity = "error" | "warn" | "info";

// ---------------------------------------------------------------------------
// Lint Results
// ---------------------------------------------------------------------------

/** A single atomic action within a fix suggestion (for multi-step code fixes). */
export interface FixAction {
  type: "remove" | "insert";
  /** For remove: first line to remove (1-based). */
  startLine?: number;
  /** For remove: last line to remove (1-based, inclusive). */
  endLine?: number;
  /** For insert: line number to insert after (1-based). */
  afterLine?: number;
  /** For insert: lines of code to add. */
  content?: string[];
}

/** Suggested correction for a lint finding. */
export interface FixSuggestion {
  /** Human description of the fix (e.g., "Add missing [_metadata.I1_core] table"). */
  description: string;
  /** TOML snippet to insert or replace (TOML handler). */
  toml?: string;
  /** Insertion hint (e.g., "after [_metadata]", "in _metadata.I1_core"). */
  location?: string;
  /** Ordered list of atomic actions for code fixes (Go/Rust handlers). */
  actions?: FixAction[];
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
  /** The //omni: pragma directive from line 1 (e.g. "//omni:code --go -library"). */
  pragma?: string;
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
  command: "lint" | "transform" | "create" | "formats" | "verify" | "studio" | "help" | "version";
  format?: string;
  targets: string[];
  verbose: boolean;
  errorsOnly: boolean;
  summaryOnly: boolean;
  dryRun: boolean;
  extensions: boolean;
  json: boolean;
  failFast: boolean;
  /** Create command: subtype for code generation. */
  subtype?: string;
  /** Create command: OmniCode key. */
  key?: string;
  /** Create command: file title. */
  title?: string;
  /** Create command: file purpose. */
  purpose?: string;
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
  // Duck-type: FixSuggestion has 'description' + (toml | actions), ResultOpts has 'line' or 'fix'
  if ("description" in opts && ("toml" in opts || "actions" in opts)) {
    return { fix: opts as FixSuggestion };
  }
  const result: Partial<LintResult> = {};
  const ro = opts as ResultOpts;
  if (ro.line !== undefined) result.line = ro.line;
  if (ro.fix !== undefined) result.fix = ro.fix;
  return result;
}

/** Tally a results array into a summary. Single-pass counting. */
export function summarize(
  file: string,
  results: LintResult[],
  health?: HealthScore,
): LintSummary {
  let errors = 0, warnings = 0, infos = 0;
  for (const r of results) {
    switch (r.severity) {
      case "error": errors++; break;
      case "warn": warnings++; break;
      case "info": infos++; break;
    }
  }
  return { file, errors, warnings, infos, results, ...(health ? { health } : {}) };
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
