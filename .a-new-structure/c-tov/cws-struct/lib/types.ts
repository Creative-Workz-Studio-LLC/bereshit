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

export function error(
  file: string, rule: string, message: string, fix?: FixSuggestion,
): LintResult {
  return fix ? { file, severity: "error", rule, message, fix } : { file, severity: "error", rule, message };
}

export function warn(
  file: string, rule: string, message: string, fix?: FixSuggestion,
): LintResult {
  return fix ? { file, severity: "warn", rule, message, fix } : { file, severity: "warn", rule, message };
}

export function info(
  file: string, rule: string, message: string, fix?: FixSuggestion,
): LintResult {
  return fix ? { file, severity: "info", rule, message, fix } : { file, severity: "info", rule, message };
}

/** Tally a results array into a summary. */
export function summarize(file: string, results: LintResult[]): LintSummary {
  return {
    file,
    errors: results.filter((r) => r.severity === "error").length,
    warnings: results.filter((r) => r.severity === "warn").length,
    infos: results.filter((r) => r.severity === "info").length,
    results,
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
